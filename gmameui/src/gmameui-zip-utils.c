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

#include "common.h"

#include <archive.h>
#include <archive_entry.h>
#include <string.h>
#include <zip.h>	/* New - for zip_rename etc */

#include "gmameui-zip-utils.h"
#include "rom_entry.h"	/* Needed for individual_rom struct */

#define _FILE_OFFSET_BITS 64	/* Needed for compiling using libarchive on i386 */

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

/* Takes either a full path or filename (including suffix) and strips
   the path from the end and the .zip from the end.

   Used to get the romname for a specified zipfile */
/* FIXME TODO Find all instances using g_strsplit and use this function */
gchar *
get_romname_from_zipfile (gchar *zipfile)
{
	gchar *basename, *romname;
	gchar **split;

	g_return_val_if_fail (zipfile != NULL, NULL);
	
	basename = g_path_get_basename (zipfile);
		
	split = g_strsplit (basename, ".", 0);

	romname = g_strdup (split[0]);
		
	g_strfreev (split);
	g_free (basename);

	return romname;
}

void
rename_rom_in_zip_file (gchar *zipfilename, gchar *oldname, gchar *newname)
{
	struct zip *ziparchive;
	gint i, num_files;
	int result;

	g_return_if_fail (zipfilename != NULL);
	
	ziparchive = zip_open (zipfilename, 0, &result);

	num_files = zip_get_num_files (ziparchive);

	for (i = 0; i < num_files; i++) {
		const char* zip_content;
		zip_content = zip_get_name (ziparchive, i, ZIP_FL_UNCHANGED);

		if (g_ascii_strcasecmp (zip_content, oldname) == 0) {

			result = zip_rename (ziparchive, i, newname);
			if (result == 0)
				GMAMEUI_DEBUG ("   Renamed file in %s at pos %d to %s",
				               zipfilename, i, newname);
			else
				GMAMEUI_DEBUG ("   Attempt to rename file in %s at pos %d to %s failed",
				               zipfilename, i, newname);
			break;
		}
	}

	result = zip_close (ziparchive);
	if (result == 0)
		GMAMEUI_DEBUG ("Zip closed successfully");
	else
		GMAMEUI_DEBUG ("Zip failed to update");
}

GList *get_zip_contents (gchar *zipfilename)
{
	struct zip *ziparchive;
	struct zip_stat *zipcompressedfile;
	gint num_files, i;
	int *error;
	GList *contents = NULL;
	
	g_return_val_if_fail (zipfilename != NULL, NULL);
	
	ziparchive = zip_open (zipfilename, 0, error);

	num_files = zip_get_num_files (ziparchive);

	GMAMEUI_DEBUG ("  Opening zip file %s, there are %d files to process", zipfilename, num_files);
	
	for (i = 0; i < num_files; i++) {
		int result;

		zipcompressedfile = g_malloc0 (sizeof (struct zip_stat));
		zip_stat_init (zipcompressedfile);

		result = zip_stat_index (ziparchive, i, 0, zipcompressedfile);

		/*GMAMEUI_DEBUG ("   Found rom %s with CRC %x (result %d)", zipcompressedfile->name,
		               zipcompressedfile->crc, result);*/

		individual_rom *rom_value = (individual_rom *) g_malloc0 (sizeof (individual_rom));

		rom_value->name = g_strdup (zipcompressedfile->name);
		/*rom_value->sha1 = g_strdup (chksum_val);*/

		/* Note - we can't just store the int value of CRC, since the -listxml output
		   provides the string (%x) version, not the integer */
		rom_value->crc = g_strdup_printf ("%x", zipcompressedfile->crc);

		contents = g_list_append (contents, rom_value);

		/* FIXME TODO Free the zipcompressedfile stat */
	}

	zip_close (ziparchive);

	return contents;

}

/* AAA FIXME TODO Have option to skip sha1, which needs to also extract the file to a tmp file */
GList *get_zip_contents_OLD (gchar *zipfilename)
{
	guchar *buffer_data; /* Space to read rom contents */
	struct archive *zipfile;
	struct archive_entry *zipentry;

	GList *contents = NULL;
	
	g_return_val_if_fail (zipfilename != NULL, NULL);
	
	zipfile = archive_read_new ();
	
	g_return_val_if_fail (zipfile != NULL, NULL);

	archive_read_support_compression_all (zipfile); /* Check that we can support all compression levels */
	archive_read_support_format_zip (zipfile);  /* Check that we can support zipfiles */
	
	if (archive_read_open_file (zipfile, zipfilename, 10240) != ARCHIVE_OK) {
		GMAMEUI_DEBUG ("Error opening the archive %s - %s", zipfilename, archive_error_string (zipfile));
		archive_read_finish (zipfile);
		return NULL;
	}

	while (archive_read_next_header (zipfile, &zipentry) == ARCHIVE_OK) {
		gchar *chksum_val;
		gint filesize;  /* Uncompressed size of the file */
		gchar *filename;
		int i;

		filename = (gchar *) archive_entry_pathname (zipentry);
		filesize = archive_entry_size (zipentry);

		if (filesize == 0) {
			/* AAA FIXME TODO Not sure why this is occurring for some ROMs
			 GMAMEUI_DEBUG ("Trying to allocate 0 bytes for %s", filename);
			 */
			continue;
		}
		/* Allocate space to store the zipped ROM */
		buffer_data = (guchar *) g_malloc0 (filesize);

		/* Read the zipped ROM into memory */
		i = archive_read_data_into_buffer (zipfile, buffer_data, filesize);

		if ((i == ARCHIVE_FATAL) || (i == ARCHIVE_WARN)) {
			GMAMEUI_DEBUG ("Error/warning from zip file %s - %s", filename, archive_error_string (zipfile));
		}
/* AAA FIXME TODO If we get the CRC from the header, shouldn't need to get the SHA1 checksum;
   and the only reason we unzip the data into the buffer is to get the SHA1 - so there's a massive
   saving!! */
		chksum_val = g_compute_checksum_for_data (G_CHECKSUM_SHA1, buffer_data, filesize);

		/*GMAMEUI_DEBUG ("Processing zip file... file %s with size %d and sha1 %s", filename, filesize, chksum_val);*/

		individual_rom *rom_value = (individual_rom *) g_malloc0 (sizeof (individual_rom));

		rom_value->name = g_strdup (filename);
		rom_value->sha1 = g_strdup (chksum_val);

		contents = g_list_append (contents, rom_value);
		
		g_free (buffer_data);
		g_free (chksum_val);

	}

	/* Close and clean up */
	archive_read_close (zipfile);
	archive_read_finish (zipfile);

	return contents;
}
