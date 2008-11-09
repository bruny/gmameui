/*
 * GMAMEUI
 *
 * Copyright 2008 Andrew Burton <adb@iinet.net.au>
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

#ifndef __MAME_EXEC_LIST_H__
#define __MAME_EXEC_LIST_H__

#include "common.h"
#include "mame-exec.h"

G_BEGIN_DECLS

/* Preferences object */
#define MAME_TYPE_EXEC_LIST            (mame_exec_list_get_type ())
#define MAME_EXEC_LIST(o)            (G_TYPE_CHECK_INSTANCE_CAST((o), MAME_TYPE_EXEC_LIST, MameExecList))
#define MAME_EXEC_LIST_CLASS(k)    (G_TYPE_CHECK_CLASS_CAST ((k), MAME_TYPE_EXEC_LIST, MameExecListClass))
#define MAME_IS_EXEC_LIST(o)         (G_TYPE_CHECK_INSTANCE_TYPE ((o), MAME_TYPE_EXEC_LIST))
#define MAME_IS_EXEC_LIST_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), MAME_TYPE_EXEC_LIST))
#define MAME_EXEC_LIST_GET_CLASS(o)  (G_TYPE_INSTANCE_GET_CLASS ((o), MAME_TYPE_EXEC_LIST, MameExecListClass))

typedef struct _MameExecList MameExecList;
typedef struct _MameExecListClass MameExecListClass;
typedef struct _MameExecListPrivate MameExecListPrivate;

struct _MameExecList {
	GList parent;
	
	MameExecListPrivate *priv;
	/* define public instance variables here */
};

struct _MameExecListClass {
	GObjectClass parent;
	/* define vtable methods and signals here */
};

GType mame_exec_list_get_type (void);
MameExecList* mame_exec_list_new (void);

void mame_exec_list_add (MameExecList *list, MameExec *exec);
GList *mame_exec_list_get_list (MameExecList *list);
MameExec* mame_exec_list_nth (MameExecList *list, guint index);
MameExec* mame_exec_list_get_exec_by_path (MameExecList *list, gchar *path);
MameExec* mame_exec_list_get_current_executable (MameExecList *list);
void mame_exec_list_set_current_executable (MameExecList *list, MameExec *exec);
gboolean mame_exec_list_has_current_executable (MameExecList *list);
int mame_exec_list_size (MameExecList *list);

G_END_DECLS

#endif
