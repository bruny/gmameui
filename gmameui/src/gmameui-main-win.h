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

#ifndef __GMAMEUI_MAIN_WIN_H__
#define __GMAMEUI_MAIN_WIN_H__

#include "gmameui-gamelist-view.h"

G_BEGIN_DECLS

/* Preferences dialog object */
#define GMAMEUI_TYPE_MAIN_WIN        (gmameui_main_win_get_type ())
#define GMAMEUI_MAIN_WIN(o)          (G_TYPE_CHECK_INSTANCE_CAST ((o), GMAMEUI_TYPE_MAIN_WIN, GMAMEUIMainWin))
#define GMAMEUI_MAIN_WIN_CLASS(k)    (G_TYPE_CHECK_CLASS_CAST((k), GMAMEUI_TYPE_MAIN_WIN, GMAMEUIMainWinClass))
#define GMAMEUI_IS_MAIN_WIN(o)       (G_TYPE_CHECK_INSTANCE_TYPE ((o), GMAMEUI_TYPE_MAIN_WIN))
#define GMAMEUI_IS_MAIN_WIN_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), GMAMEUI_TYPE_MAIN_WIN))
#define GMAMEUI_MAIN_WIN_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GMAMEUI_TYPE_MAIN_WIN, GMAMEUIMainWinClass))

typedef struct _GMAMEUIMainWin        GMAMEUIMainWin;
typedef struct _GMAMEUIMainWinClass   GMAMEUIMainWinClass;
typedef struct _GMAMEUIMainWinPrivate GMAMEUIMainWinPrivate;

struct _GMAMEUIMainWinClass {
	GtkWindowClass parent_class;
};

struct _GMAMEUIMainWin {
	GtkWindow parent;
	
	GMAMEUIMainWinPrivate *priv;
};

GType gmameui_main_win_get_type (void);
GtkWidget *gmameui_main_win_new (void);

/* FIXME TODO Make this handled in a signal */
void
gmameui_ui_set_items_sensitive (void);

G_END_DECLS

#endif /* __GMAMEUI_MAIN_WIN_H__ */

