/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * GMAMEUI
 *
 * Copyright 2010 Andrew Burton <adb@iinet.net.au>
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

#include <glib-object.h>	/* For GParamSpec */

#include "gmameui-main-win.h"
#include "filters_list.h"
#include "gmameui.h"    /* For gui_prefs */

/* Other dialogs */
#include "gui_prefs_dialog.h"
#include "gmameui-audit-dlg.h"
#include "gmameui-rominfo-dlg.h"
#include "directories.h"
#include "mame_options_dialog.h"
#include "mame_options_legacy_dialog.h"

#include "gmameui-search-entry.h"
#include "gmameui-statusbar.h"
#include "gui.h" /* main_gui */

struct _GMAMEUIMainWinPrivate {
	gint i;
	
};

#define GMAMEUI_MAIN_WIN_GET_PRIVATE(o)  (GMAMEUI_MAIN_WIN (o)->priv)

G_DEFINE_TYPE (GMAMEUIMainWin, gmameui_main_win, GTK_TYPE_WINDOW)

/* Function prototypes */
static void
gmameui_main_win_destroy         (GtkObject *object);

static gboolean
on_main_window_moved_cb          (GtkWidget *widget, GdkEventConfigure *event, gpointer data);
static gboolean
on_main_window_delete_event      (GtkWidget *widget, GdkEvent *event, gpointer user_data);
static void
on_hpaned_position_notify        (GObject *object, GParamSpec *pspec, gpointer data);

static void
on_view_type_changed             (GtkToggleAction *action, gpointer user_data);
static void
on_filter_btn_toggled            (GtkWidget *widget, gpointer user_data);
static GtkWidget *
get_filter_btn_by_id             (GtkBuilder *builder, gint i);

static void
gmameui_ui_set_favourites_sensitive (gboolean rom_is_favourite);

/* File menu callbacks */
static void
on_play_activate                  (GtkAction *action, gpointer user_data);

static void
on_play_and_record_input_activate (GtkAction *action, gpointer user_data);

static void
on_playback_input_activate        (GtkAction *action, gpointer user_data);

static void
on_select_random_game_activate    (GtkMenuItem *menuitem, gpointer user_data);

static void
on_add_to_favorites_activate      (GtkMenuItem *menuitem, gpointer user_data);

static void
on_remove_from_favorites_activate (GtkMenuItem *menuitem, gpointer user_data);

static void
on_romset_info_activate           (GtkAction *action, gpointer user_data);

static void
on_options_activate               (GtkAction *action, gpointer user_data);

static void
on_options_default_activate       (GtkAction *action, gpointer user_data);

static void
on_audit_all_games_activate       (GtkMenuItem *menuitem, gpointer user_data);
#ifdef ENABLE_ROMVALIDATION
static void
on_rebuild_romsets_activate	  (GtkAction *action, gpointer user_data);
#endif
static void
on_exit_activate                  (GtkMenuItem *menuitem, gpointer user_data);

/* View menu callbacks */
static void
on_refresh_activate              (GtkAction *action, gpointer user_data);
static void
on_filter_list_activate          (GtkAction *action, gpointer user_data);
static void
on_toolbar_view_menu_activate    (GtkAction *action, gpointer user_data);
static void
on_status_bar_view_menu_activate (GtkAction *action, gpointer user_data);
static void
on_screen_shot_activate          (GtkAction *action, gpointer user_data);

/* Options menu callbacks */
static void
on_directories_menu_activate      (GtkMenuItem *menuitem, gpointer user_data);

static void
on_preferences_activate           (GtkMenuItem *menuitem, gpointer user_data);

/* Help menu callbacks */
static void
on_help_activate                  (GtkAction *menuitem, gpointer user_data);
                                                 
static void
on_about_activate                 (GtkMenuItem *menuitem, gpointer user_data);

static void
update_favourites_list (gboolean add);

/* UI definitions for menu layout */

/* The following menu entries are always enabled */
static const GtkActionEntry gmameui_always_sensitive_menu_entries[] =
{
	/* Toplevel */
	{ "File", NULL, N_("_File") },
	{ "View", NULL, N_("_View") },
	{ "Options", NULL, N_("_Options") },
	{ "Help", NULL, N_("_Help") },

	/* File menu */
	{ "FileSelectRandom", NULL, N_("_Select Random Game"), NULL,
	  N_("Play currently selected game"), G_CALLBACK (on_select_random_game_activate) },
	{ "FileQuit", GTK_STOCK_QUIT, N_("_Quit"), "<control>Q",
	  N_("Quit GMAMEUI"), G_CALLBACK (on_exit_activate) },

	/* View menu */


	/* Option menu */
	{ "OptionDirs", GTK_STOCK_DIRECTORY, N_("_Directories..."), NULL,
	  N_("Set directory configuration"), G_CALLBACK (on_directories_menu_activate) },  
	{ "OptionPreferences", GTK_STOCK_PREFERENCES, N_("_GMAMEUI Preferences..."), NULL,
	  N_("Set GMAMEUI preferences"), G_CALLBACK (on_preferences_activate) },  
	  	  
	/* Help menu */
	{"HelpContents", GTK_STOCK_HELP, N_("_Contents"), "F1",
	 N_("Open the GMAMEUI manual"), G_CALLBACK (on_help_activate) },
	{ "HelpAbout", GTK_STOCK_ABOUT, NULL, NULL,
	 N_("About this application"), G_CALLBACK (on_about_activate) }
};

/* The following menu entries are enabled when a ROM is selected and a MAME
   executable exists */
static const GtkActionEntry gmameui_rom_and_exec_menu_entries[] =
{
	/* File menu */
	{ "FilePlayGame", GTK_STOCK_EXECUTE, N_("Play"), NULL,
	  N_("Play currently selected game"), G_CALLBACK (on_play_activate) },
	{ "FilePlayRecord", GTK_STOCK_SAVE, N_("Play and Record Input..."), NULL,
	  N_("Record a game for later playback"), G_CALLBACK (on_play_and_record_input_activate) },
	{ "FilePlaybackRecord", GTK_STOCK_OPEN, N_("Playback Input..."), NULL,
	  N_("Playback a recorded game"), G_CALLBACK (on_playback_input_activate) },
	/*{ "FileOptions", GTK_STOCK_PROPERTIES, N_("Romset MAME Options"), NULL,
	  N_("Change the MAME options of the selected romset"), G_CALLBACK (on_options_activate) },*/ 
};

/* The following menu entries are enabled when MAME executable exists */
static const GtkActionEntry gmameui_exec_menu_entries[] =
{
	{ "FileAuditAllGames", GTK_STOCK_FIND, N_("_Audit All Games"), NULL,
	  N_("Audit ROM and sample sets"), G_CALLBACK (on_audit_all_games_activate) },
/*DELETE	{ "OptionRebuildGameList", NULL, N_("_Rebuild Game List"), NULL,
	  N_("Rebuild the game list from executable information"), G_CALLBACK (on_rebuild_game_list_menu_activate) },*/
#ifdef ENABLE_ROMVALIDATION
	{ "FileRebuildRomsets", GTK_STOCK_FIND_AND_REPLACE, N_("_Rebuild Romsets"), NULL,
	  N_("Rebuild and fix romsets"), G_CALLBACK (on_rebuild_romsets_activate) },
#endif
	{ "ViewRefresh", GTK_STOCK_REFRESH, N_("Refresh"), "F5",
	  N_("Refresh game list"), G_CALLBACK (on_refresh_activate) },
	
	{ "OptionDefaultOpts", NULL, N_("Default MAME _Options..."), NULL,
	  N_("Set default MAME options"), G_CALLBACK (on_options_default_activate) },
};

/* The following menu entries are enabled when a ROM is selected */
static const GtkActionEntry gmameui_rom_menu_entries[] =
{
	{ "FileRomsetInfo", GTK_STOCK_INFO, N_("Romset Information"), NULL,
	  N_("Display the details of the selected romset"), G_CALLBACK (on_romset_info_activate) }, 
};

static const GtkActionEntry gmameui_favourite_menu_entries[] =
{
	{ "FileFavesAdd", GTK_STOCK_ADD, N_("Add to 'Favorites'"), NULL,
	  N_("Add this game to your 'Favorites' game folder"), G_CALLBACK (on_add_to_favorites_activate) },
	{ "FileFavesRemove", GTK_STOCK_REMOVE, N_("Remove from 'Favorites'"), NULL,
	  N_("Remove this game from your 'Favorites' game folder"), G_CALLBACK (on_remove_from_favorites_activate) },
};

static const GtkToggleActionEntry gmameui_view_toggle_menu_entries[] = 
{
	{ "ViewToolbar", NULL, N_("_Toolbar"), "<alt>T",
	  N_("Show or hide the toolbar"),
	  G_CALLBACK (on_toolbar_view_menu_activate), TRUE },
	{ "ViewStatusBar", NULL, N_("_Status Bar"), "<alt>S",
	  N_("Show or hide the status bar"),
	  G_CALLBACK (on_status_bar_view_menu_activate), TRUE },  
	{ "ViewFilterList", NULL, N_("Filt_er List"), "<alt>D",
	  N_("Show or hide the filter list"),
	  G_CALLBACK (on_filter_list_activate), TRUE },  
	{ "ViewSidebarPanel", NULL, N_("Scree_nshot Panel"), "<alt>N",
	  N_("Show or hide the screenshot panel"),
	  G_CALLBACK (on_screen_shot_activate), TRUE },   

	{ "ViewDetailsListView", NULL, N_("_Details"), NULL,
	  N_("Displays detailed information about each item"),
	  G_CALLBACK (on_view_type_changed), TRUE },
};

static const GtkActionEntry gmameui_column_entries[] =
{
	{ "ColumnHide", NULL, N_("Hide Column"), NULL,
	  N_("Hide Column"), G_CALLBACK (on_column_hide_activate) },
};

enum {
	TOOLBAR_PLAYGAME,
	TOOLBAR_VIEWFILTERLIST,
	TOOLBAR_VIEWSIDEBAR,
	TOOLBAR_DETAILSLISTVIEW,

	NUM_TOOLBAR_ITEMS
};

typedef struct _toolbar_item toolbar_item;

struct _toolbar_item {
	gint id;
	gchar *ui_name;		/* Name from gmameui-ui.xml e.g. /ToolBar/ViewFilterList */
	gchar *icon_name;   /* For non-stock menubar items e.g. gmameui-view-folders */
	gchar *stock_id;	/* For stock menubar items e.g. GTK_STOCK_NEW */
	gchar *label;		/* Label name e.g. N_("Show Folders") */
};

/* Note - where these are used (particularly .label), need to prefix the use
   with a _(XXX) to enable gettext translation */
static const toolbar_item toolbar_items [] = {
	{ TOOLBAR_PLAYGAME, "/ToolBar/FilePlayGame", NULL, GTK_STOCK_EXECUTE, N_("Play Game") },
	{ TOOLBAR_VIEWFILTERLIST, "/ToolBar/ViewFilterList", "gmameui-view-filters", 0, N_("Show Filters") },
	{ TOOLBAR_VIEWSIDEBAR, "/ToolBar/ViewSidebarPanel", "gmameui-view-screenshot", 0, N_("Show Sidebar") },

	{ TOOLBAR_DETAILSLISTVIEW, "/ToolBar/ViewDetailsListView", "gmameui-view-list", 0, N_("Details") },

};


/* Boilerplate functions */
static GObject *
gmameui_main_win_constructor (GType                  type,
				guint                  n_construct_properties,
				GObjectConstructParam *construct_properties)
{
	GObject          *obj;
	GMAMEUIMainWin *win;

	obj = G_OBJECT_CLASS (gmameui_main_win_parent_class)->constructor (type,
									     n_construct_properties,
									     construct_properties);

	win = GMAMEUI_MAIN_WIN (obj);

	return obj;
}

static void
gmameui_main_win_class_init (GMAMEUIMainWinClass *class)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (class);
	GtkObjectClass *gtkobject_class = GTK_OBJECT_CLASS (class);
/*	GtkWindowClass *gtkwindow_class = GTK_WINDOW_CLASS (class);*/

	gobject_class->constructor = gmameui_main_win_constructor;
/*	gobject_class->get_property = gmameui_main_win_get_property;
	gobject_class->set_property = gmameui_main_win_set_property;*/

	gtkobject_class->destroy = gmameui_main_win_destroy;

	g_type_class_add_private (class,
				  sizeof (GMAMEUIMainWinPrivate));

	/* Signals and properties go here */

}

static void
gmameui_main_win_init (GMAMEUIMainWin *main_window)
{
	GMAMEUIMainWinPrivate *priv;


	priv = G_TYPE_INSTANCE_GET_PRIVATE (main_window,
					    GMAMEUI_TYPE_MAIN_WIN,
					    GMAMEUIMainWinPrivate);

	main_window->priv = priv;
	
	/* Initialise private variables */
	GtkAccelGroup *accel_group;
	GtkTooltips *tooltips;
	
	GtkActionGroup *action_group;
	GtkWidget *menubar;
	
	GtkWidget *vbox;
	GtkBuilder *builder;
	
	gint ui_width, ui_height;
	gint xpos_filters, xpos_gamelist;
	gint show_filters, show_screenshot;
	gint show_statusbar, show_toolbar;
	gint current_mode;

	GError *error = NULL;

	const gchar *object_names[] = {
		"filter_hbox",
	};
	
GMAMEUI_DEBUG ("Setting up main window...");
	g_object_get (main_gui.gui_prefs,
		      "ui-width", &ui_width,
		      "ui-height", &ui_height,
		      "show-filterlist", &show_filters,
		      "show-screenshot", &show_screenshot,
		      "show-statusbar", &show_statusbar,
		      "show-toolbar", &show_toolbar,
		      "current-mode", &current_mode,    /* FIXME TODO Rename to show-details */
		      "xpos-filters", &xpos_filters,
		      "xpos-gamelist", &xpos_gamelist,
		      NULL);

	builder = gtk_builder_new ();
	gtk_builder_set_translation_domain (builder, GETTEXT_PACKAGE);
	
	if (!gtk_builder_add_objects_from_file (builder,
	                                        GLADEDIR "filter_bar.builder",
	                                        (gchar **) object_names,
	                                        &error))
	{
		g_warning ("Couldn't load builder file: %s", error->message);
		g_error_free (error);
		return;
	}
	
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
	search_box = gtk_hbox_new (FALSE, 6);

	/* Search text entry */
	main_gui.search_entry = mame_search_entry_new ();

	/* Search radio buttons */
	GtkWidget *search_rbtns;
	search_rbtns = GTK_WIDGET (gtk_builder_get_object (builder, "filter_hbox"));

	/* Pack it all in */
	gtk_box_pack_start (GTK_BOX (search_box), main_gui.search_entry, FALSE, FALSE, 6);
	gtk_box_pack_start (GTK_BOX (search_box), search_rbtns, FALSE, FALSE, 6);
	gtk_box_pack_start (GTK_BOX (vbox), GTK_WIDGET (search_box), FALSE, FALSE, 6);

	/* Prepare the ROM availability filter buttons */
GMAMEUI_DEBUG ("      Setting up ROM availability filter buttons...");
	GSList *filter_btn_list, *list;
	gint current_filter_btn;
	
	filter_btn_list = gtk_builder_get_objects (builder);
	list = g_slist_nth (filter_btn_list, 0);
	g_object_get (main_gui.gui_prefs, "current-rom-filter", &current_filter_btn, NULL);

	while (list) {
		GtkWidget *widget;
		const gchar *name;
		
		widget = list->data;

		name = gtk_buildable_get_name (GTK_BUILDABLE (widget));

		if (g_ascii_strncasecmp (name, "filter_btn_", 11) == 0) {
			/* Hide the radio circle so only the button is visible */
			gtk_toggle_button_set_mode (GTK_TOGGLE_BUTTON (list->data), FALSE);

			/* Set the signal callback */
			g_signal_connect (G_OBJECT (list->data), "toggled",
			                  G_CALLBACK (on_filter_btn_toggled), NULL);
		}
		
		list = g_slist_next (list);
	}

	/* Set the button based on the preference */
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (get_filter_btn_by_id (builder,
	                                                                       current_filter_btn)),
	                              TRUE);
GMAMEUI_DEBUG ("      Setting up ROM availability filter buttons... done");	
	/* End ROM availability filter buttons */
GMAMEUI_DEBUG ("    Setting up search entry field... done");
	
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
	gtk_paned_pack1 (GTK_PANED (main_gui.hpanedRight), main_gui.scrolled_window_games, TRUE, TRUE);

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
	
	
}

GtkWidget *
gmameui_main_win_new (void)
{
	GtkWidget *win;

	win = g_object_new (GMAMEUI_TYPE_MAIN_WIN,
			       "title", _("GMAMEUI"),
			       NULL);

	return win;

}


static void
gmameui_main_win_destroy (GtkObject *object)
{
	GMAMEUIMainWin *win;
	
GMAMEUI_DEBUG ("Destroying gmameui main win...");	
	win = GMAMEUI_MAIN_WIN (object);

	
	g_object_unref (win->priv);
	
/*	GTK_OBJECT_CLASS (gmameui_main_win_parent_class)->destroy (object);*/
	
GMAMEUI_DEBUG ("Destroying gmameui main win... done");
}


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
static void
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
on_toolbar_view_menu_activate (GtkAction *action, gpointer user_data)
{
	gboolean visible;

	visible = gtk_toggle_action_get_active (GTK_TOGGLE_ACTION (action));

	if (visible)
		gtk_widget_show (GTK_WIDGET (main_gui.toolbar));
	else
		gtk_widget_hide (GTK_WIDGET (main_gui.toolbar));

	g_object_set (main_gui.gui_prefs, "show-toolbar", visible, NULL);
}

static void
on_filter_list_activate (GtkAction *action, gpointer user_data)
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

static void
on_screen_shot_activate (GtkAction *action, gpointer user_data)
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

static void
on_status_bar_view_menu_activate (GtkAction *action, gpointer user_data)
{
	gboolean visible;

	visible = gtk_toggle_action_get_active (GTK_TOGGLE_ACTION (action));

	if (visible)
		gtk_widget_show (GTK_WIDGET (main_gui.statusbar));
	else
		gtk_widget_hide (GTK_WIDGET (main_gui.statusbar));

	g_object_set (main_gui.gui_prefs, "show-statusbar", visible, NULL);
}

static void
on_play_activate (GtkAction *action, gpointer user_data)
{
	play_game (gui_prefs.current_game);
}

static void
on_play_and_record_input_activate (GtkAction *action, gpointer user_data)
{
	select_inp (FALSE);
}


static void
on_playback_input_activate (GtkAction *action, gpointer user_data)
{
	select_inp (TRUE);
}

static void
on_select_random_game_activate (GtkMenuItem *menuitem, gpointer user_data)
{
	gint random_game;

	g_return_if_fail (visible_games > 0);

	random_game = (gint) g_random_int_range (0, visible_games);
	GMAMEUI_DEBUG ("random game#%i", random_game);

	mame_gamelist_view_select_random_game (main_gui.displayed_list, random_game);

}

/* This function is called when the toggle action defining the list mode is changed. */
static void
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

static void
on_filter_btn_toggled (GtkWidget *widget, gpointer user_data)
{
	const gchar *widgetname = gtk_buildable_get_name (GTK_BUILDABLE (widget));
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
get_filter_btn_by_id (GtkBuilder *builder, gint i)
{
	GtkWidget *radio = NULL;
	
	if (i == 0)
		radio = GTK_WIDGET (gtk_builder_get_object (builder, "filter_btn_all"));
	else if (i == 1)
		radio = GTK_WIDGET (gtk_builder_get_object (builder, "filter_btn_avail"));
	else if (i == 2)
		radio = GTK_WIDGET (gtk_builder_get_object (builder, "filter_btn_unavail"));

	return radio;
}

static void update_favourites_list (gboolean add)
{
	g_object_set (gui_prefs.current_game, "is-favourite", add, NULL);

	gmameui_ui_set_favourites_sensitive (add);
	
	mame_gamelist_view_update_game_in_list (main_gui.displayed_list,
						gui_prefs.current_game);
}

static void
on_add_to_favorites_activate (GtkMenuItem *menuitem, gpointer user_data)
{
	update_favourites_list (TRUE);
}


static void
on_remove_from_favorites_activate (GtkMenuItem *menuitem, gpointer user_data)
{
	update_favourites_list (FALSE);
}

static void
on_romset_info_activate (GtkAction *action, gpointer user_data)
{
	GtkWidget *rominfo_dialog;
	rominfo_dialog = mame_rominfo_dialog_new ();
	gtk_widget_show (rominfo_dialog);
}

/* If rom_name is NULL, then the default options are used and loaded, otherwise
   the rom-specific options are used */
static void
show_properties_dialog (gchar *rom_name)
{
	MameExec *exec;
	GtkWidget *dlg;
	
	exec = mame_exec_list_get_current_executable (main_gui.exec_list);
	
	g_return_if_fail (exec != NULL);
	
	/* SDLMAME uses a different set of options to XMAME. If we are running
	   XMAME, then use the legacy GXMAME method of maintaining the options */
	if (mame_exec_get_exectype (exec) == XMAME_EXEC_WIN32) {
		/* SDLMAME */	
		dlg = mame_options_dialog_new ();
	} else {		
		dlg = mame_options_legacy_dialog_new ();
	}

	gtk_widget_show (dlg);
}

static void
on_options_activate (GtkAction *action, gpointer user_data)
{
	gchar *current_rom;
	g_object_get (main_gui.gui_prefs, "current-rom", &current_rom, NULL);
	
	show_properties_dialog (current_rom);
	
	g_free (current_rom);
}

static void
on_options_default_activate (GtkAction *action, gpointer user_data)
{
	show_properties_dialog (NULL);
}

static void
on_audit_all_games_activate (GtkMenuItem *menuitem, gpointer user_data)
{
	MameExec *exec;
	GtkWidget *audit_dlg;
	
	exec = mame_exec_list_get_current_executable (main_gui.exec_list);

	g_return_if_fail (exec != NULL);

	gamelist_check (exec);

	audit_dlg = mame_audit_dialog_new (NULL);
	gtk_widget_show (audit_dlg);	
}



#ifdef ENABLE_ROMVALIDATION
static void
on_rebuild_romsets_activate (GtkAction *action, gpointer user_data)
{
	GtkWidget *romsetsmgr_dlg;
	romsetsmgr_dlg = gmameui_rommgr_dialog_new (NULL);
	gtk_widget_show (romsetsmgr_dlg);
}
#endif

static void
on_exit_activate (GtkMenuItem *menuitem, gpointer user_data)
{
	exit_gmameui ();
}

static void
on_refresh_activate (GtkAction *action, gpointer user_data)
{
	gmameui_gamelist_rebuild (main_gui.displayed_list);
}


/* Options menu */

static void
on_directories_menu_activate (GtkMenuItem *menuitem, gpointer user_data)
{
	GtkWidget *directory_window;
	directory_window = mame_directories_dialog_new (NULL);
	gtk_widget_show (directory_window);
}

static void
on_preferences_activate (GtkMenuItem *menuitem, gpointer user_data)
{
	MameGuiPrefsDialog *prefs_dialog;
	prefs_dialog = mame_gui_prefs_dialog_new ();
GMAMEUI_DEBUG("Running dialog");
	gtk_dialog_run (GTK_DIALOG (prefs_dialog));
	gtk_widget_destroy (GTK_WIDGET (prefs_dialog));
GMAMEUI_DEBUG("Done running dialog");
}


/* Help menu */

static void
on_help_activate (GtkAction *action, gpointer user_data)
{
	GError *error = NULL;
#ifdef ENABLE_GTKSHOWURI
	/* gtk_show_uri requires > GTK2.13 */
	guint timestamp;
	
	timestamp = gtk_get_current_event_time ();	
	gtk_show_uri (NULL, "ghelp:gmameui", timestamp, &error);
#elif ENABLE_LIBGNOME
	/* The following requires libgnome to be compiled in (which it is
	   by default) */
	gboolean ret;
	ret = gnome_url_show ("ghelp:gmameui", &error);
#else
	/* GTK2.13 is not available and the user has not compiled in
	   libgnome (may be a XFCE or KDE user) */
	GtkWidget *helpunavail_dlg;
	helpunavail_dlg = gmameui_dialog_create (GTK_MESSAGE_ERROR,
						 NULL,
						 _("GMAMEUI Help could not be started"));
	gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (helpunavail_dlg),
						  _("The Help file could not be opened. This will happen "
						   "if the version of GTK is not at least 2.13 and "
						   "libgnome support has not been compiled in."));
	gtk_dialog_run (GTK_DIALOG (helpunavail_dlg));
	gtk_widget_destroy (helpunavail_dlg);
#endif
	
	if (error) {
		GtkWidget *helperr_dlg;
		helperr_dlg = gmameui_dialog_create (GTK_MESSAGE_ERROR,
						     NULL,
						     _("There was an error opening the Help file"));
		gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (helperr_dlg),
							  error->message);
		gtk_dialog_run (GTK_DIALOG (helperr_dlg));
		gtk_widget_destroy (helperr_dlg);
		g_error_free (error);
		error = NULL;
	}
}

static void
on_about_activate (GtkMenuItem *menuitem, gpointer user_data)
{
	GdkPixbuf *pixbuf;
	
	const char *authors [] = {
		"Andrew Burton <adb@iinet.net.au>",
		"Based on GXMame",
		"Stephane Pontier <shadow_walker@users.sourceforge.net>",
		"Benoit Dumont <vertigo17@users.sourceforge.net>",
		"Nicos Panayides <anarxia@gmx.net>",
		"Priit Laes <x-laes@users.sourceforge.net>",
		"William Jon McCann <mccann@jhu.edu>",
		NULL
	};

	const char *translators = _("translator-credits");

	pixbuf = gmameui_get_icon_from_stock ("gmameui-screen");

	gtk_show_about_dialog (GTK_WINDOW (MainWindow),
			       "name", "GMAMEUI",
			       "version", VERSION,
			       "logo", pixbuf,
			       "copyright", _("Copyright (c) 2007-2010 Andrew Burton"),
			       "website", "http://gmameui.sourceforge.net",
			       "comments", _("A program to play MAME under Linux"),
			       "authors", authors,
			       "translator-credits", translators,
			       NULL);

	g_object_unref (pixbuf);
}


static void
gmameui_ui_set_favourites_sensitive (gboolean rom_is_favourite)
{
	gtk_widget_set_sensitive (gtk_ui_manager_get_widget (main_gui.manager,
							     "/MenuBar/FileMenu/FileFavesAddMenu"),
				  !rom_is_favourite);
	gtk_widget_set_sensitive (gtk_ui_manager_get_widget (main_gui.manager,
							     "/MenuBar/FileMenu/FileFavesRemoveMenu"),
				  rom_is_favourite);
	gtk_widget_set_sensitive (gtk_ui_manager_get_widget (main_gui.manager,
							     "/GameListPopup/FileFavesAdd"),
				  !rom_is_favourite);
	gtk_widget_set_sensitive (gtk_ui_manager_get_widget (main_gui.manager,
							     "/GameListPopup/FileFavesRemove"),
				  rom_is_favourite);
}

void
gmameui_ui_set_items_sensitive ()
{

	gboolean rom_and_exec;  /* TRUE if both an executable exists and a ROM is selected */
	rom_and_exec = (gui_prefs.current_game) && (mame_exec_list_size (main_gui.exec_list) > 0);

	gtk_action_group_set_sensitive (main_gui.gmameui_rom_exec_action_group,
					rom_and_exec);
	gtk_action_group_set_sensitive (main_gui.gmameui_exec_action_group,
					mame_exec_list_size (main_gui.exec_list));

	/* Disable ROM and Favourites UI items if no current game */
	gtk_action_group_set_sensitive (main_gui.gmameui_rom_action_group,
					(gui_prefs.current_game != NULL));
	gtk_action_group_set_sensitive (main_gui.gmameui_favourite_action_group,
					(gui_prefs.current_game != NULL));
	
	/* Set the Add/Remove Favourites depending whether the game is a favourite */
	if (gui_prefs.current_game != NULL)
		gmameui_ui_set_favourites_sensitive (mame_rom_entry_is_favourite (gui_prefs.current_game));
}

