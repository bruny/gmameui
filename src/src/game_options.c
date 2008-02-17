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

#include "game_options.h"
#include <string.h>
#include <stdlib.h>

#include "io.h"
#include "gtkjoy.h"

void
game_options_free (GameOptions *opts)
{
	if (!opts || opts == &default_options)
		return;

	g_free (opts->additional_options);
	g_free (opts);
}

/** Loads the options for the given rom.
 * If rom is NULL then it loads the default options.
 * The options are loaded from: ~/gmameui/options/romname
 */
GameOptions *
load_options (RomEntry *rom)
{
	gchar *filename;
	GameOptions *target;
	GList *ctrlr_list = NULL;
	GList *my_list = NULL;

	if (rom == NULL) {
		target = &default_options;
		g_free (target->additional_options);
		
		filename = g_build_filename (g_get_home_dir (), ".gmameui", "options", "default", NULL);
	} else {
		filename = g_build_filename (g_get_home_dir (), ".gmameui", "options", rom->romname, NULL);
		/* if there is no file with that name, exit, will use default options */
		if (!g_file_test (filename, G_FILE_TEST_EXISTS)) {
			GMAMEUI_DEBUG ("option file for %s not found",rom->romname);
			g_free (filename);
			return NULL;
		}
		/* otherwise, create a xmame_game_options struct for this game */
		target = g_malloc0 (sizeof (GameOptions));
	}
/* FIXME TODO g_new0 for GameOptions - set defaults there */
	/* default options */

	/*** Video Related ***/
	target->fullscreen = TRUE;
	target->bpp = 0;
	target->arbheight = 0;
	target->heightscale = 1;
	target->widthscale = 1;
	/*	target->scale = 0.00000;*/
	target->effect = 0;
	//strcpy (target->effect_name, " ");
	target->effect_name = g_strdup (" ");
	target->autodouble = TRUE;		
	target->dirty = TRUE;
	target->scanlines = FALSE;
	target->artwork = TRUE;
	target->use_backdrops = TRUE;
	target->use_overlays = TRUE;
	target->use_bezels = TRUE;
	target->artwork_crop = FALSE;
	target->artwork_resolution = 0;
	target->frameskipper = 0;
	target->throttle = TRUE;
	target->sleepidle = FALSE;
	target->autoframeskip = TRUE;
	target->maxautoframeskip = 8;
	target->frameskip = 0;
	target->brightness = 1.00000;
	target->gamma_correction = 1.00000;
	target->norotate = FALSE;
	target->ror = FALSE;
	target->rol = FALSE;
	target->flipx = FALSE;
	target->flipy = FALSE;

	/*** Vector Games Related ***/
	target->vectorres_flag = FALSE;
	target->vectorres = g_strdup ("640x480");
	target->beam = 1.000000;
	target->flicker = 0.000000;
	target->antialias = TRUE;
	target->translucency = TRUE;
	target->intensity = 1.5;

	/*display_opts (depend target)*/
	/*** X11 Related ***/
	target->x11_mode = 0;
	target->cursor = TRUE;
	target->mitshm = TRUE;
	target->xvext = TRUE;
	target->vidix = FALSE;
	target->force_yuv = 0;
	target->xsync = FALSE;
	target->privatecmap = FALSE;
	target->xil = TRUE;
	target->mtxil = FALSE;
	target->run_in_root_window = FALSE;
	target->root_window_id = 0;
	target->xvgeom_flag = FALSE;
	target->geometry = g_strdup ("640x480");

	/*** X11-input related ***/
	/* (X11 & GL Glide) */
	target->grabmouse = FALSE;
	target->grabkeyboard = FALSE;
	target->winkeys = FALSE;
	target->mapkey = g_strdup (" ");


	/*** OpenGL Related ***/
	target->gldblbuffer = TRUE;
	target->gltexture_size = FALSE;
	target->glforceblitmode = FALSE;
	target->glext78 = TRUE;
	target->glbilinear = TRUE;
	target->gldrawbitmap = TRUE;
	target->gldrawbitmapvec = TRUE;
	target->glcolormod = TRUE;
	target->glbeam = 1.000000;
	target->glalphablending = TRUE;
	target->glantialias = TRUE;
	target->glantialiasvec = TRUE;
	target->gllibname = g_strdup ("libGL.so.1");
	target->glulibname = g_strdup ("libGLU.so.1");
	target->cabview = FALSE;
	target->cabinet = g_strdup ("glmamejau");
	target->glres_flag = FALSE;
	target->glres = g_strdup (" ");
				

	/*** SDL Related ***/
	target->sdl_doublebuf = FALSE;
	target->sdl_auto_mode = TRUE;
	target->modenumber = 0;
	target->sdlmapkey = g_strdup (" ");

	/*** GGI Related ***/
	target->ggilinear = FALSE;
	target->ggi_force_resolution = FALSE;
	target->xres = 640;
	target->yres = 480;

#ifdef ENABLE_XMAME_SVGALIB_SUPPORT
	/*** Svgalib Related ***/
	target->tweak = FALSE;
	target->planar = TRUE;
	target->linear = FALSE;
	target->centerx = 0;
	target->centery = 0;
#endif

	/*** FX (Glide) Related ***/
	target->resolution = g_strdup ("640x480");
	target->fxgkeepaspect = TRUE;

	/*** Photon Related ***/
	target->render_mode = 0;

	/*** Photon-window Related ***/
	target->phcursor = TRUE;
                 
	/*** Photon-input Related ***/
	/*	target->phgrabmouse = FALSE;
		target->phwinkeys = FALSE;
		strcpy (target->phmapkey, " ");
	*/

	/*** Video Mode Selection Related ***/
	/*** (SVGA, xf86_dga_opts, GGI) ***/
	target->keepaspect = 1;
	target->displayaspectratio = 1.330000;
	target->disablemode = g_strdup (" ");

	/*** Sound Related ***/
	target->sound = TRUE;
	target->samples = TRUE;
	target->fakesound = FALSE;
	target->samplefre = 22050;
	target->bufsize = 3.000000;
	target->volume = -3;
	target->audiodevice = g_strdup (" ");
	target->mixerdevice = g_strdup (" ");
	target->soundfile = g_strdup ("xmameout.wav");

	/*** Input Related ***/
	target->joytype = 0;
	target->analogstick = FALSE;
	target->mouse = FALSE;
	target->hotrod = FALSE;
	target->hotrodse = FALSE;
	target->usbpspad = FALSE;
	target->rapidfire = FALSE;
	target->ugcicoin = FALSE;
	
	target->keymap = g_strdup (" ");
	target->ctrlr = g_strdup ("None");
	target->joydevname = g_strdup (get_joy_dev ());
	target->joydevname[strlen (target->joydevname) - 1] = '\0';

	/***joy_pad_opts***  *FM townpad ***/
	target->paddevname = g_strdup ("/dev/pad00");
	/***joy_x11_opts***/
	target->x11joyname = g_strdup (" ");
	/***Xinput***/
	target->XInput_trackball1 = g_strdup (" ");
	target->XInput_trackball2 = g_strdup (" ");
	target->XInput_trackball3 = g_strdup (" ");
	target->XInput_trackball4 = g_strdup (" ");
	/* 	strcpy (target->XInput_joystick1, " "); */
	/* 	strcpy (target->XInput_joystick2, " "); */
	/* 	strcpy (target->XInput_joystick3, " "); */
	/* 	strcpy (target->XInput_joystick4, " "); */

	/*** Digital sound related ***/
	target->dsp_plugin = g_strdup (" ");
	/*list-dsp-plugins  */
	target->timer = FALSE;
				

	/*** Sound mixer related ***/
	target->sound_mixer_plugin = g_strdup (" ");
	/* list-mixer-plugins */

	/***arts***/
	target->artsBufferTime = 10;
		
	/*** Alsa Sound System 0.5***/
	/* list-alsa-cards */
	target->alsacard = 0;
	target->alsadevice = 0;
        	       
        /*** Alsa Sound System 0.9 ***/
	/* list-alsa-cards
	   list-alsa-pcm	*/
	target->alsa_pcm = g_strdup ("default");
	target->alsa_buffer = 250000;
        	       
        /*** QNX Audio related ***/
	target->audio_preferred = TRUE;
				

	/*** Display  Related ***/
	/*	strcpy (target->display_plugin, " ");*/
	/* list-display-plugins	*/
	/*	target->dwidthscale = 1;
		target->dheightscale = 1;
		target->dscanlines = FALSE;
		target->display_aspect_ratio = 1.33;
		target->keep_aspect = TRUE;
	*/
	/* Misc */
	target->cheat = 1;
	target->keyboard_leds = 1;
	target->debug = 0;
	target->debug_size = g_strdup ("640x480");
	target->log_flag = FALSE;
	target->log = g_strdup ("xmame.log");
	target->cfgname_flag = FALSE;
	target->cfgname = g_strdup ("");
	target->use_additional_options = FALSE;
	target->additional_options = g_strdup ("");
	
	target->skip_disclaimer = FALSE;
	target->skip_gameinfo = FALSE;
	target->bios = 0;
	
	GKeyFile *options_file = g_key_file_new ();
	GError *error = NULL;
	g_key_file_load_from_file (options_file, filename, G_KEY_FILE_KEEP_COMMENTS, &error);

	if (error) {
		GMAMEUI_DEBUG ("options file could not be loaded: %s", error->message);
		g_error_free (error);
		g_free (filename);
		return FALSE;
	}
	g_free (filename);

	target->fullscreen = g_key_file_get_integer (options_file, "Video", "fullscreen", &error);
	target->bpp = g_key_file_get_integer (options_file, "Video", "bpp", &error);
	target->arbheight = g_key_file_get_integer (options_file, "Video", "arbheight", &error);
	target->heightscale = g_key_file_get_integer (options_file, "Video", "heightscale", &error);
	target->widthscale = g_key_file_get_integer (options_file, "Video", "widthscale", &error);
	target->effect = g_key_file_get_integer (options_file, "Video", "effect", &error);
	target->effect_name = g_key_file_get_string (options_file, "Video", "effect_name", &error);
	target->autodouble = g_key_file_get_integer (options_file, "Video", "autodouble", &error);
	target->dirty = g_key_file_get_integer (options_file, "Video", "dirty", &error);
	target->scanlines = g_key_file_get_integer (options_file, "Video", "scanlines", &error);
	target->artwork = g_key_file_get_integer (options_file, "Video", "artwork", &error);
	target->use_backdrops = g_key_file_get_integer (options_file, "Video", "use_backdrops", &error);
	target->use_overlays = g_key_file_get_integer (options_file, "Video", "use_overlays", &error);
	target->use_bezels = g_key_file_get_integer (options_file, "Video", "use_bezels", &error);
	target->artwork_crop = g_key_file_get_integer (options_file, "Video", "artwork_crop", &error);
	target->artwork_resolution = g_key_file_get_integer (options_file, "Video", "artwork_resolution", &error);
	target->frameskipper = g_key_file_get_integer (options_file, "Video", "frameskipper", &error);
	target->throttle = g_key_file_get_integer (options_file, "Video", "throttle", &error);
	target->sleepidle = g_key_file_get_integer (options_file, "Video", "sleepidle", &error);
	target->autoframeskip = g_key_file_get_integer (options_file, "Video", "autoframeskip", &error);
	target->maxautoframeskip = g_key_file_get_integer (options_file, "Video", "maxautoframeskip", &error);
	target->frameskip = g_key_file_get_integer (options_file, "Video", "frameskip", &error);
	target->brightness = g_key_file_get_double (options_file, "Video", "brightness", &error);
	/*  converted old percentage to dosmame brightness complient 0.5-2.0 range*/
	if ( (target->brightness > 2) || (target->brightness < 0.5))
		target->brightness = (target->brightness) * (1.5 / 100) + 0.5;

	target->gamma_correction = g_key_file_get_integer (options_file, "Video", "gamma_correction", &error);
	target->norotate = g_key_file_get_integer (options_file, "Video", "norotate", &error);
	target->ror = g_key_file_get_integer (options_file, "Video", "ror", &error);
	target->rol = g_key_file_get_integer (options_file, "Video", "rol", &error);
	target->flipx = g_key_file_get_integer (options_file, "Video", "flipx", &error);
	target->flipy = g_key_file_get_integer (options_file, "Video", "flipy", &error);

	target->vectorres_flag = g_key_file_get_integer (options_file, "Vector", "vectorres_flag", &error);
	target->vectorres = g_key_file_get_string (options_file, "Vector", "vectorres", &error);
	target->beam = g_key_file_get_double (options_file, "Vector", "beam", &error);
	target->flicker = g_key_file_get_double (options_file, "Vector", "flicker", &error);
	target->antialias = g_key_file_get_integer (options_file, "Vector", "antialias", &error);
	target->translucency = g_key_file_get_integer (options_file, "Vector", "translucency", &error);
	target->intensity = g_key_file_get_double (options_file, "Vector", "intensity", &error);

	target->x11_mode = g_key_file_get_double (options_file, "X11", "x11_mode", &error);

	target->cursor = g_key_file_get_integer (options_file, "X11-window", "cursor", &error);
	target->mitshm = g_key_file_get_integer (options_file, "X11-window", "mitshm", &error);
	target->xvext = g_key_file_get_integer (options_file, "X11-window", "xvext", &error);
	target->vidix = g_key_file_get_integer (options_file, "X11-window", "vidix", &error);
	target->force_yuv = g_key_file_get_integer (options_file, "X11-window", "force_yuv", &error);
	target->xsync = g_key_file_get_integer (options_file, "X11-window", "xsync", &error);
	target->privatecmap = g_key_file_get_integer (options_file, "X11-window", "privatecmap", &error);
	target->xil = g_key_file_get_integer (options_file, "X11-window", "xil", &error);
	target->mtxil = g_key_file_get_integer (options_file, "X11-window", "mtxil", &error);
	target->run_in_root_window = g_key_file_get_integer (options_file, "X11-window", "run_in_root_window", &error);
	target->root_window_id = g_key_file_get_integer (options_file, "X11-window", "root_window_id", &error);
	target->xvgeom_flag = g_key_file_get_integer (options_file, "X11-window", "xvgeom_flag", &error);
	target->geometry = g_key_file_get_string (options_file, "X11-window", "geometry", &error);

	target->grabmouse = g_key_file_get_integer (options_file, "X11-input", "grabmouse", &error);
	target->grabkeyboard = g_key_file_get_integer (options_file, "X11-input", "grabkeyboard", &error);
	target->winkeys = g_key_file_get_integer (options_file, "X11-input", "winkeys", &error);
	target->mapkey = g_key_file_get_string (options_file, "X11-input", "mapkey", &error);

	target->gldblbuffer = g_key_file_get_integer (options_file, "OpenGL", "gldblbuffer", &error);
	target->gltexture_size = g_key_file_get_integer (options_file, "OpenGL", "gltexture_size", &error);
	target->glforceblitmode = g_key_file_get_integer (options_file, "OpenGL", "glforceblitmode", &error);
	target->glext78 = g_key_file_get_integer (options_file, "OpenGL", "glext78", &error);
	target->glbilinear = g_key_file_get_integer (options_file, "OpenGL", "glbilinear", &error);
	target->gldrawbitmap = g_key_file_get_integer (options_file, "OpenGL", "gldrawbitmap", &error);
	target->gldrawbitmapvec = g_key_file_get_integer (options_file, "OpenGL", "gldrawbitmapvec", &error);
	target->glcolormod = g_key_file_get_integer (options_file, "OpenGL", "glcolormod", &error);
	target->glbeam = g_key_file_get_double (options_file, "OpenGL", "glbeam", &error);
	target->glalphablending = g_key_file_get_integer (options_file, "OpenGL", "glalphablending", &error);
	target->glantialias = g_key_file_get_integer (options_file, "OpenGL", "glantialias", &error);
	target->glantialiasvec = g_key_file_get_integer (options_file, "OpenGL", "glantialiasvec", &error);
	target->gllibname = g_key_file_get_string (options_file, "OpenGL", "gllibname", &error);
	target->glulibname = g_key_file_get_string (options_file, "OpenGL", "glulibname", &error);
	target->cabview = g_key_file_get_integer (options_file, "OpenGL", "cabview", &error);
	target->cabinet = g_key_file_get_string (options_file, "OpenGL", "cabinet", &error);
	target->glres_flag = g_key_file_get_integer (options_file, "OpenGL", "glres_flag", &error);
	target->glres = g_key_file_get_string (options_file, "OpenGL", "glres", &error);

	target->sdl_doublebuf = g_key_file_get_double (options_file, "SDL", "sdl_doublebuf", &error);
	target->sdl_auto_mode = g_key_file_get_double (options_file, "SDL", "sdl_auto_mode", &error);
	target->modenumber = g_key_file_get_double (options_file, "SDL", "modenumber", &error);
	target->sdlmapkey = g_key_file_get_string (options_file, "SDL", "sdlmapkey", &error);

	target->ggilinear = g_key_file_get_integer (options_file, "GGI", "ggilinear", &error);
	target->ggi_force_resolution = g_key_file_get_integer (options_file, "GGI", "ggi_force_resolution", &error);
	target->xres = g_key_file_get_integer (options_file, "GGI", "xres", &error);
	target->yres = g_key_file_get_integer (options_file, "GGI", "yres", &error);


#ifdef ENABLE_XMAME_SVGALIB_SUPPORT
	target->tweak = g_key_file_get_integer (options_file, "Svgalib", "tweak", &error);
	target->planar = g_key_file_get_integer (options_file, "Svgalib", "planar", &error);
	target->linear = g_key_file_get_integer (options_file, "Svgalib", "linear", &error);
	target->centerx = g_key_file_get_integer (options_file, "Svgalib", "centerx", &error);
	target->centery = g_key_file_get_integer (options_file, "Svgalib", "centery", &error);
#endif

	target->resolution = g_key_file_get_string (options_file, "FX (Glide)", "resolution", &error);
	target->fxgkeepaspect = g_key_file_get_integer (options_file, "FX (Glide)", "fxgkeepaspect", &error);

	target->render_mode = g_key_file_get_integer (options_file, "Photon", "render_mode", &error);
	target->phcursor = g_key_file_get_integer (options_file, "Photon-window", "phcursor", &error);

	target->keepaspect = g_key_file_get_integer (options_file, "Video Mode", "keepaspect", &error);
	target->displayaspectratio = g_key_file_get_double (options_file, "Video Mode", "displayaspectratio", &error);
	target->disablemode = g_key_file_get_string (options_file, "Video Mode", "disablemode", &error);

	target->sound = g_key_file_get_integer (options_file, "Sound", "sound", &error);
	target->samples = g_key_file_get_integer (options_file, "Sound", "samples", &error);
	target->fakesound = g_key_file_get_integer (options_file, "Sound", "fakesound", &error);
	target->samplefre = g_key_file_get_double (options_file, "Sound", "samplefre", &error);
	target->bufsize = g_key_file_get_double (options_file, "Sound", "bufsize", &error);
	target->volume = g_key_file_get_integer (options_file, "Sound", "volume", &error);
	target->audiodevice = g_key_file_get_string (options_file, "Sound", "audiodevice", &error);
	target->mixerdevice = g_key_file_get_string (options_file, "Sound", "mixerdevice", &error);
	target->soundfile = g_key_file_get_string (options_file, "Sound", "soundfile", &error);

	target->joytype = g_key_file_get_integer (options_file, "Input", "joytype", &error);
	target->analogstick = g_key_file_get_integer (options_file, "Input", "analogstick", &error);
	target->mouse = g_key_file_get_integer (options_file, "Input", "mouse", &error);
	target->hotrod = g_key_file_get_double (options_file, "Input", "hotrod", &error);
	target->hotrodse = g_key_file_get_double (options_file, "Input", "hotrodse", &error);
	target->usbpspad = g_key_file_get_integer (options_file, "Input", "usbpspad", &error);
	target->rapidfire = g_key_file_get_integer (options_file, "Input", "rapidfire", &error);
	target->keymap = g_key_file_get_string (options_file, "Input", "keymap", &error);
	target->ugcicoin = g_key_file_get_integer (options_file, "Input", "ugcicoin", &error);
	target->ctrlr = g_key_file_get_string (options_file, "Input", "ctrlr", &error);
	target->joydevname = g_key_file_get_string (options_file, "Input", "joydevname", &error);


	/* This is loaded from the Input section. It's left here for compatibility
	 * with gxmame <=0.35beta2.
	 */
	target->joydevname = g_key_file_get_string (options_file, "i386", "joydevname", &error);

	target->paddevname = g_key_file_get_string (options_file, "joy_pad_opts", "paddevname", &error);

	target->x11joyname = g_key_file_get_string (options_file, "joy_x11_opts", "x11joyname", &error);

	target->XInput_trackball1 = g_key_file_get_string (options_file, "Xinput", "XInput_trackball1", &error);
	target->XInput_trackball2 = g_key_file_get_string (options_file, "Xinput", "XInput_trackball2", &error);
	target->XInput_trackball3 = g_key_file_get_string (options_file, "Xinput", "XInput_trackball3", &error);
	target->XInput_trackball4 = g_key_file_get_string (options_file, "Xinput", "XInput_trackball4", &error);


	/*		} else if (!strcmp (option_value[0], "XInput_joystick1")) {
				g_snprintf (target->XInput_joystick1, 20, option_value[1]);
			} else if (!strcmp (option_value[0], "XInput_joystick2")) {
				g_snprintf (target->XInput_joystick2, 20, option_value[1]);
			} else if (!strcmp (option_value[0], "XInput_joystick3")) {
				g_snprintf (target->XInput_joystick3, 20, option_value[1]);
			} else if (!strcmp (option_value[0], "XInput_joystick4")) {
				g_snprintf (target->XInput_joystick4, 20, option_value[1]);
	*/

	target->dsp_plugin = g_key_file_get_string (options_file, "Digital sound", "dsp_plugin", &error);
	target->timer = g_key_file_get_integer (options_file, "Digital sound", "timer", &error);

	target->sound_mixer_plugin = g_key_file_get_string (options_file, "Sound mixer", "sound_mixer_plugin", &error);

	target->artsBufferTime = g_key_file_get_integer (options_file, "arts", "artsBufferTime", &error);

	target->alsacard = g_key_file_get_integer (options_file, "Alsa Sound System 0.5", "alsacard", &error);
	target->alsadevice = g_key_file_get_integer (options_file, "Alsa Sound System 0.5", "alsadevice", &error);

	target->alsa_pcm = g_key_file_get_string (options_file, "Alsa Sound System 0.9", "alsa_pcm", &error);
	target->alsa_buffer = g_key_file_get_integer (options_file, "Alsa Sound System 0.9", "alsa_buffer", &error);

	target->audio_preferred = g_key_file_get_integer (options_file, "QNX Audio", "audio_preferred", &error);

	target->cheat = g_key_file_get_integer (options_file, "Misc", "cheat", &error);
	target->keyboard_leds = g_key_file_get_integer (options_file, "Misc", "keyboard_leds", &error);
	target->debug = g_key_file_get_integer (options_file, "Misc", "debug", &error);
	target->debug_size = g_key_file_get_string (options_file, "Misc", "debug_size", &error);
	target->log_flag = g_key_file_get_integer (options_file, "Misc", "log_flag", &error);
	target->log = g_key_file_get_string (options_file, "Misc", "log", &error);
	target->cfgname_flag = g_key_file_get_integer (options_file, "Misc", "cfgname_flag", &error);
	target->cfgname = g_key_file_get_string (options_file, "Misc", "cfgname", &error);
	target->use_additional_options = g_key_file_get_integer (options_file, "Misc", "use_additional_options", &error);
	target->additional_options = g_key_file_get_string (options_file, "Misc", "additional_options", &error);
	target->skip_disclaimer = g_key_file_get_integer (options_file, "Misc", "skip_disclaimer", &error);
	target->skip_gameinfo = g_key_file_get_integer (options_file, "Misc", "skip_gameinfo", &error);
	target->bios = g_key_file_get_integer (options_file, "Misc", "bios", &error);
		
	
	/* we should not allow xv vidix and DGA at the same time, priority to older (DGA) */
	if (target->x11_mode == 1) {
		target->xvext = FALSE;
		target->vidix = FALSE;
	} else if (target->xvext == 1)
		target->vidix = FALSE;
	
	/* if ctrlr is not available in the list then -> None */
	ctrlr_list = get_ctrlr_list ();
	for (my_list = g_list_first (ctrlr_list); (my_list != NULL); my_list = g_list_next (my_list)) {
		if (strncmp (my_list->data, target->ctrlr, 20) == 0)
			break;
	}

	if (my_list == NULL)
		strcpy (target->ctrlr, "None");
	g_list_free (ctrlr_list);

	g_key_file_free (options_file);
	
	return target;
}

/**
 * Saves the options for the given rom.
 * The filename is ~/.gmameui/options/romname
 *
 * If the rom is NULL then it saves the default options
 * and the opts parameter is not used.
 */
gboolean
save_options (RomEntry    *rom,
	      GameOptions *target)
{
	GKeyFile *options_file = g_key_file_new ();
	
	gchar *filename;
	gchar double_buffer[FLOAT_BUF_SIZE];
	
	if (rom == NULL) {
		target = &default_options;
		filename = g_build_filename (g_get_home_dir (), ".gmameui", "options", "default", NULL);	
	} else {
		/* no game preferences to save */
		if (!target)
			return TRUE;

		filename = g_build_filename (g_get_home_dir (), ".gmameui", "options", rom->romname, NULL);	
	}

	g_key_file_set_integer (options_file, "Video", "fullscreen", target->fullscreen);
	g_key_file_set_integer (options_file, "Video", "bpp", target->bpp);
	g_key_file_set_integer (options_file, "Video", "arbheight", target->arbheight);
	g_key_file_set_integer (options_file, "Video", "heightscale", target->heightscale);
	g_key_file_set_integer (options_file, "Video", "widthscale", target->widthscale);
	g_key_file_set_integer (options_file, "Video", "effect", target->effect);
	g_key_file_set_string (options_file, "Video", "effect_name", target->effect_name);
	g_key_file_set_integer (options_file, "Video", "autodouble", target->autodouble);
	g_key_file_set_integer (options_file, "Video", "dirty", target->dirty);
	g_key_file_set_integer (options_file, "Video", "scanlines", target->scanlines);
	g_key_file_set_integer (options_file, "Video", "artwork", target->artwork);
	g_key_file_set_integer (options_file, "Video", "use_backdrops", target->use_backdrops);
	g_key_file_set_integer (options_file, "Video", "use_overlays", target->use_overlays);
	g_key_file_set_integer (options_file, "Video", "use_bezels", target->use_bezels);
	g_key_file_set_integer (options_file, "Video", "artwork_crop", target->artwork_crop);
	g_key_file_set_integer (options_file, "Video", "artwork_resolution", target->artwork_resolution);
	g_key_file_set_integer (options_file, "Video", "frameskipper", target->frameskipper);
	g_key_file_set_integer (options_file, "Video", "throttle", target->throttle);
	g_key_file_set_integer (options_file, "Video", "sleepidle", target->sleepidle);
	g_key_file_set_integer (options_file, "Video", "autoframeskip", target->autoframeskip);
	g_key_file_set_integer (options_file, "Video", "maxautoframeskip", target->maxautoframeskip);
	g_key_file_set_integer (options_file, "Video", "frameskip", target->frameskip);
	g_key_file_set_double (options_file, "Video", "brightness", target->brightness);
	g_key_file_set_integer (options_file, "Video", "gamma_correction", target->gamma_correction);
	g_key_file_set_integer (options_file, "Video", "norotate", target->norotate);
	g_key_file_set_integer (options_file, "Video", "ror", target->ror);
	g_key_file_set_integer (options_file, "Video", "rol", target->rol);
	g_key_file_set_integer (options_file, "Video", "flipx", target->flipx);
	g_key_file_set_integer (options_file, "Video", "flipy", target->flipy);
	
	g_key_file_set_integer (options_file, "Vector", "vectorres_flag", target->vectorres_flag);
	g_key_file_set_string (options_file, "Vector", "vectorres", target->vectorres);
	g_key_file_set_double (options_file, "Vector", "beam", target->beam);
	g_key_file_set_double (options_file, "Vector", "flicker", target->flicker);
	g_key_file_set_integer (options_file, "Vector", "antialias", target->antialias);
	g_key_file_set_integer (options_file, "Vector", "translucency", target->translucency);
	g_key_file_set_double (options_file, "Vector", "intensity", target->intensity);

	g_key_file_set_double (options_file, "X11", "x11_mode", target->x11_mode);

	g_key_file_set_integer (options_file, "X11-window", "cursor", target->cursor);
	g_key_file_set_integer (options_file, "X11-window", "mitshm", target->mitshm);
	g_key_file_set_integer (options_file, "X11-window", "xvext", target->xvext);
	g_key_file_set_integer (options_file, "X11-window", "vidix", target->vidix);
	g_key_file_set_integer (options_file, "X11-window", "force_yuv", target->force_yuv);
	g_key_file_set_integer (options_file, "X11-window", "xsync", target->xsync);
	g_key_file_set_integer (options_file, "X11-window", "privatecmap", target->privatecmap);
	g_key_file_set_integer (options_file, "X11-window", "xil", target->xil);
	g_key_file_set_integer (options_file, "X11-window", "mtxil", target->mtxil);
	g_key_file_set_integer (options_file, "X11-window", "run_in_root_window", target->run_in_root_window);
	g_key_file_set_integer (options_file, "X11-window", "root_window_id", target->root_window_id);
	g_key_file_set_integer (options_file, "X11-window", "xvgeom_flag", target->xvgeom_flag);
	g_key_file_set_string (options_file, "X11-window", "geometry", target->geometry);

	g_key_file_set_integer (options_file, "X11-input", "grabmouse", target->grabmouse);
	g_key_file_set_integer (options_file, "X11-input", "grabkeyboard", target->grabkeyboard);
	g_key_file_set_integer (options_file, "X11-input", "winkeys", target->winkeys);
	g_key_file_set_string (options_file, "X11-input", "mapkey", target->mapkey);

	g_key_file_set_integer (options_file, "OpenGL", "gldblbuffer", target->gldblbuffer);
	g_key_file_set_integer (options_file, "OpenGL", "gltexture_size", target->gltexture_size);
	g_key_file_set_integer (options_file, "OpenGL", "glforceblitmode", target->glforceblitmode);
	g_key_file_set_integer (options_file, "OpenGL", "glext78", target->glext78);
	g_key_file_set_integer (options_file, "OpenGL", "glbilinear", target->glbilinear);
	g_key_file_set_integer (options_file, "OpenGL", "gldrawbitmap", target->gldrawbitmap);
	g_key_file_set_integer (options_file, "OpenGL", "gldrawbitmapvec", target->gldrawbitmapvec);
	g_key_file_set_integer (options_file, "OpenGL", "glcolormod", target->glcolormod);
	g_key_file_set_double (options_file, "OpenGL", "glbeam", target->glbeam);
	g_key_file_set_integer (options_file, "OpenGL", "glalphablending", target->glalphablending);
	g_key_file_set_integer (options_file, "OpenGL", "glantialias", target->glantialias);
	g_key_file_set_integer (options_file, "OpenGL", "glantialiasvec", target->glantialiasvec);
	g_key_file_set_string (options_file, "OpenGL", "gllibname", target->gllibname);
	g_key_file_set_string (options_file, "OpenGL", "glulibname", target->glulibname);
	g_key_file_set_integer (options_file, "OpenGL", "cabview", target->cabview);
	g_key_file_set_string (options_file, "OpenGL", "cabinet", target->cabinet);
	g_key_file_set_integer (options_file, "OpenGL", "glres_flag", target->glres_flag);
	g_key_file_set_string (options_file, "OpenGL", "glres", target->glres);

	g_key_file_set_double (options_file, "SDL", "sdl_doublebuf", target->sdl_doublebuf);
	g_key_file_set_double (options_file, "SDL", "sdl_auto_mode", target->sdl_auto_mode);
	g_key_file_set_double (options_file, "SDL", "modenumber", target->modenumber);
	g_key_file_set_string (options_file, "SDL", "sdlmapkey", target->sdlmapkey);

	g_key_file_set_integer (options_file, "GGI", "ggilinear", target->ggilinear);
	g_key_file_set_integer (options_file, "GGI", "ggi_force_resolution", target->ggi_force_resolution);
	g_key_file_set_integer (options_file, "GGI", "xres", target->xres);
	g_key_file_set_integer (options_file, "GGI", "yres", target->yres);
	
#ifdef ENABLE_XMAME_SVGALIB_SUPPORT
	g_key_file_set_integer (options_file, "Svgalib", "tweak", target->tweak);
	g_key_file_set_integer (options_file, "Svgalib", "planar", target->planar);
	g_key_file_set_integer (options_file, "Svgalib", "linear", target->linear);
	g_key_file_set_integer (options_file, "Svgalib", "centerx", target->centerx);
	g_key_file_set_integer (options_file, "Svgalib", "centery", target->centery);

#endif
	g_key_file_set_string (options_file, "FX (Glide)", "resolution", target->resolution);
	g_key_file_set_integer (options_file, "FX (Glide)", "fxgkeepaspect", target->fxgkeepaspect);

	g_key_file_set_integer (options_file, "Photon", "render_mode", target->render_mode);
	g_key_file_set_integer (options_file, "Photon-window", "phcursor", target->phcursor);
/*
	
	*	fputs ("\n[Photon-input]\n", options_file);
		fprintf (options_file, "phgrabmouse=%i\n", target->phgrabmouse);
		fprintf (options_file, "phwinkeys=%i\n", target->phwinkeys);
		fprintf (options_file, "phmapkey=%s\n", target->phmapkey);
	*	
*/
	g_key_file_set_integer (options_file, "Video Mode", "keepaspect", target->keepaspect);
	g_key_file_set_double (options_file, "Video Mode", "displayaspectratio", target->displayaspectratio);
	g_key_file_set_string (options_file, "Video Mode", "disablemode", target->disablemode);

	g_key_file_set_integer (options_file, "Sound", "sound", target->sound);
	g_key_file_set_integer (options_file, "Sound", "samples", target->samples);
	g_key_file_set_integer (options_file, "Sound", "fakesound", target->fakesound);
	g_key_file_set_double (options_file, "Sound", "samplefre", target->samplefre);
	g_key_file_set_double (options_file, "Sound", "bufsize", target->bufsize);
	g_key_file_set_integer (options_file, "Sound", "volume", target->volume);
	g_key_file_set_string (options_file, "Sound", "audiodevice", target->audiodevice);
	g_key_file_set_string (options_file, "Sound", "mixerdevice", target->mixerdevice);
	g_key_file_set_string (options_file, "Sound", "soundfile", target->soundfile);

	g_key_file_set_integer (options_file, "Input", "joytype", target->joytype);
	g_key_file_set_integer (options_file, "Input", "analogstick", target->analogstick);
	g_key_file_set_integer (options_file, "Input", "mouse", target->mouse);
	g_key_file_set_double (options_file, "Input", "hotrod", target->hotrod);
	g_key_file_set_double (options_file, "Input", "hotrodse", target->hotrodse);
	g_key_file_set_integer (options_file, "Input", "usbpspad", target->usbpspad);
	g_key_file_set_integer (options_file, "Input", "rapidfire", target->rapidfire);
	g_key_file_set_string (options_file, "Input", "keymap", target->keymap);
	g_key_file_set_integer (options_file, "Input", "ugcicoin", target->ugcicoin);
	g_key_file_set_string (options_file, "Input", "ctrlr", target->ctrlr);
	g_key_file_set_string (options_file, "Input", "joydevname", target->joydevname);
	

	g_key_file_set_string (options_file, "joy_pad_opts", "paddevname", target->paddevname);

	g_key_file_set_string (options_file, "joy_x11_opts", "x11joyname", target->x11joyname);

	g_key_file_set_string (options_file, "Xinput", "XInput_trackball1", target->XInput_trackball1);
	g_key_file_set_string (options_file, "Xinput", "XInput_trackball2", target->XInput_trackball2);
	g_key_file_set_string (options_file, "Xinput", "XInput_trackball3", target->XInput_trackball3);
	g_key_file_set_string (options_file, "Xinput", "XInput_trackball4", target->XInput_trackball4);

/*
	*	fprintf (options_file, "XInput_joystick1=%s\n", target->XInput_joystick1);
		fprintf (options_file, "XInput_joystick2=%s\n", target->XInput_joystick2);
		fprintf (options_file, "XInput_joystick3=%s\n", target->XInput_joystick3);
		fprintf (options_file, "XInput_joystick4=%s\n", target->XInput_joystick4);
	*
*/
	g_key_file_set_string (options_file, "Digital sound", "dsp_plugin", target->dsp_plugin);
	g_key_file_set_integer (options_file, "Digital sound", "timer", target->timer);

	g_key_file_set_string (options_file, "Sound mixer", "sound_mixer_plugin", target->sound_mixer_plugin);

	g_key_file_set_integer (options_file, "arts", "artsBufferTime", target->artsBufferTime);

	g_key_file_set_integer (options_file, "Alsa Sound System 0.5", "alsacard", target->alsacard);
	g_key_file_set_integer (options_file, "Alsa Sound System 0.5", "alsadevice", target->alsadevice);

	g_key_file_set_string (options_file, "Alsa Sound System 0.9", "alsa_pcm", target->alsa_pcm);
	g_key_file_set_integer (options_file, "Alsa Sound System 0.9", "alsa_buffer", target->alsa_buffer);
	

	g_key_file_set_integer (options_file, "QNX Audio", "audio_preferred", target->audio_preferred);
/*	
	*	fputs ("\n[Display]\n", options_file);
		fprintf (options_file, "display_plugin=%s\n", target->display_plugin);
		fprintf (options_file, "dwidthscale=%i\n", target->dwidthscale);
		fprintf (options_file, "dheightscale=%i\n", target->dheightscale);
		fprintf (options_file, "dscanlines=%i\n", target->dscanlines);
		fprintf (options_file, "display_aspect_ratio=%s\n", my_dtostr (double_buffer, target->aspect_ratio));
		fprintf (options_file, "keep_aspect=%i\n", target->keep_aspect);
	*
	
*/

	g_key_file_set_integer (options_file, "Misc", "cheat", target->cheat);
	g_key_file_set_integer (options_file, "Misc", "keyboard_leds", target->keyboard_leds);
	g_key_file_set_integer (options_file, "Misc", "debug", target->debug);
	g_key_file_set_string (options_file, "Misc", "debug_size", target->debug_size);
	g_key_file_set_integer (options_file, "Misc", "log_flag", target->log_flag);
	g_key_file_set_string (options_file, "Misc", "log", target->log);
	g_key_file_set_integer (options_file, "Misc", "cfgname_flag", target->cfgname_flag);
	g_key_file_set_string (options_file, "Misc", "cfgname", target->cfgname);
	g_key_file_set_integer (options_file, "Misc", "use_additional_options", target->use_additional_options);
	g_key_file_set_string (options_file, "Misc", "additional_options", target->additional_options);
	g_key_file_set_integer (options_file, "Misc", "skip_disclaimer", target->skip_disclaimer);
	g_key_file_set_integer (options_file, "Misc", "skip_gameinfo", target->skip_gameinfo);
	g_key_file_set_integer (options_file, "Misc", "bios", target->bios);	
	
	
	g_key_file_save_to_file (options_file, filename, NULL);
	g_free (filename);
	return TRUE;
}
