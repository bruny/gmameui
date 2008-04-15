/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * GMAMEUI
 *
 * Copyright 2007-2008 Andrew Burton <adb@iinet.net.au>
 * based on Anjuta code for AnjutaPreferencesDialog object
 * Copyright (C) 2002 Dave Camp
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

#include <gtk/gtk.h>

#include "common.h"
#include "mame_options_dialog.h"

struct _MameOptionsDialogPrivate {
	GtkWidget *treeview;	/* This is for functionality of the sidebar */
	GtkListStore *store;	/* This is for functionality of the sidebar */

	GtkWidget *notebook;
};

enum {
	COL_NAME,
	COL_TITLE,
	COL_PIXBUF,
	COL_WIDGET,
	LAST_COL
};  /* Columns in the liststore */

static void mame_options_dialog_class_init    (MameOptionsDialogClass *class);
static void mame_options_dialog_init (MameOptionsDialog      *dlg);

G_DEFINE_TYPE (MameOptionsDialog, mame_options_dialog,
			   GTK_TYPE_DIALOG)

static void
mame_options_dialog_finalize (GObject *obj)
{
	MameOptionsDialog *dlg = MAME_OPTIONS_DIALOG (obj);	

	if (dlg->priv->store) {
		g_object_unref (dlg->priv->store);
		dlg->priv->store = NULL;
	}
	
	g_free (dlg->priv);
	
	((GObjectClass *) mame_options_dialog_parent_class)->finalize (obj);
}

static void
mame_options_dialog_class_init (MameOptionsDialogClass *class)
{
	GObjectClass *object_class = G_OBJECT_CLASS (class);

	object_class->finalize = mame_options_dialog_finalize;
}

static void
add_category_columns (MameOptionsDialog *dlg)
{
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	
	renderer = gmameui_cell_renderer_captioned_image_new ();
	g_object_ref_sink (renderer);
	column = gtk_tree_view_column_new_with_attributes (_("Category"),
							   renderer,
							   "text",
							   COL_TITLE,
							   "pixbuf",
							   COL_PIXBUF,
							   NULL);

	gtk_tree_view_append_column (GTK_TREE_VIEW (dlg->priv->treeview),
				     column);
}

/* When an item in the sidebar is selected, we change the tab
   to the appropriate section */
static void
selection_changed_cb (GtkTreeSelection *selection,
		      MameOptionsDialog *dlg)
{
	GtkTreeIter iter;

	if (gtk_tree_selection_get_selected (selection, NULL, &iter)) {
		GtkWidget *widget;

		gtk_tree_model_get (GTK_TREE_MODEL (dlg->priv->store), &iter,
				    COL_WIDGET, &widget, -1);
		
		gtk_notebook_set_current_page 
			(GTK_NOTEBOOK (dlg->priv->notebook),
			 gtk_notebook_page_num (GTK_NOTEBOOK (dlg->priv->notebook),
						widget));
		
	}
}

/* Creates a hbox containing a treeview (the sidebar) and a notebook */
static void
mame_options_dialog_init (MameOptionsDialog *dlg)
{
	/* Widget declarations */
	GtkWidget *hbox;
	GtkWidget *scrolled_window;
	GtkTreeSelection *selection;
	GtkTreeSortable *sortable;
	
	dlg->priv = g_new0 (MameOptionsDialogPrivate, 1);
	
	/* Now set up the dialog */
	hbox = gtk_hbox_new (FALSE, 0);	
	
	dlg->priv->treeview = gtk_tree_view_new ();
	gtk_widget_show (dlg->priv->treeview);
	gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (dlg->priv->treeview),
					   FALSE);
	dlg->priv->store = gtk_list_store_new (LAST_COL, 
					       G_TYPE_STRING,
					       G_TYPE_STRING,
					       GDK_TYPE_PIXBUF,
					       GTK_TYPE_WIDGET,
					       G_TYPE_INT);
	sortable = GTK_TREE_SORTABLE (dlg->priv->store);
	gtk_tree_sortable_set_sort_column_id (sortable, COL_TITLE,
										  GTK_SORT_ASCENDING);
	
	gtk_tree_view_set_model (GTK_TREE_VIEW (dlg->priv->treeview),
				 GTK_TREE_MODEL (dlg->priv->store));

	add_category_columns (dlg);

	scrolled_window = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
					GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
	gtk_scrolled_window_set_shadow_type 
		(GTK_SCROLLED_WINDOW (scrolled_window), GTK_SHADOW_IN);
	
	gtk_widget_show (scrolled_window);
	gtk_container_add (GTK_CONTAINER (scrolled_window), 
			   dlg->priv->treeview);
	gtk_box_pack_start (GTK_BOX (hbox), scrolled_window,
			    FALSE, FALSE, 0);

	dlg->priv->notebook = gtk_notebook_new ();
	gtk_widget_show (dlg->priv->notebook);
	gtk_notebook_set_show_tabs (GTK_NOTEBOOK (dlg->priv->notebook), FALSE);
	gtk_notebook_set_show_border (GTK_NOTEBOOK (dlg->priv->notebook), 
				      FALSE);
	gtk_container_set_border_width (GTK_CONTAINER (dlg->priv->notebook),
					8);
	
	gtk_box_pack_start (GTK_BOX (hbox), dlg->priv->notebook,
			    TRUE, TRUE, 0);


	selection = gtk_tree_view_get_selection 
		(GTK_TREE_VIEW (dlg->priv->treeview));

	gtk_tree_selection_set_mode (selection, GTK_SELECTION_SINGLE);

	g_signal_connect (selection, "changed", 
			  G_CALLBACK (selection_changed_cb), dlg);

	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dlg)->vbox), hbox,
			    TRUE, TRUE, 0);
	
	gtk_dialog_add_button (GTK_DIALOG (dlg), GTK_STOCK_CLOSE, -7);
	
	gtk_widget_show (hbox);
}

GtkWidget *
mame_options_dialog_new (void)
{
	return g_object_new (MAME_TYPE_OPTIONS_DIALOG, 
				 "title", "MAME Options",
				 NULL);
}

void
mame_options_dialog_add_page (MameOptionsDialog *dlg,
									const gchar *name,
									const gchar *title,
									GdkPixbuf *icon,
									GtkWidget *page)
{
	GtkTreeIter iter;
	GtkTreeSelection* selection =
		gtk_tree_view_get_selection (GTK_TREE_VIEW (dlg->priv->treeview));
	GtkTreeIter first;	  
	gtk_widget_show (page);
	
	gtk_notebook_append_page (GTK_NOTEBOOK (dlg->priv->notebook), page, NULL);

	gtk_list_store_append (dlg->priv->store, &iter);
	
	gtk_list_store_set (dlg->priv->store, &iter,
			    COL_NAME, name,
			    COL_TITLE, title,
			    COL_PIXBUF, icon,
			    COL_WIDGET, page,
			    -1);
	
		gtk_tree_model_get_iter_first (GTK_TREE_MODEL (dlg->priv->store),
									   &first);
		gtk_tree_selection_select_iter (selection, &first);
}
