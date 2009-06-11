/*
 * GMAMEUI
 *
 * Copyright 2008-2009 Andrew Burton <adb@iinet.net.au>
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

#ifndef __FILTER_H
#define __FILTER_H

#include "common.h"

G_BEGIN_DECLS

/* Filter */
#define GMAMEUI_TYPE_FILTER	         (gmameui_filter_get_type ())
#define GMAMEUI_FILTER(obj)	         (G_TYPE_CHECK_INSTANCE_CAST ((obj), GMAMEUI_TYPE_FILTER, GMAMEUIFilter))
#define GMAMEUI_FILTER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GMAMEUI_TYPE_FILTER, GMAMEUIFilterClass))
#define GMAMEUI_IS_FILTER(obj)	     (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GMAMEUI_TYPE_FILTER))
#define GMAMEUI_IS_FILTER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((obj), GMAMEUI_TYPE_FILTER))
#define GMAMEUI_FILTER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GMAMEUI_TYPE_FILTER, GMAMEUIFilterClass))
#define GMAMEUI_FILTER_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), GMAMEUI_TYPE_FILTER, GMAMEUIFilterPrivate))

typedef struct _GMAMEUIFilterPrivate GMAMEUIFilterPrivate;
typedef struct _GMAMEUIFilterClass GMAMEUIFilterClass;
typedef struct _GMAMEUIFilter GMAMEUIFilter;

struct _GMAMEUIFilter
{
	GtkHBox parent;
	
	GMAMEUIFilterPrivate *priv;

};

struct _GMAMEUIFilterClass
{
	GtkHBoxClass parent_class;
	/* define vtable methods and signals here */
};

GType gmameui_filter_get_type (void);

GtkWidget *gmameui_filter_new (void);

/* End filter */

G_END_DECLS

#endif
