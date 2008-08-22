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
#include <stdarg.h>

void
gmameui_message (GtkDialogFlags type,
		GtkWindow     *window,
		const gchar   *format,
		...)
{
	va_list args;
	gchar *my_args;
	GtkWidget *dialog;

	va_start (args, format);
	my_args = g_strdup_vprintf (format, args);
	va_end (args);

	dialog = gtk_message_dialog_new (window,
					 GTK_DIALOG_DESTROY_WITH_PARENT,
					 type,
					 GTK_BUTTONS_CLOSE,
					 my_args);
	g_free (my_args);
	gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);
}
