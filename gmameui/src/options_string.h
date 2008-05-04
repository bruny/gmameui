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

#ifndef __OPTIONS_STRING_H__
#define __OPTIONS_STRING_H__

#include "xmame_executable.h"
#include "game_options.h"

gchar *
create_rompath_options_string (XmameExecutable *exec);
gchar *
create_io_options_string (XmameExecutable *exec);

char * create_options_string(XmameExecutable *exec, GameOptions *target);

/* options string creation */
char *
create_vector_options_string (XmameExecutable *exec, GameOptions *target);

#endif /* __OPTIONS_STRING_H__ */
