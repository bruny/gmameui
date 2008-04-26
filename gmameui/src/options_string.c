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

#include "common.h"
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "gmameui.h"
#include "options_string.h"
#include "io.h"
#include "keyboard.h"

/* all options string are created here */
static char *create_video_options_string(XmameExecutable *exec, GameOptions *target)
{
	char *option_string;
	char float_buf[FLOAT_BUF_SIZE];
	gchar *bpp, *arbheight, *heightscale, *widthscale, *gamma_correction;
	gchar *brightness;
	gchar *dirty;
	gchar *effect;
	gchar *effect_name;
	gchar *autodouble;
	gchar *scanlines;
	gchar *frameskipper;
	gchar *throttle;
	gchar *sleepidle;
	gchar *autoframeskip;
	gchar *maxautoframeskip;
	gchar *frameskip;
	gchar *norotate;
	gchar *ror;
	gchar *rol;
	gchar *flipx;
	gchar *flipy;
	gchar *fullscreen;
	gchar *keepaspect;
	gchar *displayaspectratio;

	xmame_get_options(exec);
	
	dirty = xmame_get_boolean_option_string(exec, "dirty", target->dirty);
	brightness = xmame_get_float_option_string(exec, "brightness", target->brightness, float_buf);

	/* FIXME: Brightness for older xmame used percent
	brightness = xmame_get_int_option_string(exec, "brightness", (int)(((target->brightness)-0.5)*(100/1.5)));
	*/
		
	bpp = xmame_get_int_option_string(exec, "bpp", target->bpp);
	arbheight = xmame_get_int_option_string(exec, "arbheight", target->arbheight);
	heightscale = xmame_get_int_option_string(exec, "heightscale", target->heightscale);	
	widthscale = xmame_get_int_option_string(exec, "widthscale", target->widthscale);
	gamma_correction = xmame_get_float_option_string(exec, "gamma-correction", target->gamma_correction, float_buf);
	effect = xmame_get_int_option_string(exec, "effect", target->effect);
	effect_name = xmame_get_option_string(exec, "effect", target->effect_name);
	autodouble = xmame_get_boolean_option_string(exec, "autodouble", target->autodouble);
	scanlines = xmame_get_boolean_option_string(exec, "scanlines", target->scanlines);
	frameskipper = xmame_get_int_option_string(exec, "frameskipper", target->frameskipper);
	throttle = xmame_get_boolean_option_string(exec, "throttle", target->throttle);
	sleepidle = xmame_get_boolean_option_string(exec, "sleepidle", target->sleepidle);
	autoframeskip = xmame_get_boolean_option_string(exec, "autoframeskip", target->autoframeskip);
	maxautoframeskip = xmame_get_int_option_string(exec, "maxautoframeskip", target->maxautoframeskip);
	frameskip = xmame_get_int_option_string(exec, "frameskip", target->frameskip);
	norotate = xmame_get_boolean_option_string(exec, "norotate", target->norotate);
	ror = xmame_get_boolean_option_string(exec, "ror", target->ror);
	rol = xmame_get_boolean_option_string(exec, "rol", target->rol);
	flipx = xmame_get_boolean_option_string(exec, "flipx", target->flipx);
	flipy = xmame_get_boolean_option_string(exec, "flipy", target->flipy);
	fullscreen = xmame_get_boolean_option_string(exec, "fullscreen", target->fullscreen);
	keepaspect = xmame_get_boolean_option_string(exec, "keepaspect", target->keepaspect);
	displayaspectratio = xmame_get_float_option_string(exec, "displayaspectratio", target->displayaspectratio, float_buf);

	option_string = g_strjoin (" ",
				fullscreen?fullscreen:"",
				bpp?bpp:"",
				arbheight?arbheight:"",
				heightscale?heightscale:"",
				widthscale?widthscale:"",
				effect?effect:"",
				effect_name?effect_name:"",
				autodouble?autodouble:"",
				(dirty)?dirty:"",
				scanlines?scanlines:"",
				frameskipper?frameskipper:"",
				throttle?throttle:"",
				sleepidle?sleepidle:"",
				autoframeskip?autoframeskip:"",
				maxautoframeskip?maxautoframeskip:"",
				frameskip?frameskip:"",
				(brightness)?brightness:"",
				gamma_correction?gamma_correction:"",
				norotate?norotate:"",
				ror?ror:"",
				rol?rol:"",
				flipx?flipx:"",
				flipy?flipy:"",
				keepaspect?keepaspect:"",
				displayaspectratio?displayaspectratio:"",
				NULL
				);

	g_free(fullscreen);
	g_free(effect);
	g_free(effect_name);
	g_free(bpp);
	g_free(arbheight);
	g_free(heightscale);
	g_free(widthscale);
	g_free(dirty);
	g_free(brightness);
	g_free(gamma_correction);
	g_free(autodouble);
	g_free(scanlines);
	g_free(frameskipper);
	g_free(throttle);
	g_free(sleepidle);
	g_free(autoframeskip);
	g_free(maxautoframeskip);
	g_free(frameskip);
	g_free(norotate);
	g_free(ror);
	g_free(rol);
	g_free(flipx);
	g_free(flipy);
	g_free(keepaspect);
	g_free(displayaspectratio);

	return option_string;
}


static char *create_sound_options_string(XmameExecutable *exec, GameOptions *target)
{
	char *option_string,
	     *alsacard=NULL,
	     *alsadevice=NULL,
	     *alsa_pcm=NULL,
	     *alsa_buffer=NULL,
	     *audio_preferred,
	     *artsBufferTime=NULL,
	     *audiodevice,
	     *mixerdevice,
	     *dsp_plugin,
	     *sound_mixer_plugin, 
	     *sound, 
	     *samples, 
	     *fakesound, 
	     *samplefreq, 
	     *bufsize,
	     *volume;

	char float_buf[FLOAT_BUF_SIZE];
	
	xmame_get_options(exec);
	
	if(!g_ascii_strcasecmp(target->dsp_plugin,"waveout"))
		audiodevice = xmame_get_option_string(exec, "audiodevice", target->soundfile);
	else
		audiodevice = xmame_get_option_string(exec, "audiodevice", target->audiodevice);
	
	mixerdevice = xmame_get_option_string(exec, "mixerdevice", target->mixerdevice);
	

	alsacard = xmame_get_int_option_string(exec, "alsacard", target->alsacard);
	alsadevice = xmame_get_int_option_string(exec, "alsadevice", target->alsadevice);
	alsa_pcm = xmame_get_option_string(exec, "alsa-pcm", target->alsa_pcm);
	alsa_buffer = xmame_get_int_option_string(exec, "alsa-buffer", target->alsa_buffer);
	
	audio_preferred = xmame_get_boolean_option_string(exec, "audio_preferred", target->audio_preferred);
	artsBufferTime = xmame_get_int_option_string(exec, "artsBufferTime", target->artsBufferTime);
	dsp_plugin = xmame_get_option_string(exec, "dsp-plugin", target->dsp_plugin);	
	sound_mixer_plugin = xmame_get_option_string(exec, "sound-mixer-plugin", target->sound_mixer_plugin);
	sound = xmame_get_boolean_option_string(exec, "sound", target->sound);
	samples = xmame_get_boolean_option_string(exec, "samples", target->samples);
	fakesound = xmame_get_boolean_option_string(exec, "fakesound", target->fakesound);
	samplefreq = xmame_get_int_option_string(exec, "samplefreq", target->samplefre);
	bufsize = xmame_get_float_option_string(exec, "bufsize", target->bufsize, float_buf);
	volume = xmame_get_int_option_string(exec, "volume", target->volume);

	option_string = g_strjoin (" ",
				sound?sound:"",
				samples?samples:"",
				fakesound?fakesound:"",
				samplefreq?samplefreq:"",
				bufsize?bufsize:"",
				volume?volume:"",
				audiodevice?audiodevice:"",		/* audiodevice */
				mixerdevice?mixerdevice:"",		/* mixerdevice */
				artsBufferTime?artsBufferTime:"",	/* artsBufferTime */
				alsacard?alsacard:"",			/* alsacard */
				alsadevice?alsadevice:"",		/* alsadevice */
				alsa_pcm?alsa_pcm:"",			/* alsa_pcm */
				alsa_buffer?alsa_buffer:"",		/* alsa_buffer */
				audio_preferred?audio_preferred:"",	/* QNX audio_preferred */
				dsp_plugin?dsp_plugin:"",		/* dsp_plugin */
				sound_mixer_plugin?sound_mixer_plugin:"", /* sound_mixer_plugin */
				NULL
				);

	g_free(audiodevice);
	g_free(mixerdevice);
	g_free(artsBufferTime);
	g_free(alsacard);
	g_free(alsadevice);
	g_free(alsa_pcm);
	g_free(alsa_buffer);
	g_free(audio_preferred);
	g_free(dsp_plugin);
	g_free(sound_mixer_plugin);
	g_free(sound);
	g_free(samples);
	g_free(fakesound);
	g_free(samplefreq);
	g_free(bufsize);
	g_free(volume);	

	return option_string;
}

static char *create_renderer_options_string(XmameExecutable *exec, GameOptions *target)
{
	char *option_string=NULL;
	
	xmame_get_options(exec);
	
	switch(exec->type)
	{
		case XMAME_EXEC_X11:
		case XMAME_EXEC_XGL:
			{
				gchar *vidix;
				gchar *xvext;
				gchar *geometry;
				gchar *mitshm;
				gchar *xil;
				gchar *mtxil;
				gchar *cursor;
				gchar *xsync;
				gchar *yuv;
				gchar *x11_mode;
				gchar *privatecmap;

				/* XGL */
				gchar *gllibname;
				gchar *glulibname;
				gchar *cabinet;
				gchar *glres;
				gchar *forceblitmode;
				gchar *glext78;
				gchar *gldrawbitmap;
				gchar *glcolormod;
				gchar *gldblbuffer;
				gchar *gltexture_size;
				gchar *glbilinear;
				gchar *glalphablending;
				gchar *glantialias;
				gchar *cabview;

				mitshm = xmame_get_boolean_option_string(exec, "mitshm", target->mitshm);
				xil = xmame_get_boolean_option_string(exec, "xil", target->xil);
				mtxil = xmame_get_boolean_option_string(exec, "mtxil", target->mtxil);
				cursor = xmame_get_boolean_option_string(exec, "cursor", target->cursor);
				xsync = xmame_get_boolean_option_string(exec, "xsync", target->xsync);
				privatecmap = xmame_get_boolean_option_string(exec, "privatecmap", target->privatecmap);

				/* we should not try to xvext if mitshm is not used */
				xvext = xmame_get_boolean_option_string(exec, "xvext", target->xvext && !target->mitshm);
				vidix = xmame_get_boolean_option_string(exec, "vidix", target->vidix);
				yuv = xmame_get_int_option_string(exec, "force-yuv", target->force_yuv);
				
				x11_mode = xmame_get_int_option_string(exec, "video-mode", target->x11_mode);

				if (target->xvgeom_flag)
					geometry = xmame_get_option_string(exec, "geometry", target->geometry);
				else
					geometry = NULL;

				forceblitmode = xmame_get_boolean_option_string(exec, "glforceblitmode", target->glforceblitmode);
				glext78 = xmame_get_boolean_option_string(exec, "glext78", target->glext78);
				gldrawbitmap = xmame_get_boolean_option_string(exec, "gldrawbitmap", target->gldrawbitmap);
				glcolormod = xmame_get_boolean_option_string(exec,	"glcolormod", target->glcolormod);
				gllibname = xmame_get_option_string(exec, "gllibname", target->gllibname);				
				glulibname = xmame_get_option_string(exec, "glulibname", target->glulibname);
				cabinet = xmame_get_option_string(exec, "cabinet", target->cabinet);
				gldblbuffer = xmame_get_boolean_option_string(exec, "gldblbuffer", target->gldblbuffer);
				gltexture_size = xmame_get_int_option_string(exec, "gltexture_size", target->gltexture_size);
				glbilinear = xmame_get_boolean_option_string(exec, "glbilinear", target->glbilinear);
				glalphablending = xmame_get_boolean_option_string(exec, "glalphablending", target->glalphablending);
				glantialias = xmame_get_boolean_option_string(exec, "glantialias", target->glantialias);
				cabview = xmame_get_boolean_option_string(exec, "cabview", target->cabview);

				if (target->glres_flag)
					glres = xmame_get_option_string(exec, "glres", target->glres);
				else
					glres = NULL;

				option_string = g_strjoin (" ",
						geometry?geometry:"",
						yuv?yuv:"",
						x11_mode?x11_mode:"",
						cursor?cursor:"",
						mitshm?mitshm:"",
						xvext?xvext:"",
						vidix?vidix:"",
						xsync?xsync:"",
						privatecmap?privatecmap:"",
						xil?xil:"",
						mtxil?mtxil:"",
						gldblbuffer?gldblbuffer:"",
						gltexture_size?gltexture_size:"",
						forceblitmode?forceblitmode:"",
						glext78?glext78:"",
						glbilinear?glbilinear:"",
						gldrawbitmap?gldrawbitmap:"",
						glcolormod?glcolormod:"",
						glalphablending?glalphablending:"",
						glantialias?glantialias:"",
						gllibname?gllibname:"",
						glulibname?glulibname:"",
						cabview?cabview:"",
						cabinet?cabinet:"",
						glres?glres:"",
						NULL
					);
				
				g_free(vidix);
				g_free(xvext);
				g_free(geometry);
				g_free(mitshm);
				g_free(xil);
				g_free(mtxil);
				g_free(cursor);
				g_free(xsync);
				g_free(yuv);
				g_free(x11_mode);
				g_free(privatecmap);

				g_free(gllibname);
				g_free(glulibname);
				g_free(cabinet);
				g_free(glres);
				g_free(forceblitmode);
				g_free(glext78);
				g_free(gldrawbitmap);
				g_free(glcolormod);
				g_free(gldblbuffer);
				g_free(gltexture_size);
				g_free(glbilinear);
				g_free(glalphablending);
				g_free(glantialias);
				g_free(cabview);
			}
			break;
		case XMAME_EXEC_SDL:
			{
				gchar *SDL_mode=NULL;
				gchar *doublebuf_opt;
				gchar **modenumber_tokens;
				const gchar *sdl_mode_string;
				int i;

				if (!(target->sdl_auto_mode)) {
					if (xmame_has_option(exec, "vidmode_h") && xmame_has_option(exec, "vidmode_w")) {
					
						sdl_mode_string = xmame_get_option_value(current_exec, "modenumber", target->modenumber);
														
						if (sdl_mode_string) {
							modenumber_tokens = g_strsplit(sdl_mode_string, "x", 0);
							for (i =0; modenumber_tokens[i]; i++) {
								GMAMEUI_DEBUG("%i: %s", i, modenumber_tokens[i]);
							}
							SDL_mode = g_strdup_printf("-vidmode_w %s -vidmode_h %s", modenumber_tokens[0], modenumber_tokens[1]);
							g_strfreev(modenumber_tokens);		
						}	

					} else {
						SDL_mode = xmame_get_int_option_string(exec, "modenumber", target->modenumber);
					}
				}

				doublebuf_opt = xmame_get_boolean_option_string(exec, "doublebuf", target->sdl_doublebuf);
				
				option_string = g_strjoin (" ",
					doublebuf_opt?doublebuf_opt:"",
					SDL_mode?SDL_mode:"",
					NULL
					);

				g_free(SDL_mode);
				g_free(doublebuf_opt);
			}
			break;

#ifdef ENABLE_XMAME_SVGALIB_SUPPORT
		case XMAME_EXEC_SVGALIB:
			{
				gchar *centerx;
				gchar *centery;
				gchar *tweak;
				gchar *planar;
				gchar *linear;

				centerx = xmame_get_int_option_string(exec, "centerx", target->centerx);
				centery = xmame_get_int_option_string(exec, "centery", target->centery);
				tweak = xmame_get_boolean_option_string(exec, "tweak", target->tweak);
				planar = xmame_get_boolean_option_string(exec, "planar", target->planar);
				linear = xmame_get_boolean_option_string(exec, "linear", target->linear);
	
				option_string = g_strjoin (" ",
						tweak?tweak:"",
						planar?planar:"",
						linear?linear:"",
						centerx?centerx:"",
						centery?centery:"",
						NULL
						);

				g_free(centerx);
				g_free(centery);
				g_free(tweak);
				g_free(planar);
				g_free(linear);
			}
			break;
#endif
		case XMAME_EXEC_XFX:
		case XMAME_EXEC_SVGAFX:
			{
				gchar *resolution;
				gchar *keepaspect;

				resolution = xmame_get_option_string(exec, "resolution", target->resolution);
				keepaspect = xmame_get_boolean_option_string(exec, "keepaspect", target->fxgkeepaspect);

				option_string = g_strjoin (" ",
						resolution?resolution:"",
						keepaspect?keepaspect:"",
						NULL
						);

				g_free(resolution);
				g_free(keepaspect);
			}
			break;
		case XMAME_EXEC_GGI:
			{
				gchar *linear;
				gchar *resolution=NULL;

				linear = xmame_get_boolean_option_string(exec, "linear", target->ggilinear);

				if(target->ggi_force_resolution && xmame_has_option(exec, "xres"))
				{
					resolution = g_strdup_printf("-%s %i -%s %i",
					                             xmame_get_option_name(exec, "xres"),
					                             target->xres,
					                             xmame_get_option_name(exec, "yres"),
                                                 target->yres);
				}
				option_string = g_strjoin (" ",
						linear?linear:"",
						resolution?resolution:"",
						NULL
						);

				g_free(resolution);
				g_free(linear);
			}
			break;
		case XMAME_EXEC_PHOTON2:
			{
				gchar *render_mode;
				gchar *phcursor;

				render_mode = xmame_get_int_option_string(exec, "render-mode", target->render_mode);
				phcursor = xmame_get_boolean_option_string(exec, "phcursor", target->phcursor);

				option_string = g_strjoin (" ",
					render_mode?render_mode:"",
					phcursor?phcursor:"",
					NULL
					);

				g_free(render_mode);
				g_free(phcursor);
			}
			break;
		default:
			/* at least give an empty line */
			option_string = g_strdup("");
	}
	return option_string;
}

static char *create_input_options_string(XmameExecutable *exec, GameOptions *target)
{
	char *option_string, *joy=NULL,
			*keymap=NULL,
			*trackball1=NULL,
			*trackball2=NULL,
			*trackball3=NULL,
			*trackball4=NULL,
			*ctrlr;
	gchar *hotrod;
	gchar *hotrodse;	
	gchar *joytype;
	gchar *analogstick;
	gchar *mouse;
	gchar *usbpspad;
	gchar *grabkeyboard;
	gchar *rapidfire;
	gchar *ugcicoin;
	gchar *grabmouse;
	gchar *winkeys;

	xmame_get_options(exec);

	switch(target->joytype) {
		case 1:
		case 4:
			joy = xmame_get_option_string(exec, "joydevname", target->joydevname);
			break;
		case 2:
			joy = xmame_get_option_string(exec, "paddevname", target->paddevname);
		case 3:
			joy = xmame_get_option_string(exec, "x11joyname", target->x11joyname);
	}
	
	/*
	if (available_options->XInput_trackball)
	{
		if (strcmp(g_strstrip(target->XInput_trackball1),""))
		{
			trackball1 = g_strdup_printf ("- %s",
						target->XInput_trackball1);
		}
		if (strcmp(g_strstrip(target->XInput_trackball2),""))
		{
			trackball2 = g_strdup_printf ("- %s",
						target->XInput_trackball2);
		}
		if (strcmp(g_strstrip(target->XInput_trackball3),""))
		{
			trackball3 = g_strdup_printf ("- %s",
						target->XInput_trackball3);
		}
		if (strcmp(g_strstrip(target->XInput_trackball4),""))
		{
			trackball4 = g_strdup_printf ("- %s",
						target->XInput_trackball4);
		}
	}
	*/

	if (xmame_has_option(exec, "mapkey")) {
		int i;
		
		/* Find keymap */
		for (i=0; x11_keymaps_layout[i].shortname; i++)
		{

			/* Append all keys */
			if (!strcmp(x11_keymaps_layout[i].shortname,target->keymap))
			{
				gchar *mapkey_option;
				gchar *oldkeymap;
				struct x11_keymap_layout *keys;

				keys = x11_keymaps_layout[i].keys;
				mapkey_option = g_strdup_printf("-%s", xmame_get_option_name(exec, "mapkey"));

				while (keys->from)
				{
					oldkeymap = keymap;
					keymap = g_strdup_printf("%s %s 0x%x,0x%x",
						                oldkeymap?oldkeymap:"",
						                mapkey_option,
						                keys->from,
						                keys->to);
					g_free(oldkeymap);
					keys++;
				}
				g_free(mapkey_option);
				break;
			}
		}
	}

	if (strcmp(target->ctrlr,"None"))
		ctrlr = xmame_get_option_string(exec, "ctrlr", target->ctrlr);
	else
		ctrlr = NULL;

	grabmouse = xmame_get_boolean_option_string(exec, "grabmouse", target->grabmouse);
	winkeys = xmame_get_boolean_option_string(exec, "winkeys", target->winkeys);
	hotrod = xmame_get_boolean_option_string(exec, "hotrod", target->hotrod);
	hotrodse = xmame_get_boolean_option_string(exec, "hotrodse", target->hotrodse);
	joytype = xmame_get_int_option_string(exec, "joytype", target->joytype);
	analogstick = xmame_get_boolean_option_string(exec, "analogstick", target->analogstick);
	mouse = xmame_get_boolean_option_string(exec, "mouse", target->mouse);
	usbpspad = xmame_get_boolean_option_string(exec, "usbpspad", target->usbpspad);
	grabkeyboard = xmame_get_boolean_option_string(exec, "grabkeyboard", target->grabkeyboard);
	rapidfire = xmame_get_boolean_option_string(exec, "rapidfire", target->rapidfire);
	ugcicoin = xmame_get_boolean_option_string(exec, "ugcicoin", target->ugcicoin);	

	option_string = g_strjoin (" ",
				joytype?joytype:"",
				analogstick?analogstick:"",
				joy?joy:"",
				mouse?mouse:"",
				ctrlr?ctrlr:"",
				usbpspad?usbpspad:"",
				rapidfire?rapidfire:"",
				trackball1?trackball1:"",
				trackball2?trackball2:"",
				trackball3?trackball3:"",
				trackball4?trackball4:"",
				grabmouse?grabmouse:"",
				grabkeyboard?grabkeyboard:"",
				winkeys?winkeys:"",
				keymap?keymap:"",
				ugcicoin?ugcicoin:"",
				NULL
				);

	g_free(ctrlr);
	g_free(joy);
	g_free(trackball1);
	g_free(trackball2);
	g_free(trackball3);
	g_free(trackball4);
	g_free(keymap);
	g_free(hotrod);
	g_free(hotrodse);
	g_free(joytype);
	g_free(analogstick);
	g_free(mouse);
	g_free(usbpspad);
	g_free(grabkeyboard);
	g_free(rapidfire);
	g_free(ugcicoin);
	g_free(grabmouse);
	g_free(winkeys);

	return option_string;
}

static char *create_misc_options_string(XmameExecutable *exec, GameOptions *target)
{
	char *option_string, *artwork=NULL,
			*keyboard_leds=NULL,
			*debug=NULL,
			*network=NULL,
			*log=NULL,
			*cfgname=NULL,
			*additional_options=NULL,
			*skip_disclaimer=NULL,
			*skip_gameinfo=NULL,
			*bios;

	gchar *debug_size;
	gchar *cheat;

	xmame_get_options(exec);

	if (target->artwork) {
		gchar *use_backdrops;
		gchar *use_overlays;
		gchar *use_bezels;
		gchar *artwork_crop;
		gchar *artwork_resolution;
		gchar *use_artwork;

		use_artwork = xmame_get_boolean_option_string(exec, "artwork", target->artwork);
		use_backdrops = xmame_get_boolean_option_string(exec, "use_backdrops", target->use_backdrops);	
		use_overlays = xmame_get_boolean_option_string(exec, "use_overlays", target->use_overlays);	
		use_bezels = xmame_get_boolean_option_string(exec, "use_bezels", target->use_bezels);
		artwork_crop = xmame_get_boolean_option_string(exec, "artwork_crop", target->artwork_crop);		
		artwork_resolution = xmame_get_int_option_string(exec, "artwork_resolution", target->artwork_resolution);

		artwork = g_strjoin (" ",
					use_artwork?use_artwork:"",
					use_backdrops?use_backdrops:"",
					use_overlays?use_overlays:"",
					use_bezels?use_bezels:"",
					artwork_crop?artwork_crop:"",
					artwork_resolution?artwork_resolution:"",
					NULL
					);

		g_free(use_backdrops);
		g_free(use_overlays);
		g_free(use_bezels);
		g_free(artwork_crop);
		g_free(artwork_resolution);
		g_free(use_artwork);
	} 

	keyboard_leds = xmame_get_boolean_option_string(exec, "keyboard_leds", target->keyboard_leds);
	skip_disclaimer = xmame_get_boolean_option_string(exec, "skip_disclaimer", target->skip_disclaimer);
	skip_gameinfo = xmame_get_boolean_option_string(exec, "skip_gameinfo", target->skip_gameinfo);
	debug = xmame_get_boolean_option_string(exec, "debug", target->debug);
	debug_size = xmame_get_option_string(exec, "debug-size", target->debug_size);

	if(target->log_flag)
	{
		if(strcmp(g_strstrip(target->log),""))
			log = xmame_get_option_string(exec, "log", target->log);
		else
			log = xmame_get_option_string(exec, "log", "xmame.log");
	}

	/* for the cfg_directory option, if available, see in io options */
	if (target->cfgname_flag)
	{
		cfgname = xmame_get_option_string(exec, "cfgname", target->cfgname);
		if (!cfgname && strcmp(g_strstrip(target->cfgname),"")) {
			gchar *full_path =  g_strdup_printf("%s" G_DIR_SEPARATOR_S "%s",gui_prefs.ConfigDirectory, target->cfgname);
			cfgname = xmame_get_option_string(exec, "cfg_directory", full_path);

			g_free(full_path);
		}
	} else {
		cfgname = xmame_get_option_string(exec, "cfg_directory", gui_prefs.ConfigDirectory);
	}

	
	if (target->use_additional_options)
	{
		if(strcmp(g_strstrip(target->additional_options),""))
			additional_options = g_strdup_printf ("%s",target->additional_options);
	} 
	
	bios = xmame_get_int_option_string(exec, "bios", target->bios);
	cheat = xmame_get_boolean_option_string(exec, "cheat", target->cheat);

	option_string = g_strjoin (" ",
				artwork?artwork:"",
				cheat?cheat:"",
				keyboard_leds?keyboard_leds:"",
				debug?debug:"",
				network?network:"",
				log?log:"",
				cfgname?cfgname:"",
				additional_options?additional_options:"",
				skip_disclaimer?skip_disclaimer:"",
				skip_gameinfo?skip_gameinfo:"",
				bios?bios:"",
				NULL
				);
				
	g_free(artwork);
	g_free(cheat);
	g_free(keyboard_leds);
	g_free(debug);
	g_free(network);
	g_free(log);
	g_free(cfgname);
	g_free(skip_gameinfo);
	g_free(skip_disclaimer);
	g_free(bios);	

	return option_string;
}

char *create_vector_options_string(XmameExecutable *exec, GameOptions *target)
{
	gchar *option_string;
	gchar *vectorres;
	gchar *intensity;
	char float_buf[FLOAT_BUF_SIZE];
	gchar *flicker;
	gchar *beam;
	gchar *antialias;
	gchar *translucency;
	gchar *gldrawbitmapvec;
	gchar *glantialiasvec;
	gchar *glbeam;

	xmame_get_options(exec);
		
	if(target->vectorres_flag)
		vectorres = xmame_get_option_string(exec, "vectorres", target->vectorres);
	else
		vectorres = NULL;

	intensity = xmame_get_float_option_string(exec, "intensity", target->intensity, float_buf);
	flicker = xmame_get_float_option_string(exec, "flicker", target->flicker, float_buf);
	beam = xmame_get_float_option_string(exec, "beam", target->beam, float_buf);
	antialias = xmame_get_boolean_option_string(exec, "antialias", target->antialias);
	translucency = xmame_get_boolean_option_string(exec, "translucency", target->translucency);

	gldrawbitmapvec = xmame_get_boolean_option_string(exec, "gldrawbitmapvec", target->gldrawbitmapvec);
	glantialiasvec = xmame_get_boolean_option_string(exec, "glantialiasvec", target->glantialiasvec);
	glbeam = xmame_get_float_option_string(exec, "glbeam", target->beam, float_buf);

	option_string = g_strjoin (" ",
				antialias?antialias:"",
				translucency?translucency:"",
				flicker?flicker:"",
				beam?beam:"",
				vectorres?vectorres:"",
				intensity?intensity:"",
				gldrawbitmapvec?gldrawbitmapvec:"",
				glantialiasvec?glantialiasvec:"",
				glbeam?glbeam:"",
				NULL
				);

	g_free(intensity);
	g_free(vectorres);
	g_free(beam);
	g_free(flicker);
	g_free(antialias);
	g_free(translucency);
	g_free(gldrawbitmapvec);
	g_free(glantialiasvec);
	g_free(glbeam);

	return option_string;
}

static char *create_io_options_string(XmameExecutable *exec)
{
	char *option_string;
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

	artworkpath_option = xmame_get_option_string(exec, "artwork_directory", gui_prefs.ArtworkDirectory);
	screenshotdir_option = xmame_get_option_string(exec, "snapshot_directory", gui_prefs.SnapshotDirectory);
 	hiscoredir_option = xmame_get_option_string(exec, "hiscore_directory", gui_prefs.HiscoreDirectory);
	cheatfile_option = xmame_get_option_string(exec, "cheat_file", gui_prefs.CheatFile);
	hiscorefile_option = xmame_get_option_string(exec, "hiscore_file", gui_prefs.HiscoreFile);	
	historyfile_option = xmame_get_option_string(exec, "history_file", gui_prefs.HistoryFile);
	mameinfofile_option = xmame_get_option_string(exec, "mameinfo_file", gui_prefs.MameInfoFile);
	diffdir_option = xmame_get_option_string(exec, "diff_directory", gui_prefs.DiffDirectory);
	
	/* for this option, see in the input option as this option can change if cfname has bee set or not */
/*	if (available_options.ConfigDirectory  && (strlen(gui_prefs.ConfigDirectory)>0))
	{
		escaped_path = escape_space(gui_prefs.ConfigDirectory);
		cfg_directory_option = g_strdup_printf("-cfg_directory %s",escaped_path);
		g_free(escaped_path);
 }
	else
*/		cfg_directory_option = g_strdup("");

	inipath_option = xmame_get_option_string(exec, "inipath", gui_prefs.inipath);
	nvram_directory_option = xmame_get_option_string(exec, "nvram_directory", gui_prefs.NVRamDirectory);
	ctrlr_directory_option = xmame_get_option_string(exec, "ctrlr_directory", gui_prefs.CtrlrDirectory);
	state_directory_option = xmame_get_option_string(exec, "state_directory", gui_prefs.StateDirectory);
	input_directory_option = xmame_get_option_string(exec, "input_directory", gui_prefs.InputDirectory);
	memcard_directory_option = xmame_get_option_string(exec, "memcard_directory", gui_prefs.MemCardDirectory);

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
	
	return option_string;
}

gchar *create_rompath_options_string(XmameExecutable *exec)
{
	gchar *rompath_option_string;
	gchar *rompath;
	gchar *samplepath_option_string;
	gchar *samplepath;
	gchar *option_string;
	
	xmame_get_options(exec);

	/* G_SEARCHPATH_SEPARATOR_S returns ":" on Unix, but newer versions,
	   particularly for sdlmame, require ";" */
	if (!g_strcasecmp (exec->name, "xmame")) {
		rompath = g_strjoinv(G_SEARCHPATH_SEPARATOR_S, gui_prefs.RomPath);
		samplepath = g_strjoinv(G_SEARCHPATH_SEPARATOR_S, gui_prefs.SamplePath);
	} else {
		rompath = g_strjoinv(";", gui_prefs.RomPath);
		samplepath = g_strjoinv(";", gui_prefs.SamplePath);
	}
GMAMEUI_DEBUG ("Rompath is %s", rompath);
	rompath_option_string = xmame_get_option_string(exec, "rompath", rompath);
	samplepath_option_string = xmame_get_option_string(exec, "samplepath", samplepath);

	g_free(rompath);
	g_free(samplepath);

	option_string = g_strjoin(" ",
		rompath_option_string?rompath_option_string:"",
		samplepath_option_string?samplepath_option_string:"",
		NULL);

	g_free(rompath_option_string);
	g_free(samplepath_option_string);

	if (!option_string)
		option_string = g_strdup("");

	return option_string;
}

char *create_options_string(XmameExecutable *exec, GameOptions *target) {
	gchar *opt;
	gchar *rompath_options;
	gchar *io_options;
	gchar *video_options;
	gchar *renderer_options;
	gchar *sound_options;
	gchar *input_options;
	gchar *misc_options;

	xmame_get_options(exec);

	rompath_options = create_rompath_options_string(exec);
	io_options = create_io_options_string(exec);
	video_options = create_video_options_string(exec, target);
	renderer_options = create_renderer_options_string(exec, target);
	sound_options = create_sound_options_string(exec, target);
	input_options = create_input_options_string(exec, target);
	misc_options = create_misc_options_string(exec, target);
	
	opt = g_strjoin(" ",
			rompath_options,
			io_options,
			video_options,
			renderer_options,
			sound_options,
			input_options,
			misc_options,
			NULL
		);

	g_free(rompath_options);
	g_free(io_options);
	g_free(video_options);
	g_free(renderer_options);
	g_free(sound_options);
	g_free(input_options);
	g_free(misc_options);

	return opt;
}
