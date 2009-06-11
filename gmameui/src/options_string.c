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
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "gmameui.h"
#include "gui.h"	/* For main_gui, so we can reach the preferences */
#include "options_string.h"
#include "io.h"
#include "keyboard.h"
#include "mame_options_legacy.h"

gchar *create_io_options_string(MameExec *exec)
{
	char *option_string;
	
	gchar *artwork_dir, *snapshot_dir, *hiscore_dir, *ctrlr_dir, *cheat_file, *hiscore_file, *history_file, *mameinfo_file;
	gchar *nvram_dir, *cfg_dir, *state_dir, *inp_dir, *memcard_dir, *diff_dir, *ini_dir;
	
	gchar *artworkpath_option,
			*screenshotdir_option,
			*hiscoredir_option,
			*mameinfofile_option,
			*historyfile_option,
			*hiscorefile_option,
			*cheatfile_option,
			*diffdir_option,
			*ctrlr_directory_option,
			*cfg_directory_option,
			*inipath_option,
			*nvram_directory_option,
			*memcard_directory_option,
			*input_directory_option,
			*state_directory_option;

	mame_get_options(exec);
	
	/* FIXME TODO Free the strings */
	g_object_get (main_gui.gui_prefs,
		      "dir-artwork", &artwork_dir,
		      "dir-snapshot", &snapshot_dir,
		      "dir-hiscore", &hiscore_dir,
		      "dir-ctrlr", &ctrlr_dir,
		      "file-cheat", &cheat_file,
		      "file-hiscore", &hiscore_file,
		      "file-history", &history_file,
		      "file-mameinfo", &mameinfo_file,
		      "dir-cfg", &cfg_dir,
		      "dir-nvram", &nvram_dir,
		      "dir-state", &state_dir,
		      "dir-inp", &inp_dir,
		      "dir-memcard", &memcard_dir,
		      "dir-diff", &diff_dir,
		      "dir-ini", &ini_dir,
		      NULL);
	
	artworkpath_option = mame_get_option_string(exec, "artwork_directory", artwork_dir);
	screenshotdir_option = mame_get_option_string(exec, "snapshot_directory", snapshot_dir);
 	hiscoredir_option = mame_get_option_string(exec, "hiscore_directory", hiscore_dir);
	cheatfile_option = mame_get_option_string(exec, "cheat_file", cheat_file);
	hiscorefile_option = mame_get_option_string(exec, "hiscore_file", hiscore_file);	
	historyfile_option = mame_get_option_string(exec, "history_file", history_file);
	mameinfofile_option = mame_get_option_string(exec, "mameinfo_file", mameinfo_file);
	diffdir_option = mame_get_option_string(exec, "diff_directory", diff_dir);

	inipath_option = mame_get_option_string(exec, "inipath", ini_dir);
	cfg_directory_option = mame_get_option_string(exec, "cfg_directory", cfg_dir);
	nvram_directory_option = mame_get_option_string(exec, "nvram_directory", nvram_dir);
	ctrlr_directory_option = mame_get_option_string(exec, "ctrlr_directory", ctrlr_dir);
	state_directory_option = mame_get_option_string(exec, "state_directory", state_dir);
	input_directory_option = mame_get_option_string(exec, "input_directory", inp_dir);
	memcard_directory_option = mame_get_option_string(exec, "memcard_directory", memcard_dir);

	option_string = g_strdup_printf("%s "	/* artworkpath_option*/
			"%s "			/* screenshotdir_option */
			"%s "			/* hiscoredir_option */
			"%s "			/* cheatfile_option */
			"%s "			/* hiscorefile_option */
			"%s "			/* historyfile_option */
			"%s "			/* mameinfofile_option */
			"%s "			/* diffdir_option */
			"%s "			/* ctrlr_directory_option */
			"%s "			/* cfg_directory_option */
			"%s "			/* inipath_option */
			"%s "			/* nvram_directory_option */
			"%s "			/* memcard_directory_option */
			"%s "			/* input_directory_option */
			"%s "			/* state_directory_option */
			,
			artworkpath_option?artworkpath_option:"",
			screenshotdir_option?screenshotdir_option:"",
			hiscoredir_option?hiscoredir_option:"",
			cheatfile_option?cheatfile_option:"",
			hiscorefile_option?hiscorefile_option:"",
			historyfile_option?historyfile_option:"",
			mameinfofile_option?mameinfofile_option:"",
			diffdir_option?diffdir_option:"",
			ctrlr_directory_option?ctrlr_directory_option:"",
			cfg_directory_option?cfg_directory_option:"",
			inipath_option?inipath_option:"",
			nvram_directory_option?nvram_directory_option:"",
			memcard_directory_option?memcard_directory_option:"",
			input_directory_option?input_directory_option:"",
			state_directory_option?state_directory_option:""
			);
	g_free(artworkpath_option);
	g_free(screenshotdir_option);
	g_free(hiscoredir_option);
	g_free(cheatfile_option);
	g_free(hiscorefile_option);
	g_free(historyfile_option);
	g_free(mameinfofile_option);
	g_free(diffdir_option);
	g_free(ctrlr_directory_option);
	g_free(cfg_directory_option);
	g_free(inipath_option);
	g_free(nvram_directory_option);
	g_free(memcard_directory_option);
	g_free(input_directory_option);
	g_free(state_directory_option);
	
	g_free (artwork_dir);
	g_free (snapshot_dir);
	g_free (hiscore_dir);
	g_free (ctrlr_dir);
	g_free (cheat_file);
	g_free (hiscore_file);
	g_free (history_file);
	g_free (mameinfo_file);
	g_free (cfg_dir);
	g_free (nvram_dir);
	g_free (state_dir);
	g_free (inp_dir);
	g_free (memcard_dir);
	g_free (diff_dir);
	g_free (ini_dir);
	
	return option_string;
}

gchar *create_rompath_options_string(MameExec *exec)
{
	GValueArray *va_rom_paths;
	GValueArray *va_sample_paths;
	gchar *rompath_option_string;
	gchar *rompath;
	gchar *samplepath_option_string;
	gchar *samplepath;
	gchar *option_string;
	guint i;
	
	g_object_get (main_gui.gui_prefs,
		      "sample-paths", &va_sample_paths,
		      "rom-paths", &va_rom_paths,
		      NULL);
	
	mame_get_options(exec);

	/* G_SEARCHPATH_SEPARATOR_S returns ":" on Unix, but newer versions,
	   particularly for sdlmame, require ";" */
	
	rompath = g_new0 (gchar*, 0);
	if (va_rom_paths) {
		for (i = 0; i < va_rom_paths->n_values; i++) {
			if (!rompath)
				rompath = g_strdup (g_value_get_string (g_value_array_get_nth (va_rom_paths, i)));
			else
				rompath = g_strjoin ((g_strcasecmp (mame_exec_get_name (exec), "xmame") == 0) ? G_SEARCHPATH_SEPARATOR_S : ";",
							rompath,
							g_value_get_string (g_value_array_get_nth (va_rom_paths, i)),
							NULL);
			GMAMEUI_DEBUG ("In loop rompath is %s", rompath);
		}
	}

	samplepath = g_new0 (gchar*, 0);
	if (va_sample_paths) {
		for (i = 0; i < va_sample_paths->n_values; i++) {
			if (!samplepath)
				samplepath = g_strdup (g_value_get_string (g_value_array_get_nth (va_sample_paths, i)));
			else
				samplepath = g_strjoin ((g_strcasecmp (mame_exec_get_name (exec), "xmame") == 0) ? G_SEARCHPATH_SEPARATOR_S : ";",
							samplepath,
							g_value_get_string (g_value_array_get_nth (va_sample_paths, i)),
							NULL);
			GMAMEUI_DEBUG ("In loop samplepath is %s", samplepath);
		}
	}

	GMAMEUI_DEBUG ("Rompath is %s", rompath);
	GMAMEUI_DEBUG ("Samplepath is %s", samplepath);

	rompath_option_string = mame_get_option_string(exec, "rompath", rompath);
	samplepath_option_string = mame_get_option_string(exec, "samplepath", samplepath);

	if (rompath)
		g_free(rompath);
	if (samplepath)
		g_free(samplepath);

	option_string = g_strjoin(" ",
		rompath_option_string?rompath_option_string:"",
		samplepath_option_string?samplepath_option_string:"",
		NULL);

	g_free(rompath_option_string);
	g_free(samplepath_option_string);
	
	if (va_sample_paths)
		g_value_array_free (va_sample_paths);
	if (va_rom_paths)
		g_value_array_free (va_rom_paths);

	if (!option_string)
		option_string = g_strdup("");

	return option_string;
}

