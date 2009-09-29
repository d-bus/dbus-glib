/* -*- mode: C; c-file-style: "gnu" -*- */
/* dbus-glib.c General GLib binding stuff
 *
 * Copyright (C) 2004 Red Hat, Inc.
 *
 * Licensed under the Academic Free License version 2.1
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <config.h>
#include "dbus-glib.h"
#include "dbus-glib-lowlevel.h"
#include "dbus-gtest.h"
#include "dbus-gutils.h"
#include "dbus-gobject.h"
#include <string.h>

#include <libintl.h>
#define _(x) dgettext (GETTEXT_PACKAGE, x)
#define N_(x) x


/**
 * SECTION:dbus-gconnection
 * @short_description: DBus Connection
 * @see_also: #DBusConnection
 * @stability: Stable
 *
 * A #DBusGConnection is a boxed type abstracting a DBusConnection.
 */

/**
 * dbus_g_connection_flush:
 * @connection: the #DBusGConnection to flush
 *
 * Blocks until outgoing calls and signal emissions have been sent.
 */
void
dbus_g_connection_flush (DBusGConnection *connection)
{
  dbus_connection_flush (DBUS_CONNECTION_FROM_G_CONNECTION (connection));
}

/**
 * dbus_g_connection_ref:
 * @gconnection the #DBusGConnection to ref
 *
 * Increment refcount on a #DBusGConnection
 * 
 * Returns: the connection that was ref'd
 */
DBusGConnection*
dbus_g_connection_ref (DBusGConnection *gconnection)
{
  DBusConnection *c;

  c = DBUS_CONNECTION_FROM_G_CONNECTION (gconnection);
  dbus_connection_ref (c);
  return gconnection;
}


/**
 * dbus_g_connection_unref:
 * @gconnection: the connection to unref
 * 
 * Decrement refcount on a #DBusGConnection
 */
void
dbus_g_connection_unref (DBusGConnection *gconnection)
{
  DBusConnection *c;

  c = DBUS_CONNECTION_FROM_G_CONNECTION (gconnection);
  dbus_connection_unref (c);
}


/**
 * SECTION:dbus-gmessage
 * @short_description: DBus Message
 * @see_also: #DBusMessage
 * @stability: Stable
 *
 * A #DBusGConnection is a boxed type abstracting a DBusMessage.
 */

/**
 * dbus_g_message_ref:
 * @gmessage: the message to ref
 *
 * Increment refcount on a #DBusGMessage
 * 
 * Returns: the message that was ref'd
 */
DBusGMessage*
dbus_g_message_ref (DBusGMessage *gmessage)
{
  DBusMessage *c;

  c = DBUS_MESSAGE_FROM_G_MESSAGE (gmessage);
  dbus_message_ref (c);
  return gmessage;
}

/**
 * dbus_g_message_unref:
 * @gmessage: the message to unref
 * 
 * Decrement refcount on a #DBusGMessage
 */
void
dbus_g_message_unref (DBusGMessage *gmessage)
{
  DBusMessage *c;

  c = DBUS_MESSAGE_FROM_G_MESSAGE (gmessage);
  dbus_message_unref (c);
}

/**
 * SECTION:dbus-gerror
 * @short_description: DBus GError
 * @see_also: #GError
 * @stability: Stable
 *
 * #DBusGError is the #GError used by DBus.
 */

/**
 * dbus_g_error_quark:
 *
 * The implementation of #DBUS_GERROR error domain. See documentation
 * for #GError in GLib reference manual.
 *
 * Returns: the error domain quark for use with #GError
 */
GQuark
dbus_g_error_quark (void)
{
  static GQuark quark = 0;
  if (quark == 0)
    quark = g_quark_from_static_string ("dbus-glib-error-quark");
  return quark;
}

/**
 * dbus_g_error_has_name:
 * @error: the GError given from the remote method
 * @name: the D-BUS error name
 * @msg: the D-BUS error detailed message
 *
 * Determine whether D-BUS error name for a remote exception matches
 * the given name.  This function is intended to be invoked on a
 * GError returned from an invocation of a remote method, e.g. via
 * dbus_g_proxy_end_call.  It will silently return FALSE for errors
 * which are not remote D-BUS exceptions (i.e. with a domain other
 * than DBUS_GERROR or a code other than
 * DBUS_GERROR_REMOTE_EXCEPTION).
 *
 * Returns: TRUE iff the remote error has the given name
 */
gboolean
dbus_g_error_has_name (GError *error, const char *name)
{
  g_return_val_if_fail (error != NULL, FALSE);

  if (error->domain != DBUS_GERROR
      || error->code != DBUS_GERROR_REMOTE_EXCEPTION)
    return FALSE;

  return !strcmp (dbus_g_error_get_name (error), name);
}

/**
 * dbus_g_error_get_name:
 * @error: the #GError given from the remote method
 * @name: the D-BUS error name
 * @msg: the D-BUS error detailed message
 *
 * This function may only be invoked on a #GError returned from an
 * invocation of a remote method, e.g. via dbus_g_proxy_end_call.
 * Moreover, you must ensure that the error's domain is #DBUS_GERROR,
 * and the code is #DBUS_GERROR_REMOTE_EXCEPTION.
 *
 * Returns: the D-BUS name for a remote exception.
 */
const char *
dbus_g_error_get_name (GError *error)
{
  g_return_val_if_fail (error != NULL, NULL);
  g_return_val_if_fail (error->domain == DBUS_GERROR, NULL);
  g_return_val_if_fail (error->code == DBUS_GERROR_REMOTE_EXCEPTION, NULL);

  return error->message + strlen (error->message) + 1;
}

/**
 * dbus_connection_get_g_type:
 * Get the GLib type ID for a #DBusConnection boxed type.
 *
 * Returns: the GLib type
 */
GType
dbus_connection_get_g_type (void)
{
  static GType our_type = 0;
  
  if (our_type == 0)
    our_type = g_boxed_type_register_static ("DBusConnection",
                                             (GBoxedCopyFunc) dbus_connection_ref,
                                             (GBoxedFreeFunc) dbus_connection_unref);

  return our_type;
}

/**
 * dbus_message_get_g_type:
 * Get the GLib type ID for a #DBusMessage boxed type.
 *
 * Returns: the GLib type
 */
GType
dbus_message_get_g_type (void)
{
  static GType our_type = 0;
  
  if (our_type == 0)
    our_type = g_boxed_type_register_static ("DBusMessage",
                                             (GBoxedCopyFunc) dbus_message_ref,
                                             (GBoxedFreeFunc) dbus_message_unref);

  return our_type;
}

/**
 * dbus_g_connection_get_g_type:
 * Get the GLib type ID for a DBusGConnection boxed type.
 *
 * Returns: the GLib type
 */
GType
dbus_g_connection_get_g_type (void)
{
  static GType our_type = 0;
  
  if (our_type == 0)
    our_type = g_boxed_type_register_static ("DBusGConnection",
                                             (GBoxedCopyFunc) dbus_g_connection_ref,
                                             (GBoxedFreeFunc) dbus_g_connection_unref);

  return our_type;
}

/**
 * dbus_g_message_get_g_type:
 * Get the GLib type ID for a #DBusGMessage boxed type.
 *
 * Returns: the GLib type
 */
GType
dbus_g_message_get_g_type (void)
{
  static GType our_type = 0;
  
  if (our_type == 0)
    our_type = g_boxed_type_register_static ("DBusGMessage",
                                             (GBoxedCopyFunc) dbus_g_message_ref,
                                             (GBoxedFreeFunc) dbus_g_message_unref);

  return our_type;
}

/**
 * SECTION:dbus-glib-lowlevel
 * @short_description: DBus lower level functions
 * @stability: Unstable
 *
 * These functions can be used to access lower level of DBus.
 */

/**
 * dbus_g_connection_get_connection:
 * @gconnection:  a #DBusGConnection
 *
 * Get the #DBusConnection corresponding to this #DBusGConnection.
 * The return value does not have its refcount incremented.
 *
 * Returns: #DBusConnection 
 */
DBusConnection*
dbus_g_connection_get_connection (DBusGConnection *gconnection)
{
  g_return_val_if_fail (gconnection, NULL);
  return DBUS_CONNECTION_FROM_G_CONNECTION (gconnection);
}

extern dbus_int32_t _dbus_gmain_connection_slot;

/**
 * dbus_connection_get_g_connection:
 * @connection:  a #DBusConnection
 *
 * Get the #DBusGConnection corresponding to this #DBusConnection.  This only
 * makes sense if the #DBusConnection was originally a #DBusGConnection that was
 * registered with the GLib main loop.  The return value does not have its
 * refcount incremented.
 *
 * Returns: #DBusGConnection 
 */
DBusGConnection*
dbus_connection_get_g_connection (DBusConnection *connection)
{
  g_return_val_if_fail (connection, NULL);
  g_return_val_if_fail (dbus_connection_get_data (connection, _dbus_gmain_connection_slot), NULL);
  
  return DBUS_G_CONNECTION_FROM_CONNECTION (connection);
}


/**
 * dbus_g_message_get_message:
 * @gmessage: a #DBusGMessage
 *
 * Get the #DBusMessage corresponding to this #DBusGMessage.
 * The return value does not have its refcount incremented.
 *
 * Returns: #DBusMessage 
 */
DBusMessage*
dbus_g_message_get_message (DBusGMessage *gmessage)
{
  return DBUS_MESSAGE_FROM_G_MESSAGE (gmessage);
}

#ifdef DBUS_BUILD_TESTS

/**
 * @ingroup DBusGLibInternals
 * Unit test for general glib stuff
 * Returns: #TRUE on success.
 */
gboolean
_dbus_glib_test (const char *test_data_dir)
{
  DBusError err;
  GError *gerror = NULL;

  dbus_error_init (&err);
  dbus_set_error_const (&err, DBUS_ERROR_NO_MEMORY, "Out of memory!");

  dbus_set_g_error (&gerror, &err);
  g_assert (gerror != NULL);
  g_assert (gerror->domain == DBUS_GERROR);
  g_assert (gerror->code == DBUS_GERROR_NO_MEMORY);
  g_assert (!strcmp (gerror->message, "Out of memory!"));
  
  dbus_error_init (&err);
  g_clear_error (&gerror);

  return TRUE;
}

#endif /* DBUS_BUILD_TESTS */
