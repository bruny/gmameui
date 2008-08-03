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
#include <gdk/gdkkeysyms.h>
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
	
	g_object_set (main_gui.gui_prefs,
		      "show-screenshot", visible,
		      NULL);

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
		gtk_widget_show (GTK_WIDGET (main_gui.tri_status_bar));
	else
		gtk_widget_hide (GTK_WIDGET (main_gui.tri_status_bar));

	g_object_set (main_gui.gui_prefs, "show-statusbar", visible, NULL);
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

void
set_status_bar (gchar *game_name, gchar *game_status)
{
	gtk_statusbar_pop (main_gui.statusbar1, 1);
	gtk_statusbar_push (main_gui.statusbar1, 1, game_name);

	gtk_statusbar_pop (main_gui.statusbar2, 1);
	gtk_statusbar_push (main_gui.statusbar2, 1, game_status);
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

GtkWidget *
create_MainWindow (void)
{

	/* FIXME TODO
	 GtkWidget *combo_progress_bar;
	 GtkWidget *status_progress_bar;
	 GtkWidget *progress_progress_bar;*/

	GtkAccelGroup *accel_group;
	GtkTooltips *tooltips;

	gchar *catver_file;
	
	GtkActionGroup *action_group;
	GError *error = NULL;
	
	GtkWidget *menubar;
	
	GtkWidget *main_window;
	GtkWidget *vbox;
	PangoFontDescription *fontdesc;
	gint font_size;
	
	gint ui_width, ui_height;
	gint xpos_filters, xpos_gamelist;
	gint show_filters, show_screenshot, show_flyer;
	gint show_statusbar, show_toolbar;
	gint current_mode;
		
	g_object_get (main_gui.gui_prefs,
		      "ui-width", &ui_width,
		      "ui-height", &ui_height,
		      "show-filterlist", &show_filters,
		      "show-screenshot", &show_screenshot,
		      "show-flyer", &show_flyer,
		      "show-statusbar", &show_statusbar,
		      "show-toolbar", &show_toolbar,
		      "current-mode", &current_mode,
		      "xpos-filters", &xpos_filters,
		      "xpos-gamelist", &xpos_gamelist,
		      NULL);
	
	GladeXML *xml = glade_xml_new (GLADEDIR "main_gui.glade", "window1", GETTEXT_PACKAGE);
	if (!xml) {
		GMAMEUI_DEBUG ("Could not open Glade file %s", GLADEDIR "main_gui.glade");
		return NULL;
	}
	main_window = glade_xml_get_widget (xml, "window1");
	g_object_set_data (G_OBJECT (main_window), "MainWindow", main_window);
	
	vbox = glade_xml_get_widget (xml, "vbox");
	
	/* Set up the status bar at the bottom */
	main_gui.tri_status_bar = glade_xml_get_widget (xml, "tri_status_bar");
	main_gui.statusbar1 = glade_xml_get_widget (xml, "statusbar1");
	main_gui.statusbar2 = glade_xml_get_widget (xml, "statusbar2");
	main_gui.statusbar3 = glade_xml_get_widget (xml, "statusbar3");
	
	fontdesc = pango_font_description_copy (GTK_WIDGET (main_gui.tri_status_bar)->style->font_desc);
	font_size = pango_font_description_get_size (fontdesc);
	
	gtk_widget_set_size_request (main_gui.statusbar2, PANGO_PIXELS (font_size) * 20, -1);
	gtk_widget_set_size_request (main_gui.statusbar3, PANGO_PIXELS (font_size) * 20, -1);
	
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
		gtk_tool_button_set_label (GTK_TOOL_BUTTON (toolbar_widget), toolbar_items[z].label);
	}
	
	main_gui.hpanedLeft = glade_xml_get_widget (xml, "hpanedLeft");
	main_gui.hpanedRight = glade_xml_get_widget (xml, "hpanedRight");
	
	main_gui.filters_list = gmameui_filters_list_new ();

/* FIXME TODO
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
	 
	gtk_widget_hide (GTK_WIDGET (main_gui.combo_progress_bar));*/

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
		GList *catlist, *verlist;
		/* Categories */
		catlist = mame_gamelist_get_categories_glist (gui_prefs.gl);
		for (listpointer = g_list_first (catlist);
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
		verlist = mame_gamelist_get_versions_glist (gui_prefs.gl);
		for (listpointer = g_list_first (verlist);
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
	gtk_widget_show (main_gui.filters_list);
	g_free (catver_file);
	
	/* Enable keyboard shortcuts defined in the UI Manager */
	gtk_window_add_accel_group (GTK_WINDOW (main_window), accel_group);     /* FIXME TODO Is this one required? */
	gtk_window_add_accel_group (GTK_WINDOW (main_window),
				    gtk_ui_manager_get_accel_group (main_gui.manager));
	
	/* Add filter list on LHS */
	main_gui.scrolled_window_filters = glade_xml_get_widget (xml, "scrolledwindowFilters");
	gtk_container_add (GTK_CONTAINER (main_gui.scrolled_window_filters), GTK_WIDGET (main_gui.filters_list));
	
	/* Add placeholder for the games list in middle */
	main_gui.scrolled_window_games = glade_xml_get_widget (xml, "scrolledwindowGames");
	
	/* Add screenshot and history sidebar on RHS */
	GMAMEUISidebar *sidebar = gmameui_sidebar_new ();
	main_gui.screenshot_hist_frame = GMAMEUI_SIDEBAR (sidebar);
	gtk_paned_pack2 (main_gui.hpanedRight, GTK_WIDGET (sidebar), TRUE, TRUE);
	
	/* Set state of radio/check menu and toolbar widgets */
	gmameui_menu_set_view_mode_check (current_mode, TRUE);
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

	if (! ((current_mode == LIST_TREE) || (current_mode == DETAILS_TREE))) {
		gtk_action_group_set_sensitive (main_gui.gmameui_view_action_group, FALSE);
	}
	
	/* Show the main window and all its children */
	//gtk_widget_show_all (GTK_WIDGET (main_window));

	/* New stuff starts here */
	
	/* Create the UI of the Game List */
	create_gamelist (current_mode);
#ifdef ENABLE_DEBUG
//g_message (_("Time to create gamelist: %.02f seconds"), g_timer_elapsed (mytimer, NULL));
#endif
	/* Feed the Game List */
	create_gamelist_content ();
#ifdef ENABLE_DEBUG
//g_message (_("Time to create gamelist content: %.02f seconds"), g_timer_elapsed (mytimer, NULL));
#endif
	
	
	gtk_paned_set_position (GTK_PANED (main_gui.hpanedLeft), xpos_filters);
	g_signal_connect (G_OBJECT (main_gui.hpanedLeft), "notify::position",
			  G_CALLBACK (on_hpaned_position_notify), NULL);
	
	gtk_paned_set_position (GTK_PANED (main_gui.hpanedRight), xpos_gamelist);
	g_signal_connect (G_OBJECT (main_gui.hpanedRight), "notify::position",
			  G_CALLBACK (on_hpaned_position_notify), NULL);
	
	/* New stuff finishes here */
	
	
	/* Connect signals */
	g_signal_connect (G_OBJECT (main_window), "delete_event",
			  G_CALLBACK (on_main_window_delete_event), NULL);

	g_signal_connect (G_OBJECT (main_window), "configure_event",
			  G_CALLBACK (on_main_window_moved_cb),
			  NULL);
	
	return main_window;
}

