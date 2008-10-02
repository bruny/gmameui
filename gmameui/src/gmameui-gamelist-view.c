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

#include <gdk/gdkkeysyms.h>

#include "gmameui-gamelist-view.h"
#include "callbacks.h"
#include "interface.h"
#include "gmameui.h"
#include "gui.h"

static guint timeoutid;

/* Private function prototypes */
static void
set_list_sortable_column ();

static const
char *
column_title (int column_num)
{
	switch (column_num) {
	case GAMENAME:
		return _("Game");
	case HAS_ROMS:
		return _("ROMs");
	case HAS_SAMPLES:
		return _("Samples");
	case ROMNAME:
		return _("Directory");
	case TIMESPLAYED:
		return _("Played");
	case MANU:
		return _("Manufacturer");
	case YEAR:
		return _("Year");
	case CLONE:
		return _("Clone of");
	case DRIVER:
		return _("Driver");
	case STATUS:       /*  Available / Not Available */
		return _("Status");
	case ROMOF:
		return _("Rom of");
	case DRIVERSTATUS: /*  Working / Not Working */
		return _("Driver Status");
	case COLOR_STATUS:
		return _("Driver Colors");
	case SOUND_STATUS:
		return _("Driver Sound");
	case GRAPHIC_STATUS:
		return _("Driver Graphics");
	case NUMPLAYERS:
		return _("Players");
	case NUMBUTTONS:
		return _("Buttons");
	case MAMEVER:
		return _("Version");
	case CATEGORY:
		return _("Category");
	case FAVORITE:
		return _("Favorite");
	case CHANNELS:
		return _("Channels");
	case IS_BIOS:
		return _("BIOS");
	default:
		return NULL;
	}
}

static void
set_list_sortable_column ()
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

gboolean foreach_find_random_rom_in_store (GtkTreeModel *model,
					   GtkTreePath  *path,
					   GtkTreeIter  *iter,
					   gpointer      user_data)
{
	static gint current_row;
	gint target_row;
	gboolean return_val;
	
	return_val = FALSE;		/* Do not stop walking the store, call us with next row */
	
	target_row = (gint) user_data;
	
	if (current_row == target_row) {
		/* Found the random row we are after */
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
		
		return_val = TRUE;      /* Stop walking the store */
		current_row = 0;	/* Reset for next time we want to find a random row */
	}
	
	current_row++;
	
	return return_val;   
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
on_collapse_all_activate (GtkMenuItem     *menuitem,
			  gpointer         user_data)
{
	GtkTreeModel *model;
	GtkTreeIter iter;
	GtkTreeSelection *select;
	GtkTreePath *tree_path;

	gtk_tree_view_collapse_all (GTK_TREE_VIEW (main_gui.displayed_list));
	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (main_gui.displayed_list));
	if (gtk_tree_selection_get_selected (select, &model, &iter)) {
		tree_path = gtk_tree_model_get_path (GTK_TREE_MODEL (main_gui.tree_model), &iter);
		gtk_tree_view_scroll_to_cell (GTK_TREE_VIEW (main_gui.displayed_list), tree_path, NULL, TRUE, 0.5, 0);
		gtk_tree_path_free (tree_path);
	}
}

void
on_expand_all_activate (GtkMenuItem *menuitem,
			gpointer     user_data)
{
	GtkTreeModel *model;
	GtkTreeIter iter;
	GtkTreeSelection *select;
	GtkTreePath *tree_path;

	gtk_tree_view_expand_all (GTK_TREE_VIEW (main_gui.displayed_list));
	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (main_gui.displayed_list));
	if (gtk_tree_selection_get_selected (select, &model, &iter)) {
		tree_path = gtk_tree_model_get_path (GTK_TREE_MODEL (main_gui.tree_model), &iter);
		gtk_tree_view_scroll_to_cell (GTK_TREE_VIEW (main_gui.displayed_list), tree_path, NULL, TRUE, 0.5, 0);
		gtk_tree_path_free (tree_path);
	}
}

static gboolean
delayed_row_selected (GtkTreeSelection *selection)
{
	RomEntry *game_data;
	GtkTreeIter iter;
	GtkTreeModel *model;
	
	if (gtk_tree_selection_get_selected (selection, &model, &iter)) {
		gtk_tree_model_get (model, &iter, ROMENTRY, &game_data, -1);

		g_return_val_if_fail (game_data != NULL, FALSE);

		g_object_set (main_gui.gui_prefs, "current-rom", game_data->romname, NULL);

		select_game (game_data);
	}
	
	return FALSE;
}

/* Main list */
void
on_row_selected (GtkTreeSelection *selection,
		 gpointer          data)
{
	if (timeoutid)
		g_source_remove (timeoutid);
	timeoutid = g_timeout_add (SELECT_TIMEOUT,
				   (GSourceFunc) delayed_row_selected,
				   selection);
}

gboolean
on_list_keypress (GtkWidget   *widget,
		  GdkEventKey *event,
		  gpointer    user_data)
{
	g_return_val_if_fail (gui_prefs.current_game != NULL, FALSE);
	
	if (event && event->type == GDK_KEY_PRESS && (
						      event->keyval == GDK_KP_Enter ||
						      event->keyval == GDK_Return))
		play_game (gui_prefs.current_game);
	
	return FALSE;
}

gboolean
on_list_clicked (GtkWidget      *widget,
		 GdkEventButton *event,
		 gpointer        user_data)
{
	GtkTreePath *path=NULL;
	GtkTreeIter iter;
	gboolean myclick=FALSE;
	GValue val = { 0, };
	RomEntry *game_data;
	gint x,y;

	/* Here we figure out which node was clicked */
	myclick = gtk_tree_view_get_path_at_pos (GTK_TREE_VIEW (widget), event->x, event->y, &path, NULL, &x, &y);
	if (path == NULL) {
		GMAMEUI_DEBUG ("PATH NULL");
		return FALSE;
	}

	/* We test here if we clicked between 2 columns
	I know this will make double click on the row 0 col 0 not working
	but the result of gtk_tree_view_get_path_at_pos is TRUE
	even if we don't click on a row ????? (maybe a GTK bug)
	if ( ( (event->x==x) && (event->y==y)) || (myclick==FALSE) ) */
	if (myclick == FALSE) {
		return FALSE;
	}

	/* Here we figure out which node was clicked */
	gtk_tree_model_get_iter (GTK_TREE_MODEL (main_gui.tree_model), &iter, path);
	gtk_tree_model_get_value (GTK_TREE_MODEL (main_gui.tree_model), &iter, ROMENTRY, &val);
	game_data = g_value_get_pointer (&val);

	if (path)
		gtk_tree_path_free (path);

	if (event) {
		if (event->type == GDK_2BUTTON_PRESS && event->button == 1) {
			play_game (game_data);			/* Double-click */
		} else if (event->type == GDK_BUTTON_PRESS && event->button == 3) {
			gamelist_popupmenu_show (event);	/* Right-click */
		}
	}	

	return FALSE;
}

gboolean
on_column_click (GtkWidget         *button,
		 GdkEventButton    *event,
		 GtkTreeViewColumn *column)
{
	gchar *hide_column_menu_name;
	GtkWidget *hide_menu_item;
	
	GtkWidget *popup_menu;

	/* Only right-mouse clicks are catered for */
	if (event->type == GDK_BUTTON_PRESS && ( (GdkEventButton*)event)->button == 3) {
		
		popup_menu = gtk_ui_manager_get_widget (main_gui.manager, "/ColumnPopup");
		g_return_val_if_fail (popup_menu != NULL, FALSE);

		hide_menu_item = gtk_ui_manager_get_widget (main_gui.manager,
							    "/ColumnPopup/ColumnHide");
		
		hide_column_menu_name = g_strdup_printf (_("Hide \"%s\" Column"),
							 column_title (gtk_tree_view_column_get_sort_column_id (GTK_TREE_VIEW_COLUMN (column))));
		gtk_label_set_text (GTK_LABEL (GTK_BIN (hide_menu_item)->child),
				    hide_column_menu_name);
		g_free (hide_column_menu_name);

		/* Don't allow user to hide the game name column */
		ColumnHide_selected = gtk_tree_view_column_get_sort_column_id (GTK_TREE_VIEW_COLUMN (column));
		gtk_widget_set_sensitive (GTK_WIDGET (hide_menu_item),
					  !(ColumnHide_selected == GAMENAME));
		
		gtk_menu_popup (GTK_MENU (popup_menu), NULL, NULL,
				NULL, NULL,
				event->button, event->time);
	}
	return FALSE;
}

/* Column Popup menu */
void
on_column_hide_activate (GtkMenuItem     *menuitem,
			 gpointer         user_data)
{
	GList *column_list=NULL;
	GList *C=NULL;
	GValueArray *va;
	
	g_object_get (main_gui.gui_prefs,
		      "cols-shown", &va,
		      NULL);

	GMAMEUI_DEBUG ("Column Hide - %i",ColumnHide_selected);

	column_list=gtk_tree_view_get_columns (GTK_TREE_VIEW (main_gui.displayed_list));
	for (C = g_list_first (column_list) ; C != NULL ; C = g_list_next (C)) {
		if (ColumnHide_selected == gtk_tree_view_column_get_sort_column_id (GTK_TREE_VIEW_COLUMN (C->data)))
			break;
	}

	if (C->data) {
		gtk_tree_view_column_set_visible (C->data, FALSE);
		g_value_set_int (g_value_array_get_nth (va, ColumnHide_selected), 0);
		g_object_set (main_gui.gui_prefs,
			      "cols-shown", va,
			      NULL);
		GMAMEUI_DEBUG ("Column Hidden - %i", ColumnHide_selected);
	}
}

void
on_displayed_list_resize_column (GtkWidget      *widget,
				 GtkRequisition *requisition,
				 gpointer        user_data)
{
	GtkTreeViewColumn *col;
	gint col_id;
	GValueArray *va;
	GList *column_list;
	GList *pointer_list;
	gint i;

	ListMode current_mode;
GMAMEUI_DEBUG("Resizing columns");
	g_object_get (main_gui.gui_prefs,
		      "current-mode", &current_mode,
		      "cols-width", &va,
		      NULL);
	
	if ((current_mode == DETAILS) || (current_mode == DETAILS_TREE)) {
		column_list = gtk_tree_view_get_columns (GTK_TREE_VIEW (widget));

		for (pointer_list = g_list_first (column_list), i = 0;
		     pointer_list != NULL;
		     pointer_list = g_list_next (pointer_list), i++) {
			col = GTK_TREE_VIEW_COLUMN (pointer_list->data);
			if (gtk_tree_view_column_get_visible (col)) {
				col_id = gtk_tree_view_column_get_sort_column_id (col);
				/*GMAMEUI_DEBUG ("Setting width of column %d %s to %d",
					       col_id, gtk_tree_view_column_get_title (col),
					       gtk_tree_view_column_get_width (col));*/
				g_value_set_int (g_value_array_get_nth (va, col_id), gtk_tree_view_column_get_width (col));
				g_object_set (main_gui.gui_prefs,
					      "cols-width", va,
					      NULL);
			} else
				GMAMEUI_DEBUG ("Column %s is not visible", gtk_tree_view_column_get_title (col));
		}
		
		g_list_free (column_list);
		g_list_free (pointer_list);
	} else
		GMAMEUI_DEBUG ("Not in details mode - column sizing not allowed");
/* FIXME TODO	
	if (va)
		g_value_array_free (va);*/
GMAMEUI_DEBUG("Resizing columns... done");
}

void
on_displayed_list_row_collapsed (GtkTreeView *treeview,
				 GtkTreeIter *iter,
				 GtkTreePath *path,
				 gpointer     user_data)
{
	GtkTreeIter iter_child;
	RomEntry *tmprom;
	GtkTreeSelection *select;
	GtkTreeModel* treemodel;
	gchar *current_rom_name;

	/* If one of the child iter is selected, we select the parent iter */
	treemodel = gtk_tree_view_get_model (treeview);
	gtk_tree_model_iter_children (treemodel, &iter_child, iter);
	gtk_tree_model_get (treemodel, &iter_child, ROMENTRY, &tmprom, -1);

	while ((g_ascii_strcasecmp (current_rom_name, tmprom->romname) != 0) &&
	       (gtk_tree_model_iter_next (treemodel, &iter_child))) {
		gtk_tree_model_get (treemodel, &iter_child, ROMENTRY, &tmprom, -1);
	}

	g_object_get (main_gui.gui_prefs, "current-rom", &current_rom_name, NULL);
	
	if (g_ascii_strcasecmp (current_rom_name, tmprom->romname) == 0) {
		GtkTreePath *tree_path;
		select = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));
		tree_path = gtk_tree_model_get_path (GTK_TREE_MODEL (treemodel), iter);
		gtk_tree_view_set_cursor (GTK_TREE_VIEW (treeview),
					  tree_path,
					  NULL, FALSE);
		gtk_tree_path_free (tree_path);
	}
	
	g_free (current_rom_name);
}

void
on_displayed_list_sort_column_changed (GtkTreeSortable *treesortable,
                                       gpointer         user_data)
{
	gint sort_column_id;
	GtkSortType order;
GMAMEUI_DEBUG("Changing list sorting");
	if (gtk_tree_sortable_get_sort_column_id (treesortable, &sort_column_id, &order)) {	
		g_object_set (main_gui.gui_prefs,
			      "sort-col", sort_column_id,
			      "sort-col-direction", order,
			      NULL);
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

void
create_gamelist (ListMode list_mode)
{
	gint i;
	GtkTreeViewColumn *column;
	GtkCellRenderer *renderer;
	GtkTreeSelection *select;

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

				/* Pixbuf */
				renderer = gtk_cell_renderer_pixbuf_new ();
				gtk_tree_view_column_pack_start (GTK_TREE_VIEW_COLUMN (column),
							       renderer,
							       FALSE);
				gtk_tree_view_column_add_attribute (GTK_TREE_VIEW_COLUMN (column), renderer,
								    "pixbuf", PIXBUF);
				g_object_set (renderer, "xalign", 1.0, "ypad", 0, NULL);
				
				/* TEXT */
				renderer = gtk_cell_renderer_text_new ();
				gtk_tree_view_column_pack_end (GTK_TREE_VIEW_COLUMN (column),
							       renderer,
							       TRUE);
				gtk_tree_view_column_set_attributes (GTK_TREE_VIEW_COLUMN (column), renderer,
								     "text", i,
								     "foreground-gdk", TEXTCOLOR,
								     NULL);
				g_object_set (renderer, "ypad", 0, "yalign", 0.5, "ellipsize", PANGO_ELLIPSIZE_END, NULL);

			} else {
				renderer = gtk_cell_renderer_text_new ();
				column = gtk_tree_view_column_new_with_attributes (column_title (i), renderer,
										   "text", i,
										   "foreground-gdk", TEXTCOLOR,
										   NULL);
				g_object_set (renderer, "ellipsize", PANGO_ELLIPSIZE_END, NULL);
				gtk_tree_view_column_set_sort_column_id (column, i);
			}
			gtk_tree_view_append_column (GTK_TREE_VIEW (main_gui.displayed_list), column);
			//gtk_tree_view_column_set_min_width (GTK_TREE_VIEW_COLUMN (column), 1);
			
			/* Set minimum width to the num of chars in the header TODO Also get sort directory size */
			gint font_size, title_len;
			title_len = g_utf8_strlen (gtk_tree_view_column_get_title (GTK_TREE_VIEW_COLUMN (column)), -1);
			font_size = PANGO_PIXELS (pango_font_description_get_size (GTK_WIDGET(main_gui.displayed_list)->style->font_desc));
			gtk_tree_view_column_set_min_width (GTK_TREE_VIEW_COLUMN (column), title_len*font_size);
GMAMEUI_DEBUG ("Setting min width of column %s. title len is %d, font size is %d", gtk_tree_view_column_get_title (GTK_TREE_VIEW_COLUMN (column)), title_len, font_size);
			
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
		/* Reordering columns is disabled because it conflicts with
		  'column popup menu' and 'sort on click header' */
		gtk_tree_view_column_set_reorderable (GTK_TREE_VIEW_COLUMN (column), FALSE);
	}
	g_signal_handlers_unblock_by_func (G_OBJECT (main_gui.displayed_list), (gpointer)on_displayed_list_resize_column, NULL);

	gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (main_gui.displayed_list), TRUE);
	
	dirty_icon_cache = FALSE;

	GMAMEUI_DEBUG ("Creating gamelist structure... done");
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
	ListMode current_mode;
	gchar *current_rom_name;
	gchar *clone_color;

	g_return_if_fail (gui_prefs.gl != NULL);

	GMAMEUI_DEBUG ("POPULATE GAME LIST");
GTimer *timer = g_timer_new ();
g_timer_start (timer);
	
	g_object_get (main_gui.gui_prefs,
		      "current-mode", &current_mode,
		      "current-rom", &current_rom_name,
		      "clone-color", &clone_color,
		      NULL);

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

	/* Fill the model with data */
	for (listpointer = g_list_first (mame_gamelist_get_roms_glist (gui_prefs.gl));
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
			if (g_ascii_strcasecmp (tmprom->cloneof, "-") != 0) {
				/* Clone */
				gdk_color_parse (clone_color, &my_txtcolor);
			} else {
				/* Original */
				gdk_color_parse ("black", &my_txtcolor);
			}

			/* Set the pixbuf for the status icon */
			pixbuf = Status_Icons [tmprom->has_roms];

			/* Determine if the row is a root */
			if ((j == 0) ||
			    (g_ascii_strcasecmp (tmprom->cloneof, "-") == 0) || 
			    !my_romname_root ||
			    (g_ascii_strcasecmp (tmprom->cloneof, my_romname_root) != 0)) {
				is_root = TRUE;
			} else {
				is_root = FALSE;
			}

			/* Memorize the original names */
			if (!(g_ascii_strcasecmp (tmprom->cloneof, "-"))) {
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
	if (clone_color)
		g_free (clone_color);

	if (visible_games == 0)
		select_game (NULL);
	else
		select_game (gui_prefs.current_game);

	GMAMEUI_DEBUG ("Time taken to create_gamelist_content is %.2f", g_timer_elapsed (timer, NULL));
	g_timer_destroy (timer);
}
