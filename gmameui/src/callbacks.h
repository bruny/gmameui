/*
 * GMAMEUI
 *
 * Copyright 2007-2008 Andrew Burton <adb@iinet.net.au>
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

#ifndef __CALLBACKS_H__
#define __CALLBACKS_H__

/*#include <gtk/gtkmenuitem.h>
#include <gtk/gtktogglebutton.h>
#include <gtk/gtktreeselection.h>
#include <gtk/gtktreeviewcolumn.h>*/

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

void     on_options_default_activate            (GtkAction        *action,
                                                 gpointer         user_data);

void     on_audit_all_games_activate            (GtkMenuItem     *menuitem,
                                                 gpointer         user_data);

void     on_exit_activate                       (GtkMenuItem     *menuitem,
                                                 gpointer         user_data);

void     on_refresh_activate                    (GtkAction        *action,
                                                 gpointer         user_data);

/* Main window menu: Option */
void     on_rebuild_game_list_menu_activate     (GtkMenuItem     *menuitem,
                                                 gpointer         user_data);

void     on_directories_menu_activate           (GtkMenuItem     *menuitem,
                                                 gpointer         user_data);

/*void     on_default_option_menu_activate        (GtkMenuItem	*menuitem,
                                                 gpointer	 user_data);*/

void     on_preferences_activate             (GtkMenuItem     *menuitem,
                                                 gpointer         user_data);


/* Main window menu: Help */
void     on_help_activate                       (GtkAction     *menuitem,
                                                 gpointer       user_data);
                                                 
void     on_about_activate                      (GtkMenuItem     *menuitem,
                                                 gpointer         user_data);

#endif /* __CALLBACKS_H__ */
