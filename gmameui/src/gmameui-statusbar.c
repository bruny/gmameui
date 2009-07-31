/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * GMAMEUI
 *
 * Copyright 2009 Andrew Burton <adb@iinet.net.au>
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
#include "gmameui-statusbar.h"

struct _GMAMEUIStatusbarPrivate {

	int pulse_timeout_id;   /* Allows us to control the pulsing progress bar
				   during timeout periods */
	
	GtkWidget *progress;    /* GtkProgressBar */
	
};

G_DEFINE_TYPE (GMAMEUIStatusbar, gmameui_statusbar, GTK_TYPE_STATUSBAR)

#define GMAMEUI_STATUSBAR_GET_PRIVATE(o)  (GMAMEUI_STATUSBAR (o)->priv)

/* Function prototypes */
static void
gmameui_statusbar_finalize              (GObject *object);

static gboolean
progress_timeout                     (gpointer user_data);

/* Boilerplate functions */
static GObject *
gmameui_statusbar_constructor (GType                  type,
                            guint                  n_construct_properties,
                            GObjectConstructParam *construct_properties)
{
	GObject       *obj;
	GMAMEUIStatusbar *sb;

	obj = G_OBJECT_CLASS (gmameui_statusbar_parent_class)->constructor (type,
	                                                                 n_construct_properties,
	                                                                 construct_properties);

	sb = GMAMEUI_STATUSBAR (obj);

	return obj;
}

static void
gmameui_statusbar_class_init (GMAMEUIStatusbarClass *class)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (class);
	GtkObjectClass *gtkobject_class = GTK_OBJECT_CLASS (class);
	GtkStatusbarClass *gtkstatusbar_class = GTK_STATUSBAR_CLASS (class);

	gobject_class->constructor = gmameui_statusbar_constructor;
/*	gobject_class->get_property = gmameui_statusbar_get_property;
	gobject_class->set_property = gmameui_statusbar_set_property;*/

	gobject_class->finalize = gmameui_statusbar_finalize;

	g_type_class_add_private (class,
				  sizeof (GMAMEUIStatusbarPrivate));

	/* Signals and properties go here */

}

static void
gmameui_statusbar_init (GMAMEUIStatusbar *sb)
{
	GMAMEUIStatusbarPrivate *priv;


	priv = G_TYPE_INSTANCE_GET_PRIVATE (sb,
					    GMAMEUI_TYPE_STATUSBAR,
					    GMAMEUIStatusbarPrivate);

	sb->priv = priv;

	/* Define visual appearance */
	gtk_box_set_homogeneous (GTK_BOX (sb), FALSE);
	
	/* Initialise private variables */

	/* Add progress bar, but hide it until required */
	sb->priv->progress = gtk_progress_bar_new ();
	gtk_box_pack_start (GTK_BOX (sb), sb->priv->progress, TRUE, TRUE, 6);
	gtk_widget_hide (sb->priv->progress);

	/* Connect signals */

}

GMAMEUIStatusbar *
gmameui_statusbar_new (void)
{
	GMAMEUIStatusbar *sb;

	sb = GMAMEUI_STATUSBAR (g_object_new (GMAMEUI_TYPE_STATUSBAR, NULL));

	return sb;

}

static void
gmameui_statusbar_finalize (GObject *object)
{
	GMAMEUIStatusbar *sb;
	
GMAMEUI_DEBUG ("Destroying mame statusbar...");	
	sb = GMAMEUI_STATUSBAR (object);
	
	
	g_object_unref (sb->priv);
	
	G_OBJECT_CLASS (gmameui_statusbar_parent_class)->finalize (object);

/*	GTK_OBJECT_CLASS (gmameui_statusbar_parent_class)->destroy (object);*/
	
GMAMEUI_DEBUG ("Destroying mame statusbar... done");
}

void
gmameui_statusbar_start_pulse (GMAMEUIStatusbar *sb)
{
	/* Show the progressbar */
	gtk_widget_show (sb->priv->progress);
	
	sb->priv->pulse_timeout_id = gdk_threads_add_timeout (100,
	                                                      progress_timeout,
	                                                      sb->priv->progress);
}

void
gmameui_statusbar_stop_pulse (GMAMEUIStatusbar *sb)
{
	/* Stop the throbber - need to stop the progress_default timer first */
	if (sb->priv->pulse_timeout_id > 0) {
		g_source_remove (sb->priv->pulse_timeout_id);
		sb->priv->pulse_timeout_id = 0;
		gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (sb->priv->progress), 1.0);
	}

	/* Now hide the progressbar */
	gtk_widget_hide (sb->priv->progress);
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

void
gmameui_statusbar_set_progressbar_text (GMAMEUIStatusbar *sb, gchar *text)
{
	gtk_progress_bar_set_text (GTK_PROGRESS_BAR (sb->priv->progress),
	                           text);
}
