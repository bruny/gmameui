/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * GMAMEUI
 *
 * Copyright 2007-2009 Andrew Burton <adb@iinet.net.au>
 * based on GXMame code
 * 2002-2005 Stephane Pontier <shadow_walker@users.sourceforge.net>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>
 *
 */

#include <string.h>
#include <glade/glade.h>

#include "gmameui-sidebar.h"
#include "gmameui.h"
#include "gui.h"
#include "gui_prefs.h"

static void gmameui_sidebar_class_init (GMAMEUISidebarClass *class);
static void gmameui_sidebar_init (GMAMEUISidebar *dlg);
static gboolean gmameui_sidebar_set_history (GMAMEUISidebar *sidebar, MameRomEntry *rom);

G_DEFINE_TYPE (GMAMEUISidebar, gmameui_sidebar, GTK_TYPE_FRAME)


typedef struct {
	GdkPixbuf *orig_pixbuf; /* Originally-loaded pixbuf from disk;
				   scaling should be applied against this */
	GtkWidget *image_box;   /* Container box widget */
	GtkWidget *image;	/* GtkImage widget */
	gboolean resize;	/* Only resize when this is TRUE to prevent
				   callback from re-triggering repeatedly */
} _gmameui_picture_type;



struct _GMAMEUISidebarPrivate {

	GtkWidget *screenshot_hist_vbox;
	GtkWidget *screenshot_event_box;

	GtkWidget *main_screenshot;
	
	/* Structure to maintain the ROM images (snapshot, flyer, etc) */
	_gmameui_picture_type gmameui_picture_type[NUM_GMAMEUI_PICTURE_TYPES];
	
	GtkWidget *screenshot_notebook;

	/* Widgets for the ROM history */
	GtkWidget *history_scrollwin;
	GtkWidget *history_box;
	GtkTextBuffer *history_buffer;	
};


static gboolean
set_history (const gchar   *entry_name,
	     GtkTextBuffer *text_buffer)
{
	FILE *history_file;
	gchar *history_filename;
	GtkTextIter text_iter;
	gchar line[2000];
	gint i, n;
	gchar *tmp, *p;
	gchar **games;
	gboolean found_game = FALSE;
	gboolean pointer_in_info = FALSE;
	gboolean extra_newline = FALSE;

	g_object_get (main_gui.gui_prefs,
		      "file-history", &history_filename,
		      NULL);
	history_file = fopen (history_filename, "r");

	if (!history_file) {
		GMAMEUI_DEBUG ("History.dat file %s not found", history_filename);
		g_free (history_filename);
		return (FALSE);
	}

	/* Scan through the file. First look for '$',then $info=game, $bio and $end */
	while (fgets (line, 500, history_file)) {
		p = line;
		tmp = p;
		if (*tmp == '$' && !found_game) {
			/* Found a line */
			if (!strncmp (p, "$info", 5)) {
				p = tmp = p + 6;
				/* It is an info line */
				i = 0;
				while (*tmp && (*tmp++ != '\n'))
					i++;
				/* Sometimes the list is continued on the next line */
				i--;
				do {
					p[i] = fgetc (history_file);
					if (p[i] == '\n')
						i--;
					if (p[i] == '\r')
						i--;
					i++;

				} while ( (p[i - 1] != '$') && i < 2000 /* buffer size */);
				if (p[i - 1] == '$')
					ungetc ('$', history_file);

				p[i - 1] = 0;
				games = g_strsplit (p, ",", 20);
				n = 0;

				while ( (games[n])) {
					games[n] = g_strchomp (games[n]);

					if (!strcmp (games[n], entry_name)) {
						/* It is the info for the wanted game */
						found_game = TRUE;
					}
					n++;
				}
				g_strfreev (games);
			}
		} else if (found_game && (*tmp == '$')) {
			if (!strncmp (p, "$bio", 4))
				pointer_in_info = TRUE;
			if (!strncmp (p, "$end", 4)) {
				pointer_in_info = FALSE;
				break;
			}
		} else if (found_game && pointer_in_info) {
			i = 0;
			while (*tmp && (*tmp != '\r') && (*tmp++ != '\n'))
				i++;
			if (i == 0) {/* and a new line but not severals*/
				if (!extra_newline) {
					extra_newline = TRUE;
				} else
					extra_newline = FALSE;
			} else {
				extra_newline = FALSE;
			}

			if (!extra_newline) {
				gsize bytes_read, bytes_written;
				gchar *utf8_string;

				p[i] = '\n';
				p[i + 1] = '\0';

				/* Must convert to utf-8 otherwise we get GTK-critical warnings from gtk_text_buffer_insert
				   for non-ascii characters. xmame uses ISO-8859-1 (I think).
				*/
				utf8_string = g_convert (p, (i + 1), "UTF-8", "ISO-8859-1", &bytes_read, &bytes_written, NULL);
				gtk_text_buffer_get_end_iter (GTK_TEXT_BUFFER (text_buffer), &text_iter);
				gtk_text_buffer_insert (GTK_TEXT_BUFFER (text_buffer), &text_iter, utf8_string, -1);
				g_free (utf8_string);
			}
		}
	}

	if (!found_game) {
		GMAMEUI_DEBUG ("no history info found for %s", entry_name);
	}

	fclose (history_file);
	if (history_filename)
		g_free (history_filename);

	return (found_game);
}

static gboolean
set_game_history (MameRomEntry *rom, GtkTextBuffer *text_buffer)
{
	const gchar *entry_name;

	g_return_val_if_fail (rom != NULL, FALSE);
	
	if (mame_rom_entry_is_clone (rom))
		entry_name = mame_rom_entry_get_parent_romname (rom);
	else
		entry_name = mame_rom_entry_get_romname (rom);

	return set_history (entry_name, text_buffer);
}

static gboolean
set_info (const gchar   *entry_name,
	  GtkTextBuffer *text_buffer)
{
	
	FILE *mameinfo_dat;
	gchar *mameinfo_filename;
	GtkTextIter text_iter;
	gchar line[2000];
	gint i;
	gchar *tmp, *p;
	gboolean found_game = FALSE;
	gboolean pointer_in_info = FALSE;
	gboolean extra_newline = FALSE;

	g_object_get (main_gui.gui_prefs,
		      "file-mameinfo", &mameinfo_filename,
		      NULL);
	mameinfo_dat = fopen (mameinfo_filename, "r");

	if (!mameinfo_dat) {
		GMAMEUI_DEBUG ("mameinfo.dat file %s not found", mameinfo_filename);
		g_free (mameinfo_filename);
		return (FALSE);
	}

	/* Scan through the file. First look for '$',then $info=game, $mame and $end */
	while (fgets (line, 500, mameinfo_dat)) {
		p = line;
		tmp = p;
		/* Found a line */
		if (*tmp == '$' && !found_game) {
			/* It is an info line */
			if (!strncmp (p, "$info", 5)) {
				p = tmp = p + 6;
				while (*tmp && (*tmp != '\n') && (*tmp++ != '\r'));
				tmp--; *tmp='\0';
				/* it's the god game */
				if (!strcmp (p, entry_name)) {
					found_game = TRUE;
				}
			}
		} else if (found_game && (*tmp == '$')) {
			if (!strncmp (p, "$mame", 5))
				pointer_in_info = TRUE;
			else if (!strncmp (p, "$end", 4)) {
				pointer_in_info = FALSE;
				break;
			}
		} else if (found_game && pointer_in_info) {
			i = 0;
			while (*tmp && (*tmp != '\r') && (*tmp++ != '\n'))
				i++;
			if (i == 0) { /* and a new line but not severals*/
				if (!extra_newline) {
					extra_newline = TRUE;
				} else
					extra_newline = FALSE;
			} else {
				extra_newline = FALSE;
			}

			if (!extra_newline) {
				gsize bytes_read, bytes_written;
				gchar *utf8_string;
				p[i] = '\n';
				p[i + 1] = '\0';
				utf8_string = g_convert (p, (i+1), "UTF-8", "ISO-8859-1", &bytes_read, &bytes_written, NULL);
				gtk_text_buffer_get_end_iter (text_buffer, &text_iter);
				gtk_text_buffer_insert (text_buffer, &text_iter, utf8_string, -1);
				g_free (utf8_string);
			}
		}
	}

	if (!found_game) {
		GMAMEUI_DEBUG ("no MameInfo info found for %s", entry_name);
	}

	fclose (mameinfo_dat);
	g_free (mameinfo_filename);

	return TRUE;
}

static gboolean
set_game_info (const MameRomEntry *rom,
	       GtkTextBuffer  *text_buffer)
{
	const gchar *entry_name;

	
	g_return_val_if_fail (rom != NULL, FALSE);
	
	if (mame_rom_entry_is_clone (rom))
		entry_name = mame_rom_entry_get_parent_romname (rom);
	else
		entry_name = mame_rom_entry_get_romname (rom);

	return set_info (entry_name, text_buffer);
}

/* Returns a GtkWidget representing a GtkImage */
static GtkWidget *
get_pixbuf (MameRomEntry       *tmprom,
	    screenshot_type sctype,
	    int             wwidth,
	    int             wheight)
{
	GdkPixbuf *pixbuf;
	GtkWidget *pict = NULL;
	gchar *filename;
	gchar *filename_parent;
	gchar *zipfile;
	int width = 0;
	int height = 0;
	GError **error = NULL;

	GMAMEUI_DEBUG ("width:%i  height:%i", wwidth, wheight);

	g_return_val_if_fail (tmprom != NULL, NULL);
	
	/* Prevent a strange bug where wwidth=wheight=1 */
	if (wwidth < 20)
		wwidth = 20;

	if (wheight < 20)
		wheight = 20;
	
	gint dir_num;
	switch (sctype) {
		case (FLYERS):
			dir_num = DIR_FLYER;
			break;
		case (CABINETS):
			dir_num = DIR_CABINET;
			break;
		case (MARQUEES):
			dir_num = DIR_MARQUEE;
			break;
		case (TITLES):
			dir_num = DIR_TITLE;
			break;
		case (CONTROL_PANELS):
			dir_num = DIR_CPANEL;
			break;
		case (SNAPSHOTS):
		default:
			dir_num = DIR_SNAPSHOT;
			break;
	}
	
	gchar *directory_name;
	g_object_get (main_gui.gui_prefs,
		      directory_prefs[dir_num].name, &directory_name,
		      NULL);
	
	filename = g_strdup_printf ("%s" G_DIR_SEPARATOR_S "%s.png",
				    directory_name, mame_rom_entry_get_romname (tmprom));
	filename_parent = g_strdup_printf ("%s" G_DIR_SEPARATOR_S "%s.png",
					   directory_name, mame_rom_entry_get_parent_romname (tmprom));

	GMAMEUI_DEBUG ("Looking for image %s", filename);
	pixbuf = gdk_pixbuf_new_from_file (filename, error);
	if (filename)
		g_free (filename);
	
	/* no picture found try parent game if any*/
	if ( (!pixbuf) && mame_rom_entry_is_clone (tmprom)) {
		GMAMEUI_DEBUG ("Looking for image from parent set %s", filename_parent);
		pixbuf = gdk_pixbuf_new_from_file (filename_parent,error);
	}
	if (filename_parent)
		g_free (filename_parent);
	
	if (!pixbuf && (sctype == SNAPSHOTS)) {

		gchar *snapshot_dir;
		g_object_get (main_gui.gui_prefs,
			      "dir-snapshot", &snapshot_dir,
			      NULL);
			
		/* Since MAME 0.111, snapshots are now in a subdirectory per game
		   with numeric names 0000.png, 0001.png, etc. */
		filename = g_strdup_printf ("%s" G_DIR_SEPARATOR_S "%s" G_DIR_SEPARATOR_S "0000.png",
					    snapshot_dir, mame_rom_entry_get_romname (tmprom));
		GMAMEUI_DEBUG ("Looking for image %s", filename);
		pixbuf = gdk_pixbuf_new_from_file (filename,error);
		g_free (filename);

		/* If not found, look in parent folder */
		if ((!pixbuf) && mame_rom_entry_is_clone (tmprom)) {
			filename = g_strdup_printf ("%s" G_DIR_SEPARATOR_S "%s" G_DIR_SEPARATOR_S "0000.png",
						    snapshot_dir, mame_rom_entry_get_parent_romname (tmprom));
			GMAMEUI_DEBUG ("Looking for parent image %s", filename);
			pixbuf = gdk_pixbuf_new_from_file (filename,error);
			g_free (filename);
		}
			
		g_free (snapshot_dir);
	}
	
	/* we havent found the picture in the directory, maybe we could try in a zipfile */
	if (!pixbuf) {
		gchar *snapshotdir;
		g_object_get (main_gui.gui_prefs, "dir-snapshot", &snapshotdir, NULL);
		zipfile = g_build_filename (snapshotdir, "snap.zip", NULL);

		GMAMEUI_DEBUG ("Looking for image in zip file %s", zipfile);

		/* Look in snap.zip; if the snapshot for the ROM can not be found,
		   look for the parent's snap instead */
		pixbuf = read_pixbuf_from_zip_file (zipfile,
						    mame_rom_entry_get_romname (tmprom));
		if (pixbuf == NULL)
			pixbuf = read_pixbuf_from_zip_file (zipfile,
							    mame_rom_entry_get_parent_romname (tmprom));


		g_free (zipfile);
	}

	if (pixbuf) {
		GdkPixbuf *scaled_pixbuf;
		gboolean show_screenshot;
		
		g_object_get (main_gui.gui_prefs,
			      "show-screenshot", &show_screenshot,
			      NULL);

		width = gdk_pixbuf_get_width (pixbuf);
		height = gdk_pixbuf_get_height (pixbuf);
		if (show_screenshot == 1) {
			/* the picture is wider than the window, resize it to the window size */
			if (width > wwidth) {
				height = wwidth * ((gdouble)height / (gdouble)width);
				width = wwidth;
			}
		}
		scaled_pixbuf = gdk_pixbuf_scale_simple (pixbuf,
							 width,
							 height,
							 GDK_INTERP_BILINEAR);
		g_object_unref (pixbuf);
		pixbuf = scaled_pixbuf;
	} else {
		GMAMEUI_DEBUG ("no picture (%i), fall back to the default one", sctype);

		pixbuf = gmameui_get_icon_from_stock ("gmameui-screen");
	}

	pict = (GtkWidget *) gtk_image_new_from_pixbuf (pixbuf);
	
	if (pixbuf)
		g_object_unref (pixbuf);

	return pict;
}

static void
change_screenshot (GtkWidget       *widget,
		   GdkEventButton  *event,
		   gpointer         user_data)
{	/* prevent the mouse wheel (button 4 & 5) to change the screenshot*/
/* FIXME TODO
	if (event && event->button <= 3) {
		gui_prefs.ShowFlyer = (++gui_prefs.ShowFlyer) % 5;
		gmameui_sidebar_set_with_rom (GMAMEUI_SIDEBAR (main_gui.screenshot_hist_frame),
					      gui_prefs.current_game);
	}*/
}

static void
on_screenshot_notebook_switch_page (GtkNotebook *notebook,
				    GtkNotebookPage *page,
				    guint page_num,
				    gpointer user_data)
{
	g_object_set (main_gui.gui_prefs,
		      "show-flyer", page_num,
		      NULL);

	gmameui_sidebar_set_with_rom (GMAMEUI_SIDEBAR (main_gui.screenshot_hist_frame),
				      gui_prefs.current_game);
}

static void
gmameui_sidebar_finalize (GObject *obj)
{
	int i;
GMAMEUI_DEBUG ("Freeing sidebar");
	GMAMEUISidebar *dlg = GMAMEUI_SIDEBAR (obj);	

	for (i = 0; i < NUM_GMAMEUI_PICTURE_TYPES; i++)
		g_object_unref (G_OBJECT (dlg->priv->gmameui_picture_type[i].orig_pixbuf));
	
	g_free (dlg->priv);
	
	((GObjectClass *) gmameui_sidebar_parent_class)->finalize (obj);
GMAMEUI_DEBUG ("Freeing sidebar - done");
}

static void
gmameui_sidebar_class_init (GMAMEUISidebarClass *class)
{
	GObjectClass *object_class = G_OBJECT_CLASS (class);

	object_class->finalize = gmameui_sidebar_finalize;
}

/* This callback is invoked whenever the space allocated to the currently-displayed
   ROM image is changed, allowing the image to be resized to fit the allocated space */
static void
on_image_container_resized (GtkWidget *widget, GtkAllocation *allocation, gpointer user_data)
{
	_gmameui_picture_type *picture_type;	
	GdkPixbuf *pixbuf;		/* Image's current pixbuf */
	GdkPixbuf *scaled_pixbuf;       /* Copied, scaled pixbuf */
	int wwidth, wheight;		/* Allocated (available) container dimensions */
	int img_curr_w, img_curr_h;     /* Current dimensions of image */
	int h_width, w_height;
	int h_width_diff, w_height_diff;
	int width, height;		/* Target dimensions of image */

	picture_type = (_gmameui_picture_type *) user_data;
	
	g_return_if_fail (picture_type != NULL);
	g_return_if_fail (picture_type->orig_pixbuf != NULL);

	/* Resize only every second time (prevents scaling/setting triggering another call) */
	if (picture_type->resize) {
		wwidth = wheight = 0;

		/* Find out how much space we have to play with */
		wwidth = allocation->width;
		wheight = allocation->height;
	
		/* Use a copy of the original pixbuf, so that we are not working with
		   a gradually distorting pixbuf */
		pixbuf = gdk_pixbuf_copy (picture_type->orig_pixbuf);
	
		g_return_if_fail (pixbuf != NULL);

		img_curr_w = gdk_pixbuf_get_width (pixbuf);
		img_curr_h = gdk_pixbuf_get_height (pixbuf);
	
		w_height = (wwidth * img_curr_h) / img_curr_w;
		h_width = (wheight * img_curr_w) / img_curr_h;
		w_height_diff = wheight - w_height;
		h_width_diff = wwidth - h_width;
	
		if ((w_height_diff >= 0) && (h_width_diff >= 0)) {
		    /* Both are would be smaller than the window pick the bigger one */
		    if (w_height_diff < h_width_diff) {
		        width = wwidth;
		        height = w_height;
		    } else {
		        height = wheight;
		        width = h_width;
		    }
		        
		} else if (w_height_diff < 0) {
		    /* height is going to touch the window edge sooner
		     size based on height */
		    height = wheight;
		    width = h_width;
		} else {
		    /* width is going to touch the window edge sooner
		       size based on width */
		    width = wwidth;
		    height = w_height;
		}

		/*GMAMEUI_DEBUG ("Available size is %dx%d", wwidth, wheight);
		GMAMEUI_DEBUG ("Image size is %dx%d", gdk_pixbuf_get_width (pixbuf), gdk_pixbuf_get_height (pixbuf));
		GMAMEUI_DEBUG ("Setting image with ratio of %f", ((float) gdk_pixbuf_get_width (pixbuf) / (float) gdk_pixbuf_get_height (pixbuf)));*/
	
		scaled_pixbuf = gdk_pixbuf_scale_simple (pixbuf, width, height,
							 GDK_INTERP_BILINEAR);

		gtk_image_set_from_pixbuf (GTK_IMAGE (picture_type->image),
					   scaled_pixbuf);
	
		g_object_unref (G_OBJECT (scaled_pixbuf));
		g_object_unref (G_OBJECT (pixbuf));
	}

	/* The next time we enter this callback, we want to resize (if we
	   didn't this time) */
	picture_type->resize = !(picture_type->resize);
}

/* Creates a hbox containing a treeview (the sidebar) and a notebook */
static void
gmameui_sidebar_init (GMAMEUISidebar *sidebar)
{
	GladeXML *xml;
	int i;
	
GMAMEUI_DEBUG ("Creating sidebar");

	sidebar->priv = g_new0 (GMAMEUISidebarPrivate, 1);
	
	sidebar->priv->main_screenshot = gmameui_get_image_from_stock ("gmameui-screen");
/* FIXME TODO
	sidebar->priv->screenshot_event_box = gtk_event_box_new ();
	gtk_box_pack_start (sidebar->priv->screenshot_hist_vbox,
			    sidebar->priv->screenshot_event_box,
			    TRUE, TRUE, 6);
	gtk_container_add (GTK_CONTAINER (sidebar->priv->screenshot_event_box),
			   GTK_WIDGET (sidebar->priv->main_screenshot));
	gtk_widget_show (sidebar->priv->screenshot_event_box);
	gtk_widget_show (sidebar->priv->main_screenshot);
*/	
	xml = glade_xml_new (GLADEDIR "sidebar.glade", "screenshot_notebook", GETTEXT_PACKAGE);
	
	sidebar->priv->screenshot_hist_vbox = gtk_vbox_new (FALSE, 6);
	gtk_container_set_border_width (GTK_CONTAINER (sidebar->priv->screenshot_hist_vbox), 6);
	gtk_container_add (GTK_CONTAINER (sidebar),
			   GTK_WIDGET (sidebar->priv->screenshot_hist_vbox));
	gtk_widget_show (GTK_WIDGET (sidebar->priv->screenshot_hist_vbox));
	
	sidebar->priv->screenshot_notebook = glade_xml_get_widget (xml, "screenshot_notebook");
	gtk_box_pack_start (GTK_BOX (sidebar->priv->screenshot_hist_vbox),
			    sidebar->priv->screenshot_notebook,
			    TRUE, TRUE, 0);
	
	/* Create the structure containing each of the ROM images */
	for (i = 0; i < NUM_GMAMEUI_PICTURE_TYPES; i++) {
		gchar *widget_name;
		
		widget_name = g_strdup_printf("screenshot_box%d", i);

		/* Set a default picture */
		sidebar->priv->gmameui_picture_type[i].image = gmameui_get_image_from_stock ("gmameui-screen");
		sidebar->priv->gmameui_picture_type[i].image_box = glade_xml_get_widget (xml, widget_name);
		
		/* Add the GtkImage to the GtkBox container */
		GMAMEUI_DEBUG ("Adding image to container %s", widget_name);
		gtk_container_add (GTK_CONTAINER (sidebar->priv->gmameui_picture_type[i].image_box),
				   sidebar->priv->gmameui_picture_type[i].image);
		
		/* When the area available to the GtkImage changes, trigger re-sizing */
		g_signal_connect (GTK_WIDGET (sidebar->priv->gmameui_picture_type[i].image_box),
				  "size-allocate",
				  G_CALLBACK (on_image_container_resized),
				  &sidebar->priv->gmameui_picture_type[i]);
		
		g_free (widget_name);
	}
	
	g_signal_connect (G_OBJECT (sidebar->priv->screenshot_notebook), "switch-page",
			    G_CALLBACK (on_screenshot_notebook_switch_page),
			    NULL);

	xml = glade_xml_new (GLADEDIR "sidebar.glade", "history_scrollwin", GETTEXT_PACKAGE);
	
	/* here we create the history box that will be filled later */
	sidebar->priv->history_scrollwin = glade_xml_get_widget (xml, "history_scrollwin");
	gtk_box_pack_end (GTK_BOX (sidebar->priv->screenshot_hist_vbox),
			  sidebar->priv->history_scrollwin,
			  TRUE, TRUE, 5);

	sidebar->priv->history_buffer = gtk_text_buffer_new (NULL);
	sidebar->priv->history_box = glade_xml_get_widget (xml, "history_box");
	gtk_text_view_set_buffer (GTK_TEXT_VIEW (sidebar->priv->history_box),
				  sidebar->priv->history_buffer);
	
	gtk_widget_show (sidebar->priv->history_scrollwin);
	gtk_widget_show (sidebar->priv->history_box);

	gtk_widget_show_all (GTK_WIDGET (sidebar));

/* FIXME TODO	gtk_widget_hide (GTK_WIDGET (sidebar->priv->screenshot_event_box));*/
	GMAMEUI_DEBUG ("Finished creating sidebar");
}

GtkWidget *
gmameui_sidebar_new (void)
{
	return g_object_new (GMAMEUI_TYPE_SIDEBAR,
			     NULL);

}

void
gmameui_sidebar_set_with_rom (GMAMEUISidebar *sidebar, MameRomEntry *rom)
{
	g_return_if_fail (sidebar != NULL);
		
	GMAMEUI_DEBUG ("Setting page");
	
	screenshot_type show_flyer;
	GtkWidget *pict = NULL;
	
	gboolean had_history;
	
	g_object_get (main_gui.gui_prefs,
		      "show-flyer", &show_flyer,
		      NULL);

	if (rom) {
		UPDATE_GUI;

		GtkRequisition requisition;

		/* Get the size available; the size will be the same for all
		   images, so get the requisition for SNAPSHOTS as representative */
		gtk_widget_size_request (sidebar->priv->gmameui_picture_type[SNAPSHOTS].image_box,
					 &requisition);

		had_history = FALSE;
		had_history = gmameui_sidebar_set_history (sidebar, rom);

		pict = get_pixbuf (rom, show_flyer, requisition.width, requisition.height);
		
		/* Set the original pixbuf; when resizing, we will use this */
		sidebar->priv->gmameui_picture_type[show_flyer].orig_pixbuf = gdk_pixbuf_copy (gtk_image_get_pixbuf (GTK_IMAGE (pict)));
		
		/* Remove and add the image, necessary to refresh the image */
		gtk_container_remove (GTK_CONTAINER (sidebar->priv->gmameui_picture_type[show_flyer].image_box),
				      sidebar->priv->gmameui_picture_type[show_flyer].image);
		sidebar->priv->gmameui_picture_type[show_flyer].image = pict;
		gtk_container_add (GTK_CONTAINER (sidebar->priv->gmameui_picture_type[show_flyer].image_box),
				   sidebar->priv->gmameui_picture_type[show_flyer].image);
		gtk_widget_show_all (sidebar->priv->gmameui_picture_type[show_flyer].image_box);
		
		if (had_history) {
			gtk_widget_show (GTK_WIDGET (sidebar->priv->history_scrollwin));
		} else {
			gtk_widget_hide (GTK_WIDGET (sidebar->priv->history_scrollwin));
		}
		
	} else {
		/* erase and hide the history box */
		gtk_text_buffer_set_text (GTK_TEXT_BUFFER (sidebar->priv->history_buffer), "", -1);
		gtk_widget_hide (GTK_WIDGET (sidebar->priv->history_scrollwin));
	}
	GMAMEUI_DEBUG ("Setting page - done");
}

static gboolean
gmameui_sidebar_set_history (GMAMEUISidebar *sidebar, MameRomEntry *rom)
{
	GtkTextIter text_iter;
	
	gboolean had_info = FALSE, had_history = FALSE;
	/* erase, fill and show the history box */
	/* should freeze the history_box here rather than each function otherwise, the position of the cursor will
		    appear between mameinfo and history */
	gtk_text_buffer_set_text (sidebar->priv->history_buffer, "", -1);
	had_history = set_game_history (rom, sidebar->priv->history_buffer);

	if (had_history) {
		gtk_text_buffer_get_end_iter (sidebar->priv->history_buffer, &text_iter);
		gtk_text_buffer_insert (sidebar->priv->history_buffer, &text_iter, "\n", -1);
	}
	had_info = set_game_info (rom, sidebar->priv->history_buffer);
	
	if ((had_info) || (had_history))
		return TRUE;
	else
		return FALSE;
}

void
gmameui_sidebar_set_current_page (GMAMEUISidebar *sidebar, int page)
{
	gtk_notebook_set_current_page (GTK_NOTEBOOK (sidebar->priv->screenshot_notebook),
				       page);
}
