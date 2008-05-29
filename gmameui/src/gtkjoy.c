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
 * Code based on:
 * gtkjoy.c - general routines for handling joystick events
 * Copyright 1999 David Boynton.
 */

#include "common.h"

#include "gtkjoy.h"
#include "gmameui.h"
#include "gui.h"

#ifdef ENABLE_JOYSTICK

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

# ifdef HAVE_LINUX_JOYSTICK_H
#   include <linux/joystick.h>
# else
#   error Joystick is only supported on Linux. Recompile with --disable-joystick.
#   undef ENABLE_JOYSTICK
# endif
#endif

#define AXIS_MAX	32767
#define AXIS_MIN	-32768

const char *
get_joy_dev (void)
{
	static char *joy_dev_name[] = {
		"/dev/js0",
		"/dev/input/js0",
		"/dev/joy0",
		NULL
	};
	int i;

	for (i = 0; joy_dev_name[i]; i++) {
		if (g_file_test (joy_dev_name[i], G_FILE_TEST_EXISTS))
			break;
	}

	if (joy_dev_name[i])
		return joy_dev_name[i];
	else
		return joy_dev_name[0];
}

#define JOY_TIMEOUT 30

gboolean joystick_focus;

#ifdef ENABLE_JOYSTICK
guint repeat_source_id;

static void
on_joystick_event (gint eventtype, gint button);

static gboolean
joystick_io_func (GIOChannel *io_channel, GIOCondition cond, gpointer data)
{
	struct js_event js;
	char buf[sizeof (struct js_event)];
	gsize readrc;
	Joystick *joydata;

	joydata = data;
	if (!joydata) {
		GMAMEUI_DEBUG ("joystick_event: improper callback registration, %u", __LINE__);
		return TRUE;
	}

	g_io_channel_read_chars (io_channel, buf, 
				sizeof (struct js_event), &readrc, NULL);

	if (readrc != sizeof (struct js_event)) {
		GMAMEUI_DEBUG ("source, %d bytes read", readrc);
		return FALSE;
	}

	memcpy (&js, buf, sizeof (struct js_event));
	switch (js.type & ~JS_EVENT_INIT) {
	case JS_EVENT_BUTTON:
		if (js.number < joydata->num_buttons) 
			joydata->buttons[js.number] = js.value;
		break;
	case JS_EVENT_AXIS:
		if (js.number < joydata->num_axis) 
			joydata->axis[js.number] = js.value;
		break;
	}

	on_joystick_event (js.type, js.number);	
	return TRUE;
}
#endif	


Joystick *   
joystick_new (char * joystick)
{
#ifdef ENABLE_JOYSTICK
	Joystick *joydata;
	int joystickfd;
	const char *devname;
	char name[128] = "Unknown";

	/* open joystick device */
	devname = (joystick) ? joystick : get_joy_dev ();
	
	joystickfd = open (devname, O_RDONLY);

	if (joystickfd < 0) {
		if ( errno == ENODEV ) {
			GMAMEUI_DEBUG ("Device not configured - did you load the module?");
		} else {
			GMAMEUI_DEBUG ("Couldn't open '%s'", devname);
			perror (devname);
		}
		return NULL;
	}
	
	joydata = (Joystick *) g_malloc (sizeof (Joystick));
	if (!joydata)
		return NULL;

	memset (joydata, 0, sizeof (Joystick));

	fcntl (joystickfd, F_SETFL, O_NONBLOCK);
	ioctl (joystickfd, JSIOCGAXES, &joydata->num_axis);
	ioctl (joystickfd, JSIOCGBUTTONS, &joydata->num_buttons);
	ioctl (joystickfd, JSIOCGNAME (128), name);

	joydata->device_name = g_strdup (name);
	if (!joydata->device_name) {
		g_free (joydata);
		return NULL;
	}

	joydata->axis = g_malloc (joydata->num_axis * sizeof (int));
	if (!joydata->axis) {
		g_free (joydata->device_name);
		g_free (joydata);
		return NULL;
	}
	joydata->buttons = g_malloc (joydata->num_buttons * sizeof (int));
	if (!joydata->buttons) {
		g_free (joydata->device_name);
		g_free (joydata->axis);
		g_free (joydata);
		return NULL;
	}

	joydata->io_channel = g_io_channel_unix_new (joystickfd);
	g_io_channel_set_encoding (joydata->io_channel, NULL, NULL);
	g_io_channel_set_close_on_unref (joydata->io_channel, TRUE);

	joydata->source_id = g_io_add_watch (joydata->io_channel, G_IO_IN, joystick_io_func, joydata);

	return joydata;
#else
	return NULL;
#endif /* ENABLE_JOYSTICK */
}


void
joystick_close (Joystick * joydata)
{
#ifdef ENABLE_JOYSTICK
	if (!joydata) return;

	g_source_remove (joydata->source_id);
	g_io_channel_unref (joydata->io_channel);
	g_free (joydata->device_name);
	g_free (joydata->axis);
	g_free (joydata->buttons);
	g_free (joydata);
#endif	
}

void
joy_focus_on (void)
{
	joystick_focus = TRUE;
}

void
joy_focus_off (void)
{
	joystick_focus = FALSE;
}

#ifdef ENABLE_JOYSTICK
static void
move_up (int val)
{
	int i;
	GtkTreePath *path;
	GtkTreePath *tree_path;
	GtkTreeIter iter1;
	GtkTreeIter iter2;

	gtk_tree_view_get_cursor (GTK_TREE_VIEW (main_gui.displayed_list), &path, NULL);
	
	if (!gtk_tree_model_get_iter (GTK_TREE_MODEL (main_gui.tree_model), &iter1, path))
		return;

	for (i = 0; i < val; i++) {
		if (!gtk_tree_path_prev (path)) {
			if (!gtk_tree_path_up (path)) {
				break;
			} else {
				if (!gtk_tree_model_get_iter (GTK_TREE_MODEL (main_gui.tree_model), &iter1, path))
					break;
			}
		} else {
			if (gtk_tree_view_row_expanded (GTK_TREE_VIEW (main_gui.displayed_list), path)) {
				gtk_tree_path_down (path);
				gtk_tree_model_get_iter (GTK_TREE_MODEL (main_gui.tree_model), &iter2, path);
				iter1 = iter2;
				while (gtk_tree_model_iter_next (GTK_TREE_MODEL (main_gui.tree_model), &iter2)) {
					iter1 = iter2;
				}
			} else {
				if (!gtk_tree_model_get_iter (GTK_TREE_MODEL (main_gui.tree_model), &iter1, path))
					break;
			}
		}
	}

	tree_path = gtk_tree_model_get_path (GTK_TREE_MODEL (main_gui.tree_model), &iter1);
	gtk_tree_view_set_cursor (GTK_TREE_VIEW (main_gui.displayed_list), tree_path, NULL, FALSE);
	gtk_tree_view_scroll_to_cell (GTK_TREE_VIEW (main_gui.displayed_list), tree_path, NULL, TRUE, 0.5, 0);
	gtk_tree_path_free (tree_path);

	gtk_widget_grab_focus (main_gui.displayed_list);

}

static void
move_down (int val)
{
	int i;
	GtkTreePath *path;
	GtkTreePath *tree_path;
	GtkTreeIter iter1;
	GtkTreeIter iter2;

	gtk_tree_view_get_cursor (GTK_TREE_VIEW (main_gui.displayed_list), &path, NULL);
	if (!gtk_tree_model_get_iter (GTK_TREE_MODEL (main_gui.tree_model), &iter1, path))
		return;

	for (i = 0; i < val; i++) {
		if (gtk_tree_view_row_expanded (GTK_TREE_VIEW (main_gui.displayed_list), path)) {
			gtk_tree_model_iter_children (GTK_TREE_MODEL (main_gui.tree_model), &iter2, &iter1);
			iter1 = iter2;
		} else {
			iter2 = iter1;
			if (!gtk_tree_model_iter_next (GTK_TREE_MODEL (main_gui.tree_model), &iter1)) {
				if (gtk_tree_model_iter_parent (GTK_TREE_MODEL (main_gui.tree_model), &iter1, &iter2)) {
					if (!gtk_tree_model_iter_next (GTK_TREE_MODEL (main_gui.tree_model), &iter1)) {
						iter1 = iter2;
						break;
					}
				} else {
					iter1 = iter2;
					break;
				}
			}
		}
		path=gtk_tree_model_get_path (GTK_TREE_MODEL (main_gui.tree_model), &iter1);
	}

	tree_path = gtk_tree_model_get_path (GTK_TREE_MODEL (main_gui.tree_model), &iter1);
	gtk_tree_view_set_cursor (GTK_TREE_VIEW (main_gui.displayed_list), tree_path, NULL, FALSE);
	gtk_tree_view_scroll_to_cell (GTK_TREE_VIEW (main_gui.displayed_list), tree_path, NULL, TRUE, 0.5, 0);
	gtk_tree_path_free (tree_path);

	gtk_widget_grab_focus (main_gui.displayed_list);
}

static void toggle_expand (void)
{
	GtkTreePath *path;
	GtkTreeIter iter1;

	gtk_tree_view_get_cursor (GTK_TREE_VIEW (main_gui.displayed_list), &path, NULL);
	if (!gtk_tree_model_get_iter (GTK_TREE_MODEL (main_gui.tree_model), &iter1, path))
		return;

	if (gtk_tree_view_row_expanded (GTK_TREE_VIEW (main_gui.displayed_list), path)) {
		gtk_tree_view_collapse_row (GTK_TREE_VIEW (main_gui.displayed_list), path);
	} else {
		if (gtk_tree_model_iter_has_child (GTK_TREE_MODEL (main_gui.tree_model), &iter1))
			gtk_tree_view_expand_row (GTK_TREE_VIEW (main_gui.displayed_list), path, TRUE);
	}

}

static gint row_number (void)
{
	gint nbrow;
	GtkAdjustment *vadj;
	GdkRectangle rect;
	GtkTreePath *path;

	gtk_tree_view_get_cursor (GTK_TREE_VIEW (main_gui.displayed_list), &path, NULL);
	gtk_tree_view_get_cell_area (GTK_TREE_VIEW (main_gui.displayed_list),
				     path,
				     NULL, &rect);

	/* Getting the vertical window area */
	vadj=gtk_scrolled_window_get_vadjustment (GTK_SCROLLED_WINDOW (main_gui.scrolled_window_games));

	nbrow = (gint) ((gint)vadj->page_size / (gint)rect.height) - 1;

	return nbrow;
}

static gboolean joy_left_repetition (gpointer data)
{
	move_up (row_number ());
	repeat_source_id = g_timeout_add (JOY_TIMEOUT, (GSourceFunc)joy_left_repetition, NULL);
	return FALSE;
}

static gboolean joy_right_repetition (gpointer data)
{
	move_down (row_number ());
	repeat_source_id = g_timeout_add (JOY_TIMEOUT, (GSourceFunc)joy_right_repetition, NULL);
	return FALSE;
}

static gboolean joy_up_repetition (gpointer data)
{
	move_up (1);
	repeat_source_id = g_timeout_add (JOY_TIMEOUT, (GSourceFunc)joy_up_repetition, NULL);
	return FALSE;
}

static gboolean joy_down_repetition (gpointer data)
{
	move_down (1);
	repeat_source_id = g_timeout_add (JOY_TIMEOUT, (GSourceFunc)joy_down_repetition, NULL);
	return FALSE;
}

void
on_joystick_event (gint eventtype, gint button)
{
	/*the focus is not set: another windows has been opened
	 or this is an init event */
	if (!joystick_focus || (eventtype & JS_EVENT_INIT))
		return;

	GMAMEUI_DEBUG ("joy: event=%i, button=%i", eventtype, button);

	if ((eventtype & JS_EVENT_BUTTON) && (button < joydata->num_buttons)) {
		GMAMEUI_DEBUG ("Joystick button %i %s", button, (joydata->buttons[button])?"pressed":"released");
		/* Button released*/
		if (joydata->buttons[button] == 0)
			switch (button) {
			case 0:
// FIXME TODO				play_game (gui_prefs.current_game);
				break;
			case 1:
				toggle_expand ();
				break;
		}
	} else if ( (eventtype & JS_EVENT_AXIS) && (button < joydata->num_axis)) {
		if (repeat_source_id)
			g_source_remove (repeat_source_id);

		/* LEFT and RIGHT directions
		* (even Axis number)
		*/
		if (! (button & 1)) {
			if (joydata->axis[button] > (AXIS_MAX / 2)) {
				GMAMEUI_DEBUG ("Joystick axis %i pushed right", button);
				move_down (row_number ());
				repeat_source_id = g_timeout_add (10 * JOY_TIMEOUT,
								  (GSourceFunc) joy_right_repetition, NULL);
			} else if (joydata->axis[button] < (AXIS_MIN / 2)) {
				GMAMEUI_DEBUG ("Joystick axis %i pushed left", button);
				move_up (row_number ());
				repeat_source_id = g_timeout_add (10 * JOY_TIMEOUT,
								  (GSourceFunc) joy_left_repetition, NULL);
			}
		} else {
			/* UP and DOWN directions */
			if (joydata->axis[button] > (AXIS_MAX / 2)) {
				GMAMEUI_DEBUG ("Joystick axis %i pushed down", button);
				move_down (1);
				repeat_source_id = g_timeout_add (10 * JOY_TIMEOUT,
								  (GSourceFunc) joy_down_repetition, NULL);

			} else if (joydata->axis[button] < (AXIS_MIN / 2)) {
				GMAMEUI_DEBUG ("Joystick axis %i pushed up", button);
				move_up (1);
				repeat_source_id = g_timeout_add (10 * JOY_TIMEOUT,
								  (GSourceFunc) joy_up_repetition, NULL);
			}
		}
	}
}

#endif /* ENABLE_JOYSTICK */
