/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * GMAMEUI
 *
 * Copyright 2007-2009 Andrew Burton <adb@iinet.net.au>
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

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>  /* For pid_t */
#include <sys/wait.h>   /* For WIF* */
#include <signal.h>      /* For SIGTERM */
#include <gdk/gdkkeysyms.h>
#include <glade/glade.h>

#include "gui.h"
#include "audit.h"
#include "options_string.h"
#include "mame-exec.h"
#include "gmameui-marshaller.h"

#define BUFFER_SIZE 1000

static void
process_audit_romset (gchar *line, gint settype);

/* Audit class stuff */
G_DEFINE_TYPE (GmameuiAudit, gmameui_audit, G_TYPE_OBJECT)

struct _GmameuiAuditPrivate {
	gchar *name;
};

/* Signals enumeration */
enum
{
	ROMSET_AUDITED,		/* Emitted when a romset or sampleset line is found */
	ROM_AUDIT_COMPLETE,     /* Emitted when the romset audit process has finished */
	SAMPLE_AUDIT_COMPLETE,  /* Emitted when the sampleset audit process has finished */
	LAST_SIGNAL
};


static pid_t        command_pid;
static int          child_stdout;
static int          child_stderr;

static pid_t        command_sample_pid;
static int          child_sample_stdout;
static int          child_sample_stderr;

/* Audit class stuff */
static guint signals[LAST_SIGNAL] = { 0 };

static void gmameui_audit_class_init (GmameuiAuditClass *klass);
static void gmameui_audit_init (GmameuiAudit *au);
static void gmameui_audit_finalize (GObject *obj);

static void
gmameui_audit_class_init (GmameuiAuditClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	
/*	object_class->set_property = gmameui_audit_set_property;
	object_class->get_property = gmameui_audit_get_property;*/
	object_class->finalize = gmameui_audit_finalize;
	
	signals[ROMSET_AUDITED] = g_signal_new ("romset-audited",
						G_OBJECT_CLASS_TYPE (object_class),
						G_SIGNAL_RUN_FIRST,
						G_STRUCT_OFFSET (GmameuiAuditClass, romset_audited),
						NULL, NULL,     /* Accumulator and accumulator data */
						gmameui_marshaller_VOID__STRING_INT_INT,
						G_TYPE_NONE,    /* Return type */
						3, G_TYPE_STRING, G_TYPE_INT, G_TYPE_INT	/* Three parameters */
						);
	

	signals[ROM_AUDIT_COMPLETE] = g_signal_new ("rom-audit-complete",
						G_TYPE_FROM_CLASS(klass),
						G_SIGNAL_RUN_FIRST,
						0,		/* This signal is not handled by the class */
						NULL, NULL,     /* Accumulator and accumulator data */
						g_cclosure_marshal_VOID__VOID,
						G_TYPE_NONE,    /* Return type */
						0	        /* No parameters */
						);
	
	signals[SAMPLE_AUDIT_COMPLETE] = g_signal_new ("sample-audit-complete",
						G_TYPE_FROM_CLASS(klass),
						G_SIGNAL_RUN_FIRST,
						0,		/* This signal is not handled by the class */
						NULL, NULL,     /* Accumulator and accumulator data */
						g_cclosure_marshal_VOID__VOID,
						G_TYPE_NONE,    /* Return type */
						0	        /* No parameters */
						);
}

static void
gmameui_audit_init (GmameuiAudit *au)
{
	
GMAMEUI_DEBUG ("Creating gmameui_audit object");	
	au->priv = g_new0 (GmameuiAuditPrivate, 1);
	
GMAMEUI_DEBUG ("Creating gmameui_audit object... done");
}

GmameuiAudit* gmameui_audit_new (void)
{
	return g_object_new (GMAMEUI_TYPE_AUDIT, NULL);
}

static void
gmameui_audit_finalize (GObject *obj)
{
	GMAMEUI_DEBUG ("Finalising gmameui_audit object");
	
	GmameuiAudit *au = GMAMEUI_AUDIT (obj);
	
	
	g_free (au->priv);
	
	GMAMEUI_DEBUG ("Finalising gmameui_audit object... done");

}


/* Function takes a line e.g. 'rom astro is bad' and extracts the romname -
   this is usually the second word. This is usually used so that the calling
   area can then find the relevant ROM and update it */
gchar*
get_romset_name_from_audit_line (gchar *line)
{
	gchar *romname, *p;
	
	/* Return if romname is NULL or contains no spaces */
	g_return_val_if_fail (line != NULL, NULL);
	g_return_val_if_fail (strstr (line, " ") != NULL, NULL);
	
	romname = strstr (line, " ") + 1;
	
	/* FIXME TODO sampleset not found contains " around romname */
	for (p = strstr (line, " ") + 1; (*p && (*p != ' ') && (*p != '\n')); p++);
	*p = '\0';
	
	/*GMAMEUI_DEBUG ("%s - %s", line, romname);*/
	
	return romname;
}

static void
spawned_audit_complete (GPid child_pid, gint status, gpointer user_data)
{
	g_print ("Child PID: %lu exit status: %d\n", (gulong) child_pid, status);

	/* Not sure how the exit status works on win32. Unix code follows */

#ifdef G_OS_UNIX

	if (WIFEXITED (status)) /* Did child terminate in a normal way? */
	{
		if (WEXITSTATUS (status) == EXIT_SUCCESS)
		{
			g_print ("Child PID: %lu exited normally without errors.\n", (gulong) child_pid);
		}
		else
		{
			g_print ("Child PID: %lu exited with an error (code %d).\n", 
			         (gulong) child_pid, WEXITSTATUS (status));
		}

	}
	else if (WIFSIGNALED (status)) /* was it terminated by a signal */
	{
		g_print ("Child PID: %lu was terminated by signal %d\n", 
		         (gulong) child_pid, WTERMSIG (status));
		
	}
	else 
	{
		g_print ("Child PID: %lu was terminated in some other way.\n", 
		         (gulong) child_pid);
	}

#endif /* G_OS_UNIX */

	g_spawn_close_pid (child_pid); /* does nothing on unix, needed on win32 */

}

static gboolean
handle_sample_audit_command_stdout_io (GIOChannel * ioc,
				GIOCondition condition,
				gpointer data)
{
	gboolean broken_pipe = FALSE;
	
	if (condition & G_IO_IN) {      /* G_IO_IN = Data to read on the pipe */
		GError * error = NULL;
		GString * string;

		string = g_string_new (NULL);
		
		while (ioc->is_readable != TRUE);
		
		do {
			gint status;
			do {
				status = g_io_channel_read_line_string (ioc, string, NULL, &error);
				if (status == G_IO_STATUS_EOF) {
					/* G_IO_STATUS_EOF = End of file */
					broken_pipe = TRUE;
				} else if (status == G_IO_STATUS_AGAIN) {
					/* G_IO_STATUS_AGAIN = Resource temporarily unavailable */
					if (gtk_events_pending ()) {
						while (gtk_events_pending ()) {
							gtk_main_iteration ();
						}
					}
				} 
			} while (status == G_IO_STATUS_AGAIN && broken_pipe == FALSE);

			if (broken_pipe == TRUE) {
				break;
			}

			if (status != G_IO_STATUS_NORMAL) {
				if (error != NULL) {
					g_warning ("handle_audit_command_stdout_io(): %s", error->message);
					g_error_free (error);
				}
				continue;
			}
			
			string = g_string_truncate (string, string->len - 1);
			if (string->len <= 1) {
				continue;
			}

			process_audit_romset (string->str, AUDIT_TYPE_SAMPLE);

			while (gtk_events_pending ())
				gtk_main_iteration ();
			
		} while (g_io_channel_get_buffer_condition (ioc) & G_IO_IN);

		g_string_free (string, TRUE);
	}
	
	if (!(condition & G_IO_IN) || broken_pipe == TRUE) {
		GMAMEUI_DEBUG ("Sample audit completed");

		g_signal_emit (gui_prefs.audit, signals[SAMPLE_AUDIT_COMPLETE], 0, NULL);
		return FALSE;
	}

	return TRUE;
}

static gboolean
handle_audit_command_stdout_io (GIOChannel * ioc,
				GIOCondition condition,
				gpointer data)
{
	gboolean broken_pipe = FALSE;
	
	if (condition & G_IO_IN) {      /* G_IO_IN = Data to read on the pipe */
		GError * error = NULL;
		GString * string;
		string = g_string_new (NULL);
		
		while (ioc->is_readable != TRUE);
		
		do {
			gint status;
			do {
				status = g_io_channel_read_line_string (ioc, string, NULL, &error);
				if (status == G_IO_STATUS_EOF) {
					/* G_IO_STATUS_EOF = End of file */
					broken_pipe = TRUE;
				} else if (status == G_IO_STATUS_AGAIN) {
					/* G_IO_STATUS_AGAIN = Resource temporarily unavailable */
				} 
			} while (status == G_IO_STATUS_AGAIN && broken_pipe == FALSE);

			if (broken_pipe == TRUE) {
				break;
			}

			if (status != G_IO_STATUS_NORMAL) {
				if (error != NULL) {
					g_warning ("handle_audit_command_stdout_io(): %s", error->message);
					g_error_free (error);
				}
				continue;
			}
			
			string = g_string_truncate (string, string->len - 1);
			if (string->len <= 1) {
				continue;
			}

			process_audit_romset (string->str, AUDIT_TYPE_ROM);

			while (gtk_events_pending ())
				gtk_main_iteration ();
			
		} while (g_io_channel_get_buffer_condition (ioc) & G_IO_IN);

		g_string_free (string, TRUE);
	}
	
	if (!(condition & G_IO_IN) || broken_pipe == TRUE) {
		/* FIXME TODO Pipe finishes for auditing single rom before ROM_AUDITED signal can be emitted
		   Test with the less complicated example before using this one */
		GMAMEUI_DEBUG ("Audit completed");

		g_signal_emit (gui_prefs.audit, signals[ROM_AUDIT_COMPLETE], 0, NULL);
		g_io_channel_shutdown (ioc, TRUE, NULL);
		
		return FALSE;
	}

	return TRUE;
}

/* The sample auditing returns details about the sampleset being incorrect on stderr,
   so we need to catch if on this pipe. Note the romset being incorrect happens on
   stdout, so can be caught on the stdout pipe. */
static gboolean
handle_audit_command_stderr_io (GIOChannel * ioc,
				GIOCondition condition,
				gpointer data)
{
	gboolean broken_pipe = FALSE;

	if (condition & G_IO_IN) {      /* G_IO_IN = Data to read on the pipe */
		GError * error = NULL;
		GString * string;

		string = g_string_new (NULL);
		
		while (ioc->is_readable != TRUE);
		
		do {
			gint status;
			do {
				status = g_io_channel_read_line_string (ioc, string, NULL, &error);
				if (status == G_IO_STATUS_EOF) {
					/* G_IO_STATUS_EOF = End of file */
					broken_pipe = TRUE;
				} else if (status == G_IO_STATUS_AGAIN) {
					/* G_IO_STATUS_AGAIN = Resource temporarily unavailable */
				}
			} while (status == G_IO_STATUS_AGAIN && broken_pipe == FALSE);

			if (broken_pipe == TRUE) {
				break;
			}

			if (status != G_IO_STATUS_NORMAL) {
				if (error != NULL) {
					g_warning ("handle_audit_command_stderr_io(): %s", error->message);
					g_error_free (error);
				}
			}
			
			string = g_string_truncate (string, string->len - 1);
			if (string->len <= 1) {
				continue;
			}

			process_audit_romset (string->str, AUDIT_TYPE_SAMPLE);
			
		} while (g_io_channel_get_buffer_condition (ioc) & G_IO_IN);

		g_string_free (string, TRUE);
	}
	
	if (!(condition & G_IO_IN) || broken_pipe == TRUE) {
		g_io_channel_shutdown (ioc, TRUE, NULL);
		return FALSE;
	}

	return TRUE;
}

/* This function processes a line from the output of the MAME audit functions
   verifyroms and verifysamples. This result is emitted as a signal so any
   interested client can handle it. This lets us run the audit as a separate
   process and handle the output on a line-by-line process. */
static void
process_audit_romset (gchar *line, gint settype) {
	gchar *p;
	gchar *tmp;
	gint result;
	
	result = 0;
	
	tmp = g_strdup (line);

	/* 0121	romset name [parent] is {good|bad|best available}
	   0105 romset name {correct|incorrect|best available} */
	/* Only process the romset lines; individual chips are ignored */
	if ((strncmp (tmp, "romset", 6) == 0) || (strncmp (tmp, "sampleset", 9) == 0)) {
		for (p = strstr (tmp, " ") + 1; (*p && (*p != ' ') && (*p != '\n')); p++);
		*p = '\0';
		p++;

		/* FIXME TODO ROMs report 'is bad' even if the error is just an
		   incorrect length on one of the roms */
		if (strstr (p, "incorrect") || strstr (p, "is bad"))
		{
			/* Version 0.107 (?) and earlier used:
			    romset XXX incorrect
			   Version 0.117 (?) and later used:
			    romset XXX is bad */
			result = INCORRECT;
		} else if ((strstr (p, "correct")) || (strstr(p, "is good"))) {
			/* Version 0.107 (?) and earlier used:
			    romset XXX correct
			   Version 0.117 (?) and later used:
			    romset XXX is good */
			result = CORRECT;
		} else if (strstr (p, "not found")) {
			result = NOT_AVAIL;
		} else if (strstr (p, "best available")) {
			/* Version 0.107 (?) and earlier used:
			    romset XXX best available
			   Version 0.117 (?) and later used:
			    romset XXX is best available */
			result = BEST_AVAIL;
		} else {
			GMAMEUI_DEBUG ("Audit: Could not recognise line %s", p);
			result = UNKNOWN;
		}

	} else if (!strncmp (tmp, "name", 4) || !strncmp (tmp, "---", 3)) {
		/* do nothing */
	} else {
		/* Line is for a rom within a romset */
		result = NOTROMSET;
	}

	/* Emit signal for clients to handle as they see fit */
	g_signal_emit (gui_prefs.audit, signals[ROMSET_AUDITED], 0, line, settype, result);
	 
	g_free (tmp);

}

/* Start the audit for a single ROM */
void mame_audit_start_single (gchar *romname)
{
	MameExec *exec;
	gchar *command;
	gchar *rompath_option;
	const gchar *option_name;
	
	exec = mame_exec_list_get_current_executable (main_gui.exec_list);
	
	g_return_if_fail (exec != NULL);
	
	rompath_option = create_rompath_options_string (exec);

	option_name = mame_get_option_name (exec, "verifyroms");
	
	command = g_strdup_printf ("%s -%s %s %s",
				   mame_exec_get_path (exec),
				   option_name,
				   rompath_option,
				   romname);
	
	mame_exec_launch_command (command, &command_pid, &child_stdout, &child_stderr);

	/* Add a function to watch for stdout */
	mame_executable_set_up_io_channel(child_stdout,
			  G_IO_IN|G_IO_PRI|G_IO_ERR|G_IO_HUP|G_IO_NVAL,
			  handle_audit_command_stdout_io,
			  NULL);
	
	/* Add a function to call when the spawned process finishes */
	g_child_watch_add (command_pid, (GChildWatchFunc) spawned_audit_complete, NULL);
	
	g_free (command);

	/* Samples */
	option_name = mame_get_option_name (exec, "verifysamples");
	command = g_strdup_printf("%s -%s %s %s",
				  mame_exec_get_path (exec),
				  option_name,
				  rompath_option,
				  romname);

	mame_exec_launch_command (command, &command_sample_pid, &child_sample_stdout, &child_sample_stderr);
	
	/* Add a function to watch for stdout */
	mame_executable_set_up_io_channel(child_sample_stdout,
					  G_IO_IN|G_IO_PRI|G_IO_ERR|G_IO_HUP|G_IO_NVAL,
					  handle_sample_audit_command_stdout_io, NULL);

	/* Add a function to watch for stderr. Lines "sampleset "cosmicg" not found! return on the stderr,
	   so will not be found and processed if we don't monitor stderr as well */
	mame_executable_set_up_io_channel(child_sample_stderr,
					  G_IO_IN|G_IO_PRI|G_IO_ERR|G_IO_HUP|G_IO_NVAL,
					  handle_audit_command_stderr_io, NULL);

	/* Add a function to call when the spawned process finishes */
	g_child_watch_add (command_sample_pid, (GChildWatchFunc) spawned_audit_complete, NULL);
	
	/* Free strings */
	g_free (command);	
}

/* Start the full audit across all ROMs */
void
mame_audit_start_full (void)
{
	MameExec *exec;
	gchar *rompath_option;
	GList *listpointer;
	MameRomEntry *tmprom;
	const gchar *option_name;
	
	gchar *command;
	
	exec = mame_exec_list_get_current_executable (main_gui.exec_list);
	
	g_return_if_fail (exec != NULL);

	option_name = mame_get_option_name (exec, "verifyroms");

	if (!option_name) {
		gmameui_message (ERROR, NULL, _("Don't know how to verify roms with this version of xmame."));
		return;
	}

	/* In more recent versions of MAME, only the available (i.e. where the filename exists)
	   ROMs are audited. So, we need to set the status of all the others to 'unavailable' */
	GList *romlist;
	romlist = mame_gamelist_get_roms_glist (gui_prefs.gl);
	for (listpointer = g_list_first (romlist);
	     (listpointer != NULL);
	     listpointer = g_list_next (listpointer))
	{
		tmprom = (MameRomEntry *) listpointer->data;
		g_object_set (tmprom, "has-roms", NOT_AVAIL, NULL);
	}
	
	rompath_option = create_rompath_options_string (exec);

	/* FIXME TODO  2>/dev/null will send stderr to /dev/null, so we won't need to add g_io_watch to it */
	command = g_strdup_printf("%s -%s %s", mame_exec_get_path (exec), option_name, rompath_option);

	mame_exec_launch_command (command, &command_pid, &child_stdout, &child_stderr);

	/* Add a function to watch for stdout */
	mame_executable_set_up_io_channel(child_stdout,
			  G_IO_IN|G_IO_PRI|G_IO_ERR|G_IO_HUP|G_IO_NVAL,
			  handle_audit_command_stdout_io,
			  NULL);

	/* Add a function to watch for stderr 
	mame_executable_set_up_io_channel(child_stderr,
			  G_IO_IN|G_IO_PRI|G_IO_ERR|G_IO_HUP|G_IO_NVAL,
			  handle_audit_command_stderr_io,
			  NULL);*/

	/* Add a function to call when the spawned process finishes. This
	   is compulsory since we use G_SPAWN_DO_NOT_REAP_CHILD */
	g_child_watch_add (command_pid, (GChildWatchFunc) spawned_audit_complete, NULL);

	/* Free strings */
	g_free (command);

	/* Samples now */
	command = g_strdup_printf("%s -%s %s", mame_exec_get_path (exec), mame_get_option_name (exec, "verifysamples"), rompath_option);

	mame_exec_launch_command (command, &command_sample_pid, &child_sample_stdout, &child_sample_stderr);
	
	/* Add a function to watch for stdout */
	mame_executable_set_up_io_channel(child_sample_stdout,
					  G_IO_IN|G_IO_PRI|G_IO_ERR|G_IO_HUP|G_IO_NVAL,
					  handle_sample_audit_command_stdout_io, NULL);

	/* Add a function to watch for stderr */
	mame_executable_set_up_io_channel(child_sample_stderr,
					  G_IO_IN|G_IO_PRI|G_IO_ERR|G_IO_HUP|G_IO_NVAL,
					  handle_audit_command_stderr_io, NULL);

	/* Add a function to call when the spawned process finishes */
	g_child_watch_add (command_sample_pid, (GChildWatchFunc) spawned_audit_complete, NULL);
	
	/* Free strings */
	g_free (command);

}

/* Stop the audit in process; this will trigger a signal emission for HUP */
void
mame_audit_stop_full_audit (GmameuiAudit *au)
{
	if (command_pid > 0)
		kill (command_pid, SIGTERM);
	if (command_sample_pid > 0)
		kill (command_sample_pid, SIGTERM);
}
