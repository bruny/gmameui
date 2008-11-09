/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * GMAMEUI
 *
 * Copyright 2007-2008 Andrew Burton <adb@iinet.net.au>
 * based on GXMame code
 * 2002-2005 Stephane Pontier <shadow_walker@users.sourceforge.net>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "common.h"

#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <glade/glade.h>

#include "gmameui.h"
#include "gui.h"
#include "unzip.h"
#include "io.h"

const int ROM_ICON_SIZE = 16;

static guint timeout_icon;

/**** Sidebar functionality ****/

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

static void
gmameui_sidebar_set_with_rom (GMAMEUISidebar *sidebar, RomEntry *rom);

static gboolean
gmameui_sidebar_set_history (GMAMEUISidebar *sidebar, RomEntry *rom);

static void
gmameui_sidebar_set_current_page (GMAMEUISidebar *sidebar, int page);

/**** Sidebar functionality ****/

static GdkPixbuf *
get_pixbuf_from_zip_file (ZIP *zip, gchar *romname, gchar *parent_romname);

static void
set_game_pixbuff_from_iter (GtkTreeIter *iter,
			    ZIP         *zip,
			    gint         page_size)
{
	RomEntry *tmprom;
	GdkRectangle rect;
	GtkTreePath *tree_path;
	ListMode current_mode;

	gtk_tree_model_get (GTK_TREE_MODEL (main_gui.tree_model), iter, ROMENTRY, &tmprom, -1);
	tree_path = gtk_tree_model_get_path (GTK_TREE_MODEL (main_gui.tree_model), iter);
	gtk_tree_view_get_cell_area (GTK_TREE_VIEW (main_gui.displayed_list),
				     tree_path,
				     NULL, &rect);
	gtk_tree_path_free (tree_path);

	if (tmprom->has_roms == CORRECT
	    && tmprom->status
	    && (rect.y + rect.height) > 0
	    && (rect.y < page_size)
	    && !tmprom->icon_pixbuf) {
		g_object_get (main_gui.gui_prefs, "current-mode", &current_mode, NULL);
		    
		tmprom->icon_pixbuf = get_icon_for_rom (tmprom, ROM_ICON_SIZE, zip);

		if (tmprom->icon_pixbuf) {
			if ((current_mode == LIST_TREE) || (current_mode == DETAILS_TREE))
				gtk_tree_store_set (GTK_TREE_STORE (main_gui.tree_model), iter,
						    PIXBUF, tmprom->icon_pixbuf,
						    -1);
			else
				gtk_list_store_set (GTK_LIST_STORE (main_gui.tree_model), iter,
						    PIXBUF, tmprom->icon_pixbuf,
						    -1);
		}
	}
}

/* This function is to set the game icon from the zip file for each visible game */
static gboolean
adjustment_scrolled_delayed (void)
{
	GtkTreeIter iter;
	GtkTreeIter iter_child;
	GtkTreePath *tree_path;
	guint i;
	ZIP *zip;
	gchar *zipfile;
	gboolean valid;
	GtkAdjustment *vadj;
	gchar *icon_dir;
	
	g_object_get (main_gui.gui_prefs,
		      "dir-icons", &icon_dir,
		      NULL);

	/* open the zip file only at the begining */
	zipfile = g_build_filename (icon_dir, "icons.zip", NULL);
	zip = openzip (zipfile);

	/* Getting the vertical window area */
	vadj=gtk_scrolled_window_get_vadjustment (GTK_SCROLLED_WINDOW (main_gui.scrolled_window_games));

	/* Disable the callback */
	g_signal_handlers_block_by_func (G_OBJECT (gtk_scrolled_window_get_vadjustment (GTK_SCROLLED_WINDOW (main_gui.scrolled_window_games))),
					 (gpointer)adjustment_scrolled, NULL);
/* FIXME TODO Use gtk_tree_model_foreach similar to callback for selecting random row */
	if (visible_games > 0) {
		valid = gtk_tree_model_get_iter_first (GTK_TREE_MODEL (main_gui.tree_model), &iter);
		set_game_pixbuff_from_iter (&iter,zip, (gint) (vadj->page_size));
		i = 0;
		while ((i < visible_games) && valid) {
			tree_path = gtk_tree_model_get_path (GTK_TREE_MODEL (main_gui.tree_model), &iter);
			/* Set the icon for all children if the parent row is expanded */
			if (gtk_tree_view_row_expanded (GTK_TREE_VIEW (main_gui.displayed_list), tree_path)) {
				if (gtk_tree_model_iter_children (GTK_TREE_MODEL (main_gui.tree_model), &iter_child, &iter)) {
					set_game_pixbuff_from_iter (&iter_child,zip, (gint) (vadj->page_size));
					while ((i < visible_games) && (gtk_tree_model_iter_next (GTK_TREE_MODEL (main_gui.tree_model), &iter_child)) ) {
						set_game_pixbuff_from_iter (&iter_child, zip, (gint) (vadj->page_size));
						i++;
					}
				}
			}

			gtk_tree_path_free (tree_path);
			if (i < visible_games) {
				valid=gtk_tree_model_iter_next (GTK_TREE_MODEL (main_gui.tree_model), &iter);
				if (valid) {
					set_game_pixbuff_from_iter (&iter,zip, (gint) (vadj->page_size));
					i++;
				}
			}
		}
	}

	/* Re-Enable the callback */
	g_signal_handlers_unblock_by_func (G_OBJECT (gtk_scrolled_window_get_vadjustment (GTK_SCROLLED_WINDOW (main_gui.scrolled_window_games))),
					 (gpointer) adjustment_scrolled, NULL);

	if (zip)
		closezip (zip);
	g_free (zipfile);
	g_free (icon_dir);
	
	return FALSE;
}

void
adjustment_scrolled (GtkAdjustment *adjustment,
		     gpointer       user_data)
{
	if (timeout_icon)
		g_source_remove (timeout_icon);
	timeout_icon =
		g_timeout_add (ICON_TIMEOUT,
			       (GSourceFunc) adjustment_scrolled_delayed, NULL);
}

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
set_game_history (const RomEntry *rom,
		  GtkTextBuffer  *text_buffer)
{
	const gchar *entry_name;

	if (!rom || !rom->romname)
		return FALSE;

	
	if (rom->cloneof && (rom->cloneof[0] != '-'))
		entry_name = rom->cloneof;
	else
		entry_name = rom->romname;

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
set_game_info (const RomEntry *rom,
	       GtkTextBuffer  *text_buffer)
{
	const gchar *entry_name;

	if (!rom || !rom->romname)
		return FALSE;

	
	if (rom->cloneof && (rom->cloneof[0] != '-'))
		entry_name = rom->cloneof;
	else
		entry_name = rom->romname;

	return set_info (entry_name, text_buffer);
}

/* Get a pixbuf from a zip file for a specified rom. If the pixbuf could not
   be found, search for a pixbuf for the parent */
static GdkPixbuf *
get_pixbuf_from_zip_file (ZIP *zip, gchar *romname, gchar *parent_romname)
{
	struct zipent* zipent;
	GdkPixbuf *pixbuf = NULL;
	gchar *filename;
	gchar *tmp_buffer;
	gchar *parent_tmp_buffer = NULL;
	gsize parent_buf_size = 0;
	gchar *parent_filename;
	GError *error = NULL;

	GdkPixbufLoader *loader;

	filename = g_strdup_printf ("%s.", romname);
	parent_filename = g_strdup_printf ("%s.", parent_romname);

	while ((zipent = readzip (zip)) != 0) {
		/* this should allows to find any format of picture in the zip, not only bmp */
		if (!strncmp (filename,zipent->name, strlen (romname) + 1)) {
			GMAMEUI_DEBUG (_("Found file name %s with CRC: %u, size %i"),
				       zipent->name,
				       zipent->crc32,
				       zipent->uncompressed_size);
			
			tmp_buffer = read_zipentry (zip, zipent);
			
			if (tmp_buffer) {
				GMAMEUI_DEBUG (_("Attempting to uncompress %s"), zipent->name);
				/* if the file successfully uncompress, try to load it in a pixbuf loader */
				loader = gdk_pixbuf_loader_new ();
				if (gdk_pixbuf_loader_write (loader, (guchar *)tmp_buffer, (gsize) zipent->uncompressed_size, &error)) {
					pixbuf = gdk_pixbuf_loader_get_pixbuf (loader);
					gdk_pixbuf_loader_close (loader, &error);
					GMAMEUI_DEBUG (_("Loaded pixbuf"));
				} else {
					GMAMEUI_DEBUG (_("Error while uncompressing %s: %s"), zipent->name, error->message);
					g_error_free (error);
					error = NULL;
				}
				g_free (tmp_buffer);
				tmp_buffer = NULL;
			}
			/* prevent to read all zip file if we have found the picture's game (uncompressed successfuly or not) */
			break;

		} else if (!strncmp (parent_filename, zipent->name, strlen (parent_romname) + 1)) {
			parent_tmp_buffer = read_zipentry (zip, zipent);
			parent_buf_size = zipent->uncompressed_size;
		}
	}
	g_free (filename);
	g_free (parent_filename);

	/* no picture found try parent game if any*/
	if (!pixbuf && parent_tmp_buffer) {
		loader = gdk_pixbuf_loader_new ();
		if (!gdk_pixbuf_loader_write (loader, (guchar *)parent_tmp_buffer, parent_buf_size, &error)) {
			GMAMEUI_DEBUG (_("Error while uncompressing %s: %s"), zipent->name, error->message);
		} else {
			pixbuf = gdk_pixbuf_loader_get_pixbuf (loader);
			gdk_pixbuf_loader_close (loader, &error);
		}
	}

	g_free (parent_tmp_buffer);
					   
	return pixbuf;
}


/* Returns a GtkWidget representing a GtkImage */
static GtkWidget *
get_pixbuf (RomEntry       *rom,
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

	if (!rom)
		return NULL;
	
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
	
	filename = g_strdup_printf ("%s" G_DIR_SEPARATOR_S "%s.png", directory_name, rom->romname);
	filename_parent = g_strdup_printf ("%s" G_DIR_SEPARATOR_S "%s.png", directory_name, rom->cloneof);

	GMAMEUI_DEBUG ("Looking for image %s", filename);
	pixbuf = gdk_pixbuf_new_from_file (filename, error);
	if (filename)
		g_free (filename);
	
	/* no picture found try parent game if any*/
	if ( (!pixbuf) && strcmp (rom->cloneof,"-")) {
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
		filename = g_strdup_printf ("%s" G_DIR_SEPARATOR_S "%s" G_DIR_SEPARATOR_S "0000.png", snapshot_dir, rom->romname);
		GMAMEUI_DEBUG ("Looking for image %s", filename);
		pixbuf = gdk_pixbuf_new_from_file (filename,error);
		g_free (filename);

		/* If not found, look in parent folder */
		if ((!pixbuf) && strcmp (rom->cloneof,"-")) {
			filename = g_strdup_printf ("%s" G_DIR_SEPARATOR_S "%s" G_DIR_SEPARATOR_S "0000.png", snapshot_dir, rom->cloneof);
			GMAMEUI_DEBUG ("Looking for parent image %s", filename);
			pixbuf = gdk_pixbuf_new_from_file (filename,error);
			g_free (filename);
		}
			
		g_free (snapshot_dir);
	}
	
	/* we havent found the picture in the directory, maybe we could try in a zipfile */
	if (!pixbuf) {
		ZIP *zip;
		gchar *snapshotdir;
		g_object_get (main_gui.gui_prefs, "dir-snapshot", &snapshotdir, NULL);
		zipfile = g_build_filename (snapshotdir, "snap.zip", NULL);
		zip = openzip (zipfile);

		if (zip) {
			GMAMEUI_DEBUG ("Looking for image in zip file %s", zipfile);
			pixbuf = get_pixbuf_from_zip_file (zip, rom->romname, rom->cloneof);
			closezip (zip);
		} else
			GMAMEUI_DEBUG (_("Error - cannot open zip file %s"), zipfile);

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

void
init_gui (void)
{
	GtkTooltips *tooltips;
	gchar *filename;

	screenshot_type show_flyer;
	
#ifdef ENABLE_DEBUG
	GTimer *mytimer;

	mytimer = g_timer_new ();
	g_timer_start (mytimer);
#endif
	tooltips = gtk_tooltips_new ();

	/* Default Pixbuf once for all windows */
	filename = g_build_filename (DATADIR, "gmameui", "gmameui.png", NULL);
        gtk_window_set_default_icon_from_file (filename, NULL);
	g_free (filename);

	gmameui_icons_init ();	
#ifdef ENABLE_DEBUG
g_message (_("Time to initialise icons: %.02f seconds"), g_timer_elapsed (mytimer, NULL));
#endif
	/* Create the main window */
	MainWindow = create_MainWindow ();
#ifdef ENABLE_DEBUG
g_message (_("Time to create main window, filters and gamelist: %.02f seconds"), g_timer_elapsed (mytimer, NULL));
#endif

	/* Show and hence realize mainwindow so that MainWindow->window is available */
	gtk_widget_show (MainWindow);
	
	/* Need to set the size here otherwise it move when we create the gamelist 
	if (show_screenshot)
		gtk_paned_set_position (main_gui.hpanedRight, xpos_gamelist);*/

	/* Grab focus on the game list */
	gtk_widget_grab_focus (main_gui.displayed_list);

	g_signal_connect (G_OBJECT (gtk_scrolled_window_get_vadjustment (GTK_SCROLLED_WINDOW (main_gui.scrolled_window_games))), "changed",
	                  G_CALLBACK (adjustment_scrolled),
	                  NULL);
	g_signal_connect (G_OBJECT (gtk_scrolled_window_get_vadjustment (GTK_SCROLLED_WINDOW (main_gui.scrolled_window_games))), "value-changed",
	                  G_CALLBACK (adjustment_scrolled),
	                  NULL);

	/* Need to set the notebook page here otherwise it segfault */
	g_object_get (main_gui.gui_prefs, "show-flyer", &show_flyer, NULL);
	gmameui_sidebar_set_current_page (main_gui.screenshot_hist_frame, show_flyer);
	
#ifdef ENABLE_DEBUG
	g_timer_stop (mytimer);
	g_message (_("Time to complete start of UI: %.02f seconds"), g_timer_elapsed (mytimer, NULL));
	g_timer_destroy (mytimer);
#endif
}

static void
set_current_executable (MameExec *new_exec)
{
	gboolean valid;
	
	if (new_exec) {
		GMAMEUI_DEBUG ("Executable changed to %s", mame_exec_get_path (new_exec));
		mame_exec_list_set_current_executable (main_gui.exec_list, new_exec);
		g_object_set (main_gui.gui_prefs, "current-executable", mame_exec_get_path (new_exec), NULL);
	} else {
		gmameui_message (ERROR, NULL, _("Executable is not valid MAME executable... skipping"));
		/* FIXME TODO Remove the executable from the list */
	}

	/* Set sensitive the UI elements that require an executable to be set */
	gtk_action_group_set_sensitive (main_gui.gmameui_rom_exec_action_group, new_exec != NULL);
	gtk_action_group_set_sensitive (main_gui.gmameui_exec_action_group, new_exec != NULL);
}

/* executable selected from the menu */
static void
on_executable_selected (GtkRadioAction *action,
			gpointer          user_data)
{
	gint index = GPOINTER_TO_INT (user_data);

	MameExec *exec;

	exec = mame_exec_list_nth (main_gui.exec_list, index);

	GMAMEUI_DEBUG ("Picking executable %d - %s", index, mame_exec_get_name (exec));
	set_current_executable (exec);
}

/* Dynamically create the executables menu. */
void
add_exec_menu (void)
{
	gchar *current_exec;
	int num_execs;
	int i;
	MameExec *exec;

	g_object_get (main_gui.gui_prefs, "current-executable", &current_exec, NULL);
	
	/* This is really hard to understand from the API. Why do they make it so hard?
	   Ref: http://log.emmanuelebassi.net/archives/2006/08/boogie-woogie-bugle-boy/ */

	/* Remove any existing group if it exists */
	if (main_gui.gmameui_exec_radio_action_group) {
		gtk_ui_manager_remove_ui (main_gui.manager,
					  main_gui.gmameui_exec_merge_id);
		gtk_ui_manager_remove_action_group (main_gui.manager,
						    main_gui.gmameui_exec_radio_action_group);
		g_object_unref (main_gui.gmameui_exec_radio_action_group);
		main_gui.gmameui_exec_radio_action_group = NULL;
	}

	/* Create a new radio action group for the executables */
	GtkActionGroup *exec_radio_action_group;
	GSList *radio_group = NULL;
	exec_radio_action_group = gtk_action_group_new ("GmameuiWindowRadioExecActions");
	gtk_ui_manager_insert_action_group (main_gui.manager, exec_radio_action_group, 0);
	
	num_execs = mame_exec_list_size (main_gui.exec_list);

	main_gui.gmameui_exec_merge_id = gtk_ui_manager_new_merge_id (main_gui.manager);
	 
	/* No items - attach an insensitive place holder */
	if (num_execs == 0) {
		GtkAction *action;
			
		action = g_object_new (GTK_TYPE_ACTION,
				       "name", "execs-empty",
				       "label", N_("No executables"),
				       "sensitive", FALSE,
				       NULL);
		gtk_action_group_add_action (exec_radio_action_group, action);
		g_object_unref (action);
		
		gtk_ui_manager_add_ui (main_gui.manager,
				       main_gui.gmameui_exec_merge_id,
				       "/MenuBar/OptionsMenu/OptionMameExecutables",
				       "execs-empty",
				       "execs-empty",
				       GTK_UI_MANAGER_MENUITEM,
				       FALSE);
	} else {
		for (i = 0; i < num_execs; i++) {
			exec = mame_exec_list_nth (main_gui.exec_list, i);
			gchar *exec_name = g_strdup_printf ("%s (%s) %s",
							    mame_exec_get_name (exec),
							    mame_exec_get_target (exec),
							    mame_exec_get_version (exec));
		
			/* Create a new radio action for the executable */
			gchar *name = g_strdup_printf ("exec-%d", i);
			gchar *action_name = g_strdup (name);
			GtkRadioAction *action;
			action = g_object_new (GTK_TYPE_RADIO_ACTION,
					       "name", action_name,
					       "label", exec_name,
					       "value", i,
					       "stock_id", NULL,
					       NULL);
			g_signal_connect (action, "activate",
					  G_CALLBACK (on_executable_selected),
					  GINT_TO_POINTER (i));
			gtk_action_group_add_action (exec_radio_action_group, GTK_ACTION (action));
			gtk_radio_action_set_group (action, radio_group);
			radio_group = gtk_radio_action_get_group (action);
			g_object_unref (action);
		
			gtk_ui_manager_add_ui (main_gui.manager,
					       main_gui.gmameui_exec_merge_id,
					       "/MenuBar/OptionsMenu/OptionMameExecutables",
					       name, action_name,
					       GTK_UI_MANAGER_MENUITEM,
					       FALSE);
			GMAMEUI_DEBUG ("Adding action '%s'", action_name);
			
			if (current_exec) {
				if (g_ascii_strcasecmp (mame_exec_get_path (exec), current_exec) == 0) {
					GMAMEUI_DEBUG ("Setting %s as current executable", mame_exec_get_path (exec));
					gtk_radio_action_set_current_value (action, i);
				}
			}
		
			/* TODO Free the executable */
		
			g_free (action_name);
			g_free (exec_name);
			g_free (name);	
		}
		
	}
	main_gui.gmameui_exec_radio_action_group = exec_radio_action_group;

	g_free (current_exec);
}

void gmameui_ui_set_favourites_sensitive (gboolean rom_is_favourite)
{
	gtk_widget_set_sensitive (gtk_ui_manager_get_widget (main_gui.manager,
							     "/MenuBar/FileMenu/FileFavesAddMenu"),
				  !rom_is_favourite);
	gtk_widget_set_sensitive (gtk_ui_manager_get_widget (main_gui.manager,
							     "/MenuBar/FileMenu/FileFavesRemoveMenu"),
				  rom_is_favourite);
	gtk_widget_set_sensitive (gtk_ui_manager_get_widget (main_gui.manager,
							     "/GameListPopup/FileFavesAdd"),
				  !rom_is_favourite);
	gtk_widget_set_sensitive (gtk_ui_manager_get_widget (main_gui.manager,
							     "/GameListPopup/FileFavesRemove"),
				  rom_is_favourite);
}

void gmameui_ui_set_items_sensitive () {

	gboolean rom_and_exec;  /* TRUE if both an executable exists and a ROM is selected */
	rom_and_exec = (gui_prefs.current_game) && (mame_exec_list_size (main_gui.exec_list) > 0);

	gtk_action_group_set_sensitive (main_gui.gmameui_rom_exec_action_group,
					rom_and_exec);
	gtk_action_group_set_sensitive (main_gui.gmameui_exec_action_group,
					mame_exec_list_size (main_gui.exec_list));

	/* Disable ROM and Favourites UI items if no current game */
	gtk_action_group_set_sensitive (main_gui.gmameui_rom_action_group,
					(gui_prefs.current_game != NULL));
	gtk_action_group_set_sensitive (main_gui.gmameui_favourite_action_group,
					(gui_prefs.current_game != NULL));
	
	/* Set the Add/Remove Favourites depending whether the game is a favourite */
	if (gui_prefs.current_game != NULL)
		gmameui_ui_set_favourites_sensitive (gui_prefs.current_game->favourite);
}

void
gamelist_popupmenu_show (GdkEventButton *event)
{
	GtkWidget *popup_menu;

	popup_menu = gtk_ui_manager_get_widget (main_gui.manager, "/GameListPopup");
	g_return_if_fail (popup_menu != NULL);

	MameExec *exec = mame_exec_list_get_current_executable (main_gui.exec_list);
	if (exec)
		mame_get_options (exec);
	

	gtk_menu_popup (GTK_MENU (popup_menu), NULL, NULL,
			NULL, NULL,
			event->button, event->time);
}

/* Gets the pixbuf representing a ROM's icon (if the icon directory is
   specified). Search first for the .ico file for the ROM, then for the parent
   ROM, then for the icon zipfile */
GdkPixbuf *
get_icon_for_rom (RomEntry *rom,
		  guint     size,
		  ZIP      *zip)
{
	GdkPixbuf *pixbuf, *scaled_pixbuf = NULL;
	gchar *icon_filename;
	gchar *icon_path;
	gchar *icon_dir;
	GError **error = NULL;

	if (!rom)
		return NULL;

	GMAMEUI_DEBUG ("Attempting to get icon for ROM %s", rom->romname);
	
	g_object_get (main_gui.gui_prefs, "dir-icons", &icon_dir, NULL);
	
	icon_filename = g_strdup_printf ("%s.ico", rom->romname);
	icon_path = g_build_filename (icon_dir, icon_filename, NULL);
	pixbuf = gdk_pixbuf_new_from_file (icon_path, error);
	
	g_free (icon_filename);
	g_free (icon_path);

	/* If icon not found, try looking for parent's icon */
	if ((pixbuf == NULL) && strcmp (rom->cloneof, "-")) {
		GMAMEUI_DEBUG ("Attempting to get icon for ROM %s from parent %s", rom->romname, rom->cloneof);
		icon_filename = g_strdup_printf ("%s.ico", rom->cloneof);
		icon_path = g_build_filename (icon_dir, icon_filename, NULL);
		pixbuf = gdk_pixbuf_new_from_file (icon_path, error);

		g_free (icon_filename);
		g_free (icon_path);
	}

	/* If icon not found, look in a zipfile */
	if (pixbuf == NULL) {
		if (zip != 0) {
			GMAMEUI_DEBUG ("Attempting to get icon for ROM %s from icon zipfile", rom->romname);
			rewindzip (zip);
			pixbuf = get_pixbuf_from_zip_file (zip, rom->romname, rom->cloneof);					
		}
	}

	if (pixbuf != NULL) {
		GMAMEUI_DEBUG ("Found icon for ROM %s, scaling to size %ix%i", rom->romname, size, size);
		scaled_pixbuf = gdk_pixbuf_scale_simple (pixbuf, size, size, GDK_INTERP_BILINEAR);
		g_object_unref (pixbuf);
	} else
		GMAMEUI_DEBUG ("Could not find icon for %s", rom->romname);
	
	g_free (icon_dir);
	
	return scaled_pixbuf;
}

/* Returns a pixbuf from the current GNOME theme for a
   specified stock ID */
static GdkPixbuf *
gmameui_get_icon_from_theme (const char *id,
			    GtkIconSize size)
{
	GtkIconTheme *theme;
	int width, height;
	GdkPixbuf *pixbuf;

	theme = gtk_icon_theme_get_default ();
        gtk_icon_size_lookup (size, &width, &height);

	pixbuf = gtk_icon_theme_load_icon (theme, id, width, 0, NULL);

	return pixbuf;
}

/* Returns a pixbuf from the GMAMEUI install directory for a
   specified filename */
static GdkPixbuf *
gmameui_get_pixbuf_from_gmameui_install (const char *filename)
{
	GdkPixbuf *pixbuf;
	gchar *path, *fn;

	fn = g_strconcat (filename, ".png", NULL);
	
	path = g_build_filename (DATADIR, "gmameui", fn, NULL);
	pixbuf = gdk_pixbuf_new_from_file (path, NULL);
	
	g_free (fn);
	g_free (path);

	return pixbuf;
}

static GdkPixbuf *
composite_icons (const GdkPixbuf *src1,
		 const GdkPixbuf *src2)
{
	GdkPixbuf *dest;
	GdkPixbuf *scaled;
	gint w1, w2, h1, h2;
	gint dest_x, dest_y;
	gboolean do_scale;

	if (!src1)
		return NULL;

	dest = gdk_pixbuf_copy (src1);

	if (!src2)
		return dest;

	w1 = gdk_pixbuf_get_width (src1);
	h1 = gdk_pixbuf_get_height (src1);
	w2 = gdk_pixbuf_get_width (src2);
	h2 = gdk_pixbuf_get_height (src2);

	do_scale = ((float)w1 * 0.8) < w2;

	/* scale the emblem down if it will obscure the entire bottom image */
	if (do_scale)
		scaled = gdk_pixbuf_scale_simple (src2, w1 / 2, h1 / 2, GDK_INTERP_BILINEAR);
	else
		scaled = (GdkPixbuf *)src2;

	w2 = gdk_pixbuf_get_width (scaled);
	h2 = gdk_pixbuf_get_height (scaled);

	dest_x = w1 - w2;
	dest_y = h1 - h2;

	gdk_pixbuf_composite (scaled, dest,
			      dest_x, dest_y,
			      w2, h2,
			      dest_x, dest_y,
			      1.0, 1.0,
			      GDK_INTERP_BILINEAR, 0xFF);

	if (do_scale)
		g_object_unref (scaled);

	return dest;
}

static GHashTable *icons_table = NULL;

#define GMAMEUI_PIXBUF_FOR_ID(x) (g_hash_table_lookup (icons_table, x))
#define GMAMEUI_ICON_SIZE_MENU    "menu"
#define GMAMEUI_ICON_SIZE_BUTTON  "button"
#define GMAMEUI_ICON_SIZE_TOOLBAR "toolbar"
#define GMAMEUI_ICON_SIZE_FOLDER  "toolbar"
#define GMAMEUI_ICON_SIZE_EMBLEM  "toolbar"
#define GMAMEUI_ICON_SIZE_DIALOG  "dialog"

static void
add_composite_icon_from_stock (const char *new_id,
			       const char *id1,
			       const char *id2)
{
	GdkPixbuf *bottom_pixbuf;
	GdkPixbuf *top_pixbuf;
	GdkPixbuf *pixbuf;

	bottom_pixbuf = gmameui_get_icon_from_stock (id1);
	top_pixbuf = gmameui_get_icon_from_stock (id2);

	pixbuf = composite_icons (bottom_pixbuf, top_pixbuf);
	g_hash_table_insert (icons_table, (gpointer) new_id, (gpointer) pixbuf);

	if (top_pixbuf)
		g_object_unref (top_pixbuf);
	if (bottom_pixbuf)
		g_object_unref (bottom_pixbuf);
}

static void
add_composite_icons (void)
{
	add_composite_icon_from_stock ("gmameui-folder-favorite", "gmameui-folder", "gmameui-emblem-favorite");
	add_composite_icon_from_stock ("gmameui-folder-sound", "gmameui-folder", "gmameui-emblem-sound");
	add_composite_icon_from_stock ("gmameui-folder-available", "gmameui-folder", "gmameui-emblem-correct");
	add_composite_icon_from_stock ("gmameui-folder-unavailable", "gmameui-folder", "gmameui-emblem-unavailable");
    	add_composite_icon_from_stock ("gmameui-folder-incorrect", "gmameui-folder", "gmameui-emblem-incorrect");
	add_composite_icon_from_stock ("gmameui-folder-played", "gmameui-folder", "gmameui-emblem-played");
    	add_composite_icon_from_stock ("gmameui-folder-working", "gmameui-folder", "gmameui-emblem-correct");
	add_composite_icon_from_stock ("gmameui-folder-not-working", "gmameui-folder", "gmameui-emblem-not-working");
	add_composite_icon_from_stock ("gmameui-folder-date", "gmameui-folder", "gmameui-emblem-date");
	add_composite_icon_from_stock ("gmameui-folder-manufacturer", "gmameui-folder", "gmameui-emblem-manufacturer");

	add_composite_icon_from_stock ("gmameui-rom-correct", "gmameui-rom", "gmameui-emblem-correct");
	add_composite_icon_from_stock ("gmameui-rom-unavailable", "gmameui-rom", "gmameui-emblem-unavailable");
	add_composite_icon_from_stock ("gmameui-rom-incorrect", "gmameui-rom", "gmameui-emblem-incorrect");
	add_composite_icon_from_stock ("gmameui-rom-problems", "gmameui-rom", "gmameui-emblem-not-working");
	add_composite_icon_from_stock ("gmameui-rom-unknown", "gmameui-rom", "gmameui-emblem-unknown");
}

void
gmameui_icons_init (void)
{
	GtkIconTheme *theme;
	int i;
	static char *items[][4] = {
		/* { gmameui-stock-id, size-string, theme-stock-id, fallback-stock-id } */
		{ "gmameui-view-list", GMAMEUI_ICON_SIZE_MENU, "stock_list_enum-off", "gmameui-view-list" },
		{ "gmameui-view-tree", GMAMEUI_ICON_SIZE_MENU, "stock_navigator-levels", "gmameui-view-tree" },
		{ "gmameui-view-folders", GMAMEUI_ICON_SIZE_MENU, "stock_toggle-preview", "gmameui-view-folders" },
		{ "gmameui-view-screenshot", GMAMEUI_ICON_SIZE_MENU, "stock_toggle-preview", "gmameui-view-screenshot" },
		{ "gmameui-general-toolbar", GMAMEUI_ICON_SIZE_TOOLBAR, "gmameui-general", NULL },
		{ "gmameui-sound-toolbar", GMAMEUI_ICON_SIZE_TOOLBAR, "stock_volume", "gmameui-sound" },
		{ "gmameui-display-toolbar", GMAMEUI_ICON_SIZE_TOOLBAR, "display", "gmameui-sound" },
		{ "gmameui-joystick-toolbar", GMAMEUI_ICON_SIZE_TOOLBAR, "gnome-joystick", "gmameui-joystick" },
		{ "gmameui-joystick", GMAMEUI_ICON_SIZE_DIALOG, "gnome-joystick", "gmameui-joystick" },
		{ "gmameui-mouse", GMAMEUI_ICON_SIZE_DIALOG, "gnome-dev-mouse-optical", "gmameui-mouse" },
		{ "gmameui-keyboard", GMAMEUI_ICON_SIZE_DIALOG, "gnome-dev-keyboard", "gmameui-keyboard" },
		{ "gmameui-folder", GMAMEUI_ICON_SIZE_FOLDER, "stock_folder", "gmameui-folder" },
		{ "gmameui-folder-open", GMAMEUI_ICON_SIZE_FOLDER, "stock_open", "gmameui-folder-open" },
		{ "gmameui-rom", GMAMEUI_ICON_SIZE_FOLDER, "gnome-dev-media-ms", "gmameui-rom" },
		{ "gmameui-emblem-favorite", GMAMEUI_ICON_SIZE_EMBLEM, "emblem-favorite", "gmameui-emblem-favorite" },
		{ "gmameui-emblem-played", GMAMEUI_ICON_SIZE_EMBLEM, "emblem-distinguished", "gmameui-emblem-played" },
		{ "gmameui-emblem-correct", GMAMEUI_ICON_SIZE_EMBLEM, "emblem-default", "gmameui-emblem-correct" },
		{ "gmameui-emblem-incorrect", GMAMEUI_ICON_SIZE_EMBLEM, "emblem-urgent", "gmameui-emblem-incorrect" },
		{ "gmameui-emblem-unavailable", GMAMEUI_ICON_SIZE_EMBLEM, "emblem-noread", "gmameui-emblem-unavailable" },
		{ "gmameui-emblem-sound", GMAMEUI_ICON_SIZE_EMBLEM, "emblem-sound", "gmameui-emblem-sound" },
		{ "gmameui-emblem-not-working", GMAMEUI_ICON_SIZE_EMBLEM, "emblem-important", "gmameui-emblem-not-working" },
		{ "gmameui-emblem-date", GMAMEUI_ICON_SIZE_EMBLEM, "stock_calendar-view-year", "gmameui-emblem-date" },
		{ "gmameui-emblem-unknown", GMAMEUI_ICON_SIZE_EMBLEM, "stock_unknown", "gmameui-emblem-unknown" },
		{ "gmameui-emblem-manufacturer", GMAMEUI_ICON_SIZE_EMBLEM, "gnome-run", "gmameui-emblem-menufacturer" },
	};

	theme = gtk_icon_theme_get_default ();

	if (icons_table == NULL) {
		icons_table = g_hash_table_new (g_str_hash, g_str_equal);
	}

	for (i = 0; i < (int) G_N_ELEMENTS (items); i++) {
		GdkPixbuf *pixbuf = NULL;
		GtkIconSize size;
		int num;
		int j;

		num = G_N_ELEMENTS (items[i]);

		if (strcmp (items[i][1], "menu") == 0)
			size = GTK_ICON_SIZE_MENU;
		else if (strcmp (items[i][1], "toolbar") == 0)
			size = GTK_ICON_SIZE_LARGE_TOOLBAR;
		else if (strcmp (items[i][1], "emblem") == 0)
			size = GTK_ICON_SIZE_LARGE_TOOLBAR;
		else if (strcmp (items[i][1], "button") == 0)
			size = GTK_ICON_SIZE_BUTTON;
		else
			size = GTK_ICON_SIZE_DIALOG;

		for (j = 2; j < num && items[i][j]; j++) {
			pixbuf = gmameui_get_icon_from_theme (items[i][j], size);
			if (pixbuf)
				break;
		}

		if (pixbuf == NULL) {
			for (j = 2; j < num && items[i][j]; j++) {
				pixbuf = gmameui_get_pixbuf_from_gmameui_install (items[i][j]);
				if (pixbuf)
					break;
			}

		}

		if (pixbuf == NULL) {
			g_warning ("Couldn't find themed icon for \"%s\"",
				   items[i][0]);
			continue;
		}

		/* The pixbuf will be unref'ed when we destroy the
		 * hash table */
		g_hash_table_insert (icons_table, (gpointer) items[i][0], (gpointer) pixbuf);
	}

	/* add special case for screenshot */
	{
		GdkPixbuf *pixbuf;
		pixbuf = gmameui_get_pixbuf_from_gmameui_install ("gmameui-screen");
		g_hash_table_insert (icons_table, (gpointer) "gmameui-screen", (gpointer) pixbuf);
	}	

	/* build the composite icons */
	add_composite_icons ();
}

GdkPixbuf *
gmameui_get_icon_from_stock (const char *id)
{
	GdkPixbuf *pixbuf = NULL;

	pixbuf = GMAMEUI_PIXBUF_FOR_ID (id);
	if (pixbuf)
		g_object_ref (pixbuf);
	
	return pixbuf;
}

GtkWidget *
gmameui_get_image_from_stock (const char *id)
{
	GtkWidget *image = NULL;
	GdkPixbuf *pixbuf;

	pixbuf = gmameui_get_icon_from_stock (id);

	if (pixbuf) {
		image = gtk_image_new_from_pixbuf (pixbuf);
		g_object_unref (pixbuf);
	} else {
		image = gtk_image_new_from_stock (GTK_STOCK_MISSING_IMAGE, GTK_ICON_SIZE_BUTTON);
	}
	
	return image;
}

/* this function checks if an icon update is needed and get the icon */
void
get_status_icons (void)
{
	if (dirty_icon_cache
	    || !Status_Icons [CORRECT]
	    || !Status_Icons [INCORRECT]
	    || !Status_Icons [UNKNOWN]
	    || !Status_Icons [PROBLEMS]
	    || !Status_Icons [NOT_AVAIL]) {
		if (dirty_icon_cache) {
			if (Status_Icons [CORRECT]) {
				g_object_unref (Status_Icons [CORRECT]);
				Status_Icons [CORRECT] = NULL;
			}
			if (Status_Icons [INCORRECT]) {
				g_object_unref (Status_Icons [INCORRECT]);
				Status_Icons [INCORRECT] = NULL;
			}
		    	if (Status_Icons[UNKNOWN]) {
				g_object_unref (Status_Icons [UNKNOWN]);
				Status_Icons [UNKNOWN] = NULL;
			}
			if (Status_Icons [PROBLEMS]) {
				g_object_unref (Status_Icons [PROBLEMS]);
				Status_Icons [PROBLEMS] = NULL;
			}
			if (Status_Icons [BEST_AVAIL]) {
				g_object_unref (Status_Icons [BEST_AVAIL]);
				Status_Icons [BEST_AVAIL] = NULL;
			}
			if (Status_Icons[NOT_AVAIL]) {
				g_object_unref (Status_Icons [NOT_AVAIL]);
				Status_Icons [NOT_AVAIL] = NULL;
			}
		}

	 	if (Status_Icons [CORRECT] == NULL)
			Status_Icons [CORRECT] = gmameui_get_icon_from_stock ("gmameui-rom-correct");
	 	if (Status_Icons [INCORRECT] == NULL)
			Status_Icons [INCORRECT] = gmameui_get_icon_from_stock ("gmameui-rom-incorrect");
	 	if (Status_Icons [UNKNOWN] == NULL)
			Status_Icons [UNKNOWN] =  gmameui_get_icon_from_stock ("gmameui-rom-unknown");
	 	if (Status_Icons [PROBLEMS] == NULL)
			Status_Icons [PROBLEMS] = gmameui_get_icon_from_stock ("gmameui-rom-problems");
	 	if (Status_Icons [BEST_AVAIL] == NULL)
			Status_Icons [BEST_AVAIL] = gmameui_get_icon_from_stock ("gmameui-rom-problems");
		if (Status_Icons [NOT_AVAIL] == NULL)
			Status_Icons [NOT_AVAIL] = gmameui_get_icon_from_stock ("gmameui-rom-unavailable");
	}
}

void
select_game (RomEntry *rom)
{
	gui_prefs.current_game = rom;

	if (rom != NULL) {
		/* update statusbar */
		set_status_bar (rom_entry_get_list_name (rom),
				_(rom_status_string_value [rom->has_roms]));

		/* update screenshot panel */
		gmameui_sidebar_set_with_rom (GMAMEUI_SIDEBAR (main_gui.screenshot_hist_frame),
					      rom);
	} else {
		/* no roms selected display the default picture */

		GMAMEUI_DEBUG ("no games selected");
		
		/* update statusbar */
		set_status_bar (_("No game selected"), "");

		/* update screenshot panel */
		gmameui_sidebar_set_with_rom (GMAMEUI_SIDEBAR (main_gui.screenshot_hist_frame),
					      NULL);
	}

	gmameui_ui_set_items_sensitive ();
}

void
update_game_in_list (RomEntry *tmprom)
{
	const gchar *my_hassamples;
	GdkColor my_txtcolor;
	GdkPixbuf *pixbuf;
	gboolean is_tree_store;
	ListMode current_mode;
	gchar *clone_color;

	if (!tmprom)
		return;

	g_object_get (main_gui.gui_prefs,
		      "current-mode", &current_mode,
		      "clone-color", &clone_color,
		      NULL);
	
	/* Whether the Tree Model will a tree or a list */
	is_tree_store = (current_mode == LIST_TREE) || (current_mode == DETAILS_TREE);

	rom_entry_get_list_name (tmprom);
	
	/* Has Samples */
	if (tmprom->nb_samples == 0)
		my_hassamples = "";
	else {
		my_hassamples = (tmprom->has_samples == CORRECT) ? _("Yes") : _("No");
	}
	
	/* Clone colour */
	if ((!strcmp (tmprom->cloneof, "-")) || (!clone_color)) {
		clone_color = g_strdup ("black");
	}
	gdk_color_parse (clone_color, &my_txtcolor);
	

	/* Set the pixbuf for the status icon */
	pixbuf = Status_Icons [tmprom->has_roms];

	if (is_tree_store) {
		gtk_tree_store_set (GTK_TREE_STORE (main_gui.tree_model), &tmprom->position,
				    GAMENAME,                   tmprom->name_in_list,
				    HAS_SAMPLES,                my_hassamples,
				    ROMNAME,                    tmprom->romname,
				    TIMESPLAYED,                tmprom->timesplayed,
				    MANU,                       tmprom->manu,
				    YEAR,                       tmprom->year,
				    CLONE,                      tmprom->cloneof,
				    DRIVER,                     tmprom->driver,
				    MAMEVER,                    tmprom->mame_ver_added,
				    CATEGORY,                   tmprom->category,
				    TEXTCOLOR,                  &my_txtcolor,           /* text color */
				    PIXBUF,                     pixbuf,                 /* pixbuf */
				    -1);
	} else {
		gtk_list_store_set (GTK_LIST_STORE (main_gui.tree_model), &tmprom->position,
				    GAMENAME,                   tmprom->name_in_list,
				    HAS_SAMPLES,                my_hassamples,
				    ROMNAME,                    tmprom->romname,
				    TIMESPLAYED,                tmprom->timesplayed,
				    MANU,                       tmprom->manu,
				    YEAR,                       tmprom->year,
				    CLONE,                      tmprom->cloneof,
				    DRIVER,                     tmprom->driver,
				    MAMEVER,                    tmprom->mame_ver_added,
				    CATEGORY,                   tmprom->category,
				    TEXTCOLOR,                  &my_txtcolor,           /* text color */
				    PIXBUF,                     pixbuf,                 /* pixbuf */
				    -1);
	}
	
	g_free (clone_color);

}

void
select_inp (gboolean play_record)
{
	GtkWidget *inp_selection;
	gchar *inp_dir;
	gchar *current_rom_name;

	RomEntry *rom;

	g_return_if_fail (mame_exec_list_has_current_executable (main_gui.exec_list));
	
	joy_focus_off ();
	
	rom = gui_prefs.current_game;
	
	g_object_get (main_gui.gui_prefs,
		      "dir-inp", &inp_dir,
		      "current-rom", &current_rom_name,
		      NULL);
	
	if (play_record) {
		inp_selection = gtk_file_chooser_dialog_new (_("Choose inp file to play"),
							     GTK_WINDOW (MainWindow),
							     GTK_FILE_CHOOSER_ACTION_OPEN,
							     GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
							     GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
							     NULL);
		gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (inp_selection), inp_dir);
		
		if (gtk_dialog_run (GTK_DIALOG (inp_selection)) == GTK_RESPONSE_ACCEPT) {
			gtk_widget_hide (inp_selection);
			process_inp_function (rom,
					      gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (inp_selection)),
					      0);
		}
		gtk_widget_destroy (inp_selection);
	} else {
		gchar *inp_filename;
		gchar *tmp;
		tmp = g_strdup_printf("%s.inp", current_rom_name);
		inp_filename = g_build_filename (inp_dir, tmp, NULL);
		GMAMEUI_DEBUG ("Recording new inp file to %s", inp_filename);
		
		if (g_file_test (inp_filename, G_FILE_TEST_EXISTS)) {
			GtkWidget *dialog;

			dialog = gtk_message_dialog_new (GTK_WINDOW (MainWindow),
							 GTK_DIALOG_MODAL,
							 GTK_MESSAGE_WARNING,
							 GTK_BUTTONS_YES_NO,
							 _("File exists"));
			gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (dialog),
								  _("A file named '%s' already exists. Do you want to overwrite it?"),
								  inp_filename);
			if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_YES) {
				gtk_widget_hide (dialog);
				process_inp_function (rom, inp_filename, 1);
			}
			gtk_widget_destroy (dialog);
		} else 
			process_inp_function (rom, inp_filename, 1);
		
		g_free (tmp);
		g_free (inp_filename);	
	}
	
	joy_focus_on ();
	
	g_free (inp_dir);
	g_free (current_rom_name);
}

/**** Sidebar functionality ****/
static void gmameui_sidebar_class_init    (GMAMEUISidebarClass *class);
static void gmameui_sidebar_init (GMAMEUISidebar      *dlg);

G_DEFINE_TYPE (GMAMEUISidebar, gmameui_sidebar,
			   GTK_TYPE_FRAME)

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

static void
gmameui_sidebar_set_with_rom (GMAMEUISidebar *sidebar, RomEntry *rom)
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
gmameui_sidebar_set_history (GMAMEUISidebar *sidebar, RomEntry *rom)
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

static void
gmameui_sidebar_set_current_page (GMAMEUISidebar *sidebar, int page)
{
	gtk_notebook_set_current_page (GTK_NOTEBOOK (sidebar->priv->screenshot_notebook),
				       page);
}

/**** End Sidebar functionality ****/

