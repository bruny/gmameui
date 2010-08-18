/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * GMAMEUI
 *
 * Copyright 2010 Andrew Burton <adb@iinet.net.au>
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

#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <expat.h>
#include <stdarg.h>

#include "gmameui-listoutput.h"
#include "mame-exec.h"
#include "gui.h"	/* FIXME TODO For gui_prefs */
#include "gmameui-marshaller.h"

#define BUFFER_SIZE 1000
#define XML_BUFFER_SIZE 4096

static void gmameui_listoutput_class_init (GMAMEUIListOutputClass *klass);
static void gmameui_listoutput_init (GMAMEUIListOutput *pr);
static void gmameui_listoutput_finalize (GObject *obj);

G_DEFINE_TYPE (GMAMEUIListOutput, gmameui_listoutput, G_TYPE_OBJECT)

struct _GMAMEUIListOutputPrivate {
	MameExec *exec;

	gint total_games;		/* Total number of romsets reported by parser */
	gint game_count;		/* Progress count of romsets parsed so far */

	FILE *mameHandle;		/* Expat reference to the XML stream input */
	XML_Parser xmlParser;		/* Expat XML parser */
	gboolean stop;			/* Flag to handle user cancelling the process */

	int character_count;		/* Handle XML input buffer */
	char text_buf[BUFFER_SIZE];     /* Handle XML input buffer */

	MameRomEntry *current_rom;      /* ROM being populated from the XML input */
	gchar *current_romset_name;     /* Romset being processed from the XML input - used as
					   lightweight alternative to current_rom when creating
					   hashtable of ROM information */
	gboolean processing_romset;     /* Are we in the middle of processing a ROM? */
	
	int cpu_count;
	int sound_count;

	
};

/* Signals enumeration */
enum
{
	LISTOUTPUT_PARSE_STARTED,		/* Emitted when starting -listinfo or -listxml */
	LISTOUTPUT_PARSE_FINISHED,		/* Emitted when parse is completed */
	LISTOUTPUT_ROMSET_PARSED,		/* Emitted when a romset has been parsed */
	LAST_SIGNAL
};

/* Audit class stuff */
static guint signals[LAST_SIGNAL] = { 0 };

void
gmameui_listoutput_set_exec (GMAMEUIListOutput *parser, MameExec *exec)
{

	g_return_if_fail (exec != NULL);

	parser->priv->exec = exec;
}

/* Create a new ROM from the XML handling */
static void
create_gamelist_romset (GMAMEUIListOutput *parser)
{
	gchar *romname, *driver;
	
	MameRomEntry *rom = parser->priv->current_rom;

	g_return_if_fail (rom != NULL);

	++parser->priv->game_count;
	
	g_object_get (rom, "romname", &romname, "driver", &driver, NULL);
	
	if (driver)
		mame_gamelist_add (gui_prefs.gl, rom);

	g_signal_emit (parser, signals[LISTOUTPUT_ROMSET_PARSED],
	               0, romname,
	               parser->priv->game_count, parser->priv->total_games);

	// g_object_unref (parser->priv->current_rom);	/* FIXME TODO Fails in compare_game_name */
}

static const
XML_Char *XMLGetAttrPtr(const XML_Char **atts, const XML_Char *name)
{
	for(; atts[0]; atts += 2) {
		if(!strcmp(atts[0], name))
			return atts[1];
	}

	return 0;
}

static const
XML_Char *XMLGetAttr(const XML_Char **atts, const XML_Char *name, const XML_Char *def)
 {
	const XML_Char *ret = XMLGetAttrPtr(atts, name);

	if(ret) 
		return ret;
	else
		return def;
}

static int
read_int_attribute (const XML_Char **atts, const XML_Char *name, int def)
{
	const XML_Char *ret = XMLGetAttrPtr(atts, name);

	if (!ret)
		return def;
	else
		return atoi(ret);
}

static gchar *
read_string_attribute (const XML_Char **atts, const XML_Char *name)
{
	const XML_Char *ret = XMLGetAttrPtr(atts, name);

	return (gchar *) ret;
}

static gboolean
read_boolean_attribute (const XML_Char **atts, const XML_Char *name, const gchar* comparison)
{
	const XML_Char *ret = XMLGetAttrPtr(atts, name);
	
	if (ret == NULL)
		return FALSE;
	
	return !strcmp(ret, comparison);

}

gint cpu_count;
gint sound_count;

static void
XMLDataHandler (GMAMEUIListOutput *parser, const XML_Char *s, int len)
{
	char *p;

	size_t chars_remain;

	chars_remain = BUFFER_SIZE - parser->priv->character_count;
	if (len == 0)
		return;

	if (chars_remain > 0) {
		size_t copy_chars;
		
		p = &parser->priv->text_buf[parser->priv->character_count];

		copy_chars = (chars_remain > (size_t)len)? (size_t)len: chars_remain - 1;
		
		memcpy(p, s, copy_chars);

		p[copy_chars+1] = '\0';

		parser->priv->character_count += copy_chars;
	}
}

static void
XMLStartRomHandler2 (void *user_data, const XML_Char *name, const XML_Char **atts)
{
	GMAMEUIListOutput *parser = (GMAMEUIListOutput *) user_data;

	if(!strcmp(name, "game"))
	{
		gchar *romname;
		romname = g_strdup (read_string_attribute (atts, "name"));
/*GMAMEUI_DEBUG ("  Setting up romset %s", romname);*/
		parser->priv->current_rom = get_rom_from_gamelist_by_name (gui_prefs.gl, romname);
		g_free (romname);
		
		parser->priv->processing_romset = TRUE;
	}
	else if (g_ascii_strcasecmp (name, "rom") == 0)
	{
		/* Only start processing rom info if we are working on a romset */
		if (parser->priv->processing_romset == TRUE) {
			individual_rom *rom_value = (individual_rom *) g_malloc0 (sizeof (individual_rom));

			rom_value->name = g_strdup (read_string_attribute (atts, "name"));
			rom_value->sha1 = g_strdup (read_string_attribute (atts, "sha1"));
			rom_value->crc = /*read_int_attribute (atts, "crc", 0);*/
				g_strdup (read_string_attribute (atts, "crc"));
			rom_value->merge = g_strdup (read_string_attribute (atts, "merge"));
			rom_value->status = g_strdup (read_string_attribute (atts, "status"));

			/*GMAMEUI_DEBUG ("    Reading information for %s in romset %s",
				       rom_value->name, mame_rom_entry_get_romname (parser->priv->current_rom));*/
		
			/* FIXME TODO romset listing may be null */
			/* FIXME TODO Pass the name and sha1 so we can hide individual_rom struct in rom_entry.c */
			mame_rom_entry_add_rom_ref (parser->priv->current_rom, rom_value);
		}

	}
}

static void
XMLEndRomHandler2 (void *user_data, const XML_Char *name, const XML_Char **atts)
{
	GMAMEUIListOutput *parser = (GMAMEUIListOutput *) user_data;

	if(!strcmp(name, "game"))
	{
		parser->priv->processing_romset = FALSE;
	}
}

static void
XMLEndRomHandler (MameRomEntry *rom, const XML_Char *name)
{
	/* No need to do anything here */
}

static void
XMLStartRomHandler (void *user_data, const XML_Char *name, const XML_Char **atts)
{
	/* i.e. we are now processing the ROM data */

	MameRomEntry *rom = (MameRomEntry *) user_data;
	int i;
	
	if (g_ascii_strcasecmp (name, "rom") == 0)
	{
		individual_rom *rom_value = (individual_rom *) g_malloc0 (sizeof (individual_rom));

		rom_value->name = g_strdup (read_string_attribute (atts, "name"));
		rom_value->sha1 = g_strdup (read_string_attribute (atts, "sha1"));
		rom_value->crc = g_strdup (read_string_attribute (atts, "crc"));
		rom_value->merge = g_strdup (read_string_attribute (atts, "merge"));
		rom_value->status = g_strdup (read_string_attribute (atts, "status"));

		GMAMEUI_DEBUG ("  Reading information for %s in romset %s",
		               rom_value->name, mame_rom_entry_get_romname (rom));
		
		/* FIXME TODO romset listing may be null */
		/* FIXME TODO Pass the name and sha1 so we can hide individual_rom struct in rom_entry.c */
		mame_rom_entry_add_rom_ref (rom, rom_value);

	} else if(!strcmp(name, "chip")) {
		const char *type = XMLGetAttr(atts, "type", 0);

		if(!strcmp(type, "cpu")){
			if(cpu_count <= NB_CPU) {
				CPUInfo *cpu = (CPUInfo *) g_malloc0 (sizeof (CPUInfo));

				cpu->sound_flag = FALSE;
				cpu->name = "-";
				cpu->clock = 0;

				cpu->name = g_strdup (read_string_attribute (atts, "name"));
				cpu->clock = read_int_attribute (atts, "clock", 0);
				/* sound_flag doesn't appear to be in the
				   -listxml output anymore */
				cpu->sound_flag = read_boolean_attribute (atts, "soundonly", "yes");

				mame_rom_entry_add_cpu (rom, cpu_count, cpu->name, cpu->clock);
				cpu_count++;
					
				g_free (cpu);
			}
		} else if(!strcmp(type, "audio")) {
			if(sound_count <= NB_CPU) {
				SoundCPUInfo *cpu = (SoundCPUInfo *) g_malloc0 (sizeof (SoundCPUInfo));

				cpu->name = "-";
				cpu->clock = 0;
					
				cpu->name = g_strdup (read_string_attribute (atts, "name"));
				cpu->clock = read_int_attribute (atts, "clock", 0);

				mame_rom_entry_add_soundcpu (rom, sound_count, cpu->name, cpu->clock);
				sound_count++;

				g_free (cpu);
			}
		}
	} else if(!strcmp(name, "input")) {

		g_object_set (rom,
			      "num-players", read_int_attribute (atts, "players", 0),
			      "num-buttons", read_int_attribute (atts, "buttons", 0),
			      NULL);
	} else if(!strcmp(name, "driver")) {
			
		g_object_set (rom,
			      "num-colours", read_int_attribute (atts, "palettesize", 0),
			      NULL);
	} else if(!strcmp(name, "video")) {
		for (i =0; atts[i]; i += 2) {
			if (!strcmp(atts[i], "refresh"))  {
				g_object_set (rom, "screen-freq", g_ascii_strtod(atts[i+1], NULL), NULL);
			}
		}
		g_object_set (rom,
			      "screenx", read_int_attribute (atts, "width", 0),
			      "screeny", read_int_attribute (atts, "height", 0),
			      NULL);
	} else if (!strcmp(name, "display")) {
		/* New for SDLMame and MAME32. Values will be:
		<display type="raster" rotate="0" width="256" height="240" refresh="60.000000" />
		<display type="vector" rotate="0" refresh="60.000000" />
		<display type="vector" rotate="180" flipx="yes" refresh="38.000000" /> */
		for (i =0; atts[i]; i += 2) {
			if (!strcmp(atts[i], "refresh")) {
				g_object_set (rom, "screen-freq", g_ascii_strtod(atts[i+1], NULL), NULL);
			}
		}
			
		g_object_set (rom,
			      "screenx", read_int_attribute (atts, "width", 0),
			      "screeny", read_int_attribute (atts, "height", 0),
			      NULL);
	}

}

// AAA FIXME TODO Not required
static void
XMLStartIndivRomHandler (void *user_data, const XML_Char *name, const XML_Char **atts)
{
	/* i.e. we are now processing the ROM data */

	GMAMEUIListOutput *parser = (GMAMEUIListOutput *) user_data;
	
	if (g_ascii_strcasecmp (name, "game") == 0) {
		parser->priv->current_romset_name = g_strdup (read_string_attribute (atts, "name"));
		/*GMAMEUI_DEBUG ("  Processing romset %s", parser->priv->current_romset_name);*/
	} else if (g_ascii_strcasecmp (name, "rom") == 0) {
		gchar *sha1;
		sha1 = g_strdup (read_string_attribute (atts, "sha1"));
		if (sha1) {
			/*GMAMEUI_DEBUG ("    Adding reference to %s from romset %s to hash table",
				       read_string_attribute (atts, "sha1"),
				       parser->priv->current_romset_name);*/
/*AAA FIXME TODO - only insert if it doesn't exist; if it does, then existing entry will be leaked
AAA FIXME TODO - only insert if ROM is available*/
			g_hash_table_insert (gui_prefs.rom_hashtable,
				             g_strdup (sha1),
				             g_strdup (parser->priv->current_romset_name));
			g_free (sha1);
		} /*else {
			GMAMEUI_DEBUG ("    Skipping %s from fromset %s - bad or no dump",
			               read_string_attribute (atts, "name"),
				       parser->priv->current_romset_name);
		}*/
	}
}

static void
XMLEndIndivRomHandler (MameRomEntry *rom, const XML_Char *name)
{
	/* No need to do anything here */
}

static void
XMLStartHandler (GMAMEUIListOutput *parser, const XML_Char *name, const XML_Char **atts)
{
	MameRomEntry *rom;
	int i;	

	/* Check to see if the user has requested to stop the parsing process */
	if (parser->priv->stop) {
		GMAMEUI_DEBUG ("Request made to stop the parsing...");
		XML_StopParser (parser->priv->xmlParser, FALSE);
		GMAMEUI_DEBUG ("... parsing stopped");
	}

	XML_SetCharacterDataHandler(parser->priv->xmlParser, NULL);

	if(!strcmp(name, "game"))
	{
		char *p;
		char *tmp;

		parser->priv->current_rom = mame_rom_entry_new ();
		parser->priv->cpu_count = 0;
		parser->priv->sound_count = 0;

		rom = parser->priv->current_rom;

		mame_rom_entry_set_romname (rom, g_strdup (read_string_attribute (atts, "name")));
		mame_rom_entry_set_cloneof (rom, g_strdup (read_string_attribute (atts, "cloneof")));
		mame_rom_entry_set_romof (rom, g_strdup (read_string_attribute (atts, "romof")));
		mame_rom_entry_set_isbios (rom, read_boolean_attribute (atts, "isbios", "yes"));
		
		for (i =0; atts[i]; i += 2) {
			if (!strcmp(atts[i], "sourcefile")) {

				tmp = g_strdup(atts[i+1]);

				/* strip extension from sourcefile */
				for (p = tmp; *p && *p != '.'; p++);
				*p = 0;

				mame_rom_entry_set_driver (rom, tmp);
				g_free(tmp);
			}
		}
	}
	else if (parser->priv->current_rom)
	{
		rom = parser->priv->current_rom;
		
		if(!strcmp(name, "rom")) {
			mame_rom_entry_add_rom (rom);
		} else if(!strcmp(name, "input")) {
			for (i =0; atts[i]; i += 2) {
				if (!strcmp(atts[i], "control")) {
					g_object_set (rom, "control-type", get_control_type (atts[i+1]), NULL);
				}
			}
		} else if(!strcmp(name, "control")) {
			/* Control is an element of input in later versions of the
			   XML output */
			g_object_set (rom, "control-type", get_control_type (read_string_attribute (atts, "type")), NULL);
			
		} else if(!strcmp(name, "driver")) {
			
			g_object_set (rom,
				      "driver-status", get_driver_status (read_string_attribute (atts, "status")),
				      "driver-status-emulation", get_driver_status (read_string_attribute (atts, "emulation")),
				      "driver-status-colour", get_driver_status (read_string_attribute (atts, "color")),
				      "driver-status-sound", get_driver_status (read_string_attribute (atts, "sound")),
				      "driver-status-graphics", get_driver_status (read_string_attribute (atts, "graphic")),
				      NULL);
		} else if(!strcmp(name, "video")) {
			g_object_set (rom,
				      "is-horizontal", read_boolean_attribute (atts, "orientation", "horizontal"),
				      "is-vector", read_boolean_attribute (atts, "screen", "vector"),
				      NULL);
		} else if (!strcmp(name, "display")) {
			/* New for SDLMame and MAME32. Values will be:
			<display type="raster" rotate="0" width="256" height="240" refresh="60.000000" />
			<display type="vector" rotate="0" refresh="60.000000" />
			<display type="vector" rotate="180" flipx="yes" refresh="38.000000" /> */
			for (i =0; atts[i]; i += 2) {
				if (!strcmp(atts[i], "type")) {
					g_object_set (rom, "is-vector", !strcmp(atts[i+1], "vector"), NULL);
				}
			}
		} else if(!strcmp(name, "sound")) {
			g_object_set (rom, "num-channels", read_int_attribute(atts, "channels", 0), NULL);
		} else if(!strcmp(name, "sample")) {
			mame_rom_entry_add_sample (rom);
		} else if(!strcmp(name, "year") || !strcmp(name, "description") || !strcmp(name, "manufacturer")) {

			XML_SetCharacterDataHandler(parser->priv->xmlParser, 
				(XML_CharacterDataHandler ) &XMLDataHandler);

			parser->priv->character_count = 0;
			memset(parser->priv->text_buf, 0, BUFFER_SIZE); 
		}
	}
}
 
static void
XMLEndHandler (GMAMEUIListOutput *parser, const XML_Char *name)
{
	if(!strcmp(name, "game")) {
		create_gamelist_romset (parser);
	} 
	else if (parser->priv->text_buf[0] && name) {
		MameRomEntry *rom = parser->priv->current_rom;
		
		g_return_if_fail (rom != NULL);
		
		if (!strcmp(name, "year")) {
			mame_rom_entry_set_year (rom, parser->priv->text_buf);
		} else if (!strcmp(name, "manufacturer")) {
			mame_rom_entry_set_manufacturer (rom, parser->priv->text_buf);
		} else if (!strcmp(name, "description")) {
			mame_rom_entry_set_name (rom, parser->priv->text_buf);
		}

	}
}

static gboolean
start_gamelist_parse (GMAMEUIListOutput *parser)
{
	int len;
	int final;
	int bufferPos = 0;

	XML_Parser xmlParser = parser->priv->xmlParser;
	FILE *mameHandle = parser->priv->mameHandle;

	parser->priv->game_count = 0;
	
	for(;;)
    {
		char *buffer;

		buffer = XML_GetBuffer(xmlParser, XML_BUFFER_SIZE);

		if (!buffer) {
			GMAMEUI_DEBUG("Failed to allocate buffer.");
		}

		if (mameHandle == NULL) {
			GMAMEUI_DEBUG ("The handle is no longer available");
			break;
		}

		len = fread(buffer, 1, XML_BUFFER_SIZE, mameHandle);
		final = !len;

		if(len && !XML_ParseBuffer(xmlParser, len, final))
		{
			int delta;
			/* FIXME : not tested */
			char *bufferPre;
			int sizePre;
			char *bufferPost;
			int sizePost;
			static const int contextSize = 80;
			int errorCode = XML_GetErrorCode(xmlParser);
			/* do not assume it >= 0 */

			int errorPosInBuffer = 
				XML_GetCurrentByteIndex(xmlParser) - bufferPos;
						
			if(errorPosInBuffer < 0)
			{
				static char buf[] = "<<<BEFORE>>>";
				bufferPre = buf;
				sizePre = sizeof(buf) -1;
			}
			else if((delta = errorPosInBuffer - contextSize) < 0)
			{
				bufferPre = buffer;
				sizePre = errorPosInBuffer;
			}
			else
	 		{
				bufferPre = buffer + delta;
				sizePre = contextSize;
			}

			if(errorPosInBuffer < len)  /* useless */
			{
				static char buf[] = "<<<AFTER>>>";
				bufferPost = buf;
				sizePost = sizeof(buf);
			}
			else if((delta = errorPosInBuffer + len - contextSize) < 0)
			{
				bufferPost = buffer + errorPosInBuffer;
				sizePost = -delta;
			}
			else
			{
				bufferPost = buffer + errorPosInBuffer;
				sizePost = contextSize;
			}

			fprintf(stderr, 
				"error %d:%s at Line:%d Column:%d at %d "
				"near \"%.*s\"<>\"%.*s"
				"\n",
				errorCode, XML_ErrorString(errorCode),
				(int) XML_GetCurrentLineNumber(xmlParser),
				(int) XML_GetCurrentColumnNumber(xmlParser),
				(int) XML_GetCurrentByteIndex(xmlParser),
				sizePre, bufferPre,
				sizePost, bufferPost);


			return FALSE;
		}
      
		if(final)
			break;

		bufferPos += len;
	}

	return TRUE;
}

/**
 *  Create a gamelist (GList consisting of MameRomEntry objects) from the output
 *  of -listxml. Triggered when rebuilding the gamelist.
 */
static gboolean
create_gamelist_xmlinfo (GMAMEUIListOutput *parser)
{
	gboolean res;

	g_return_val_if_fail (parser->priv->exec != NULL, FALSE);

	if (gui_prefs.gl) {
		g_object_unref (gui_prefs.gl);
		gui_prefs.gl = NULL;
		
		gui_prefs.gl = mame_gamelist_new ();
	}
	g_object_set (gui_prefs.gl,
		      "name", mame_exec_get_name (parser->priv->exec),
		      "version", mame_exec_get_version (parser->priv->exec),
		      NULL);

	parser->priv->total_games = mame_exec_get_game_count (parser->priv->exec);

	g_return_val_if_fail (parser->priv->total_games, FALSE);

	parser->priv->mameHandle = mame_open_pipe (parser->priv->exec, "-%s",
	                                           mame_get_option_name (parser->priv->exec, "listxml"));

	g_return_val_if_fail (parser->priv->mameHandle, FALSE);

	parser->priv->xmlParser = XML_ParserCreate (NULL);
	XML_SetElementHandler (parser->priv->xmlParser, 
	                       (XML_StartElementHandler) &XMLStartHandler,
	                       (XML_EndElementHandler)   &XMLEndHandler);

	/* Set the GMAMEUIListOutput object as user data so the priv object
	   data can be used in the parser event callbacks */
	XML_SetUserData (parser->priv->xmlParser, parser);

	res = start_gamelist_parse (parser);

	/* Clean up - also occurs if user Cancels the operation */
	GMAMEUI_DEBUG ("Cleaning up parser...");
	mame_close_pipe (parser->priv->exec, parser->priv->mameHandle);
	/*DELETEif (parser->priv->mameHandle) {
		pclose (parser->priv->mameHandle);
		parser->priv->mameHandle = NULL;
	}*/

	if (parser->priv->xmlParser);
		XML_ParserFree (parser->priv->xmlParser);
	GMAMEUI_DEBUG ("Cleaning up parser... done");
	
	return res;
}

/**
 *  Entry function to generate the -listxml or -listoutput for the full
 *  romset
 */
gboolean
gmameui_listoutput_parse (GMAMEUIListOutput *parser)
{
	gboolean ret;

	g_return_val_if_fail (parser->priv->exec != NULL, FALSE);

	/* Check if we will use listinfo or listxml. Later versions of XMAME and
	   all versions of SDLMAME use listxml. */
	if (mame_has_option (parser->priv->exec, "listinfo")) {
#ifdef OBSOLETE_XMAME
		/* Only versions 0.83 and earlier used listinfo */
		GMAMEUI_DEBUG ("Recreating gamelist using -listinfo\n");
		ret = create_gamelist_listinfo (parser->priv->exec);
#else
		GMAMEUI_DEBUG ("GMAMEUI does not support the obsolete option -listinfo");
#endif
	} else if (mame_has_option(parser->priv->exec, "listxml")) {
		GMAMEUI_DEBUG("Recreating gamelist using -listxml\n");
		ret = create_gamelist_xmlinfo (parser);
	} else {
		gmameui_message(ERROR, NULL, _("I don't know how to generate a gamelist for this version of MAME!"));
		ret = FALSE;
	}
	
	/* Emit signal indicating we are finished */
	g_signal_emit (parser, signals[LISTOUTPUT_PARSE_FINISHED], 0, NULL);
	
	/* Hack - create_gamelist_xmlinfo and _listinfo unref the gui_prefs.gl
	   object, which also destroys the current_game object. This is a short
	   term hack to reset it */
	gchar *current_rom;
	g_object_get (main_gui.gui_prefs, "current-rom", &current_rom, NULL);
	gui_prefs.current_game = get_rom_from_gamelist_by_name (gui_prefs.gl, current_rom);
	/* End hack - once we push the gui_prefs as a g_object, we should be
	   able to delete the hack */
	return ret;
}


/**
 *  Update the ROM to include other information not contained in the gamelist
 *  file but which is available from the -listxml option. Usually triggered
 *  from the MAME ROM Information dialog.
 */
MameRomEntry *
gmameui_listoutput_parse_rom (GMAMEUIListOutput *parser,
                              MameExec *exec,
                              MameRomEntry *rom)

{
	//XML_Parser xmlParser;
	//FILE *mame_handle;
	
	g_return_val_if_fail (rom != NULL, NULL);
	
	cpu_count = sound_count = 0;

	GMAMEUI_DEBUG ("Starting parsing ROM");
	parser->priv->mameHandle = mame_open_pipe(exec, "-%s %s",
				     mame_get_option_name(exec, "listxml"),
				     mame_rom_entry_get_romname (rom));

	g_return_val_if_fail (parser->priv->mameHandle != NULL, FALSE);
	
	parser->priv->xmlParser = XML_ParserCreate (NULL);
	XML_SetElementHandler (parser->priv->xmlParser,
			       (XML_StartElementHandler) &XMLStartRomHandler,
			       (XML_EndElementHandler)   &XMLEndRomHandler);
	XML_SetUserData (parser->priv->xmlParser, rom);
	
	int final;
	int bytes_read;
	for (;;) {
		char *buffer;

		buffer = XML_GetBuffer (parser->priv->xmlParser, XML_BUFFER_SIZE);
		
		if (!buffer) {
			GMAMEUI_DEBUG("Failed to allocate buffer.");
		}

		bytes_read = fread (buffer, 1, XML_BUFFER_SIZE, parser->priv->mameHandle);
		final = !bytes_read;

		if (bytes_read && !XML_ParseBuffer(parser->priv->xmlParser, bytes_read, final))
		{
			GMAMEUI_DEBUG ("Error!");
		}
      
		if (final)
			break;

	}

	mame_close_pipe (exec, parser->priv->mameHandle);
	/*DELETEpclose (parser->priv->mameHandle);
	parser->priv->mameHandle = NULL;*/
	XML_ParserFree (parser->priv->xmlParser);
	
	return rom;
}

/**
 *  Entry method to generate the hashtable containing the ROM SHA1 and romset
 *  information. Used in fixing romsets (might be possible to merge this with the
 *  listxml output above
 */
// AAA FIXME Rename since purpose is different
gboolean
gmameui_listoutput_generate_rom_hash (GMAMEUIListOutput *parser, MameExec *exec)
{
	GTimer *timer;

	g_return_val_if_fail (parser != NULL, FALSE);
	g_return_val_if_fail (exec != NULL, FALSE);
//DELETE	g_return_val_if_fail (gui_prefs.rom_hashtable != NULL, FALSE);

	timer = g_timer_new ();
	
	parser->priv->cpu_count = parser->priv->sound_count = 0;

	GMAMEUI_DEBUG ("Starting parsing listxml for ROM information");
	parser->priv->mameHandle = mame_open_pipe (exec, "-%s",
				      mame_get_option_name(exec, "listxml"));

	g_return_val_if_fail (parser->priv->mameHandle != NULL, FALSE);
	
	parser->priv->xmlParser = XML_ParserCreate (NULL);
	XML_SetElementHandler (parser->priv->xmlParser,
			       (XML_StartElementHandler) &XMLStartRomHandler2,
			       (XML_EndElementHandler)   &XMLEndRomHandler2);
	
	/* Set the GMAMEUIListOutput object as user data so the priv object
	   data can be used in the parser event callbacks */
	XML_SetUserData (parser->priv->xmlParser, parser);

	start_gamelist_parse (parser);

	mame_close_pipe (exec, parser->priv->mameHandle);

	XML_ParserFree (parser->priv->xmlParser);
	
	GMAMEUI_DEBUG ("Finished parsing listxml for ROM information in %0.2f seconds", g_timer_elapsed (timer, NULL));

	g_timer_destroy (timer);
	
	return TRUE;
}

/* Stop the parsing of the listxml output. This is usually triggered upon the
   user clicking Cancel in the parsing dialog */
gboolean gmameui_listoutput_parse_stop (GMAMEUIListOutput *parser)
{
	GMAMEUI_DEBUG ("Stopping the listoutput parse...");
	pclose (parser->priv->mameHandle);
	parser->priv->mameHandle = NULL;

	/* Set the flag to TRUE so that we can handle it in the appropriate XML
	   handling callback - expat.h says we can only stop the parsing in
	   a callback */
	parser->priv->stop = TRUE;

	GMAMEUI_DEBUG ("Stopping the listoutput parse... done");

	return TRUE;
}

static void
gmameui_listoutput_finalize (GObject *obj)
{
	
	GMAMEUIListOutput *parser = GMAMEUI_LISTOUTPUT (obj);

	GMAMEUI_DEBUG ("Finalising gmameui_listoutput object...");

	/* FIXME TODO Can't unref until this is a gobject
	if (parser->priv->exec)
		g_object_unref (parser->priv->exec);*/

	/* FIXME TODO Causes segfault
	if (parser->priv->current_rom)
		g_object_unref (parser->priv->current_rom);*/
	
// FIXME TODO	g_free (parser->priv);

	GMAMEUI_DEBUG ("Finalising gmameui_listoutput object... done");
	

}

static void
gmameui_listoutput_class_init (GMAMEUIListOutputClass *klass)
{
	
	
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	
	object_class->finalize = gmameui_listoutput_finalize;
	
	signals[LISTOUTPUT_PARSE_STARTED] = g_signal_new ("listoutput-parse-started",
						G_OBJECT_CLASS_TYPE (object_class),
						G_SIGNAL_RUN_FIRST,
						/*G_STRUCT_OFFSET (GMAMEUIListOutputClass, listoutput_start),*/
	                    0,		/* This signal is not handled by the class */
						NULL, NULL,     /* Accumulator and accumulator data */
						g_cclosure_marshal_VOID__VOID,
						G_TYPE_NONE,    /* Return type */
						0	        /* No parameters */
						);
	
	signals[LISTOUTPUT_PARSE_FINISHED] = g_signal_new ("listoutput-parse-finished",
						G_TYPE_FROM_CLASS(klass),
						G_SIGNAL_RUN_FIRST,
						0,		/* This signal is not handled by the class */
						NULL, NULL,     /* Accumulator and accumulator data */
						g_cclosure_marshal_VOID__VOID,
						G_TYPE_NONE,    /* Return type */
						0	        /* No parameters */
						);

	signals[LISTOUTPUT_ROMSET_PARSED] = g_signal_new ("listoutput-romset-parsed",
						G_TYPE_FROM_CLASS(klass),
						G_SIGNAL_RUN_FIRST,
						0,		/* This signal is not handled by the class */
						NULL, NULL,     /* Accumulator and accumulator data */
						gmameui_marshaller_VOID__STRING_INT_INT,
						G_TYPE_NONE,    /* Return type */
						3, G_TYPE_STRING, G_TYPE_INT, G_TYPE_INT	/* Three parameters */
						);
}

static void
gmameui_listoutput_init (GMAMEUIListOutput *pr)
{
	pr->priv = g_new0 (GMAMEUIListOutputPrivate, 1);

	pr->priv->stop = FALSE; /* Set to TRUE in XML handling callback to
	 stop the XML parsing */
}

GMAMEUIListOutput* gmameui_listoutput_new (void)
{
	return g_object_new (GMAMEUI_TYPE_LISTOUTPUT, NULL);
}


#ifdef OBSOLETE_XMAME

/* The following code is used for MAME versions (0.92 at least uses listxml)
   that only support -listinfo. It is retained here for reference */

/**
* Gets a hash table with all supported drivers
* Updates game count;
*/
static GHashTable *
get_driver_table (MameExec *exec, int *total_games)
{
	FILE *xmame_pipe;
	gchar line[BUFFER_SIZE];
	GHashTable *driver_htable;
	gchar *driverkey;
	gchar *drivervalue;
	gchar *driverkey2;
	gchar *drivervalue2;
	int i;
	char *p, *p2;

	/* Generate the table for drivers */
	/* without including neither history nor mameinfo to have less to parse after*/
	xmame_pipe = mame_open_pipe(exec, "-%s -%s /dev/null -%s /dev/null",
				     mame_get_option_name(exec, "listsourcefile"), 
				     mame_get_option_name(exec, "mameinfo_file"),
				     mame_get_option_name(exec, "history_file"));

	g_return_val_if_fail (xmame_pipe != NULL, NULL);

	driver_htable = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);

	/*why the hell there is a delay when we reach the end of this loop???
	  Probably because XMame <0.61 didn't flushed out the buffer */
	while (fgets(line, BUFFER_SIZE, xmame_pipe))
	{
		driverkey = p = p2 = line;
		for(i = 0;(*p && (*p != '/') && (*p != ':') && (*p != '\n'));i++,p++);
		/* we should got the total supported games*/
		if (*p == '/')
		{
			for(p2++;(*p2 && (*p2++ != ' ')););
			*p2 = '\0';
			for(p++;(*p && (*p++ != '/')););
			drivervalue = p;
			for(i=0,p++;(*p && (*p != '.') && (*p != '\n'));i++,p++);
			*p = '\0';

			if (i > 0)
			{
				/* need to remove the null character otherwise, the g_str_equal does work 
				   at the next stage */
				driverkey2=g_strndup(driverkey,strlen(driverkey)-1);
				drivervalue2=g_strdup(drivervalue);
				g_hash_table_insert(driver_htable,driverkey2,drivervalue2);
			}
			
		} else if (*p == ':')
		{
			p[0]='\0';
			if (!strcmp(driverkey,"Total Supported"))
			{
				p += 2;
				for(drivervalue = p;(*p && (*p != '\n'));p++);
				*p = '\0';
				*total_games = atoi(drivervalue);
				GMAMEUI_DEBUG("total games supported=:%d", *total_games);
			}
		}

		/* need to update here otherwise, the progress window stay blanc until
		we exit this loop */
		while (gtk_events_pending()) gtk_main_iteration();
	}

	mame_close_pipe(exec, xmame_pipe);
	GMAMEUI_DEBUG("drivers list loaded");

	return driver_htable;
}

/**
 *  Create a gamelist (GList consisting of MameRomEntry objects) from the output
 *  of -listinfo. Triggered when rebuilding the gamelist.
 *  Note this function is for legacy versions of MAME. More recent versions use
 *  -listxml.
 */
static gboolean
create_gamelist_listinfo (MameExec *exec)
{
	FILE *xmame_pipe;
	gchar line[BUFFER_SIZE];
	MameRomEntry *rom;
	char term_char;	

	gchar *p = NULL;
	gchar *keyword = NULL;
	gchar *value = NULL;
	gchar **tmp_array;
	gchar *line_key;
	gchar *driver;
	gint tmp_counter = 0;

	gint cpu_count;
	gint sound_count;
	gfloat done;
	int num_games = 0, total_games = 0;

	GHashTable *driver_htable;

	driver_htable = get_driver_table(exec, &total_games);

	if (!driver_htable) {
		progress_window_destroy(progress_window);
		return FALSE;
	}

	if (gui_prefs.gl) {
		g_object_unref (gui_prefs.gl);
		gui_prefs.gl = NULL;
		
		gui_prefs.gl = mame_gamelist_new ();
	}

	g_object_set (gui_prefs.gl,
		      "name", mame_exec_get_name (exec),
		      "version", mame_exec_get_version (exec),
		      NULL);
	
	g_message(_("creating game list, Please wait:"));
		
	/* Generate the list */
	/* without including neither history nor mameinfo to have less to parse after*/
	xmame_pipe = mame_open_pipe(exec, "-%s -%s /dev/null -%s /dev/null",
				     mame_get_option_name(exec, "listinfo"),
				     mame_get_option_name(exec, "mameinfo_file"),
				     mame_get_option_name(exec, "history_file"));
	
	while (fgets(line, BUFFER_SIZE, xmame_pipe))
	{
		/*the game begin here*/
		if (!strncmp(line, "game (", 6)) {
			rom = mame_rom_entry_new ();
			cpu_count = 0;
			sound_count = 0;

			while (fgets(line, BUFFER_SIZE, xmame_pipe)) {
				/* the game ends here */
				if (line[0] == ')')
					break;

				/* find the keyword separated by a space */
				for (keyword = line; (*keyword == ' ') || (*keyword == '\t'); keyword++);

				if (*keyword == 0)
					continue;

				/* null terminate the keyword */
				for (p = keyword+1; (*p) && (*p != ' '); p++);

				/* set the value pointer to the start of the value */
				if (*p) {
					*p = 0;
					value = p + 1;
				} else {
					value = p;
				}

				/* skip spaces until the value begins */
				if (*value) {
					for(; *value == ' '; value++);
				}

				/* terminate character is the same */
				if ((*value == '(') || (*value == '\"')) {

					if (*value == '(')
						term_char = ')';
					else
						term_char = '\"';

					/* skip spaces until the beginning of the inner value */
					for(value = value + 1; *value == ' '; value++);
				}
				else
					term_char = ' ';

				/* terminate value at null or the terminating character
				* also allow for escape sequences such as '\"' or '\)'
				*/
				for (p = value; ((*p) && (*p != '\n')) && (*p != term_char || (*(p-1) == '\\')); p++);
				*p = 0;

				if (*(p-1) == ' ')
					*(p-1) = 0;

				if (!strcmp(keyword, "name")) {
					mame_rom_entry_set_romname (rom, value);
				} else if (!strcmp(keyword, "description"))
				{
					mame_rom_entry_set_name (rom, value);
				}
				else if (!strcmp(keyword, "year"))
					mame_rom_entry_set_year (rom, value);
				else if (!strcmp(keyword, "manufacturer"))
				{
					if(strncmp(value, "???", 3))
						mame_rom_entry_set_manufacturer (rom, value);
				}
				else if (!strcmp(keyword, "rom"))
					mame_rom_entry_add_rom (rom);
				else if (!strcmp(keyword, "sample"))
					mame_rom_entry_add_sample (rom);
				else if (!strcmp(keyword, "cloneof"))
					mame_rom_entry_set_cloneof (rom, value);
				else if (!strcmp(keyword, "romof"))
					mame_rom_entry_set_romof (rom, value);
				/* FIXME TODO else if (!strcmp(keyword, "sampleof"))
					rom->sampleof = g_strdup(value);*/
				else if (!strcmp(keyword, "chip")) {

					gboolean is_sound;
					tmp_counter = 0;
					
					tmp_array = g_strsplit(value, " ",0);
				

					is_sound = strcmp(tmp_array[1], "cpu");
					tmp_counter = 2;
					
					while (tmp_array[tmp_counter]) {

					/* FIXME TODO	if (!strcmp(tmp_array[tmp_counter], "name")) {
							if (is_sound)
								rom->sound_info[sound_count].name = g_strdup (tmp_array[tmp_counter+1]);
							else
								rom->cpu_info[cpu_count].name = g_strdup (tmp_array[tmp_counter+1]);

							tmp_counter++;

						} else if (!strcmp(tmp_array[tmp_counter], "clock")) {
							if (is_sound)
								rom->sound_info[sound_count].clock = atoi(tmp_array[tmp_counter+1]);
							else
								rom->cpu_info[cpu_count].clock = atoi(tmp_array[tmp_counter+1]);

							tmp_counter++;

						} else if (!strcmp(tmp_array[tmp_counter], "flags")) {
						
							if (!is_sound)
								rom->cpu_info[cpu_count].sound_flag = TRUE;
						}*/
						

						tmp_counter++;
					}

					g_strfreev(tmp_array);
				}
				else if (!strcmp(keyword, "video")) {
					tmp_counter = 0;

					tmp_array = g_strsplit(value, " ", 0);					

					while (tmp_array[tmp_counter]) {

						line_key = tmp_array[tmp_counter];

						if (!strcmp(line_key, "screen")) {
							g_object_set (rom, "is-vector", !strncmp(tmp_array[tmp_counter+1], "vector", 6), NULL);
							tmp_counter++;
						} else if (!strcmp(line_key, "orientation")) {
							g_object_set (rom, "is-vector", !strncmp(tmp_array[tmp_counter+1], "h", 1), NULL);
							tmp_counter++;
						} else if (!strcmp(line_key, "x")) {
							g_object_set (rom, "screenx", atoi(tmp_array[tmp_counter+1]), NULL); 
							tmp_counter++;
						} else if (!strcmp(line_key, "y")) {
							g_object_set (rom, "screeny", atoi(tmp_array[tmp_counter+1]), NULL); 
						}

						tmp_counter++;
					}		

					g_strfreev(tmp_array);
				}
				else if (!strcmp(keyword, "input")) {
				
					/* same to extract different info from input field */
					tmp_array = g_strsplit(value, " ", 0);

					tmp_counter = 0;

					while ((tmp_array[tmp_counter] != NULL)) {
	
						if (!strcmp (tmp_array[tmp_counter], "players")) {
							g_object_set (rom,
								      "num-players",
								      atoi(tmp_array[tmp_counter + 1]),
								      NULL);
							
						}
						else if (!strcmp (tmp_array[tmp_counter], "control")) {
							g_object_set (rom,
								      "control",
								      get_control_type (tmp_array[tmp_counter + 1]),
								      NULL);

							tmp_counter++;		
						}else if (!strcmp (tmp_array[tmp_counter], "buttons")) {
							g_object_set (rom,
								      "num-buttons",
								      atoi(tmp_array[tmp_counter + 1]),
								      NULL);
							tmp_counter++;
						}
						tmp_counter++;
					}
					g_strfreev(tmp_array);

				} else if (!strcmp(keyword, "driver")) {
				
					tmp_array = g_strsplit(value, " ", 10);

					tmp_counter = 0;

					while ((tmp_array[tmp_counter] != NULL)) {

						if (!strcmp(tmp_array[tmp_counter], "status")) { 
						/* FIXME TODO rom->status = !strcmp(tmp_array[tmp_counter+1], "good");*/
							tmp_counter++;
						}
						else if (!strcmp(tmp_array[tmp_counter], "palettesize")) {
							g_object_set (rom,
								      "num-colours",
								      atoi(tmp_array[tmp_counter+1]),
								      NULL);
							tmp_counter++;
						}

						tmp_counter++;
					}

					g_strfreev(tmp_array);

				} else if (!strcmp(keyword, "sound"))
				{
					tmp_array = g_strsplit(value, " ",3);
					if (!strcmp(tmp_array[0], "channels"))
						g_object_set (rom,
							      "num-channels",
							      atoi(tmp_array[1]),
							      NULL);
					g_strfreev(tmp_array);
				}

			}
			
			driver = g_hash_table_lookup(driver_htable, mame_rom_entry_get_romname (rom));

			if (!driver)
				mame_rom_entry_set_driver (rom, "Unknown");
			else
				mame_rom_entry_set_driver (rom, driver);

			mame_gamelist_add (gui_prefs.gl, rom);

		}
	}
	
	pclose(xmame_pipe);
		
	g_hash_table_destroy(driver_htable);
	
	g_object_set (gui_prefs.gl, "num-games", num_games, NULL);

	return TRUE;
}
#endif


