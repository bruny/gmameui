/*
 * GMAMEUI
 *
 * Copyright 2007-2008 Andrew Burton <adb@iinet.net.au>
 * based on Anjuta code
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

#ifndef MAME_OPTIONS_DIALOG_H
#define MAME_OPTIONS_DIALOG_H

G_BEGIN_DECLS

#include <gtk/gtk.h>

#define MAME_TYPE_OPTIONS_DIALOG        (mame_options_dialog_get_type ())
#define MAME_OPTIONS_DIALOG(o)          (G_TYPE_CHECK_INSTANCE_CAST ((o), MAME_TYPE_OPTIONS_DIALOG, MameOptionsDialog))
#define MAME_OPTIONS_DIALOG_CLASS(k)    (G_TYPE_CHECK_CLASS_CAST((k), MAME_TYPE_OPTIONS_DIALOG, MameOptionsDialogClass))
#define MAME_IS_OPTIONS_DIALOG(o)       (G_TYPE_CHECK_INSTANCE_TYPE ((o), MAME_TYPE_OPTIONS_DIALOG))
#define MAME_IS_OPTIONS_DIALOG_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), MAME_TYPE_OPTIONS_DIALOG))

typedef struct _MameOptionsDialog        MameOptionsDialog;
typedef struct _MameOptionsDialogClass   MameOptionsDialogClass;
typedef struct _MameOptionsDialogPrivate MameOptionsDialogPrivate;

struct _MameOptionsDialog {
	GtkDialog parent;
	
	MameOptionsDialogPrivate *priv;
};

struct _MameOptionsDialogClass {
	GtkDialogClass parent;
};

GType mame_options_dialog_get_type (void);

GtkWidget *mame_options_dialog_new (void);

G_END_DECLS

#endif
