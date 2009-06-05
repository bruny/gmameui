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

#include <gtk/gtk.h>

#include "rom_entry.h"  /* For MameRomEntry */
#include "gmameui.h"	/* For ListMode */

G_BEGIN_DECLS

/* Preferences dialog object */
#define MAME_TYPE_GAMELIST_VIEW        (mame_gamelist_view_get_type ())
#define MAME_GAMELIST_VIEW(o)          (G_TYPE_CHECK_INSTANCE_CAST ((o), MAME_TYPE_GAMELIST_VIEW, MameGamelistView))
#define MAME_GAMELIST_VIEW_CLASS(k)    (G_TYPE_CHECK_CLASS_CAST((k), MAME_TYPE_GAMELIST_VIEW, MameGamelistViewClass))
#define MAME_IS_GAMELIST_VIEW(o)       (G_TYPE_CHECK_INSTANCE_TYPE ((o), MAME_TYPE_GAMELIST_VIEW))
#define MAME_IS_GAMELIST_VIEW_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), MAME_TYPE_GAMELIST_VIEW))
#define MAME_GAMELIST_VIEW_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), MAME_TYPE_GAMELIST_VIEW, MameGamelistViewClass))

typedef struct _MameGamelistView        MameGamelistView;
typedef struct _MameGamelistViewClass   MameGamelistViewClass;
typedef struct _MameGamelistViewPrivate MameGamelistViewPrivate;

struct _MameGamelistView {
	GtkTreeView parent;
	
	MameGamelistViewPrivate *priv;
};

struct _MameGamelistViewClass {
	GtkTreeViewClass parent_class;

};

GType mame_gamelist_view_get_type (void);
MameGamelistView *mame_gamelist_view_new (void);

guint visible_games;
#ifdef TREESTORE
void mame_gamelist_view_change_model        (MameGamelistView *gamelist_view);
#endif
void mame_gamelist_view_select_random_game  (MameGamelistView *gamelist_view,
                                             gint              i);
void mame_gamelist_view_update_game_in_list (MameGamelistView *gamelist_view,
                                             MameRomEntry     *tmprom);
/*GtkTreeModel *
mame_gamelist_view_get_data_model     (MameGamelistView *gamelist_view);*/
void
mame_gamelist_view_change_views (MameGamelistView *gamelist_view);
#ifdef TREESTORE
void on_collapse_all_activate               (GtkMenuItem      *menuitem,
                                             gpointer          user_data);
void on_expand_all_activate                 (GtkMenuItem      *menuitem,
                                             gpointer          user_data);
#endif
void on_column_hide_activate                (GtkMenuItem      *menuitem,
                                             gpointer          user_data);

MameRomEntry* gamelist_get_selected_game (void);

void gmameui_gamelist_rebuild ();
gboolean adjustment_scrolled_delayed (MameGamelistView *gamelist_view);

void mame_gamelist_view_scroll_to_selected_game (MameGamelistView *gamelist_view);

void mame_gamelist_view_repopulate_contents (MameGamelistView *gamelist_view);

G_END_DECLS

#endif /* __GMAMEUI_GAMELIST_VIEW_H__ */
