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
#include <stdlib.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtkcheckbutton.h>
#include <gtk/gtkcombo.h>
#include <gtk/gtkhscale.h>
#include <gtk/gtkhseparator.h>
#include <gtk/gtklabel.h>
#include <gtk/gtkrange.h>
#include <gtk/gtkscale.h>
#include <gtk/gtkspinbutton.h>
#include <gtk/gtktable.h>
#include <gtk/gtkvbox.h>
#include <gtk/gtkvseparator.h>

#include <glade/glade.h>

#include "gmameui.h"
#include "options.h"
#include "io.h"
#include "callbacks.h"
#include "interface.h"
#include "gui.h"
#include "keyboard.h"

#define BUFFER_SIZE 1000

static gboolean dirty_options_flag;

/* display */
static GtkWidget *frameskip_combo;
static GtkWidget *frameskipper_combo;
static GtkWidget *maxautoframeskip_combo;
static GtkWidget *effect_combo;
static GtkWidget *effect_name;
static GtkWidget *arbheight_entry;
static GtkWidget *arbheight_checkbutton;
static GtkWidget *heightscale_spinbutton;
static GtkObject *heightscale_spinbutton_adj;
static GtkWidget *widthscale_spinbutton;
static GtkObject *widthscale_spinbutton_adj;
static GtkWidget *autodouble_checkbutton;
static GtkWidget *dirty_checkbutton;
static GtkWidget *scanlines_checkbutton;
static GtkWidget *throttle_checkbutton;
static GtkWidget *sleepidle_checkbutton;
static GtkWidget *brightness_hscale;
static GtkWidget *bpp_combo;
static GtkWidget *gamma_hscale;
static GtkWidget *rotation_combo;
static GtkWidget *norotate_checkbutton;
static GtkWidget *flipx_checkbutton;
static GtkWidget *flipy_checkbutton;
static GtkWidget *autoframeskip_checkbutton;
static GtkWidget *fts_label;
static GtkWidget *fs_method_label;
static GtkWidget *maxfs_label;
/* misc */
static GtkWidget *artwork_checkbutton;
/* Renderer */
static GtkObject *gamma_hscale_adj;
static GtkObject *brightness_hscale_adj;

/* svgalib */
#ifdef ENABLE_XMAME_SVGALIB_SUPPORT
static GtkWidget *svga_tweak_table;
static GtkWidget *planar_checkbutton;
static GtkWidget *linear_checkbutton;
static GtkWidget *tweaked_mode_checkbutton;
static GtkWidget *centerx_spinbutton;
static GtkObject *centerx_spinbutton_adj;
static GtkWidget *centery_spinbutton;
static GtkObject *centery_spinbutton_adj;
#endif
/* xfx */
static GtkWidget *fxgkeepaspect_checkbutton;
static GtkWidget *fx_combo;
 /* SDL */
static GtkWidget *render_mode_checkbutton;
static GtkWidget *phcursor_checkbutton;
static GtkWidget *sdl_modes_label;
static GtkWidget *sdl_modes_combo;
static GtkWidget *sdl_modes_spinbutton;
static GtkObject *sdl_modes_spinbutton_adj;
static GtkWidget *fullscreen_checkbutton;
static GtkWidget *doublebuf_checkbutton;
static GtkWidget *sdl_modes_checkbutton;

/* OpenGL */
static GtkWidget *gltexture_size_spinbutton;
static GtkObject *gltexture_size_spinbutton_adj;
static GtkWidget *glext78_checkbutton;
static GtkWidget *glbilinear_checkbutton;
static GtkWidget *gldrawbitmap_checkbutton;
static GtkWidget *glantialias_checkbutton;
static GtkWidget *glalphablending_checkbutton;
static GtkWidget *gldblbuffer_checkbutton;
static GtkWidget *glforceblitmode_checkbutton;
static GtkWidget *cabview_checkbutton;
static GtkWidget *cabinet_entry;
static GtkWidget *glres_checkbutton;
static GtkWidget *glres_combo;
static GtkWidget *gllib_entry;
static GtkWidget *glulib_entry;
static GtkWidget *glcolormod_checkbutton;
 /* X11 */
static GtkWidget *video_mode_combo;
static GtkWidget *xvfullscreen_checkbutton;
static GtkWidget *xsync_checkbutton;
static GtkWidget *privatecmap_checkbutton;
static GtkWidget *xil_checkbutton;
static GtkWidget *mtxil_checkbutton;
static GtkWidget *cursor_checkbutton;
static GtkWidget *mitshm_checkbutton;
static GtkWidget *xvgeom_checkbutton;
static GtkWidget *xvgeom_combo;
static GtkWidget *yuv_combo;
static GtkWidget *displayaspectratio_entry;
static GtkWidget *disable_mode_entry;
static GtkWidget *keepaspect_checkbutton;

 /* ggi */
static GtkWidget *ggi_res_table;
static GtkWidget *ggilinear_checkbutton;
static GtkWidget *ggi_force_resolution_checkbutton;
static GtkWidget *ggix_spinbutton;
static GtkObject *ggix_spinbutton_adj;
static GtkWidget *ggiy_spinbutton;
static GtkObject *ggiy_spinbutton_adj;
/* Sound */
static GtkWidget *dsp_plugin_combo;
static GtkWidget *mixer_plugin_combo;
static GtkWidget *alsa_buffer_label;
static GtkWidget *alsacard_label;
static GtkWidget *alsadevice_label;
static GtkWidget *alsa_frame;
static GtkWidget *arts_frame;
static GtkWidget *qnx_frame;
static GtkWidget *PCM_label;
static GtkWidget *audiodevice_combo;
static GtkWidget *mixerdevice_label;
static GtkWidget *mixerdevice_combo;
static GtkWidget *samplefreq_label;
static GtkWidget *audiodevice_label;
static GtkWidget *samplefreq_combo;
static GtkWidget *volume_frame;
static GtkWidget *bufsize_frame;
static GtkWidget *sound_checkbutton;
static GtkWidget *samples_checkbutton;
static GtkWidget *db_hscale;
static GtkObject *db_hscale_adj;
static GtkWidget *bufsize_hscale;
static GtkObject *bufsize_hscale_adj;
static GtkWidget *fakesound_checkbutton;
static GtkWidget *timer_checkbutton;
static GtkWidget *audio_preferred_checkbutton;
static GtkWidget *arts_spinbutton;
static GtkObject *arts_spinbutton_adj;
static GtkWidget *alsa_buffer_spinbutton;
static GtkObject *alsa_buffer_spinbutton_adj;
static GtkWidget *alsacard_spinbutton;
static GtkObject *alsacard_spinbutton_adj;
static GtkWidget *alsadevice_spinbutton;
static GtkObject *alsadevice_spinbutton_adj;
static GtkWidget *pcm_entry;
static GtkWidget *soundfile_entry;
static GtkWidget *soundfile_hbox;
static GtkWidget *audiodevice_hbox;
/* Controllers*/
static GtkWidget *Xinput_joy_frame;
static GtkWidget *joydevname_label;
static GtkWidget *paddevname_label;
static GtkWidget *x11joyname_label;
static GtkWidget *winkeys_checkbutton;
static GtkWidget *mouse_checkbutton;
static GtkWidget *grabmouse_checkbutton;
static GtkWidget *grabkeyboard_checkbutton;
static GtkWidget *XInput_trackball1_entry;
static GtkWidget *XInput_trackball3_entry;
static GtkWidget *XInput_trackball2_entry;
static GtkWidget *XInput_trackball4_entry;
static GtkWidget *usbpspad_checkbutton;
static GtkWidget *hotrod_checkbutton;
static GtkWidget *rapidfire_checkbutton;
static GtkWidget *analogstick_checkbutton;
static GtkWidget *XInput_joystick1_entry;
static GtkWidget *XInput_joystick3_entry;
static GtkWidget *XInput_joystick2_entry;
static GtkWidget *XInput_joystick4_entry;
static GtkWidget *paddevname_entry;
static GtkWidget *x11joyname_entry;
static GtkWidget *hotrodse_checkbutton;
static GtkWidget *joytype_combo;
static GtkWidget *joydevname_entry;
static GtkWidget *config_name_checkbutton;
static GtkWidget *config_name_entry;
static GtkWidget *ctrlr_combo;
static GtkWidget *keymaptype_combo;
static GtkWidget *ugcicoin_checkbutton;
/* Misc */
static GtkWidget *log_label;
static GtkWidget *debug_size_label;
static GtkWidget *debug_size_combo;
static GtkWidget *bios_label;
static GtkWidget *bios_combo;
static GtkWidget *artwork_resolution_combo;
static GtkWidget *artwork_frame;

static GtkWidget *use_backdrops_checkbutton;
static GtkWidget *use_bezels_checkbutton;
static GtkWidget *use_overlays_checkbutton;
static GtkWidget *artcrop_checkbutton;
static GtkWidget *keyboard_leds_checkbutton;
static GtkWidget *cheat_checkbutton;
static GtkWidget *debug_checkbutton;
static GtkWidget *debug_size_combo;
static GtkWidget *log_flag_checkbutton;
static GtkWidget *log_entry;
static GtkWidget *additional_options_checkbutton;
static GtkWidget *additional_options_entry;

/* Vector */
static GtkWidget *translucency_checkbutton;
static GtkWidget *beam_hscale;
static GtkWidget *flicker_hscale;
static GtkWidget *intensity_hscale;
static GtkWidget *gldrawbitmapvec_checkbutton;
static GtkWidget *glantialiasvec_checkbutton;
static GtkWidget *antialias_checkbutton;
static GtkWidget *vectorres_combo;
static GtkWidget *vector_fixed_resolution_checkbutton;
static GtkWidget *disclaimer_checkbutton;
static GtkWidget *gameinfo_checkbutton;

const char *bios_version[] = {
	N_("Europe, 1 Slot (also been seen on a 4 slot)"),
	N_("Europe, 4 Slot"),
	N_("US, 2 Slot"),
	N_("US, 6 Slot (V5?)"),
	N_("Asia S3 Ver 6"),
	N_("Japan, Ver 6 VS Bios"),
	N_("Japan, Older"),
	N_("Universe Bios v1.0 (hack)"),
	N_("Universe Bios v1.1 (hack)"),
	N_("Debug (Development) Bios"),
	N_("AES Console (Asia?) Bios"),
	N_("Universe Bios v1.2 (hack)"),
	NULL
};

const char *resolution_table[] = {
	"320x200",
	"640x480",
	"800x600",
	"1024x768",
	"1280x1024",
	"1600x1200",
	NULL
};

static GtkWidget *
options_frame_new (const char *text)
{
	PangoFontDescription *font_desc;
	GtkWidget *label;
	GtkWidget *frame = gtk_frame_new (text);

	gtk_container_set_border_width (GTK_CONTAINER (frame), 6);
	gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_NONE);

	label = gtk_frame_get_label_widget (GTK_FRAME (frame));

	font_desc = pango_font_description_new ();
	pango_font_description_set_weight (font_desc,
					   PANGO_WEIGHT_BOLD);
	gtk_widget_modify_font (label, font_desc);
	pango_font_description_free (font_desc);

	return frame;
}

static GtkWidget *
options_frame_create_child (GtkWidget *widget)
{
	GtkWidget *vbox;
	GtkWidget *hbox;
	GtkWidget *label;

	hbox = gtk_hbox_new (FALSE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (hbox), 6);
	gtk_container_add (GTK_CONTAINER (widget), hbox);
	gtk_widget_show (hbox);

	label = gtk_label_new ("    ");
	gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
	gtk_widget_show (label);

	vbox = gtk_vbox_new (FALSE, 6);
	gtk_box_pack_start (GTK_BOX (hbox), vbox, TRUE, TRUE, 0);
	gtk_widget_show (vbox);

	return vbox;
}

static void
button_toggled      (GtkObject       *checkbutton,
                     gpointer         user_data)
{
	gtk_widget_set_sensitive (GTK_WIDGET (user_data), gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (checkbutton)));
}

static void
not_button_toggled  (GtkObject       *checkbutton,
                     gpointer         user_data)
{
	gtk_widget_set_sensitive (GTK_WIDGET (user_data), !gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (checkbutton)));
}

static void
autoframeskip_toggled   (GtkObject       *autoframeskip_checkbutton,
                         gpointer         user_data)
{
	gtk_widget_set_sensitive (frameskip_combo, !gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (autoframeskip_checkbutton)));
	gtk_widget_set_sensitive (frameskipper_combo, gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (autoframeskip_checkbutton)));
	gtk_widget_set_sensitive (maxautoframeskip_combo, gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (autoframeskip_checkbutton)));
	gtk_widget_set_sensitive (fts_label, !gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (autoframeskip_checkbutton)));
	gtk_widget_set_sensitive (fs_method_label, gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (autoframeskip_checkbutton)));
	gtk_widget_set_sensitive (maxfs_label, gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (autoframeskip_checkbutton)));
}


static GtkWidget *
combo_new_empty(GType key_type) {
	GtkWidget *combo;
	GtkListStore *model;
	GtkCellRenderer *renderer;
	
	model = gtk_list_store_new(2, key_type, G_TYPE_STRING);
	combo = gtk_combo_box_new_with_model(GTK_TREE_MODEL(model));

	/* Add a renderer only for the 2nd column.
	* The first one is a key so the user shouldn't see it.
	*/
	renderer = gtk_cell_renderer_text_new ();
	gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, TRUE);
	gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer,
                                "text", 1,
                                NULL);

	return combo;
}

/* Same as above, but assume widget has been built in Glade, so we pass widget in, don't create it */
static GtkWidget *
combo_new_empty_from_glade(GtkWidget *combo, GType key_type) {
	GtkListStore *model;
	GtkCellRenderer *renderer;
	
	model = gtk_list_store_new(2, key_type, G_TYPE_STRING);
	gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (model));

	/* Add a renderer only for the 2nd column.
	* The first one is a key so the user shouldn't see it.
	*/
	renderer = gtk_cell_renderer_text_new ();
	gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, TRUE);
	gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer,
                                "text", 1,
                                NULL);

	return combo;
}

/**
* Creates a new combo for the given option.
*
* The GtkListStore with the data is saved in the combo as "model"
*/
static GtkWidget *
combo_new(const gchar *option, GType key_type) {

	GtkWidget *combo;
	GtkListStore *model;
	GtkCellRenderer *renderer;
	const gchar **values;
	const gchar **keys;
	int i;
	GtkTreeIter iter;
	gboolean is_int_option;

	is_int_option = (key_type == G_TYPE_INT);

	if (option) {
		values = xmame_get_option_values (current_exec, option);
		keys = xmame_get_option_keys (current_exec, option);
	} else {
		values = NULL;
		keys = NULL;
	}

	if (is_int_option)
		model = gtk_list_store_new(2, G_TYPE_INT, G_TYPE_STRING);
	else
		model = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
	
	if (values) {
		for (i=0; values[i]; i++) {
			gtk_list_store_append(model, &iter);

			if (is_int_option) {
				gtk_list_store_set(model, &iter, 0, i, 1, values[i], -1);
			} else {
				gtk_list_store_set(model, &iter, 0, keys[i], 1, values[i], -1);
			}
		}
	}
	
	combo = gtk_combo_box_new_with_model(GTK_TREE_MODEL(model));

	/* Add a renderer only for the 2nd column.
	* The first one is a key so the user shouldn't see it.
	*/
	renderer = gtk_cell_renderer_text_new ();
	gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, TRUE);
	gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer,
                                "text", 1,
                                NULL);


	gtk_widget_set_sensitive(combo, xmame_has_option(current_exec, option));
	return combo;
}

/**
* Creates a new combo for the given option.
*
* The GtkListStore with the data is saved in the combo as "model"
*/
static GtkWidget *
combo_new_from_glade(GtkWidget *combo, const gchar *option, GType key_type) {

//	GtkWidget *combo;
	GtkListStore *model;
	GtkCellRenderer *renderer;
	const gchar **values;
	const gchar **keys;
	int i;
	GtkTreeIter iter;
	gboolean is_int_option;

	is_int_option = (key_type == G_TYPE_INT);

	if (option) {
		values = xmame_get_option_values (current_exec, option);
		keys = xmame_get_option_keys (current_exec, option);
	} else {
		values = NULL;
		keys = NULL;
	}

	if (is_int_option)
		model = gtk_list_store_new(2, G_TYPE_INT, G_TYPE_STRING);
	else
		model = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
	
	if (values) {
		for (i=0; values[i]; i++) {
			gtk_list_store_append(model, &iter);

			if (is_int_option) {
				gtk_list_store_set(model, &iter, 0, i, 1, values[i], -1);
			} else {
				gtk_list_store_set(model, &iter, 0, keys[i], 1, values[i], -1);
			}
		}
	}
	
	//combo = gtk_combo_box_new_with_model(GTK_TREE_MODEL(model));
	gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL(model));

	/* Add a renderer only for the 2nd column.
	* The first one is a key so the user shouldn't see it.
	*/
	renderer = gtk_cell_renderer_text_new ();
	gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, TRUE);
	gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer,
                                "text", 1,
                                NULL);


	gtk_widget_set_sensitive(combo, xmame_has_option(current_exec, option));
	return combo;
}

static void combo_append_int_value(GtkWidget *combo, int key, const gchar *desc)
{
	GtkTreeIter iter;
	GtkTreeModel *model;

	model = gtk_combo_box_get_model(GTK_COMBO_BOX(combo));
	gtk_list_store_append(GTK_LIST_STORE(model), &iter);
	gtk_list_store_set(GTK_LIST_STORE(model), &iter, 0, key, 1, desc, -1);
}

static void combo_append_string_value(GtkWidget *combo, const gchar* key, const gchar *desc)
{
	GtkTreeIter iter;
	GtkTreeModel *model;

	model = gtk_combo_box_get_model(GTK_COMBO_BOX(combo));
	gtk_list_store_append(GTK_LIST_STORE(model), &iter);
	gtk_list_store_set(GTK_LIST_STORE(model), &iter, 0, key, 1, desc, -1);
}

static void combo_set_key(GtkWidget *combo, const gchar *key)
{
	GtkTreeModel *model;
	GtkTreeIter iter;
	const gchar *current_key;

	if (!key)
		return;

	model = gtk_combo_box_get_model(GTK_COMBO_BOX(combo));

	if (gtk_tree_model_get_iter_first(model, &iter) == FALSE)
		return;

	do {
		gtk_tree_model_get(model, &iter, 0, &current_key, -1);

		if (current_key && !strcmp(current_key, key)) {
			gtk_combo_box_set_active_iter(GTK_COMBO_BOX(combo), &iter);
			return;
		}
	} while(gtk_tree_model_iter_next(model, &iter));

}

static void combo_set_int_key(GtkWidget *combo, int key)
{
	GtkTreeModel *model;
	GtkTreeIter iter;
	int current_key;

	model = gtk_combo_box_get_model(GTK_COMBO_BOX(combo));

	if (gtk_tree_model_get_iter_first(model, &iter) == FALSE)
		return;

	do {
		gtk_tree_model_get(model, &iter, 0, &current_key, -1);

		if (current_key == key) {
			gtk_combo_box_set_active_iter(GTK_COMBO_BOX(combo), &iter);
			return;
		}
	} while(gtk_tree_model_iter_next(model, &iter));

}

static void combo_set_index(GtkWidget *combo, int index)
{
	gtk_combo_box_set_active(GTK_COMBO_BOX(combo), index);
}

static const gchar *combo_get_key(GtkWidget *combo)
{
	GtkTreeIter iter;
	GtkTreeModel *model;
	const gchar *current_key = NULL;

	if (gtk_combo_box_get_active_iter(GTK_COMBO_BOX(combo), &iter) == FALSE)
		return NULL;

	model = gtk_combo_box_get_model(GTK_COMBO_BOX(combo));
	gtk_tree_model_get(model, &iter, 0, &current_key, -1);

	return current_key;
}

static int combo_get_int_key(GtkWidget *combo)
{
	GtkTreeIter iter;
	GtkTreeModel *model;
	int current_key;

	if (gtk_combo_box_get_active_iter(GTK_COMBO_BOX(combo), &iter) == FALSE)
		return -1;

	model = gtk_combo_box_get_model(GTK_COMBO_BOX(combo));
	gtk_tree_model_get(model, &iter, 0, &current_key, -1);

	return current_key;
}

static const gchar *combo_get_text(GtkWidget *combo)
{
	GtkTreeIter iter;
	GtkTreeModel *model;
	const gchar *current_key = NULL;

	if (gtk_combo_box_get_active_iter(GTK_COMBO_BOX(combo), &iter) == FALSE)
		return NULL;

	model = gtk_combo_box_get_model(GTK_COMBO_BOX(combo));
	gtk_tree_model_get(model, &iter, 1, &current_key, -1);

	return current_key;
}

static int combo_get_index(GtkWidget *combo)
{
	return gtk_combo_box_get_active(GTK_COMBO_BOX(combo));
}

static void
joystick_choosen      (GtkObject       *combo_entry,
                       gpointer         user_data)
{
	const gchar *used_text;
	
	used_text = combo_get_text(GTK_WIDGET(combo_entry));

	if (!used_text || !g_ascii_strncasecmp ("No joystick", used_text, 10)) {
		gtk_widget_set_sensitive (analogstick_checkbutton, FALSE);
		gtk_widget_set_sensitive (usbpspad_checkbutton, FALSE);
		gtk_widget_set_sensitive (rapidfire_checkbutton, FALSE);
	} else {
		gtk_widget_set_sensitive (analogstick_checkbutton, TRUE);
		gtk_widget_set_sensitive (usbpspad_checkbutton, TRUE);
		gtk_widget_set_sensitive (rapidfire_checkbutton, TRUE);
	}

	/* Fm Town Pad */
	if (used_text && !g_ascii_strncasecmp ("FM", used_text, 2)) {
		gtk_widget_set_sensitive (paddevname_label, TRUE);
		gtk_widget_set_sensitive (paddevname_entry, TRUE);
	} else {
		gtk_widget_set_sensitive (paddevname_label, FALSE);
		gtk_widget_set_sensitive (paddevname_entry, FALSE);
	}

	 /* X11 input extension */
	if (used_text && !g_ascii_strncasecmp ("X11", used_text, 3)) {
		gtk_widget_set_sensitive (x11joyname_label, TRUE);
		gtk_widget_set_sensitive (x11joyname_entry, TRUE);
	} else {
		gtk_widget_set_sensitive (x11joyname_label, FALSE);
		gtk_widget_set_sensitive (x11joyname_entry, FALSE);
	}

	/* i386 style or new i386 linux 1.x.x driver */
	if (used_text && 
		(!g_ascii_strncasecmp ("i386", used_text, 4) || 
	    !g_ascii_strncasecmp ("Standard", used_text, 8))
	   ) {
		gtk_widget_set_sensitive (joydevname_label, TRUE);
		gtk_widget_set_sensitive (joydevname_entry, TRUE);
	} else {
		gtk_widget_set_sensitive (joydevname_label, FALSE);
		gtk_widget_set_sensitive (joydevname_entry, FALSE);
	}

}


static void
on_dirty_option     (GtkObject       *object,
                     gpointer         user_data)
{
	dirty_options_flag = TRUE;
	if (user_data)
		gtk_widget_set_sensitive (GTK_WIDGET (user_data), TRUE);
}


static void
file_or_device (void)
{
	const gchar *plugin;

	
	plugin = combo_get_key (dsp_plugin_combo);
 

	if (plugin && !strcmp (plugin, "waveout")) {
		gtk_widget_set_sensitive (GTK_WIDGET (soundfile_hbox), TRUE);
		gtk_widget_set_sensitive (GTK_WIDGET (audiodevice_hbox), FALSE);
	} else {
		gtk_widget_set_sensitive (GTK_WIDGET (soundfile_hbox), FALSE);
		gtk_widget_set_sensitive (GTK_WIDGET (audiodevice_hbox), TRUE);
	}
}
	 

void
add_display_options_tab (GtkWidget    *properties_windows,
			 GtkNotebook  *target_notebook,
			 RomEntry     *rom,
			 GameOptions *target,
			 GtkWidget    *apply_button,
			 GtkWidget    *reset_button)
{
	GtkWidget *image;
	GtkWidget *effects_vbox;
	GtkWidget *effect_vbox;
	GtkWidget *correction_vbox;
	GtkWidget *gamma_label;
	GtkWidget *brightness_label;
/*	GtkWidget *display_general_vbox;
	GtkWidget *frameskip_table;*/
	GtkWidget *heightscale_label;
	GtkWidget *widthscale_label;
	GtkWidget *bpp_label;
/*	GtkObject *heightscale_spinbutton_adj; */
/*	GtkObject *widthscale_spinbutton_adj; */
	GtkWidget *display_label;
/*	GtkWidget *percent_label; 
	GtkWidget *hbox7;*/
	gchar title[BUFFER_SIZE];
	int i;

	xmame_get_options (current_exec);

	dirty_options_flag = FALSE;

	image = gmameui_get_image_from_stock ("gmameui-display-toolbar");
GMAMEUI_DEBUG ("Adding display options tab\n");
	display_label = gtk_hbox_new (FALSE, 5);
	gtk_box_pack_start (GTK_BOX (display_label), image, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (display_label), gtk_label_new (_("Display")), FALSE, FALSE, 0);
	gtk_widget_show_all (display_label);

	GladeXML *xml = glade_xml_new (GLADEDIR "properties.glade", "display_hbox", NULL);
	GtkWidget *display_hbox = glade_xml_get_widget (xml, "display_hbox");
	gtk_notebook_append_page (GTK_NOTEBOOK (target_notebook), display_hbox, display_label);

	/* Display - Effects */

	effects_vbox = glade_xml_get_widget (xml, "effects_vbox");
	/* Scanline effects no longer built-in to MAME - refer to 
	   http://aarongiles.com/?p=175 and newvideo.txt in MAME directory
	   for more information.
	   Scanlines now generated as overlay effects that are .png files
	   stored in the artwork directory. */
	effect_combo = glade_xml_get_widget (xml, "effect_combo");
	effect_vbox = glade_xml_get_widget (xml, "effect_vbox");
	if (xmame_get_option_value_count (current_exec, "effect") == -1) {
		gtk_widget_hide (effect_combo);
		effect_name = glade_xml_get_widget (xml, "effect_name");
		gtk_entry_set_text (GTK_ENTRY (effect_name),
				    target->effect_name);
	} else {
		effect_combo = combo_new_from_glade(effect_combo, "effect", G_TYPE_INT);
		combo_set_index(effect_combo, target->effect);
		gtk_widget_hide_all (effect_vbox);
	}

	/* -scanlines option has been removed since version XXX */
	scanlines_checkbutton = glade_xml_get_widget (xml, "scanlines_checkbutton");
	if (xmame_has_option (current_exec, "scanlines")) {
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (scanlines_checkbutton),
					      target->scanlines);
	} else {
		gtk_widget_hide (scanlines_checkbutton);
	}
	
	/* Display - Correction */
	correction_vbox = glade_xml_get_widget (xml, "correction_vbox");

	gamma_label = glade_xml_get_widget (xml, "gamma_label");

	gamma_hscale = glade_xml_get_widget (xml, "gamma_hscale");
	gtk_adjustment_set_value (GTK_ADJUSTMENT (gtk_range_get_adjustment (GTK_RANGE (gamma_hscale))), target->gamma_correction);

	brightness_label = glade_xml_get_widget (xml, "brightness_label");

	/* FIXME: Detect brightness range */

	/*if (available_options->dos_brightness)
	{*/
		brightness_hscale = glade_xml_get_widget (xml, "brightness_hscale");
		gtk_adjustment_set_value (GTK_ADJUSTMENT (gtk_range_get_adjustment (GTK_RANGE (brightness_hscale))), target->brightness);
	/*}
	else
	{
		brightness_hscale_adj = gtk_adjustment_new ( ((target->brightness)-0.5)* (100/1.5), 0, 100, 1, 1, 0);
		brightness_hscale = gtk_hscale_new (GTK_ADJUSTMENT (brightness_hscale_adj));
		gtk_scale_set_digits (GTK_SCALE (brightness_hscale), 0);
	}
	*/

	/*if (!available_options->dos_brightness)
	{
		percent_label = gtk_label_new ("%");
		gtk_widget_show (percent_label);
		gtk_box_pack_start (GTK_BOX (hbox7), percent_label, FALSE, FALSE, 0);
	}*/

	/* Display - Display options */
	GtkWidget *dirty_checkbutton = glade_xml_get_widget (xml, "dirty_checkbutton");
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (dirty_checkbutton), target->dirty);
	
	GtkWidget *throttle_checkbutton = glade_xml_get_widget (xml, "throttle_checkbutton");
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (throttle_checkbutton), target->throttle);
	
	GtkWidget *sleepidle_checkbutton = glade_xml_get_widget (xml, "sleepidle_checkbutton");
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (sleepidle_checkbutton), target->sleepidle);

	/* Display - Frameskipping */
	
	/* ADB TODO This doesn't seem to be disabling the right widgets */
	autoframeskip_checkbutton = glade_xml_get_widget (xml, "autoframeskip_checkbutton");
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (autoframeskip_checkbutton), target->autoframeskip);
	
	/* Frames to skip */
	fts_label = glade_xml_get_widget (xml, "fts_label");

	frameskip_combo = glade_xml_get_widget (xml, "frameskip_combo");
	frameskip_combo = combo_new_from_glade (frameskip_combo, "frameskip", G_TYPE_INT);
	combo_append_int_value(frameskip_combo, 0, _("Draw every frame"));
	for (i = 1; i < 12; i++) {
		g_snprintf (title, BUFFER_SIZE, _("Skip %i of 12 frames"), i);
		combo_append_int_value(frameskip_combo, i, title);
	}

	if (target->frameskip == 0 || target->frameskip > 11)
		target->frameskip = 0;
	combo_set_index(frameskip_combo, target->frameskip);
	gtk_widget_set_sensitive (frameskip_combo, !(target->autoframeskip));
	
	/* Frameskip method -frameskipper option unavailable since XXX */
	fs_method_label = glade_xml_get_widget (xml, "fts_method_label");
	frameskipper_combo = glade_xml_get_widget (xml, "frameskipper_combo");
	if (xmame_has_option (current_exec, "frameskipper")) {
		frameskipper_combo = combo_new_from_glade (frameskipper_combo, "frameskipper", G_TYPE_INT);
		combo_set_index(frameskipper_combo, target->frameskipper);
		gtk_widget_set_sensitive (GTK_WIDGET (frameskipper_combo), target->autoframeskip
			  && xmame_has_option (current_exec, "frameskipper"));
	} else {
		gtk_widget_hide (frameskipper_combo);
		gtk_widget_hide (fs_method_label);
	}
	
	maxfs_label = glade_xml_get_widget (xml, "maxfs_label");

	maxautoframeskip_combo = glade_xml_get_widget (xml, "maxautoframeskip_combo");
	maxautoframeskip_combo = combo_new_from_glade (maxautoframeskip_combo, "maxautoframeskip", G_TYPE_INT);

	for (i = 0; i <= 11; i++) {
		g_snprintf (title, BUFFER_SIZE, "%i", i);
		combo_append_int_value(maxautoframeskip_combo, i, title);
	} 
	combo_set_int_key(maxautoframeskip_combo, target->maxautoframeskip);
	
	gtk_widget_set_sensitive (maxautoframeskip_combo, target->autoframeskip);

	/* Display - Resolution */
	heightscale_label = glade_xml_get_widget (xml, "heightscale_lable");

	widthscale_label = glade_xml_get_widget (xml, "widthscale_lable");

	bpp_label = glade_xml_get_widget (xml, "bpp_lable");

	heightscale_spinbutton = glade_xml_get_widget (xml, "heightscale_spinbutton");
	gtk_spin_button_set_value (GTK_SPIN_BUTTON (heightscale_spinbutton), target->heightscale);

	bpp_combo = glade_xml_get_widget (xml, "bpp_combo");
	bpp_combo = combo_new_from_glade (bpp_combo, "bpp", G_TYPE_INT);

	combo_append_int_value(bpp_combo, 0, _("Auto"));
	combo_append_int_value(bpp_combo, 8, _("8 bits"));
	combo_append_int_value(bpp_combo, 15, _("15 bits"));
	combo_append_int_value(bpp_combo, 16, _("16 bits"));
	combo_append_int_value(bpp_combo, 32, _("32 bits"));
	
	combo_set_int_key(bpp_combo, target->bpp);

	widthscale_spinbutton = glade_xml_get_widget (xml, "widthscale_spinbutton");
/*!!*/	gtk_spin_button_set_value (GTK_SPIN_BUTTON (widthscale_spinbutton), target->widthscale);
	
	arbheight_entry = glade_xml_get_widget (xml, "arbheight_entry");
	g_snprintf (title, BUFFER_SIZE, "%i", target->arbheight);
	gtk_entry_set_text (GTK_ENTRY (arbheight_entry), title);
	gtk_widget_set_sensitive (arbheight_entry, target->arbheight);

	arbheight_checkbutton = glade_xml_get_widget (xml, "arbheight_checkbutton");
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (arbheight_checkbutton), target->arbheight);

	autodouble_checkbutton = glade_xml_get_widget (xml, "autodouble_checkbutton");
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (autodouble_checkbutton), target->autodouble);

	/* Display - Rotation */

	rotation_combo = glade_xml_get_widget (xml, "rotation_combo");
	rotation_combo = combo_new_empty_from_glade (rotation_combo, G_TYPE_INT);

	combo_append_int_value(rotation_combo, 0, _("None"));
	combo_append_int_value(rotation_combo, 1, _("Clockwise"));
	combo_append_int_value(rotation_combo, 2, _("Anti-clockwise"));

	if (target->ror)
		combo_set_int_key(rotation_combo, 1);
	else if (target->rol)
		combo_set_int_key(rotation_combo, 2);
	else
		combo_set_int_key(rotation_combo, 0);

	norotate_checkbutton = glade_xml_get_widget (xml, "norotate_checkbutton");
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (norotate_checkbutton), target->norotate);

	flipx_checkbutton = glade_xml_get_widget (xml, "flipx_checkbutton");
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (flipx_checkbutton), target->flipx);

	flipy_checkbutton = glade_xml_get_widget (xml, "flipx_checkbutton");
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (flipy_checkbutton), target->flipy);
			
	gtk_widget_set_sensitive ( GTK_WIDGET (dirty_checkbutton), xmame_has_option (current_exec, "dirty"));
	gtk_widget_set_sensitive (fts_label, !gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (autoframeskip_checkbutton)));
	gtk_widget_set_sensitive (fs_method_label, gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (autoframeskip_checkbutton)));
	gtk_widget_set_sensitive (maxfs_label, gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (autoframeskip_checkbutton)));

	g_signal_connect (G_OBJECT (dirty_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (dirty_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
	g_signal_connect (G_OBJECT (throttle_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (throttle_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
	g_signal_connect (G_OBJECT (sleepidle_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (sleepidle_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
	g_signal_connect (G_OBJECT (scanlines_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (scanlines_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
	g_signal_connect (G_OBJECT (norotate_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (norotate_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
	g_signal_connect (G_OBJECT (flipx_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (flipx_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
	g_signal_connect (G_OBJECT (flipy_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (flipy_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
	g_signal_connect (G_OBJECT (autodouble_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (autodouble_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    reset_button);

	g_signal_connect (G_OBJECT (autoframeskip_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (autoframeskip_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
	g_signal_connect (G_OBJECT (arbheight_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (arbheight_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
	g_signal_connect_after (G_OBJECT (autoframeskip_checkbutton), "toggled",
				G_CALLBACK (autoframeskip_toggled),
				NULL);
	g_signal_connect_after (G_OBJECT (arbheight_checkbutton), "toggled",
				G_CALLBACK (button_toggled),
				G_OBJECT (arbheight_entry));

	g_signal_connect (G_OBJECT (effect_combo), "changed",
			  G_CALLBACK (on_dirty_option),
			  apply_button);
	g_signal_connect (G_OBJECT (effect_combo), "changed",
			  G_CALLBACK (on_dirty_option),
			  reset_button);
	g_signal_connect (G_OBJECT (rotation_combo), "changed",
			  G_CALLBACK (on_dirty_option),
			  apply_button);
	g_signal_connect (G_OBJECT (rotation_combo), "changed",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
	g_signal_connect (G_OBJECT (frameskip_combo), "changed",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (frameskip_combo), "changed",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
	g_signal_connect (G_OBJECT (maxautoframeskip_combo), "changed",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (maxautoframeskip_combo), "changed",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
	g_signal_connect (G_OBJECT (bpp_combo), "changed",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (bpp_combo), "changed",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
	g_signal_connect (G_OBJECT (arbheight_entry), "changed",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (arbheight_entry), "changed",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
	g_signal_connect (G_OBJECT (frameskipper_combo), "changed",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (frameskipper_combo), "changed",
			    G_CALLBACK (on_dirty_option),
			    reset_button);

	g_signal_connect (G_OBJECT (gtk_range_get_adjustment (GTK_RANGE (brightness_hscale))), "value-changed",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (gtk_range_get_adjustment (GTK_RANGE (brightness_hscale))), "value-changed",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
	g_signal_connect (G_OBJECT (gtk_range_get_adjustment (GTK_RANGE (gamma_hscale))), "value-changed",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (gtk_range_get_adjustment (GTK_RANGE (gamma_hscale))), "value-changed",
			    G_CALLBACK (on_dirty_option),
			    reset_button);

	g_signal_connect (G_OBJECT (widthscale_spinbutton), "changed",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (widthscale_spinbutton), "changed",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
	g_signal_connect (G_OBJECT (heightscale_spinbutton), "changed",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (heightscale_spinbutton), "changed",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
GMAMEUI_DEBUG ("Finished adding display options tab");
}

static GtkWidget *
get_x11_rendering_frame (GameOptions *target,
			 GtkWidget *apply_button,
			 GtkWidget *reset_button)
{
	GtkWidget *x11_frame_parent;
	GtkWidget *x11_frame;
	GtkWidget *x11_table;
	GtkWidget *yuv_label;
	GtkWidget *video_mode_label;	
	int i;

	xmame_get_options (current_exec);

	/* X11 */
	x11_frame_parent = options_frame_new (_("X11 options"));
	x11_frame = options_frame_create_child (x11_frame_parent);

	x11_table = gtk_table_new (9, 1, FALSE);
	gtk_table_set_row_spacings (GTK_TABLE (x11_table), 6);
	gtk_table_set_col_spacings (GTK_TABLE (x11_table), 6);
	gtk_widget_show (x11_table);
	gtk_box_pack_start (GTK_BOX (x11_frame), x11_table, FALSE, FALSE, 0);

	/* Video mode label */	
	video_mode_label = gtk_label_new (_("Video mode:"));
	gtk_misc_set_alignment (GTK_MISC (video_mode_label), 0, 0.5);
	gtk_widget_show (video_mode_label);
	gtk_table_attach_defaults (GTK_TABLE (x11_table), video_mode_label, 0, 1, 0, 1);
	
	video_mode_combo = combo_new("video-mode", G_TYPE_INT);
	gtk_widget_show (video_mode_combo);
	gtk_table_attach_defaults (GTK_TABLE (x11_table), video_mode_combo, 1, 2, 0, 1);
	combo_set_index (video_mode_combo, target->x11_mode);

	xvfullscreen_checkbutton = gtk_check_button_new_with_label (_("Fullscreen"));
	gtk_widget_show (xvfullscreen_checkbutton);
	gtk_table_attach_defaults (GTK_TABLE (x11_table), xvfullscreen_checkbutton, 0, 2, 1, 2);

	cursor_checkbutton = gtk_check_button_new_with_label (_("Show cursor"));
	gtk_widget_show (cursor_checkbutton);
	gtk_table_attach_defaults (GTK_TABLE (x11_table), cursor_checkbutton, 0, 2, 2, 3);

	mitshm_checkbutton = gtk_check_button_new_with_label (_("Use MIT shared memory"));
	gtk_widget_show (mitshm_checkbutton);
	gtk_table_attach_defaults (GTK_TABLE (x11_table), mitshm_checkbutton, 0, 2, 3, 4);

	/* XV Geometry */
	xvgeom_checkbutton = gtk_check_button_new_with_label (_("Geometry:"));
	gtk_widget_show (xvgeom_checkbutton);
	gtk_table_attach_defaults (GTK_TABLE (x11_table), xvgeom_checkbutton, 0, 1, 4, 5);

	xvgeom_combo = combo_new_empty (G_TYPE_STRING);
	gtk_widget_show (xvgeom_combo);
	gtk_table_attach_defaults (GTK_TABLE (x11_table), xvgeom_combo, 1, 2, 4, 5);

	for (i=0; resolution_table[i]; i++)
		combo_append_string_value(xvgeom_combo, resolution_table[i], resolution_table[i]);

	/* Force YUV Mode */
	yuv_label = gtk_label_new (_("Force YUV mode:"));
	gtk_widget_show (yuv_label);
	gtk_table_attach_defaults (GTK_TABLE (x11_table), yuv_label, 0, 1, 5, 6);

	yuv_combo = combo_new ("force-yuv", G_TYPE_INT);
	combo_set_index(yuv_combo, target->force_yuv);

	gtk_widget_show (yuv_combo);
	gtk_table_attach_defaults (GTK_TABLE (x11_table), yuv_combo, 1, 2, 5, 6);


	/* X Sync */
	xsync_checkbutton = gtk_check_button_new_with_label (_("X Synch"));
	gtk_widget_show (xsync_checkbutton);
	gtk_table_attach_defaults (GTK_TABLE (x11_table), xsync_checkbutton, 0, 2, 6, 7);

	/* Private C map */
	privatecmap_checkbutton = gtk_check_button_new_with_label (_("Private colormap"));
	gtk_widget_show (privatecmap_checkbutton);
	gtk_table_attach_defaults (GTK_TABLE (x11_table), privatecmap_checkbutton, 0, 2, 7, 8);

	xil_checkbutton = gtk_check_button_new_with_label (_("XIL scaling"));
	gtk_widget_show (xil_checkbutton);
	gtk_table_attach_defaults (GTK_TABLE (x11_table), xil_checkbutton, 0, 1, 8, 9);
	
	mtxil_checkbutton = gtk_check_button_new_with_label (_("XIL multi-threading"));
	gtk_widget_show (mtxil_checkbutton);
	gtk_table_attach_defaults (GTK_TABLE (x11_table), mtxil_checkbutton, 1, 2, 8, 9);
	
	/* x11 frame */
	gtk_widget_set_sensitive (GTK_WIDGET (mitshm_checkbutton), xmame_has_option (current_exec, "mitshm"));
	gtk_widget_set_sensitive (GTK_WIDGET (xvfullscreen_checkbutton), xmame_has_option (current_exec, "fullscreen"));
	gtk_widget_set_sensitive (GTK_WIDGET (cursor_checkbutton), xmame_has_option (current_exec, "cursor"));
	gtk_widget_set_sensitive (GTK_WIDGET (xsync_checkbutton), xmame_has_option (current_exec, "xsync"));
	gtk_widget_set_sensitive (GTK_WIDGET (xil_checkbutton), xmame_has_option (current_exec, "xil"));
	gtk_widget_set_sensitive (GTK_WIDGET (mtxil_checkbutton), xmame_has_option (current_exec, "mtxil") && target->xil);

	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (cursor_checkbutton), target->cursor);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (mitshm_checkbutton), target->mitshm);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (xvgeom_checkbutton), target->xvgeom_flag);

	combo_set_key(xvgeom_combo, target->geometry);

	gtk_widget_set_sensitive (GTK_WIDGET (xvgeom_combo), target->xvgeom_flag /*&& 
								available_options.xvext
								available_options.mitshm && 
								target->mitshm &&
								target->xvext && 
								*/);

	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (xvfullscreen_checkbutton), target->fullscreen);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (xsync_checkbutton), target->xsync);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (privatecmap_checkbutton), target->privatecmap);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (xil_checkbutton), target->xil);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (mtxil_checkbutton), target->mtxil);

	g_signal_connect (G_OBJECT (video_mode_combo), "changed",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (cursor_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (cursor_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
	g_signal_connect (G_OBJECT (mitshm_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (mitshm_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    reset_button);

	g_signal_connect (G_OBJECT (xvgeom_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (xvgeom_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
	g_signal_connect (G_OBJECT (xvgeom_combo), "changed",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (xvgeom_combo), "changed",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
	g_signal_connect (G_OBJECT (yuv_combo), "changed",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (yuv_combo), "changed",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
	g_signal_connect (G_OBJECT (xvfullscreen_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (xvfullscreen_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
		
	g_signal_connect_after (G_OBJECT (xvgeom_checkbutton), "toggled",
			    G_CALLBACK (button_toggled),
			    xvgeom_combo);

	g_signal_connect (G_OBJECT (xsync_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (xsync_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
	g_signal_connect (G_OBJECT (privatecmap_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (privatecmap_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
	g_signal_connect (G_OBJECT (xil_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (xil_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
	g_signal_connect_after (G_OBJECT (xil_checkbutton), "toggled",
				  G_CALLBACK (button_toggled),
				  G_OBJECT (mtxil_checkbutton));
	g_signal_connect (G_OBJECT (mtxil_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (mtxil_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    reset_button);

	return x11_frame_parent;
}

static GtkWidget *
get_xgl_rendering_frame (GameOptions *target,
			 GtkWidget    *apply_button,
			 GtkWidget    *reset_button)
{
	GtkWidget *gl_frame_parent;
	GtkWidget *gl_frame;
	GtkWidget *ogl_table;
	GtkWidget *gltexture_size_label;
	GtkWidget *gllib_label;
	GtkWidget *glulib_label;
	int i;

	xmame_get_options (current_exec);

	gl_frame_parent = options_frame_new (_("OpenGL options"));
	gl_frame = options_frame_create_child (gl_frame_parent);

	ogl_table = gtk_table_new (9, 4, FALSE);
	gtk_table_set_row_spacings (GTK_TABLE (ogl_table), 6);
	gtk_table_set_col_spacings (GTK_TABLE (ogl_table), 6);
	gtk_widget_show (ogl_table);
	gtk_container_add (GTK_CONTAINER (gl_frame), ogl_table);

	gltexture_size_label = gtk_label_new (_("Texture size:"));
	gtk_widget_show (gltexture_size_label);
	gtk_table_attach_defaults (GTK_TABLE (ogl_table), gltexture_size_label, 0, 1, 0, 1);
	gtk_misc_set_alignment (GTK_MISC (gltexture_size_label), 0, 0.5);

	gltexture_size_spinbutton_adj = gtk_adjustment_new (target->gltexture_size, 0, 1000, 1, 10, 10);
	gltexture_size_spinbutton = gtk_spin_button_new (GTK_ADJUSTMENT (gltexture_size_spinbutton_adj), 1, 0);
	gtk_widget_show (gltexture_size_spinbutton);
	gtk_table_attach_defaults (GTK_TABLE (ogl_table), gltexture_size_spinbutton, 1, 2, 0, 1);

	glext78_checkbutton = gtk_check_button_new_with_label (_("GL extension #78"));
	gtk_widget_show (glext78_checkbutton);
	gtk_table_attach_defaults (GTK_TABLE (ogl_table), glext78_checkbutton, 0, 1, 1, 2);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (glext78_checkbutton), TRUE);
	
	glbilinear_checkbutton = gtk_check_button_new_with_label (_("Bilinear filtering"));
	gtk_widget_show (glbilinear_checkbutton);
	gtk_table_attach_defaults (GTK_TABLE (ogl_table), glbilinear_checkbutton, 1, 2, 1, 2);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (glbilinear_checkbutton), TRUE);

	gldrawbitmap_checkbutton = gtk_check_button_new_with_label (_("Draw bitmap"));
	gtk_widget_show (gldrawbitmap_checkbutton);
	gtk_table_attach_defaults (GTK_TABLE (ogl_table), gldrawbitmap_checkbutton, 0, 1, 2, 3);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gldrawbitmap_checkbutton), TRUE);
	

	glantialias_checkbutton = gtk_check_button_new_with_label (_("Antialiasing"));
	gtk_widget_show (glantialias_checkbutton);
	gtk_table_attach_defaults (GTK_TABLE (ogl_table), glantialias_checkbutton, 1, 2, 2, 3);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (glantialias_checkbutton), TRUE);

	glalphablending_checkbutton = gtk_check_button_new_with_label (_("Alphablending"));
	gtk_widget_show (glalphablending_checkbutton);
	gtk_table_attach_defaults (GTK_TABLE (ogl_table), glalphablending_checkbutton, 0, 1, 3, 4);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (glalphablending_checkbutton), TRUE);

	gldblbuffer_checkbutton = gtk_check_button_new_with_label (_("Double buffering"));
	gtk_widget_show (gldblbuffer_checkbutton);
	gtk_table_attach_defaults (GTK_TABLE (ogl_table), gldblbuffer_checkbutton, 1, 2, 3, 4);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gldblbuffer_checkbutton), TRUE);

	glforceblitmode_checkbutton = gtk_check_button_new_with_label (_("True color blitter"));
	gtk_widget_show (glforceblitmode_checkbutton);
	gtk_table_attach_defaults (GTK_TABLE (ogl_table), glforceblitmode_checkbutton, 1, 2, 4, 5);

	glcolormod_checkbutton = gtk_check_button_new_with_label (_("Color modulation"));
	gtk_widget_show (glcolormod_checkbutton);
	gtk_table_attach_defaults (GTK_TABLE (ogl_table), glcolormod_checkbutton, 0, 1, 4, 5);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (glcolormod_checkbutton), TRUE);

	cabview_checkbutton = gtk_check_button_new_with_label (_("Cabinet model:"));
	gtk_widget_show (cabview_checkbutton);
	gtk_table_attach_defaults (GTK_TABLE (ogl_table), cabview_checkbutton, 0, 1, 5, 6);

	cabinet_entry = (GtkWidget *) gtk_entry_new ();
	gtk_entry_set_max_length (GTK_ENTRY (cabinet_entry), 20);
	gtk_widget_show (cabinet_entry);
	gtk_table_attach_defaults (GTK_TABLE (ogl_table), cabinet_entry, 1, 2, 5, 6);

	glres_checkbutton = gtk_check_button_new_with_label (_("Resolution"));
	gtk_widget_show (glres_checkbutton);
	gtk_table_attach_defaults (GTK_TABLE (ogl_table), glres_checkbutton, 0, 1, 6, 7);

	glres_combo = combo_new_empty (G_TYPE_STRING);
	gtk_widget_show (glres_combo);
	gtk_table_attach_defaults (GTK_TABLE (ogl_table), glres_combo, 1, 2, 6, 7);

	combo_append_string_value(glres_combo, "", "");
	for (i=0; resolution_table[i]; i++)
		combo_append_string_value(glres_combo, resolution_table[i], resolution_table[i]);
	
	gllib_label = gtk_label_new (_("OpenGL library:"));
	gtk_widget_show (gllib_label);
	gtk_table_attach_defaults (GTK_TABLE (ogl_table), gllib_label, 0, 1, 7, 8);
	gtk_misc_set_alignment (GTK_MISC (gllib_label), 0, 0.5);

	gllib_entry = gtk_entry_new ();
	gtk_widget_show (gllib_entry);
	gtk_table_attach_defaults (GTK_TABLE (ogl_table), gllib_entry, 1, 2, 7, 8);

	gtk_entry_set_text (GTK_ENTRY (gllib_entry), "libGL.so.1");

	glulib_label = gtk_label_new (_("GLU library:"));
	gtk_widget_show (glulib_label);
	gtk_table_attach_defaults (GTK_TABLE (ogl_table), glulib_label, 0, 1, 8, 9);
	gtk_misc_set_alignment (GTK_MISC (glulib_label), 0, 0.5);
	
	glulib_entry = gtk_entry_new ();
	gtk_widget_show (glulib_entry);
	gtk_table_attach_defaults (GTK_TABLE (ogl_table), glulib_entry, 1, 2, 8, 9);

	gtk_entry_set_text (GTK_ENTRY (glulib_entry), "libGLU.so.1");

	/* set sensitivity */
	gtk_widget_set_sensitive (gllib_entry, xmame_has_option (current_exec, "gllibname"));
	gtk_widget_set_sensitive (glulib_entry, xmame_has_option (current_exec, "glulibname"));
	gtk_widget_set_sensitive (glext78_checkbutton, xmame_has_option (current_exec, "glext78"));
	gtk_widget_set_sensitive (gldrawbitmap_checkbutton, xmame_has_option (current_exec, "gldrawbitmap"));
	gtk_widget_set_sensitive (glcolormod_checkbutton, xmame_has_option (current_exec, "glcolormod"));
	gtk_widget_set_sensitive (glforceblitmode_checkbutton, xmame_has_option (current_exec, "glforceblitmode"));
	gtk_widget_set_sensitive (GTK_WIDGET (cabinet_entry), target->cabview && 
	                                                    xmame_has_option (current_exec, "cabview"));

	/* Open GL frame */
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gldblbuffer_checkbutton), target->gldblbuffer);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (glbilinear_checkbutton), target->glbilinear);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (glext78_checkbutton), target->glext78);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (glforceblitmode_checkbutton), target->glforceblitmode);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gldrawbitmap_checkbutton), target->gldrawbitmap);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (glcolormod_checkbutton), target->glcolormod);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (glantialias_checkbutton), target->glantialias);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (glalphablending_checkbutton), target->glalphablending);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (cabview_checkbutton), target->cabview);
	
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (glres_checkbutton), target->glres_flag);
	gtk_entry_set_text (GTK_ENTRY (cabinet_entry), target->cabinet);
	gtk_widget_set_sensitive (GTK_WIDGET (glres_combo), target->glres_flag);
	gtk_entry_set_text (GTK_ENTRY (gllib_entry), target->gllibname);
	gtk_entry_set_text (GTK_ENTRY (glulib_entry), target->glulibname);

	g_signal_connect (G_OBJECT (gldblbuffer_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (gldblbuffer_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
	g_signal_connect (G_OBJECT (gltexture_size_spinbutton), "changed",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (gltexture_size_spinbutton), "changed",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
	g_signal_connect (G_OBJECT (glbilinear_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (glbilinear_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
	g_signal_connect (G_OBJECT (glext78_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (glext78_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
	g_signal_connect (G_OBJECT (glforceblitmode_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (glforceblitmode_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
	g_signal_connect (G_OBJECT (gldrawbitmap_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (gldrawbitmap_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
	g_signal_connect (G_OBJECT (glcolormod_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (glcolormod_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
	g_signal_connect (G_OBJECT (glantialias_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (glantialias_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
	g_signal_connect (G_OBJECT (glalphablending_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (glalphablending_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
	g_signal_connect (G_OBJECT (cabview_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (cabview_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
	g_signal_connect_after (G_OBJECT (cabview_checkbutton), "toggled",
				  G_CALLBACK (button_toggled),
				  G_OBJECT (cabinet_entry));
	g_signal_connect (G_OBJECT (cabinet_entry), "changed",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (cabinet_entry), "changed",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
	g_signal_connect (G_OBJECT (glres_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (glres_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
	g_signal_connect_after (G_OBJECT (glres_checkbutton), "toggled",
				  G_CALLBACK (button_toggled),
				  G_OBJECT (glres_combo));
	g_signal_connect (G_OBJECT (glres_combo), "changed",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (glres_combo), "changed",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
	g_signal_connect (G_OBJECT (gllib_entry), "changed",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (gllib_entry), "changed",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
	g_signal_connect (G_OBJECT (glulib_entry), "changed",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (glulib_entry), "changed",
			    G_CALLBACK (on_dirty_option),
			    reset_button);

	return gl_frame_parent;
}

static GtkWidget *
get_sdl_rendering_frame (GameOptions *target,
			 GtkWidget    *apply_button,
			 GtkWidget    *reset_button)
{
	GtkWidget *sdl_frame_parent;
	GtkWidget *sdl_frame;
	GtkWidget *sdl_table;
	
	xmame_get_options (current_exec);

	sdl_frame_parent = options_frame_new (_("SDL options"));
	sdl_frame = options_frame_create_child (sdl_frame_parent);

	sdl_table = gtk_table_new (4, 2, FALSE);
	gtk_table_set_row_spacings (GTK_TABLE (sdl_table), 6);
	gtk_table_set_col_spacings (GTK_TABLE (sdl_table), 6);
	gtk_widget_show (sdl_table);
	gtk_container_add (GTK_CONTAINER (sdl_frame), sdl_table);

	/* Full screen */
	fullscreen_checkbutton = gtk_check_button_new_with_label (_("Full Screen"));
	gtk_widget_show (fullscreen_checkbutton);
	gtk_table_attach_defaults (GTK_TABLE (sdl_table), fullscreen_checkbutton, 0, 1, 0, 1);

	/* Double buffering */
	doublebuf_checkbutton = gtk_check_button_new_with_label (_("Double Buffering"));
	gtk_widget_show (doublebuf_checkbutton);
	gtk_table_attach_defaults (GTK_TABLE (sdl_table), doublebuf_checkbutton, 0, 1, 1, 2);

	sdl_modes_checkbutton = gtk_check_button_new_with_label (_("Auto Resolution"));
	gtk_widget_show (sdl_modes_checkbutton);
	gtk_table_attach_defaults (GTK_TABLE (sdl_table), sdl_modes_checkbutton, 0, 2, 2, 3);

	sdl_modes_label = gtk_label_new (_("SDL Modes:"));
	gtk_widget_show (sdl_modes_label);
	gtk_table_attach_defaults (GTK_TABLE (sdl_table), sdl_modes_label, 0, 1, 3, 4);

	gtk_misc_set_alignment (GTK_MISC (sdl_modes_label), 0.9, 0.5);

	sdl_modes_spinbutton_adj = gtk_adjustment_new (target->modenumber, 0, 15, 1, 10, 10);
	sdl_modes_spinbutton = gtk_spin_button_new (GTK_ADJUSTMENT (sdl_modes_spinbutton_adj), 1, 0);
	gtk_table_attach_defaults (GTK_TABLE (sdl_table), sdl_modes_spinbutton, 1, 2, 4, 5);
	
	sdl_modes_combo = combo_new ("modenumber", G_TYPE_INT);
	combo_set_index(sdl_modes_combo, target->modenumber);

	gtk_table_attach_defaults (GTK_TABLE (sdl_table), sdl_modes_combo, 1, 2, 3, 4);

	/*** SDL related ***/
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (fullscreen_checkbutton), target->fullscreen);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (doublebuf_checkbutton), target->sdl_doublebuf);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (sdl_modes_checkbutton), (target->sdl_auto_mode));

	g_signal_connect (G_OBJECT (fullscreen_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (fullscreen_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
	g_signal_connect (G_OBJECT (doublebuf_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (doublebuf_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
	g_signal_connect (G_OBJECT (sdl_modes_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (sdl_modes_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
	g_signal_connect_after (G_OBJECT (sdl_modes_checkbutton), "toggled",
				  G_CALLBACK (not_button_toggled),
				  sdl_modes_label);

	/* only show the combobox if sdl_modes exist */
	if (xmame_get_option_value_count (current_exec, "") > 0) {
		gtk_widget_set_sensitive (GTK_WIDGET (sdl_modes_combo), !(target->sdl_auto_mode));
		gtk_widget_show (sdl_modes_combo);
		g_signal_connect (G_OBJECT (sdl_modes_combo), "changed",
				  G_CALLBACK (on_dirty_option),
				    apply_button);
		g_signal_connect (G_OBJECT (sdl_modes_combo), "changed",
				    G_CALLBACK (on_dirty_option),
				    reset_button);
		g_signal_connect_after (G_OBJECT (sdl_modes_checkbutton), "toggled",
					  G_CALLBACK (not_button_toggled),
					  sdl_modes_combo);
	} else {
		gtk_widget_set_sensitive (GTK_WIDGET (sdl_modes_spinbutton), !(target->sdl_auto_mode));
		gtk_widget_show (sdl_modes_spinbutton);
		g_signal_connect (G_OBJECT (sdl_modes_spinbutton), "changed",
				    G_CALLBACK (on_dirty_option),
				    apply_button);
		g_signal_connect (G_OBJECT (sdl_modes_spinbutton), "changed",
				    G_CALLBACK (on_dirty_option),
				    reset_button);
		g_signal_connect_after (G_OBJECT (sdl_modes_checkbutton), "toggled",
					  G_CALLBACK (not_button_toggled),
					  sdl_modes_spinbutton);
	}

	return sdl_frame_parent;
}

static GtkWidget *
get_fx_rendering_frame (GameOptions *target,
			GtkWidget   *apply_button,
			GtkWidget   *reset_button)
{
	GtkWidget *xfx_frame_parent;
	GtkWidget *xfx_frame;
	GtkWidget *fxglide_table;
	GtkWidget *fxresolution_label;
	int i;

	xmame_get_options (current_exec);

	xfx_frame_parent = options_frame_new (_("FX (Glide) options"));
	xfx_frame = options_frame_create_child (xfx_frame_parent);

	fxglide_table = gtk_table_new (2, 2, FALSE);
	gtk_table_set_row_spacings (GTK_TABLE (fxglide_table), 6);
	gtk_table_set_col_spacings (GTK_TABLE (fxglide_table), 6);
	gtk_widget_show (fxglide_table);
	gtk_container_add (GTK_CONTAINER (xfx_frame), fxglide_table);

	fxresolution_label = gtk_label_new (_("Resolution:"));
	gtk_widget_show (fxresolution_label);
	gtk_table_attach_defaults (GTK_TABLE (fxglide_table), fxresolution_label, 0, 1, 1, 2);
	gtk_misc_set_alignment (GTK_MISC (fxresolution_label), 0, 0.5);

	fxgkeepaspect_checkbutton = gtk_check_button_new_with_label (_("keep aspect ratio"));
	gtk_widget_show (fxgkeepaspect_checkbutton);
	gtk_table_attach_defaults (GTK_TABLE (fxglide_table), fxgkeepaspect_checkbutton, 0, 2, 0, 1);

	fx_combo = combo_new_empty(G_TYPE_STRING);
	gtk_widget_show (fx_combo);
	gtk_table_attach_defaults (GTK_TABLE (fxglide_table), fx_combo, 1, 2, 1, 2);

	combo_append_string_value(fx_combo, "", "");
	for (i=0; resolution_table[i]; i++)
		combo_append_string_value(fx_combo, resolution_table[i], resolution_table[i]);

	combo_set_key(fx_combo, target->resolution);

	/*** FX glide related ***/
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (fxgkeepaspect_checkbutton), target->fxgkeepaspect);
	gtk_widget_set_sensitive (GTK_WIDGET (sdl_modes_label), !(target->sdl_auto_mode));

	g_signal_connect (G_OBJECT (fxgkeepaspect_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (fxgkeepaspect_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
	g_signal_connect (G_OBJECT (fx_combo), "changed",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (fx_combo), "changed",
			    G_CALLBACK (on_dirty_option),
			    reset_button);

	return xfx_frame_parent;
}

static GtkWidget *
get_photon_rendering_frame (GameOptions *target,
			    GtkWidget  *apply_button,
			    GtkWidget  *reset_button)
{
	GtkWidget *photon_frame_parent;
	GtkWidget *photon_frame;
	GtkWidget *photon_table;
	
	xmame_get_options (current_exec);

	photon_frame_parent = options_frame_new (_("Photon options"));
	photon_frame = options_frame_create_child (photon_frame_parent);

	photon_table = gtk_table_new (2, 1, FALSE);
	gtk_table_set_row_spacings (GTK_TABLE (photon_table), 6);
	gtk_table_set_col_spacings (GTK_TABLE (photon_table), 6);
	gtk_widget_show (photon_table);
	gtk_container_add (GTK_CONTAINER (photon_frame), photon_table);

	render_mode_checkbutton = gtk_check_button_new_with_label (_("Full Screen"));
	gtk_widget_show (render_mode_checkbutton);
	gtk_table_attach_defaults (GTK_TABLE (photon_table), render_mode_checkbutton, 0, 1, 0, 1);

	phcursor_checkbutton = gtk_check_button_new_with_label (_("Show cursor"));
	gtk_widget_show (phcursor_checkbutton);
	gtk_table_attach_defaults (GTK_TABLE (photon_table), phcursor_checkbutton, 0, 1, 1, 2);

	/*** Photon2 related ***/
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (render_mode_checkbutton), target->render_mode);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (phcursor_checkbutton), !(target->phcursor));

	g_signal_connect (G_OBJECT (render_mode_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (render_mode_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
	g_signal_connect (G_OBJECT (phcursor_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (phcursor_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    reset_button);

	return photon_frame_parent;
}

static GtkWidget *
get_ggi_rendering_frame (GameOptions *target,
			 GtkWidget  *apply_button,
			 GtkWidget  *reset_button)
{
	GtkWidget *ggi_frame_parent;
	GtkWidget *ggi_frame;
	GtkWidget *ggi_table;
	GtkWidget *ggix_label;
	GtkWidget *ggiy_label;

	xmame_get_options (current_exec);

	ggi_frame_parent = options_frame_new (_("GGI options"));
	ggi_frame = options_frame_create_child (ggi_frame_parent);

	ggi_table = gtk_table_new (3, 1, FALSE);
	gtk_table_set_row_spacings (GTK_TABLE (ggi_table), 6);
	gtk_table_set_col_spacings (GTK_TABLE (ggi_table), 6);
	gtk_widget_show (ggi_table);
	gtk_container_add (GTK_CONTAINER (ggi_frame), ggi_table);

	ggilinear_checkbutton = gtk_check_button_new_with_label (_("linear framebuffer (fast)"));
	gtk_widget_show (ggilinear_checkbutton);
	gtk_table_attach_defaults (GTK_TABLE (ggi_table), ggilinear_checkbutton, 0, 1, 0, 1);

	ggi_force_resolution_checkbutton = gtk_check_button_new_with_label (_("Force resolutions"));
	gtk_widget_show (ggi_force_resolution_checkbutton);
	gtk_table_attach_defaults (GTK_TABLE (ggi_table), ggi_force_resolution_checkbutton, 0, 1, 1, 2);

	ggi_res_table = gtk_table_new (2, 2, FALSE);
	gtk_widget_show (ggi_res_table);
	gtk_table_attach_defaults (GTK_TABLE (ggi_table), ggi_res_table, 0, 1, 2, 3);

	ggix_label = gtk_label_new ("X:");
	gtk_widget_show (ggix_label);
	gtk_table_attach_defaults (GTK_TABLE (ggi_res_table), ggix_label, 0, 1, 0, 1);
	gtk_misc_set_alignment (GTK_MISC (ggix_label), 0, 0.5);

	ggix_spinbutton_adj = gtk_adjustment_new (target->xres, 320, 2048, 128, 128, 10);
	ggix_spinbutton = gtk_spin_button_new (GTK_ADJUSTMENT (ggix_spinbutton_adj), 1, 0);
	gtk_widget_show (ggix_spinbutton);
	gtk_table_attach_defaults (GTK_TABLE (ggi_res_table), ggix_spinbutton, 1, 2, 0, 1);

	ggiy_label = gtk_label_new ("Y:");
	gtk_widget_show (ggiy_label);
	gtk_table_attach_defaults (GTK_TABLE (ggi_res_table), ggiy_label, 2, 3, 0, 1);
	gtk_misc_set_alignment (GTK_MISC (ggiy_label), 0, 0.5);

	ggiy_spinbutton_adj = gtk_adjustment_new (target->yres, 200, 2048, 1, 256, 10);
	ggiy_spinbutton = gtk_spin_button_new (GTK_ADJUSTMENT (ggiy_spinbutton_adj), 1, 0);
	gtk_widget_show (ggiy_spinbutton);
	gtk_table_attach_defaults (GTK_TABLE (ggi_res_table), ggiy_spinbutton, 3, 4, 0, 1);

	/*** GGI related ***/
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (ggilinear_checkbutton), target->ggilinear);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (ggi_force_resolution_checkbutton), target->ggi_force_resolution);
	gtk_widget_set_sensitive (GTK_WIDGET (ggi_res_table), target->ggi_force_resolution);

	g_signal_connect (G_OBJECT (ggilinear_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (ggilinear_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
	g_signal_connect (G_OBJECT (ggi_force_resolution_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (ggi_force_resolution_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
	g_signal_connect_after (G_OBJECT (ggi_force_resolution_checkbutton), "toggled",
				  G_CALLBACK (button_toggled),
				 ggi_res_table );
	g_signal_connect (G_OBJECT (ggix_spinbutton), "changed",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (ggix_spinbutton), "changed",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
	g_signal_connect (G_OBJECT (ggiy_spinbutton), "changed",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (ggiy_spinbutton), "changed",
			    G_CALLBACK (on_dirty_option),
			    reset_button);

	return ggi_frame_parent;
}

#ifdef ENABLE_XMAME_SVGALIB_SUPPORT
static GtkWidget *
get_svga_rendering_frame (GameOptions *target,
			  GtkWidget *apply_button,
			  GtkWidget *reset_button)
{
	GtkWidget *svga_frame_parent;
	GtkWidget *svga_frame;
	GtkWidget *svga_table;
	GtkWidget *centery_label;
	GtkWidget *centerx_label;
	
	xmame_get_options (current_exec);

	svga_frame_parent = options_frame_new (_("SVGA options"));
	svga_frame = options_frame_create_child (svga_frame_parent);

	svga_table = gtk_table_new (4, 1, FALSE);
	gtk_table_set_row_spacings (GTK_TABLE (svga_table), 6);
	gtk_table_set_col_spacings (GTK_TABLE (svga_table), 6);
	gtk_widget_show (svga_table);
	gtk_container_add (GTK_CONTAINER (svga_frame), svga_table);

	planar_checkbutton = gtk_check_button_new_with_label (_("planar (modeX) modes"));
	gtk_widget_show (planar_checkbutton);
	gtk_table_attach_defaults (GTK_TABLE (svga_table), planar_checkbutton, 0, 1, 0, 1);

	linear_checkbutton = gtk_check_button_new_with_label (_("linear framebuffer (fast)"));
	gtk_widget_show (linear_checkbutton);
	gtk_table_attach_defaults (GTK_TABLE (svga_table), linear_checkbutton, 0, 1, 1, 2);

	tweaked_mode_checkbutton = gtk_check_button_new_with_label (_("Use tweaked VGA modes"));
	gtk_widget_show (tweaked_mode_checkbutton);
	gtk_table_attach_defaults (GTK_TABLE (svga_table), tweaked_mode_checkbutton, 0, 1, 2, 3);

	svga_tweak_table = gtk_table_new (2, 2, FALSE);
	gtk_widget_show (svga_tweak_table);
	gtk_table_attach_defaults (GTK_TABLE (svga_table), svga_tweak_table, 0, 1, 3, 4);

	centerx_label = gtk_label_new (_("Center X:"));
	gtk_widget_show (centerx_label);
	gtk_table_attach_defaults (GTK_TABLE (svga_tweak_table), centerx_label, 0, 1, 0, 1);
	gtk_misc_set_alignment (GTK_MISC (centerx_label), 0, 0.5);

	centerx_spinbutton_adj = gtk_adjustment_new (target->centerx, -1000, 1000, 1, 10, 10);
	centerx_spinbutton = gtk_spin_button_new (GTK_ADJUSTMENT (centerx_spinbutton_adj), 1, 0);
	gtk_widget_show (centerx_spinbutton);
	gtk_table_attach_defaults (GTK_TABLE (svga_tweak_table), centerx_spinbutton, 1, 2, 0, 1);

	centery_label = gtk_label_new (_("Center Y:"));
	gtk_widget_show (centery_label);
	gtk_table_attach_defaults (GTK_TABLE (svga_tweak_table), centery_label, 2, 3, 0, 1);
	gtk_misc_set_alignment (GTK_MISC (centery_label), 0, 0.5);

	centery_spinbutton_adj = gtk_adjustment_new (target->centery, -500, 500, 1, 10, 10);
	centery_spinbutton = gtk_spin_button_new (GTK_ADJUSTMENT (centery_spinbutton_adj), 1, 0);
	gtk_widget_show (centery_spinbutton);
	gtk_table_attach_defaults (GTK_TABLE (svga_tweak_table), centery_spinbutton, 3, 4, 0, 1);

	/*** SVGA related ***/
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (planar_checkbutton), target->planar);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (linear_checkbutton), target->linear);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (tweaked_mode_checkbutton), target->tweak);
	gtk_widget_set_sensitive (GTK_WIDGET (svga_tweak_table), target->tweak);

	g_signal_connect (G_OBJECT (planar_checkbutton), "toggled",
			  G_CALLBACK (on_dirty_option),
			  apply_button);
	g_signal_connect (G_OBJECT (planar_checkbutton), "toggled",
			  G_CALLBACK (on_dirty_option),
			  reset_button);
	g_signal_connect (G_OBJECT (linear_checkbutton), "toggled",
			  G_CALLBACK (on_dirty_option),
			  apply_button);
	g_signal_connect (G_OBJECT (linear_checkbutton), "toggled",
			  G_CALLBACK (on_dirty_option),
			  reset_button);
	g_signal_connect (G_OBJECT (tweaked_mode_checkbutton), "toggled",
			  G_CALLBACK (on_dirty_option),
			  apply_button);
	g_signal_connect (G_OBJECT (tweaked_mode_checkbutton), "toggled",
			  G_CALLBACK (on_dirty_option),
			  reset_button);
	g_signal_connect_after (G_OBJECT (tweaked_mode_checkbutton), "toggled",
				G_CALLBACK (button_toggled),
				svga_tweak_table);
	g_signal_connect (G_OBJECT (centerx_spinbutton), "changed",
			  G_CALLBACK (on_dirty_option),
			  apply_button);
	g_signal_connect (G_OBJECT (centerx_spinbutton), "changed",
			  G_CALLBACK (on_dirty_option),
			  reset_button);
	g_signal_connect (G_OBJECT (centery_spinbutton), "changed",
			  G_CALLBACK (on_dirty_option),
			  apply_button);
	g_signal_connect (G_OBJECT (centery_spinbutton), "changed",
			  G_CALLBACK (on_dirty_option),
			  reset_button);

	return svga_frame_parent;
}
#endif

void
add_rendering_options_tab (GtkWidget    *properties_windows,
                           GtkNotebook  *target_notebook,
                           RomEntry     *rom,
                           GameOptions *target,
                           GtkWidget    *apply_button,
                           GtkWidget    *reset_button)
{
	GtkWidget *image;
	GtkWidget *rendering_table;

	GtkWidget *video_mode_frame;
	GtkWidget *video_mode_table;
	GtkWidget *disablemode_label;
	
	GtkWidget *rendering_label;

	GtkWidget *rendering_frame;
	GtkWidget *rendering_frame2;
	
	gchar *used_text;
	
	xmame_get_options (current_exec);
	dirty_options_flag = FALSE;

	rendering_table = gtk_table_new (4, 1, FALSE);
	gtk_widget_show (rendering_table);
	gtk_container_set_border_width (GTK_CONTAINER (rendering_table), 10);
	gtk_table_set_row_spacings (GTK_TABLE (rendering_table), 6);
	gtk_table_set_col_spacings (GTK_TABLE (rendering_table), 6);

	rendering_frame2 = NULL;

	switch (current_exec->type) {
	case XMAME_EXEC_X11:
	case XMAME_EXEC_XGL:
		rendering_frame = get_x11_rendering_frame (target, apply_button, reset_button);
		rendering_frame2 = get_xgl_rendering_frame (target, apply_button, reset_button);
		break;
	case XMAME_EXEC_SDL:
		rendering_frame = get_sdl_rendering_frame (target, apply_button, reset_button);
		break;
	case XMAME_EXEC_XFX:
	case XMAME_EXEC_SVGAFX:
		rendering_frame = get_fx_rendering_frame (target, apply_button, reset_button);
		break;
	case XMAME_EXEC_PHOTON2:
		rendering_frame = get_photon_rendering_frame (target, apply_button, reset_button);
		break;
	case XMAME_EXEC_GGI:
		rendering_frame = get_ggi_rendering_frame (target, apply_button, reset_button);
		break;
#ifdef ENABLE_XMAME_SVGALIB_SUPPORT
	case XMAME_EXEC_SVGALIB:
		rendering_frame = get_svga_rendering_frame (target, apply_button, reset_button);
		break;
#endif
	default:
		rendering_frame = NULL;
		break;
	}

	if (rendering_frame) {
		gtk_widget_show (rendering_frame);
		gtk_table_attach (GTK_TABLE (rendering_table), rendering_frame, 0, 1, 2, 3,
				  (GTK_SHRINK | GTK_FILL),
				  (GTK_SHRINK | GTK_FILL), 0, 0);

		if (rendering_frame2) {
			gtk_widget_show (rendering_frame2);
			gtk_table_attach (GTK_TABLE (rendering_table), rendering_frame2, 1, 2, 2, 3,
					  (GTK_SHRINK | GTK_FILL),
					  (GTK_SHRINK | GTK_FILL), 0, 0);
		}
	}

	/* Video Mode */
	video_mode_frame = options_frame_new (_("Video mode"));
	gtk_widget_show (video_mode_frame);
	gtk_table_attach (GTK_TABLE (rendering_table), video_mode_frame, 0, 1, 3, 4,
			  (GTK_SHRINK | GTK_FILL),
			  (GTK_SHRINK | GTK_FILL), 0, 0);
	video_mode_frame = options_frame_create_child (video_mode_frame);

	video_mode_table = gtk_table_new (2, 2, FALSE);
	gtk_table_set_row_spacings (GTK_TABLE (video_mode_table), 6);
	gtk_table_set_col_spacings (GTK_TABLE (video_mode_table), 6);
	gtk_widget_show (video_mode_table);
	gtk_container_add (GTK_CONTAINER (video_mode_frame), video_mode_table);

	keepaspect_checkbutton = gtk_check_button_new_with_label (_("Keep aspect ratio:"));
	gtk_widget_show (keepaspect_checkbutton);
	gtk_table_attach_defaults (GTK_TABLE (video_mode_table), keepaspect_checkbutton, 0, 1, 0, 1);

	displayaspectratio_entry = gtk_entry_new ();
	gtk_widget_show (displayaspectratio_entry);
	gtk_table_attach_defaults (GTK_TABLE (video_mode_table), displayaspectratio_entry, 1, 2, 0, 1);
	gtk_entry_set_text (GTK_ENTRY (displayaspectratio_entry), "1.33");

	disablemode_label = gtk_label_new (_("Disable mode:"));
	gtk_widget_show (disablemode_label);
	gtk_table_attach_defaults (GTK_TABLE (video_mode_table), disablemode_label, 0, 1, 1, 2);
	gtk_misc_set_alignment (GTK_MISC (disablemode_label), 0, 0.5);

	disable_mode_entry = (GtkWidget *) gtk_entry_new ();
	gtk_entry_set_max_length (GTK_ENTRY (disable_mode_entry), 14);
	gtk_widget_show (disable_mode_entry);
	gtk_table_attach_defaults (GTK_TABLE (video_mode_table), disable_mode_entry, 1, 2, 1, 2);

/*end modes*/

	image = gmameui_get_image_from_stock ("gmameui-display-toolbar");

	rendering_label = gtk_hbox_new (FALSE, 5);
	gtk_box_pack_start (GTK_BOX (rendering_label), image, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (rendering_label), gtk_label_new (_("Rendering")), FALSE, FALSE, 0);
	gtk_widget_show_all (rendering_label);

	gtk_notebook_append_page (GTK_NOTEBOOK (target_notebook), rendering_table, rendering_label);

	gtk_widget_set_sensitive (GTK_WIDGET (video_mode_frame), TRUE);
	
	/*** Video Mode related ***/
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (keepaspect_checkbutton), target->keepaspect);
	gtk_widget_set_sensitive (GTK_WIDGET (displayaspectratio_entry), (target->keepaspect));
	used_text = g_strdup_printf ("%f", target->displayaspectratio);
	gtk_entry_set_text (GTK_ENTRY (displayaspectratio_entry), used_text);
	g_free (used_text);
	gtk_entry_set_text (GTK_ENTRY (disable_mode_entry), target->disablemode);

	g_signal_connect (G_OBJECT (keepaspect_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (keepaspect_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
	g_signal_connect_after (G_OBJECT (keepaspect_checkbutton), "toggled",
				  G_CALLBACK (button_toggled),
				  displayaspectratio_entry);
	g_signal_connect (G_OBJECT (displayaspectratio_entry), "changed",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (displayaspectratio_entry), "changed",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
	g_signal_connect (G_OBJECT (disable_mode_entry), "changed",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (disable_mode_entry), "changed",
			    G_CALLBACK (on_dirty_option),
			    reset_button);

}


void
add_sound_options_tab (GtkWidget    *properties_windows,
                       GtkNotebook  *target_notebook,
                       RomEntry     *rom,
                       GameOptions *target,
                       GtkWidget    *apply_button,
                       GtkWidget    *reset_button)
{
	GtkWidget *image;
	GtkWidget *mixer_plugin_label;
	GtkWidget *dsp_plugin_label;
/*	GtkWidget *arts_table;*/
	GtkWidget *artsBufferTime_label;
	GtkWidget *ms_label;
	GtkWidget *db_label;
	GtkWidget *alsa_hseparator;
	GtkWidget *sound_label;
	GtkWidget *soundfile_label;
/*	GtkWidget *soundfile_button;*/
	gchar *used_text;

	xmame_get_options (current_exec);
	dirty_options_flag = FALSE;

	image = gmameui_get_image_from_stock ("gmameui-sound-toolbar");
GMAMEUI_DEBUG ("Adding sound options tab\n");
	sound_label = gtk_hbox_new (FALSE, 5);
	gtk_box_pack_start (GTK_BOX (sound_label), image, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (sound_label), gtk_label_new (_("Sound")), FALSE, FALSE, 0);
	gtk_widget_show_all (sound_label);

	GladeXML *xml = glade_xml_new (GLADEDIR "properties.glade", "sound_hbox", NULL);
	GtkWidget *sound_hbox = glade_xml_get_widget (xml, "sound_hbox");
	gtk_notebook_append_page (GTK_NOTEBOOK (target_notebook), sound_hbox, sound_label);
	
	/* Sound enable */
	sound_checkbutton = glade_xml_get_widget (xml, "sound_checkbutton");
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (sound_checkbutton), TRUE);

	/* Samples enable */
	samples_checkbutton = glade_xml_get_widget (xml, "samples_checkbutton");
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (samples_checkbutton), TRUE);

	/* Fake Sound enable */
	fakesound_checkbutton = glade_xml_get_widget (xml, "fakesound_checkbutton");

	/* Samplerate */
	samplefreq_label = glade_xml_get_widget (xml, "samplefreq_label");
	samplefreq_combo = glade_xml_get_widget (xml, "samplefreq_combo");
	samplefreq_combo = combo_new_empty_from_glade (samplefreq_combo, G_TYPE_INT);
	combo_append_int_value(samplefreq_combo, 0, "");
	combo_append_int_value(samplefreq_combo, 8000, "8000");
	combo_append_int_value(samplefreq_combo, 11025, "11025");
	combo_append_int_value(samplefreq_combo, 16000, "16000");
	combo_append_int_value(samplefreq_combo, 22050, "22050");
	combo_append_int_value(samplefreq_combo, 44100, "44100");
	combo_append_int_value(samplefreq_combo, 48000, "48000");

	/* Volume */
	db_hscale = glade_xml_get_widget (xml, "db_hscale");
	gtk_adjustment_set_value (GTK_ADJUSTMENT (gtk_range_get_adjustment (GTK_RANGE (db_hscale))), target->volume);
	db_label = glade_xml_get_widget (xml, "db_label");
	
	/* Buffer Size */
	bufsize_hscale = glade_xml_get_widget (xml, "bufsize_hscale");
	gtk_adjustment_set_value (GTK_ADJUSTMENT (gtk_range_get_adjustment (GTK_RANGE (bufsize_hscale))), target->bufsize);
	ms_label = glade_xml_get_widget (xml, "ms_label");

	/* Audio device */
	audiodevice_label = glade_xml_get_widget (xml, "audiodevice_label");

	audiodevice_combo = glade_xml_get_widget (xml, "audiodevice_combo");

	/* Mixer device */
	mixerdevice_label = glade_xml_get_widget (xml, "mixerdevice_label");

	mixerdevice_combo = glade_xml_get_widget (xml, "mixerdevice_combo");
		
	/* DSP plugin */
	dsp_plugin_label = glade_xml_get_widget (xml, "dsp_plugin_label");

	dsp_plugin_combo = glade_xml_get_widget (xml, "dsp_plugin_combo");
	dsp_plugin_combo = combo_new_from_glade (dsp_plugin_combo, "dsp-plugin", G_TYPE_STRING);
	combo_set_key(dsp_plugin_combo, target->dsp_plugin);

	
	/* Mixer plugin */
	mixer_plugin_label = glade_xml_get_widget (xml, "mixer_plugin_label");
	
	mixer_plugin_combo = glade_xml_get_widget (xml, "mixer_plugin_combo");
	mixer_plugin_combo = combo_new_from_glade (mixer_plugin_combo, "sound-mixer-plugin", G_TYPE_STRING);
	combo_set_key(mixer_plugin_combo, target->sound_mixer_plugin);

	/* soundfile */
	soundfile_label = glade_xml_get_widget (xml, "soundfile_label");

	soundfile_entry = glade_xml_get_widget (xml, "soundfile_entry");

	/* timer */
	timer_checkbutton = glade_xml_get_widget (xml, "timer_checkbutton");

	/* aRTs options */
	arts_spinbutton = glade_xml_get_widget (xml, "arts_spinbutton");
	gtk_spin_button_set_value (GTK_SPIN_BUTTON (arts_spinbutton), target->artsBufferTime);

	artsBufferTime_label = glade_xml_get_widget (xml, "artsBufferTime_label");

	/* QNX options */
	audio_preferred_checkbutton = glade_xml_get_widget (xml, "audio_preferred_checkbutton");
	
	/* Alsa options */
	alsa_frame = glade_xml_get_widget (xml, "alsa_frame");
	PCM_label = glade_xml_get_widget (xml, "PCM_label");
	
	/*workaround to allow correct translation of "" with gettext */
	used_text = g_strdup_printf (_("Buffer size %s")," (s):");
	alsa_buffer_label = glade_xml_get_widget (xml, "alsa_buffer_label");
	gtk_label_set_text (GTK_LABEL (alsa_buffer_label), used_text);
	g_free (used_text);
	
	alsa_hseparator = glade_xml_get_widget (xml, "alsa_hseparator");
	
	alsacard_label = glade_xml_get_widget (xml, "alsacard_label");

	alsadevice_label = glade_xml_get_widget (xml, "alsadevice_label");

	alsa_buffer_spinbutton = glade_xml_get_widget (xml, "alsa_buffer_spinbutton");
	gtk_spin_button_set_value (GTK_SPIN_BUTTON (alsa_buffer_spinbutton), target->alsa_buffer);
	
	alsacard_spinbutton = glade_xml_get_widget (xml, "alsacard_spinbutton");
	gtk_spin_button_set_value (GTK_SPIN_BUTTON (alsacard_spinbutton), target->alsacard);

	alsadevice_spinbutton = glade_xml_get_widget (xml, "alsadevice_spinbutton");
	gtk_spin_button_set_value (GTK_SPIN_BUTTON (alsadevice_spinbutton), target->alsadevice);
	
	pcm_entry = glade_xml_get_widget (xml, "pcm_entry");

	/* end sound options */

	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (sound_checkbutton), target->sound);
	gtk_widget_set_sensitive (GTK_WIDGET (samples_checkbutton), target->sound);

	gtk_widget_set_sensitive (GTK_WIDGET (db_hscale), target->sound);
	gtk_widget_set_sensitive (GTK_WIDGET (db_label), target->sound);
	gtk_widget_set_sensitive (GTK_WIDGET (bufsize_hscale), target->sound);
	gtk_widget_set_sensitive (GTK_WIDGET (ms_label), target->sound);
	
	gtk_widget_set_sensitive (GTK_WIDGET (samplefreq_combo), target->sound);
	gtk_widget_set_sensitive (GTK_WIDGET (audiodevice_label), target->sound);
	gtk_widget_set_sensitive (GTK_WIDGET (samplefreq_label), target->sound);
	gtk_widget_set_sensitive (GTK_WIDGET (mixerdevice_label), target->sound);
	gtk_widget_set_sensitive (GTK_WIDGET (audiodevice_combo), target->sound);
	gtk_widget_set_sensitive (GTK_WIDGET (mixerdevice_combo), target->sound);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (samples_checkbutton), target->samples);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (fakesound_checkbutton), target->fakesound);

	combo_set_int_key(samplefreq_combo, target->samplefre);

	if (target->audiodevice)
		gtk_entry_set_text (GTK_ENTRY (GTK_BIN(audiodevice_combo)->child), target->audiodevice);

	if (target->mixerdevice)
		gtk_entry_set_text (GTK_ENTRY (GTK_BIN(mixerdevice_combo)->child), target->mixerdevice);

	/*gtk_entry_set_text (GTK_ENTRY (soundfile_entry), target->soundfile);*/
	printf("target sound filename is %s\n", target->soundfile);/* TODO */
	gtk_file_chooser_set_current_name (GTK_FILE_CHOOSER (soundfile_entry), target->soundfile);

	if (target->dsp_plugin && !strcmp (target->dsp_plugin, "waveout"))
	{
		gtk_widget_set_sensitive (GTK_WIDGET (soundfile_entry), TRUE);
		gtk_widget_set_sensitive (GTK_WIDGET (audiodevice_combo), FALSE);
	}
	else
	{
		gtk_widget_set_sensitive (GTK_WIDGET (soundfile_entry), FALSE);
		gtk_widget_set_sensitive (GTK_WIDGET (audiodevice_combo), TRUE);
	}


	combo_set_key(dsp_plugin_combo, target->dsp_plugin);
	combo_set_key(mixer_plugin_combo, target->sound_mixer_plugin);

	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (timer_checkbutton), target->timer);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (audio_preferred_checkbutton), target->audio_preferred);

	gtk_entry_set_text (GTK_ENTRY (pcm_entry), target->alsa_pcm);

	gtk_widget_set_sensitive (GTK_WIDGET (audio_preferred_checkbutton),
				  xmame_has_option (current_exec, "audio_preferred"));

	gtk_widget_set_sensitive (GTK_WIDGET (alsa_frame),
				  xmame_get_option_key_value (current_exec, "dsp-plugin", "alsa") != NULL);
	
	gtk_widget_set_sensitive (GTK_WIDGET (arts_spinbutton),
				  xmame_has_option (current_exec, "artsBufferTime"));
	gtk_widget_set_sensitive (GTK_WIDGET (alsacard_label),
				  xmame_has_option (current_exec, "alsacard"));

	gtk_widget_set_sensitive (GTK_WIDGET (alsacard_spinbutton),
				  xmame_has_option (current_exec, "alsacard"));

	gtk_widget_set_sensitive (GTK_WIDGET (alsadevice_label),
				  xmame_has_option (current_exec, "alsadevice"));

	gtk_widget_set_sensitive (GTK_WIDGET (alsadevice_spinbutton),
				  xmame_has_option (current_exec, "alsadevice"));

	gtk_widget_set_sensitive (GTK_WIDGET (alsa_buffer_label),
				  xmame_has_option (current_exec, "alsa-buffer"));

	gtk_widget_set_sensitive (GTK_WIDGET (alsa_buffer_spinbutton),
				  xmame_has_option (current_exec, "alsa-buffer"));

	gtk_widget_set_sensitive (GTK_WIDGET (PCM_label),
				  xmame_has_option (current_exec, "alsa-pcm"));

	gtk_widget_set_sensitive (GTK_WIDGET (pcm_entry),
				  xmame_has_option (current_exec, "alsa-pcm"));

	g_signal_connect_after (G_OBJECT (sound_checkbutton), "toggled",
				  G_CALLBACK (button_toggled),
				  G_OBJECT (samples_checkbutton));
	g_signal_connect_after (G_OBJECT (sound_checkbutton), "toggled",
				  G_CALLBACK (button_toggled),
				  G_OBJECT (volume_frame));
	g_signal_connect_after (G_OBJECT (sound_checkbutton), "toggled",
				  G_CALLBACK (button_toggled),
				  G_OBJECT (bufsize_frame));
	g_signal_connect_after (G_OBJECT (sound_checkbutton), "toggled",
				  G_CALLBACK (button_toggled),
				  G_OBJECT (samplefreq_combo));
	g_signal_connect_after (G_OBJECT (sound_checkbutton), "toggled",
				  G_CALLBACK (button_toggled),
				  G_OBJECT (audiodevice_combo));
	g_signal_connect_after (G_OBJECT (sound_checkbutton), "toggled",
				  G_CALLBACK (button_toggled),
				  G_OBJECT (mixerdevice_combo));
	g_signal_connect_after (G_OBJECT (sound_checkbutton), "toggled",
				  G_CALLBACK (button_toggled),
				  G_OBJECT (samplefreq_label));
	g_signal_connect_after (G_OBJECT (sound_checkbutton), "toggled",
				  G_CALLBACK (button_toggled),
				  G_OBJECT (audiodevice_label));
	g_signal_connect_after (G_OBJECT (sound_checkbutton), "toggled",
				  G_CALLBACK (button_toggled),
				  G_OBJECT (mixerdevice_label));

	g_signal_connect_after (G_OBJECT (dsp_plugin_combo), "changed",
			    G_CALLBACK (file_or_device),
			    NULL);
GMAMEUI_DEBUG ("Finished adding sound options tab\n");
}

void
add_controller_options_tab (GtkWidget    *properties_windows,
			    GtkNotebook  *target_notebook,
			    RomEntry     *rom,
			    GameOptions  *target,
			    GtkWidget    *apply_button,
			    GtkWidget    *reset_button)
{
	GtkWidget *controllers_vbox;
	GtkWidget *keyboard_frame;
	GtkWidget *keyboard_table;
	GtkWidget *keymapping_frame;
	GtkWidget *keymapping_table;
	GtkWidget *ctrlr_frame;
	GtkWidget *ctrlr_table;
	GtkWidget *ctrlr_label;
	GList *ctrlr_list = NULL;
	GList *my_list = NULL;
	GtkWidget *keymaptype_label;
	GtkWidget *mouse_frame;
	GtkWidget *mouse_table;
	GtkWidget *image;
	GtkWidget *Xinput_trackball_frame;
	GtkWidget *Xinput_trackball_table;
	GtkWidget *XInput_trackball1_label;
	GtkWidget *XInput_trackball2_label;
	GtkWidget *XInput_trackball3_label;
	GtkWidget *XInput_trackball4_label;
	GtkWidget *joystick_frame;
	GtkWidget *joystick_table;
	GtkWidget *joytype_label;
	GtkWidget *xinput_joy_table;
	GtkWidget *XInput_joystick1_label;
	GtkWidget *XInput_joystick2_label;
	GtkWidget *XInput_joystick3_label;
	GtkWidget *XInput_joystick4_label;
	GtkWidget *control_label;
	int i;
	GtkSizeGroup *group = gtk_size_group_new (GTK_SIZE_GROUP_HORIZONTAL);

	xmame_get_options (current_exec);
	dirty_options_flag = FALSE;

	image = gmameui_get_image_from_stock ("gmameui-joystick-toolbar");

	control_label = gtk_hbox_new (FALSE, 5);
	gtk_box_pack_start (GTK_BOX (control_label), image, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (control_label), gtk_label_new (_("Controllers")), FALSE, FALSE, 0);
	gtk_widget_show_all (control_label);

	controllers_vbox = gtk_vbox_new (FALSE, 6);
	gtk_widget_show (controllers_vbox);
	gtk_notebook_append_page (GTK_NOTEBOOK (target_notebook), controllers_vbox, control_label);

/*** Joystick ***/
	joystick_frame = options_frame_new (_("Joystick"));
	gtk_widget_show (joystick_frame);
	gtk_box_pack_start (GTK_BOX (controllers_vbox), joystick_frame, FALSE, FALSE, 0);

	joystick_table = gtk_table_new (5, 7, FALSE);
	gtk_table_set_row_spacings (GTK_TABLE (joystick_table), 6);
	gtk_table_set_col_spacings (GTK_TABLE (joystick_table), 6);
	gtk_widget_show (joystick_table);
	gtk_container_add (GTK_CONTAINER (joystick_frame), joystick_table);

	image = gmameui_get_image_from_stock ("gmameui-joystick");
	gtk_widget_show (image);
	gtk_table_attach (GTK_TABLE (joystick_table), image, 0, 1, 0, 5, 0, GTK_EXPAND | GTK_FILL, 0, 0);
	gtk_misc_set_alignment (GTK_MISC (image), 0.5, 0);
	gtk_misc_set_padding (GTK_MISC (image), 5, 5);
	gtk_size_group_add_widget (GTK_SIZE_GROUP (group), image);

/* Joytype */
	joytype_label = gtk_label_new (_("Joystick type:"));
	gtk_widget_show (joytype_label);
	gtk_table_attach_defaults (GTK_TABLE (joystick_table), joytype_label, 1, 2, 0, 1);
	gtk_misc_set_alignment (GTK_MISC (joytype_label), 0, 0.5);

	joytype_combo = combo_new ("joytype", G_TYPE_INT);
	gtk_widget_show (joytype_combo);
	gtk_table_attach_defaults (GTK_TABLE (joystick_table), joytype_combo, 2, 4, 0, 1);

/* joystick prefix */
	joydevname_label = gtk_label_new (_("Joystick device prefix:"));
	gtk_widget_show (joydevname_label);
	gtk_table_attach_defaults (GTK_TABLE (joystick_table), joydevname_label, 1, 3, 1, 2);
	gtk_misc_set_alignment (GTK_MISC (joydevname_label), 0, 0.5);

	joydevname_entry = (GtkWidget *) gtk_entry_new ();
	gtk_entry_set_max_length (GTK_ENTRY (joydevname_entry), 20);
	gtk_widget_show (joydevname_entry);
	gtk_table_attach_defaults (GTK_TABLE (joystick_table), joydevname_entry, 3, 4, 1, 2);
	gtk_entry_set_text (GTK_ENTRY (joydevname_entry), "/dev/joy");

/* FM Townpad */
	paddevname_label = gtk_label_new (_("Name of pad device:\n (FM townpad)"));
	gtk_widget_show (paddevname_label);
	gtk_table_attach_defaults (GTK_TABLE (joystick_table), paddevname_label, 1, 3, 2, 3);
	gtk_label_set_justify (GTK_LABEL (paddevname_label), GTK_JUSTIFY_LEFT);
	gtk_misc_set_alignment (GTK_MISC (paddevname_label), 0, 0.5);

	paddevname_entry = gtk_entry_new ();
	gtk_widget_show (paddevname_entry);
	gtk_table_attach_defaults (GTK_TABLE (joystick_table), paddevname_entry, 3, 4, 2, 3);
	gtk_entry_set_text (GTK_ENTRY (paddevname_entry), "/dev/pad00");

/* X-based joy */
	x11joyname_label = gtk_label_new (_("X-based device:"));
	gtk_widget_show (x11joyname_label);
	gtk_table_attach_defaults (GTK_TABLE (joystick_table), x11joyname_label, 1, 3, 3, 4);
	gtk_misc_set_alignment (GTK_MISC (x11joyname_label), 0, 0.5);

	x11joyname_entry = gtk_entry_new ();
	gtk_widget_show (x11joyname_entry);
	gtk_table_attach_defaults (GTK_TABLE (joystick_table), x11joyname_entry, 3, 4, 3, 4);

/* analog joystick */
	analogstick_checkbutton = gtk_check_button_new_with_label (_("Analog joystick"));
	gtk_widget_show (analogstick_checkbutton);
	gtk_table_attach_defaults (GTK_TABLE (joystick_table), analogstick_checkbutton, 5, 6, 0, 1);

/* rapid fire */
	rapidfire_checkbutton = gtk_check_button_new_with_label (_("Rapid-fire"));
	gtk_widget_show (rapidfire_checkbutton);
	gtk_table_attach_defaults (GTK_TABLE (joystick_table), rapidfire_checkbutton, 5, 6, 1, 2);

/* USB PAD*/
	usbpspad_checkbutton = gtk_check_button_new_with_label (_("USB PS Game Pads"));
	gtk_widget_show (usbpspad_checkbutton);
	gtk_table_attach_defaults (GTK_TABLE (joystick_table), usbpspad_checkbutton, 5, 6, 2, 3);

/* Xinput */
	Xinput_joy_frame = options_frame_new (_("Xinput joystick"));
	gtk_widget_show (Xinput_joy_frame);
	gtk_table_attach_defaults (GTK_TABLE (joystick_table), Xinput_joy_frame, 1, 7, 4, 5);
	Xinput_joy_frame = options_frame_create_child (Xinput_joy_frame);

	xinput_joy_table = gtk_table_new (2, 4, FALSE);
	gtk_table_set_row_spacings (GTK_TABLE (xinput_joy_table), 6);
	gtk_table_set_col_spacings (GTK_TABLE (xinput_joy_table), 6);
	gtk_widget_show (xinput_joy_table);
	gtk_container_add (GTK_CONTAINER (Xinput_joy_frame), xinput_joy_table);

	XInput_joystick1_label = gtk_label_new ("1:");
	gtk_widget_show (XInput_joystick1_label);
	gtk_table_attach (GTK_TABLE (xinput_joy_table), XInput_joystick1_label, 0, 1, 0, 1, 0, 0, 0, 0);
	gtk_misc_set_alignment (GTK_MISC (XInput_joystick1_label), 0, 0.5);

	XInput_joystick1_entry = (GtkWidget *) gtk_entry_new ();
	gtk_entry_set_max_length (GTK_ENTRY (XInput_joystick1_entry), 20);
	gtk_widget_show (XInput_joystick1_entry);
	gtk_table_attach_defaults (GTK_TABLE (xinput_joy_table), XInput_joystick1_entry, 1, 2, 0, 1);

	XInput_joystick2_label = gtk_label_new ("2:");
	gtk_widget_show (XInput_joystick2_label);
	gtk_table_attach (GTK_TABLE (xinput_joy_table), XInput_joystick2_label, 2, 3, 0, 1, 0, 0, 0, 0);
	gtk_misc_set_alignment (GTK_MISC (XInput_joystick2_label), 0, 0.5);

	XInput_joystick3_label = gtk_label_new ("3:");
	gtk_widget_show (XInput_joystick3_label);
	gtk_table_attach (GTK_TABLE (xinput_joy_table), XInput_joystick3_label, 0, 1, 1, 2, 0, 0, 0, 0);
	gtk_misc_set_alignment (GTK_MISC (XInput_joystick3_label), 0, 0.5);

	XInput_joystick4_label = gtk_label_new ("4:");
	gtk_widget_show (XInput_joystick4_label);
	gtk_table_attach (GTK_TABLE (xinput_joy_table), XInput_joystick4_label, 2, 3, 1, 2, 0, 0, 0, 0);
	gtk_misc_set_alignment (GTK_MISC (XInput_joystick4_label), 0, 0.5);

	XInput_joystick3_entry = (GtkWidget *) gtk_entry_new ();
	gtk_entry_set_max_length (GTK_ENTRY (XInput_joystick3_entry), 20);
	gtk_widget_show (XInput_joystick3_entry);
	gtk_table_attach_defaults (GTK_TABLE (xinput_joy_table), XInput_joystick3_entry, 1, 2, 1, 2);

	XInput_joystick2_entry = (GtkWidget *) gtk_entry_new ();
	gtk_entry_set_max_length (GTK_ENTRY (XInput_joystick2_entry), 20);
	gtk_widget_show (XInput_joystick2_entry);
	gtk_table_attach_defaults (GTK_TABLE (xinput_joy_table), XInput_joystick2_entry, 3, 4, 0, 1);

	XInput_joystick4_entry = (GtkWidget *) gtk_entry_new ();
	gtk_entry_set_max_length (GTK_ENTRY (XInput_joystick4_entry), 20);
	gtk_widget_show (XInput_joystick4_entry);
	gtk_table_attach_defaults (GTK_TABLE (xinput_joy_table), XInput_joystick4_entry, 3, 4, 1, 2);

/*** Mouse ***/
	mouse_frame = options_frame_new (_("Mouse / Trackball"));
	gtk_widget_show (mouse_frame);
	gtk_box_pack_start (GTK_BOX (controllers_vbox), mouse_frame, FALSE, FALSE, 0);

	mouse_table = gtk_table_new (2, 3, FALSE);
	gtk_table_set_row_spacings (GTK_TABLE (mouse_table), 6);
	gtk_table_set_col_spacings (GTK_TABLE (mouse_table), 6);
	gtk_widget_show (mouse_table);
	gtk_container_add (GTK_CONTAINER (mouse_frame), mouse_table);

	image = gmameui_get_image_from_stock ("gmameui-mouse");
	gtk_widget_show (image);
	gtk_table_attach (GTK_TABLE (mouse_table), image, 0, 1, 0, 2, 0, GTK_EXPAND | GTK_FILL, 0, 0);
	gtk_misc_set_alignment (GTK_MISC (image), 0.5, 0);
	gtk_misc_set_padding (GTK_MISC (image), 5, 5);
	gtk_size_group_add_widget (GTK_SIZE_GROUP (group), image);

/* use mouse */
	mouse_checkbutton = gtk_check_button_new_with_label (_("Use mouse"));
	gtk_widget_show (mouse_checkbutton);
	gtk_table_attach_defaults (GTK_TABLE (mouse_table), mouse_checkbutton, 1, 2, 0, 1);

/* grab mouse */
	grabmouse_checkbutton = gtk_check_button_new_with_label (_("Grab mouse"));
	gtk_widget_show (grabmouse_checkbutton);
	gtk_table_attach_defaults (GTK_TABLE (mouse_table), grabmouse_checkbutton, 2, 3, 0, 1);

/* Xinput */
	Xinput_trackball_frame = options_frame_new (_("Xinput trackball"));
	gtk_widget_show (Xinput_trackball_frame);
	gtk_table_attach_defaults (GTK_TABLE (mouse_table), Xinput_trackball_frame, 1, 3, 1, 2);
	Xinput_trackball_frame = options_frame_create_child (Xinput_trackball_frame);

	Xinput_trackball_table = gtk_table_new (2, 4, FALSE);
	gtk_table_set_row_spacings (GTK_TABLE (Xinput_trackball_table), 6);
	gtk_table_set_col_spacings (GTK_TABLE (Xinput_trackball_table), 6);
	gtk_widget_show (Xinput_trackball_table);
	gtk_container_add (GTK_CONTAINER (Xinput_trackball_frame), Xinput_trackball_table);

	XInput_trackball1_label = gtk_label_new ("1:");
	gtk_widget_show (XInput_trackball1_label);
	gtk_table_attach (GTK_TABLE (Xinput_trackball_table), XInput_trackball1_label, 0, 1, 0, 1, 0, 0, 0, 0);
	gtk_misc_set_alignment (GTK_MISC (XInput_trackball1_label), 0, 0.5);

	XInput_trackball1_entry = (GtkWidget *) gtk_entry_new ();
	gtk_entry_set_max_length (GTK_ENTRY (XInput_trackball1_entry), 20);
	gtk_widget_show (XInput_trackball1_entry);
	gtk_table_attach_defaults (GTK_TABLE (Xinput_trackball_table), XInput_trackball1_entry, 1, 2, 0, 1);

	XInput_trackball2_label = gtk_label_new ("2:");
	gtk_widget_show (XInput_trackball2_label);
	gtk_table_attach (GTK_TABLE (Xinput_trackball_table), XInput_trackball2_label, 2, 3, 0, 1, 0, 0, 0, 0);
	gtk_misc_set_alignment (GTK_MISC (XInput_trackball2_label), 0, 0.5);

	XInput_trackball3_label = gtk_label_new ("3:");
	gtk_widget_show (XInput_trackball3_label);
	gtk_table_attach (GTK_TABLE (Xinput_trackball_table), XInput_trackball3_label, 0, 1, 1, 2, 0, 0, 0, 0);
	gtk_misc_set_alignment (GTK_MISC (XInput_trackball3_label), 0, 0.5);

	XInput_trackball4_label = gtk_label_new ("4:");
	gtk_widget_show (XInput_trackball4_label);
	gtk_table_attach (GTK_TABLE (Xinput_trackball_table), XInput_trackball4_label, 2, 3, 1, 2, 0, 0, 0, 0);
	gtk_misc_set_alignment (GTK_MISC (XInput_trackball4_label), 0, 0.5);

	XInput_trackball3_entry = (GtkWidget *) gtk_entry_new ();
	gtk_entry_set_max_length (GTK_ENTRY (XInput_trackball3_entry), 20);
	gtk_widget_show (XInput_trackball3_entry);
	gtk_table_attach_defaults (GTK_TABLE (Xinput_trackball_table), XInput_trackball3_entry, 1, 2, 1, 2);

	XInput_trackball2_entry = (GtkWidget *) gtk_entry_new ();
	gtk_entry_set_max_length (GTK_ENTRY (XInput_trackball2_entry), 20);
	gtk_widget_show (XInput_trackball2_entry);
	gtk_table_attach_defaults (GTK_TABLE (Xinput_trackball_table), XInput_trackball2_entry, 3, 4, 0, 1);

	XInput_trackball4_entry = (GtkWidget *) gtk_entry_new ();
	gtk_entry_set_max_length (GTK_ENTRY (XInput_trackball4_entry), 20);
	gtk_widget_show (XInput_trackball4_entry);
	gtk_table_attach_defaults (GTK_TABLE (Xinput_trackball_table), XInput_trackball4_entry, 3, 4, 1, 2);

/*** Keyboard ***/
	keyboard_frame = options_frame_new (_("Keyboard"));
	gtk_widget_show (keyboard_frame);
	gtk_box_pack_start (GTK_BOX (controllers_vbox), keyboard_frame, FALSE, FALSE, 0);

	keyboard_table = gtk_table_new (3, 4, FALSE);
	gtk_table_set_row_spacings (GTK_TABLE (keyboard_table), 6);
	gtk_table_set_col_spacings (GTK_TABLE (keyboard_table), 6);
	gtk_widget_show (keyboard_table);
	gtk_container_add (GTK_CONTAINER (keyboard_frame), keyboard_table);

	image = gmameui_get_image_from_stock ("gmameui-keyboard");
	gtk_widget_show (image);
	gtk_table_attach (GTK_TABLE (keyboard_table), image, 0, 1, 0, 3, 0, GTK_EXPAND | GTK_FILL, 0, 0);
	gtk_misc_set_alignment (GTK_MISC (image), 0.5, 0);
	gtk_misc_set_padding (GTK_MISC (image), 5, 5);
	gtk_size_group_add_widget (GTK_SIZE_GROUP (group), image);

/* use config */
 	config_name_checkbutton = gtk_check_button_new_with_label (_("Use config name:"));
 	gtk_widget_show (config_name_checkbutton);
 	gtk_table_attach_defaults (GTK_TABLE (keyboard_table), config_name_checkbutton, 1, 2, 0, 1);

	config_name_entry = (GtkWidget *) gtk_entry_new ();
	gtk_entry_set_max_length (GTK_ENTRY (config_name_entry), 20);
	gtk_widget_show (config_name_entry);
	gtk_table_attach_defaults (GTK_TABLE (keyboard_table), config_name_entry, 2, 3, 0, 1);

	/* Ctrlr */
	ctrlr_frame = options_frame_new (_("Control Panel"));
	gtk_widget_show (ctrlr_frame);
	gtk_table_attach_defaults (GTK_TABLE (keyboard_table), ctrlr_frame, 3, 4, 0, 3);
	ctrlr_frame = options_frame_create_child (ctrlr_frame);

	ctrlr_table = gtk_table_new (2, 3, FALSE);
	gtk_table_set_row_spacings (GTK_TABLE (ctrlr_table), 6);
	gtk_table_set_col_spacings (GTK_TABLE (ctrlr_table), 6);
	gtk_widget_show (ctrlr_table);
	gtk_container_add (GTK_CONTAINER (ctrlr_frame), ctrlr_table);

	ctrlr_label = gtk_label_new (_("Controller:"));
	gtk_widget_show (ctrlr_label);
	gtk_table_attach_defaults (GTK_TABLE (ctrlr_table), ctrlr_label, 0, 1, 0, 1);
	gtk_misc_set_alignment (GTK_MISC (ctrlr_label), 0, 0.5);

	ctrlr_combo = combo_new_empty(G_TYPE_STRING);
	gtk_widget_show (ctrlr_combo);
	gtk_table_attach_defaults (GTK_TABLE (ctrlr_table), ctrlr_combo, 1, 2, 0, 1);
	
	combo_append_string_value(ctrlr_combo, "", "");
	/* Getting ctrlr list */
	ctrlr_list = get_ctrlr_list ();
	for (my_list = g_list_first (ctrlr_list);
		  (my_list != NULL);
		  my_list = g_list_next (my_list))
	{
		/* combo box makes a private copy of the text so it is safe to pass a pointer */
		combo_append_string_value(ctrlr_combo, (gchar*) (my_list->data), (gchar*) (my_list->data));
	}
	g_list_free (ctrlr_list);

/* HotRod */
	hotrod_checkbutton = gtk_check_button_new_with_label (_("HotRod support"));
	gtk_widget_show (hotrod_checkbutton);
	gtk_table_attach_defaults (GTK_TABLE (ctrlr_table), hotrod_checkbutton, 0, 2, 1, 2);

/* HotRod Second Edition */
	hotrodse_checkbutton = gtk_check_button_new_with_label (_("HotRod SE support"));
	gtk_widget_show (hotrodse_checkbutton);
	gtk_table_attach_defaults (GTK_TABLE (ctrlr_table), hotrodse_checkbutton, 0, 2, 2, 3);

/* grab keyboard */
	grabkeyboard_checkbutton = gtk_check_button_new_with_label (_("Grab keyboard"));
	gtk_widget_show (grabkeyboard_checkbutton);
	gtk_table_attach_defaults (GTK_TABLE (keyboard_table), grabkeyboard_checkbutton, 1, 2, 1, 2);

/* winkeys */
	winkeys_checkbutton = gtk_check_button_new_with_label (_("Enable Windows keys"));
	gtk_widget_show (winkeys_checkbutton);
	gtk_table_attach_defaults (GTK_TABLE (keyboard_table), winkeys_checkbutton, 2, 3, 1, 2);

/* key mapping... one day maybe :-) */
	keymapping_frame = options_frame_new (_("Key mapping"));
	gtk_widget_show (keymapping_frame);
	gtk_table_attach_defaults (GTK_TABLE (keyboard_table), keymapping_frame, 1, 3, 2, 3);
	keymapping_frame = options_frame_create_child (keymapping_frame);

	keymapping_table = gtk_table_new (2, 1, FALSE);
	gtk_widget_show (keymapping_table);
	gtk_container_add (GTK_CONTAINER (keymapping_frame), keymapping_table);

	keymaptype_label = gtk_label_new (_("Keyboard Layout type:"));
	gtk_widget_show (keymaptype_label);
	gtk_table_attach_defaults (GTK_TABLE (keymapping_table), keymaptype_label, 0, 1, 0, 1);
	gtk_misc_set_alignment (GTK_MISC (keymaptype_label), 0, 0.5);

	keymaptype_combo = combo_new_empty(G_TYPE_STRING);

	gtk_widget_show (keymaptype_combo);
	gtk_table_attach_defaults (GTK_TABLE (keymapping_table), keymaptype_combo, 1, 2, 0, 1);
	

	combo_append_string_value(keymaptype_combo, "", _("Default Layout"));

	for (i=0;x11_keymaps_layout[i].shortname;i++)
	{
		combo_append_string_value(keymaptype_combo, x11_keymaps_layout[i].shortname, _(x11_keymaps_layout[i].name));
	}
	
/* ugcicoin */
	ugcicoin_checkbutton = gtk_check_button_new_with_label (_("UGCI (tm) Coin/Play support"));
	gtk_widget_show (ugcicoin_checkbutton);
	gtk_table_attach_defaults (GTK_TABLE (joystick_table), ugcicoin_checkbutton, 5, 7, 3, 4);

/* end controllers*/

	gtk_widget_set_sensitive (paddevname_label, FALSE);
	gtk_widget_set_sensitive (paddevname_entry, FALSE);
	gtk_widget_set_sensitive (x11joyname_label, FALSE);
	gtk_widget_set_sensitive (x11joyname_entry, FALSE);
	gtk_widget_set_sensitive (joydevname_label, FALSE);
	gtk_widget_set_sensitive (joydevname_entry, FALSE);

	switch (target->joytype)
	{
		case (0): /* no joystick */
			gtk_widget_set_sensitive (analogstick_checkbutton, FALSE);
			gtk_widget_set_sensitive (usbpspad_checkbutton, FALSE);
			gtk_widget_set_sensitive (rapidfire_checkbutton, FALSE);
			break;
		case (1): /* i386 style */
		case (4): /* new i386 linux 1.x.x driver */
			gtk_widget_set_sensitive (joydevname_label, TRUE);
			gtk_widget_set_sensitive (joydevname_entry, TRUE);
			break;
		case (2): /* Fm Town Pad */
			gtk_widget_set_sensitive (paddevname_label, TRUE);
			gtk_widget_set_sensitive (paddevname_entry, TRUE);
			break;
		case (3): /* X11 input extension */
			gtk_widget_set_sensitive (x11joyname_label, TRUE);
			gtk_widget_set_sensitive (x11joyname_entry, TRUE);
			break;
	}


	combo_set_index(joytype_combo, target->joytype);

	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (analogstick_checkbutton), target->analogstick);

	combo_set_key(ctrlr_combo, target->ctrlr);
	
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (hotrod_checkbutton), target->hotrod);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (hotrodse_checkbutton), target->hotrodse);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (usbpspad_checkbutton), target->usbpspad);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (rapidfire_checkbutton), target->rapidfire);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (ugcicoin_checkbutton), target->ugcicoin);
	gtk_entry_set_text (GTK_ENTRY (joydevname_entry), target->joydevname);
	gtk_entry_set_text (GTK_ENTRY (paddevname_entry), target->paddevname);
	gtk_entry_set_text (GTK_ENTRY (x11joyname_entry), target->x11joyname);
/*	gtk_entry_set_text (GTK_ENTRY (XInput_joystick1_entry), target->XInput_joystick1);
	gtk_entry_set_text (GTK_ENTRY (XInput_joystick2_entry), target->XInput_joystick2);
	gtk_entry_set_text (GTK_ENTRY (XInput_joystick3_entry), target->XInput_joystick3);
	gtk_entry_set_text (GTK_ENTRY (XInput_joystick4_entry), target->XInput_joystick4);*/
	
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (mouse_checkbutton), target->mouse);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (grabmouse_checkbutton), target->grabmouse);
	gtk_entry_set_text (GTK_ENTRY (XInput_trackball1_entry), target->XInput_trackball1);
	gtk_entry_set_text (GTK_ENTRY (XInput_trackball2_entry), target->XInput_trackball2);
	gtk_entry_set_text (GTK_ENTRY (XInput_trackball3_entry), target->XInput_trackball3);
	gtk_entry_set_text (GTK_ENTRY (XInput_trackball4_entry), target->XInput_trackball4);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (grabkeyboard_checkbutton), target->grabkeyboard);
	
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (winkeys_checkbutton), target->winkeys);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (config_name_checkbutton), target->cfgname_flag);
	gtk_entry_set_text (GTK_ENTRY (config_name_entry), target->cfgname);

	gtk_widget_set_sensitive (GTK_WIDGET (ctrlr_combo), 
				  xmame_has_option (current_exec, "ctrlr"));

	gtk_widget_set_sensitive (hotrod_checkbutton,
				  xmame_has_option (current_exec, "hotrod"));

	gtk_widget_set_sensitive (hotrodse_checkbutton,
				  xmame_has_option (current_exec, "hotrodse"));

	gtk_widget_set_sensitive (GTK_WIDGET (Xinput_joy_frame), FALSE);

	gtk_widget_set_sensitive (GTK_WIDGET (grabmouse_checkbutton),
				  target->mouse && xmame_has_option (current_exec, "grabmouse"));

	gtk_widget_set_sensitive (GTK_WIDGET (grabkeyboard_checkbutton),
				  xmame_has_option (current_exec, "grabkeyboard"));

	gtk_widget_set_sensitive (GTK_WIDGET (config_name_checkbutton),
				  (xmame_has_option (current_exec, "cfgname") ||
				   xmame_has_option (current_exec, "cfg_directory")));

	gtk_widget_set_sensitive (GTK_WIDGET (config_name_entry), 
				  target->cfgname_flag && 
				  (xmame_has_option (current_exec, "cfgname") ||
				   xmame_has_option (current_exec, "cfg_directory")));


	combo_set_key(keymaptype_combo, target->keymap);

	g_signal_connect (G_OBJECT (joytype_combo), "changed",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (joytype_combo), "changed",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
	g_signal_connect_after (G_OBJECT (joytype_combo), "changed",
			          G_CALLBACK (joystick_choosen),
			          NULL);
	g_signal_connect (G_OBJECT (analogstick_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (analogstick_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
	g_signal_connect (G_OBJECT (hotrod_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (hotrod_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
	g_signal_connect (G_OBJECT (hotrodse_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (hotrodse_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
	g_signal_connect (G_OBJECT (usbpspad_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (usbpspad_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
	g_signal_connect (G_OBJECT (rapidfire_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (rapidfire_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
	g_signal_connect (G_OBJECT (ugcicoin_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (ugcicoin_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
	g_signal_connect (G_OBJECT (joydevname_entry), "changed",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (joydevname_entry), "changed",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
	g_signal_connect (G_OBJECT (paddevname_entry), "changed",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (paddevname_entry), "changed",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
	g_signal_connect (G_OBJECT (x11joyname_entry), "changed",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (x11joyname_entry), "changed",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
/*	g_signal_connect (G_OBJECT (XInput_joystick1_entry), "changed",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (XInput_joystick1_entry), "changed",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
	g_signal_connect (G_OBJECT (XInput_joystick2_entry), "changed",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (XInput_joystick2_entry), "changed",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
	g_signal_connect (G_OBJECT (XInput_joystick3_entry), "changed",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (XInput_joystick3_entry), "changed",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
	g_signal_connect (G_OBJECT (XInput_joystick4_entry), "changed",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (XInput_joystick4_entry), "changed",
			    G_CALLBACK (on_dirty_option),
			    reset_button);*/
	g_signal_connect (G_OBJECT (mouse_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (mouse_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
	if (xmame_has_option (current_exec, "grabmouse"))
		g_signal_connect_after (G_OBJECT (mouse_checkbutton), "toggled",
				          G_CALLBACK (button_toggled),
				          grabmouse_checkbutton);
	g_signal_connect (G_OBJECT (grabmouse_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (grabmouse_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
	g_signal_connect (G_OBJECT (XInput_trackball1_entry), "changed",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (XInput_trackball1_entry), "changed",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
	g_signal_connect (G_OBJECT (XInput_trackball2_entry), "changed",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (XInput_trackball2_entry), "changed",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
	g_signal_connect (G_OBJECT (XInput_trackball3_entry), "changed",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (XInput_trackball3_entry), "changed",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
	g_signal_connect (G_OBJECT (XInput_trackball4_entry), "changed",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (XInput_trackball4_entry), "changed",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
	g_signal_connect (G_OBJECT (winkeys_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (winkeys_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
	g_signal_connect (G_OBJECT (grabkeyboard_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    apply_button);
	g_signal_connect (G_OBJECT (grabkeyboard_checkbutton), "toggled",
			    G_CALLBACK (on_dirty_option),
			    reset_button);
 	g_signal_connect (G_OBJECT (keymaptype_combo), "changed",
 			    G_CALLBACK (on_dirty_option),
 			    apply_button);
 	g_signal_connect (G_OBJECT (keymaptype_combo), "changed",
 			    G_CALLBACK (on_dirty_option),
 			    reset_button);
	if (xmame_has_option (current_exec, "cfgname") || xmame_has_option (current_exec, "cfg_directory"))
	{
		g_signal_connect (G_OBJECT (config_name_checkbutton), "toggled",
				    G_CALLBACK (on_dirty_option),
				    apply_button);
		g_signal_connect (G_OBJECT (config_name_checkbutton), "toggled",
				    G_CALLBACK (on_dirty_option),
				    reset_button);
		g_signal_connect (G_OBJECT (config_name_entry), "changed",
				    G_CALLBACK (on_dirty_option),
				    apply_button);
		g_signal_connect (G_OBJECT (config_name_entry), "changed",
				    G_CALLBACK (on_dirty_option),
				    reset_button);
		g_signal_connect_after (G_OBJECT (config_name_checkbutton), "toggled",
				          G_CALLBACK (button_toggled),
				          config_name_entry);
	}
	if (xmame_has_option (current_exec, "ctrlr")) {
		g_signal_connect (G_OBJECT (ctrlr_combo), "changed",
				  G_CALLBACK (on_dirty_option),
				  apply_button);
		g_signal_connect (G_OBJECT (ctrlr_combo), "changed",
				  G_CALLBACK (on_dirty_option),
				  reset_button);
	}

}


void
add_misc_options_tab (GtkWidget *properties_windows,
		      GtkNotebook  *target_notebook,
		      RomEntry     *rom,
		      GameOptions  *target,
		      GtkWidget    *apply_button,
		      GtkWidget    *reset_button)
{
	GtkWidget *image;
	GtkWidget *misc_vbox;
	GtkWidget *artwork_table;
/*	GtkWidget *other_misc_frame;
	GtkWidget *other_misc_table;*/
	GtkWidget *misc_label;
/*	GtkWidget *label;*/
	int i;
	
	xmame_get_options (current_exec);	
	dirty_options_flag = FALSE;

	image = gmameui_get_image_from_stock ("gmameui-general-toolbar");

	misc_label = gtk_hbox_new (FALSE, 5);
	gtk_box_pack_start (GTK_BOX (misc_label), image, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (misc_label), gtk_label_new (_("Miscellaneous")), FALSE, FALSE, 0);
	gtk_widget_show_all (misc_label);

	GladeXML *xml = glade_xml_new (GLADEDIR "properties.glade", "miscellaneous_vbox", NULL);
	misc_vbox = glade_xml_get_widget (xml, "miscellaneous_vbox");
	gtk_notebook_append_page (GTK_NOTEBOOK (target_notebook), misc_vbox, misc_label);
	gtk_widget_show_all (misc_vbox);

	/* additional artwork */
	artwork_checkbutton = glade_xml_get_widget (xml, "artwork_checkbutton");
	use_backdrops_checkbutton = glade_xml_get_widget (xml, "use_backdrops_checkbutton");
	use_bezels_checkbutton = glade_xml_get_widget (xml, "use_bezels_checkbutton");
	use_overlays_checkbutton = glade_xml_get_widget (xml, "use_overlays_checkbutton");

	/* artwork_resolution */
	artwork_resolution_combo = glade_xml_get_widget (xml, "artwork_resolution_combo");
	artwork_resolution_combo = combo_new_empty_from_glade(artwork_resolution_combo, G_TYPE_INT);
	combo_append_int_value(artwork_resolution_combo, 0, _("Auto"));
	combo_append_int_value(artwork_resolution_combo, 1, _("Standard"));
	combo_append_int_value(artwork_resolution_combo, 2, _("High"));
/* FIXME Do we hide these options, or disable them? Need to come up with a policy */
	gtk_widget_set_sensitive(artwork_resolution_combo, xmame_has_option(current_exec, "artwork_resolution"));
	
	/* artwork crop */
	artcrop_checkbutton = glade_xml_get_widget (xml, "artcrop_checkbutton");

	/* misc options */
	keyboard_leds_checkbutton = glade_xml_get_widget (xml, "keyboard_leds_checkbutton");
	additional_options_checkbutton = glade_xml_get_widget (xml, "additional_options_checkbutton");
	additional_options_entry = glade_xml_get_widget (xml, "additional_options_entry");
	debug_checkbutton = glade_xml_get_widget (xml, "debug_checkbutton");
	debug_size_label = glade_xml_get_widget (xml, "debug_size_label");
	debug_size_combo = glade_xml_get_widget (xml, "debug_size_combo");
	debug_size_combo = combo_new_empty_from_glade(debug_size_combo, G_TYPE_STRING);
	combo_append_string_value(debug_size_combo, "640x480", "640x480");
	combo_append_string_value(debug_size_combo, "800x600", "800x600");
	combo_append_string_value(debug_size_combo, "1024x768", "1024x768");
	combo_append_string_value(debug_size_combo, "1280x1024", "1280x1024");
	combo_append_string_value(debug_size_combo, "1600x1200", "1600x1200");
	
	log_flag_checkbutton = glade_xml_get_widget (xml, "log_flag_checkbutton");
	log_label = glade_xml_get_widget (xml, "log_label");
	log_entry = glade_xml_get_widget (xml, "log_entry");

	disclaimer_checkbutton = glade_xml_get_widget (xml, "disclaimer_checkbutton");
	gameinfo_checkbutton = glade_xml_get_widget (xml, "gameinfo_checkbutton");
	cheat_checkbutton = glade_xml_get_widget (xml, "cheat_checkbutton");

	bios_label = glade_xml_get_widget (xml, "bios_label");
	bios_combo = glade_xml_get_widget (xml, "bios_combo");
	bios_combo = combo_new_empty_from_glade (bios_combo, G_TYPE_INT);
	i = 0;
	do {
		combo_append_int_value(bios_combo, i, _(bios_version[i]));
		i++;
	} while (bios_version[i]);
	
	/* end misc options */
	
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (artwork_checkbutton), target->artwork);
	/* the use_backdrops option will be used for all new backdrop option maybe change after */
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (use_backdrops_checkbutton), target->use_backdrops);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (use_bezels_checkbutton), target->use_bezels);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (use_overlays_checkbutton), target->use_overlays);

	combo_set_int_key(artwork_resolution_combo, target->artwork_resolution);

	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (artcrop_checkbutton), target->artwork_crop);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (keyboard_leds_checkbutton), target->keyboard_leds);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (cheat_checkbutton), target->cheat);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (disclaimer_checkbutton), target->skip_disclaimer);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gameinfo_checkbutton), target->skip_gameinfo);

	combo_set_int_key(bios_combo, target->bios);

	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (debug_checkbutton), target->debug);

	combo_set_key(debug_size_combo, target->debug_size);

	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (additional_options_checkbutton), target->use_additional_options);
	
	gtk_entry_set_text (GTK_ENTRY (additional_options_entry), target->additional_options);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (log_flag_checkbutton), target->log_flag);
	
	gtk_entry_set_text (GTK_ENTRY (log_entry), target->log);
	
	gtk_widget_set_sensitive (GTK_WIDGET (artwork_resolution_combo),
				  xmame_has_option (current_exec, "artwork_resolution") && target->artwork);

	gtk_widget_set_sensitive (GTK_WIDGET (use_backdrops_checkbutton),
				  xmame_has_option (current_exec, "use_backdrops") && target->artwork);
	gtk_widget_set_sensitive (GTK_WIDGET (use_bezels_checkbutton),
				  xmame_has_option (current_exec, "use_backdrops") && target->artwork);
	gtk_widget_set_sensitive (GTK_WIDGET (use_overlays_checkbutton),
				  xmame_has_option (current_exec, "use_backdrops") && target->artwork);

	gtk_widget_set_sensitive (GTK_WIDGET (artcrop_checkbutton),
				  xmame_has_option (current_exec, "artwork_crop") && target->artwork);

	gtk_widget_set_sensitive (GTK_WIDGET (keyboard_leds_checkbutton),
				  xmame_has_option (current_exec, "keyboard_leds"));
	
	gtk_widget_set_sensitive (GTK_WIDGET (disclaimer_checkbutton),
				  xmame_has_option (current_exec, "skip_disclaimer"));

	gtk_widget_set_sensitive (GTK_WIDGET (gameinfo_checkbutton),
				  xmame_has_option (current_exec, "skip_gameinfo"));

	gtk_widget_set_sensitive (GTK_WIDGET (bios_label),
				  xmame_has_option (current_exec, "bios"));

	gtk_widget_set_sensitive (GTK_WIDGET (bios_combo),
				  xmame_has_option (current_exec, "bios"));

	gtk_widget_set_sensitive (GTK_WIDGET (debug_checkbutton), 
				  xmame_has_option (current_exec, "debug"));

	gtk_widget_set_sensitive (GTK_WIDGET (debug_size_label), 
				  xmame_has_option (current_exec, "debug") && target->debug);
	gtk_widget_set_sensitive (GTK_WIDGET (debug_size_combo),
				  xmame_has_option (current_exec, "debug") && target->debug);

	gtk_widget_set_sensitive (GTK_WIDGET (additional_options_entry), target->use_additional_options);
	gtk_widget_set_sensitive (GTK_WIDGET (log_label), target->log_flag);
	gtk_widget_set_sensitive (GTK_WIDGET (log_entry), target->log_flag);

	g_signal_connect (G_OBJECT (artwork_checkbutton), "toggled",
			  G_CALLBACK (on_dirty_option),
			  apply_button);
	g_signal_connect (G_OBJECT (artwork_checkbutton), "toggled",
			  G_CALLBACK (on_dirty_option),
			  reset_button);
	if (xmame_has_option (current_exec, "use_backdrops")) {
		g_signal_connect_after (G_OBJECT (artwork_checkbutton), "toggled",
					G_CALLBACK (button_toggled),
					use_backdrops_checkbutton);
		g_signal_connect_after (G_OBJECT (artwork_checkbutton), "toggled",
					G_CALLBACK (button_toggled),
					use_bezels_checkbutton);
		g_signal_connect_after (G_OBJECT (artwork_checkbutton), "toggled",
					G_CALLBACK (button_toggled),
					use_overlays_checkbutton);
	}
	if (xmame_has_option (current_exec, "artwork_resolution"))
		g_signal_connect_after (G_OBJECT (artwork_checkbutton), "toggled",
				    	  G_CALLBACK (button_toggled),
				    	  artwork_resolution_combo);
	if (xmame_has_option (current_exec, "artwork_crop"))
		g_signal_connect_after (G_OBJECT (artwork_checkbutton), "toggled",
					G_CALLBACK (button_toggled),
					artcrop_checkbutton);

	if (xmame_has_option (current_exec, "debug")) {
		g_signal_connect_after (G_OBJECT (debug_checkbutton), "toggled",
				    	  G_CALLBACK (button_toggled),
				    	  debug_size_combo);
		g_signal_connect_after (G_OBJECT (debug_checkbutton), "toggled",
				    	  G_CALLBACK (button_toggled),
				    	  debug_size_label);
	}

	g_signal_connect_after (G_OBJECT (log_flag_checkbutton), "toggled",
			    	  G_CALLBACK (button_toggled),
			    	  log_label);
	g_signal_connect_after (G_OBJECT (log_flag_checkbutton), "toggled",
			    	  G_CALLBACK (button_toggled),
			    	  log_entry);

	GMAMEUI_DEBUG ("Finished adding misc options\n");
}

void
add_vector_options_tab (GtkWidget    *properties_windows,
                        GtkNotebook  *target_notebook,
                        RomEntry     *rom,
                        GameOptions  *target,
                        GtkWidget    *apply_button,
                        GtkWidget    *reset_button)
{
	GtkWidget *image;
	GtkWidget *vector_vbox;
	GtkWidget *beam_label;
	GtkWidget *flicker_label;
	GtkWidget *intensity_label;
	GtkWidget *gl_vector_frame;

	GtkWidget *vector_label;
	int i;

	GladeXML *xml;
	xml = glade_xml_new (GLADEDIR "properties.glade", "vector_vbox", NULL);
	
	xmame_get_options (current_exec);
	dirty_options_flag = FALSE;
  
	image = gmameui_get_image_from_stock ("gmameui-general-toolbar");

	vector_label = gtk_hbox_new (FALSE, 5);
	gtk_box_pack_start (GTK_BOX (vector_label), image, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (vector_label), gtk_label_new (_("Vector")), FALSE, FALSE, 0);
	gtk_widget_show_all (vector_label);

	vector_vbox = glade_xml_get_widget (xml, "vector_vbox");
	gtk_notebook_append_page (GTK_NOTEBOOK (target_notebook), vector_vbox, vector_label);

	/* Anti-aliasing */
	antialias_checkbutton = glade_xml_get_widget (xml, "antialias_checkbutton");

	/* Translucent vectors */
	translucency_checkbutton = glade_xml_get_widget (xml, "translucency_checkbutton");

	/* Beam size */
	beam_label = glade_xml_get_widget (xml, "beam_label");

	beam_hscale = glade_xml_get_widget (xml, "beam_hscale");
	gtk_adjustment_set_value (GTK_ADJUSTMENT (gtk_range_get_adjustment (GTK_RANGE (beam_hscale))), target->beam);

	/* Flicker */
	flicker_label = glade_xml_get_widget (xml, "flicker_label");

	flicker_hscale = glade_xml_get_widget (xml, "flicker_hscale");
	gtk_adjustment_set_value (GTK_ADJUSTMENT (gtk_range_get_adjustment (GTK_RANGE (flicker_hscale))), target->flicker);

	/* Intensity */
	intensity_label = glade_xml_get_widget (xml, "intensity_label");

	intensity_hscale = glade_xml_get_widget (xml, "intensity_hscale");
	gtk_adjustment_set_value (GTK_ADJUSTMENT (gtk_range_get_adjustment (GTK_RANGE (intensity_hscale))), target->intensity);

	/* Fixed resolution */
	vector_fixed_resolution_checkbutton = glade_xml_get_widget (xml, "vector_fixed_resolution_checkbutton");

	/* TODO Disable if fixed res checkbutton is disabled */
	vectorres_combo = glade_xml_get_widget (xml, "vectorres_combo");
	vectorres_combo = combo_new_empty_from_glade (vectorres_combo, G_TYPE_STRING);

	for (i=0; resolution_table[i]; i++)
		combo_append_string_value(vectorres_combo, resolution_table[i], resolution_table[i]);

	/* Open GL */
	gldrawbitmapvec_checkbutton = glade_xml_get_widget (xml, "gldrawbitmapvec_checkbutton");
	
	glantialiasvec_checkbutton = glade_xml_get_widget (xml, "glantialiasvec_checkbutton");
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (glantialiasvec_checkbutton), TRUE);

	/* End vector options */
	/* TODO */
	if (! (current_exec->type && (XMAME_EXEC_XGL || XMAME_EXEC_X11)))
		gtk_widget_set_sensitive (GTK_WIDGET (gl_vector_frame), FALSE);

	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (antialias_checkbutton), target->antialias);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (translucency_checkbutton), target->translucency);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (vector_fixed_resolution_checkbutton), target->vectorres_flag);

	combo_set_key(vectorres_combo, target->vectorres);
	
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gldrawbitmapvec_checkbutton), target->gldrawbitmapvec);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (glantialiasvec_checkbutton), target->glantialiasvec);
	gtk_widget_set_sensitive (GTK_WIDGET (intensity_label),
		xmame_has_option (current_exec, "intensity"));

	gtk_widget_set_sensitive (GTK_WIDGET (intensity_hscale),
		xmame_has_option (current_exec, "intensity"));
}


void
save_properties_options (RomEntry    *rom,
			 GameOptions *target)
{
	gchar *used_text;
	const gchar *value;	
/* FIXME TODO In place of strncpy, make sure length of field in Glade limits entry */
	if (!dirty_options_flag)
		return;
GMAMEUI_DEBUG ("Getting xmame options");
	xmame_get_options (current_exec);
	
	if (!target)
		target = &default_options;

/*** Video Related ***/
GMAMEUI_DEBUG ("Getting arbheight");

	target->bpp = combo_get_int_key(bpp_combo);

	if (target->bpp < 0)
		target->bpp = 0;

	used_text = gtk_editable_get_chars (GTK_EDITABLE (arbheight_entry), 0, -1);
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (arbheight_checkbutton)))	
		target->arbheight = atoi (used_text);
	else
		target->arbheight = 0;
	g_free (used_text);
		
	target->heightscale = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (heightscale_spinbutton));
	target->widthscale = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (widthscale_spinbutton));

	target->effect = combo_get_index(effect_combo);
GMAMEUI_DEBUG ("Getting effect details");
	used_text = gtk_editable_get_chars (GTK_EDITABLE (effect_name), 0, -1);
	target->effect_name = g_strdup (used_text);
	g_free (used_text);
GMAMEUI_DEBUG ("Done getting effect details");	
	target->autodouble = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (autodouble_checkbutton));
	target->dirty = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (dirty_checkbutton));
	target->scanlines = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (scanlines_checkbutton));
	
	target->frameskipper = combo_get_index(frameskipper_combo);

	if (target->frameskipper < 0)
		target->frameskipper = 0;
		
	target->throttle = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (throttle_checkbutton));
	target->sleepidle = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (sleepidle_checkbutton));
	target->autoframeskip = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (autoframeskip_checkbutton));

	target->maxautoframeskip = combo_get_int_key(maxautoframeskip_combo);
	target->frameskip = combo_get_int_key(frameskip_combo);

	if (target->frameskip < 0)
		target->frameskip = 0;

	/*if (available_options->dos_brightness) */
		target->brightness = gtk_range_get_adjustment (GTK_RANGE (brightness_hscale))->value;
#if 0
	else
	{
		/* percentage converted to dosmame brightness complient 0.5-2.0 range*/
		target->brightness = (gtk_range_get_adjustment (GTK_RANGE (brightness_hscale))->value)* (1.5/100)+0.5;
	}
#endif
	target->gamma_correction = gtk_range_get_adjustment (GTK_RANGE (gamma_hscale))->value;
	target->norotate = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (norotate_checkbutton));

	target->ror = (combo_get_int_key(rotation_combo) == 1);
	target->rol = (combo_get_int_key(rotation_combo) == 2);
					
	target->flipx = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (flipx_checkbutton));
	target->flipy = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (flipy_checkbutton));
		
	/* from misc tab */
	target->artwork = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (artwork_checkbutton));
GMAMEUI_DEBUG ("Got artwork details");
	/* Rendering Options */

	switch (current_exec->type)
	{
		case XMAME_EXEC_X11:
		case XMAME_EXEC_XGL:
			if (xmame_get_option_value_count (current_exec, "video-mode") > 0)
			{
				target->x11_mode = combo_get_index(video_mode_combo);
			}

			target->cursor = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (cursor_checkbutton));
			target->mitshm = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (mitshm_checkbutton));
			target->xvgeom_flag = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (xvgeom_checkbutton));
			//strncpy (target->geometry, combo_get_key(xvgeom_combo), 9);
			target->geometry = g_strdup (combo_get_key(xvgeom_combo));

			target->force_yuv = combo_get_index (yuv_combo);
			if (target->force_yuv < 0)
				target->force_yuv = 0;
			
			target->fullscreen = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (xvfullscreen_checkbutton));
			target->xsync = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (xsync_checkbutton));
			target->privatecmap = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (privatecmap_checkbutton));
			target->xil = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (xil_checkbutton));
			target->mtxil = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (mtxil_checkbutton));
		
			target->gldblbuffer = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (gldblbuffer_checkbutton));
			target->gltexture_size = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (gltexture_size_spinbutton));
			target->glbilinear = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (glbilinear_checkbutton));
			target->glext78 = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (glext78_checkbutton));
			target->glforceblitmode = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (glforceblitmode_checkbutton));
			target->gldrawbitmap = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (gldrawbitmap_checkbutton));
			target->glcolormod = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (glcolormod_checkbutton));
			target->glantialias = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (glantialias_checkbutton));
			target->glalphablending = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (glalphablending_checkbutton));
			target->cabview = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (cabview_checkbutton));

			used_text = gtk_editable_get_chars (GTK_EDITABLE (cabinet_entry), 0, -1);
			//strncpy (target->cabinet, used_text, 20);
			target->cabinet = g_strdup (used_text);
			g_free (used_text);
			target->glres_flag = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (glres_checkbutton));
			
			value = combo_get_key(glres_combo);

			if (!value)
				target->glres[0] = '\0';
			else
				target->glres = g_strdup (value);
			//strncpy (target->glres, value, 9);
			

			used_text = gtk_editable_get_chars (GTK_EDITABLE (gllib_entry), 0, -1);
			//strncpy (target->gllibname, used_text, 20);
			target->gllibname = g_strdup (used_text);
			g_free (used_text);

			used_text = gtk_editable_get_chars (GTK_EDITABLE (glulib_entry), 0, -1);
			//strncpy (target->glulibname, used_text, 20);
			target->glulibname = g_strdup (used_text);
			g_free (used_text);
			break;

		case XMAME_EXEC_SDL:
			target->fullscreen = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (fullscreen_checkbutton));
			target->sdl_doublebuf = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (doublebuf_checkbutton));
			target->sdl_auto_mode = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (sdl_modes_checkbutton));
			target->modenumber = 0;
			/* FIXME
			if (current_exec->sdl_modes_hash_table)
			{
				g_hash_table_foreach (current_exec->sdl_modes_hash_table,
		 	         	  (GHFunc)sdl_modes_combo_comp_inv,
						   target);
			} else {*/
				target->modenumber = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (sdl_modes_spinbutton));
			/*}*/	
			break;

		case XMAME_EXEC_XFX:
		case XMAME_EXEC_SVGAFX:
			target->fxgkeepaspect = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (fxgkeepaspect_checkbutton));
			
			strncpy (target->resolution,
				combo_get_key(fx_combo),
				20);
			
			break;

		case XMAME_EXEC_PHOTON2:
			target->render_mode = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (render_mode_checkbutton));
			target->phcursor = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (phcursor_checkbutton));
			break;

		case XMAME_EXEC_GGI:
			target->ggilinear = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (ggilinear_checkbutton));
			target->ggi_force_resolution= gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (ggi_force_resolution_checkbutton));
			target->xres = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (ggix_spinbutton));
			target->yres = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (ggiy_spinbutton));
			break;

#ifdef ENABLE_XMAME_SVGALIB_SUPPORT
		case XMAME_EXEC_SVGALIB:
			target->planar = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (planar_checkbutton));
			target->linear = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (linear_checkbutton));
			target->tweak = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (tweaked_mode_checkbutton));
			target->centerx = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (centerx_spinbutton));
			target->centery = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (centery_spinbutton));
			break;
#endif
		case XMAME_EXEC_UNKNOWN:
			/* nothing */
			break;
	}
				
/*** Video Mode related ***/
	target->keepaspect = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (keepaspect_checkbutton));
	used_text = gtk_editable_get_chars (GTK_EDITABLE (displayaspectratio_entry), 0, -1);
	target->displayaspectratio = atof (used_text);
	g_free (used_text);
	used_text = gtk_editable_get_chars (GTK_EDITABLE (disable_mode_entry), 0, -1);
	//strncpy (target->disablemode, used_text, 14);
	target->disablemode = g_strdup (used_text);
	g_free (used_text);
GMAMEUI_DEBUG ("Got video details");
/* Sound */
	target->sound = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (sound_checkbutton));
	target->samples = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (samples_checkbutton));
	target->fakesound = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (fakesound_checkbutton));
	target->samplefre = combo_get_int_key(samplefreq_combo);
	target->volume = gtk_range_get_adjustment (GTK_RANGE (db_hscale))->value;
	target->bufsize = gtk_range_get_adjustment (GTK_RANGE (bufsize_hscale))->value;

	//strncpy (target->audiodevice, gtk_entry_get_text(GTK_ENTRY(GTK_BIN(audiodevice_combo)->child)), 20);
	target->audiodevice = g_strdup (gtk_entry_get_text(GTK_ENTRY(GTK_BIN(audiodevice_combo)->child)));
	//strncpy (target->mixerdevice, gtk_entry_get_text(GTK_ENTRY(GTK_BIN(mixerdevice_combo)->child)), 20);
	target->mixerdevice = g_strdup (gtk_entry_get_text(GTK_ENTRY(GTK_BIN(mixerdevice_combo)->child)));

	if (combo_get_key(dsp_plugin_combo))
		target->dsp_plugin = g_strdup (combo_get_key(dsp_plugin_combo));
		//strncpy(target->dsp_plugin, combo_get_key(dsp_plugin_combo), 20);

	if (combo_get_key(mixer_plugin_combo))
		target->sound_mixer_plugin = g_strdup (combo_get_key(mixer_plugin_combo));
		//strncpy(target->sound_mixer_plugin, combo_get_key(mixer_plugin_combo), 20);
	
	used_text = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (soundfile_entry));
	//strncpy (target->soundfile, used_text, 50);
	target->soundfile = g_strdup (used_text);
	g_free (used_text);
			
	target->timer = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (timer_checkbutton));
	target->artsBufferTime = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (arts_spinbutton));
	target->audio_preferred = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (audio_preferred_checkbutton));
	used_text = gtk_editable_get_chars (GTK_EDITABLE (pcm_entry), 0, -1);
	//strncpy (target->alsa_pcm, used_text, 20);
	target->alsa_pcm = g_strdup (used_text);
	g_free (used_text);
	target->alsa_buffer = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (alsa_buffer_spinbutton));
	target->alsacard = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (alsacard_spinbutton));
	target->alsadevice = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (alsadevice_spinbutton));
GMAMEUI_DEBUG ("Got sound details");
/* controllers */
	target->joytype = combo_get_index (joytype_combo);
	
	target->analogstick = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (analogstick_checkbutton));
	/* TODO
	target->hotrod = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (hotrod_checkbutton));
	target->hotrodse = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (hotrodse_checkbutton));
	*/

	value = combo_get_key(ctrlr_combo);

	if (!value)
		target->ctrlr[0] = '\0';
	else
		target->ctrlr = g_strdup (value);
		//strncpy (target->ctrlr, value, 20);
	
	target->usbpspad = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (usbpspad_checkbutton));
	target->rapidfire = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (rapidfire_checkbutton));
	target->ugcicoin = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (ugcicoin_checkbutton));
	used_text = gtk_editable_get_chars (GTK_EDITABLE (joydevname_entry), 0, -1);
	//strncpy (target->joydevname, used_text, 20);
	target->joydevname = g_strdup (used_text);
	g_free (used_text);
	used_text = gtk_editable_get_chars (GTK_EDITABLE (paddevname_entry), 0, -1);
	//strncpy (target->paddevname, used_text, 20);
	target->paddevname = g_strdup (used_text);
	g_free (used_text);
	used_text = gtk_editable_get_chars (GTK_EDITABLE (x11joyname_entry), 0, -1);
	//strncpy (target->x11joyname, used_text, 20);
	target->x11joyname = g_strdup (used_text);
	g_free (used_text);
GMAMEUI_DEBUG("Got joystick details");
/*	used_text = gtk_editable_get_chars (GTK_EDITABLE (XInput_joystick1_entry), 0, -1);
	strncpy (target->XInput_joystick1, used_text, 20);
	g_free (used_text);
	used_text = gtk_editable_get_chars (GTK_EDITABLE (XInput_joystick2_entry), 0, -1);
	strncpy (target->XInput_joystick2, used_text, 20);
	g_free (used_text);
	used_text = gtk_editable_get_chars (GTK_EDITABLE (XInput_joystick3_entry), 0, -1);
	strncpy (target->XInput_joystick3, used_text, 20);
	g_free (used_text);
	used_text = gtk_editable_get_chars (GTK_EDITABLE (XInput_joystick4_entry), 0, -1);
	strncpy (target->XInput_joystick4, used_text, 20);
	g_free (used_text);*/
	target->mouse = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (mouse_checkbutton));
	target->grabmouse = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (grabmouse_checkbutton));
	used_text = gtk_editable_get_chars (GTK_EDITABLE (XInput_trackball1_entry), 0, -1);
	//strncpy (target->XInput_trackball1, used_text, 20);
	target->XInput_trackball1 = g_strdup (used_text);
	g_free (used_text);
	used_text = gtk_editable_get_chars (GTK_EDITABLE (XInput_trackball2_entry), 0, -1);
	//strncpy (target->XInput_trackball2, used_text, 20);
	target->XInput_trackball2 = g_strdup (used_text);
	g_free (used_text);
	used_text = gtk_editable_get_chars (GTK_EDITABLE (XInput_trackball3_entry), 0, -1);
	target->XInput_trackball3 = g_strdup (used_text);
	//strncpy (target->XInput_trackball3, used_text, 20);
	g_free (used_text);
	used_text = gtk_editable_get_chars (GTK_EDITABLE (XInput_trackball4_entry), 0, -1);
	target->XInput_trackball4 = g_strdup (used_text);
	//strncpy (target->XInput_trackball4, used_text, 20);
	g_free (used_text);
/* TODO
	target->winkeys = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (winkeys_checkbutton));
	target->grabkeyboard = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (grabkeyboard_checkbutton));

	strncpy (target->keymap, combo_get_key(keymaptype_combo), 4);
	target->keymap = g_strdup (combo_get_key(keymaptype_combo));
*/
GMAMEUI_DEBUG ("Got controller details");
	/* vector */
	if (!rom || rom->vector)
	{
		target->antialias = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (antialias_checkbutton));
		target->translucency = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (translucency_checkbutton));
		target->beam = gtk_range_get_adjustment (GTK_RANGE (beam_hscale))->value;
		target->flicker = gtk_range_get_adjustment (GTK_RANGE (flicker_hscale))->value;
		target->intensity = gtk_range_get_adjustment (GTK_RANGE (intensity_hscale))->value;
		target->vectorres_flag = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (vector_fixed_resolution_checkbutton));
			
		value = combo_get_key(vectorres_combo);

		if (value)
			target->vectorres = g_strdup (value);
			//strncpy (target->vectorres, value, 9);
		else
			target->vectorres[0] = '\0';
			
		/* (open gl) */
		target->gldrawbitmapvec = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (gldrawbitmapvec_checkbutton));
		target->glantialiasvec = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (glantialiasvec_checkbutton));
	}
	
/* misc */
	/*artwork*/
	target->artwork = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (artwork_checkbutton));
	target->use_backdrops = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (use_backdrops_checkbutton));
	target->use_bezels = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (use_bezels_checkbutton));
	target->use_overlays = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (use_overlays_checkbutton));
	target->artwork_resolution = combo_get_int_key(artwork_resolution_combo);

	target->artwork_crop = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (artcrop_checkbutton));
	/* misc */
	target->keyboard_leds = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (keyboard_leds_checkbutton));
	target->cheat = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (cheat_checkbutton));
	target->debug = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (debug_checkbutton));
	target->skip_disclaimer = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (disclaimer_checkbutton));
	target->skip_gameinfo = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (gameinfo_checkbutton));

	//strncpy (target->debug_size, combo_get_key(debug_size_combo), 20);
	target->debug_size = g_strdup (combo_get_key(debug_size_combo));
	
	target->use_additional_options = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (additional_options_checkbutton));
	/* here I'm using directly the additional option because I don't know its length */
	g_free (target->additional_options);
	target->additional_options = gtk_editable_get_chars (GTK_EDITABLE (additional_options_entry), 0, -1);
	target->log_flag = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (log_flag_checkbutton));
	used_text = gtk_editable_get_chars (GTK_EDITABLE (log_entry), 0, -1);
	//strncpy (target->log, used_text, 20);
	target->log = g_strdup (used_text);
	g_free (used_text);
	target->cfgname_flag = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (config_name_checkbutton));
	used_text = gtk_editable_get_chars (GTK_EDITABLE (config_name_entry), 0, -1);
	//strncpy (target->cfgname, used_text, 20);
	target->cfgname = g_strdup (used_text);
	g_free (used_text);
	
	target->bios = combo_get_int_key(bios_combo);
		
	dirty_options_flag = FALSE;
}

void
load_properties_options (RomEntry  *rom,
			 GtkWidget *properties_window)
{
	gchar title[BUFFER_SIZE];
	gchar *used_text;
	GameOptions *target = NULL;
	
	xmame_get_options (current_exec);

	if (rom)
		target = load_options (rom);
	
	if (!target)
		target = &default_options;
	
	g_object_set_data (G_OBJECT (properties_window), "opts", target);

	/***********/
	/* Display */
	/***********/

	combo_set_index(effect_combo, target->effect);
	
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (scanlines_checkbutton), target->scanlines);

	gtk_adjustment_set_value (GTK_ADJUSTMENT (gamma_hscale_adj) , target->gamma_correction);

	/* FIXME */
	/*if (available_options->dos_brightness)*/
		gtk_adjustment_set_value (GTK_ADJUSTMENT (brightness_hscale_adj) , target->brightness);
	/*else
		gtk_adjustment_set_value (GTK_ADJUSTMENT (brightness_hscale_adj) , ( (target->brightness)-0.5)* (100/1.5));*/

	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (dirty_checkbutton), target->dirty);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (throttle_checkbutton), target->throttle);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (sleepidle_checkbutton), target->sleepidle);


	combo_set_int_key(frameskip_combo, target->frameskip);
	gtk_widget_set_sensitive (frameskip_combo, !(target->autoframeskip));

	combo_set_index(frameskipper_combo, target->frameskipper);
	
	gtk_widget_set_sensitive (frameskipper_combo, target->autoframeskip);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (autoframeskip_checkbutton), target->autoframeskip);

	combo_set_int_key(maxautoframeskip_combo, target->maxautoframeskip);
	gtk_widget_set_sensitive (maxautoframeskip_combo, target->autoframeskip);

	gtk_adjustment_set_value (GTK_ADJUSTMENT (heightscale_spinbutton_adj), target->heightscale);

	combo_set_int_key(bpp_combo, target->bpp);
	
	gtk_adjustment_set_value (GTK_ADJUSTMENT (widthscale_spinbutton_adj) , target->widthscale);

	g_snprintf (title, BUFFER_SIZE, "%i", target->arbheight);
	gtk_entry_set_text (GTK_ENTRY (arbheight_entry), title);
	gtk_widget_set_sensitive (arbheight_entry, target->arbheight);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (arbheight_checkbutton), target->arbheight);

	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (autodouble_checkbutton), target->autodouble);

	if (target->ror)
		combo_set_int_key(rotation_combo, 1);
	else if (target->rol)
		combo_set_int_key(rotation_combo, 2);
	else
		combo_set_int_key(rotation_combo, 0);
	
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (norotate_checkbutton), target->norotate);

	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (flipx_checkbutton), target->flipx);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (flipy_checkbutton), target->flipy);

	/*************/
	/* Rendering */
	/*************/

	switch (current_exec->type)
	{
		case XMAME_EXEC_X11: /* X11 */
		case XMAME_EXEC_XGL: /* Open GL */
			combo_set_index(video_mode_combo, target->x11_mode);
			
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (cursor_checkbutton), target->cursor);
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (mitshm_checkbutton), target->mitshm);
			gtk_widget_set_sensitive (GTK_WIDGET (mitshm_checkbutton),
				xmame_has_option (current_exec, "mitshm"));

			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (xvgeom_checkbutton), target->xvgeom_flag);

			combo_set_key(xvgeom_combo, target->geometry);

			gtk_widget_set_sensitive (GTK_WIDGET (xvgeom_combo), target->xvgeom_flag /*&&
								available_options.xvext
								available_options.mitshm &&
								target->mitshm &&
								target->xvext &&
								*/);


			combo_set_index(yuv_combo, target->force_yuv);
		
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (xvfullscreen_checkbutton), target->fullscreen);
			gtk_widget_set_sensitive (GTK_WIDGET (xvfullscreen_checkbutton),
				xmame_has_option (current_exec, "fullscreen"));

			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (xsync_checkbutton), target->xsync);
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (privatecmap_checkbutton), target->privatecmap);

			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (xil_checkbutton), target->xil);
			gtk_widget_set_sensitive (GTK_WIDGET (xil_checkbutton), 
				xmame_has_option (current_exec, "xil"));
			
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (mtxil_checkbutton), target->mtxil);
			gtk_widget_set_sensitive (GTK_WIDGET (mtxil_checkbutton),
				xmame_has_option (current_exec, "mtxil"));
		
			/* Open GL */
			gtk_adjustment_set_value (GTK_ADJUSTMENT (sdl_modes_spinbutton_adj) , target->modenumber);
			gtk_adjustment_set_value (GTK_ADJUSTMENT (gltexture_size_spinbutton_adj) , target->gltexture_size);
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gldblbuffer_checkbutton), target->gldblbuffer);
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (glbilinear_checkbutton), target->glbilinear);
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (glext78_checkbutton), target->glext78);
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (glforceblitmode_checkbutton), target->glforceblitmode);
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gldrawbitmap_checkbutton), target->gldrawbitmap);
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (glcolormod_checkbutton), target->glcolormod);
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (glantialias_checkbutton), target->glantialias);
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (glalphablending_checkbutton), target->glalphablending);
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (cabview_checkbutton), target->cabview);
			gtk_widget_set_sensitive (GTK_WIDGET (cabinet_entry), target->cabview);
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (glres_checkbutton), target->glres_flag);
			gtk_entry_set_text (GTK_ENTRY (cabinet_entry), target->cabinet);
			gtk_widget_set_sensitive (GTK_WIDGET (glres_combo), target->glres_flag);
			gtk_entry_set_text (GTK_ENTRY (gllib_entry), target->gllibname);
			gtk_entry_set_text (GTK_ENTRY (glulib_entry), target->glulibname);
			break;

		case XMAME_EXEC_SDL:	/* SDL */
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (fullscreen_checkbutton), target->fullscreen);
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (doublebuf_checkbutton), target->sdl_doublebuf);
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (sdl_modes_checkbutton), (target->sdl_auto_mode));
			if (xmame_get_option_value_count (current_exec, "modenumber") > 0)
			{
				gtk_widget_set_sensitive (GTK_WIDGET (sdl_modes_combo), ! (target->sdl_auto_mode));
			}
			else
			{
				gtk_widget_set_sensitive (GTK_WIDGET (sdl_modes_spinbutton), ! (target->sdl_auto_mode));
			}
			break;

		case XMAME_EXEC_XFX:
		case XMAME_EXEC_SVGAFX:	/* FX Glide */
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (fxgkeepaspect_checkbutton), target->fxgkeepaspect);

			combo_set_key(fx_combo, target->resolution);
			gtk_widget_set_sensitive (GTK_WIDGET (sdl_modes_label), ! (target->sdl_auto_mode));
			break;

		case XMAME_EXEC_PHOTON2: /* Photon 2 */
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (render_mode_checkbutton), target->render_mode);
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (phcursor_checkbutton), ! (target->phcursor));
			break;

		case XMAME_EXEC_GGI:
			gtk_adjustment_set_value (GTK_ADJUSTMENT (ggix_spinbutton_adj), target->xres);
			gtk_adjustment_set_value (GTK_ADJUSTMENT (ggiy_spinbutton_adj), target->yres);
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (ggilinear_checkbutton), target->ggilinear);
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (ggi_force_resolution_checkbutton), target->ggi_force_resolution);
			gtk_widget_set_sensitive (GTK_WIDGET (ggi_res_table), target->ggi_force_resolution);
			break;

#ifdef ENABLE_XMAME_SVGALIB_SUPPORT
		case XMAME_EXEC_SVGALIB:
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (planar_checkbutton), target->planar);
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (linear_checkbutton), target->linear);
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (tweaked_mode_checkbutton), target->tweak);
			gtk_widget_set_sensitive (GTK_WIDGET (svga_tweak_table), target->tweak);
			gtk_adjustment_set_value (GTK_ADJUSTMENT (centerx_spinbutton_adj), target->centerx);
			gtk_adjustment_set_value (GTK_ADJUSTMENT (centery_spinbutton_adj), target->centery);
			break;
#endif
		case XMAME_EXEC_UNKNOWN:
			/* do nothing */
			break;
	}

	/*** Video Mode related ***/
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (keepaspect_checkbutton), target->keepaspect);
	gtk_widget_set_sensitive (GTK_WIDGET (displayaspectratio_entry), (target->keepaspect));
	used_text = g_strdup_printf ("%f", target->displayaspectratio);
	gtk_entry_set_text (GTK_ENTRY (displayaspectratio_entry), used_text);
	g_free (used_text);
	gtk_entry_set_text (GTK_ENTRY (disable_mode_entry), target->disablemode);

	/*********/
	/* Sound */
	/*********/

	gtk_adjustment_set_value (GTK_ADJUSTMENT (db_hscale_adj), target->volume);
	gtk_adjustment_set_value (GTK_ADJUSTMENT (bufsize_hscale_adj), target->bufsize);

	gtk_adjustment_set_value (GTK_ADJUSTMENT (arts_spinbutton_adj), target->artsBufferTime);
	gtk_adjustment_set_value (GTK_ADJUSTMENT (alsa_buffer_spinbutton_adj), target->alsa_buffer);
	gtk_adjustment_set_value (GTK_ADJUSTMENT (alsacard_spinbutton_adj), target->alsacard);
	gtk_adjustment_set_value (GTK_ADJUSTMENT (alsadevice_spinbutton_adj), target->alsadevice);

	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (sound_checkbutton), target->sound);
	gtk_widget_set_sensitive (GTK_WIDGET (samples_checkbutton), target->sound);
	gtk_widget_set_sensitive (GTK_WIDGET (volume_frame), target->sound);
	gtk_widget_set_sensitive (GTK_WIDGET (bufsize_frame), target->sound);
	gtk_widget_set_sensitive (GTK_WIDGET (samplefreq_combo), target->sound);
	gtk_widget_set_sensitive (GTK_WIDGET (audiodevice_label), target->sound);
	gtk_widget_set_sensitive (GTK_WIDGET (samplefreq_label), target->sound);
	gtk_widget_set_sensitive (GTK_WIDGET (mixerdevice_label), target->sound);
	gtk_widget_set_sensitive (GTK_WIDGET (audiodevice_combo), target->sound);
	gtk_widget_set_sensitive (GTK_WIDGET (mixerdevice_combo), target->sound);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (samples_checkbutton), target->samples);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (fakesound_checkbutton), target->fakesound);

	combo_set_int_key(samplefreq_combo, target->samplefre);
	gtk_entry_set_text (GTK_ENTRY (GTK_BIN(audiodevice_combo)->child), target->audiodevice);
	gtk_entry_set_text (GTK_ENTRY (GTK_BIN(mixerdevice_combo)->child), target->mixerdevice);

	gtk_entry_set_text (GTK_ENTRY (soundfile_entry), target->soundfile);
	if (!strcmp (target->dsp_plugin, "waveout"))
	{
		gtk_widget_set_sensitive (GTK_WIDGET (soundfile_hbox), TRUE);
		gtk_widget_set_sensitive (GTK_WIDGET (audiodevice_hbox), FALSE);
	}
	else
	{
		gtk_widget_set_sensitive (GTK_WIDGET (soundfile_hbox), FALSE);
		gtk_widget_set_sensitive (GTK_WIDGET (audiodevice_hbox), TRUE);
	}

	combo_set_key(dsp_plugin_combo, target->dsp_plugin);
	combo_set_key(mixer_plugin_combo, target->sound_mixer_plugin);
	
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (timer_checkbutton), target->timer);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (audio_preferred_checkbutton), target->audio_preferred);
	gtk_entry_set_text (GTK_ENTRY (pcm_entry), target->alsa_pcm);

	gtk_widget_set_sensitive (GTK_WIDGET (qnx_frame), xmame_has_option (current_exec, "audio_preferred"));
	gtk_widget_set_sensitive (GTK_WIDGET (alsa_frame), xmame_get_option_key_value (current_exec, "dsp-plugin", "alsa") != NULL);

	{
		gboolean has_arts;

		has_arts = 
		 (xmame_get_option_key_value (current_exec, "dsp-plugin", "arts") != NULL) ||
		 (xmame_get_option_key_value (current_exec, "dsp-plugin", "arts_smotek") != NULL) ||		
		 (xmame_get_option_key_value (current_exec, "dsp-plugin", "arts_teira") != NULL);

		gtk_widget_set_sensitive (GTK_WIDGET (arts_frame), has_arts);
	}
	gtk_widget_set_sensitive (GTK_WIDGET (alsacard_label), xmame_has_option (current_exec, "alsacard"));
	gtk_widget_set_sensitive (GTK_WIDGET (alsacard_spinbutton), xmame_has_option (current_exec, "alsacard"));
	gtk_widget_set_sensitive (GTK_WIDGET (alsadevice_label), xmame_has_option (current_exec, "alsadevice"));
	gtk_widget_set_sensitive (GTK_WIDGET (alsadevice_spinbutton), xmame_has_option (current_exec, "alsadevice"));
	gtk_widget_set_sensitive (GTK_WIDGET (alsa_buffer_label), xmame_has_option (current_exec, "alsa-buffer"));
	gtk_widget_set_sensitive (GTK_WIDGET (alsa_buffer_spinbutton), xmame_has_option (current_exec, "alsa-buffer"));
	gtk_widget_set_sensitive (GTK_WIDGET (PCM_label), xmame_has_option (current_exec, "alsa-pcm"));
	gtk_widget_set_sensitive (GTK_WIDGET (pcm_entry), xmame_has_option (current_exec, "alsa-pcm"));

	/**************/
	/* Controller */
	/**************/

	gtk_widget_set_sensitive (paddevname_label, FALSE);
	gtk_widget_set_sensitive (paddevname_entry, FALSE);
	gtk_widget_set_sensitive (x11joyname_label, FALSE);
	gtk_widget_set_sensitive (x11joyname_entry, FALSE);
	gtk_widget_set_sensitive (joydevname_label, FALSE);
	gtk_widget_set_sensitive (joydevname_entry, FALSE);
	gtk_widget_set_sensitive (hotrod_checkbutton, FALSE);
	gtk_widget_set_sensitive (hotrodse_checkbutton, FALSE);
	switch (target->joytype)
	{
		case 0:
			gtk_widget_set_sensitive (analogstick_checkbutton, FALSE);
			gtk_widget_set_sensitive (hotrod_checkbutton, TRUE);
			gtk_widget_set_sensitive (hotrodse_checkbutton, TRUE);
			gtk_widget_set_sensitive (usbpspad_checkbutton, FALSE);
			gtk_widget_set_sensitive (rapidfire_checkbutton, FALSE);
			break;
		case 1:
			gtk_widget_set_sensitive (joydevname_label, TRUE);
			gtk_widget_set_sensitive (joydevname_entry, TRUE);
			break;
		case 2:
			gtk_widget_set_sensitive (paddevname_label, TRUE);
			gtk_widget_set_sensitive (paddevname_entry, TRUE);
			break;
		case 3:
			gtk_widget_set_sensitive (x11joyname_label, TRUE);
			gtk_widget_set_sensitive (x11joyname_entry, TRUE);
			break;
		case 4:
			gtk_widget_set_sensitive (joydevname_label, TRUE);
			gtk_widget_set_sensitive (joydevname_entry, TRUE);
			break;
	}

	if (target->joytype > 7)
		target->joytype = 0;

	combo_set_index(joytype_combo, target->joytype);

	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (analogstick_checkbutton), target->analogstick);
	
	combo_set_key(ctrlr_combo, target->ctrlr);

	gtk_widget_set_sensitive (GTK_WIDGET (ctrlr_combo), xmame_has_option (current_exec, "ctrlr"));
	gtk_widget_set_sensitive (hotrod_checkbutton, xmame_has_option (current_exec, "hotrod"));
	gtk_widget_set_sensitive (hotrodse_checkbutton, xmame_has_option (current_exec, "hotrodse"));
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (hotrod_checkbutton), target->hotrod);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (hotrodse_checkbutton), target->hotrodse);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (usbpspad_checkbutton), target->usbpspad);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (rapidfire_checkbutton), target->rapidfire);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (ugcicoin_checkbutton), target->ugcicoin);
	gtk_entry_set_text (GTK_ENTRY (joydevname_entry), target->joydevname);
	gtk_entry_set_text (GTK_ENTRY (paddevname_entry), target->paddevname);
	gtk_entry_set_text (GTK_ENTRY (x11joyname_entry), target->x11joyname);
/*	gtk_entry_set_text (GTK_ENTRY (XInput_joystick1_entry), target->XInput_joystick1);
	gtk_entry_set_text (GTK_ENTRY (XInput_joystick2_entry), target->XInput_joystick2);
	gtk_entry_set_text (GTK_ENTRY (XInput_joystick3_entry), target->XInput_joystick3);
	gtk_entry_set_text (GTK_ENTRY (XInput_joystick4_entry), target->XInput_joystick4);*/
	gtk_widget_set_sensitive (GTK_WIDGET (Xinput_joy_frame), FALSE);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (mouse_checkbutton), target->mouse);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (grabmouse_checkbutton), target->grabmouse);
	gtk_widget_set_sensitive (GTK_WIDGET (grabmouse_checkbutton), target->mouse && xmame_has_option (current_exec, "grabmouse"));
	gtk_entry_set_text (GTK_ENTRY (XInput_trackball1_entry), target->XInput_trackball1);
	gtk_entry_set_text (GTK_ENTRY (XInput_trackball2_entry), target->XInput_trackball2);
	gtk_entry_set_text (GTK_ENTRY (XInput_trackball3_entry), target->XInput_trackball3);
	gtk_entry_set_text (GTK_ENTRY (XInput_trackball4_entry), target->XInput_trackball4);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (grabkeyboard_checkbutton), target->grabkeyboard);
	gtk_widget_set_sensitive (GTK_WIDGET (grabkeyboard_checkbutton), xmame_has_option (current_exec, "grabkeyboard"));
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (winkeys_checkbutton), target->winkeys);
	gtk_widget_set_sensitive (GTK_WIDGET (config_name_checkbutton), 
		xmame_has_option (current_exec, "cfgname") || xmame_has_option (current_exec, "cfg_directory"));

	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (config_name_checkbutton), target->cfgname_flag);
	gtk_widget_set_sensitive (GTK_WIDGET (config_name_entry), 
		target->cfgname_flag && 
			 (xmame_has_option (current_exec, "cfgname") || xmame_has_option (current_exec, "cfg_directory")));

	gtk_entry_set_text (GTK_ENTRY (config_name_entry), target->cfgname);

	combo_set_key(keymaptype_combo, target->keymap);

	/********/
	/* Misc */
	/********/

	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (artwork_checkbutton), target->artwork);
	/* the use_backdrops option will be used for all new backdrop option maybe change after */

	gtk_widget_set_sensitive (GTK_WIDGET (use_backdrops_checkbutton), xmame_has_option (current_exec, "use_backdrops") && target->artwork);
	gtk_widget_set_sensitive (GTK_WIDGET (use_bezels_checkbutton), xmame_has_option (current_exec, "use_backdrops") && target->artwork);
	gtk_widget_set_sensitive (GTK_WIDGET (use_overlays_checkbutton), xmame_has_option (current_exec, "use_backdrops") && target->artwork);

	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (use_backdrops_checkbutton), target->use_backdrops);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (use_bezels_checkbutton), target->use_bezels);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (use_overlays_checkbutton), target->use_overlays);
	combo_set_int_key(artwork_resolution_combo, target->artwork_resolution);
	
	gtk_widget_set_sensitive (GTK_WIDGET (artwork_resolution_combo), 
				  xmame_has_option (current_exec, "artwork_resolution") && target->artwork);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (artcrop_checkbutton), target->artwork_crop);

	gtk_widget_set_sensitive (GTK_WIDGET (artcrop_checkbutton), xmame_has_option (current_exec, "artwork_crop") && target->artwork);

	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (keyboard_leds_checkbutton), target->keyboard_leds);
	gtk_widget_set_sensitive (GTK_WIDGET (keyboard_leds_checkbutton), xmame_has_option (current_exec, "keyboard_leds"));
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (cheat_checkbutton), target->cheat);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (disclaimer_checkbutton), target->skip_disclaimer);
	gtk_widget_set_sensitive (GTK_WIDGET (disclaimer_checkbutton), xmame_has_option (current_exec, "skip_disclaimer"));
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gameinfo_checkbutton), target->skip_gameinfo);
	gtk_widget_set_sensitive (GTK_WIDGET (gameinfo_checkbutton), xmame_has_option (current_exec, "skip_gameinfo"));
	gtk_widget_set_sensitive (GTK_WIDGET (bios_label), xmame_has_option (current_exec, "bios"));
	gtk_widget_set_sensitive (GTK_WIDGET (bios_combo), xmame_has_option (current_exec, "bios"));
	
	combo_set_int_key(bios_combo, target->bios);

	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (debug_checkbutton), target->debug);
	gtk_widget_set_sensitive (GTK_WIDGET (debug_checkbutton), xmame_has_option (current_exec, "debug"));
	gtk_widget_set_sensitive (GTK_WIDGET (debug_size_label), xmame_has_option (current_exec, "debug") && target->debug);
	gtk_widget_set_sensitive (GTK_WIDGET (debug_size_combo), xmame_has_option (current_exec, "debug") && target->debug);

	combo_set_key(debug_size_combo, target->debug_size);
	
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (additional_options_checkbutton), target->use_additional_options);
	gtk_widget_set_sensitive (GTK_WIDGET (additional_options_entry), target->use_additional_options);
	gtk_entry_set_text (GTK_ENTRY (additional_options_entry), target->additional_options);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (log_flag_checkbutton), target->log_flag);
	gtk_widget_set_sensitive (GTK_WIDGET (log_label), target->log_flag);
	gtk_widget_set_sensitive (GTK_WIDGET (log_entry), target->log_flag);
	gtk_entry_set_text (GTK_ENTRY (log_entry), target->log);
	
	dirty_options_flag = FALSE;
}
