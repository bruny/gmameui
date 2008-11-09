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

#include "common.h"

#ifndef __XMAME_OPTIONS_H__
#define __XMAME_OPTIONS_H__

/* MameOption is the result of parsing the command-line MAME options */
typedef struct {
	gchar *name;		/* Name of the option */
	gboolean no_option; /* XMAME only: Is this option toggled by passing -no at the front */
	gchar *description; /* Description */
	gchar *type;		/* XMAME only: MAME argument type, contained in <> in output from help, e.g. string, float, int, arg */
	gchar **keys;
	gchar **possible_values;
} MameOption;

void          xmame_options_init (void);
void          xmame_options_free (void);

void          xmame_option_free (MameOption *opt);
const gchar * xmame_option_get_gmameui_name (const gchar *alias);
const gchar * mame_option_get_gmameui_name (const gchar *alias);

#endif
