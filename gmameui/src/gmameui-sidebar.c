/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * GMAMEUI
 *
 * Copyright 2007-2010 Andrew Burton <adb@iinet.net.au>
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

#include "common.h"

#include <string.h>
#include <gtkimageview/gtkimageview.h>

#include "gmameui-sidebar.h"
#include "gmameui.h"
#include "gui.h"
#include "gui_prefs.h"
#include "gmameui-zip-utils.h"

static void gmameui_sidebar_class_init (GMAMEUISidebarClass *class);
static void gmameui_sidebar_init (GMAMEUISidebar *dlg);
static gboolean gmameui_sidebar_set_history (GMAMEUISidebar *sidebar, MameRomEntry *rom);

G_DEFINE_TYPE (GMAMEUISidebar, gmameui_sidebar, GTK_TYPE_FRAME)

struct _GMAMEUISidebarPrivate {

	GtkWidget *screenshot_hist_vbox;

	GtkWidget *main_screenshot;

	GtkWidget *imagebox;
	GtkWidget *image;	/* GtkImageView widget */
	gboolean resize;	/* Only resize when this is TRUE to prevent
				   callback from re-triggering repeatedly */
	
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

static float
get_resize_ratio (GdkPixbuf *pixbuf, GtkAllocation *allocation)
{
	gdouble ratio;
	
	gdouble width_ratio = (gdouble) allocation->width / gdk_pixbuf_get_width (pixbuf);
	gdouble height_ratio = (gdouble) allocation->height / gdk_pixbuf_get_height (pixbuf);
	ratio = MIN (width_ratio, height_ratio);

	return ratio;
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
get_pixbuf (MameRomEntry *tmprom, GtkAllocation *allocation)
{
	GdkPixbuf *pixbuf;
	GtkWidget *pict = NULL;
	gchar *filename;
	gchar *filename_parent;
	gchar *zipfile;
	GError **error = NULL;

	g_return_val_if_fail (tmprom != NULL, NULL);
	
	gint dir_num = DIR_SNAPSHOT;
	/*DELETEswitch (sctype) {
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
	}*/
	
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
	
	if (!pixbuf/*DELETE && (sctype == SNAPSHOTS)*/) {

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

	if (!pixbuf) {
		//DELETEGMAMEUI_DEBUG ("no picture (%i), fall back to the default one", sctype);

		pixbuf = gmameui_get_icon_from_stock ("gmameui-screen");
	}

	pict = gtk_image_view_new ();
	gtk_image_view_set_pixbuf (GTK_IMAGE_VIEW (pict), pixbuf, TRUE);

	gdouble expected_zoom = get_resize_ratio (pixbuf, allocation);
	gtk_image_view_set_zoom (GTK_IMAGE_VIEW (pict), expected_zoom);
	
	if (pixbuf)
		g_object_unref (pixbuf);
	
	return pict;
}

static void
gmameui_sidebar_finalize (GObject *obj)
{

GMAMEUI_DEBUG ("Freeing sidebar");
	GMAMEUISidebar *dlg = GMAMEUI_SIDEBAR (obj);	

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
	GMAMEUISidebar *sidebar;	
	GdkPixbuf *pixbuf;

	sidebar = (GMAMEUISidebar *) user_data;
	
	g_return_if_fail (sidebar != NULL);

	/* Resize only every second time (prevents scaling/setting triggering another call) */
	if (sidebar->priv->resize) {
		pixbuf = gtk_image_view_get_pixbuf (GTK_IMAGE_VIEW (sidebar->priv->image));
		
		gdouble expected_zoom = get_resize_ratio (pixbuf, allocation);

		gtk_image_view_set_zoom (GTK_IMAGE_VIEW (sidebar->priv->image), expected_zoom);
	}

	/* The next time we enter this callback, we want to resize (if we
	   didn't this time) */
	sidebar->priv->resize = !(sidebar->priv->resize);
}

/* Creates a hbox containing a treeview (the sidebar) and a notebook */
static void
gmameui_sidebar_init (GMAMEUISidebar *sidebar)
{
	
GMAMEUI_DEBUG ("Creating sidebar");

	sidebar->priv = g_new0 (GMAMEUISidebarPrivate, 1);
	
	sidebar->priv->main_screenshot = gmameui_get_image_from_stock ("gmameui-screen");
	
	sidebar->priv->screenshot_hist_vbox = gtk_vbox_new (FALSE, 6);
	gtk_container_set_border_width (GTK_CONTAINER (sidebar->priv->screenshot_hist_vbox), 6);
	gtk_container_add (GTK_CONTAINER (sidebar),
			   GTK_WIDGET (sidebar->priv->screenshot_hist_vbox));
	gtk_widget_show (GTK_WIDGET (sidebar->priv->screenshot_hist_vbox));

	
	sidebar->priv->imagebox = gtk_vbox_new (TRUE, 6);
	
	gtk_box_pack_start (GTK_BOX (sidebar->priv->screenshot_hist_vbox),
	                    sidebar->priv->imagebox, TRUE, TRUE, 6);

	/* When the area available to the GtkImage changes, trigger re-sizing */
	g_signal_connect (GTK_WIDGET (sidebar->priv->imagebox),
	                  "size-allocate",G_CALLBACK (on_image_container_resized),
	                  sidebar);

	/* Create a GtkScrolledWindow containing a GtkTextView for the history details */
	sidebar->priv->history_scrollwin = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (sidebar->priv->history_scrollwin),
	                                     GTK_SHADOW_IN);
	gtk_box_pack_end (GTK_BOX (sidebar->priv->screenshot_hist_vbox),
			  sidebar->priv->history_scrollwin,
			  TRUE, TRUE, 6);

	sidebar->priv->history_buffer = gtk_text_buffer_new (NULL);
	sidebar->priv->history_box = gtk_text_view_new ();
	gtk_text_view_set_buffer (GTK_TEXT_VIEW (sidebar->priv->history_box),
				  sidebar->priv->history_buffer);

	gtk_container_add (GTK_CONTAINER (sidebar->priv->history_scrollwin),
	                   sidebar->priv->history_box);

	gtk_widget_show_all (GTK_WIDGET (sidebar));

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
	
	GtkWidget *pict = NULL;
	
	gboolean had_history;

	if (rom) {
		UPDATE_GUI;

		GtkAllocation allocation;

		/* Get the size available; the size will be the same for all
		   images, so get the requisition for SNAPSHOTS as representative */
		gtk_widget_get_allocation (sidebar->priv->imagebox,
		                           &allocation);

		had_history = FALSE;
		had_history = gmameui_sidebar_set_history (sidebar, rom);

		pict = get_pixbuf (rom, &allocation);

		/* Remove and add the image, necessary to refresh the image */
		gtk_container_remove (GTK_CONTAINER (sidebar->priv->imagebox),
				      sidebar->priv->image);
		sidebar->priv->image = pict;
		gtk_container_add (GTK_CONTAINER (sidebar->priv->imagebox),
				   sidebar->priv->image);
		gtk_widget_show_all (sidebar->priv->imagebox);
		
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
