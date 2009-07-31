/*
 * GMAMEUI
 *
 * Copyright 2009 Andrew Burton <adb@iinet.net.au>
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

#ifndef __GMAMEUI_STATUSBAR_H__
#define __GMAMEUI_STATUSBAR_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

/* Preferences dialog object */
#define GMAMEUI_TYPE_STATUSBAR        (gmameui_statusbar_get_type ())
#define GMAMEUI_STATUSBAR(o)          (G_TYPE_CHECK_INSTANCE_CAST ((o), GMAMEUI_TYPE_STATUSBAR, GMAMEUIStatusbar))
#define GMAMEUI_STATUSBAR_CLASS(k)    (G_TYPE_CHECK_CLASS_CAST((k), GMAMEUI_TYPE_STATUSBAR, GMAMEUIStatusbarClass))
#define GMAMEUI_IS_STATUSBAR(o)       (G_TYPE_CHECK_INSTANCE_TYPE ((o), GMAMEUI_TYPE_STATUSBAR))
#define GMAMEUI_IS_STATUSBAR_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), GMAMEUI_TYPE_STATUSBAR))
#define GMAMEUI_STATUSBAR_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GMAMEUI_TYPE_STATUSBAR, GMAMEUIStatusbarClass))

typedef struct _GMAMEUIStatusbar        GMAMEUIStatusbar;
typedef struct _GMAMEUIStatusbarClass   GMAMEUIStatusbarClass;
typedef struct _GMAMEUIStatusbarPrivate GMAMEUIStatusbarPrivate;

struct _GMAMEUIStatusbar {
	GtkStatusbar parent;
	
	GMAMEUIStatusbarPrivate *priv;
};

struct _GMAMEUIStatusbarClass {
	GtkStatusbarClass parent_class;
};

GType GMAMEUI_statusbar_get_type (void);
GMAMEUIStatusbar *gmameui_statusbar_new (void);

/* Following functions are used while background activity is happening, e.g.
   auditing or loading the gamelist */
void gmameui_statusbar_start_pulse (GMAMEUIStatusbar *sb);
void gmameui_statusbar_stop_pulse (GMAMEUIStatusbar *sb);
void gmameui_statusbar_set_progressbar_text (GMAMEUIStatusbar *sb, gchar *text);

G_END_DECLS

#endif /* __GMAMEUI_STATUSBAR_H__ */
