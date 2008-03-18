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
#include "network_game.h"

static guint timeout_icon;

static void        update_screenshot_panel (RomEntry *rom);

static void
set_game_pixbuff_from_iter (GtkTreeIter *iter,
			    ZIP         *zip,
			    gint         page_size)
{
	RomEntry *tmprom;
	GdkRectangle rect;
	GtkTreePath *tree_path;

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

		tmprom->icon_pixbuf = get_icon_for_rom (tmprom, gui_prefs.ListFontHeight, zip);

		if (tmprom->icon_pixbuf) {

			if ((gui_prefs.current_mode == LIST_TREE) || (gui_prefs.current_mode == DETAILS_TREE))
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

	/* open the zip file only at the begining */
	zipfile = g_build_filename (gui_prefs.IconDirectory, "icons.zip", NULL);
	zip = openzip (zipfile);

	/* Getting the vertical window area */
	vadj=gtk_scrolled_window_get_vadjustment (GTK_SCROLLED_WINDOW (main_gui.scrolled_window_games));

	/* Disable the callback */
	g_signal_handlers_block_by_func (G_OBJECT (gtk_scrolled_window_get_vadjustment (GTK_SCROLLED_WINDOW (main_gui.scrolled_window_games))),
					 (gpointer)adjustment_scrolled, NULL);

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

static void
create_toolbar (void)
{
	GtkWidget *tmp_toolbar_icon;
	GtkToolItem *item;

	/* addition of toolbar buttons */
	item = gtk_tool_button_new_from_stock (GTK_STOCK_NEW);
	gtk_toolbar_insert (main_gui.toolbar, item, -1);
	gtk_tool_button_set_label (GTK_TOOL_BUTTON (item), _("Play Game"));
	gtk_widget_show_all (GTK_WIDGET (item));
	g_signal_connect (G_OBJECT (item), "clicked",
			  G_CALLBACK (on_play_clicked), NULL);

	item = gtk_tool_button_new_from_stock (GTK_STOCK_PROPERTIES);
	gtk_toolbar_insert (main_gui.toolbar, item, -1);
	gtk_widget_show_all (GTK_WIDGET (item));
	g_signal_connect (G_OBJECT (item), "clicked",
			  G_CALLBACK (on_properties_clicked), NULL);

	item = gtk_tool_button_new_from_stock (GTK_STOCK_REFRESH);
	gtk_toolbar_insert (main_gui.toolbar, item, -1);
	gtk_widget_show_all (GTK_WIDGET (item));
	g_signal_connect (G_OBJECT (item), "clicked",
			  G_CALLBACK (on_refresh_clicked), NULL);

	item = gtk_separator_tool_item_new ();
	gtk_widget_show (GTK_WIDGET (item));
	gtk_toolbar_insert (main_gui.toolbar, item, -1);

	/* filters and snapshots buttons */
	tmp_toolbar_icon = gmameui_get_image_from_stock ("gmameui-view-folders");
	item = gtk_toggle_tool_button_new ();
	gtk_tool_button_set_icon_widget (GTK_TOOL_BUTTON (item), tmp_toolbar_icon);
	gtk_tool_button_set_label (GTK_TOOL_BUTTON (item), _("Show Folders"));
	gtk_toolbar_insert (main_gui.toolbar, item, -1);
	gtk_widget_show_all (GTK_WIDGET (item));

	main_gui.filterShowButton = GTK_TOGGLE_TOOL_BUTTON (item);

	gtk_toggle_tool_button_set_active (main_gui.filterShowButton, gui_prefs.ShowFolderList);

	tmp_toolbar_icon = gmameui_get_image_from_stock ("gmameui-view-screenshot");
	item = gtk_toggle_tool_button_new ();
	gtk_tool_button_set_icon_widget (GTK_TOOL_BUTTON (item), tmp_toolbar_icon);
	gtk_tool_button_set_label (GTK_TOOL_BUTTON (item), _("Show Sidebar"));
	gtk_toolbar_insert (main_gui.toolbar, item, -1);
	gtk_widget_show_all (GTK_WIDGET (item));

	main_gui.snapShowButton = GTK_TOGGLE_TOOL_BUTTON (item);

	gtk_toggle_tool_button_set_active (main_gui.snapShowButton, gui_prefs.ShowScreenShot);

	item = gtk_separator_tool_item_new ();
	gtk_widget_show (GTK_WIDGET (item));
	gtk_toolbar_insert (main_gui.toolbar, item, -1);

	/* listing mode buttons */
	tmp_toolbar_icon = gmameui_get_image_from_stock ("gmameui-view-list");
	item = gtk_toggle_tool_button_new ();
	gtk_tool_button_set_icon_widget (GTK_TOOL_BUTTON (item), tmp_toolbar_icon);
	gtk_tool_button_set_label (GTK_TOOL_BUTTON (item), _("List"));
	gtk_toolbar_insert (main_gui.toolbar, item, -1);
	gtk_widget_show_all (GTK_WIDGET (item));

	main_gui.list_view_button = GTK_TOGGLE_TOOL_BUTTON (item);

	tmp_toolbar_icon = gmameui_get_image_from_stock ("gmameui-view-tree");
	item = gtk_toggle_tool_button_new ();
	gtk_tool_button_set_icon_widget (GTK_TOOL_BUTTON (item), tmp_toolbar_icon);
	gtk_tool_button_set_label (GTK_TOOL_BUTTON (item), _("List Tree"));
	gtk_toolbar_insert (main_gui.toolbar, item, -1);
	gtk_widget_show_all (GTK_WIDGET (item));

	main_gui.list_tree_view_button = GTK_TOGGLE_TOOL_BUTTON (item);

	tmp_toolbar_icon = gmameui_get_image_from_stock ("gmameui-view-list");
	item = gtk_toggle_tool_button_new ();
	gtk_tool_button_set_icon_widget (GTK_TOOL_BUTTON (item), tmp_toolbar_icon);
	gtk_tool_button_set_label (GTK_TOOL_BUTTON (item), _("Details"));
	gtk_toolbar_insert (main_gui.toolbar, item, -1);
	gtk_widget_show_all (GTK_WIDGET (item));

	main_gui.details_view_button = GTK_TOGGLE_TOOL_BUTTON (item);

	tmp_toolbar_icon = gmameui_get_image_from_stock ("gmameui-view-tree");
	item = gtk_toggle_tool_button_new ();
	gtk_tool_button_set_icon_widget (GTK_TOOL_BUTTON (item), tmp_toolbar_icon);
	gtk_tool_button_set_label (GTK_TOOL_BUTTON (item), _("Details Tree"));
	gtk_toolbar_insert (main_gui.toolbar, item, -1);
	gtk_widget_show_all (GTK_WIDGET (item));

	main_gui.details_tree_view_button = GTK_TOGGLE_TOOL_BUTTON (item);

	/* init the mode button */
	switch (gui_prefs.current_mode) {
	case (LIST):
		gtk_toggle_tool_button_set_active (main_gui.list_view_button, TRUE);
		break;
	case (LIST_TREE):
		gtk_toggle_tool_button_set_active (main_gui.list_tree_view_button, TRUE);
		break;
	case (DETAILS):
		gtk_toggle_tool_button_set_active (main_gui.details_view_button, TRUE);
		break;
	case (DETAILS_TREE):
	default:
		gtk_toggle_tool_button_set_active (main_gui.details_tree_view_button, TRUE);
		break;
	}

	/* Connection of toolbar buttons signals */
	g_signal_connect (G_OBJECT (main_gui.filterShowButton), "toggled",
			  G_CALLBACK (on_filterShowButton_toggled),
			  NULL);
	g_signal_connect (G_OBJECT (main_gui.snapShowButton), "toggled",
			  G_CALLBACK (on_snapShowButton_toggled),
			  NULL);
	g_signal_connect (G_OBJECT (main_gui.list_view_button), "toggled",
			  G_CALLBACK (on_mode_button_clicked),
			  GINT_TO_POINTER (LIST));
	g_signal_connect (G_OBJECT (main_gui.list_tree_view_button), "toggled",
			  G_CALLBACK (on_mode_button_clicked),
			  GINT_TO_POINTER (LIST_TREE));
	g_signal_connect (G_OBJECT (main_gui.details_view_button), "toggled",
			  G_CALLBACK (on_mode_button_clicked),
			  GINT_TO_POINTER (DETAILS));
	g_signal_connect (G_OBJECT (main_gui.details_tree_view_button), "toggled",
			  G_CALLBACK (on_mode_button_clicked),
			  GINT_TO_POINTER (DETAILS_TREE));
}

static gboolean
set_history (const gchar   *entry_name,
	     GtkTextBuffer *text_buffer)
{
	FILE *history_file;
	GtkTextIter text_iter;
	gchar line[2000];
	gint i, n;
	gchar *tmp, *p;
	gchar **games;
	gboolean found_game = FALSE;
	gboolean pointer_in_info = FALSE;
	gboolean extra_newline = FALSE;

	history_file = fopen (gui_prefs.HistoryFile, "r");

	if (!history_file) {
		GMAMEUI_DEBUG ("History.dat file not found");
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
	GtkTextIter text_iter;
	gchar line[2000];
	gint i;
	gchar *tmp, *p;
	gboolean found_game = FALSE;
	gboolean pointer_in_info = FALSE;
	gboolean extra_newline = FALSE;

	mameinfo_dat = fopen (gui_prefs.MameInfoFile, "r");

	if (!mameinfo_dat) {
		GMAMEUI_DEBUG ("mameinfo_dat file not found");
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

	switch (sctype) {
	case (SNAPSHOTS):
		filename = g_strdup_printf ("%s" G_DIR_SEPARATOR_S "%s.png", gui_prefs.SnapshotDirectory, rom->romname);
		filename_parent = g_strdup_printf ("%s" G_DIR_SEPARATOR_S "%s.png", gui_prefs.SnapshotDirectory, rom->cloneof);
		zipfile = g_build_filename (gui_prefs.SnapshotDirectory, "snap.zip", NULL);
		break;
	case (FLYERS):
		filename = g_strdup_printf ("%s" G_DIR_SEPARATOR_S "%s.png", gui_prefs.FlyerDirectory, rom->romname);
		filename_parent = g_strdup_printf ("%s" G_DIR_SEPARATOR_S "%s.png", gui_prefs.FlyerDirectory, rom->cloneof);
		zipfile = g_build_filename (gui_prefs.FlyerDirectory, "flyers.zip", NULL);
		break;
	case (CABINETS):
		filename = g_strdup_printf ("%s" G_DIR_SEPARATOR_S "%s.png", gui_prefs.CabinetDirectory, rom->romname);
		filename_parent = g_strdup_printf ("%s" G_DIR_SEPARATOR_S "%s.png", gui_prefs.CabinetDirectory, rom->cloneof);
		zipfile = g_build_filename (gui_prefs.CabinetDirectory, "cabinets.zip", NULL);
		break;
	case (MARQUEES):
		filename = g_strdup_printf ("%s" G_DIR_SEPARATOR_S "%s.png", gui_prefs.MarqueeDirectory, rom->romname);
		filename_parent = g_strdup_printf ("%s" G_DIR_SEPARATOR_S "%s.png", gui_prefs.MarqueeDirectory, rom->cloneof);
		zipfile = g_build_filename (gui_prefs.MarqueeDirectory, "marquees.zip", NULL);
		break;
	case (TITLES):
		filename = g_strdup_printf ("%s" G_DIR_SEPARATOR_S "%s.png", gui_prefs.TitleDirectory, rom->romname);
		filename_parent = g_strdup_printf ("%s" G_DIR_SEPARATOR_S "%s.png", gui_prefs.TitleDirectory, rom->cloneof);
		zipfile = g_build_filename (gui_prefs.TitleDirectory, "titles.zip", NULL);
		break;
	case (CONTROL_PANELS):
		filename = g_strdup_printf ("%s" G_DIR_SEPARATOR_S "%s.png", gui_prefs.CPanelDirectory, rom->romname);
		filename_parent = g_strdup_printf ("%s" G_DIR_SEPARATOR_S "%s.png", gui_prefs.CPanelDirectory, rom->cloneof);
		zipfile = g_build_filename (gui_prefs.TitleDirectory, "cpanels.zip", NULL);
		break;
	default:
		filename = g_strdup_printf ("%s" G_DIR_SEPARATOR_S "%s.png", gui_prefs.SnapshotDirectory, rom->romname);
		filename_parent = g_strdup_printf ("%s" G_DIR_SEPARATOR_S "%s.png", gui_prefs.SnapshotDirectory, rom->cloneof);
		zipfile = g_build_filename (gui_prefs.SnapshotDirectory, "snap.zip", NULL);
	}

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
			/* Since MAME 0.111, snapshots are now in a subdirectory per game
			   with numeric names 0000.png, 0001.png, etc. */
			/* TODO Need to do the same for the roms parent if this is a clone */
			filename = g_strdup_printf ("%s" G_DIR_SEPARATOR_S "%s" G_DIR_SEPARATOR_S "0000.png", gui_prefs.SnapshotDirectory, rom->romname);
			GMAMEUI_DEBUG ("Looking for image %s", filename);
			pixbuf = gdk_pixbuf_new_from_file (filename,error);
			g_free (filename);
		}
	}
	
	/* we havent found the picture in the directory, maybe we could try in a zipfile */
	if (!pixbuf) {
		ZIP *zip;
		struct zipent* zipent;
		gchar *tmp_buffer;
		gchar *parent_tmp_buffer = NULL;
		gsize parent_buf_size = 0
;
		gchar *parent_filename;

		zip = openzip (zipfile);

		if (zip) {
			GdkPixbufLoader *loader;

			GMAMEUI_DEBUG ("Succesfully open zip file '%s' !", zipfile);
			filename = g_strdup_printf ("%s.", rom->romname);
			parent_filename = g_strdup_printf ("%s.", rom->cloneof);

			while ( (zipent = readzip (zip)) != 0) {
				/* this should allows to find any format of picture in the zip, not only bmp */
				if (!strncmp (filename,zipent->name, strlen (rom->romname) + 1)) {
					GMAMEUI_DEBUG ("found file name %s\twith CRC:%i\tsize%i",
							zipent->name,
							zipent->crc32,
							zipent->uncompressed_size);
					tmp_buffer = read_zipentry (zip, zipent);
					if (tmp_buffer) {	/* if the file successfully uncompress, try to load it in a pixbuf loader */
						loader = gdk_pixbuf_loader_new ();
						if (!gdk_pixbuf_loader_write (loader, (guchar *)tmp_buffer, zipent->uncompressed_size, error)) {
							GMAMEUI_DEBUG ("Error while uncompressing %s from %s", zipent->name, zipfile);
						} else {
							gdk_pixbuf_loader_close (loader,error);
							pixbuf = gdk_pixbuf_loader_get_pixbuf (loader);
						}
						g_free (tmp_buffer);
					}
					/* prevent to read all zip file if we have found the picture's game (uncompressed successfuly or not) */
					break;

				} else if (!strncmp (parent_filename, zipent->name, strlen (rom->cloneof) + 1)) {
					parent_tmp_buffer = read_zipentry (zip, zipent);
					parent_buf_size = zipent->uncompressed_size;
				}
			}
			g_free (filename);
			g_free (parent_filename);

			/* no picture found try parent game if any*/
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
		
	}

	if (pixbuf) {
		GdkPixbuf *scaled_pixbuf;

		width = gdk_pixbuf_get_width (pixbuf);
		height = gdk_pixbuf_get_height (pixbuf);
		if (gui_prefs.ShowScreenShot == 1) {
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
	if (event && event->button <= 3) {
		gui_prefs.ShowFlyer = (++gui_prefs.ShowFlyer) % 5;
		update_screenshot_panel (gui_prefs.current_game);
	}
}

static void
update_screenshot_panel (RomEntry *rom)
{
	gboolean had_info = FALSE, had_history = FALSE;
	int wwidth, wheight;
	GtkWidget *pict = NULL;
	GtkTextIter text_iter;

	wwidth = 0; wheight = 0;

	if (rom) {
		UPDATE_GUI;
		if (gui_prefs.ShowScreenShotTab == 0) {
			gdk_drawable_get_size ((main_gui.screenshot_event_box)->window, &wwidth, &wheight);
		} else {
			GtkRequisition requisition;
			gtk_widget_size_request (main_gui.screenshot_box1, &requisition);
			wwidth = requisition.width;
			wheight = requisition.height;
		}

		/* erase, fill and show the history box */
		/* should freeze the history_box here rather than each function otherwise, the position of the cursor will
		    appear between mameinfo and history */
		gtk_text_buffer_set_text (main_gui.history_buffer, "", -1);
		had_history = set_game_history (rom, main_gui.history_buffer);
		if (had_history) {
			gtk_text_buffer_get_end_iter (main_gui.history_buffer, &text_iter);
			gtk_text_buffer_insert (main_gui.history_buffer, &text_iter, "\n", -1);
		}
		had_info = set_game_info (rom, main_gui.history_buffer);

		if (gui_prefs.ShowScreenShotTab == 0) {
			/* Remove the elements from the container */
			gtk_container_remove (GTK_CONTAINER (main_gui.screenshot_event_box), main_gui.main_screenshot);
			gtk_container_remove (GTK_CONTAINER (main_gui.screenshot_hist_vbox), main_gui.screenshot_event_box);
			pict = get_pixbuf (rom, gui_prefs.ShowFlyer, wwidth, wheight);
			main_gui.main_screenshot = pict;
			main_gui.screenshot_event_box = gtk_event_box_new ();
			
			/* And add them again */
			gtk_container_add (GTK_CONTAINER (main_gui.screenshot_event_box), GTK_WIDGET (main_gui.main_screenshot));
			gtk_widget_show (main_gui.screenshot_event_box);
			gtk_widget_show (main_gui.main_screenshot);
			g_signal_connect (G_OBJECT (main_gui.screenshot_event_box), "button-release-event",
					  G_CALLBACK (change_screenshot),
					  NULL);
		} else {
			/* Remove the elements from the container */
			switch (gui_prefs.ShowFlyer) {
			case (SNAPSHOTS):
				gtk_container_remove (GTK_CONTAINER (main_gui.screenshot_box1), main_gui.screenshot1);
				pict = get_pixbuf (rom, 0, wwidth, wheight);
				main_gui.screenshot1 = pict;
				break;
			case (FLYERS):
				gtk_container_remove (GTK_CONTAINER (main_gui.screenshot_box2), main_gui.screenshot2);
				pict = get_pixbuf (rom, 1, wwidth, wheight);
				main_gui.screenshot2 = pict;
				break;
			case (CABINETS):
				gtk_container_remove (GTK_CONTAINER (main_gui.screenshot_box3), main_gui.screenshot3);
				pict = get_pixbuf (rom, 2, wwidth, wheight);
				main_gui.screenshot3 = pict;
				break;
			case (MARQUEES):
				gtk_container_remove (GTK_CONTAINER (main_gui.screenshot_box4), main_gui.screenshot4);
				pict = get_pixbuf (rom, 3, wwidth, wheight);
				main_gui.screenshot4 = pict;
				break;
			case (TITLES):
				gtk_container_remove (GTK_CONTAINER (main_gui.screenshot_box5), main_gui.screenshot5);
				pict = get_pixbuf (rom, 4, wwidth, wheight);
				main_gui.screenshot5 = pict;
				break;
			case (CONTROL_PANELS):
				gtk_container_remove (GTK_CONTAINER (main_gui.screenshot_box6), main_gui.screenshot6);
				pict = get_pixbuf (rom, 5, wwidth, wheight);
				main_gui.screenshot6 = pict;
				break;	
			}
			
			/* And add them again */
			switch (gui_prefs.ShowFlyer) {
			case (SNAPSHOTS):
				gtk_container_add (GTK_CONTAINER (main_gui.screenshot_box1), GTK_WIDGET (main_gui.screenshot1));
				gtk_widget_show (main_gui.screenshot_box1);
				gtk_widget_show (main_gui.screenshot1);
				break;
			case (FLYERS):
				gtk_container_add (GTK_CONTAINER (main_gui.screenshot_box2), GTK_WIDGET (main_gui.screenshot2));
				gtk_widget_show (main_gui.screenshot_box2);
				gtk_widget_show (main_gui.screenshot2);
				break;
			case (CABINETS):
				gtk_container_add (GTK_CONTAINER (main_gui.screenshot_box3), GTK_WIDGET (main_gui.screenshot3));
				gtk_widget_show (main_gui.screenshot_box3);
				gtk_widget_show (main_gui.screenshot3);
				break;
			case (MARQUEES):
				gtk_container_add (GTK_CONTAINER (main_gui.screenshot_box4), GTK_WIDGET (main_gui.screenshot4));
				gtk_widget_show (main_gui.screenshot_box4);
				gtk_widget_show (main_gui.screenshot4);
				break;
			case (TITLES):
				gtk_container_add (GTK_CONTAINER (main_gui.screenshot_box5), GTK_WIDGET (main_gui.screenshot5));
				gtk_widget_show (main_gui.screenshot_box5);
				gtk_widget_show (main_gui.screenshot5);
				break;
			case (CONTROL_PANELS):
				gtk_container_add (GTK_CONTAINER (main_gui.screenshot_box6), GTK_WIDGET (main_gui.screenshot6));
				gtk_widget_show (main_gui.screenshot_box6);
				gtk_widget_show (main_gui.screenshot6);
				break;
			}					
		}
		
		if (had_history || had_info) {
			gtk_widget_show (GTK_WIDGET (main_gui.history_scrollwin));
			if (gui_prefs.ShowScreenShotTab == 0)
				gtk_box_pack_end (main_gui.screenshot_hist_vbox, main_gui.screenshot_event_box, FALSE, TRUE, 5);
		} else {
			gtk_widget_hide (GTK_WIDGET (main_gui.history_scrollwin));
			if (gui_prefs.ShowScreenShotTab == 0)
				gtk_box_pack_end (main_gui.screenshot_hist_vbox, main_gui.screenshot_event_box, TRUE, TRUE, 5);
		}
		
	} else {
		/* no roms selected display the default picture */ 
		if (gui_prefs.ShowScreenShotTab == 0) {
			gtk_container_remove (GTK_CONTAINER (main_gui.screenshot_event_box), main_gui.main_screenshot);
			gtk_container_remove (GTK_CONTAINER (main_gui.screenshot_hist_vbox), main_gui.screenshot_event_box);

			main_gui.main_screenshot = gmameui_get_image_from_stock ("gmameui-screen");
			main_gui.screenshot_event_box = gtk_event_box_new ();
			gtk_box_pack_end (main_gui.screenshot_hist_vbox, main_gui.screenshot_event_box, TRUE, TRUE, 5);
			gtk_container_add (GTK_CONTAINER (main_gui.screenshot_event_box), GTK_WIDGET (main_gui.main_screenshot));
			gtk_widget_show (main_gui.screenshot_event_box);
			gtk_widget_show (main_gui.main_screenshot);
		}

		/* erase and hide the history box */
		gtk_text_buffer_set_text (GTK_TEXT_BUFFER (main_gui.history_buffer), "", -1);
		gtk_widget_hide (GTK_WIDGET (main_gui.history_scrollwin));
	}
}

static void
on_screenshot_notebook_switch_page (GtkNotebook *notebook,
				    GtkNotebookPage *page,
				    guint page_num,
				    gpointer user_data)
{
	gui_prefs.ShowFlyer = page_num;
	update_screenshot_panel (gui_prefs.current_game);
}

static void
create_gamelist_popupmenu (void)
{
	GtkWidget *separator;
	GtkWidget *select_random_game;
	GtkWidget *record_game;
	GtkWidget *playback_game;
	GtkWidget *properties;
	GtkWidget *play_game;
	GtkWidget *play_network_game;
	GtkWidget *add_to_favorites;
	GtkWidget *remove_from_favorites;
	GtkTooltips *tooltips;
	GtkWidget *popup_menu;

	tooltips = gtk_tooltips_new ();

	/* build the gamelist popup menu */
	popup_menu = gtk_menu_new ();
	gtk_widget_ref (GTK_WIDGET (popup_menu));
	g_object_set_data_full (G_OBJECT (MainWindow), "popup_gamelist_menu", popup_menu,
				  (GtkDestroyNotify) gtk_widget_unref);

	/* Play game */
	play_game = gtk_menu_item_new_with_label (_("Play"));
	gtk_widget_ref (play_game);
	g_object_set_data_full (G_OBJECT (popup_menu), "play_game", play_game,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (play_game);
	gtk_container_add (GTK_CONTAINER (popup_menu), play_game);
	gtk_tooltips_set_tip (tooltips, play_game, _("Play selected game"), NULL);
  
	/* Play network game */
	play_network_game = gtk_image_menu_item_new_from_stock (GTK_STOCK_NETWORK, NULL);
	gtk_label_set_text_with_mnemonic (GTK_LABEL (GTK_BIN (play_network_game)->child), _("Play network game"));
	
	gtk_widget_ref (play_network_game);
	g_object_set_data_full (G_OBJECT (popup_menu),
	              "play_network_game",
	              play_network_game,
				  (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_show (play_network_game);
	gtk_container_add (GTK_CONTAINER (popup_menu), play_network_game);
	gtk_tooltips_set_tip (tooltips, play_network_game, _("Play selected game over a network"), NULL);
  	
	separator = gtk_menu_item_new ();
	gtk_widget_show (separator);
	gtk_container_add (GTK_CONTAINER (popup_menu), separator);
	gtk_widget_set_sensitive (separator, FALSE);

	/* Record game */
	record_game = gtk_image_menu_item_new_from_stock (GTK_STOCK_SAVE, NULL);
	gtk_label_set_text_with_mnemonic (GTK_LABEL (GTK_BIN (record_game)->child), _("Play and Record Input..."));
	gtk_widget_ref (record_game);
	gtk_widget_show (record_game);
	gtk_container_add (GTK_CONTAINER (popup_menu), record_game);
	g_object_set_data_full (G_OBJECT (popup_menu), "record_game", record_game,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_tooltips_set_tip (tooltips, record_game, _("Record a game for later playback"), NULL);
	
	/* Playback game */
	playback_game = gtk_image_menu_item_new_from_stock (GTK_STOCK_OPEN, NULL);
	gtk_label_set_text_with_mnemonic (GTK_LABEL (GTK_BIN (playback_game)->child), _("Playback Input..."));
	gtk_widget_ref (playback_game);
	gtk_widget_show (playback_game);
	gtk_container_add (GTK_CONTAINER (popup_menu), playback_game);
	g_object_set_data_full (G_OBJECT (popup_menu), "playback_game", playback_game,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_tooltips_set_tip (tooltips, playback_game, _("Playback a recorded game"), NULL);
	
	separator = gtk_menu_item_new ();
	gtk_widget_show (separator);
	gtk_container_add (GTK_CONTAINER (popup_menu), separator);
	gtk_widget_set_sensitive (separator, FALSE);

	/* add to favorites */
	add_to_favorites = gtk_image_menu_item_new_from_stock (GTK_STOCK_ADD, NULL);
	gtk_label_set_text_with_mnemonic (GTK_LABEL (GTK_BIN (add_to_favorites)->child), _("Add to 'Favorites'"));
	gtk_widget_ref (add_to_favorites);
	g_object_set_data_full (G_OBJECT (popup_menu), "add_to_favorites", add_to_favorites,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (add_to_favorites);
	gtk_container_add (GTK_CONTAINER (popup_menu), add_to_favorites);
	gtk_tooltips_set_tip (tooltips, add_to_favorites, _("Add this game to your 'Favorites'"), NULL);
	
	/* remove from favorites */
	remove_from_favorites = gtk_image_menu_item_new_from_stock (GTK_STOCK_REMOVE, NULL);
	gtk_label_set_text_with_mnemonic (GTK_LABEL (GTK_BIN (remove_from_favorites)->child), _("Remove from 'Favorites'"));
	gtk_widget_ref (remove_from_favorites);
	g_object_set_data_full (G_OBJECT (popup_menu), "remove_from_favorites", remove_from_favorites,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (remove_from_favorites);
	gtk_container_add (GTK_CONTAINER (popup_menu), remove_from_favorites);
	gtk_tooltips_set_tip (tooltips, remove_from_favorites, _("Remove this game from your 'Favorites' game folder"), NULL);

	separator = gtk_menu_item_new ();
	gtk_widget_show (separator);
	gtk_container_add (GTK_CONTAINER (popup_menu), separator);
	gtk_widget_set_sensitive (separator, FALSE);

	select_random_game = gtk_menu_item_new_with_label (_("Select Random Game"));
	gtk_widget_ref (select_random_game);
	g_object_set_data_full (G_OBJECT (popup_menu), "select_random_game", select_random_game,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (select_random_game);
	gtk_container_add (GTK_CONTAINER (popup_menu), select_random_game);
	gtk_tooltips_set_tip (tooltips, select_random_game, _("Randomly select a game"), NULL);
	
	separator = gtk_menu_item_new ();
	gtk_widget_show (separator);
	gtk_container_add (GTK_CONTAINER (popup_menu), separator);
	gtk_widget_set_sensitive (separator, FALSE);

	properties = gtk_image_menu_item_new_from_stock (GTK_STOCK_PROPERTIES, NULL);
	gtk_widget_ref (properties);
	g_object_set_data_full (G_OBJECT (popup_menu), "properties", properties,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (properties);
	gtk_container_add (GTK_CONTAINER (popup_menu), properties);
	gtk_tooltips_set_tip (tooltips, properties, _("Display the properties of the selected game"), NULL);


	g_signal_connect (G_OBJECT (play_game), "activate",
                      	    G_CALLBACK (on_play_activate),
                            NULL);

	g_signal_connect (G_OBJECT (play_network_game), "activate",
                      	    G_CALLBACK (on_network_play_activate),
                            NULL);

	g_signal_connect (G_OBJECT (record_game), "activate",
			    G_CALLBACK (on_play_and_record_input_activate),
			    NULL);

	g_signal_connect (G_OBJECT (playback_game), "activate",
			    G_CALLBACK (on_playback_input_activate),
			    NULL);

	g_signal_connect (G_OBJECT (add_to_favorites), "activate",
			    G_CALLBACK (on_add_to_favorites_activate),
			    NULL);

	g_signal_connect (G_OBJECT (remove_from_favorites), "activate",
			    G_CALLBACK (on_remove_from_favorites_activate),
			    NULL);

	g_signal_connect (G_OBJECT (select_random_game), "activate",
			    G_CALLBACK (on_select_random_game_activate),
			    NULL);

	g_signal_connect (G_OBJECT (properties), "activate",
			    G_CALLBACK (on_properties_activate),
			    NULL);
}

static void
create_columns_popupmenu (void)
{
	GtkTooltips *tooltips;
	GtkWidget *cancel;
	GtkWidget *separator1;

	tooltips = gtk_tooltips_new ();

	/* build the gamelist popup menu */
	main_gui.popup_column_menu = GTK_MENU (gtk_menu_new ());
	gtk_widget_ref (GTK_WIDGET (main_gui.popup_column_menu));
	g_object_set_data_full (G_OBJECT (MainWindow), "popup_column_menu", main_gui.popup_column_menu,
				  (GtkDestroyNotify) gtk_widget_unref);


	main_gui.popup_column_hide = gtk_menu_item_new_with_label (_("Hide Column"));
	gtk_widget_ref (main_gui.popup_column_hide);
	g_object_set_data_full (G_OBJECT (MainWindow), "hide", main_gui.popup_column_hide,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (main_gui.popup_column_hide);
	gtk_container_add (GTK_CONTAINER (main_gui.popup_column_menu), main_gui.popup_column_hide);
	gtk_tooltips_set_tip (tooltips, main_gui.popup_column_hide, _("Hide Column"), NULL);
  	g_signal_connect (G_OBJECT (main_gui.popup_column_hide), "activate",
                      	    G_CALLBACK (on_column_hide_activate),
                            NULL);

	main_gui.popup_column_layout = gtk_menu_item_new_with_label (_("Column Layout..."));
	gtk_widget_ref (main_gui.popup_column_layout);
	gtk_widget_show (main_gui.popup_column_layout);
	gtk_container_add (GTK_CONTAINER (main_gui.popup_column_menu), main_gui.popup_column_layout);
	g_object_set_data_full (G_OBJECT (MainWindow), "column_layout_menu", main_gui.popup_column_layout,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_tooltips_set_tip (tooltips, main_gui.popup_column_layout, _("Show, Hide or Order Columns"), NULL);
	g_signal_connect (G_OBJECT (main_gui.popup_column_layout), "activate",
			    G_CALLBACK (on_column_layout_activate),
			    NULL);

	separator1 = gtk_menu_item_new ();
	gtk_widget_show (separator1);
	gtk_container_add (GTK_CONTAINER (main_gui.popup_column_menu), separator1);
	gtk_widget_set_sensitive (separator1, FALSE);

	cancel = gtk_image_menu_item_new_from_stock (GTK_STOCK_CANCEL, NULL);
	gtk_widget_ref (cancel);
	gtk_widget_show (cancel);
	gtk_container_add (GTK_CONTAINER (main_gui.popup_column_menu), cancel);
	g_object_set_data_full (G_OBJECT (MainWindow), "cancel", cancel,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_tooltips_set_tip (tooltips, main_gui.popup_column_layout, _("Cancel"), NULL);
}

void
init_gui (void)
{
	GtkTooltips *tooltips;
	gchar *filename;

	tooltips = gtk_tooltips_new ();

	/* Default Pixbuf once for all windows */
	filename = g_build_filename (DATADIR, "gmameui", "gmameui.png", NULL);
        gtk_window_set_default_icon_from_file (filename, NULL);
	g_free (filename);

	gmameui_icons_init ();

	/* Create the main window */
	main_gui.details_view_button = NULL;
	main_gui.details_tree_view_button = NULL;
	MainWindow = create_MainWindow ();
	
	/* if the ListFont is empty or not loadable, use default font */
	gui_prefs.ListFontStruct = NULL;
	gui_prefs.ListFontHeight = 16;

	main_gui.filters_tree_model = NULL;
	/* Create the UI of the filter List */
	create_filterslist ();
	
	gtk_paned_set_position (main_gui.hpanedLeft, gui_prefs.Splitters[0]);
	gtk_paned_set_position (main_gui.hpanedRight, gui_prefs.Splitters[1]);

	gtk_widget_hide (GTK_WIDGET (main_gui.combo_progress_bar));

	gtk_window_set_default_size (GTK_WINDOW (MainWindow),
				    gui_prefs.GUIWidth,
				    gui_prefs.GUIHeight);

	gtk_window_move (GTK_WINDOW (MainWindow),
				    gui_prefs.GUIPosX,
				    gui_prefs.GUIPosY);

	/* Show and hence realize mainwindow so that MainWindow->window is available */
	gtk_widget_show (MainWindow);
	/* Need to create the menu to have all button in the toolbar ??? not really needed */
	create_toolbar ();
	add_exec_menu ();

	switch (gui_prefs.current_mode) {
	case (LIST):
		gtk_check_menu_item_set_active (main_gui.list_view_menu, TRUE);
		break;
	case (LIST_TREE):
		gtk_check_menu_item_set_active (main_gui.list_tree_view_menu, TRUE);
		break;
	case (DETAILS):
		gtk_check_menu_item_set_active (main_gui.details_view_menu, TRUE);
		break;
	case (DETAILS_TREE):
		gtk_check_menu_item_set_active (main_gui.details_tree_view_menu, TRUE);
		break;
	}

	if (! ((gui_prefs.current_mode == LIST_TREE) || (gui_prefs.current_mode == DETAILS_TREE))) {
		gtk_widget_set_sensitive (GTK_WIDGET (main_gui.expand_all_menu), FALSE);
		gtk_widget_set_sensitive (GTK_WIDGET (main_gui.collapse_all_menu), FALSE);
	}
	gtk_check_menu_item_set_active (main_gui.toolbar_view_menu, gui_prefs.ShowToolBar);
	gtk_check_menu_item_set_active (main_gui.status_bar_view_menu, gui_prefs.ShowStatusBar);
	gtk_check_menu_item_set_active (main_gui.folder_list_menu, gui_prefs.ShowFolderList);
	gtk_check_menu_item_set_active (main_gui.screen_shot_menu, gui_prefs.ShowScreenShot);

	/* Screenshot Event Box */
	main_gui.main_screenshot = gmameui_get_image_from_stock ("gmameui-screen");

	main_gui.screenshot_event_box = gtk_event_box_new ();
	gtk_box_pack_start (main_gui.screenshot_hist_vbox,main_gui.screenshot_event_box, TRUE, TRUE, 5);
	gtk_container_add (GTK_CONTAINER (main_gui.screenshot_event_box), GTK_WIDGET (main_gui.main_screenshot));
	gtk_widget_show (main_gui.screenshot_event_box);
	gtk_widget_show (main_gui.main_screenshot);

	/* Screenshot Notebook */
	GladeXML *xml = glade_xml_new (GLADEDIR "sidebar.glade", "screenshot_notebook", NULL);
	
	main_gui.screenshot_notebook = glade_xml_get_widget (xml, "screenshot_notebook");
	gtk_box_pack_start (main_gui.screenshot_hist_vbox, main_gui.screenshot_notebook, TRUE, TRUE, 0);

	/* Snap */
	main_gui.screenshot1 = gmameui_get_image_from_stock ("gmameui-screen");
	main_gui.screenshot_box1 = glade_xml_get_widget (xml, "screenshot_box1");
	gtk_container_add (GTK_CONTAINER (main_gui.screenshot_box1), GTK_WIDGET (main_gui.screenshot1));

	/* Flyer */
	main_gui.screenshot2 = gmameui_get_image_from_stock ("gmameui-screen");
	main_gui.screenshot_box2 = glade_xml_get_widget (xml, "screenshot_box2");
	gtk_container_add (GTK_CONTAINER (main_gui.screenshot_box2), GTK_WIDGET (main_gui.screenshot2));

	/* Cab */
	main_gui.screenshot3 = gmameui_get_image_from_stock ("gmameui-screen");
	main_gui.screenshot_box3 = glade_xml_get_widget (xml, "screenshot_box3");
	gtk_container_add (GTK_CONTAINER (main_gui.screenshot_box3), GTK_WIDGET (main_gui.screenshot3));

	/* Marquee */
	main_gui.screenshot4 = gmameui_get_image_from_stock ("gmameui-screen");
	main_gui.screenshot_box4 = glade_xml_get_widget (xml, "screenshot_box4");
	gtk_container_add (GTK_CONTAINER (main_gui.screenshot_box4), GTK_WIDGET (main_gui.screenshot4));

	/* Title */
	main_gui.screenshot5 = gmameui_get_image_from_stock ("gmameui-screen");
	main_gui.screenshot_box5 = glade_xml_get_widget (xml, "screenshot_box5");
	gtk_container_add (GTK_CONTAINER (main_gui.screenshot_box5), GTK_WIDGET (main_gui.screenshot5));

	/* Control Panels */
	main_gui.screenshot6 = gmameui_get_image_from_stock ("gmameui-screen");
	main_gui.screenshot_box6 = glade_xml_get_widget (xml, "screenshot_box6");
	gtk_container_add (GTK_CONTAINER (main_gui.screenshot_box6), GTK_WIDGET (main_gui.screenshot6));

	g_signal_connect (G_OBJECT (main_gui.screenshot_notebook), "switch-page",
			    G_CALLBACK (on_screenshot_notebook_switch_page),
			    NULL);

	xml = glade_xml_new (GLADEDIR "sidebar.glade", "history_scrollwin", NULL);
	/* here we create the history box that will be filled later */
	main_gui.history_scrollwin = glade_xml_get_widget (xml, "history_scrollwin");
	gtk_box_pack_end (main_gui.screenshot_hist_vbox, main_gui.history_scrollwin, TRUE, TRUE, 5);

	main_gui.history_buffer = gtk_text_buffer_new (NULL);
	main_gui.history_box = glade_xml_get_widget (xml, "history_box");
	gtk_text_view_set_buffer (GTK_TEXT_VIEW (main_gui.history_box), main_gui.history_buffer);
	
	gtk_widget_show (main_gui.history_scrollwin);
	gtk_widget_show (main_gui.history_box);
	
	/* Show Hide Screenshot */
	gtk_check_menu_item_set_active (main_gui.screen_shot_tab_menu,
					gui_prefs.ShowScreenShotTab);
	if (gui_prefs.ShowScreenShotTab == FALSE)
		gtk_widget_hide (GTK_WIDGET (main_gui.screenshot_notebook));
	else
		gtk_widget_hide (GTK_WIDGET (main_gui.screenshot_event_box));

	/* Create the popup menu */
	create_gamelist_popupmenu ();
	create_columns_popupmenu ();
	
	/* Create the UI of the Game List */
	create_gamelist (gui_prefs.current_mode);

	/* Feed the Filters List */
	create_filterslist_content ();

	/* Feed the Game List */
	create_gamelist_content ();

	/* Need to set the size here otherwise it move when we create the gamelist */
	if (gui_prefs.ShowScreenShot)
		gtk_paned_set_position (main_gui.hpanedRight, gui_prefs.Splitters[1]);

	/* Grab focus on the game list */
	gtk_widget_grab_focus (main_gui.displayed_list);

	g_signal_connect (G_OBJECT (gtk_scrolled_window_get_vadjustment (GTK_SCROLLED_WINDOW (main_gui.scrolled_window_games))), "changed",
	                  G_CALLBACK (adjustment_scrolled),
	                  NULL);
	g_signal_connect (G_OBJECT (gtk_scrolled_window_get_vadjustment (GTK_SCROLLED_WINDOW (main_gui.scrolled_window_games))), "value-changed",
	                  G_CALLBACK (adjustment_scrolled),
	                  NULL);

	/* Need to set the notebook page here otherwise it segfault */
	gtk_notebook_set_current_page (GTK_NOTEBOOK (main_gui.screenshot_notebook), gui_prefs.ShowFlyer);
}

static void
set_current_executable (XmameExecutable *new_exec)
{
	
	if (new_exec) {
		GMAMEUI_DEBUG ("Executable changed to %s", new_exec->path);
	}

	current_exec = new_exec;

	/* check if the executable is still valid */
	if (!xmame_executable_is_valid (new_exec)) {
		if (new_exec) {
			gmameui_message (ERROR, NULL, _("%s is not a valid executable"), new_exec->path);
		}

		gtk_widget_set_sensitive (GTK_WIDGET (main_gui.audit_all_games_menu), FALSE);
		gtk_widget_set_sensitive (GTK_WIDGET (main_gui.properties_menu), FALSE);
	} else {
		gamelist_check (new_exec);
		gtk_widget_set_sensitive (GTK_WIDGET (main_gui.audit_all_games_menu), TRUE);
		gtk_widget_set_sensitive (GTK_WIDGET (main_gui.properties_menu), TRUE);
	}
}

/* executable selected from the menu */
static void
on_executable_selected (GtkCheckMenuItem *menuitem,
			gpointer          user_data)
{
	if (menuitem->active) {
		gint index = GPOINTER_TO_INT (user_data);
		XmameExecutable *exec;

		exec = xmame_table_get_by_index (index);

		GMAMEUI_DEBUG ("on executable selected");
		set_current_executable (exec);
	}
}

void
add_exec_menu (void)
{
	gchar *full_name;
	gint i;
	XmameExecutable *exec;
	GtkWidget *temp_menu_item;
	GSList *exec_group_group = NULL;

	/* clean up the older menu */
	if (main_gui.executable_menu) {
		gtk_container_foreach (GTK_CONTAINER (main_gui.executable_menu), (GtkCallback)gtk_widget_destroy, NULL);
		/* need to set the GList to NULL otherwise segfault as soon as I set new executables
		   are all the other really needed ? */
		gtk_menu_item_remove_submenu (GTK_MENU_ITEM (main_gui.executables_title));
		gtk_widget_destroy (main_gui.executable_menu);
	}

	/* recreate the menu */
	main_gui.executable_menu = gtk_menu_new ();
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (main_gui.executables_title), main_gui.executable_menu);

	/* Make sure we have a default executable
	   if we have anything in the table
	*/
	if (!current_exec)
		current_exec = xmame_table_get_by_index (0);

	for (i = 0; ; i++) {
		exec = xmame_table_get_by_index (i);

		if (!exec)
			break;

		full_name = g_strdup_printf ("%s (%s) %s", exec->name, exec->target, exec->version);
		GMAMEUI_DEBUG ("Adding %s", full_name);

		/* add the executable to the menu */
		temp_menu_item = gtk_radio_menu_item_new_with_label (exec_group_group, full_name);
		g_free (full_name);
		exec_group_group = gtk_radio_menu_item_get_group (GTK_RADIO_MENU_ITEM (temp_menu_item));
		gtk_widget_ref (temp_menu_item);

		/* the menu is referenced by the path of the executable */
		g_object_set_data_full (G_OBJECT (MainWindow), exec->path, temp_menu_item,
					(GtkDestroyNotify) gtk_widget_unref);
		gtk_widget_show (temp_menu_item);

		gtk_container_add (GTK_CONTAINER (main_gui.executable_menu), temp_menu_item);

		GMAMEUI_DEBUG ("Comparing %s", exec->path);
	
		if (exec == current_exec) {
			gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (temp_menu_item), TRUE);

			set_current_executable (exec);
		}

		g_signal_connect (G_OBJECT (temp_menu_item), "activate",
				  G_CALLBACK (on_executable_selected),
				  GINT_TO_POINTER (i));

		GMAMEUI_DEBUG ("\t %s exec added checked to %i", exec->path, !strcmp (exec->path, current_exec->path));
		
	}
	
	while (gtk_events_pending ()) gtk_main_iteration ();
}

void
gamelist_popupmenu_show (RomEntry       *rom,
			 GdkEventButton *event)
{
	gpointer popup_menu_ptr;
	GtkWidget *popup_menu;

	GtkWidget *add_to_favorites;
	GtkWidget *remove_from_favorites;

	GtkWidget *play_game;
	GtkWidget *play_network_game;
	GtkWidget *record_game;
	GtkWidget *playback_game;

	popup_menu_ptr = g_object_get_data (G_OBJECT (MainWindow), "popup_gamelist_menu");
	
	if (!popup_menu_ptr)
		return;

	popup_menu = GTK_WIDGET (popup_menu_ptr);

	play_game = g_object_get_data (G_OBJECT (popup_menu), "play_game");
	play_network_game = g_object_get_data (G_OBJECT (popup_menu), "play_network_game");
	record_game = g_object_get_data (G_OBJECT (popup_menu), "record_game");
	playback_game = g_object_get_data (G_OBJECT (popup_menu), "playback_game");

	add_to_favorites = g_object_get_data (G_OBJECT (popup_menu), "add_to_favorites");
	remove_from_favorites = g_object_get_data (G_OBJECT (popup_menu), "remove_from_favorites");

	gtk_widget_set_sensitive (add_to_favorites, !rom->favourite);
	gtk_widget_set_sensitive (remove_from_favorites, rom->favourite);
	
	if (!current_exec) {
		gtk_widget_set_sensitive (play_game, FALSE);
		gtk_widget_set_sensitive (play_network_game, FALSE);
		gtk_widget_set_sensitive (record_game, FALSE);
		gtk_widget_set_sensitive (playback_game, FALSE);

	} else {

		xmame_get_options (current_exec);

		gtk_widget_set_sensitive (play_game, TRUE);
		gtk_widget_set_sensitive (record_game, TRUE);
		gtk_widget_set_sensitive (playback_game, TRUE);

		gtk_widget_set_sensitive (play_network_game,
			 (xmame_has_option (current_exec, "master") || xmame_has_option (current_exec, "slave"))
		);
	}

	gtk_menu_popup (GTK_MENU (popup_menu),NULL,NULL,
			NULL,NULL, event->button, event->time);
}



void
create_filterslist (void)
{
	GtkTreeViewColumn *column;
	GtkCellRenderer *renderer;
	GtkTreeSelection *select;

	GMAMEUI_DEBUG ("DISPLAY FILTERS LIST");

	/* Tree View Creation */
	main_gui.filters_displayed_list = gtk_tree_view_new ();

	/* Column */
	column = gtk_tree_view_column_new ();
	gtk_tree_view_column_set_title  (GTK_TREE_VIEW_COLUMN (column), " ");
	gtk_tree_view_column_set_sort_column_id (column, 0);
	renderer = gtk_cell_renderer_text_new ();
	gtk_tree_view_column_pack_end (GTK_TREE_VIEW_COLUMN (column),
				       renderer,
				       FALSE);
	gtk_tree_view_column_set_attributes (GTK_TREE_VIEW_COLUMN (column), renderer,
					     "text", 0,
					     NULL);
	g_object_set (renderer, "ypad", 0, "yalign", 0.5, NULL);
	renderer = gtk_cell_renderer_pixbuf_new ();
	gtk_tree_view_column_pack_end (GTK_TREE_VIEW_COLUMN (column),
                                             renderer,
                                             FALSE);
	gtk_tree_view_column_add_attribute (GTK_TREE_VIEW_COLUMN (column), renderer,
					    "pixbuf", 2);
	g_object_set (renderer, "xalign", 1.0, "ypad", 0, NULL);
	gtk_tree_view_column_set_sizing (column, GTK_TREE_VIEW_COLUMN_AUTOSIZE);
	gtk_tree_view_append_column (GTK_TREE_VIEW (main_gui.filters_displayed_list), column);

	gtk_container_add (GTK_CONTAINER (main_gui.scrolled_window_filters), main_gui.filters_displayed_list);
	gtk_widget_show_all (main_gui.scrolled_window_filters);
	gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (main_gui.filters_displayed_list), FALSE);

	/* Callback - Row has been selected */
	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (main_gui.filters_displayed_list));
	gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
	g_signal_connect (G_OBJECT (select), "changed",
			  G_CALLBACK (on_filter_row_selected),
			  NULL);
	/* Callback - Row has been collapsed */
	g_signal_connect (G_OBJECT (main_gui.filters_displayed_list), "row-collapsed",
			  G_CALLBACK (on_filter_row_collapsed),
			  NULL);
}


static void
add_filter_to_list (const gchar   *name,
		    simple_filter *folder_filter,
		    GtkTreeIter   *iter)
{
	gchar *filter_key;
	GdkPixbuf *icon_pixbuf;

	icon_pixbuf = get_icon_for_filter (folder_filter);
	gtk_tree_store_append (GTK_TREE_STORE (main_gui.filters_tree_model), iter, NULL);
	gtk_tree_store_set (GTK_TREE_STORE (main_gui.filters_tree_model), iter,
			    0,		name,
			    1,		folder_filter,
			    2,		icon_pixbuf,
			    3,		folder_filter->FolderID,
			    -1);

	filter_key = g_strdup_printf ("Filter%i", folder_filter->FolderID);
	g_object_set_data_full (G_OBJECT (main_gui.filters_tree_model), filter_key, folder_filter, g_free);
	g_free (filter_key);

}

/** FolderID - Unique folder identifier for this filter
 *  type - Column number to get the value to inspect
 *  is - whether we should find the value in type, or the opposite
 *  value - the string value to compare against the value in type
 *  int_value - the integer value to compare against the value in type
 *  update_list - 
 */
simple_filter *create_folder_filter (folder_filters_list FolderID, Columns_type type,
				     gboolean is, gchar *value, gint int_value,
				     gboolean update_list) {
	
	simple_filter *folder_filter = g_new (simple_filter,1);

	folder_filter->type = type;
	folder_filter->is = is;
	folder_filter->value = value;
	folder_filter->int_value = int_value;
	folder_filter->FolderID = FolderID;
	folder_filter->update_list = update_list;

        return folder_filter;
}

void create_sub_folder_filter (simple_filter *folder_filter, gchar *folder_name, GtkTreeIter *sub_iter, GtkTreeIter *iter) {
     	GdkPixbuf *icon_pixbuf;
	gchar *filter_key;
	
	icon_pixbuf = get_icon_for_filter (folder_filter);
	gtk_tree_store_append (GTK_TREE_STORE (main_gui.filters_tree_model), sub_iter, iter);
	gtk_tree_store_set (GTK_TREE_STORE (main_gui.filters_tree_model), sub_iter,
			    0,		folder_name,
			    1,		folder_filter,
			    2,		icon_pixbuf,
			    3,		folder_filter->FolderID,
			    -1);
	filter_key = g_strdup_printf ("Filter%i", folder_filter->FolderID);
	g_object_set_data_full (G_OBJECT (main_gui.filters_tree_model), filter_key, folder_filter, g_free);
	g_free (filter_key);

	filter_key = g_strdup_printf ("Filter%i_image", folder_filter->FolderID);
	g_object_set_data_full (G_OBJECT (main_gui.filters_tree_model), filter_key, icon_pixbuf, g_object_unref);
	g_free (filter_key);
}
	
/* FIXME: Possible memory leak */
void
create_filterslist_content (void)
{
	static char *empty_value = " ";
	static char *dash_value = "-";
	static char *neogeo_value;
	static char *cps1_value = "cps1";
	static char *cps2_value = "cps2";
	static char *trackball_value = "trackball";
	static char *status_good = "good";
	
	GtkTreeIter iter;
	GtkTreeIter sub_iter;
	GtkTreeSelection *select;
	gchar *text;
	GdkPixbuf *icon_pixbuf;
	simple_filter *folder_filter;
	GList *listpointer;
	gint folder_ID_selected;
	gint i;
	gboolean valid, is_root;
	gchar *filter_key;

	GMAMEUI_DEBUG ("POPULATE FILTERS LIST");

	/* Recent versions of MAME use neodrvr */
	if (xmame_compare_raw_version (current_exec, "0.116") >= 0)
		neogeo_value = "neodrvr";
	else
		neogeo_value = "neogeo"; 
	
	/* Free tree model */
	if (main_gui.filters_tree_model) {
		gtk_tree_store_clear (GTK_TREE_STORE (main_gui.filters_tree_model));
		g_object_unref (main_gui.filters_tree_model);
	}

	/* Tree Model Structure */
	main_gui.filters_tree_model = (GtkTreeModel *) gtk_tree_store_new (4,
									   G_TYPE_STRING,
									   G_TYPE_POINTER,
									   GDK_TYPE_PIXBUF,
									   G_TYPE_INT);

	folder_ID_selected = NUMBER_FOLDER;

	/* ALL GAMES */
	folder_filter = create_folder_filter (ALL, DRIVER, FALSE,
					      empty_value, 0, TRUE);
	add_filter_to_list (_("All Games"), folder_filter, &iter);

	/* AVAILABLES */
	folder_filter = create_folder_filter (AVAILABLE, HAS_ROMS, FALSE,
					      NULL, NOT_AVAIL, TRUE);
	add_filter_to_list (_("Available"), folder_filter, &iter);

	current_filter = folder_filter;

	/* BROKEN */
	folder_filter = create_folder_filter (FILTER_INCORRECT, HAS_ROMS, TRUE,
					      NULL, INCORRECT, TRUE);
	add_filter_to_list (_("Incorrect"), folder_filter, &iter);	
	
	/* UNAVAILABLE */
	folder_filter = create_folder_filter (UNAVAILABLE, HAS_ROMS, TRUE,
					      NULL, NOT_AVAIL, TRUE);
	add_filter_to_list (_("Unavailable"), folder_filter, &iter);

	/* NEOGEO */
	folder_filter = create_folder_filter (NEOGEO, DRIVER, TRUE,
					      neogeo_value, 0, TRUE);
	add_filter_to_list (_("Neo-Geo"), folder_filter, &iter);

	/* CPS1 */
	folder_filter = create_folder_filter (CPS1, DRIVER, TRUE,
					      cps1_value, 0, TRUE);
	add_filter_to_list (_("CPS1"), folder_filter, &iter);
	
	/* CPS2 */
	folder_filter = create_folder_filter (CPS2, DRIVER, TRUE,
					      cps2_value, 0, TRUE);
	add_filter_to_list (_("CPS2"), folder_filter, &iter);

	/* IMPERFECT */
	folder_filter = create_folder_filter (IMPERFECT, DRIVER, FALSE,
					      empty_value, 0, FALSE);
	add_filter_to_list (_("Imperfect"), folder_filter, &iter);
	
	/* IMPERFECT - COLORS */
     	folder_filter = create_folder_filter (IMPERFECT_COLORS, COLOR_STATUS, FALSE,
				      status_good, 0, TRUE);
	create_sub_folder_filter (folder_filter, _("Imperfect Colors"), &sub_iter, &iter);

	/* IMPERFECT - SOUND */
     	folder_filter = create_folder_filter (IMPERFECT_SOUND, SOUND_STATUS, FALSE,
				      status_good, 0, TRUE);
	create_sub_folder_filter (folder_filter, _("Imperfect Sound"), &sub_iter, &iter);

	/* IMPERFECT - GRAPHICS */
     	folder_filter = create_folder_filter (IMPERFECT_GRAPHIC, GRAPHIC_STATUS, FALSE,
				      status_good, 0, TRUE);
	create_sub_folder_filter (folder_filter, _("Imperfect Graphics"), &sub_iter, &iter);

	/* SAMPLES */
	folder_filter = create_folder_filter (SAMPLES, HAS_SAMPLES, FALSE,
					      empty_value, 0, TRUE);
	add_filter_to_list (_("Samples"), folder_filter, &iter);
	
	/* MANUFACTURERS */
	folder_filter = create_folder_filter (MANUFACTURERS, DRIVER, FALSE,
					      empty_value, 0, FALSE);
	add_filter_to_list (_("Manufacturer"), folder_filter, &iter);

	/* MANUFACTURERS - LIST */
	for (listpointer = g_list_first (game_list.manufacturers);
	     (listpointer);
	     listpointer = g_list_next (listpointer)) {
		folder_filter = create_folder_filter (folder_ID_selected++, MANU, TRUE,
						      (gchar *) listpointer->data, 0, TRUE);
		create_sub_folder_filter (folder_filter, (gchar *)listpointer->data, &sub_iter, &iter);
	}

	/* YEARS */
	folder_filter = create_folder_filter (YEARS, DRIVER, FALSE,
					      empty_value, 0, FALSE);
	add_filter_to_list (_("Year"), folder_filter, &iter);

	/* YEARS - LIST */
	for (listpointer = g_list_last (game_list.years);
	     (listpointer);
	     listpointer = g_list_previous (listpointer)) {
		if (!strcmp (listpointer->data, "-"))
			text = _("<unknown>");
		else
			text = (gchar* ) listpointer->data;

	     	folder_filter = create_folder_filter (folder_ID_selected++, YEAR, TRUE,
					      (gchar *) listpointer->data, 0, TRUE);
		create_sub_folder_filter (folder_filter, text, &sub_iter, &iter);
	}


	/* WORKING */
	folder_filter = create_folder_filter (WORKING, STATUS, TRUE,
				      NULL, CORRECT, TRUE);
	add_filter_to_list (_("Working"), folder_filter, &iter);

	/* NONWORKING */
     	folder_filter = create_folder_filter (NONWORKING, STATUS, FALSE,
				      NULL, CORRECT, TRUE);
	add_filter_to_list (_("Non-Working"), folder_filter, &iter);
	
	/* CUSTOM */
     	folder_filter = create_folder_filter (CUSTOMS, DRIVER, FALSE,
				      empty_value, 0, FALSE);
	add_filter_to_list (_("Custom"), folder_filter, &iter);

	/* CUSTOM - FAVORITES */
     	folder_filter = create_folder_filter (FAVORITES, FAVORITE, TRUE,
				      NULL, 0, TRUE);
	create_sub_folder_filter (folder_filter, _("Favorites"), &sub_iter, &iter);

	/* CUSTOM - PLAYED */
     	folder_filter = create_folder_filter (PLAYED, TIMESPLAYED, FALSE,
				      NULL, 0, TRUE);
	create_sub_folder_filter (folder_filter, _("Played"), &sub_iter, &iter);

	/* ORIGINALS */
     	folder_filter = create_folder_filter (ORIGINALS, CLONE, TRUE,
				      dash_value, 0, TRUE);
	add_filter_to_list (_("Originals"), folder_filter, &iter);

	/* CLONES */
     	folder_filter = create_folder_filter (CLONES, CLONE, FALSE,
				      dash_value, 0, TRUE);
	add_filter_to_list (_("Clones"), folder_filter, &iter);

	/* RASTERS */
	folder_filter = create_folder_filter (RASTERS, VECTOR, FALSE,
				      NULL, 0, TRUE);
	add_filter_to_list (_("Raster"), folder_filter, &iter);

	/* VECTORS */
     	folder_filter = create_folder_filter (VECTORS, VECTOR, TRUE,
				      NULL, 0, TRUE);
	add_filter_to_list (_("Vector"), folder_filter, &iter);

	/* TRACKBALL */
     	folder_filter = create_folder_filter (TRACKBALL, CONTROL, TRUE,
				      trackball_value, 0, TRUE);
	add_filter_to_list (_("Trackball"), folder_filter, &iter);

	/* STEREO */
     	folder_filter = create_folder_filter (STEREO, CHANNELS, TRUE,
				      NULL, 2, TRUE);
	add_filter_to_list (_("Stereo"), folder_filter, &iter);

	/* BIOS */
     	folder_filter = create_folder_filter (FILTER_BIOS, IS_BIOS, TRUE,
				      NULL, 0, TRUE);
	add_filter_to_list (_("BIOS"), folder_filter, &iter);
	
	/* DRIVERS */
	folder_filter = create_folder_filter (DRIVERS, DRIVER, FALSE,
			      empty_value, 0, FALSE);
	add_filter_to_list (_("Driver"), folder_filter, &iter);

	/* DRIVERS - LIST*/
	for (listpointer = g_list_first (game_list.drivers);
	     (listpointer);
	     listpointer = g_list_next (listpointer)) {
		if (!strcmp (listpointer->data, "-"))
			text = _("<unknown>");
		else
			text = (gchar* ) listpointer->data;

		folder_filter = create_folder_filter (folder_ID_selected++, DRIVER, TRUE,
				      (gchar *)listpointer->data, 0, TRUE);
		create_sub_folder_filter (folder_filter, text, &sub_iter, &iter);
	}

	/* CATEGORY - Requires catver.ini */
/*	if (gui_prefs.catver_avail) {
		GMAMEUI_DEBUG ("catver_avail is set");*/
		folder_filter = create_folder_filter (CATEGORIES, CATEGORY, FALSE,
				      empty_value, 0, FALSE);
		add_filter_to_list (_("Category"), folder_filter, &iter);

		/* CATEGORY - LIST */
		for (listpointer = g_list_first (game_list.categories);
		     (listpointer);
		     listpointer = g_list_next (listpointer)) {

			folder_filter = create_folder_filter (folder_ID_selected++, CATEGORY, TRUE,
					      (gchar *)listpointer->data, 0, TRUE);
			create_sub_folder_filter (folder_filter, (gchar *) listpointer->data, &sub_iter, &iter);
		}


		/* VERSION - Requires catver.ini  */
		folder_filter = create_folder_filter (VERSIONS, MAMEVER, FALSE,
				      empty_value, 0, FALSE);
		add_filter_to_list (_("Version"), folder_filter, &iter);

		/* VERSION - LIST */
		for (listpointer = g_list_first (game_list.versions);
		     (listpointer);
		     listpointer = g_list_next (listpointer)) {
			folder_filter = create_folder_filter (folder_ID_selected++, MAMEVER, TRUE,
					      (gchar *)listpointer->data, 0, TRUE);
			create_sub_folder_filter (folder_filter, (gchar *) listpointer->data, &sub_iter, &iter);		     
		}
/*	}*/

	/* Link Tree Model with Tree View + Sort */
	gtk_tree_view_set_model (GTK_TREE_VIEW (main_gui.filters_displayed_list), GTK_TREE_MODEL (main_gui.filters_tree_model));
	gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE (main_gui.filters_tree_model), 3, GTK_SORT_ASCENDING);


	/* Select the default row */
	valid = gtk_tree_model_get_iter_first (GTK_TREE_MODEL (main_gui.filters_tree_model), &iter);
	gtk_tree_model_get (GTK_TREE_MODEL (main_gui.filters_tree_model), &iter, 1, &folder_filter, -1);
	is_root = TRUE;
	i = 0;
	while ((folder_filter->FolderID != gui_prefs.FolderID) && (valid)) {
		if (gtk_tree_model_iter_has_child (GTK_TREE_MODEL (main_gui.filters_tree_model), &iter)) {
			if (gtk_tree_model_iter_children (GTK_TREE_MODEL (main_gui.filters_tree_model), &sub_iter, &iter)) {
				gtk_tree_model_get (GTK_TREE_MODEL (main_gui.filters_tree_model), &sub_iter, 1, &folder_filter, -1);
				is_root = FALSE;
				i++;
				while ((folder_filter->FolderID != gui_prefs.FolderID) && (gtk_tree_model_iter_next (GTK_TREE_MODEL (main_gui.filters_tree_model), &sub_iter)) ) {
					gtk_tree_model_get (GTK_TREE_MODEL (main_gui.filters_tree_model), &sub_iter, 1, &folder_filter, -1);
					i++;
				}
			}
		}

		if (folder_filter->FolderID != gui_prefs.FolderID) {
			valid = gtk_tree_model_iter_next (GTK_TREE_MODEL (main_gui.filters_tree_model), &iter);
			if (valid) {
				gtk_tree_model_get (GTK_TREE_MODEL (main_gui.filters_tree_model), &iter, 1, &folder_filter, -1);
				is_root = TRUE;
				i++;
			}
		}
	}
	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (main_gui.filters_displayed_list));
	if (folder_filter->FolderID != gui_prefs.FolderID) {
		valid = gtk_tree_model_get_iter_first (GTK_TREE_MODEL (main_gui.filters_tree_model), &iter);
		is_root = TRUE;
	}

	GtkTreePath *tree_path;
	if (is_root) {
		/*GtkTreePath *tree_path;*/
		g_signal_handlers_block_by_func (G_OBJECT (select), (gpointer)on_filter_row_selected, NULL);
		tree_path = gtk_tree_model_get_path (GTK_TREE_MODEL (main_gui.filters_tree_model), &iter);
		gtk_tree_view_set_cursor (GTK_TREE_VIEW (main_gui.filters_displayed_list),
					  tree_path,
					  NULL, FALSE);
		gtk_tree_path_free (tree_path);
		g_signal_handlers_unblock_by_func (G_OBJECT (select), (gpointer)on_filter_row_selected, NULL);
		gtk_tree_model_get (main_gui.filters_tree_model, &iter, 1, &folder_filter, -1);
		current_filter = folder_filter;

		icon_pixbuf = gmameui_get_icon_from_stock ("gmameui-folder-open");

		gtk_tree_store_set (GTK_TREE_STORE (main_gui.filters_tree_model), &iter,
				    2,		icon_pixbuf,
				    -1);
		/* Scroll to selection */
		tree_path = gtk_tree_model_get_path (GTK_TREE_MODEL (main_gui.filters_tree_model), &iter);
	} else {
		/*GtkTreePath *tree_path;*/
		tree_path = gtk_tree_model_get_path (GTK_TREE_MODEL (main_gui.filters_tree_model), &iter);
		valid = gtk_tree_view_expand_row (GTK_TREE_VIEW (main_gui.filters_displayed_list),
						  tree_path,
						  TRUE);
		gtk_tree_path_free (tree_path);

		g_signal_handlers_block_by_func (G_OBJECT (select), (gpointer)on_filter_row_selected, NULL);
		tree_path = gtk_tree_model_get_path (GTK_TREE_MODEL (main_gui.filters_tree_model), &sub_iter);
		gtk_tree_view_set_cursor (GTK_TREE_VIEW (main_gui.filters_displayed_list),
					  tree_path,
					  NULL, FALSE);
		gtk_tree_path_free (tree_path);
		g_signal_handlers_unblock_by_func (G_OBJECT (select), (gpointer)on_filter_row_selected, NULL);
		gtk_tree_model_get (main_gui.filters_tree_model, &sub_iter, 1, &folder_filter, -1);
		current_filter = folder_filter;
		icon_pixbuf = gmameui_get_icon_from_stock ("gmameui-folder-open");
		gtk_tree_store_set (GTK_TREE_STORE (main_gui.filters_tree_model), &sub_iter,
				    2,		icon_pixbuf,
				    -1);
		/* Scroll to selection */
		tree_path = gtk_tree_model_get_path (GTK_TREE_MODEL (main_gui.filters_tree_model), &sub_iter);
	}
	gtk_tree_view_scroll_to_cell (GTK_TREE_VIEW (main_gui.filters_displayed_list), tree_path, NULL, TRUE, 0.5, 0);
	gtk_tree_path_free (tree_path);
}

void
hide_filters (void)
{
	gui_prefs.ShowFolderList = 0;
	gui_prefs.Splitters[0] = main_gui.scrolled_window_filters->allocation.width;
	gtk_paned_set_position (main_gui.hpanedLeft, 0);

	gtk_widget_hide (GTK_WIDGET (main_gui.scrolled_window_filters));
}

void
show_filters (void)
{
	gui_prefs.ShowFolderList = 1;
	gtk_paned_set_position (main_gui.hpanedLeft, gui_prefs.Splitters[0]);

	gtk_widget_show (GTK_WIDGET (main_gui.scrolled_window_filters));
}

void
hide_snaps (void)
{
	gui_prefs.ShowScreenShot = 0;
	gui_prefs.Splitters[1] = main_gui.scrolled_window_games->allocation.width;
	gtk_paned_set_position (main_gui.hpanedRight, -1);

	gtk_widget_hide (GTK_WIDGET (main_gui.screenshot_hist_frame));
}

void
show_snaps (void)
{
	gui_prefs.ShowScreenShot = 1;
	gtk_paned_set_position (main_gui.hpanedRight, gui_prefs.Splitters[1]);

	gtk_widget_show (GTK_WIDGET (main_gui.screenshot_hist_frame));
}


void
hide_snaps_tab (void)
{
	gui_prefs.ShowScreenShotTab = 0;
	gtk_widget_hide (GTK_WIDGET (main_gui.screenshot_notebook));
	gtk_widget_show (GTK_WIDGET (main_gui.screenshot_event_box));
	update_screenshot_panel (gui_prefs.current_game);
}


void
show_snaps_tab (void)
{
	gui_prefs.ShowScreenShotTab = 1;
	gtk_widget_hide (GTK_WIDGET (main_gui.screenshot_event_box));
	gtk_widget_show (GTK_WIDGET (main_gui.screenshot_notebook));
	gtk_notebook_set_current_page (GTK_NOTEBOOK (main_gui.screenshot_notebook), gui_prefs.ShowFlyer);
	update_screenshot_panel (gui_prefs.current_game);
}


void
hide_toolbar (void)
{
	gui_prefs.ShowToolBar = 0;
	gtk_widget_hide (GTK_WIDGET (main_gui.toolbar));
}


void
show_toolbar (void)
{
	gui_prefs.ShowToolBar = 1;
	gtk_widget_show (GTK_WIDGET (main_gui.toolbar));
}


void
hide_status_bar (void)
{
	gui_prefs.ShowStatusBar = 0;
	gtk_widget_hide (GTK_WIDGET (main_gui.tri_status_bar));
}


void
show_status_bar (void)
{
	gui_prefs.ShowStatusBar = 1;
	gtk_widget_show (GTK_WIDGET (main_gui.tri_status_bar));
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
	GError **error = NULL;

	if (!rom)
		return NULL;

	filename2 = g_strdup_printf ("%s" G_DIR_SEPARATOR_S "%s.ico", gui_prefs.IconDirectory, rom->romname);
	pixbuf = gdk_pixbuf_new_from_file (filename2, error);
	g_free (filename2);

		/* no picture found try parent game if any*/
	if ((pixbuf == NULL) && strcmp (rom->cloneof, "-")) {
		filename2 = g_strdup_printf ("%s" G_DIR_SEPARATOR_S "%s.ico", gui_prefs.IconDirectory, rom->cloneof);
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
	
	return scaled_pixbuf;
}

GdkPixbuf *
get_icon_for_filter (simple_filter *current_filter)
{
	GdkPixbuf *icon_pixbuf = NULL;

	switch (current_filter->FolderID) {
	case ALL:
	case ORIGINALS:
	case CLONES:
	case RASTERS:
	case VECTORS:
	case TRACKBALL:
	case DRIVERS:
	case CUSTOMS:
	case NEOGEO:
	case CPS1:
	case CPS2:
	case FILTER_BIOS:
	case CATEGORIES:
	case VERSIONS:
		icon_pixbuf = gmameui_get_icon_from_stock ("gmameui-folder");
		break;
	case AVAILABLE:
		icon_pixbuf = gmameui_get_icon_from_stock ("gmameui-folder-available");
		break;
	case UNAVAILABLE:
		icon_pixbuf = gmameui_get_icon_from_stock ("gmameui-folder-unavailable");
		break;
	case WORKING:
		icon_pixbuf = gmameui_get_icon_from_stock ("gmameui-folder-working");
		break;
	case NONWORKING:
		icon_pixbuf = gmameui_get_icon_from_stock ("gmameui-folder-not-working");
		break;
	case FILTER_INCORRECT:
		icon_pixbuf = gmameui_get_icon_from_stock ("gmameui-folder-incorrect");
	    	break;
	case PLAYED:
		icon_pixbuf = gmameui_get_icon_from_stock ("gmameui-folder-played");
		break;
	case FAVORITES:
		icon_pixbuf = gmameui_get_icon_from_stock ("gmameui-folder-favorite");
		break;
	case YEARS:
		icon_pixbuf = gmameui_get_icon_from_stock ("gmameui-folder-date");
		break;
	case STEREO:
		icon_pixbuf = gmameui_get_icon_from_stock ("gmameui-folder-sound");
		break;
	case MANUFACTURERS:
		icon_pixbuf = gmameui_get_icon_from_stock ("gmameui-folder-manufacturer");
		break;
	default:
		switch (current_filter->type) {
		case YEAR:
			icon_pixbuf = gmameui_get_icon_from_stock ("gmameui-folder-date");
			break;
		case MANU:
			icon_pixbuf = gmameui_get_icon_from_stock ("gmameui-folder-manufacturer");
			break;
		case CATEGORY:
		case MAMEVER:
		default:
			icon_pixbuf = gmameui_get_icon_from_stock ("gmameui-folder");
			break;
		}
		break;
	}

	return icon_pixbuf;
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
		if (Status_Icons [NOT_AVAIL] == NULL)
			Status_Icons [NOT_AVAIL] = gmameui_get_icon_from_stock ("gmameui-rom-unavailable");
	}
}

RomEntry *
gamelist_get_selected_game (void)
{
	RomEntry *game_data;
	GtkTreeIter iter;
	GtkTreeModel *model;
	GtkTreeSelection *select;

	game_data = NULL;

	if (!main_gui.displayed_list)
		return NULL;

	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (main_gui.displayed_list));

	if (gtk_tree_selection_get_selected (select, &model, &iter))
	{
		gtk_tree_model_get (model, &iter, ROMENTRY, &game_data, -1);
	}

	return game_data;
}


void set_list_sortable_column ()
{
	if ((gui_prefs.current_mode == DETAILS) || (gui_prefs.current_mode == DETAILS_TREE)) {
		GMAMEUI_DEBUG("Sorting - using sort order %d", gui_prefs.SortColumn);
		if (gui_prefs.SortReverse)
			gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE (main_gui.tree_model), gui_prefs.SortColumn, GTK_SORT_DESCENDING);
		else
			gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE (main_gui.tree_model), gui_prefs.SortColumn, GTK_SORT_ASCENDING);
	} else {
		g_signal_handlers_block_by_func (G_OBJECT (main_gui.tree_model), (gpointer)on_displayed_list_sort_column_changed, NULL);
		/* FIXME we sometimes have here a gtk warning why????? */
		gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE (main_gui.tree_model), GAMENAME, GTK_SORT_ASCENDING);
		g_signal_handlers_unblock_by_func (G_OBJECT (main_gui.tree_model), (gpointer)on_displayed_list_sort_column_changed, NULL);
	}	/* Select the correct row */
}

void
create_gamelist_content (void)
{
	GList *listpointer;
	RomEntry *tmprom;
	gchar *my_romname_root = NULL;
	gchar *my_hassamples;
	gchar *my_control;
	GdkColor *my_txtcolor;
	GtkTreeSelection *select;
	GtkTreeIter iter;
	GtkTreeIter iter_root;
	GtkTreeIter iter_child;
	GtkTreeModel *store;
	gboolean tree_store;   /* If the model is a tree or a list */
	gboolean is_root;
	guint i = 0;
	gint j = 0;
	gboolean valid;
	gchar *message;
	RomEntry *selected_game;

	GMAMEUI_DEBUG ("POPULATE GAME LIST");
GTimer *timer = g_timer_new ();
g_timer_start (timer);
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
	if ( (gui_prefs.current_mode == LIST_TREE) || (gui_prefs.current_mode == DETAILS_TREE))
		tree_store = TRUE;
	else
		tree_store = FALSE;

	/* Get the status icon */
	get_status_icons ();

	/* Create a model. */
	if (tree_store)
		store = (GtkTreeModel *) gtk_tree_store_new (NUMBER_COLUMN_TOTAL,
							     G_TYPE_STRING,
							     G_TYPE_STRING,
							     G_TYPE_STRING,
							     G_TYPE_STRING,
							     G_TYPE_STRING,
							     G_TYPE_STRING,
							     G_TYPE_INT,
							     G_TYPE_STRING,
							     G_TYPE_STRING,
							     G_TYPE_STRING,
							     G_TYPE_STRING,
							     G_TYPE_STRING,
							     G_TYPE_STRING,
							     G_TYPE_STRING,
							     G_TYPE_STRING,
							     G_TYPE_STRING,
							     G_TYPE_STRING,
							     G_TYPE_INT,
							     G_TYPE_INT,
							     G_TYPE_STRING,
							     G_TYPE_STRING,
							     G_TYPE_STRING,
							     G_TYPE_STRING,
							     G_TYPE_STRING,
							     G_TYPE_STRING,
							     G_TYPE_STRING,
							     G_TYPE_STRING,
							     G_TYPE_STRING,
							     G_TYPE_STRING,
							     G_TYPE_STRING,
							     G_TYPE_INT,
							     G_TYPE_STRING,
							     G_TYPE_POINTER,     /* Rom Entry */
							     GDK_TYPE_COLOR,     /* Text Color */
							     GDK_TYPE_PIXBUF);   /* Pixbuf */
	else
		store = (GtkTreeModel *) gtk_list_store_new (NUMBER_COLUMN_TOTAL,
							     G_TYPE_STRING,
							     G_TYPE_STRING,
							     G_TYPE_STRING,
							     G_TYPE_STRING,
							     G_TYPE_STRING,
							     G_TYPE_STRING,
							     G_TYPE_INT,
							     G_TYPE_STRING,
							     G_TYPE_STRING,
							     G_TYPE_STRING,
							     G_TYPE_STRING,
							     G_TYPE_STRING,
							     G_TYPE_STRING,
							     G_TYPE_STRING,
							     G_TYPE_STRING,
							     G_TYPE_STRING,
							     G_TYPE_STRING,
							     G_TYPE_INT,
							     G_TYPE_INT,
							     G_TYPE_STRING,
							     G_TYPE_STRING,
							     G_TYPE_STRING,
							     G_TYPE_STRING,
							     G_TYPE_STRING,
							     G_TYPE_STRING,
							     G_TYPE_STRING,
							     G_TYPE_STRING,
							     G_TYPE_STRING,
							     G_TYPE_STRING,
							     G_TYPE_STRING,
							     G_TYPE_INT,
							     G_TYPE_STRING,
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
				my_hassamples = tmprom->has_samples ? _("Yes") : _("No");

			/* Control */
			if (!strcmp (tmprom->control, "trackball"))
				my_control = _("Yes");
			else
				my_control = _("No");
			
			/* Clone Color + Pixbuf width */
			if (strcmp (tmprom->cloneof, "-")) {  /* Clone */
				my_txtcolor = &gui_prefs.clone_color;
			} else { /* Original */
				my_txtcolor = NULL; /* Black */
			}
			/* Pixbuf */
			if (tmprom->icon_pixbuf)
				pixbuf = tmprom->icon_pixbuf;
			else
				pixbuf = Status_Icons [tmprom->has_roms];
			if (!pixbuf)
				pixbuf = Status_Icons [CORRECT];

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
						    HAS_ROMS,     tmprom->has_roms ? _("Yes") : _("No"),
						    HAS_SAMPLES,  my_hassamples,
						    ROMNAME,      tmprom->romname,
						    VECTOR,       tmprom->vector ? _("Vector") : _("Raster"),
						    CONTROL,      my_control,
						    TIMESPLAYED,  tmprom->timesplayed,
						    MANU,         tmprom->manu,
						    YEAR,         tmprom->year,
						    CLONE,        tmprom->cloneof,
						    DRIVER,       tmprom->driver,
						    STATUS,       tmprom->has_roms ? _("Available") : _("Not Available"),
						    ROMOF,        tmprom->romof,
						    DRIVERSTATUS, tmprom->status ? _("Working") : _("Not Working"),
						    COLOR_STATUS, tmprom->driver_status_color,
						    SOUND_STATUS, tmprom->driver_status_sound,
						    GRAPHIC_STATUS, tmprom->driver_status_graphic,
						    NUMPLAYERS,   tmprom->num_players,
						    NUMBUTTONS,   tmprom->num_buttons,
						    CPU1,         tmprom->cpu_info[0].name,
						    CPU2,         tmprom->cpu_info[1].name,
						    CPU3,         tmprom->cpu_info[2].name,
						    CPU4,         tmprom->cpu_info[3].name,
						    SOUND1,       tmprom->sound_info[0].name,
						    SOUND2,       tmprom->sound_info[1].name,
						    SOUND3,       tmprom->sound_info[2].name,
						    SOUND4,       tmprom->sound_info[3].name,
						    MAMEVER,      tmprom->mame_ver_added,
						    CATEGORY,     tmprom->category,
						    FAVORITE,     tmprom->favourite ? _("Yes") : _("No"),
						    CHANNELS,     tmprom->channels,
						    IS_BIOS,      tmprom->is_bios ? _("Yes") : _("No"),
						    ROMENTRY,     tmprom,                 /* rom entry */
						    TEXTCOLOR,    my_txtcolor,            /* text color */
						    PIXBUF,       pixbuf,                 /* pixbuf */
						    -1);
				if (is_root)
					iter_root = iter;
			} else {
				gtk_list_store_append (GTK_LIST_STORE (store), &iter);  /* Acquire an iterator */
				gtk_list_store_set (GTK_LIST_STORE (store), &iter,
						    GAMENAME,     tmprom->name_in_list,
						    HAS_ROMS,     tmprom->has_roms ? _("Yes") : _("No"),
						    HAS_SAMPLES,  my_hassamples,
						    ROMNAME,      tmprom->romname,
						    VECTOR,       tmprom->vector ? _("Vector") : _("Raster"),
						    CONTROL,      my_control,
						    TIMESPLAYED,  tmprom->timesplayed,
						    MANU,         tmprom->manu,
						    YEAR,         tmprom->year,
						    CLONE,        tmprom->cloneof,
						    DRIVER,       tmprom->driver,
						    STATUS,       tmprom->has_roms ? _("Available") : _("Not Available"),
						    ROMOF,        tmprom->romof,
						    DRIVERSTATUS, tmprom->status ? _("Working") : _("Not Working"),
						    COLOR_STATUS, tmprom->driver_status_color,
						    SOUND_STATUS, tmprom->driver_status_sound,
						    GRAPHIC_STATUS, tmprom->driver_status_graphic,
						    NUMPLAYERS,   tmprom->num_players,
						    NUMBUTTONS,   tmprom->num_buttons,
						    CPU1,         tmprom->cpu_info[0].name,
						    CPU2,         tmprom->cpu_info[1].name,
						    CPU3,         tmprom->cpu_info[2].name,
						    CPU4,         tmprom->cpu_info[3].name,
						    SOUND1,       tmprom->sound_info[0].name,
						    SOUND2,       tmprom->sound_info[1].name,
						    SOUND3,       tmprom->sound_info[2].name,
						    SOUND4,       tmprom->sound_info[3].name,
						    MAMEVER,      tmprom->mame_ver_added,
						    CATEGORY,     tmprom->category,
						    FAVORITE,     tmprom->favourite ? _("Yes") : _("No"),
						    CHANNELS,     tmprom->channels,
						    IS_BIOS,      tmprom->is_bios ? _("Yes") : _("No"),
						    ROMENTRY,     tmprom,                 /* rom entry */
						    TEXTCOLOR,    my_txtcolor,            /* text color */
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
	if (main_gui.tree_model == NULL) {
		g_signal_connect (G_OBJECT (main_gui.tree_model), "sort-column-changed",
				  G_CALLBACK (on_displayed_list_sort_column_changed),
				  NULL);
	}

	/* Update the corresponding tree view */
	if (main_gui.displayed_list) {
		/* Link the view with the model */
		gtk_tree_view_set_model (GTK_TREE_VIEW (main_gui.displayed_list), GTK_TREE_MODEL (main_gui.tree_model));
		/* Get the selection */
		select = gtk_tree_view_get_selection (GTK_TREE_VIEW (main_gui.displayed_list));

		/* Sort the list */
		set_list_sortable_column ();

		/* Find the selected game in the gamelist, and scroll to it, opening any expanders */
		if (visible_games > 0) {
			RomEntry *curr_rom;

			valid = gtk_tree_model_get_iter_first (GTK_TREE_MODEL (main_gui.tree_model), &iter);
			gtk_tree_model_get (GTK_TREE_MODEL (main_gui.tree_model), &iter, ROMENTRY, &curr_rom, -1);
			is_root = TRUE;
			i = 0;
			while ( (i < visible_games) && (curr_rom != selected_game) && (valid)) {

				if (gtk_tree_model_iter_has_child (GTK_TREE_MODEL (main_gui.tree_model), &iter)) {

					if (gtk_tree_model_iter_children (GTK_TREE_MODEL (main_gui.tree_model), &iter_child, &iter)) {
						gtk_tree_model_get (GTK_TREE_MODEL (main_gui.tree_model), &iter_child, ROMENTRY, &curr_rom, -1);
						is_root = FALSE;
						i++;

						while ((i < visible_games)
						       && (curr_rom != selected_game)
						       && (gtk_tree_model_iter_next (GTK_TREE_MODEL (main_gui.tree_model), &iter_child))) {
							gtk_tree_model_get (GTK_TREE_MODEL (main_gui.tree_model), &iter_child, ROMENTRY, &curr_rom, -1);
							i++;
						}
					}
				}

				if ((i < visible_games) && (curr_rom != selected_game)) {
					valid = gtk_tree_model_iter_next (GTK_TREE_MODEL (main_gui.tree_model), &iter);
					if (valid) {
						gtk_tree_model_get (GTK_TREE_MODEL (main_gui.tree_model), &iter, ROMENTRY, &curr_rom, -1);
						is_root = TRUE;
						i++;
					}
				}
			}

			select = gtk_tree_view_get_selection (GTK_TREE_VIEW (main_gui.displayed_list));
			if (curr_rom != selected_game) {
				valid = gtk_tree_model_get_iter_first (GTK_TREE_MODEL (main_gui.tree_model), &iter);
				is_root = TRUE;
			}
	
			GtkTreePath *tree_path = gtk_tree_model_get_path (GTK_TREE_MODEL (main_gui.tree_model), &iter);
			if (is_root) {
				gtk_tree_view_set_cursor (GTK_TREE_VIEW (main_gui.displayed_list),
							  tree_path,
							  NULL, FALSE);
			} else {
				valid=gtk_tree_view_expand_row (GTK_TREE_VIEW (main_gui.displayed_list),
								tree_path,
								TRUE);
				gtk_tree_path_free (tree_path);
				tree_path = gtk_tree_model_get_path (GTK_TREE_MODEL (main_gui.tree_model), &iter_child);
				gtk_tree_view_set_cursor (GTK_TREE_VIEW (main_gui.displayed_list),
							  tree_path,
							  NULL, FALSE);
			}
			/* Scroll to selection */
			gtk_tree_view_scroll_to_cell (GTK_TREE_VIEW (main_gui.displayed_list), tree_path, NULL, TRUE, 0.5, 0);
			gtk_tree_path_free (tree_path);
			
			
			
		}
		/* Header clickable. */
		if ( (gui_prefs.current_mode == DETAILS) || (gui_prefs.current_mode == DETAILS_TREE))
			gtk_tree_view_set_headers_clickable (GTK_TREE_VIEW (main_gui.displayed_list),
							     TRUE);
		else
			gtk_tree_view_set_headers_clickable (GTK_TREE_VIEW (main_gui.displayed_list),
							     FALSE);
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
	gint i, j, k;
	GtkTreeViewColumn *column;
	GtkTreeViewColumn *base_column;
	GtkCellRenderer *renderer;
	GtkTreeSelection *select;
	GList *col_list = NULL;
	GList *listpointer = NULL;
	static gboolean first_run = TRUE;

	GMAMEUI_DEBUG ("DISPLAY GAME LIST");

	/* We Create the TreeView only if it is NULL (this will occur only once) */
	if (main_gui.displayed_list == NULL) {
		main_gui.displayed_list = gtk_tree_view_new ();
		for (i = 0; i < NUMBER_COLUMN; i++) {
			if (i == GAMENAME) {
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

		/* Callback - Column Order has changed */
		g_signal_connect (G_OBJECT (main_gui.displayed_list), "columns-changed",
				G_CALLBACK (on_columns_changed),
				NULL);
		/* Callback - Click on the list */
		g_signal_connect (G_OBJECT (main_gui.displayed_list), "button-press-event",
				G_CALLBACK (on_list_clicked),
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
		
		if ( (list_mode == DETAILS) || (list_mode == DETAILS_TREE))
			gtk_tree_view_set_headers_clickable (GTK_TREE_VIEW (main_gui.displayed_list),
							     TRUE);
		else
			gtk_tree_view_set_headers_clickable (GTK_TREE_VIEW (main_gui.displayed_list),
							     FALSE);
	}

	/* Show or hide Header */
	if ( (list_mode == DETAILS) || (list_mode == DETAILS_TREE))
		gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (main_gui.displayed_list), TRUE);
	else
		gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (main_gui.displayed_list), FALSE);

	/* Reorder the columns */
	base_column=NULL;
	g_signal_handlers_block_by_func (G_OBJECT (main_gui.displayed_list), (gpointer)on_columns_changed, NULL);
	
	col_list = gtk_tree_view_get_columns (GTK_TREE_VIEW (main_gui.displayed_list));
	
	for (i = 0; i < NUMBER_COLUMN; i++) {
		for (j = 0; gui_prefs.ColumnOrder[j] != i; j++)
			{}
		for (listpointer = g_list_first (col_list), k = 0;
		     ((listpointer) && (k < NUMBER_COLUMN) && (j != gtk_tree_view_column_get_sort_column_id (listpointer->data)) );
		     listpointer = g_list_next (listpointer), k++)
			{}
		column = listpointer->data;
		gtk_tree_view_move_column_after (GTK_TREE_VIEW (main_gui.displayed_list),
						 GTK_TREE_VIEW_COLUMN (column),
						 GTK_TREE_VIEW_COLUMN (base_column));
		base_column = column;
	}
	g_signal_handlers_unblock_by_func (G_OBJECT (main_gui.displayed_list), (gpointer)on_columns_changed, NULL);
	g_list_free (col_list);
	/* Update the columns */
	/* FIXME When switching from LIST mode to DETAILS, it puts a mess in the size of the
	GAMENAME column even if I block the callback?????? */
	g_signal_handlers_block_by_func (G_OBJECT (main_gui.displayed_list), (gpointer)on_displayed_list_resize_column, NULL);
	for (i = 0; i < NUMBER_COLUMN; i++) {
		column = gtk_tree_view_get_column (GTK_TREE_VIEW (main_gui.displayed_list), gui_prefs.ColumnOrder[i]);

		/* Font */
		j = 0;
		col_list = gtk_tree_view_column_get_cell_renderers (GTK_TREE_VIEW_COLUMN (column));
		for (listpointer = g_list_first (col_list); (listpointer); listpointer = g_list_next (listpointer)) {
			if (j == 0) {		/* Text */
				g_object_set (G_OBJECT (listpointer->data), "font", gui_prefs.ListFont, NULL);
				j = 1;
			}
		}
		g_list_free (col_list);
		/* Columns visible, Column size,... */
		if ( (list_mode == DETAILS) || (list_mode == DETAILS_TREE)) {	/* COLUMNS */
			if (gui_prefs.ColumnShown[i]==FALSE) {
				gtk_tree_view_column_set_visible (column, FALSE);
			} else {
				gtk_tree_view_column_set_visible (column, TRUE);
				if (gui_prefs.ColumnWidth[i] == 0) {
					gtk_tree_view_column_set_sizing (column, GTK_TREE_VIEW_COLUMN_AUTOSIZE);
				} else {
					gtk_tree_view_column_set_sizing (column, GTK_TREE_VIEW_COLUMN_FIXED);
					gtk_tree_view_column_set_fixed_width (column, gui_prefs.ColumnWidth[i]);
				}
				gtk_tree_view_column_set_resizable (column, TRUE);
				gtk_tree_view_column_set_reorderable (column, TRUE);
			}
		} else {	/* NO COLUMNS */
			if (i == GAMENAME) {
				gtk_tree_view_column_set_visible (column, TRUE);
				gtk_tree_view_column_set_sizing (column, GTK_TREE_VIEW_COLUMN_AUTOSIZE);
				gtk_tree_view_column_set_resizable (column, FALSE);
				gtk_tree_view_column_set_reorderable (column, FALSE);
			} else {
				gtk_tree_view_column_set_visible (column, FALSE);
			}
		}
		/* Reordable is disable for the time beeing because it make conflics with
		  'column popup menu' and 'sort on click header' */
		gtk_tree_view_column_set_reorderable (GTK_TREE_VIEW_COLUMN (column), FALSE);
	}
	g_signal_handlers_unblock_by_func (G_OBJECT (main_gui.displayed_list), (gpointer)on_displayed_list_resize_column, NULL);

	first_run = FALSE;
	dirty_icon_cache = FALSE;

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
		/* update menus */
		gtk_widget_set_sensitive (GTK_WIDGET (main_gui.play_menu), TRUE);
		gtk_widget_set_sensitive (GTK_WIDGET (main_gui.properties_menu), TRUE);
		gtk_widget_set_sensitive (GTK_WIDGET (main_gui.add_to_favorites), !rom->favourite);
		gtk_widget_set_sensitive (GTK_WIDGET (main_gui.remove_from_favorites), rom->favourite);

		/* update statusbar */
		set_status_bar (rom_entry_get_list_name (rom),
				rom_status_string_value [rom->has_roms]);

GMAMEUI_DEBUG ("ROM Selected: %s", rom->romname);
GMAMEUI_DEBUG ("   Emulation status: %s", rom->driver_status_emulation);
GMAMEUI_DEBUG ("   Color status: %s", rom->driver_status_color);
GMAMEUI_DEBUG ("   Sound status: %s", rom->driver_status_sound);
GMAMEUI_DEBUG ("   Graphic status: %s", rom->driver_status_graphic);
		/* update screenshot panel */
		update_screenshot_panel (rom);
	} else {
		/* no roms selected display the default picture */

		GMAMEUI_DEBUG ("no games selected");
		/* update menus */
		gtk_widget_set_sensitive (GTK_WIDGET (main_gui.play_menu), FALSE);
		gtk_widget_set_sensitive (GTK_WIDGET (main_gui.properties_menu), FALSE);
		gtk_widget_set_sensitive (GTK_WIDGET (main_gui.add_to_favorites), FALSE);
		gtk_widget_set_sensitive (GTK_WIDGET (main_gui.remove_from_favorites), FALSE);

		/* update statusbar */
		set_status_bar (_("No game selected"), "");

		/* update screenshot panel */
		update_screenshot_panel (NULL);

	}

}

void
show_progress_bar (void)
{
	if (gui_prefs.ShowStatusBar) {
		gchar *displayed_message;
		displayed_message = g_strdup_printf (_("Game search %i%% complete"), 0);
		
		gtk_widget_hide (GTK_WIDGET (main_gui.tri_status_bar));
		gtk_statusbar_push (main_gui.status_progress_bar, 1, displayed_message);
		gtk_widget_show (GTK_WIDGET (main_gui.combo_progress_bar));
		g_free (displayed_message);
	}	
}

void
hide_progress_bar (void)
{
	if (gui_prefs.ShowStatusBar) {
		gtk_widget_hide (GTK_WIDGET (main_gui.combo_progress_bar));
		gtk_statusbar_pop (main_gui.status_progress_bar, 1);
		gtk_widget_show (GTK_WIDGET (main_gui.tri_status_bar));
	}
}

void
update_progress_bar (gfloat current_value)
{
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
}

void
update_game_in_list (RomEntry *tmprom)
{
	const gchar *my_hassamples;
	const gchar *my_control;
	GdkColor *my_txtcolor;
	GdkPixbuf *pixbuf;
	gboolean is_tree_store;
	gint pixbuf_width = 0;

	if (!tmprom)
		return;

	/* Whether the Tree Model will a tree or a list */
	if ( (gui_prefs.current_mode == LIST_TREE) || (gui_prefs.current_mode == DETAILS_TREE))
		is_tree_store = TRUE;
	else
		is_tree_store = FALSE;

	rom_entry_get_list_name (tmprom);
	
	/* Has Samples */
	if (tmprom->nb_samples == 0)
		my_hassamples = "";
	else {
		my_hassamples = tmprom->has_samples ? _("Yes") : _("No");
	}
	
	/* Control */
	if (!strcmp (tmprom->control, "trackball"))
		my_control = _("Yes");
	else
		my_control = _("No");
	
	/* Clone Color + Pixbuf width */
	if (strcmp (tmprom->cloneof, "-")) {  /* Clone */
		pixbuf_width = 50;
		my_txtcolor = &gui_prefs.clone_color;
	} else { /* Original */
		pixbuf_width = 20;
		my_txtcolor = NULL; /* Black */
	}
	/* Pixbuf */
/* FIXME This is also done in create_gamelist_content above - consolidate */
	if (tmprom->has_roms == UNKNOWN) {
		pixbuf = Status_Icons [UNKNOWN];
	} else if (tmprom->has_roms == INCORRECT) {
		pixbuf = Status_Icons [INCORRECT];
	} else if (!tmprom->status) {
		pixbuf = Status_Icons [PROBLEMS];
	} else if (tmprom->has_roms == NOT_AVAIL) {
		pixbuf = Status_Icons [NOT_AVAIL];
	} else {
		if (tmprom->icon_pixbuf) {
			pixbuf = tmprom->icon_pixbuf;
		} else {
			pixbuf = Status_Icons [CORRECT];
		}
	}

	if (is_tree_store) {
		gtk_tree_store_set (GTK_TREE_STORE (main_gui.tree_model), &tmprom->position,
				    GAMENAME,                   tmprom->name_in_list,
				    HAS_ROMS,                   tmprom->has_roms ? _("Yes") : _("No"),	/* FIXME */
				    HAS_SAMPLES,                my_hassamples,
				    ROMNAME,                    tmprom->romname,
				    VECTOR,                     tmprom->vector ? _("Vector") : _("Raster"),
				    CONTROL,                    my_control,
				    TIMESPLAYED,                tmprom->timesplayed,
				    MANU,                       tmprom->manu,
				    YEAR,                       tmprom->year,
				    CLONE,                      tmprom->cloneof,
				    DRIVER,                     tmprom->driver,
				    STATUS,                     tmprom->has_roms ? _("Available") : _("Not Available"),	/* FIXME */
				    ROMOF,                      tmprom->romof,
				    DRIVERSTATUS,               tmprom->status ? _("Working") : _("Not Working"),
				    COLOR_STATUS,		tmprom->driver_status_color,
				    SOUND_STATUS, 		tmprom->driver_status_sound,
				    GRAPHIC_STATUS,		tmprom->driver_status_graphic,
				    NUMPLAYERS,                 tmprom->num_players,
				    NUMBUTTONS,                 tmprom->num_buttons,
				    CPU1,                       tmprom->cpu_info[0].name,
				    CPU2,                       tmprom->cpu_info[1].name,
				    CPU3,                       tmprom->cpu_info[2].name,
				    CPU4,                       tmprom->cpu_info[3].name,
				    SOUND1,                     tmprom->sound_info[0].name,
				    SOUND2,                     tmprom->sound_info[1].name,
				    SOUND3,                     tmprom->sound_info[2].name,
				    SOUND4,                     tmprom->sound_info[3].name,
				    MAMEVER,                    tmprom->mame_ver_added,
				    CATEGORY,                   tmprom->category,
				    FAVORITE,                   tmprom->favourite ? _("Yes") : _("No"),
				    CHANNELS,                   tmprom->channels,
				    IS_BIOS,      tmprom->is_bios ? _("Yes") : _("No"),
				    TEXTCOLOR,                  my_txtcolor,            /* text color */
				    PIXBUF,                     pixbuf,                 /* pixbuf */
				    -1);
	} else {
		gtk_list_store_set (GTK_LIST_STORE (main_gui.tree_model), &tmprom->position,
				    GAMENAME,                   tmprom->name_in_list,
				    HAS_ROMS,                   tmprom->has_roms ? _("Yes") : _("No"),	/* FIXME */
				    HAS_SAMPLES,                my_hassamples,
				    ROMNAME,                    tmprom->romname,
				    VECTOR,                     tmprom->vector ? _("Vector") : _("Raster"),
				    CONTROL,                    my_control,
				    TIMESPLAYED,                tmprom->timesplayed,
				    MANU,                       tmprom->manu,
				    YEAR,                       tmprom->year,
				    CLONE,                      tmprom->cloneof,
				    DRIVER,                     tmprom->driver,
				    STATUS,                     tmprom->has_roms ? _("Available") : _("Not Available"),	/* FIXME */
				    ROMOF,                      tmprom->romof,
				    DRIVERSTATUS,               tmprom->status ? _("Working") : _("Not Working"),
				    COLOR_STATUS,		tmprom->driver_status_color,
				    SOUND_STATUS, 		tmprom->driver_status_sound,
				    GRAPHIC_STATUS,		tmprom->driver_status_graphic,
				    NUMPLAYERS,                 tmprom->num_players,
				    NUMBUTTONS,                 tmprom->num_buttons,
				    CPU1,                       tmprom->cpu_info[0].name,
				    CPU2,                       tmprom->cpu_info[1].name,
				    CPU3,                       tmprom->cpu_info[2].name,
				    CPU4,                       tmprom->cpu_info[3].name,
				    SOUND1,                     tmprom->sound_info[0].name,
				    SOUND2,                     tmprom->sound_info[1].name,
				    SOUND3,                     tmprom->sound_info[2].name,
				    SOUND4,                     tmprom->sound_info[3].name,
				    MAMEVER,                    tmprom->mame_ver_added,
				    CATEGORY,                   tmprom->category,
				    FAVORITE,                   tmprom->favourite ? _("Yes") : _("No"),
				    CHANNELS,                   tmprom->channels,
				    IS_BIOS,      tmprom->is_bios ? _("Yes") : _("No"),
				    TEXTCOLOR,                  my_txtcolor,            /* text color */
				    PIXBUF,                     pixbuf,                 /* pixbuf */
				    -1);
	}

}

static void
precheck_for_record (RomEntry *rom,
		     gchar    *inp_selection)
{
	const gchar *inp_file;
	inp_file = inp_selection;
	/* test if the inp file exist */
	GMAMEUI_DEBUG ("check play file selected: {%s}", inp_file);
	if (g_file_test (inp_file, G_FILE_TEST_EXISTS)) {
		/* if yes print a message and return to the selection screen */

		GtkWidget *dialog;
		gint result;

		dialog = gtk_message_dialog_new (GTK_WINDOW (MainWindow),
						 GTK_DIALOG_MODAL,
						 GTK_MESSAGE_WARNING,
						 GTK_BUTTONS_YES_NO,
						 _("A file named '%s' already exists.\nDo you want to overwrite it?"),
						 inp_file);
		result = gtk_dialog_run (GTK_DIALOG (dialog));
		switch (result) {
		case GTK_RESPONSE_YES:
			gtk_widget_hide (dialog);
			record_game (rom, inp_selection);
			break;
		default:
			break;
		}
		gtk_widget_destroy (dialog);

	} else {
		record_game (rom, inp_selection);
	}
}

void
select_inp (RomEntry *rom,
	    gboolean  play_record)
{
	GtkWidget *inp_selection;
	gchar *temp_text;

	if (play_record) {
		inp_selection = gtk_file_chooser_dialog_new (_("Choose inp file to play"),
							     GTK_WINDOW (MainWindow),
							     GTK_FILE_CHOOSER_ACTION_OPEN,
							     GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
							     GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
							     NULL);
	} else {
		inp_selection = gtk_file_chooser_dialog_new (_("Choose inp file to record"),
							     GTK_WINDOW (MainWindow),
							     GTK_FILE_CHOOSER_ACTION_SAVE,
							     GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
							     GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
							     NULL);
	}

	gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (inp_selection), gui_prefs.InputDirectory);
	if (!play_record) {
		temp_text = g_strdup_printf ("%s.inp", rom->romname);
		gtk_file_chooser_set_current_name (GTK_FILE_CHOOSER (inp_selection), temp_text);
		g_free (temp_text);
	}

	/* reenable joystick, was disabled in callback.c (on_playback_input_activate/on_play_and_record_input_activate)*/
	if (gtk_dialog_run (GTK_DIALOG (inp_selection)) == GTK_RESPONSE_ACCEPT) {
		if (play_record) {
			playback_game (rom, gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (inp_selection)));
		} else {
			precheck_for_record (rom, gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (inp_selection)));
		}
	}
	gtk_widget_destroy (inp_selection);
}
