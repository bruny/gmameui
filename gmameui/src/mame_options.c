/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * GMAMEUI
 *
 * Copyright 2007-2008 Andrew Burton <adb@iinet.net.au>
 * based on Anjuta code for AnjutaPreferences object
 * Copyright (C) 2000 - 2003  Naba Kumar  <naba@gnome.org>
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

#include <string.h>
#include <stdlib.h>

#include "mame_options.h"
#include "io.h"
#include "gui.h"	/* gmameui_get_icon_from_stock */



static void mame_options_class_init (MameOptionsClass *klass);
static void mame_options_init (MameOptions *opt);
static void mame_options_finalize (GObject *obj);
static void mame_options_set_property (GObject *object,
					 guint prop_id,
					 const GValue *value,
					 GParamSpec *pspec);
static void mame_options_get_property (GObject *object,
					 guint prop_id,
					 GValue *value,
					 GParamSpec *pspec);

G_DEFINE_TYPE (MameOptions, mame_options, G_TYPE_OBJECT)


typedef struct
{
	gint id;
	gchar *category;
	gchar *key;
	gint def_value;
	gint lower;
	gint upper;
	GValue value;
	
	/* Initialised in class init */
	GType type;
} _MameSupportedOptionsInt;

typedef struct
{
	gint id;
	gchar *category;
	gchar *key;
	gboolean def_value;
	GValue value;
	
	/* Initialised in class init */
	GType type;
} _MameSupportedOptionsBool;

typedef struct
{
	gint id;
	gchar *category;
	gchar *key;
	gdouble def_value;
	gdouble lower;
	gdouble upper;
	GValue value;
	
	/* Initialised in class init */
	GType type;
} _MameSupportedOptionsDbl;

typedef struct
{
	gint id;
	gchar *category;
	gchar *key;
	gchar* def_value;
	GValue value;
	
	/* Initialised in class init */
	GType type;
} _MameSupportedOptionsString;

enum {

	OPTIONS_0,
	OPTION_PERF_AUTOFRAMESKIP,
	OPTION_PERF_FRAMESKIP,
	OPTION_PERF_SPEED,
	OPTION_PERF_THROTTLE,
	OPTION_PERF_SLEEP,
	OPTION_PERF_REFRESHSPEED,
	OPTION_PERF_MULTITHREADING,
	OPTION_PERF_SDLVIDEOFPS,
	OPTION_SOUND_SOUND,
	OPTION_SOUND_SAMPLES,
	OPTION_SOUND_SAMPLERATE,
	OPTION_SOUND_VOLUME,
	OPTION_SOUND_LATENCY,
	OPTION_DEBUG_LOG,
	OPTION_DEBUG_DEBUGSCRIPT,
	OPTION_DEBUG_VERBOSE,
	OPTION_DEBUG_UPDATE_IN_PAUSE,
	OPTION_DEBUG_DEBUG,
	OPTION_DEBUG_OSLOG,
	OPTION_MISC_BIOS,
	OPTION_MISC_CHEAT,
	OPTION_MISC_SKIPGAMEINFO,
	OPTION_PLAYBACK_AUTOSAVE,
/* Disable these since performance is bad
	 OPTION_PLAYBACK_MNGWRITE,
	OPTION_PLAYBACK_AVIWRITE,
	OPTION_PLAYBACK_WAVWRITE,*/
	OPTION_VIDEO_VIDEO,
	OPTION_VIDEO_SCALEMODE,
	OPTION_VIDEO_WINDOW,
	OPTION_VIDEO_MAXIMISE,
	OPTION_VIDEO_KEEPASPECT,
	OPTION_VIDEO_UNEVEN,
	OPTION_VIDEO_CENTERH,
	OPTION_VIDEO_CENTERV,
	OPTION_VIDEO_WAITVSYNC,
	OPTION_VIDEO_EFFECT,
	OPTION_VIDEO_ARTCROP,
	OPTION_VIDEO_BACKDROPS,
	OPTION_VIDEO_BEZELS,
	OPTION_VIDEO_OVERLAYS,
	OPTION_SCREEN_BRIGHTNESS,
	OPTION_SCREEN_CONTRAST,
	OPTION_SCREEN_GAMMA,
	OPTION_SCREEN_PAUSE_BRIGHTNESS,
	OPTION_ROTATION_ROL,
	OPTION_ROTATION_ROR,
	OPTION_ROTATION_AUTOROL,
	OPTION_ROTATION_AUTOROR,
	OPTION_VECTOR_ANTIALIAS,
	OPTION_INPUT_MOUSE,
	OPTION_INPUT_JOYSTICK,
	OPTION_INPUT_LIGHTGUN,
	OPTION_INPUT_MULTIMOUSE,
	OPTION_INPUT_MULTIKEYBOARD,
	OPTION_INPUT_STEADYKEY,
	OPTION_INPUT_OFFSCREEN_RELOAD,
	OPTION_INPUT_KEYMAP,
	OPTION_INPUT_KEYMAP_FILE,
	OPTION_OPENGL_FILTER,
	OPTION_OPENGL_PRESCALE,
	OPTION_OPENGL_NOTEXTURERECT,
	OPTION_OPENGL_FORCEPOW2,
	OPTION_OPENGL_VBO,
	OPTION_OPENGL_PBO,
	OPTION_OPENGL_GLSL,
	OPTION_OPENGL_GLSLFILTER,
	OPTION_OPENGL_GLSLVIDATTR,
	OPTION_VECTOR_BEAM,
	OPTION_VECTOR_FLICKER,
	
	
	
	NUM_OPTIONS
};

/*
   Most options are defined in sdlmame's src/emu/emuopts.c file

   Note that the category and key can only use hyphen; MAME options that
   use underscores must have the underscore in the widget name in the .glade
   file, and use the hyphen here.
   For example, MAME option audio_latency must be represented here as
   audio-latency.
 */

/* Integer options */
static _MameSupportedOptionsInt MameSupportedOptionsInt[] =
{
	{ OPTION_PERF_FRAMESKIP, "Performance", "frameskip", 0, 0, 12, { 0, } },
	{ OPTION_SOUND_LATENCY, "Sound", "audio-latency", 0, 0, 12, { 0, } },
	{ OPTION_OPENGL_PRESCALE, "OpenGL", "prescale", 0, 0, 2048, { 0, } },
};

/* Boolean options. These options are prefixed by 'no' if the value is set off */
static _MameSupportedOptionsBool MameSupportedOptionsBool[] =
{
	{ OPTION_PERF_AUTOFRAMESKIP, "Performance", "autoframeskip", TRUE, { 0, } },
	{ OPTION_PERF_THROTTLE, "Performance", "throttle", TRUE, { 0, } },
	{ OPTION_PERF_SLEEP, "Performance", "sleep", TRUE, { 0, } },
	{ OPTION_PERF_REFRESHSPEED, "Performance", "refreshspeed", TRUE, { 0, } },
	{ OPTION_PERF_MULTITHREADING, "Performance", "multithreading", TRUE, { 0, } },
	{ OPTION_PERF_SDLVIDEOFPS, "Performance", "sdlvideofps", FALSE, { 0, } },
	{ OPTION_SOUND_SOUND, "Sound", "sound", TRUE, { 0, } },
	{ OPTION_SOUND_SAMPLES, "Sound", "samples", TRUE, { 0, } },
	{ OPTION_DEBUG_LOG, "Debugging", "log", FALSE, { 0, } },	
	{ OPTION_DEBUG_VERBOSE, "Debugging", "verbose", FALSE, { 0, } },
	{ OPTION_DEBUG_UPDATE_IN_PAUSE, "Debugging", "update_in_pause", TRUE, { 0, } },
	{ OPTION_DEBUG_DEBUG, "Debugging", "debug", FALSE, { 0, } },
	{ OPTION_DEBUG_OSLOG, "Debugging", "oslog", FALSE, { 0, } },
	{ OPTION_MISC_CHEAT, "Misc", "cheat", TRUE, { 0, } },
	{ OPTION_MISC_SKIPGAMEINFO, "Misc", "skip_gameinfo", FALSE, { 0, } },
	{ OPTION_PLAYBACK_AUTOSAVE, "Playback", "autosave", FALSE, { 0, } },
/*	{ OPTION_PLAYBACK_MNGWRITE, "Playback", "mngwrite", FALSE, { 0, } },
	{ OPTION_PLAYBACK_AVIWRITE, "Playback", "aviwrite", FALSE, { 0, } },
	{ OPTION_PLAYBACK_WAVWRITE, "Playback", "wavwrite", FALSE, { 0, } },*/
	{ OPTION_VIDEO_WINDOW, "Video", "window", FALSE, { 0, } },
	{ OPTION_VIDEO_MAXIMISE, "Video", "maximize", TRUE, { 0, } },
	{ OPTION_VIDEO_KEEPASPECT, "Video", "keepaspect", TRUE, { 0, } },
	{ OPTION_VIDEO_UNEVEN, "Video", "unevenstretch", TRUE, { 0, } },
	{ OPTION_VIDEO_CENTERH, "Video", "centerh", TRUE, { 0, } },
	{ OPTION_VIDEO_CENTERV, "Video", "centerv", TRUE, { 0, } },
	{ OPTION_VIDEO_WAITVSYNC, "Video", "waitvsync", TRUE, { 0, } },
	{ OPTION_VIDEO_ARTCROP, "Artwork", "artwork_crop", FALSE, { 0, } },
	{ OPTION_VIDEO_BACKDROPS, "Artwork", "use_backdrops", TRUE, { 0, } },
	{ OPTION_VIDEO_BEZELS, "Artwork", "use_bezels", TRUE, { 0, } },
	{ OPTION_VIDEO_OVERLAYS, "Artwork", "use_overlays", TRUE, { 0, } },
	{ OPTION_ROTATION_ROL, "Rotation", "rol", TRUE, { 0, } },
	{ OPTION_ROTATION_ROR, "Rotation", "ror", TRUE, { 0, } },
	{ OPTION_ROTATION_AUTOROL, "Rotation", "autorol", TRUE, { 0, } },
	{ OPTION_ROTATION_AUTOROR, "Rotation", "autoror", TRUE, { 0, } },
	{ OPTION_VECTOR_ANTIALIAS, "Vector", "antialias", TRUE, { 0, } },
	{ OPTION_INPUT_MOUSE, "Input", "mouse", TRUE, { 0, } },
	{ OPTION_INPUT_JOYSTICK, "Input", "joystick", TRUE, { 0, } },
	{ OPTION_INPUT_LIGHTGUN, "Input", "lightgun", TRUE, { 0, } },
	{ OPTION_INPUT_MULTIMOUSE, "Input", "multimouse", TRUE, { 0, } },
	{ OPTION_INPUT_MULTIKEYBOARD, "Input", "multikeyboard", TRUE, { 0, } },
	{ OPTION_INPUT_STEADYKEY, "Input", "steadykey", TRUE, { 0, } },
	{ OPTION_INPUT_OFFSCREEN_RELOAD, "Input", "offscreen_reload", TRUE, { 0, } },
	{ OPTION_INPUT_KEYMAP, "Input", "keymap", FALSE, { 0, } },
	{ OPTION_OPENGL_FILTER, "OpenGL", "filter", TRUE, { 0, } },
	{ OPTION_OPENGL_NOTEXTURERECT, "OpenGL", "gl_notexturerect", TRUE, { 0, } },
	{ OPTION_OPENGL_FORCEPOW2, "OpenGL", "gl_forcepow2texture", FALSE, { 0, } },
	{ OPTION_OPENGL_VBO, "OpenGL", "gl_vbo", TRUE, { 0, } },
	{ OPTION_OPENGL_PBO, "OpenGL", "gl_pbo", TRUE, { 0, } },
	{ OPTION_OPENGL_GLSL, "OpenGL", "gl_glsl", TRUE, { 0, } },
	{ OPTION_OPENGL_GLSLVIDATTR, "OpenGL", "gl_glsl_vid_attr", TRUE, { 0, } },	
};

/* Double options */
static _MameSupportedOptionsDbl MameSupportedOptionsDbl[] =
{
	{ OPTION_PERF_SPEED, "Performance", "speed", 1.0, 0.01, 100.0, { 0, } },
	{ OPTION_SOUND_VOLUME, "Sound", "volume", 0, -32, 0, { 0, } },
	{ OPTION_SCREEN_BRIGHTNESS, "Screen", "brightness", 1.0, 0.1, 2.0, { 0, } },
	{ OPTION_SCREEN_CONTRAST, "Screen", "contrast", 1.0, 0.1, 2.0, { 0, } },
	{ OPTION_SCREEN_GAMMA, "Screen", "gamma", 1.0, 0.1, 3.0, { 0, } },
	{ OPTION_SCREEN_PAUSE_BRIGHTNESS, "Screen", "pause_brightness", 0.65, 0.0, 1.0, { 0, } },
	{ OPTION_VECTOR_BEAM, "Vector", "beam", 1.0, 0.0, 1.0, { 0, } },	/* FIXME TODO Unknown min/max */
	{ OPTION_VECTOR_FLICKER, "Vector", "flicker", 0, 0.0, 1.0, { 0, } },    /* FIXME TODO Unknown min/max */
};

/* FIXME TODO Set default values for ALL options */

/* String options */
/* Note that options intended to be displayed in ComboBoxes (e.g. effect) should be strings */
static _MameSupportedOptionsString MameSupportedOptionsString[] =
{	
	{ OPTION_DEBUG_DEBUGSCRIPT, "Debugging", "debugscript", "", { 0, } },
	{ OPTION_MISC_BIOS, "Misc", "bios", "", { 0, } },
/*	{ OPTION_MISC_BIOS, "Playback", "mngwrite", "", { 0, } },
	{ OPTION_MISC_BIOS, "Playback", "aviwrite", "", { 0, } },
	{ OPTION_MISC_BIOS, "Playback", "wavwrite", "", { 0, } },*/

	/* The following are combobox entries so are marked as strings */
	{ OPTION_VIDEO_VIDEO, "Video", "video", "soft", { 0, } },
	{ OPTION_VIDEO_EFFECT, "Video", "effect", "none",  { 0, } },
	{ OPTION_VIDEO_SCALEMODE, "Video", "scalemode", "none", { 0, } },
	{ OPTION_OPENGL_GLSLFILTER, "OpenGL", "gl_glsl_filter", "0", { 0, } },
	{ OPTION_SOUND_SAMPLERATE, "Sound", "samplerate", "44100", { 0, } },

	/* Filename entries */
	{ OPTION_INPUT_KEYMAP_FILE, "Input", "keymap_file", "", { 0, } },

};

struct _MameOptionsPriv
{
	GKeyFile *options_file;
	gchar *filename;	/* Filename of the options file */
};


/**
 * mame_options_new:
 * 
 * Creates a new #MameOptions object
 * 
 * Return value: A #MameOptions object.
 */
MameOptions *
mame_options_new (void)
{
	/* TODO Pass in object MameRomEntry - if null, then its default options */
	MameOptions *pr;
	
	pr = g_object_new (MAME_TYPE_OPTIONS, NULL);
	
	return pr;
	
}

/* FIXME TODO
   Used with unblock_update_property_on_change_str () to prevent GtkEntry fields
   from triggering an event whenever a new letter is entered;
   i.e. we only want to trigger a 'changed' event when the GtkEntry loses focus 
static gboolean
block_update_property_on_change_str (GtkWidget *widget, GdkEvent *event,
							  gpointer user_data)
{
	MameProperty *p = (MameProperty *) user_data;

	gtk_signal_handler_block_by_func (GTK_OBJECT(p->object), G_CALLBACK (update_property_on_change_str), p);
	return FALSE;
}

* Used with block_update_property_on_change_str () to prevent GtkEntry fields
   from triggering an event whenever a new letter is entered;
   i.e. we only want to trigger a 'changed' event when the GtkEntry loses focus *
static gboolean
unblock_update_property_on_change_str (GtkWidget *widget, GdkEvent *event,
							  gpointer user_data)
{
	MameProperty *p = (MameProperty *) user_data;

	gtk_signal_handler_unblock_by_func (GTK_OBJECT(p->object), G_CALLBACK (update_property_on_change_str), p);
	return FALSE;
}*/


gchar *
mame_options_get_option_string (MameOptions *opts, gchar *category)
{
	MameExec *exec;
	gchar *command;
	gchar *catkey;
	guint i;
	
	g_return_val_if_fail (opts != NULL, NULL);

	exec = mame_exec_list_get_current_executable (main_gui.exec_list);
	
	g_return_val_if_fail (exec != NULL, NULL);
	
	command = g_strdup ("");

	/* Parse each of the bool, int, str and dbl options and concatenate them
	   to the option string */
	
	for (i = 0; i < G_N_ELEMENTS (MameSupportedOptionsBool); i++) {
		gboolean val;
		
		/* Only use the command if it is supported */
		if ((g_ascii_strcasecmp (category, MameSupportedOptionsBool[i].category) == 0) && 
		    (mame_has_option (exec, MameSupportedOptionsBool[i].key))) {	
			catkey = g_strdup_printf("%s-%s",
						 MameSupportedOptionsBool[i].category,
						 MameSupportedOptionsBool[i].key);
			val = mame_options_get_bool (opts, catkey);
		
			if (val) {
				command = g_strconcat (command,
						       " -",
						       MameSupportedOptionsBool[i].key,
						       NULL);
			} else {
				/* Some toggle options are disabled using 'no' as a
				   prefix (e.g. -nolog) but we need to check whether MAME
				   recognises them first */
				if (mame_option_supports_no_prefix (exec, MameSupportedOptionsBool[i].key)) {
					command = g_strconcat (command,
							       " -no",
							       MameSupportedOptionsBool[i].key,
							       NULL);
				} /* else we simply omit it */

			}
		
			g_free (catkey);
		}
	}
	
	for (i = 0; i < G_N_ELEMENTS (MameSupportedOptionsInt); i++) {
		gint val;
		gchar *str;

		/* Only use the command if it is supported */
		if ((g_ascii_strcasecmp (category, MameSupportedOptionsInt[i].category) == 0) && 
		    (mame_has_option (exec, MameSupportedOptionsInt[i].key))) {	
			catkey = g_strdup_printf("%s-%s",
						 MameSupportedOptionsInt[i].category,
						 MameSupportedOptionsInt[i].key);
			val = mame_options_get_int (opts, catkey);
			str = g_strdup_printf ("%d", val);
		
			command = g_strconcat (command,
					       " -",
					       MameSupportedOptionsInt[i].key,
					       " ",
					       str,
					       NULL);
			g_free (catkey);
			g_free (str);
		}
	}
	
	for (i = 0; i < G_N_ELEMENTS (MameSupportedOptionsDbl); i++) {
		gdouble val;
		gchar *str;
		
		/* Only use the command if it is supported */
		if ((g_ascii_strcasecmp (category, MameSupportedOptionsDbl[i].category) == 0) && 
		    (mame_has_option (exec, MameSupportedOptionsDbl[i].key))) {
			catkey = g_strdup_printf("%s-%s",
						 MameSupportedOptionsDbl[i].category,
						 MameSupportedOptionsDbl[i].key);
			val = mame_options_get_dbl (opts, catkey);
			str = g_strdup_printf ("%.2f", val);

			command = g_strconcat (command,
					       " -",
					       MameSupportedOptionsDbl[i].key,
					       " ",
					       str,
					       NULL);
			g_free (catkey);
			g_free (str);
		}
	}

	for (i = 0; i < G_N_ELEMENTS (MameSupportedOptionsString); i++) {
		gchar* val;
		/* Only use the command if it is supported */
		if ((g_ascii_strcasecmp (category, MameSupportedOptionsString[i].category) == 0) && 
		    (mame_has_option (exec, MameSupportedOptionsString[i].key))) {
			catkey = g_strdup_printf("%s-%s",
						 MameSupportedOptionsString[i].category,
						 MameSupportedOptionsString[i].key);
			val = mame_options_get (opts, catkey);
		
			/* FIXME TODO Don't append string if 0 length */
			command = g_strconcat (command,
					       " -",
					       MameSupportedOptionsString[i].key,
					       " ",
					       val,
					       NULL);
			g_free (catkey);
			g_free (val);
		}
	}
	
	return command;
}

/* The following mame_options_save_xxx functions are triggered when the
   MameOptions object properties are changed as part of the notify- signal */
static void
mame_options_save_string (MameOptions *opts, GParamSpec *param, gpointer user_data)
{
	GMAMEUI_DEBUG ("mame_options_save_string: %s", param->name);
	/* param->name will be of the form Category-Key, e.g. Video-beam */
	
//	gchar** stv;
//	GValue *dbl_value;
	_MameSupportedOptionsString *opt;
	const gchar *value;

	g_return_if_fail (MAME_IS_OPTIONS (opts));
	g_return_if_fail (param->name != NULL);
	
	opt = (_MameSupportedOptionsString *) user_data;
	g_return_if_fail (opt != NULL);
	
	value = g_value_get_string (&opt->value);
	
//	stv = g_strsplit (param->name, "-", 2); /* Split only on the first '-', so keys with a '-' are ignored */
	g_key_file_set_string (opts->priv->options_file, opt->category, opt->key, value);
//	g_strfreev (stv);
	
	g_key_file_save_to_file (opts->priv->options_file, opts->priv->filename);
}

static void
mame_options_save_double (MameOptions *opts, GParamSpec *param, gpointer user_data)
{
	GMAMEUI_DEBUG ("mame_options_save_double: %s", param->name);
	
	/* param->name will be of the form Category-Key, e.g. Video-beam */
	
//	gchar** stv;
//	GValue *dbl_value;
	_MameSupportedOptionsDbl *opt;
	gdouble value;

	g_return_if_fail (MAME_IS_OPTIONS (opts));
	g_return_if_fail (param->name != NULL);
	
	opt = (_MameSupportedOptionsDbl *) user_data;
	g_return_if_fail (opt != NULL);
	
	value = g_value_get_double (&opt->value);
	
//	stv = g_strsplit (param->name, "-", 2); /* Split only on the first '-', so keys with a '-' are ignored */
	g_key_file_set_double (opts->priv->options_file, opt->category, opt->key, value);
//	g_strfreev (stv);
	
	g_key_file_save_to_file (opts->priv->options_file, opts->priv->filename);
}

static void
mame_options_save_int (MameOptions *opts, GParamSpec *param, gpointer user_data)
{
	GMAMEUI_DEBUG ("mame_options_save_int: %s", param->name);
	
	/* param->name will be of the form Category-Key, e.g. Video-beam */
	
//	gchar** stv;
//	GValue *int_value;
	_MameSupportedOptionsInt *opt;
	gint value;

	g_return_if_fail (MAME_IS_OPTIONS (opts));
	g_return_if_fail (param->name != NULL);
	
	opt = (_MameSupportedOptionsInt *) user_data;
	g_return_if_fail (opt != NULL);
	
	value = g_value_get_int (&opt->value);

//	stv = g_strsplit (param->name, "-", 2); /* Split only on the first '-', so keys with a '-' are ignored */
	g_key_file_set_integer (opts->priv->options_file, opt->category, opt->key, value);
//	g_strfreev (stv);
	
	g_key_file_save_to_file (opts->priv->options_file, opts->priv->filename);
}

static void
mame_options_save_bool (MameOptions *opts, GParamSpec *param, gpointer user_data)
{
	GMAMEUI_DEBUG ("mame_options_save_bool: %s", param->name);

	/* param->name will be of the form Category-Key, e.g. Video-beam */
	
//	gchar** stv;
	_MameSupportedOptionsBool *opt;
//	GValue *bool_value;
	gboolean value;

	g_return_if_fail (MAME_IS_OPTIONS (opts));
	g_return_if_fail (param->name != NULL);
	
	opt = (_MameSupportedOptionsBool *) user_data;
	g_return_if_fail (opt != NULL);
	
	value = g_value_get_boolean (&opt->value);
	
//	stv = g_strsplit (opt->key, "-", 2); /* Split only on the first '-', so keys with a '-' are ignored */
	g_key_file_set_boolean (opts->priv->options_file, opt->category, opt->key, value);
//	g_strfreev (stv);
	
	g_key_file_save_to_file (opts->priv->options_file, opts->priv->filename);
}

/* Gets the value from the keyfile for the specified key, which should be of the
   format category.key, i.e. Sound-volume */
gchar *
mame_options_get (MameOptions *opts, const gchar *key)
{
	gchar *ret_val;
	gchar **stv;
	GError *error = NULL;
	
	g_return_val_if_fail (MAME_IS_OPTIONS (opts), NULL);
	g_return_val_if_fail (key != NULL, NULL);
	
	ret_val = 0;
	stv = g_strsplit (key, "-", 2);
	ret_val = g_key_file_get_string (opts->priv->options_file, stv[0], stv[1], &error);
	GMAMEUI_DEBUG ("Getting settings for %s - %s. Value is %s", stv[0], stv[1], ret_val);
	g_strfreev (stv);
	
	if (error) {
		GMAMEUI_DEBUG ("Error retrieving string option %s: %s", key, error->message);
		g_error_free (error);
		error = NULL;
/* FIXME TODO Set default value and use the G_CONSTRUCT parameter to set it up automatically
   (both for this and other similar instances */		
		/* Value doesn't exist in the file, so we need to use the default value */
		GParamSpec *spec;
		GValue value = { 0, };

		spec = g_object_class_find_property (G_OBJECT_GET_CLASS (opts), key);
		
		g_value_init (&value, G_TYPE_STRING);
		g_param_value_set_default (spec, &value);
		
		ret_val = g_strdup (g_value_get_string (&value));
		GMAMEUI_DEBUG (_("Retrieving default string value for %s: %s"), key, ret_val);
	}
	
	return ret_val;
}

/* The following functions invoke g_object_set, which triggers the notify
   signal. Each of the notify callback handlers subsequently save the keyfile.
 
   Note that since the property name should contain '_', we need to process the key first */
void
mame_options_set_int (MameOptions *opts, gchar *key, gint value)
{
	gchar *prop_name;
	
	prop_name = g_strdup (key);
	prop_name = g_strcanon (prop_name, G_CSET_A_2_Z G_CSET_a_2_z G_CSET_DIGITS "-", '-');
	g_object_set (opts, prop_name, value, NULL);
	g_free (prop_name);
}

void
mame_options_set_dbl (MameOptions *opts, gchar *key, gdouble value)
{
	gchar *prop_name;
	
	prop_name = g_strdup (key);
	prop_name = g_strcanon (prop_name, G_CSET_A_2_Z G_CSET_a_2_z G_CSET_DIGITS "-", '-');
	g_object_set (opts, prop_name, value, NULL);
	g_free (prop_name);
}

void
mame_options_set_string (MameOptions *opts, gchar *key, gchar* value)
{
	gchar *prop_name;
	
	prop_name = g_strdup (key);
	prop_name = g_strcanon (prop_name, G_CSET_A_2_Z G_CSET_a_2_z G_CSET_DIGITS "-", '-');
	g_object_set (opts, prop_name, value, NULL);
	g_free (prop_name);
}

/* Key should be passed in in the form category-key, e.g. Video-artwork_crop */
gint
mame_options_get_int (MameOptions *opts, const gchar *key)
{
	gint ret_val;
	gchar** stv;
	GError *error = NULL;

	g_return_val_if_fail (MAME_IS_OPTIONS (opts), 0);
	g_return_val_if_fail (key != NULL, 0);
	g_return_val_if_fail (opts->priv->options_file != NULL, 0);

	ret_val = 0;
	stv = g_strsplit (key, "-", 2);
	ret_val = g_key_file_get_integer (opts->priv->options_file, stv[0], stv[1], &error);

	g_strfreev (stv);

	if (error) {
		GMAMEUI_DEBUG ("Error retrieving int option %s: %s", key, error->message);
		g_error_free (error);
		error = NULL;
		
		/* Value doesn't exist in the file, so we need to use the default value */
		GParamSpec *spec;
		GValue value = { 0, };

		spec = g_object_class_find_property (G_OBJECT_GET_CLASS (opts), key);
		
		g_value_init (&value, G_TYPE_INT);
		g_param_value_set_default (spec, &value);
		
		ret_val = g_value_get_int (&value);
		GMAMEUI_DEBUG (_("Retrieving default int value for %s: %d"), key, ret_val);
	}
	
	return ret_val;
}

gdouble
mame_options_get_dbl (MameOptions *opts, const gchar *key)
{
	gdouble ret_val;
	gchar** stv;
	GError *error = NULL;

	g_return_val_if_fail (MAME_IS_OPTIONS (opts), 0);
	g_return_val_if_fail (key != NULL, 0);
	g_return_val_if_fail (opts->priv->options_file != NULL, 0);

	ret_val = 0;
	stv = g_strsplit (key, "-", 2);
	ret_val = g_key_file_get_double (opts->priv->options_file, stv[0], stv[1], &error);

	g_strfreev (stv);

	if (error) {
		GMAMEUI_DEBUG ("Error retrieving double option %s: %s", key, error->message);
		g_error_free (error);
		error = NULL;
		
		/* Value doesn't exist in the file, so we need to use the default value */
		GParamSpec *spec;
		GValue value = { 0, };

		spec = g_object_class_find_property (G_OBJECT_GET_CLASS (opts), key);
		
		g_value_init (&value, G_TYPE_DOUBLE);
		g_param_value_set_default (spec, &value);
		
		ret_val = g_value_get_double (&value);
		GMAMEUI_DEBUG (_("Retrieving default double value for %s: %.2f"), key, ret_val);
	}
	
	return ret_val;
}

gboolean
mame_options_get_bool (MameOptions *opts, const gchar *key)
{
	gboolean ret_val;
	gchar** stv;
	GError *error = NULL;

	g_return_val_if_fail (MAME_IS_OPTIONS (opts), 0);
	g_return_val_if_fail (key != NULL, 0);
	g_return_val_if_fail (opts->priv->options_file != NULL, 0);

	ret_val = 0;
	stv = g_strsplit (key, "-", 2);
	ret_val = g_key_file_get_boolean (opts->priv->options_file, stv[0], stv[1], &error);
	
	g_strfreev (stv);

	if (error) {
		GMAMEUI_DEBUG ("Error retrieving boolean option %s: %s", key, error->message);
		g_error_free (error);
		error = NULL;
		
		/* Value doesn't exist in the file, so we need to use the default value */
		GParamSpec *spec;
		GValue value = { 0, };

		spec = g_object_class_find_property (G_OBJECT_GET_CLASS (opts), key);
		
		g_value_init (&value, G_TYPE_BOOLEAN);
		g_param_value_set_default (spec, &value);
		
		ret_val = g_value_get_boolean (&value);
		GMAMEUI_DEBUG (_("Retrieving default boolean value for %s: %d"), key, ret_val);
	}
	
	return ret_val;
}


/* The property name (prop_name) will only contain '-', not '_', so we need to
   also temporarily perform this find/replace with the key to compare properly
   in all the find_xxx_offset functions */

static int
find_bool_offset (gchar *prop_name)
{
	guint i, found;
	gchar **tokens;
	tokens = g_strsplit (prop_name, "-", 2);

	for (i = 0; i < G_N_ELEMENTS (MameSupportedOptionsBool); i++) {
		gchar *tmp_prop_name;
		tmp_prop_name = g_strdup (MameSupportedOptionsBool[i].key);
		tmp_prop_name = g_strcanon (tmp_prop_name, G_CSET_A_2_Z G_CSET_a_2_z G_CSET_DIGITS "-", '-');
		
		if (g_ascii_strcasecmp (tmp_prop_name, tokens[1]) == 0) {
			found = i;
			g_free (tmp_prop_name);
			break;
		}
		g_free (tmp_prop_name);
	}
	
	g_strfreev (tokens);
	
	if (i == G_N_ELEMENTS (MameSupportedOptionsBool))
		GMAMEUI_DEBUG ("WARNING - offset for boolean prop %s not found", prop_name);
	
	return found;
}

static guint
find_str_offset (gchar *prop_name)
{
	guint i, found;
	gchar **tokens;
	tokens = g_strsplit (prop_name, "-", 2);

	for (i = 0; i < G_N_ELEMENTS (MameSupportedOptionsString); i++) {
		gchar *tmp_prop_name;
		tmp_prop_name = g_strdup (MameSupportedOptionsString[i].key);
		tmp_prop_name = g_strcanon (tmp_prop_name, G_CSET_A_2_Z G_CSET_a_2_z G_CSET_DIGITS "-", '-');
			
		if (g_ascii_strcasecmp (tmp_prop_name, tokens[1]) == 0) {
			found = i;
			g_free (tmp_prop_name);
			break;
		}
		g_free (tmp_prop_name);
	}
	
	g_strfreev (tokens);

	if (i == G_N_ELEMENTS (MameSupportedOptionsString))
		GMAMEUI_DEBUG ("WARNING - offset for string prop %s not found", prop_name);
	
	return found;
}

static guint
find_int_offset (gchar *prop_name)
{
	guint i, found;
	gchar **tokens;
	tokens = g_strsplit (prop_name, "-", 2);

	for (i = 0; i < G_N_ELEMENTS (MameSupportedOptionsInt); i++) {
		gchar *tmp_prop_name;
		tmp_prop_name = g_strdup (MameSupportedOptionsInt[i].key);
		tmp_prop_name = g_strcanon (tmp_prop_name, G_CSET_A_2_Z G_CSET_a_2_z G_CSET_DIGITS "-", '-');
		
		if (g_ascii_strcasecmp (tmp_prop_name, tokens[1]) == 0) {
			found = i;
			g_free (tmp_prop_name);
			break;
		}
		g_free (tmp_prop_name);
	}
	
	g_strfreev (tokens);

	if (i == G_N_ELEMENTS (MameSupportedOptionsInt))
		GMAMEUI_DEBUG ("WARNING - offset for integer prop %s not found", prop_name);
	
	return found;
}

static int
find_dbl_offset (gchar *prop_name)
{
	guint i, found;
	gchar **tokens;
	tokens = g_strsplit (prop_name, "-", 2);

	for (i = 0; i < G_N_ELEMENTS (MameSupportedOptionsDbl); i++) {
		gchar *tmp_prop_name;
		tmp_prop_name = g_strdup (MameSupportedOptionsDbl[i].key);
		tmp_prop_name = g_strcanon (tmp_prop_name, G_CSET_A_2_Z G_CSET_a_2_z G_CSET_DIGITS "-", '-');
		
		if (g_ascii_strcasecmp (tmp_prop_name, tokens[1]) == 0) {
			found = i;
			g_free (tmp_prop_name);
			break;
		}
		g_free (tmp_prop_name);
	}
	
	g_strfreev (tokens);
	
	if (i == G_N_ELEMENTS (MameSupportedOptionsDbl))
		GMAMEUI_DEBUG ("WARNING - offset for double prop %s not found", prop_name);
	
	return found;
}

static void
mame_options_set_property (GObject *object,
                           guint prop_id,
                           const GValue *value,
                           GParamSpec *pspec)
{
	int offset;
	
	switch (pspec->value_type) {
		case G_TYPE_BOOLEAN:
			offset = find_bool_offset (pspec->name);
			GMAMEUI_DEBUG ("Attempting to set bool prop %s at pos %d(%d) with value %d",
				       pspec->name, prop_id, offset, g_value_get_boolean (value));
			g_value_set_boolean (&MameSupportedOptionsBool[offset].value,
					     g_value_get_boolean (value));
			break;
		case G_TYPE_DOUBLE:
			offset = find_dbl_offset (pspec->name);
			GMAMEUI_DEBUG ("Attempting to set double prop %s at pos %d(%d) with value %.2f",
				       pspec->name, prop_id, offset, g_value_get_double (value));
			g_value_set_double (&MameSupportedOptionsDbl[offset].value,
					    g_value_get_double (value));
			break;
		case G_TYPE_INT:
			offset = find_int_offset (pspec->name);
			GMAMEUI_DEBUG ("Attempting to set integer prop %s at pos %d(%d) with value %d",
				       pspec->name, prop_id, offset, g_value_get_int (value));
			g_value_set_int (&MameSupportedOptionsInt[offset].value,
					 g_value_get_int (value));
			break;
		case G_TYPE_STRING:
			offset = find_str_offset (pspec->name);
			GMAMEUI_DEBUG ("Attempting to set string prop %s at pos %d(%d) with value %s",
				       pspec->name, prop_id, offset, g_value_get_string (value));
			g_value_set_string (&MameSupportedOptionsString[offset].value,
					    g_value_get_string (value));
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

static void
mame_options_get_property (GObject *object,
                           guint prop_id,
                           GValue *value,
                           GParamSpec *pspec)
{
	int offset;
	
	switch (pspec->value_type) {
		case G_TYPE_BOOLEAN:
			offset = find_bool_offset (pspec->name);
			GMAMEUI_DEBUG ("Attempting to get bool prop %s at pos %d",
				       pspec->name, prop_id);
			g_value_set_boolean (value, g_value_get_boolean (&MameSupportedOptionsBool[offset].value));
			break;
		case G_TYPE_DOUBLE:
			offset = find_dbl_offset (pspec->name);
			GMAMEUI_DEBUG ("Attempting to get double prop %s at pos %d",
				       pspec->name, prop_id);
			g_value_set_double (value, g_value_get_double (&MameSupportedOptionsDbl[offset].value));
			break;
		case G_TYPE_INT:
			offset = find_int_offset (pspec->name);
			GMAMEUI_DEBUG ("Attempting to get integer prop %s at pos %d",
				       pspec->name, prop_id);
			g_value_set_int (value, g_value_get_int (&MameSupportedOptionsInt[offset].value));
			break;
		case G_TYPE_STRING:
			offset = find_str_offset (pspec->name);
			GMAMEUI_DEBUG ("Attempting to get string prop %s at pos %d",
				       pspec->name, prop_id);
			g_value_set_string (value, g_value_get_string (&MameSupportedOptionsString[offset].value));
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

/* Property names can have _ and -, but signal names can't - need to have a consistent approach */
static gchar
*create_property_name (gchar *category, gchar *key)
{
	gchar *ret;
	
	ret = g_strdup_printf ("%s-%s", category, key);
	
	/* Replace any characters that are not letters or '-'. This
	   should replace any underline characters */
	ret = g_strcanon (ret, G_CSET_A_2_Z G_CSET_a_2_z G_CSET_DIGITS "-", '-');
	
	return ret;
}

static void
mame_options_class_init (MameOptionsClass *klass)
{
	guint i;
	
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	
	object_class->set_property = mame_options_set_property;
	object_class->get_property = mame_options_get_property;
	object_class->finalize = mame_options_finalize;
GMAMEUI_DEBUG ("Creating MameOptions class object...");
	/* Initialise the class properties - any MAME option which contains an
	   underscore needs to be have the underscore replaced by a hyphen; this
	   is to workaround issues with GLib where a property/signal name cannot
	   contain both. This affects the setting the property name, the notify
	   signal name and when finding the offset for get/set */
	
	/* Initialise boolean class properties */
	for (i = 0; i < G_N_ELEMENTS (MameSupportedOptionsBool); i++) {
		gchar *attr_name;
		
		attr_name = create_property_name (MameSupportedOptionsBool[i].category,
						  MameSupportedOptionsBool[i].key);
		GMAMEUI_DEBUG ("   Creating boolean class property %s with id %d",
			       attr_name,
			       MameSupportedOptionsBool[i].id);
		/* Initialise type */
		MameSupportedOptionsBool[i].type = G_TYPE_BOOLEAN;
		g_value_init (&MameSupportedOptionsBool[i].value, G_TYPE_BOOLEAN);
		
		/* Install the property */
		g_object_class_install_property (object_class,
					MameSupportedOptionsBool[i].id,
					g_param_spec_boolean (attr_name, "", "", MameSupportedOptionsBool[i].def_value, G_PARAM_READWRITE));
		
		g_free (attr_name);
	}
	
	/* Initialise double class properties */
	for (i = 0; i < G_N_ELEMENTS (MameSupportedOptionsDbl); i++) {
		gchar *attr_name;
		
		attr_name = create_property_name (MameSupportedOptionsDbl[i].category,
						  MameSupportedOptionsDbl[i].key);
		
		GMAMEUI_DEBUG ("   Creating double class property %s with id %d",
			       attr_name,
			       MameSupportedOptionsDbl[i].id);
		/* Initialise type */
		MameSupportedOptionsDbl[i].type = G_TYPE_DOUBLE;
		g_value_init (&MameSupportedOptionsDbl[i].value, G_TYPE_DOUBLE);
		
		/* Install the property */
		g_object_class_install_property (object_class,
					MameSupportedOptionsDbl[i].id,
					g_param_spec_double (attr_name, "", "", MameSupportedOptionsDbl[i].lower, MameSupportedOptionsDbl[i].upper, MameSupportedOptionsDbl[i].def_value, G_PARAM_READWRITE));
		
		g_free (attr_name);
	}
	
	/* Initialise integer class properties */
	for (i = 0; i < G_N_ELEMENTS (MameSupportedOptionsInt); i++) {
		gchar *attr_name;
		
		attr_name = create_property_name (MameSupportedOptionsInt[i].category,
						  MameSupportedOptionsInt[i].key);
		
		GMAMEUI_DEBUG ("   Creating integer class property %s with id %d",
			       attr_name,
			       MameSupportedOptionsInt[i].id);
		/* Initialise type */
		MameSupportedOptionsInt[i].type = G_TYPE_INT;
		g_value_init (&MameSupportedOptionsInt[i].value, G_TYPE_INT);
		
		/* Install the property */
		g_object_class_install_property (object_class,
					MameSupportedOptionsInt[i].id,
					g_param_spec_int (attr_name, "", "", MameSupportedOptionsInt[i].lower, MameSupportedOptionsInt[i].upper, MameSupportedOptionsInt[i].def_value, G_PARAM_READWRITE));
		
		g_free (attr_name);
	}
	
	/* Initialise string class properties */
	for (i = 0; i < G_N_ELEMENTS (MameSupportedOptionsString); i++) {
		gchar *attr_name;
		
		attr_name = create_property_name (MameSupportedOptionsString[i].category,
						  MameSupportedOptionsString[i].key);
		
		GMAMEUI_DEBUG ("   Creating string class property %s with id %d",
			       attr_name,
			       MameSupportedOptionsString[i].id);
		/* Initialise type */
		MameSupportedOptionsString[i].type = G_TYPE_STRING;
		g_value_init (&MameSupportedOptionsString[i].value, G_TYPE_STRING);
		
		/* Install the property */
		g_object_class_install_property (object_class,
					MameSupportedOptionsString[i].id,
					g_param_spec_string (attr_name, "", "", MameSupportedOptionsString[i].def_value, G_PARAM_READWRITE));
		
		g_free (attr_name);
	}
GMAMEUI_DEBUG ("Creating MameOptions class object... done");
}

static void
mame_options_init (MameOptions *opts)
{
	guint i;
	
	opts->priv = g_new0 (MameOptionsPriv, 1);
	
	/* This gets called from mame_options_new. From that function, also pass in a RomEntry
	   If RomEntry is NULL, get default options, otherwise get the options
	   for a specific ROM */
	opts->priv->filename = g_build_filename (g_get_user_config_dir (),
	                                         "gmameui", "options",
	                                         "default.ini", NULL);
	opts->priv->options_file = g_key_file_new ();
	GError *error = NULL;
	g_key_file_load_from_file (opts->priv->options_file,
				   opts->priv->filename,
				   G_KEY_FILE_KEEP_COMMENTS,
				   &error);

	if (error) {
		GMAMEUI_DEBUG ("options file could not be loaded: %s", error->message);
		g_error_free (error);
		error = NULL;
	}
	
	/* Set handlers so that whenever the values are changed (from anywhere), the signal handler
	   is invoked; the callback then saves to the g_key_file */
	GMAMEUI_DEBUG ("Setting up notify signal handlers for MameOptions boolean properties...");
	for (i = 0; i < G_N_ELEMENTS (MameSupportedOptionsBool); i++) {
		gchar *signal_name;
		gchar *prop_name;
		
		prop_name = create_property_name (MameSupportedOptionsBool[i].category,
						  MameSupportedOptionsBool[i].key);
		signal_name = g_strdup_printf ("notify::%s", prop_name);       
		GMAMEUI_DEBUG ("  Setting boolean signal handler %s", signal_name);

		g_signal_connect (opts, signal_name, (GCallback) mame_options_save_bool, &MameSupportedOptionsBool[i]);
	
		g_free (prop_name);
		g_free (signal_name);
	}
	GMAMEUI_DEBUG ("Setting up notify signal handlers for MameOptions boolean properties... done");

	GMAMEUI_DEBUG ("Setting up notify signal handlers for MameOptions double properties...");
	for (i = 0; i < G_N_ELEMENTS (MameSupportedOptionsDbl); i++) {
		gchar *signal_name;
		gchar *prop_name;
		
		prop_name = create_property_name (MameSupportedOptionsDbl[i].category,
						  MameSupportedOptionsDbl[i].key);
		signal_name = g_strdup_printf ("notify::%s", prop_name);
		GMAMEUI_DEBUG ("  Setting double signal handler %s", signal_name);

		g_signal_connect (opts, signal_name, (GCallback) mame_options_save_double, &MameSupportedOptionsDbl[i]);
	
		g_free (prop_name);
		g_free (signal_name);
	}
	GMAMEUI_DEBUG ("Setting up notify signal handlers for MameOptions double properties... done");
	
	GMAMEUI_DEBUG ("Setting up notify signal handlers for MameOptions integer properties...");
	for (i = 0; i < G_N_ELEMENTS (MameSupportedOptionsInt); i++) {
		gchar *signal_name;
		gchar *prop_name;
		
		prop_name = create_property_name (MameSupportedOptionsInt[i].category,
						  MameSupportedOptionsInt[i].key);
		signal_name = g_strdup_printf ("notify::%s", prop_name);
		GMAMEUI_DEBUG ("  Setting integer signal handler %s", signal_name);

		g_signal_connect (opts, signal_name, (GCallback) mame_options_save_int, &MameSupportedOptionsInt[i]);
		
		g_free (prop_name);
		g_free (signal_name);
	}
	GMAMEUI_DEBUG ("Setting up notify signal handlers for MameOptions integer properties... done");
	
	GMAMEUI_DEBUG ("Setting up notify signal handlers for MameOptions string properties...");
	for (i = 0; i < G_N_ELEMENTS (MameSupportedOptionsString); i++) {
		gchar *signal_name;
		gchar *prop_name;
		
		prop_name = create_property_name (MameSupportedOptionsString[i].category,
						  MameSupportedOptionsString[i].key);
		signal_name = g_strdup_printf ("notify::%s", prop_name);
		GMAMEUI_DEBUG ("  Setting string signal handler %s", signal_name);

		g_signal_connect (opts, signal_name, (GCallback) mame_options_save_string, &MameSupportedOptionsString[i]);
		
		g_free (prop_name);
		g_free (signal_name);
	}
	GMAMEUI_DEBUG ("Setting up notify signal handlers for MameOptions string properties... done");
}

static void
mame_options_finalize (GObject *obj)
{
	MameOptions *opts = MAME_OPTIONS (obj);
GMAMEUI_DEBUG ("Finalising mame_options object...");
	
	if (opts->priv->options_file)
		g_key_file_free (opts->priv->options_file);
	g_free (opts->priv->filename);
	
	/* FIXME TODO Free the string options */
	
	g_free (opts->priv);
GMAMEUI_DEBUG ("Finalising mame_options object... done");
}

