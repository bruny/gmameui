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

#ifndef __GUI_H__
#define __GUI_H__

#include "common.h"

#include <gtk/gtkcheckmenuitem.h>
#include <gtk/gtkframe.h>
#include <gtk/gtkmenu.h>
#include <gtk/gtkpaned.h>
#include <gtk/gtkprogressbar.h>
#include <gtk/gtkstatusbar.h>
#include <gtk/gtktextbuffer.h>
#include <gtk/gtktogglebutton.h>
#include <gtk/gtktoolbar.h>

#include "gmameui.h"
#include "mame_options.h"
#include "mame_options_legacy.h"
#include "filters_list.h"
#include "gui_prefs.h"
#include "gmameui-sidebar.h"
#include "gmameui-gamelist-view.h"
#include "mame-exec-list.h"
#include "gmameui-gamelist-view.h"
#include "gmameui-statusbar.h"

GtkWidget *MainWindow;

/* FIXME TODO Move these to the priv and have functions to request from the main window */
struct main_gui_struct {

	GtkWidget *toolbar;

	GtkWidget *combo_progress_bar;
	GtkStatusbar *status_progress_bar;
	GtkProgressBar *progress_progress_bar;

	GMAMEUIStatusbar *statusbar;

	GtkPaned *hpanedLeft;
	GtkPaned *hpanedRight;

	GtkWidget *scrolled_window_filters;
	GMAMEUIFiltersList *filters_list;

	GtkWidget        *scrolled_window_games;
	MameGamelistView *displayed_list;           /* The GtkTreeView displaying the ROMs */
	GtkWidget        *search_entry;             /* GtkEntry used for searching tree view */

	GMAMEUISidebar *screenshot_hist_frame;

	GtkWidget *executable_menu;
	
	GtkUIManager *manager;
	GtkActionGroup *gmameui_rom_action_group;   /* Item entries that require a ROM */
	GtkActionGroup *gmameui_rom_exec_action_group;  /* Item entries that require both a ROM and an exec */
	GtkActionGroup *gmameui_exec_action_group;  /* Item entries that require an exec */
	GtkActionGroup *gmameui_favourite_action_group;

	GtkActionGroup *gmameui_exec_radio_action_group;	/* Executable radio buttons */
	gint gmameui_exec_merge_id;
	
	MameOptions *options;
	MameOptionsLegacy *legacy_options;
	
	MameGuiPrefs *gui_prefs;
	
	MameExecList *exec_list;
};

struct main_gui_struct main_gui;

/* New icon code */
void gmameui_icons_init (void);
GdkPixbuf * gmameui_get_icon_from_stock (const char *);
GtkWidget * gmameui_get_image_from_stock (const char *);

void add_exec_menu(void);
int init_gui(void);

GdkPixbuf * get_icon_for_rom (MameRomEntry *rom, guint size, gchar *icon_dir,
							  gchar *zipfilename, gboolean usecustomicons);
GdkPixbuf * gmameui_get_icon_from_stock (const char *id);
GtkWidget * gmameui_get_image_from_stock (const char *id);
void get_status_icons (void);
void gmameui_icons_init (void);

void select_inp (gboolean play_record);
void select_game (MameRomEntry *rom);

#endif /* __GUI_H__ */
