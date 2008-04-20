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
#include <gtk/gtkhpaned.h>
#include <gtk/gtkhseparator.h>
#include <gtk/gtkimagemenuitem.h>
#include <gtk/gtklabel.h>
#include <gtk/gtkmenubar.h>
#include <gtk/gtkradiomenuitem.h>
#include <gtk/gtkscrolledwindow.h>
#include <gtk/gtkstock.h>
#include <gtk/gtkvbox.h>

#include "callbacks.h"
#include "interface.h"
#include "gui.h"

/* Callbacks for main window */

/* Close the main window */
static gboolean
on_MainWindow_delete_event (GtkWidget       *widget,
			    GdkEvent        *event,
			    gpointer         user_data)
{
	exit_gmameui ();
	return TRUE;
}

GtkWidget *
create_MainWindow (void)
{

  GtkWidget *MainWindow;
  GtkWidget *vbox1;
  GtkWidget *menubar1;
/* guint tmp_key;*/
  GtkWidget *file_menu;
  GtkWidget *file_menu_menu;
  GtkWidget *play;
  GtkWidget *separator9;
  GtkWidget *play_and_record_input;
  GtkWidget *playback_input;
  GtkWidget *separator3;
  GtkWidget *add_to_favorites;
  GtkWidget *remove_from_favorites;
  GtkWidget *separator2;
  GtkWidget *select_random_game;
  GtkWidget *separator14;
  GtkWidget *properties;
  GtkWidget *separator7;
  GtkWidget *audit_all_games;
  GtkWidget *separator8;
  GtkWidget *exit;
  GtkWidget *view_menu;
  GtkWidget *view_menu_menu;
  GtkWidget *toolbar_view_menu;
  GtkWidget *status_bar_view_menu;
  GtkWidget *folder_list;
  GtkWidget *screen_shot;
  GtkWidget *screen_shot_tab;
  GtkWidget *separator4;
  GSList *ViewMode_group = NULL;
/*  GtkWidget *large_icons_view_menu; */
/*  GtkWidget *small_icons_view_menu; */
  GtkWidget *list_view_menu;
  GtkWidget *list_tree_view_menu;
  GtkWidget *details_view_menu;
  GtkWidget *details_tree_view_menu;
  GtkWidget *separator13;
  GtkWidget *expand;
  GtkWidget *collapse;
  GtkWidget *separator5;
  GtkWidget *column_layout;
  GtkWidget *separator12;
  GtkWidget *the_prefix;
  GtkWidget *separator11;
  GtkWidget *refresh;
  GtkWidget *option_menu;
  GtkWidget *option_menu_menu;
  GtkWidget *clone_color_menu;
  GtkWidget *rebuild_game_list_menu;
  GtkWidget *separator6;
  GtkWidget *directories_menu;
  GtkWidget *default_option_menu;
  GtkWidget *startup_option;
  GtkWidget *separator10;
  GtkWidget *executable_title;
  GtkWidget *help_menu;
  GtkWidget *help_menu_menu;
  GtkWidget *aboutMenu;
  GtkWidget *toolbar1;
  GtkWidget *hpanedLeft;
  GtkWidget *scrolledwindowFilters;
  GtkWidget *hpanedRight;
  GtkWidget *scrolledwindowGames;
  GtkWidget *ScreenShot_hist_frame;
  GtkWidget *ScreenShot_hist_vbox;
  GtkWidget *tri_status_bar;
  GtkWidget *statusbar1;
  GtkWidget *statusbar2;
  GtkWidget *statusbar3;
  GtkWidget *combo_progress_bar;
  GtkWidget *status_progress_bar;
  GtkWidget *progress_progress_bar;
  GtkAccelGroup *accel_group;
  GtkTooltips *tooltips;
  PangoFontDescription *fontdesc;
  gint font_size;
	
	GtkActionGroup *action_group;
	GError *error = NULL;

  tooltips = gtk_tooltips_new ();

  accel_group = gtk_accel_group_new ();

  MainWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  g_object_set_data (G_OBJECT (MainWindow), "MainWindow", MainWindow);
  gtk_window_set_title (GTK_WINDOW (MainWindow), _("GMAMEUI Arcade Machine Emulator"));
  gtk_window_set_default_size (GTK_WINDOW (MainWindow), 640, 400);

  vbox1 = gtk_vbox_new (FALSE, 1);
  gtk_widget_show (vbox1);
  gtk_container_add (GTK_CONTAINER (MainWindow), vbox1);

  menubar1 = gtk_menu_bar_new ();
  gtk_widget_show (menubar1);
  gtk_box_pack_start (GTK_BOX (vbox1), menubar1, FALSE, FALSE, 0);

  file_menu = gtk_menu_item_new_with_mnemonic (_("_File"));
  gtk_widget_show (file_menu);
  gtk_container_add (GTK_CONTAINER (menubar1), file_menu);
  gtk_tooltips_set_tip (tooltips, file_menu, _("Contains commands for working with the selected item"), NULL);

  file_menu_menu = gtk_menu_new ();
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (file_menu), file_menu_menu);

  /* Play menu item */
  play = gtk_menu_item_new_with_mnemonic (_("_Play"));
  gtk_widget_show (play);
  main_gui.play_menu = GTK_MENU_ITEM (play);
  gtk_container_add (GTK_CONTAINER (file_menu_menu), play);
  gtk_tooltips_set_tip (tooltips, play, _("Play currently selected game"), NULL);
  gtk_widget_add_accelerator (play, "activate", accel_group,
                              GDK_Return, 0,
                              GTK_ACCEL_VISIBLE);

  separator9 = gtk_menu_item_new ();
  gtk_widget_show (separator9);
  gtk_container_add (GTK_CONTAINER (file_menu_menu), separator9);
  gtk_widget_set_sensitive (separator9, FALSE);

  play_and_record_input = gtk_image_menu_item_new_from_stock (GTK_STOCK_SAVE, accel_group);
  gtk_label_set_text_with_mnemonic (GTK_LABEL (GTK_BIN (play_and_record_input)->child), _("Play and _Record Input..."));

  gtk_widget_show (play_and_record_input);
  gtk_container_add (GTK_CONTAINER (file_menu_menu), play_and_record_input);
  gtk_tooltips_set_tip (tooltips, play_and_record_input, _("Record a game for later playback"), NULL);

  playback_input = gtk_image_menu_item_new_from_stock (GTK_STOCK_OPEN, accel_group);
  gtk_label_set_text_with_mnemonic (GTK_LABEL (GTK_BIN (playback_input)->child), _("P_layback Input..."));

  gtk_widget_show (playback_input);
  gtk_container_add (GTK_CONTAINER (file_menu_menu), playback_input);
  gtk_tooltips_set_tip (tooltips, playback_input, _("Playback a recorded game"), NULL);

  separator3 = gtk_menu_item_new ();
  gtk_widget_show (separator3);
  gtk_container_add (GTK_CONTAINER (file_menu_menu), separator3);
  gtk_widget_set_sensitive (separator3, FALSE);

  /* Add to favorites menu item */
  add_to_favorites = gtk_image_menu_item_new_from_stock (GTK_STOCK_ADD, NULL);
  gtk_label_set_text_with_mnemonic (GTK_LABEL (GTK_BIN (add_to_favorites)->child), _("A_dd to 'Favorites'"));

  gtk_widget_show (add_to_favorites);
  main_gui.add_to_favorites = GTK_MENU_ITEM (add_to_favorites);
  gtk_container_add (GTK_CONTAINER (file_menu_menu), add_to_favorites);
  gtk_tooltips_set_tip (tooltips, add_to_favorites, _("Add this game to your 'Favorites' game folder"), NULL);

  /* Remove from favorites menu item */
  remove_from_favorites = gtk_image_menu_item_new_from_stock (GTK_STOCK_REMOVE, NULL);
  gtk_label_set_text_with_mnemonic (GTK_LABEL (GTK_BIN (remove_from_favorites)->child), _("R_emove from 'Favorites'"));

  gtk_widget_show (remove_from_favorites);
  main_gui.remove_from_favorites = GTK_MENU_ITEM (remove_from_favorites);
  gtk_container_add (GTK_CONTAINER (file_menu_menu), remove_from_favorites);
  gtk_tooltips_set_tip (tooltips, remove_from_favorites, _("Remove this game from your 'Favorites' game folder"), NULL);

  separator2 = gtk_menu_item_new ();
  gtk_widget_show (separator2);
  gtk_container_add (GTK_CONTAINER (file_menu_menu), separator2);
  gtk_widget_set_sensitive (separator2, FALSE);

  select_random_game = gtk_menu_item_new_with_mnemonic (_("_Select Random Game"));
  gtk_widget_show (select_random_game);
  gtk_container_add (GTK_CONTAINER (file_menu_menu), select_random_game);
  gtk_tooltips_set_tip (tooltips, select_random_game, _("Randomly select a game"), NULL);

  separator14 = gtk_menu_item_new ();
  gtk_widget_show (separator14);
  gtk_container_add (GTK_CONTAINER (file_menu_menu), separator14);
  gtk_widget_set_sensitive (separator14, FALSE);

  /* Properties Menu Item */
  properties = gtk_image_menu_item_new_from_stock (GTK_STOCK_PROPERTIES, accel_group);
  gtk_widget_show (properties);
  main_gui.properties_menu = GTK_MENU_ITEM (properties);
  gtk_container_add (GTK_CONTAINER (file_menu_menu), properties);
  gtk_tooltips_set_tip (tooltips, properties, _("Display the properties of the selected game"), NULL);

  separator7 = gtk_menu_item_new ();
  gtk_widget_show (separator7);
  gtk_container_add (GTK_CONTAINER (file_menu_menu), separator7);
  gtk_widget_set_sensitive (separator7, FALSE);

  /* Audit all games menu item */
  audit_all_games = gtk_menu_item_new_with_mnemonic (_("_Audit All Games"));
  gtk_widget_show (audit_all_games);
  main_gui.audit_all_games_menu = GTK_MENU_ITEM (audit_all_games);
  gtk_container_add (GTK_CONTAINER (file_menu_menu), audit_all_games);
  gtk_tooltips_set_tip (tooltips, audit_all_games, _("Audit ROM and samples sets"), NULL);

  separator8 = gtk_menu_item_new ();
  gtk_widget_show (separator8);
  gtk_container_add (GTK_CONTAINER (file_menu_menu), separator8);
  gtk_widget_set_sensitive (separator8, FALSE);

  exit = gtk_image_menu_item_new_from_stock (GTK_STOCK_QUIT, accel_group);
  gtk_widget_show (exit);
  gtk_container_add (GTK_CONTAINER (file_menu_menu), exit);
  gtk_tooltips_set_tip (tooltips, exit, _("Exit the application"), NULL);

  view_menu = gtk_menu_item_new_with_mnemonic (_("_View"));
  gtk_widget_show (view_menu);
  gtk_container_add (GTK_CONTAINER (menubar1), view_menu);
  gtk_tooltips_set_tip (tooltips, view_menu, _("Contains commands for manipulating the view"), NULL);

  view_menu_menu = gtk_menu_new ();
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (view_menu), view_menu_menu);


  toolbar_view_menu = gtk_check_menu_item_new_with_mnemonic (_("_Toolbar"));
  gtk_widget_show (toolbar_view_menu);
  main_gui.toolbar_view_menu = GTK_CHECK_MENU_ITEM (toolbar_view_menu);
  gtk_container_add (GTK_CONTAINER (view_menu_menu), toolbar_view_menu);
  gtk_tooltips_set_tip (tooltips, toolbar_view_menu, _("Show or hide the toolbar"), NULL);
  gtk_widget_add_accelerator (toolbar_view_menu, "activate", accel_group,
                              GDK_T, GDK_MOD1_MASK,
                              GTK_ACCEL_VISIBLE);
  gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (toolbar_view_menu), TRUE);


  status_bar_view_menu = gtk_check_menu_item_new_with_mnemonic (_("_Status Bar"));
  gtk_widget_show (status_bar_view_menu);
  main_gui.status_bar_view_menu = GTK_CHECK_MENU_ITEM (status_bar_view_menu);
  gtk_container_add (GTK_CONTAINER (view_menu_menu), status_bar_view_menu);
  gtk_tooltips_set_tip (tooltips, status_bar_view_menu, _("Show or hide the status bar"), NULL);
  gtk_widget_add_accelerator (status_bar_view_menu, "activate", accel_group,
                              GDK_S, GDK_MOD1_MASK,
                              GTK_ACCEL_VISIBLE);
  gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (status_bar_view_menu), TRUE);


  /* Folder List */
  folder_list = gtk_check_menu_item_new_with_mnemonic (_("Fold_er List"));
  gtk_widget_show (folder_list);
  main_gui.folder_list_menu = GTK_CHECK_MENU_ITEM (folder_list);
  gtk_container_add (GTK_CONTAINER (view_menu_menu), folder_list);
  gtk_tooltips_set_tip (tooltips, folder_list, _("Show or hide the folder list"), NULL);
  gtk_widget_add_accelerator (folder_list, "activate", accel_group,
                              GDK_D, GDK_MOD1_MASK,
                              GTK_ACCEL_VISIBLE);
  gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (folder_list), TRUE);


  screen_shot = gtk_check_menu_item_new_with_mnemonic (_("Scree_nshot Panel"));
  gtk_widget_show (screen_shot);
  main_gui.screen_shot_menu = GTK_CHECK_MENU_ITEM (screen_shot);
  gtk_container_add (GTK_CONTAINER (view_menu_menu), screen_shot);
  gtk_tooltips_set_tip (tooltips, screen_shot, _("Show or hide the screenshot panel"), NULL);
  gtk_widget_add_accelerator (screen_shot, "activate", accel_group,
                              GDK_N, GDK_MOD1_MASK,
                              GTK_ACCEL_VISIBLE);
  gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (screen_shot), TRUE);


  screen_shot_tab = gtk_check_menu_item_new_with_mnemonic (_("Scree_nshot Panel Tab"));
  gtk_widget_show (screen_shot_tab);
  main_gui.screen_shot_tab_menu = GTK_CHECK_MENU_ITEM (screen_shot_tab);
  gtk_container_add (GTK_CONTAINER (view_menu_menu), screen_shot_tab);
  gtk_tooltips_set_tip (tooltips, screen_shot_tab, _("Show or hide the screenshot panel tabulation"), NULL);
  gtk_widget_add_accelerator (screen_shot_tab, "activate", accel_group,
                              GDK_O, GDK_MOD1_MASK,
                              GTK_ACCEL_VISIBLE);
  gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (screen_shot_tab), TRUE);


  separator4 = gtk_menu_item_new ();
  gtk_widget_show (separator4);
  gtk_container_add (GTK_CONTAINER (view_menu_menu), separator4);
  gtk_widget_set_sensitive (separator4, FALSE);

  /* List */
  list_view_menu = gtk_radio_menu_item_new_with_mnemonic (ViewMode_group, _("_List"));
  ViewMode_group = gtk_radio_menu_item_get_group (GTK_RADIO_MENU_ITEM (list_view_menu));
  gtk_widget_show (list_view_menu);
  main_gui.list_view_menu = GTK_CHECK_MENU_ITEM (list_view_menu);
  gtk_container_add (GTK_CONTAINER (view_menu_menu), list_view_menu);
  gtk_tooltips_set_tip (tooltips, list_view_menu, _("Displays items in a list."), NULL);

  /* List Tree */
  list_tree_view_menu = gtk_radio_menu_item_new_with_mnemonic (ViewMode_group, _("List _Tree"));
  ViewMode_group = gtk_radio_menu_item_get_group (GTK_RADIO_MENU_ITEM (list_tree_view_menu));
  gtk_widget_show (list_tree_view_menu);
  main_gui.list_tree_view_menu = GTK_CHECK_MENU_ITEM (list_tree_view_menu);
  gtk_container_add (GTK_CONTAINER (view_menu_menu), list_tree_view_menu);
  gtk_tooltips_set_tip (tooltips, list_tree_view_menu, _("Displays items in a tree list with clones indented."), NULL);
  gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (list_tree_view_menu), TRUE);


  /* Details */
  details_view_menu = gtk_radio_menu_item_new_with_mnemonic (ViewMode_group, _("_Details"));
  ViewMode_group = gtk_radio_menu_item_get_group (GTK_RADIO_MENU_ITEM (details_view_menu));
  gtk_widget_show (details_view_menu);
  main_gui.details_view_menu = GTK_CHECK_MENU_ITEM (details_view_menu);
  gtk_container_add (GTK_CONTAINER (view_menu_menu), details_view_menu);
  gtk_tooltips_set_tip (tooltips, details_view_menu, _("Displays detailed information about each item."), NULL);

  /* Details Tree */
  details_tree_view_menu = gtk_radio_menu_item_new_with_mnemonic (ViewMode_group, _("Detai_ls Tree"));
  ViewMode_group = gtk_radio_menu_item_get_group (GTK_RADIO_MENU_ITEM (details_tree_view_menu));
  gtk_widget_show (details_tree_view_menu);
  main_gui.details_tree_view_menu = GTK_CHECK_MENU_ITEM (details_tree_view_menu);
  gtk_container_add (GTK_CONTAINER (view_menu_menu), details_tree_view_menu);
  gtk_tooltips_set_tip (tooltips, details_tree_view_menu, _("Displays detailed information about each item with clones indented."), NULL);
  gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (details_tree_view_menu), TRUE);

  separator13 = gtk_menu_item_new ();
  gtk_widget_show (separator13);
  gtk_container_add (GTK_CONTAINER (view_menu_menu), separator13);
  gtk_widget_set_sensitive (separator13, FALSE);

  expand = gtk_menu_item_new_with_mnemonic (_("Expand All"));
  gtk_widget_show (expand);
  main_gui.expand_all_menu = GTK_MENU_ITEM (expand);
  gtk_container_add (GTK_CONTAINER (view_menu_menu), expand);
  gtk_tooltips_set_tip (tooltips, expand, _("Expand all rows"), NULL);

  collapse = gtk_menu_item_new_with_mnemonic (_("Collapse All"));
  gtk_widget_show (collapse);
  main_gui.collapse_all_menu = GTK_MENU_ITEM (collapse);
  gtk_container_add (GTK_CONTAINER (view_menu_menu), collapse);
  gtk_tooltips_set_tip (tooltips, collapse, _("Collapse all rows"), NULL);

  separator5 = gtk_menu_item_new ();
  gtk_widget_show (separator5);
  gtk_container_add (GTK_CONTAINER (view_menu_menu), separator5);
  gtk_widget_set_sensitive (separator5, FALSE);

  column_layout = gtk_menu_item_new_with_mnemonic (_("_Column Layout..."));
  gtk_widget_show (column_layout);
  gtk_container_add (GTK_CONTAINER (view_menu_menu), column_layout);
  gtk_tooltips_set_tip (tooltips, column_layout, _("Set column layout"), NULL);

  separator12 = gtk_menu_item_new ();
  gtk_widget_show (separator12);
  gtk_container_add (GTK_CONTAINER (view_menu_menu), separator12);
  gtk_widget_set_sensitive (separator12, FALSE);

  the_prefix = gtk_check_menu_item_new_with_mnemonic (_("The _Prefix"));
  gtk_widget_show (the_prefix);
  main_gui.modify_the_menu = GTK_CHECK_MENU_ITEM (the_prefix);
  gtk_container_add (GTK_CONTAINER (view_menu_menu), the_prefix);
  gtk_tooltips_set_tip (tooltips, the_prefix, _("Show \"The\" at the end."), NULL);
  gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (the_prefix), TRUE);


  separator11 = gtk_menu_item_new ();
  gtk_widget_show (separator11);
  gtk_container_add (GTK_CONTAINER (view_menu_menu), separator11);
  gtk_widget_set_sensitive (separator11, FALSE);

  refresh = gtk_image_menu_item_new_from_stock (GTK_STOCK_REFRESH, NULL);
  gtk_widget_show (refresh);
  main_gui.refresh_menu = GTK_MENU_ITEM (refresh);
  gtk_container_add (GTK_CONTAINER (view_menu_menu), refresh);
  gtk_tooltips_set_tip (tooltips, refresh, _("Refresh game list"), NULL);
  gtk_widget_add_accelerator (refresh, "activate", accel_group,
                              GDK_F5, 0,
                              GTK_ACCEL_VISIBLE);

  option_menu = gtk_menu_item_new_with_mnemonic (_("_Option"));
  gtk_widget_show (option_menu);
  gtk_container_add (GTK_CONTAINER (menubar1), option_menu);
  gtk_tooltips_set_tip (tooltips, option_menu, _("Contains commands for user interface options"), NULL);

  option_menu_menu = gtk_menu_new ();
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (option_menu), option_menu_menu);


  clone_color_menu = gtk_image_menu_item_new_from_stock (GTK_STOCK_SELECT_COLOR, accel_group);
  gtk_label_set_text_with_mnemonic (GTK_LABEL (GTK_BIN (clone_color_menu)->child), _("Clone _Color..."));
  gtk_widget_show (clone_color_menu);
  gtk_container_add (GTK_CONTAINER (option_menu_menu), clone_color_menu);
  gtk_tooltips_set_tip (tooltips, clone_color_menu, _("Set clones color in gamelist"), NULL);

  rebuild_game_list_menu = gtk_menu_item_new_with_mnemonic (_("_Rebuild Game List"));
  gtk_widget_show (rebuild_game_list_menu);
  gtk_container_add (GTK_CONTAINER (option_menu_menu), rebuild_game_list_menu);
  gtk_tooltips_set_tip (tooltips, rebuild_game_list_menu, _("Rebuild the game list from executable information"), NULL);

  separator6 = gtk_menu_item_new ();
  gtk_widget_show (separator6);
  gtk_container_add (GTK_CONTAINER (option_menu_menu), separator6);
  gtk_widget_set_sensitive (separator6, FALSE);

  directories_menu = gtk_menu_item_new_with_mnemonic (_("_Directories..."));
  gtk_widget_show (directories_menu);
  gtk_container_add (GTK_CONTAINER (option_menu_menu), directories_menu);
  gtk_tooltips_set_tip (tooltips, directories_menu, _("Set directory configuration"), NULL);

  default_option_menu = gtk_menu_item_new_with_mnemonic (_("Default _Options..."));
  gtk_widget_show (default_option_menu);
  gtk_container_add (GTK_CONTAINER (option_menu_menu), default_option_menu);
  gtk_tooltips_set_tip (tooltips, default_option_menu, _("Set default game options"), NULL);

  startup_option = gtk_image_menu_item_new_from_stock (GTK_STOCK_PREFERENCES, accel_group);
  gtk_label_set_text_with_mnemonic (GTK_LABEL (GTK_BIN (startup_option)->child), _("_Startup Options..."));
  gtk_widget_show (startup_option);
  gtk_container_add (GTK_CONTAINER (option_menu_menu), startup_option);
  gtk_tooltips_set_tip (tooltips, startup_option, _("Set startup options"), NULL);

  separator10 = gtk_menu_item_new ();
  gtk_widget_show (separator10);
  gtk_container_add (GTK_CONTAINER (option_menu_menu), separator10);
  gtk_widget_set_sensitive (separator10, FALSE);

  executable_title = gtk_menu_item_new_with_mnemonic (_("_Executable"));
  gtk_widget_show (executable_title);
  main_gui.executables_title = GTK_MENU_ITEM (executable_title);
  gtk_container_add (GTK_CONTAINER (option_menu_menu), executable_title);

  help_menu = gtk_menu_item_new_with_mnemonic (_("_Help"));
  gtk_widget_show (help_menu);
  gtk_container_add (GTK_CONTAINER (menubar1), help_menu);
  gtk_tooltips_set_tip (tooltips, help_menu, _("Contains commands for displaying help"), NULL);

  help_menu_menu = gtk_menu_new ();
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (help_menu), help_menu_menu);

  aboutMenu = gtk_menu_item_new_with_mnemonic (_("_About..."));
  gtk_widget_show (aboutMenu);
  gtk_container_add (GTK_CONTAINER (help_menu_menu), aboutMenu);
  gtk_tooltips_set_tip (tooltips, aboutMenu, _("Displays program and Copyright information"), NULL);

	
	main_gui.manager = gtk_ui_manager_new ();
	
	action_group = gtk_action_group_new ("GmameuiWindowAlwaysSensitiveActions");
	gtk_action_group_set_translation_domain (action_group, NULL);
	gtk_action_group_add_actions (action_group,
				      gmameui_always_sensitive_menu_entries,
				      G_N_ELEMENTS (gmameui_always_sensitive_menu_entries),
				      MainWindow);
	gtk_ui_manager_insert_action_group (main_gui.manager, action_group, 0);
	g_object_unref (action_group);
	/* TODO window->priv->always_sensitive_action_group = action_group;*/
	
	action_group = gtk_action_group_new ("GmameuiWindowROMActions");
	gtk_action_group_set_translation_domain (action_group, NULL);
	gtk_action_group_add_actions (action_group,
				      gmameui_rom_menu_entries,
				      G_N_ELEMENTS (gmameui_rom_menu_entries),
				      MainWindow);
	gtk_ui_manager_insert_action_group (main_gui.manager, action_group, 0);
	g_object_unref (action_group);
	main_gui.gmameui_rom_action_group = action_group;
	
	action_group = gtk_action_group_new ("GmameuiWindowToggleActions");
	gtk_action_group_set_translation_domain (action_group, NULL);
	gtk_action_group_add_toggle_actions (action_group,
					     gmameui_view_toggle_menu_entries,
					     G_N_ELEMENTS (gmameui_view_toggle_menu_entries),
					     MainWindow);
	gtk_ui_manager_insert_action_group (main_gui.manager, action_group, 0);
	g_object_unref (action_group);

	action_group = gtk_action_group_new ("GmameuiWindowToggleActions");
	gtk_action_group_set_translation_domain (action_group, NULL);
	gtk_action_group_add_radio_actions (action_group,
					    gmameui_view_radio_menu_entries,
					    G_N_ELEMENTS (gmameui_view_radio_menu_entries),
					    0,  /* TODO */
					    G_CALLBACK (on_view_type_changed),
					    MainWindow);
	gtk_ui_manager_insert_action_group (main_gui.manager, action_group, 0);
	g_object_unref (action_group);

	/* Column popup */
	action_group = gtk_action_group_new ("GmameuiColumnPopupActions");
	gtk_action_group_set_translation_domain (action_group, NULL);
	gtk_action_group_add_actions (action_group,
				      gmameui_column_entries,
				      G_N_ELEMENTS (gmameui_column_entries),
				      MainWindow);
	gtk_ui_manager_insert_action_group (main_gui.manager, action_group, 0);
	g_object_unref (action_group);
	
	/* Now load the UI definition */
	gtk_ui_manager_add_ui_from_file (main_gui.manager, GMAMEUI_UI_DIR "gmameui-ui.xml", &error);
	if (error != NULL)
	{
		GMAMEUI_DEBUG ("Error loading gmameui-ui.xml: %s", error->message);
		g_error_free (error);
	}
	
	/* TODO
	menubar = gtk_ui_manager_get_widget (main_gui.manager, "/MenuBar");
	gtk_box_pack_start (GTK_BOX (vbox1), 
			    menubar, 
			    FALSE, 
			    FALSE, 
			    0);*/
	
  toolbar1 = gtk_toolbar_new ();
  gtk_toolbar_set_orientation (GTK_TOOLBAR (toolbar1), GTK_ORIENTATION_HORIZONTAL);
  gtk_toolbar_set_style (GTK_TOOLBAR (toolbar1), GTK_TOOLBAR_BOTH);
  gtk_widget_show (toolbar1);
  main_gui.toolbar = GTK_TOOLBAR (toolbar1);
  gtk_box_pack_start (GTK_BOX (vbox1), toolbar1, FALSE, FALSE, 0);

  hpanedLeft = gtk_hpaned_new ();
  gtk_widget_show (hpanedLeft);
  main_gui.hpanedLeft = GTK_PANED (hpanedLeft);
  gtk_box_pack_start (GTK_BOX (vbox1), hpanedLeft, TRUE, TRUE, 0);
  gtk_paned_set_position (GTK_PANED (hpanedLeft), 150);
  scrolledwindowFilters = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindowFilters);
  main_gui.scrolled_window_filters = scrolledwindowFilters;
  gtk_paned_pack1 (GTK_PANED (hpanedLeft), scrolledwindowFilters, FALSE, FALSE);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindowFilters), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  hpanedRight = gtk_hpaned_new ();
  gtk_widget_show (hpanedRight);
  main_gui.hpanedRight = GTK_PANED (hpanedRight);
  gtk_paned_pack2 (GTK_PANED (hpanedLeft), hpanedRight, TRUE, FALSE);
  gtk_paned_set_position (GTK_PANED (hpanedRight), 300);

  scrolledwindowGames = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindowGames);
  main_gui.scrolled_window_games = scrolledwindowGames;
  gtk_paned_pack1 (GTK_PANED (hpanedRight), scrolledwindowGames, TRUE, TRUE);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindowGames), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

	/* Create the screenshot and history sidebar */
	GMAMEUISidebar *sidebar = gmameui_sidebar_new ();
	gtk_paned_pack2 (GTK_PANED (hpanedRight), sidebar, FALSE, FALSE);
	main_gui.screenshot_hist_frame = GMAMEUI_SIDEBAR (sidebar);
	
	
  tri_status_bar = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (tri_status_bar);
  main_gui.tri_status_bar = tri_status_bar;
  gtk_box_pack_start (GTK_BOX (vbox1), tri_status_bar, FALSE, FALSE, 0);

  statusbar1 = gtk_statusbar_new ();
  gtk_widget_show (statusbar1);
  main_gui.statusbar1 = GTK_STATUSBAR (statusbar1);
  gtk_box_pack_start (GTK_BOX (tri_status_bar), statusbar1, TRUE, TRUE, 0);

  fontdesc = pango_font_description_copy (GTK_WIDGET (tri_status_bar)->style->font_desc);
  font_size = pango_font_description_get_size (fontdesc);

  statusbar2 = gtk_statusbar_new ();
  gtk_widget_show (statusbar2);
  main_gui.statusbar2 = GTK_STATUSBAR (statusbar2);
  gtk_box_pack_start (GTK_BOX (tri_status_bar), statusbar2, FALSE, FALSE, 0);
  gtk_widget_set_size_request (statusbar2, PANGO_PIXELS (font_size) * 10, -1);

  statusbar3 = gtk_statusbar_new ();
  gtk_widget_show (statusbar3);
  main_gui.statusbar3 = GTK_STATUSBAR (statusbar3);
  gtk_box_pack_end (GTK_BOX (tri_status_bar), statusbar3, FALSE, FALSE, 0);
  gtk_widget_set_size_request (statusbar3, PANGO_PIXELS (font_size) * 10, -1);

  combo_progress_bar = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (combo_progress_bar);
  main_gui.combo_progress_bar = combo_progress_bar;
  gtk_box_pack_start (GTK_BOX (vbox1), combo_progress_bar, FALSE, FALSE, 0);

  status_progress_bar = gtk_statusbar_new ();
  gtk_widget_show (status_progress_bar);
  main_gui.status_progress_bar = GTK_STATUSBAR (status_progress_bar);
  gtk_box_pack_start (GTK_BOX (combo_progress_bar), status_progress_bar, TRUE, TRUE, 0);

  progress_progress_bar = gtk_progress_bar_new ();
  gtk_widget_show (progress_progress_bar);
  main_gui.progress_progress_bar = GTK_PROGRESS_BAR (progress_progress_bar);
  gtk_box_pack_end (GTK_BOX (combo_progress_bar), progress_progress_bar, TRUE, TRUE, 0);

  g_signal_connect (G_OBJECT (MainWindow), "delete_event",
                      G_CALLBACK (on_MainWindow_delete_event),
                      NULL);
  g_signal_connect (G_OBJECT (play), "activate",
                      G_CALLBACK (on_play_activate),
                      NULL);
  g_signal_connect (G_OBJECT (play_and_record_input), "activate",
                      G_CALLBACK (on_play_and_record_input_activate),
                      NULL);
  g_signal_connect (G_OBJECT (playback_input), "activate",
                      G_CALLBACK (on_playback_input_activate),
                      NULL);
  g_signal_connect (G_OBJECT (select_random_game), "activate",
                      G_CALLBACK (on_select_random_game_activate),
                      NULL);
  g_signal_connect (G_OBJECT (add_to_favorites), "activate",
                      G_CALLBACK (on_add_to_favorites_activate),
                      NULL);
  g_signal_connect (G_OBJECT (remove_from_favorites), "activate",
                      G_CALLBACK (on_remove_from_favorites_activate),
                      NULL);
  g_signal_connect (G_OBJECT (properties), "activate",
                      G_CALLBACK (on_properties_activate),
                      NULL);
  g_signal_connect (G_OBJECT (audit_all_games), "activate",
                      G_CALLBACK (on_audit_all_games_activate),
                      NULL);
  g_signal_connect (G_OBJECT (exit), "activate",
                      G_CALLBACK (on_exit_activate),
                      NULL);
  g_signal_connect (G_OBJECT (toolbar_view_menu), "activate",
                      G_CALLBACK (on_toolbar_view_menu_activate),
                      NULL);
  g_signal_connect (G_OBJECT (status_bar_view_menu), "activate",
                      G_CALLBACK (on_status_bar_view_menu_activate),
                      NULL);
  g_signal_connect (G_OBJECT (folder_list), "activate",
                      G_CALLBACK (on_folder_list_activate),
                      NULL);
  g_signal_connect (G_OBJECT (screen_shot), "activate",
                      G_CALLBACK (on_screen_shot_activate),
                      NULL);
  g_signal_connect (G_OBJECT (screen_shot_tab), "activate",
                      G_CALLBACK (on_screen_shot_tab_activate),
                      NULL);
  g_signal_connect (G_OBJECT (list_view_menu), "activate",
                      G_CALLBACK (on_list_view_menu_activate),
                      NULL);
  g_signal_connect (G_OBJECT (list_tree_view_menu), "activate",
                      G_CALLBACK (on_list_tree_view_menu_activate),
                      NULL);
  g_signal_connect (G_OBJECT (details_view_menu), "activate",
                      G_CALLBACK (on_details_view_menu_activate),
                      NULL);
  g_signal_connect (G_OBJECT (details_tree_view_menu), "activate",
                      G_CALLBACK (on_details_tree_view_menu_activate),
                      NULL);
  g_signal_connect (G_OBJECT (expand), "activate",
                      G_CALLBACK (on_expand_all_activate),
                      NULL);
  g_signal_connect (G_OBJECT (collapse), "activate",
                      G_CALLBACK (on_collapse_all_activate),
                      NULL);
  g_signal_connect (G_OBJECT (column_layout), "activate",
                      G_CALLBACK (on_column_layout_activate),
                      NULL);
  g_signal_connect (G_OBJECT (the_prefix), "activate",
                      G_CALLBACK (on_the_prefix_activate),
                      NULL);
  g_signal_connect (G_OBJECT (refresh), "activate",
                      G_CALLBACK (on_refresh_activate),
                      NULL);
  g_signal_connect (G_OBJECT (clone_color_menu), "activate",
                      G_CALLBACK (on_clone_color_menu_activate),
                      NULL);
/*  g_signal_connect (G_OBJECT (game_list_font), "activate",
                      G_CALLBACK (on_game_list_font_activate),
                      NULL);*/
  g_signal_connect (G_OBJECT (rebuild_game_list_menu), "activate",
                      G_CALLBACK (on_rebuild_game_list_menu_activate),
                      NULL);
  g_signal_connect (G_OBJECT (directories_menu), "activate",
                      G_CALLBACK (on_directories_menu_activate),
                      NULL);
  g_signal_connect (G_OBJECT (default_option_menu), "activate",
                      G_CALLBACK (on_default_option_menu_activate),
                      NULL);
  g_signal_connect (G_OBJECT (startup_option), "activate",
                      G_CALLBACK (on_startup_option_activate),
                      NULL);
  g_signal_connect (G_OBJECT (aboutMenu), "activate",
                      G_CALLBACK (on_about_activate),
                      NULL);

  g_object_set_data (G_OBJECT (MainWindow), "tooltips", tooltips);

  gtk_window_add_accel_group (GTK_WINDOW (MainWindow), accel_group);

  return MainWindow;
}

