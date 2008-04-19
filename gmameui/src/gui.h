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
	GtkMenu *popup_column_menu;
	GtkWidget *popup_column_hide;
	GtkWidget *popup_column_layout;

	GtkMenuItem *play_menu;
	GtkMenuItem *properties_menu;
	GtkMenuItem *audit_all_games_menu;
	GtkMenuItem *add_to_favorites;
	GtkMenuItem *remove_from_favorites;
	GtkCheckMenuItem *folder_list_menu;
	GtkCheckMenuItem *screen_shot_menu;
	GtkCheckMenuItem *screen_shot_tab_menu;
	GtkCheckMenuItem *list_view_menu;
	GtkCheckMenuItem *list_indented_view_menu;
	GtkCheckMenuItem *list_tree_view_menu;
	GtkCheckMenuItem *details_view_menu;
	GtkCheckMenuItem *details_indented_view_menu;
	GtkCheckMenuItem *details_tree_view_menu;
	GtkCheckMenuItem *toolbar_view_menu;
	GtkCheckMenuItem *status_bar_view_menu;
	GtkMenuItem *expand_all_menu;
	GtkMenuItem *collapse_all_menu;
	GtkCheckMenuItem *modify_the_menu;
	GtkMenuItem *refresh_menu;

	GtkToolbar *toolbar;
	GtkWidget *hseparator1;
	GtkToggleToolButton *filterShowButton;
	GtkToggleToolButton *snapShowButton;
	GtkToggleToolButton *list_view_button;
	GtkToggleToolButton *list_indented_view_button;
	GtkToggleToolButton *list_tree_view_button;
	GtkToggleToolButton *details_view_button;
	GtkToggleToolButton *details_indented_view_button;
	GtkToggleToolButton *details_tree_view_button;

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
	GtkWidget *filters_displayed_list;   /* Tree View */
	GtkTreeModel *filters_tree_model;

	GtkWidget *scrolled_window_games;
	GtkWidget *displayed_list;   /* Tree View */
	GtkTreeModel *tree_model;

	GMAMEUISidebar *screenshot_hist_frame;

	GtkWidget *xmame_executables_combo;
	GtkMenuItem *executables_title;
	GtkWidget *executable_menu;

	GtkWidget *clone_selector;

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
void create_filterslist (void);
void create_filterslist_content (void);
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
GdkPixbuf * get_icon_for_filter (simple_filter *current_filter);
GdkPixbuf * gmameui_get_icon_from_stock (const char *id);
GtkWidget * gmameui_get_image_from_stock (const char *id);
void        gmameui_icons_init (void);

void create_gamelist_content (void);
void create_gamelist (ListMode list_mode);
RomEntry * gamelist_get_selected_game (void);
void gamelist_popupmenu_show (RomEntry *rom, GdkEventButton *event);
void show_progress_bar (void);
void hide_progress_bar (void);
void update_progress_bar (gfloat current_value);
void update_game_in_list (RomEntry *rom);
void select_inp (RomEntry *Rom, gboolean play_record);
void create_Menu (void);
void select_game (RomEntry *rom);

#endif /* __GUI_H__ */
