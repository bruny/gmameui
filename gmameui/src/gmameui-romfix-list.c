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

#include "common.h"
#include "gmameui-romfix-list.h"
#include "gmameui-marshaller.h"

G_DEFINE_TYPE (GMAMEUIRomfixList, gmameui_romfix_list, G_TYPE_OBJECT)

struct _GMAMEUIRomfixListPrivate
{
	GList *fixes;
};

/* Signals enumeration */
enum
{
        ROMFIX_LIST_ADDED,               /* Item added to the list of romset fixes */
        LAST_ROMFIX_LIST_SIGNAL
};

/* Audit class stuff */
static guint signals[LAST_ROMFIX_LIST_SIGNAL] = { 0 };


void gmameui_romfix_list_add (GMAMEUIRomfixList *fixeslist, romset_fixes *romset_fixes)
{
	g_return_if_fail (romset_fixes != NULL);
	
	GMAMEUI_DEBUG ("Added item %s to list", romset_fixes->romset_name);

	fixeslist->priv->fixes = g_list_append (fixeslist->priv->fixes, romset_fixes);

	GMAMEUI_DEBUG ("Emitting signal!");
	g_signal_emit (fixeslist, signals[ROMFIX_LIST_ADDED],
	               0, romset_fixes->romset_name, romset_fixes);
	GMAMEUI_DEBUG ("Emitting signal done!");
}

static void
gmameui_romfix_list_finalize (GObject *object)
{
  G_OBJECT_CLASS (gmameui_romfix_list_parent_class)->finalize (object);
}

static void
gmameui_romfix_list_class_init (GMAMEUIRomfixListClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (GMAMEUIRomfixListPrivate));

  object_class->finalize = gmameui_romfix_list_finalize;

	signals[ROMFIX_LIST_ADDED] = g_signal_new ("romfix-list-added",
                                                G_TYPE_FROM_CLASS(klass),
                                                G_SIGNAL_RUN_FIRST,
                                                0,              /* This signal is not handled by the class */
                                                NULL, NULL,     /* Accumulator and accumulator data */
                                                gmameui_marshaller_VOID__STRING_POINTER,
                                                G_TYPE_NONE,    /* Return type */
                                                2, G_TYPE_STRING, G_TYPE_POINTER        /* Two parameters */
                                                );

}

static void
gmameui_romfix_list_init (GMAMEUIRomfixList *self)
{
	self->priv = g_new0 (GMAMEUIRomfixListPrivate, 1);
	
	self->priv->fixes = NULL;
}

GMAMEUIRomfixList*
gmameui_romfix_list_new (void)
{
  return g_object_new (GMAMEUI_TYPE_ROMFIX_LIST, NULL);
}
