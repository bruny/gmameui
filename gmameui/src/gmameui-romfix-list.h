/*
 * GMAMEUI
 *
 * Copyright 2010 Andrew Burton <adb@iinet.net.au>
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

#ifndef _GMAMEUI_ROMFIX_LIST_H
#define _GMAMEUI_ROMFIX_LIST_H

//#include <glib-object.h>
#include "common.h"

G_BEGIN_DECLS

#define GMAMEUI_TYPE_ROMFIX_LIST gmameui_romfix_list_get_type()

#define GMAMEUI_ROMFIX_LIST(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
  GMAMEUI_TYPE_ROMFIX_LIST, GMAMEUIRomfixList))

#define GMAMEUI_ROMFIX_LIST_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), \
  GMAMEUI_TYPE_ROMFIX_LIST, GMAMEUIRomfixListClass))

#define GMAMEUI_IS_ROMFIX_LIST(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
  GMAMEUI_TYPE_ROMFIX_LIST))

#define GMAMEUI_IS_ROMFIX_LIST_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), \
  GMAMEUI_TYPE_ROMFIX_LIST))

#define GMAMEUI_ROMFIX_LIST_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), \
  GMAMEUI_TYPE_ROMFIX_LIST, GMAMEUIRomfixListClass))

#define GMAMEUI_ROMFIX_LIST_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), GMAMEUI_TYPE_ROMFIX_LIST, GMAMEUIRomfixListPrivate))

typedef struct _GMAMEUIRomfixListPrivate GMAMEUIRomfixListPrivate;

typedef struct {
  GObject parent;

	GMAMEUIRomfixListPrivate *priv;
} GMAMEUIRomfixList;

typedef struct {
  GObjectClass parent_class;
} GMAMEUIRomfixListClass;

typedef struct {
	gchar *romname;
	gchar *container;	/* Name of the romset that is a source for a missing ROM */
	gint status;
} romfix;

/* Struct representing the possible fixes for a romset */
typedef struct {
	gchar *romset_name;     /* Abbreviated (8 char) name */
	gchar *romset_fullname; /* Full name */
	gint status;
	GList *romfixes;        /* GList of romfix structs */
} romset_fixes;

GType gmameui_romfix_list_get_type (void);

GMAMEUIRomfixList* gmameui_romfix_list_new (void);

void gmameui_romfix_list_add (GMAMEUIRomfixList *, romset_fixes *);

G_END_DECLS

#endif /* _GMAMEUI_ROMFIX_LIST_H */
