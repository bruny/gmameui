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
on_play_activate (GtkMenuItem *menuitem,
		  gpointer     user_data)
{
	if (!current_exec)
		return;
	play_game (gui_prefs.current_game);
}
void
on_play_clicked (GtkToolButton *button,
		 gpointer       user_data)
{
	if (!current_exec)
		return;
	play_game (gui_prefs.current_game);
}

void
on_play_and_record_input_activate (GtkMenuItem     *menuitem,
				   gpointer         user_data)
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
on_playback_input_activate             (GtkMenuItem     *menuitem,
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
	gui_prefs.current_game->favourite = add;
	gtk_widget_set_sensitive (GTK_WIDGET (main_gui.add_to_favorites), !add);
	gtk_widget_set_sensitive (GTK_WIDGET (main_gui.remove_from_favorites), add);
	
	/* problems because the row values are completly changed, I redisplay the complete game list */
	if (current_filter->type == FAVORITE)
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
on_properties_activate (GtkMenuItem     *menuitem,
			gpointer         user_data)
{
	show_properties_dialog ();
}

void
on_properties_clicked (GtkToolButton *button,
		       gpointer       user_data)
{
	show_properties_dialog ();
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
on_toolbar_view_menu_activate (GtkCheckMenuItem *menuitem,
			       gpointer          user_data)
{
	if (menuitem->active) {
		GMAMEUI_DEBUG ("Show toolbar");
		show_toolbar ();
	} else {
		GMAMEUI_DEBUG ("Hide toolbar");
		hide_toolbar ();
	}
}


void
on_status_bar_view_menu_activate       (GtkCheckMenuItem     *menuitem,
                                        gpointer         user_data)
{
	if (menuitem->active) {
		GMAMEUI_DEBUG ("Show status bar");
		show_status_bar ();
	} else {
		GMAMEUI_DEBUG ("Hide status bar");
		hide_status_bar ();
	}
}


void
on_folder_list_activate (GtkCheckMenuItem *menuitem,
			 gpointer         user_data)
{
	if (menuitem->active) {
		GMAMEUI_DEBUG ("Show filters menu");
		/* shouldn't I block the signal to toggle button
		   otherwise, infinite loop?
		   it seems it's working anyway ??
		   maybe the gtk_toggle_button_set_active doesn't provoke a call
		   to on_mode_button_clicked*/
		gtk_toggle_tool_button_set_active (main_gui.filterShowButton, TRUE);
		show_filters ();
	} else {
		GMAMEUI_DEBUG ("Hide filters menu");
		gtk_toggle_tool_button_set_active (main_gui.filterShowButton, FALSE);
		hide_filters ();
	}

}


void
on_screen_shot_activate (GtkCheckMenuItem *menuitem,
			 gpointer         user_data)
{
	if (menuitem->active) {
		GMAMEUI_DEBUG ("Show snaps menu");
		gtk_toggle_tool_button_set_active (main_gui.snapShowButton, TRUE);
		show_snaps ();
	} else {
		GMAMEUI_DEBUG ("Hide snaps menu");
		gtk_toggle_tool_button_set_active (main_gui.snapShowButton, FALSE);
		hide_snaps ();
	}
}

void
on_screen_shot_tab_activate (GtkCheckMenuItem *menuitem,
			     gpointer         user_data)
{
	if (menuitem->active) {
		GMAMEUI_DEBUG ("Show snaps tab menu");
		show_snaps_tab (GMAMEUI_SIDEBAR (main_gui.screenshot_hist_frame));
	} else {
		GMAMEUI_DEBUG ("Hide snaps tab menu");
		hide_snaps_tab (GMAMEUI_SIDEBAR (main_gui.screenshot_hist_frame));
	}
}

void
on_list_view_menu_activate (GtkCheckMenuItem *menuitem,
			    gpointer          user_data)
{
	if (!main_gui.list_view_button)
		return;

	gtk_toggle_tool_button_set_active (main_gui.list_view_button, menuitem->active);
}

void
on_list_tree_view_menu_activate (GtkCheckMenuItem *menuitem,
				 gpointer          user_data)
{
	if (!main_gui.list_tree_view_button)
		return;

	gtk_toggle_tool_button_set_active (main_gui.list_tree_view_button, menuitem->active);
}


void
on_details_view_menu_activate (GtkCheckMenuItem *menuitem,
			       gpointer          user_data)
{
	/* Avoids GTK Critical warning on startup
	   before the toolbar is created.
	*/
	if (!main_gui.details_view_button)
		return;

	gtk_toggle_tool_button_set_active (main_gui.details_view_button, menuitem->active);
}

void
on_details_tree_view_menu_activate (GtkCheckMenuItem *menuitem,
				    gpointer          user_data)
{
	/* Avoids GTK Critical warning on startup
	   before the toolbar is created.
	*/
	if (!main_gui.details_tree_view_button)
		return;

	gtk_toggle_tool_button_set_active (main_gui.details_tree_view_button, menuitem->active);
}

/* This function is called when the radio option defining the list mode is
   changed */
void     on_view_type_changed                   (GtkRadioAction *action,
                                                 gpointer       user_data)
{
	gint val;
	
	val = gtk_radio_action_get_current_value (action);
/*	
	switch (val)
		case 0:
// TODO			gtk_toggle_tool_button_set_active (main_gui.list_view_button, menuitem->active);
			break;
		case 1:
// TODO			gtk_toggle_tool_button_set_active (main_gui.list_tree_view_button, menuitem->active);
			break;
		case 2:
// TODO			gtk_toggle_tool_button_set_active (main_gui.details_view_button, menuitem->active);
			break;
		case 3:
// TODO			gtk_toggle_tool_button_set_active (main_gui.details_tree_view_button, menuitem->active);
			break;

	on_list_view_menu_activate
	on_list_tree_view_menu_activate
	on_details_view_menu_activate
	on_details_tree_view_menu_activate
*/
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
	gtk_widget_set_sensitive (GTK_WIDGET (main_gui.refresh_menu), FALSE);
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
	gtk_widget_set_sensitive (GTK_WIDGET (main_gui.refresh_menu), TRUE);
}

void
on_refresh_activate (GtkMenuItem     *menuitem,
		     gpointer         user_data)
{
	quick_refresh_list ();
}

void
on_refresh_clicked (GtkToolButton *button,
		    gpointer       user_data)
{
	quick_refresh_list ();
}


/* Main window menu: Option */
static void
color_selected  (GtkButton       *button,
                 gpointer         user_data)
{
	GdkColor mycolor;
	gtk_color_selection_get_current_color (GTK_COLOR_SELECTION (GTK_COLOR_SELECTION_DIALOG (user_data)->colorsel), &mycolor);
	gui_prefs.clone_color.red = mycolor.red;
	gui_prefs.clone_color.green = mycolor.green;
	gui_prefs.clone_color.blue = mycolor.blue;
	gtk_widget_destroy (user_data);
	create_gamelist_content ();
}

void
on_clone_color_menu_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	GdkColor mycolor;
	main_gui.clone_selector = gtk_color_selection_dialog_new (_("Clone color selection"));
	gtk_widget_hide (GTK_COLOR_SELECTION_DIALOG (main_gui.clone_selector)->help_button);
	gtk_window_set_transient_for (GTK_WINDOW (main_gui.clone_selector),GTK_WINDOW (MainWindow));
	gtk_window_set_modal (GTK_WINDOW (main_gui.clone_selector), TRUE);

	mycolor.pixel = 0;
	mycolor.red = (guint16) gui_prefs.clone_color.red;
	mycolor.green = (guint16) gui_prefs.clone_color.green;
	mycolor.blue = (guint16) gui_prefs.clone_color.blue;
	gtk_color_selection_set_current_color (GTK_COLOR_SELECTION (GTK_COLOR_SELECTION_DIALOG (main_gui.clone_selector)->colorsel), &mycolor);

	g_signal_connect (G_OBJECT (GTK_COLOR_SELECTION_DIALOG (main_gui.clone_selector)->ok_button), "clicked",
			  G_CALLBACK (color_selected),
			  (gpointer) main_gui.clone_selector);
	g_signal_connect_swapped (G_OBJECT (GTK_COLOR_SELECTION_DIALOG (main_gui.clone_selector)->cancel_button), "clicked",
				  G_CALLBACK (gtk_widget_destroy),
				  (gpointer) main_gui.clone_selector);
	g_signal_connect (G_OBJECT (main_gui.clone_selector), "delete_event",
			  G_CALLBACK (gtk_widget_destroy),
			  NULL);
	gtk_widget_show (main_gui.clone_selector);
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
	create_filterslist_content ();
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

/* Toolbar */
void
on_filterShowButton_toggled (GtkToggleToolButton *togglebutton,
			     gpointer             user_data)
{
	if (gtk_toggle_tool_button_get_active (togglebutton)) {
		GMAMEUI_DEBUG ("Show filters");
		/* the on_folder_list_activate callback will call show_filter ()*/
		gtk_check_menu_item_set_active (main_gui.folder_list_menu, TRUE);
	} else {
		GMAMEUI_DEBUG ("Hide filters");
		/* the on_folder_list_activate callback will call hide_filter ()*/
		gtk_check_menu_item_set_active (main_gui.folder_list_menu, FALSE);
	}
}

void
on_snapShowButton_toggled (GtkToggleToolButton *togglebutton,
			   gpointer             user_data)
{
	if (gtk_toggle_tool_button_get_active (togglebutton)) {
		GMAMEUI_DEBUG ("Show snaps");
		gtk_check_menu_item_set_active (main_gui.screen_shot_menu, TRUE);
	} else {
		GMAMEUI_DEBUG ("Hide snaps");
		gtk_check_menu_item_set_active (main_gui.screen_shot_menu, FALSE);
	}
}

void
on_mode_button_clicked (GtkToggleToolButton *button,
			gpointer             user_data)
{
	if (gtk_toggle_tool_button_get_active (button)) {
		if (gui_prefs.current_mode != (ListMode)GPOINTER_TO_INT (user_data)) {
			gui_prefs.previous_mode = gui_prefs.current_mode;
			gui_prefs.current_mode = GPOINTER_TO_INT (user_data);
			GMAMEUI_DEBUG ("Current mode changed %d --> %d", gui_prefs.previous_mode, gui_prefs.current_mode);

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

			switch (gui_prefs.previous_mode) {
			case (LIST):
				gtk_toggle_tool_button_set_active (main_gui.list_view_button, FALSE);
				break;
			case (LIST_TREE):
				gtk_toggle_tool_button_set_active (main_gui.list_tree_view_button, FALSE);
				break;
			case (DETAILS):
				gtk_toggle_tool_button_set_active (main_gui.details_view_button, FALSE);
				break;
			case (DETAILS_TREE):
				gtk_toggle_tool_button_set_active (main_gui.details_tree_view_button, FALSE);
				break;
			}

			if ( (gui_prefs.current_mode==LIST_TREE) || (gui_prefs.current_mode == DETAILS_TREE)) {
				gtk_widget_set_sensitive (GTK_WIDGET (main_gui.expand_all_menu), TRUE);
				gtk_widget_set_sensitive (GTK_WIDGET (main_gui.collapse_all_menu), TRUE);
			} else {
				gtk_widget_set_sensitive (GTK_WIDGET (main_gui.expand_all_menu), FALSE);
				gtk_widget_set_sensitive (GTK_WIDGET (main_gui.collapse_all_menu), FALSE);
			}

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
	} else {
		if (gui_prefs.current_mode == (ListMode)GPOINTER_TO_INT (user_data))
			gtk_toggle_tool_button_set_active (button, TRUE);
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


static gboolean
delayed_filter_row_selected (GtkTreeSelection *selection)
{
	gchar *text;
	simple_filter *current_folder_filter;
	simple_filter *folder_filter;
	GtkTreeIter iter;
	GtkTreeIter sub_iter;
	GtkTreeModel *model;
	GdkPixbuf *icon_pixbuf;
	gboolean valid;
	gboolean is_root;
	gint i;

	if (gtk_tree_selection_get_selected (selection, &model, &iter)) {
		gtk_tree_model_get (main_gui.filters_tree_model, &iter, 1, &current_folder_filter, 0, &text, -1);
		if ( (current_folder_filter->update_list)
		     && (current_filter != current_folder_filter)) {
			/* Set open pixbuf */
			icon_pixbuf = gmameui_get_icon_from_stock ("gmameui-folder-open");
			gtk_tree_store_set (GTK_TREE_STORE (main_gui.filters_tree_model), &iter,
					    2,		icon_pixbuf,
					    -1);
			/* Set normal pixbuf */
			valid = gtk_tree_model_get_iter_first (GTK_TREE_MODEL (main_gui.filters_tree_model), &iter);
			gtk_tree_model_get (GTK_TREE_MODEL (main_gui.filters_tree_model), &iter, 1, &folder_filter, -1);
			is_root = TRUE;
			i = 0;
			while ( (folder_filter!=current_filter) && (valid)) {
				if (gtk_tree_model_iter_has_child (GTK_TREE_MODEL (main_gui.filters_tree_model),&iter)) {
					if (gtk_tree_model_iter_children (GTK_TREE_MODEL (main_gui.filters_tree_model), &sub_iter, &iter)) {
						gtk_tree_model_get (GTK_TREE_MODEL (main_gui.filters_tree_model), &sub_iter, 1, &folder_filter, -1);
						is_root = FALSE;
						i++;
						while ((folder_filter != current_filter) && (gtk_tree_model_iter_next (GTK_TREE_MODEL (main_gui.filters_tree_model), &sub_iter))) {
							gtk_tree_model_get (GTK_TREE_MODEL (main_gui.filters_tree_model), &sub_iter, 1, &folder_filter, -1);
							i++;
						}
					}
				}
				if (folder_filter != current_filter) {
					valid = gtk_tree_model_iter_next (GTK_TREE_MODEL (main_gui.filters_tree_model), &iter);
					if (valid) {
						gtk_tree_model_get (GTK_TREE_MODEL (main_gui.filters_tree_model), &iter, 1, &folder_filter, -1);
						is_root = TRUE;
						i++;
					}
				}
			}
			icon_pixbuf = get_icon_for_filter (current_filter);
			if (is_root)
				gtk_tree_store_set (GTK_TREE_STORE (main_gui.filters_tree_model), &iter,
						    2,		icon_pixbuf,
						    -1);
			else
				gtk_tree_store_set (GTK_TREE_STORE (main_gui.filters_tree_model), &sub_iter,
						    2,		icon_pixbuf,
						    -1);
			/* Update the list */
			current_filter = current_folder_filter;
			create_gamelist_content ();
		}
		gui_prefs.FolderID = current_folder_filter->FolderID;
	}
	return FALSE;
}

/* Filter List */
void
on_filter_row_selected (GtkTreeSelection *selection,
			gpointer          data)
{
	if (timeoutfoldid)
		g_source_remove (timeoutfoldid);
	timeoutfoldid =
	    g_timeout_add (SELECT_TIMEOUT,
			 (GSourceFunc) delayed_filter_row_selected, selection);
}

void
on_filter_row_collapsed (GtkTreeView *treeview,
			 GtkTreeIter *iter,
			 GtkTreePath *path,
			 gpointer     user_data)
{
	GtkTreeIter iter_child;
	simple_filter *folder_filter;
	GtkTreeSelection *select;
	GtkTreeModel* treemodel;


	/* If one of the child iter is selected, we select the parent iter */
	treemodel = gtk_tree_view_get_model (treeview);
	gtk_tree_model_iter_children (treemodel, &iter_child, iter);
	gtk_tree_model_get (treemodel, &iter_child, 1, &folder_filter, -1);
	while ((current_filter != folder_filter) && (gtk_tree_model_iter_next (treemodel, &iter_child))) {
		gtk_tree_model_get (treemodel, &iter_child, 1, &folder_filter, -1);
	}

	if (current_filter == folder_filter) {
		GtkTreePath *tree_path;
		select = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));
		tree_path = gtk_tree_model_get_path (GTK_TREE_MODEL (treemodel), iter);
		gtk_tree_view_set_cursor (GTK_TREE_VIEW (treeview),
					  tree_path,
					  NULL, FALSE);
		gtk_tree_path_free (tree_path);
	}
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

	if (event->type == GDK_BUTTON_PRESS && ( (GdkEventButton*)event)->button == 3) {
		hide_column_menu_name = g_strdup_printf (_("Hide \"%s\" Column"), column_title (gtk_tree_view_column_get_sort_column_id (GTK_TREE_VIEW_COLUMN (column))));
		gtk_label_set_text (GTK_LABEL (GTK_BIN (main_gui.popup_column_hide)->child), hide_column_menu_name);
		g_free (hide_column_menu_name);
		ColumnHide_selected=gtk_tree_view_column_get_sort_column_id (GTK_TREE_VIEW_COLUMN (column));
		if (ColumnHide_selected == GAMENAME) {
			gtk_widget_set_sensitive (GTK_WIDGET (main_gui.popup_column_hide), FALSE);
		} else {
			gtk_widget_set_sensitive (GTK_WIDGET (main_gui.popup_column_hide), TRUE);
		}
		gtk_menu_popup (main_gui.popup_column_menu, NULL, NULL,
				NULL, NULL, event->button, event->time);
		return FALSE;
	}
	return FALSE;
}
