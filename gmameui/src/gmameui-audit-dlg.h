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

#ifndef __GMAMEUI_AUDIT_DLG_H__
#define __GMAMEUI_AUDIT_DLG_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

/* Preferences dialog object */
#define MAME_TYPE_AUDIT_DIALOG        (mame_audit_dialog_get_type ())
#define MAME_AUDIT_DIALOG(o)          (G_TYPE_CHECK_INSTANCE_CAST ((o), MAME_TYPE_AUDIT_DIALOG, MameAuditDialog))
#define MAME_AUDIT_DIALOG_CLASS(k)    (G_TYPE_CHECK_CLASS_CAST((k), MAME_TYPE_AUDIT_DIALOG, MameAuditDialogClass))
#define MAME_IS_AUDIT_DIALOG(o)       (G_TYPE_CHECK_INSTANCE_TYPE ((o), MAME_TYPE_AUDIT_DIALOG))
#define MAME_IS_AUDIT_DIALOG_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), MAME_TYPE_AUDIT_DIALOG))
#define MAME_AUDIT_DIALOG_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), MAME_TYPE_AUDIT_DIALOG, MameAuditDialogClass))

typedef struct _MameAuditDialog        MameAuditDialog;
typedef struct _MameAuditDialogClass   MameAuditDialogClass;
typedef struct _MameAuditDialogPrivate MameAuditDialogPrivate;

struct _MameAuditDialogClass {
	GtkDialogClass parent_class;
};

struct _MameAuditDialog {
	GtkDialog parent;
	
	MameAuditDialogPrivate *priv;
};

GType mame_audit_dialog_get_type (void);
GtkWidget *mame_audit_dialog_new (GtkWindow *parent);

G_END_DECLS

#endif /* __GMAMEUI_AUDIT_DLG_H__ */
