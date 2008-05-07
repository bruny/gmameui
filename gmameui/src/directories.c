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
#include <string.h>
#include <stdio.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtkcellrenderertext.h>
#include <gtk/gtkeditable.h>
#include <gtk/gtkentry.h>
#include <gtk/gtkfilechooserdialog.h>
#include <gtk/gtkhseparator.h>
#include <gtk/gtklabel.h>
#include <gtk/gtknotebook.h>
#include <gtk/gtkscrolledwindow.h>
#include <gtk/gtkstock.h>
#include <gtk/gtktable.h>
#include <gtk/gtkvbox.h>
#include <glade/glade.h>

#include "gui.h"
#include "directories.h"
#include "io.h"
#include "game_options.h"

static void
on_dir_browse_button_clicked        (GtkButton       *button,
                                        gpointer         user_data);

static void
on_file_browse_button_clicked       (GtkButton       *button,
                                        gpointer         user_data);

void add_path_to_tree_view (GtkWidget *button, gpointer user_data);
void add_item_to_tree_view (GtkWidget *button, gpointer user_data);
void remove_path_from_tree_view (GtkWidget *button, gpointer user_data);

static void
directories_selection_save_changes       (GtkWidget *widget);

static GtkWidget *directories_selection;

GtkTreeView *xmame_execs_tree_view;
GtkTreeModel *xmame_execs_tree_model;
static GtkWidget *xmame_execs_add_button;
static GtkWidget *xmame_execs_remove_button;

GtkTreeView *roms_path_tree_view;
GtkTreeModel *roms_path_tree_model;
static GtkWidget *roms_add_button;
static GtkWidget *roms_remove_button;

GtkTreeView *samples_path_tree_view;
GtkTreeModel *samples_path_tree_model;
static GtkWidget *samples_add_button;
static GtkWidget *samples_remove_button;

static GtkWidget *artwork_path_entry;
static GtkWidget *snapshot_path_entry;
static GtkWidget *artwork_path_entry;
static GtkWidget *flyer_path_entry;
static GtkWidget *cabinet_path_entry;
static GtkWidget *marquee_path_entry;
static GtkWidget *title_path_entry;
static GtkWidget *cpanel_path_entry;

static GtkWidget *cheatfile_path_entry;
static GtkWidget *hiscorefile_path_entry;
static GtkWidget *historyfile_path_entry;
static GtkWidget *mameinfofile_path_entry;
static GtkWidget *ctrlr_directory_entry;
static GtkWidget *catver_path_entry;
static GtkWidget *icons_path_entry;

gchar *delimited_str;   /* Used for processing list stores into gchar* arrays */

void
create_folderselection (gpointer current_entry,
			gboolean select_dir)
{
	GtkWidget *filechooser;
	gchar *temp_text;

	if (select_dir) {
		filechooser = gtk_file_chooser_dialog_new (_("Browse for Folder"),
							   GTK_WINDOW (directories_selection),
							   GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
							   GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
							   GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
							   NULL);
	} else {
		filechooser = gtk_file_chooser_dialog_new (_("Browse for File"),
							   GTK_WINDOW (directories_selection),
							   GTK_FILE_CHOOSER_ACTION_OPEN,
							   GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
							   GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
							   NULL);
	}
         
	temp_text = gtk_editable_get_chars (GTK_EDITABLE (current_entry), 0, -1);      
	gtk_file_chooser_set_filename (GTK_FILE_CHOOSER (filechooser), temp_text);    
	g_free (temp_text);
	
	if (gtk_dialog_run (GTK_DIALOG (filechooser)) == GTK_RESPONSE_ACCEPT) {
		gtk_entry_set_text (GTK_ENTRY (current_entry),
				    gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (filechooser)));
	}
	gtk_widget_destroy (filechooser);
}

static void
directories_selection_response (GtkWidget *dialog,
				gint       response_id,
				gpointer   user_data)
{
	switch (response_id) {
		case GTK_RESPONSE_OK:
			directories_selection_save_changes (dialog);
			break;
	}

	gtk_list_store_clear (GTK_LIST_STORE (xmame_execs_tree_model));
	gtk_list_store_clear (GTK_LIST_STORE (roms_path_tree_model));
	gtk_list_store_clear (GTK_LIST_STORE (samples_path_tree_model));

	gtk_widget_destroy (dialog);
}

GtkWidget *
create_directories_selection (void)
{
	gint i;
	GtkAccelGroup *accel_group;

	GtkTreeIter iter;
	GtkTreeModel *store;
	GtkTreeViewColumn *column;
	GtkCellRenderer *renderer;
	GtkTreeSelection *select;
	gchar **xmame_executables_array;

	GladeXML *xml = glade_xml_new (GLADEDIR "directories.glade", NULL, NULL);
	GtkWidget *directories_selection = glade_xml_get_widget (xml, "directories_selection");
	gtk_widget_show (GTK_WIDGET (directories_selection));

	accel_group = gtk_accel_group_new ();

	/* XMame Exe List */
	store = (GtkTreeModel *) gtk_list_store_new (1, G_TYPE_STRING);
	xmame_executables_array = xmame_table_get_all ();

	for (i = 0; xmame_executables_array[i] != NULL; i++) {
		gtk_list_store_append (GTK_LIST_STORE (store), &iter);  /* Acquire an iterator */
		gtk_list_store_set (GTK_LIST_STORE (store), &iter,
				    0, xmame_executables_array[i],
				    -1);
	}

	xmame_execs_tree_model = store;
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes (" ", renderer,
							   "text", 0,
							   NULL);

	xmame_execs_tree_view = GTK_TREE_VIEW (glade_xml_get_widget (xml, "xmame_execs_tree_view"));
	gtk_tree_view_append_column (GTK_TREE_VIEW (xmame_execs_tree_view), column);
	gtk_tree_view_set_model (GTK_TREE_VIEW (xmame_execs_tree_view), GTK_TREE_MODEL (store));

    	/* Roms Path List */
	store = (GtkTreeModel *) gtk_list_store_new (1, G_TYPE_STRING);
	if (gui_prefs.RomPath) {
		for (i = 0; gui_prefs.RomPath[i] != NULL; i++) {
			gtk_list_store_append (GTK_LIST_STORE (store), &iter);  /* Acquire an iterator */
			gtk_list_store_set (GTK_LIST_STORE (store), &iter,
					    0, gui_prefs.RomPath[i],
					    -1);
		}
	}
	roms_path_tree_model = store;
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes (" ", renderer,
							   "text", 0,
							   NULL);

	roms_path_tree_view = GTK_TREE_VIEW (glade_xml_get_widget (xml, "roms_path_tree_view"));
	gtk_tree_view_append_column (GTK_TREE_VIEW (roms_path_tree_view), column);
	gtk_tree_view_set_model (GTK_TREE_VIEW (roms_path_tree_view), GTK_TREE_MODEL (store));

	/* Samples Path List */
	store = (GtkTreeModel *) gtk_list_store_new (1,
						     G_TYPE_STRING);
	if (gui_prefs.SamplePath) {
		for (i = 0; gui_prefs.SamplePath[i] != NULL; i++) {
			gtk_list_store_append (GTK_LIST_STORE (store), &iter);  /* Acquire an iterator */
			gtk_list_store_set (GTK_LIST_STORE (store), &iter, 
					    0, gui_prefs.SamplePath[i],
					    -1);
		}
	}
	samples_path_tree_model = store;
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes (" ", renderer,
							   "text", 0,
							   NULL);

	samples_path_tree_view = GTK_TREE_VIEW (glade_xml_get_widget (xml, "samples_path_treeview"));
	gtk_tree_view_append_column (GTK_TREE_VIEW (samples_path_tree_view), column);
	gtk_tree_view_set_model (GTK_TREE_VIEW (samples_path_tree_view), GTK_TREE_MODEL (store));
    
	
	snapshot_path_entry = glade_xml_get_widget (xml, "snapshot_path_entry");
	gtk_entry_set_text (GTK_ENTRY (snapshot_path_entry), gui_prefs.SnapshotDirectory);	
	artwork_path_entry = glade_xml_get_widget (xml, "artwork_path_entry");
	gtk_entry_set_text (GTK_ENTRY (artwork_path_entry), gui_prefs.ArtworkDirectory);
	flyer_path_entry = glade_xml_get_widget (xml, "flyer_path_entry");
	gtk_entry_set_text (GTK_ENTRY (flyer_path_entry), gui_prefs.FlyerDirectory);
	cabinet_path_entry = glade_xml_get_widget (xml, "cabinet_path_entry");
	gtk_entry_set_text (GTK_ENTRY (cabinet_path_entry), gui_prefs.CabinetDirectory);
	marquee_path_entry = glade_xml_get_widget (xml, "marquee_path_entry");
	gtk_entry_set_text (GTK_ENTRY (marquee_path_entry), gui_prefs.MarqueeDirectory);
	title_path_entry = glade_xml_get_widget (xml, "title_path_entry");
	gtk_entry_set_text (GTK_ENTRY (title_path_entry), gui_prefs.TitleDirectory);
	cpanel_path_entry = glade_xml_get_widget (xml, "cpanel_path_entry");
	gtk_entry_set_text (GTK_ENTRY (cpanel_path_entry), gui_prefs.CPanelDirectory);

	catver_path_entry = glade_xml_get_widget (xml, "catver_path_entry");
	gtk_entry_set_text (GTK_ENTRY (catver_path_entry), gui_prefs.catverDirectory);
	historyfile_path_entry = glade_xml_get_widget (xml, "historyfile_path_entry");
	gtk_entry_set_text (GTK_ENTRY (historyfile_path_entry), gui_prefs.HistoryFile);
	mameinfofile_path_entry = glade_xml_get_widget (xml, "mameinfofile_path_entry");
	gtk_entry_set_text (GTK_ENTRY (mameinfofile_path_entry), gui_prefs.MameInfoFile);
	cheatfile_path_entry = glade_xml_get_widget (xml, "cheatfile_path_entry");
	gtk_entry_set_text (GTK_ENTRY (cheatfile_path_entry), gui_prefs.CheatFile);
	hiscorefile_path_entry = glade_xml_get_widget (xml, "hiscorefile_path_entry");
	gtk_entry_set_text (GTK_ENTRY (hiscorefile_path_entry), gui_prefs.HiscoreFile);
	ctrlr_directory_entry = glade_xml_get_widget (xml, "ctrlr_directory_entry");
	gtk_entry_set_text (GTK_ENTRY (ctrlr_directory_entry), gui_prefs.CtrlrDirectory);
	icons_path_entry = glade_xml_get_widget (xml, "icons_path_entry");
	gtk_entry_set_text (GTK_ENTRY (icons_path_entry), gui_prefs.IconDirectory);

	/* Button Cicked */
	/* Executables */
	xmame_execs_add_button = glade_xml_get_widget (xml, "xmame_execs_add_button");
	xmame_execs_remove_button = glade_xml_get_widget (xml, "xmame_execs_remove_button");
	g_signal_connect (G_OBJECT (xmame_execs_add_button), "clicked",
			    G_CALLBACK (add_item_to_tree_view), xmame_execs_tree_model);
	g_signal_connect (G_OBJECT (xmame_execs_remove_button), "clicked",
			    G_CALLBACK (remove_path_from_tree_view),
			    xmame_execs_tree_view);

	/* MAME basic */
	samples_add_button = glade_xml_get_widget (xml, "samples_add_button");
	samples_remove_button = glade_xml_get_widget (xml, "samples_remove_button");
	g_signal_connect (G_OBJECT (samples_add_button), "clicked",
			    G_CALLBACK (add_path_to_tree_view),
			    samples_path_tree_model);
	g_signal_connect (G_OBJECT (samples_remove_button), "clicked",
			    G_CALLBACK (remove_path_from_tree_view),
			    samples_path_tree_view);

	roms_add_button = glade_xml_get_widget (xml, "roms_add_button");
	roms_remove_button = glade_xml_get_widget (xml, "roms_remove_button");
	g_signal_connect (G_OBJECT (roms_add_button), "clicked",
			    G_CALLBACK (add_path_to_tree_view),
			    roms_path_tree_model);
	g_signal_connect (G_OBJECT (roms_remove_button), "clicked",
			    G_CALLBACK (remove_path_from_tree_view),
			    roms_path_tree_view);

	/* MAME Paths */
	GtkWidget *snapshot_browse_button = glade_xml_get_widget (xml, "snapshot_browse_button");
	g_signal_connect (G_OBJECT (snapshot_browse_button), "clicked",
			    G_CALLBACK (on_dir_browse_button_clicked),
			    snapshot_path_entry);
	GtkWidget *artwork_browse_button = glade_xml_get_widget (xml, "artwork_browse_button");
	g_signal_connect (G_OBJECT (artwork_browse_button), "clicked",
			    G_CALLBACK (on_dir_browse_button_clicked),
			    artwork_path_entry);
	GtkWidget *flyer_browse_button = glade_xml_get_widget (xml, "flyer_browse_button");
	g_signal_connect (G_OBJECT (flyer_browse_button), "clicked",
			    G_CALLBACK (on_dir_browse_button_clicked),
			    flyer_path_entry);
	GtkWidget *cabinet_browse_button = glade_xml_get_widget (xml, "cabinet_browse_button");
	g_signal_connect (G_OBJECT (cabinet_browse_button), "clicked",
			    G_CALLBACK (on_dir_browse_button_clicked),
			    cabinet_path_entry);
	GtkWidget *marquee_browse_button = glade_xml_get_widget (xml, "marquee_browse_button");
	g_signal_connect (G_OBJECT (marquee_browse_button), "clicked",
			    G_CALLBACK (on_dir_browse_button_clicked),
			    marquee_path_entry);
	GtkWidget *title_browse_button = glade_xml_get_widget (xml, "title_browse_button");
	g_signal_connect (G_OBJECT (title_browse_button), "clicked",
			    G_CALLBACK (on_dir_browse_button_clicked),
			    title_path_entry);
	
	/* Support files */
	GtkWidget *catver_browse_button = glade_xml_get_widget (xml, "catver_browse_button");
	g_signal_connect (G_OBJECT (catver_browse_button), "clicked",
			    G_CALLBACK (on_dir_browse_button_clicked),
			    catver_path_entry);
	GtkWidget *historyfile_browse_button = glade_xml_get_widget (xml, "historyfile_browse_button");
	g_signal_connect (G_OBJECT (historyfile_browse_button), "clicked",
			    G_CALLBACK (on_file_browse_button_clicked),
			    historyfile_path_entry);
	GtkWidget *mameinfofile_browse_button = glade_xml_get_widget (xml, "mameinfofile_browse_button");
	g_signal_connect (G_OBJECT (mameinfofile_browse_button), "clicked",
			    G_CALLBACK (on_file_browse_button_clicked),
			    mameinfofile_path_entry);
	
	GtkWidget *cheat_browse_button = glade_xml_get_widget (xml, "cheat_browse_button");
	g_signal_connect (G_OBJECT (cheat_browse_button), "clicked",
			    G_CALLBACK (on_file_browse_button_clicked),
			    cheatfile_path_entry);
	
	GtkWidget *hiscore_browse_button = glade_xml_get_widget (xml, "hiscore_browse_button");
	g_signal_connect (G_OBJECT (hiscore_browse_button), "clicked",
			    G_CALLBACK (on_file_browse_button_clicked),
			    hiscorefile_path_entry);

	GtkWidget *ctrlr_browse_button = glade_xml_get_widget (xml, "ctrlr_browse_button");
	g_signal_connect (G_OBJECT (ctrlr_browse_button), "clicked",
			    G_CALLBACK (on_dir_browse_button_clicked),
			    ctrlr_directory_entry);
	
	GtkWidget *icons_browse_button = glade_xml_get_widget (xml, "icons_browse_button");
	g_signal_connect (G_OBJECT (icons_browse_button), "clicked",
			    G_CALLBACK (on_dir_browse_button_clicked),
			    icons_path_entry);
	
	/* XMame Execs List */
	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (xmame_execs_tree_view));
	gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);

	/* Roms Path List */
	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (samples_path_tree_view));
	gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);

	/* Samples Path List */
	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (roms_path_tree_view));
	gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);

	gtk_window_add_accel_group (GTK_WINDOW (directories_selection), accel_group);

	g_signal_connect (G_OBJECT (directories_selection), "response",
		  G_CALLBACK (directories_selection_response),
		  NULL);

	return directories_selection;
}

void add_path_to_tree_view (GtkWidget *button,
			    gpointer user_data)
{
	GtkTreeModel *model = (gpointer) user_data;
	GtkTreeIter iter;
	gchar *temp_text, *text;
	gint i, size;
	gboolean already_exist = FALSE;
	
	/* Create new directory file chooser */
	GtkWidget *chooser = gtk_file_chooser_dialog_new ("Select directory",
							  NULL, GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
							  GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
							  GTK_STOCK_OK, GTK_RESPONSE_OK,
							  NULL);
	gint response = gtk_dialog_run (GTK_DIALOG (chooser));
	switch (response)
	{
		case GTK_RESPONSE_OK:
			temp_text = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (chooser));
			gtk_widget_destroy (chooser);    

			/* Check if what we are trying to add is not already in the list */
			already_exist = FALSE;
			size = gtk_tree_model_iter_n_children (GTK_TREE_MODEL (model), NULL);

			for (i = 0; i < size; i++) {
				gtk_tree_model_iter_nth_child (GTK_TREE_MODEL (model), &iter, NULL, i);
				gtk_tree_model_get (model, &iter, 0, &text, -1);
				if (g_ascii_strcasecmp (text, temp_text) == 0)
					already_exist = TRUE;
			}

			if (!already_exist) {    
				/* Acquire an iterator */
				gtk_list_store_append (GTK_LIST_STORE (model), &iter);
				gtk_list_store_set (GTK_LIST_STORE (model), &iter,
						    0, temp_text,
						    -1);
			}

			g_free (temp_text);
			break;
		default:
			gtk_widget_destroy (chooser);
			break;
	}				    
}

void add_item_to_tree_view (GtkWidget *button,
			    gpointer user_data)
{
	GtkTreeModel *model = (gpointer) user_data;
	GtkTreeIter iter;
	gchar *temp_text, *text;
	gint i, size;
	gboolean already_exist = FALSE;
	
	/* Create new directory file chooser */
	GtkWidget *chooser = gtk_file_chooser_dialog_new ("Select MAME executable",
							  NULL, GTK_FILE_CHOOSER_ACTION_OPEN,
							  GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
							  GTK_STOCK_OK, GTK_RESPONSE_OK,
							  NULL);
	gint response = gtk_dialog_run (GTK_DIALOG (chooser));
	switch (response)
	{
		case GTK_RESPONSE_OK:
			temp_text = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (chooser));
			gtk_widget_destroy (chooser);
			
			/* Check if what we are trying to add is not already in the list */
			already_exist = FALSE;
			size = gtk_tree_model_iter_n_children (GTK_TREE_MODEL (model), NULL);

			for (i = 0; i < size; i++) {
				gtk_tree_model_iter_nth_child (GTK_TREE_MODEL (model), &iter, NULL, i);
				gtk_tree_model_get (model, &iter, 0, &text, -1);
				if (!strcmp (text, temp_text))
					already_exist = TRUE;
			}

			if (!already_exist) {
				gtk_list_store_append (GTK_LIST_STORE (model), &iter);  /* Acquire an iterator */
				gtk_list_store_set (GTK_LIST_STORE (model), &iter,
						    0, temp_text,
						    -1);
			}

			g_free (temp_text);
			break;
		default:
			gtk_widget_destroy (chooser);
			break;
	}				    
}

void
remove_path_from_tree_view (GtkWidget *button, gpointer user_data)
{
	GtkTreeView *view = (gpointer) user_data;
	GtkTreeModel *model;
	GtkTreeIter iter;

	GtkTreeSelection *select;
	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (view));
	model = gtk_tree_view_get_model (GTK_TREE_VIEW (view));
	if (gtk_tree_selection_get_selected (select, &model, &iter)) {
		gtk_tree_selection_unselect_all (select);
		gtk_list_store_remove (GTK_LIST_STORE (model), &iter);
	}
}


void
on_dir_browse_button_clicked           (GtkButton       *button,
                                        gpointer         user_data)
{
	create_folderselection (user_data, TRUE);
}


void
on_file_browse_button_clicked         (GtkButton       *button,
                                        gpointer         user_data)
{
	create_folderselection (user_data, FALSE);
}

#if 0
static gboolean
remove_hash_value (gpointer key,
		   gpointer value,
		   gpointer user_data)
{
	g_free (value);
	return TRUE;
}
#endif


  gboolean path_tree_model_foreach (GtkTreeModel  *model,
                              GtkTreePath   *path,
                              GtkTreeIter   *iter,
                              gpointer       userdata)
  {
    gchar *name;
	gchar *tmp;  

    gtk_tree_model_get (model, iter, 0, &name, -1);

	if (!delimited_str) {
		delimited_str = g_strdup (name);	
	} else {
		tmp = g_strjoin (":", delimited_str, name, NULL);
		delimited_str = g_strdup (tmp);
		g_free (tmp);
	}
	  
	 /* Return FALSE to retrieve next row */
	  return FALSE;
  }

void
directories_selection_save_changes (GtkWidget *widget)
{
	GtkTreeIter iter;
	gint size;
	gint i;
	gchar *text = NULL;
	gboolean changed_flag;

	g_return_if_fail (xmame_execs_tree_model != NULL);
	g_return_if_fail (roms_path_tree_model != NULL);
	g_return_if_fail (samples_path_tree_model != NULL);
	
	changed_flag = FALSE;
	
	/* free the main config paths*/
	/* GMAMEUI */
	if (gui_prefs.FlyerDirectory) g_free (gui_prefs.FlyerDirectory);
	if (gui_prefs.CabinetDirectory) g_free (gui_prefs.CabinetDirectory);
	if (gui_prefs.MarqueeDirectory) g_free (gui_prefs.MarqueeDirectory);
	if (gui_prefs.TitleDirectory) g_free (gui_prefs.TitleDirectory);
	if (gui_prefs.CPanelDirectory) g_free (gui_prefs.CPanelDirectory);
	if (gui_prefs.IconDirectory) g_free (gui_prefs.IconDirectory);
	/* XMame basic */
	if (gui_prefs.SamplePath) g_strfreev (gui_prefs.SamplePath);
	if (gui_prefs.ArtworkDirectory) g_free (gui_prefs.ArtworkDirectory);
	/* XMame additional */
	if (gui_prefs.SnapshotDirectory) g_free (gui_prefs.SnapshotDirectory);
	if (gui_prefs.HistoryFile) g_free (gui_prefs.HistoryFile);
	if (gui_prefs.MameInfoFile) g_free (gui_prefs.MameInfoFile);
	if (gui_prefs.CheatFile) g_free (gui_prefs.CheatFile);
	if (gui_prefs.HiscoreFile) g_free (gui_prefs.HiscoreFile);
	/* User Resources *
	 User Resources are not yet implemented in the directories window
	if (gui_prefs.HiscoreDirectory) g_free (gui_prefs.HiscoreDirectory);
	if (gui_prefs.DiffDirectory) g_free (gui_prefs.DiffDirectory);
	if (gui_prefs.InputDirectory) g_free (gui_prefs.InputDirectory);
	if (gui_prefs.ConfigDirectory) g_free (gui_prefs.ConfigDirectory);
	if (gui_prefs.NVRamDirectory) g_free (gui_prefs.NVRamDirectory);
	if (gui_prefs.MemCardDirectory) g_free (gui_prefs.MemCardDirectory);
	if (gui_prefs.StateDirectory) g_free (gui_prefs.StateDirectory);
	if (gui_prefs.inipath) g_free (gui_prefs.inipath);*/


	/* and replace them by the new ones*/
	/* GMAMEUI */
	gchar *catver_path;
	catver_path = gtk_editable_get_chars (GTK_EDITABLE (catver_path_entry), 0, -1);
	if (g_ascii_strcasecmp (gui_prefs.catverDirectory, catver_path)) {
		GMAMEUI_DEBUG ("catver path changed from %s to %s",
			       gui_prefs.catverDirectory,
			       catver_path);
		g_free (gui_prefs.catverDirectory);
		gui_prefs.catverDirectory = g_strdup (catver_path);
		/* Updating with the new catver file */
		g_list_foreach (game_list.versions, (GFunc)g_free, NULL);
		g_list_free (game_list.versions);
		game_list.versions = NULL;
		load_catver_ini ();

		/* Updating the UI if necessary */
		Columns_type type;
		g_object_get (selected_filter, "type", &type, NULL);
		if ( (type == CATEGORY) || (type == MAMEVER)
		     || (gui_prefs.FolderID == CATEGORIES) || (gui_prefs.FolderID == VERSIONS) ) {
			gui_prefs.FolderID = AVAILABLE;
			gmameui_message (WARNING, GTK_WINDOW (directories_selection), _("Current Folder may not exist after loading the new catver file.\nMoving to the \"Available\" game folder"));
			create_gamelist_content ();
		} else if ( (gui_prefs.current_mode == DETAILS || gui_prefs.current_mode == DETAILS_TREE)
			  && (gui_prefs.ColumnShown[MAMEVER]==TRUE || gui_prefs.ColumnShown[CATEGORY] == TRUE)) {
			create_gamelist_content ();
		}
	}
	g_free (catver_path);

	/* XMame basic */
	gui_prefs.ArtworkDirectory = gtk_editable_get_chars (GTK_EDITABLE (artwork_path_entry), 0, -1);
	gui_prefs.FlyerDirectory = gtk_editable_get_chars (GTK_EDITABLE (flyer_path_entry), 0, -1);
	gui_prefs.CabinetDirectory = gtk_editable_get_chars (GTK_EDITABLE (cabinet_path_entry), 0, -1);
	gui_prefs.MarqueeDirectory = gtk_editable_get_chars (GTK_EDITABLE (marquee_path_entry), 0, -1);
	gui_prefs.TitleDirectory = gtk_editable_get_chars (GTK_EDITABLE (title_path_entry), 0, -1);
	gui_prefs.CPanelDirectory = gtk_editable_get_chars (GTK_EDITABLE (cpanel_path_entry), 0, -1);
	gui_prefs.IconDirectory = gtk_editable_get_chars (GTK_EDITABLE (icons_path_entry), 0, -1);

	/* XMame additional */
	gui_prefs.SnapshotDirectory = gtk_editable_get_chars (GTK_EDITABLE (snapshot_path_entry), 0, -1);
	gui_prefs.HistoryFile = gtk_editable_get_chars (GTK_EDITABLE (historyfile_path_entry), 0, -1);
	gui_prefs.MameInfoFile = gtk_editable_get_chars (GTK_EDITABLE (mameinfofile_path_entry), 0, -1);
	gui_prefs.HiscoreFile = gtk_editable_get_chars (GTK_EDITABLE (hiscorefile_path_entry), 0, -1);
	gui_prefs.CheatFile = gtk_editable_get_chars (GTK_EDITABLE (cheatfile_path_entry), 0, -1);
	/* If the ctrlr directory has changed, we reload the default options */
	if (!gtk_editable_get_chars (GTK_EDITABLE (ctrlr_directory_entry), 0, -1)
	    || strcmp (gui_prefs.CtrlrDirectory, gtk_editable_get_chars (GTK_EDITABLE (ctrlr_directory_entry), 0, -1)))
	{
		g_free (gui_prefs.CtrlrDirectory);
		gui_prefs.CtrlrDirectory = gtk_editable_get_chars (GTK_EDITABLE (ctrlr_directory_entry), 0, -1);
		load_options (NULL);
	}

	/* Get sample paths from tree model */
	gtk_tree_model_foreach (GTK_TREE_MODEL (samples_path_tree_model), path_tree_model_foreach, NULL);
	gui_prefs.SamplePath = g_strsplit (delimited_str, ":", -1);
	g_free (delimited_str);
	delimited_str = NULL;

	/* Get ROM paths from tree model and determine whether the list is different from that
	   in the preferences; if so, we have changed and should prompt the user to
	   update the game list */
	gchar *roms_paths_as_str;
	gtk_tree_model_foreach (GTK_TREE_MODEL (roms_path_tree_model), path_tree_model_foreach, NULL);
	roms_paths_as_str = g_strjoinv (":",gui_prefs.RomPath);
	if (g_ascii_strcasecmp (delimited_str, roms_paths_as_str) != 0)
		changed_flag = TRUE;
	gui_prefs.RomPath = g_strsplit (delimited_str, ":", -1);
	g_free (roms_paths_as_str);
	g_free (delimited_str);
	delimited_str = NULL;

	if (changed_flag) {
		/* Do we perform the quickcheck? */
		/* FIXME TODO Do this automatically as a background thread, updating the
		   progress bar
		GtkWidget *dialog;
		gint result;
		dialog = gtk_message_dialog_new (GTK_WINDOW (directories_selection),
						GTK_DIALOG_MODAL,
						GTK_MESSAGE_WARNING,
						GTK_BUTTONS_YES_NO,
						_("The Rom Path has been modified.\nDo you want to refresh the gamelist after the directory window is closed?"));
		result = gtk_dialog_run (GTK_DIALOG (dialog));
		switch (result)
		{
			case GTK_RESPONSE_YES:
				refresh_game_list=TRUE;
				break;
			default:
				break;
		}
		gtk_widget_destroy (dialog);
		*/
	}

	/* XMame Executables */
	current_exec = NULL;
	xmame_table_free ();
	xmame_table_init ();
	size = gtk_tree_model_iter_n_children (GTK_TREE_MODEL (xmame_execs_tree_model),NULL);
	for (i = 0; i < size; i++) {
		gtk_tree_model_iter_nth_child (GTK_TREE_MODEL (xmame_execs_tree_model), &iter, NULL, i);
		gtk_tree_model_get (xmame_execs_tree_model, &iter, 0, &text, -1);
		xmame_table_add (text);
	}

	/* Save the directories file */
	save_dirs_ini();

	/* remove and destroy the submenu */
	add_exec_menu ();

	/* here we should already have a valid executable except if mame is not present in path */
	if (!current_exec) {
		gmameui_message (ERROR, GTK_WINDOW (directories_selection), _("No valid xmame executables found"));
	}
	
	g_free (text);
}
