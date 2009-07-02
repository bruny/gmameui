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

	mame_gamelist_view_repopulate_contents (main_gui.displayed_list);
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
		gtk_widget_show (GTK_WIDGET (main_gui.tri_status_bar));
	else
		gtk_widget_hide (GTK_WIDGET (main_gui.tri_status_bar));

	g_object_set (main_gui.gui_prefs, "show-statusbar", visible, NULL);
}

/* This function is called when the radio option defining the list mode is changed. */
void
on_view_type_changed (GtkRadioAction *action, gpointer user_data)
{
	guint val;
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
#ifdef TREESTORE
		gtk_action_group_set_sensitive (main_gui.gmameui_view_action_group,
						(current_mode == LIST_TREE) ||
						(current_mode == DETAILS_TREE));
#endif
		mame_gamelist_view_change_views (main_gui.displayed_list);

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
#ifdef TREESTORE
		case (LIST_TREE):
			widget = gtk_ui_manager_get_widget (main_gui.manager,
							    "/MenuBar/ViewMenu/ViewTreeViewMenu");
			break;
#endif
		case (DETAILS):
			widget = gtk_ui_manager_get_widget (main_gui.manager,
							    "/MenuBar/ViewMenu/ViewDetailsListViewMenu");
			break;
#ifdef TREESTORE
		case (DETAILS_TREE):
			widget = gtk_ui_manager_get_widget (main_gui.manager,
							    "/MenuBar/ViewMenu/ViewDetailsTreeViewMenu");
			break;
#endif
	}
	gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (widget), state);
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

typedef struct _folder_filter folder_filter;

struct _folder_filter {
	gint type;
	gint filterid;
	gchar *name;
	gboolean is;
	gchar *text_value;
	gint int_value;
	gboolean update_list;   // FIXME TODO Is this used?
	gchar *pixbuf;
	gchar *category;	// FIXME TODO Is this used?
};

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
	
	guint i;

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
	
	gtk_widget_set_size_request (GTK_WIDGET (main_gui.statusbar2), PANGO_PIXELS (font_size) * 20, -1);
	gtk_widget_set_size_request (GTK_WIDGET (main_gui.statusbar3), PANGO_PIXELS (font_size) * 20, -1);
	
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
#ifdef TREESTORE	
	action_group = gtk_action_group_new ("GmameuiWindowViewActions");
	gtk_action_group_set_translation_domain (action_group, GETTEXT_PACKAGE);
	gtk_action_group_add_actions (action_group,
				      gmameui_view_expand_menu_entries,
				      G_N_ELEMENTS (gmameui_view_expand_menu_entries),
				      MainWindow);
	gtk_ui_manager_insert_action_group (main_gui.manager, action_group, 0);
	g_object_unref (action_group);
	main_gui.gmameui_view_action_group = action_group;
#endif	
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
		gtk_tool_button_set_label (GTK_TOOL_BUTTON (toolbar_widget), _(toolbar_items[z].label));
	}
	
	main_gui.hpanedLeft = glade_xml_get_widget (xml, "hpanedLeft");
	main_gui.hpanedRight = glade_xml_get_widget (xml, "hpanedRight");
	
	/* Set up the search field */
GMAMEUI_DEBUG ("Setting up search entry field...");
	GtkWidget *search_box;
	search_box = glade_xml_get_widget (xml, "filter_hbox");
	main_gui.search_entry = mame_search_entry_new ();
	/* In order to pack before the filter buttons, their pack must be set to End */
	gtk_box_pack_start (GTK_BOX (search_box), main_gui.search_entry, FALSE, FALSE, 6);
	gtk_widget_show (main_gui.search_entry);
GMAMEUI_DEBUG ("Setting up search entry field... done");

	/* Prepare the ROM availability filter buttons */
GMAMEUI_DEBUG ("Setting up ROM availability filter buttons...");
	
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
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (get_filter_btn_by_id (xml, current_filter_btn)),
				      TRUE);

GMAMEUI_DEBUG ("Setting up ROM availability filter buttons... done");	
	/* End ROM availability filter buttons */
		
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
	MameExec *exec = mame_exec_list_get_current_executable (main_gui.exec_list);
	static char *neogeo_value;
	GList *listpointer;

	if (mame_compare_raw_version (exec, "0.116") >= 0)
		neogeo_value = "neodrvr";
	else
		neogeo_value = "neogeo"; 

	folder_filter folder_filters [] = {
		/* Availability-related filters */
		{ DRIVER, FILTER_ALL, _("All ROMs"), FALSE, " ", 0, TRUE, NULL, _("Available") },
		{ HAS_ROMS, FILTER_AVAILABLE, _("Available"), FALSE, NULL, NOT_AVAIL, TRUE, NULL, _("Available") },
		{ HAS_ROMS, FILTER_UNAVAILABLE, _("Unavailable"), TRUE, NULL, NOT_AVAIL, TRUE, "gmameui-emblem-unavailable", _("Available") },
	
		/* Status-related filters */
		{ HAS_ROMS, FILTER_CORRECT, _("Correct"), TRUE, NULL, CORRECT, TRUE, "gmameui-emblem-correct", _("Status") },
		{ HAS_ROMS, FILTER_BEST_AVAIL, _("Best Available"), TRUE, NULL, BEST_AVAIL, TRUE, "gmameui-emblem-not-working", _("Status") },
		{ HAS_ROMS, FILTER_INCORRECT, _("Incorrect"), TRUE, NULL, INCORRECT, TRUE, "gmameui-emblem-incorrect", _("Status") },
	
		/* Architecture-related filters */
		{ DRIVER, FILTER_ARCH_NEOGEO, _("Neo-Geo"), TRUE, neogeo_value, 0, TRUE, NULL, _("Architecture") },	
		{ DRIVER, FILTER_ARCH_CPS1, _("CPS1"), TRUE, "cps1", 0, TRUE, NULL, _("Architecture") },
		{ DRIVER, FILTER_ARCH_CPS2, _("CPS2"), TRUE, "cps2", 0, TRUE, NULL, _("Architecture") },	
		{ DRIVER, FILTER_ARCH_CPS3, _("CPS3"), TRUE, "cps3", 0, TRUE, NULL, _("Architecture") },
		{ DRIVER, FILTER_ARCH_SEGAS16, _("Sega System 16"), TRUE, "segas16b", 0, TRUE, NULL, _("Architecture") },
		{ DRIVER, FILTER_ARCH_SEGAM2, _("Sega Model 2"), TRUE, "model2", 0, TRUE, NULL, _("Architecture") },
		{ DRIVER, FILTER_ARCH_NAMCOS22, _("Namco System 22"), TRUE, "namcos22", 0, TRUE, NULL, _("Architecture") },
		{ IS_BIOS, FILTER_ARCH_BIOS, _("BIOS"), TRUE, NULL, TRUE, TRUE, NULL, _("Architecture") },
	
		/* Imperfect filters */
		{ DRIVER_STATUS, FILTER_IMPERFECT_DRIVER, _("Driver"), FALSE, NULL, DRIVER_STATUS_GOOD, TRUE, "gmameui-emblem-not-working", _("Imperfect") },
		{ COLOR_STATUS, FILTER_IMPERFECT_COLORS, _("Colors"), FALSE, NULL, DRIVER_STATUS_GOOD, TRUE, "gmameui-emblem-not-working", _("Imperfect") },
		{ SOUND_STATUS, FILTER_IMPERFECT_SOUND, _("Sound"), FALSE, NULL, DRIVER_STATUS_GOOD, TRUE, "gmameui-emblem-not-working", _("Imperfect") },
		{ GRAPHIC_STATUS, FILTER_IMPERFECT_GRAPHIC, _("Graphics"), FALSE, NULL, DRIVER_STATUS_GOOD, TRUE, "gmameui-emblem-not-working", _("Imperfect") },

		/* Game Details filters */
		{ CLONE, FILTER_DETAILS_ORIGINALS, _("Originals"), TRUE, "-", 0, TRUE, NULL, _("Game Details") },
		{ CLONE, FILTER_DETAILS_CLONES, _("Clones"), FALSE, "-", 0, TRUE, NULL, _("Game Details") },
		{ HAS_SAMPLES, FILTER_DETAILS_SAMPLES, _("Samples"), FALSE, NULL, 0, TRUE, "gmameui-emblem-sound", _("Game Details") },
		{ CHANNELS, FILTER_DETAILS_STEREO, _("Stereo"), TRUE, NULL, 2, TRUE, "gmameui-emblem-sound", _("Game Details") },
		{ VECTOR, FILTER_DETAILS_RASTERS, _("Raster"), FALSE, NULL, 0, TRUE, NULL, _("Game Details") },
		{ VECTOR, FILTER_DETAILS_VECTORS, _("Vector"), TRUE, NULL, 0, TRUE, NULL, _("Game Details") },
/*TODO		{ ORIENTATION, FILTER_ORIENTATION_V, _("Vertical"), TRUE, "vertical", 0, TRUE, NULL, _("Game Details") },
		{ ORIENTATION, FILTER_ORIENTATION_H, _("Horizontal"), TRUE, "horizontal", 0, TRUE, NULL, _("Game Details") },*/
		{ CONTROL, FILTER_CONTROL_TRACKBALL, _("Trackball"), TRUE, NULL, TRACKBALL, TRUE, NULL, _("Game Details") },
		{ CONTROL, FILTER_CONTROL_LIGHTGUN, _("Lightgun"), TRUE, NULL, LIGHTGUN, TRUE, NULL, _("Game Details") },

		/* Custom filters */
		{ FAVORITE, FILTER_CUSTOM_FAVORITES, _("Favorites"), TRUE, NULL, 0, TRUE, "gmameui-emblem-favorite", _("Custom") },
		{ TIMESPLAYED, FILTER_CUSTOM_PLAYED, _("Played"), FALSE, NULL, 0, TRUE, NULL, _("Custom") },
	};
	
	GMAMEUIFilter *folder_filter, *avail_folder_filter;

	/* Create the list of filters, and add each one to the sidebar */
	for (i = 0; i < G_N_ELEMENTS (folder_filters); i++) {
		folder_filter = gmameui_filter_new ();
		g_object_set (folder_filter,
			      "name", folder_filters[i].name,
			      "folderid", folder_filters[i].filterid,
			      "type", folder_filters[i].type,
			      "is", folder_filters[i].is,
			      "value", folder_filters[i].text_value,
			      "int_value", folder_filters[i].int_value,
			      "update_list", folder_filters[i].update_list,
			      NULL);
		if (folder_filters[i].pixbuf)
			g_object_set (folder_filter,
				      "pixbuf", gmameui_get_icon_from_stock (folder_filters[i].pixbuf),
				      NULL);
	
		GMAMEUI_DEBUG ("Creating new filter: %s", folder_filters[i].name);
	
		gmameui_filters_list_add_filter (main_gui.filters_list, folder_filter, folder_filters[i].category);
		GMAMEUI_DEBUG ("Added filter %s to list", folder_filters[i].name);
	
		/* Set the filter to iterate to within the tree afterwards. If we
		   try to iterate to the first filter to be added, it doesn't
		   work - FIXME TODO */
		if (folder_filters[i].filterid == FILTER_CORRECT) {
			avail_folder_filter = folder_filter;
		}
	
		g_object_unref (folder_filter);
	}

	/* Select the Correct filter as the default upon startup */
	gmameui_filters_list_select (main_gui.filters_list, avail_folder_filter);
	/* FIXME TODO Causes a segfault: g_object_unref (avail_folder_filter); */

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
					   "folderid", FILTER_CATEGORIES,
					   "type", CATEGORY,
					   "is", TRUE,
					   "value", (gchar *)listpointer->data,
					   "int_value", 0,
					   "update_list", TRUE,
					   NULL);
			     gmameui_filters_list_add_filter (main_gui.filters_list,
							      folder_filter,
							      _("Category"));
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
					   "folderid", FILTER_VERSIONS,
					   "type", MAMEVER,
					   "is", TRUE,
					   "value", (gchar *)listpointer->data,
					   "int_value", 0,
					   "update_list", TRUE,
					   NULL);
			     gmameui_filters_list_add_filter (main_gui.filters_list,
							      folder_filter,
							      _("Version"));
			     g_object_unref (folder_filter);
		}
	}
	gtk_widget_show (GTK_WIDGET (main_gui.filters_list));
	g_free (catver_file);
	/* Finished setting up filters list */
	
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
	GtkWidget *sidebar = gmameui_sidebar_new ();
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
#ifdef TREESTORE
	if (! ((current_mode == LIST_TREE) || (current_mode == DETAILS_TREE))) {
		gtk_action_group_set_sensitive (main_gui.gmameui_view_action_group, FALSE);
	}
#endif
	/* Create the UI of the Game List */
	main_gui.displayed_list = mame_gamelist_view_new ();
	mame_gamelist_view_scroll_to_selected_game (main_gui.displayed_list);
	
	gtk_container_add (GTK_CONTAINER (main_gui.scrolled_window_games), GTK_WIDGET (main_gui.displayed_list));
	gtk_widget_show_all (main_gui.scrolled_window_games);
	
	gtk_paned_set_position (GTK_PANED (main_gui.hpanedLeft), xpos_filters);

	/* Update UI if executables or entries in the MameGamelistView are
	   not available */
	gmameui_ui_set_items_sensitive ();

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
	
	return main_window;
}

