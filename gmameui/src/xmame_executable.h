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

#ifndef __XMAME_EXECUTABLE_H__
#define __XMAME_EXECUTABLE_H__


#include <sys/types.h>  /* For pid_t */
#include "xmame_options.h"

typedef enum {
	XMAME_EXEC_UNKNOWN,
	XMAME_EXEC_X11,
#ifdef ENABLE_XMAME_SVGALIB_SUPPORT
	XMAME_EXEC_SVGALIB,
#endif
	XMAME_EXEC_GGI,
	XMAME_EXEC_XGL,
	XMAME_EXEC_XFX,
	XMAME_EXEC_SVGAFX,
	XMAME_EXEC_SDL,
	XMAME_EXEC_PHOTON2,
	XMAME_EXEC_WIN32
} ExecutableType;

/**
* A Mame executable.
*/
typedef struct {
	ExecutableType type;
	gchar *name;
	gchar *target;
	gchar *version;
	gchar *path;
	const gchar *noloadconfig_option;
	const gchar *showusage_option;
	/** All supported options for the executable. */
	GHashTable *options;
} XmameExecutable;

/** Initializes the xmame executables table */
void xmame_table_init (void);

/** Destroys the table and all executables inside 
*
*/
void              xmame_table_free (void);

gint              xmame_table_size (void);

/** Adds the executable with the given path to the table
* and returns the executable.
* Guarantees: 
* - only one executable in the table with the same path.
* - only valid executables are added.
* - version information is in the struct.
*/
XmameExecutable * xmame_table_add (const gchar *path);

/**
* Gets the executable with the given path 
*/
XmameExecutable * xmame_table_get (const gchar *path);

/**
* Gets the executable with the given index.
*/
XmameExecutable * xmame_table_get_by_index (int index);

/** Returns a NULL terminated list
* with all the paths in the table.
* Free the list with g_free.
*/
gchar **          xmame_table_get_all (void);

gboolean          xmame_executable_set_version (XmameExecutable *exec);


/**
* Compares the version of this executable with the given one.
*
* Returns:
*	 <0 this executable < version
*	  0 this executable == version
*	 >0 this executable > version
*/
int               xmame_compare_version (const XmameExecutable *exec, const gchar *version);

/**
* Compares the version of this executable with the given one, using the version numbers in
* the version string.
*
* Returns:
*	 <0 this executable < version
*	  0 this executable == version
*	 >0 this executable > version
*/
int               xmame_compare_raw_version (const XmameExecutable *exec, const gchar *version);

/* Use this macro to check for valid executables.
   This is to make it clear what we are doing.
*/
#define xmame_executable_is_valid(exec) xmame_executable_set_version(exec)

/** Gets the available_options for the executable
* It will only parse them the first time so
* you can call this often.
*/
const GHashTable * xmame_get_options (XmameExecutable *exec);

/** Checks if the executable has the given option */
gboolean xmame_has_option (XmameExecutable *exec, const gchar *option_name);

/** Returns the option with the given name */
const MameOption *xmame_get_option (XmameExecutable *exec, const gchar *option_name);

/** Returns the option string for this option.
* If the option does not exist or it does not take arguments it returns NULL.
*/
gchar *xmame_get_option_string (const XmameExecutable *exec, const gchar *option_name, const gchar *arguments);

/** Returns the option string for an option with int argument.
* If the option does not exist or the option does not take an int parameter it returns NULL.
*
* Example: xmame_get_int_option_string(exec, "modenumber", 2) will return:
" -modenumber 2
*/
gchar *xmame_get_int_option_string (const XmameExecutable *exec, const gchar *option_name, int argument);

/** Returns the option string for an option that does not take any arguments.
* If the option does not exist or it takes arguments it will return NULL.
*/
gchar *xmame_get_boolean_option_string (const XmameExecutable *exec, const gchar *option_name, gboolean is_enabled);

/* Returns the option string for an option that takes a float arguments.
* If the option does not exist or it does take float arguments it will return NULL.
*/
gchar *xmame_get_float_option_string (const XmameExecutable *exec, const gchar *option_name, float argument, char* buf);

/** Returns the mame name for this option.
* For example: "mapkey" will return "sdlmapkey" in xmame.SDL because they were declared aliases.
* See ADD_OPTION_ALIAS in option_hash_init() to see how this works.
*/
const gchar *xmame_get_option_name (const XmameExecutable *exec, const gchar* option_name);

/**
* Returns the type for the given option.
* Possible types include: NULL, "int", "string", "float", "arg"
*/
const gchar *xmame_get_option_type (const XmameExecutable *exec, const gchar *option_name);

/**
* Returns the description for the given option.
*/
const gchar *xmame_get_option_description (const XmameExecutable *exec, const gchar *option_name);

/**
* Returns a NULL terminated-array with possible values for this option.
*/
const gchar **xmame_get_option_values (const XmameExecutable *exec, const gchar *option_name);

const gchar **xmame_get_option_keys (const XmameExecutable *exec, const gchar *option_name);
int xmame_get_option_value_index (const XmameExecutable *exec, const gchar *option_name, const gchar *value);
const gchar *xmame_get_option_value (const XmameExecutable *exec, const gchar *option_name, int index);
const gchar *xmame_get_option_key (const XmameExecutable *exec, const gchar *option_name, const gchar *value);
const gchar *xmame_get_option_key_value (const XmameExecutable *exec, const gchar *option_name, const gchar *key);

/** Counts the number of possible values for this option. 
* On error it returns -1.
*/
int xmame_get_option_value_count (const XmameExecutable *exec, const gchar *option_name);

/**
* Runs the executable with the given options and returns an open pipe.
*/
FILE * xmame_open_pipe (const XmameExecutable *exec, const gchar *format, ...)
__attribute__((format(printf, 2, 3)))
;

void xmame_close_pipe (const XmameExecutable *exec, FILE *pipe);

void
mame_exec_launch_command (gchar *command, pid_t *pid, int *stdout, int *stderr);

GIOChannel *
mame_executable_set_up_io_channel (gint fd, GIOCondition cond, GIOFunc func, gpointer data);

#endif
