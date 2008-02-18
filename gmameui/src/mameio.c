/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
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

#include "game_list.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <expat.h>
#include <stdarg.h>
#include <gtk/gtkmain.h>

#include "gui.h"
#include "progression_window.h"

#define BUFFER_SIZE 1000
#define MAX_ELEMENT_NAME 32

int xmame_exec_get_game_count(XmameExecutable *exec)
{
	FILE *handle;
	char buffer[1024];
	gchar *option;
	int ret = 0;
	int count = 0;

	if (!exec)
		return 0;

	xmame_get_options(exec);

	option = g_strdup (xmame_get_option_name(exec, "listfull"));
	GMAMEUI_DEBUG ("Retrieving full game list using %s.", option);

	handle = xmame_open_pipe(exec, "-%s", option);

	GMAMEUI_DEBUG ("Game list retrieved... parsing.");
	
	while(fgets(buffer, 1024, handle))
	{
		/* Skip the header row */
		if (!strncmp(buffer, "Name:     Description:", 22))
			continue;

		static char keywork[] = "Total Supported: ";
		if(!strncmp(buffer, keywork, sizeof(keywork) -1))
		{
			ret = atoi(buffer + sizeof(keywork) -1);
			break;
		}
		
		count++;
	}
	pclose(handle);

	/* If there was no line stating Total Supported:, then we have
	   to get a list of all rows returned (subtracting the header)
	   to get the final count */
	if (ret == 0) ret = count;
	
	GMAMEUI_DEBUG ("Game count obtained - %d", ret);
	
	g_free (option);

	return ret;
}

typedef struct 
{
	FILE *xmameHandle;
	ProgressWindow *progress_window;

	XML_Parser xmlParser;
	int character_count;
	char text_buf[BUFFER_SIZE];

	int game_count;
	int total_games;

	RomEntry *current_rom;
	int cpu_count;
	int sound_count;
} TCreateGameList;

static void CreateGameListProgress(TCreateGameList *_this)
{
	float pos;

	pos = _this->game_count ? ((float) _this->game_count) / _this->total_games : 0.0;

	if(pos > 0 && pos <= 1)
		progress_window_set_value(_this->progress_window, pos);

	UPDATE_GUI;
}

static void CreateGameListGame(TCreateGameList *_this)
{
	RomEntry *rom = _this->current_rom;

	if (!rom)
		return;

	++_this->game_count;

	if (rom->driver && rom->driver[0])
		gamelist_add(rom);

	CreateGameListProgress(_this);

	_this->current_rom = NULL;
}

static const XML_Char *XMLGetAttrPtr(const XML_Char **atts,
				     const XML_Char *name)
{
	for(; atts[0]; atts += 2) {
		if(!strcmp(atts[0], name))
			return atts[1];
	}

	return 0;
}

static const XML_Char *XMLGetAttr(const XML_Char **atts, 
				  const XML_Char *name,
				  const XML_Char *def)
 {
	const XML_Char *ret = XMLGetAttrPtr(atts, name);

	if(ret) 
		return ret;
	else
		return def;
}

static int read_int_attribute(const XML_Char **atts, 
			   const XML_Char *name,
			   int def) {
	const XML_Char *ret = XMLGetAttrPtr(atts, name);

	if (!ret)
		return def;
	else
		return atoi(ret);
}

static void XMLDataHandler(TCreateGameList *_this,
			   const XML_Char *s,
			   int len)
{
	char *p;

	size_t chars_remain;
	
	chars_remain = BUFFER_SIZE - _this->character_count;
	if (len == 0)
		return;

	if (chars_remain > 0) {
		size_t copy_chars;
		
		p = &_this->text_buf[_this->character_count];

		copy_chars = (chars_remain > (size_t)len)? (size_t)len: chars_remain - 1;
		
		memcpy(p, s, copy_chars);

		p[copy_chars+1] = '\0';

		_this->character_count += copy_chars;
	}
}

static void XMLStartHandler(TCreateGameList *_this,
			    const XML_Char *name,
			    const XML_Char **atts)
{
	RomEntry *rom;
	int i;	

	XML_SetCharacterDataHandler(_this->xmlParser, NULL);

	if(!strcmp(name, "game"))
	{
		char *p;
		char *tmp;
		

		_this->current_rom = rom_entry_new();
		_this->cpu_count = 0;
		_this->sound_count = 0;

		rom = _this->current_rom;

		for (i =0; atts[i]; i += 2) {
			if(!strcmp(atts[i], "name")) {
				g_strlcpy(rom->romname, atts[i+1], MAX_ROMNAME);

			} else if (!strcmp(atts[i], "cloneof")) {
				rom->cloneof = g_strdup(atts[i+1]);

			} else if (!strcmp(atts[i], "romof")) {
				rom->romof = g_strdup(atts[i+1]);
			} else if (!strcmp(atts[i], "isbios")) {
				rom->is_bios = !strcmp(atts[i+1], "yes");
			} else if (!strcmp(atts[i], "sourcefile")) {

				tmp = g_strdup(atts[i+1]);

				/* strip extension from sourcefile */
				for (p = tmp; *p && *p != '.'; p++);
				*p = 0;

				rom_entry_set_driver(rom, tmp);

				g_free(tmp);
			}
		}
	}
	else if(_this->current_rom)
	{
		rom = _this->current_rom;

		if(!strcmp(name, "rom"))
		{
			rom->nb_roms++;
		}
		else if(!strcmp(name, "chip"))
		{
			const char *type = XMLGetAttr(atts, "type", 0);

			if(!strcmp(type, "cpu"))
			{
				//_this->cpu_count++;

				if(_this->cpu_count <= NB_CPU) {
					CPUInfo *cpu;

					cpu = &rom->cpu_info[_this->cpu_count];
					cpu->sound_flag = FALSE;

					for (i =0; atts[i]; i += 2) {
						if (!strcmp(atts[i], "name")) {
							g_strlcpy(cpu->name, atts[i+1], MAX_CPU);
						} else if (!strcmp(atts[i], "clock")) {
							cpu->clock = atoi(atts[i+1]);
						} else if (!strcmp(atts[i], "soundonly")) {
							cpu->sound_flag = !strcmp(atts[i+1], "yes");
						}
					}
_this->cpu_count++;
				}
			}
			else if(!strcmp(type, "audio"))
			{
				//_this->sound_count++;

				if(_this->sound_count <= NB_CPU) {
					SoundCPUInfo *cpu;

					cpu = &rom->sound_info[_this->sound_count];

					for (i =0; atts[i]; i += 2) {
						if (!strcmp(atts[i], "name")) {
							g_strlcpy(cpu->name, atts[i+1], MAX_CPU);
						} else if (!strcmp(atts[i], "clock")) {
							cpu->clock = atoi(atts[i+1]);
						}
					}

				}
_this->sound_count++;
			}
		}
		else if(!strcmp(name, "input"))
		{

			for (i =0; atts[i]; i += 2) {
				if (!strcmp(atts[i], "players")) {
					rom->num_players = atoi(atts[i+1]);
				} else if (!strcmp(atts[i], "control")) {
					g_strlcpy(rom->control, atts[i+1], MAX_CONTROL);
				} else if (!strcmp(atts[i], "buttons")) {
					rom->num_buttons = atoi(atts[i+1]);	
				}
			}
		}
		else if(!strcmp(name, "control"))
		{
			/* Control is an element of input in later versions of the
			   XML output */
			for (i =0; atts[i]; i += 2) {
				if (!strcmp(atts[i], "type")) {
					g_strlcpy(rom->control, atts[i+1], MAX_CONTROL);
				}
			}
		}
		else if(!strcmp(name, "driver"))
		{
			
			for (i =0; atts[i]; i += 2) {

				if (!strcmp(atts[i], "status")) {
					rom->status = !strcmp(atts[i+1], "good");
				} else if (!strcmp(atts[i], "emulation")) {
					rom->driver_status_emulation = g_strdup (atts[i+1]);
				} else if (!strcmp(atts[i], "color")) {
					rom->driver_status_color = g_strdup (atts[i+1]);
				} else if (!strcmp(atts[i], "sound")) {
					rom->driver_status_sound = g_strdup (atts[i+1]);
				} else if (!strcmp(atts[i], "graphic")) {
					rom->driver_status_graphic = g_strdup (atts[i+1]);
				} else if (!strcmp(atts[i], "palettesize")) {
					rom->colors = atoi(atts[i+1]);
				}
				
			}
		}
		else if(!strcmp(name, "video"))
		{
			for (i =0; atts[i]; i += 2) {
				
				if (!strcmp(atts[i], "screen")) {
					rom->vector = !strcmp(atts[i+1], "vector");

				} else if (!strcmp(atts[i], "orientation")) {
					rom->horizontal = !strcmp(atts[i+1], "horizontal");

				} else if (!strcmp(atts[i], "refresh"))  {
					rom->screen_freq = g_ascii_strtod(atts[i+1], NULL);

				} else if  (!strcmp(atts[i], "width"))  {
					rom->screen_y = atoi(atts[i+1]);

				} else if  (!strcmp(atts[i], "height"))  {
					rom->screen_x = atoi(atts[i+1]);

				}
			}
		}
		else if (!strcmp(name, "display"))
		{
			/* New for SDLMame and MAME32. Values will be:
			<display type="raster" rotate="0" width="256" height="240" refresh="60.000000" />
			<display type="vector" rotate="0" refresh="60.000000" />
			<display type="vector" rotate="180" flipx="yes" refresh="38.000000" /> */
			for (i =0; atts[i]; i += 2) {
				if (!strcmp(atts[i], "type")) {
					rom->vector = !strcmp(atts[i+1], "vector");
				} else if (!strcmp(atts[i], "width")) {
					rom->screen_y = atoi(atts[i+1]);
				} else if (!strcmp(atts[i], "height")) {
					rom->screen_x = atoi(atts[i+1]);	
				} else if (!strcmp(atts[i], "refresh")) {
					rom->screen_freq = g_ascii_strtod(atts[i+1], NULL);	
				}
			}
		}
		else if(!strcmp(name, "sound"))
		{
			rom->channels = read_int_attribute(atts, "channels", 0);
		}
		else if(!strcmp(name, "sample"))
		{
			rom->nb_samples++;
		}

		else if(
			!strcmp(name, "year") ||
			!strcmp(name, "description") ||
			!strcmp(name, "manufacturer")
		) {

			XML_SetCharacterDataHandler(_this->xmlParser, 
				(XML_CharacterDataHandler ) &XMLDataHandler);

			_this->character_count = 0;
			memset(_this->text_buf, 0, BUFFER_SIZE); 
		}
	}

}
 
static void XMLEndHandler(TCreateGameList *_this,
			  const XML_Char *name)
{
	if(!strcmp(name, "game")) {
		CreateGameListGame(_this);
	} 
	else if (_this->text_buf[0] && name) {
		RomEntry *rom = _this->current_rom;

		if (rom) {

			if (!strcmp(name, "year")) {
				rom_entry_set_year(rom, _this->text_buf);
			} else if (!strcmp(name, "manufacturer")) {
				rom->manu = g_strdup(_this->text_buf);
			} else if (!strcmp(name, "description")) {
				rom_entry_set_name(rom, _this->text_buf);
			}
		}
	}
}

#define XML_BUFFER_SIZE (4096)

static gboolean CreateGameListRun(TCreateGameList *_this)
{
	int len;
	int final;
	int bufferPos = 0;

	XML_Parser xmlParser = _this->xmlParser;
	FILE *xmameHandle = _this->xmameHandle;

	_this->game_count = 0;
	
	for(;;)
    {
		char *buffer;

		buffer = XML_GetBuffer(xmlParser, XML_BUFFER_SIZE);

		if (!buffer) {
			GMAMEUI_DEBUG("Failed to allocate buffer.");
		}

		len = fread(buffer, 1, XML_BUFFER_SIZE, xmameHandle);
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
				XML_GetCurrentLineNumber(xmlParser),
				XML_GetCurrentColumnNumber(xmlParser),
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
 
static gboolean create_gamelist_xmlinfo(XmameExecutable *exec)
{
	gboolean res;
	TCreateGameList _this;
	
	memset(&_this, 0, sizeof(TCreateGameList));

	_this.progress_window = progress_window_new(FALSE);
	progress_window_set_title(_this.progress_window, _("Creating game list..."));
	progress_window_set_text(_this.progress_window, _("Creating game list, please wait"));

	progress_window_show(_this.progress_window);
	UPDATE_GUI;

	gamelist_free();

	game_list.name = g_strdup(exec->name);
	game_list.version = g_strdup(exec->version);

	_this.total_games = xmame_exec_get_game_count(exec);
	if (!_this.total_games) {
		progress_window_destroy(_this.progress_window);
		return FALSE;
	}
	
	_this.xmameHandle = xmame_open_pipe(exec, "-%s", xmame_get_option_name(exec,"listxml"));
	if (!_this.xmameHandle)
		return FALSE;

	_this.xmlParser = XML_ParserCreate(NULL);
	XML_SetElementHandler(_this.xmlParser, 
			(XML_StartElementHandler) &XMLStartHandler, 
			(XML_EndElementHandler)   &XMLEndHandler);
	XML_SetUserData(_this.xmlParser, &_this);

	progress_window_set_title(_this.progress_window,
		_("Creating game list (%d games)..."), _this.total_games);

	UPDATE_GUI;

	res = CreateGameListRun(&_this);

	pclose(_this.xmameHandle);

	XML_ParserFree(_this.xmlParser);

	progress_window_destroy(_this.progress_window);
	return res;
}

/**
* Listinfo parser
*/

/**
* Gets a hash table with all supported drivers
* Updates game count;
*/
static GHashTable *get_driver_table(XmameExecutable *exec, int *total_games) {
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
	xmame_pipe = xmame_open_pipe(exec, "-%s -%s /dev/null -%s /dev/null",
		xmame_get_option_name(exec, "listsourcefile"), 
		xmame_get_option_name(exec, "mameinfo_file"),
		xmame_get_option_name(exec, "history_file"));

	if (!xmame_pipe)
	{
		gmameui_message(ERROR, NULL, _("Error executing %s"), exec->path);
		return NULL;
	}

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

	xmame_close_pipe(exec, xmame_pipe);
	GMAMEUI_DEBUG("drivers list loaded");

	return driver_htable;
}

static gboolean create_gamelist_listinfo(XmameExecutable *exec)
{
	FILE *xmame_pipe;
	gchar line[BUFFER_SIZE];
	RomEntry *rom;
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
	int total_games = 0;

	GHashTable *driver_htable;
	ProgressWindow *progress_window;
	
	/* display the progression window */
	progress_window = progress_window_new(FALSE);
	progress_window_set_title(progress_window,_("Creating game list..."));
	progress_window_show(progress_window);

	progress_window_set_text(progress_window, _("receiving data, please wait"));

	while (gtk_events_pending()) gtk_main_iteration();

	driver_htable = get_driver_table(exec, &total_games);

	if (!driver_htable) {
		progress_window_destroy(progress_window);
		return FALSE;
	}

	gamelist_free();

	game_list.name = g_strdup(exec->name);
	game_list.version = g_strdup(exec->version);

	g_message(_("creating game list, Please wait:"));

	progress_window_set_title(progress_window, _("Creating game list (%d games)..."), total_games);
	while (gtk_events_pending()) gtk_main_iteration();
		
	/* Generate the list */
	/* without including neither history nor mameinfo to have less to parse after*/
	xmame_pipe = xmame_open_pipe(exec, "-%s -%s /dev/null -%s /dev/null",
		xmame_get_option_name(exec, "listinfo"),
		xmame_get_option_name(exec, "mameinfo_file"),
		xmame_get_option_name(exec, "history_file"));
	
	while (fgets(line, BUFFER_SIZE, xmame_pipe))
	{
		/*the game begin here*/
		if (!strncmp(line, "game (", 6)) {
			rom = rom_entry_new();
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
					g_strlcpy(rom->romname, value, MAX_ROMNAME);

				} else if (!strcmp(keyword, "description"))
				{
					rom_entry_set_name(rom, value);
				}
				else if (!strcmp(keyword, "year"))
					rom_entry_set_year(rom, value);
				else if (!strcmp(keyword, "manufacturer"))
				{
					if(strncmp(value, "???", 3))
						rom->manu = g_strdup(value);
				}
				else if (!strcmp(keyword, "rom"))
					rom->nb_roms++;
				else if (!strcmp(keyword, "sample"))
					rom->nb_samples++;
				else if (!strcmp(keyword, "cloneof"))
					rom->cloneof = g_strdup(value);
				else if (!strcmp(keyword, "romof"))
					rom->romof = g_strdup(value);
				else if (!strcmp(keyword, "sampleof"))
					rom->sampleof = g_strdup(value);
				else if (!strcmp(keyword, "chip")) {

					gboolean is_sound;
					tmp_counter = 0;
					
					tmp_array = g_strsplit(value, " ",0);
				

					is_sound = strcmp(tmp_array[1], "cpu");
					tmp_counter = 2;
					
					while (tmp_array[tmp_counter]) {

						if (!strcmp(tmp_array[tmp_counter], "name")) {
							if (is_sound)
								g_strlcpy(rom->sound_info[sound_count].name, tmp_array[tmp_counter+1], MAX_CPU);
							else
								g_strlcpy(rom->cpu_info[cpu_count].name, tmp_array[tmp_counter+1], MAX_CPU);

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
						}
						

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
							rom->vector = !strncmp(tmp_array[tmp_counter+1], "vector", 6);
							tmp_counter++;
						} else if (!strcmp(line_key, "orientation")) {
							rom->horizontal = !strncmp(tmp_array[tmp_counter+1], "h", 1);
							tmp_counter++;
						} else if (!strcmp(line_key, "x")) {
							rom->screen_x = atoi(tmp_array[tmp_counter+1]);
							tmp_counter++;
						} else if (!strcmp(line_key, "y")) {
							rom->screen_y = atoi(tmp_array[tmp_counter+1]);
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
	
						if (!strcmp
						    (tmp_array[tmp_counter], "players")) {

							rom->num_players = atoi(tmp_array[tmp_counter + 1]);
							
						}
						else if (!strcmp
						    (tmp_array[tmp_counter], "control")) {

							g_strlcpy(rom->control,tmp_array[tmp_counter + 1],  MAX_CONTROL);
							tmp_counter++;		


						}else if (!strcmp
						    (tmp_array[tmp_counter], "buttons")) {

							rom->num_buttons = atoi(tmp_array[tmp_counter + 1]);
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
							rom->status = !strcmp(tmp_array[tmp_counter+1], "good");
							tmp_counter++;
						}
						else if (!strcmp(tmp_array[tmp_counter], "palettesize")) {
							rom->colors = atoi(tmp_array[tmp_counter+1]);
							tmp_counter++;
						}

						tmp_counter++;
					}

					g_strfreev(tmp_array);

				} else if (!strcmp(keyword, "sound"))
				{
					tmp_array = g_strsplit(value, " ",3);
					if (!strcmp(tmp_array[0], "channels"))
						rom->channels = atoi(tmp_array[1]);
					g_strfreev(tmp_array);
				}

			}
			
			driver = g_hash_table_lookup(driver_htable, rom->romname);

			if (!driver)
				rom_entry_set_driver(rom, "Unknown");
			else
				rom_entry_set_driver(rom, driver);

			gamelist_add(rom);

			done = (gfloat) ((gfloat) (game_list.num_games) /
					 (gfloat) (total_games));

			progress_window_set_value(progress_window, done);
			UPDATE_GUI;
		}
	}
	
	pclose(xmame_pipe);
		
	g_hash_table_destroy(driver_htable);
	
                
	progress_window_destroy(progress_window);
	return TRUE;
}

gboolean gamelist_parse(XmameExecutable *exec)
{
	gboolean ret;

	if (!exec)
	{
		gmameui_message(ERROR, NULL, _("xmame not found"));
		return FALSE;
	}
	
	xmame_get_options(exec);

	/* Check if we will use listinfo or listxml */
	if (xmame_has_option(exec, "listinfo")) {
		GMAMEUI_DEBUG("Recreating gamelist using -listinfo\n");
		ret = create_gamelist_listinfo(exec);
	}
	else if (xmame_has_option(exec, "listxml")) {
		GMAMEUI_DEBUG("Recreating gamelist using -listxml\n");
		ret = create_gamelist_xmlinfo(exec);
	} else {
		gmameui_message(ERROR, NULL, _("I don't know how to generate a gamelist for this version of xmame!"));
		ret = FALSE;
	}
	
	return ret;
}
