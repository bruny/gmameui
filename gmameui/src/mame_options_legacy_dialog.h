/*
 * GMAMEUI
 *
 * Copyright 2007-2008 Andrew Burton <adb@iinet.net.au>
 * based on Anjuta code
 * Copyright (C) 2002 Dave Camp
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

#ifndef MAME_OPTIONS_LEGACY_DIALOG_H
#define MAME_OPTIONS_LEGACY_DIALOG_H

G_BEGIN_DECLS

#include <gtk/gtk.h>

#define MAME_TYPE_OPTIONS_LEGACY_DIALOG        (mame_options_legacy_dialog_get_type ())
#define MAME_OPTIONS_LEGACY_DIALOG(o)          (G_TYPE_CHECK_INSTANCE_CAST ((o), MAME_TYPE_OPTIONS_LEGACY_DIALOG, MameOptionsLegacyDialog))
#define MAME_OPTIONS_LEGACY_DIALOG_CLASS(k)    (G_TYPE_CHECK_CLASS_CAST((k), MAME_TYPE_OPTIONS_LEGACY_DIALOG, MameOptionsLegacyDialogClass))
#define MAME_IS_OPTIONS_LEGACY_DIALOG(o)       (G_TYPE_CHECK_INSTANCE_TYPE ((o), MAME_TYPE_OPTIONS_LEGACY_DIALOG))
#define MAME_IS_OPTIONS_LEGACY_DIALOG_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), MAME_TYPE_OPTIONS_LEGACY_DIALOG))
#define MAME_OPTIONS_LEGACY_DIALOG_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), MAME_TYPE_OPTIONS_LEGACY_DIALOG, MameOptionsLegacyDialogClass))

typedef struct _MameOptionsLegacyDialog        MameOptionsLegacyDialog;
typedef struct _MameOptionsLegacyDialogClass   MameOptionsLegacyDialogClass;
typedef struct _MameOptionsLegacyDialogPrivate MameOptionsLegacyDialogPrivate;

struct _MameOptionsLegacyDialog {
	GtkDialog parent;
	
	MameOptionsLegacyDialogPrivate *priv;
};

struct _MameOptionsLegacyDialogClass {
	GtkDialogClass parent;
};

GType mame_options_legacy_dialog_get_type (void);

GtkWidget *mame_options_legacy_dialog_new (void);

G_END_DECLS

#endif
