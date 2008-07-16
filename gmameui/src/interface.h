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

#ifndef __INTERFACE_H__
#define __INTERFACE_H__

GtkWidget* create_MainWindow (void);
void on_hpaned_position_notify (GObject *object, GParamSpec *pspec, gpointer data);
void on_folder_list_activate (GtkAction *action, gpointer user_data);
void on_toolbar_view_menu_activate (GtkAction *action, gpointer user_data);
void on_status_bar_view_menu_activate (GtkAction *action, gpointer user_data);
void on_screen_shot_activate (GtkAction *action, gpointer user_data);
void on_list_view_menu_activate (GtkCheckMenuItem *menuitem, gpointer user_data);
void on_list_tree_view_menu_activate (GtkCheckMenuItem *menuitem, gpointer user_data);
void on_details_view_menu_activate (GtkCheckMenuItem *menuitem, gpointer user_data);
void on_details_tree_view_menu_activate (GtkCheckMenuItem *menuitem, gpointer user_data);

void on_view_type_changed (GtkRadioAction *action, gpointer user_data);

void set_status_bar (gchar *game_name, gchar *game_status);
void show_progress_bar (void);
void hide_progress_bar (void);
void update_progress_bar (gfloat current_value);

enum {
	TOOLBAR_PLAYGAME,
	TOOLBAR_VIEWFOLDERLIST,
	TOOLBAR_VIEWSIDEBAR,
	TOOLBAR_LISTVIEW,
	TOOLBAR_TREEVIEW,
	TOOLBAR_DETAILSLISTVIEW,
	TOOLBAR_DETAILSTREEVIEW,
	NUM_TOOLBAR_ITEMS
};

typedef struct _toolbar_item toolbar_item;

struct _toolbar_item {
	gint id;
	gchar *ui_name;		/* Name from gmameui-ui.xml e.g. /ToolBar/ViewFolderList */
	gchar *icon_name;   /* For non-stock menubar items e.g. gmameui-view-folders */
	gchar *stock_id;	/* For stock menubar items e.g. GTK_STOCK_NEW */
	gchar *label;		/* Label name e.g. N_("Show Folders") */
};

static const toolbar_item toolbar_items [] = {
	{ TOOLBAR_PLAYGAME, "/ToolBar/FilePlayGame", NULL, GTK_STOCK_NEW, N_("Play Game") },
	{ TOOLBAR_VIEWFOLDERLIST, "/ToolBar/ViewFolderList", "gmameui-view-folders", 0, N_("Show Folders") },
	{ TOOLBAR_VIEWSIDEBAR, "/ToolBar/ViewSidebarPanel", "gmameui-view-screenshot", 0, N_("Show Sidebar") },
	{ TOOLBAR_LISTVIEW, "/ToolBar/ViewListView", "gmameui-view-list", 0, N_("List") },
	{ TOOLBAR_TREEVIEW, "/ToolBar/ViewTreeView", "gmameui-view-tree", 0, N_("List Tree") },
	{ TOOLBAR_DETAILSLISTVIEW, "/ToolBar/ViewDetailsListView", "gmameui-view-list", 0, N_("Details") },
	{ TOOLBAR_DETAILSTREEVIEW, "/ToolBar/ViewDetailsTreeView", "gmameui-view-tree", 0, N_("Details Tree") },
};

#endif /* __INTERFACE_H__ */
