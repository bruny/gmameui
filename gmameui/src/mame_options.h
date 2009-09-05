/*
 * GMAMEUI
 *
 * Copyright 2007-2009 Andrew Burton <adb@iinet.net.au>
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

#ifndef _MAME_OPTIONS_H
#define _MAME_OPTIONS_H

G_BEGIN_DECLS

#define MAME_TYPE_OPTIONS        (mame_options_get_type ())
#define MAME_OPTIONS(o)          (G_TYPE_CHECK_INSTANCE_CAST ((o), MAME_TYPE_OPTIONS, MameOptions))
#define MAME_OPTIONS_CLASS(k)    (G_TYPE_CHECK_CLASS_CAST((k), MAME_TYPE_OPTIONS, MameOptionsClass))
#define MAME_IS_OPTIONS(o)       (G_TYPE_CHECK_INSTANCE_TYPE ((o), MAME_TYPE_OPTIONS))
#define MAME_IS_OPTIONS_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), MAME_TYPE_OPTIONS))

typedef struct _MameOptions        MameOptions;
typedef struct _MameOptionsClass   MameOptionsClass;
typedef struct _MameOptionsPriv    MameOptionsPriv;

struct _MameOptions
{
	GObject parent;
	
	/*< private >*/
	MameOptionsPriv *priv;
};

struct _MameOptionsClass
{
	GObjectClass parent;
};

GType mame_options_get_type (void);

MameOptions *
mame_options_new (void);

/* Generate the command-line for the executable */
gchar *
mame_options_get_option_string (MameOptions *pr, gchar *category);

gchar *
mame_options_get (MameOptions *opts, const gchar *key);

gint
mame_options_get_int (MameOptions *opts, const gchar *key);

gboolean
mame_options_get_bool (MameOptions *opts, const gchar *key);

gdouble
mame_options_get_dbl (MameOptions *opts, const gchar *key);

void
mame_options_set_int (MameOptions *opts, gchar *key, gint value);

void
mame_options_set_dbl (MameOptions *opts, gchar *key, gdouble value);

void
mame_options_set_string (MameOptions *opts, gchar *key, gchar* value);

G_END_DECLS

#endif
