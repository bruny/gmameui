/*
 * GMAMEUI
 *
 * Copyright 2008 Andrew Burton <adb@iinet.net.au>
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

#ifndef __MAME_EXEC_H__
#define __MAME_EXEC_H__

#include <sys/types.h>  /* For pid_t */
#include "xmame_options.h"
#include "common.h"

G_BEGIN_DECLS

/* Preferences object */
#define MAME_TYPE_EXEC            (mame_exec_get_type ())
#define MAME_EXEC(o)            (G_TYPE_CHECK_INSTANCE_CAST((o), MAME_TYPE_EXEC, MameExec))
#define MAME_EXEC_CLASS(k)    (G_TYPE_CHECK_CLASS_CAST ((k), MAME_TYPE_EXEC, MameExecClass))
#define MAME_IS_EXEC(o)         (G_TYPE_CHECK_INSTANCE_TYPE ((o), MAME_TYPE_EXEC))
#define MAME_IS_EXEC_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), MAME_TYPE_EXEC))
#define MAME_EXEC_GET_CLASS(o)  (G_TYPE_INSTANCE_GET_CLASS ((o), MAME_TYPE_EXEC, MameExecClass))

typedef struct _MameExec MameExec;
typedef struct _MameExecClass MameExecClass;
typedef struct _MameExecPrivate MameExecPrivate;

struct _MameExec {
	GObject parent;
	
	MameExecPrivate *priv;
	/* define public instance variables here */
};

struct _MameExecClass {
	GObjectClass parent;
	/* define vtable methods and signals here */
};

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

enum
{
	PROP_EXEC_0,
	/* Executable Properties */
	PROP_EXEC_PATH,
	PROP_EXEC_NAME,
	PROP_EXEC_TARGET,
	PROP_EXEC_VERSION

};

GType mame_exec_get_type (void);
MameExec* mame_exec_new (void);
MameExec* mame_exec_new_from_path (gchar *path);
gchar* mame_exec_get_name (MameExec *exec);
gchar* mame_exec_get_path (MameExec *exec);
gchar* mame_exec_get_version (MameExec *exec);
gchar* mame_exec_get_target (MameExec *exec);
gchar* mame_exec_get_noloadconfig_option (MameExec *exec);
ExecutableType mame_exec_get_exectype (MameExec *exec);


/**
* Compares the version of this executable with the given one, using the version numbers in
* the version string.
*
* Returns:
*	 <0 this executable < version
*	  0 this executable == version
*	 >0 this executable > version
*/
int
mame_compare_raw_version (const MameExec *exec, const gchar *version);

/** Gets the available_options for the executable
* It will only parse them the first time so
* you can call this often.
*/
const GHashTable *
mame_get_options (MameExec *exec);

/** Returns the mame name for this option.
* For example: "mapkey" will return "sdlmapkey" in xmame.SDL because they were declared aliases.
* See ADD_OPTION_ALIAS in option_hash_init() to see how this works.
*/
const gchar
*mame_get_option_name (const MameExec *exec, const gchar* option_name);

gboolean
mame_option_supports_no_prefix (MameExec *exec, const gchar *option_name);

/** Checks if the executable has the given option */
gboolean
mame_has_option (MameExec *exec, const gchar *option_name);

/** Returns the option with the given name */
const MameOption *
mame_get_option (MameExec *exec, const gchar *option_name);

const gchar *
mame_get_option_value (const MameExec *exec, const gchar *option_name, int index);

/**
* Returns a NULL terminated-array with possible values for this option.
*/
const gchar **
mame_get_option_values (const MameExec *exec, const gchar *option_name);

/* Counts the number of possible values for this option. 
* On error it returns -1.
*/
int
mame_get_option_value_count (const MameExec *exec, const gchar *option_name);

const gchar **
mame_get_option_keys (const MameExec *exec, const gchar *option_name);

const gchar *
mame_get_option_key_value (const MameExec *exec, const gchar *option_name, const gchar *key);

/** Returns the option string for this option.
* If the option does not exist or it does not take arguments it returns NULL.
*/
gchar *
mame_get_option_string (const MameExec *exec, const gchar *option_name, const gchar *arguments);

/** Returns the option string for an option with int argument.
* If the option does not exist or the option does not take an int parameter it returns NULL.
*
* Example: xmame_get_int_option_string(exec, "modenumber", 2) will return:
" -modenumber 2
*/
gchar *
mame_get_int_option_string (const MameExec *exec, const gchar *option_name, int argument);

/** Returns the option string for an option that does not take any arguments.
* If the option does not exist or it takes arguments it will return NULL.
*/
gchar *
mame_get_boolean_option_string (const MameExec *exec, const gchar *option_name, gboolean is_enabled);

/* Returns the option string for an option that takes a float arguments.
* If the option does not exist or it does take float arguments it will return NULL.
*/
gchar *
mame_get_float_option_string (const MameExec *exec, const gchar *option_name, float argument, char* buf);

void
mame_exec_launch_command (gchar *command, pid_t *pid, int *stdout, int *stderr);

int
mame_exec_get_game_count (MameExec *exec);

GIOChannel *
mame_executable_set_up_io_channel (gint fd, GIOCondition cond, GIOFunc func, gpointer data);

/**
* Runs the executable with the given options and returns an open pipe.
*/
FILE *
mame_open_pipe (const MameExec *exec, const gchar *format, ...)
__attribute__((format(printf, 2, 3)))
;

void
mame_close_pipe (const MameExec *exec, FILE *pipe);

G_END_DECLS

#endif