/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * GMAMEUI
 *
 * Copyright 2007-2008 Andrew Burton <adb@iinet.net.au>
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


/* Set in Glade file, add new property, add get handler, add set handler, add support in gui_prefs_dialog,
   add load in init, add cleanup in finalise */


#include <string.h>
#include <sys/stat.h>   /* For S_IRWXU */

#include <gtk/gtkcheckbutton.h>
#include <gtk/gtkentry.h>
#include <gtk/gtklabel.h>
#include <glib/gstdio.h>	/* For g_mkdir */
#include <glade/glade.h>

#include "common.h"
#include "gmameui.h"
#include "gui_prefs.h"
#include "rom_entry.h"
#include "io.h"

/* Preferences */
static void mame_gui_prefs_class_init (MameGuiPrefsClass *klass);
static void mame_gui_prefs_init (MameGuiPrefs *pr);
static void mame_gui_prefs_finalize (GObject *obj);
static void mame_gui_prefs_set_property (GObject *object,
					 guint prop_id,
					 const GValue *value,
					 GParamSpec *pspec);
static void mame_gui_prefs_get_property (GObject *object,
					 guint prop_id,
					 GValue *value,
					 GParamSpec *pspec);
static void mame_gui_prefs_save_bool (MameGuiPrefs *pr, GParamSpec *param, gpointer user_data);
static void mame_gui_prefs_save_int (MameGuiPrefs *pr, GParamSpec *param, gpointer user_data);
static void mame_gui_prefs_save_int_arr (MameGuiPrefs *pr, GParamSpec *param, gpointer user_data);
static void mame_gui_prefs_save_string (MameGuiPrefs *pr, GParamSpec *param, gpointer user_data);
static void mame_gui_prefs_save_string_arr (MameGuiPrefs *pr, GParamSpec *param, gpointer user_data);
static gboolean mame_gui_prefs_get_bool_property_from_key_file (MameGuiPrefs *pr, gchar *property);
static gint mame_gui_prefs_get_int_property_from_key_file (MameGuiPrefs *pr, gchar *property);
static gchar* mame_gui_prefs_get_string_property_from_key_file (MameGuiPrefs *pr, gchar *property);

G_DEFINE_TYPE (MameGuiPrefs, mame_gui_prefs, G_TYPE_OBJECT)

struct _MameGuiPrefsPrivate {

	/* References to the ini file that contains our prefs */
	GKeyFile *prefs_ini_file;
	gchar *filename;
	
	/* UI preferences - these aren't set in the prefs dialog, but in callbacks */
	gint ui_width;
	gint ui_height;
	
	gboolean show_toolbar;
	gboolean show_statusbar;
	gboolean show_filterlist;
	gboolean show_screenshot;
	
	gint xpos_filters;		/* The position of the paned for the filters*/
	gint xpos_gamelist;		/* The position of the paned for the gamelist */
	
	screenshot_type ShowFlyer;
	
	ListMode current_mode;
	ListMode previous_mode;
	
	GValueArray *cols_shown;	/* Array of integer, 0 hidden, 1 shown */
	GValueArray *cols_width;	/* Array of integer */
	
	gint sort_col;
	gint sort_col_direction;
		
	/* Startup preferences */
	//gint GameCheck;
	gboolean GameCheck;		/* Check for new games on startup FIXME NOT USED? */
	gboolean VersionCheck;		/* Check for new version of MAME on startup */
	gboolean use_xmame_options;     /* Use MAME options, or options set within GMAMEUI */
	gboolean gui_joy;
	gchar *joystick_name;
	
	/* Column layout preferences */
	
	/* Miscellaneous option preferences */
	gboolean theprefix;
	gchar *clone_color;
	//RomEntry *current_rom;
	gchar *current_rom_name;
	gchar *current_executable_name;
	
	/* Directory preferences */
	GValueArray *executable_paths;
	GValueArray *rom_paths;
	GValueArray *sample_paths;      /* FIXME TODO Do we REALLY need to support multiple dirs here? */
	gchar *directories[NUM_DIRS];
	
};



static void
mame_gui_prefs_set_property (GObject *object,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *pspec)
{
	MameGuiPrefs *prefs;
	GValueArray *va = NULL;	/* Don't free this - the calling function must take the responsibility to do so */
	
	prefs = MAME_GUI_PREFS (object);

	if ((prop_id < NUM_PROPERTIES) && (prop_id >= PROP_DIR_ARTWORK)) {
		GMAMEUI_DEBUG ("Setting the directory value at %d with value %s",
			       (prop_id - PROP_DIR_ARTWORK), g_value_get_string (value));
		/* We are processing a directory property */
		prefs->priv->directories[prop_id - PROP_DIR_ARTWORK] = g_strdup (g_value_get_string (value));
		/* Note we use prop_id - PROP_DIR_ARTWORK since PDA is the first of the directory
		   properties */
		return;
	}

	switch (prop_id) {
		case PROP_UI_WIDTH:
			prefs->priv->ui_width = g_value_get_int (value);
			break;
		case PROP_UI_HEIGHT:
			prefs->priv->ui_height = g_value_get_int (value);
			break;
		case PROP_SHOW_TOOLBAR:
			prefs->priv->show_toolbar = g_value_get_boolean (value);
			break;
		case PROP_SHOW_STATUSBAR:
			prefs->priv->show_statusbar = g_value_get_boolean (value);
			break;
		case PROP_SHOW_FILTERLIST:
			prefs->priv->show_filterlist = g_value_get_boolean (value);
			break;
		case PROP_SHOW_SCREENSHOT:
			prefs->priv->show_screenshot = g_value_get_boolean (value);
			break;
		case PROP_SHOW_FLYER:
			prefs->priv->ShowFlyer = g_value_get_int (value);
			break;
		case PROP_XPOS_FILTERS:
			prefs->priv->xpos_filters = g_value_get_int (value);
			break;
		case PROP_XPOS_GAMELIST:
			prefs->priv->xpos_gamelist = g_value_get_int (value);
			break;
		case PROP_GAMECHECK:
			prefs->priv->GameCheck = g_value_get_boolean (value);
			break;
		case PROP_VERSIONCHECK:
			prefs->priv->VersionCheck = g_value_get_boolean (value);
			break;
		case PROP_USEXMAMEOPTIONS:
			prefs->priv->use_xmame_options = g_value_get_boolean (value);
			break;
		case PROP_USEJOYINGUI:
			prefs->priv->gui_joy = g_value_get_boolean (value);
			break;
		case PROP_JOYSTICKNAME:
			prefs->priv->joystick_name = g_strdup (g_value_get_string (value));
			break;
		case PROP_THEPREFIX:
			prefs->priv->theprefix = g_value_get_boolean (value);
			break;
		case PROP_CLONECOLOR:
			prefs->priv->clone_color = g_strdup (g_value_get_string (value));
			break;
		case PROP_CURRENT_ROM:
			//prefs->priv->current_rom = g_value_get_object (value);
			prefs->priv->current_rom_name = g_strdup (g_value_get_string (value));
			break;
		case PROP_CURRENT_EXECUTABLE:
			prefs->priv->current_executable_name = g_strdup (g_value_get_string (value));
			break;	
		case PROP_EXECUTABLE_PATHS:
			va = g_value_get_boxed (value);
			if (prefs->priv->rom_paths)
				g_value_array_free (prefs->priv->executable_paths);
			prefs->priv->executable_paths = va != NULL ? g_value_array_copy (va) : NULL;
			break;
		case PROP_ROM_PATHS:
			va = g_value_get_boxed (value);
			if (prefs->priv->rom_paths)
				g_value_array_free (prefs->priv->rom_paths);
			prefs->priv->rom_paths = va != NULL ? g_value_array_copy (va) : NULL;
			break;
		case PROP_SAMPLE_PATHS:
			va = g_value_get_boxed (value);
			if (prefs->priv->sample_paths)
				g_value_array_free (prefs->priv->sample_paths);
			prefs->priv->sample_paths = va != NULL ? g_value_array_copy (va) : NULL;
			break;
		case PROP_CURRENT_MODE:
			prefs->priv->current_mode = g_value_get_int (value);
			break;
		case PROP_PREVIOUS_MODE:
			prefs->priv->previous_mode = g_value_get_int (value);
			break;
		case PROP_COLS_SHOWN:
			va = g_value_get_boxed (value);
			if (prefs->priv->cols_shown)
				g_value_array_free (prefs->priv->cols_shown);
			prefs->priv->cols_shown = va != NULL ? g_value_array_copy (va) : NULL;
			break;
		case PROP_COLS_WIDTH:
			va = g_value_get_boxed (value);
			if (prefs->priv->cols_width)
				g_value_array_free (prefs->priv->cols_width);
			prefs->priv->cols_width = va != NULL ? g_value_array_copy (va) : NULL;
			break;
		case PROP_SORT_COL:
			prefs->priv->sort_col = g_value_get_int (value);
			break;
		case PROP_SORT_COL_DIR:
			prefs->priv->sort_col_direction = g_value_get_int (value);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
	
	/* As long as the notify::<property> callback is set, the handler
	   will then manage saving the key file */
}

static void
mame_gui_prefs_get_property (GObject *object,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *pspec)
{
	MameGuiPrefs *prefs = MAME_GUI_PREFS (object);

	if ((prop_id < NUM_PROPERTIES) && (prop_id >= PROP_DIR_CFG)) {
		/* This clause deals with the non-configurable user directories */
		gchar *dir;
		dir = g_build_filename (g_get_home_dir (), ".gmameui",
					prefs->priv->directories[prop_id - PROP_DIR_ARTWORK],
					NULL);
					
		GMAMEUI_DEBUG ("Getting the directory value at %d with value %s",
			       (prop_id - PROP_DIR_ARTWORK), dir);
			       
		g_value_set_string (value, dir);
		g_free (dir);
		return;
	}
	
	if ((prop_id < NUM_PROPERTIES) && (prop_id >= PROP_DIR_ARTWORK)) {
		/* This clause deals with the configurable directories */
		GMAMEUI_DEBUG ("Getting the directory value at %d with value %s",
			       (prop_id - PROP_DIR_ARTWORK),
			       prefs->priv->directories[prop_id - PROP_DIR_ARTWORK]);
		/* We are processing a directory property */
		g_value_set_string (value, prefs->priv->directories[prop_id - PROP_DIR_ARTWORK]);
		/* Note we use prop_id - PROP_DIR_ARTWORK since PDA is the first of the directory
		   properties */
		return;
	}

	switch (prop_id) {
		case PROP_UI_WIDTH:
			g_value_set_int (value, prefs->priv->ui_width);
			break;
		case PROP_UI_HEIGHT:
			g_value_set_int (value, prefs->priv->ui_height);
			break;
		case PROP_SHOW_TOOLBAR:
			g_value_set_boolean (value, prefs->priv->show_toolbar);
			break;
		case PROP_SHOW_STATUSBAR:
			g_value_set_boolean (value, prefs->priv->show_statusbar);
			break;
		case PROP_SHOW_FILTERLIST:
			g_value_set_boolean (value, prefs->priv->show_filterlist);
			break;
		case PROP_SHOW_SCREENSHOT:
			g_value_set_boolean (value, prefs->priv->show_screenshot);
			break;
		case PROP_SHOW_FLYER:
			g_value_set_int (value, prefs->priv->ShowFlyer);
			break;
		case PROP_XPOS_FILTERS:
			g_value_set_int (value, prefs->priv->xpos_filters);
			break;
		case PROP_XPOS_GAMELIST:
			g_value_set_int (value, prefs->priv->xpos_gamelist);
			break;
		case PROP_GAMECHECK:
			g_value_set_boolean (value, prefs->priv->GameCheck);
			break;
		case PROP_VERSIONCHECK:
			g_value_set_boolean (value, prefs->priv->VersionCheck);
			break;
		case PROP_USEXMAMEOPTIONS:
			g_value_set_boolean (value, prefs->priv->use_xmame_options);
			break;
		case PROP_USEJOYINGUI:
			g_value_set_boolean (value, prefs->priv->gui_joy);
			break;
		case PROP_JOYSTICKNAME:
			g_value_set_string (value, prefs->priv->joystick_name);
			break;
		case PROP_THEPREFIX:
			g_value_set_boolean (value, prefs->priv->theprefix);
			break;
		case PROP_CLONECOLOR:
			g_value_set_string (value, prefs->priv->clone_color);
			break;
		case PROP_CURRENT_ROM:
			//g_value_set_object (value, prefs->priv->current_rom);
			g_value_set_string (value, prefs->priv->current_rom_name);
			break;
		case PROP_CURRENT_EXECUTABLE:
			g_value_set_string (value, prefs->priv->current_executable_name);
			break;
		case PROP_EXECUTABLE_PATHS:
			g_value_set_boxed (value, prefs->priv->executable_paths);
			break;	
		case PROP_ROM_PATHS:
			g_value_set_boxed (value, prefs->priv->rom_paths);
			break;
		case PROP_SAMPLE_PATHS:
			g_value_set_boxed (value, prefs->priv->sample_paths);
			break;
		case PROP_CURRENT_MODE:
			g_value_set_int (value, prefs->priv->current_mode);
			break;
		case PROP_PREVIOUS_MODE:
			g_value_set_int (value, prefs->priv->previous_mode);
			break;
		case PROP_COLS_SHOWN:
			g_value_set_boxed (value, prefs->priv->cols_shown);
			break;
		case PROP_COLS_WIDTH:
			g_value_set_boxed (value, prefs->priv->cols_width);
			break;
		case PROP_SORT_COL:
			g_value_set_int (value, prefs->priv->sort_col);
			break;
		case PROP_SORT_COL_DIR:
			g_value_set_int (value, prefs->priv->sort_col_direction);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

static void
mame_gui_prefs_finalize (GObject *obj)
{
	GMAMEUI_DEBUG ("Finalising mame_gui_prefs object");
	
	MameGuiPrefs *pr = MAME_GUI_PREFS (obj);
	
	/* Free the key file for the preferences ini */
	if (pr->priv->prefs_ini_file)
		g_key_file_free (pr->priv->prefs_ini_file);
	g_free (pr->priv->filename);
	
	/* Free the properties that are strings or GValueArrays */
	if (pr->priv->joystick_name)
		g_free (pr->priv->joystick_name);
	
	if (pr->priv->clone_color)
		g_free (pr->priv->clone_color);

	if (pr->priv->cols_shown)
		g_value_array_free (pr->priv->cols_shown);
	if (pr->priv->cols_width)
		g_value_array_free (pr->priv->cols_width);
	
/*	if (pr->priv->current_rom_name)
		g_free (pr->priv->current_rom_name);
	if (pr->priv->current_executable_name)
		g_free (pr->priv->current_executable_name);*/
	
	if (pr->priv->executable_paths)
		g_value_array_free (pr->priv->executable_paths);
	if (pr->priv->rom_paths)
		g_value_array_free (pr->priv->rom_paths);
	if (pr->priv->sample_paths)
		g_value_array_free (pr->priv->sample_paths);
	
	int i;

	for (i = 0; i < NUM_DIRS; i++) {
		if (pr->priv->directories[i])
			g_free (pr->priv->directories[i]);
	}
	
// FIXME TODO	g_free (pr->priv);
	
	GMAMEUI_DEBUG ("Finalising mame_gui_prefs object... done");
	
	/* FIXME TODO Unref all the strings and destroy the object */
}

static void
mame_gui_prefs_class_init (MameGuiPrefsClass *klass)
{
	
	
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	
	object_class->set_property = mame_gui_prefs_set_property;
	object_class->get_property = mame_gui_prefs_get_property;
	object_class->finalize = mame_gui_prefs_finalize;

	/* UI preferences */
	g_object_class_install_property (object_class,
					 PROP_UI_WIDTH,
					 g_param_spec_int ("ui-width", "Window Width", "Width of the main window", 0, 2000, 800, G_PARAM_READWRITE));
	g_object_class_install_property (object_class,
					 PROP_UI_HEIGHT,
					 g_param_spec_int ("ui-height", "Window Height", "Height of the main window", 0, 2000, 600, G_PARAM_READWRITE));
	g_object_class_install_property (object_class,
					 PROP_SHOW_TOOLBAR,
					 g_param_spec_boolean ("show-toolbar", "Show Toolbar", "Show the main toolbar", TRUE, G_PARAM_READWRITE));
	g_object_class_install_property (object_class,
					 PROP_SHOW_STATUSBAR,
					 g_param_spec_boolean ("show-statusbar", "Show Statusbar", "Show the statusbar", TRUE, G_PARAM_READWRITE));
	g_object_class_install_property (object_class,
					 PROP_SHOW_FILTERLIST,
					 g_param_spec_boolean ("show-filterlist", "Show Filter List", "Show the filters", TRUE, G_PARAM_READWRITE));
	g_object_class_install_property (object_class,
					 PROP_SHOW_SCREENSHOT,
					 g_param_spec_boolean ("show-screenshot", "Show Sidebar", "Show the screenshot sidebar", TRUE, G_PARAM_READWRITE));
	g_object_class_install_property (object_class,
					 PROP_SHOW_FLYER,
					 g_param_spec_int ("show-flyer", "Image Type", "Image type to display", 0, 5, 0, G_PARAM_READWRITE));
	g_object_class_install_property (object_class,
					 PROP_CURRENT_MODE,
					 g_param_spec_int ("current-mode", "Current Mode", "Current Mode", LIST, DETAILS_TREE, DETAILS, G_PARAM_READWRITE));
	g_object_class_install_property (object_class,
					 PROP_PREVIOUS_MODE,
					 g_param_spec_int ("previous-mode", "Previous Mode", "Previous Mode", LIST, DETAILS_TREE, LIST, G_PARAM_READWRITE));
	g_object_class_install_property (object_class,
					 PROP_COLS_SHOWN,
					 g_param_spec_value_array ("cols-shown", "Columns Shown", "Which Columns Are Shown or Hidden", NULL, G_PARAM_READWRITE));
	g_object_class_install_property (object_class,
					 PROP_COLS_WIDTH,
					 g_param_spec_value_array ("cols-width", "Column Width", "Width of Each Column", NULL, G_PARAM_READWRITE));
	g_object_class_install_property (object_class,
					 PROP_SORT_COL,
					 g_param_spec_int ("sort-col", "Sort Column", "Sort column", 0, NUMBER_COLUMN, GAMENAME, G_PARAM_READWRITE));
	g_object_class_install_property (object_class,
					 PROP_SORT_COL_DIR,
					 g_param_spec_int ("sort-col-direction", "Sort Column Direction", "Direction in which column data is sorted", GTK_SORT_ASCENDING, GTK_SORT_DESCENDING, GTK_SORT_ASCENDING, G_PARAM_READWRITE));
	g_object_class_install_property (object_class,
					 PROP_XPOS_FILTERS,
					 g_param_spec_int ("xpos-filters", "Filters Xpos", "X position of the filters hpaned", 0, 1000, 150, G_PARAM_READWRITE));
	g_object_class_install_property (object_class,
					 PROP_XPOS_GAMELIST,
					 g_param_spec_int ("xpos-gamelist", "Gamelist Xpos", "X position of the gamelist hpaned", 0, 1000, 500, G_PARAM_READWRITE));
	
	/* Startup preferences */
	g_object_class_install_property (object_class,
					 PROP_GAMECHECK,
					 g_param_spec_boolean ("gamecheck", "Game Check", "Check for games on startup", TRUE, G_PARAM_READWRITE));
	g_object_class_install_property (object_class,
					 PROP_VERSIONCHECK,
					 g_param_spec_boolean ("versioncheck", "Version Check", "Check for new version on startup", TRUE, G_PARAM_READWRITE));
	g_object_class_install_property (object_class,
					 PROP_USEXMAMEOPTIONS,
					 g_param_spec_boolean ("usexmameoptions", "Use MAME Options", "Use MAME options, rather than those set in GMAMEUI", FALSE, G_PARAM_READWRITE));
	g_object_class_install_property (object_class,
					 PROP_USEJOYINGUI,
					 g_param_spec_boolean ("usejoyingui", "Use Joystick in GUI", "Use the joystick to navigate in GMAMEUI", 0, G_PARAM_READWRITE));
	g_object_class_install_property (object_class,
					 PROP_JOYSTICKNAME,
					 g_param_spec_string ("joystick-name", "Joystick Name", "Device name of the joystick", "/dev/js0", G_PARAM_READWRITE));
	
	/* Miscellaneous preferences */
	g_object_class_install_property (object_class,
					 PROP_THEPREFIX,
					 g_param_spec_boolean ("theprefix", "Display 'The'", "Display 'The' as a prefix in the gamelist", TRUE, G_PARAM_READWRITE));
	g_object_class_install_property (object_class,
					 PROP_CLONECOLOR,
					 g_param_spec_string ("clone-color", "Clone Color", "Clone Color", "##5F5F5F", G_PARAM_READWRITE));      /* Default to grey */
	g_object_class_install_property (object_class,
					 PROP_CURRENT_ROM,
					 g_param_spec_string ("current-rom", "Current Rom", "The currently selected ROM", NULL, G_PARAM_READWRITE));
	g_object_class_install_property (object_class,
					 PROP_CURRENT_EXECUTABLE,
					 g_param_spec_string ("current-executable", "Current executable", "The currently selected MAME executable", NULL, G_PARAM_READWRITE));
	g_object_class_install_property (object_class,
					 PROP_EXECUTABLE_PATHS,
					 g_param_spec_value_array ("executable-paths", "Executable Paths", "Paths to the MAME executables", NULL, G_PARAM_READWRITE));
	g_object_class_install_property (object_class,
					 PROP_ROM_PATHS,
					 g_param_spec_value_array ("rom-paths", "ROM Paths", "Directories containing MAME ROMs", NULL, G_PARAM_READWRITE));
	g_object_class_install_property (object_class,
					 PROP_SAMPLE_PATHS,
					 g_param_spec_value_array ("sample-paths", "Sample Paths", "Directories containing MAME samples", NULL, G_PARAM_READWRITE));
	
	/* Directory preferences */
	int i;  /* Used for walking the directory_prefs struct array */
	for (i = 0; i < NUM_DIRS; i++) {
		/*GMAMEUI_DEBUG ("Installing new property %s with id %d", directory_prefs[i].name, directory_prefs[i].prop_id);*/
		g_object_class_install_property (object_class,
						 directory_prefs[i].prop_id,
						 g_param_spec_string (directory_prefs[i].name, "", "", "", G_PARAM_READWRITE));
	}
}

static void
mame_gui_prefs_init (MameGuiPrefs *pr)
{
	gboolean load_result;
	gint *int_array;
	gchar **str_array;
	gsize columnsize;
	gsize paths;	/* FIXME Define max number of rom/sample dirs */
	guint i;
	
	GMAMEUI_DEBUG ("Creating GUI prefs object");	
	pr->priv = g_new0 (MameGuiPrefsPrivate, 1);
	
	int_array = g_new0 (gint, NUMBER_COLUMN);       /* Used for loading integer lists from prefs file */
	
	pr->priv->cols_shown = g_value_array_new (NUMBER_COLUMN);
	pr->priv->cols_width = g_value_array_new (NUMBER_COLUMN);
	
	pr->priv->executable_paths = g_value_array_new (4);     /* FIXME TODO MAX_EXECUTABLES */
	pr->priv->rom_paths = g_value_array_new (4);    /* FIXME TODO - define max number */
	pr->priv->sample_paths = g_value_array_new (4); /* FIXME TODO - define max number */
	
	pr->priv->filename = g_build_filename (g_get_home_dir (), ".gmameui", "gmameui.ini", NULL);

	pr->priv->prefs_ini_file = g_key_file_new ();
	GError *error = NULL;
	load_result = g_key_file_load_from_file (pr->priv->prefs_ini_file, pr->priv->filename,
						 G_KEY_FILE_KEEP_COMMENTS, &error);

	if (!load_result) {
		GMAMEUI_DEBUG ("Error loading %s - %s", pr->priv->filename, error->message);	
	
		g_error_free (error);
		error = NULL;
	}

	/* UI preferences */
	pr->priv->ui_width = mame_gui_prefs_get_int_property_from_key_file (pr, "ui-width");
	pr->priv->ui_height = mame_gui_prefs_get_int_property_from_key_file (pr, "ui-height");
	pr->priv->show_toolbar = mame_gui_prefs_get_bool_property_from_key_file (pr, "show-toolbar");
	pr->priv->show_statusbar = mame_gui_prefs_get_bool_property_from_key_file (pr, "show-statusbar");
	pr->priv->show_filterlist = mame_gui_prefs_get_bool_property_from_key_file (pr, "show-filterlist");
	pr->priv->show_screenshot = mame_gui_prefs_get_bool_property_from_key_file (pr, "show-screenshot");
	pr->priv->ShowFlyer = mame_gui_prefs_get_int_property_from_key_file (pr, "show-flyer");
	pr->priv->current_mode = mame_gui_prefs_get_int_property_from_key_file (pr, "current-mode");
	pr->priv->previous_mode = mame_gui_prefs_get_int_property_from_key_file (pr, "previous-mode");
	int_array = g_key_file_get_integer_list (pr->priv->prefs_ini_file, "Preferences", "cols-shown", &columnsize, &error);
	for (i = 0; i < NUMBER_COLUMN; ++i) {
		GValue val = { 0, };
		
		g_value_init (&val, G_TYPE_INT);
		g_value_set_int (&val, int_array != NULL ? int_array[i] : 1);	/* If not available, default to shown */

		GMAMEUI_DEBUG ("Value for cols-shown at %d is %d", i, g_value_get_int (&val));
		g_value_array_append (pr->priv->cols_shown, &val);
	}

	int_array = g_key_file_get_integer_list (pr->priv->prefs_ini_file, "Preferences", "cols-width", &columnsize, &error);
	for (i = 0; i < NUMBER_COLUMN; ++i) {
		GValue val = { 0, };
		/* FIXME TODO 0 means auto sized columns */
		g_value_init (&val, G_TYPE_INT);
		g_value_set_int (&val, int_array != NULL ? int_array[i] : 0);	/* If not available, default to 0 */
		GMAMEUI_DEBUG ("Value for cols-width at %d is %d", i, g_value_get_int (&val));
		g_value_array_append (pr->priv->cols_width, &val);
	}
	pr->priv->sort_col = mame_gui_prefs_get_int_property_from_key_file (pr, "sort-col");
	pr->priv->sort_col_direction = mame_gui_prefs_get_int_property_from_key_file (pr, "sort-col-direction");
	pr->priv->xpos_filters = mame_gui_prefs_get_int_property_from_key_file (pr, "xpos-filters");
	pr->priv->xpos_gamelist = mame_gui_prefs_get_int_property_from_key_file (pr, "xpos-gamelist");
	
	/* Startup preferences */
	pr->priv->GameCheck = mame_gui_prefs_get_bool_property_from_key_file (pr, "gamecheck");
	pr->priv->VersionCheck = mame_gui_prefs_get_bool_property_from_key_file (pr, "versioncheck");
	pr->priv->use_xmame_options = mame_gui_prefs_get_bool_property_from_key_file (pr, "usexmameoptions");
	pr->priv->gui_joy = mame_gui_prefs_get_bool_property_from_key_file (pr, "usejoyingui");
	pr->priv->joystick_name = mame_gui_prefs_get_string_property_from_key_file (pr, "joystick-name");
	if (!pr->priv->joystick_name)
		pr->priv->joystick_name = g_strdup (get_joy_dev ());
	
	/* Miscellaneous preferences */
	pr->priv->theprefix = mame_gui_prefs_get_bool_property_from_key_file (pr, "theprefix");
	pr->priv->clone_color = mame_gui_prefs_get_string_property_from_key_file (pr, "clone-color");
	if (!pr->priv->clone_color)
		pr->priv->clone_color = g_strdup ("grey");
	pr->priv->current_rom_name = mame_gui_prefs_get_string_property_from_key_file (pr, "current-rom");
	pr->priv->current_executable_name = mame_gui_prefs_get_string_property_from_key_file (pr, "current-executable");

	/* Load the executable paths */
	str_array = g_key_file_get_string_list (pr->priv->prefs_ini_file, "Preferences", "executable-paths", &paths, &error);
	for (i = 0; i < paths; i++) {
		GValue val = { 0, };
		
		g_value_init (&val, G_TYPE_STRING);
		if ((str_array) && (str_array[i] != NULL))
			g_value_set_string (&val, str_array[i]);	/* If not available, default to NULL */
		GMAMEUI_DEBUG ("Value for executable-paths at %d is %s", i, g_value_get_string (&val));
		g_value_array_append (pr->priv->executable_paths, &val);
	}
	
	/* Load the ROM paths */
	str_array = g_key_file_get_string_list (pr->priv->prefs_ini_file, "Preferences", "rom-paths", &paths, &error);
	for (i = 0; i < paths; i++) {
		GValue val = { 0, };
		
		g_value_init (&val, G_TYPE_STRING);
		if ((str_array) && (str_array[i] != NULL))
			g_value_set_string (&val, str_array[i]);	/* If not available, default to NULL */
		GMAMEUI_DEBUG ("Value for rom-paths at %d is %s", i, g_value_get_string (&val));
		g_value_array_append (pr->priv->rom_paths, &val);
	}
	
	/* Load the Samples paths */
	str_array = g_key_file_get_string_list (pr->priv->prefs_ini_file, "Preferences", "sample-paths", &paths, &error);
	for (i = 0; i < paths; i++) {
		GValue val = { 0, };
		
		g_value_init (&val, G_TYPE_STRING);
		if ((str_array) && (str_array[i] != NULL))
			g_value_set_string (&val, str_array[i]);	/* If not available, default to NULL */
		GMAMEUI_DEBUG ("Value for sample-paths at %d is %s", i, g_value_get_string (&val));
		g_value_array_append (pr->priv->sample_paths, &val);
	}
	
	/* Directory preferences */
	GMAMEUI_DEBUG ("Reading directories preferences from file");
	for (i = 0; i < NUM_DIRS; i++) {
		pr->priv->directories[i] = mame_gui_prefs_get_string_property_from_key_file (pr, directory_prefs[i].name);
		
		/* If no values set, set default values */
		if (!pr->priv->directories[i])
			pr->priv->directories[i] = g_strdup (directory_prefs[i].default_dir);   /* strdup, since we free in finalize */
		GMAMEUI_DEBUG ("Setting directory %d (%s) to %s", i, directory_prefs[i].name, pr->priv->directories[i]);
		
		/* If directory does not exist (and it exists under $HOME/.gmameui), create it */
		if (i >= PROP_DIR_CFG) {
			gchar *dir;
			dir = g_build_filename (g_get_home_dir (), ".gmameui", directory_prefs[i].default_dir, NULL);
			if (!g_file_test (dir, G_FILE_TEST_EXISTS | G_FILE_TEST_IS_DIR)) {
				GMAMEUI_DEBUG ("Directory %s does not exist, creating it", dir);
				g_mkdir (dir, S_IRWXU);
			}
			g_free (dir);
		}
		
	}
	GMAMEUI_DEBUG ("Reading directories preferences from file... done");
	
	/* Set handlers so that whenever the values are changed (from anywhere), the signal handler
	   is invoked; the callback then saves to the g_key_file */
	g_signal_connect (pr, "notify::ui-width", (GCallback) mame_gui_prefs_save_int, NULL);
	g_signal_connect (pr, "notify::ui-height", (GCallback) mame_gui_prefs_save_int, NULL);
	g_signal_connect (pr, "notify::show-toolbar", (GCallback) mame_gui_prefs_save_bool, NULL);
	g_signal_connect (pr, "notify::show-statusbar", (GCallback) mame_gui_prefs_save_bool, NULL);
	g_signal_connect (pr, "notify::show-filterlist", (GCallback) mame_gui_prefs_save_bool, NULL);
	g_signal_connect (pr, "notify::show-screenshot", (GCallback) mame_gui_prefs_save_bool, NULL);
	g_signal_connect (pr, "notify::show-flyer", (GCallback) mame_gui_prefs_save_int, NULL);
	g_signal_connect (pr, "notify::current-mode", (GCallback) mame_gui_prefs_save_int, NULL);
	g_signal_connect (pr, "notify::previous-mode", (GCallback) mame_gui_prefs_save_int, NULL);
	g_signal_connect (pr, "notify::cols-shown", (GCallback) mame_gui_prefs_save_int_arr, NULL);
	g_signal_connect (pr, "notify::cols-width", (GCallback) mame_gui_prefs_save_int_arr, NULL);
	g_signal_connect (pr, "notify::sort-col", (GCallback) mame_gui_prefs_save_int, NULL);
	g_signal_connect (pr, "notify::sort-col-direction", (GCallback) mame_gui_prefs_save_int, NULL);
	g_signal_connect (pr, "notify::xpos-filters", (GCallback) mame_gui_prefs_save_int, NULL);
	g_signal_connect (pr, "notify::xpos-gamelist", (GCallback) mame_gui_prefs_save_int, NULL);
	g_signal_connect (pr, "notify::gamecheck", (GCallback) mame_gui_prefs_save_bool, NULL);
	g_signal_connect (pr, "notify::versioncheck", (GCallback) mame_gui_prefs_save_bool, NULL);
	g_signal_connect (pr, "notify::usexmameoptions", (GCallback) mame_gui_prefs_save_bool, NULL);
	g_signal_connect (pr, "notify::usejoyingui", (GCallback) mame_gui_prefs_save_bool, NULL);
	g_signal_connect (pr, "notify::joystick-name", (GCallback) mame_gui_prefs_save_string, NULL);
	g_signal_connect (pr, "notify::theprefix", (GCallback) mame_gui_prefs_save_bool, NULL);
	g_signal_connect (pr, "notify::clone-color", (GCallback) mame_gui_prefs_save_string, NULL);
	g_signal_connect (pr, "notify::current-rom", (GCallback) mame_gui_prefs_save_string, NULL);
	g_signal_connect (pr, "notify::current-executable", (GCallback) mame_gui_prefs_save_string, NULL);
	g_signal_connect (pr, "notify::executable-paths", (GCallback) mame_gui_prefs_save_string_arr, NULL);
	g_signal_connect (pr, "notify::rom-paths", (GCallback) mame_gui_prefs_save_string_arr, NULL);
	g_signal_connect (pr, "notify::sample-paths", (GCallback) mame_gui_prefs_save_string_arr, NULL); 
	
	for (i = 0; i < NUM_DIRS; i++) {
		gchar *signal_name;
		signal_name = g_strdup_printf("notify::%s", directory_prefs[i].name);
		GMAMEUI_DEBUG("Connecting signal %s", signal_name);
		g_signal_connect (pr, signal_name, (GCallback) mame_gui_prefs_save_string, NULL);
		g_free (signal_name);
	}
GMAMEUI_DEBUG ("Creating GUI prefs object... done");
}

MameGuiPrefs* mame_gui_prefs_new (void)
{
	return g_object_new (MAME_TYPE_GUI_PREFS, NULL);
}

static void mame_gui_prefs_save_int (MameGuiPrefs *pr, GParamSpec *param, gpointer user_data)
{
	gchar *key;
	gint value;
	GError *error = NULL;

	g_return_if_fail (pr->priv->prefs_ini_file != NULL);
	g_return_if_fail (pr->priv->filename != NULL);
	
	key = g_strdup (g_param_spec_get_name (param));
	
	g_return_if_fail (key != NULL);	
	
	g_object_get (pr, key, &value, NULL);
	
	GMAMEUI_DEBUG ("Saving %s - setting integer value %s with value %d",
		       pr->priv->filename, key, value);

	/* Set the value and save the file */
	g_key_file_set_integer (pr->priv->prefs_ini_file, "Preferences", key, value);
	g_key_file_save_to_file (pr->priv->prefs_ini_file, pr->priv->filename, &error);
	
	if (error) {
		GMAMEUI_DEBUG ("Error saving %s - %s", pr->priv->filename, error->message);
		g_error_free (error);
	}
	
	g_free (key);
	
	/* FIXME TODO Preferences is a temporary group name - need to find a way to
	   add more groups */
}

/* This function is used to handle the integer arrays representing the columns -
   both whether the column is shown, and it's width */
static void mame_gui_prefs_save_int_arr (MameGuiPrefs *pr, GParamSpec *param, gpointer user_data)
{
	gchar *key;
	GValueArray *va;
	gint *value;
	int i;
	GError *error = NULL;

	g_return_if_fail (pr->priv->prefs_ini_file != NULL);
	g_return_if_fail (pr->priv->filename != NULL);
	
	key = g_strdup (g_param_spec_get_name (param));
	
	g_return_if_fail (key != NULL);	
	
	g_object_get (pr, key, &va, NULL);
	
	g_return_if_fail (va != NULL);
	
	GMAMEUI_DEBUG ("Saving integer array %s", key);
	
	/* Copy all the elements in the value array to an integer array */
	value = g_new0 (gint, NUMBER_COLUMN);
	for (i = 0; i < NUMBER_COLUMN; i++) {
		//GMAMEUI_DEBUG ("Value at %d is %d", i, g_value_get_int (g_value_array_get_nth (va, i)));
		value[i] = g_value_get_int (g_value_array_get_nth (va, i));
	}
	
	/*GMAMEUI_DEBUG ("Saving %s - setting integer array value %s",
		       pr->priv->filename, key);*/

	/* Set the value and save the file */
	g_key_file_set_integer_list (pr->priv->prefs_ini_file, "Preferences", key, value, NUMBER_COLUMN);
	g_key_file_save_to_file (pr->priv->prefs_ini_file, pr->priv->filename, &error);
	
	if (error) {
		GMAMEUI_DEBUG ("Error saving %s - %s", pr->priv->filename, error->message);
		g_error_free (error);
	}
	
	g_free (key);
	
	/* FIXME TODO Preferences is a temporary group name - need to find a way to
	   add more groups */
}

static void mame_gui_prefs_save_bool (MameGuiPrefs *pr, GParamSpec *param, gpointer user_data)
{
	gchar *key;
	gboolean value;
	GError *error = NULL;

	g_return_if_fail (pr->priv->prefs_ini_file != NULL);
	g_return_if_fail (pr->priv->filename != NULL);
	
	key = g_strdup (g_param_spec_get_name (param));
	
	g_return_if_fail (key != NULL);	
	
	g_object_get (pr, key, &value, NULL);
	
	GMAMEUI_DEBUG ("Saving %s - setting boolean value %s with value %d",
		       pr->priv->filename, key, value);

	/* Set the value and save the file */
	g_key_file_set_boolean (pr->priv->prefs_ini_file, "Preferences", key, value);
	g_key_file_save_to_file (pr->priv->prefs_ini_file, pr->priv->filename, &error);
	
	if (error) {
		GMAMEUI_DEBUG ("Error saving %s - %s", pr->priv->filename, error->message);
		g_error_free (error);
	}
	
	g_free (key);
	
	/* FIXME TODO Preferences is a temporary group name - need to find a way to
	   add more groups */
}

static void mame_gui_prefs_save_string (MameGuiPrefs *pr, GParamSpec *param, gpointer user_data)
{
	gchar *key;
	gchar *value;
	GError *error = NULL;

	g_return_if_fail (pr->priv->prefs_ini_file != NULL);
	g_return_if_fail (pr->priv->filename != NULL);
	
	key = g_strdup (g_param_spec_get_name (param));
	
	g_return_if_fail (key != NULL);	
	
	g_object_get (pr, key, &value, NULL);
	
	GMAMEUI_DEBUG ("Saving %s - setting string value %s with value %s",
		       pr->priv->filename, key, value);

	/* Set the value and save the file */
	g_key_file_set_string (pr->priv->prefs_ini_file, "Preferences", key, value);
	g_key_file_save_to_file (pr->priv->prefs_ini_file, pr->priv->filename, &error);
	
	if (error) {
		GMAMEUI_DEBUG ("Error saving %s - %s", pr->priv->filename, error->message);
		g_error_free (error);
	}
	
	g_free (key);
	
	/* FIXME TODO Preferences is a temporary group name - need to find a way to
	   add more groups */
}



/* This function is used to handle the string arrays representing the ROM
   and sample paths */
static void mame_gui_prefs_save_string_arr (MameGuiPrefs *pr, GParamSpec *param, gpointer user_data)
{
	gchar *key;
	GValueArray *va;
	gchar **value;
	guint n_va;
	guint i;
	GError *error = NULL;

	g_return_if_fail (pr->priv->prefs_ini_file != NULL);
	g_return_if_fail (pr->priv->filename != NULL);
	
	key = g_strdup (g_param_spec_get_name (param));
	
	g_return_if_fail (key != NULL);	
	
	g_object_get (pr, key, &va, NULL);
	
	g_return_if_fail (va != NULL);
	g_return_if_fail (va->n_values > 0);
	GMAMEUI_DEBUG ("Saving string array %s", key);
	
	/* Copy all the elements in the value array to an integer array */
	n_va = va->n_values;
	value = g_new0 (gchar*, n_va);
	for (i = 0; i < n_va; i++) {
		GMAMEUI_DEBUG ("Value at %d is %s", i, g_value_get_string (g_value_array_get_nth (va, i)));
		value[i] = g_strdup (g_value_get_string (g_value_array_get_nth (va, i)));
	}
	
	GMAMEUI_DEBUG ("Saving %s - setting string array value %s",
		       pr->priv->filename, key);

	/* Set the value and save the file */
	g_key_file_set_string_list (pr->priv->prefs_ini_file, "Preferences", key, value, n_va);
	g_key_file_save_to_file (pr->priv->prefs_ini_file, pr->priv->filename, &error);
	
	if (error) {
		GMAMEUI_DEBUG ("Error saving %s - %s", pr->priv->filename, error->message);
		g_error_free (error);
	}
	
	GMAMEUI_DEBUG ("Saving string array %s... done", key);
	
	g_free (key);
	/* FIXME TODO g_strfreev (value);*/
	
	/* FIXME TODO Preferences is a temporary group name - need to find a way to
	   add more groups */
}

/* Retrieve a boolean property from the preferences ini file. If an error occurs (usually because
   the preferences file does not exist, or the key value is not present, then the default value
   in the g_param_spec is used instead */
static gboolean mame_gui_prefs_get_bool_property_from_key_file (MameGuiPrefs *pr, gchar *property) {
	GError *error = NULL;
	gboolean val;
	
	val = g_key_file_get_boolean (pr->priv->prefs_ini_file, "Preferences", property, &error);

	if (error) {
		GMAMEUI_DEBUG ("Error retrieving boolean UI option %s - %s", property, error->message);
		
		g_error_free (error);
		error = NULL;
		
		GParamSpec *spec;
		GValue value = { 0, };

		spec = g_object_class_find_property (G_OBJECT_GET_CLASS (pr), property);
		
		g_value_init (&value, G_TYPE_BOOLEAN);
		g_param_value_set_default (spec, &value);
		
		val = g_value_get_boolean (&value);
		GMAMEUI_DEBUG (_("Retrieving default boolean value for %s: %i"), property, val);
	}
	
	return val;
}

/* Retrieve an integer property from the preferences ini file. If an error occurs (usually because
   the preferences file does not exist, or the key value is not present, then the default value
   in the g_param_spec is used instead */
static gint mame_gui_prefs_get_int_property_from_key_file (MameGuiPrefs *pr, gchar *property) {
	GError *error = NULL;
	gint val;
	
	val = g_key_file_get_integer (pr->priv->prefs_ini_file, "Preferences", property, &error);

	if (error) {
		GMAMEUI_DEBUG ("Error retrieving integer UI option %s - %s", property, error->message);
		g_error_free (error);
		error = NULL;

		GParamSpec *spec;
		GValue value = { 0, };

		spec = g_object_class_find_property (G_OBJECT_GET_CLASS (pr), property);
		
		g_value_init (&value, G_TYPE_INT);
		g_param_value_set_default (spec, &value);
		
		val = g_value_get_int (&value);
		GMAMEUI_DEBUG (_("Retrieving default integer value for %s: %i"), property, val);
	}
	
	return val;
}

/* Retrieve a gchar* property from the preferences ini file. If an error occurs (usually because
   the preferences file does not exist, or the key value is not present, then the default value
   in the g_param_spec is used instead */
static gchar* mame_gui_prefs_get_string_property_from_key_file (MameGuiPrefs *pr, gchar *property) {
	GError *error = NULL;
	gchar* val;
	
	val = g_key_file_get_string (pr->priv->prefs_ini_file, "Preferences", property, &error);

	if (error) {
		GMAMEUI_DEBUG ("Error retrieving string UI option %s - %s", property, error->message);
		g_error_free (error);
		error = NULL;
		
		GParamSpec *spec;
		GValue value = { 0, };

		spec = g_object_class_find_property (G_OBJECT_GET_CLASS (pr), property);
		
		g_value_init (&value, G_TYPE_STRING);
		g_param_value_set_default (spec, &value);
		
		val = g_strdup (g_value_get_string (&value));
		GMAMEUI_DEBUG (_("Retrieving default string value for %s: %s"), property, val);
	}
	
	return val;
}
