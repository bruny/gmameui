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

#include "common.h"
#include <stdlib.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtklabel.h>
#include <gtk/gtkstock.h>
#include <gtk/gtkvbox.h>

#include "gmameui.h"
#include "progression_window.h"
#include "gui.h"

gboolean
progress_window_cancelled (ProgressWindow *window)
{
	return window->cancel_pressed;
}

static void
cancel_progress_window (ProgressWindow *window)
{
	window->cancel_pressed = TRUE;
}

static void
progress_response (GtkWidget *dialog,
		   gint       response_id,
		   gpointer   user_data)
{
	switch (response_id) {
	case GTK_RESPONSE_CANCEL:
		cancel_progress_window ((ProgressWindow *)user_data);
	default:
		gtk_widget_hide (dialog);
		break;
	}
}

ProgressWindow *
progress_window_new (gboolean cancel_available)
{
	ProgressWindow *progress_window;
	GtkWidget *loading_progress_window;
	GtkWidget *vbox2;
	GtkWidget *current_rom_loading;
	GtkWidget *game_loading_progression;
	GtkWidget *label;
	gchar     *text;

	progress_window = (ProgressWindow *) malloc (sizeof (ProgressWindow));

	loading_progress_window = gtk_dialog_new_with_buttons (_("Progress Window"),
							       GTK_WINDOW (MainWindow),
							       GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_NO_SEPARATOR,
							       NULL);

	gtk_container_set_border_width (GTK_CONTAINER (loading_progress_window), 15);
	gtk_window_set_default_size (GTK_WINDOW (loading_progress_window), 350, 120);
	gtk_window_set_resizable (GTK_WINDOW (loading_progress_window), TRUE);

	vbox2 = gtk_vbox_new (TRUE, 0);
	gtk_widget_show (vbox2);
	gtk_container_add (GTK_CONTAINER (GTK_DIALOG (loading_progress_window)->vbox), vbox2);

	text = g_strdup_printf ("<b>%s</b>", _("Loading Game..."));
	label = gtk_label_new (text);
	gtk_widget_show (label);
	gtk_label_set_use_markup (GTK_LABEL (label), TRUE);
	g_free (text);
	gtk_box_pack_start (GTK_BOX (vbox2), label, FALSE, FALSE, 11);
	gtk_misc_set_alignment (GTK_MISC (label), 0, 0.5);

	game_loading_progression = gtk_progress_bar_new ();
	gtk_widget_show (game_loading_progression);
	gtk_box_pack_start (GTK_BOX (vbox2), game_loading_progression, FALSE, FALSE, 11);

	current_rom_loading = gtk_label_new (_("rom"));
	gtk_widget_show (current_rom_loading);
	gtk_box_pack_start (GTK_BOX (vbox2), current_rom_loading, FALSE, FALSE, 0);

	if (FALSE) /* cancel_available */
	{
		gtk_dialog_add_button (GTK_DIALOG (loading_progress_window),
				       GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL);
	}

	progress_window->window = loading_progress_window;
	progress_window->cancel_pressed = FALSE;
	progress_window->current_rom_loading = current_rom_loading;
	progress_window->label = label;
	progress_window->progress_bar = game_loading_progression;

	g_signal_connect (loading_progress_window, "response",
			  G_CALLBACK (progress_response),
			  progress_window);

	return progress_window;
}

void
progress_window_show (ProgressWindow *progress_window)
{
	gtk_widget_show (progress_window->window);
}

void
progress_window_destroy (ProgressWindow *progress_window)
{
	gtk_widget_destroy (progress_window->window);
}

void
progress_window_set_title (ProgressWindow *progress_window,
			   const gchar    *format, ...)
{
	va_list args;
	gchar *buffer;
	gchar *markup;

	va_start (args, format);
	buffer = g_strdup_vprintf (format, args);
	va_end (args);

	gtk_window_set_title (GTK_WINDOW (progress_window->window), buffer);
	markup = g_strdup_printf ("<b>%s</b>", buffer);
	gtk_label_set_markup (GTK_LABEL (progress_window->label), markup);

	g_free (markup);
	g_free (buffer);
}

void
progress_window_set_text (ProgressWindow *progress_window,
			  const gchar    *text)
{
	gtk_label_set_text (GTK_LABEL (progress_window->current_rom_loading), text);
}

void
progress_window_set_value (ProgressWindow *progress_window,
			   gfloat          current_value)
{

	gchar *percent_text;

	gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (progress_window->progress_bar), current_value);

	percent_text = g_strdup_printf ("%.0f%%", current_value * 100);

	gtk_progress_bar_set_text (GTK_PROGRESS_BAR (progress_window->progress_bar), percent_text);

	g_free (percent_text);
}
