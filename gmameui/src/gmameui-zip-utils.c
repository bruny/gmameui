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

#include <archive.h>
#include <archive_entry.h>
#include <string.h>

#include "gmameui-zip-utils.h"
#include "common.h"

static GdkPixbuf *
load_pixbuf_data (gchar *data, gint size)
{
	GdkPixbufLoader *loader;
	GdkPixbuf *pixbuf;
	GError *error = NULL;
	
	pixbuf = NULL;
	
	loader = gdk_pixbuf_loader_new ();
	
	if (gdk_pixbuf_loader_write (loader,
								 (guchar *) data,
								 (gsize) size,
								 &error)) {
		pixbuf = gdk_pixbuf_loader_get_pixbuf (loader);
		gdk_pixbuf_loader_close (loader, &error);
		GMAMEUI_DEBUG (_("Loaded pixbuf"));
	} else {
		GMAMEUI_DEBUG ("Error while loading pixbuf data: %s", error->message);
		g_error_free (error);
		error = NULL;
	}
	
	return pixbuf;
}

/* Given a specified zip file and target romname, return the relevant pixbuf */
GdkPixbuf *
read_pixbuf_from_zip_file (gchar *zipfilename, gchar *romname)
{
	GdkPixbuf *pixbuf;
	gchar *buffer_data; /* Space to read found pixbuf entry */
	struct archive *zipfile;
	struct archive_entry *zipentry;
	
	g_return_val_if_fail (zipfilename != NULL, NULL);
	g_return_val_if_fail (romname != NULL, NULL);
	
	zipfile = archive_read_new ();
	
	g_return_val_if_fail (zipfile != NULL, NULL);

	archive_read_support_compression_all (zipfile); /* Check that we can support all compression levels */
	archive_read_support_format_zip (zipfile);  /* Check that we can support zipfiles */
	
	if (archive_read_open_file (zipfile, zipfilename, 10240) != ARCHIVE_OK) {
		GMAMEUI_DEBUG ("Error opening the archive %s", zipfilename);
		return NULL;
	}

	pixbuf = NULL;
	
	while (archive_read_next_header (zipfile, &zipentry) == ARCHIVE_OK) {
		gint filesize;  /* Uncompressed size of the file */

		filesize = archive_entry_size (zipentry);

		if (g_ascii_strncasecmp ((gchar *) archive_entry_pathname (zipentry),
								 romname,
								 strlen (romname)) == 0) {
			GMAMEUI_DEBUG ("Found entry in zip file for ROM %s", romname);

			buffer_data = (gchar *) g_malloc0 (filesize);
			
			/* Read the zipped pixbuf into memory */
			archive_read_data_into_buffer (zipfile, buffer_data, filesize);
			
			pixbuf = load_pixbuf_data (buffer_data, filesize);
									 
			g_free (buffer_data);

			break;  /* Found it - no need to continue processing */
		}
	}
	
	/* Close and clean up */
	archive_read_close (zipfile);
	archive_read_finish (zipfile);
	
	return pixbuf;
	
}