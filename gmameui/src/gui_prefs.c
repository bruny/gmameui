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
#include <string.h>

#include <gtk/gtkcheckbutton.h>
#include <gtk/gtkentry.h>
#include <gtk/gtklabel.h>
#include <gtk/gtkstock.h>
#include <gtk/gtktable.h>
#include <glade/glade.h>

#include "gmameui.h"
#include "gui.h"
#include "gtkjoy.h"
#include "gui_prefs.h"

#ifdef ENABLE_JOYSTICK
static GtkWidget *gui_joy_entry;
static GtkWidget *gui_joy_checkbutton;

static void
button_toggled2 (GtkWidget *checkbutton,
		 gpointer   user_data)
{
	gtk_widget_set_sensitive (GTK_WIDGET (user_data),
				  gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (checkbutton)));
}

#endif

static GtkWidget *gui_use_xmame_options_checkbutton;
static GtkWidget *checkgames_checkbutton;
static GtkWidget *VersionCheck_checkbutton;

static void
prefs_ok (GtkWidget *widget)
{
#ifdef ENABLE_JOYSTICK
	gchar *used_text;

	gui_prefs.gui_joy = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (gui_joy_checkbutton));
	used_text = gtk_editable_get_chars (GTK_EDITABLE (gui_joy_entry), 0, -1);
	strncpy (gui_prefs.Joystick_in_GUI, used_text, 20);
	g_free (used_text);
	
	/* activate or desactivate joystick */
	if (gui_prefs.gui_joy) {
		if ((joydata = joystick_new (gui_prefs.Joystick_in_GUI)))
			g_message (_("Joystick %s found"), joydata->device_name);
		else
			g_message (_("No Joystick found"));
	} else {
		joystick_close (joydata);
		joydata = NULL;
	}
#endif
	gui_prefs.GameCheck = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (checkgames_checkbutton));
	gui_prefs.VersionCheck = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (VersionCheck_checkbutton));
	gui_prefs.use_xmame_options = 
		gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (gui_use_xmame_options_checkbutton));

}

static void
prefs_response (GtkWidget *dialog,
		gint       response_id,
		gpointer   user_data)
{
	switch (response_id) {
		case GTK_RESPONSE_OK:
			prefs_ok (dialog);
			break;
	}
	gtk_widget_destroy (dialog);
}

GtkWidget *
create_gui_prefs_window (void)
{
	GtkWidget *gui_prefs_window;
	GtkWidget *gui_prefs_table;
	GtkWidget *gui_joy_label;
	GtkWidget *gui_joy_entry;
	GtkWidget *gui_joy_checkbutton;
	GtkWidget *gui_use_xmame_options_checkbutton;
	GtkWidget *checkgames_checkbutton;
	GtkWidget *VersionCheck_checkbutton;

	GladeXML *xml = glade_xml_new (GLADEDIR "startup_prefs.glade", NULL, NULL);
	if (!xml) {
		GMAMEUI_DEBUG ("Could not open Glade file %s", GLADEDIR "startup_prefs.glade");
		return NULL;
	}
	gui_prefs_window = glade_xml_get_widget (xml, "gui_prefs_window");
	gtk_widget_show (gui_prefs_window);
	
	checkgames_checkbutton = glade_xml_get_widget (xml, "checkgames_checkbutton");

	VersionCheck_checkbutton = glade_xml_get_widget (xml, "VersionCheck_checkbutton");

	gui_use_xmame_options_checkbutton = glade_xml_get_widget (xml, "gui_use_xmame_options_checkbutton");

	gui_joy_checkbutton = glade_xml_get_widget (xml, "gui_joy_checkbutton");
	gui_joy_label = glade_xml_get_widget (xml, "gui_joy_label");
	gui_joy_entry = glade_xml_get_widget (xml, "gui_joy_entry");
#ifdef ENABLE_JOYSTICK
	gtk_widget_show (gui_joy_checkbutton);
	gtk_widget_show (gui_joy_label);
	gtk_widget_show (gui_joy_entry);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gui_joy_checkbutton), gui_prefs.gui_joy);
	gtk_entry_set_text (GTK_ENTRY (gui_joy_entry), gui_prefs.Joystick_in_GUI);
	gtk_widget_set_sensitive (GTK_WIDGET (gui_joy_entry),gui_prefs.gui_joy);
	g_signal_connect_after (gui_joy_checkbutton, "toggled",
				G_CALLBACK (button_toggled2),
				G_OBJECT (gui_joy_entry));

	g_signal_connect_after (gui_joy_checkbutton, "toggled",
				G_CALLBACK (button_toggled2),
				G_OBJECT (gui_joy_label));

#endif
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkgames_checkbutton), gui_prefs.GameCheck);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (VersionCheck_checkbutton), gui_prefs.VersionCheck);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gui_use_xmame_options_checkbutton), gui_prefs.use_xmame_options);	

	g_signal_connect (G_OBJECT (gui_prefs_window), "response",
			  G_CALLBACK (prefs_response),
			  NULL);

	return gui_prefs_window;
}
