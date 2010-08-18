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

#include <gdk-pixbuf/gdk-pixbuf.h>

#ifndef __ZIP_UTILS_H__
#define __ZIP_UTILS_H__


GdkPixbuf *
read_pixbuf_from_zip_file (gchar *zipfilename, gchar *romname);

GList *
get_zip_contents (gchar *zipfilename);

gchar *
get_romname_from_zipfile (gchar *zipfile);

void
rename_rom_in_zip_file (gchar *zipfilename, gchar *oldname, gchar *newname);

#endif /* __ZIP_UTILS__ */
