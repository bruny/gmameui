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

#include <gdk/gdkkeysyms.h>
#include <gtk/gtkcolorseldialog.h>
#include <gtk/gtkfontsel.h>
#include <gtk/gtklabel.h>
#include <gtk/gtkmain.h>
#include <gtk/gtktreestore.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "callbacks.h"
#include "interface.h"
#include "about.h"
#include "directories.h"
#include "gmameui.h"
#include "audit.h"
#include "gui.h"
#include "properties.h"
#include "progression_window.h"
#include "io.h"
#include "gui_prefs_dialog.h"

static guint timeoutid;

void update_favourites_list (gboolean add);
gboolean foreach_find_random_rom_in_store (GtkTreeModel *model,
					   GtkTreePath  *path,
					   GtkTreeIter  *iter,
					   gpointer      user_data);

/* Main window menu: File */
void
on_play_activate (GtkAction *action,
		  gpointer  user_data)
{
	g_return_if_fail (current_exec != NULL);

	play_game (gui_prefs.current_game);
}

void
on_play_and_record_input_activate (GtkAction *action,
				   gpointer  user_data)
{
	select_inp (FALSE);
}


void
on_playback_input_activate (GtkAction *action,
			    gpointer  user_data)
{
	select_inp (TRUE);
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
	
	target_row = (gint *) user_data;
	
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

void
on_select_random_game_activate         (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	gint random_game;

	g_return_if_fail (visible_games > 0);

	random_game = (gint) g_random_int_range (0, visible_games);
	GMAMEUI_DEBUG ("random game#%i", random_game);

	gtk_tree_model_foreach (GTK_TREE_MODEL (main_gui.tree_model),
				foreach_find_random_rom_in_store,
				(gpointer *) random_game);
}

void update_favourites_list (gboolean add) {
	Columns_type type;

	gui_prefs.current_game->favourite = add;

	gmameui_ui_set_favourites_sensitive (add);
	
	g_object_get (selected_filter,
		      "type", &type,
		      NULL);
	/* problems because the row values are completly changed, I redisplay the complete game list */
	if (type == FAVORITE)
		create_gamelist_content ();
	else
		update_game_in_list (gui_prefs.current_game);
}

void
on_add_to_favorites_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	update_favourites_list (TRUE);
}


void
on_remove_from_favorites_activate      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	update_favourites_list (FALSE);
}

/* If rom_name is NULL, then the default options are used and loaded, otherwise
   the rom-specific options are used */
static void
show_properties_dialog (gchar *rom_name)
{
	g_return_if_fail (current_exec != NULL);
	
	/* SDLMAME uses a different set of options to XMAME. If we are running
	   XMAME, then use the legacy GXMAME method of maintaining the options */
	if (current_exec->type == XMAME_EXEC_WIN32) {
		/* SDLMAME */
		GtkWidget *options_dialog = mame_options_get_dialog (main_gui.options);

		GladeXML *xml = glade_xml_new (GLADEDIR "options.glade", NULL, NULL);
		mame_options_add_page (main_gui.options, xml, "Display", "Display",
		                       "gmameui-display-toolbar");
		mame_options_add_page (main_gui.options, xml, "OpenGL", "OpenGL",
		                       "gmameui-display-toolbar");
		mame_options_add_page (main_gui.options, xml, "Sound", "Sound",
		                       "gmameui-sound-toolbar");
		mame_options_add_page (main_gui.options, xml, "Input", "Input",
		                       "gmameui-joystick-toolbar");
		mame_options_add_page (main_gui.options, xml, "performance_vbox", "Performance",
		                       "gmameui-general-toolbar");
		mame_options_add_page (main_gui.options, xml, "misc_vbox", "Miscellaneous",
		                       "gmameui-general-toolbar");
		mame_options_add_page (main_gui.options, xml, "debugging_vbox", "Debugging",
		                       "gmameui-rom");

		
		gtk_dialog_run (GTK_WIDGET (options_dialog));
		gtk_widget_destroy (GTK_WIDGET (options_dialog));

	} else {
		/* XMAME options */
		GtkWidget *properties_window;

		/* have to test if a game is selected or not
		   then only after launch the properties window */
		/* joystick focus turned off, will be turned on again in properties.c (exit_properties_window) */
		joy_focus_off ();
		if (rom_name)
			properties_window = create_properties_windows (gui_prefs.current_game);
		else
			properties_window = create_properties_windows (NULL);
		gtk_widget_show (properties_window);
	}
}

void
on_properties_activate (GtkAction *action,
			gpointer  user_data)
{
	show_rom_properties ();
}

void
on_options_activate (GtkAction *action,
		     gpointer  user_data)
{
	gchar *current_rom;
	g_object_get (main_gui.gui_prefs, "current-rom", &current_rom, NULL);
	
	show_properties_dialog (current_rom);
	
	g_free (current_rom);
}

void
on_options_default_activate (GtkAction *action,
			     gpointer  user_data)
{
	show_properties_dialog (NULL);
}

void
on_audit_all_games_activate (GtkMenuItem     *menuitem,
			     gpointer         user_data)
{
	if (!current_exec) {
		gmameui_message (ERROR, NULL, _("No xmame executables defined"));
		/* reenable joystick */
		joy_focus_on ();
		return;
	}

	gamelist_check (current_exec);

	create_checking_games_window ();
	UPDATE_GUI;
	launch_checking_games_window ();
}


void
on_exit_activate (GtkMenuItem     *menuitem,
		  gpointer         user_data)
{
	exit_gmameui ();
}


/* Main window menu: View */
void
on_toolbar_view_menu_activate (GtkAction *action,
			       gpointer          user_data)
{
	gboolean visible;

	visible = gtk_toggle_action_get_active (GTK_TOGGLE_ACTION (action));

	if (visible)
		gtk_widget_show (GTK_WIDGET (main_gui.toolbar));
	else
		gtk_widget_hide (GTK_WIDGET (main_gui.toolbar));

	g_object_set (main_gui.gui_prefs,
		      "show-toolbar", visible,
		      NULL);
}


void
on_status_bar_view_menu_activate       (GtkAction *action,
                                        gpointer         user_data)
{
	gboolean visible;

	visible = gtk_toggle_action_get_active (GTK_TOGGLE_ACTION (action));

	if (visible)
		gtk_widget_show (GTK_WIDGET (main_gui.tri_status_bar));
	else
		gtk_widget_hide (GTK_WIDGET (main_gui.tri_status_bar));

	g_object_set (main_gui.gui_prefs,
		      "show-statusbar", visible,
		      NULL);
}


void
on_folder_list_activate (GtkAction *action,
			 gpointer         user_data)
{
	gboolean visible;

	visible = gtk_toggle_action_get_active (GTK_TOGGLE_ACTION (action));

	g_object_set (main_gui.gui_prefs,
		      "show-filterlist", visible,
		      NULL);
	
	if (visible) {
		show_filters ();
	} else {
		hide_filters ();
	}

}


void
on_screen_shot_activate (GtkAction *action,
			 gpointer         user_data)
{
	gboolean visible;

	visible = gtk_toggle_action_get_active (GTK_TOGGLE_ACTION (action));
	
	g_object_set (main_gui.gui_prefs,
		      "show-screenshot", visible,
		      NULL);

	if (visible) {
		show_snaps ();
	} else {
		hide_snaps ();
	}
}

/* This function is called when the radio option defining the list mode is changed. */
void     on_view_type_changed                   (GtkRadioAction *action,
                                                 gpointer       user_data)
{
	gint val;
	ListMode current_mode;
	ListMode previous_mode;
	
	g_object_get (main_gui.gui_prefs,
		      "current-mode", &current_mode,
		      "previous-mode", &previous_mode,
		      NULL);
	
	val = gtk_radio_action_get_current_value (action);

	if (current_mode != val) {
		previous_mode = current_mode;
		current_mode = val;
		GMAMEUI_DEBUG ("Current mode changed %d --> %d", previous_mode, current_mode);
		g_object_set (main_gui.gui_prefs,
			      "current-mode", current_mode,
			      "previous-mode", previous_mode,
			      NULL);
		
		gtk_action_group_set_sensitive (main_gui.gmameui_view_action_group,
						(current_mode == LIST_TREE) ||
						(current_mode == DETAILS_TREE));

		/* Rebuild the UI */
		create_gamelist (current_mode);

		/* Rebuild the List only if we change from/to tree mode */
		if ((current_mode == DETAILS_TREE) || (current_mode == LIST_TREE)) {
			if ( (previous_mode != DETAILS_TREE) && (previous_mode != LIST_TREE))
				create_gamelist_content ();
		} else {
			if ((previous_mode == DETAILS_TREE) || (previous_mode == LIST_TREE))
				create_gamelist_content ();
		}
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

static void
quick_refresh_list (void)
{
	static gboolean quick_check_running;
	GList *list_pointer;
	RomEntry *rom;
	
	if (quick_check_running) {
		GMAMEUI_DEBUG ("Quick check already running");
		return;
	}

	quick_check_running = 1;
// FIXME TODO	gtk_widget_set_sensitive (GTK_WIDGET (main_gui.refresh_menu), FALSE);
	/* remove all information concerning the presence of roms */
	for (list_pointer = g_list_first (game_list.roms); list_pointer; list_pointer = g_list_next (list_pointer)) {
		rom = (RomEntry *)list_pointer->data;
		rom->has_roms = UNKNOWN;
	}
	/* refresh the display */
	create_gamelist_content ();

	game_list.not_checked_list = g_list_copy (game_list.roms);

	quick_check ();
	/* final refresh only if we are in AVAILABLE or UNAVAILABLE Folder*/
/* DELETE Not using FolderID in new prefs
	if ((gui_prefs.FolderID == AVAILABLE) || (gui_prefs.FolderID == UNAVAILABLE)) {
		create_gamelist_content ();
		GMAMEUI_DEBUG ("Final Refresh");
	}*/
	quick_check_running = 0;
// FIXME TODO	gtk_widget_set_sensitive (GTK_WIDGET (main_gui.refresh_menu), TRUE);
}

void
on_refresh_activate (GtkAction *action,
		     gpointer  user_data)
{
	quick_refresh_list ();
}

void
on_rebuild_game_list_menu_activate     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	gtk_widget_set_sensitive (main_gui.scrolled_window_games, FALSE);
	UPDATE_GUI;
	
	GMAMEUI_DEBUG ("recreate game list");
	gamelist_parse (current_exec);
	GMAMEUI_DEBUG ("reload everything");
	gamelist_save ();
	load_games_ini ();
	load_catver_ini ();
	create_gamelist_content ();
	gtk_widget_set_sensitive (main_gui.scrolled_window_games, TRUE);
}

void
on_directories_menu_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	GtkWidget *directory_window;
	directory_window = create_directories_selection ();
	gtk_widget_show (directory_window);
}

void
on_preferences_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	/*
	GtkWidget *gui_prefs_window;
	gui_prefs_window = create_gui_prefs_window ();
	gtk_widget_show (gui_prefs_window);
	*/
	
	MameGuiPrefsDialog *prefs_dialog;
	prefs_dialog = mame_gui_prefs_dialog_new ();
GMAMEUI_DEBUG("Running dialog");
//	gtk_dialog_run (prefs_dialog);
GMAMEUI_DEBUG("Done running dialog");
//	gtk_widget_destroy (prefs_dialog);
}


/* Main window menu: Help */
void
on_about_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	about_window_show ();
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

static gboolean
delayed_row_selected (GtkTreeSelection *selection)
{
	RomEntry *game_data;
	GtkTreeIter iter;
	GtkTreeModel *model;
	
	if (gtk_tree_selection_get_selected (selection, &model, &iter)) {
		gtk_tree_model_get (model, &iter, ROMENTRY, &game_data, -1);

		g_return_if_fail (game_data != NULL);

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
	g_return_if_fail (gui_prefs.current_game != NULL);
	
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
