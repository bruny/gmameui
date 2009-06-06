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
#include <gtk/gtk.h>

#include "mame_options_legacy.h"
#include "io.h"
#include "gui.h"	/* For main_gui.exec_list */

static void mame_options_legacy_class_init (MameOptionsLegacyClass *klass);
static void mame_options_legacy_init (MameOptionsLegacy *opt);
static void mame_options_legacy_finalize (GObject *obj);
static void mame_options_legacy_set_property (GObject *object,
					 guint prop_id,
					 const GValue *value,
					 GParamSpec *pspec);
static void mame_options_legacy_get_property (GObject *object,
					 guint prop_id,
					 GValue *value,
					 GParamSpec *pspec);

G_DEFINE_TYPE (MameOptionsLegacy, mame_options_legacy, G_TYPE_OBJECT)

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
} _MameSupportedOptionsLegacyInt;

typedef struct
{
	gint id;
	gchar *category;
	gchar *key;
	gboolean def_value;
	GValue value;
	
	/* Initialised in class init */
	GType type;
} _MameSupportedOptionsLegacyBool;

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
} _MameSupportedOptionsLegacyDbl;

typedef struct
{
	gint id;
	gchar *category;
	gchar *key;
	gchar* def_value;
	GValue value;
	
	/* Initialised in class init */
	GType type;
} _MameSupportedOptionsLegacyString;

enum {
	/* Video values */
	LEGACY_OPTIONS_0,
	LEGACY_OPTION_DIRTY,
	LEGACY_OPTION_FULLSCREEN,
	LEGACY_OPTION_ARBHEIGHT,
	LEGACY_OPTION_AUTODOUBLE,
	LEGACY_OPTION_THROTTLE,
	LEGACY_OPTION_SLEEPIDLE,
	LEGACY_OPTION_FRAMESTORUN,
	LEGACY_OPTION_AUTOFRAMESKIP,
	LEGACY_OPTION_MAXAUTOFRAMESKIP,
	LEGACY_OPTION_BRIGHTNESS,
	LEGACY_OPTION_PAUSE_BRIGHTNESS,
	LEGACY_OPTION_EFFECT,
	LEGACY_OPTION_SCANLINES,
	LEGACY_OPTION_GAMMA,
	LEGACY_OPTION_NOROTATE,
	LEGACY_OPTION_ROR,
	LEGACY_OPTION_ROL,
	LEGACY_OPTION_AUTOROR,
	LEGACY_OPTION_AUTOROL,
	LEGACY_OPTION_FLIPX,
	LEGACY_OPTION_FLIPY,

	LEGACY_OPTION_HEIGHTSCALE,
	LEGACY_OPTION_WIDTHSCALE,
	LEGACY_OPTION_BPP,
	
	/* Artwork */
	LEGACY_OPTION_ARTWORK,
	LEGACY_OPTION_ARTWORKRES,
	LEGACY_OPTION_USE_BACKDROPS,
	LEGACY_OPTION_BACKDROP,
	LEGACY_OPTION_USE_OVERLAYS,
	LEGACY_OPTION_USE_BEZELS,
	LEGACY_OPTION_ARTWORK_CROP,
	LEGACY_OPTION_ARTCROP,
	
	/* Vector */
	LEGACY_OPTION_VECTORRES,
	LEGACY_OPTION_BEAM,
	LEGACY_OPTION_FLICKER,
	LEGACY_OPTION_INTENSITY,
	LEGACY_OPTION_ANTIALIAS,
	LEGACY_OPTION_TRANSLUCENCY,
	
	/* OpenGL options */
	LEGACY_OPTION_GLDOUBLEBUF,
	LEGACY_OPTION_GLTEXTURESIZE,
	LEGACY_OPTION_GLBILINEAR,
	LEGACY_OPTION_GLBEAM,
	LEGACY_OPTION_GLANTIALIAS,
	LEGACY_OPTION_GLANTIALIASVEC,
	LEGACY_OPTION_GLCABVIEW,
	
	/* Sound options */
	LEGACY_OPTION_SOUND_SOUND,
	LEGACY_OPTION_SOUND_SAMPLES,
	LEGACY_OPTION_SOUND_SAMPLERATE,
	LEGACY_OPTION_SOUND_FAKESOUND,
	LEGACY_OPTION_SOUND_BUFSIZE,
	LEGACY_OPTION_SOUND_VOLUME,
	LEGACY_OPTION_SOUND_TIMER,
	LEGACY_OPTION_SOUND_AUDIODEVICE,
	LEGACY_OPTION_SOUND_MIXERDEVICE,
	LEGACY_OPTION_SOUND_DSPPLUGIN,
	LEGACY_OPTION_SOUND_MIXERPLUGIN,
	
	/* X11 Input options */
	LEGACY_OPTION_X11INPUT_GRABMOUSE,
	LEGACY_OPTION_X11INPUT_GRABKEYBOARD,
	LEGACY_OPTION_X11INPUT_ALWAYSUSEMOUSE,
	LEGACY_OPTION_X11INPUT_CURSOR,
	LEGACY_OPTION_X11INPUT_WINKEYS,
	
	/* Input options */
	LEGACY_OPTION_INPUT_JOYTYPE,
	LEGACY_OPTION_INPUT_ANALOGSTICK,
	LEGACY_OPTION_INPUT_UGCICOIN,
	LEGACY_OPTION_INPUT_STEADYKEY,
	LEGACY_OPTION_INPUT_USBPSPAD,
	LEGACY_OPTION_INPUT_RAPIDFIRE,
	
	/* File I/O */
	LEGACY_OPTION_FILE_STDERR,
	LEGACY_OPTION_FILE_STDOUT,
	LEGACY_OPTION_FILE_LOG,
	
	/* MAME options */
	LEGACY_OPTION_MAME_CHEAT,
	LEGACY_OPTION_MAME_SKIPDISCLAIMER,
	LEGACY_OPTION_MAME_SKIPGAMEINFO,
	LEGACY_OPTION_MAME_BIOS,
	LEGACY_OPTION_MAME_AUTOSAVE,
	LEGACY_OPTION_MAME_KEYBOARDLEDS
};

/* Integer options */
static _MameSupportedOptionsLegacyInt MameSupportedOptionsLegacyInt[] =
{
	{ LEGACY_OPTION_ARBHEIGHT, "Video", "arbheight", 1, 0, 4096, { 0, } },    /* AKA ah */
	{ LEGACY_OPTION_HEIGHTSCALE, "Video", "heightscale", 1, 1, 8, { 0, } },    /* AKA hs */
	{ LEGACY_OPTION_WIDTHSCALE, "Video", "widthscale", 1, 1, 8, { 0, } },    /* AKA ws */
	{ LEGACY_OPTION_MAXAUTOFRAMESKIP, "Video", "maxautoframeskip", 1, 1, 12, { 0, } },    /* AKA mafs */
	{ LEGACY_OPTION_GLTEXTURESIZE, "OpenGL", "gltexture_size", 0, 0, 2048, { 0, } },
};

/* Boolean options. These options are prefixed by 'no' if the value is set off */
static _MameSupportedOptionsLegacyBool MameSupportedOptionsLegacyBool[] =
{
	{ LEGACY_OPTION_DIRTY, "Video", "dirty", TRUE, { 0, } },
	{ LEGACY_OPTION_FULLSCREEN, "Video", "fullscreen", TRUE, { 0, } },
	{ LEGACY_OPTION_AUTODOUBLE, "Video", "autodouble", TRUE, { 0, } },     /* AKA adb */
	{ LEGACY_OPTION_THROTTLE, "Video", "throttle", TRUE, { 0, } },     /* AKA th */
	{ LEGACY_OPTION_SLEEPIDLE, "Video", "sleepidle", TRUE, { 0, } },     /* AKA si */
	{ LEGACY_OPTION_SCANLINES, "Video", "scanlines", TRUE, { 0, } },
	{ LEGACY_OPTION_AUTOFRAMESKIP, "Video", "autoframeskip", TRUE, { 0, } },     /* AKA afs */
	{ LEGACY_OPTION_NOROTATE, "Video", "norotate", TRUE, { 0, } },     /* AKA nr */
	{ LEGACY_OPTION_ROR, "Video", "ror", FALSE, { 0, } },     /* AKA ror */
	{ LEGACY_OPTION_ROL, "Video", "rol", FALSE, { 0, } },     /* AKA rol */
	{ LEGACY_OPTION_AUTOROR, "Video", "autoror", TRUE, { 0, } },
	{ LEGACY_OPTION_AUTOROL, "Video", "autorol", TRUE, { 0, } },
	{ LEGACY_OPTION_FLIPX, "Video", "flipx", TRUE, { 0, } },  /* AKA fx */
	{ LEGACY_OPTION_FLIPY, "Video", "flipy", TRUE, { 0, } },  /* AKA fy */
	
	/* Artwork */
	{ LEGACY_OPTION_ARTWORK, "Artwork", "artwork", TRUE, { 0, } },  /* AKA art */
	{ LEGACY_OPTION_USE_BACKDROPS, "Artwork", "use_backdrops", TRUE, { 0, } },
	{ LEGACY_OPTION_BACKDROP, "Artwork", "backdrop", TRUE, { 0, } },
	{ LEGACY_OPTION_USE_OVERLAYS, "Artwork", "use_overlays", TRUE, { 0, } },
	{ LEGACY_OPTION_USE_BEZELS, "Artwork", "use_bezels", TRUE, { 0, } },      /* AKA bezel */
	{ LEGACY_OPTION_ARTWORK_CROP, "Artwork", "artwork_crop", TRUE, { 0, } },
	{ LEGACY_OPTION_ARTCROP, "Artwork", "artcrop", TRUE, { 0, } },
	
	/* Vector */
	{ LEGACY_OPTION_ANTIALIAS, "Vector", "antialias", TRUE, { 0, } },    /* AKA aa */
	{ LEGACY_OPTION_TRANSLUCENCY, "Vector", "translucency", TRUE, { 0, } },    /* AKA t */
	
	/* OpenGL */
	{ LEGACY_OPTION_GLDOUBLEBUF, "OpenGL", "gldblbuffer", TRUE, { 0, } },
	{ LEGACY_OPTION_GLBILINEAR, "OpenGL", "glbilinear", TRUE, { 0, } },      /* AKA glbilin */
	{ LEGACY_OPTION_GLANTIALIAS, "OpenGL", "glantialias", TRUE, { 0, } },      /* AKA glaa */
	{ LEGACY_OPTION_GLANTIALIASVEC, "OpenGL", "glantialiasvec", FALSE, { 0, } },      /* AKA glaav */
	{ LEGACY_OPTION_GLCABVIEW, "OpenGL", "cabview", FALSE, { 0, } },

	/* X11 Input */
	{ LEGACY_OPTION_X11INPUT_GRABMOUSE, "X11Input", "grabmouse", TRUE, { 0, } },
	{ LEGACY_OPTION_X11INPUT_GRABKEYBOARD, "X11Input", "grabkeyboard", TRUE, { 0, } },
	{ LEGACY_OPTION_X11INPUT_ALWAYSUSEMOUSE, "X11Input", "alwaysusemouse", TRUE, { 0, } },
	{ LEGACY_OPTION_X11INPUT_CURSOR, "X11Input", "cursor", TRUE, { 0, } },
	{ LEGACY_OPTION_X11INPUT_WINKEYS, "X11Input", "winkeys", TRUE, { 0, } },
	
	/* Input device */
	{ LEGACY_OPTION_INPUT_ANALOGSTICK, "Input", "analogstick", TRUE, { 0, } },
	{ LEGACY_OPTION_INPUT_UGCICOIN, "Input", "ugcicoin", TRUE, { 0, } },
	{ LEGACY_OPTION_INPUT_STEADYKEY, "Input", "steadykey", TRUE, { 0, } },
	{ LEGACY_OPTION_INPUT_USBPSPAD, "Input", "usbpspad", TRUE, { 0, } },
	{ LEGACY_OPTION_INPUT_RAPIDFIRE, "Input", "rapidfire", TRUE, { 0, } },
	
	/* Sound */
	{ LEGACY_OPTION_SOUND_SOUND, "Sound", "sound", TRUE, { 0, } },  /* AKA ti */
	{ LEGACY_OPTION_SOUND_SAMPLES, "Sound", "samples", TRUE, { 0, } },  /* AKA sam */
	{ LEGACY_OPTION_SOUND_FAKESOUND, "Sound", "fakesound", FALSE, { 0, } },  /* AKA ti */
	{ LEGACY_OPTION_SOUND_TIMER, "Sound", "timer", TRUE, { 0, } },  /* AKA ti */
	
	/* File I/O */
	{ LEGACY_OPTION_FILE_LOG, "File", "log", FALSE, { 0, } },
	
	/* MAME */
	{ LEGACY_OPTION_MAME_CHEAT, "MAME", "cheat", FALSE, { 0, } },
	{ LEGACY_OPTION_MAME_SKIPDISCLAIMER, "MAME", "skip_disclaimer", FALSE, { 0, } },
	{ LEGACY_OPTION_MAME_SKIPGAMEINFO, "MAME", "skip_gameinfo", FALSE, { 0, } },
	{ LEGACY_OPTION_MAME_AUTOSAVE, "MAME", "autosave", FALSE, { 0, } },
	{ LEGACY_OPTION_MAME_KEYBOARDLEDS, "MAME", "keyboard_leds", FALSE, { 0, } },
		
};

/* Double options */
static _MameSupportedOptionsLegacyDbl MameSupportedOptionsLegacyDbl[] =
{
	{ LEGACY_OPTION_BRIGHTNESS, "Video", "brightness", 1, 0.5, 2.0, { 0, } },    /* AKA brt */
	{ LEGACY_OPTION_PAUSE_BRIGHTNESS, "Video", "pause_brightness", 1, 0.5, 2.0, { 0, } },    /* AKA pbrt */
	{ LEGACY_OPTION_GAMMA, "Video", "gamma", 1, 0.5, 2.0, { 0, } },    /* AKA gc */
	{ LEGACY_OPTION_BEAM, "Vector", "beam", 1, 1, 16, { 0, } },    /* AKA B */
	{ LEGACY_OPTION_FLICKER, "Vector", "flicker", 0, 0, 1, { 0, } },    /* AKA f */
	{ LEGACY_OPTION_INTENSITY, "Vector", "intensity", 1, 0, 1, { 0, } },
	{ LEGACY_OPTION_SOUND_VOLUME, "Sound", "volume", 0, -32, 0, { 0, } },       /* AKA v */
	{ LEGACY_OPTION_SOUND_BUFSIZE, "Sound", "bufsize", 1, 0, 5, { 0, } },   /* AKA bs */
	
	{ LEGACY_OPTION_GLBEAM, "OpenGL", "glbeam", 1, 0, 100, { 0, } },
};

/* FIXME TODO Set default values for ALL options */

/* String options */
/* Note that options intended to be displayed in ComboBoxes (e.g. effect) should be strings */
static _MameSupportedOptionsLegacyString MameSupportedOptionsLegacyString[] =
{
	{ LEGACY_OPTION_VECTORRES, "Vector", "vectorres", "640x480", { 0, } },
	{ LEGACY_OPTION_SOUND_AUDIODEVICE, "Sound", "audiodevice", "", { 0, } },
	{ LEGACY_OPTION_SOUND_MIXERDEVICE, "Sound", "mixerdevice", "", { 0, } },
	{ LEGACY_OPTION_SOUND_DSPPLUGIN, "Sound", "dsp-plugin", "", { 0, } },
	{ LEGACY_OPTION_SOUND_MIXERPLUGIN, "Sound", "sound-mixer-plugin", "", { 0, } },
	{ LEGACY_OPTION_FILE_STDERR, "File", "stderr-file", "stderr.log", { 0, } },
	{ LEGACY_OPTION_FILE_STDOUT, "File", "stdout-file", "stdout.log", { 0, } },
	/* The following are combobox entries so are marked as strings */
	{ LEGACY_OPTION_EFFECT, "Video", "effect", "0", { 0, } },    /* AKA ef */
	{ LEGACY_OPTION_AUTOFRAMESKIP, "Video", "frameskip", "0", { 0, } },    /* AKA fs */
	{ LEGACY_OPTION_EFFECT, "Video", "bpp", "0", { 0, } },
	{ LEGACY_OPTION_ARTWORKRES, "Artwork", "artwork_resolution", "0", { 0, } },
	{ LEGACY_OPTION_MAME_BIOS, "MAME", "bios", "0", { 0, } },
	{ LEGACY_OPTION_INPUT_JOYTYPE, "Input", "joytype", "0", { 0, } },
	{ LEGACY_OPTION_SOUND_SAMPLERATE, "Sound", "samplefreq", "44100", { 0, } },

};

struct _MameOptionsLegacyPriv
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
MameOptionsLegacy *
mame_options_legacy_new (void)
{
	/* TODO Pass in object RomEntry - if null, then its default options */
	MameOptionsLegacy *pr;
	
	pr = g_object_new (MAME_TYPE_OPTIONS_LEGACY, NULL);
	
	return pr;
	
}

gchar *
mame_options_legacy_get_option_string (MameOptionsLegacy *opts, gchar *category)
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
	
	for (i = 0; i < G_N_ELEMENTS (MameSupportedOptionsLegacyBool); i++) {
		gboolean val;
		
		/* Only use the command if it is supported */
		if ((g_ascii_strcasecmp (category, MameSupportedOptionsLegacyBool[i].category) == 0) && 
		    (mame_has_option (exec, MameSupportedOptionsLegacyBool[i].key))) {	
			catkey = g_strdup_printf("%s-%s",
						 MameSupportedOptionsLegacyBool[i].category,
						 MameSupportedOptionsLegacyBool[i].key);
			val = mame_legacy_options_get_bool (opts, catkey);
		
			if (val) {
				command = g_strconcat (command,
						       " -",
						       MameSupportedOptionsLegacyBool[i].key,
						       NULL);
			} else {
				/* Some toggle options are disabled using 'no' as a
				   prefix (e.g. -nolog) but we need to check whether MAME
				   recognises them first */
				if (mame_option_supports_no_prefix (exec, MameSupportedOptionsLegacyBool[i].key)) {
					command = g_strconcat (command,
							       " -no",
							       MameSupportedOptionsLegacyBool[i].key,
							       NULL);
				} /* else we simply omit it */

			}
		
			g_free (catkey);
		}
	}
	
	for (i = 0; i < G_N_ELEMENTS (MameSupportedOptionsLegacyInt); i++) {
		gint val;
		gchar *str;

		/* Only use the command if it is supported */
		if ((g_ascii_strcasecmp (category, MameSupportedOptionsLegacyInt[i].category) == 0) && 
		    (mame_has_option (exec, MameSupportedOptionsLegacyInt[i].key))) {	
			catkey = g_strdup_printf("%s-%s",
						 MameSupportedOptionsLegacyInt[i].category,
						 MameSupportedOptionsLegacyInt[i].key);
			val = mame_legacy_options_get_int (opts, catkey);
			str = g_strdup_printf ("%d", val);
		
			command = g_strconcat (command,
					       " -",
					       MameSupportedOptionsLegacyInt[i].key,
					       " ",
					       str,
					       NULL);
			g_free (catkey);
			g_free (str);
		}
	}
	
	for (i = 0; i < G_N_ELEMENTS (MameSupportedOptionsLegacyDbl); i++) {
		gdouble val;
		gchar *str;
		
		/* Only use the command if it is supported */
		if ((g_ascii_strcasecmp (category, MameSupportedOptionsLegacyDbl[i].category) == 0) && 
		    (mame_has_option (exec, MameSupportedOptionsLegacyDbl[i].key))) {
			catkey = g_strdup_printf("%s-%s",
						 MameSupportedOptionsLegacyDbl[i].category,
						 MameSupportedOptionsLegacyDbl[i].key);
			val = mame_legacy_options_get_dbl (opts, catkey);
			str = g_strdup_printf ("%.2f", val);

			command = g_strconcat (command,
					       " -",
					       MameSupportedOptionsLegacyDbl[i].key,
					       " ",
					       str,
					       NULL);
			g_free (catkey);
			g_free (str);
		}
	}

	for (i = 0; i < G_N_ELEMENTS (MameSupportedOptionsLegacyString); i++) {
		gchar* val;
		/* Only use the command if it is supported */
		if ((g_ascii_strcasecmp (category, MameSupportedOptionsLegacyString[i].category) == 0) && 
		    (mame_has_option (exec, MameSupportedOptionsLegacyString[i].key))) {
			catkey = g_strdup_printf("%s-%s",
						 MameSupportedOptionsLegacyString[i].category,
						 MameSupportedOptionsLegacyString[i].key);
			val = mame_legacy_options_get (opts, catkey);
		
			/* FIXME TODO Don't append string if 0 length */
			command = g_strconcat (command,
					       " -",
					       MameSupportedOptionsLegacyString[i].key,
					       " ",
					       val,
					       NULL);
			g_free (catkey);
			g_free (val);
		}
	}
	
	return command;
}

/* The following mame_options_legacy_save_xxx functions are triggered when the
   MameOptionsLegacy object properties are changed as part of the notify- signal */
static void
mame_options_legacy_save_string (MameOptionsLegacy *opts, GParamSpec *param, gpointer user_data)
{
	GMAMEUI_DEBUG ("mame_options_save_string: %s", param->name);
	/* param->name will be of the form Category-Key, e.g. Video-beam */
	
//	gchar** stv;
//	GValue *dbl_value;
	_MameSupportedOptionsLegacyString *opt;
	gchar *value;

	g_return_if_fail (MAME_IS_OPTIONS_LEGACY (opts));
	g_return_if_fail (param->name != NULL);
	
	opt = (_MameSupportedOptionsLegacyString *) user_data;
	g_return_if_fail (opt != NULL);
	
	value = g_value_get_string (&opt->value);
	
//	stv = g_strsplit (param->name, "-", 2); /* Split only on the first '-', so keys with a '-' are ignored */
	g_key_file_set_string (opts->priv->options_file, opt->category, opt->key, value);
//	g_strfreev (stv);
	
	g_key_file_save_to_file (opts->priv->options_file, opts->priv->filename, NULL);
}

static void
mame_options_legacy_save_double (MameOptionsLegacy *opts, GParamSpec *param, gpointer user_data)
{
	GMAMEUI_DEBUG ("mame_options_save_double: %s", param->name);
	
	/* param->name will be of the form Category-Key, e.g. Video-beam */
	
//	gchar** stv;
//	GValue *dbl_value;
	_MameSupportedOptionsLegacyDbl *opt;
	gdouble value;

	g_return_if_fail (MAME_IS_OPTIONS_LEGACY (opts));
	g_return_if_fail (param->name != NULL);
	
	opt = (_MameSupportedOptionsLegacyDbl *) user_data;
	g_return_if_fail (opt != NULL);
	
	value = g_value_get_double (&opt->value);
	
//	stv = g_strsplit (param->name, "-", 2); /* Split only on the first '-', so keys with a '-' are ignored */
	g_key_file_set_double (opts->priv->options_file, opt->category, opt->key, value);
//	g_strfreev (stv);
	
	g_key_file_save_to_file (opts->priv->options_file, opts->priv->filename, NULL);
}

static void
mame_options_legacy_save_int (MameOptionsLegacy *opts, GParamSpec *param, gpointer user_data)
{
	GMAMEUI_DEBUG ("mame_options_save_int: %s", param->name);
	
	/* param->name will be of the form Category-Key, e.g. Video-beam */
	
//	gchar** stv;
//	GValue *int_value;
	_MameSupportedOptionsLegacyInt *opt;
	gint value;

	g_return_if_fail (MAME_IS_OPTIONS_LEGACY (opts));
	g_return_if_fail (param->name != NULL);
	
	opt = (_MameSupportedOptionsLegacyInt *) user_data;
	g_return_if_fail (opt != NULL);
	
	value = g_value_get_int (&opt->value);

//	stv = g_strsplit (param->name, "-", 2); /* Split only on the first '-', so keys with a '-' are ignored */
	g_key_file_set_integer (opts->priv->options_file, opt->category, opt->key, value);
//	g_strfreev (stv);
	
	g_key_file_save_to_file (opts->priv->options_file, opts->priv->filename, NULL);
}

static void
mame_options_legacy_save_bool (MameOptionsLegacy *opts, GParamSpec *param, gpointer user_data)
{
	GMAMEUI_DEBUG ("mame_options_save_bool: %s", param->name);

	/* param->name will be of the form Category-Key, e.g. Video-beam */
	
//	gchar** stv;
	_MameSupportedOptionsLegacyBool *opt;
//	GValue *bool_value;
	gboolean value;

	g_return_if_fail (MAME_IS_OPTIONS_LEGACY (opts));
	g_return_if_fail (param->name != NULL);
	
	opt = (_MameSupportedOptionsLegacyBool *) user_data;
	g_return_if_fail (opt != NULL);
	
	value = g_value_get_boolean (&opt->value);
	
//	stv = g_strsplit (opt->key, "-", 2); /* Split only on the first '-', so keys with a '-' are ignored */
	g_key_file_set_boolean (opts->priv->options_file, opt->category, opt->key, value);
//	g_strfreev (stv);
	
	g_key_file_save_to_file (opts->priv->options_file, opts->priv->filename, NULL);
}
/*DELETE
static void
mame_options_legacy_save_bool_old (MameOptionsLegacy *opts, GParamSpec *param, gpointer user_data)
{
	GMAMEUI_DEBUG ("mame_options_save_bool: %s", param->name);

	* param->name will be of the form Category-Key, e.g. Video-beam *
	
	gchar** stv;
	GValue *bool_value;
	gboolean value;

	g_return_if_fail (MAME_IS_OPTIONS_LEGACY (opts));
	g_return_if_fail (param->name != NULL);
	
	bool_value = (GValue *) user_data;
	g_return_if_fail (bool_value != NULL);
	
	value = g_value_get_boolean (bool_value);
	
	stv = g_strsplit (param->name, "-", 2); * Split only on the first '-', so keys with a '-' are ignored *
	g_key_file_set_boolean (opts->priv->options_file, stv[0], stv[1], value);
	g_strfreev (stv);
	
	g_key_file_save_to_file (opts->priv->options_file, opts->priv->filename, NULL);
}*/

/* Gets the value from the keyfile for the specified key, which should be of the
   format category.key, i.e. Sound-volume */
gchar *
mame_legacy_options_get (MameOptionsLegacy *opts, const gchar *key)
{
	gchar *ret_val;
	gchar **stv;
	GError *error = NULL;
	
	g_return_val_if_fail (MAME_IS_OPTIONS_LEGACY (opts), NULL);
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
mame_legacy_options_set_int (MameOptionsLegacy *opts, gchar *key, gint value)
{
	gchar *prop_name;
	
	prop_name = g_strdup (key);
	prop_name = g_strcanon (prop_name, G_CSET_A_2_Z G_CSET_a_2_z G_CSET_DIGITS "-", '-');
	g_object_set (opts, prop_name, value, NULL);
	g_free (prop_name);
}

void
mame_legacy_options_set_dbl (MameOptionsLegacy *opts, gchar *key, gdouble value)
{
	gchar *prop_name;
	
	prop_name = g_strdup (key);
	prop_name = g_strcanon (prop_name, G_CSET_A_2_Z G_CSET_a_2_z G_CSET_DIGITS "-", '-');
	g_object_set (opts, prop_name, value, NULL);
	g_free (prop_name);
}

void
mame_legacy_options_set_string (MameOptionsLegacy *opts, gchar *key, gchar* value)
{
	gchar *prop_name;
	
	prop_name = g_strdup (key);
	prop_name = g_strcanon (prop_name, G_CSET_A_2_Z G_CSET_a_2_z G_CSET_DIGITS "-", '-');
	g_object_set (opts, prop_name, value, NULL);
	g_free (prop_name);
}

/* Key should be passed in in the form category-key, e.g. Video-artwork_crop */
gint
mame_legacy_options_get_int (MameOptionsLegacy *opts, const gchar *key)
{
	gint ret_val;
	gchar** stv;
	GError *error = NULL;

	g_return_val_if_fail (MAME_IS_OPTIONS_LEGACY (opts), 0);
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
mame_legacy_options_get_dbl (MameOptionsLegacy *opts, const gchar *key)
{
	gdouble ret_val;
	gchar** stv;
	GError *error = NULL;

	g_return_val_if_fail (MAME_IS_OPTIONS_LEGACY (opts), 0);
	g_return_val_if_fail (key != NULL, 0);
	g_return_val_if_fail (opts->priv->options_file != NULL, 0);

	ret_val = 0;
	stv = g_strsplit (key, "-", 2);
	ret_val = g_key_file_get_integer (opts->priv->options_file, stv[0], stv[1], &error);

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
mame_legacy_options_get_bool (MameOptionsLegacy *opts, const gchar *key)
{
	gboolean ret_val;
	gchar** stv;
	GError *error = NULL;

	g_return_val_if_fail (MAME_IS_OPTIONS_LEGACY (opts), 0);
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

	for (i = 0; i < G_N_ELEMENTS (MameSupportedOptionsLegacyBool); i++) {
		gchar *tmp_prop_name;
		tmp_prop_name = g_strdup (MameSupportedOptionsLegacyBool[i].key);
		tmp_prop_name = g_strcanon (tmp_prop_name, G_CSET_A_2_Z G_CSET_a_2_z G_CSET_DIGITS "-", '-');
		
		if (g_ascii_strcasecmp (tmp_prop_name, tokens[1]) == 0) {
			found = i;
			g_free (tmp_prop_name);
			break;
		}
		g_free (tmp_prop_name);
	}
	
	g_strfreev (tokens);
	
	if (i == G_N_ELEMENTS (MameSupportedOptionsLegacyBool))
		GMAMEUI_DEBUG ("WARNING - offset for boolean prop %s not found", prop_name);
	
	return found;
}

static guint
find_str_offset (gchar *prop_name)
{
	guint i, found;
	gchar **tokens;
	tokens = g_strsplit (prop_name, "-", 2);

	for (i = 0; i < G_N_ELEMENTS (MameSupportedOptionsLegacyString); i++) {
		gchar *tmp_prop_name;
		tmp_prop_name = g_strdup (MameSupportedOptionsLegacyString[i].key);
		tmp_prop_name = g_strcanon (tmp_prop_name, G_CSET_A_2_Z G_CSET_a_2_z G_CSET_DIGITS "-", '-');
			
		if (g_ascii_strcasecmp (tmp_prop_name, tokens[1]) == 0) {
			found = i;
			g_free (tmp_prop_name);
			break;
		}
		g_free (tmp_prop_name);
	}
	
	g_strfreev (tokens);

	if (i == G_N_ELEMENTS (MameSupportedOptionsLegacyString))
		GMAMEUI_DEBUG ("WARNING - offset for string prop %s not found", prop_name);
	
	return found;
}

static guint
find_int_offset (gchar *prop_name)
{
	guint i, found;
	gchar **tokens;
	tokens = g_strsplit (prop_name, "-", 2);

	for (i = 0; i < G_N_ELEMENTS (MameSupportedOptionsLegacyInt); i++) {
		gchar *tmp_prop_name;
		tmp_prop_name = g_strdup (MameSupportedOptionsLegacyInt[i].key);
		tmp_prop_name = g_strcanon (tmp_prop_name, G_CSET_A_2_Z G_CSET_a_2_z G_CSET_DIGITS "-", '-');
		
		if (g_ascii_strcasecmp (tmp_prop_name, tokens[1]) == 0) {
			found = i;
			g_free (tmp_prop_name);
			break;
		}
		g_free (tmp_prop_name);
	}
	
	g_strfreev (tokens);

	if (i == G_N_ELEMENTS (MameSupportedOptionsLegacyInt))
		GMAMEUI_DEBUG ("WARNING - offset for integer prop %s not found", prop_name);
	
	return found;
}

static int
find_dbl_offset (gchar *prop_name)
{
	guint i, found;
	gchar **tokens;
	tokens = g_strsplit (prop_name, "-", 2);

	for (i = 0; i < G_N_ELEMENTS (MameSupportedOptionsLegacyDbl); i++) {
		gchar *tmp_prop_name;
		tmp_prop_name = g_strdup (MameSupportedOptionsLegacyDbl[i].key);
		tmp_prop_name = g_strcanon (tmp_prop_name, G_CSET_A_2_Z G_CSET_a_2_z G_CSET_DIGITS "-", '-');
		
		if (g_ascii_strcasecmp (tmp_prop_name, tokens[1]) == 0) {
			found = i;
			g_free (tmp_prop_name);
			break;
		}
		g_free (tmp_prop_name);
	}
	
	g_strfreev (tokens);
	
	if (i == G_N_ELEMENTS (MameSupportedOptionsLegacyDbl))
		GMAMEUI_DEBUG ("WARNING - offset for double prop %s not found", prop_name);
	
	return found;
}

static void
mame_options_legacy_set_property (GObject *object,
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
			g_value_set_boolean (&MameSupportedOptionsLegacyBool[offset].value,
					     g_value_get_boolean (value));
			break;
		case G_TYPE_DOUBLE:
			offset = find_dbl_offset (pspec->name);
			GMAMEUI_DEBUG ("Attempting to set double prop %s at pos %d(%d) with value %.2f",
				       pspec->name, prop_id, offset, g_value_get_double (value));
			g_value_set_double (&MameSupportedOptionsLegacyDbl[offset].value,
					    g_value_get_double (value));
			break;
		case G_TYPE_INT:
			offset = find_int_offset (pspec->name);
			GMAMEUI_DEBUG ("Attempting to set integer prop %s at pos %d(%d) with value %d",
				       pspec->name, prop_id, offset, g_value_get_int (value));
			g_value_set_int (&MameSupportedOptionsLegacyInt[offset].value,
					 g_value_get_int (value));
			break;
		case G_TYPE_STRING:
			offset = find_str_offset (pspec->name);
			GMAMEUI_DEBUG ("Attempting to set string prop %s at pos %d(%d) with value %s",
				       pspec->name, prop_id, offset, g_value_get_string (value));
			g_value_set_string (&MameSupportedOptionsLegacyString[offset].value,
					    g_value_get_string (value));
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

static void
mame_options_legacy_get_property (GObject *object,
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
			g_value_set_boolean (value, g_value_get_boolean (&MameSupportedOptionsLegacyBool[offset].value));
			break;
		case G_TYPE_DOUBLE:
			offset = find_dbl_offset (pspec->name);
			GMAMEUI_DEBUG ("Attempting to get double prop %s at pos %d",
				       pspec->name, prop_id);
			g_value_set_double (value, g_value_get_double (&MameSupportedOptionsLegacyDbl[offset].value));
			break;
		case G_TYPE_INT:
			offset = find_int_offset (pspec->name);
			GMAMEUI_DEBUG ("Attempting to get integer prop %s at pos %d",
				       pspec->name, prop_id);
			g_value_set_int (value, g_value_get_int (&MameSupportedOptionsLegacyInt[offset].value));
			break;
		case G_TYPE_STRING:
			offset = find_str_offset (pspec->name);
			GMAMEUI_DEBUG ("Attempting to get string prop %s at pos %d",
				       pspec->name, prop_id);
			g_value_set_string (value, g_value_get_string (&MameSupportedOptionsLegacyString[offset].value));
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
mame_options_legacy_class_init (MameOptionsLegacyClass *klass)
{
	guint i;
	
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	
	object_class->set_property = mame_options_legacy_set_property;
	object_class->get_property = mame_options_legacy_get_property;
	object_class->finalize = mame_options_legacy_finalize;
GMAMEUI_DEBUG ("Creating MameOptionsLegacy class object...");
	/* Initialise the class properties - any MAME option which contains an
	   underscore needs to be have the underscore replaced by a hyphen; this
	   is to workaround issues with GLib where a property/signal name cannot
	   contain both. This affects the setting the property name, the notify
	   signal name and when finding the offset for get/set */
	
	/* Initialise boolean class properties */
	for (i = 0; i < G_N_ELEMENTS (MameSupportedOptionsLegacyBool); i++) {
		gchar *attr_name;
		
/*		attr_name = g_strdup_printf ("%s-%s",   * Name can't have . *
					     MameSupportedOptionsLegacyBool[i].category,
					     MameSupportedOptionsLegacyBool[i].key);
Issue: g_param_spec_internal indicates that mixing _ and - cannot be done; signals need - */
		attr_name = create_property_name (MameSupportedOptionsLegacyBool[i].category,
						  MameSupportedOptionsLegacyBool[i].key);
		GMAMEUI_DEBUG ("   Creating boolean class property %s with id %d",
			       attr_name,
			       MameSupportedOptionsLegacyBool[i].id);
		/* Initialise type */
		MameSupportedOptionsLegacyBool[i].type = G_TYPE_BOOLEAN;
		g_value_init (&MameSupportedOptionsLegacyBool[i].value, G_TYPE_BOOLEAN);
		
		/* Install the property */
		g_object_class_install_property (object_class,
					MameSupportedOptionsLegacyBool[i].id,
					g_param_spec_boolean (attr_name, "", "", MameSupportedOptionsLegacyBool[i].def_value, G_PARAM_READWRITE));
		
		g_free (attr_name);
	}
	
	/* Initialise double class properties */
	for (i = 0; i < G_N_ELEMENTS (MameSupportedOptionsLegacyDbl); i++) {
		gchar *attr_name;
		
/*		attr_name = g_strdup_printf ("%s-%s",   * Name can't have . *
					     MameSupportedOptionsLegacyDbl[i].category,
					     MameSupportedOptionsLegacyDbl[i].key);*/
		attr_name = create_property_name (MameSupportedOptionsLegacyDbl[i].category,
						  MameSupportedOptionsLegacyDbl[i].key);
		
		GMAMEUI_DEBUG ("   Creating double class property %s with id %d",
			       attr_name,
			       MameSupportedOptionsLegacyDbl[i].id);
		/* Initialise type */
		MameSupportedOptionsLegacyDbl[i].type = G_TYPE_DOUBLE;
		g_value_init (&MameSupportedOptionsLegacyDbl[i].value, G_TYPE_DOUBLE);
		
		/* Install the property */
		g_object_class_install_property (object_class,
					MameSupportedOptionsLegacyDbl[i].id,
					g_param_spec_double (attr_name, "", "", MameSupportedOptionsLegacyDbl[i].lower, MameSupportedOptionsLegacyDbl[i].upper, MameSupportedOptionsLegacyDbl[i].def_value, G_PARAM_READWRITE));
		
		g_free (attr_name);
	}
	
	/* Initialise integer class properties */
	for (i = 0; i < G_N_ELEMENTS (MameSupportedOptionsLegacyInt); i++) {
		gchar *attr_name;
		
/*		attr_name = g_strdup_printf ("%s-%s",   * Name can't have . *
					     MameSupportedOptionsLegacyInt[i].category,
					     MameSupportedOptionsLegacyInt[i].key);*/
		attr_name = create_property_name (MameSupportedOptionsLegacyInt[i].category,
						  MameSupportedOptionsLegacyInt[i].key);
		
		GMAMEUI_DEBUG ("   Creating integer class property %s with id %d",
			       attr_name,
			       MameSupportedOptionsLegacyInt[i].id);
		/* Initialise type */
		MameSupportedOptionsLegacyInt[i].type = G_TYPE_INT;
		g_value_init (&MameSupportedOptionsLegacyInt[i].value, G_TYPE_INT);
		
		/* Install the property */
		g_object_class_install_property (object_class,
					MameSupportedOptionsLegacyInt[i].id,
					g_param_spec_int (attr_name, "", "", MameSupportedOptionsLegacyInt[i].lower, MameSupportedOptionsLegacyInt[i].upper, MameSupportedOptionsLegacyInt[i].def_value, G_PARAM_READWRITE));
		
		g_free (attr_name);
	}
	
	/* Initialise string class properties */
	for (i = 0; i < G_N_ELEMENTS (MameSupportedOptionsLegacyString); i++) {
		gchar *attr_name;
		
		/*attr_name = g_strdup_printf ("%s-%s",   * Name can't have . *
					     MameSupportedOptionsLegacyString[i].category,
					     MameSupportedOptionsLegacyString[i].key);*/
		attr_name = create_property_name (MameSupportedOptionsLegacyString[i].category,
						  MameSupportedOptionsLegacyString[i].key);
		
		GMAMEUI_DEBUG ("   Creating string class property %s with id %d",
			       attr_name,
			       MameSupportedOptionsLegacyString[i].id);
		/* Initialise type */
		MameSupportedOptionsLegacyString[i].type = G_TYPE_STRING;
		g_value_init (&MameSupportedOptionsLegacyString[i].value, G_TYPE_STRING);
		
		/* Install the property */
		g_object_class_install_property (object_class,
					MameSupportedOptionsLegacyString[i].id,
					g_param_spec_string (attr_name, "", "", MameSupportedOptionsLegacyString[i].def_value, G_PARAM_READWRITE));
		
		g_free (attr_name);
	}
GMAMEUI_DEBUG ("Creating MameOptionsLegacy class object... done");
}

static void
mame_options_legacy_init (MameOptionsLegacy *opts)
{
	guint i;
	
	opts->priv = g_new0 (MameOptionsLegacyPriv, 1);
	
	/* This gets called from mame_options_new. From that function, also pass in a RomEntry
	   If RomEntry is NULL, get default options, otherwise get the options
	   for a specific ROM */
	opts->priv->filename = g_build_filename (g_get_home_dir (),
					       ".gmameui", "options", "default_legacy.ini", NULL);
	opts->priv->options_file = g_key_file_new ();
	GError *error = NULL;
	g_key_file_load_from_file (opts->priv->options_file,
				   opts->priv->filename,
				   G_KEY_FILE_KEEP_COMMENTS,
				   &error);

	if (error) {
		GMAMEUI_DEBUG ("legacy options file could not be loaded: %s", error->message);
		g_error_free (error);
		error = NULL;
	}
	
	/* Set handlers so that whenever the values are changed (from anywhere), the signal handler
	   is invoked; the callback then saves to the g_key_file */
	GMAMEUI_DEBUG ("Setting up notify signal handlers for MameOptionsLegacy boolean properties...");
	for (i = 0; i < G_N_ELEMENTS (MameSupportedOptionsLegacyBool); i++) {
		gchar *signal_name;
		gchar *prop_name;
		
		prop_name = create_property_name (MameSupportedOptionsLegacyBool[i].category,
						  MameSupportedOptionsLegacyBool[i].key);
		/*signal_name = g_strdup_printf ("notify::%s-%s",
					       MameSupportedOptionsLegacyBool[i].category,
					       MameSupportedOptionsLegacyBool[i].key);*/
		signal_name = g_strdup_printf ("notify::%s", prop_name);       
		GMAMEUI_DEBUG ("  Setting boolean signal handler %s", signal_name);

		//g_signal_connect (opts, signal_name, (GCallback) mame_options_legacy_save_bool, &MameSupportedOptionsLegacyBool[i].value);
		g_signal_connect (opts, signal_name, (GCallback) mame_options_legacy_save_bool, &MameSupportedOptionsLegacyBool[i]);
	
		g_free (prop_name);
		g_free (signal_name);
	}
	GMAMEUI_DEBUG ("Setting up notify signal handlers for MameOptionsLegacy boolean properties... done");

	GMAMEUI_DEBUG ("Setting up notify signal handlers for MameOptionsLegacy double properties...");
	for (i = 0; i < G_N_ELEMENTS (MameSupportedOptionsLegacyDbl); i++) {
		gchar *signal_name;
		gchar *prop_name;
		
		prop_name = create_property_name (MameSupportedOptionsLegacyDbl[i].category,
						  MameSupportedOptionsLegacyDbl[i].key);
		
		/*signal_name = g_strdup_printf ("notify::%s-%s",
					       MameSupportedOptionsLegacyDbl[i].category,
					       MameSupportedOptionsLegacyDbl[i].key);*/
		signal_name = g_strdup_printf ("notify::%s", prop_name);
		GMAMEUI_DEBUG ("  Setting double signal handler %s", signal_name);

		g_signal_connect (opts, signal_name, (GCallback) mame_options_legacy_save_double, &MameSupportedOptionsLegacyDbl[i]);
	
		g_free (prop_name);
		g_free (signal_name);
	}
	GMAMEUI_DEBUG ("Setting up notify signal handlers for MameOptionsLegacy double properties... done");
	
	GMAMEUI_DEBUG ("Setting up notify signal handlers for MameOptionsLegacy integer properties...");
	for (i = 0; i < G_N_ELEMENTS (MameSupportedOptionsLegacyInt); i++) {
		gchar *signal_name;
		gchar *prop_name;
		
		prop_name = create_property_name (MameSupportedOptionsLegacyInt[i].category,
						  MameSupportedOptionsLegacyInt[i].key);
		/*signal_name = g_strdup_printf ("notify::%s-%s",
					       MameSupportedOptionsLegacyInt[i].category,
					       MameSupportedOptionsLegacyInt[i].key);*/
		signal_name = g_strdup_printf ("notify::%s", prop_name);
		GMAMEUI_DEBUG ("  Setting integer signal handler %s", signal_name);

		g_signal_connect (opts, signal_name, (GCallback) mame_options_legacy_save_int, &MameSupportedOptionsLegacyInt[i]);
		
		g_free (prop_name);
		g_free (signal_name);
	}
	GMAMEUI_DEBUG ("Setting up notify signal handlers for MameOptionsLegacy integer properties... done");
	
	GMAMEUI_DEBUG ("Setting up notify signal handlers for MameOptionsLegacy string properties...");
	for (i = 0; i < G_N_ELEMENTS (MameSupportedOptionsLegacyString); i++) {
		gchar *signal_name;
		gchar *prop_name;
		
		prop_name = create_property_name (MameSupportedOptionsLegacyString[i].category,
						  MameSupportedOptionsLegacyString[i].key);
		/*signal_name = g_strdup_printf ("notify::%s-%s",
					       MameSupportedOptionsLegacyString[i].category,
					       MameSupportedOptionsLegacyString[i].key);*/
		signal_name = g_strdup_printf ("notify::%s", prop_name);
		GMAMEUI_DEBUG ("  Setting string signal handler %s", signal_name);

		g_signal_connect (opts, signal_name, (GCallback) mame_options_legacy_save_string, &MameSupportedOptionsLegacyString[i]);
		
		g_free (prop_name);
		g_free (signal_name);
	}
	GMAMEUI_DEBUG ("Setting up notify signal handlers for MameOptionsLegacy string properties... done");
}

static void
mame_options_legacy_finalize (GObject *obj)
{
	MameOptionsLegacy *opts = MAME_OPTIONS_LEGACY (obj);
GMAMEUI_DEBUG ("Finalising mame_options_legacy object...");
	
	if (opts->priv->options_file)
		g_key_file_free (opts->priv->options_file);
	g_free (opts->priv->filename);
	
	/* FIXME TODO Free the string options */
	
	g_free (opts->priv);
GMAMEUI_DEBUG ("Finalising mame_options_legacy object... done");
}
