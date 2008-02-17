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

#include "gmameui.h"
#include "gui.h"
#include "column_layout.h"

static void
on_column_layout_up_button_clicked      (GtkButton       *button,
                                         gpointer         user_data);
static void
on_column_layout_down_button_clicked    (GtkButton       *button,
                                         gpointer         user_data);
static void
on_column_layout_show_button_clicked    (GtkButton       *button,
                                         gpointer         user_data);
static void
on_column_layout_hide_button_clicked   (GtkButton       *button,
                                         gpointer         user_data);

static void
on_column_layout_hide_select (GtkTreeSelection *selection, gpointer data);

static void
on_column_layout_show_select (GtkTreeSelection *selection, gpointer data);



static GtkWidget *column_layout_up_button;
static GtkWidget *column_layout_down_button;
static GtkWidget *column_layout_show_button;
static GtkWidget *column_layout_hide_button;
static GtkWidget *column_layout_apply_button;

static gboolean apply_sensitive;
static GtkTreeView *column_show_tree_view;
static GtkTreeModel *column_show_tree_model;
static GtkTreeIter column_show_iter;
static GtkTreeView *column_hide_tree_view;
static GtkTreeModel *column_hide_tree_model;
static GtkTreeIter column_hide_iter;

static gint position_show_list;
static gint position_hide_list;
static gint     TempColumnOrder[NUMBER_COLUMN];
static gboolean TempColumnShown[NUMBER_COLUMN];
static gint     TempColumnShownId[NUMBER_COLUMN];
static gint     TempColumnHiddenId[NUMBER_COLUMN];

static void
apply_changes (void)
{
	gint i;
	for (i = 0; i < NUMBER_COLUMN; i++) {
		gui_prefs.ColumnOrder[i] = TempColumnOrder[i];
		gui_prefs.ColumnShown[i] = TempColumnShown[i];
		gui_prefs.ColumnShownId[i] = TempColumnShownId[i];
		gui_prefs.ColumnHiddenId[i] = TempColumnHiddenId[i];
	}
}

static void
column_layout_ok (GtkWidget *dialog,
		  gpointer   user_data)
{
	if (apply_sensitive == TRUE) {
		apply_changes ();
		if ((gui_prefs.current_mode == DETAILS) || (gui_prefs.current_mode == DETAILS_TREE)) {
			create_gamelist (gui_prefs.current_mode);
		}
	}

	gtk_widget_destroy (dialog);
}

static void
column_layout_apply (GtkWidget *widget,
		     gpointer   user_data)
{
	apply_changes ();
	if ((gui_prefs.current_mode == DETAILS) || (gui_prefs.current_mode == DETAILS_TREE)) {
		create_gamelist (gui_prefs.current_mode);
	}
	gtk_widget_set_sensitive (GTK_WIDGET (user_data), FALSE);
	apply_sensitive = FALSE;
}

static void
column_layout_response (GtkWidget *dialog,
			gint       response_id,
			gpointer   user_data)
{
	switch (response_id) {
	case GTK_RESPONSE_OK:
		column_layout_ok (dialog, user_data);
		break;
	case GTK_RESPONSE_APPLY:
		column_layout_apply (dialog, user_data);
		break;
	case GTK_RESPONSE_CANCEL:
	default:
		gtk_widget_destroy (dialog);
		break;
	}
}

GtkWidget *
create_column_layout_window (void)
{
	GtkWidget *column_layout_window;
	GtkWidget *column_layout_table;
	GtkWidget *column_show_vbox;
	GtkWidget *column_show_label;
	GtkWidget *column_show_scrolledwindow;
	GtkWidget *column_show_viewport;
	GtkWidget *column_hide_vbox;
	GtkWidget *column_hide_label;
	GtkWidget *column_hide_scrolledwindow;
	GtkWidget *column_hide_viewport;
	GtkWidget *column_layout_move_table;
	GtkWidget *column_layout_ok_button;
	GtkWidget *column_layout_cancel_button;
	GtkAccelGroup *accel_group;
	GtkTreeIter iter;
	GtkTreeModel *store = NULL;
	GtkTreeView *treeview = NULL;
	GtkTreeViewColumn *column = NULL;
	GtkCellRenderer *renderer = NULL;
	GList *column_show_num_list = NULL;
	GList *column_hide_num_list = NULL;
	GtkTreeSelection *select;
	gint i;
	GList *C = NULL;

	position_show_list = -1;
	position_hide_list = -1;

	g_list_free (column_show_num_list);
	column_show_num_list = get_columns_shown_list ();

	g_list_free (column_hide_num_list);
	column_hide_num_list = get_columns_hidden_list ();

	for (i = 0; i <= NUMBER_COLUMN; i++)
	{
		TempColumnOrder[i] = gui_prefs.ColumnOrder[i];
		TempColumnShown[i] = gui_prefs.ColumnShown[i];
		TempColumnShownId[i] = gui_prefs.ColumnShownId[i];
		TempColumnHiddenId[i] = gui_prefs.ColumnHiddenId[i];
	}

	/* UI Start Here */
	accel_group = gtk_accel_group_new ();

	column_layout_window = gtk_dialog_new_with_buttons (_ ("Column Layout"),
							    GTK_WINDOW (MainWindow),
							     GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_NO_SEPARATOR,
							    NULL);

	column_layout_table = gtk_table_new (3, 3, FALSE);
	gtk_widget_show (column_layout_table);
	gtk_container_add (GTK_CONTAINER (GTK_DIALOG (column_layout_window)->vbox), column_layout_table);
	gtk_container_set_border_width (GTK_CONTAINER (column_layout_table), 5);
	gtk_table_set_row_spacings (GTK_TABLE (column_layout_table), 3);
	gtk_table_set_col_spacings (GTK_TABLE (column_layout_table), 3);


	/* Column Shown */
	column_show_vbox = gtk_vbox_new (FALSE, 0);
	gtk_widget_show (column_show_vbox);
	gtk_table_attach (GTK_TABLE (column_layout_table), column_show_vbox, 3, 4, 1, 2,
			  (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			  (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);


	column_show_label = gtk_label_new (_ ("Shown columns"));
	gtk_widget_show (column_show_label);
	gtk_box_pack_start (GTK_BOX (column_show_vbox), column_show_label, FALSE, FALSE, 0);
	gtk_misc_set_alignment (GTK_MISC (column_show_label), 0, 0.5);


	column_show_scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (column_show_scrolledwindow);
	gtk_box_pack_start (GTK_BOX (column_show_vbox), column_show_scrolledwindow, TRUE, TRUE, 0);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (column_show_scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);


	column_show_viewport = gtk_viewport_new (NULL, NULL);
	gtk_widget_show (column_show_viewport);
	gtk_container_add (GTK_CONTAINER (column_show_scrolledwindow), column_show_viewport);

	/* Building the Tree View - Column Show */
	store = (GtkTreeModel *) gtk_list_store_new (2,
						     G_TYPE_STRING,
						     G_TYPE_INT);
	for (C = g_list_first (column_show_num_list); C != NULL; C = g_list_next (C))
	{
		gtk_list_store_append (GTK_LIST_STORE (store), &iter);  /* Acquire an iterator */
		gtk_list_store_set (GTK_LIST_STORE (store), &iter,
				    0,                   column_title (GPOINTER_TO_INT (C->data)),
				    1,                   GPOINTER_TO_INT (C->data),
				    -1);
	}
	column_show_tree_model = store;
	treeview = GTK_TREE_VIEW (gtk_tree_view_new ());
	gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (treeview), FALSE);
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes (" ", renderer,
							   "text", 0,
							   NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
	gtk_tree_view_set_model (GTK_TREE_VIEW (treeview), GTK_TREE_MODEL (store));
	column_show_tree_view = treeview;
	gtk_widget_show_all (GTK_WIDGET (column_show_tree_view));
	gtk_container_add (GTK_CONTAINER (column_show_viewport), GTK_WIDGET (column_show_tree_view));


	/* Column Hide */

	column_hide_vbox = gtk_vbox_new (FALSE, 0);
	gtk_widget_show (column_hide_vbox);
	gtk_table_attach (GTK_TABLE (column_layout_table), column_hide_vbox, 1, 2, 1, 2,
			  (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			  (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);


	column_hide_label = gtk_label_new (_ ("Available columns"));
	gtk_widget_show (column_hide_label);
	gtk_box_pack_start (GTK_BOX (column_hide_vbox), column_hide_label, FALSE, FALSE, 0);
	gtk_misc_set_alignment (GTK_MISC (column_hide_label), 0, 0.5);

	column_hide_scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (column_hide_scrolledwindow);
	gtk_box_pack_start (GTK_BOX (column_hide_vbox), column_hide_scrolledwindow, TRUE, TRUE, 0);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (column_hide_scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

	column_hide_viewport = gtk_viewport_new (NULL, NULL);
	gtk_widget_show (column_hide_viewport);
	gtk_container_add (GTK_CONTAINER (column_hide_scrolledwindow), column_hide_viewport);

	/* Building the Tree View - Column Hide */
	store = (GtkTreeModel *) gtk_list_store_new (2,
						     G_TYPE_STRING,
						     G_TYPE_INT);
	for (C = g_list_first (column_hide_num_list); C != NULL; C = g_list_next (C))
	{
		gtk_list_store_append (GTK_LIST_STORE (store), &iter);  /* Acquire an iterator */
		gtk_list_store_set (GTK_LIST_STORE (store), &iter,
				    0,                   column_title (GPOINTER_TO_INT (C->data)),
				    1,                   GPOINTER_TO_INT (C->data),
				    -1);
	}
	column_hide_tree_model=store;
	treeview = GTK_TREE_VIEW (gtk_tree_view_new ());
	gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (treeview), FALSE);
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes (" ", renderer,
							   "text", 0,
							   NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
	gtk_tree_view_set_model (GTK_TREE_VIEW (treeview), GTK_TREE_MODEL (store));
	column_hide_tree_view = treeview;
	gtk_widget_show_all (GTK_WIDGET (column_hide_tree_view));
	gtk_container_add (GTK_CONTAINER (column_hide_viewport), GTK_WIDGET (column_hide_tree_view));


	/* Move Buttons */
	column_layout_move_table = gtk_table_new (4, 3, FALSE);
	gtk_widget_show (column_layout_move_table);
	gtk_table_attach (GTK_TABLE (column_layout_table), column_layout_move_table, 2, 3, 1, 2,
			  GTK_SHRINK,
			  GTK_SHRINK, 0, 0);
	gtk_container_set_border_width (GTK_CONTAINER (column_layout_move_table), 5);
	gtk_table_set_row_spacings (GTK_TABLE (column_layout_move_table), 6);
	gtk_table_set_col_spacings (GTK_TABLE (column_layout_move_table), 6);

	column_layout_up_button = gtk_button_new_from_stock (GTK_STOCK_GO_UP);
	gtk_widget_show (column_layout_up_button);
	gtk_table_attach (GTK_TABLE (column_layout_move_table), column_layout_up_button, 2, 4, 1, 2,
			  (GtkAttachOptions) (GTK_FILL),
			  (GtkAttachOptions) (0), 0, 0);

	column_layout_down_button = gtk_button_new_from_stock (GTK_STOCK_GO_DOWN);
	gtk_widget_show (column_layout_down_button);
	gtk_table_attach (GTK_TABLE (column_layout_move_table), column_layout_down_button, 2, 4, 3, 4,
			  (GtkAttachOptions) (GTK_FILL),
			  (GtkAttachOptions) (0), 0, 0);

	column_layout_show_button = gtk_button_new_from_stock (GTK_STOCK_ADD);
	gtk_widget_show (column_layout_show_button);
	gtk_table_attach (GTK_TABLE (column_layout_move_table), column_layout_show_button, 3, 5, 2, 3,
			  (GtkAttachOptions) (GTK_FILL),
			  (GtkAttachOptions) (0), 0, 0);

	column_layout_hide_button = gtk_button_new_from_stock (GTK_STOCK_REMOVE);
	gtk_widget_show (column_layout_hide_button);
	gtk_table_attach (GTK_TABLE (column_layout_move_table), column_layout_hide_button, 1, 3, 2, 3,
			  (GtkAttachOptions) (GTK_FILL),
			  (GtkAttachOptions) (0), 0, 0);

	gtk_widget_set_sensitive (GTK_WIDGET (column_layout_up_button), FALSE);
	gtk_widget_set_sensitive (GTK_WIDGET (column_layout_down_button), FALSE);
	gtk_widget_set_sensitive (GTK_WIDGET (column_layout_show_button), FALSE);
	gtk_widget_set_sensitive (GTK_WIDGET (column_layout_hide_button), FALSE);


	/* Buttons */
	column_layout_apply_button = gtk_dialog_add_button (GTK_DIALOG (column_layout_window),
							    GTK_STOCK_APPLY,
							    GTK_RESPONSE_APPLY);
	gtk_widget_set_sensitive (GTK_WIDGET (column_layout_apply_button), FALSE);
	apply_sensitive = FALSE;

	column_layout_cancel_button = gtk_dialog_add_button (GTK_DIALOG (column_layout_window),
							     GTK_STOCK_CANCEL,
							     GTK_RESPONSE_CANCEL);

	column_layout_ok_button = gtk_dialog_add_button (GTK_DIALOG (column_layout_window),
							 GTK_STOCK_OK,
							 GTK_RESPONSE_OK);

	/* Move Buttons */
	g_signal_connect (G_OBJECT (column_layout_up_button), "clicked",
			    G_CALLBACK (on_column_layout_up_button_clicked),
			    NULL);
	g_signal_connect (G_OBJECT (column_layout_down_button), "clicked",
			    G_CALLBACK (on_column_layout_down_button_clicked),
			    NULL);
	g_signal_connect (G_OBJECT (column_layout_show_button), "clicked",
			    G_CALLBACK (on_column_layout_show_button_clicked),
			    NULL);
	g_signal_connect (G_OBJECT (column_layout_hide_button), "clicked",
			    G_CALLBACK (on_column_layout_hide_button_clicked),
			    NULL);
	/* List */
	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (column_hide_tree_view));
	gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
	g_signal_connect (G_OBJECT (select), "changed",
			  G_CALLBACK (on_column_layout_hide_select),
			  NULL);
	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (column_show_tree_view));
	gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
	g_signal_connect (G_OBJECT (select), "changed",
			  G_CALLBACK (on_column_layout_show_select),
			  NULL);

	gtk_window_add_accel_group (GTK_WINDOW (column_layout_window), accel_group);

	g_signal_connect (column_layout_window, "response",
			  G_CALLBACK (column_layout_response), NULL);

	return column_layout_window;
}

void
on_column_layout_up_button_clicked      (GtkButton       *button,
                                         gpointer         user_data)
{
	gint temp_position_show_list;
	guint temp;
	GtkTreeIter my_iter;

	temp_position_show_list = position_show_list;
	/* ReOrder ColumnOrder */
	temp = TempColumnOrder[TempColumnShownId[position_show_list]];
	TempColumnOrder[TempColumnShownId[position_show_list]] = TempColumnOrder[TempColumnShownId[position_show_list - 1]];
	TempColumnOrder[TempColumnShownId[position_show_list - 1]] = temp;
	/* ReOrder ColumnShownId */
	temp = TempColumnShownId[position_show_list];
	TempColumnShownId[position_show_list] = TempColumnShownId[position_show_list - 1];
	TempColumnShownId[position_show_list - 1] = temp;
	/* Rebuild UI List */
	my_iter = column_show_iter;
	position_show_list--;
	if (position_show_list != 0)
	{
		if (gtk_tree_model_iter_nth_child (GTK_TREE_MODEL (column_show_tree_model), &my_iter, NULL, position_show_list))
			gtk_list_store_move_before (GTK_LIST_STORE (column_show_tree_model),
						    &column_show_iter,
						    &my_iter);
	}else
	{
		gtk_list_store_move_after (GTK_LIST_STORE (column_show_tree_model),
					   &column_show_iter,
					   NULL);
	}
	if (position_show_list == 0)
		gtk_widget_set_sensitive (GTK_WIDGET (column_layout_up_button), FALSE);
	else
		gtk_widget_set_sensitive (GTK_WIDGET (column_layout_up_button), TRUE);
	if ( (position_show_list+1) == gtk_tree_model_iter_n_children (GTK_TREE_MODEL (column_show_tree_model), NULL) )
		gtk_widget_set_sensitive (GTK_WIDGET (column_layout_down_button), FALSE);
	else
		gtk_widget_set_sensitive (GTK_WIDGET (column_layout_down_button), TRUE);
	/* Activate Apply Button */
	gtk_widget_set_sensitive (GTK_WIDGET (column_layout_apply_button), TRUE);
	apply_sensitive = TRUE;
}


void
on_column_layout_down_button_clicked    (GtkButton       *button,
                                         gpointer         user_data)
{
	gint temp_position_show_list;
	guint temp;
	GtkTreeIter my_iter;

	temp_position_show_list=position_show_list;
	/* ReOrder ColumnOrder */
	temp = TempColumnOrder[TempColumnShownId[position_show_list]];
	TempColumnOrder[TempColumnShownId[position_show_list]] = TempColumnOrder[TempColumnShownId[position_show_list + 1]];
	TempColumnOrder[TempColumnShownId[position_show_list + 1]] = temp;
	/* ReOrder ColumnShownId */
	temp = TempColumnShownId[position_show_list];
	TempColumnShownId[position_show_list] = TempColumnShownId[position_show_list + 1];
	TempColumnShownId[position_show_list + 1] = temp;
	/* Rebuild UI List */
	my_iter = column_show_iter;
	position_show_list++;
	if (gtk_tree_model_iter_nth_child (GTK_TREE_MODEL (column_show_tree_model), &my_iter, NULL, position_show_list))
		gtk_list_store_move_after (GTK_LIST_STORE (column_show_tree_model),
                                             &column_show_iter,
                                             &my_iter);
	if (position_show_list == 0)
		gtk_widget_set_sensitive (GTK_WIDGET (column_layout_up_button), FALSE);
	else
		gtk_widget_set_sensitive (GTK_WIDGET (column_layout_up_button), TRUE);
	if (position_show_list == gtk_tree_model_iter_n_children (GTK_TREE_MODEL (column_show_tree_model), NULL) - 1)
		gtk_widget_set_sensitive (GTK_WIDGET (column_layout_down_button), FALSE);
	else
		gtk_widget_set_sensitive (GTK_WIDGET (column_layout_down_button), TRUE);
	/* Activate Apply Button */
	gtk_widget_set_sensitive (GTK_WIDGET (column_layout_apply_button), TRUE);
	apply_sensitive = TRUE;
}


void
on_column_layout_show_button_clicked    (GtkButton       *button,
                                         gpointer         user_data)
{
	gint i, new_pos, tmp_rowId;
	GtkTreeIter tmp_iter;
	GtkTreeIter new_iter;
	GtkTreeSelection *select;

	/* Update ColumnShown + ColumnShownId / ColumnHiddenId */
	TempColumnShown[TempColumnHiddenId[position_hide_list]] = TRUE;
	new_pos = 0;
	for (i = 0;
	     TempColumnOrder[TempColumnShownId[i]] < TempColumnOrder[TempColumnHiddenId[position_hide_list]]
		     && TempColumnShownId[i] >= 0;
	     new_pos = ++i)
	{}
	for (i = NUMBER_COLUMN - 1; i > new_pos; i--)
	{
		TempColumnShownId[i] = TempColumnShownId[i - 1];
	}
	TempColumnShownId[new_pos] = TempColumnHiddenId[position_hide_list];
	for (i = position_hide_list; i < NUMBER_COLUMN - 1; i++)
	{
		TempColumnHiddenId[i] = TempColumnHiddenId[i + 1];
	}
	TempColumnHiddenId[NUMBER_COLUMN - 1] = -1;

	/* Rebuild UI */
	tmp_iter = column_hide_iter;
	gtk_tree_model_get (column_hide_tree_model, &column_hide_iter, 1, &tmp_rowId, -1);
	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (column_hide_tree_view));
	if (position_hide_list == gtk_tree_model_iter_n_children (GTK_TREE_MODEL (column_hide_tree_model), NULL) - 1) {
		gtk_tree_selection_unselect_all (select);
		position_hide_list = -1;
	} else {
		gtk_tree_model_iter_next (GTK_TREE_MODEL (column_hide_tree_model), &column_hide_iter);
		gtk_tree_selection_select_iter  (select, &column_hide_iter);
		position_hide_list--;
	}
	gtk_list_store_remove (GTK_LIST_STORE (column_hide_tree_model), &tmp_iter);

	new_pos = -1;
	do {
		new_pos++;
		if (gtk_tree_model_iter_nth_child (GTK_TREE_MODEL (column_show_tree_model), &tmp_iter, NULL, new_pos)) {
			gtk_tree_model_get (column_show_tree_model, &tmp_iter, 1, &i, -1);
		} else {
			new_pos = -1;
			break;
		}
	} while (TempColumnOrder[i] < TempColumnOrder[tmp_rowId]);
	if (new_pos == -1) {
		gtk_list_store_append (GTK_LIST_STORE (column_show_tree_model), &new_iter);
	} else {
		gtk_list_store_insert (GTK_LIST_STORE (column_show_tree_model), &new_iter, new_pos);
		if (new_pos <= position_show_list)
			position_show_list++;
	}

	gtk_list_store_set (GTK_LIST_STORE (column_show_tree_model), &new_iter,
			    0,                   column_title (tmp_rowId),
			    1,                   tmp_rowId,
			    -1);

	/* Activate Apply Button */
	gtk_widget_set_sensitive (GTK_WIDGET (column_layout_apply_button), TRUE);
	apply_sensitive = TRUE;
}


void
on_column_layout_hide_button_clicked   (GtkButton       *button,
					gpointer         user_data)
{
	gint i, new_pos, tmp_rowId;
	GtkTreeIter tmp_iter;
	GtkTreeIter new_iter;
	GtkTreeSelection *select;

	/* Update ColumnShown + ColumnShownId / ColumnHiddenId */
	TempColumnShown[TempColumnShownId[position_show_list]] = FALSE;
	new_pos = 0;
	for (i = 0;
	     TempColumnOrder[TempColumnHiddenId[i]] < TempColumnOrder[TempColumnShownId[position_show_list]]
		     && TempColumnHiddenId[i] >= 0;
	     new_pos = ++i)
	{}
	for (i = NUMBER_COLUMN - 1; i > new_pos; i--)
	{
		TempColumnHiddenId[i] = TempColumnHiddenId[i - 1];
	}
	TempColumnHiddenId[new_pos] = TempColumnShownId[position_show_list];
	for (i = position_show_list; i < NUMBER_COLUMN - 1; i++)
	{
		TempColumnShownId[i] = TempColumnShownId[i + 1];
	}
	TempColumnShownId[NUMBER_COLUMN - 1] = -1;

	/* Rebuild UI */
	tmp_iter = column_show_iter;
	gtk_tree_model_get (column_show_tree_model, &column_show_iter, 1, &tmp_rowId, -1);
	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (column_show_tree_view));
	if (position_show_list == gtk_tree_model_iter_n_children (GTK_TREE_MODEL (column_show_tree_model), NULL) - 1)
	{
		gtk_tree_selection_unselect_all (select);
		position_show_list = -1;
	}else
	{
		gtk_tree_model_iter_next (GTK_TREE_MODEL (column_show_tree_model), &column_show_iter);
		gtk_tree_selection_select_iter (select, &column_show_iter);
		position_show_list--;
	}
	gtk_list_store_remove (GTK_LIST_STORE (column_show_tree_model), &tmp_iter);

	new_pos = -1;
	do {
		new_pos++;
		if (gtk_tree_model_iter_nth_child (GTK_TREE_MODEL (column_hide_tree_model), &tmp_iter, NULL, new_pos))
		{
			gtk_tree_model_get (column_hide_tree_model, &tmp_iter, 1, &i, -1);
		}else
		{
			new_pos = -1;
			break;
		}
	} while (TempColumnOrder[i] < TempColumnOrder[tmp_rowId]);
	if (new_pos == -1)
	{
		gtk_list_store_append (GTK_LIST_STORE (column_hide_tree_model), &new_iter);
	}else
	{
		gtk_list_store_insert (GTK_LIST_STORE (column_hide_tree_model), &new_iter, new_pos);
		if (new_pos <= position_hide_list)
			position_hide_list++;
	}

	gtk_list_store_set (GTK_LIST_STORE (column_hide_tree_model), &new_iter,
			    0,                   column_title (tmp_rowId),
			    1,                   tmp_rowId,
			    -1);

	/* Activate Apply Button */
	gtk_widget_set_sensitive (GTK_WIDGET (column_layout_apply_button), TRUE);
	apply_sensitive = TRUE;
}


void
on_column_layout_hide_select (GtkTreeSelection *selection, gpointer data)
{
	GtkTreeIter iter;
	GtkTreePath *tree_path;
	guint rowId;
	gint rowpos;

	if (gtk_tree_selection_get_selected (selection, &column_hide_tree_model, &iter))
	{
		gtk_tree_model_get (column_hide_tree_model, &iter, 1, &rowId, -1);
		tree_path = gtk_tree_model_get_path (GTK_TREE_MODEL (column_hide_tree_model), &iter);
		rowpos = atoi (gtk_tree_path_to_string (tree_path));
		gtk_tree_path_free (tree_path);
		column_hide_iter = iter;
		position_hide_list = rowpos;
		GMAMEUI_DEBUG ("Select hide: %i (%i) Order %i", rowpos, rowId, TempColumnOrder[rowId]);
		gtk_widget_set_sensitive (GTK_WIDGET (column_layout_show_button), TRUE);
	}else
	{
		position_hide_list = -1;
		GMAMEUI_DEBUG ("Unselect hide");
		gtk_widget_set_sensitive (GTK_WIDGET (column_layout_show_button), FALSE);
	}
}

void
on_column_layout_show_select (GtkTreeSelection *selection, gpointer data)
{
	GtkTreeIter iter;
	GtkTreePath *tree_path;
	gint rowId;
	gint rowpos;

	if (gtk_tree_selection_get_selected (selection, &column_show_tree_model, &iter))
	{
		gtk_tree_model_get (column_show_tree_model, &iter, 1, &rowId, -1);
		tree_path = gtk_tree_model_get_path (GTK_TREE_MODEL (column_show_tree_model), &iter);
		rowpos = atoi (gtk_tree_path_to_string (tree_path));
		gtk_tree_path_free (tree_path);
		column_show_iter = iter;
		position_show_list = rowpos;
		GMAMEUI_DEBUG ("Select show: %i (%i) Order %i", rowpos, rowId, TempColumnOrder[rowId]);
	}else
	{
		position_show_list = -1;
		rowId = -1;
		GMAMEUI_DEBUG ("Unselect show");
	}

	if ((position_show_list == 0) || (position_show_list == -1))
		gtk_widget_set_sensitive (GTK_WIDGET (column_layout_up_button), FALSE);
	else
		gtk_widget_set_sensitive (GTK_WIDGET (column_layout_up_button), TRUE);
	if ((position_show_list == gtk_tree_model_iter_n_children (GTK_TREE_MODEL (column_show_tree_model), NULL) - 1) || (position_show_list == -1))
		gtk_widget_set_sensitive (GTK_WIDGET (column_layout_down_button), FALSE);
	else
		gtk_widget_set_sensitive (GTK_WIDGET (column_layout_down_button), TRUE);
	/* We don't hide the Game Name Column */
	if ((rowId == GAMENAME) || (rowId == -1))
		gtk_widget_set_sensitive (GTK_WIDGET (column_layout_hide_button), FALSE);
	else
		gtk_widget_set_sensitive (GTK_WIDGET (column_layout_hide_button), TRUE);
}
