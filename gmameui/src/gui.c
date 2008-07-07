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

#include <gdk/gdkkeysyms.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gdk-pixbuf/gdk-pixbuf-loader.h>

#include <gtk/gtkcellrendererpixbuf.h>
#include <gtk/gtkcellrenderertext.h>
#include <gtk/gtkcombo.h>
#include <gtk/gtkeditable.h>
#include <gtk/gtkentry.h>
#include <gtk/gtkeventbox.h>
#include <gtk/gtkfilesel.h>
#include <gtk/gtkimagemenuitem.h>
#include <gtk/gtklabel.h>
#include <gtk/gtkliststore.h>
#include <gtk/gtkmain.h>
#include <gtk/gtknotebook.h>
#include <gtk/gtkradiomenuitem.h>
#include <gtk/gtkscrolledwindow.h>
#include <gtk/gtkstock.h>
#include <gtk/gtktextview.h>
#include <gtk/gtktreestore.h>
#include <gtk/gtkvbox.h>
#include <glade/glade.h>

#include "callbacks.h"
#include "interface.h"
#include "gmameui.h"
#include "gui.h"
#include "progression_window.h"
#include "unzip.h"
#include "io.h"

const int ROM_ICON_SIZE = 16;

static guint timeout_icon;

gboolean foreach_find_rom_in_store (GtkTreeModel *model,
				    GtkTreePath  *path,
				    GtkTreeIter  *iter,
				    gpointer      user_data);

/**** Sidebar functionality ****/
struct _GMAMEUISidebarPrivate {

	GtkBox    *screenshot_hist_vbox;
	GtkWidget *screenshot_event_box;

	GtkWidget *main_screenshot;
	
	GtkWidget *screenshot_notebook;
	GtkWidget *screenshot_box1;
	GtkWidget *screenshot_box2;
	GtkWidget *screenshot_box3;
	GtkWidget *screenshot_box4;
	GtkWidget *screenshot_box5;
	GtkWidget *screenshot_box6;
	GtkWidget *screenshot1;
	GtkWidget *screenshot2;
	GtkWidget *screenshot3;
	GtkWidget *screenshot4;
	GtkWidget *screenshot5;
	GtkWidget *screenshot6;

	GtkWidget *history_scrollwin;
	GtkWidget *history_box;
	GtkTextBuffer *history_buffer;

	
};
/**** Sidebar functionality ****/

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

		    g_object_get (main_gui.gui_prefs,
				  "current-mode", &current_mode,
				  NULL);
		    
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
// FIXME TODO	zipfile = g_build_filename (gui_prefs.SnapshotDirectory, "snap.zip", NULL);

	GMAMEUI_DEBUG ("directory is %s, filename is %s, parent is %s", directory_name, filename, filename_parent);

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
	
	if (!pixbuf) {
		if (sctype == SNAPSHOTS) {
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
	}
	
	/* FIXME TODO
	* we havent found the picture in the directory, maybe we could try in a zipfile *
	if (!pixbuf) {
		ZIP *zip;
		struct zipent* zipent;
		gchar *tmp_buffer;
		gchar *parent_tmp_buffer = NULL;
		gsize parent_buf_size = 0;
		gchar *parent_filename;

		zip = openzip (zipfile);

		if (zip) {
			GdkPixbufLoader *loader;

			GMAMEUI_DEBUG ("Succesfully open zip file '%s' !", zipfile);
			filename = g_strdup_printf ("%s.", rom->romname);
			parent_filename = g_strdup_printf ("%s.", rom->cloneof);

			while ( (zipent = readzip (zip)) != 0) {
				* this should allows to find any format of picture in the zip, not only bmp *
				if (!strncmp (filename,zipent->name, strlen (rom->romname) + 1)) {
					GMAMEUI_DEBUG ("found file name %s\twith CRC:%i\tsize%i",
							zipent->name,
							zipent->crc32,
							zipent->uncompressed_size);
					tmp_buffer = read_zipentry (zip, zipent);
					if (tmp_buffer) {	* if the file successfully uncompress, try to load it in a pixbuf loader *
						loader = gdk_pixbuf_loader_new ();
						if (!gdk_pixbuf_loader_write (loader, (guchar *)tmp_buffer, zipent->uncompressed_size, error)) {
							GMAMEUI_DEBUG ("Error while uncompressing %s from %s", zipent->name, zipfile);
						} else {
							gdk_pixbuf_loader_close (loader,error);
							pixbuf = gdk_pixbuf_loader_get_pixbuf (loader);
						}
						g_free (tmp_buffer);
					}
					* prevent to read all zip file if we have found the picture's game (uncompressed successfuly or not) *
					break;

				} else if (!strncmp (parent_filename, zipent->name, strlen (rom->cloneof) + 1)) {
					parent_tmp_buffer = read_zipentry (zip, zipent);
					parent_buf_size = zipent->uncompressed_size;
				}
			}
			g_free (filename);
			g_free (parent_filename);

			* no picture found try parent game if any*
			if (!pixbuf && parent_tmp_buffer) {
				loader = gdk_pixbuf_loader_new ();
				if (!gdk_pixbuf_loader_write (loader, (guchar *)parent_tmp_buffer, parent_buf_size, error)) {
					GMAMEUI_DEBUG ("Error while uncompressing %s from %s", zipent->name, zipfile);
				} else {
					gdk_pixbuf_loader_close (loader, error);
					pixbuf = gdk_pixbuf_loader_get_pixbuf (loader);
				}
			}

			g_free (parent_tmp_buffer);
		}

		if (zip)
			closezip (zip);
		else
			GMAMEUI_DEBUG ("Error, cannot open zip file '%s' !\n", zipfile);

		g_free (zipfile);
		
	}*/

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
gmameui_menu_set_view_mode_check (gint view_mode, gboolean state)
{
	GtkWidget *widget;
	
	switch (view_mode) {
		case (LIST):
			widget = gtk_ui_manager_get_widget (main_gui.manager,
							    "/MenuBar/ViewMenu/ViewListViewMenu");
			break;
		case (LIST_TREE):
			widget = gtk_ui_manager_get_widget (main_gui.manager,
							    "/MenuBar/ViewMenu/ViewTreeViewMenu");
			break;
		case (DETAILS):
			widget = gtk_ui_manager_get_widget (main_gui.manager,
							    "/MenuBar/ViewMenu/ViewDetailsListViewMenu");
			break;
		case (DETAILS_TREE):
			widget = gtk_ui_manager_get_widget (main_gui.manager,
							    "/MenuBar/ViewMenu/ViewDetailsTreeViewMenu");
			break;
	}
	gtk_check_menu_item_set_active (widget, state);
}

static gboolean on_main_window_moved_cb (GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
	gint x, y, w, h;
	gdk_drawable_get_size (GDK_DRAWABLE (widget->window), &x, &y);
	gdk_window_get_position (GDK_WINDOW (widget->window), &w, &h);

	g_object_set (main_gui.gui_prefs,
		      "ui-width", widget->allocation.width,
		      "ui-height", widget->allocation.height,
		      NULL);
	return FALSE;
}

void
init_gui (void)
{
	GtkTooltips *tooltips;
	gchar *filename;
	
	gint ui_width;
	gint ui_height;
	gint xpos_filters;
	gint xpos_gamelist;
	gboolean show_filters;
	gboolean show_screenshot;
	gboolean show_statusbar;
	gboolean show_toolbar;
	screenshot_type show_flyer;
	ListMode current_mode;
	
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
g_message (_("Time to create main window and filters: %.02f seconds"), g_timer_elapsed (mytimer, NULL));
#endif

	gtk_widget_hide (GTK_WIDGET (main_gui.combo_progress_bar));

	g_object_get (main_gui.gui_prefs,
		      "ui-width", &ui_width,
		      "ui-height", &ui_height,
		      "show-filterlist", &show_filters,
		      "show-screenshot", &show_screenshot,
		      "show-flyer", &show_flyer,
		      "current-mode", &current_mode,
		      "show-statusbar", &show_statusbar,
		      "show-toolbar", &show_toolbar,
		      "xpos-filters", &xpos_filters,
		      "xpos-gamelist", &xpos_gamelist,
		      NULL);

	gtk_window_set_default_size (GTK_WINDOW (MainWindow),
				     ui_width,
				     ui_height);
	
	gtk_paned_set_position (main_gui.hpanedLeft, xpos_filters);
	gtk_paned_set_position (main_gui.hpanedRight, xpos_gamelist);

	/* FIXME TODO
	gtk_window_move (GTK_WINDOW (MainWindow),
			 gui_prefs.GUIPosX,
			 gui_prefs.GUIPosY);*/

	/* Show and hence realize mainwindow so that MainWindow->window is available */
	gtk_widget_show (MainWindow);

	/* Set state of radio/check menu and toolbar widgets */
	gmameui_menu_set_view_mode_check (current_mode, TRUE);
	gtk_toggle_action_set_active (gtk_ui_manager_get_action (main_gui.manager,
								 "/MenuBar/ViewMenu/ViewSidebarPanelMenu"),
				      show_screenshot);

	gtk_toggle_action_set_active (gtk_ui_manager_get_action (main_gui.manager,
								 "/MenuBar/ViewMenu/ViewFolderListMenu"),
				      show_filters);
	
	gtk_toggle_action_set_active (gtk_ui_manager_get_action (main_gui.manager,
								 "/MenuBar/ViewMenu/ViewStatusBarMenu"),
				      show_statusbar);
	gtk_toggle_action_set_active (gtk_ui_manager_get_action (main_gui.manager,
								 "/MenuBar/ViewMenu/ViewToolbar"),
				      show_toolbar);

	if (! ((current_mode == LIST_TREE) || (current_mode == DETAILS_TREE))) {
		gtk_action_group_set_sensitive (main_gui.gmameui_view_action_group, FALSE);
	}

	/* Create the UI of the Game List */
	create_gamelist (current_mode);
#ifdef ENABLE_DEBUG
g_message (_("Time to create gamelist: %.02f seconds"), g_timer_elapsed (mytimer, NULL));
#endif
	/* Feed the Game List */
	create_gamelist_content ();
#ifdef ENABLE_DEBUG
g_message (_("Time to create gamelist content: %.02f seconds"), g_timer_elapsed (mytimer, NULL));
#endif
	/* Need to set the size here otherwise it move when we create the gamelist */
	if (show_screenshot)
		gtk_paned_set_position (main_gui.hpanedRight, xpos_gamelist);

	/* Grab focus on the game list */
	gtk_widget_grab_focus (main_gui.displayed_list);

	g_signal_connect (G_OBJECT (gtk_scrolled_window_get_vadjustment (GTK_SCROLLED_WINDOW (main_gui.scrolled_window_games))), "changed",
	                  G_CALLBACK (adjustment_scrolled),
	                  NULL);
	g_signal_connect (G_OBJECT (gtk_scrolled_window_get_vadjustment (GTK_SCROLLED_WINDOW (main_gui.scrolled_window_games))), "value-changed",
	                  G_CALLBACK (adjustment_scrolled),
	                  NULL);

	/* Need to set the notebook page here otherwise it segfault */
	gmameui_sidebar_set_current_page (main_gui.screenshot_hist_frame, show_flyer);
	
	/* Once we have created and populated the window, link to the configure-event */
	g_signal_connect (G_OBJECT (MainWindow), "configure_event",
			  G_CALLBACK (on_main_window_moved_cb),
			  NULL);
	
#ifdef ENABLE_DEBUG
	g_timer_stop (mytimer);
	g_message (_("Time to complete start of UI: %.02f seconds"), g_timer_elapsed (mytimer, NULL));
	g_timer_destroy (mytimer);
#endif
}

static void
set_current_executable (XmameExecutable *new_exec)
{
	gboolean valid;
	
	if (new_exec) {
		GMAMEUI_DEBUG ("Executable changed to %s", new_exec->path);
	}

	current_exec = new_exec;
	
	valid = xmame_executable_is_valid (new_exec);

	/* check if the executable is still valid */
	if (!valid) {
		if (new_exec) {
			gmameui_message (ERROR, NULL, _("%s is not a valid executable"), new_exec->path);
			/* FIXME TODO Remove the executable from the list */
		}
	} else
		g_object_set (main_gui.gui_prefs, "current-executable", new_exec->path, NULL);

	/* Set sensitive the UI elements that require an executable to be set */
	gtk_action_group_set_sensitive (main_gui.gmameui_rom_exec_action_group, valid);
	gtk_action_group_set_sensitive (main_gui.gmameui_exec_action_group, valid);
}

/* executable selected from the menu */
static void
on_executable_selected (GtkRadioAction *action,
			gpointer          user_data)
{
	gint index = GPOINTER_TO_INT (user_data);

	XmameExecutable *exec;

	exec = xmame_table_get_by_index (index);

	GMAMEUI_DEBUG ("Picking executable %d - %s", index, exec->name);
	set_current_executable (exec);
}

/* Dynamically create the executables menu. */
void
add_exec_menu (void)
{
	gchar *current_exec;
	int num_execs;
	int i;
	XmameExecutable *exec;

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
	
	num_execs = xmame_table_size ();

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
			exec = xmame_table_get_by_index (i);
			gchar *exec_name = g_strdup_printf ("%s (%s) %s", exec->name, exec->target, exec->version);
		
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
				if (g_ascii_strcasecmp (exec->path, current_exec) == 0) {
					GMAMEUI_DEBUG ("Setting %s as current executable", exec->path);
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
	rom_and_exec = (gui_prefs.current_game) && (xmame_table_size () > 0);

	gtk_action_group_set_sensitive (main_gui.gmameui_rom_exec_action_group,
					rom_and_exec);
	gtk_action_group_set_sensitive (main_gui.gmameui_exec_action_group,
					xmame_table_size () > 0);
	gtk_action_group_set_sensitive (main_gui.gmameui_rom_action_group,
					gui_prefs.current_game);

	/* Disable favourites if no current game */
	gtk_action_group_set_sensitive (main_gui.gmameui_favourite_action_group,
					(gui_prefs.current_game != NULL));
	if (gui_prefs.current_game != NULL)
		gmameui_ui_set_favourites_sensitive (gui_prefs.current_game->favourite);
}

void
gamelist_popupmenu_show (GdkEventButton *event)
{
	GtkWidget *popup_menu;

	popup_menu = gtk_ui_manager_get_widget (main_gui.manager, "/GameListPopup");
	g_return_if_fail (popup_menu != NULL);

	if (current_exec)
		xmame_get_options (current_exec);

	gtk_menu_popup (GTK_MENU (popup_menu), NULL, NULL,
			NULL, NULL,
			event->button, event->time);
}

void
hide_filters (void)
{
	g_object_set (main_gui.gui_prefs,
		      "xpos-filters", main_gui.scrolled_window_filters->allocation.width,
		      NULL);
	gtk_paned_set_position (main_gui.hpanedLeft, 0);

	gtk_widget_hide (GTK_WIDGET (main_gui.scrolled_window_filters));
}

void
show_filters (void)
{
	gint xpos_filters;
	g_object_get (main_gui.gui_prefs,
		      "xpos-filters", &xpos_filters,
		      NULL);
	gtk_paned_set_position (main_gui.hpanedLeft, xpos_filters);

	gtk_widget_show (GTK_WIDGET (main_gui.scrolled_window_filters));
}

void
hide_snaps (void)
{
	g_object_set (main_gui.gui_prefs,
		      "xpos-gamelist", main_gui.scrolled_window_games->allocation.width,
		      NULL);
	gtk_paned_set_position (main_gui.hpanedRight, -1);

	gtk_widget_hide (GTK_WIDGET (main_gui.screenshot_hist_frame));
}

void
show_snaps (void)
{
	gint xpos_gamelist;
	g_object_get (main_gui.gui_prefs,
		      "xpos-gamelist", &xpos_gamelist,
		      NULL);
	gtk_paned_set_position (main_gui.hpanedLeft, xpos_gamelist);

	gtk_widget_show (GTK_WIDGET (main_gui.screenshot_hist_frame));
}

/* get an icon for a rom, if not found, try the original game if the game is a clone */
GdkPixbuf *
get_icon_for_rom (RomEntry *rom,
		  guint     size,
		  ZIP      *zip)
{
	GdkPixbuf *pixbuf, *scaled_pixbuf = NULL;
	gchar filename [MAX_ROMNAME + 1], *filename2;
	gchar parent_filename [MAX_ROMNAME + 1];
	gchar *icon_dir;
	GError **error = NULL;

	if (!rom)
		return NULL;

	g_object_get (main_gui.gui_prefs,
		      "dir-icons", &icon_dir,
		      NULL);
	
	filename2 = g_strdup_printf ("%s" G_DIR_SEPARATOR_S "%s.ico", icon_dir, rom->romname);
	pixbuf = gdk_pixbuf_new_from_file (filename2, error);
	g_free (filename2);

		/* no picture found try parent game if any*/
	if ((pixbuf == NULL) && strcmp (rom->cloneof, "-")) {
		filename2 = g_strdup_printf ("%s" G_DIR_SEPARATOR_S "%s.ico", icon_dir, rom->cloneof);
		pixbuf = gdk_pixbuf_new_from_file (filename, error);
		g_free (filename2);
	}

	/* we havent found the picture in the directory, maybe we could try in a zipfile */
	if (pixbuf == NULL) {
		struct zipent * zipent;
		char *tmp_buffer;
		char *parent_tmp_buffer = NULL;
		gsize parent_buf_size = 0;
		if (zip != 0) {
			GdkPixbufLoader *loader;

			rewindzip (zip);
			g_snprintf (filename, MAX_ROMNAME + 1 , "%s.", rom->romname);
			g_snprintf (parent_filename, MAX_ROMNAME + 1, "%s.", rom->cloneof);
					
			while ( (zipent = readzip (zip)) != 0) {
				/* this should allows to find any format of picture in the zip, not only bmp */
				if (!strncmp (filename, zipent->name, strlen (rom->romname) + 1)) {
					tmp_buffer = read_zipentry (zip, zipent);
					if (tmp_buffer) {
						/* if the file successfully uncompress, try to load it in a pixbuf loader */
						loader = gdk_pixbuf_loader_new ();
						if (!gdk_pixbuf_loader_write (loader, (guchar *)tmp_buffer, zipent->uncompressed_size, error)) {
							GMAMEUI_DEBUG ("Error while uncompressing %s ", zipent->name);
						} else {
							gdk_pixbuf_loader_close (loader, error);
							pixbuf = gdk_pixbuf_loader_get_pixbuf (loader);
						}
						g_free (tmp_buffer);
						tmp_buffer = NULL;
					}
					/* prevent to read all zip file if we have found the picture's game (uncompressed successfuly or not) */
					break;
				} else if (strcmp (rom->cloneof, "-") && !strncmp (parent_filename, zipent->name, strlen (rom->cloneof) + 1)) {
					parent_tmp_buffer = read_zipentry (zip, zipent);
					parent_buf_size = zipent->uncompressed_size;
				}
			}

			/* no picture found try parent game if any */
			if (pixbuf == NULL) {
				if (parent_tmp_buffer) {
					loader = gdk_pixbuf_loader_new ();
					if (!gdk_pixbuf_loader_write (loader, (guchar *)parent_tmp_buffer, parent_buf_size, error)) {
						GMAMEUI_DEBUG ("Error while uncompressing %s ",zipent->name);
					} else {
						gdk_pixbuf_loader_close (loader,error);
						pixbuf = gdk_pixbuf_loader_get_pixbuf (loader);
					}
						parent_tmp_buffer = NULL;
				}
			}

			if (parent_tmp_buffer)
				g_free (parent_tmp_buffer);
		}
	}

	if (pixbuf) {
		scaled_pixbuf = gdk_pixbuf_scale_simple (pixbuf,
							size, size, GDK_INTERP_BILINEAR);
		g_object_unref (pixbuf);
	}
	
	g_free (icon_dir);
	
	return scaled_pixbuf;
}

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

static GdkPixbuf *
gmameui_get_pixbuf_from_gmameui_install (const char *filename)
{
	GdkPixbuf *pixbuf;
	char *path, *fn;

	fn = g_strconcat (filename, ".png", NULL);
	path = g_build_filename (DATADIR, "gmameui", fn, NULL);
	g_free (fn);
	pixbuf = gdk_pixbuf_new_from_file (path, NULL);
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
static void
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

void set_list_sortable_column ()
{
	ListMode current_mode;
	gint sort_col;
	gint sort_col_dir;
	
	g_object_get (main_gui.gui_prefs,
		      "current-mode", &current_mode,
		      "sort-col", &sort_col,
		      "sort-col-direction", &sort_col_dir,
		      NULL);
	
	if ((current_mode == DETAILS) || (current_mode == DETAILS_TREE)) {
		GMAMEUI_DEBUG("Sorting - using sort order %d", sort_col);
		gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE (main_gui.tree_model),
						      sort_col, sort_col_dir);

	} else {
		g_signal_handlers_block_by_func (G_OBJECT (main_gui.tree_model), (gpointer)on_displayed_list_sort_column_changed, NULL);
		gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE (main_gui.tree_model),
						      GAMENAME, GTK_SORT_ASCENDING);
		g_signal_handlers_unblock_by_func (G_OBJECT (main_gui.tree_model), (gpointer)on_displayed_list_sort_column_changed, NULL);
	}	/* Select the correct row */
}

/* This function walks the tree store containing the ROM data until
   it finds the row that has the ROM from the preferences. It then
   scrolls to that row, and opens the parent row if the found
   row is a child */
gboolean foreach_find_rom_in_store (GtkTreeModel *model,
				    GtkTreePath  *path,
				    GtkTreeIter  *iter,
				    gpointer      user_data)
{
	RomEntry *rom;
	gchar *current_rom_name = (gchar *) user_data;
/*	gchar *tree_path_str;*/
	
	/* Don't even bother trying to walk the store if the current game
	   is not set. We are just wasting our time */
	g_return_val_if_fail ((current_rom_name != NULL), TRUE);
	
	gtk_tree_model_get (model, iter,
			    ROMENTRY, &rom,
			    -1);
/*	
	tree_path_str = gtk_tree_path_to_string (path);
	GMAMEUI_DEBUG ("Row %s: name is %s", tree_path_str, rom->romname);
	g_free (tree_path_str);
*/	
	if (g_ascii_strcasecmp (rom->romname, current_rom_name) == 0) {
		GMAMEUI_DEBUG ("Found row in tree view - %s", rom->romname);
				
		/* Scroll to selection */
		gtk_tree_view_scroll_to_cell (GTK_TREE_VIEW (main_gui.displayed_list),
					      path, NULL, TRUE, 0.5, 0);

		/* Need to expand the parent row (if a child) */
		gtk_tree_view_expand_to_path (GTK_TREE_VIEW (main_gui.displayed_list),
					      path);

		/* And highlight the row */
		gtk_tree_view_set_cursor (GTK_TREE_VIEW (main_gui.displayed_list),
					  path,
					  NULL, FALSE);
		
		return TRUE;    /* Found the row we are after, no need to keep walking */
	}
	
	return FALSE;   /* Do not stop walking the store, call us with next row */
}

void
create_gamelist_content (void)
{
	GList *listpointer;
	RomEntry *tmprom;
	gchar *my_romname_root = NULL;
	gchar *my_hassamples;
	GdkColor my_txtcolor;
	GtkTreeIter iter;
	GtkTreeIter iter_root;
	GtkTreeModel *store;
	gboolean tree_store;   /* If the model is a tree or a list */
	gboolean is_root;
	gint j = 0;
	gchar *message;
	RomEntry *selected_game;
	ListMode current_mode;
	gchar *current_rom_name;
	gchar *clone_color;

	GMAMEUI_DEBUG ("POPULATE GAME LIST");
GTimer *timer = g_timer_new ();
g_timer_start (timer);
	
	g_object_get (main_gui.gui_prefs,
		      "current-mode", &current_mode,
		      "current-rom", &current_rom_name,
		      "clone-color", &clone_color,
		      NULL);
	
	selected_game = gui_prefs.current_game;

	/* Status Bar Message */
	gtk_statusbar_pop (main_gui.statusbar3, 1);
	gtk_statusbar_push (main_gui.statusbar3, 1, _("Wait..."));
	if ((main_gui.displayed_list) && (main_gui.tree_model)) {
		store = NULL;
		gtk_tree_view_set_model (GTK_TREE_VIEW (main_gui.displayed_list), GTK_TREE_MODEL (store));
		/* Update UI */
		/*while (gtk_events_pending ())
			gtk_main_iteration ();*/
	}

	/* Whether the Tree Model will a tree or a list */
	tree_store = ((current_mode == LIST_TREE) || (current_mode == DETAILS_TREE));

	/* Get the status icon */
	get_status_icons ();

	/* Create a model. */
	if (tree_store)
		store = (GtkTreeModel *) gtk_tree_store_new (NUMBER_COLUMN + 3,
							     G_TYPE_STRING,     /* Name */
							     G_TYPE_STRING,     /* Has samples */
							     G_TYPE_STRING,     /* ROM name */
							     G_TYPE_INT,	/* Times played */
							     G_TYPE_STRING,     /* Manu */
							     G_TYPE_STRING,     /* Year */
							     G_TYPE_STRING,     /* Clone of */
							     G_TYPE_STRING,     /* Driver */
							     G_TYPE_STRING,     /* Ver added */
							     G_TYPE_STRING,     /* Category */
							     G_TYPE_POINTER,     /* Rom Entry */
							     GDK_TYPE_COLOR,     /* Text Color */
							     GDK_TYPE_PIXBUF);   /* Pixbuf */
	else
		store = (GtkTreeModel *) gtk_list_store_new (NUMBER_COLUMN + 3,
							     G_TYPE_STRING,     /* Name */
							     G_TYPE_STRING,     /* Has samples */
							     G_TYPE_STRING,     /* ROM name */
							     G_TYPE_INT,	/* Times played */
							     G_TYPE_STRING,     /* Manu */
							     G_TYPE_STRING,     /* Year */
							     G_TYPE_STRING,     /* Clone of */
							     G_TYPE_STRING,     /* Driver */
							     G_TYPE_STRING,     /* Ver added */
							     G_TYPE_STRING,     /* Category */
							     G_TYPE_POINTER,     /* Rom Entry */
							     GDK_TYPE_COLOR,     /* Text Color */
							     GDK_TYPE_PIXBUF);   /* Pixbuf */

	/* fill the model with data */
	for (listpointer = g_list_first (game_list.roms);
	     (listpointer);
	     listpointer= g_list_next (listpointer)) {
		tmprom = (RomEntry *) listpointer->data;
		if (game_filtered (tmprom)) {
			GdkPixbuf *pixbuf = NULL;

			rom_entry_get_list_name (tmprom);

			/* Has Samples */
			if (tmprom->nb_samples == 0)
				my_hassamples = NULL;
			else
				my_hassamples = (tmprom->has_samples == CORRECT) ? _("Yes") : _("No");
		
			/* Clone Color + Pixbuf width */
			if (strcmp (tmprom->cloneof, "-")) {
				/* Clone */
				gdk_color_parse (clone_color, &my_txtcolor);
			} else {
				/* Original */
				gdk_color_parse ("black", &my_txtcolor);
			}

			/* Set the pixbuf for the status icon */
			pixbuf = Status_Icons [tmprom->has_roms];

			/* Determine if the row is a root */
			if ( (j == 0) || !(strcmp (tmprom->cloneof, "-")) || !my_romname_root || (strcmp (tmprom->cloneof, my_romname_root)) ) {
				is_root = TRUE;
			} else {
				is_root = FALSE;
			}

			/* Memorize the original names */
			if (! (strcmp (tmprom->cloneof, "-"))) {
				if (my_romname_root)
					g_free (my_romname_root);

				my_romname_root= g_strdup (tmprom->romname);
			}

			if (tree_store) {
				if (is_root)
					gtk_tree_store_append (GTK_TREE_STORE (store), &iter, NULL);  /* Acquire an iterator */
				else
					gtk_tree_store_append (GTK_TREE_STORE (store), &iter, &iter_root);  /* Acquire an iterator */

				gtk_tree_store_set (GTK_TREE_STORE (store), &iter,
						    GAMENAME,     tmprom->name_in_list,
						    HAS_SAMPLES,  my_hassamples,
						    ROMNAME,      tmprom->romname,
						    TIMESPLAYED,  tmprom->timesplayed,
						    MANU,         tmprom->manu,
						    YEAR,         tmprom->year,
						    CLONE,        tmprom->cloneof,
						    DRIVER,       tmprom->driver,
						    MAMEVER,      tmprom->mame_ver_added,
						    CATEGORY,     tmprom->category,
						    ROMENTRY,     tmprom,                 /* rom entry */
						    TEXTCOLOR,    &my_txtcolor,           /* text color */
						    PIXBUF,       pixbuf,                 /* pixbuf */
						    -1);
				if (is_root)
					iter_root = iter;
			} else {
				gtk_list_store_append (GTK_LIST_STORE (store), &iter);  /* Acquire an iterator */
				gtk_list_store_set (GTK_LIST_STORE (store), &iter,
						    GAMENAME,     tmprom->name_in_list,
						    HAS_SAMPLES,  my_hassamples,
						    ROMNAME,      tmprom->romname,
						    TIMESPLAYED,  tmprom->timesplayed,
						    MANU,         tmprom->manu,
						    YEAR,         tmprom->year,
						    CLONE,        tmprom->cloneof,
						    DRIVER,       tmprom->driver,
						    MAMEVER,      tmprom->mame_ver_added,
						    CATEGORY,     tmprom->category,
						    ROMENTRY,     tmprom,                 /* rom entry */
						    TEXTCOLOR,    &my_txtcolor,            /* text color */
						    PIXBUF,       pixbuf,                 /* pixbuf */
						    -1);
			}
			tmprom->position = iter;
			tmprom->is_in_list = TRUE;
			j++;
		} else {
			tmprom->is_in_list = FALSE;
		}
	}
	visible_games = j;

	/* Callbacks - Sorting order has changed */
	main_gui.tree_model = GTK_TREE_MODEL (store);
	if (main_gui.tree_model != NULL) {
		g_signal_connect (G_OBJECT (main_gui.tree_model), "sort-column-changed",
				  G_CALLBACK (on_displayed_list_sort_column_changed),
				  NULL);
	}

	/* Update the corresponding tree view */
	if (main_gui.displayed_list) {
		/* Link the view with the model */
		gtk_tree_view_set_model (GTK_TREE_VIEW (main_gui.displayed_list), GTK_TREE_MODEL (main_gui.tree_model));

		/* Sort the list */
		set_list_sortable_column ();

		/* Find the selected game in the gamelist, and scroll to it, opening any expanders */
		if (visible_games > 0) {
			/* Scroll to the game specified from the preferences */
			gtk_tree_model_foreach (GTK_TREE_MODEL (main_gui.tree_model), foreach_find_rom_in_store, current_rom_name);
		}
		/* Header clickable. */
		gtk_tree_view_set_headers_clickable (GTK_TREE_VIEW (main_gui.displayed_list),
						     (current_mode == DETAILS) ||
						     (current_mode == DETAILS_TREE));
		
	}

	/* Status Bar Message */
	message = g_strdup_printf ("%d %s", visible_games, visible_games > 1 ? _("games") : _("game"));
	gtk_statusbar_pop (main_gui.statusbar3, 1);
	gtk_statusbar_push (main_gui.statusbar3, 1, message);

	/* Free Memory */
	if (message)
		g_free (message);
	if (my_romname_root)
		g_free (my_romname_root);
	g_free (clone_color);

	if (visible_games == 0)
		select_game (NULL);
	else
		select_game (selected_game);
	GMAMEUI_DEBUG ("Time taken to create_gamelist_content is %.2f", g_timer_elapsed (timer, NULL));
	g_timer_destroy (timer);
}

void
create_gamelist (ListMode list_mode)
{
	gint i;
	GtkTreeViewColumn *column;
	GtkCellRenderer *renderer;
	GtkTreeSelection *select;

	static gboolean first_run = TRUE;

	GMAMEUI_DEBUG ("Creating gamelist structure");

	/* We Create the TreeView only if it is NULL (this will occur only once) */
	if (main_gui.displayed_list == NULL) {
		main_gui.displayed_list = gtk_tree_view_new ();
		for (i = 0; i < NUMBER_COLUMN; i++) {
			if (i == GAMENAME) {
				/* Game name column contains both the status icon and the name */
				column = gtk_tree_view_column_new ();
				gtk_tree_view_column_set_title  (GTK_TREE_VIEW_COLUMN (column), column_title (i));
				gtk_tree_view_column_set_sort_column_id (column, i);
				/* TEXT */
				renderer = gtk_cell_renderer_text_new ();
				gtk_tree_view_column_pack_end (GTK_TREE_VIEW_COLUMN (column),
							       renderer,
							       FALSE);
				gtk_tree_view_column_set_attributes (GTK_TREE_VIEW_COLUMN (column), renderer,
								     "text", i,
								     "foreground-gdk", TEXTCOLOR,
								     NULL);
				g_object_set (renderer, "ypad", 0, "yalign", 0.5, NULL);
				/* Pixbuf */
				renderer = gtk_cell_renderer_pixbuf_new ();
				gtk_tree_view_column_pack_end (GTK_TREE_VIEW_COLUMN (column),
							       renderer,
							       FALSE);
				gtk_tree_view_column_add_attribute (GTK_TREE_VIEW_COLUMN (column), renderer,
								    "pixbuf", PIXBUF);
				g_object_set (renderer, "xalign", 1.0, "ypad", 0, NULL);
			} else {
				renderer = gtk_cell_renderer_text_new ();
				column = gtk_tree_view_column_new_with_attributes (column_title (i), renderer,
										   "text", i,
										   "foreground-gdk", TEXTCOLOR,
										   NULL);
				gtk_tree_view_column_set_sort_column_id (column, i);
			}
			gtk_tree_view_append_column (GTK_TREE_VIEW (main_gui.displayed_list), column);
			gtk_tree_view_column_set_min_width (GTK_TREE_VIEW_COLUMN (column), 1);
			g_signal_connect (column->button, "event",
					  G_CALLBACK (on_column_click),
					  column);
		}

		gtk_container_add (GTK_CONTAINER (main_gui.scrolled_window_games), main_gui.displayed_list);
		gtk_widget_show_all (main_gui.scrolled_window_games);

		/* Callback - Row has been selected */
		select = gtk_tree_view_get_selection (GTK_TREE_VIEW (main_gui.displayed_list));
		gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
		g_signal_connect (G_OBJECT (select), "changed",
				  G_CALLBACK (on_row_selected),
				  NULL);

		/* Callback - Click on the list */
		g_signal_connect (G_OBJECT (main_gui.displayed_list), "button-press-event",
				G_CALLBACK (on_list_clicked),
				NULL);
		/* Callback - Keypress on the list */
		g_signal_connect (G_OBJECT (main_gui.displayed_list), "key-press-event",
				G_CALLBACK (on_list_keypress),
				NULL);
		/* Callback - Column size modified */
		g_signal_connect (G_OBJECT (main_gui.displayed_list), "size-request",
				G_CALLBACK (on_displayed_list_resize_column),
				  NULL);
		/* Callback - Row has been collapsed */
		g_signal_connect (G_OBJECT (main_gui.displayed_list), "row-collapsed",
				G_CALLBACK (on_displayed_list_row_collapsed),
				NULL);

	}

	/* Header clickable Tree Model must exist. */
	if (main_gui.tree_model) {
		/* We sort the list */
		set_list_sortable_column();
		
		gtk_tree_view_set_headers_clickable (GTK_TREE_VIEW (main_gui.displayed_list),
						     (list_mode == DETAILS) || (list_mode == DETAILS_TREE));
	}

	GValueArray *va_shown = NULL;
	GValueArray *va_width = NULL;

	g_object_get (main_gui.gui_prefs,
		      "cols-shown", &va_shown,
		      "cols-width", &va_width,
		      NULL);
	
	/* Update the columns */
	/* FIXME When switching from LIST mode to DETAILS, it puts a mess in the size of the
	GAMENAME column even if I block the callback?????? */
	g_signal_handlers_block_by_func (G_OBJECT (main_gui.displayed_list), (gpointer)on_displayed_list_resize_column, NULL);
	for (i = 0; i < NUMBER_COLUMN; i++) {
		/* Iterate over the columns */
		column = gtk_tree_view_get_column (GTK_TREE_VIEW (main_gui.displayed_list), i);

		/* Columns visible, Column size,... */
		if ( (list_mode == DETAILS) || (list_mode == DETAILS_TREE)) {	/* COLUMNS */

			//if (gui_prefs.ColumnShown[i]==FALSE) {
			if (g_value_get_int (g_value_array_get_nth (va_shown, i)) == FALSE) {
				gtk_tree_view_column_set_visible (column, FALSE);
			} else {
				gint col_width;
				
				col_width = g_value_get_int (g_value_array_get_nth (va_width, i));
				gtk_tree_view_column_set_visible (column, TRUE);
				if (col_width == 0) {
					gtk_tree_view_column_set_sizing (column, GTK_TREE_VIEW_COLUMN_AUTOSIZE);
				} else {
					gtk_tree_view_column_set_sizing (column, GTK_TREE_VIEW_COLUMN_FIXED);
					gtk_tree_view_column_set_fixed_width (column, col_width);
				}
				gtk_tree_view_column_set_resizable (column, TRUE);
				
				
			}
		} else {	/* NO COLUMNS */
			if (i == GAMENAME) {
				gtk_tree_view_column_set_visible (column, TRUE);
				gtk_tree_view_column_set_sizing (column, GTK_TREE_VIEW_COLUMN_AUTOSIZE);
				gtk_tree_view_column_set_resizable (column, FALSE);
			} else {
				gtk_tree_view_column_set_visible (column, FALSE);
			}
		}
		/* Reordable is disable for the time beeing because it make conflics with
		  'column popup menu' and 'sort on click header' */
		gtk_tree_view_column_set_reorderable (GTK_TREE_VIEW_COLUMN (column), FALSE);
	}
	g_signal_handlers_unblock_by_func (G_OBJECT (main_gui.displayed_list), (gpointer)on_displayed_list_resize_column, NULL);

	gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (main_gui.displayed_list), TRUE);
	
	first_run = FALSE;
	dirty_icon_cache = FALSE;

	GMAMEUI_DEBUG ("Creating gamelist structure... done");
}

void
set_status_bar (gchar *game_name, gchar *game_status)
{
	gtk_statusbar_pop (main_gui.statusbar1, 1);
	gtk_statusbar_push (main_gui.statusbar1, 1, game_name);

	gtk_statusbar_pop (main_gui.statusbar2, 1);
	gtk_statusbar_push (main_gui.statusbar2, 1, game_status);
}

void
select_game (RomEntry *rom)
{
	gui_prefs.current_game = rom;

	if (rom) {
		/* update statusbar */
		set_status_bar (rom_entry_get_list_name (rom),
				rom_status_string_value [rom->has_roms]);

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
show_progress_bar (void)
{
/* FIXME TODO Not currently implemented 
	if (gui_prefs.ShowStatusBar) {
		gchar *displayed_message;
		displayed_message = g_strdup_printf (_("Game search %i%% complete"), 0);
		
		gtk_widget_hide (GTK_WIDGET (main_gui.tri_status_bar));
		gtk_statusbar_push (main_gui.status_progress_bar, 1, displayed_message);
		gtk_widget_show (GTK_WIDGET (main_gui.combo_progress_bar));
		g_free (displayed_message);
	}	*/
}

void
hide_progress_bar (void)
{
/* FIXME TODO Not currently implemented 
	if (gui_prefs.ShowStatusBar) {
		gtk_widget_hide (GTK_WIDGET (main_gui.combo_progress_bar));
		gtk_statusbar_pop (main_gui.status_progress_bar, 1);
		gtk_widget_show (GTK_WIDGET (main_gui.tri_status_bar));
	}*/
}

void
update_progress_bar (gfloat current_value)
{
/* FIXME TODO Not currently implemented 
	static gint current_displayed_value;
	gchar *displayed_message;

	if (gui_prefs.ShowStatusBar == FALSE)
		return;

	if (current_displayed_value!= (gint) (current_value * 100)) {
		current_displayed_value= (gint) (current_value * 100);
		displayed_message = g_strdup_printf (_("Game search %i%% complete"), current_displayed_value);
		gtk_statusbar_pop (main_gui.status_progress_bar, 1);
		gtk_statusbar_push (main_gui.status_progress_bar, 1, displayed_message);
		g_free (displayed_message);
	}

	gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (main_gui.progress_progress_bar), current_value);
*/
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
	
	g_return_if_fail (current_exec != NULL);
	
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
			gint result;

			dialog = gtk_message_dialog_new (GTK_WINDOW (MainWindow),
							 GTK_DIALOG_MODAL,
							 GTK_MESSAGE_WARNING,
							 GTK_BUTTONS_YES_NO,
							 _("A file named '%s' already exists.\nDo you want to overwrite it?"),
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

/* Creates a hbox containing a treeview (the sidebar) and a notebook */
static void
gmameui_sidebar_init (GMAMEUISidebar *sidebar)
{
	GladeXML *xml;
	
GMAMEUI_DEBUG ("Creating sidebar");

	sidebar->priv = g_new0 (GMAMEUISidebarPrivate, 1);
	
	sidebar->priv->main_screenshot = gmameui_get_image_from_stock ("gmameui-screen");

	sidebar->priv->screenshot_event_box = gtk_event_box_new ();
	gtk_box_pack_start (sidebar->priv->screenshot_hist_vbox,
			    sidebar->priv->screenshot_event_box,
			    TRUE, TRUE, 6);
	gtk_container_add (GTK_CONTAINER (sidebar->priv->screenshot_event_box),
			   GTK_WIDGET (sidebar->priv->main_screenshot));
	gtk_widget_show (sidebar->priv->screenshot_event_box);
	gtk_widget_show (sidebar->priv->main_screenshot);
	
	xml = glade_xml_new (GLADEDIR "sidebar.glade", "screenshot_notebook", GETTEXT_PACKAGE);
	
	sidebar->priv->screenshot_hist_vbox = gtk_vbox_new (FALSE, 6);
	gtk_container_set_border_width (GTK_CONTAINER (sidebar->priv->screenshot_hist_vbox), 6);
	gtk_container_add (GTK_CONTAINER (sidebar),
			   GTK_WIDGET (sidebar->priv->screenshot_hist_vbox));
	gtk_widget_show (GTK_WIDGET (sidebar->priv->screenshot_hist_vbox));
	
	sidebar->priv->screenshot_notebook = glade_xml_get_widget (xml, "screenshot_notebook");
	gtk_box_pack_start (sidebar->priv->screenshot_hist_vbox,
			    sidebar->priv->screenshot_notebook,
			    TRUE, TRUE, 0);
	
	/* Snap */
	sidebar->priv->screenshot1 = gmameui_get_image_from_stock ("gmameui-screen");
	sidebar->priv->screenshot_box1 = glade_xml_get_widget (xml, "screenshot_box1");
	gtk_container_add (GTK_CONTAINER (sidebar->priv->screenshot_box1),
			   GTK_WIDGET (sidebar->priv->screenshot1));

	/* Flyer */
	sidebar->priv->screenshot2 = gmameui_get_image_from_stock ("gmameui-screen");
	sidebar->priv->screenshot_box2 = glade_xml_get_widget (xml, "screenshot_box2");
	gtk_container_add (GTK_CONTAINER (sidebar->priv->screenshot_box2),
			   GTK_WIDGET (sidebar->priv->screenshot2));

	/* Cab */
	sidebar->priv->screenshot3 = gmameui_get_image_from_stock ("gmameui-screen");
	sidebar->priv->screenshot_box3 = glade_xml_get_widget (xml, "screenshot_box3");
	gtk_container_add (GTK_CONTAINER (sidebar->priv->screenshot_box3),
			   GTK_WIDGET (sidebar->priv->screenshot3));

	/* Marquee */
	sidebar->priv->screenshot4 = gmameui_get_image_from_stock ("gmameui-screen");
	sidebar->priv->screenshot_box4 = glade_xml_get_widget (xml, "screenshot_box4");
	gtk_container_add (GTK_CONTAINER (sidebar->priv->screenshot_box4),
			   GTK_WIDGET (sidebar->priv->screenshot4));

	/* Title */
	sidebar->priv->screenshot5 = gmameui_get_image_from_stock ("gmameui-screen");
	sidebar->priv->screenshot_box5 = glade_xml_get_widget (xml, "screenshot_box5");
	gtk_container_add (GTK_CONTAINER (sidebar->priv->screenshot_box5),
			   GTK_WIDGET (sidebar->priv->screenshot5));

	/* Control Panels */
	sidebar->priv->screenshot6 = gmameui_get_image_from_stock ("gmameui-screen");
	sidebar->priv->screenshot_box6 = glade_xml_get_widget (xml, "screenshot_box6");
	gtk_container_add (GTK_CONTAINER (sidebar->priv->screenshot_box6),
			   GTK_WIDGET (sidebar->priv->screenshot6));

	g_signal_connect (G_OBJECT (sidebar->priv->screenshot_notebook), "switch-page",
			    G_CALLBACK (on_screenshot_notebook_switch_page),
			    NULL);

	xml = glade_xml_new (GLADEDIR "sidebar.glade", "history_scrollwin", GETTEXT_PACKAGE);
	
	/* here we create the history box that will be filled later */
	sidebar->priv->history_scrollwin = glade_xml_get_widget (xml, "history_scrollwin");
	gtk_box_pack_end (sidebar->priv->screenshot_hist_vbox,
			  sidebar->priv->history_scrollwin,
			  TRUE, TRUE, 5);

	sidebar->priv->history_buffer = gtk_text_buffer_new (NULL);
	sidebar->priv->history_box = glade_xml_get_widget (xml, "history_box");
	gtk_text_view_set_buffer (GTK_TEXT_VIEW (sidebar->priv->history_box),
				  sidebar->priv->history_buffer);
	
	gtk_widget_show (sidebar->priv->history_scrollwin);
	gtk_widget_show (sidebar->priv->history_box);

	gtk_widget_show_all (GTK_WIDGET (sidebar));

	gtk_widget_hide (GTK_WIDGET (sidebar->priv->screenshot_event_box));
	GMAMEUI_DEBUG ("Finished creating sidebar");
}

GtkWidget *
gmameui_sidebar_new (void)
{
	return g_object_new (GMAMEUI_TYPE_SIDEBAR,
			     NULL);

}

struct _sidebar_screenshot {
	GtkWidget *screenshot_box;
	GtkWidget *screenshot;
};

typedef struct _sidebar_screenshot sidebar_screenshot;

static void
gmameui_sidebar_set_with_rom (GMAMEUISidebar *sidebar, RomEntry *rom)
{
	g_return_if_fail (sidebar != NULL);
		
	GMAMEUI_DEBUG ("Setting page");
	
	screenshot_type show_flyer;
	int wwidth, wheight;
	GtkWidget *pict = NULL;
	
	gboolean had_history;

	wwidth = 0; wheight = 0;
	
	g_object_get (main_gui.gui_prefs,
		      "show-flyer", &show_flyer,
		      NULL);

	if (rom) {
		UPDATE_GUI;

		GtkRequisition requisition;
		gtk_widget_size_request (sidebar->priv->screenshot_box1,
					 &requisition);
		wwidth = requisition.width;
		wheight = requisition.height;

		had_history = FALSE;
		had_history = gmameui_sidebar_set_history (sidebar, rom);

			pict = get_pixbuf (rom, show_flyer, wwidth, wheight);
			/* Remove the elements from the container and then re-add.
			   This is necessary to refresh the image */
			switch (show_flyer) {
			case (SNAPSHOTS):
				gtk_container_remove (GTK_CONTAINER (sidebar->priv->screenshot_box1),
						      sidebar->priv->screenshot1);
				sidebar->priv->screenshot1 = pict;					
				gtk_container_add (GTK_CONTAINER (sidebar->priv->screenshot_box1),
						   GTK_WIDGET (sidebar->priv->screenshot1));
				gtk_widget_show_all (sidebar->priv->screenshot_box1);
				break;
			case (FLYERS):
				gtk_container_remove (GTK_CONTAINER (sidebar->priv->screenshot_box2),
						      sidebar->priv->screenshot2);
				sidebar->priv->screenshot2 = pict;
				gtk_container_add (GTK_CONTAINER (sidebar->priv->screenshot_box2),
						   GTK_WIDGET (sidebar->priv->screenshot2));
				gtk_widget_show_all (sidebar->priv->screenshot_box2);
				break;
			case (CABINETS):
				gtk_container_remove (GTK_CONTAINER (sidebar->priv->screenshot_box3),
						      sidebar->priv->screenshot3);
				sidebar->priv->screenshot3 = pict;
				gtk_container_add (GTK_CONTAINER (sidebar->priv->screenshot_box3),
						   GTK_WIDGET (sidebar->priv->screenshot3));
				gtk_widget_show_all (sidebar->priv->screenshot_box3);
				break;
			case (MARQUEES):
				gtk_container_remove (GTK_CONTAINER (sidebar->priv->screenshot_box4),
						      sidebar->priv->screenshot4);
				sidebar->priv->screenshot4 = pict;
				gtk_container_add (GTK_CONTAINER (sidebar->priv->screenshot_box4),
						   GTK_WIDGET (sidebar->priv->screenshot4));
				gtk_widget_show_all (sidebar->priv->screenshot_box4);
				break;
			case (TITLES):
				gtk_container_remove (GTK_CONTAINER (sidebar->priv->screenshot_box5),
						      sidebar->priv->screenshot5);
				sidebar->priv->screenshot5 = pict;
				gtk_container_add (GTK_CONTAINER (sidebar->priv->screenshot_box5),
						   GTK_WIDGET (sidebar->priv->screenshot5));
				gtk_widget_show_all (sidebar->priv->screenshot_box5);
				break;
			case (CONTROL_PANELS):
				gtk_container_remove (GTK_CONTAINER (sidebar->priv->screenshot_box6),
						      sidebar->priv->screenshot6);
				sidebar->priv->screenshot6 = pict;
				gtk_container_add (GTK_CONTAINER (sidebar->priv->screenshot_box6),
						   GTK_WIDGET (sidebar->priv->screenshot6));
				gtk_widget_show_all (sidebar->priv->screenshot_box6);
				break;	
			}
		
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

