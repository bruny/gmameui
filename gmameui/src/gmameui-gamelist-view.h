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

#ifndef __GMAMEUI_GAMELIST_VIEW_H__
#define __GMAMEUI_GAMELIST_VIEW_H__

#include "common.h"
//#include "callbacks.h"
#include "interface.h"
#include "gmameui.h"

guint visible_games;

void create_gamelist_content (void);
void create_gamelist (ListMode list_mode);

gboolean foreach_find_random_rom_in_store (GtkTreeModel *model,
					   GtkTreePath  *path,
					   GtkTreeIter  *iter,
					   gpointer      user_data);
gboolean foreach_find_rom_in_store (GtkTreeModel *model,
				    GtkTreePath  *path,
				    GtkTreeIter  *iter,
				    gpointer      user_data);
void     on_collapse_all_activate               (GtkMenuItem *menuitem,
                                                 gpointer         user_data);
void     on_expand_all_activate                 (GtkMenuItem *menuitem,
                                                 gpointer         user_data);
void     on_displayed_list_resize_column        (GtkWidget *widget,
                                                 GtkRequisition *requisition,
                                                 gpointer user_data);
void     on_displayed_list_sort_column_changed  (GtkTreeSortable *treesortable,
                                                 gpointer user_data);
void     on_displayed_list_row_collapsed        (GtkTreeView *treeview,
                                                 GtkTreeIter *arg1,
                                                 GtkTreePath *arg2,
                                                 gpointer user_data);
/* Main list */
void on_row_selected (GtkTreeSelection *selection, gpointer data);

gboolean on_list_keypress (GtkWidget *widget, GdkEventKey *event, gpointer user_data); 
gboolean on_list_clicked (GtkWidget *widget, GdkEventButton *event, gpointer user_data);
gboolean on_column_click (GtkWidget *button, GdkEventButton *event, GtkTreeViewColumn* column);

/* Column Popup menu */
void on_column_hide_activate (GtkMenuItem *menuitem, gpointer user_data);

RomEntry * gamelist_get_selected_game (void);

void gmameui_gamelist_rebuild ();

#endif /* __GMAMEUI_GAMELIST_VIEW_H__ */
