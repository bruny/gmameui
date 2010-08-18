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

#ifndef __GMAMEUI_ROMMGR_DLG_H__
#define __GMAMEUI_ROMMGR_DLG_H__

#include "common.h"

G_BEGIN_DECLS

/* Preferences dialog object */
#define GMAMEUI_TYPE_ROMMGR_DIALOG        (gmameui_rommgr_dialog_get_type ())
#define GMAMEUI_ROMMGR_DIALOG(o)          (G_TYPE_CHECK_INSTANCE_CAST ((o), GMAMEUI_TYPE_ROMMGR_DIALOG, GMAMEUIRomMgrDialog))
#define GMAMEUI_ROMMGR_DIALOG_CLASS(k)    (G_TYPE_CHECK_CLASS_CAST((k), GMAMEUI_TYPE_ROMMGR_DIALOG, GMAMEUIRomMgrDialogClass))
#define GMAMEUI_IS_ROMMGR_DIALOG(o)       (G_TYPE_CHECK_INSTANCE_TYPE ((o), GMAMEUI_TYPE_ROMMGR_DIALOG))
#define GMAMEUI_IS_ROMMGR_DIALOG_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), GMAMEUI_TYPE_ROMMGR_DIALOG))
#define GMAMEUI_ROMMGR_DIALOG_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GMAMEUI_TYPE_ROMMGR_DIALOG, GMAMEUIRomMgrDialogClass))

typedef struct _GMAMEUIRomMgrDialog        GMAMEUIRomMgrDialog;
typedef struct _GMAMEUIRomMgrDialogClass   GMAMEUIRomMgrDialogClass;
typedef struct _GMAMEUIRomMgrDialogPrivate GMAMEUIRomMgrDialogPrivate;

struct _GMAMEUIRomMgrDialogClass {
	GtkDialogClass parent_class;
};

struct _GMAMEUIRomMgrDialog {
	GtkDialog parent;
	
	GMAMEUIRomMgrDialogPrivate *priv;
};

GType gmameui_rommgr_dialog_get_type (void);
GtkWidget *gmameui_rommgr_dialog_new (GtkWindow *parent);

G_END_DECLS

#endif /* __GMAMEUI_ROMMGR_DLG_H__ */