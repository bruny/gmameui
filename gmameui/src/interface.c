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
#include <glib-object.h>	/* For GParamSpec */

#include "callbacks.h"
#include "interface.h"
#include "gui.h"
#include "filters_list.h"
#include "gmameui.h"    /* For gui_prefs */

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


static void
on_hpaned_position_notify (GObject *object, GParamSpec *pspec, gpointer data)
{
	gint position;

	g_object_get(object, pspec->name, &position, NULL);
	GMAMEUI_DEBUG("position change report: object=%p, position value=%d, parameter=%p\n",
	       object, position, data);
	
	g_object_set (main_gui.gui_prefs,
		      "xpos-filters", main_gui.scrolled_window_filters->allocation.width,
		      "xpos-gamelist", main_gui.scrolled_window_games->allocation.width,
		      NULL);		
}


GtkWidget *
create_MainWindow (void)
{

  GtkWidget *MainWindow;
  GtkWidget *vbox1;
  GtkWidget *toolbar1;
  GtkWidget *hpanedLeft;
  GtkWidget *scrolledwindowFilters;
  GtkWidget *hpanedRight;
  GtkWidget *scrolledwindowGames;
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
	gchar *catver_file;
	
	GtkActionGroup *action_group;
	GError *error = NULL;
	
	GtkWidget *menubar;
	
  tooltips = gtk_tooltips_new ();

  accel_group = gtk_accel_group_new ();

  MainWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  g_object_set_data (G_OBJECT (MainWindow), "MainWindow", MainWindow);
  gtk_window_set_title (GTK_WINDOW (MainWindow), _("GMAMEUI Arcade Machine Emulator"));
  gtk_window_set_default_size (GTK_WINDOW (MainWindow), 640, 400);
	gtk_widget_set_events (GTK_WIDGET (MainWindow),
			       GDK_STRUCTURE_MASK);     /* Required to catch configure-event */

  vbox1 = gtk_vbox_new (FALSE, 1);
  gtk_widget_show (vbox1);
  gtk_container_add (GTK_CONTAINER (MainWindow), vbox1);
	
	/* Prepare the UI manager to build the menu and toolbar */
	main_gui.manager = gtk_ui_manager_new ();
	
	action_group = gtk_action_group_new ("GmameuiWindowAlwaysSensitiveActions");
	gtk_action_group_set_translation_domain (action_group, GETTEXT_PACKAGE);
	gtk_action_group_add_actions (action_group,
				      gmameui_always_sensitive_menu_entries,
				      G_N_ELEMENTS (gmameui_always_sensitive_menu_entries),
				      MainWindow);
	gtk_ui_manager_insert_action_group (main_gui.manager, action_group, 0);
	g_object_unref (action_group);
	/* TODO window->priv->always_sensitive_action_group = action_group;*/

	action_group = gtk_action_group_new ("GmameuiWindowExecROMActions");
	gtk_action_group_set_translation_domain (action_group, GETTEXT_PACKAGE);
	gtk_action_group_add_actions (action_group,
				      gmameui_rom_and_exec_menu_entries,
				      G_N_ELEMENTS (gmameui_rom_and_exec_menu_entries),
				      MainWindow);
	gtk_ui_manager_insert_action_group (main_gui.manager, action_group, 0);
	g_object_unref (action_group);
	main_gui.gmameui_rom_exec_action_group = action_group;
	
	action_group = gtk_action_group_new ("GmameuiWindowExecActions");
	gtk_action_group_set_translation_domain (action_group, GETTEXT_PACKAGE);
	gtk_action_group_add_actions (action_group,
				      gmameui_exec_menu_entries,
				      G_N_ELEMENTS (gmameui_exec_menu_entries),
				      MainWindow);
	gtk_ui_manager_insert_action_group (main_gui.manager, action_group, 0);
	g_object_unref (action_group);
	main_gui.gmameui_exec_action_group = action_group;	
	
	action_group = gtk_action_group_new ("GmameuiWindowROMActions");
	gtk_action_group_set_translation_domain (action_group, GETTEXT_PACKAGE);
	gtk_action_group_add_actions (action_group,
				      gmameui_rom_menu_entries,
				      G_N_ELEMENTS (gmameui_rom_menu_entries),
				      MainWindow);
	gtk_ui_manager_insert_action_group (main_gui.manager, action_group, 0);
	g_object_unref (action_group);
	main_gui.gmameui_rom_action_group = action_group;

	action_group = gtk_action_group_new ("GmameuiWindowFavouriteActions");
	gtk_action_group_set_translation_domain (action_group, GETTEXT_PACKAGE);
	gtk_action_group_add_actions (action_group,
				      gmameui_favourite_menu_entries,
				      G_N_ELEMENTS (gmameui_favourite_menu_entries),
				      MainWindow);
	gtk_ui_manager_insert_action_group (main_gui.manager, action_group, 0);
	g_object_unref (action_group);
	main_gui.gmameui_favourite_action_group = action_group;
	
	action_group = gtk_action_group_new ("GmameuiWindowViewActions");
	gtk_action_group_set_translation_domain (action_group, GETTEXT_PACKAGE);
	gtk_action_group_add_actions (action_group,
				      gmameui_view_expand_menu_entries,
				      G_N_ELEMENTS (gmameui_view_expand_menu_entries),
				      MainWindow);
	gtk_ui_manager_insert_action_group (main_gui.manager, action_group, 0);
	g_object_unref (action_group);
	main_gui.gmameui_view_action_group = action_group;
	
	action_group = gtk_action_group_new ("GmameuiWindowToggleActions");
	gtk_action_group_set_translation_domain (action_group, GETTEXT_PACKAGE);
	gtk_action_group_add_toggle_actions (action_group,
					     gmameui_view_toggle_menu_entries,
					     G_N_ELEMENTS (gmameui_view_toggle_menu_entries),
					     MainWindow);
	gtk_ui_manager_insert_action_group (main_gui.manager, action_group, 0);
	g_object_unref (action_group);

	action_group = gtk_action_group_new ("GmameuiWindowRadioActions");
	gtk_action_group_set_translation_domain (action_group, GETTEXT_PACKAGE);
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
	gtk_action_group_set_translation_domain (action_group, GETTEXT_PACKAGE);
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
	
	/* Set up the menu */
	menubar = gtk_ui_manager_get_widget (main_gui.manager, "/MenuBar");
	gtk_box_pack_start (GTK_BOX (vbox1), 
			    menubar, 
			    FALSE, 
			    FALSE, 
			    0);
	
	/* Dynamically add the list of known MAME executables to the toolbar */
	add_exec_menu ();

	/* Set up the toolbar */
	main_gui.toolbar = gtk_ui_manager_get_widget (main_gui.manager, "/ToolBar");
	gtk_box_pack_start (GTK_BOX (vbox1), 
			    main_gui.toolbar, 
			    FALSE, 
			    FALSE, 
			    0);
	
	GtkWidget *toolbar_widget;
	GtkWidget *toolbar_icon;
	toolbar_widget = gtk_ui_manager_get_widget (main_gui.manager,
						    "/ToolBar/FilePlayGame");
	gtk_tool_button_set_stock_id (toolbar_widget, GTK_STOCK_NEW);
	gtk_tool_button_set_label (toolbar_widget, N_("Play Game"));
	
	toolbar_widget = gtk_ui_manager_get_widget (main_gui.manager,
						    "/ToolBar/ViewFolderList");
	toolbar_icon = gmameui_get_image_from_stock ("gmameui-view-folders");
	gtk_tool_button_set_label (toolbar_widget, N_("Show Folders"));
	gtk_widget_show (toolbar_icon);
	gtk_tool_button_set_icon_widget (GTK_TOOL_BUTTON (toolbar_widget),
					 GTK_WIDGET (toolbar_icon));

	toolbar_widget = gtk_ui_manager_get_widget (main_gui.manager,
						    "/ToolBar/ViewSidebarPanel");
	toolbar_icon = gmameui_get_image_from_stock ("gmameui-view-screenshot");
	gtk_widget_show (toolbar_icon);
	gtk_tool_button_set_icon_widget (GTK_TOOL_BUTTON (toolbar_widget),
					 toolbar_icon);
	gtk_tool_button_set_label (toolbar_widget, N_("Show Sidebar"));
	
	toolbar_widget = gtk_ui_manager_get_widget (main_gui.manager,
						    "/ToolBar/ViewListView");
	toolbar_icon = gmameui_get_image_from_stock ("gmameui-view-list");
	gtk_tool_button_set_label (toolbar_widget, N_("List"));
	gtk_widget_show (toolbar_icon);
	gtk_tool_button_set_icon_widget (GTK_TOOL_BUTTON (toolbar_widget),
					 GTK_WIDGET (toolbar_icon));

	toolbar_widget = gtk_ui_manager_get_widget (main_gui.manager,
						    "/ToolBar/ViewTreeView");
	toolbar_icon = gmameui_get_image_from_stock ("gmameui-view-tree");
	gtk_tool_button_set_label (toolbar_widget, N_("List Tree"));
	gtk_widget_show (toolbar_icon);
	gtk_tool_button_set_icon_widget (GTK_TOOL_BUTTON (toolbar_widget),
					 GTK_WIDGET (toolbar_icon));
	
	toolbar_widget = gtk_ui_manager_get_widget (main_gui.manager,
						    "/ToolBar/ViewDetailsListView");
	toolbar_icon = gmameui_get_image_from_stock ("gmameui-view-list");
	gtk_tool_button_set_label (toolbar_widget, N_("Details"));
	gtk_widget_show (toolbar_icon);
	gtk_tool_button_set_icon_widget (GTK_TOOL_BUTTON (toolbar_widget),
					 GTK_WIDGET (toolbar_icon));
	
	toolbar_widget = gtk_ui_manager_get_widget (main_gui.manager,
						    "/ToolBar/ViewDetailsTreeView");
	toolbar_icon = gmameui_get_image_from_stock ("gmameui-view-tree");
	gtk_tool_button_set_label (toolbar_widget, N_("Details Tree"));
	gtk_widget_show (toolbar_icon);
	gtk_tool_button_set_icon_widget (GTK_TOOL_BUTTON (toolbar_widget),
					 GTK_WIDGET (toolbar_icon));
	
	/* Enable keyboard shortcuts defined in the UI Manager */
	gtk_window_add_accel_group (GTK_WINDOW (MainWindow),
				    gtk_ui_manager_get_accel_group (main_gui.manager));
	
	hpanedLeft = gtk_hpaned_new ();
	gtk_widget_show (hpanedLeft);
	main_gui.hpanedLeft = GTK_PANED (hpanedLeft);
	gtk_box_pack_start (GTK_BOX (vbox1), hpanedLeft, TRUE, TRUE, 0);
	gtk_paned_set_position (GTK_PANED (hpanedLeft), 150);
	g_signal_connect (G_OBJECT (hpanedLeft), "notify::position",
			  G_CALLBACK (on_hpaned_position_notify), NULL);
	
  scrolledwindowFilters = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindowFilters);
  main_gui.scrolled_window_filters = scrolledwindowFilters;
  gtk_paned_pack1 (GTK_PANED (hpanedLeft), scrolledwindowFilters, FALSE, FALSE);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindowFilters), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

	main_gui.filters_list = gmameui_filters_list_new ();
	gtk_container_add (scrolledwindowFilters, main_gui.filters_list);
	gtk_widget_show_all (main_gui.filters_list);

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
	
	/* Populate the filters list - this should be done in filters_list.c */
	
	/* Recent versions of MAME use neodrvr */
	static char *neogeo_value;
	GList *listpointer;
	if (xmame_compare_raw_version (current_exec, "0.116") >= 0)
		neogeo_value = "neodrvr";
	else
		neogeo_value = "neogeo"; 
	GMAMEUIFilter *folder_filter;
	
	/* Availability-related filters */
	folder_filter = gmameui_filter_new ();
	g_object_set (folder_filter,
		      "name", _("All ROMs"),
		      "folderid", ALL,
		      "type", DRIVER,
		      "is", FALSE,
		      "value", " ",
		      "int_value", 0,
		      "update_list", TRUE,
		      NULL);
		      
	gmameui_filters_list_add_filter (main_gui.filters_list, folder_filter, "Available");
	g_object_unref (folder_filter);
	
	folder_filter = gmameui_filter_new ();
	g_object_set (folder_filter,
		      "name", _("Available"),
		      "folderid", AVAILABLE,
		      "type", HAS_ROMS,
		      "is", FALSE,
		      "value", NULL,
		      "int_value", NOT_AVAIL,
		      "update_list", TRUE,
		      "pixbuf", gmameui_get_icon_from_stock ("gmameui-emblem-correct"),
		      NULL);
		      
	gmameui_filters_list_add_filter (main_gui.filters_list, folder_filter, "Available");
	/* Select the Available filter as the default upon startup */
	gmameui_filters_list_select (main_gui.filters_list, folder_filter);

	g_object_unref (folder_filter);
	
	folder_filter = gmameui_filter_new ();
		g_object_set (folder_filter,
		      "name", _("Unavailable"),
		      "folderid", UNAVAILABLE,
		      "type", HAS_ROMS,
		      "is", TRUE,
		      "value", NULL,
		      "int_value", NOT_AVAIL,
		      "update_list", TRUE,
		      "pixbuf", gmameui_get_icon_from_stock ("gmameui-emblem-unavailable"),
		      NULL);
		      
	gmameui_filters_list_add_filter (main_gui.filters_list, folder_filter, "Available");
	g_object_unref (folder_filter);
	
	folder_filter = gmameui_filter_new ();
			g_object_set (folder_filter,
		      "name", _("Incorrect"),
		      "folderid", FILTER_INCORRECT,
		      "type", HAS_ROMS,
		      "is", TRUE,
		      "value", NULL,
		      "int_value", INCORRECT,
		      "update_list", TRUE,
		      "pixbuf", gmameui_get_icon_from_stock ("gmameui-emblem-incorrect"),
		      NULL);
		      
	gmameui_filters_list_add_filter (main_gui.filters_list, folder_filter, "Available");
	g_object_unref (folder_filter);

	
		
	/* Architecture-related filters */
	folder_filter = gmameui_filter_new ();
		g_object_set (folder_filter,
		      "name", _("Neo-Geo"),
		      "folderid", NEOGEO,
		      "type", DRIVER,
		      "is", TRUE,
		      "value", neogeo_value,
		      "int_value", 0,
		      "update_list", TRUE,
		      NULL);
	gmameui_filters_list_add_filter (main_gui.filters_list, folder_filter, "Architecture");
	g_object_unref (folder_filter);

		folder_filter = gmameui_filter_new ();
		g_object_set (folder_filter,
		      "name", _("CPS1"),
		      "folderid", CPS1,
		      "type", DRIVER,
		      "is", TRUE,
		      "value", "cps1",
		      "int_value", 0,
		      "update_list", TRUE,
		      NULL);
	gmameui_filters_list_add_filter (main_gui.filters_list, folder_filter, "Architecture");
	g_object_unref (folder_filter);
	
	folder_filter = gmameui_filter_new ();
	g_object_set (folder_filter,
		      "name", _("CPS2"),
		      "folderid", CPS2,
		      "type", DRIVER,
		      "is", TRUE,
		      "value", "cps2",
		      "int_value", 0,
		      "update_list", TRUE,
		      NULL);
	gmameui_filters_list_add_filter (main_gui.filters_list, folder_filter, "Architecture");
	g_object_unref (folder_filter);

	folder_filter = gmameui_filter_new ();
	g_object_set (folder_filter,
		      "name", _("CPS3"),
		      "folderid", CPS3,
		      "type", DRIVER,
		      "is", TRUE,
		      "value", "cps3",
		      "int_value", 0,
		      "update_list", TRUE,
		      NULL);
	gmameui_filters_list_add_filter (main_gui.filters_list, folder_filter, "Architecture");
	g_object_unref (folder_filter);

	folder_filter = gmameui_filter_new ();
	g_object_set (folder_filter,
		      "name", _("Sega System 16"),
		      "folderid", SEGAS16,
		      "type", DRIVER,
		      "is", TRUE,
		      "value", "segas16b",
		      "int_value", 0,
		      "update_list", TRUE,
		      NULL);
	gmameui_filters_list_add_filter (main_gui.filters_list, folder_filter, "Architecture");
	g_object_unref (folder_filter);

	folder_filter = gmameui_filter_new ();
	g_object_set (folder_filter,
		      "name", _("Sega Model 2"),
		      "folderid", SEGAM2,
		      "type", DRIVER,
		      "is", TRUE,
		      "value", "model2",
		      "int_value", 0,
		      "update_list", TRUE,
		      NULL);
	gmameui_filters_list_add_filter (main_gui.filters_list, folder_filter, "Architecture");
	g_object_unref (folder_filter);
	
	folder_filter = gmameui_filter_new ();
	g_object_set (folder_filter,
		      "name", _("Namco System 22"),
		      "folderid", NAMCOS22,
		      "type", DRIVER,
		      "is", TRUE,
		      "value", "namcos22",
		      "int_value", 0,
		      "update_list", TRUE,
		      NULL);
	gmameui_filters_list_add_filter (main_gui.filters_list, folder_filter, "Architecture");
	g_object_unref (folder_filter);

	folder_filter = gmameui_filter_new ();
	g_object_set (folder_filter,
		      "name", _("BIOS"),
		      "folderid", FILTER_BIOS,
		      "type", IS_BIOS,
		      "is", TRUE,
		      "value", NULL,
		      "int_value", TRUE,
		      "update_list", TRUE,
		      NULL);
	gmameui_filters_list_add_filter (main_gui.filters_list, folder_filter, "Architecture");
	g_object_unref (folder_filter);
	
	/* Custom filters */
	folder_filter = gmameui_filter_new ();
		g_object_set (folder_filter,
		      "name", _("Favorites"),
		      "folderid", FAVORITES,
		      "type", FAVORITE,
		      "is", TRUE,
		      "value", NULL,
		      "int_value", 0,
		      "update_list", TRUE,
		      "pixbuf", gmameui_get_icon_from_stock ("gmameui-emblem-favorite"),
		      NULL);
	gmameui_filters_list_add_filter (main_gui.filters_list, folder_filter, "Custom");
	g_object_unref (folder_filter);
	
		folder_filter = gmameui_filter_new ();
		g_object_set (folder_filter,
		      "name", _("Played"),
		      "folderid", PLAYED,
		      "type", TIMESPLAYED,
		      "is", FALSE,
		      "value", NULL,
		      "int_value", 0,
		      "update_list", TRUE,
		      NULL);
	gmameui_filters_list_add_filter (main_gui.filters_list, folder_filter, "Custom");
	g_object_unref (folder_filter);
	
	
	/* Imperfect filters */
	folder_filter = gmameui_filter_new ();
	g_object_set (folder_filter,
		      "name", _("Colors"),
		      "folderid", IMPERFECT_COLORS,
		      "type", COLOR_STATUS,
		      "is", FALSE,
		      "value", NULL,
		      "int_value", DRIVER_STATUS_GOOD,
		      "update_list", TRUE,
		      "pixbuf", gmameui_get_icon_from_stock ("gmameui-emblem-not-working"),
		      NULL);
	gmameui_filters_list_add_filter (main_gui.filters_list, folder_filter, "Imperfect");
	g_object_unref (folder_filter);

	folder_filter = gmameui_filter_new ();
	g_object_set (folder_filter,
		      "name", _("Sound"),
		      "folderid", IMPERFECT_SOUND,
		      "type", SOUND_STATUS,
		      "is", FALSE,
		      "value", NULL,
		      "int_value", DRIVER_STATUS_GOOD,
		      "update_list", TRUE,
		      "pixbuf", gmameui_get_icon_from_stock ("gmameui-emblem-not-working"),
		      NULL);
	gmameui_filters_list_add_filter (main_gui.filters_list, folder_filter, "Imperfect");
	g_object_unref (folder_filter);

	folder_filter = gmameui_filter_new ();
	g_object_set (folder_filter,
		      "name", _("Graphics"),
		      "folderid", IMPERFECT_GRAPHIC,
		      "type", GRAPHIC_STATUS,
		      "is", FALSE,
		      "value", NULL,
		      "int_value", DRIVER_STATUS_GOOD,
		      "update_list", TRUE,
		      "pixbuf", gmameui_get_icon_from_stock ("gmameui-emblem-not-working"),
		      NULL);
	gmameui_filters_list_add_filter (main_gui.filters_list, folder_filter, "Imperfect");
	g_object_unref (folder_filter);

	/* Game information filters */
	folder_filter = gmameui_filter_new ();
	g_object_set (folder_filter,
		      "name", _("Originals"),
		      "folderid", ORIGINALS,
		      "type", CLONE,
		      "is", TRUE,
		      "value", "-",
		      "int_value", 0,
		      "update_list", TRUE,
		      NULL);
	gmameui_filters_list_add_filter (main_gui.filters_list, folder_filter, "Game Details");
	g_object_unref (folder_filter);

	folder_filter = gmameui_filter_new ();
	g_object_set (folder_filter,
		      "name", _("Clones"),
		      "folderid", CLONES,
		      "type", CLONE,
		      "is", FALSE,
		      "value", "-",
		      "int_value", 0,
		      "update_list", TRUE,
		      NULL);
	gmameui_filters_list_add_filter (main_gui.filters_list, folder_filter, "Game Details");
	g_object_unref (folder_filter);
	
	folder_filter = gmameui_filter_new ();
	g_object_set (folder_filter,
		      "name", _("Samples"),
		      "folderid", SAMPLES,
		      "type", HAS_SAMPLES,
		      "is", FALSE,
		      "value", NULL,
		      "int_value", 0,
		      "update_list", TRUE,
		      "pixbuf", gmameui_get_icon_from_stock ("gmameui-emblem-sound"),
		      NULL);
	gmameui_filters_list_add_filter (main_gui.filters_list, folder_filter, "Game Details");
	g_object_unref (folder_filter);
	
	folder_filter = gmameui_filter_new ();
	g_object_set (folder_filter,
		      "name", _("Stereo"),
		      "folderid", STEREO,
		      "type", CHANNELS,
		      "is", TRUE,
		      "value", NULL,
		      "int_value", 2,
		      "update_list", TRUE,
		      "pixbuf", gmameui_get_icon_from_stock ("gmameui-emblem-sound"),
		      NULL);
	gmameui_filters_list_add_filter (main_gui.filters_list, folder_filter, "Game Details");
	g_object_unref (folder_filter);
	
	folder_filter = gmameui_filter_new ();
	g_object_set (folder_filter,
		      "name", _("Raster"),
		      "folderid", RASTERS,
		      "type", VECTOR,
		      "is", FALSE,
		      "value", NULL,
		      "int_value", 0,
		      "update_list", TRUE,
		      NULL);
	gmameui_filters_list_add_filter (main_gui.filters_list, folder_filter, "Game Details");
	g_object_unref (folder_filter);	
	
	folder_filter = gmameui_filter_new ();
	g_object_set (folder_filter,
		      "name", _("Vector"),
		      "folderid", VECTORS,
		      "type", VECTOR,
		      "is", TRUE,
		      "value", NULL,
		      "int_value", 0,
		      "update_list", TRUE,
		      NULL);
	gmameui_filters_list_add_filter (main_gui.filters_list, folder_filter, "Game Details");
	g_object_unref (folder_filter);

	/* FIXME TODO
	folder_filter = gmameui_filter_new ();
	g_object_set (folder_filter,
		      "name", "Vertical",
		      "folderid", ORIENTATION,
		      "type", ORIENTATION,
		      "is", TRUE,
		      "value", "vertical",
		      "int_value", 0,
		      "update_list", TRUE,
		      NULL);
	gmameui_filters_list_add_filter (main_gui.filters_list, folder_filter, "Game Details");
	g_object_unref (folder_filter);

	folder_filter = gmameui_filter_new ();
	g_object_set (folder_filter,
		      "name", "Horizontal",
		      "folderid", ORIENTATION,
		      "type", ORIENTATION,
		      "is", TRUE,
		      "value", "horizontal",
		      "int_value", 0,
		      "update_list", TRUE,
		      NULL);
	gmameui_filters_list_add_filter (main_gui.filters_list, folder_filter, "Game Details");
	g_object_unref (folder_filter);*/
	
	folder_filter = gmameui_filter_new ();
	g_object_set (folder_filter,
		      "name", _("Trackball"),
		      "folderid", FILTER_CONTROL_TRACKBALL,
		      "type", CONTROL,
		      "is", TRUE,
		      "value", NULL,
		      "int_value", TRACKBALL,
		      "update_list", TRUE,
		      "pixbuf", gmameui_get_icon_from_stock ("gmameui-joystick"),
		      NULL);
	gmameui_filters_list_add_filter (main_gui.filters_list, folder_filter, "Game Details");
	g_object_unref (folder_filter);	

	folder_filter = gmameui_filter_new ();
	g_object_set (folder_filter,
		      "name", _("Lightgun"),
		      "folderid", FILTER_CONTROL_LIGHTGUN,
		      "type", CONTROL,
		      "is", TRUE,
		      "value", NULL,
		      "int_value", LIGHTGUN,
		      "update_list", TRUE,
		      "pixbuf", gmameui_get_icon_from_stock ("gmameui-joystick"),
		      NULL);
	gmameui_filters_list_add_filter (main_gui.filters_list, folder_filter, "Game Details");
	g_object_unref (folder_filter);	
	
	/* Catver.ini filters - Category and Version */
	g_object_get (main_gui.gui_prefs, "file-catver", &catver_file, NULL);
	
	if (catver_file) {
		/* Categories */
		for (listpointer = g_list_first (game_list.categories);
		     (listpointer);
		     listpointer = g_list_next (listpointer)) {
			     folder_filter = gmameui_filter_new ();
			     g_object_set (folder_filter,
					   "name", (gchar *)listpointer->data,
					   "folderid", CATEGORIES,
					   "type", CATEGORY,
					   "is", TRUE,
					   "value", (gchar *)listpointer->data,
					   "int_value", 0,
					   "update_list", TRUE,
					   NULL);
			     gmameui_filters_list_add_filter (main_gui.filters_list,
							      folder_filter,
							      "Category");
			     g_object_unref (folder_filter);
		}
		/* Version */
		for (listpointer = g_list_first (game_list.versions);
		     (listpointer);
		     listpointer = g_list_next (listpointer)) {
			     folder_filter = gmameui_filter_new ();
			     g_object_set (folder_filter,
					   "name", (gchar *)listpointer->data,
					   "folderid", VERSIONS,
					   "type", MAMEVER,
					   "is", TRUE,
					   "value", (gchar *)listpointer->data,
					   "int_value", 0,
					   "update_list", TRUE,
					   NULL);
			     gmameui_filters_list_add_filter (main_gui.filters_list,
							      folder_filter,
							      "Version");
			     g_object_unref (folder_filter);
		}
	}
	g_free (catver_file);

	
	hpanedRight = gtk_hpaned_new ();
	gtk_widget_show (hpanedRight);
	main_gui.hpanedRight = GTK_PANED (hpanedRight);
	gtk_paned_pack2 (GTK_PANED (hpanedLeft), hpanedRight, TRUE, FALSE);
	gtk_paned_set_position (GTK_PANED (hpanedRight), 300);
	g_signal_connect (G_OBJECT (hpanedRight), "notify::position",
			  G_CALLBACK (on_hpaned_position_notify), NULL);

	scrolledwindowGames = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (scrolledwindowGames);
	main_gui.scrolled_window_games = scrolledwindowGames;
	gtk_paned_pack1 (GTK_PANED (hpanedRight), scrolledwindowGames, TRUE, TRUE);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindowGames),
					GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

	/* Create the screenshot and history sidebar */
	GMAMEUISidebar *sidebar = gmameui_sidebar_new ();
	gtk_paned_pack2 (GTK_PANED (hpanedRight), sidebar, FALSE, FALSE);
	main_gui.screenshot_hist_frame = GMAMEUI_SIDEBAR (sidebar);

	g_signal_connect (G_OBJECT (MainWindow), "delete_event",
			  G_CALLBACK (on_MainWindow_delete_event),
			  NULL);

	g_object_set_data (G_OBJECT (MainWindow), "tooltips", tooltips);

	gtk_window_add_accel_group (GTK_WINDOW (MainWindow), accel_group);

	return MainWindow;
}

