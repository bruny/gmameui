/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * GMAMEUI
 *
 * Copyright 2007-2008 Andrew Burton <adb@iinet.net.au>
 * based on GXMame code
 * Copyright 2003, Luc Saillard <hcl@users.sourceforge.net>
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

#include "keyboard.h"

/*
 * For now, sdl and x11 share the same keymap
 */

static struct x11_keymap_layout x11_keymap_layout_fr[] = 
{
    {0x00b2,0x0029},
    {0x0026,0x0002},
    {0x00e9,0x0003},
    {0x0022,0x0004},
    {0x0027,0x0005},
    {0x0028,0x0006},
    {0x002d,0x0007},
    {0x00e8,0x0008},
    {0x005f,0x0009},
    {0x00e7,0x000a},
    {0x00e0,0x000b},
    {0x0029,0x000c},
    {0x003d,0x000d},
    {0x0041,0x0010},
    {0x005a,0x0011},
    {0x005e,0x001a},
    {0x0024,0x001b},
    {0x0051,0x001e},
    {0x004d,0x0027},
    {0x00f9,0x0028},
    {0x002a,0x0029},
    {0x0057,0x002c},
    {0x002c,0x0032},
    {0x003b,0x0033},
    {0x003a,0x0034},
    {0x0021,0x0035},
    {0x0000,0x0000}
};

/* suggested german keymap mods by Peter Trauner <peter.trauner@jk.uni-linz.ac.at> */
static struct x11_keymap_layout x11_keymap_layout_de[] = 
{
    {0xfe52,0x0029},
    {0x00df,0x000c},
    {0xfe51,0x000d},
    {0x00fc,0x001a},
    {0x002b,0x001b},
    {0x00f6,0x0027},
    {0x00e4,0x0028},
    {0x0023,0x002b},
    {0x0059,0x002c},
    {0x005a,0x0015},
    {0x002d,0x0035},
    {0x0000,0x0000}
};

struct x11_keymaps_layout x11_keymaps_layout[] = {
    { "fr", N_("French Layout"), x11_keymap_layout_fr },
    { "de", N_("German Layout"), x11_keymap_layout_de },
    { NULL, NULL, NULL}
};

