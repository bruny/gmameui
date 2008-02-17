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

#ifndef __GAME_OPTIONS_H__
#define __GAME_OPTIONS_H__

#include "game_list.h"

typedef struct {
	/*** Video Related ***/
	gboolean fullscreen;        /* Start fullscreen  */
	guint bpp;                  /* Specify the colordepth the core should render,one of:
                                   auto(0), 8, 16 */
	guint arbheight;            /* Scale video to exactly this height (0 =
	                               disable) (0-4096)*/
	guint heightscale;          /* Set Y-Scale aspect ratio (0-8)*/
	guint widthscale;           /* Set X-Scale aspect ratio (0-8)*/
	gfloat scale;               /* Set X-Y Scale to the same aspect ratio. For
	                               vector games scale (and also width- and
	                               heightscale) may have value's like 1.5 and even
	                               0.5. For scaling of regular games this will be
	                               rounded to an int*/
	guint effect;               /* Video effect */
	gchar* effect_name;         /* Video effect - replaces effect from version XXX */
	gboolean autodouble;		/* Enable/disable automatic scale doubling for 1:2
					               pixel aspect ratio games*/
	gboolean dirty;             /* Enable/disable use of dirty rectangles*/
	gboolean scanlines;         /* Enable/disable displaying simulated scanlines*/
	gboolean artwork;           /* Use/don't use artwork if available*/
	gboolean use_backdrops;		/* use backdrop artwork */
	gboolean use_overlays;		/* use overlay artwork */
	gboolean use_bezels;		/* use bezel artwork */
	gboolean artwork_crop;		/* crop artwork to game screen only */
	guint artwork_resolution;   /* artwork resolution (0 for auto) */
	guint frameskipper;         /* Select which frameskipper to use */
	gboolean throttle;          /* Enable/disable throttle*/
	gboolean sleepidle;         /* Enable/disable sleep during idle*/
	gboolean autoframeskip;     /* Enable/disable autoframeskip*/
	guint maxautoframeskip;     /* Set highest allowed frameskip for autoframeskip*/
	guint frameskip;            /* Set frameskip when not using autoframeskip (0 - 11)*/
	gfloat brightness;          /* Set the brightness (0.5-2.0) (old 0-100%)*/
	gfloat gamma_correction;    /* Set the gamma-correction (0.5-2.0)*/
	gboolean norotate;          /* Disable rotation*/
	gboolean ror;               /* Rotate display 90 degrees rigth*/
	gboolean rol;               /* Rotate display 90 degrees left*/
	gboolean flipx;             /* Flip X axis*/
	gboolean flipy;             /* Flip Y axis*/
	
	/*** Vector Games Related ***/
	gboolean vectorres_flag;	    
	gchar* vectorres;   		/* Always scale vectorgames to XresxYres, keeping    
 			      		   their aspect ratio. This overrides the scale
 			      		   options  */  				    
	gfloat beam;	      		/* Set the beam size for vector games (1 - 15)*/
	gfloat flicker;       		/* Set the flicker for vector games*/		    
	gboolean antialias;   		/* Enable/disable antialiasing*/
	gboolean translucency;		/* Enable/disable tranlucency*/ 		    
	gfloat intensity;		/* set intensity in vector games */
			
	/*display_opts (depend target)*/
	/*** X11 Related ***/
	guint x11_mode;			/* Select x11 video mode */
	gboolean cursor;		/* Show/don't show the cursor */		      
	gboolean mitshm;		/* Use/don't use MIT Shared Mem (if available and  
					   compiled in) */
	gboolean xvext;			/* Use/don't use Xv extension for hardware scaling
	                                   (if available and compiled in)) */			      
	gboolean vidix;			/* Use/don't use VIDIX for hardware scaling (if    
					   available and compiled in))*/
	gint force_yuv;         /* Force YUV mode (for video cards with broken RGB
					   overlays) */
	gboolean xsync;			/* Use/don't use XSync instead of XFlush as screen 
					   refresh method */				      
	gboolean privatecmap;		/* Enable/disable use of private color map */	      
	gboolean xil;			/* Enable/disable use of XIL for scaling (if       
					   available and compiled in) */		      
	gboolean mtxil;			/* Enable/disable multi threading of XIL */	      
	gboolean run_in_root_window;	/* Enable/disable running in root window */	      
	gint root_window_id;		/* Create the xmame-window in an alternate	      
					   root-window, mostly usefull for frontends! */      
	gboolean xvgeom_flag;	        /* use geometry option for xv*/	      
	gchar* geometry;	        /* Specify the location of the window (size with xv)*/	      

	 /*** X11-input related ***/
	 /* (X11 & GL Glide) */
	gboolean grabmouse;		/* Enable/disable mousegrabbing (also alt +        
					   pagedown) */ 				      
	gboolean grabkeyboard;		/* Enable/disable keyboardgrabbing (also alt +
					   pageup) */
	gboolean winkeys;		/* Enable/disable mapping of windowskeys under X */   
	gchar* mapkey;		/* Set a specific key mapping, see xmamerc.dist
					   mapkey 0x0020,0x001d */   

	/*** OpenGL Related ***/
	gboolean gldblbuffer;		/* Enable/disable double buffering (default: true) */
	gint gltexture_size;		/* Force the max width and height of one texture
	 			           segment (default: autosize)  */
	gboolean glforceblitmode;	/* Force blitter for true color modes 15/32bpp
					   (default: true) */
	gboolean glext78;		/* Force the usage of the gl extension #78, if
	 	    			   available (paletted texture, default: true) */
	gboolean glbilinear;		/* Enable/disable bilinear filtering (default:
					   true) */
	gboolean gldrawbitmap;		/* Enable/Disable the drawing of the bitmap - e.g. 
					   disable it within vector games for a speedup
					   (default: true) */
	gboolean gldrawbitmapvec;	/* Enable/Disable the drawing of the bitmap only
					   for vector games - speedup (default: true) */
	gboolean glcolormod;		/* Enable/Disable color modulation
					  (intensity,gamma) (default: true) */
	gfloat glbeam;			/* Set the beam size for vector games (default:
					   1.0) */
	gboolean glalphablending;	/* Enable/disable alphablending if available
	 				  (default: true) */
	gboolean glantialias;		/* Enable/disable antialiasing (default: true) */
	gboolean glantialiasvec;	/* Enable/disable vector antialiasing (default:
					   true)  */
	gchar* gllibname;		/* Choose the dynamically loaded OpenGL Library
	 			                   (default libGL.so.1) */
	gchar* glulibname;		/* Choose the dynamically loaded GLU Library
	                               (default libGLU.so.1) */
	gboolean cabview;           /* Start/Don't start in cabinet view mode
	                               (default: false) */
	gchar* cabinet;		/* Specify which cabinet model to use (default:
	 			           glmamejau) */
	gboolean glres_flag;
	gchar* glres;			/* Always scale games to XresxYres, keeping their
					   aspect ratio. This overrides the scale options */  

	/*** SDL Related ***/
	gboolean sdl_auto_mode;      /* use auto mode (no modenumber option) */
	gint modenumber;             /* Try to use the 'n' possible full-screen mode */
	gchar* sdlmapkey;         /* Set a specific key mapping, see xmamerc.dist */
	gboolean sdl_doublebuf;      /* Double buffering (0.78.1 and above) */
	
	/*** GGI Related ***/
	gboolean ggilinear;          /* Enable/disable use of linear framebuffer (fast) */
	gboolean ggi_force_resolution;
	guint xres;                  /* Force the X resolution */
	guint yres;                  /* Force the Y resolution */

#ifdef ENABLE_XMAME_SVGALIB_SUPPORT
	/*** Svgalib Related ***/
	gboolean tweak;              /* Enable/disable svgalib tweaked video modes */      
	gboolean planar;             /* Enable/disable use of planar (modeX) modes
	                                (slow) */					      
	gboolean linear;             /* Enable/disable use of linear framebuffer (fast)  */
	gint centerx;                /* Adjust the horizontal center of tweaked vga modes */
	gint centery;                /* Adjust the vertical center of tweaked vga modes */ 
#endif

	/*** FX (Glide) Related ***/
	gchar* resolution;		/* Specify the resolution/ windowsize to use in    
					   the form of XRESxYRES */			      
	gboolean fxgkeepaspect;		/* Try / don't try to keep the aspect ratio of a   
					   game */

	/*** Photon Related ***/
	guint render_mode;		/* Select Photon rendering video mode:
						0 Normal window  (hotkey left-alt + insert)
						1 Fullscreen Video Overlay (hotkey left-alt + 
						home) */

	 /*** Photon-window Related ***/
	gboolean phcursor;		/* Show / don't show the cursor. */
	                 
	 /*** Photon-input Related ***/
	gboolean phgrabmouse;		/* Enable/disable mousegrabbing (also alt + 
					  pagedown) */
	gboolean phwinkeys;		/* Enable/disable mapping of windowskeys under 
					   Photon */
	gchar* phmapkey;		/* Set a specific key mapping, see phmamerc.dist */
 
	
	 /*** Video Mode Selection Related ***/
	 /* (SVGA, xf86_dga_opts, GGI) */
	gboolean keepaspect;		/* Try / don't try to keep the aspect ratio of a
					   game when selecting the best videomode */
	gfloat displayaspectratio;	/* Set the display aspect ratio of your monitor.
					   This is used for -keepaspect The default = 1.33
					   (4/3). Use 0.75 (3/4) for a portrait monitor */
	gchar* disablemode;		/*Don't use mode XRESxYRESxDEPTH this can be used
					   to disable specific video modes which don't
					   work on your system. The xDEPTH part of the
					   string is optional. This option may be used
					   more then once */

	/*** Sound Related ***/
	gboolean sound;			/* Enable/disable sound (if available) */
	gboolean samples;		/* Use/don't use samples (if available) */
	gboolean fakesound;		/* Generate sound even when sound is disabled,
					   this is needed for some games which won't run
					   without sound */
	gint samplefre;			/* Set the playback sample-frequency/rate */
	gfloat bufsize;			/* Number of frames of sound to buffer */
	gint volume;			/* Set volume to <int> db, (-32 (soft) - 0(loud) ) */
	gchar* audiodevice;		/* Use an alternative audiodevice */
	gchar* mixerdevice;		/* Use an alternative mixerdevice */
	gchar* soundfile;		/* audiodevice for waveout plugin */

	/*** Input Related ***/
	guint joytype;   /* Select type of joystick support to use:	      
					   0 - No joystick.
                       The rest depends on the compile options
                       and xmame version
					 */
	gboolean analogstick;		/* Use Joystick as analog for analog controls */

	gchar* keymap;		/* Select layout type of keymap (fr,de, ...) */

	gboolean mouse;			/* Enable/disable mouse (if supported) */
	gboolean hotrod;		/* Enable HotRod joystick support */
	gboolean hotrodse;		/* Select HotRod SE joystick support */
	gboolean usbpspad;		/* The Joystick(s) are USB PS Game Pads */
	gboolean rapidfire;		/* Enable rapid-fire support for joysticks */

	gchar* joydevname;		/* Joystick device prefix
					   (defaults to /dev/joy) for bsd
					   (defaults to /dev/js) for linux */

	 /***joy_pad_opts***  *FM townpad*/
	gchar* paddevname;		/* Name of pad device (defaults to /dev/pad00) */
	 /***joy_x11_opts***/
	gchar* x11joyname;		/* Name of X-based joystick device (if compiled in) */
	 /***Xinput***/
	gchar* XInput_trackball1;	/* Device name for trackball of player 1 */
	gchar* XInput_trackball2;	/* Device name for trackball of player 2 */
	gchar* XInput_trackball3;	/* Device name for trackball of player 3 */
	gchar* XInput_trackball4;	/* Device name for trackball of player 4 */
	/* gchar XInput_joystick1[20]; */      /* Device name for joystick of player 1 */
	/* gchar XInput_joystick2[20]; */      /* Device name for joystick of player 2 */
	/* gchar XInput_joystick3[20]; */      /* Device name for joystick of player 3 */
	/* gchar XInput_joystick4[20]; */      /* Device name for joystick of player 4 */
	gchar* ctrlr;		/* Controller Name */

	/*** Digital sound related ***/
	gchar* dsp_plugin;		/* Select which plugin to use for digital sound    
	list-dsp-plugins		   List available sound-dsp plugins */
	gboolean timer;			/* Use / don't use timer based audio (normally it
					   will be used automagically when nescesarry) */
	
	/*** Sound mixer related ***/
	gchar* sound_mixer_plugin;	/* Select which plugin to use for the sound mixer
	list-mixer-plugins		   List available sound-mixer plugins */
	
	/***arts***/
	gint artsBufferTime;		/* aRts buffer delay time */

	 /*** Alsa Sound System 0.5***/
	guint alsacard;			/* select card # or card id (0-32) */
	guint alsadevice;		/* select device # */
	        	       
	/*** Alsa Sound System 0.9***/
	gchar* alsa_pcm;	/* Specify the PCM by name */
	guint alsa_buffer;		/* Set the buffer size [micro sec] (default: 250000) */
	        	       
	/*** QNX Audio related ***/
	gboolean audio_preferred;	/* Use the preferred device or use the primary
					   device. */

	/*** Display  Related ***/
	gchar *display_plugin;	/* Select which plugin to use for the display
	list-display-plugins		   available display plugins */
	gfloat display_aspect_ratio;	/* Set the aspect ratio of your monitor, this is   
					   used for mode selection calculations. Usually   
					   this is 4/3 (1.33) in some cases this is 3/4    
					   (0.75) or even 16/9 (1.77) (0.5 - 2.0)*/
	gboolean keep_aspect;		/* Try / don't try to keep the correct aspect ratio
					   when selecting the best mode */
	
	/*** Misc ***/
	gboolean cheat;
	gboolean keyboard_leds;
	gboolean debug;
	gchar* debug_size;
	gboolean log_flag;
	gchar* log;
	gboolean cfgname_flag;
	gchar *cfgname;			/* Set the config name in case you use several
					               control panels */
	gboolean use_additional_options;
	gchar *additional_options;
	
	gboolean skip_disclaimer;   /* Skip the disclaimer info */
	gboolean skip_gameinfo;     /* Skip the game info */
	
	gint bios;                  /* Change system bios */
	gboolean ugcicoin;          /* Enable/disable UGCI(tm) Coin/Play support */	
	
} GameOptions;


/** Default game options
* They are always loaded because they are used the most.
* Per game options are loaded on demand
*/
GameOptions default_options;

GameOptions *
load_options(RomEntry *rom);

gboolean
save_options(RomEntry *rom, GameOptions *opts);

void game_options_free(GameOptions *opts);

#endif
