/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * GMAMEUI
 *
 * Copyright 2007-2009 Andrew Burton <adb@iinet.net.au>
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

#include <unistd.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtkvbox.h>
#include <glib-object.h>	/* For GParamSpec */

#include "callbacks.h"
#include "interface.h"
#include "gui.h"
#include "filters_list.h"
#include "gmameui.h"    /* For gui_prefs */
#include "gmameui-search-entry.h"
#include "gmameui-statusbar.h"

static void
on_filter_btn_toggled (GtkWidget *widget, gpointer user_data);

/* Callbacks for main window */

/* Close the main window */
static gboolean
on_main_window_delete_event (GtkWidget *widget,
			     GdkEvent *event,
			     gpointer user_data)
{
	exit_gmameui ();
	return TRUE;
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

/* When the draggable panes are changed, save their position */
void
on_hpaned_position_notify (GObject *object, GParamSpec *pspec, gpointer data)
{
	gint position;

	g_object_get(object, pspec->name, &position, NULL);
	GMAMEUI_DEBUG("position change report: object=%p, position value=%d, parameter=%p",
	       object, position, data);

	g_object_set (main_gui.gui_prefs,
		      "xpos-filters", main_gui.scrolled_window_filters->allocation.width,
		      "xpos-gamelist", main_gui.scrolled_window_games->allocation.width,
		      NULL);
}

static void
on_filter_btn_toggled (GtkWidget *widget, gpointer user_data)
{
	const gchar *widgetname = gtk_widget_get_name (widget);
	GMAMEUI_DEBUG ("Filter for widget %s clicked", widgetname);
	if (g_ascii_strcasecmp (widgetname, "filter_btn_all") == 0) {
		g_object_set (main_gui.gui_prefs, "current-rom-filter", 0, NULL);
	} else if (g_ascii_strcasecmp (widgetname, "filter_btn_avail") == 0) {
		g_object_set (main_gui.gui_prefs, "current-rom-filter", 1, NULL);
	} else if (g_ascii_strcasecmp (widgetname, "filter_btn_unavail") == 0) {
		g_object_set (main_gui.gui_prefs, "current-rom-filter", 2, NULL);
	} 

	mame_gamelist_view_update_filter (main_gui.displayed_list);
}

static GtkWidget *
get_filter_btn_by_id (GladeXML *xml, gint i)
{
	GtkWidget *radio = NULL;
	
	if (i == 0)
		radio = glade_xml_get_widget (xml, "filter_btn_all");
	else if (i == 1)
		radio = glade_xml_get_widget (xml, "filter_btn_avail");
	else if (i == 2)
		radio = glade_xml_get_widget (xml, "filter_btn_unavail");

	return radio;
}

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

	g_object_set (main_gui.gui_prefs, "show-toolbar", visible, NULL);
}

void
on_folder_list_activate (GtkAction *action,
			 gpointer         user_data)
{
	gboolean visible;

	visible = gtk_toggle_action_get_active (GTK_TOGGLE_ACTION (action));

	g_object_set (main_gui.gui_prefs, "show-filterlist", visible, NULL);
	
	if (visible) {
		/* Show filters */
		gint xpos_filters;
		g_object_get (main_gui.gui_prefs, "xpos-filters", &xpos_filters, NULL);
		gtk_paned_set_position (main_gui.hpanedLeft, xpos_filters);

		gtk_widget_show (GTK_WIDGET (main_gui.scrolled_window_filters));
	} else {
		/* Hide filters */
		gtk_paned_set_position (main_gui.hpanedLeft, 0);

		gtk_widget_hide (GTK_WIDGET (main_gui.scrolled_window_filters));
	}

}

void
on_screen_shot_activate (GtkAction *action,
			 gpointer         user_data)
{
	gboolean visible;

	visible = gtk_toggle_action_get_active (GTK_TOGGLE_ACTION (action));
	
	g_object_set (main_gui.gui_prefs, "show-screenshot", visible, NULL);

	if (visible) {
		/* Show snapshot */
		gint xpos_gamelist;
		g_object_get (main_gui.gui_prefs, "xpos-gamelist", &xpos_gamelist, NULL);
		gtk_paned_set_position (main_gui.hpanedRight, xpos_gamelist);

		gtk_widget_show (GTK_WIDGET (main_gui.screenshot_hist_frame));
	} else {
		/* Hide snapshot */
		gtk_paned_set_position (main_gui.hpanedRight, -1);
		gtk_widget_hide (GTK_WIDGET (main_gui.screenshot_hist_frame));
	}
}

void
on_status_bar_view_menu_activate       (GtkAction *action,
                                        gpointer         user_data)
{
	gboolean visible;

	visible = gtk_toggle_action_get_active (GTK_TOGGLE_ACTION (action));

	if (visible)
		gtk_widget_show (GTK_WIDGET (main_gui.statusbar));
	else
		gtk_widget_hide (GTK_WIDGET (main_gui.statusbar));

	g_object_set (main_gui.gui_prefs, "show-statusbar", visible, NULL);
}

/* This function is called when the toggle action defining the list mode is changed. */
void
on_view_type_changed (GtkToggleAction *action, gpointer user_data)
{
	guint val;
	ListMode current_mode;
	ListMode previous_mode;
	
	g_object_get (main_gui.gui_prefs,
		      "current-mode", &current_mode,
		      "previous-mode", &previous_mode,
		      NULL);
	
	val = gtk_toggle_action_get_active (action);

	if (current_mode != val) {
		previous_mode = current_mode;
		current_mode = val;
		GMAMEUI_DEBUG ("Current mode changed %d --> %d", previous_mode, current_mode);
		g_object_set (main_gui.gui_prefs,
			      "current-mode", current_mode,
			      "previous-mode", previous_mode,
			      NULL);

		mame_gamelist_view_change_views (main_gui.displayed_list);

	}	 
}

GtkWidget *
create_MainWindow (void)
{

	GtkAccelGroup *accel_group;
	GtkTooltips *tooltips;
	
	GtkActionGroup *action_group;
	GError *error = NULL;
	
	GtkWidget *menubar;
	
	GtkWidget *main_window;
	GtkWidget *vbox;
	
	gint ui_width, ui_height;
	gint xpos_filters, xpos_gamelist;
	gint show_filters, show_screenshot, show_flyer;
	gint show_statusbar, show_toolbar;
	gint current_mode;
GMAMEUI_DEBUG ("Setting up main window...");
	g_object_get (main_gui.gui_prefs,
		      "ui-width", &ui_width,
		      "ui-height", &ui_height,
		      "show-filterlist", &show_filters,
		      "show-screenshot", &show_screenshot,
		      "show-flyer", &show_flyer,
		      "show-statusbar", &show_statusbar,
		      "show-toolbar", &show_toolbar,
		      "current-mode", &current_mode,    /* FIXME TODO Rename to show-details */
		      "xpos-filters", &xpos_filters,
		      "xpos-gamelist", &xpos_gamelist,
		      NULL);
	
	GladeXML *xml = glade_xml_new (GLADEDIR "filter_bar.glade", "filter_hbox", GETTEXT_PACKAGE);
	if (!xml) {
		GMAMEUI_DEBUG ("Could not open Glade file %s", GLADEDIR "filter_bar.glade");
		return NULL;
	}
	main_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	g_object_set_data (G_OBJECT (main_window), "MainWindow", main_window);
	
	vbox = gtk_vbox_new (FALSE, 0);
	gtk_container_add (GTK_CONTAINER (main_window), vbox);
	
	tooltips = gtk_tooltips_new ();
	g_object_set_data (G_OBJECT (main_window), "tooltips", tooltips);

	accel_group = gtk_accel_group_new ();

	gtk_window_set_title (GTK_WINDOW (main_window), _("GMAMEUI Arcade Machine Emulator"));
	gtk_window_set_default_size (GTK_WINDOW (main_window), ui_width, ui_height);
	gtk_widget_set_events (GTK_WIDGET (main_window),
			       GDK_STRUCTURE_MASK);     /* Required to catch configure-event */
	
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
	
	action_group = gtk_action_group_new ("GmameuiWindowToggleActions");
	gtk_action_group_set_translation_domain (action_group, GETTEXT_PACKAGE);
	gtk_action_group_add_toggle_actions (action_group,
					     gmameui_view_toggle_menu_entries,
					     G_N_ELEMENTS (gmameui_view_toggle_menu_entries),
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
		error = NULL;
	}
	
	/* Set up the menu */
	menubar = gtk_ui_manager_get_widget (main_gui.manager, "/MenuBar");
	gtk_box_pack_start (GTK_BOX (vbox), menubar, FALSE, FALSE, 0);	
	
	/* Dynamically add the list of known MAME executables to the toolbar */
	add_exec_menu ();
	
	/* Set up the toolbar */
	main_gui.toolbar = gtk_ui_manager_get_widget (main_gui.manager, "/ToolBar");
	gtk_box_pack_start (GTK_BOX (vbox), GTK_WIDGET (main_gui.toolbar), FALSE, FALSE, 0);
	
	GtkWidget *toolbar_widget;
	GtkWidget *toolbar_icon;

	int z;
	for (z = 0; z < NUM_TOOLBAR_ITEMS; z++) {
		toolbar_widget = gtk_ui_manager_get_widget (main_gui.manager,
							    toolbar_items[z].ui_name);
		if (toolbar_items[z].stock_id != 0)
			gtk_tool_button_set_stock_id (GTK_TOOL_BUTTON (toolbar_widget), toolbar_items[z].stock_id);
		else {
			toolbar_icon = gmameui_get_image_from_stock (toolbar_items[z].icon_name);
			gtk_widget_show (toolbar_icon);
			gtk_tool_button_set_icon_widget (GTK_TOOL_BUTTON (toolbar_widget), GTK_WIDGET (toolbar_icon));
		}
		gtk_tool_button_set_label (GTK_TOOL_BUTTON (toolbar_widget), _(toolbar_items[z].label));
	}

	/* Set state of check menu and toolbar widgets */
	gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (gtk_ui_manager_get_action (main_gui.manager,
				      "/MenuBar/ViewMenu/ViewSidebarPanelMenu")),
				      show_screenshot);

	gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (gtk_ui_manager_get_action (main_gui.manager,
				      "/MenuBar/ViewMenu/ViewFolderListMenu")),
				      show_filters);

	gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (gtk_ui_manager_get_action (main_gui.manager,
				      "/MenuBar/ViewMenu/ViewStatusBarMenu")),
				      show_statusbar);
	gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (gtk_ui_manager_get_action (main_gui.manager,
				      "/MenuBar/ViewMenu/ViewToolbarMenu")),
				      show_toolbar);

	gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (gtk_ui_manager_get_action (main_gui.manager,
				      "/MenuBar/ViewMenu/ViewDetailsListViewMenu")),
	                              current_mode);
	
	main_gui.hpanedLeft = gtk_hpaned_new ();
	main_gui.hpanedRight = gtk_hpaned_new ();
	
	/* Set up the search field */
GMAMEUI_DEBUG ("    Setting up search entry field...");
	GtkWidget *search_box;
	search_box = glade_xml_get_widget (xml, "filter_hbox");
	main_gui.search_entry = mame_search_entry_new ();
	
	/* In order to pack before the filter buttons, their pack must be set to End */
	gtk_box_pack_start (GTK_BOX (search_box), main_gui.search_entry, FALSE, FALSE, 6);
	gtk_widget_show (main_gui.search_entry);
GMAMEUI_DEBUG ("    Setting up search entry field... done");

	/* Prepare the ROM availability filter buttons */
GMAMEUI_DEBUG ("    Setting up ROM availability filter buttons...");
	GList *filter_btn_list, *list;
	gint current_filter_btn;
	
	filter_btn_list = glade_xml_get_widget_prefix (xml, "filter_btn_");
	g_object_get (main_gui.gui_prefs, "current-rom-filter", &current_filter_btn, NULL);

	for (list = g_list_first (filter_btn_list);
	     list != NULL;
	     list = g_list_next (list)) {	
		/* Hide the radio circle so only the button is visible */
		gtk_toggle_button_set_mode (GTK_TOGGLE_BUTTON (list->data), FALSE);

		/* Set the signal callback */
		g_signal_connect (G_OBJECT (list->data), "toggled",
				  G_CALLBACK (on_filter_btn_toggled), NULL);
	}

	/* Set the button based on the preference */
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (get_filter_btn_by_id (xml,
	                                                                       current_filter_btn)),
				      TRUE);
GMAMEUI_DEBUG ("    Setting up ROM availability filter buttons... done");	
	/* End ROM availability filter buttons */
	
	/* Enable keyboard shortcuts defined in the UI Manager */
	gtk_window_add_accel_group (GTK_WINDOW (main_window), accel_group);     /* FIXME TODO Is this one required? */
	gtk_window_add_accel_group (GTK_WINDOW (main_window),
				    gtk_ui_manager_get_accel_group (main_gui.manager));

	/* Add filter list on LHS */
GMAMEUI_DEBUG ("    Setting up LHS filters list...");	
	main_gui.scrolled_window_filters = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (main_gui.scrolled_window_filters),
	                                GTK_POLICY_AUTOMATIC,
	                                GTK_POLICY_AUTOMATIC);
	main_gui.filters_list = gmameui_filters_list_new ();
	gtk_container_add (GTK_CONTAINER (main_gui.scrolled_window_filters), GTK_WIDGET (main_gui.filters_list));
GMAMEUI_DEBUG ("    Setting up LHS filters list... done");

	/* Add placeholder for the games list in middle */
	main_gui.scrolled_window_games = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (main_gui.scrolled_window_games),
	                                     GTK_SHADOW_IN);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (main_gui.scrolled_window_games),
	                                GTK_POLICY_AUTOMATIC,
	                                GTK_POLICY_AUTOMATIC);

	GtkWidget *box2 = gtk_vbox_new (FALSE, 0);
	gtk_paned_pack1 (GTK_PANED (main_gui.hpanedRight), box2, TRUE, TRUE);

	gtk_box_pack_start (GTK_BOX (box2), search_box, FALSE, FALSE, 6);
	
	/* Add screenshot and history sidebar on RHS */
	GtkWidget *sidebar = gmameui_sidebar_new ();
	main_gui.screenshot_hist_frame = GMAMEUI_SIDEBAR (sidebar);
	gtk_paned_pack2 (main_gui.hpanedRight, GTK_WIDGET (sidebar), TRUE, TRUE);
	
	/* Create the UI of the Game List */
GMAMEUI_DEBUG ("    Setting up gamelist view...");
	main_gui.displayed_list = mame_gamelist_view_new ();
	
	gtk_container_add (GTK_CONTAINER (main_gui.scrolled_window_games), GTK_WIDGET (main_gui.displayed_list));
	gtk_widget_show_all (main_gui.scrolled_window_games);
GMAMEUI_DEBUG ("    Setting up gamelist view... done");
	gtk_box_pack_start (GTK_BOX (box2), main_gui.scrolled_window_games, TRUE, TRUE, 6);

	gtk_box_pack_start (GTK_BOX (vbox), GTK_WIDGET (main_gui.hpanedLeft), TRUE, TRUE, 6);
	gtk_paned_add1 (GTK_PANED (main_gui.hpanedLeft), main_gui.scrolled_window_filters);
	gtk_paned_add2 (GTK_PANED (main_gui.hpanedLeft), GTK_WIDGET (main_gui.hpanedRight));
	
	gtk_paned_set_position (GTK_PANED (main_gui.hpanedLeft), xpos_filters);

	/* Set up the status bar at the bottom */
GMAMEUI_DEBUG ("    Setting up statusbar...");
	main_gui.statusbar = gmameui_statusbar_new ();
	gtk_box_pack_end (GTK_BOX (vbox), GTK_WIDGET (main_gui.statusbar), FALSE, FALSE, 0);
GMAMEUI_DEBUG ("    Setting up statusbar... done");

	/* Update UI if executables or entries in the MameGamelistView are
	   not available */
	gmameui_ui_set_items_sensitive ();

	/* Set callbacks that trigger the position to be saved when moved */
	g_signal_connect (G_OBJECT (main_gui.hpanedLeft), "notify::position",
			  G_CALLBACK (on_hpaned_position_notify), NULL);
	
	gtk_paned_set_position (GTK_PANED (main_gui.hpanedRight), xpos_gamelist);
	g_signal_connect (G_OBJECT (main_gui.hpanedRight), "notify::position",
			  G_CALLBACK (on_hpaned_position_notify), NULL);
	
	/* Connect signals */
	g_signal_connect (G_OBJECT (main_window), "delete_event",
			  G_CALLBACK (on_main_window_delete_event), NULL);

	g_signal_connect (G_OBJECT (main_window), "configure_event",
			  G_CALLBACK (on_main_window_moved_cb),
			  NULL);

GMAMEUI_DEBUG ("Setting up main window... done");
	
	return main_window;
}
