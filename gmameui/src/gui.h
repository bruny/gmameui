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

#ifndef __GUI_H__
#define __GUI_H__

#include "common.h"

#include <gtk/gtkcheckmenuitem.h>
#include <gtk/gtkframe.h>
#include <gtk/gtkmenu.h>
#include <gtk/gtkpaned.h>
#include <gtk/gtkprogressbar.h>
#include <gtk/gtkstatusbar.h>
#include <gtk/gtktextbuffer.h>
#include <gtk/gtktogglebutton.h>
#include <gtk/gtktoolbar.h>

#include "gmameui.h"
#include "unzip.h"
#include "callbacks.h"
#include "mame_options.h"
#include "filters_list.h"

/* The following menu entries are always enabled */
static const GtkActionEntry gmameui_always_sensitive_menu_entries[] =
{
	/* Toplevel */
	{ "File", NULL, N_("_File") },
	{ "View", NULL, N_("_View") },
	{ "Options", NULL, N_("_Options") },
	{ "Help", NULL, N_("_Help") },

	/* File menu */
	{ "FileAuditAllGames", NULL, N_("_Audit All Games"), NULL,
	  N_("Audit ROM and sample sets"), G_CALLBACK (on_audit_all_games_activate) },
	{ "FileSelectRandom", NULL, N_("_Select Random Game"), NULL,
	  N_("Play currently selected game"), G_CALLBACK (on_select_random_game_activate) },
	{ "FileQuit", GTK_STOCK_QUIT, N_("_Quit"), "<control>Q",
	  N_("Quit GMAMEUI"), G_CALLBACK (on_exit_activate) },
	
	/* View menu */
	{ "ViewColumnLayout", NULL, N_("_Column Layout..."), NULL,
	  N_("Set column layout"), G_CALLBACK (on_column_layout_activate) },
	{ "ViewRefresh", GTK_STOCK_REFRESH, N_("Refresh"), "F5",
	  N_("Refresh game list"), G_CALLBACK (on_refresh_activate) },

	/* Option menu */
	{ "OptionCloneColor", GTK_STOCK_SELECT_COLOR, N_("Clone Color..."), NULL,
	  N_("Set clones color in gamelist"), G_CALLBACK (on_clone_color_menu_activate) },
	{ "OptionRebuildGameList", NULL, N_("_Rebuild Game List"), NULL,
	  N_("Rebuild the game list from executable information"), G_CALLBACK (on_rebuild_game_list_menu_activate) },
	{ "OptionDirs", NULL, N_("_Directories..."), NULL,
	  N_("Set directory configuration"), G_CALLBACK (on_directories_menu_activate) },  
	{ "OptionDefaultOpts", NULL, N_("Default _Options..."), NULL,
	  N_("Set default game options"), G_CALLBACK (on_default_option_menu_activate) },
	{ "OptionStartupOpts", GTK_STOCK_PREFERENCES, N_("_Startup Options..."), NULL,
	  N_("Set startup options"), G_CALLBACK (on_startup_option_activate) },  
	  	  
	/* Help menu */
	{"HelpContents", GTK_STOCK_HELP, N_("_Contents"), "F1",
	 N_("Open the GMAMEUI manual"), NULL/*G_CALLBACK (on_help_activate)*/ },
	{ "HelpAbout", GTK_STOCK_ABOUT, NULL, NULL,
	 N_("About this application"), G_CALLBACK (on_about_activate) }
};

/* The following menu entries are enabled when a ROM is selected */
static const GtkActionEntry gmameui_rom_menu_entries[] =
{
	/* File menu */
	{ "FilePlayGame", NULL, N_("Play"), NULL,
	  N_("Play currently selected game"), G_CALLBACK (on_play_activate) },
	{ "FilePlayRecord", GTK_STOCK_SAVE, N_("Play and Record Input..."), NULL,
	  N_("Record a game for later playback"), G_CALLBACK (on_play_and_record_input_activate) },
	{ "FilePlaybackRecord", GTK_STOCK_OPEN, N_("Playback Input..."), NULL,
	  N_("Playback a recorded game"), G_CALLBACK (on_playback_input_activate) },
	{ "FileFavesAdd", GTK_STOCK_ADD, N_("Add to 'Favorites'"), NULL,
	  N_("Add this game to your 'Favorites' game folder"), G_CALLBACK (on_add_to_favorites_activate) },
	{ "FileFavesRemove", GTK_STOCK_REMOVE, N_("Remove from 'Favorites'"), NULL,
	  N_("Remove this game from your 'Favorites' game folder"), G_CALLBACK (on_remove_from_favorites_activate) },
	{ "FileProperties", GTK_STOCK_INFO, N_("Properties"), NULL,
	  N_("Display the properties of the selected game"), G_CALLBACK (on_properties_activate) }, 
	{ "FileOptions", GTK_STOCK_PROPERTIES, N_("Options"), NULL,
	  N_("Change the options of the selected game"), G_CALLBACK (on_options_activate) }, 
};

/* The following menu entries are enabled when the view is changed to a tree view */
static const GtkActionEntry gmameui_view_expand_menu_entries[] =
{
	{ "ViewExpandAll", NULL, N_("Expand All"), NULL,
	  N_("Expand all rows"), G_CALLBACK (on_expand_all_activate) },
	{ "ViewCollapseAll", NULL, N_("Collapse All"), NULL,
	  N_("Collapse all rows"), G_CALLBACK (on_collapse_all_activate) },
};

static const GtkToggleActionEntry gmameui_view_toggle_menu_entries[] = 
{
	{ "ViewToolbar", NULL, N_("_Toolbar"), "<alt>T",
	  N_("Show or hide the toolbar"),
	  G_CALLBACK (on_toolbar_view_menu_activate), TRUE },
	{ "ViewStatusBar", NULL, N_("_Status Bar"), "<alt>S",
	  N_("Show or hide the status bar"),
	  G_CALLBACK (on_status_bar_view_menu_activate), TRUE },  
	{ "ViewFolderList", NULL, N_("Fold_er List"), "<alt>D",
	  N_("Show or hide the folder list"),
	  G_CALLBACK (on_folder_list_activate), TRUE },  
	{ "ViewSidebarPanel", NULL, N_("Scree_nshot Panel"), "<alt>N",
	  N_("Show or hide the screenshot panel"),
	  G_CALLBACK (on_screen_shot_activate), TRUE },   
	{ "ViewSidebarPanelTab", NULL, N_("Scree_nshot Panel Tab"), "<alt>O",
	  N_("Show or hide the screenshot panel tabulation"),
	  G_CALLBACK (on_screen_shot_tab_activate), TRUE },  
	  
	{ "ViewThePrefix", NULL, N_("The _Prefix"), NULL,
	  N_("Show \"The\" at the end."),
	  G_CALLBACK (on_the_prefix_activate), TRUE },
};

static const GtkRadioActionEntry gmameui_view_radio_menu_entries[] =
{
	{ "ViewListView", NULL, N_("_List"), NULL,
	  N_("Displays items in a list"), LIST },
	{ "ViewTreeView", NULL, N_("List _Tree"), NULL,
	  N_("Displays items in a tree list with clones indented"), LIST_TREE },
	{ "ViewDetailsListView", NULL, N_("_Details"), NULL,
	  N_("Displays detailed information about each item"), DETAILS },
	{ "ViewDetailsTreeView", NULL, N_("Detai_ls Tree"), NULL,
	  N_("Displays detailed information about each item with clones indented"), DETAILS_TREE },
};

static const GtkActionEntry gmameui_column_entries[] =
{
	{ "ColumnHide", NULL, N_("Hide Column"), NULL,
	  N_("Hide Column"), G_CALLBACK (on_column_hide_activate) },
	{ "ColumnLayout", NULL, N_("Column Layout..."), NULL,
	  N_("Show or Hide Column"), G_CALLBACK (on_column_layout_activate) },
};

/**** Sidebar functionality ****/
#define GMAMEUI_TYPE_SIDEBAR        (gmameui_sidebar_get_type ())
#define GMAMEUI_SIDEBAR(o)          (G_TYPE_CHECK_INSTANCE_CAST ((o), GMAMEUI_TYPE_SIDEBAR, GMAMEUISidebar))
#define GMAMEUI_SIDEBAR_CLASS(k)    (G_TYPE_CHECK_CLASS_CAST((k), GMAMEUI_TYPE_SIDEBAR, GMAMEUISidebarClass))
#define GMAMEUI_IS_SIDEBAR(o)       (G_TYPE_CHECK_INSTANCE_TYPE ((o), GMAMEUI_TYPE_SIDEBAR))
#define GMAMEUI_IS_SIDEBAR_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), GMAMEUI_TYPE_SIDEBAR))

typedef struct _GMAMEUISidebar        GMAMEUISidebar;
typedef struct _GMAMEUISidebarClass   GMAMEUISidebarClass;
typedef struct _GMAMEUISidebarPrivate GMAMEUISidebarPrivate;

/*
 * Main object structure
 */

struct _GMAMEUISidebar 
{
	GtkFrame parent; /* Container for all the nested widgets */

	/*< private > */
	GMAMEUISidebarPrivate *priv;
};

struct _GMAMEUISidebarClass {
	GtkFrameClass parent_class;
};

GType gmameui_sidebar_get_type (void);

GtkWidget *gmameui_sidebar_new (void);

static void
gmameui_sidebar_set_page (GMAMEUISidebar *sidebar, RomEntry *rom, screenshot_type page);

static void
gmameui_sidebar_set_with_rom (GMAMEUISidebar *sidebar, RomEntry *rom);

static gboolean
gmameui_sidebar_set_history (GMAMEUISidebar *sidebar, RomEntry *rom);

static void
gmameui_sidebar_set_current_page (GMAMEUISidebar *sidebar, int page);

/**** End Sidebar functionality ****/

GtkWidget *MainWindow;

struct main_gui_struct {

	GtkToolbar *toolbar;

	GtkWidget *combo_progress_bar;
	GtkStatusbar *status_progress_bar;
	GtkProgressBar *progress_progress_bar;
	GtkWidget *tri_status_bar;
	GtkStatusbar *statusbar1;
	GtkStatusbar *statusbar2;
	GtkStatusbar *statusbar3;

	GtkPaned *hpanedLeft;
	GtkPaned *hpanedRight;

	GtkWidget *scrolled_window_filters;
	GMAMEUIFiltersList *filters_list;

	GtkWidget *scrolled_window_games;
	GtkWidget *displayed_list;   /* Tree View */
	GtkTreeModel *tree_model;

	GMAMEUISidebar *screenshot_hist_frame;

	GtkWidget *executable_menu;
	
	GtkUIManager *manager;
	GtkActionGroup *gmameui_rom_action_group;
	GtkActionGroup *gmameui_view_action_group;
	
	MameOptions *options;
};

struct main_gui_struct main_gui;

void adjustment_scrolled (GtkAdjustment *adjustment,
                          gpointer       user_data);
/* New icon code */
void gmameui_icons_init (void);
GdkPixbuf * gmameui_get_icon_from_stock (const char *);
GtkWidget * gmameui_get_image_from_stock (const char *);

void add_exec_menu(void);
void init_gui(void);

/* to add the dropbox from the directory.c whenn changing list*/
void add_exec_menu (void);
void hide_filters (void);
void show_filters (void);
void hide_snaps (void);
void show_snaps (void);
void hide_snaps_tab (GMAMEUISidebar *sidebar);
void show_snaps_tab (GMAMEUISidebar *sidebar);
void hide_toolbar (void);
void show_toolbar (void);
void hide_status_bar (void);
void show_status_bar (void);

GdkPixbuf * get_icon_for_rom (RomEntry *rom, guint size, ZIP *zip);
GdkPixbuf * gmameui_get_icon_from_stock (const char *id);
GtkWidget * gmameui_get_image_from_stock (const char *id);
void        gmameui_icons_init (void);

void create_gamelist_content (void);
void create_gamelist (ListMode list_mode);
RomEntry * gamelist_get_selected_game (void);
void gamelist_popupmenu_show (RomEntry *rom, GdkEventButton *event);
void gmameui_toolbar_set_favourites_sensitive (gboolean rom_is_favourite);
void gmameui_menu_set_view_mode_check (gint view_mode, gboolean state);
void show_progress_bar (void);
void hide_progress_bar (void);
void update_progress_bar (gfloat current_value);
void update_game_in_list (RomEntry *rom);
void select_inp (RomEntry *Rom, gboolean play_record);
void create_Menu (void);
void select_game (RomEntry *rom);

#endif /* __GUI_H__ */
