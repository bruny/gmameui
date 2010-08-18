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

#ifndef __GMAMEUI_LISTOUTPUT_H__
#define __GMAMEUI_LISTOUTPUT_H__

#include "mame-exec.h"
#include "rom_entry.h"

G_BEGIN_DECLS

/* Preferences object */
#define GMAMEUI_TYPE_LISTOUTPUT            (gmameui_listoutput_get_type ())
#define GMAMEUI_LISTOUTPUT(o)            (G_TYPE_CHECK_INSTANCE_CAST((o), GMAMEUI_TYPE_LISTOUTPUT, GMAMEUIListOutput))
#define GMAMEUI_LISTOUTPUT_CLASS(k)    (G_TYPE_CHECK_CLASS_CAST ((k), GMAMEUI_TYPE_LISTOUTPUT, GMAMEUIListOutputClass))
#define GMAMEUI_IS_LISTOUTPUT(o)         (G_TYPE_CHECK_INSTANCE_TYPE ((o), GMAMEUI_TYPE_LISTOUTPUT))
#define GMAMEUI_IS_LISTOUTPUT_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), GMAMEUI_TYPE_LISTOUTPUT))
#define GMAMEUI_LISTOUTPUT_GET_CLASS(o)  (G_TYPE_INSTANCE_GET_CLASS ((o), GMAMEUI_TYPE_LISTOUTPUT, GMAMEUIListOutputClass))

typedef struct _GMAMEUIListOutput GMAMEUIListOutput;
typedef struct _GMAMEUIListOutputClass GMAMEUIListOutputClass;
typedef struct _GMAMEUIListOutputPrivate GMAMEUIListOutputPrivate;

struct _GMAMEUIListOutput {
	GObject parent;
	
	GMAMEUIListOutputPrivate *priv;
	/* define public instance variables here */
};

struct _GMAMEUIListOutputClass {
	GObjectClass parent;
	/* define vtable methods and signals here */

	/* Signal prototypes */
	//void  (* listoutput_start) (GMAMEUIListOutput *parser);
	//void  (* listoutput_finish) (GMAMEUIListOutput *parser);
};

/* Preferences */
enum
{
	PROP_LISTOUTPUT_0,
	/* UI preferences */


	NUM_LISTOUTPUT_PROPERTIES
};


GType gmameui_listoutput_get_type (void);
GMAMEUIListOutput* gmameui_listoutput_new (void);
void gmameui_listoutput_set_exec (GMAMEUIListOutput *parser, MameExec *exec);
gboolean gmameui_listoutput_parse (GMAMEUIListOutput *parser);
gboolean gmameui_listoutput_parse_stop (GMAMEUIListOutput *parser);
MameRomEntry* gmameui_listoutput_parse_rom (GMAMEUIListOutput *parser, MameExec *exec, MameRomEntry *rom);
gboolean gmameui_listoutput_generate_rom_hash (GMAMEUIListOutput *parser, MameExec *exec);

G_END_DECLS

#endif /* __GMAMEUI_LISTOUTPUT_H__ */

