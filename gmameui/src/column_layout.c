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
#include <stdlib.h>
#include <gtk/gtkcellrenderertext.h>
#include <gtk/gtklabel.h>
#include <gtk/gtkliststore.h>
#include <gtk/gtkradiobutton.h>
#include <gtk/gtkscrolledwindow.h>
#include <gtk/gtkstock.h>
#include <gtk/gtktable.h>
#include <gtk/gtkvbox.h>
#include <glade/glade.h>

#include "gmameui.h"
#include "gui.h"
#include "column_layout.h"

void
column_layout_window_response (GtkWidget *widget, gpointer user_data);

static GladeXML *xml;

void
column_layout_window_response (GtkWidget *widget, gpointer user_data)
{
	/*GladeXML *xml = (gpointer) user_data;*/
	
	gui_prefs.ColumnShown[GAMENAME] = TRUE;

	gui_prefs.ColumnShown[ROMNAME] = gtk_toggle_button_get_active (glade_xml_get_widget (xml, "chk_directory"));
	gui_prefs.ColumnShown[TIMESPLAYED] = gtk_toggle_button_get_active (glade_xml_get_widget (xml, "chk_playcount"));
	gui_prefs.ColumnShown[MANU] = gtk_toggle_button_get_active (glade_xml_get_widget (xml, "chk_manufacturer"));
	gui_prefs.ColumnShown[YEAR] = gtk_toggle_button_get_active (glade_xml_get_widget (xml, "chk_year"));
	gui_prefs.ColumnShown[DRIVER] = gtk_toggle_button_get_active (glade_xml_get_widget (xml, "chk_driver"));
	gui_prefs.ColumnShown[ROMOF] = gtk_toggle_button_get_active (glade_xml_get_widget (xml, "chk_cloneof"));
	gui_prefs.ColumnShown[MAMEVER] = gtk_toggle_button_get_active (glade_xml_get_widget (xml, "chk_version"));
	gui_prefs.ColumnShown[CATEGORY] = gtk_toggle_button_get_active (glade_xml_get_widget (xml, "chk_category"));
	
	gtk_widget_destroy (widget);
	
	if ((gui_prefs.current_mode == DETAILS) || (gui_prefs.current_mode == DETAILS_TREE))
		create_gamelist (gui_prefs.current_mode);
}

GtkWidget *
create_column_layout_window (void)
{
	GtkWidget *column_layout_window;
	GtkWidget *chk_gamename;
	GtkWidget *chk_directory;
	GtkWidget *chk_playcount;
	GtkWidget *chk_manufacturer;
	GtkWidget *chk_year;
	GtkWidget *chk_driver;
	GtkWidget *chk_cloneof;
	GtkWidget *chk_version;
	GtkWidget *chk_category;
	
	/*GladeXML **/xml = glade_xml_new (GLADEDIR "column_layout.glade", NULL, NULL);
	if (!xml) {
		GMAMEUI_DEBUG ("Could not open Glade file %s", GLADEDIR "column_layout.glade");
		return NULL;
	}
	column_layout_window = glade_xml_get_widget (xml, "column_layout_window");
	gtk_widget_show (column_layout_window);

	chk_gamename = glade_xml_get_widget (xml, "chk_gamename");      /* TODO Dont let user turn this off */
	chk_directory = glade_xml_get_widget (xml, "chk_directory");
	chk_playcount = glade_xml_get_widget (xml, "chk_playcount");
	chk_manufacturer = glade_xml_get_widget (xml, "chk_manufacturer");
	chk_year = glade_xml_get_widget (xml, "chk_year");
	chk_driver = glade_xml_get_widget (xml, "chk_driver");
	chk_cloneof = glade_xml_get_widget (xml, "chk_cloneof");
	chk_version = glade_xml_get_widget (xml, "chk_version");
	chk_category = glade_xml_get_widget (xml, "chk_category");
	
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (chk_directory),
				      gui_prefs.ColumnShown[ROMNAME]);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (chk_playcount),
				      gui_prefs.ColumnShown[TIMESPLAYED]);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (chk_manufacturer),
				      gui_prefs.ColumnShown[MANU]);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (chk_year),
				      gui_prefs.ColumnShown[YEAR]);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (chk_driver),
				      gui_prefs.ColumnShown[DRIVER]);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (chk_cloneof),
				      gui_prefs.ColumnShown[ROMOF]);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (chk_version),
				      gui_prefs.ColumnShown[MAMEVER]);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (chk_category),
				      gui_prefs.ColumnShown[CATEGORY]);
	
	g_signal_connect (column_layout_window, "response",
		G_CALLBACK (column_layout_window_response),
		xml);
	
	return column_layout_window;
}

