/*
 * GMAMEUI
 *
 * Copyright 2010 Andrew Burton <adb@iinet.net.au>
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

#include "gui.h"
#include "gmameui-listoutput.h"
#include "mame-exec.h"
#include "gmameui-zip-utils.h"
#include "gmameui-rommgr-dlg.h"
#include "rom_entry.h"
#include "game_list.h"

/* Improvements:
	- button to fix ROM where available
    - processing count list
    - don't hang while generating --listxml

	needs to sort roms and clones
	needs to add labels
	needs to have re-sizable column headings
	neogeo roms are coming up as incorrect (e.g. nam1975), even though they are fine
	some roms are reporting more errors than there are (e.g. ghoulsu)

*/

enum {
	COL_NAME,
	COL_REGION,
	COL_STATUSTXT,
	COL_STATUS,
};

struct _GMAMEUIRomMgrDialogPrivate {
	GtkBuilder *builder;

	GtkTreeStore *treestore;
	GtkListStore *liststore;

	GtkTreeModel *filter_model;	/* Wraps the treestore and allows filtering of
	                               Correct romsets */

	GtkTreeView *tv;
	GtkTreeView *lv;

	GList *avail_romsets;	/* GList of gchar* items representing romset names */

	gint total_romsets, total_ok;
};

#define GMAMEUI_ROMMGR_DIALOG_GET_PRIVATE(o)  (GMAMEUI_ROMMGR_DIALOG (o)->priv)

G_DEFINE_TYPE (GMAMEUIRomMgrDialog, gmameui_rommgr_dialog, GTK_TYPE_DIALOG)

/* Function prototypes */
static void
gmameui_rommgr_dialog_response             (GtkDialog *dialog, gint response);
static void
gmameui_rommgr_dialog_destroy              (GtkObject *object);

/* This function generates a GList of all the romsets that exist in the rom paths */
static void
get_avail_romsets (GMAMEUIRomMgrDialog *dialog)
{
	GValueArray *va_rom_paths;
	GDir *romdir;
	const gchar *rompath;
	
	g_object_get (main_gui.gui_prefs, "rom-paths", &va_rom_paths, NULL);
	if (va_rom_paths) {
		guint i;
		for (i = 0; i < va_rom_paths->n_values; i++) {
			const gchar *romset_path;
			
			rompath = g_value_get_string (g_value_array_get_nth (va_rom_paths, i));

			romdir = g_dir_open (rompath, 0, NULL);	/* AAA FIXME TODO Handle error */

			while (romset_path = g_dir_read_name (romdir)) {
				gchar *romset_path_full;

				romset_path_full = g_build_filename (rompath, romset_path, NULL);
				

				/* If it's a zip file... FIXME TODO Doesn't discriminate on zip files */
				if (g_file_test (romset_path_full, G_FILE_TEST_IS_REGULAR)) {
					MameRomEntry *romset;
					gchar *romname;

					romname = get_romname_from_zipfile (romset_path);
					
					romset = get_rom_from_gamelist_by_name (gui_prefs.gl, romname);
					if (romset)
						dialog->priv->avail_romsets = g_list_append (dialog->priv->avail_romsets, g_strdup (romname));
					/*else
						GMAMEUI_DEBUG ("  Error - Couldn't find romset %s in gamelist", romset_path);*/

					g_free (romname);
				} /*else
					GMAMEUI_DEBUG ("IGNORING FILE %s", romset_path);*/

				g_free (romset_path_full);
			}

			g_dir_close (romdir);
		}
	}
}

static void
add_romset_contents_to_hashtable (gpointer data, gpointer user_data)
{
	MameRomEntry *romset = NULL;
	gchar *romname = NULL;
	
	romname = (gchar *) data;

	g_return_if_fail (romname != NULL);
	
//GMAMEUI_DEBUG ("In add_romset_contents_to_hashtable processing rom %s", romname);
	romset = get_rom_from_gamelist_by_name (gui_prefs.gl, romname);

//GMAMEUI_DEBUG ("  Found romset %s", mame_rom_entry_get_romname (romset));
	g_return_if_fail (romset != NULL);
	
	mame_rom_entry_add_roms_to_hashtable (romset);
//GMAMEUI_DEBUG ("Finished add_romset_contents_to_hashtable");
}

static gchar *
get_romfix_status (romfix *fix)
{
	int i;
	gchar *msg;

	i = fix->status;
	
	/* AAA FIXME TODO matches enum in rom_entry.c */
	if (i == 0)
		msg = g_strdup (_("Incorrect"));
	else if (i == 1)
		msg = g_strdup (_("OK"));
	else if (i == 2)
		msg = g_strdup (_("should be renamed to"));
	else if (i == 3)
		msg = g_strdup_printf (_("available in parent %s"), fix->container);
	else if (i == 4)
		msg = g_strdup_printf (_("duplicated in parent %s"), fix->container);
	else if (i == 5)
		msg = g_strdup_printf (_("available in romset %s"), fix->container);
	else if (i == 6)
		msg = g_strdup (_("contained in a BIOS"));
	else
		g_assert_not_reached ();

	return msg;
}

/* This callback handles when the GMAMEUIRomfixList emits that a romset fixlist
   has been generated, and updates the treeview with the details */
static void
on_romset_fix_found (GMAMEUIRomfixList *fixlist,
				   gchar *romset_name,
				   gpointer *data,
				   gpointer user_data)
{
	GtkTreeIter iter, child_iter;
	gchar *rom_fullname;
	GList *roms;

	romset_fixes *fixes = (romset_fixes *) data;
	GMAMEUIRomMgrDialog *dialog = (GMAMEUIRomMgrDialog *) user_data;

		/* Add the romset to the tree */
	rom_fullname = g_strdup_printf ("%s (%s)",
	                                fixes->romset_fullname, fixes->romset_name);
	gtk_tree_store_append (dialog->priv->treestore, &iter, NULL);
	gtk_tree_store_set (dialog->priv->treestore, &iter,
	                    COL_NAME, rom_fullname,
	                    COL_REGION, "",
	                    COL_STATUSTXT, /*get_romfix_status (fixes->status)*/"",
	                    COL_STATUS, fixes->status,	/* If status is OK, set to 1, for filtering */
	                    -1);
	g_free (rom_fullname);

	/* Now add the roms to the tree as nested elements underneath the romset */
	roms = g_list_first (fixes->romfixes);
	while (roms) {
		romfix *fix = roms->data;

		gtk_tree_store_append (dialog->priv->treestore, &child_iter, &iter);
		gtk_tree_store_set (dialog->priv->treestore, &child_iter,
		                    COL_NAME, fix->romname,
		                    COL_REGION, fix->region,
		                    COL_STATUSTXT, get_romfix_status (fix),
		                    COL_STATUS, fix->status,	/* If status is OK, set to 1, for filtering */
		                    -1);
		
		roms = g_list_next (roms);
	}

	UPDATE_GUI;
	
}

static void
romset_find_fixes (gpointer data, gpointer user_data)
{
	MameRomEntry *romset;
	romset_fixes *fixes;
	gchar *romname;

	romname = (gchar *) data;

	g_return_if_fail (romname != NULL);

	GMAMEUIRomMgrDialog *dialog = (GMAMEUIRomMgrDialog *) user_data;
	
	fixes = NULL;
	
	romset = get_rom_from_gamelist_by_name (gui_prefs.gl, romname);

	//GMAMEUI_DEBUG ("  Looking for fixes for %s", romname);
	
	g_return_val_if_fail (romset != NULL, TRUE);

	/* Check the status for each rom in the romset */
	fixes = mame_rom_entry_find_fixes (romset);

	/* Add to our collected list */
	gmameui_romfix_list_add (gui_prefs.fixes, fixes);

	/* Once romset is returned, update the dialog with the ROMs and their statuses */
	g_return_val_if_fail (fixes != NULL, TRUE);
	
	/* Update the counts */
	dialog->priv->total_romsets++;
	if (fixes->status == 1) dialog->priv->total_ok++;

	/* Update labels *
	gchar *count;
	count = g_strdup_printf ("%s", dialog->priv->total_romsets);
	//gtk_label_set_text (dialog->priv->total_lbl);
	g_free (count);

	gchar *count;
	count = g_strdup_printf ("%s", dialog->priv->total_ok);
	//gtk_label_set_text (dialog->priv->ok_lbl);
	g_free (count);*/
	
	return TRUE;
}

/* Starts the scan of all the ROMs. Should be called in a g_idle_add
   callback to allow the signals to be trapped and displayed by the
   dialog */
static gboolean
start_scan_process (gpointer *data)
{
	GMAMEUIListOutput *parser;
	MameExec *exec;
	GtkWidget *widget;

	GTimer *timer;

	GMAMEUIRomMgrDialog *dialog = (GMAMEUIRomMgrDialog *) data;

	timer = g_timer_new ();
	
	/* Read ROM information for each romset using -listxml */
	parser = gmameui_listoutput_new ();
	exec = mame_exec_list_get_current_executable (main_gui.exec_list);
	gmameui_listoutput_generate_rom_hash (parser, exec);	// Rename this file?
	g_object_unref (parser);

	GMAMEUI_DEBUG ("  Romset rebuild - processed -listxml data in %0.2f seconds", g_timer_elapsed (timer, NULL));

	/* Create hash table for available ROMs in romsets */
	gui_prefs.rom_hashtable = g_hash_table_new (g_str_hash, g_str_equal);

	g_timer_start (timer);
	
	/* Get list of available romset zipfiles */
	get_avail_romsets (dialog);
	GMAMEUI_DEBUG ("  Romset rebuild - got list of all available romsets in %0.2f seconds", g_timer_elapsed (timer, NULL));

	/* Add the contents of all the available romsets to a hash table */
	g_list_foreach (dialog->priv->avail_romsets, (GFunc) add_romset_contents_to_hashtable, NULL);
	GMAMEUI_DEBUG ("  Romset rebuild - added contents of available romsets to hashtable in %0.2f seconds", g_timer_elapsed (timer, NULL));

	g_timer_start (timer);

	g_signal_connect (G_OBJECT (gui_prefs.fixes), "romfix-list-added",
	                  G_CALLBACK (on_romset_fix_found), dialog);
	
	/* Find fix for each romset */
	g_list_foreach (dialog->priv->avail_romsets, (GFunc) romset_find_fixes, dialog);

	GMAMEUI_DEBUG ("  Romset rebuild - finished in %0.2f seconds", g_timer_elapsed (timer, NULL));

	/* Ask user whether they want to have more detailed look for missing ROMs */

	/* Enable fix button */
	widget = GTK_WIDGET (gtk_builder_get_object (dialog->priv->builder, "btn_fix"));
	gtk_widget_set_sensitive (widget, TRUE);
	
	/* Destroy hash table */
	GMAMEUI_DEBUG ("Destroying rom hashtable...");
	g_hash_table_destroy (gui_prefs.rom_hashtable);
	g_hash_table_unref (gui_prefs.rom_hashtable);	/* FIXME TODO Is this needed after destroy? */
	GMAMEUI_DEBUG ("Destroying rom hashtable... done");

	g_timer_destroy (timer);
	
/*
For each zip file in romdir 
	Open zip file
	For each file in romset
		Add available files to hash table
		Look for fixes for rename
			If fix
				Copy zip file to backup dir
				backedup = true
	 			rename
			Else
				Add to notify queue
		Look for fixes to move to parent
			If fix
				Copy zip file to backup dir
				backedup = true
			Else
				Add to notify queue
		Look for fixes to delete
				If fix
				Copy zip file to backup dir
				backedup = true
			Else
				Add to notify queue	
		If not found and not merged rom (i.e. should exist in parent)
			Add romset to 'fixable' GList - GList of items to review from the hash table
		End if
	End for
	Close zipfile
End for
Display results

Then, for deeper scan:
for each item in GList
	Get corresponding zip
	For each item in zip vs expected rom
		If not found
			Look in hash table
			If found
				Add to rom
			End if
		End if
	End for
end for

toutrun has two diff roms with same sha1
samsho2 uses neogeo bios, so reports roms as missing
 */


	return FALSE;
}

/* Triggered whenever the model is told to refilter */
static gboolean
filter_func (GtkTreeModel *model,
             GtkTreeIter  *iter,
             GtkCheckButton *chk)
{
	gint toggled;
	gint romset_status;

	/* Get the gamename, and whether it is filtered by ROM, from the iter */
	gtk_tree_model_get (model, iter, COL_STATUS, &romset_status, -1);

	toggled = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (chk));

	/* Automatically hide romsets that are in the parent */
	if (romset_status == 3)
		return FALSE;
	
	if (toggled) {
		/* If toggled, hide romsets and ROMs that are marked as "OK" */
		return (romset_status != 1);
	} else {
		/* Show all the rows */
		return TRUE;
	}

} 

static void
on_btn_fixes_clicked (GtkWidget *widget, gpointer user_data)
{
	GMAMEUIRomMgrDialog *dialog;

	dialog = (GMAMEUIRomMgrDialog *) user_data;

	GMAMEUI_DEBUG ("Clicked!");
gmameui_romfix_list_process_fixes (gui_prefs.fixes);
	GMAMEUI_DEBUG ("Done!");
}

static void
on_hidecorrectromsets_toggled (GtkWidget *widget, gpointer user_data)
{
	GMAMEUIRomMgrDialog *dialog;

	dialog = (GMAMEUIRomMgrDialog *) user_data;

	/* Good reference for filtering:
	   http://tadeboro.blogspot.com/2009/05/gtktreemodel-and-filtering-4.html */

	gtk_tree_model_filter_refilter (GTK_TREE_MODEL_FILTER (dialog->priv->filter_model));
}

static void
on_row_selected (GtkTreeSelection *selection, gpointer data)
{
	GtkTreeIter iter;
	GtkTreeModel *model;
	gchar *romset;

	/* AAA FIXME TODO Want to add the romfix as a reference to the row? That way we don't need
	   to read the romset name from the tree and then search through the list... */

	if (gtk_tree_selection_get_selected (selection, &model, &iter)) {
		gtk_tree_model_get (model, &iter, 0, &romset, -1);

		//g_return_val_if_fail (rom != NULL, FALSE);

		GMAMEUI_DEBUG ("Selected romset fix reference for %s", romset);

	}
}


/* Boilerplate functions */
static GObject *
gmameui_rommgr_dialog_constructor (GType                  type,
				guint                  n_construct_properties,
				GObjectConstructParam *construct_properties)
{
	GObject          *obj;
	GMAMEUIRomMgrDialog *dialog;

	obj = G_OBJECT_CLASS (gmameui_rommgr_dialog_parent_class)->constructor (type,
									     n_construct_properties,
									     construct_properties);

	dialog = GMAMEUI_ROMMGR_DIALOG (obj);

	return obj;
}

static void
gmameui_rommgr_dialog_class_init (GMAMEUIRomMgrDialogClass *class)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (class);
	GtkObjectClass *gtkobject_class = GTK_OBJECT_CLASS (class);
	GtkDialogClass *gtkdialog_class = GTK_DIALOG_CLASS (class);

	gobject_class->constructor = gmameui_rommgr_dialog_constructor;
/*	gobject_class->get_property = gmameui_rommgr_dialog_get_property;
	gobject_class->set_property = gmameui_rommgr_dialog_set_property;*/

	gtkobject_class->destroy = gmameui_rommgr_dialog_destroy;
	gtkdialog_class->response = gmameui_rommgr_dialog_response;


	g_type_class_add_private (class,
				  sizeof (GMAMEUIRomMgrDialogPrivate));

	/* Signals and properties go here */

}

static void
gmameui_rommgr_dialog_init (GMAMEUIRomMgrDialog *dialog)
{
	GMAMEUIRomMgrDialogPrivate *priv;

	GtkTreeSelection *select;	/* Handle rows in the treeview being selected */
	GtkWidget *rommgr_vbox;
	GtkWidget *chk_hidecorrectromsets;
	GtkWidget *widget;

	GError *error = NULL;
	
	const gchar *object_names[] = {
		"vbox1",
		"romset_treeview",
		"roms_listview",
		"treestore1",
		"liststore1",
		"chk_hideok",
		NULL
	};

	priv = G_TYPE_INSTANCE_GET_PRIVATE (dialog,
					    GMAMEUI_TYPE_ROMMGR_DIALOG,
					    GMAMEUIRomMgrDialogPrivate);

	dialog->priv = priv;
	
	/* Initialise private variables */

	
	/* Build the UI and connect signals here */
	priv->builder = gtk_builder_new ();
	gtk_builder_set_translation_domain (priv->builder, GETTEXT_PACKAGE);

	if (!gtk_builder_add_objects_from_file (priv->builder,
	                                        GLADEDIR "romset_mgr.builder",
	                                        (gchar **) object_names,
	                                        &error)) {
		g_warning ("Couldn't load builder file: %s", error->message);
		g_error_free (error);
		return;
	}

	/* Get the dialog contents */
	rommgr_vbox = GTK_WIDGET (gtk_builder_get_object (priv->builder, "vbox1"));

	/* Add our dialog contents to the vbox of the dialog class */
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox),
			    rommgr_vbox, TRUE, TRUE, 0);
		
	gtk_widget_show_all (GTK_WIDGET (rommgr_vbox));
	
	gtk_dialog_set_has_separator (GTK_DIALOG (dialog), FALSE);

	gtk_dialog_add_button (GTK_DIALOG (dialog), GTK_STOCK_CLOSE, GTK_RESPONSE_CLOSE);

	/* Set up the stores */
	priv->treestore = gtk_builder_get_object (priv->builder, "treestore1");
	priv->liststore = gtk_builder_get_object (priv->builder, "liststore1");
	priv->tv = gtk_builder_get_object (priv->builder, "romset_treeview");
	/*priv->lv = gtk_builder_get_object (priv->builder, "roms_listview");
	gtk_tree_view_set_model (priv->tv, priv->treestore);
	gtk_tree_view_set_model (priv->lv, priv->liststore);*/

	/* Checkbox hiding rows that are correct */
	chk_hidecorrectromsets = GTK_WIDGET (gtk_builder_get_object (priv->builder, "chk_hideok"));
	g_signal_connect (G_OBJECT (chk_hidecorrectromsets), "toggled",
	                  G_CALLBACK (on_hidecorrectromsets_toggled), dialog);
	
	/* Wrap the model up in a filter model to allow the checkbox to filter rows */ 
	priv->filter_model = gtk_tree_model_filter_new (GTK_TREE_MODEL (priv->treestore), NULL);
	gtk_tree_model_filter_set_visible_func (GTK_TREE_MODEL_FILTER (priv->filter_model),
						(GtkTreeModelFilterVisibleFunc) filter_func,
						chk_hidecorrectromsets,
						NULL);

	/* Replace the current model (set in the GtkBuilder .ui file) with the
	   filter model (containing the base model) */ 
	gtk_tree_view_set_model (priv->tv, priv->filter_model);

	/* Add a selection handler for when an item in the tree is clicked */
	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (priv->tv));
	gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
	g_signal_connect (G_OBJECT (select), "changed",
	                  G_CALLBACK (on_row_selected), NULL);
	
	dialog->priv->avail_romsets = NULL;

	widget = GTK_WIDGET (gtk_builder_get_object (priv->builder, "btn_fix"));
	g_signal_connect (G_OBJECT (widget), "clicked",
	                  G_CALLBACK (on_btn_fixes_clicked), dialog);
	gtk_widget_set_sensitive (widget, FALSE);
	
	/* Initialise the counts */
	dialog->priv->total_romsets = 0;
	dialog->priv->total_ok = 0;
	
	g_idle_add (start_scan_process, dialog);
}

GtkWidget *
gmameui_rommgr_dialog_new (GtkWindow *parent)
{
	GtkWidget *dialog;

	dialog = g_object_new (GMAMEUI_TYPE_ROMMGR_DIALOG,
			       "title", _("Romset Manager"),
			       NULL);

	if (parent)
		gtk_window_set_transient_for (GTK_WINDOW (dialog), parent);

	return dialog;

}

static void
gmameui_rommgr_dialog_response (GtkDialog *dialog, gint response)
{
	GMAMEUIRomMgrDialogPrivate *priv;
	
	priv = G_TYPE_INSTANCE_GET_PRIVATE (GMAMEUI_ROMMGR_DIALOG (dialog),
					    GMAMEUI_TYPE_ROMMGR_DIALOG,
					    GMAMEUIRomMgrDialogPrivate);
	
	
	switch (response)
	{
		case GTK_RESPONSE_CLOSE:
			/* Close button clicked */
			gtk_widget_destroy (GTK_WIDGET (dialog));
			
			break;
		case GTK_RESPONSE_DELETE_EVENT:
			/* Dialog closed */
			
			gtk_widget_destroy (GTK_WIDGET (dialog));

			break;
		default:
			g_assert_not_reached ();
	}
}

static void
gmameui_rommgr_dialog_destroy (GtkObject *object)
{
	GMAMEUIRomMgrDialog *dlg;
	
GMAMEUI_DEBUG ("Destroying gmameui romset mgr dialog...");	
	dlg = GMAMEUI_ROMMGR_DIALOG (object);
	
	if (dlg->priv->builder)
		g_object_unref (dlg->priv->builder);

	/* Unref the list of avail roms */
	g_list_foreach (dlg->priv->avail_romsets, (GFunc) g_free, NULL);
	g_list_free (dlg->priv->avail_romsets);
	dlg->priv->avail_romsets = NULL;
	
	g_object_unref (dlg->priv);
	
/*	GTK_OBJECT_CLASS (gmameui_rommgr_dialog_parent_class)->destroy (object);*/
GMAMEUI_DEBUG ("Destroying gmameui romset mgr dialog... done");
}
