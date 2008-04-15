/*
 * GMAMEUI
 *
 * Copyright 2007-2008 Andrew Burton <adb@iinet.net.au>
 * based on Anjuta code
 * Copyright (C) 2002 Dave Camp
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

#ifndef PREFS_CELL_RENDERER_H
#define PREFS_CELL_RENDERER_H

#include <gtk/gtkcellrenderer.h>
#include <gtk/gtkcellrenderertext.h>
#include <gtk/gtkcellrendererpixbuf.h>

G_BEGIN_DECLS

#define GMAMEUI_TYPE_CELL_RENDERER_CAPTIONED_IMAGE		(gmameui_cell_renderer_captioned_image_get_type ())
#define GMAMEUI_CELL_RENDERER_CAPTIONED_IMAGE(obj)		(GTK_CHECK_CAST ((obj), GMAMEUI_TYPE_CELL_RENDERER_CAPTIONED_IMAGE, GmameuiCellRendererCaptionedImage))
#define GMAMEUI_CELL_RENDERER_CAPTIONED_IMAGE_CLASS(klass)	(GTK_CHECK_CLASS_CAST ((klass), GMAMEUI_TYPE_CELL_RENDERER_CAPTIONED_IMAGE, GmameuiCellRendererCaptionedImageClass))
#define GMAMEUI_IS_CELL_RENDERER_CAPTIONED_IMAGE(obj)		(GTK_CHECK_TYPE ((obj), GMAMEUI_TYPE_CELL_RENDERER_CAPTIONED_IMAGE))
#define GMAMEUI_IS_CELL_RENDERER_CAPTIONED_IMAGE_CLASS(klass)	(GTK_CHECK_CLASS_TYPE ((klass), GMAMEUI_TYPE_CELL_RENDERER_CAPTIONED_IMAGE))
#define GMAMEUI_CELL_RENDERER_CAPTIONED_IMAGE_GET_CLASS(obj)   (GTK_CHECK_GET_CLASS ((obj), GMAMEUI_TYPE_CELL_RENDERER_CAPTIONED_IMAGE, GmameuiCellRendererCaptionedImageClass))

typedef struct _GmameuiCellRendererCaptionedImage        GmameuiCellRendererCaptionedImage;
typedef struct _GmameuiCellRendererCaptionedImageClass   GmameuiCellRendererCaptionedImageClass;

struct _GmameuiCellRendererCaptionedImage {
	GtkCellRenderer parent;

	GtkCellRenderer *image;
	GtkCellRenderer *caption;
};

struct _GmameuiCellRendererCaptionedImageClass {
	GtkCellRendererClass parent_class;
};

GType            gmameui_cell_renderer_captioned_image_get_type (void);
GtkCellRenderer *gmameui_cell_renderer_captioned_image_new      (void);

G_END_DECLS

#endif
