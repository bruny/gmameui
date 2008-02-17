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

#ifndef __IO_H__
#define __IO_H__

#include "common.h"

#include <glib.h>
#include <stdio.h>

#define FLOAT_BUF_SIZE G_ASCII_DTOSTR_BUF_SIZE
const gchar *
my_dtostr(char* buf, gdouble d);

gboolean
load_games_ini(void);

gboolean
load_gmameui_ini(void);

gboolean
load_dirs_ini(void);

gboolean
load_catver_ini(void);

gboolean
load_gmameuirc(void);

void
quick_check(void);

gboolean
save_games_ini(void);

gboolean
save_gmameui_ini(void);

gboolean
save_dirs_ini(void);

gboolean
save_gmameuirc(void);

GList *
get_ctrlr_list (void);

#endif /* __IO_H__ */
