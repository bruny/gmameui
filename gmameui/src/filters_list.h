/*
 * GMAMEUI
 *
 * Copyright 2008 Andrew Burton <adb@iinet.net.au>
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

#ifndef __FILTERS_LIST_H
#define __FILTERS_LIST_H

#include <gtk/gtkscrolledwindow.h>
#include <gtk/gtkexpander.h>

#include "common.h"
#include "filter.h"
#include "gossip-cell-renderer-expander.h"

G_BEGIN_DECLS

/* Filter list - this is the containing scrollbar widget */

#define GMAMEUI_TYPE_FILTERS_LIST	         (gmameui_filters_list_get_type ())
#define GMAMEUI_FILTERS_LIST(obj)	         (G_TYPE_CHECK_INSTANCE_CAST ((obj), GMAMEUI_TYPE_FILTERS_LIST, GMAMEUIFiltersList))
#define GMAMEUI_FILTERS_LIST_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GMAMEUI_TYPE_FILTERS_LIST, GMAMEUIFiltersListClass))
#define GMAMEUI_IS_FILTERS_LIST(obj)	     (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GMAMEUI_TYPE_FILTERS_LIST))
#define GMAMEUI_IS_FILTERS_LIST_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((obj), GMAMEUI_TYPE_FILTERS_LIST))
#define GMAMEUI_FILTERS_LIST_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GMAMEUI_TYPE_FILTERS_LIST, GMAMEUIFiltersListClass))

typedef struct _GMAMEUIFiltersListPrivate GMAMEUIFiltersListPrivate;
typedef struct _GMAMEUIFiltersListClass GMAMEUIFiltersListClass;
typedef struct _GMAMEUIFiltersList GMAMEUIFiltersList;

struct _GMAMEUIFiltersList
{
	GtkTreeView    parent;

	GMAMEUIFiltersListPrivate *priv;
};

struct _GMAMEUIFiltersListClass
{
	GtkTreeViewClass parent_class;

};

GType gmameui_filters_list_get_type (void);

GtkWidget *gmameui_filters_list_new (void);

void
gmameui_filters_list_add_filter (GMAMEUIFiltersList *fl,
						 GMAMEUIFilter *filter, gchar *group);

void
gmameui_filters_list_select (GMAMEUIFiltersList *fl,
							 GMAMEUIFilter *filter);
			
/* Model */
typedef enum {
	GMAMEUI_FILTER_LIST_MODEL_COLUMN_PIXBUF = 0,
	GMAMEUI_FILTER_LIST_MODEL_COLUMN_NAME,
	GMAMEUI_FILTER_LIST_MODEL_COLUMN_FILTER,
	GMAMEUI_FILTER_LIST_MODEL_IS_GROUP,
	GMAMEUI_FILTER_LIST_MODEL_IS_SEPARATOR,
	GMAMEUI_FILTER_LIST_MODEL_N_COLUMNS
} GMAMEUIFiltersListModelColumn;

G_END_DECLS

#endif
