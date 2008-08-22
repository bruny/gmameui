/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
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

#include "rom_entry.h"
#include "gui.h"	/* For main_gui.gui_prefs */

RomEntry *
rom_entry_new (void)
{
	RomEntry *rom;
	int i;

	rom = (RomEntry*) g_malloc0 (sizeof (RomEntry));

	for (i = 0; i < NB_CPU; i++) {
		rom->cpu_info[i].name = g_strdup ("-");
		rom->sound_info[i].name = g_strdup ("-");
	}
	
	/* fill the some fields with default value if we have problems */
	rom->timesplayed = 0;
	rom->has_roms = UNKNOWN;
	rom->has_samples = UNKNOWN;
	rom->favourite = FALSE;

	return rom;
}

void
rom_entry_free (RomEntry *rom)
{
	int i;
	
	if (!rom)
		return;

	g_free (rom->romname);
	
	/* optimization: clonesort points to romname
	* if original. See: gamelist_add ()
	*/
	if (rom->clonesort != rom->romname)
		g_free (rom->clonesort);

	for (i = 0; i < NB_CPU; i++) {
		if (rom->cpu_info[i].name) g_free (rom->cpu_info[i].name);
		if (rom->sound_info[i].name) g_free (rom->sound_info[i].name);
	}
	
	if (rom->gamename) g_free (rom->gamename);
	if (rom->gamenameext) g_free (rom->gamenameext);

	if (rom->manu) g_free (rom->manu);
	if (rom->cloneof) g_free (rom->cloneof);
	if (rom->romof) g_free (rom->romof);
	if (rom->sampleof) g_free (rom->sampleof);
	
	/* if (rom->clonesort) g_free (rom->clonesort); points to romname, which has been free'd */
	
	if (rom->icon_pixbuf) g_object_unref (rom->icon_pixbuf);
	
	if (rom->name_in_list) g_free (rom->name_in_list);

	rom = NULL;
}

void
rom_entry_set_name (RomEntry *rom,
		    gchar    *value)
{
	char *p;
	
	if (!rom)
		return;

	if (!g_strncasecmp (value, "The ",4))
	{
		value += 4;
		rom->the_trailer = TRUE;
		rom->name_in_list = NULL;
	} else {
		rom->name_in_list = g_strdup (value);
	}

	for (p = value; *p && (*p != '/') && (*p != '(');p++);

	if (*p == '/') {

		/*Fix for F/A (Japan)*/
		if (!strncmp ((p - 1), "F/A", 3))
		{
			rom->gamenameext = g_strdup (p + 2);
			*(p + 2) = '\0';
		}
		else {
			rom->gamenameext = g_strdup (p + 1);
			*p = '\0';
		}
	
	} else if (*p =='(') {
		rom->gamenameext = g_strdup (p);
		*p = 0;
	}

	if (!rom->gamenameext)
		rom->gamenameext = g_strdup ("");

	rom->gamename = g_strdup (value);
}

void
rom_entry_set_driver (RomEntry    *rom,
		      const gchar *driver)
{
	rom->driver = g_strdup (driver);
	mame_gamelist_add_driver (gui_prefs.gl, driver);
}

void
rom_entry_set_year (RomEntry    *rom,
		    const gchar *year)
{
	rom->year = g_strdup (year);
	mame_gamelist_add_year (gui_prefs.gl, year);
}

ControlType get_control_type (gchar *control_type)
{
	ControlType type;
	
	g_return_val_if_fail (control_type != NULL, 0);

	/* FIXME TODO There are additional types to add */
	if (g_ascii_strcasecmp (control_type, "trackball") == 0)
		type = TRACKBALL;
	else if (g_ascii_strcasecmp (control_type, "lightgun") == 0)
		type = LIGHTGUN;
	else
		type = JOYSTICK;
	
	return type;
}

DriverStatus get_driver_status (gchar *driver_status)
{
	DriverStatus status;
	
	g_return_val_if_fail (driver_status != NULL, 0);

	/* FIXME TODO There are additional types to add */
	if (g_ascii_strcasecmp (driver_status, "good") == 0)
		status = DRIVER_STATUS_GOOD;
	else if (g_ascii_strcasecmp (driver_status, "imperfect") == 0)
		status = DRIVER_STATUS_IMPERFECT;
	else if (g_ascii_strcasecmp (driver_status, "preliminary") == 0)
		status = DRIVER_STATUS_PRELIMINARY;
	else
		status = DRIVER_STATUS_UNKNOWN;
	
	return status;
}

gchar **
rom_entry_get_manufacturers (RomEntry * rom)
{
	gchar **manufacturer_fields;

	if (!rom->manu)
		return NULL;

	manufacturer_fields = g_strsplit (rom->manu, "]", 0);
	if (!manufacturer_fields[0])
		return NULL;

	if (manufacturer_fields[1])
	{
		/* we have two winners [Company 1] (company 2)*/
		g_strstrip (g_strdelimit (manufacturer_fields[0], "[", ' '));
		g_strstrip (g_strdelimit (manufacturer_fields[1], "()", ' '));
	} else
	{
		g_strfreev (manufacturer_fields);
		manufacturer_fields = g_strsplit (rom->manu, "+", 0);
		if (manufacturer_fields[1] != NULL)
		{
			/* we have two winners  Company1+Company2*/
			g_strstrip (g_strdelimit (manufacturer_fields[0], "[", ' '));
			g_strstrip (g_strdelimit (manufacturer_fields[1], "()", ' '));
		} else
		{
			g_strfreev (manufacturer_fields);
			manufacturer_fields = g_strsplit (rom->manu, "/", 0);
			if (manufacturer_fields[1] != NULL)
			{
				/* we have two winners  Company1/Company2*/
				g_strstrip (g_strdelimit (manufacturer_fields[0], "[", ' '));
				g_strstrip (g_strdelimit (manufacturer_fields[1], "()", ' '));
			}
			else
			{
				g_strfreev (manufacturer_fields);
				manufacturer_fields = g_strsplit (rom->manu, "(", 0);
				if (manufacturer_fields[1] != NULL)
				{
					/* we have two winners  Company1/Company2*/
					g_strstrip (g_strdelimit (manufacturer_fields[0], "[", ' '));
					g_strstrip (g_strdelimit (manufacturer_fields[1], "()", ' '));
				}
			}
		}
	}
	return manufacturer_fields;
}

/* FIXME This function should either set or return. Do we need to do both? See
   where it is called and use the most appropriate (pref. return, not set) */
const gchar *
rom_entry_get_list_name (RomEntry *rom)
{
	gboolean the_prefix;
	
	g_return_val_if_fail (rom != NULL, NULL);

	g_object_get (main_gui.gui_prefs,
		      "theprefix", &the_prefix,
		      NULL);
	
	if (!rom->the_trailer) {
		if (!rom->name_in_list) {
			rom->name_in_list = g_strdup_printf ("%s %s", rom->gamename, rom->gamenameext);
		}
	} else  {
		if (the_prefix) {
			if (!rom->name_in_list || strncmp (rom->name_in_list, "The", 3)) {
				g_free (rom->name_in_list);
				rom->name_in_list = g_strdup_printf ("The %s %s", rom->gamename, rom->gamenameext);
			}
		} else {
			if (!rom->name_in_list || !strncmp (rom->name_in_list, "The", 3)) {
				g_free (rom->name_in_list);
				rom->name_in_list = g_strdup_printf ("%s, The %s", rom->gamename, rom->gamenameext);
			}
		}

	} 

	return rom->name_in_list;
}
