/*
 * GMAMEUI
 *
 * Copyright 2007-2008 Andrew Burton <adb@iinet.net.au>
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

#ifndef __GUI_PREFS_DIALOG_H__
#define __GUI_PREFS_DIALOG_H__



G_BEGIN_DECLS

#include <gtk/gtk.h>

/* Preferences dialog object */
#define MAME_TYPE_GUI_PREFS_DIALOG        (mame_gui_prefs_dialog_get_type ())
#define MAME_GUI_PREFS_DIALOG(o)          (G_TYPE_CHECK_INSTANCE_CAST ((o), MAME_TYPE_GUI_PREFS_DIALOG, MameGuiPrefsDialog))
#define MAME_GUI_PREFS_DIALOG_CLASS(k)    (G_TYPE_CHECK_CLASS_CAST((k), MAME_TYPE_GUI_PREFS_DIALOG, MameGuiPrefsDialogClass))
#define MAME_IS_GUI_PREFS_DIALOG(o)       (G_TYPE_CHECK_INSTANCE_TYPE ((o), MAME_TYPE_GUI_PREFS_DIALOG))
#define MAME_IS_GUI_PREFS_DIALOG_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), MAME_TYPE_GUI_PREFS_DIALOG))

typedef struct _MameGuiPrefsDialog        MameGuiPrefsDialog;
typedef struct _MameGuiPrefsDialogClass   MameGuiPrefsDialogClass;
typedef struct _MameGuiPrefsDialogPrivate MameGuiPrefsDialogPrivate;

struct _MameGuiPrefsDialog {
	GtkDialog parent;
	
	MameGuiPrefsDialogPrivate *priv;
};

struct _MameGuiPrefsDialogClass {
	GtkDialogClass parent;
};

GType mame_gui_prefs_dialog_get_type (void);
GtkWidget *mame_gui_prefs_dialog_new (void);

G_END_DECLS


#endif /* __GUI_PREFS_DIALOG_H__ */
