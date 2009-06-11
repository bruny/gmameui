/*
 * GMAMEUI
 *
 * Copyright 2007-2008 Andrew Burton <adb@iinet.net.au>
 * based on Anjuta code
 * Copyright (C) 2000 - 2003  Naba Kumar  <naba@gnome.org>
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

#ifndef _MAME_OPTIONS_LEGACY_H
#define _MAME_OPTIONS_LEGACY_H

G_BEGIN_DECLS

#define MAME_TYPE_OPTIONS_LEGACY        (mame_options_legacy_get_type ())
#define MAME_OPTIONS_LEGACY(o)          (G_TYPE_CHECK_INSTANCE_CAST ((o), MAME_TYPE_OPTIONS_LEGACY, MameOptionsLegacy))
#define MAME_OPTIONS_LEGACY_CLASS(k)    (G_TYPE_CHECK_CLASS_CAST((k), MAME_TYPE_OPTIONS_LEGACY, MameOptionsLegacyClass))
#define MAME_IS_OPTIONS_LEGACY(o)       (G_TYPE_CHECK_INSTANCE_TYPE ((o), MAME_TYPE_OPTIONS_LEGACY))
#define MAME_IS_OPTIONS_LEGACY_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), MAME_TYPE_OPTIONS_LEGACY))

typedef struct _MameOptionsLegacy        MameOptionsLegacy;
typedef struct _MameOptionsLegacyClass   MameOptionsLegacyClass;
typedef struct _MameOptionsLegacyPriv    MameOptionsLegacyPriv;

struct _MameOptionsLegacy
{
	GObject parent;
	
	/*< private >*/
	MameOptionsLegacyPriv *priv;
};

struct _MameOptionsLegacyClass
{
	GObjectClass parent;
};

GType mame_options_legacy_get_type (void);

MameOptionsLegacy *
mame_options_legacy_new (void);

/* Generate the command-line for the executable */
gchar *
mame_options_legacy_get_option_string (MameOptionsLegacy *opts, gchar *category);

gchar *
mame_legacy_options_get (MameOptionsLegacy *opts, const gchar *key);

gint
mame_legacy_options_get_int (MameOptionsLegacy *opts, const gchar *key);

gboolean
mame_legacy_options_get_bool (MameOptionsLegacy *opts, const gchar *key);

gdouble
mame_legacy_options_get_dbl (MameOptionsLegacy *opts, const gchar *key);

void
mame_legacy_options_set_int (MameOptionsLegacy *opts, gchar *key, gint value);

void
mame_legacy_options_set_dbl (MameOptionsLegacy *opts, gchar *key, gdouble value);

void
mame_legacy_options_set_string (MameOptionsLegacy *opts, gchar *key, gchar* value);


G_END_DECLS

#endif
