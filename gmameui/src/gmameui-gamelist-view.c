/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * GMAMEUI
 *
 * Copyright 2007-2010 Andrew Burton <adb@iinet.net.au>
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
#include <string.h> /* For strcasestr */

#include "gmameui-gamelist-view.h"
#include "game_list.h"
#include "rom_entry.h"
#include "gui.h"	/* For main_gui struct */
#include "gui_prefs.h"
#include "audit.h"
#include "gmameui-listoutput-dlg.h"

static const int ROM_ICON_SIZE = 24;

struct _MameGamelistViewPrivate {
	GtkTreeModel *liststore;	/* The representation of the model in List mode */

	GtkTreeModel *curr_model;       /* A pointer to either the base liststore or treestore */

	GtkTreeModel *sort_model;       /* Implementation of GtkTreeModelSort */
	GtkTreeModel *filter_model;     /* Implementation of GtkTreeModelFilter */

	guint timeout_icon;
};

G_DEFINE_TYPE (MameGamelistView, mame_gamelist_view, GTK_TYPE_TREE_VIEW)

#define MAME_GAMELIST_VIEW_GET_PRIVATE(o)  (MAME_GAMELIST_VIEW (o)->priv)

#define SELECT_TIMEOUT 110
#define ICON_TIMEOUT 100

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
populate_model_from_gamelist (MameGamelistView *gamelist_view, MameGamelist *gl);
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
/* Callbacks handling when the auditing finishes */
static void
on_audit_complete             (GmameuiAudit *audit,
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
		if (list_mode == DETAILS) {
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

/* Determines whether a romset should be filtered (hidden from view)
   based on the filter option passed in */
static gboolean
game_filtered (MameRomEntry * rom, gint rom_filter_opt)
{
	/* gchar **manufacturer; */
	
	gboolean is;
	Columns_type type;
	gchar *value;
	gint int_value;
	gboolean retval;

	/* ROM information */
	gboolean is_bios;
	gboolean is_favourite;
	gboolean is_clone;
	gboolean is_vector;
	
	g_return_val_if_fail (selected_filter != NULL, FALSE);
	g_return_val_if_fail (rom != NULL, FALSE);
	
	retval = FALSE;

	g_object_get (selected_filter,
		      "is", &is,
		      "type", &type,
		      "value", &value,
		      "int_value", &int_value,
		      NULL);

	is_bios = mame_rom_entry_is_bios (rom);

	/* Only display a BIOS rom if the BIOS filter is explicitly stated */
	if (is_bios) { 
		if (type == IS_BIOS) {
			retval = ( (is && is_bios) ||
				 (!is && !is_bios));
		} else
			retval = FALSE;
	} else {
		switch (type) {
			
			gchar *driver, *ver_added, *category;
			DriverStatus driver_status;
			DriverStatus driver_status_colour;
			DriverStatus driver_status_sound;
			DriverStatus driver_status_graphics;
			gint timesplayed, num_channels;
			RomStatus rom_status;
			ControlType control;
			
			case DRIVER:
				g_object_get (rom, "driver", &driver, NULL);
				retval = ((is && !g_strcasecmp (driver,value)) ||
					 (!is && g_strcasecmp (driver,value)));
				g_free (driver);
				break;
			case CLONE:
				is_clone = mame_rom_entry_is_clone (rom);
				retval = ((is && !is_clone) || (!is && is_clone));
				break;
			case CONTROL:
				g_object_get (rom, "control-type", &control, NULL);
				retval = ((is && (control == (ControlType) int_value))  ||
					 (!is && !(control == (ControlType) int_value)));
				break;
			case MAMEVER:				
				g_object_get (rom, "version-added", &ver_added, NULL);
				if (ver_added)
					retval = (g_ascii_strcasecmp (ver_added, value) == 0);
				g_free (ver_added);
				break;
			case CATEGORY:
				g_object_get (rom, "category", &category, NULL);
				if (category)
					retval = (g_ascii_strcasecmp (category, value) == 0);
				g_free (category);
				break;
			case FAVORITE:
				is_favourite = mame_rom_entry_is_favourite (rom);
				retval = ( (is && is_favourite) ||
					 (!is && !is_favourite));
				break;
			case VECTOR:
				is_vector = mame_rom_entry_is_vector (rom);
				retval = ( (is && is_vector) ||
					 (!is && !is_vector));
				break;
			case DRIVER_STATUS:
				/* This is a summary of imperfect colour, sound, graphic
				   and emulation */
				g_object_get (rom, "driver-status", &driver_status, NULL);
				retval = ( (is && driver_status == (DriverStatus)int_value) ||
					 (!is && !driver_status == (DriverStatus)int_value));
				break;
			case COLOR_STATUS:
				g_object_get (rom, "driver-status-colour", &driver_status_colour, NULL);
				retval = ( (is && (driver_status_colour == (DriverStatus)int_value))  ||
					 (!is && ! (driver_status_colour == (DriverStatus)int_value)));
				break;
			case SOUND_STATUS:
				g_object_get (rom, "driver-status-sound", &driver_status_sound, NULL);
				retval = ( (is && (driver_status_sound == (DriverStatus)int_value))  ||
					 (!is && ! (driver_status_sound == (DriverStatus)int_value)));
				break;
			case GRAPHIC_STATUS:
				g_object_get (rom, "driver-status-graphics", &driver_status_graphics, NULL);
				retval = ( (is && (driver_status_graphics == (DriverStatus)int_value))  ||
					 (!is && ! (driver_status_graphics == (DriverStatus)int_value)));
				break;
			case HAS_ROMS:
				rom_status = mame_rom_entry_get_rom_status (rom);
				retval = ((is && (rom_status == (RomStatus) int_value))  ||
					 (!is && !(rom_status == (RomStatus) int_value)));
				break;
			case HAS_SAMPLES:
				retval = ( (is && (mame_rom_entry_has_samples (rom) == int_value))  ||
					 (!is && ! (mame_rom_entry_has_samples (rom) == int_value)));
				break;
			case TIMESPLAYED:
				g_object_get (rom, "times-played", &timesplayed, NULL);
				retval = ( (is && (timesplayed == int_value)) ||
					 (!is && ! (timesplayed == int_value)));
				break;
			case CHANNELS:
				g_object_get (rom, "num-channels", &num_channels, NULL);
				retval = ( (is && (num_channels == int_value)) ||
					 (!is && (num_channels != int_value)));
				break;
			/* We are not currently supporting the YEAR and MANUFACTURER filters
			   since it makes the LHS filter list too long 
		
			case YEAR:
				retval = ( (is && (rom->year == value)) ||
					 (!is && (rom->year != value)));
				break;
				* comparing parsed text and text *
			case MANU:
				manufacturer = rom_entry_get_manufacturers (rom);
				* we have now one or two clean manufacturer (s) we still need to differentiates sub companies*
				if (manufacturer[1] != NULL) {
					if ( (is && !g_strncasecmp (manufacturer[0], value, 5)) ||
					     (!is && g_strncasecmp (manufacturer[0], value, 5)) ||
					     (is && !g_strncasecmp (manufacturer[1], value, 5)) ||
					     (!is && g_strncasecmp (manufacturer[1], value, 5))
					     ) {
						g_strfreev (manufacturer);
						retval = TRUE;

					}
				} else {
					if ( (is && !g_strncasecmp (manufacturer[0], value, 5)) ||
					     (!is && g_strncasecmp (manufacturer[0], value, 5))
					     ) {
						g_strfreev (manufacturer);
						retval = TRUE;
					}
				}
				g_strfreev (manufacturer);
				break;*/
			default:
				GMAMEUI_DEBUG ("Trying to filter, but filter type %d is not handled", type);
				retval = FALSE;
			}
	}
	g_free (value);
	
	/* Final additional check whether the ROM should be displayed based on the ROM
	   filter settings and whether the ROM is available or unavailable */
	if (retval) {
		if (rom_filter_opt == 1) {
			/* Only show Available */
			retval = (mame_rom_entry_get_rom_status (rom) != NOT_AVAIL) ? TRUE : FALSE;
		} else if (rom_filter_opt == 2) {
			/* Only show Unavailable */
			retval = (mame_rom_entry_get_rom_status (rom) == NOT_AVAIL) ? TRUE : FALSE;
		} else {
		/* No need to process for All ROMs */

		}
	}

	return retval;
}

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

	gtk_list_store_set (GTK_LIST_STORE (gamelist_view->priv->curr_model), &iter,
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

/* FIXME TODO Move this functionality to gmameui-statusbar.c */
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

	gtk_statusbar_pop (GTK_STATUSBAR (main_gui.statusbar), 1);
	gtk_statusbar_push (GTK_STATUSBAR (main_gui.statusbar), 1, message);
	
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
	
	/* Unset the tree model until it has been filled */
	if ((gamelist_view) && (gamelist_view->priv->curr_model)) {
		gtk_tree_view_set_model (GTK_TREE_VIEW (gamelist_view), NULL);
	}

	gamelist_view->priv->curr_model = gamelist_view->priv->liststore;
	
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
 
	/* Sort the list */
	set_list_sortable_column (gamelist_view);

	/* Callback when the sort column/order changes */
	g_signal_connect (G_OBJECT (gamelist_view->priv->sort_model), "sort-column-changed",
			  G_CALLBACK (on_displayed_list_sort_column_changed), NULL);

	/* The column headers are clickable in Details view only */
	gtk_tree_view_set_headers_clickable (GTK_TREE_VIEW (gamelist_view), (current_mode == DETAILS));

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
	
	if (current_mode == DETAILS) {
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
   scrolls to that row */
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
	
	/* Find the selected game in the gamelist, and scroll to it */
	if (visible_games > 0) {
		/* Scroll to the game specified from the preferences */
		gtk_tree_model_foreach (GTK_TREE_MODEL (gamelist_view->priv->sort_model),
					foreach_find_rom_in_store,
					current_rom_name);
	}
	
	g_free (current_rom_name);
}

static void
gamelist_popupmenu_show (GdkEventButton *event)
{
	GtkWidget *popup_menu;

	popup_menu = gtk_ui_manager_get_widget (main_gui.manager, "/GameListPopup");
	g_return_if_fail (popup_menu != NULL);

	gtk_menu_popup (GTK_MENU (popup_menu), NULL, NULL,
			NULL, NULL,
			event->button, event->time);
}

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
GMAMEUI_DEBUG ("!!! on_list_clicked");	
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


	gtk_list_store_set (GTK_LIST_STORE (model), iter,
			    GAMENAME, mame_rom_entry_get_list_name (rom),
			    -1);

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
	
	if (current_mode == DETAILS) {
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
		visible_games++;
		return TRUE;
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

	/* Update number of visible games */
	set_status_bar_game_count (main_gui.displayed_list);
}

static void
populate_model_from_gamelist (MameGamelistView *gamelist_view, MameGamelist *gl)
{
	GList *listpointer;
	MameRomEntry *tmprom;
	gchar *my_hassamples;
	GtkTreeIter iter;
	gint rom_filter_opt;
	const gchar *name_in_list;

	g_return_if_fail (gamelist_view->priv->curr_model != NULL);

	/* Get the current ROM filter setting */
	g_object_get (main_gui.gui_prefs, "current-rom-filter", &rom_filter_opt, NULL);
	
	/* Always clear and repopulate, since we call from numerous instances */
	gtk_list_store_clear (GTK_LIST_STORE (gamelist_view->priv->curr_model));

	/* Fill the model with data */
	for (listpointer = g_list_first (mame_gamelist_get_roms_glist (gl));
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
		pixbuf = gmameui_icon_mgr_get_pixbuf_for_status (mame_rom_entry_get_rom_status (tmprom));

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
	      

		gtk_list_store_append (GTK_LIST_STORE (gamelist_view->priv->curr_model), &iter);  /* Acquire an iterator */

		gtk_list_store_set (GTK_LIST_STORE (gamelist_view->priv->curr_model), &iter,
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

	populate_model_from_gamelist (gamelist_view, gui_prefs.gl);
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

/**
 * on_romset_audited:
 * @audit: the currently selected #MameExec
 * @audit_line:
 * @type:
 * @auditresult:
 * @user_data:
 *
 * Triggered whenever a ROM is audited, either from the audit dialog, from
 * the menu command, or when rebuilding the gamelist. We want to update the
 * gamelist to display any changes to the status icon, and determine whether the
 * ROM should now be hidden/displayed.
 */
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
		/*GMAMEUI_DEBUG ("  Now processing ROM %s with result %d", mame_rom_entry_get_romname (rom), auditresult);*/
		g_object_set (rom, "has-roms", auditresult, NULL);

		/* Update the status icon for the ROM */
		pixbuf = get_icon_for_rom (rom, ROM_ICON_SIZE, icondir, iconzipfile, prefercustomicons);

		/* Update the liststore with the icon and whether the ROM is
		   displayed based on the filter setting and the audit value */
		GtkTreeIter iter = mame_rom_entry_get_position (rom);
		gtk_list_store_set (GTK_LIST_STORE (gamelist_view->priv->curr_model), &iter,
				    FILTERED, game_filtered (rom, rom_filter_opt),
		                    PIXBUF,   pixbuf,
				    -1);

		/* Increment the status bar as appropriate */
		set_status_bar_game_count (gamelist_view);
		
		if (pixbuf)
			g_object_unref (pixbuf);
	}
	
	g_free (icondir);
	g_free (iconzipfile);

}

/**
 * on_audit_complete:
 * @user_data:
 *
 * Triggered when the audit has finished, so we can stop the progressbar and
 * hide it.
 */
static void
on_audit_complete (GmameuiAudit *audit, gpointer user_data)
{
	GMAMEUIStatusbar *sb = (gpointer) user_data;
	
	gmameui_statusbar_stop_pulse (sb);
}
	
/**
 * gmameui_gamelist_rebuild:
 * @gamelist_view:
 *
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
	GtkWidget *dlg = gmameui_listoutput_dialog_new (NULL);

	/* Need to run the dialog, rather than show it since we need to save
	   the gamelist only when done, otherwise we have a race condition and
	   the save only captures the first few romsets processed; the dialog
	   takes care of destroying itself */
	gtk_dialog_run (GTK_DIALOG (dlg));
	GMAMEUI_DEBUG ("Parsing MAME output to recreate game list... done");
	
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

	GMAMEUI_DEBUG ("Starting auditing process...");
	/* Trigger the statusbar to show a progressbar */
	/* FIXME TODO These should be controlled via g_signal_emit
	   calls for loading the gamelist etc */
	gmameui_statusbar_start_pulse (main_gui.statusbar);
	gmameui_statusbar_set_progressbar_text (main_gui.statusbar,
	                                        _("Auditing MAME ROMs..."));

	/* Handle ROMs being audited */
	g_signal_connect (gui_prefs.audit, "romset-audited",
			  G_CALLBACK (on_romset_audited), main_gui.displayed_list);
	g_signal_connect (gui_prefs.audit, "rom-audit-complete",
			  G_CALLBACK (on_audit_complete), main_gui.statusbar);
	
	mame_audit_start_full ();

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

void
adjustment_scrolled (GtkAdjustment *adjustment,
		     gpointer       user_data)
{
	MameGamelistView *view = (gpointer) user_data;

	if (view->priv->timeout_icon)
		g_source_remove (view->priv->timeout_icon);

	else
		view->priv->timeout_icon = g_timeout_add (ICON_TIMEOUT,
					      (GSourceFunc) adjustment_scrolled_delayed,
					      view);
	view->priv->timeout_icon = 0;
}

