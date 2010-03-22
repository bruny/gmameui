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

#ifndef __GMAMEUI_LISTOUTPUT_DLG_H__
#define __GMAMEUI_LISTOUTPUT_DLG_H__

G_BEGIN_DECLS

/* Preferences dialog object */
#define GMAMEUI_TYPE_LISTOUTPUT_DIALOG        (gmameui_listoutput_dialog_get_type ())
#define GMAMEUI_LISTOUTPUT_DIALOG(o)          (G_TYPE_CHECK_INSTANCE_CAST ((o), GMAMEUI_TYPE_LISTOUTPUT_DIALOG, GMAMEUIListOutputDialog))
#define GMAMEUI_LISTOUTPUT_DIALOG_CLASS(k)    (G_TYPE_CHECK_CLASS_CAST((k), GMAMEUI_TYPE_LISTOUTPUT_DIALOG, GMAMEUIListOutputDialogClass))
#define GMAMEUI_IS_LISTOUTPUT_DIALOG(o)       (G_TYPE_CHECK_INSTANCE_TYPE ((o), GMAMEUI_TYPE_LISTOUTPUT_DIALOG))
#define GMAMEUI_IS_LISTOUTPUT_DIALOG_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), GMAMEUI_TYPE_LISTOUTPUT_DIALOG))
#define GMAMEUI_LISTOUTPUT_DIALOG_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GMAMEUI_TYPE_LISTOUTPUT_DIALOG, GMAMEUIListOutputDialogClass))

typedef struct _GMAMEUIListOutputDialog        GMAMEUIListOutputDialog;
typedef struct _GMAMEUIListOutputDialogClass   GMAMEUIListOutputDialogClass;
typedef struct _GMAMEUIListOutputDialogPrivate GMAMEUIListOutputDialogPrivate;

struct _GMAMEUIListOutputDialogClass {
	GtkDialogClass parent_class;
};

struct _GMAMEUIListOutputDialog {
	GtkDialog parent;
	
	GMAMEUIListOutputDialogPrivate *priv;
};

GType gmameui_listoutput_dialog_get_type (void);
GtkWidget *gmameui_listoutput_dialog_new (GtkWindow *parent);

G_END_DECLS

#endif /* __GMAMEUI_LISTOUTPUT_DLG_H__ */
