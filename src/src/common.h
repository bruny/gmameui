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

#ifndef __COMMON_H__
#define __COMMON_H__

#define _POSIX_C_SOURCE 199506L

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

/* Standard gettext macros. */
#ifdef ENABLE_NLS
#  include <libintl.h>
#  undef _
#  define _(String) dgettext (PACKAGE, String)
#  ifdef gettext_noop
#    define N_(String) gettext_noop (String)
#  else
#    define N_(String) (String)
#  endif
#else
#  define textdomain(String) (String)
#  define gettext(String) (String)
#  define dgettext(Domain,Message) (Message)
#  define dcgettext(Domain,Message,Type) (Message)
#  define bindtextdomain(Domain,Directory) (Domain)
#  define _(String) (String)
#  define N_(String) (String)
#endif

#ifdef ENABLE_DEBUG

#ifdef __GNUC__
#define GMAMEUI_DEBUG(format, args...) g_log (NULL, G_LOG_LEVEL_DEBUG, format, ##args)
#else
# include <stdarg.h>
static void GMAMEUI_DEBUG (const gchar *format, ...)
{
	va_list args;
	va_start (args, format);
	g_logv (NULL, G_LOG_LEVEL_DEBUG, format, args);
	va_end (args);
}
#endif /* __GNUC__ */

#else
#  ifdef __GNUC__
#    define GMAMEUI_DEBUG(msg...) do { } while(0)
#  else
#    define GMAMEUI_DEBUG(...) do { } while(0)
#  endif
#endif /* ENABLE_DEBUG */

#define ERROR GTK_MESSAGE_ERROR
#define	WARNING GTK_MESSAGE_WARNING
#define	MESSAGE GTK_MESSAGE_INFO

void gmameui_message (GtkDialogFlags type, GtkWindow *window, const gchar *format, ...)
     __attribute__((format (printf, 3, 4)));

#define UPDATE_GUI do { while (gtk_events_pending ()) { gtk_main_iteration (); } } while (0)

#endif
