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
