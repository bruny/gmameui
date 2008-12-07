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

#include "xmame_options.h"

/* maps options to their GMAMEUI option name
* all options MUST be in this table at least once. 
*/
static GHashTable *option_map;

#define ADD_OPTION_ALIAS(gmameui_name, mame_name) g_hash_table_insert (option_map, mame_name, gmameui_name)
#define ADD_OPTION(mame_name) g_hash_table_insert (option_map, mame_name, mame_name)

void
xmame_options_free (void)
{
	g_hash_table_destroy (option_map);
}

void
xmame_options_init (void)
{
	option_map = g_hash_table_new (g_str_hash, g_str_equal);
	
	/* Core search path options */
	ADD_OPTION ("rompath");
	ADD_OPTION ("samplepath");
	ADD_OPTION ("artwork_directory");
	ADD_OPTION ("ctrlr_directory");
	ADD_OPTION ("inipath");
	ADD_OPTION ("fontpath");
	
	/* Core output directory options */
	ADD_OPTION ("cfg_directory");
	ADD_OPTION ("nvram_directory");
	ADD_OPTION ("memcard_directory");
	ADD_OPTION ("input_directory");
	ADD_OPTION ("state_directory");
	ADD_OPTION ("snapshot_directory");
	ADD_OPTION ("diff_directory");
	ADD_OPTION ("comment_directory");
	
	/* Core filename options */
	ADD_OPTION ("cheat_file");
	
	/* Core state/playback options */
	ADD_OPTION ("autosave");
	ADD_OPTION ("mngwrite");
	ADD_OPTION ("aviwrite");
	ADD_OPTION ("wavwrite");
	/* FIXME TODO - Others */
	
	/* Core performance options */
	ADD_OPTION ("autoframeskip");
	ADD_OPTION ("frameskip");
	ADD_OPTION ("seconds_to_run");
	ADD_OPTION ("throttle");
	ADD_OPTION ("sleep");
	ADD_OPTION ("speed");
	ADD_OPTION ("refreshspeed");
	
	/* Core rotation options */
	ADD_OPTION ("rotate");
	ADD_OPTION ("ror");
	ADD_OPTION ("rol");
	ADD_OPTION ("autoror");
	ADD_OPTION ("autorol");
	ADD_OPTION ("flipx");
	ADD_OPTION ("flipy");
	
	/* Core artwork options */
	ADD_OPTION ("artwork_crop");
	ADD_OPTION ("use_backdrops");
	ADD_OPTION ("use_overlays");
	ADD_OPTION ("use_bezels");
	
	/* Core screen options */
	ADD_OPTION ("brightness");
	ADD_OPTION ("contrast");
	ADD_OPTION ("gamma");
	ADD_OPTION ("pause_brightness");
	
	/* Core vector options */
	ADD_OPTION ("antialias");
	ADD_OPTION ("beam");
	ADD_OPTION ("flicker");
	
	/* Core sound options */
	ADD_OPTION ("sound");
	ADD_OPTION ("samplerate");
	ADD_OPTION ("samples");
	ADD_OPTION ("volume");
	
	/* Core input options */
	ADD_OPTION ("coin_lockout");
	ADD_OPTION ("mouse");
	ADD_OPTION ("joystick");
	ADD_OPTION ("lightgun");
	ADD_OPTION ("multikeyboard");
	ADD_OPTION ("multimouse");
	ADD_OPTION ("steadykey");
	ADD_OPTION ("offscreen_reload");
	ADD_OPTION ("joystick_map");
	ADD_OPTION ("joystick_deadzone");
	ADD_OPTION ("joystick_saturation");
	
	/* Core input automatic enable options TODO */
	
	/* Core misc options */
	ADD_OPTION ("bios");
	ADD_OPTION ("cheat");
	ADD_OPTION ("skip_gameinfo");
	
	/* Core debugging options */
	ADD_OPTION ("log");
	ADD_OPTION ("verbose");
	ADD_OPTION ("update_in_pause");
	ADD_OPTION ("debug");
	ADD_OPTION ("debugscript");
	/* Debugging options */
	ADD_OPTION ("oslog");
	
	/* Performance options */
	ADD_OPTION ("multithreading");
	ADD_OPTION ("sdlvideofps");
	
	/* Video options */
	ADD_OPTION ("video");
	ADD_OPTION ("numscreens");
	ADD_OPTION ("window");
	ADD_OPTION ("maximize");
	ADD_OPTION ("keepaspect");
	ADD_OPTION ("unevenstretch");
	ADD_OPTION ("effect");
	ADD_OPTION ("centerh");
	ADD_OPTION ("centerv");
	ADD_OPTION ("waitvsync");
	ADD_OPTION ("yuvmode");
	
	/* OpenGL-specific options */
	ADD_OPTION ("filter");
	ADD_OPTION ("prescale");
	ADD_OPTION ("gl_forcepow2texture");
	ADD_OPTION ("gl_notexturerect");
	ADD_OPTION ("gl_vbo");
	ADD_OPTION ("gl_pbo");
	ADD_OPTION ("gl_glsl");
	ADD_OPTION ("gl_glsl_filter");
	/* A number of other OpenGL options FIXME TODO */
	ADD_OPTION ("gl_glsl_vid_attr");
	
	/* Per-window video options TODO */
	
	/* Full screen options */
	ADD_OPTION ("switchres");
	ADD_OPTION ("useallheads");
	
	/* Sound options */
	ADD_OPTION ("audio_latency");
	
	/* SDL keyboard mapping TODO */
	
	/* SDL joystick mapping TODO */
	
	
	/* XMAME or redundant options */
	ADD_OPTION ("alsa-buffer");
	ADD_OPTION ("alsa-pcm");
	ADD_OPTION ("alsacard");
	ADD_OPTION ("alsadevice");
	ADD_OPTION ("analogstick");
	ADD_OPTION ("arbheight");
	ADD_OPTION ("artsBufferTime");
	ADD_OPTION ("artwork");
	ADD_OPTION ("artwork_resolution");
	ADD_OPTION ("audiodevice");
	ADD_OPTION ("audio_preferred");
	ADD_OPTION ("autodouble");
	ADD_OPTION ("bind");
	ADD_OPTION ("bpp");
	ADD_OPTION ("bufsize");
	ADD_OPTION ("cabview");
	ADD_OPTION ("centerx");
	ADD_OPTION ("centery");
	ADD_OPTION ("cfgname");
	ADD_OPTION ("ctrlr");
	ADD_OPTION ("cursor");
	
	ADD_OPTION ("debug-size");
	ADD_OPTION ("dirty");
	ADD_OPTION ("displayaspectratio");
	ADD_OPTION ("doublebuf");
	ADD_OPTION ("dsp-plugin");
	ADD_OPTION ("fakesound");
	ADD_OPTION ("force-yuv");
	ADD_OPTION ("frameskipper");
	ADD_OPTION ("fullscreen");
	ADD_OPTION ("gamma-correction");
	ADD_OPTION ("geometry");
	ADD_OPTION ("glalphablending");
	ADD_OPTION ("glantialias");
	ADD_OPTION ("glantialiasvec");
	ADD_OPTION ("glbeam");
	ADD_OPTION ("glbilinear");
	ADD_OPTION ("glcolormod");
	ADD_OPTION ("gldblbuffer");
	ADD_OPTION ("gldrawbitmap");
	ADD_OPTION ("gldrawbitmapvec");
	ADD_OPTION ("glext78");
	ADD_OPTION ("glforceblitmode");
	ADD_OPTION ("gllibname");
	ADD_OPTION ("glulibname");
	ADD_OPTION ("glres");
	ADD_OPTION ("gltexture_size");
	ADD_OPTION ("grabkeyboard");
	ADD_OPTION ("grabmouse");
	ADD_OPTION ("heightscale");
	ADD_OPTION ("hiscore_directory");
	ADD_OPTION ("hiscore_file");
	ADD_OPTION ("history_file");
	ADD_OPTION ("hotrod");
	ADD_OPTION ("hotrodse");
	ADD_OPTION ("intensity");
	ADD_OPTION ("joydevname");
	ADD_OPTION ("joytype");
	ADD_OPTION ("keyboard_leds");
	ADD_OPTION ("linear");
	ADD_OPTION ("list");
	ADD_OPTION ("listdetails");
	ADD_OPTION ("listfull");
	ADD_OPTION ("listgames");
	ADD_OPTION ("listinfo");
	ADD_OPTION ("listmodes");
	ADD_OPTION ("listsourcefile");
	ADD_OPTION ("listxml");
	ADD_OPTION ("list-alsa-cards");
	ADD_OPTION ("list-alsa-pcm");
	ADD_OPTION ("list-display-plugins");
	ADD_OPTION ("list-dsp-plugins");
	ADD_OPTION ("list-mixer-plugins");
	ADD_OPTION ("loadconfig");
	ADD_OPTION ("mameinfo_file");
	ADD_OPTION ("mapkey");
	ADD_OPTION ("master");
	ADD_OPTION ("maxautoframeskip");
	ADD_OPTION ("mitshm");
	ADD_OPTION ("mixerdevice");
	ADD_OPTION ("modenumber");
	
	ADD_OPTION ("mtxil");
	ADD_OPTION ("netmapkey");
	ADD_OPTION ("norotate");
	ADD_OPTION ("paddevname");
	ADD_OPTION ("parallelsync");
	ADD_OPTION ("phcursor");
	ADD_OPTION ("planar");
	ADD_OPTION ("playback");
	ADD_OPTION ("privatecmap");
	ADD_OPTION ("rapidfire");
	ADD_OPTION ("render-mode");
	ADD_OPTION ("resolution");
	ADD_OPTION ("samplefreq");
	ADD_OPTION ("scanlines");
	ADD_OPTION ("skip_disclaimer");
	ADD_OPTION ("slave");
	ADD_OPTION ("sleepidle");
	ADD_OPTION ("sound-mixer-plugin");
	ADD_OPTION ("statedebug");
	ADD_OPTION ("timer");
	ADD_OPTION ("translucency");
	ADD_OPTION ("tweak");
	ADD_OPTION ("ugcicoin");
	ADD_OPTION ("usbpspad");
	ADD_OPTION ("vectorres");
	ADD_OPTION ("verifyroms");
	ADD_OPTION ("verifysamples");
	ADD_OPTION ("video-mode");
	ADD_OPTION ("vidix");
	ADD_OPTION ("vidmode_h");
	ADD_OPTION ("vidmode_w");
	ADD_OPTION ("widthscale");
	ADD_OPTION ("winkeys");
	ADD_OPTION ("x11joyname");
	ADD_OPTION ("xil");
	ADD_OPTION ("XInput_trackball");
	ADD_OPTION ("xres");
	ADD_OPTION ("xsync");
	ADD_OPTION ("xvext");
	ADD_OPTION ("xyres");
	ADD_OPTION ("yuv");
	ADD_OPTION ("yv12");
	
	/* Alternative names for options */
	ADD_OPTION_ALIAS ("artwork_directory",  "artworkpath");
	ADD_OPTION_ALIAS ("artwork_directory",  "artpath");
	ADD_OPTION_ALIAS ("cheat_file",         "cheatfile");
	ADD_OPTION_ALIAS ("diff_directory",     "diffdir");
	ADD_OPTION_ALIAS ("gamma-correction",   "gamma");
	ADD_OPTION_ALIAS ("hiscore_directory",  "spooldir");
	ADD_OPTION_ALIAS ("hiscore_file",       "hiscorefile");
	ADD_OPTION_ALIAS ("history_file",       "historyfile");
	ADD_OPTION_ALIAS ("loadconfig",         "readconfig");
	ADD_OPTION_ALIAS ("mameinfo_file",      "mameinfofile");
	ADD_OPTION_ALIAS ("mapkey",             "sdlmapkey");
	ADD_OPTION_ALIAS ("snapshot_directory", "screenshotdir");
	ADD_OPTION_ALIAS ("video-mode",         "x11-mode");
	ADD_OPTION_ALIAS ("yuvmode",         "scalemode");      /* Between 124u1 and 127u2 */
}

const gchar *
xmame_option_get_gmameui_name (const gchar *alias)
{
	return (const gchar*)g_hash_table_lookup (option_map, alias);
}

const gchar *
mame_option_get_gmameui_name (const gchar *alias)
{
	return (const gchar*)g_hash_table_lookup (option_map, alias);
}

void
xmame_option_free (MameOption *opt)
{
	g_free (opt->name);
	g_free (opt->description);
	g_free (opt->type);
	g_strfreev (opt->possible_values);
	g_strfreev (opt->keys);
}
