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
 */

#ifndef __PROGRESSION_WINDOW_H__
#define __PROGRESSION_WINDOW_H__

#include "common.h"


typedef struct {
	GtkWidget *window;
	GtkWidget *progress_bar;
	GtkWidget *current_rom_loading;
	GtkWidget *label;
	gboolean cancel_pressed;
} ProgressWindow;

ProgressWindow *
progress_window_new(gboolean cancel_available);

void progress_window_show(ProgressWindow *progress_window);

void progress_window_destroy(ProgressWindow *progress_window);

void progress_window_set_title(ProgressWindow *progress_window, const gchar *format, ...);

void progress_window_set_text(ProgressWindow *progress_window, const gchar *text);

void progress_window_set_value(ProgressWindow *progress_window, gfloat current_value);

gboolean progress_window_cancelled(ProgressWindow *progress_window);

#endif /* __PROGRESSION_WINDOW_H__ */
