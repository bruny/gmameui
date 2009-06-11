/*
 * GMAMEUI
 *
 * Copyright 2007-2008 Andrew Burton <adb@iinet.net.au>
 * based on GXMame code
 * 2002-2005 Stephane Pontier <shadow_walker@users.sourceforge.net>
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

#ifndef __AUDIT_H__
#define __AUDIT_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS
#define GMAMEUI_TYPE_AUDIT            (gmameui_audit_get_type ())
#define GMAMEUI_AUDIT(o)            (G_TYPE_CHECK_INSTANCE_CAST((o), GMAMEUI_TYPE_AUDIT, GmameuiAudit))
#define GMAMEUI_AUDIT_CLASS(k)    (G_TYPE_CHECK_CLASS_CAST ((k), GMAMEUI_TYPE_AUDIT, GmameuiAuditClass))
#define GMAMEUI_IS_AUDIT(o)         (G_TYPE_CHECK_INSTANCE_TYPE ((o), GMAMEUI_TYPE_AUDIT))
#define GMAMEUI_IS_AUDIT_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), GMAMEUI_TYPE_AUDIT))
#define GMAMEUI_AUDIT_GET_CLASS(o)  (G_TYPE_INSTANCE_GET_CLASS ((o), GMAMEUI_TYPE_AUDIT, GmameuiAuditClass))

typedef struct _GmameuiAudit GmameuiAudit;
typedef struct _GmameuiAuditClass GmameuiAuditClass;
typedef struct _GmameuiAuditPrivate GmameuiAuditPrivate;

struct _GmameuiAudit {
	GObject parent;
	
	GmameuiAuditPrivate *priv;
	/* define public instance variables here */
};

struct _GmameuiAuditClass {
	GObjectClass parent;
	/* define vtable methods and signals here */
	
	/* Signal prototypes */
	void  (* romset_audited) (GmameuiAudit *audit, gchar *audit_line, gint type, gint auditresult);
};

/* Properties */
enum
{
	PROP_AUDIT_0,
	NUM_AUDIT_PROPERTIES
};

enum {
	AUDIT_TYPE_ROM,
	AUDIT_TYPE_SAMPLE
};

GType gmameui_audit_get_type (void);
GmameuiAudit* gmameui_audit_new (void);

void   mame_audit_start_full           (void);
void   mame_audit_start_single         (gchar *romname);
void   mame_audit_stop_full_audit      (GmameuiAudit *au);
gchar* get_romset_name_from_audit_line (gchar *line);

G_END_DECLS


#endif
