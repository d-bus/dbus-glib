/* Regression test for freedesktop.org #5688.
 *
 * Copyright © 2009 Collabora Ltd. <http://www.collabora.co.uk/>
 * Copyright © 2009 Nokia Corporation
 *
 * In preparation for dbus-glib relicensing (if it ever happens), this file is
 * licensed under (at your option) either the AFL v2.1, the GPL v2 or later,
 * or an MIT/X11-style license:
 *
 * Licensed under the Academic Free License version 2.1
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include <dbus/dbus-glib.h>
#include <dbus/dbus-glib-lowlevel.h>

#include "dbus/dbus-gutils.h"     /* for DBUS_G_CONNECTION_FROM_CONNECTION */

#include "my-object.h"

GMainLoop *loop;

int
main (int argc, char **argv)
{
  DBusConnection *d_connection;
  DBusGConnection *connection;
  DBusError d_error;
  GObject *obj;

  dbus_error_init (&d_error);
  loop = g_main_loop_new (NULL, FALSE);

  g_type_init ();
  g_log_set_always_fatal (G_LOG_LEVEL_WARNING | G_LOG_LEVEL_CRITICAL);

  d_connection = dbus_bus_get_private (DBUS_BUS_SESSION, NULL);

  if (d_connection == NULL)
    g_error ("%s: %s", d_error.name, d_error.message);

  dbus_connection_setup_with_g_main (d_connection, NULL);

  connection = DBUS_G_CONNECTION_FROM_CONNECTION (d_connection);

  obj = g_object_new (MY_TYPE_OBJECT, NULL);
  dbus_g_connection_register_g_object (connection, "/foo", obj);
  g_assert (dbus_g_connection_lookup_g_object (connection, "/foo") == obj);
  g_assert (dbus_g_connection_lookup_g_object (connection, "/bar") == NULL);

  dbus_connection_close (d_connection);
  dbus_connection_unref (d_connection);

  g_object_unref (obj);

  return 0;
}
