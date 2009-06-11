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
 * Code based on:
 * gtkjoy.c - general routines for handling joystick events
 * Copyright 1999 David Boynton.
 */
#ifndef __GTKJOY_H__
#define __GTKJOY_H__

#include <glib.h>

typedef struct {
	char *device_name;
	int num_axis;
	int *axis;
	int num_buttons;
	int *buttons;
	GIOChannel *io_channel;
	guint source_id;
} Joystick;

Joystick *
joystick_new (char * joystick);

void
joystick_close (Joystick *joydata);

void
joy_focus_on (void);

void
joy_focus_off (void);

const char *
get_joy_dev (void);

#endif
