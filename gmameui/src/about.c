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

#include <gtk/gtk.h>

#include "gmameui.h"
#include "about.h"
#include "gui.h"

#if GTK_CHECK_VERSION(2,6,0)

void
about_window_show (void)
{
	GdkPixbuf *pixbuf;
	
	const char *authors [] = {
		"Andrew Burton <adb@iinet.net.au",
		"Based on GXMame",
		"Stephane Pontier <shadow_walker@users.sourceforge.net>",
		"Benoit Dumont <vertigo17@users.sourceforge.net>",
		"Nicos Panayides <anarxia@gmx.net>",
		"Priit Laes <x-laes@users.sourceforge.net>",
		"William Jon McCann <mccann@jhu.edu>",
		NULL
	};

	const char *translators = _("translator-credits");

	pixbuf = gmameui_get_icon_from_stock ("gmameui-screen");

	gtk_show_about_dialog (GTK_WINDOW (MainWindow),
			       "name", "GMAMEUI",
			       "version", VERSION,
			       "logo", pixbuf,
			       "copyright", _("Copyright (c) 2007-2009 Andrew Burton"),
			       "website", "http://gmameui.sourceforge.net",
			       "comments", _("A program to play MAME under Linux"),
			       "authors", authors,
			       "translator-credits", translators,
			       NULL);

	g_object_unref (pixbuf);
}

#else

static GtkWidget *AboutWindow = NULL;
static GtkWidget *CreditsWindow = NULL;

static gboolean
on_CreditsWindow_delete_event (GtkWidget *widget,
			       GdkEvent  *event,
			       gpointer   user_data)
{
	gtk_widget_hide (CreditsWindow);
	gtk_widget_destroy (CreditsWindow);
	CreditsWindow = NULL;
	return FALSE;
}

static void
on_CreditsWindow_response (GtkDialog *dialog,
			   gint       response,
			   gpointer   user_data)
{
	gtk_widget_destroy (GTK_WIDGET (dialog));
	CreditsWindow = NULL;
}


static GtkWidget *
create_CreditsWindow (GtkWindow *parent)
{
	GtkWidget *creditsWindow;
	GtkNotebook *credits_notebook;
	GtkWidget *dialog_vbox;
	GtkWidget *translators;
	GtkWidget *developers;
	GtkWidget *developers_label;
	GtkWidget *translators_label;

	creditsWindow = gtk_dialog_new_with_buttons (_("Credits"),
						    parent, 
						    GTK_DIALOG_NO_SEPARATOR
						     |GTK_DIALOG_DESTROY_WITH_PARENT,
						    GTK_STOCK_CLOSE, GTK_RESPONSE_CLOSE,
						    NULL);
	gtk_window_set_resizable (GTK_WINDOW (creditsWindow), FALSE);

	dialog_vbox = GTK_DIALOG (creditsWindow)->vbox;
	gtk_widget_show (dialog_vbox);

	credits_notebook = GTK_NOTEBOOK (gtk_notebook_new ());
	gtk_widget_show (GTK_WIDGET (credits_notebook));

	gtk_box_pack_start (GTK_BOX (dialog_vbox), GTK_WIDGET (credits_notebook), FALSE, FALSE, 0);

	developers_label = gtk_label_new (_("Developers"));
	developers = gtk_label_new(_("Andrew Burton\n"
				     "Code based on GXMame\n"
				     "Stephane Pontier <shadow_walker@users.sourceforge.net>\n"
				     "Benoit Dumont <vertigo17@users.sourceforge.net>\n"
				     "Nicos Panayides <anarxia@gmx.net>\n"
				     "Priit Laes <x-laes@users.sourceforge.net>\n"
				     "William Jon McCann <mccann@jhu.edu>\n"));
	gtk_widget_show (developers);

	gtk_notebook_append_page (credits_notebook, developers, developers_label);

	translators_label = gtk_label_new (_("Translators"));
	translators = gtk_label_new(_("The following people translated GMAMEUI:\n"
				      "Brazilian Portugese (pt_BR) - Alfredo Jr <junix@linuxinabox.org>\n"
				      "Italian (it) - Ugo Viti <ugo.viti@initzero.it>\n"
				      "Swedish (sv) - Daniel Nylander <dnylander@users.sourceforge.net>\n"
				      "French (fr) - Loïc Martin <loic.martin3@gmail.com>"));
	gtk_widget_show (translators);
	gtk_notebook_append_page (credits_notebook, translators, translators_label);

	g_signal_connect (G_OBJECT (creditsWindow), "delete_event",
			  G_CALLBACK (on_CreditsWindow_delete_event),
			  G_OBJECT (creditsWindow));
	g_signal_connect (G_OBJECT (creditsWindow), "response",
			  G_CALLBACK (on_CreditsWindow_response),
			  G_OBJECT (creditsWindow));

	gtk_widget_show_all (GTK_WIDGET (creditsWindow));
	return creditsWindow;
}

static void
on_aboutWindow_response (GtkDialog *dialog,
			 gint       response,
			 gpointer   user_data)
{
	switch (response) {
	case 1:	/* credits */
		if (CreditsWindow)
			gtk_window_present (GTK_WINDOW (CreditsWindow));
		else {
			CreditsWindow = create_CreditsWindow (GTK_WINDOW (AboutWindow));
			gtk_widget_show (CreditsWindow);
		}
		break;
	default:
		gtk_widget_destroy (GTK_WIDGET (dialog));
		AboutWindow = NULL;
		CreditsWindow = NULL;
	}
}

static void
about_window_hide (void)
{
	gtk_widget_hide (AboutWindow);
	gtk_widget_destroy (AboutWindow);
	AboutWindow = NULL;
	CreditsWindow = NULL;
}

static gboolean
on_AboutWindow_delete_event (GtkWidget *widget,
			     GdkEvent  *event,
			     gpointer   user_data)
{
	about_window_hide ();
	return FALSE;
}

static GtkWidget *
create_AboutWindow (void)
{
	GtkWidget *AboutWindow;
	GtkWidget *dialog_vbox;
	GtkWidget *image;
	GtkWidget *vbox2;
	GtkWidget *label2;
	GtkWidget *label3, *label4, *url_label;
	gchar *abouttitle, *text;

	AboutWindow = gtk_dialog_new_with_buttons (_("About GMAMEUI"),
						   GTK_WINDOW (MainWindow), 
						   GTK_DIALOG_NO_SEPARATOR
						   | GTK_DIALOG_DESTROY_WITH_PARENT,
						   _("Credits"), 1,
						   GTK_STOCK_CLOSE, GTK_RESPONSE_CLOSE,
						   NULL);
	gtk_button_box_set_layout (GTK_BUTTON_BOX (GTK_DIALOG (AboutWindow)->action_area), GTK_BUTTONBOX_EDGE);
	gtk_window_set_resizable (GTK_WINDOW (AboutWindow), FALSE);

	dialog_vbox = GTK_DIALOG (AboutWindow)->vbox;
	gtk_widget_show (dialog_vbox);

	vbox2 = gtk_vbox_new (FALSE, 3);
	gtk_widget_show (vbox2);
	gtk_box_pack_start (GTK_BOX (dialog_vbox), vbox2, FALSE, FALSE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (vbox2), 10);

	image = gmameui_get_image_from_stock ("gmameui-screen");
	gtk_widget_show (image);
	gtk_box_pack_start (GTK_BOX (vbox2), image, TRUE, TRUE, 0);

	abouttitle = g_strdup_printf ("<big><b>%s %s</b></big>", _("GMAMEUI"), VERSION);
	label2 = gtk_label_new (abouttitle);
	g_free (abouttitle);
	gtk_label_set_use_markup (GTK_LABEL (label2), TRUE);
	
	gtk_widget_show (label2);
	gtk_box_pack_start (GTK_BOX (vbox2), label2, TRUE, TRUE, 0);

	label3 = gtk_label_new (_("A program to play MAME under Linux"));
	gtk_widget_show (label3);
	gtk_box_pack_start (GTK_BOX (vbox2), label3, TRUE, TRUE, 0);

	text = g_strdup_printf ("<span size=\"small\">%s</span>", _("Copyright (c) 2007-2009 Andrew Burton"));
	label4 = gtk_label_new (text);
	g_free (text);
	gtk_label_set_use_markup (GTK_LABEL (label4), TRUE);
	gtk_widget_show (label4);
	gtk_box_pack_start (GTK_BOX (vbox2), label4, TRUE, TRUE, 0);

	url_label = gtk_label_new ("http://gmameui.sourceforge.net");
	gtk_label_set_selectable (GTK_LABEL (url_label), TRUE);
	gtk_widget_show (url_label);
	gtk_box_pack_end (GTK_BOX (vbox2), url_label, TRUE, TRUE, 10);

	g_signal_connect (G_OBJECT (AboutWindow), "delete_event",
			  G_CALLBACK (on_AboutWindow_delete_event),
			  G_OBJECT (AboutWindow));
	g_signal_connect (G_OBJECT (AboutWindow), "response",
			  G_CALLBACK (on_aboutWindow_response),
			  G_OBJECT (AboutWindow));

	return AboutWindow;
}


void
about_window_show (void)
{
	if (AboutWindow) {
		gtk_window_present (GTK_WINDOW (AboutWindow));
	} else {
		AboutWindow = create_AboutWindow ();
		gtk_widget_show (AboutWindow);
	}
}


#endif
