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

#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include "gmameui.h"
#include "gui.h"
#include "io.h"
#include "gmameui-gamelist-view.h"
#include "gmameui-sidebar.h"
#include "gmameui-zip-utils.h"

int
init_gui (void)
{
	GtkTooltips *tooltips;
	gchar *filename;
	
	tooltips = gtk_tooltips_new ();

	/* Default Pixbuf once for all windows */
	filename = g_build_filename (DATADIR, "gmameui", "gmameui.png", NULL);
        gtk_window_set_default_icon_from_file (filename, NULL);
	g_free (filename);

	gmameui_icons_init ();	

	/* Create the main window */
	MainWindow = create_MainWindow ();
	g_return_val_if_fail ((MainWindow != NULL), -1);

	/* Show and hence realize mainwindow so that MainWindow->window is available */
	gtk_widget_show_all (MainWindow);
	UPDATE_GUI;
	
	/* Need to set the size here otherwise it move when we create the gamelist 
	if (show_screenshot)
		gtk_paned_set_position (main_gui.hpanedRight, xpos_gamelist);*/

	/* Grab focus on the game list */
	gtk_widget_grab_focus (GTK_WIDGET (main_gui.displayed_list));

	/* Invoked whenever the gamelist is scrolled up or down */
	g_signal_connect (G_OBJECT (gtk_scrolled_window_get_vadjustment (GTK_SCROLLED_WINDOW (main_gui.scrolled_window_games))),
	                  "value-changed",
	                  G_CALLBACK (adjustment_scrolled),
	                  main_gui.displayed_list);

	return 0;
}

static void
set_current_executable (MameExec *new_exec)
{
	gint response;
	
	if (new_exec) {
		GtkWidget *dlg;
		
		GMAMEUI_DEBUG ("Executable changed to %s", mame_exec_get_path (new_exec));
		mame_exec_list_set_current_executable (main_gui.exec_list, new_exec);
		g_object_set (main_gui.gui_prefs, "current-executable", mame_exec_get_path (new_exec), NULL);
		
		dlg = gtk_message_dialog_new (GTK_WINDOW (MainWindow),
					      GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
					      GTK_MESSAGE_QUESTION,
					      GTK_BUTTONS_CANCEL,
					      _("Do you want to rebuild the gamelist?"));
		gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (dlg),
							  _("The selected MAME executable has changed, so the list of supported ROMs will have changed. It is recommended that you rebuild the gamelist. Do you want to rebuild now?"));
		gtk_dialog_add_button (GTK_DIALOG (dlg),
				       _("Rebuild gamelist"),
				       GTK_RESPONSE_YES);
		response = gtk_dialog_run (GTK_DIALOG (dlg));
		gtk_widget_destroy (dlg);
				       
	} else {
		gmameui_message (ERROR, NULL, _("Executable is not valid MAME executable... skipping"));
		/* FIXME TODO Remove the executable from the list */
	}

	/* Set sensitive the UI elements that require an executable to be set */
	gtk_action_group_set_sensitive (main_gui.gmameui_rom_exec_action_group, new_exec != NULL);
	gtk_action_group_set_sensitive (main_gui.gmameui_exec_action_group, new_exec != NULL);
	
	/* Rebuild the gamelist */
	if (response == GTK_RESPONSE_YES) {
		gmameui_gamelist_rebuild (main_gui.displayed_list);
	}
}

/* executable selected from the menu */
static void
on_executable_selected (GtkRadioAction *action,
			gpointer          user_data)
{
	gint signal_index;      /* Index of the current action in the group for which "changed" has been emitted */
	gint selected_index;    /* Index of the newly selected action */
	
	/* Because the "changed" signal is emitted on each member of the group,
	   need to check that the current action is the one that is newly selected
	   so this function doesn't process for each member of the group */
	signal_index = GPOINTER_TO_INT (user_data);
	selected_index = gtk_radio_action_get_current_value (action);

	if (signal_index == selected_index) {	
		MameExec *exec;

		exec = mame_exec_list_nth (main_gui.exec_list, signal_index);

		GMAMEUI_DEBUG ("Picking executable %d - %s", signal_index, mame_exec_get_name (exec));
		set_current_executable (exec);
	}
}

/* Dynamically create the executables menu. */
void
add_exec_menu (void)
{
	gchar *current_exec;
	int num_execs;
	int i;
	gboolean default_exec;
	MameExec *exec;

	g_object_get (main_gui.gui_prefs, "current-executable", &current_exec, NULL);
	
	/* Determine whether the current-executable is defined - if not, we will
	   have to pick a default later as we add items */
	if (mame_exec_list_get_exec_by_path (main_gui.exec_list, current_exec) == NULL)
		default_exec = 0;
	else
		default_exec = 1;
	
	
	/* This is really hard to understand from the API. Why do they make it so hard?
	   Ref: http://log.emmanuelebassi.net/archives/2006/08/boogie-woogie-bugle-boy/ */

	/* Remove any existing group if it exists */
	if (main_gui.gmameui_exec_radio_action_group) {
		gtk_ui_manager_remove_ui (main_gui.manager,
					  main_gui.gmameui_exec_merge_id);
		gtk_ui_manager_remove_action_group (main_gui.manager,
						    main_gui.gmameui_exec_radio_action_group);
		g_object_unref (main_gui.gmameui_exec_radio_action_group);
		main_gui.gmameui_exec_radio_action_group = NULL;
	}

	/* Create a new radio action group for the executables */
	GtkActionGroup *exec_radio_action_group;
	GSList *radio_group = NULL;
	exec_radio_action_group = gtk_action_group_new ("GmameuiWindowRadioExecActions");
	gtk_action_group_set_translation_domain (exec_radio_action_group, GETTEXT_PACKAGE);
	gtk_ui_manager_insert_action_group (main_gui.manager, exec_radio_action_group, 0);
	
	num_execs = mame_exec_list_size (main_gui.exec_list);

	main_gui.gmameui_exec_merge_id = gtk_ui_manager_new_merge_id (main_gui.manager);
	 
	/* No items - attach an insensitive place holder */
	if (num_execs == 0) {
		GtkAction *action;
			
		action = gtk_action_new ("execs-empty",
		                         _("No executables"),
		                         NULL,
		                         NULL);
		gtk_action_set_sensitive (action, FALSE);
		                         
		gtk_action_group_add_action (exec_radio_action_group, action);
		g_object_unref (action);
		
		gtk_ui_manager_add_ui (main_gui.manager,
				       main_gui.gmameui_exec_merge_id,
				       "/MenuBar/OptionsMenu/OptionMameExecutables",
				       "execs-empty",
				       "execs-empty",
				       GTK_UI_MANAGER_MENUITEM,
				       FALSE);
	} else {
		for (i = 0; i < num_execs; i++) {
			exec = mame_exec_list_nth (main_gui.exec_list, i);
			gchar *exec_name = g_strdup_printf ("%s (%s) %s",
							    mame_exec_get_name (exec),
							    mame_exec_get_target (exec),
							    mame_exec_get_version (exec));
		
			/* Create a new radio action for the executable */
			gchar *name = g_strdup_printf ("exec-%d", i);
			gchar *action_name = g_strdup (name);
			GtkRadioAction *action;
			action = g_object_new (GTK_TYPE_RADIO_ACTION,
					       "name", action_name,
					       "label", exec_name,
					       "value", i,
					       "stock_id", NULL,
					       NULL);

			gtk_action_group_add_action (exec_radio_action_group, GTK_ACTION (action));
			gtk_radio_action_set_group (action, radio_group);
			radio_group = gtk_radio_action_get_group (action);
			g_object_unref (action);
		
			gtk_ui_manager_add_ui (main_gui.manager,
					       main_gui.gmameui_exec_merge_id,
					       "/MenuBar/OptionsMenu/OptionMameExecutables",
					       name, action_name,
					       GTK_UI_MANAGER_MENUITEM,
					       FALSE);
			GMAMEUI_DEBUG ("Adding action '%s'", action_name);
			
			if (current_exec) {
				if (g_ascii_strcasecmp (mame_exec_get_path (exec), current_exec) == 0) {
					GMAMEUI_DEBUG ("Setting %s as current executable", mame_exec_get_path (exec));
					gtk_radio_action_set_current_value (action, i);
				}
			}

			/* Set the callback when this exec is chosen (need to do this
			   AFTER gtk_radio_action_set_current_value to prevent signal
			   from triggering during startup) */
			g_signal_connect (action, "activate",
					  G_CALLBACK (on_executable_selected),
					  GINT_TO_POINTER (i));
			
			/* If the current exec is not available, pick the first item as default */
			if ((i == 0) && (!default_exec)) {
				gtk_radio_action_set_current_value (action, i);
				
				/* Manually emit the signal - the above is not enough */
				g_signal_emit_by_name (action, "activate", GINT_TO_POINTER (i));
			}
			
			/* TODO Free the executable */
		
			g_free (action_name);
			g_free (exec_name);
			g_free (name);	
		}
		
	}
	main_gui.gmameui_exec_radio_action_group = exec_radio_action_group;

	g_free (current_exec);
}

void gmameui_ui_set_favourites_sensitive (gboolean rom_is_favourite)
{
	gtk_widget_set_sensitive (gtk_ui_manager_get_widget (main_gui.manager,
							     "/MenuBar/FileMenu/FileFavesAddMenu"),
				  !rom_is_favourite);
	gtk_widget_set_sensitive (gtk_ui_manager_get_widget (main_gui.manager,
							     "/MenuBar/FileMenu/FileFavesRemoveMenu"),
				  rom_is_favourite);
	gtk_widget_set_sensitive (gtk_ui_manager_get_widget (main_gui.manager,
							     "/GameListPopup/FileFavesAdd"),
				  !rom_is_favourite);
	gtk_widget_set_sensitive (gtk_ui_manager_get_widget (main_gui.manager,
							     "/GameListPopup/FileFavesRemove"),
				  rom_is_favourite);
}

void gmameui_ui_set_items_sensitive () {

	gboolean rom_and_exec;  /* TRUE if both an executable exists and a ROM is selected */
	rom_and_exec = (gui_prefs.current_game) && (mame_exec_list_size (main_gui.exec_list) > 0);

	gtk_action_group_set_sensitive (main_gui.gmameui_rom_exec_action_group,
					rom_and_exec);
	gtk_action_group_set_sensitive (main_gui.gmameui_exec_action_group,
					mame_exec_list_size (main_gui.exec_list));

	/* Disable ROM and Favourites UI items if no current game */
	gtk_action_group_set_sensitive (main_gui.gmameui_rom_action_group,
					(gui_prefs.current_game != NULL));
	gtk_action_group_set_sensitive (main_gui.gmameui_favourite_action_group,
					(gui_prefs.current_game != NULL));
	
	/* Set the Add/Remove Favourites depending whether the game is a favourite */
	if (gui_prefs.current_game != NULL)
		gmameui_ui_set_favourites_sensitive (mame_rom_entry_is_favourite (gui_prefs.current_game));
}

/* Gets the pixbuf representing a ROM's icon (if the icon directory is
   specified). Search first for the .ico file for the ROM, then for the parent
   ROM, then for the icon zipfile */
GdkPixbuf *
get_icon_for_rom (MameRomEntry *rom,
		  guint size,
		  gchar *icon_dir,
		  gchar *zipfilename,
		  gboolean usecustomicons)
{
	GdkPixbuf *pixbuf, *scaled_pixbuf = NULL;
	gchar *icon_filename;
	gchar *icon_path;
	GError **error = NULL;

	const gchar *romname;
	const gchar *parent_romname;

	g_return_val_if_fail (rom != NULL, NULL);
	
	romname = mame_rom_entry_get_romname (rom);
	parent_romname = mame_rom_entry_get_parent_romname (rom);

	GMAMEUI_DEBUG ("Attempting to get icon for ROM %s", romname);

	/* Use the status icon if we are not using custom icons, or if the
	   ROM status is not correct (want to emphasis incorrect ROMs) */
	if (!usecustomicons || (mame_rom_entry_get_rom_status (rom) != CORRECT)) {
		pixbuf = gdk_pixbuf_copy (Status_Icons [mame_rom_entry_get_rom_status (rom)]);
		/* Return pixbuf here, since we don't need to scale/resize it */
		return pixbuf;
	} else {
		/* Look for <romname>.ico in icon dir */
		icon_filename = g_strdup_printf ("%s.ico", romname);
		icon_path = g_build_filename (icon_dir, icon_filename, NULL);
		pixbuf = gdk_pixbuf_new_from_file (icon_path, error);
	
		g_free (icon_filename);
		g_free (icon_path);
	}

	/* If icon not found, try looking for parent's icon */
	if ((pixbuf == NULL) && mame_rom_entry_is_clone (rom)) {
		GMAMEUI_DEBUG ("Attempting to get icon for ROM %s from parent %s", romname, parent_romname);
		icon_filename = g_strdup_printf ("%s.ico", parent_romname);
		icon_path = g_build_filename (icon_dir, icon_filename, NULL);
		pixbuf = gdk_pixbuf_new_from_file (icon_path, error);

		g_free (icon_filename);
		g_free (icon_path);
	}

	/* If icon not found, look in a zipfile */
	if (pixbuf == NULL)
		pixbuf = read_pixbuf_from_zip_file (zipfilename, (gchar *)  romname);

	if (pixbuf != NULL) {
		GMAMEUI_DEBUG ("Found icon for ROM %s, scaling to size %ix%i", romname, size, size);
		scaled_pixbuf = gdk_pixbuf_scale_simple (pixbuf, size, size, GDK_INTERP_BILINEAR);
		g_object_unref (pixbuf);
	} else
		GMAMEUI_DEBUG ("Could not find icon for %s", romname);
	
	return scaled_pixbuf;
}

/* Returns a pixbuf from the current GNOME theme for a
   specified stock ID */
static GdkPixbuf *
gmameui_get_icon_from_theme (const char *id,
			    GtkIconSize size)
{
	GtkIconTheme *theme;
	int width, height;
	GdkPixbuf *pixbuf;

	theme = gtk_icon_theme_get_default ();
        gtk_icon_size_lookup (size, &width, &height);

	pixbuf = gtk_icon_theme_load_icon (theme, id, width, 0, NULL);

	return pixbuf;
}

/* Returns a pixbuf from the GMAMEUI install directory for a
   specified filename */
static GdkPixbuf *
gmameui_get_pixbuf_from_gmameui_install (const char *filename)
{
	GdkPixbuf *pixbuf;
	gchar *path, *fn;

	fn = g_strconcat (filename, ".png", NULL);
	
	path = g_build_filename (DATADIR, "gmameui", fn, NULL);
	pixbuf = gdk_pixbuf_new_from_file (path, NULL);
	
	g_free (fn);
	g_free (path);

	return pixbuf;
}

static GdkPixbuf *
composite_icons (const GdkPixbuf *src1,
		 const GdkPixbuf *src2)
{
	GdkPixbuf *dest;
	GdkPixbuf *scaled;
	gint w1, w2, h1, h2;
	gint dest_x, dest_y;
	gboolean do_scale;

	if (!src1)
		return NULL;

	dest = gdk_pixbuf_copy (src1);

	if (!src2)
		return dest;

	w1 = gdk_pixbuf_get_width (src1);
	h1 = gdk_pixbuf_get_height (src1);
	w2 = gdk_pixbuf_get_width (src2);
	h2 = gdk_pixbuf_get_height (src2);

	do_scale = ((float)w1 * 0.8) < w2;

	/* scale the emblem down if it will obscure the entire bottom image */
	if (do_scale)
		scaled = gdk_pixbuf_scale_simple (src2, w1 / 2, h1 / 2, GDK_INTERP_BILINEAR);
	else
		scaled = (GdkPixbuf *)src2;

	w2 = gdk_pixbuf_get_width (scaled);
	h2 = gdk_pixbuf_get_height (scaled);

	dest_x = w1 - w2;
	dest_y = h1 - h2;

	gdk_pixbuf_composite (scaled, dest,
			      dest_x, dest_y,
			      w2, h2,
			      dest_x, dest_y,
			      1.0, 1.0,
			      GDK_INTERP_BILINEAR, 0xFF);

	if (do_scale)
		g_object_unref (scaled);

	return dest;
}

static GHashTable *icons_table = NULL;

#define GMAMEUI_PIXBUF_FOR_ID(x) (g_hash_table_lookup (icons_table, x))
#define GMAMEUI_ICON_SIZE_MENU    "menu"
#define GMAMEUI_ICON_SIZE_BUTTON  "button"
#define GMAMEUI_ICON_SIZE_TOOLBAR "toolbar"
#define GMAMEUI_ICON_SIZE_FOLDER  "toolbar"
#define GMAMEUI_ICON_SIZE_EMBLEM  "toolbar"
#define GMAMEUI_ICON_SIZE_DIALOG  "dialog"

static void
add_composite_icon_from_stock (const char *new_id,
			       const char *id1,
			       const char *id2)
{
	GdkPixbuf *bottom_pixbuf;
	GdkPixbuf *top_pixbuf;
	GdkPixbuf *pixbuf;

	bottom_pixbuf = gmameui_get_icon_from_stock (id1);
	top_pixbuf = gmameui_get_icon_from_stock (id2);

	pixbuf = composite_icons (bottom_pixbuf, top_pixbuf);
	g_hash_table_insert (icons_table, (gpointer) new_id, (gpointer) pixbuf);

	if (top_pixbuf)
		g_object_unref (top_pixbuf);
	if (bottom_pixbuf)
		g_object_unref (bottom_pixbuf);
}

static void
add_composite_icons (void)
{
	add_composite_icon_from_stock ("gmameui-folder-favorite", "gmameui-folder", "gmameui-emblem-favorite");
	add_composite_icon_from_stock ("gmameui-folder-sound", "gmameui-folder", "gmameui-emblem-sound");
	add_composite_icon_from_stock ("gmameui-folder-available", "gmameui-folder", "gmameui-emblem-correct");
	add_composite_icon_from_stock ("gmameui-folder-unavailable", "gmameui-folder", "gmameui-emblem-unavailable");
    	add_composite_icon_from_stock ("gmameui-folder-incorrect", "gmameui-folder", "gmameui-emblem-incorrect");
	add_composite_icon_from_stock ("gmameui-folder-played", "gmameui-folder", "gmameui-emblem-played");
    	add_composite_icon_from_stock ("gmameui-folder-working", "gmameui-folder", "gmameui-emblem-correct");
	add_composite_icon_from_stock ("gmameui-folder-not-working", "gmameui-folder", "gmameui-emblem-not-working");
	add_composite_icon_from_stock ("gmameui-folder-date", "gmameui-folder", "gmameui-emblem-date");
	add_composite_icon_from_stock ("gmameui-folder-manufacturer", "gmameui-folder", "gmameui-emblem-manufacturer");

	add_composite_icon_from_stock ("gmameui-rom-correct", "gmameui-rom", "gmameui-emblem-correct");
	add_composite_icon_from_stock ("gmameui-rom-unavailable", "gmameui-rom", "gmameui-emblem-unavailable");
	add_composite_icon_from_stock ("gmameui-rom-incorrect", "gmameui-rom", "gmameui-emblem-incorrect");
	add_composite_icon_from_stock ("gmameui-rom-problems", "gmameui-rom", "gmameui-emblem-not-working");
	add_composite_icon_from_stock ("gmameui-rom-unknown", "gmameui-rom", "gmameui-emblem-unknown");
}

void
gmameui_icons_init (void)
{
	GtkIconTheme *theme;
	int i;
	static char *items[][4] = {
		/* { gmameui-stock-id, size-string, theme-stock-id, fallback-stock-id } */
		{ "gmameui-view-list", GMAMEUI_ICON_SIZE_MENU, "stock_list_enum-off", "gmameui-view-list" },
		{ "gmameui-view-tree", GMAMEUI_ICON_SIZE_MENU, "stock_navigator-levels", "gmameui-view-tree" },
		{ "gmameui-view-folders", GMAMEUI_ICON_SIZE_MENU, "stock_toggle-preview", "gmameui-view-folders" },
		{ "gmameui-view-screenshot", GMAMEUI_ICON_SIZE_MENU, "stock_toggle-preview", "gmameui-view-screenshot" },
		{ "gmameui-general-toolbar", GMAMEUI_ICON_SIZE_TOOLBAR, "gmameui-general", NULL },
		{ "gmameui-sound-toolbar", GMAMEUI_ICON_SIZE_TOOLBAR, "stock_volume", "gmameui-sound" },
		{ "gmameui-display-toolbar", GMAMEUI_ICON_SIZE_TOOLBAR, "display", "gmameui-sound" },
		{ "gmameui-joystick-toolbar", GMAMEUI_ICON_SIZE_TOOLBAR, "gnome-joystick", "gmameui-joystick" },
		{ "gmameui-joystick", GMAMEUI_ICON_SIZE_DIALOG, "gnome-joystick", "gmameui-joystick" },
		{ "gmameui-mouse", GMAMEUI_ICON_SIZE_DIALOG, "gnome-dev-mouse-optical", "gmameui-mouse" },
		{ "gmameui-keyboard", GMAMEUI_ICON_SIZE_DIALOG, "gnome-dev-keyboard", "gmameui-keyboard" },
		{ "gmameui-folder", GMAMEUI_ICON_SIZE_FOLDER, "stock_folder", "gmameui-folder" },
		{ "gmameui-folder-open", GMAMEUI_ICON_SIZE_FOLDER, "stock_open", "gmameui-folder-open" },
		{ "gmameui-rom", GMAMEUI_ICON_SIZE_FOLDER, "gnome-dev-media-ms", "gmameui-rom" },
		{ "gmameui-emblem-favorite", GMAMEUI_ICON_SIZE_EMBLEM, "emblem-favorite", "gmameui-emblem-favorite" },
		{ "gmameui-emblem-played", GMAMEUI_ICON_SIZE_EMBLEM, "emblem-distinguished", "gmameui-emblem-played" },
		{ "gmameui-emblem-correct", GMAMEUI_ICON_SIZE_EMBLEM, "emblem-default", "gmameui-emblem-correct" },
		{ "gmameui-emblem-incorrect", GMAMEUI_ICON_SIZE_EMBLEM, "emblem-urgent", "gmameui-emblem-incorrect" },
		{ "gmameui-emblem-unavailable", GMAMEUI_ICON_SIZE_EMBLEM, "emblem-noread", "gmameui-emblem-unavailable" },
		{ "gmameui-emblem-sound", GMAMEUI_ICON_SIZE_EMBLEM, "emblem-sound", "gmameui-emblem-sound" },
		{ "gmameui-emblem-not-working", GMAMEUI_ICON_SIZE_EMBLEM, "emblem-important", "gmameui-emblem-not-working" },
		{ "gmameui-emblem-date", GMAMEUI_ICON_SIZE_EMBLEM, "stock_calendar-view-year", "gmameui-emblem-date" },
		{ "gmameui-emblem-unknown", GMAMEUI_ICON_SIZE_EMBLEM, "stock_unknown", "gmameui-emblem-unknown" },
		{ "gmameui-emblem-manufacturer", GMAMEUI_ICON_SIZE_EMBLEM, "gnome-run", "gmameui-emblem-menufacturer" },
	};

	theme = gtk_icon_theme_get_default ();

	if (icons_table == NULL) {
		icons_table = g_hash_table_new (g_str_hash, g_str_equal);
	}

	for (i = 0; i < (int) G_N_ELEMENTS (items); i++) {
		GdkPixbuf *pixbuf = NULL;
		GtkIconSize size;
		int num;
		int j;

		num = G_N_ELEMENTS (items[i]);

		if (strcmp (items[i][1], "menu") == 0)
			size = GTK_ICON_SIZE_MENU;
		else if (strcmp (items[i][1], "toolbar") == 0)
			size = GTK_ICON_SIZE_LARGE_TOOLBAR;
		else if (strcmp (items[i][1], "emblem") == 0)
			size = GTK_ICON_SIZE_LARGE_TOOLBAR;
		else if (strcmp (items[i][1], "button") == 0)
			size = GTK_ICON_SIZE_BUTTON;
		else
			size = GTK_ICON_SIZE_DIALOG;

		for (j = 2; j < num && items[i][j]; j++) {
			pixbuf = gmameui_get_icon_from_theme (items[i][j], size);
			if (pixbuf)
				break;
		}

		if (pixbuf == NULL) {
			for (j = 2; j < num && items[i][j]; j++) {
				pixbuf = gmameui_get_pixbuf_from_gmameui_install (items[i][j]);
				if (pixbuf)
					break;
			}

		}

		if (pixbuf == NULL) {
			g_warning ("Couldn't find themed icon for \"%s\"",
				   items[i][0]);
			continue;
		}

		/* The pixbuf will be unref'ed when we destroy the
		 * hash table */
		g_hash_table_insert (icons_table, (gpointer) items[i][0], (gpointer) pixbuf);
	}

	/* add special case for screenshot */
	{
		GdkPixbuf *pixbuf;
		pixbuf = gmameui_get_pixbuf_from_gmameui_install ("gmameui-screen");
		g_hash_table_insert (icons_table, (gpointer) "gmameui-screen", (gpointer) pixbuf);
	}	

	/* build the composite icons */
	add_composite_icons ();
}

GdkPixbuf *
gmameui_get_icon_from_stock (const char *id)
{
	GdkPixbuf *pixbuf = NULL;

	pixbuf = GMAMEUI_PIXBUF_FOR_ID (id);
	if (pixbuf)
		g_object_ref (pixbuf);
	
	return pixbuf;
}

GtkWidget *
gmameui_get_image_from_stock (const char *id)
{
	GtkWidget *image = NULL;
	GdkPixbuf *pixbuf;

	pixbuf = gmameui_get_icon_from_stock (id);

	if (pixbuf) {
		image = gtk_image_new_from_pixbuf (pixbuf);
		g_object_unref (pixbuf);
	} else {
		image = gtk_image_new_from_stock (GTK_STOCK_MISSING_IMAGE, GTK_ICON_SIZE_BUTTON);
	}
	
	return image;
}

/* this function checks if an icon update is needed and get the icon */
void
get_status_icons (void)
{
	if (dirty_icon_cache
	    || !Status_Icons [CORRECT]
	    || !Status_Icons [INCORRECT]
	    || !Status_Icons [UNKNOWN]
	    || !Status_Icons [PROBLEMS]
	    || !Status_Icons [NOT_AVAIL]) {
		if (dirty_icon_cache) {
			if (Status_Icons [CORRECT]) {
				g_object_unref (Status_Icons [CORRECT]);
				Status_Icons [CORRECT] = NULL;
			}
			if (Status_Icons [INCORRECT]) {
				g_object_unref (Status_Icons [INCORRECT]);
				Status_Icons [INCORRECT] = NULL;
			}
		    	if (Status_Icons[UNKNOWN]) {
				g_object_unref (Status_Icons [UNKNOWN]);
				Status_Icons [UNKNOWN] = NULL;
			}
			if (Status_Icons [PROBLEMS]) {
				g_object_unref (Status_Icons [PROBLEMS]);
				Status_Icons [PROBLEMS] = NULL;
			}
			if (Status_Icons [BEST_AVAIL]) {
				g_object_unref (Status_Icons [BEST_AVAIL]);
				Status_Icons [BEST_AVAIL] = NULL;
			}
			if (Status_Icons[NOT_AVAIL]) {
				g_object_unref (Status_Icons [NOT_AVAIL]);
				Status_Icons [NOT_AVAIL] = NULL;
			}
		}

	 	if (Status_Icons [CORRECT] == NULL)
			Status_Icons [CORRECT] = gmameui_get_icon_from_stock ("gmameui-rom-correct");
	 	if (Status_Icons [INCORRECT] == NULL)
			Status_Icons [INCORRECT] = gmameui_get_icon_from_stock ("gmameui-rom-incorrect");
	 	if (Status_Icons [UNKNOWN] == NULL)
			Status_Icons [UNKNOWN] =  gmameui_get_icon_from_stock ("gmameui-rom-unknown");
	 	if (Status_Icons [PROBLEMS] == NULL)
			Status_Icons [PROBLEMS] = gmameui_get_icon_from_stock ("gmameui-rom-problems");
	 	if (Status_Icons [BEST_AVAIL] == NULL)
			Status_Icons [BEST_AVAIL] = gmameui_get_icon_from_stock ("gmameui-rom-problems");
		if (Status_Icons [NOT_AVAIL] == NULL)
			Status_Icons [NOT_AVAIL] = gmameui_get_icon_from_stock ("gmameui-rom-unavailable");
	}
}

/* FIXME TODO Invoke this function via a signal handler, rather than calling
   directly */
void
select_game (MameRomEntry *rom)
{
	gui_prefs.current_game = rom;

	if (rom != NULL) {
		/* update screenshot panel */
		gmameui_sidebar_set_with_rom (GMAMEUI_SIDEBAR (main_gui.screenshot_hist_frame),
					      rom);
	} else {
		/* no roms selected display the default picture */

		GMAMEUI_DEBUG ("no games selected");
		
		/* update screenshot panel */
		gmameui_sidebar_set_with_rom (GMAMEUI_SIDEBAR (main_gui.screenshot_hist_frame),
					      NULL);
	}

	gmameui_ui_set_items_sensitive ();
}

/* FIXME TODO Move to callbacks.c */
void
select_inp (gboolean play_record)
{
	GtkWidget *inp_selection;
	gchar *inp_dir;
	gchar *current_rom_name;
	MameRomEntry *rom;
	
	g_return_if_fail (mame_exec_list_has_current_executable (main_gui.exec_list));
	
	joy_focus_off ();
	
	rom = gui_prefs.current_game;
	
	g_object_get (main_gui.gui_prefs,
		      "dir-inp", &inp_dir,
		      "current-rom", &current_rom_name,
		      NULL);
	
	if (play_record) {
		inp_selection = gtk_file_chooser_dialog_new (_("Choose inp file to play"),
							     GTK_WINDOW (MainWindow),
							     GTK_FILE_CHOOSER_ACTION_OPEN,
							     GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
							     GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
							     NULL);
		gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (inp_selection), inp_dir);
		
		if (gtk_dialog_run (GTK_DIALOG (inp_selection)) == GTK_RESPONSE_ACCEPT) {
			gtk_widget_hide (inp_selection);
			process_inp_function (rom,
					      gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (inp_selection)),
					      0);
		}
		gtk_widget_destroy (inp_selection);
	} else {
		gchar *inp_filename;
		gchar *tmp;
		tmp = g_strdup_printf("%s.inp", current_rom_name);
		inp_filename = g_build_filename (inp_dir, tmp, NULL);
		GMAMEUI_DEBUG ("Recording new inp file to %s", inp_filename);
		
		if (g_file_test (inp_filename, G_FILE_TEST_EXISTS)) {
			GtkWidget *dialog;

			dialog = gtk_message_dialog_new (GTK_WINDOW (MainWindow),
							 GTK_DIALOG_MODAL,
							 GTK_MESSAGE_WARNING,
							 GTK_BUTTONS_YES_NO,
							 _("File exists"));
			gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (dialog),
								  _("A file named '%s' already exists. Do you want to overwrite it?"),
								  inp_filename);
			if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_YES) {
				gtk_widget_hide (dialog);
				process_inp_function (rom, inp_filename, 1);
			}
			gtk_widget_destroy (dialog);
		} else 
			process_inp_function (rom, inp_filename, 1);
		
		g_free (tmp);
		g_free (inp_filename);	
	}
	
	joy_focus_on ();
	
	g_free (inp_dir);
	g_free (current_rom_name);
}
