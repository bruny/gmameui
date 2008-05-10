/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
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

/**
 * SECTION:cell-renderer-captioned-image
 * @short_description: Captioned image cell renderer
 * 
 */

#include <stdlib.h>
#include <glib/gi18n.h>

#include "cell-renderer-captioned-image.h"


static void gmameui_cell_renderer_captioned_image_init (GmameuiCellRendererCaptionedImage *cell);
static void gmameui_cell_renderer_captioned_image_class_init (GmameuiCellRendererCaptionedImageClass *class);

enum {
	PROP_0,
	
	PROP_TEXT,
	PROP_PIXBUF
};

#define PAD 3
#define SPACE 5

G_DEFINE_TYPE (GmameuiCellRendererCaptionedImage,
	       gmameui_cell_renderer_captioned_image,
	       GTK_TYPE_CELL_RENDERER);

static void
gmameui_cell_renderer_captioned_image_get_property (GObject *object,
						   guint param_id,
						   GValue *value,
						   GParamSpec *pspec)
{
	GmameuiCellRendererCaptionedImage *cell = GMAMEUI_CELL_RENDERER_CAPTIONED_IMAGE (object);
	
	switch (param_id) {
	case PROP_TEXT:
		g_object_get_property (G_OBJECT (cell->caption), 
				       "text", value);
		break;
	case PROP_PIXBUF:
		g_object_get_property (G_OBJECT (cell->image), 
				       "pixbuf", value);
		break;
	default :
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, param_id, pspec);
		break;
	}
}

static void
gmameui_cell_renderer_captioned_image_set_property (GObject *object,
						   guint param_id,
						   const GValue *value,
						   GParamSpec *pspec)
{
	GmameuiCellRendererCaptionedImage *cell = GMAMEUI_CELL_RENDERER_CAPTIONED_IMAGE (object);
	
	switch (param_id) {
	case PROP_TEXT:
		g_object_set_property (G_OBJECT (cell->caption), "text", value);
		break;
		
	case PROP_PIXBUF:
		g_object_set_property (G_OBJECT (cell->image), "pixbuf", 
				       value);
		break;
		
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, param_id, pspec);
		break;
	}
}

GtkCellRenderer *
gmameui_cell_renderer_captioned_image_new (void)
{
	return GTK_CELL_RENDERER (g_object_new (gmameui_cell_renderer_captioned_image_get_type (), NULL));
}

static void
gmameui_cell_renderer_captioned_image_get_size (GtkCellRenderer *gtk_cell,
					       GtkWidget *widget,
					       GdkRectangle *cell_area,
					       int *x_offset,
					       int *y_offset,
					       int *width,
					       int *height)
{
	int text_x_offset;
	int text_y_offset;
	int text_width;
	int text_height;
	
	GmameuiCellRendererCaptionedImage *cell = GMAMEUI_CELL_RENDERER_CAPTIONED_IMAGE (gtk_cell);
	
	gtk_cell_renderer_get_size (cell->image, widget, cell_area, 
				    NULL, NULL, width, height);
	
	gtk_cell_renderer_get_size (cell->caption, widget, cell_area,
				    &text_x_offset, 
				    &text_y_offset,
				    &text_width,
				    &text_height);


	if (height) {
		*height = *height + text_height + PAD;
	}
	
	if (width) {
		*width = MAX (*width, text_width);
		*width += SPACE * 2;
	}
}

static void
gmameui_cell_renderer_captioned_image_render (GtkCellRenderer *gtk_cell,
					     GdkWindow *window,
					     GtkWidget *widget,
					     GdkRectangle *background_area,
					     GdkRectangle *cell_area,
					     GdkRectangle *expose_area,
					     guint flags)

{
	GmameuiCellRendererCaptionedImage *cell = GMAMEUI_CELL_RENDERER_CAPTIONED_IMAGE (gtk_cell);
	GdkRectangle text_area;
	GdkRectangle pixbuf_area;
	int width, height;
	
	gtk_cell_renderer_get_size (cell->image, widget, cell_area, 
				    NULL, NULL, &width, &height);
	
	pixbuf_area.y = cell_area->y;
	pixbuf_area.x = cell_area->x;
	pixbuf_area.height = height;
	pixbuf_area.width = cell_area->width;

	gtk_cell_renderer_get_size (cell->caption, widget, cell_area, 
				    NULL, NULL, &width, &height);

	text_area.x = cell_area->x + (cell_area->width - width) / 2;
	text_area.y = cell_area->y + (pixbuf_area.height + PAD);
	text_area.height = height;
	text_area.width = width;

	gtk_cell_renderer_render (cell->image, window, widget, 
				  background_area, &pixbuf_area,
				  expose_area, flags);

	gtk_cell_renderer_render (cell->caption, window, widget,
				  background_area, &text_area,
				  expose_area, flags);
}

static void
gmameui_cell_renderer_captioned_image_dispose (GObject *obj)
{
	GmameuiCellRendererCaptionedImage *cell = GMAMEUI_CELL_RENDERER_CAPTIONED_IMAGE (obj);
	
	g_object_unref (cell->image);
	g_object_unref (cell->caption);
}

static void
gmameui_cell_renderer_captioned_image_init (GmameuiCellRendererCaptionedImage *cell)
{
	cell->image = gtk_cell_renderer_pixbuf_new ();
	cell->caption = gtk_cell_renderer_text_new ();
}

static void
gmameui_cell_renderer_captioned_image_class_init (GmameuiCellRendererCaptionedImageClass *class)
{
	GObjectClass *object_class = G_OBJECT_CLASS (class);
	GtkCellRendererClass *cell_class = GTK_CELL_RENDERER_CLASS (class);
	
	object_class->dispose = gmameui_cell_renderer_captioned_image_dispose;
	
	object_class->get_property = gmameui_cell_renderer_captioned_image_get_property;
	object_class->set_property = gmameui_cell_renderer_captioned_image_set_property;
	
	cell_class->get_size = gmameui_cell_renderer_captioned_image_get_size;
	cell_class->render = gmameui_cell_renderer_captioned_image_render;

	g_object_class_install_property (object_class,
					 PROP_TEXT,
					 g_param_spec_string ("text",
							      _("Text"),
							      _("Text to render"),
							      NULL,
							      G_PARAM_READWRITE));
	g_object_class_install_property (object_class,
					 PROP_PIXBUF,
					 g_param_spec_object ("pixbuf",
							      _("Pixbuf Object"),
							      _("The pixbuf to render."),
							      GDK_TYPE_PIXBUF,
							      G_PARAM_READWRITE));
}
