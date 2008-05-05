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
#include "gui_prefs.h"
#include "column_layout.h"

static guint timeoutid;
static guint timeoutfoldid;


/* Main window menu: File */
void
on_play_activate (GtkAction *action,
		  gpointer  user_data)
{
	if (!current_exec)
		return;
	play_game (gui_prefs.current_game);
}

void
on_play_and_record_input_activate (GtkAction *action,
				   gpointer  user_data)
{
	if (!current_exec)
		return;
	/* joystick focus turned off, will be turned on again in:
	   gui.c (select_inp) when cancelling the file selection
	   gmameui.c (record_game) */
	joy_focus_off ();
	select_inp (gui_prefs.current_game, FALSE);
}


void
on_playback_input_activate             (GtkAction *action/*GtkMenuItem     *menuitem*/,
                                        gpointer         user_data)
{
	if (!current_exec)
		return;
	/* joystick focus turned off, will be turned on again in:
	   gui.c (select_inp)
	   gmameui.c (playback_game)*/
	joy_focus_off ();
	select_inp (gui_prefs.current_game, TRUE);
}


void
on_select_random_game_activate         (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	gint random_game;
	gint i;
	gboolean is_root;
	gboolean valid;
	GtkTreeIter iter, iter_child;
	GtkTreeSelection *select;

	random_game = (gint) g_random_int_range (0, visible_games);
	GMAMEUI_DEBUG ("random game#%i", random_game);

	valid = gtk_tree_model_get_iter_first (GTK_TREE_MODEL (main_gui.tree_model), &iter);
	i=0;
	is_root=TRUE;
	while ((i < random_game) && valid)
	{
		if (gtk_tree_model_iter_has_child (GTK_TREE_MODEL (main_gui.tree_model),&iter))
		{
			if (gtk_tree_model_iter_children (GTK_TREE_MODEL (main_gui.tree_model), &iter_child, &iter))
			{
				is_root = FALSE;
				i++;
				while (gtk_tree_model_iter_next (GTK_TREE_MODEL (main_gui.tree_model), &iter_child) && (i < random_game))
				{
					i++;
				}
			}
		}
		if (i < random_game)
		{
			valid = gtk_tree_model_iter_next (GTK_TREE_MODEL (main_gui.tree_model), &iter);
			if (valid)
			{
				is_root = TRUE;
				i++;
			}
		}
	}
	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (main_gui.displayed_list));
	if (is_root)
	{
		GtkTreePath *path = gtk_tree_model_get_path (GTK_TREE_MODEL (main_gui.tree_model), &iter);
		gtk_tree_view_set_cursor (GTK_TREE_VIEW (main_gui.displayed_list),
					  path,
					  NULL, FALSE);
		/* Scroll to selection */
		gtk_tree_view_scroll_to_cell (GTK_TREE_VIEW (main_gui.displayed_list), path, NULL, TRUE, 0.5, 0);
		gtk_tree_path_free (path);
	}else
	{
		GtkTreePath *path, *child_path;
		path = gtk_tree_model_get_path (GTK_TREE_MODEL (main_gui.tree_model), &iter);
		valid = gtk_tree_view_expand_row (GTK_TREE_VIEW (main_gui.displayed_list),
						  path,
						  TRUE);
		gtk_tree_path_free (path);
		child_path = gtk_tree_model_get_path (GTK_TREE_MODEL (main_gui.tree_model), &iter_child);
		gtk_tree_view_set_cursor (GTK_TREE_VIEW (main_gui.displayed_list),
					  child_path,
					  NULL, FALSE);
		/* Scroll to selection */
		gtk_tree_view_scroll_to_cell (GTK_TREE_VIEW (main_gui.displayed_list), child_path, NULL, TRUE, 0.5, 0);
		gtk_tree_path_free (child_path);
	}
}

void update_favourites_list (gboolean add) {
	Columns_type type;
	
	gui_prefs.current_game->favourite = add;

	gmameui_toolbar_set_favourites_sensitive (add);
	
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

static void
show_properties_dialog (void)
{
	GtkWidget *properties_window;
	if (!current_exec)
		return;
	/* have to test if a game is selected or not
	   then only after launch the properties window */
	/* joystick focus turned off, will be turned on again in properties.c (exit_properties_window) */
	joy_focus_off ();
	properties_window = create_properties_windows (gui_prefs.current_game);
	gtk_widget_show (properties_window);
}

void
on_properties_activate (GtkAction *action,
			gpointer  user_data)
{
	show_rom_properties ();
}

void on_options_activate (GtkAction *action,
			  gpointer  user_data)
{
	/* SDLMAME uses a different set of options to XMAME. If we are running
	   XMAME, then use the legacy GXMAME method of maintaining the options */
	if (current_exec->type == XMAME_EXEC_WIN32) {
		
		GtkWidget *options_dialog = mame_options_get_dialog (main_gui.options);

		GladeXML *xml = glade_xml_new (GLADEDIR "options.glade", NULL, NULL);
		mame_options_add_page (main_gui.options, xml, "Display", "Display",
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
		show_properties_dialog ();
	}
}

void
on_audit_all_games_activate (GtkMenuItem     *menuitem,
			     gpointer         user_data)
{
	if (!current_exec) {
		gmameui_message (ERROR, NULL, _("No xmame executables defined"));
		gtk_widget_destroy (user_data);
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
		show_toolbar ();
	else
		hide_toolbar ();
}


void
on_status_bar_view_menu_activate       (GtkAction *action,
                                        gpointer         user_data)
{
	gboolean visible;

	visible = gtk_toggle_action_get_active (GTK_TOGGLE_ACTION (action));

	if (visible)
		show_status_bar ();
	else
		hide_status_bar ();
}


void
on_folder_list_activate (GtkAction *action,
			 gpointer         user_data)
{
	gboolean visible;

	visible = gtk_toggle_action_get_active (GTK_TOGGLE_ACTION (action));

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

	if (visible) {
		show_snaps ();
	} else {
		hide_snaps ();
	}
}

void
on_screen_shot_tab_activate (GtkAction *action,
			     gpointer         user_data)
{
	gboolean visible;

	visible = gtk_toggle_action_get_active (GTK_TOGGLE_ACTION (action));

	if (visible)
		show_snaps_tab (GMAMEUI_SIDEBAR (main_gui.screenshot_hist_frame));
	else
		hide_snaps_tab (GMAMEUI_SIDEBAR (main_gui.screenshot_hist_frame));
}

/* This function is called when the radio option defining the list mode is
   changed. */
void     on_view_type_changed                   (GtkRadioAction *action,
                                                 gpointer       user_data)
{
	gint val;
	GtkWidget *widget;
	
	val = gtk_radio_action_get_current_value (action);


	if (gui_prefs.current_mode != val) {
		gui_prefs.previous_mode = gui_prefs.current_mode;
		gui_prefs.current_mode = val;
		GMAMEUI_DEBUG ("Current mode changed %d --> %d", gui_prefs.previous_mode, gui_prefs.current_mode);
			
		if ( (gui_prefs.current_mode==LIST_TREE) || (gui_prefs.current_mode == DETAILS_TREE))
			gtk_action_group_set_sensitive (main_gui.gmameui_view_action_group, TRUE);
		else
			gtk_action_group_set_sensitive (main_gui.gmameui_view_action_group, FALSE);

		/* Rebuild the UI */
		create_gamelist (gui_prefs.current_mode);

		/* Rebuild the List only if we change from/to tree mode */
		if ((gui_prefs.current_mode == DETAILS_TREE) || (gui_prefs.current_mode == LIST_TREE)) {
			if ( (gui_prefs.previous_mode != DETAILS_TREE) && (gui_prefs.previous_mode != LIST_TREE))
				create_gamelist_content ();
		} else {
			if ((gui_prefs.previous_mode == DETAILS_TREE) || (gui_prefs.previous_mode == LIST_TREE))
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

void
on_column_layout_activate (GtkCheckMenuItem *menuitem,
			   gpointer          user_data)
{
	GtkWidget *column_layout;
	column_layout = create_column_layout_window ();
	gtk_widget_show (column_layout);
}

void
on_the_prefix_activate                 (GtkCheckMenuItem     *menuitem,
                                        gpointer         user_data)
{
	gboolean is_active;

	is_active = gtk_check_menu_item_get_active (menuitem);
	gui_prefs.ModifyThe = is_active;
	create_gamelist_content ();
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
	if ((gui_prefs.FolderID == AVAILABLE) || (gui_prefs.FolderID == UNAVAILABLE)) {
		create_gamelist_content ();
		GMAMEUI_DEBUG ("Final Refresh");
	}
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
on_clone_color_menu_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	GtkWidget *clone_selector_dlg = gtk_color_selection_dialog_new (_("Clone color selection"));
	GdkColor mycolor;
	gint response;

	mycolor.pixel = 0;
	mycolor.red = (guint16) gui_prefs.clone_color.red;
	mycolor.green = (guint16) gui_prefs.clone_color.green;
	mycolor.blue = (guint16) gui_prefs.clone_color.blue;

	gtk_color_selection_set_current_color (GTK_COLOR_SELECTION (GTK_COLOR_SELECTION_DIALOG (clone_selector_dlg)->colorsel), &mycolor);

	response = gtk_dialog_run (GTK_DIALOG (clone_selector_dlg));
	
	switch (response) {
		case GTK_RESPONSE_OK:
			gtk_color_selection_get_current_color (GTK_COLOR_SELECTION (GTK_COLOR_SELECTION_DIALOG (clone_selector_dlg)->colorsel), &mycolor);
			gui_prefs.clone_color.red = mycolor.red;
			gui_prefs.clone_color.green = mycolor.green;
			gui_prefs.clone_color.blue = mycolor.blue;
			
			gtk_widget_destroy (clone_selector_dlg);
			
			create_gamelist_content ();
			break;
		default:
			gtk_widget_destroy (clone_selector_dlg);
			break;
	}
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
on_default_option_menu_activate        (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	GtkWidget *general_properties_window;
	if (!current_exec)
		return;
	/* have to test if a game is selected or not
	   then only after lauche the properties window*/
	general_properties_window = create_properties_windows (NULL);
	gtk_widget_show (general_properties_window);
}


void
on_startup_option_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	GtkWidget *gui_prefs_window;
	gui_prefs_window = create_gui_prefs_window ();
	gtk_widget_show (gui_prefs_window);
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

	GMAMEUI_DEBUG ("Column Hide - %i",ColumnHide_selected);

	column_list=gtk_tree_view_get_columns (GTK_TREE_VIEW (main_gui.displayed_list));
	for (C = g_list_first (column_list) ; C != NULL ; C = g_list_next (C)) {
		if (ColumnHide_selected == gtk_tree_view_column_get_sort_column_id (GTK_TREE_VIEW_COLUMN (C->data)))
			break;
	}

	if (C->data) {
		gtk_tree_view_column_set_visible (C->data, FALSE);
		gui_prefs.ColumnShown [ColumnHide_selected] = FALSE;
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
		/* select the game only if it wasn't the previously selected one
		 (prevent screenshot flickers)*/
		if (game_data != gui_prefs.current_game)
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

	if (event && event->type == GDK_2BUTTON_PRESS) {
		if (event->button == 1) {
			GMAMEUI_DEBUG ("double click");
			play_game (game_data);
		}
	}

	if (event && event->type == GDK_BUTTON_PRESS && event->button == 3) {
		 gamelist_popupmenu_show (gui_prefs.current_game, event);
	}
	return FALSE;
}


/* Used to detect if key was pressed in the main list
   that prevent key release detection from another window, dialog box or popup*/
gboolean
on_displayed_list_key_press_event (GtkWidget	*widget,
				   GdkEventKey	*event,
				   gpointer	 user_data)
{
	displayedlist_keypressed = TRUE;

	return FALSE;
}

void
on_displayed_list_resize_column (GtkWidget      *widget,
				 GtkRequisition *requisition,
				 gpointer        user_data)
{
	GList *column_list;
	GList *pointer_list;
	gint i;

	if ((gui_prefs.current_mode == DETAILS) || (gui_prefs.current_mode == DETAILS_TREE)) {
		column_list = gtk_tree_view_get_columns (GTK_TREE_VIEW (widget));

		for (pointer_list = g_list_first (column_list), i = 0;
		     pointer_list != NULL;
		     pointer_list = g_list_next (pointer_list), i++)
			if (gtk_tree_view_column_get_visible (GTK_TREE_VIEW_COLUMN (pointer_list->data)))
				gui_prefs.ColumnWidth[gtk_tree_view_column_get_sort_column_id (GTK_TREE_VIEW_COLUMN (pointer_list->data))] = gtk_tree_view_column_get_width (GTK_TREE_VIEW_COLUMN (pointer_list->data));
  /* This is to debug the fact that this callback is called when switching fron LIST to DETAIL mode
	even if I block it??? (See the create_gamelist in gui.c)
	if (gtk_tree_view_column_get_sort_column_id (GTK_TREE_VIEW_COLUMN (pointer_list->data))==GAMENAME)
	GMAMEUI_DEBUG ("Gamename column: %i",gtk_tree_view_column_get_width (GTK_TREE_VIEW_COLUMN (pointer_list->data))); */

		g_list_free (column_list);
		g_list_free (pointer_list);
	}
}

void
on_displayed_list_sort_column_changed (GtkTreeSortable *treesortable,
                                       gpointer         user_data)
{
	gint sort_column_id;
	GtkSortType order;

	if (gtk_tree_sortable_get_sort_column_id (treesortable, &sort_column_id, &order)) {
		if (order == GTK_SORT_DESCENDING)
			gui_prefs.SortReverse = TRUE;
		else
			gui_prefs.SortReverse = FALSE;
		gui_prefs.SortColumn = sort_column_id;
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

	/* If one of the child iter is selected, we select the parent iter */
	treemodel = gtk_tree_view_get_model (treeview);
	gtk_tree_model_iter_children (treemodel, &iter_child, iter);
	gtk_tree_model_get (treemodel, &iter_child, ROMENTRY, &tmprom, -1);
	while ((gui_prefs.current_game != tmprom) && (gtk_tree_model_iter_next (treemodel, &iter_child))) {
		gtk_tree_model_get (treemodel, &iter_child, ROMENTRY, &tmprom, -1);
	}

	if (gui_prefs.current_game == tmprom) {
		GtkTreePath *tree_path;
		select = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));
		tree_path = gtk_tree_model_get_path (GTK_TREE_MODEL (treemodel), iter);
		gtk_tree_view_set_cursor (GTK_TREE_VIEW (treeview),
					  tree_path,
					  NULL, FALSE);
		gtk_tree_path_free (tree_path);
	}
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
