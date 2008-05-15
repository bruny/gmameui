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

#ifndef __CALLBACKS_H__
#define __CALLBACKS_H__

#include <gtk/gtkcheckmenuitem.h>
#include <gtk/gtkmenuitem.h>
#include <gtk/gtktogglebutton.h>
#include <gtk/gtktreeselection.h>
#include <gtk/gtktreeviewcolumn.h>

/* Main window menu: File */
void     on_play_activate                       (GtkAction *action,
                                                 gpointer         user_data);

void     on_play_and_record_input_activate      (GtkAction *action,
                                                 gpointer         user_data);

void     on_playback_input_activate             (GtkAction *action,
                                                 gpointer         user_data);

void     on_select_random_game_activate         (GtkMenuItem     *menuitem,
                                                 gpointer         user_data);

void     on_add_to_favorites_activate           (GtkMenuItem     *menuitem,
                                                 gpointer         user_data);

void     on_remove_from_favorites_activate      (GtkMenuItem     *menuitem,
                                                 gpointer         user_data);

void     on_properties_activate                 (GtkAction        *action,
                                                 gpointer         user_data);

void     on_options_activate                    (GtkAction        *action,
                                                 gpointer         user_data);

void     on_audit_all_games_activate            (GtkMenuItem     *menuitem,
                                                 gpointer         user_data);

void     on_exit_activate                       (GtkMenuItem     *menuitem,
                                                 gpointer         user_data);


/* Main window menu: View */
void     on_toolbar_view_menu_activate          (GtkAction *action,
                                                 gpointer         user_data);

void     on_status_bar_view_menu_activate       (GtkAction *action,
                                                 gpointer         user_data);

void     on_folder_list_activate                (GtkAction *action,
                                                 gpointer         user_data);

void     on_screen_shot_activate                (GtkAction *action,
                                                 gpointer         user_data);

void     on_screen_shot_tab_activate            (GtkAction *action,
                                                 gpointer         user_data);

void     on_list_view_menu_activate             (GtkCheckMenuItem *menuitem,
                                                 gpointer         user_data);

void     on_list_tree_view_menu_activate        (GtkCheckMenuItem *menuitem,
                                                 gpointer         user_data);

void     on_details_view_menu_activate          (GtkCheckMenuItem *menuitem,
                                                 gpointer         user_data);

void     on_details_tree_view_menu_activate     (GtkCheckMenuItem *menuitem,
                                                 gpointer         user_data);

void     on_column_layout_activate              (GtkCheckMenuItem *menuitem,
                                                 gpointer         user_data);

void     on_view_type_changed                   (GtkRadioAction *action,
                                                 gpointer       user_data);

void     on_expand_all_activate                 (GtkMenuItem *menuitem,
                                                 gpointer         user_data);

void     on_collapse_all_activate               (GtkMenuItem *menuitem,
                                                 gpointer         user_data);

void     on_the_prefix_activate                 (GtkCheckMenuItem *menuitem,
                                                 gpointer         user_data);

void     on_refresh_activate                    (GtkAction        *action,
                                                 gpointer         user_data);

/* Main window menu: Option */
void     on_clone_color_menu_activate           (GtkMenuItem     *menuitem,
                                                 gpointer         user_data);

void     on_game_list_font_activate             (GtkMenuItem     *menuitem,
                                                 gpointer         user_data);

void     on_rebuild_game_list_menu_activate     (GtkMenuItem     *menuitem,
                                                 gpointer         user_data);

void     on_directories_menu_activate           (GtkMenuItem     *menuitem,
                                                 gpointer         user_data);

void     on_default_option_menu_activate        (GtkMenuItem	*menuitem,
                                                 gpointer	 user_data);

void     on_startup_option_activate             (GtkMenuItem     *menuitem,
                                                 gpointer         user_data);


/* Main window menu: Help */
void     on_about_activate                      (GtkMenuItem     *menuitem,
                                                 gpointer         user_data);


/* Column Popup menu */
void     on_column_hide_activate                (GtkMenuItem     *menuitem,
                                                 gpointer         user_data);

/* Toolbar */
void     on_filterShowButton_toggled            (GtkToggleToolButton *togglebutton,
                                                 gpointer             user_data);

void     on_snapShowButton_toggled              (GtkToggleToolButton *togglebutton,
                                                 gpointer             user_data);

void     on_mode_button_clicked                 (GtkToggleToolButton *button,
                                                 gpointer             user_data);

/* Main list */
void     on_row_selected                        (GtkTreeSelection *selection,
                                                 gpointer data);

gboolean on_list_clicked                        (GtkWidget *widget,
                                                 GdkEventButton *event,
                                                 gpointer user_data);

void     on_columns_changed                     (GtkTreeView *treeview,
                                                 gpointer user_data);

void     on_displayed_list_resize_column        (GtkWidget *widget,
                                                 GtkRequisition *requisition,
                                                 gpointer user_data);

void     on_displayed_list_sort_column_changed  (GtkTreeSortable *treesortable,
                                                 gpointer user_data);

void     on_displayed_list_row_collapsed        (GtkTreeView *treeview,
                                                 GtkTreeIter *arg1,
                                                 GtkTreePath *arg2,
                                                 gpointer user_data);

gboolean on_column_click                        (GtkWidget *button,
                                                 GdkEventButton *event,
                                                 GtkTreeViewColumn* column);


#endif /* __CALLBACKS_H__ */
