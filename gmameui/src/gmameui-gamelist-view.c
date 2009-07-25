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

#include <gdk/gdkkeysyms.h>
#include <string.h>

#include "gmameui-gamelist-view.h"
#include "game_list.h"
#include "gmameui.h"    /* For game_filtered */
#include "rom_entry.h"
#include "gui.h"	/* For main_gui struct */
#include "io.h"         /* For gmameui_gamelist_rebuild */
#include "gui_prefs.h"
#include "audit.h"

static const int ROM_ICON_SIZE = 24;

struct _MameGamelistViewPrivate {
	GtkTreeModel *liststore;	/* The representation of the model in List mode */
#ifdef TREESTORE
	GtkTreeModel *treestore;	/* The representation of the model in Tree mode */
#endif
	GtkTreeModel *curr_model;       /* A pointer to either the base liststore or treestore */

	GtkTreeModel *sort_model;       /* Implementation of GtkTreeModelSort */
	GtkTreeModel *filter_model;     /* Implementation of GtkTreeModelFilter */
#ifdef TREESTORE	
	gboolean is_tree_mode;
#endif
};

G_DEFINE_TYPE (MameGamelistView, mame_gamelist_view, GTK_TYPE_TREE_VIEW)

#define MAME_GAMELIST_VIEW_GET_PRIVATE(o)  (MAME_GAMELIST_VIEW (o)->priv)

enum {
	LISTSTORE,
	TREESTORE
};

/* Function prototypes */
static void
mame_gamelist_view_finalize              (GObject *object);
static const char *
column_title (int column_num);

static void
on_row_selected (GtkTreeSelection *selection,
		 gpointer          data);

static void
set_list_sortable_column     (MameGamelistView *gamelist_view);
static void
create_tree_model            (MameGamelistView *gamelist_view);
static void
populate_model_from_gamelist (MameGamelistView *gamelist_view, GtkTreeModel *model);
static gboolean
filter_func                  (GtkTreeModel *model,
			      GtkTreeIter  *iter,
			      GtkEntry     *entry );

/* Callbacks */
static gboolean
on_list_keypress                      (GtkWidget *widget, GdkEventKey *event, gpointer user_data); 
static gboolean
on_list_clicked                       (GtkWidget *widget, GdkEventButton *event, gpointer user_data);
static void
on_displayed_list_sort_column_changed (GtkTreeSortable *treesortable,
				       gpointer user_data);
#ifdef TREESTORE
static void
on_displayed_list_row_collapsed       (GtkTreeView *treeview,
				       GtkTreeIter *arg1,
				       GtkTreePath *arg2,
				       gpointer user_data);
#endif
static gboolean
on_column_click                       (GtkWidget *button,
				       GdkEventButton *event,
				       GtkTreeViewColumn* column);
static void
on_displayed_list_resize_column       (GtkWidget *widget,
				       GtkRequisition *requisition,
				       gpointer user_data);
static gboolean
foreach_find_random_rom_in_store      (GtkTreeModel *model,
				       GtkTreePath  *path,
				       GtkTreeIter  *iter,
				       gpointer      user_data);
static gboolean
foreach_find_rom_in_store             (GtkTreeModel *model,
				       GtkTreePath  *path,
				       GtkTreeIter  *iter,
				       gpointer      user_data);

static gboolean
foreach_update_prefix_in_store (GtkTreeModel *model,
				GtkTreePath  *path,
				GtkTreeIter  *iter,
				gpointer      user_data);

static gboolean
foreach_update_filter (GtkTreeModel *model,
		       GtkTreePath  *path,
		       GtkTreeIter  *iter,
		       gpointer      user_data);

/* Callbacks handling when the preferences change */
static void
on_prefs_column_shown_toggled (MameGuiPrefs *prefs, gpointer va, gpointer user_data);
static void
on_prefs_theprefix_toggled    (MameGuiPrefs *prefs, gboolean theprefix, gpointer user_data);
static void
on_prefs_prefercustomicons_toggled  (MameGuiPrefs *prefs, gboolean customicons, gpointer user_data);

/* Callbacks handling when the Search Entry text changes */
static void
on_search_changed             (GtkEntry *entry,
			       gchar *criteria,
			       gpointer user_data);

/* Callbacks handling when a ROM is audited */
static void
on_romset_audited             (GmameuiAudit *audit,
			       gchar *audit_line,
			       gint type,
			       gint auditresult,
			       gpointer user_data);

/* Boilerplate functions */
static GObject *
mame_gamelist_view_constructor (GType                  type,
				guint                  n_construct_properties,
				GObjectConstructParam *construct_properties)
{
	GObject          *obj;
	MameGamelistView *gamelist_view;

	obj = G_OBJECT_CLASS (mame_gamelist_view_parent_class)->constructor (type,
									     n_construct_properties,
									     construct_properties);

	gamelist_view = MAME_GAMELIST_VIEW (obj);

	return obj;
}

static void
mame_gamelist_view_class_init (MameGamelistViewClass *class)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (class);
/*	GtkObjectClass *gtkobject_class = GTK_OBJECT_CLASS (class);
	GtkTreeViewClass *gtktreeview_class = GTK_TREE_VIEW_CLASS (class);*/

	gobject_class->constructor = mame_gamelist_view_constructor;
/*	gobject_class->get_property = mame_gamelist_view_get_property;
	gobject_class->set_property = mame_gamelist_view_set_property;*/

	gobject_class->finalize = mame_gamelist_view_finalize;

	g_type_class_add_private (class,
				  sizeof (MameGamelistViewPrivate));

	/* Signals and properties go here */
	
}

static void
mame_gamelist_view_init (MameGamelistView *gamelist_view)
{
	MameGamelistViewPrivate *priv;


	priv = G_TYPE_INSTANCE_GET_PRIVATE (gamelist_view,
					    MAME_TYPE_GAMELIST_VIEW,
					    MameGamelistViewPrivate);

	gamelist_view->priv = priv;
	
	/* Initialise private variables */
#ifdef TREESTORE
	priv->treestore = (GtkTreeModel *) gtk_tree_store_new (NUMBER_COLUMN + 4,
							       G_TYPE_STRING,     /* Name */
							       G_TYPE_STRING,     /* Has samples */
							       G_TYPE_STRING,     /* ROM name */
							       G_TYPE_INT,	  /* Times played */
							       G_TYPE_STRING,     /* Manu */
							       G_TYPE_STRING,     /* Year */
							       G_TYPE_STRING,     /* Clone of */
							       G_TYPE_STRING,     /* Driver */
							       G_TYPE_STRING,     /* Ver added */
							       G_TYPE_STRING,     /* Category */
							       G_TYPE_POINTER,    /* Rom Entry */
							       PANGO_TYPE_STYLE,
							       G_TYPE_BOOLEAN,
							       GDK_TYPE_PIXBUF);  /* Pixbuf */
#endif
	priv->liststore = (GtkTreeModel *) gtk_list_store_new (NUMBER_COLUMN + 4,
							       G_TYPE_STRING,     /* Name */
							       G_TYPE_STRING,     /* Has samples */
							       G_TYPE_STRING,     /* ROM name */
							       G_TYPE_INT,	  /* Times played */
							       G_TYPE_STRING,     /* Manu */
							       G_TYPE_STRING,     /* Year */
							       G_TYPE_STRING,     /* Clone of */
							       G_TYPE_STRING,     /* Driver */
							       G_TYPE_STRING,     /* Ver added */
							       G_TYPE_STRING,     /* Category */
							       G_TYPE_POINTER,    /* Rom Entry */
							       PANGO_TYPE_STYLE,
							       G_TYPE_BOOLEAN,
							       GDK_TYPE_PIXBUF);  /* Pixbuf */
	
	/* Build the UI and connect signals here */

	gint i;
	GtkTreeViewColumn *column;
	GtkCellRenderer *renderer;
	GtkTreeSelection *select;

	GMAMEUI_DEBUG ("Creating gamelist structure");

	ListMode list_mode;
	GValueArray *va_shown = NULL;
	GValueArray *va_width = NULL;

	g_object_get (main_gui.gui_prefs,
		      "current-mode", &list_mode,
      		      "cols-shown", &va_shown,
		      "cols-width", &va_width,
		      NULL);
	
	for (i = 0; i < NUMBER_COLUMN; i++) {
		if (i == GAMENAME) {
			/* Game name column contains both the status icon and the name */
			column = gtk_tree_view_column_new ();
			gtk_tree_view_column_set_title  (GTK_TREE_VIEW_COLUMN (column), column_title (i));
			gtk_tree_view_column_set_sort_column_id (column, i);

			/* Pixbuf */
			renderer = gtk_cell_renderer_pixbuf_new ();
			gtk_tree_view_column_pack_start (GTK_TREE_VIEW_COLUMN (column),
						       renderer,
						       FALSE);
			gtk_tree_view_column_add_attribute (GTK_TREE_VIEW_COLUMN (column), renderer,
							    "pixbuf", PIXBUF);
			g_object_set (renderer,
				      "xalign", 1.0,
				      "ypad", 0,
				      "height", ROM_ICON_SIZE,  /* All rows should share same height */
				      NULL);
				
			/* TEXT */
			renderer = gtk_cell_renderer_text_new ();
			gtk_tree_view_column_pack_end (GTK_TREE_VIEW_COLUMN (column),
						       renderer,
						       TRUE);

			/* The text is set to the GAMENAME column; the TEXTSTYLE column
			   contains a PangoStyle for italic if the ROM is a clone */
			gtk_tree_view_column_set_attributes (GTK_TREE_VIEW_COLUMN (column), renderer,
							     "text", i,
							     "style", TEXTSTYLE,
							     NULL);
			g_object_set (renderer,
				      "ypad", 0, "yalign", 0.5,
				      "ellipsize", PANGO_ELLIPSIZE_END,
				      NULL);

		} else {
			renderer = gtk_cell_renderer_text_new ();
			column = gtk_tree_view_column_new_with_attributes (column_title (i), renderer,
									   "text", i,
									   NULL);
			g_object_set (renderer, "ellipsize", PANGO_ELLIPSIZE_END, NULL);
			gtk_tree_view_column_set_sort_column_id (column, i);
		}
		gtk_tree_view_append_column (GTK_TREE_VIEW (gamelist_view), column);
			
		/* Set minimum width to the num of chars in the header TODO Also get sort directory size */
		gint font_size, title_len;
		title_len = g_utf8_strlen (gtk_tree_view_column_get_title (GTK_TREE_VIEW_COLUMN (column)), -1);
		font_size = PANGO_PIXELS (pango_font_description_get_size (GTK_WIDGET (gamelist_view)->style->font_desc));

		gtk_tree_view_column_set_min_width (GTK_TREE_VIEW_COLUMN (column),
						    title_len * font_size);

		
		/* Determine whether the columns is to be shown, and its width */
		if ((list_mode == DETAILS)
#ifdef TREESTORE
		    || (list_mode == DETAILS_TREE)
#endif
		    ) {
			/* If we are in Details mode, show column if selected in Preferences */
			if (g_value_get_int (g_value_array_get_nth (va_shown, i)) == FALSE) {
				gtk_tree_view_column_set_visible (column, FALSE);
			} else {
				gint col_width;
				
				col_width = g_value_get_int (g_value_array_get_nth (va_width, i));
				gtk_tree_view_column_set_visible (column, TRUE);
				if (col_width == 0) {
					gtk_tree_view_column_set_sizing (column, GTK_TREE_VIEW_COLUMN_AUTOSIZE);
				} else {
					gtk_tree_view_column_set_sizing (column, GTK_TREE_VIEW_COLUMN_FIXED);
					gtk_tree_view_column_set_fixed_width (column, col_width);
				}
				gtk_tree_view_column_set_resizable (column, TRUE);
			}
		} else {
			/* When we are not in Details mode, hide all columns except the GAMENAME */
			if (i == GAMENAME) {
				gtk_tree_view_column_set_visible (column, TRUE);
				gtk_tree_view_column_set_sizing (column, GTK_TREE_VIEW_COLUMN_AUTOSIZE);
				gtk_tree_view_column_set_resizable (column, FALSE);
			} else {
				gtk_tree_view_column_set_visible (column, FALSE);
			}
		}

		/* Reordering columns is disabled because it conflicts with
		  'column popup menu' and 'sort on click header' */
		gtk_tree_view_column_set_reorderable (GTK_TREE_VIEW_COLUMN (column), FALSE);		
		
		/* Callback - column header right-clicked */
		g_signal_connect (column->button, "event",
				  G_CALLBACK (on_column_click), column);
	}
	
	/* Create the model - either a GtkTreeStore or a GtkListStore */
	create_tree_model (gamelist_view);
	
	/* Callback - Row has been selected */
	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (gamelist_view));
	gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
	g_signal_connect (G_OBJECT (select), "changed",
			  G_CALLBACK (on_row_selected), NULL);

	/* Callback - Click on the list */
	g_signal_connect (G_OBJECT (gamelist_view), "button-press-event",
			  G_CALLBACK (on_list_clicked), NULL);
	/* Callback - Keypress on the list */
	g_signal_connect (G_OBJECT (gamelist_view), "key-press-event",
			  G_CALLBACK (on_list_keypress), NULL);
	/* Callback - Column size modified */
	g_signal_connect (G_OBJECT (gamelist_view), "size-request",
			  G_CALLBACK (on_displayed_list_resize_column), NULL);
#ifdef TREESTORE
	/* Callback - Row has been collapsed */
	g_signal_connect (G_OBJECT (gamelist_view), "row-collapsed",
			  G_CALLBACK (on_displayed_list_row_collapsed), NULL);
#endif
	/* Callback handling on MameGuiPrefs changes */
	g_signal_connect (G_OBJECT (main_gui.gui_prefs), "col-toggled",
			  G_CALLBACK (on_prefs_column_shown_toggled), gamelist_view);
	g_signal_connect (G_OBJECT (main_gui.gui_prefs), "prefercustomicons-toggled",
			  G_CALLBACK (on_prefs_prefercustomicons_toggled), gamelist_view);
	g_signal_connect (G_OBJECT (main_gui.gui_prefs), "theprefix-toggled",
			  G_CALLBACK (on_prefs_theprefix_toggled), gamelist_view);

	/* Listen for the search criteria being changed */		
	g_signal_connect (main_gui.search_entry, "search-changed",
			  G_CALLBACK (on_search_changed), NULL);
	
	dirty_icon_cache = FALSE;
	
	GMAMEUI_DEBUG ("Creating gamelist structure... done");

}

MameGamelistView *
mame_gamelist_view_new (void)
{
	MameGamelistView *gamelist_view;

	gamelist_view = MAME_GAMELIST_VIEW (g_object_new (MAME_TYPE_GAMELIST_VIEW, NULL));
	
	g_object_set (gamelist_view,
		      "rules-hint", TRUE,       /* Set the alternate colours for each row */
		      NULL);

	return gamelist_view;

}

static void
mame_gamelist_view_finalize (GObject *object)
{
	MameGamelistView *gamelist_view;
	
GMAMEUI_DEBUG ("Destroying mame gamelist view...");	
	gamelist_view = MAME_GAMELIST_VIEW (object);
	
	/* Clear the model */
	gtk_list_store_clear (GTK_LIST_STORE (gamelist_view->priv->curr_model));
	
	/* FIXME TODO Need to destroy the models */
		
	g_object_unref (gamelist_view->priv);
	
	G_OBJECT_CLASS (mame_gamelist_view_parent_class)->finalize (object);

/*	GTK_OBJECT_CLASS (mame_gamelist_view_parent_class)->destroy (object);*/
	
GMAMEUI_DEBUG ("Destroying mame gamelist view... done");
}
/* End boilerplate functions */


static guint timeoutid;
static gint ColumnHide_selected;

static const
char *
column_title (int column_num)
{
	switch (column_num) {
	case GAMENAME:
		return _("Game");
	case HAS_ROMS:
		return _("ROMs");
	case HAS_SAMPLES:
		return _("Samples");
	case ROMNAME:
		return _("Directory");
	case TIMESPLAYED:
		return _("Played");
	case MANU:
		return _("Manufacturer");
	case YEAR:
		return _("Year");
	case CLONE:
		return _("Clone of");
	case DRIVER:
		return _("Driver");
	case ROMOF:
		return _("Rom of");

	/*  Working / Not Working or Good / Imperfect / Preliminary */
	case DRIVER_STATUS:
		return _("Driver Status");
	case COLOR_STATUS:
		return _("Driver Colors");
	case SOUND_STATUS:
		return _("Driver Sound");
	case GRAPHIC_STATUS:
		return _("Driver Graphics");
	case NUMPLAYERS:
		return _("Players");
	case NUMBUTTONS:
		return _("Buttons");
	case MAMEVER:
		return _("Version");
	case CATEGORY:
		return _("Category");
	case FAVORITE:
		return _("Favorite");
	case CHANNELS:
		return _("Channels");
	case IS_BIOS:
		return _("BIOS");
	default:
		return NULL;
	}
}

#ifdef TREESTORE
/* This function switches between the GtkTreeStore and GtkListStore by
   re-creating the GtkTreeModel */
void
mame_gamelist_view_change_model (MameGamelistView *gamelist_view)
{
	g_return_if_fail (gamelist_view != NULL);

	create_tree_model (gamelist_view);

}
#endif

/* Called when switching views from the menu */
void
mame_gamelist_view_change_views (MameGamelistView *gamelist_view)
{
	gint curr_mode;
	gint prev_mode;
	int i;
	
	/* Get the mode and previous mode */
	g_object_get (main_gui.gui_prefs,
		      "current-mode", &curr_mode,
		      "previous-mode", &prev_mode,
		      NULL);
	
	if (curr_mode == DETAILS) {
		GValueArray *va_shown = NULL;
		
		/* Get preferences for which columns are shown */
		g_object_get (main_gui.gui_prefs, "cols-shown", &va_shown, NULL);
		
		/* If we are switching to a details mode, then need to show all columns
		   that the preferences are set to show */
		for (i = 1; i < NUMBER_COLUMN; i++) {
			if (g_value_get_int (g_value_array_get_nth (va_shown, i)) == TRUE) {
				
				gtk_tree_view_column_set_visible (gtk_tree_view_get_column (GTK_TREE_VIEW (gamelist_view), i),
							  TRUE);
			}
		}
		
		/* Cleanup */
		if (va_shown) {
			g_value_array_free (va_shown);
			va_shown = NULL;
		}

	} else {
		/* If we are switching to a list mode, need to hide the columns */
		for (i = 1; i < NUMBER_COLUMN; i++)
			gtk_tree_view_column_set_visible (gtk_tree_view_get_column (GTK_TREE_VIEW (gamelist_view), i),
							  FALSE);
	}
	
#ifdef TREESTORE		
	/* Changing from list to details and vice versa, now we need to change
	   the model, too */
#endif
}

void
mame_gamelist_view_select_random_game (MameGamelistView *gamelist_view, gint i)
{
	GtkTreeModel *model;
	model = gtk_tree_view_get_model (GTK_TREE_VIEW (gamelist_view));
	
	g_return_if_fail (model != NULL);
	
	gtk_tree_model_foreach (GTK_TREE_MODEL (model),
				foreach_find_random_rom_in_store,
				(gpointer *) i);
}

/* FIXME TODO Merge with the other code that performs a gtk_tree_store_set */
void
mame_gamelist_view_update_game_in_list (MameGamelistView *gamelist_view, MameRomEntry *tmprom)
{
	const gchar *my_hassamples;
	GdkPixbuf *pixbuf;
#ifdef TREESTORE
	gboolean is_tree_store;
#endif
	ListMode current_mode;
	gint rom_filter_opt;
	PangoStyle pangostyle;
	const gchar *name_in_list;
	GtkTreeIter iter;
	gchar *iconzipfile;
	gchar *icondir;
	gboolean prefercustomicons;
	
	g_return_if_fail (tmprom != NULL);

	g_object_get (main_gui.gui_prefs,
		      "current-mode", &current_mode,
		      "current-rom-filter", &rom_filter_opt,	      
		      "dir-icons", &icondir,
		      "prefercustomicons", &prefercustomicons,
		      NULL);
	
	iconzipfile = g_build_filename (icondir, "icons.zip", NULL);
	
#ifdef TREESTORE	
	/* Whether the Tree Model will a tree or a list */
	is_tree_store = (current_mode == LIST_TREE) || (current_mode == DETAILS_TREE);
#endif
	name_in_list = mame_rom_entry_get_list_name (tmprom);

	/* Has Samples */
	if (!mame_rom_entry_has_samples (tmprom))
		my_hassamples = "";
	else {
		my_hassamples = (mame_rom_entry_get_sample_status (tmprom) == CORRECT) ? _("Yes") : _("No");
	}

	/* Mark if clone */
	if (mame_rom_entry_is_clone (tmprom))
		pangostyle = PANGO_STYLE_ITALIC;
	else
		pangostyle = PANGO_STYLE_NORMAL;

	gchar *romname;
	gint timesplayed;
	gchar *manu;
	gchar *year;
	gchar *cloneof;
	gchar *driver, *category, *version;

	g_object_get (tmprom,
		      "romname", &romname,
		      "times-played", &timesplayed,
		      "manufacturer", &manu,
		      "year", &year,
		      "cloneof", &cloneof,
		      "driver", &driver,
		      "category", &category,
		      "version-added", &version,
		      NULL);
	
	/* Set the pixbuf for the status icon */
	pixbuf = get_icon_for_rom (tmprom, ROM_ICON_SIZE, icondir, iconzipfile, prefercustomicons);
	
	iter = mame_rom_entry_get_position (tmprom);

#ifdef TREESTORE
	if (is_tree_store) {
		gtk_tree_store_set (GTK_TREE_STORE (gamelist_view->priv->curr_model),
				    &iter,
				    GAMENAME,     name_in_list,
				    HAS_SAMPLES,  my_hassamples,
				    ROMNAME,      romname,
				    TIMESPLAYED,  timesplayed,
				    MANU,         manu,
				    YEAR,         year,
				    CLONE,        cloneof,
				    DRIVER,       driver,
				    MAMEVER,      version,
				    CATEGORY,     category,
				    TEXTSTYLE,    pangostyle,
				    FILTERED,     game_filtered (tmprom, rom_filter_opt),
				    PIXBUF,       pixbuf,
				    -1);
	} else {
#endif
		gtk_list_store_set (GTK_LIST_STORE (gamelist_view->priv->curr_model),
				    &iter,
				    GAMENAME,     name_in_list,
				    HAS_SAMPLES,  my_hassamples,
				    ROMNAME,      romname,
				    TIMESPLAYED,  timesplayed,
				    MANU,         manu,
				    YEAR,         year,
				    CLONE,        cloneof,
				    DRIVER,       driver,
				    MAMEVER,      version,
				    CATEGORY,     category,
				    TEXTSTYLE,    pangostyle,
				    FILTERED,     game_filtered (tmprom, rom_filter_opt),
				    PIXBUF,       pixbuf,
				    -1);
#ifdef TREESTORE
	}
#endif

	g_free (romname);
	g_free (manu);
	g_free (year);
	g_free (cloneof);
	g_free (driver);
	g_free (category);
	g_free (version);
	
	g_free (iconzipfile);
	g_free (icondir);

}

static void
set_status_bar_game_count (MameGamelistView *gamelist_view)
{
	gchar *message;

	/* Count the number of rows displayed in the filter model (i.e. number
	   displayed after filtering is applied, not the total).
	   FIXME TODO Note we use visible_games here since it is used in so many
	   other places to determine whether to perform specific actions. */
	visible_games = gtk_tree_model_iter_n_children (GTK_TREE_MODEL (gamelist_view->priv->filter_model), NULL);
		
	message = g_strdup_printf ("%d %s", visible_games, visible_games == 1 ? _("game") : _("games"));
	
	gtk_statusbar_pop (main_gui.statusbar, 1);
	gtk_statusbar_push (main_gui.statusbar, 1, message);
	
	g_free (message);
}

static void
create_tree_model (MameGamelistView *gamelist_view)
{
	ListMode current_mode;
	
	g_return_if_fail (gui_prefs.gl != NULL);

GTimer *timer = g_timer_new ();
g_timer_start (timer);
	
	g_object_get (main_gui.gui_prefs, "current-mode", &current_mode, NULL);
	GMAMEUI_DEBUG ("  Creating the MameGameListView Model in mode %d", current_mode);
	
	/* Status Bar Message */
	set_status_bar (_("Wait..."), NULL);
	
	/* Unset the tree model until it has been filled */
	if ((gamelist_view) && (gamelist_view->priv->curr_model)) {
		gtk_tree_view_set_model (GTK_TREE_VIEW (gamelist_view), NULL);
	}
#ifdef TREESTORE
	/* Whether the Tree Model will a tree or a list */
	if ((current_mode == LIST_TREE) || (current_mode == DETAILS_TREE)) {
		gamelist_view->priv->is_tree_mode = TRUE;
		gamelist_view->priv->curr_model = gamelist_view->priv->treestore;		
	} else {
		gamelist_view->priv->is_tree_mode = FALSE;
#endif
		gamelist_view->priv->curr_model = gamelist_view->priv->liststore;

#ifdef TREESTORE
	}
#endif
	
	/* Get the status icon */
	get_status_icons ();
	
	/* Add a filter, and link the model to a filter function so that
	   calls to gtk_tree_model_filter_refilter cause the model to be
	   filtered from the MameSearchEntry field
	 
	   In order to perform both filtering and sorting, the model structure is set as:
	      Sortable
	      +- Filter
	         +- Store
	   where Store is the structure of the Gamelist
	 */
	gamelist_view->priv->filter_model = gtk_tree_model_filter_new (gamelist_view->priv->curr_model, NULL);
	gtk_tree_model_filter_set_visible_func (GTK_TREE_MODEL_FILTER (gamelist_view->priv->filter_model),
						(GtkTreeModelFilterVisibleFunc) filter_func,
						main_gui.search_entry,
						NULL);
	gamelist_view->priv->sort_model = gtk_tree_model_sort_new_with_model (GTK_TREE_MODEL (gamelist_view->priv->filter_model));
	gtk_tree_view_set_model (GTK_TREE_VIEW (gamelist_view), GTK_TREE_MODEL (gamelist_view->priv->sort_model));
	
	/* Populate the model here, since we want the sort/filter to apply, triggering
	   the ROM icon loading */
	populate_model_from_gamelist (gamelist_view, gamelist_view->priv->curr_model);
	 
	/* Sort the list */
	set_list_sortable_column (gamelist_view);

	/* Callback when the sort column/order changes */
	g_signal_connect (G_OBJECT (gamelist_view->priv->sort_model), "sort-column-changed",
			  G_CALLBACK (on_displayed_list_sort_column_changed), NULL);

	/* The column headers are clickable in Details view only */
	gtk_tree_view_set_headers_clickable (GTK_TREE_VIEW (gamelist_view),
					     (current_mode == DETAILS)
#ifdef TREESTORE
					     ||
					     (current_mode == DETAILS_TREE)
#endif
					     );

	GMAMEUI_DEBUG ("Time taken to create_gamelist_content is %.2f", g_timer_elapsed (timer, NULL));
	g_timer_destroy (timer);
	
	GMAMEUI_DEBUG ("  Creating the MameGameListView Model... done");
	
}

static void
set_list_sortable_column (MameGamelistView *gamelist_view)
{
	GtkTreeModel *model;
	ListMode current_mode;
	gint sort_col;
	gint sort_col_dir;

	/* The model implemented by the MameGamelistView is the GtkTreeSortable */
	model = gtk_tree_view_get_model (GTK_TREE_VIEW (gamelist_view));

	g_return_if_fail (model != NULL);
	
	g_object_get (main_gui.gui_prefs,
		      "current-mode", &current_mode,
		      "sort-col", &sort_col,    /* FIXME TODO Make priv in this object */
		      "sort-col-direction", &sort_col_dir,/* FIXME TODO Make priv in this object */
		      NULL);
	
	if ((current_mode == DETAILS)
#ifdef TREESTORE
	    || (current_mode == DETAILS_TREE)
#endif
	    ) {
		GMAMEUI_DEBUG("Sorting - using sort order %d", sort_col);
		gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE (model),
						      sort_col, sort_col_dir);

	} else {
		g_signal_handlers_block_by_func (G_OBJECT (model),
						 (gpointer) on_displayed_list_sort_column_changed,
						 NULL);
		gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE (model),
						      GAMENAME,
						      GTK_SORT_ASCENDING);
		g_signal_handlers_unblock_by_func (G_OBJECT (model),
						   (gpointer) on_displayed_list_sort_column_changed,
						   NULL);
	}	/* Select the correct row */
}

/*
 foreach_find_random_rom_in_store
   user_data contains a random number representing the target row; we
   continue to move through the store until we reach the target row,
   expanding the parent row if necessary
 */
static gboolean
foreach_find_random_rom_in_store (GtkTreeModel *model,
				  GtkTreePath  *path,
				  GtkTreeIter  *iter,
				  gpointer      user_data)
{
	static gint current_row;
	gint target_row;
	gboolean return_val;
	
	return_val = FALSE;		/* Do not stop walking the store, call us with next row */
	
	target_row = (gint) user_data;
	
	if (current_row == target_row) {
		/* Found the random row we are after */
		/* Scroll to selection */
		gtk_tree_view_scroll_to_cell (GTK_TREE_VIEW (main_gui.displayed_list),
					      path, NULL, TRUE, 0.5, 0);
#ifdef TREESTORE
		/* Need to expand the parent row (if a child) */
		gtk_tree_view_expand_to_path (GTK_TREE_VIEW (main_gui.displayed_list),
					      path);
#endif
		/* And highlight the row */
		gtk_tree_view_set_cursor (GTK_TREE_VIEW (main_gui.displayed_list),
					  path, NULL, FALSE);
		
		return_val = TRUE;      /* Stop walking the store */
		current_row = 0;	/* Reset for next time we want to find a random row */
	}
	
	current_row++;
	
	return return_val;   
}

/* This function walks the tree store containing the ROM data until
   it finds the row that has the ROM from the preferences. It then
   scrolls to that row, and opens the parent row if the found
   row is a child */
static gboolean
foreach_find_rom_in_store (GtkTreeModel *model,
			   GtkTreePath  *path,
			   GtkTreeIter  *iter,
			   gpointer      user_data)
{
	MameRomEntry *rom;
	
	gchar *current_rom_name = (gchar *) user_data;
	
	/* Don't even bother trying to walk the store if the current game
	   is not set. We are just wasting our time */
	g_return_val_if_fail ((current_rom_name != NULL), TRUE);
	
	gtk_tree_model_get (model, iter, ROMENTRY, &rom, -1);
/*	
	tree_path_str = gtk_tree_path_to_string (path);
	GMAMEUI_DEBUG ("Row %s: name is %s", tree_path_str, rom->romname);
	g_free (tree_path_str);
*/	
	if (g_ascii_strcasecmp (mame_rom_entry_get_romname (rom), current_rom_name) == 0) {
		GMAMEUI_DEBUG ("Found row in tree view - %s", mame_rom_entry_get_romname (rom));

		/* Scroll to selection */
		gtk_tree_view_scroll_to_cell (GTK_TREE_VIEW (main_gui.displayed_list),
					      path, NULL, TRUE, 0.5, 0);
#ifdef TREESTORE
		/* Need to expand the parent row (if a child) */
		gtk_tree_view_expand_to_path (GTK_TREE_VIEW (main_gui.displayed_list),
					      path);
#endif
		/* And highlight the row */
		gtk_tree_view_set_cursor (GTK_TREE_VIEW (main_gui.displayed_list),
					  path, NULL, FALSE);
		
		return TRUE;    /* Found the row we are after, no need to keep walking */
	}
	
	return FALSE;   /* Do not stop walking the store, call us with next row */
}

/* Scroll to, and highlight, the current-rom in the preferences */
void
mame_gamelist_view_scroll_to_selected_game (MameGamelistView *gamelist_view)
{	
	gchar *current_rom_name;
	g_object_get (main_gui.gui_prefs, "current-rom", &current_rom_name, NULL);
	
	/* Find the selected game in the gamelist, and scroll to it, opening any expanders */
	if (visible_games > 0) {
		/* Scroll to the game specified from the preferences */
		gtk_tree_model_foreach (GTK_TREE_MODEL (gamelist_view->priv->sort_model),
					foreach_find_rom_in_store,
					current_rom_name);
	}
	
	g_free (current_rom_name);
}

#ifdef TREESTORE
void
on_collapse_all_activate (GtkMenuItem     *menuitem,
			  gpointer         user_data)
{
	GtkTreeModel *model;
	GtkTreeIter iter;
	GtkTreeSelection *select;
	GtkTreePath *tree_path;

	gtk_tree_view_collapse_all (GTK_TREE_VIEW (main_gui.displayed_list));
	
	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (main_gui.displayed_list));

	if (gtk_tree_selection_get_selected (select, &model, &iter)) {
		tree_path = gtk_tree_model_get_path (GTK_TREE_MODEL (model), &iter);
		gtk_tree_view_scroll_to_cell (GTK_TREE_VIEW (main_gui.displayed_list),
					      tree_path, NULL, TRUE, 0.5, 0);
		gtk_tree_path_free (tree_path);
	}
}

void
on_expand_all_activate (GtkMenuItem *menuitem,
			gpointer     user_data)
{
	GtkTreeModel *model;
	GtkTreeIter iter;
	GtkTreeSelection *select;
	GtkTreePath *tree_path;

	gtk_tree_view_expand_all (GTK_TREE_VIEW (main_gui.displayed_list));
	
	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (main_gui.displayed_list));

	if (gtk_tree_selection_get_selected (select, &model, &iter)) {
		tree_path = gtk_tree_model_get_path (GTK_TREE_MODEL (model), &iter);
		gtk_tree_view_scroll_to_cell (GTK_TREE_VIEW (main_gui.displayed_list),
					      tree_path, NULL, TRUE, 0.5, 0);
		gtk_tree_path_free (tree_path);
	}
}
#endif

static gboolean
delayed_row_selected (GtkTreeSelection *selection)
{
	MameRomEntry *rom = NULL;
	GtkTreeIter iter;
	GtkTreeModel *model;

	if (gtk_tree_selection_get_selected (selection, &model, &iter)) {
		gtk_tree_model_get (model, &iter, ROMENTRY, &rom, -1);

		g_return_val_if_fail (rom != NULL, FALSE);

		g_object_set (main_gui.gui_prefs, "current-rom", mame_rom_entry_get_romname (rom), NULL);

//		select_game (rom);
	}
select_game (rom);
	return FALSE;
}

static void
on_row_selected (GtkTreeSelection *selection,
		 gpointer          data)
{
	if (timeoutid)
		g_source_remove (timeoutid);
	
	timeoutid = g_timeout_add (SELECT_TIMEOUT,
				   (GSourceFunc) delayed_row_selected,
				   selection);
}

static gboolean
on_list_keypress (GtkWidget   *widget,
		  GdkEventKey *event,
		  gpointer    user_data)
{
	g_return_val_if_fail (gui_prefs.current_game != NULL, FALSE);
	
	if (event && event->type == GDK_KEY_PRESS &&
	    (event->keyval == GDK_KP_Enter || event->keyval == GDK_Return))
		play_game (gui_prefs.current_game);
	
	return FALSE;
}

static gboolean
on_list_clicked (GtkWidget      *widget,
		 GdkEventButton *event,
		 gpointer        user_data)
{
	GtkTreeModel *model;
	GtkTreePath *path=NULL;
	GtkTreeIter iter;
	gboolean myclick=FALSE;
	GValue val = { 0, };
	MameRomEntry *game_data;
	gint x,y;

	/* Here we figure out which node was clicked */
	myclick = gtk_tree_view_get_path_at_pos (GTK_TREE_VIEW (widget),
						 event->x, event->y,
						 &path, NULL,
						 &x, &y);
	g_return_val_if_fail (path != NULL, FALSE);

	/* We test here if we clicked between 2 columns
	I know this will make double click on the row 0 col 0 not working
	but the result of gtk_tree_view_get_path_at_pos is TRUE
	even if we don't click on a row ????? (maybe a GTK bug)
	if ( ( (event->x==x) && (event->y==y)) || (myclick==FALSE) ) */
	if (myclick == FALSE) {
		return FALSE;
	}

	model = gtk_tree_view_get_model (GTK_TREE_VIEW (widget));
	
	/* Here we figure out which node was clicked */
	gtk_tree_model_get_iter (GTK_TREE_MODEL (model), &iter, path);
	gtk_tree_model_get_value (GTK_TREE_MODEL (model), &iter, ROMENTRY, &val);
	game_data = g_value_get_pointer (&val);

	if (path)
		gtk_tree_path_free (path);

	if (event) {
		if (event->type == GDK_2BUTTON_PRESS && event->button == 1) {
			play_game (game_data);			/* Double-click */
		} else if (event->type == GDK_BUTTON_PRESS && event->button == 3) {
			gamelist_popupmenu_show (event);	/* Right-click */
		}
	}	

	return FALSE;
}

/* Event - invoked when the mouse is clicked on the column headers */
static gboolean
on_column_click (GtkWidget         *button,
		 GdkEventButton    *event,
		 GtkTreeViewColumn *column)
{

	/* Only right-mouse clicks are catered for */
	if (event->type == GDK_BUTTON_PRESS && ((GdkEventButton *) event)->button == 3) {

		GtkWidget *hide_menu_item;
		GtkWidget *popup_menu;
		gchar *hide_column_menu_name;
		
		popup_menu = gtk_ui_manager_get_widget (main_gui.manager, "/ColumnPopup");
		g_return_val_if_fail (popup_menu != NULL, FALSE);

		hide_menu_item = gtk_ui_manager_get_widget (main_gui.manager,
							    "/ColumnPopup/ColumnHide");
		
		hide_column_menu_name = g_strdup_printf (_("Hide \"%s\" Column"),
							 column_title (gtk_tree_view_column_get_sort_column_id (GTK_TREE_VIEW_COLUMN (column))));
		gtk_label_set_text (GTK_LABEL (GTK_BIN (hide_menu_item)->child),
				    hide_column_menu_name);
		g_free (hide_column_menu_name);

		/* Don't allow user to hide the game name column */
		ColumnHide_selected = gtk_tree_view_column_get_sort_column_id (GTK_TREE_VIEW_COLUMN (column));
		gtk_widget_set_sensitive (GTK_WIDGET (hide_menu_item),
					  !(ColumnHide_selected == GAMENAME));
		
		gtk_menu_popup (GTK_MENU (popup_menu),
				NULL, NULL,
				NULL, NULL,
				event->button, event->time);
	}
	
	return FALSE;
}

/* Go through the model and update the display name of the ROM to reflect
   the current value of theprefix */
static gboolean
foreach_update_prefix_in_store (GtkTreeModel *model,
				GtkTreePath  *path,
				GtkTreeIter  *iter,
				gpointer      user_data)
{
	MameRomEntry *rom;
	
	gtk_tree_model_get (model, iter, ROMENTRY, &rom, -1);

	g_return_val_if_fail (rom != NULL, FALSE);

#ifdef TREESTORE
	if (gtk_tree_model_get_flags (model) & GTK_TREE_MODEL_LIST_ONLY) {
#endif
		gtk_list_store_set (GTK_LIST_STORE (model), iter,
				    GAMENAME, mame_rom_entry_get_list_name (rom),
				    -1);
#ifdef TREESTORE
	} else {
		gtk_tree_store_set (GTK_TREE_STORE (model), iter,
				    GAMENAME, mame_rom_entry_get_list_name (rom),
				    -1);
	}
#endif
	return FALSE;   /* Do not stop walking the store, call us with next row */
}

/* Handler for when the columns in the GuiPrefs object are changed (usually via
   the GUI Preferences dialog */
static void
on_prefs_column_shown_toggled (MameGuiPrefs *prefs, gpointer va_cols, gpointer user_data)
{
	guint i;

	GValueArray *cols = (gpointer) va_cols;
	GList *column_list = NULL;
	GList *C = NULL;
	
	g_return_if_fail (cols != NULL);
	
	MameGamelistView *gamelist_view = (gpointer) user_data;
	column_list = gtk_tree_view_get_columns (GTK_TREE_VIEW (gamelist_view));
	i = 0;

	for (C = g_list_first (column_list); C != NULL; C = g_list_next (C)) {
		GtkTreeViewColumn *column = C->data;
		
		if ((gtk_tree_view_column_get_visible (column)) && 
		    (g_value_get_int (g_value_array_get_nth (cols, i)) == 0)) {
			    /* Column is visible, but shouldn't be */
			    gtk_tree_view_column_set_visible (column, FALSE);
		}
		
		if ((!gtk_tree_view_column_get_visible (column)) && 
		    (g_value_get_int (g_value_array_get_nth (cols, i)) == 1)) {
			    /* Column is not visible, but should be */
			    gtk_tree_view_column_set_visible (column, TRUE);
		}
		i++;
	}
	
	g_list_free (column_list);
}

/* Handler for when 'Prefer custom icons' in the GuiPrefs object is changed
   (usually via the GUI Preferences dialog). If turned on, want to use
   custom icons for non-correct ROMs. Trigger this by invoking
   adjustment_scrolled_delayed which updates the icons appropriately. This is
   done in a g_timeout_add clause to prevent blocking the UI */
static void
on_prefs_prefercustomicons_toggled (MameGuiPrefs *prefs, gboolean prefercustomicons, gpointer user_data)
{

	MameGamelistView *gamelist_view;
	
	gamelist_view = (gpointer) user_data;

	/* Call adjustment_scrolled_delayed which updates the icons in the
	   visible range of the gamelist */
	g_timeout_add (ICON_TIMEOUT,
		       (GSourceFunc) adjustment_scrolled_delayed, gamelist_view);

}

/* Handler for when the 'The' prefix in the GuiPrefs object is changed
   (usually via the GUI Preferences dialog) */
static void
on_prefs_theprefix_toggled (MameGuiPrefs *prefs, gboolean theprefix, gpointer user_data)
{
	MameGamelistView *gamelist_view;
	
	gamelist_view = (gpointer) user_data;

	gtk_tree_model_foreach (GTK_TREE_MODEL (gamelist_view->priv->curr_model),
				foreach_update_prefix_in_store,
				(gpointer) theprefix);
}

/* Hide a column from the popup menu; changing the main_gui.gui_prefs setting
   triggers the callback on_prefs_column_shown_toggled, so there is no need to
   explicitly hide the column here */
void
on_column_hide_activate (GtkMenuItem     *menuitem,
			 gpointer         user_data)
{
	GList *column_list = NULL;
	GList *C = NULL;
	GValueArray *va;
	
	g_object_get (main_gui.gui_prefs, "cols-shown", &va, NULL);

/*	GMAMEUI_DEBUG ("Column Hide - %i",ColumnHide_selected);*/

	column_list=gtk_tree_view_get_columns (GTK_TREE_VIEW (main_gui.displayed_list));
	for (C = g_list_first (column_list) ; C != NULL ; C = g_list_next (C)) {
		if (ColumnHide_selected == gtk_tree_view_column_get_sort_column_id (GTK_TREE_VIEW_COLUMN (C->data)))
			break;
	}

	if (C->data) {
		g_value_set_int (g_value_array_get_nth (va, ColumnHide_selected), 0);
		g_object_set (main_gui.gui_prefs, "cols-shown", va, NULL);
/*		GMAMEUI_DEBUG ("Column Hidden - %i", ColumnHide_selected);*/
	}
}

static void
on_displayed_list_resize_column (GtkWidget      *widget,
				 GtkRequisition *requisition,
				 gpointer        user_data)
{
	GtkTreeViewColumn *col;
	gint col_id;
	GValueArray *va;
	GList *column_list;
	GList *pointer_list;
	gint i;

	ListMode current_mode;
GMAMEUI_DEBUG("Resizing columns");
	g_object_get (main_gui.gui_prefs,
		      "current-mode", &current_mode,
		      "cols-width", &va,
		      NULL);
	
	if ((current_mode == DETAILS)
#ifdef TREESTORE
	    || (current_mode == DETAILS_TREE)
#endif
	    ) {
		column_list = gtk_tree_view_get_columns (GTK_TREE_VIEW (widget));

		for (pointer_list = g_list_first (column_list), i = 0;
		     pointer_list != NULL;
		     pointer_list = g_list_next (pointer_list), i++) {
			col = GTK_TREE_VIEW_COLUMN (pointer_list->data);
			if (gtk_tree_view_column_get_visible (col)) {
				col_id = gtk_tree_view_column_get_sort_column_id (col);
				/*GMAMEUI_DEBUG ("Setting width of column %d %s to %d",
					       col_id, gtk_tree_view_column_get_title (col),
					       gtk_tree_view_column_get_width (col));*/
				g_value_set_int (g_value_array_get_nth (va, col_id), gtk_tree_view_column_get_width (col));
				g_object_set (main_gui.gui_prefs, "cols-width", va, NULL);
			} else
				GMAMEUI_DEBUG ("  Column %s is not visible", gtk_tree_view_column_get_title (col));
		}
		
		g_list_free (column_list);
		g_list_free (pointer_list);
	} else
		GMAMEUI_DEBUG ("  Not in details mode - column sizing not allowed");
/* FIXME TODO	
	if (va)
		g_value_array_free (va);*/
GMAMEUI_DEBUG("Resizing columns... done");
}

#ifdef TREESTORE
void
on_displayed_list_row_collapsed (GtkTreeView *treeview,
				 GtkTreeIter *iter,
				 GtkTreePath *path,
				 gpointer     user_data)
{
	GtkTreeIter iter_child;
	RomEntry *tmprom;
	GtkTreeSelection *select;
	GtkTreeModel* treemodel;
	gchar *current_rom_name;

	/* If one of the child iter is selected, we select the parent iter */
	treemodel = gtk_tree_view_get_model (treeview);
	gtk_tree_model_iter_children (treemodel, &iter_child, iter);
	gtk_tree_model_get (treemodel, &iter_child, ROMENTRY, &tmprom, -1);

	while ((g_ascii_strcasecmp (current_rom_name, tmprom->romname) != 0) &&
	       (gtk_tree_model_iter_next (treemodel, &iter_child))) {
		gtk_tree_model_get (treemodel, &iter_child, ROMENTRY, &tmprom, -1);
	}

	g_object_get (main_gui.gui_prefs, "current-rom", &current_rom_name, NULL);
	
	if (g_ascii_strcasecmp (current_rom_name, tmprom->romname) == 0) {
		GtkTreePath *tree_path;
		select = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));
		tree_path = gtk_tree_model_get_path (GTK_TREE_MODEL (treemodel), iter);
		gtk_tree_view_set_cursor (GTK_TREE_VIEW (treeview),
					  tree_path,
					  NULL, FALSE);
		gtk_tree_path_free (tree_path);
	}
	
	g_free (current_rom_name);
}
#endif

void
on_displayed_list_sort_column_changed (GtkTreeSortable *treesortable,
                                       gpointer         user_data)
{
	gint sort_column_id;
	GtkSortType order;

	if (gtk_tree_sortable_get_sort_column_id (treesortable, &sort_column_id, &order)) {
GMAMEUI_DEBUG ("Changing column %d to sort by %d", sort_column_id, order);
		g_object_set (main_gui.gui_prefs,
			      "sort-col", sort_column_id,
			      "sort-col-direction", order,
			      NULL);
	}
}

MameRomEntry *
gamelist_get_selected_game (void)
{
	MameRomEntry *game_data;
	GtkTreeIter iter;
	GtkTreeModel *model;
	GtkTreeSelection *select;

	game_data = NULL;

	g_return_val_if_fail (main_gui.displayed_list != NULL, NULL);

	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (main_gui.displayed_list));

	if (gtk_tree_selection_get_selected (select, &model, &iter))
		gtk_tree_model_get (model, &iter, ROMENTRY, &game_data, -1);

	return game_data;
}

/* Filters the GtkTreeModel by seeing if the text in the MameSearchEntry
   is a case insensitive match for text in the first column */
static gboolean
filter_func (GtkTreeModel *model,
             GtkTreeIter  *iter,
             GtkEntry     *entry)
{

	const gchar *needle;
	const gchar *haystack;
	gint filtered;

	/* Get the gamename, and whether it is filtered by ROM, from the iter */
	gtk_tree_model_get (model, iter,
			    GAMENAME, &haystack,
			    FILTERED, &filtered,
			    -1);
	needle = gtk_entry_get_text (entry);

	/*GMAMEUI_DEBUG ("Checking for filter on %s: %d", haystack, filtered);*/
	
	/* If no text has been entered in the MameSearchEntry, don't do a string
	   comparison (which will segfault), but instead just check whether the
	   ROM has been filtered */
	if (haystack == NULL)
		return filtered;
	
	/*GMAMEUI_DEBUG ("Comparing search criteria %s against %s", needle, haystack);*/
	if ((strcasestr (haystack, needle) != NULL) && filtered) {
		return TRUE;
		visible_games++;
	} else
		return FALSE;


} 

/* Callback handler for when data is entered in the search criteria field.
   It asks the GtkTreeView to refilter itself, invoking the callback registered
   in the call to gtk_tree_model_filter_set_visible_func () */
static void
on_search_changed (GtkEntry *entry, gchar *criteria, gpointer user_data)
{
	GtkTreeModelSort *sort;

	/*
	   The model structure is set as:
	      Sortable
	      +- Filter
	         +- Store
	   So, we need to first get the model from the main_gui.displayed_list, which
	   returns the Sortable, then get the model from the Sortable, which yields the
	   Filter. We can then perform the refilter on this.
	 */
	
	sort = GTK_TREE_MODEL_SORT (gtk_tree_view_get_model (GTK_TREE_VIEW (main_gui.displayed_list)));

	gtk_tree_model_filter_refilter (GTK_TREE_MODEL_FILTER (gtk_tree_model_sort_get_model (sort)));
}

static void
populate_model_from_gamelist (MameGamelistView *gamelist_view, GtkTreeModel *model)
{
	GList *listpointer;
	MameRomEntry *tmprom;
#ifdef TREESTORE
	gchar *my_romname_root = NULL;
#endif
	gchar *my_hassamples;
	GtkTreeIter iter;
#ifdef TREESTORE
	GtkTreeIter iter_root;
	gboolean is_root;
#endif
	gint rom_filter_opt;
	const gchar *name_in_list;

	g_return_if_fail (model != NULL);

	/* Get the current ROM filter setting */
	g_object_get (main_gui.gui_prefs, "current-rom-filter", &rom_filter_opt, NULL);
	
	/* Always clear and repopulate, since we call from numerous instances */
	gtk_list_store_clear (GTK_LIST_STORE (model));
		
	/* Fill the model with data */
	for (listpointer = g_list_first (mame_gamelist_get_roms_glist (gui_prefs.gl));
	     (listpointer);
	     listpointer = g_list_next (listpointer)) {

		tmprom = (MameRomEntry *) listpointer->data;	     

		PangoStyle pangostyle;
		GdkPixbuf *pixbuf = NULL;

		name_in_list = mame_rom_entry_get_list_name (tmprom);

		/* Has Samples */
		if (!mame_rom_entry_has_samples (tmprom))
			my_hassamples = NULL;
		else
			my_hassamples = (mame_rom_entry_get_sample_status (tmprom) == CORRECT) ? _("Yes") : _("No");
		
		if (mame_rom_entry_is_clone (tmprom)) {
			pangostyle = PANGO_STYLE_ITALIC;	/* Clone */
		} else {
			pangostyle = PANGO_STYLE_NORMAL;	/* Original */
		}

		/* Set the pixbuf for the status icon - start as being the status icon,
		   then look for the custom icon in the adjustment_scrolled_delayed
		   callback since we don't want to load icons from zips for EVERY ROM */
		pixbuf = gdk_pixbuf_copy (Status_Icons [mame_rom_entry_get_rom_status (tmprom)]);

#ifdef TREESTORE
		/* Determine if the row is a root */
		if ((j == 0) ||
		    (g_ascii_strcasecmp (tmprom->cloneof, "-") == 0) || 
		    !my_romname_root ||
		    (g_ascii_strcasecmp (tmprom->cloneof, my_romname_root) != 0)) {
				is_root = TRUE;
			} else {
				is_root = FALSE;
			}
#endif
		/* Memorize the original names */
/*		if (!(g_ascii_strcasecmp (tmprom->cloneof, "-"))) {
			if (my_romname_root)
				g_free (my_romname_root);

			my_romname_root= g_strdup (tmprom->romname);
		}*/
		gchar *romname, *manu, *year, *cloneof, *driver, *category, *version;
		gint timesplayed;

		g_object_get (tmprom,
			      "romname", &romname,
			      "times-played", &timesplayed,
			      "manufacturer", &manu,
			      "year", &year,
			      "cloneof", &cloneof,
			      "driver", &driver,
			      "category", &category,
			      "version-added", &version,
			      NULL);
	      
#ifdef TREESTORE
		if (gamelist_view->priv->is_tree_mode) {

			if (is_root)
				gtk_tree_store_append (GTK_TREE_STORE (model),
						       &iter, NULL);  /* Acquire an iterator */
			else
				gtk_tree_store_append (GTK_TREE_STORE (model),
						       &iter, &iter_root);  /* Acquire an iterator */

			gtk_tree_store_set (GTK_TREE_STORE (model), &iter,
					    GAMENAME,     name_in_list,
					    HAS_SAMPLES,  my_hassamples,
					    ROMNAME,      romname,
					    TIMESPLAYED,  timesplayed,
					    MANU,         manu,
					    YEAR,         year,
					    CLONE,        cloneof,
					    DRIVER,       driver,
					    MAMEVER,      version,
					    CATEGORY,     category,
					    ROMENTRY,     tmprom,
					    TEXTSTYLE,    pangostyle,
					    FILTERED,     game_filtered (tmprom, rom_filter_opt),
					    PIXBUF,       pixbuf,
					    -1);

			if (is_root)
				iter_root = iter;
			} else {
#endif

			gtk_list_store_append (GTK_LIST_STORE (model), &iter);  /* Acquire an iterator */

			gtk_list_store_set (GTK_LIST_STORE (model), &iter,
					    GAMENAME,     name_in_list,
					    HAS_SAMPLES,  my_hassamples,
					    ROMNAME,      romname,
					    TIMESPLAYED,  timesplayed,
					    MANU,         manu,
					    YEAR,         year,
					    CLONE,        cloneof,
					    DRIVER,       driver,
					    MAMEVER,      version,
					    CATEGORY,     category,
					    ROMENTRY,     tmprom,
					    TEXTSTYLE,    pangostyle,
					    FILTERED,     game_filtered (tmprom, rom_filter_opt),
					    PIXBUF,       pixbuf,
					    -1);

#ifdef TREESTORE
		}
#endif
		mame_rom_entry_set_position (tmprom, iter);
			
		g_free (romname);
		g_free (manu);
		g_free (year);
		g_free (cloneof);
		g_free (driver);
		g_free (category);
		g_free (version);

	}
	
	set_status_bar_game_count (gamelist_view);

}

static gboolean
foreach_update_filter (GtkTreeModel *model,
		       GtkTreePath  *path,
		       GtkTreeIter  *iter,
		       gpointer      user_data)
{
	MameRomEntry *tmprom;
	gint rom_filter_opt;

	rom_filter_opt = (gint) user_data;

	gtk_tree_model_get (model, iter,
			    ROMENTRY, &tmprom,
			    -1);

	gtk_list_store_set (GTK_LIST_STORE (model), iter,
			    FILTERED, game_filtered (tmprom, rom_filter_opt),
			    -1);

	return FALSE;
}

/* Completely clear the existing gamelist and repopulate it - usually after
   starting GMAMEUI, or when rebuilding the contents */
void
mame_gamelist_view_repopulate_contents (MameGamelistView *gamelist_view)
{
	g_return_if_fail (gamelist_view != NULL);

	populate_model_from_gamelist (gamelist_view, gamelist_view->priv->curr_model);
}

/* For each ROM in the model, recalculate whether it should be shown, based on the
   selected filter. Invoked whenever the LHS filter selection is changed, or the
   top filter buttons are changed */
void
mame_gamelist_view_update_filter (MameGamelistView *gamelist_view)
{
	gint rom_filter_opt;
	
	g_return_if_fail (gamelist_view != NULL);	
	
	/* Get the current ROM filter setting */
	g_object_get (main_gui.gui_prefs, "current-rom-filter", &rom_filter_opt, NULL);

	gtk_tree_model_foreach (GTK_TREE_MODEL (gamelist_view->priv->curr_model),
				foreach_update_filter,
				(gpointer) rom_filter_opt);

	set_status_bar_game_count (gamelist_view);

}

static void
on_romset_audited (GmameuiAudit *audit,
		   gchar *audit_line,
		   gint type,
		   gint auditresult,
		   gpointer user_data)
{
	MameGamelistView *gamelist_view;
	MameRomEntry *rom;
	const gchar *romname;
	gchar *icondir, *iconzipfile;
	gint rom_filter_opt;
	gboolean prefercustomicons;

	gamelist_view = (gpointer) user_data;

	g_return_if_fail (gamelist_view != NULL);

	g_object_get (main_gui.gui_prefs,
		      "current-rom-filter", &rom_filter_opt,
		      "dir-icons", &icondir,
		      "prefercustomicons", &prefercustomicons,
		      NULL);

	iconzipfile = g_build_filename (icondir, "icons.zip", NULL);
	
	/* Get the name of the rom being audited so we can update the processing label */
	romname = get_romset_name_from_audit_line (audit_line);
	rom = get_rom_from_gamelist_by_name (gui_prefs.gl, romname);

	g_return_if_fail (rom != NULL);

	if (type == AUDIT_TYPE_ROM) {
		GdkPixbuf *pixbuf = NULL;
		GMAMEUI_DEBUG ("  Now processing ROM %s", mame_rom_entry_get_romname (rom));
		g_object_set (rom, "has-roms", auditresult, NULL);
		pixbuf = get_icon_for_rom (rom, ROM_ICON_SIZE, icondir, iconzipfile, prefercustomicons);
		GtkTreeIter iter = mame_rom_entry_get_position (rom);

		gtk_list_store_set (GTK_LIST_STORE (gamelist_view->priv->curr_model), &iter,
				    FILTERED, game_filtered (rom, rom_filter_opt),
		                    PIXBUF,   pixbuf,
				    -1);

		set_status_bar_game_count (gamelist_view);
		
		if (pixbuf)
			g_object_unref (pixbuf);
	}

	g_free (icondir);
	g_free (iconzipfile);

}

/*
 * Invoked from the menu, or after the executable has been changed. Need to run
 * mame -listxml to get all supported games. Since differing versions of MAME
 * may support different ROMs, need to assume all ROMs are UNAVAILABLE.
 * Clear the GtkTreeView, clear the gamelist, too.
 * We also conduct an audit.
 */
void
gmameui_gamelist_rebuild (MameGamelistView *gamelist_view)
{

g_return_if_fail (gamelist_view != NULL);
	
	gtk_widget_set_sensitive (main_gui.scrolled_window_games, FALSE);
	UPDATE_GUI;

	/* Set the GtkTreeView's model to NULL to clear the display */
	gtk_tree_view_set_model (GTK_TREE_VIEW (gamelist_view), NULL);
	
	GMAMEUI_DEBUG ("Parsing MAME output to recreate game list...");
	gamelist_parse (mame_exec_list_get_current_executable (main_gui.exec_list));

	mame_gamelist_save (gui_prefs.gl);

	GMAMEUI_DEBUG ("Reloading everything...");
	load_games_ini ();
	load_catver_ini ();

	/* Repopulate the GtkTreeView with contents of the gamelist */
	mame_gamelist_view_repopulate_contents (main_gui.displayed_list);

	/* Set the GtkTreeView's model to the newly-populated model */
	gtk_tree_view_set_model (GTK_TREE_VIEW (gamelist_view),
	                         GTK_TREE_MODEL (gamelist_view->priv->sort_model));
		
	gtk_widget_set_sensitive (main_gui.scrolled_window_games, TRUE);
	
	GMAMEUI_DEBUG ("Done rebuilding gamelist");

	GMAMEUI_DEBUG ("Auditing");
	g_signal_connect (gui_prefs.audit, "romset-audited",
			  G_CALLBACK (on_romset_audited), main_gui.displayed_list);
	mame_audit_start_full ();
	GMAMEUI_DEBUG ("Done auditing");

	/* Update the filter, since all ROMs will now be marked in
	   mame_audit_start_full as UNAVAILABLE until audited and we want to hide
	   them until they are audited as meeting the current filter settings */
	mame_gamelist_view_update_filter (main_gui.displayed_list);
}

/* This function is to set the game icon from the zip file for each visible game;
   it is called as a timeout whenever the scrolled window changes (gamelist scrolled up or down) */
gboolean
adjustment_scrolled_delayed (MameGamelistView *gamelist_view)
{
	GtkTreeIter iter;

	GtkTreePath *tree_path;
	guint i;
	gboolean valid;
	GtkAdjustment *vadj;
	gchar *icondir;
	gchar *iconzipfile;
	gboolean prefercustomicons;
	
	g_return_val_if_fail (main_gui.gui_prefs != NULL, FALSE);
	
GMAMEUI_DEBUG ("Entering adjustment_scrolled_delayed");
	g_object_get (main_gui.gui_prefs,
		      "dir-icons", &icondir,
		      "prefercustomicons", &prefercustomicons,
		      NULL);

	g_return_val_if_fail (visible_games > 0, FALSE);
	
	iconzipfile = g_build_filename (icondir, "icons.zip", NULL);

	/* Getting the vertical window area */
	vadj = gtk_scrolled_window_get_vadjustment (GTK_SCROLLED_WINDOW (main_gui.scrolled_window_games));
	
	/* Disable the callback */
	g_signal_handlers_block_by_func (G_OBJECT (vadj),
					 adjustment_scrolled,
					 gamelist_view);

	/* Get the first iter from the model */
	valid = gtk_tree_model_get_iter_first (GTK_TREE_MODEL (gamelist_view->priv->sort_model), &iter);

	i = 0;

	MameRomEntry *tmprom;

	while ((i < visible_games) && valid) {
		GtkTreeIter filter_iter;
		GtkTreeIter model_iter;

		gtk_tree_model_get (GTK_TREE_MODEL (gamelist_view->priv->sort_model),
				    &iter,
				    ROMENTRY, &tmprom,
				    -1);
		/*GMAMEUI_DEBUG ("Updating icon for %s - the value of icon_pixbuf is %d",
				 tmprom->romname,
				 !tmprom->icon_pixbuf);*/
		
		/* Get the path so we can get the new iter after the store is updated */
		tree_path = gtk_tree_model_get_path (GTK_TREE_MODEL (gamelist_view->priv->sort_model), &iter);

		/* Get the position of the current iter's path so we can see
		   whether it falls within the visible area */
		GdkRectangle rect;
		gtk_tree_view_get_cell_area (GTK_TREE_VIEW (main_gui.displayed_list),
					     tree_path,
					     NULL, &rect);

		/* Only update the pixbuf if it is in the viewable area (defined by the rect) */
		if ((rect.y + rect.height) > 0 && (rect.y < vadj->page_size)) {
			GdkPixbuf *icon;

			icon = get_icon_for_rom (tmprom, ROM_ICON_SIZE, icondir, iconzipfile, prefercustomicons);

			mame_rom_entry_set_icon (tmprom, icon);
			
			/* Sort +- Filter +- Model, so need to convert the iter
			   on the sort to get to the base before we can update the base model */

			/* Get the filter model and iter */
			gtk_tree_model_sort_convert_iter_to_child_iter (GTK_TREE_MODEL_SORT (gamelist_view->priv->sort_model),
						&filter_iter,
						&iter);

			/* Get the base model and iter from the filter */
			gtk_tree_model_filter_convert_iter_to_child_iter (GTK_TREE_MODEL_FILTER (gamelist_view->priv->filter_model),
						  &model_iter,
						  &filter_iter);

			/* Update the ROM in the list store with the icon from the zip file */
			if (icon) {
				gtk_list_store_set (GTK_LIST_STORE (gamelist_view->priv->curr_model),
						    &model_iter,
						    PIXBUF, icon,
						    -1);
				/* After the list store has been updated, the iterator
				   is lost, so we need to get it from the updated model */
				gtk_tree_model_get_iter (GTK_TREE_MODEL (gamelist_view->priv->sort_model),
							 &iter, tree_path);
				
			}
		}

		/* Move to the next item in the list */
		valid = gtk_tree_model_iter_next (GTK_TREE_MODEL (gamelist_view->priv->sort_model), &iter);

		gtk_tree_path_free (tree_path);

		i++;
	}

	/* Re-Enable the callback */
	g_signal_handlers_unblock_by_func (G_OBJECT (vadj),
					   adjustment_scrolled,
					   gamelist_view);

	g_free (iconzipfile);
	g_free (icondir);
GMAMEUI_DEBUG ("Leaving adjustment_scrolled_delayed");
	return FALSE;
}
