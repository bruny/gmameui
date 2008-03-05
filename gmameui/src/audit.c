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

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <gdk/gdkkeysyms.h>
#include <glade/glade.h>

#include "gui.h"
#include "audit.h"
#include "options_string.h"

#define BUFFER_SIZE 1000

static GtkWidget *roms_check_progressbar;
static GtkWidget *samples_check_progressbar;
static GtkWidget *checking_games_window;
static GtkWidget *checking_games_label;
static GtkWidget *details_check_text;
static GtkTextBuffer *details_check_buffer;
static GtkWidget *correct_roms_value;
static GtkWidget *bestavailable_roms_value;
static GtkWidget *notfound_roms_value;
static GtkWidget *incorrect_roms_value;
static GtkWidget *total_roms_value;
static GtkWidget *correct_samples_value;
static GtkWidget *incorrect_samples_value;
static GtkWidget *total_samples_value;
static GtkWidget *close_audit_button;
static GtkWidget *stop_audit_button;

static int audit_cancelled, close_audit;

static GtkWidget *
options_frame_new (const char *text)
{
	PangoFontDescription *font_desc;
	GtkWidget *label;
	GtkWidget *frame = gtk_frame_new (text);

	gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_NONE);

	label = gtk_frame_get_label_widget (GTK_FRAME (frame));

	font_desc = pango_font_description_new ();
	pango_font_description_set_weight (font_desc,
					   PANGO_WEIGHT_BOLD);
	gtk_widget_modify_font (label, font_desc);
	pango_font_description_free (font_desc);

	return frame;
}

static GtkWidget *
options_frame_create_child (GtkWidget *widget)
{
	GtkWidget *vbox;
	GtkWidget *hbox;
	GtkWidget *label;

	hbox = gtk_hbox_new (FALSE, 0);
	gtk_container_add (GTK_CONTAINER (widget), hbox);
	gtk_widget_show (hbox);

	label = gtk_label_new ("    ");
	gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
	gtk_widget_show (label);

	vbox = gtk_vbox_new (FALSE, 6);
	gtk_box_pack_start (GTK_BOX (hbox), vbox, TRUE, TRUE, 0);
	gtk_widget_show (vbox);

	return vbox;
}

static gboolean
stop_audit (GtkWidget *widget,
	    GtkWidget *button)
{
	audit_cancelled = TRUE;
	gtk_widget_set_sensitive (button, FALSE);
	close_audit = FALSE;
GMAMEUI_DEBUG("Audit stopped");
	return TRUE;
}


static void
audit_response (GtkWidget *dialog,
		gint       response_id,
		gpointer   user_data)
{
	switch (response_id) {
	case GTK_RESPONSE_REJECT:
		stop_audit (dialog, user_data);
		break;
	case GTK_RESPONSE_CLOSE:
		gtk_widget_destroy (dialog);
	default:
		break;
	}
}

GtkWidget *
create_checking_games_window (void)
{
	GladeXML *xml = glade_xml_new (GLADEDIR "audit_window.glade", NULL, NULL);
	if (!xml) {
		GMAMEUI_DEBUG ("Could not open Glade file %s", GLADEDIR "audit_window.glade");
		return NULL;
	}
	checking_games_window = glade_xml_get_widget (xml, "checking_games_window");
	gtk_widget_show (checking_games_window);

	stop_audit_button = glade_xml_get_widget (xml, "stop_audit_button");
	close_audit_button = glade_xml_get_widget (xml, "close_audit_button");

	/* only allows to click it when the audit is done */
	gtk_widget_set_sensitive (close_audit_button, FALSE);

	details_check_buffer = gtk_text_buffer_new (NULL);
	details_check_text = glade_xml_get_widget (xml, "details_check_text");
	gtk_text_view_set_buffer (GTK_TEXT_VIEW (details_check_text), details_check_buffer);

	checking_games_label = glade_xml_get_widget (xml, "checking_games_label");
#if GTK_CHECK_VERSION(2,6,0)
	gtk_label_set_ellipsize (GTK_LABEL (checking_games_label), PANGO_ELLIPSIZE_MIDDLE);
#endif

	correct_roms_value = glade_xml_get_widget (xml, "correct_roms_value");
	bestavailable_roms_value = glade_xml_get_widget (xml, "bestavailable_roms_value");
	incorrect_roms_value = glade_xml_get_widget (xml, "incorrect_roms_value");
	notfound_roms_value = glade_xml_get_widget (xml, "notfound_roms_value");
	total_roms_value = glade_xml_get_widget (xml, "total_roms_value");
	roms_check_progressbar = glade_xml_get_widget (xml, "roms_check_progressbar");

	correct_samples_value = glade_xml_get_widget (xml, "correct_samples_value");
	incorrect_samples_value = glade_xml_get_widget (xml, "incorrect_samples_value");
	total_samples_value = glade_xml_get_widget (xml, "total_samples_value");
	samples_check_progressbar = glade_xml_get_widget (xml, "samples_check_progressbar");

	g_signal_connect (checking_games_window, "response",
			  G_CALLBACK (audit_response), stop_audit_button);
			  
	return checking_games_window;
}

gint process_audit_romset (gchar *line, gboolean error_during_check) {
	gchar *p;

	/* 0121	romset name [parent] is {good|bad|best available}
	   0105 romset name {correct|incorrect|best available} */
	/* Only process the romset lines; individual chips are ignored */
	if ((!strncmp (line, "romset", 6)) || (!strncmp (line, "sampleset", 9))) {
		for (p = strstr (line, " ") + 1/*line + 7*/; (*p && (*p != ' ') && (*p != '\n')); p++);
		*p = '\0';
		p++;

		/* FIXME TODO ROMs report 'is bad' even if the error is just an
		   incorrect length on one of the roms */
		if ((strstr (p, "incorrect") && error_during_check == TRUE)
			|| (strstr (p, "is bad") && error_during_check == TRUE))
		{
			/* Version 0.107 (?) and earlier used:
			    romset XXX incorrect
			   Version 0.117 (?) and later used:
			    romset XXX is bad */
			return INCORRECT;
		} else 
		if ((strstr (p, "correct")) || (strstr(p, "is good"))) {
			/* Version 0.107 (?) and earlier used:
			    romset XXX correct
			   Version 0.117 (?) and later used:
			    romset XXX is good */
			return CORRECT;
		}  else if (strstr (p, "not found")) {
			return NOT_AVAIL;
		} else if (strstr (p, "best available")) {
			/* Version 0.107 (?) and earlier used:
			    romset XXX best available
			   Version 0.117 (?) and later used:
			    romset XXX is best available */
			return BEST_AVAIL;
		} else {
			printf ("Audit: Could not recognise line %s\n", p);
			return UNKNOWN;
		}

		error_during_check = FALSE;

	} else if (!strncmp (line, "name", 4) || !strncmp (line, "---", 3)) {
		/* do nothing */
	} else {
		error_during_check = TRUE;
		return NOTROMSET;
	}
}

/* due to the pipe buffer size,
   freeze the qui a moment before displaying results */
void
launch_checking_games_window (void)
{
	FILE *xmame_pipe;
	gchar line[BUFFER_SIZE];
	gchar title[BUFFER_SIZE];
	gchar numb[10];
	gfloat done;
	guint nb_checked = 1, nb_good = 0, nb_incorrect = 0, nb_bestavailable = 0, nb_notfound = 0;
	gboolean error_during_check = FALSE;
	gchar *name;
	gchar *rompath_option;
	gint auditresult;
	GList *listpointer;
	RomEntry *tmprom = NULL;
	GtkTextIter text_iter;
	const gchar *option_name;	

	if (!xmame_get_options (current_exec))
		return;

	audit_cancelled = FALSE;
	close_audit = FALSE;
	option_name = xmame_get_option_name (current_exec, "verifyroms");

	if (!option_name) {
		gmameui_message (ERROR, NULL, _("Don't know how to verify roms with this version of xmame."));
		return;
	}

	/* In more recent versions of MAME, only the available (i.e. where the filename exists)
	   ROMs are audited. So, we need to set the status of all the others to 'unavailable' */
	for (listpointer = g_list_first (game_list.roms);
	     (listpointer != NULL);
	     listpointer = g_list_next (listpointer))
	{
		tmprom = (RomEntry *) listpointer->data;
		tmprom->has_roms = NOT_AVAIL;
	}

	
	rompath_option = create_rompath_options_string (current_exec);
		
	xmame_pipe = xmame_open_pipe (current_exec, "-%s %s",
				      option_name, rompath_option);
	
	if (!xmame_pipe) {
		GMAMEUI_DEBUG ("Could not run audit");
		g_free (rompath_option);
		return;
	}

	/* Loading */
	fflush (xmame_pipe);
	
	while (fgets (line, BUFFER_SIZE, xmame_pipe) && !audit_cancelled) {
		/* jump the last comments */
		if (line[0] == '\0' || line[1] == '\0')
			break;
	
		gtk_text_buffer_get_end_iter (GTK_TEXT_BUFFER (details_check_buffer), &text_iter);

		auditresult = process_audit_romset (line, error_during_check);
		done = (gfloat) (nb_good + nb_incorrect + nb_notfound + nb_bestavailable) / (gfloat) (game_list.num_games);
		
		if (auditresult == CORRECT) {
			nb_good++;
			g_snprintf (numb, 10, "%d", nb_good);
			gtk_label_set_text (GTK_LABEL (correct_roms_value), numb);}
		else if (auditresult == INCORRECT) {
			nb_incorrect++;
			g_snprintf (numb, 10, "%d", nb_incorrect);
			gtk_label_set_text (GTK_LABEL (incorrect_roms_value), numb);
			sprintf (title, "%s: Incorrect\n", line);
			gtk_text_buffer_insert (GTK_TEXT_BUFFER (details_check_buffer), &text_iter, title, -1);}
		else if (auditresult == BEST_AVAIL) {
			nb_bestavailable++;
			g_snprintf (numb, 10,"%d", nb_bestavailable);
			gtk_label_set_text (GTK_LABEL (bestavailable_roms_value), numb);
		} else if (auditresult == NOT_AVAIL) {
			nb_notfound++;
			g_snprintf (numb, 10, "%d", nb_notfound);
			gtk_label_set_text (GTK_LABEL (notfound_roms_value), numb);
			sprintf (title, "%s: Not found\n", line);
			gtk_text_buffer_insert (GTK_TEXT_BUFFER (details_check_buffer), &text_iter, title, -1);
		}

		if (auditresult != NOTROMSET) {
			g_snprintf (numb, 10, "%d", nb_good + nb_incorrect + nb_notfound + nb_bestavailable);
			gtk_label_set_text (GTK_LABEL (total_roms_value), numb);

			/* find the rom in the list - need to skip leading romset/sampleset */
			name = g_strrstr (line, " ") + 1;
			listpointer = g_list_find_custom (game_list.roms, name,
							  (GCompareFunc) g_ascii_strcasecmp);

			if (listpointer) {
				tmprom = (RomEntry *) listpointer->data;
				tmprom->has_roms = auditresult;
				g_snprintf (title, BUFFER_SIZE, "%s", rom_entry_get_list_name (tmprom));
			}
			
			g_snprintf (title, BUFFER_SIZE, "%s", name);
			gtk_label_set_text (GTK_LABEL (checking_games_label), title);
			
			gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (roms_check_progressbar), done);
		} else {
			gtk_text_buffer_insert (GTK_TEXT_BUFFER (details_check_buffer), &text_iter, line, -1);
			error_during_check = TRUE;
		}

		while (gtk_events_pending ())
			gtk_main_iteration ();
		fflush (xmame_pipe);
	}

	xmame_close_pipe (current_exec, xmame_pipe);

	if (audit_cancelled) {
		g_free (rompath_option);
		gtk_window_set_title (GTK_WINDOW (checking_games_window), _("Audit Stopped"));
		gtk_label_set_text (GTK_LABEL (checking_games_label), _("Stopped"));
		goto audit_finished;
	}
	/* Samples now */
	nb_good = nb_incorrect = 0;
	nb_checked = 1;

	xmame_pipe = xmame_open_pipe (current_exec, "-%s %s", 
				      xmame_get_option_name (current_exec, "verifysamples"),
				      rompath_option);

	g_free (rompath_option);

	/* Loading */
	while (fgets (line, BUFFER_SIZE, xmame_pipe) && !audit_cancelled) {
		/* jump the last comments */
		if (line[0] == '\0' || line[1] == '\0')
			break;
	
		gtk_text_buffer_get_end_iter (GTK_TEXT_BUFFER (details_check_buffer), &text_iter);
		auditresult = process_audit_romset (line, error_during_check);
		
		done = (gfloat) ( (gfloat) (nb_checked) / (gfloat) (game_list.num_sample_games));

		if (auditresult == CORRECT) {
			nb_good++;
			g_snprintf (numb, 10,"%d", nb_good);
			gtk_label_set_text (GTK_LABEL (correct_samples_value), numb);
		} else if (auditresult == INCORRECT) {
			nb_incorrect++;
			g_snprintf (numb, 10, "%d", nb_incorrect);
			gtk_label_set_text (GTK_LABEL (incorrect_samples_value), numb);
		}

		if (auditresult != NOTROMSET) {
			g_snprintf (numb, 10,"%d", nb_good+nb_incorrect);
			gtk_label_set_text (GTK_LABEL (total_samples_value), numb);
			
			/* find the rom in the list - need to skip leading romset/sampleset */
			name = g_strrstr (line, " ") + 1;

			listpointer = g_list_find_custom (game_list.roms, name,
							  (GCompareFunc) g_ascii_strcasecmp);
			if (listpointer) {
				tmprom = (RomEntry *) listpointer->data;
				tmprom->has_samples = auditresult;

				g_snprintf (title, BUFFER_SIZE, "%s", rom_entry_get_list_name (tmprom));
			}
			/*continue with the GUI */
			nb_checked++;
			error_during_check = FALSE;
			g_snprintf (title, BUFFER_SIZE, "%s", name);
			gtk_label_set_text (GTK_LABEL (checking_games_label), title);

			gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (samples_check_progressbar), done);
		} else {
			gtk_text_buffer_insert (GTK_TEXT_BUFFER (details_check_buffer), &text_iter, line, -1);
			error_during_check = TRUE;
		}
		while (gtk_events_pending ())
			gtk_main_iteration ();
		fflush (xmame_pipe);
	}
	pclose (xmame_pipe);

	if (audit_cancelled) {
		gtk_window_set_title (GTK_WINDOW (checking_games_window), _("Audit Stopped"));
		gtk_label_set_text (GTK_LABEL (checking_games_label), _("Stopped"));
		goto audit_finished;
	}

	gtk_window_set_title (GTK_WINDOW (checking_games_window), _("Audit done"));
		gtk_label_set_text (GTK_LABEL (checking_games_label), _("Done"));
	gtk_text_buffer_get_end_iter (GTK_TEXT_BUFFER (details_check_buffer), &text_iter);
	gtk_text_buffer_insert (GTK_TEXT_BUFFER (details_check_buffer), &text_iter, _("Audit done"), -1);

audit_finished:

	gtk_widget_set_sensitive (stop_audit_button, FALSE);
	gtk_widget_set_sensitive (close_audit_button, TRUE);

	create_gamelist_content ();
	if (close_audit)
		gtk_widget_destroy (checking_games_window);
	audit_cancelled = FALSE;

	while (gtk_events_pending ())
		gtk_main_iteration ();
}


