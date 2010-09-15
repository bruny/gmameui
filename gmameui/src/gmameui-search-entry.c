/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * GMAMEUI
 *
 * Copyright 2009-2010 Andrew Burton <adb@iinet.net.au>
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

#include "common.h"
#include "gmameui-search-entry.h"
#include "gmameui-marshaller.h"

struct _MameSearchEntryPrivate {
	gint search_timeout;
};

G_DEFINE_TYPE (MameSearchEntry, mame_search_entry, GTK_TYPE_ENTRY)

#define MAME_SEARCH_ENTRY_GET_PRIVATE(o)  (MAME_SEARCH_ENTRY (o)->priv)

/* Signals enumeration */
enum
{
	SEARCH_TEXT_CHANGED,    /* Emitted when the search criteria changes */
	LAST_SEARCH_SIGNAL
};
static guint signals[LAST_SEARCH_SIGNAL] = { 0 };

/* Function prototypes */
static void
mame_search_entry_finalize              (GObject *object);

static void
search_entry_changed (MameSearchEntry *entry, gpointer user_data);
static gboolean
search_entry_changed_timeout (GtkWidget *widget);

#if GTK_CHECK_VERSION(2,16,0)
static void
search_entry_clear (GtkEntry *entry, GtkEntryIconPosition icon_pos,
					GdkEvent *event, MameSearchEntry *search_entry);
#endif

/* Boilerplate functions */
static GObject *
mame_search_entry_constructor (GType                  type,
				guint                  n_construct_properties,
				GObjectConstructParam *construct_properties)
{
	GObject          *obj;
	MameSearchEntry *entry;

	obj = G_OBJECT_CLASS (mame_search_entry_parent_class)->constructor (type,
									     n_construct_properties,
									     construct_properties);

	entry = MAME_SEARCH_ENTRY (obj);

	return obj;
}

static void
mame_search_entry_class_init (MameSearchEntryClass *class)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (class);
	GtkObjectClass *gtkobject_class = GTK_OBJECT_CLASS (class);
	GtkEntryClass *gtkentry_class = GTK_ENTRY_CLASS (class);

	gobject_class->constructor = mame_search_entry_constructor;
/*	gobject_class->get_property = mame_search_entry_get_property;
	gobject_class->set_property = mame_search_entry_set_property;*/

	gobject_class->finalize = mame_search_entry_finalize;

	g_type_class_add_private (class,
				  sizeof (MameSearchEntryPrivate));

	/* Signals and properties go here */

	/* Signal emitted when search criteria changes */
	signals[SEARCH_TEXT_CHANGED] = g_signal_new ("search-changed",
						     G_OBJECT_CLASS_TYPE (gobject_class),
						     G_SIGNAL_RUN_LAST,
						     G_STRUCT_OFFSET (MameSearchEntryClass, search),
						     NULL, NULL,     /* Accumulator and accumulator data */
						     gmameui_marshaller_VOID__STRING,
						     G_TYPE_NONE,    /* Return type */
						     1, G_TYPE_STRING);
	
}

static void
mame_search_entry_init (MameSearchEntry *entry)
{
	MameSearchEntryPrivate *priv;


	priv = G_TYPE_INSTANCE_GET_PRIVATE (entry,
					    MAME_TYPE_SEARCH_ENTRY,
					    MameSearchEntryPrivate);

	entry->priv = priv;
	
	/* Initialise private variables */

	/* Add a 'clear' icon if GTK supports it; Rhythmbox uses libsexy if the
	   version is less than 2.16 */
	#if GTK_CHECK_VERSION(2,16,0) 
	gtk_entry_set_icon_from_stock (GTK_ENTRY (entry),
								   GTK_ENTRY_ICON_SECONDARY,
								   GTK_STOCK_CLEAR);
	gtk_entry_set_icon_tooltip_text (GTK_ENTRY (entry),
									 GTK_ENTRY_ICON_SECONDARY,
									 _("Clear the search text"));
	g_signal_connect_object (GTK_ENTRY (entry), "icon-press",
							 G_CALLBACK (search_entry_clear), entry, 0);
	#endif

	g_signal_connect (G_OBJECT (entry), "changed",
					  G_CALLBACK (search_entry_changed), NULL);
}

MameSearchEntry *
mame_search_entry_new (void)
{
	MameSearchEntry *entry;

	entry = MAME_SEARCH_ENTRY (g_object_new (MAME_TYPE_SEARCH_ENTRY, NULL));

	return entry;

}

static void
mame_search_entry_finalize (GObject *object)
{
	MameSearchEntry *entry;
	
GMAMEUI_DEBUG ("Destroying mame search entry...");	
	entry = MAME_SEARCH_ENTRY (object);
	
	
	g_object_unref (entry->priv);
	
	G_OBJECT_CLASS (mame_search_entry_parent_class)->finalize (object);

/*	GTK_OBJECT_CLASS (mame_search_entry_parent_class)->destroy (object);*/
	
GMAMEUI_DEBUG ("Destroying mame search entry... done");
}

static gboolean
search_entry_changed_timeout (GtkWidget *widget)
{
	MameSearchEntry *entry;

	entry = (MameSearchEntry *) widget;
	/* Emit the signal so that the gmameui-gamelist-view can handle it. */
	gdk_threads_enter ();

	g_signal_emit (G_OBJECT (widget), signals[SEARCH_TEXT_CHANGED], 0,
				   gtk_entry_get_text (GTK_EDITABLE (widget)));
	entry->priv->search_timeout = 0;
	
	gdk_threads_leave ();
	
	return FALSE;
}

/* Callback for when the MameSearchEntry field is changed; this adds a
   timeout to fire every 300ms to reduce the frequency of the refresh */
static void
search_entry_changed (MameSearchEntry *entry, gpointer user_data)
{
	GtkTreeModelFilter *filter;
	
	if (entry->priv->search_timeout != 0) {
		g_source_remove (entry->priv->search_timeout);
		entry->priv->search_timeout = 0;
	}
	
	/* emit it now if we're clearing the entry */
	if (gtk_entry_get_text (GTK_ENTRY (entry)))
		entry->priv->search_timeout = g_timeout_add (300, (GSourceFunc) search_entry_changed_timeout, entry);
	else
		search_entry_changed_timeout (entry);
 
}

#if GTK_CHECK_VERSION(2,16,0)
static void
search_entry_clear (GtkEntry *entry, GtkEntryIconPosition icon_pos,
					GdkEvent *event, MameSearchEntry *search_entry)
{
	gtk_entry_set_text (GTK_ENTRY (search_entry), "");
}
#endif