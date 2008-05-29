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

/* Prepare the commandline to use to play a game */

#include "common.h"
#include <stdlib.h>
#include <string.h>

#include <gtk/gtkentry.h>
#include <gtk/gtklabel.h>
#include <gtk/gtkradiobutton.h>
#include <gtk/gtkspinbutton.h>
#include <gtk/gtkstock.h>
#include <gtk/gtktable.h>

#include "network_game.h"
#include "gmameui.h"
#include "options_string.h"
#include "io.h"
#include "gui.h"


struct network_game_options {
	gboolean server_mode;
	int port;               /* Bind port. 0 = no binding */
	gboolean parallelsync;  /* Perform network input sync in advance:
	                           Causes ~16 ms input delay but more suitable
	                           for relatively slow machines */
	gboolean statedebug;    /* Check complete machine state against slaves
                              at each frame -- extremely slow,
                              for debugging (master only) */
	gboolean netmapkey;     /* When enabled all players use the player 1 keys.
	                           For use with *real* multiplayer games.
                               Makes player 1 keys control whichever player number
                               you're actually assigned by the master
                               (slave only) */
	int no_players;
	const gchar *server;    /* Set master hostname */
};

static void
button_toggled      (GtkObject       *checkbutton,
                     gpointer         user_data)
{
	gtk_widget_set_sensitive(GTK_WIDGET(user_data),gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton)));
}

static gchar *create_network_options_string(XmameExecutable *exec, const struct network_game_options *options) {

	gchar *parallelsync_string=NULL;
	gchar *bind = NULL;
	gchar *network = NULL;	
	
	parallelsync_string = xmame_get_boolean_option_string(exec, "parallelsync", options->parallelsync);

	if (options->port)
		bind = xmame_get_int_option_string(exec, "bind", options->port);

	if(options->server_mode) {	/* master */
		gchar *master;
		gchar *statedebug_string;

		master = xmame_get_int_option_string(exec, "master", options->no_players);

		if (!master)
			return NULL;

		statedebug_string = xmame_get_boolean_option_string(exec, "statedebug", options->statedebug);

		network = g_strdup_printf ("%s "
						"%s "		/* statedebug */
						"%s "		/* parallelsync */
						"%s",		/* bind */
						master,
						statedebug_string?statedebug_string:"",
						parallelsync_string?parallelsync_string:"",
						bind?bind:""
						);

		g_free(master);
		g_free(statedebug_string);
	}
	else {	/* slave */
			gchar *slave;
			gchar *netmapkey_string;

			slave = xmame_get_option_string(exec, "slave", options->server);

			if (!slave)
				return NULL;

			netmapkey_string = xmame_get_boolean_option_string(exec, "netmapkey", options->netmapkey);

			network = g_strdup_printf ("%s "
						"%s "
						"%s "		/* parallelsync */
						"%s",		/* bind */
						slave?slave:"",
						netmapkey_string?netmapkey_string:"",
						parallelsync_string?parallelsync_string:"",
						bind?bind:""
						);

			g_free(slave);
			g_free(netmapkey_string);
	}
	
	g_free(parallelsync_string);
	g_free(bind);

	return network;
}


static void play_network_game(RomEntry *rom, const struct network_game_options *net_options)
{
/* This code is commented out since networking is not supported by
   recent versions of MAME
	gchar *opt;
	gchar *general_options;
	gchar *vector_options;
	gchar *network_options;
	GameOptions *target;

	if (!rom)
		return;

	if (!current_exec)
	{
		gmameui_message(ERROR, NULL, _("No xmame executables defined"));
		return;
	}

	if (gui_prefs.use_xmame_options)
	{
		opt = g_strdup_printf("%s %s 2>&1", current_exec->path, rom->romname);
		launch_emulation(rom, opt);
		g_free(opt);
		return;
	}

	target = load_options(rom);
		
	if (!target)
		target = &default_options;
	
	* prepares options*
	general_options = create_options_string(current_exec, target);
	network_options = create_network_options_string(current_exec, net_options);
	
	if (rom->vector)
		vector_options = create_vector_options_string(current_exec, target);
	else
		vector_options = g_strdup("");
	

	game_options_free(target);
	
	* create the command *
	opt=g_strdup_printf("%s %s %s %s -noloadconfig %s 2>&1",
			current_exec->path,
			general_options,
			network_options,
			vector_options,
			rom->romname);

	*free options*
	g_free(general_options);
	g_free(vector_options);
	g_free(network_options);	

	launch_emulation(rom, opt);
	g_free(opt);*/
}

static void on_ok_click (GtkObject *checkbutton,
 			gpointer         user_data) {

	struct network_game_options *game_options;
	GObject *options_win;
	GtkWidget *widget;
	RomEntry *rom;

	game_options = (struct network_game_options *) malloc(sizeof(struct network_game_options));

	options_win = G_OBJECT(user_data);	


	rom = g_object_get_data(options_win, "rom");

	/* Server mode */
	widget = g_object_get_data(options_win, "server_mode_radiobutton");
	game_options->server_mode = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget));

	widget = g_object_get_data(options_win, "parallelsync_checkbutton");
	game_options->parallelsync = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget));

	/* Port */
	game_options->port = 0;

	widget = g_object_get_data(options_win, "bind_checkbutton");

	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(widget))) {
		widget = g_object_get_data(options_win, "bind_spinbutton");
		game_options->port = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
	}

	if (game_options->server_mode) {
		widget = g_object_get_data(options_win, "master_spinbutton");
		game_options->no_players = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));

		widget = g_object_get_data(options_win, "statedebug_checkbutton");
		game_options->statedebug = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget));
	} else {
		widget = g_object_get_data(options_win, "slave_entry");
		game_options->server = g_strdup(gtk_entry_get_text(GTK_ENTRY(widget)));

		widget = g_object_get_data(options_win, "netmapkey_checkbutton");
		game_options->netmapkey = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget));
	}

	gtk_widget_destroy(GTK_WIDGET(user_data));
	play_network_game(rom, game_options);
	
}

static GtkWidget* create_network_options_window(RomEntry *rom)
{
	GtkWidget *network_window;
	GtkWidget *network_table;
	GtkWidget *network_mode_label;
	GtkWidget *slave_label;
	GtkWidget *master_label;
	GtkWidget *ok_button;
	GtkWidget *cancel_button;
	GtkWidget *buttons_hbox;
	GtkWidget *netmapkey_checkbutton;
	GtkWidget *parallelsync_checkbutton;
	GtkWidget *client_mode_radiobutton;
	GtkWidget *server_mode_radiobutton;
	GtkWidget *slave_entry;
	GtkWidget *master_spinbutton;
	GtkWidget *bind_checkbutton;
	GtkWidget *bind_spinbutton;
	GtkWidget *statedebug_checkbutton;
	GtkObject *master_spinbutton_adj;
	GtkObject *bind_spinbutton_adj;
	GSList *net_mode_group = NULL;
	
	network_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

	g_object_set_data (G_OBJECT (network_window), "network_window", network_window);
	gtk_window_set_title (GTK_WINDOW (network_window), _("Network game options"));
	gtk_window_set_position (GTK_WINDOW (network_window), GTK_WIN_POS_MOUSE);
	gtk_window_set_transient_for(GTK_WINDOW(network_window),GTK_WINDOW(MainWindow));
	gtk_window_set_modal(GTK_WINDOW(network_window),TRUE);
	
	network_table = gtk_table_new (10, 3, FALSE);
	gtk_widget_ref (network_table);
	g_object_set_data_full (G_OBJECT (network_window), "network_table", network_table,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (network_table);
	gtk_container_add (GTK_CONTAINER (network_window), network_table);

	g_object_set_data(G_OBJECT(network_window), "rom", (gpointer) rom);

	/* network mode */
	network_mode_label = gtk_label_new (_("Network Mode"));
	gtk_widget_ref (network_mode_label);
	g_object_set_data_full (G_OBJECT (network_window), "network_mode_label", network_mode_label,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (network_mode_label);
	gtk_table_attach (GTK_TABLE (network_table), network_mode_label, 0, 1, 0, 1,
			  (GtkAttachOptions) (0),
			  (GtkAttachOptions) (GTK_FILL), 0, 0);


	/* client */
	client_mode_radiobutton = gtk_radio_button_new_with_label (net_mode_group, _("Client"));
	gtk_widget_ref (client_mode_radiobutton);
	g_object_set_data_full (G_OBJECT (network_window), "client_mode_radiobutton", client_mode_radiobutton,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (client_mode_radiobutton);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (client_mode_radiobutton), FALSE);
	gtk_table_attach (GTK_TABLE (network_table), client_mode_radiobutton, 1, 2, 0, 1,
			  (GtkAttachOptions) (GTK_FILL),
			  (GtkAttachOptions) (GTK_FILL), 0, 0);

	/* server */
	server_mode_radiobutton = gtk_radio_button_new_with_label (
                               gtk_radio_button_get_group (GTK_RADIO_BUTTON(client_mode_radiobutton))
	                          , _("Server"));
	gtk_widget_ref (server_mode_radiobutton);
	g_object_set_data_full (G_OBJECT (network_window), "server_mode_radiobutton", server_mode_radiobutton,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (server_mode_radiobutton);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (server_mode_radiobutton), TRUE);
	gtk_table_attach (GTK_TABLE (network_table), server_mode_radiobutton, 2, 3, 0, 1,
			  (GtkAttachOptions) (GTK_FILL),
			  (GtkAttachOptions) (0), 0, 0);
	

	/* Server host:port */
	slave_label = gtk_label_new (_("Host[:Port] : "));
	gtk_widget_ref (slave_label);
	g_object_set_data_full (G_OBJECT (network_window), "slave_label", slave_label,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (slave_label);
	gtk_table_attach (GTK_TABLE (network_table), slave_label, 0, 1, 1, 2,
			  (GtkAttachOptions) (GTK_FILL),
			  (GtkAttachOptions) (0), 0, 0);

	slave_entry = gtk_entry_new ();
	gtk_entry_set_max_length (GTK_ENTRY(slave_entry), 20);
	gtk_widget_ref (slave_entry);
	g_object_set_data_full (G_OBJECT (network_window), "slave_entry", slave_entry,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (slave_entry);
	gtk_table_attach (GTK_TABLE (network_table), slave_entry, 1, 3, 1, 2,
			  (GtkAttachOptions) (GTK_FILL),
			  (GtkAttachOptions) (0), 0, 0);

	/* Number of players */
	master_label = gtk_label_new (_("Number of players : "));
	gtk_widget_ref (master_label);
	g_object_set_data_full (G_OBJECT (network_window), "master_label", master_label,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (master_label);
	gtk_table_attach (GTK_TABLE (network_table), master_label, 0, 1, 2, 3,
			  (GtkAttachOptions) (GTK_FILL),
			  (GtkAttachOptions) (0), 0, 0);

	master_spinbutton_adj = gtk_adjustment_new (0, 2, 100, 1, 10, 10);
	master_spinbutton = gtk_spin_button_new (GTK_ADJUSTMENT (master_spinbutton_adj), 1, 0);
	gtk_widget_ref (master_spinbutton);
	g_object_set_data_full (G_OBJECT (network_window), "master_spinbutton", master_spinbutton,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (master_spinbutton);
	gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (master_spinbutton), TRUE);
	gtk_table_attach (GTK_TABLE (network_table), master_spinbutton, 1, 2, 2, 3,
			  (GtkAttachOptions) (GTK_FILL),
			  (GtkAttachOptions) (0), 0, 0);

	/* netmap key */
	netmapkey_checkbutton = gtk_check_button_new_with_label (_("Use NetMapKey"));
	gtk_widget_ref (netmapkey_checkbutton);
	g_object_set_data_full (G_OBJECT (network_window), "netmapkey_checkbutton", netmapkey_checkbutton,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (netmapkey_checkbutton);
	gtk_table_attach (GTK_TABLE (network_table), netmapkey_checkbutton, 0, 3, 3, 4,
			  (GtkAttachOptions) (GTK_FILL),
			  (GtkAttachOptions) (0), 0, 0);

	/* statedebug */
	statedebug_checkbutton = gtk_check_button_new_with_label (_("Statedebug"));
	gtk_widget_ref (statedebug_checkbutton);
	g_object_set_data_full (G_OBJECT (network_window), "statedebug_checkbutton", statedebug_checkbutton,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (statedebug_checkbutton);
	gtk_table_attach (GTK_TABLE (network_table), statedebug_checkbutton, 0, 3, 4, 5,
			  (GtkAttachOptions) (GTK_FILL),
			  (GtkAttachOptions) (0), 0, 0);

	/* bind */
	bind_checkbutton = gtk_check_button_new_with_label (_("Bind port:"));
	gtk_widget_ref (bind_checkbutton);
	g_object_set_data_full (G_OBJECT (network_window), "bind_checkbutton", bind_checkbutton,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (bind_checkbutton);
	gtk_table_attach (GTK_TABLE (network_table), bind_checkbutton, 0, 1, 5, 6,
			  (GtkAttachOptions) (GTK_FILL),
			  (GtkAttachOptions) (0), 0, 0);

	bind_spinbutton_adj = gtk_adjustment_new (0, 1, 65535, 1, 10, 10);
	bind_spinbutton =  gtk_spin_button_new (GTK_ADJUSTMENT (bind_spinbutton_adj), 1, 0);
	gtk_widget_ref (bind_spinbutton);
	g_object_set_data_full (G_OBJECT (network_window), "bind_spinbutton", bind_spinbutton,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (bind_spinbutton);
	gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (bind_spinbutton), TRUE);
	gtk_table_attach (GTK_TABLE (network_table), bind_spinbutton, 1, 3, 5, 6,
			  (GtkAttachOptions) (GTK_FILL),
			  (GtkAttachOptions) (0), 0, 0);

	/* parallelsync */
	parallelsync_checkbutton = gtk_check_button_new_with_label (_("Perform network input sync in advance"));
	gtk_widget_ref (parallelsync_checkbutton);
	g_object_set_data_full (G_OBJECT (network_window), "parallelsync_checkbutton", parallelsync_checkbutton,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (parallelsync_checkbutton);
	gtk_table_attach (GTK_TABLE (network_table), parallelsync_checkbutton, 0, 3, 6, 7,
			  (GtkAttachOptions) (GTK_FILL),
			  (GtkAttachOptions) (0), 0, 0);


	/* buttons */
	buttons_hbox= gtk_hbox_new (FALSE, 0);
	gtk_widget_ref (buttons_hbox);
	g_object_set_data_full (G_OBJECT (network_window), "buttons_hbox", buttons_hbox,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (buttons_hbox);
	gtk_table_attach (GTK_TABLE (network_table), buttons_hbox, 0, 4, 7, 8,
			  (GtkAttachOptions) (GTK_FILL),
			  (GtkAttachOptions) (0), 0, 0);

	gtk_container_set_border_width (GTK_CONTAINER (buttons_hbox), 5);

	cancel_button = gtk_button_new_from_stock (GTK_STOCK_CANCEL);
	gtk_widget_ref (cancel_button);
	g_object_set_data_full (G_OBJECT (network_window), "cancel_button", cancel_button,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (cancel_button);
	
	ok_button = gtk_button_new_from_stock (GTK_STOCK_OK);
	gtk_widget_ref (ok_button);
	g_object_set_data_full (G_OBJECT (network_window), "ok_button", ok_button,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (ok_button);
	
	gtk_box_set_homogeneous(GTK_BOX (buttons_hbox), TRUE);
	gtk_box_pack_start_defaults(GTK_BOX (buttons_hbox), ok_button);
	gtk_box_pack_start_defaults(GTK_BOX (buttons_hbox), cancel_button);


	gtk_widget_set_sensitive(netmapkey_checkbutton,
		xmame_has_option(current_exec, "netmapkey"));

	gtk_widget_set_sensitive(bind_checkbutton,
		xmame_has_option(current_exec, "bind"));

	gtk_widget_set_sensitive(bind_spinbutton,
		xmame_has_option(current_exec, "bind"));

	gtk_widget_set_sensitive(statedebug_checkbutton,
		xmame_has_option(current_exec, "statedebug"));

	gtk_widget_set_sensitive(parallelsync_checkbutton,
		xmame_has_option(current_exec, "parallelsync"));


	g_signal_connect_after (G_OBJECT (server_mode_radiobutton), "toggled",
			    	  G_CALLBACK (button_toggled),
			    	  master_label);
	g_signal_connect_after (G_OBJECT (server_mode_radiobutton), "toggled",
			    	  G_CALLBACK (button_toggled),
			    	  master_spinbutton);

	if(xmame_has_option(current_exec, "statedebug"))
	{
		g_signal_connect_after (G_OBJECT (server_mode_radiobutton), "toggled",
			    	  G_CALLBACK (button_toggled),
			    	  statedebug_checkbutton);
	}

	g_signal_connect_after (G_OBJECT (client_mode_radiobutton), "toggled",
			    	  G_CALLBACK (button_toggled),
			    	  slave_label);

	g_signal_connect_after (G_OBJECT (client_mode_radiobutton), "toggled",
			    	  G_CALLBACK (button_toggled),
			    	  slave_entry);

	g_signal_connect_after (G_OBJECT (client_mode_radiobutton), "toggled",
			    	  G_CALLBACK (button_toggled),
			    	  netmapkey_checkbutton);

	g_signal_connect_after (G_OBJECT (bind_checkbutton), "toggled",
			    	  G_CALLBACK (button_toggled),
			    	  bind_spinbutton);

	g_signal_connect (G_OBJECT (ok_button), "clicked",
			           G_CALLBACK (on_ok_click),
			           G_OBJECT (network_window));

	g_signal_connect_swapped (G_OBJECT (cancel_button), "clicked",
			           G_CALLBACK (gtk_widget_destroy),
			           G_OBJECT (network_window));

	g_signal_connect_swapped (G_OBJECT (network_window), "delete-event",
			           G_CALLBACK (gtk_widget_destroy),
			           G_OBJECT (network_window));

	return network_window;
}

void on_network_play_activate  (GtkMenuItem     *menuitem,
                               gpointer         user_data)
{
	GtkWidget *network_window;
	RomEntry *rom;
/* FIXME TODO
	rom = gui_prefs.current_game;

	xmame_get_options(current_exec);

	if (xmame_has_option(current_exec, "master") || xmame_has_option(current_exec, "slave")) {
		network_window = create_network_options_window(rom);
		gtk_widget_show(network_window);
	} else {
		gmameui_message(ERROR, NULL, "Your xmame does not support network games.");
	}*/
}
