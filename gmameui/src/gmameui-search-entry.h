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

#ifndef __GMAMEUI_SEARCH_ENTRY_H__
#define __GMAMEUI_SEARCH_ENTRY_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

/* Preferences dialog object */
#define MAME_TYPE_SEARCH_ENTRY        (mame_search_entry_get_type ())
#define MAME_SEARCH_ENTRY(o)          (G_TYPE_CHECK_INSTANCE_CAST ((o), MAME_TYPE_SEARCH_ENTRY, MameSearchEntry))
#define MAME_SEARCH_ENTRY_CLASS(k)    (G_TYPE_CHECK_CLASS_CAST((k), MAME_TYPE_SEARCH_ENTRY, MameSearchEntryClass))
#define MAME_IS_SEARCH_ENTRY(o)       (G_TYPE_CHECK_INSTANCE_TYPE ((o), MAME_TYPE_SEARCH_ENTRY))
#define MAME_IS_SEARCH_ENTRY_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), MAME_TYPE_SEARCH_ENTRY))
#define MAME_SEARCH_ENTRY_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), MAME_TYPE_SEARCH_ENTRY, MameSearchEntryClass))

typedef struct _MameSearchEntry        MameSearchEntry;
typedef struct _MameSearchEntryClass   MameSearchEntryClass;
typedef struct _MameSearchEntryPrivate MameSearchEntryPrivate;

struct _MameSearchEntry {
	GtkEntry parent;
	
	MameSearchEntryPrivate *priv;
};

struct _MameSearchEntryClass {
	GtkEntryClass parent_class;

	void (*search) (MameSearchEntry *entry, const char *text);
};

GType mame_search_entry_get_type (void);
MameSearchEntry *mame_search_entry_new (void);

G_END_DECLS

#endif /* __GMAMEUI_SEARCH_ENTRY_H__ */
