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

	ADD_OPTION ("alsa-buffer");
	ADD_OPTION ("alsa-pcm");
	ADD_OPTION ("alsacard");
	ADD_OPTION ("alsadevice");
	ADD_OPTION ("analogstick");
	ADD_OPTION ("antialias");
	ADD_OPTION ("arbheight");
	ADD_OPTION ("artsBufferTime");
	ADD_OPTION ("artwork");
	ADD_OPTION ("artwork_crop");
	ADD_OPTION ("artwork_directory");
	ADD_OPTION ("artwork_resolution");
	ADD_OPTION ("audiodevice");
	ADD_OPTION ("audio_preferred");
	ADD_OPTION ("autodouble");
	ADD_OPTION ("autoframeskip");
	ADD_OPTION ("bind");
	ADD_OPTION ("bios");
	ADD_OPTION ("bpp");
	ADD_OPTION ("brightness");
	ADD_OPTION ("bufsize");
	ADD_OPTION ("cabview");
	ADD_OPTION ("centerx");
	ADD_OPTION ("centery");
	ADD_OPTION ("cfg_directory");
	ADD_OPTION ("cfgname");
	ADD_OPTION ("cheat");
	ADD_OPTION ("cheat_file");
	ADD_OPTION ("ctrlr");
	ADD_OPTION ("ctrlr_directory");
	ADD_OPTION ("cursor");
	ADD_OPTION ("debug");
	ADD_OPTION ("debug-size");
	ADD_OPTION ("diff_directory");
	ADD_OPTION ("dirty");
	ADD_OPTION ("displayaspectratio");
	ADD_OPTION ("doublebuf");
	ADD_OPTION ("dsp-plugin");
	ADD_OPTION ("effect");
	ADD_OPTION ("fakesound");
	ADD_OPTION ("flipx");
	ADD_OPTION ("flipy");
	ADD_OPTION ("force-yuv");
	ADD_OPTION ("frameskip");
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
	ADD_OPTION ("inipath");
	ADD_OPTION ("input_directory");
	ADD_OPTION ("intensity");
	ADD_OPTION ("joydevname");
	ADD_OPTION ("joytype");
	ADD_OPTION ("keepaspect");
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
	ADD_OPTION ("memcard_directory");
	ADD_OPTION ("mitshm");
	ADD_OPTION ("mixerdevice");
	ADD_OPTION ("modenumber");
	ADD_OPTION ("mouse");
	ADD_OPTION ("mtxil");
	ADD_OPTION ("netmapkey");
	ADD_OPTION ("norotate");
	ADD_OPTION ("nvram_directory");
	ADD_OPTION ("paddevname");
	ADD_OPTION ("parallelsync");
	ADD_OPTION ("phcursor");
	ADD_OPTION ("planar");
	ADD_OPTION ("playback");
	ADD_OPTION ("privatecmap");
	ADD_OPTION ("rapidfire");
	ADD_OPTION ("render-mode");
	ADD_OPTION ("resolution");
	ADD_OPTION ("rol");
	ADD_OPTION ("rompath");
	ADD_OPTION ("ror");
	ADD_OPTION ("samplefreq");
	ADD_OPTION ("samplepath");
	ADD_OPTION ("samples");
	ADD_OPTION ("scanlines");
	ADD_OPTION ("skip_disclaimer");
	ADD_OPTION ("skip_gameinfo");
	ADD_OPTION ("slave");
	ADD_OPTION ("sleepidle");
	ADD_OPTION ("snapshot_directory");
	ADD_OPTION ("sound");
	ADD_OPTION ("sound-mixer-plugin");
	ADD_OPTION ("statedebug");
	ADD_OPTION ("state_directory");
	ADD_OPTION ("throttle");
	ADD_OPTION ("timer");
	ADD_OPTION ("translucency");
	ADD_OPTION ("tweak");
	ADD_OPTION ("ugcicoin");
	ADD_OPTION ("usbpspad");
	ADD_OPTION ("use_backdrops");
	ADD_OPTION ("use_bezels");
	ADD_OPTION ("use_overlays");
	ADD_OPTION ("vectorres");
	ADD_OPTION ("verifyroms");
	ADD_OPTION ("verifysamples");
	ADD_OPTION ("video-mode");
	ADD_OPTION ("vidix");
	ADD_OPTION ("vidmode_h");
	ADD_OPTION ("vidmode_w");
	ADD_OPTION ("volume");
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
}

const gchar *
xmame_option_get_gmameui_name (const gchar *alias)
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
