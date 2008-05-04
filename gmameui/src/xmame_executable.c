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

#include "xmame_executable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include "io.h"

const gchar *noloadconfig_option_name [] = {
	"noloadconfig",
	"noreadconfig"
};

const gchar *showusage_option_name [] = {
	"help",
	"showusage"
};

#define BUFFER_SIZE 1000
#define MAX_EXECUTABLES (10)

static XmameExecutable *xmame_table [MAX_EXECUTABLES + 1];

void
xmame_table_init (void)
{
	memset (xmame_table, 0, sizeof (XmameExecutable*) * MAX_EXECUTABLES);
}

#define foreach_exec(i, exec) for (i = 0; (exec = xmame_table[i]); i++)

gint
xmame_table_size (void)
{
	XmameExecutable *exec;
	int i;

	foreach_exec (i, exec);
		
	return i;
}

XmameExecutable *
xmame_table_get (const gchar *path)
{
	XmameExecutable *exec;
	int i;

	if (!path)
		return NULL;

	foreach_exec (i, exec) {
		if (!strcmp (exec->path, path)) {
			return exec;
		}
	}

	return NULL;
}

XmameExecutable *
xmame_table_get_by_index (int index)
{

	if (index < 0 || index >= MAX_EXECUTABLES)
		return NULL;

	return xmame_table[index];
}

gchar **
xmame_table_get_all (void)
{
	int size;
	int i;
	gchar **all_paths;
	XmameExecutable *ptr;

	size = xmame_table_size () + 1;

	all_paths = g_malloc0 (size * sizeof (gchar *));
	if (size == 1)
		return all_paths;

	foreach_exec (i, ptr) {
		all_paths[i] = ptr->path;
	}

	return all_paths;
}

/** Creates a new executable.
 * Path will be set to the absolute path for the executable.
 * Accepts absolute pathnames and program names in the PATH.
 *
 * If the executable is not valid it returns NULL.
*/
static XmameExecutable *
xmame_executable_new (const gchar *path)
{
	XmameExecutable *xmame_exec;

	if (!path)
		return NULL;

	xmame_exec = g_malloc0 (sizeof (XmameExecutable));
	xmame_exec->path = g_find_program_in_path (path);

	if (!xmame_exec->path)
	{
		gmameui_message (WARNING, NULL, _("%s is not a valid xmame executable"), path);
		g_free (xmame_exec);
		xmame_exec= NULL;
	}

	return xmame_exec;
}

static void
xmame_executable_free_options (XmameExecutable *exec)
{
	if (exec->options) {
		g_hash_table_destroy (exec->options);
		exec->options = NULL;
	}
}

static void
xmame_executable_free (XmameExecutable *exec)
{
	if (!exec)
		return;

	if (exec->name)
		g_free (exec->name);
	if (exec->version)
		g_free (exec->version);
	if (exec->path)
		g_free (exec->path);
	
	xmame_executable_free_options (exec);

	g_free (exec);
}

XmameExecutable *
xmame_table_add (const gchar *path)
{
	XmameExecutable * new_exec;
	XmameExecutable * exec;
	int i;

	if (!path)
		return NULL;

	new_exec = xmame_executable_new (path);

	if (!new_exec)
		return NULL;

	/* check if the executable is already in the list */
	foreach_exec (i,exec) {
		if (!strcmp (exec->path, new_exec->path)) {
			xmame_executable_free (new_exec);
			return exec;
		}
	}
	
	if (xmame_executable_set_version (new_exec))
	{
		int i;

		for (i = 0; i < MAX_EXECUTABLES; i++) {

			if (!xmame_table[i]) {
				xmame_table[i] = new_exec;
				return new_exec;
			}
		}
		
		xmame_executable_free (new_exec);
		gmameui_message (ERROR, NULL, _("This version of GMAMEUI supports up to %i xmame executables."), MAX_EXECUTABLES);
		return NULL;
	}
	else
	{
		xmame_executable_free (new_exec);
		return NULL;
	}
}

void
xmame_table_free (void)
{
	XmameExecutable *exec;
	int i;

	foreach_exec (i, exec) {
		xmame_executable_free (exec);
	}

	memset (xmame_table, 0, sizeof (XmameExecutable*) * MAX_EXECUTABLES);
}

#ifdef ENABLE_XMAME_SVGALIB_SUPPORT
/* version detection for xmame.svgalib */
static gboolean
xmamesvga_executable_set_version (XmameExecutable *exec)
{
	FILE *xmame_pipe;
	FILE *tmp;
	gchar *opt, *p;
	gchar *tmp_file;
	gchar line[BUFFER_SIZE];
	gchar *tmp_xmame_target;
	gchar *tmp_xmame_version;
	gboolean xmame_ok;

	/* FIXME: Generate true tmp name */
	tmp_file = g_strdup ("/tmp/gmameui.12344");
	
	GMAMEUI_DEBUG ("Executing %s with tmp_file \"%s\"", exec->path, tmp_file);
	opt = g_strdup_printf ("%s -version -noloadconfig -out %s 2>/dev/null", exec->path, tmp_file);
	xmame_pipe = popen (opt, "r");
	g_free (opt);
	if (!xmame_pipe)
		goto exit_with_error;

	while (fgets (line, BUFFER_SIZE, xmame_pipe));
	pclose (xmame_pipe);
	
	xmame_ok = FALSE;
	tmp = fopen (tmp_file, "r");
	if (tmp) {
		while (fgets (line, BUFFER_SIZE, tmp)) {
			/* that should catch any info displayed before the version name */
			if (!strncmp (line, "info", 4) )
				continue;
	
			/* try to find the '(' */
			for (p = line; (*p && (*p != '(')); p++);
			if (!*p)
				continue;
			* (p - 1) = '\0';
			p++;
			tmp_xmame_target = p;

			/* now try to find the ')' */
			for (; (*p && (*p != ')')); p++);
			if (!*p)
				continue;
			*p = '\0';
			p += 2;
			tmp_xmame_version = p;
		
			/* do I need that ? */
			for (; (*p && (*p != '\n')); p++);
			*p = '\0';
			if (strncmp (tmp_xmame_version, "version", 7))
				continue;
			
			GMAMEUI_DEBUG ("checking xmame version: %s (%s) %s", line, tmp_xmame_target, tmp_xmame_version);
			
			exec->name = g_strdup (line);
			exec->target = g_strdup (tmp_xmame_target);
			exec->version = g_strdup (tmp_xmame_version);
			
			if (!strcmp (exec->target, "x11"))
				exec->type = XMAME_EXEC_X11;
			else if (!strcmp (exec->target, "svgalib"))
				exec->type = XMAME_EXEC_SVGALIB;
			else if (!strcmp (exec->target, "ggi"))
				exec->type = XMAME_EXEC_GGI;
			else if (!strcmp (exec->target, "xgl"))
				exec->type = XMAME_EXEC_XGL;
			else if (!strcmp (exec->target, "xfx"))
				exec->type = XMAME_EXEC_XFX;
			else if (!strcmp (exec->target, "svgafx"))
				exec->type = XMAME_EXEC_SVGAFX;
			else if (!strcmp (exec->target, "SDL"))
				exec->type = XMAME_EXEC_SDL;
			else if (!strcmp (exec->target, "photon2"))
				exec->type = XMAME_EXEC_PHOTON2;
			else
				exec->type = XMAME_EXEC_UNKNOWN;

			xmame_ok = TRUE;
		}
		fclose (tmp);
		unlink (tmp_file);
		g_free (tmp_file);
	}

	return xmame_ok;
exit_with_error:
	GMAMEUI_DEBUG ("Could not execute.");
	unlink (tmp_file);
	g_free (tmp_file);
	return FALSE;
}
#endif

/*#ifdef ENABLE_WINMAME_SUPPORT*/
/**
* version detection for win32 mame or SDLMame.
*
*/
static gboolean
winmame_executable_set_version (XmameExecutable *exec)
{
	FILE *xmame_pipe;
	gchar *opt, *p;
	gchar line[BUFFER_SIZE];
	gchar *tmp_xmame_version;
	gboolean xmame_ok = FALSE;

	exec->noloadconfig_option = noloadconfig_option_name[1];
	exec->showusage_option = showusage_option_name[1];

	opt = g_strdup_printf ("%s -help -noreadconfig 2>/dev/null", exec->path);
	GMAMEUI_DEBUG ("Trying %s", opt);
	xmame_pipe = popen (opt, "r");
	g_free (opt);
	if (!xmame_pipe)
	{
		GMAMEUI_DEBUG ("Could not execute.");
		return FALSE;
	}

	while (fgets (line, BUFFER_SIZE, xmame_pipe) && !xmame_ok)
	{
		GMAMEUI_DEBUG ("Reading line %s", line);

		/* that should catch any info displayed before the version name */
		if (!strncmp (line, "info", 4) )
			continue;

		/* try to find the second word v.??? (???) */
		for (p = line; (*p && (*p != ' '));p++);
		if (!*p)
			continue;
		* (p-1) = '\0';
		p++;
		tmp_xmame_version = p;	

		/* Stop at ')' */
		for (; (*p && (*p != ')'));p++);
		if (!*p)
			continue;

		if (* (p+1))
			* (p+1) = '\0';
						
		GMAMEUI_DEBUG ("checking xmame version: %s (Win32) %s", line, tmp_xmame_version);
			
		exec->name = g_strdup (line);
		exec->target = g_strdup ("Win32");
		exec->version = g_strdup (tmp_xmame_version);

		xmame_ok = TRUE;
	}
		
	pclose (xmame_pipe);
		
	if (xmame_ok)
		exec->type = XMAME_EXEC_WIN32;

	return xmame_ok;
}
/*#endif*/

/* Sets the version and type information for the executable.
* This is called automatically for every executable
* added to the table so there is no need to call it.
*
* Note: This is called before parsing the options
* so we cannot use
*/
/* FIXME: Possible memory leak... */
gboolean
xmame_executable_set_version (XmameExecutable *exec)
{
	FILE *xmame_pipe;
	gchar *opt, *p;
	gchar line[BUFFER_SIZE];
	gchar *tmp_xmame_target, *tmp_xmame_version;
	gdouble version;
	gchar *version_str;
	gboolean xmame_ok = FALSE;

	if (!exec || !exec->path)
		return FALSE;

	if (g_file_test (exec->path, G_FILE_TEST_IS_EXECUTABLE) == FALSE)
		return FALSE;

	if (exec->version)
	{
		g_free (exec->version);
		exec->version = NULL;
	}
	if (exec->target)
	{
		g_free (exec->target);
		exec->target = NULL;
	}
	if (exec->name)
	{
		g_free (exec->name);
		exec->name = NULL;
	}

	exec->noloadconfig_option = noloadconfig_option_name[0];
	exec->showusage_option = showusage_option_name[0];
	exec->type = XMAME_EXEC_UNKNOWN;

	opt = g_strdup_printf ("%s -version 2>&1", exec->path);	/* Note use 2>&1 rather than 2>/dev/null so if call fails the error is captured */
	GMAMEUI_DEBUG ("Trying %s", opt);
	xmame_pipe = popen (opt, "r");
	g_free (opt);
	if (!xmame_pipe)
	{
		GMAMEUI_DEBUG ("Could not execute.");
		return FALSE;
	}
	while (fgets (line, BUFFER_SIZE, xmame_pipe) && !xmame_ok)
	{
		GMAMEUI_DEBUG ("Reading line %s", line);

#ifdef ENABLE_XMAME_SVGALIB_SUPPORT
		/* detect svgalib target */
		if (!strncmp (line, "[svgalib:", 9))
		{
			pclose (xmame_pipe);
			return xmamesvga_executable_set_version (exec);
		}
#endif

/*#ifdef ENABLE_WINMAME_SUPPORT*/
		/* Probably win32 mame */
		if (!strncmp (line, "Error: unknown option: -version", 31)) {
			pclose (xmame_pipe);
			return winmame_executable_set_version (exec);
		}
/*#endif*/
		/* that should catch any info displayed before the version name */
		if (!strncmp (line, "info", 4) )
			continue;
		/* try to find the '(' */
		for (p = line; (*p && (*p != '(')); p++);
		if (!*p)
			continue;
		* (p-1) = '\0';
		p++;
		tmp_xmame_target = p;

		/* now try to find the ')' */
		for (; (*p && (*p != ')')); p++);
		if (!*p)
			continue;
		*p = '\0';
		p += 2;
		tmp_xmame_version = p;
		
		/* do I need that ? */
		for (; (*p && (*p != '\n'));p++);
		*p = '\0';
		if (strncmp (tmp_xmame_version, "version", 7))
			continue;
			
		GMAMEUI_DEBUG ("checking xmame version: %s (%s) %s", line, tmp_xmame_target, tmp_xmame_version);
			
		exec->name = g_strdup (line);
		exec->target = g_strdup (tmp_xmame_target);
		exec->version = g_strdup (tmp_xmame_version);

		xmame_ok = TRUE;
	}
		
	pclose (xmame_pipe);
		
	if (xmame_ok)
	{
		GMAMEUI_DEBUG ("name=%s. target=%s. version=%s.",
			      exec->name,
			      exec->target,
			      exec->version);

		if (!strcmp (exec->target, "x11"))
			exec->type = XMAME_EXEC_X11;
#ifdef ENABLE_XMAME_SVGALIB_SUPPORT
		else if (!strcmp (exec->target, "svgalib"))
			exec->type = XMAME_EXEC_SVGALIB;
#endif
		else if (!strcmp (exec->target, "ggi"))
			exec->type = XMAME_EXEC_GGI;
		else if (!strcmp (exec->target, "xgl"))
			exec->type = XMAME_EXEC_XGL;
		else if (!strcmp (exec->target, "xfx"))
			exec->type = XMAME_EXEC_XFX;
		else if (!strcmp (exec->target, "svgafx"))
			exec->type = XMAME_EXEC_SVGAFX;
		else if (!strcmp (exec->target, "SDL"))
			exec->type = XMAME_EXEC_SDL;
		else if (!strcmp (exec->target, "photon2"))
			exec->type = XMAME_EXEC_PHOTON2;
		else
			exec->type = XMAME_EXEC_UNKNOWN;
	}

	/* first get the version number to compare it to check SDL modes */
	if (exec->version) {
		version_str = exec->version;

		while (*version_str && !isdigit (*version_str))
			version_str++;

		version = g_ascii_strtod (version_str, NULL);

		if (version == 0.68)
			exec->showusage_option = showusage_option_name[1];
	} else {
		exec->version = g_strdup ("Unknown");
	}
	return xmame_ok;	
}

int
xmame_compare_version (const XmameExecutable *exec,
		       const gchar           *version)
{
	return strcmp (exec->version, version);
}

/* This function compares the raw version (i.e. the 0.xxx number)
   excluding any leading letters or trailing data. For example, a
   version number 'v0.121u2 (Dec  6 2007)' would be transformed
   to 0.121u2. This is compared against the string-format version
   number supplied */
int
xmame_compare_raw_version (const XmameExecutable *exec,
		       const gchar           *version)
{
	if (!exec)
		return FALSE;
	
	gchar *rawversion = g_strdup (exec->version);

	rawversion = strstr (rawversion, "0");
	gchar *p = strstr (rawversion, " ");
	*p = '\0';

/*	if (!strcmp (rawversion, "0.110"))
		GMAMEUI_DEBUG ("Version is same as 0.110");
	else if (strcmp (rawversion, "0.110") > 0)
		GMAMEUI_DEBUG ("Version is greater than 0.110");
	else GMAMEUI_DEBUG ("Version is lower than 0.110");*/
	return strcmp (rawversion, version);
}

static void
mame_opt_free (gpointer opt)
{
	MameOption *opt_opt =  opt;
	xmame_option_free (opt_opt);
}

static MameOption *
parse_option (gchar *line,
	      FILE  *xmame_pipe,
	      int   *more_input)
{
	MameOption *opt;
	gchar *p;
	gchar *start_p;
	gboolean noopt;
	gboolean list_mode;
	gboolean can_have_list;
	const gchar *gmameui_name;
	gboolean option_is_alternative = FALSE;
	GSList *values = NULL;
	
	if (line[0] != '-')
	{
		*more_input = (fgets (line, BUFFER_SIZE, xmame_pipe) != NULL);
		return NULL;
	}
	start_p = line + 1;

	/* check for [no] */
	noopt = !strncmp ("[no]", start_p, 4);

	/* skip [no] */
	if (noopt)
		start_p += 4;
	
	for (p = start_p; *p && *p != ' '; p++);

	*p = '\0';
	
	/* Lookup in mapping hash table and check if we know the option */
	gmameui_name = xmame_option_get_gmameui_name (start_p);

	if (!gmameui_name) {
		*more_input = (fgets (line, BUFFER_SIZE, xmame_pipe) != NULL);
		return NULL;
	}

	opt = (MameOption*) malloc (sizeof (MameOption));
	opt->no_option = noopt;
	opt->name = g_strdup (start_p);
	opt->type = NULL;
	opt->description = NULL;
	opt->possible_values = NULL;
	opt->keys = NULL;

	/* Skip spaces */
	start_p = p + 1;
	while (*start_p == ' ')
		start_p++;

	/* alternative option name */
	if (*start_p == '/') {
		start_p++;

		/* Skip spaces */
		while (*start_p == ' ')
			start_p++;

		/* alternative option is on the next line */
		if (*start_p != '-') {
			option_is_alternative = TRUE;
		}

		/* Skip alternative option name */
		while (*start_p != ' ' && *start_p != '\0')
			start_p++;


		/* Skip spaces */
		while (*start_p == ' ')
			start_p++;
	}

	/* Option type */
	if (*start_p == '<') {
		start_p++;

		for (p = start_p; *p && *p != '>'; p++);

		if (*p == '\0')
			return opt;

		*p = '\0';

		opt->type = g_strdup (start_p);

		start_p = p + 1;

		/* Skip spaces */
		while (*start_p == ' ')
			start_p++;
	}

	/* Strip newline */
	for (p = start_p; *p && *p != '\n'; p++);
	*p = '\0';

	/* Strip space at the end */
	p--;
	while (*p == ' ') {
		*p = '\0';
		p--;
	}

	opt->description = g_strdup (start_p);
	list_mode = FALSE;
	can_have_list = (opt->type) && !strcmp (opt->type, "int");

	while ( (*more_input = (fgets (line, BUFFER_SIZE, xmame_pipe) != NULL))) {
		start_p = line;

		if (option_is_alternative) {
			option_is_alternative = FALSE;
			if (*start_p == '-') {
				while (*start_p != ' ' && *start_p != '\0')
					start_p++;
			}
		}

		if (*start_p == '-' || *start_p == '*' || *start_p == '\0' || *start_p == '\n' || *start_p == '\r')
			break;

		/* Option type */
		if (*start_p == '<') {
			start_p++;

			for (p = start_p; *p && *p != '>'; p++);

			if (*p == '\0')
				return opt;

			*p = '\0';

			opt->type = g_strdup (start_p);

			start_p = p + 1;

			/* Skip spaces */
			while (*start_p == ' ')
				start_p++;
		} 
		/* Skip spaces */
		while (*start_p == ' ' || *start_p == '\t')
			start_p++;
	
		/* Strip newline */
		for (p = start_p; *p && *p != '\n'; p++);
		*p = '\0';

		/* Strip space at the end */
		p--;
		while (*p == ' ') {
			*p = '\0';
			p--;
		}

		/* If the line starts with the number then it's a list item */
		if (g_ascii_isdigit (start_p[0]) && can_have_list) {
			gchar *orig_start_p;
			int opening_brackets = 0;
			int closing_brackets = 0;

			orig_start_p = start_p;
			/* skip the number */
			while (g_ascii_isdigit (*start_p))
				start_p++;
	
			/* skip spaces and = */
			while (*start_p == ' ' || *start_p == '=')
				start_p++;		

			/* if we havent started list mode try harder to see if 
			it's a number that is part of the description.
			*/
			if (!list_mode) {

				/* count opening and closing brackets */
				for (p = start_p; *p; p++) {
					if (*p == '(')
						opening_brackets++;
					else if (*p == ')')
						closing_brackets++;
				}

				/* if we have more closing than opening brackets
				this means that we are continuing from a previous line.
				*/
				if (closing_brackets > opening_brackets) {
					gchar *new_desc;

					new_desc = g_strdup_printf ("%s %s", opt->description, orig_start_p);
					g_free (opt->description);
					opt->description = new_desc;
				} else
					list_mode = TRUE;
			}

			if (list_mode)
				values = g_slist_append (values, g_strdup (start_p));

		} else {
			gchar *new_desc;

			/* if we are on list mode then append this to the last item. */
			if (list_mode && values) {
				GSList *last_item = g_slist_last (values);

				new_desc = g_strdup_printf ("%s %s", (gchar*)last_item->data, start_p);
				g_free (last_item->data);
				last_item->data = new_desc;
			} else { /* otherwise append it to the description */
				new_desc = g_strdup_printf ("%s %s", opt->description, start_p);
				g_free (opt->description);
				opt->description = new_desc;
			}
		}
	}

	if (values) {
		guint i;
		guint list_size = g_slist_length (values);
		GSList *tmp_list = values;

		opt->possible_values = (gchar**) malloc (sizeof (gchar*) * (list_size + 1));
		for (i =0; i < list_size; i++) {
			opt->possible_values[i] = tmp_list->data;
			tmp_list = g_slist_next (tmp_list);
		}
		opt->possible_values[list_size] = NULL;

		g_slist_free (values);
	}

	return opt;
}

gboolean
xmame_has_option (const XmameExecutable *exec,
		  const gchar           *option_name)
{
	return xmame_get_option (exec, option_name) != NULL;
}

const MameOption *
xmame_get_option (const XmameExecutable *exec,
		  const gchar           *option_name)
{
	if (!exec->options)
		return NULL;

#ifdef ENABLE_DEBUG
	if (!xmame_option_get_gmameui_name (option_name))
		GMAMEUI_DEBUG ("Invalid option: %s", option_name);
#endif
	return g_hash_table_lookup (exec->options, option_name);

}

const gchar *
xmame_get_option_name (const XmameExecutable *exec,
		       const gchar           *option_name)
{
	const MameOption *opt = g_hash_table_lookup (exec->options, option_name);
	if (!opt)
		return NULL;

	return opt->name;
}

const gchar *
xmame_get_option_type (const XmameExecutable *exec,
		       const gchar           *option_name)
{
	const MameOption *opt = g_hash_table_lookup (exec->options, option_name);
	if (!opt)
		return NULL;

	return opt->type;
}

const gchar *
xmame_get_option_description (const XmameExecutable *exec,
			      const gchar           *option_name)
{
	const MameOption *opt = g_hash_table_lookup (exec->options, option_name);
	if (!opt)
		return NULL;

	return opt->description;
}

const gchar **
xmame_get_option_keys (const XmameExecutable *exec,
		       const gchar           *option_name)
{
	const MameOption *opt = g_hash_table_lookup (exec->options, option_name);
	if (!opt)
		return NULL;

	return (const gchar **)opt->keys;
}

const gchar *
xmame_get_option_key (const XmameExecutable *exec,
		      const gchar           *option_name,
		      const gchar           *value)
{
	gchar **values;
	gchar **keys;
	int count;
	int key_count;
	const MameOption *opt = g_hash_table_lookup (exec->options, option_name);
	if (!opt || !value)
		return NULL;

	keys = opt->keys;
	values = opt->possible_values;
	if (!values || ! keys)
		return NULL;

	for (count = 0; values[count]; count++) {
		if (!strcmp (values[count], value))
			break;
	}

	if (values[count]) {
		for (key_count = 0; keys[key_count] && key_count < count; key_count++);
		
		return keys[key_count];
	}

	return NULL;
}

const gchar *
xmame_get_option_key_value (const XmameExecutable *exec,
			    const gchar           *option_name,
			    const gchar           *key)
{
	gchar **keys;
	gchar **values;
	int count;
	int key_count;
	const MameOption *opt = g_hash_table_lookup (exec->options, option_name);
	if (!opt || !key)
		return NULL;

	keys = opt->keys;
	values = opt->possible_values;

	if (!values || !keys)
		return NULL;

	for (count = 0; keys[count]; count++) {
		if (!strcmp (keys[count], key))
			break;
	}
	if (keys[count]) {
		for (key_count = 0; values[key_count] && key_count < count; key_count++);
		
		return values[key_count];
	}

	return NULL;
}

const gchar **
xmame_get_option_values (const XmameExecutable *exec,
			 const gchar           *option_name)
{
	const MameOption *opt = g_hash_table_lookup (exec->options, option_name);
	if (!opt)
		return NULL;

	return (const gchar **)opt->possible_values;
}

const gchar *
xmame_get_option_value (const XmameExecutable *exec,
			const gchar           *option_name,
			int                    index)
{
	gchar **values;
	int count;
	const MameOption *opt = g_hash_table_lookup (exec->options, option_name);
	if (!opt)
		return NULL;

	values = opt->possible_values;
	if (!values)
		return NULL;

	for (count = 0; values[count] && count < index; count++);

	return values[count];
}

int
xmame_get_option_value_index (const XmameExecutable *exec,
			      const gchar           *option_name,
			      const gchar           *value)
{
	gchar **values;
	int count;
	const MameOption *opt = g_hash_table_lookup (exec->options, option_name);
	if (!opt)
		return -1;

	values = opt->possible_values;
	if (!values)
		return -1;

	for (count = 0; values[count]; count++) {
		if (!strcmp (values[count], value))
			return count;
	}

	return -1;
}

int
xmame_get_option_value_count (const XmameExecutable *exec,
			      const gchar           *option_name)
{
	gchar **values;
	int count;
	const MameOption *opt = g_hash_table_lookup (exec->options, option_name);
	if (!opt)
		return -1;

	values = opt->possible_values;
	if (!values)
		return -1;

	for (count = 0; values[count]; count++);

	return count;
}

gchar *
xmame_get_option_string (const XmameExecutable *exec,
			 const gchar           *option_name,
			 const gchar           *arguments)
{
	MameOption *opt;
	gchar *stripped_args;
	gchar *escaped_args;
	gchar *opt_string;

	g_return_if_fail (exec->options != NULL);

	opt = g_hash_table_lookup (exec->options, option_name);
	if (!opt) {
		GMAMEUI_DEBUG ("xmame_get_option_string: Option %s not found", option_name);
		return NULL;
	}

	/* FIXME Newer versions (i.e. SDLMame no longer use <int> to designate option types.
	   Temporarily commenting this out */
	if (!arguments/* || !opt->type*/) {
		GMAMEUI_DEBUG ("xmame_get_option_string: No arguments supplied, or no opt type for option %s", option_name);
		opt_string  = NULL;
	} else {
		stripped_args = g_strdup (arguments);
		stripped_args = g_strstrip (stripped_args);	/* Strip whitespace */
		if (strlen (stripped_args) == 0) {
			g_free (stripped_args);
			return NULL;
		}
		escaped_args = g_shell_quote (stripped_args);
		opt_string =  g_strdup_printf ("-%s %s", opt->name, escaped_args);
		g_free (stripped_args);
		g_free (escaped_args);
	}

	return opt_string;
}

gchar *
xmame_get_float_option_string (const XmameExecutable *exec,
			       const gchar           *option_name,
			       float                  argument,
			       char                  *float_buf)
{
	MameOption *opt;
	gchar *opt_string;
	const gchar *opt_value_string;

	if (!exec->options)
		return NULL;

	opt = g_hash_table_lookup (exec->options, option_name);
	if (!opt || !opt->type)
		return NULL;
	
	if (strcmp (opt->type, "float") && strcmp (opt->type, "arg"))
		return NULL;
	
	opt_value_string = my_dtostr (float_buf, argument);
	opt_string = xmame_get_option_string (exec, option_name, opt_value_string);

	return opt_string;
}

gchar *
xmame_get_int_option_string (const XmameExecutable *exec,
			     const gchar           *option_name,
			     int                    argument)
{
	MameOption *opt;
	gchar *opt_string;
	gchar *opt_value_string;


	if (!exec->options)
		return NULL;

	opt = g_hash_table_lookup (exec->options, option_name);
	if (!opt)
		return NULL;
	
	if (!opt->type || strcmp (opt->type, "int"))
		return NULL;
	
	opt_value_string = g_strdup_printf (" %i", argument);
	
	opt_string = xmame_get_option_string (exec, option_name, opt_value_string);
	g_free (opt_value_string);

	return opt_string;
}

gchar *
xmame_get_boolean_option_string (const XmameExecutable *exec,
				 const gchar           *option_name,
				 gboolean               is_enabled)
{
	MameOption *opt;
	gchar *opt_string;

	if (!exec->options)
		return NULL;

	opt = g_hash_table_lookup (exec->options, option_name);
	if (!opt)
		return NULL;

	/* boolean options have no type */
	if (opt->type)
		return NULL;

	if (opt->no_option)
		opt_string = g_strdup_printf ("-%s%s", (is_enabled) ? "" : "no", opt->name);
	else if (is_enabled)
		opt_string = g_strdup_printf ("-%s", opt->name);
	else
		opt_string = NULL;

	return opt_string;
}

FILE *
xmame_open_pipe (const XmameExecutable *exec,
		 const gchar           *format,
		 ...)
{
	va_list args;
	gchar *my_args;
	gchar *opt;
	FILE *xmame_pipe;
	
	va_start (args, format);
	my_args = g_strdup_vprintf (format, args);
	va_end (args);

	opt = g_strdup_printf ("%s -%s %s 2>/dev/null", exec->path, exec->noloadconfig_option, my_args);
	GMAMEUI_DEBUG ("Running: %s", opt);
	xmame_pipe = popen (opt, "r");

	g_free (opt);
	g_free (my_args);

	return xmame_pipe;
}

void
xmame_close_pipe (const XmameExecutable *exec,
		  FILE                  *pipe)
{
	pclose (pipe);
}

static gchar **
slist_to_array (GSList *list)
{
	guint i;
	GSList *tmp_list;
	gchar **array;
	guint list_size;

	if (!list)
		return NULL;
			
	list_size = g_slist_length (list);
	tmp_list = list;

	array = (gchar**) malloc (sizeof (gchar*) * (list_size + 1));
	for (i =0; i < list_size; i++) {
		array[i] = tmp_list->data;
		tmp_list = g_slist_next (tmp_list);
	}
	array[list_size] = NULL;
	g_slist_free (list);

	return array;
}

static void
parse_list_option (XmameExecutable *exec,
		   const gchar     *option_name,
		   const gchar     *list_option)
{
	MameOption *option;
	gchar *key, *value;
	gchar *keyword, *name, *p;
	gchar line[BUFFER_SIZE];
	GSList *value_key = NULL;
	GSList *value_value = NULL;
	FILE *xmame_pipe;
	int i;

	if (!xmame_has_option (exec, list_option) || !xmame_has_option (exec, option_name))
		return;

	option = (MameOption*)xmame_get_option (exec, option_name);
	xmame_pipe = xmame_open_pipe (exec, "-%s", xmame_get_option_name (exec, list_option));
	if (!xmame_pipe)
		return;
	
	/* header : Digital sound plugins: */
	fgets (line, BUFFER_SIZE, xmame_pipe);
	/* first empty line */
	fgets (line, BUFFER_SIZE, xmame_pipe);
	
	while (fgets (line, BUFFER_SIZE, xmame_pipe))
	{
		/* prevent to get other things as plugins (Open GL copyright line) */
		if (line[0] == '\n')
			break;
		else
		{
			/* remove traling \n */
			line[strlen (line) - 1] = 0;
			/* find the end of the keyword */
			for (i = 1, keyword = p = line; (*p && (*p++ != ' ')); i++);
			keyword[i] = '\0';
			/* find the begining of the plugin complete name */
			for (i = 0, name = ++p; (*p && (*p++ == ' ')); i++);
			g_strstrip (name);
			GMAMEUI_DEBUG ("plugin found: %s, code (%s)", name, keyword);
			key = g_strndup (keyword, strlen (keyword)-1);
			value = g_strdup (name);
			value_key = g_slist_append (value_key, key);
			value_value = g_slist_append (value_value, value);
		}
	}
	xmame_close_pipe (exec, xmame_pipe);

	if (value_key) {
		if (option->keys)
			g_strfreev (option->keys);
		if (option->possible_values)
			g_strfreev (option->possible_values);

		option->keys = slist_to_array (value_key);
		option->possible_values = slist_to_array (value_value);
	}
}

static void
parse_listmodes_option (XmameExecutable *exec,
			const gchar     *option_name,
			const gchar     *list_option)
{
	MameOption *option;
	gchar *p, *name, *key, *value, *keyword;
	GSList *value_key = NULL;
	GSList *value_value = NULL;
	gchar line[BUFFER_SIZE];
	FILE *xmame_pipe;
	int i;
	
	option = (MameOption*)xmame_get_option (exec, option_name);
	
	if (!option || !xmame_has_option (exec, list_option))
		return;
		
	xmame_pipe = xmame_open_pipe (exec, "-%s", xmame_get_option_name (exec, list_option));
	if (!xmame_pipe)
		return;

	GMAMEUI_DEBUG ("getting xmame SDL mode");
	while (fgets (line, BUFFER_SIZE, xmame_pipe))
	{
		if (!strncmp (line, "Modes available:", 16))
		{
				GMAMEUI_DEBUG ("begin mode listing");
				while (fgets (line, BUFFER_SIZE, xmame_pipe))
				{
					if (line[0] == '\n')
						break;
					else
					{
						/* remove traling \n */
						line[strlen (line) - 1] = 0;
						/* find the end of the keyword */
						for (i = 1, keyword = p = line; (*p && (*p++ != ')')); i++);
						keyword[i] = '\0';
						g_strstrip (keyword);
						/* find the begining of the plugin complete name */
						name = p+6;
						g_strstrip (name);
						GMAMEUI_DEBUG ("Resolution found: %s, code (%s)", name, keyword);
						key = g_strndup (keyword, strlen (keyword) - 1);
						value = g_strdup (name);
						
						value_key = g_slist_append (value_key, key);
						value_value = g_slist_append (value_value, value);
					}
				}
		}
	}
	xmame_close_pipe (exec, xmame_pipe);
	
	if (value_key) {
		if (option->keys)
			g_strfreev (option->keys);
		if (option->possible_values)
			g_strfreev (option->possible_values);

		option->keys = slist_to_array (value_key);
		option->possible_values = slist_to_array (value_value);
	}
}

const GHashTable *
xmame_get_options (XmameExecutable *exec)
{
	FILE *xmame_pipe;
	gchar line[BUFFER_SIZE];
	GHashTable *option_hash;

	if (!exec)
		return NULL;

	if (exec->options)
		return exec->options;

	GMAMEUI_DEBUG ("Getting options using parameter %s\n", exec->showusage_option);
	
	xmame_pipe = xmame_open_pipe (exec, "-%s", exec->showusage_option);
	if (!xmame_pipe)
		return NULL;

	GMAMEUI_DEBUG ("checking xmame options");
	if (fgets (line, BUFFER_SIZE, xmame_pipe))
	{
		int more_input = 0;
		option_hash = g_hash_table_new_full (g_str_hash, g_str_equal, NULL, mame_opt_free);
		
		do {
			MameOption *my_opt;
			const gchar *gmameui_name;
			my_opt = parse_option (line, xmame_pipe, &more_input);

			if (my_opt) {
				gmameui_name = xmame_option_get_gmameui_name (my_opt->name);

#ifdef OPTIONS_DEBUG
				GMAMEUI_DEBUG ("Found option: %s %s%s (%s)",
					my_opt->type ? my_opt->type : "",
					my_opt->no_option ? "[no]" : "",
					my_opt->name,
					gmameui_name);
				GMAMEUI_DEBUG ("Description: \"%s\"", my_opt->description);
#  ifdef ENABLE_DEBUG
				if (my_opt->possible_values) {
					int i;
					GMAMEUI_DEBUG ("Possible values: ");
					for (i = 0; my_opt->possible_values[i]; i++) {
						GMAMEUI_DEBUG ("%i %s", i, my_opt->possible_values[i]);
					}
				}
#  endif
#endif
				if (gmameui_name)
					g_hash_table_insert (option_hash, (gpointer)gmameui_name, my_opt);
			}
		} while (more_input);

		if (exec->options)
			g_hash_table_destroy (exec->options);

		exec->options = option_hash;
	}

	xmame_close_pipe (exec, xmame_pipe);

	parse_list_option (exec, "dsp-plugin", "list-dsp-plugins");
	parse_list_option (exec, "sound-mixer-plugin", "list-mixer-plugins");
	parse_listmodes_option (exec, "modenumber", "listmodes");
	
	return exec->options;
}
