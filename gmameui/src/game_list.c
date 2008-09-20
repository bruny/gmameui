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

#include <string.h>
#include <stdlib.h>

#include "game_list.h"
#include "io.h"
#include "gui.h"
#include "rom_entry.h"


#define LINE_BUF 1024

/* Separator for game list fields.
* This avoids problems when source
* files are saved in different encodings.
*/
#define SEP "\xAC"


/* Internal MameGamelist functions */
static void mame_gamelist_class_init (MameGamelistClass *klass);
static void mame_gamelist_init (MameGamelist *gl);
static void mame_gamelist_finalize (GObject *obj);
static void mame_gamelist_set_property (GObject *object,
					guint prop_id,
					const GValue *value,
					GParamSpec *pspec);
static void mame_gamelist_get_property (GObject *object,
					guint prop_id,
					GValue *value,
					GParamSpec *pspec);
const gchar *
glist_insert_unique (GList **list, const gchar *data);

G_DEFINE_TYPE (MameGamelist, mame_gamelist, G_TYPE_OBJECT)

struct _MameGamelistPrivate {
	gchar *name;
	gchar *version;
	gint num_games;
	gint num_sample_games;

	GList *roms;
	GList *years;
	GList *manufacturers;
	GList *drivers;
	GList *categories;
	GList *versions;
	GList *not_checked_list;	/* Only used if def QUICK_CHECK_ENABLED */
};



static void
mame_gamelist_class_init (MameGamelistClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	
	object_class->set_property = mame_gamelist_set_property;
	object_class->get_property = mame_gamelist_get_property;
	object_class->finalize = mame_gamelist_finalize;

	g_object_class_install_property (object_class,
					 PROP_GAMELIST_NAME,
					 g_param_spec_string ("name", "Gamelist Name", "Name of the gamelist", NULL, G_PARAM_READWRITE));
	g_object_class_install_property (object_class,
					 PROP_GAMELIST_VERSION,
					 g_param_spec_string ("version", "Gamelist version", "Version of the gamelist", NULL, G_PARAM_READWRITE));
	g_object_class_install_property (object_class,
					 PROP_GAMELIST_NUM_GAMES,
					 g_param_spec_int ("num-games", "Number of games", "Number of games", 0, 10000, 0, G_PARAM_READWRITE));
	g_object_class_install_property (object_class,
					 PROP_GAMELIST_NUM_SAMPLES,
					 g_param_spec_int ("num-samples", "Number of samples", "Number of samples", 0, 10000, 0, G_PARAM_READWRITE));
}

static void
mame_gamelist_init (MameGamelist *gl)
{
	
GMAMEUI_DEBUG ("Creating mame_gamelist object");	
	gl->priv = g_new0 (MameGamelistPrivate, 1);
	
GMAMEUI_DEBUG ("Creating mame_gamelist object... done");
}

MameGamelist* mame_gamelist_new (void)
{
	return g_object_new (MAME_TYPE_GAMELIST, NULL);
}

static void
mame_gamelist_finalize (GObject *obj)
{
	GMAMEUI_DEBUG ("Finalising mame_gamelist object");
	
	MameGamelist *gl = MAME_GAMELIST (obj);
	
	if (gl->priv->name)
		g_free (gl->priv->name);
	if (gl->priv->version)
		g_free (gl->priv->version);
GMAMEUI_DEBUG ("Freeing roms");
	if (gl->priv->roms)
	{
		g_list_foreach (gl->priv->roms, (GFunc) rom_entry_free, NULL);
		g_list_free (gl->priv->roms);
	}
GMAMEUI_DEBUG ("Freeing roms... done");
	if (gl->priv->years)
	{
		g_list_foreach (gl->priv->years, (GFunc) g_free, NULL);
		g_list_free (gl->priv->years);
	}
	if (gl->priv->manufacturers)
	{
		g_list_foreach (gl->priv->manufacturers, (GFunc) g_free, NULL);
		g_list_free (gl->priv->manufacturers);
	}
	if (gl->priv->drivers)
	{
		g_list_foreach (gl->priv->drivers, (GFunc) g_free, NULL);
		g_list_free (gl->priv->drivers);
	}
#ifdef QUICK_CHECK_ENABLED
	if (gl->priv->not_checked_list) {
		g_list_foreach (gl->priv->not_checked_list, (GFunc) rom_entry_free, NULL);
		g_list_free (gl->priv->not_checked_list);
	}
#endif
	if (gl->priv->categories) {
		g_list_foreach (gl->priv->categories, (GFunc) g_free, NULL);
		g_list_free (gl->priv->categories);
	}
	
	if (gl->priv->versions) {
		g_list_foreach (gl->priv->versions, (GFunc) g_free, NULL);
		g_list_free (gl->priv->versions);
	}
	
	g_free (gl->priv);
	
	GMAMEUI_DEBUG ("Finalising mame_gamelist object... done");

}

static void
mame_gamelist_set_property (GObject *object,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *pspec)
{
	MameGamelist *gl;
GMAMEUI_DEBUG("Setting gamelist property %d with value %s", prop_id, g_value_get_string (value));
	gl = MAME_GAMELIST (object);

	switch (prop_id) {
		case PROP_GAMELIST_NAME:
			gl->priv->name = g_strdup (g_value_get_string (value));
			break;
		case PROP_GAMELIST_VERSION:
			gl->priv->version = g_strdup (g_value_get_string (value));
			break;
		case PROP_GAMELIST_NUM_GAMES:
			gl->priv->num_games = g_value_get_int (value);
			break;
		case PROP_GAMELIST_NUM_SAMPLES:
			gl->priv->num_sample_games = g_value_get_int (value);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

static void
mame_gamelist_get_property (GObject *object,
			    guint prop_id,
			    GValue *value,
			    GParamSpec *pspec)
{
	MameGamelist *gl = MAME_GAMELIST (object);

	switch (prop_id) {
		case PROP_GAMELIST_NAME:
			g_value_set_string (value, gl->priv->name);
			break;
		case PROP_GAMELIST_VERSION:
			g_value_set_string (value, gl->priv->version);
			break;
		case PROP_GAMELIST_NUM_GAMES:
			g_value_set_int (value, gl->priv->num_games);
			break;
		case PROP_GAMELIST_NUM_SAMPLES:
			g_value_set_int (value, gl->priv->num_sample_games);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

/**
* Inserts a string to a sorted glist if it's not present.
* The function always returns a pointer to the string in the list.
 FIXME TODO Calling functions not using return var
*/
const gchar *
glist_insert_unique (GList **list, const gchar *data) {
	GList *listpointer;
	gchar *data_copy;

	/*g_return_val_if_fail (data != NULL, NULL);*/
	if (data == NULL)
		return NULL;

	/* Check if the data already exists in the list */
	if (*list) {
		listpointer = g_list_first (*list);

		while ( (listpointer != NULL))
		{
			if (!strcmp (listpointer->data, data)) {
				return listpointer->data;
			}
		
			listpointer = g_list_next (listpointer);
		}
	}

	/* Data does not exist in the list - add it */
	data_copy = g_strdup (data);
	*list = g_list_insert_sorted (*list, data_copy, (GCompareFunc)strcmp);

	return data_copy;
}

static gint
compare_game_name (RomEntry *rom1,
		   RomEntry *rom2)
{
	return strcmp (rom1->clonesort, rom2->clonesort);
}

#define FIELDS_PER_RECORD 27 + (NB_CPU * 4)

void mame_gamelist_add (MameGamelist *gl, RomEntry *rom) {
	gchar **manufacturer_fields;
	int i;

	g_return_if_fail (gl != NULL);
	g_return_if_fail (rom != NULL);
	g_return_if_fail (rom->romname != NULL);


	/*generate glist for manufacturers*/
	manufacturer_fields = rom_entry_get_manufacturers (rom);
	if (manufacturer_fields) {
		for (i = 0; i < 2; i++) {
			if (manufacturer_fields[i])
				mame_gamelist_add_manufacturer (gl, manufacturer_fields[i]);
		}				
		g_strfreev (manufacturer_fields);
	}
/* FIXME TODO These should all be set in the rom creation */	
	if (!rom->cloneof) {
		rom->cloneof = g_strdup ("-");
	}

	if (!rom->sampleof) {
		rom->sampleof = g_strdup ("-");
	}

	if (!rom->romof) {
		rom->romof = g_strdup ("-");
	}

	if (rom->cloneof[0] == '-') {

		/* original. Point to romname */
		rom->clonesort = rom->romname;

	} else {
		rom->clonesort = g_strdup_printf ("%s-%s",
			rom->cloneof , rom->romname);
	}

	if (!rom->year)
		rom_entry_set_year (rom, _("Unknown"));

	gl->priv->roms = g_list_insert_sorted (gl->priv->roms, (gpointer) rom, (GCompareFunc )compare_game_name);

	gl->priv->num_games++;

	if  (rom->nb_samples > 0)
		gl->priv->num_sample_games++;
}

GList* mame_gamelist_get_roms_glist (MameGamelist *gl) {
	g_return_val_if_fail (gl != NULL, NULL);
	
	return gl->priv->roms;
}

GList* mame_gamelist_get_categories_glist (MameGamelist *gl) {
	g_return_val_if_fail (gl != NULL, NULL);
	
	return gl->priv->categories;
}

GList* mame_gamelist_get_versions_glist (MameGamelist *gl) {
	g_return_val_if_fail (gl != NULL, NULL);
	
	return gl->priv->versions;
}

gboolean mame_gamelist_load (MameGamelist *gl)
{
	gchar *filename;
	FILE *gamelist;
	gint j;
	gchar line[LINE_BUF];
	gchar **tmp_array;
	gchar *tmp, *p;
	RomEntry *rom;
	gboolean exe_version_checked = FALSE;
	gint offset;
	int i;
	int supported_games = 0;	

	g_return_val_if_fail (gl != NULL, FALSE);
	
	filename = g_build_filename (g_get_home_dir (), ".gmameui", "gamelist", NULL);

	g_message (_("Loading gamelist %s"), filename);
	
	gamelist = fopen (filename, "r");
	g_free (filename);

	if (!gamelist) {
		GMAMEUI_DEBUG ("Could not open gamelist file %s", filename);
		gl->priv->version = g_strdup ("unknown");
		return FALSE;
	}
	
	while (fgets (line, LINE_BUF, gamelist)) {
		p = line;
		tmp = line;

		/* Skip comments */
		if (*tmp != '#') {
			/* FIXME TODO Need to handle gamelist being empty */
			while (*tmp && (*tmp != '\n')) {
				tmp++;
			}
			*tmp = '\0';

			tmp_array = g_strsplit (p, SEP, FIELDS_PER_RECORD);

			/* Check if the record is corrupted */
			for (i=0; i < FIELDS_PER_RECORD; i++) {
				if (!tmp_array[i]) {
					g_strfreev (tmp_array);
					fclose (gamelist);
					gl->priv->version = g_strdup ("unknown");
					gmameui_message (ERROR, NULL, _("Game list is corrupted."));	
					return FALSE;
				}
			}

			rom = rom_entry_new ();

			if (!rom || !tmp_array)
			{
				g_strfreev (tmp_array);
				fclose (gamelist);
				gl->priv->version = g_strdup ("unknown");
				gmameui_message (ERROR, NULL, _("Out of memory while loading gamelist"));
				return FALSE;
			}
			
			g_strlcpy (rom->romname, tmp_array[0], MAX_ROMNAME);
			rom->gamename = g_strdup (tmp_array[1]);
			rom->gamenameext = g_strdup (tmp_array[2]);
			rom->the_trailer = !strcmp (tmp_array[3], "true");
			rom->is_bios = !strcmp (tmp_array[4], "true");
			rom_entry_set_year (rom, tmp_array[5]);
			rom->manu = g_strdup (tmp_array[6]);
			rom->cloneof = g_strdup (tmp_array[7]);
			rom->romof = g_strdup (tmp_array[8]);
			rom_entry_set_driver (rom, tmp_array[9]);
			
			rom->status = atoi (tmp_array[10]);
			rom->driver_status_color = atoi (tmp_array[11]);
			rom->driver_status_sound = atoi (tmp_array[12]);
			rom->driver_status_graphic = atoi (tmp_array[13]);
			rom->colors = atoi (tmp_array[14]);

			/* offset of cpu infos in the array */
			offset = 15;
			for (j = 0; j < NB_CPU; j++)
			{
				if (!strncmp (tmp_array[ (j * 2) + offset], "(sound)", 7)) {
					p = tmp_array[ (j * 2) + offset];
					p += 7;
					rom->cpu_info[j].name = g_strdup (p);
					rom->cpu_info[j].sound_flag = TRUE;
				}
				else
				{
					rom->cpu_info[j].name = g_strdup (tmp_array[ (j * 2) + offset]);
					rom->cpu_info[j].sound_flag = FALSE;
				}
				rom->cpu_info[j].clock = atoi (tmp_array[ (j * 2) + offset + 1]);
			}

			/* calculate offset of sound cpu infos in the array */
			offset = 15 + (NB_CPU * 2);

			for (j = 0; j < NB_CPU; j++)
			{
				if (strcmp (tmp_array[offset + (j * 2)], "")) {
					rom->sound_info[j].name = g_strdup (tmp_array[offset + (j * 2)]);
				}
				rom->sound_info[j].clock = atoi (tmp_array[offset + (j * 2) + 1]);
			}

			offset = 15 + (NB_CPU * 4);

			rom->num_players = atoi (tmp_array[offset + 0]);
			rom->num_buttons = atoi (tmp_array[offset + 1]);
			rom->control = atoi (tmp_array[offset + 2]);

			rom->vector = !strcmp (tmp_array[offset + 3], "true");
		
			rom->screen_x = atoi (tmp_array[offset + 4]);
			rom->screen_y = atoi (tmp_array[offset + 5]);
			rom->screen_freq = atoi (tmp_array[offset + 6]);
			rom->horizontal = (*tmp_array[offset + 7] == 'h');

			rom->channels = atoi (tmp_array[offset + 8]);

			rom->nb_roms = atoi (tmp_array[offset + 9]);
			rom->nb_samples = atoi (tmp_array[offset + 10]);
			rom->sampleof = g_strdup (tmp_array[offset + 11]);

			g_strfreev (tmp_array);

			mame_gamelist_add (gl, rom);
			supported_games++;

		} else if (!exe_version_checked) {
			/* Check for the GMAMEUI version in the top comment line */
			while (*tmp && (*tmp != '\n')) {
				tmp++;
			}
			*tmp = '\0';
			tmp_array = g_strsplit (p, " ", 3);

			GMAMEUI_DEBUG ("Checking version of gamelist file: %s - %s", tmp_array[1], tmp_array[2]);

			if (strcmp (tmp_array[1], "GMAMEUI") || !tmp_array[2]) {
				gl->priv->version = g_strdup ("unknown");
				g_strfreev (tmp_array);
				fclose (gamelist);
				return FALSE;
			}
			if (g_ascii_strtod (tmp_array[2], NULL) < 0.91)
			{
				gl->priv->version = g_strdup ("too old");
				g_strfreev (tmp_array);
				fclose (gamelist);
				return FALSE;
			}
			if (g_ascii_strtod (tmp_array[2], NULL) > 0.91)
			{
				gl->priv->version = g_strdup ("unknown");
				g_strfreev (tmp_array);
				fclose (gamelist);
				return FALSE;
			}
			exe_version_checked = TRUE;
			GMAMEUI_DEBUG ("Checking version of gamelist file - OK");
			g_strfreev (tmp_array);

		}else {
			while (*tmp && (*tmp != '\n')) {
				tmp++;
			}
			*tmp = '\0';
			p += 2; /* Skip # */

			if (!strncmp (p, "Version", 7))
			{
				p += 8;
				gl->priv->version = g_strdup (p);
			} if (!strncmp (p, "Name", 4))
			{
				p += 5;
				gl->priv->name = g_strdup (p);
			}
		}
	}
	fclose (gamelist);

	GMAMEUI_DEBUG ("List for %s %s", gl->priv->name, gl->priv->version);
	g_message (_("Loaded %d roms by %d manufacturers covering %d years."), gl->priv->num_games,
				g_list_length (gl->priv->manufacturers), g_list_length (gl->priv->years));
	g_message (_("with %d games supporting samples."), gl->priv->num_sample_games);

	return (TRUE);	
}

/**
* Appends a rom entry to the gamelist.
*/
static void
mame_gamelist_print (FILE     *handle,
		RomEntry *rom)
{
	int i;
	char float_buf[FLOAT_BUF_SIZE];


	if (!rom)
		return;

	fprintf (handle,
		"%s" SEP	/* romname */
		"%s" SEP	/* gamename */
		"%s" SEP	/* gamenameext */
		"%s" SEP	/* the trailer */
		"%s" SEP	/* is_bios */
		"%s" SEP	/* year */
		"%s" SEP	/* manu */
		"%s" SEP	/* clone of */
		"%s" SEP	/* rom of */
		"%s" SEP	/* driver */
		"%d" SEP	/* status */
		"%d" SEP	/* driver color status */
		"%d" SEP	/* driver sound status */
		"%d" SEP	/* driver sound graphic */
		"%i" SEP	/* colors */
		,
		rom->romname,
		rom->gamename,
		rom->gamenameext,
		rom->the_trailer ? "true" : "false",
		rom->is_bios ? "true" : "false",
		rom->year,
		rom->manu,
		rom->cloneof,
		rom->romof,
		rom->driver,
		rom->status,
		rom->driver_status_color,
		rom->driver_status_sound,
		rom->driver_status_graphic,
		rom->colors
	);

	for (i=0; i < NB_CPU; i++) {
		fprintf (handle, "%s" SEP "%i" SEP, 
			rom->cpu_info[i].name, rom->cpu_info[i].clock);
	}
	for (i=0; i < NB_CPU; i++) {
		fprintf (handle, "%s" SEP "%i" SEP, 
			rom->sound_info[i].name, rom->sound_info[i].clock);
	}

	fprintf (handle,
		"%i" SEP	/* players */
		"%i" SEP	/* buttons */
		"%d" SEP	/* control */
		"%s" SEP	/* vector */
		"%i" SEP	/* screen X */
		"%i" SEP	/* screen Y */
		"%s" SEP	/* screen frequency */
		"%s" SEP	/* orientation */
		"%i" SEP	/* channels */
		"%i" SEP	/* roms */
		"%i" SEP	/* samples */
		"\n",
		rom->num_players,
		rom->num_buttons,
		rom->control,
		rom->vector ? "true" : "false",
		rom->screen_x,
		rom->screen_y,
		my_dtostr (float_buf, rom->screen_freq),
		rom->horizontal ? "horizontal" : "vertical",
		rom->channels,
		rom->nb_roms,
		rom->nb_samples
	);

}

gboolean mame_gamelist_save (MameGamelist *gl) {
	GList *listpointer;
	FILE *gamelist;
GMAMEUI_DEBUG ("Saving gamelist");
	g_message (_("Saving gamelist."));
	
	g_return_val_if_fail (gl != NULL, FALSE);
	
	gchar *filename = g_build_filename (g_get_home_dir (), ".gmameui", "gamelist", NULL);
  
	gamelist = fopen (filename, "w");
	g_free (filename);

	g_return_val_if_fail (gamelist != NULL, FALSE);

	fprintf (gamelist,
		"# GMAMEUI 0.91\n"
		"# Name %s\n"
		"# Version %s\n"
		"# list of xmame games for GMAMEUI front-end\n"
		"# The fileformat is: "
		"romname" SEP
		"gamename" SEP
		"gamenameext" SEP
		"the_trailer" SEP
		"is_bios" SEP
		"year" SEP
		"manufacturer" SEP
		"cloneof" SEP
		"romof" SEP
		"driver" SEP
		"driverstatus" SEP
		"drivercolorstatus" SEP
		"driversoundstatus" SEP
		"drivergraphicstatus" SEP
		"colors" SEP
		"cpu1" SEP
		"cpu1_clock" SEP
		"cpu2" SEP
		"cpu2_clock" SEP
		"cpu3" SEP
		"cpu3_clock" SEP
		"cpu4" SEP
		"cpu4_clock" SEP
		"sound1" SEP
		"sound1_clock" SEP
		"sound2" SEP
		"sound2_clock" SEP
		"sound3" SEP
		"sound3_clock" SEP
		"sound4" SEP
		"sound4_clock" SEP
		"num_players" SEP
		"num_buttons" SEP
		"control" SEP
		"vector" SEP
		"screen_x" SEP
		"screen_y" SEP
		"screen_freq" SEP
		"horizontal" SEP
		"channels" SEP
		"num_roms" SEP
		"num_samples" SEP
		"sampleof\n",
		gl->priv->name,
		gl->priv->version
	);

	listpointer = g_list_first (gl->priv->roms);

	while (listpointer) {
		mame_gamelist_print (gamelist, (RomEntry*)listpointer->data);
		listpointer = g_list_next (listpointer);
	}

	fclose (gamelist);
GMAMEUI_DEBUG ("Saving gamelist... done");
	return TRUE;
}

/**
* Checks the gamelist and if its needs to be build/rebuild
* it asks the user and proceeds accordingly.
*
* Checks:
* - Gamelist was read but it was a version we don't support.
* - Gamelist is not available.
* - Gamelist was read but it was created with a very old version of gmameui.
* - Gamelist does not match the current executable (and VersionCheck = TRUE)
*/
void
gamelist_check (XmameExecutable *exec)
{
	GtkWidget *dialog = NULL;

	gint result;
	gboolean versioncheck;  /* Check the gamelist against the current executable */
	gchar *gl_name, *gl_version;
	
	g_object_get (main_gui.gui_prefs,
		      "versioncheck", &versioncheck,
		      NULL);

	if (!exec)
		return;

	g_object_get (gui_prefs.gl,
		      "name", &gl_name,
		      "version", &gl_version,
		      NULL);
	
	if (!gl_version || !strcmp (gl_version, "unknown")) {
		dialog = gtk_message_dialog_new (GTK_WINDOW (MainWindow),
							GTK_DIALOG_MODAL,
							GTK_MESSAGE_WARNING,
							GTK_BUTTONS_YES_NO,
							_("Could not recognise the gamelist version.\n"
							  "Do you want to rebuild the gamelist?"));

	} else if (!strcmp (gl_version, "none")) {

		dialog = gtk_message_dialog_new (GTK_WINDOW (MainWindow),
							GTK_DIALOG_MODAL,
							GTK_MESSAGE_WARNING,
							GTK_BUTTONS_YES_NO,
							_("Gamelist not available,\n"
 							  "Do you want to build the gamelist?"));

	} else if (!strcmp (gl_version, "too old")) {

		dialog = gtk_message_dialog_new (GTK_WINDOW (MainWindow),
							GTK_DIALOG_MODAL,
							GTK_MESSAGE_WARNING,
							GTK_BUTTONS_YES_NO,
							_("Gamelist was created with an older version of GMAMEUI.\n"
							  "The gamelist is not supported.\n"
							  "Do you want to rebuild the gamelist?"));

	} else if (versioncheck) {	
		if (strcmp (exec->name, gl_name) ||
			strcmp (exec->version, gl_version))
		{

			dialog = gtk_message_dialog_new (GTK_WINDOW (MainWindow),
							GTK_DIALOG_MODAL,
							GTK_MESSAGE_WARNING,
							GTK_BUTTONS_YES_NO,
							_("The gamelist is from:\n"
							  "%s %s\n"
							  "and the current executable is:\n"
							  "%s %s\n"
							  "Do you want to rebuild the gamelist?"),
							  gl_name,
							  gl_version,
							  exec->name,
							  exec->version);

		}
	}

	if (dialog) {
		result = gtk_dialog_run (GTK_DIALOG (dialog));
		gtk_widget_destroy (dialog);

		switch (result)
		{
			case GTK_RESPONSE_YES:
					gtk_widget_set_sensitive (main_gui.scrolled_window_games, FALSE);
					UPDATE_GUI;

				save_games_ini ();

				if (gamelist_parse (exec)) {
					mame_gamelist_save (gui_prefs.gl);
					load_games_ini ();
					load_catver_ini ();
					quick_check ();
					create_gamelist_content ();
				}

				gtk_widget_set_sensitive (main_gui.scrolled_window_games, TRUE);
				break;
		}
		
	}		
}

RomEntry* get_rom_from_gamelist_by_name (MameGamelist *gl, gchar *romname) {
	GList *listpointer;
	RomEntry *tmprom = NULL;
	
	g_return_val_if_fail ((gl != NULL), NULL);
	g_return_val_if_fail ((romname != NULL), NULL);
	
	for (listpointer = g_list_first (gl->priv->roms);
	     (listpointer != NULL);
	     listpointer = g_list_next (listpointer))
	{
		tmprom = (RomEntry *) listpointer->data;
		if (!strcmp (tmprom->romname, romname))
		{
			break;
		}
	}

	return tmprom;
}

void mame_gamelist_add_driver (MameGamelist *gl, gchar *driver) {
	g_return_if_fail (gl != NULL);
	
	glist_insert_unique (&gl->priv->drivers, driver);
}

void mame_gamelist_add_year (MameGamelist *gl, gchar *year) {
	g_return_if_fail (gl != NULL);
	
	glist_insert_unique (&gl->priv->years, year);
}

void mame_gamelist_add_version (MameGamelist *gl, gchar *version) {
	g_return_if_fail (gl != NULL);
	
	glist_insert_unique (&gl->priv->versions, version);
}

void mame_gamelist_add_category (MameGamelist *gl, gchar *category) {
	g_return_if_fail (gl != NULL);
	
	glist_insert_unique (&gl->priv->categories, category);
}

void mame_gamelist_add_manufacturer (MameGamelist *gl, gchar *manufacturer) {
	g_return_if_fail (gl != NULL);
	
	glist_insert_unique (&gl->priv->manufacturers, manufacturer);
}

void mame_gamelist_set_not_checked_list (MameGamelist *gl, GList *source) {
	g_return_if_fail (gl != NULL);
	
	gl->priv->not_checked_list = g_list_copy (source);
}
