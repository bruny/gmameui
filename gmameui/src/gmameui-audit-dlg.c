/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * GMAMEUI
 *
 * Copyright 2007-2009 Andrew Burton <adb@iinet.net.au>
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

#include <glade/glade.h>

#include "gmameui.h"			/* Contains the GmameuiAudit object */
#include "gui.h"			/* For main_gui */
#include "audit.h"
#include "gmameui-gamelist-view.h"
#include "gmameui-audit-dlg.h"

struct _MameAuditDialogPrivate {
	GladeXML *xml;

	/* Label for presenting details of current audit activity */
	GtkWidget *checking_games_label;
	
	/* Textview containing the audit results details */
	GtkWidget *details_check_text;
	GtkTextBuffer *details_check_buffer;
	
	/* Pulse progress bar and timer to increment it every timeout interval */
	GtkProgressBar *pbar;
	int timer;
	
	GtkWidget *close_audit_button;
	GtkWidget *stop_audit_button;

	/* Audit running count labels */
	GtkWidget *correct_roms_value;
	GtkWidget *bestavailable_roms_value;
	GtkWidget *notfound_roms_value;
	GtkWidget *incorrect_roms_value;
	GtkWidget *total_roms_value;
	GtkWidget *correct_samples_value;
	GtkWidget *incorrect_samples_value;
	GtkWidget *notfound_samples_value;
	GtkWidget *total_samples_value;
	
	/* Total number of ROMs and samples supported by this version of MAME */
	gint num_roms;
	gint num_samples;
	
	/* Number of ROMs and samples checked so far */
	guint nb_roms_checked;
	guint nb_samples_checked;
	
	/* Arrays storing the results for the counts */
	gint romset_count[NUMBER_STATUS];
	gint sampleset_count[NUMBER_STATUS];
	
	/* When both the audit and sample MAME threads have completed, then
	   the window can be updated */
	gboolean audit_complete;
	gboolean sample_audit_complete;
	
	/* Signal handler IDs to disconnect when the user cancels the audit */
	gint command_sigid;
	gint command_sample_sigid;
	gint romset_sigid;
};

#define MAME_AUDIT_DIALOG_GET_PRIVATE(o)  (MAME_AUDIT_DIALOG (o)->priv)

G_DEFINE_TYPE (MameAuditDialog, mame_audit_dialog, GTK_TYPE_DIALOG)

/* Function prototypes */
static gboolean
progress_timeout                        (gpointer user_data);
static void
ngmameui_audit_window_set_details_label (MameAuditDialog *dlg, gchar *text);
static void
update_text_buffer                      (MameAuditDialog *dlg, gchar *text);
static void
mame_audit_dialog_update_labels         (MameAuditDialog *dlg);
static void
on_romset_audited                       (GmameuiAudit *audit, gchar *audit_line,
										 gint type, gint auditresult, gpointer user_data);
static void
on_rom_audit_complete                   (GmameuiAudit *audit, gpointer user_data);
static void
on_sample_audit_complete                (GmameuiAudit *audit, gpointer user_data);
static void
gmameui_audit_window_update_complete    (MameAuditDialog *dlg);
static void
mame_audit_dialog_response             (GtkDialog *dialog, gint response);
static void
mame_audit_dialog_destroy              (GtkObject *object);

/* Boilerplate functions */
static GObject *
mame_audit_dialog_constructor (GType                  type,
				guint                  n_construct_properties,
				GObjectConstructParam *construct_properties)
{
	GObject          *obj;
	MameAuditDialog *dialog;

	obj = G_OBJECT_CLASS (mame_audit_dialog_parent_class)->constructor (type,
									     n_construct_properties,
									     construct_properties);

	dialog = MAME_AUDIT_DIALOG (obj);

	return obj;
}

static void
mame_audit_dialog_class_init (MameAuditDialogClass *class)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (class);
	GtkObjectClass *gtkobject_class = GTK_OBJECT_CLASS (class);
	GtkDialogClass *gtkdialog_class = GTK_DIALOG_CLASS (class);

	gobject_class->constructor = mame_audit_dialog_constructor;
/*	gobject_class->get_property = mame_audit_dialog_get_property;
	gobject_class->set_property = mame_audit_dialog_set_property;*/

	gtkobject_class->destroy = mame_audit_dialog_destroy;
	gtkdialog_class->response = mame_audit_dialog_response;


	g_type_class_add_private (class,
				  sizeof (MameAuditDialogPrivate));

	/* Signals and properties go here */

}

static void
mame_audit_dialog_init (MameAuditDialog *dialog)
{
	MameAuditDialogPrivate *priv;

	GtkWidget *audit_vbox;
	GtkWidget *label1;
	
	gint size;
	PangoFontDescription *font_desc;
	
	int i;
	
	priv = G_TYPE_INSTANCE_GET_PRIVATE (dialog,
					    MAME_TYPE_AUDIT_DIALOG,
					    MameAuditDialogPrivate);

	dialog->priv = priv;
	
	/* Initialise private variables */
	priv->audit_complete = FALSE;
	priv->sample_audit_complete = FALSE;
	
	/* Re-initialise all status counts */
	for (i = 0; i < NUMBER_STATUS; i++) {
		priv->romset_count[i] = 0;
		priv->sampleset_count[i] = 0;
	}
	priv->num_roms = priv->num_samples = 0;
	/* Done initialising private variables */
	
	/* Build the UI and connect signals here */
	priv->xml = glade_xml_new (GLADEDIR "audit_window.glade", "vbox1", GETTEXT_PACKAGE);
	if (!priv->xml) {
		GMAMEUI_DEBUG ("Could not open Glade file %s", GLADEDIR "audit_window.glade");
		return;
	}

	/* Get the dialog contents */
	audit_vbox = glade_xml_get_widget (priv->xml, "vbox1");

	/* Add our dialog contents to the vbox of the dialog class */
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox),
						audit_vbox, TRUE, TRUE, 0);
	
	/* Set resizable to FALSE so that closing the expander shrinks the dialog */
	gtk_window_set_resizable (GTK_WINDOW (dialog), FALSE);
	
	gtk_widget_show_all (GTK_WIDGET (audit_vbox));
	
	gtk_dialog_set_has_separator (GTK_DIALOG (dialog), FALSE);
	
	/* Set the main text in the dialog to be large and bold */
	label1 = glade_xml_get_widget (priv->xml, "label1");
	size = pango_font_description_get_size (label1->style->font_desc);
	font_desc = pango_font_description_new ();
	pango_font_description_set_weight (font_desc, PANGO_WEIGHT_BOLD);
	pango_font_description_set_size (font_desc, size * PANGO_SCALE_LARGE);
	gtk_widget_modify_font (label1, font_desc);
	pango_font_description_free (font_desc);

	/* Text view for audit results */
	priv->details_check_buffer = gtk_text_buffer_new (NULL);
	priv->details_check_text = glade_xml_get_widget (priv->xml, "details_check_text");
	gtk_text_view_set_buffer (GTK_TEXT_VIEW (priv->details_check_text), priv->details_check_buffer);
	
	/* Label to present name of currently-processing ROM */
	priv->checking_games_label = glade_xml_get_widget (priv->xml, "checking_games_label");
#if GTK_CHECK_VERSION(2,6,0)
	gtk_label_set_ellipsize (GTK_LABEL (priv->checking_games_label), PANGO_ELLIPSIZE_MIDDLE);
#endif
	ngmameui_audit_window_set_details_label (dialog, _("Auditing MAME ROMs"));
	
	/* Labels for the running audit totals */
	priv->correct_roms_value = glade_xml_get_widget (priv->xml, "correct_roms_value");
	priv->bestavailable_roms_value = glade_xml_get_widget (priv->xml, "bestavailable_roms_value");
	priv->incorrect_roms_value = glade_xml_get_widget (priv->xml, "incorrect_roms_value");
	priv->notfound_roms_value = glade_xml_get_widget (priv->xml, "notfound_roms_value");
	priv->total_roms_value = glade_xml_get_widget (priv->xml, "total_roms_value");

	priv->correct_samples_value = glade_xml_get_widget (priv->xml, "correct_samples_value");
	priv->incorrect_samples_value = glade_xml_get_widget (priv->xml, "incorrect_samples_value");
	priv->notfound_samples_value = glade_xml_get_widget (priv->xml, "notfound_samples_value");
	priv->total_samples_value = glade_xml_get_widget (priv->xml, "total_samples_value");
	
	/* Set the progress bar to pulse every 100 ms. MAME no longer reports
	   missing ROMs, so its harder to estimate total progress */
	priv->pbar = (GtkProgressBar*) glade_xml_get_widget (priv->xml, "progressbar1");
	priv->timer = gdk_threads_add_timeout (100, progress_timeout, priv->pbar);
	
	/* Buttons */
	priv->stop_audit_button = gtk_dialog_add_button (GTK_DIALOG (dialog),
													 GTK_STOCK_STOP,
													 GTK_RESPONSE_REJECT);
		
	priv->close_audit_button = gtk_dialog_add_button (GTK_DIALOG (dialog),
													  GTK_STOCK_CLOSE,
													  GTK_RESPONSE_CLOSE);
	gtk_widget_set_sensitive (priv->close_audit_button, FALSE);
		
	/* Signal emitted whenever the audit process processes a romset or sampleset line */
	priv->romset_sigid = g_signal_connect (gui_prefs.audit, "romset-audited",
										   G_CALLBACK (on_romset_audited), dialog);
	
	/* Signal emitted when the ROM audit process finishes */
	priv->command_sigid = g_signal_connect (gui_prefs.audit, "rom-audit-complete",
											G_CALLBACK (on_rom_audit_complete), dialog);
	
	/* Signal emitted when the Sample audit process finishes */
	priv->command_sample_sigid = g_signal_connect (gui_prefs.audit, "sample-audit-complete",
												   G_CALLBACK (on_sample_audit_complete), dialog);
	
	/* We also need to initiate the audit process */
	/* FIXME TODO Do this in g_idle_add? */
	/* FIXME TODO Add return value which, if failed, stops the throbber, and shows an error message */
	mame_audit_start_full ();
}

GtkWidget *
mame_audit_dialog_new (GtkWindow *parent)
{
	GtkWidget *dialog;

	dialog = g_object_new (MAME_TYPE_AUDIT_DIALOG,
			       "title", _("Auditing ROMs"),
			       NULL);

	if (parent)
		gtk_window_set_transient_for (GTK_WINDOW (dialog), parent);

	return dialog;

}

static void
mame_audit_dialog_response (GtkDialog *dialog, gint response)
{
	MameAuditDialogPrivate *priv;
	
	priv = G_TYPE_INSTANCE_GET_PRIVATE (MAME_AUDIT_DIALOG (dialog),
					    MAME_TYPE_AUDIT_DIALOG,
					    MameAuditDialogPrivate);
	
	/* Stop the progress bar pulsing if it is in progress */
	if (priv->timer > 0) {
		g_source_remove (priv->timer);
		priv->timer = 0;
		gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (priv->pbar), 1.0);
	}
	
	/* Disconnect signal handlers so that these handlers do not further
	   update the audit window. If we created the MameAudit object only when
	   it was used, and destroyed it straight after, we wouldn't need to do this */
	g_signal_handler_disconnect (gui_prefs.audit, priv->romset_sigid);
	g_signal_handler_disconnect (gui_prefs.audit, priv->command_sigid);
	g_signal_handler_disconnect (gui_prefs.audit, priv->command_sample_sigid);
	
	switch (response)
	{
		case GTK_RESPONSE_REJECT:
			/* Stop button clicked */
			GMAMEUI_DEBUG ("Stopping");
			
			/* Stop the auditing process */
			mame_audit_stop_full_audit (gui_prefs.audit);
			
			gtk_window_set_title (GTK_WINDOW (dialog), _("Audit Stopped"));
			ngmameui_audit_window_set_details_label (MAME_AUDIT_DIALOG (dialog), _("Stopped"));
	
			gtk_widget_set_sensitive (priv->stop_audit_button, FALSE);
			gtk_widget_set_sensitive (priv->close_audit_button, TRUE);
			break;
		case GTK_RESPONSE_CLOSE:
			/* Close button clicked */
			gtk_widget_destroy (GTK_WIDGET (dialog));
			
			mame_gamelist_view_repopulate_contents (main_gui.displayed_list);

			break;
		case GTK_RESPONSE_DELETE_EVENT:
			/* Dialog closed */
			
			/* Stop the auditing process */
			mame_audit_stop_full_audit (gui_prefs.audit);

			gtk_widget_destroy (GTK_WIDGET (dialog));
			
			mame_gamelist_view_repopulate_contents (main_gui.displayed_list);

			break;
		default:
			g_assert_not_reached ();
	}
}

static void
mame_audit_dialog_destroy (GtkObject *object)
{
	MameAuditDialog *dlg;
	
GMAMEUI_DEBUG ("Destroying mame audit dialog...");	
	dlg = MAME_AUDIT_DIALOG (object);
	
	if (dlg->priv->xml)
		g_object_unref (dlg->priv->xml);
	
	g_object_unref (dlg->priv);
	
/*	GTK_OBJECT_CLASS (mame_audit_dialog_parent_class)->destroy (object);*/
	
GMAMEUI_DEBUG ("Destroying mame audit dialog... done");
}

static gboolean
progress_timeout (gpointer user_data)
{
	g_return_val_if_fail (user_data != NULL, FALSE);
	
	GtkWidget *pbar = (gpointer) user_data;
	gtk_progress_bar_pulse (GTK_PROGRESS_BAR (pbar));

	while (gtk_events_pending ())
		gtk_main_iteration ();
	
	/* Timeout function - need to return TRUE so it keeps getting called */
	return TRUE;
}

static void
ngmameui_audit_window_set_details_label (MameAuditDialog *dlg, gchar *text)
{
	PangoFontDescription *font_desc;
		
	gtk_label_set_text (GTK_LABEL (dlg->priv->checking_games_label), text);
	
	font_desc = pango_font_description_new ();
	pango_font_description_set_style (font_desc, PANGO_STYLE_ITALIC);
	gtk_widget_modify_font (dlg->priv->checking_games_label, font_desc);
	pango_font_description_free (font_desc);
}

static void
update_text_buffer (MameAuditDialog *dlg, gchar *text)
{
	GtkTextIter text_iter;
	
	gtk_text_buffer_get_end_iter (GTK_TEXT_BUFFER (dlg->priv->details_check_buffer), &text_iter);
	gtk_text_buffer_insert (GTK_TEXT_BUFFER (dlg->priv->details_check_buffer), &text_iter, text, -1);
	
	/* Append carriage return */
	gtk_text_buffer_get_end_iter (GTK_TEXT_BUFFER (dlg->priv->details_check_buffer), &text_iter);
	gtk_text_buffer_insert (GTK_TEXT_BUFFER (dlg->priv->details_check_buffer), &text_iter, "\n", -1);
}

/* This function is invoked as a callback from the g_spawn_async process when
   content is received on the pipe */
static void
on_romset_audited (GmameuiAudit *audit,
				   gchar *audit_line,
				   gint type,
				   gint auditresult,
				   gpointer user_data)
{
	gchar *title;
	
	MameAuditDialog *dlg = (gpointer) user_data;
	
	while (gtk_events_pending ())
		gtk_main_iteration ();

	/*GMAMEUI_DEBUG ("Audit: %d - %s", auditresult, audit_line);*/
	
	g_return_if_fail (dlg != NULL);
	g_return_if_fail ((auditresult >= 0) && (auditresult < NUMBER_STATUS));
	
	if (auditresult != NOTROMSET) {
		const gchar *romname;
		MameRomEntry *tmprom;
		
		/* Write the audit details to the text buffer */
		if ((auditresult == INCORRECT) || (auditresult == BEST_AVAIL))
			update_text_buffer (dlg, audit_line);

		/* Get the name of the rom being audited so we can update the processing label */
		romname = get_romset_name_from_audit_line (audit_line);

		tmprom = get_rom_from_gamelist_by_name (gui_prefs.gl, romname);
		
		g_return_if_fail (tmprom != NULL);
		
		if (type == AUDIT_TYPE_ROM) {
			dlg->priv->romset_count[auditresult]++;
			dlg->priv->nb_roms_checked++;

			title = g_strdup_printf (_("Auditing romset %s"), romname);

			g_object_set (tmprom, "has-roms", auditresult, NULL);

			/* FIXME TODO Redraw gamelist with updated ROM details */

			
		} else if (type == AUDIT_TYPE_SAMPLE) {
			dlg->priv->sampleset_count[auditresult]++;
			dlg->priv->nb_samples_checked++;
			
			title = g_strdup_printf (_("Auditing sampleset %s"), romname);

			g_object_set (tmprom, "has-samples", auditresult, NULL);

			/* FIXME TODO Redraw gamelist with updated ROM details */

		}

		ngmameui_audit_window_set_details_label (dlg, title);
		g_free (title);
	} else {
		/* Place-holder should we ever decide to handle individual ROM within the romset */
	}

	mame_audit_dialog_update_labels (dlg);
}

/* This function is invoked as a callback from the g_spawn_async process when
   the process is finished */
static void
on_rom_audit_complete (GmameuiAudit *audit, gpointer user_data)
{
	MameAuditDialog *dlg = (gpointer) user_data;
	
	/*GMAMEUI_DEBUG ("In audit window - rom audit has completed");*/
	dlg->priv->audit_complete = TRUE;
	
	/* Only update the window if both the ROM and Sample audits
	   have completed */
	if (dlg->priv->sample_audit_complete)
		gmameui_audit_window_update_complete (dlg);
}

/* This function is invoked as a callback from the g_spawn_async process when
   the process is finished */
static void
on_sample_audit_complete (GmameuiAudit *audit, gpointer user_data)
{
	MameAuditDialog *dlg = (gpointer) user_data;
	
	/*GMAMEUI_DEBUG ("In audit window - sample audit has completed");*/
	dlg->priv->sample_audit_complete = TRUE;
	
	/* Only update the window if both the ROM and Sample audits
	   have completed */
	if (dlg->priv->audit_complete)
		gmameui_audit_window_update_complete (dlg);
}

static void
gmameui_audit_window_update_complete (MameAuditDialog *dlg)
{
	gtk_window_set_title (GTK_WINDOW (dlg), _("Audit done"));
	ngmameui_audit_window_set_details_label (dlg, _("Done"));
	
	update_text_buffer (dlg, _("Audit done"));
	
	/* Stop the throbber - need to stop the progress_default timer first */
	if (dlg->priv->timer > 0) {
		g_source_remove (dlg->priv->timer);
		dlg->priv->timer = 0;
		gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (dlg->priv->pbar), 1.0);
	}
	
	gtk_widget_set_sensitive (dlg->priv->stop_audit_button, FALSE);
	gtk_widget_set_sensitive (dlg->priv->close_audit_button, TRUE);

}

static void
mame_audit_dialog_update_labels (MameAuditDialog *dialog)
{
	MameAuditDialogPrivate *priv;
	gchar numb[10];
	
	g_return_if_fail (dialog != NULL);
	
	priv = G_TYPE_INSTANCE_GET_PRIVATE (dialog,
					    MAME_TYPE_AUDIT_DIALOG,
					    MameAuditDialogPrivate);
	
	g_return_if_fail (priv != NULL);

	g_snprintf (numb, 10, "%d", priv->romset_count[CORRECT]);
	gtk_label_set_text (GTK_LABEL (priv->correct_roms_value), numb);
	
	g_snprintf (numb, 10, "%d", priv->romset_count[INCORRECT]);
	gtk_label_set_text (GTK_LABEL (priv->incorrect_roms_value), numb);
	
	g_snprintf (numb, 10,"%d", priv->romset_count[BEST_AVAIL]);
	gtk_label_set_text (GTK_LABEL (priv->bestavailable_roms_value), numb);
	
	g_snprintf (numb, 10,"%d", priv->romset_count[NOT_AVAIL]);
	gtk_label_set_text (GTK_LABEL (priv->notfound_roms_value), numb);
	
	g_snprintf (numb, 10,"%d", priv->sampleset_count[CORRECT]);
	gtk_label_set_text (GTK_LABEL (priv->correct_samples_value), numb);
	
	g_snprintf (numb, 10, "%d", priv->sampleset_count[INCORRECT]);
	gtk_label_set_text (GTK_LABEL (priv->incorrect_samples_value), numb);
	
	g_snprintf (numb, 10, "%d", priv->sampleset_count[NOT_AVAIL]);
	gtk_label_set_text (GTK_LABEL (priv->notfound_samples_value), numb);
	
	g_snprintf (numb, 10, "%d", priv->nb_roms_checked);
	gtk_label_set_text (GTK_LABEL (priv->total_roms_value), numb);
	
	g_snprintf (numb, 10,"%d", priv->nb_samples_checked);
	gtk_label_set_text (GTK_LABEL (priv->total_samples_value), numb);
}
