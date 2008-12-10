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
 
#ifndef __ROMINFO_DLG_H__
#define __ROMINFO_DLG_H__

#include "common.h"

G_BEGIN_DECLS

/* Preferences dialog object */
#define MAME_TYPE_ROMINFO_DIALOG        (mame_rominfo_dialog_get_type ())
#define MAME_ROMINFO_DIALOG(o)          (G_TYPE_CHECK_INSTANCE_CAST ((o), MAME_TYPE_ROMINFO_DIALOG, MameRomInfoDialog))
#define MAME_ROMINFO_DIALOG_CLASS(k)    (G_TYPE_CHECK_CLASS_CAST((k), MAME_TYPE_ROMINFO_DIALOG, MameRomInfoDialogClass))
#define MAME_IS_ROMINFO_DIALOG(o)       (G_TYPE_CHECK_INSTANCE_TYPE ((o), MAME_TYPE_ROMINFO_DIALOG))
#define MAME_IS_ROMINFO_DIALOG_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), MAME_TYPE_ROMINFO_DIALOG))
#define MAME_ROMINFO_DIALOG_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), MAME_TYPE_ROMINFO_DIALOG, MameRomInfoDialogClass))

typedef struct _MameRomInfoDialog        MameRomInfoDialog;
typedef struct _MameRomInfoDialogClass   MameRomInfoDialogClass;
typedef struct _MameRomInfoDialogPrivate MameRomInfoDialogPrivate;

struct _MameRomInfoDialogClass {
	GtkDialogClass parent_class;
};

struct _MameRomInfoDialog {
	GtkDialog parent;
	
	MameRomInfoDialogPrivate *priv;
};

GType mame_rominfo_dialog_get_type (void);
GtkWidget *mame_rominfo_dialog_new ();

G_END_DECLS

#endif /* __ROMINFO_DLG_H__ */