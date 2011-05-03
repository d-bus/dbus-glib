/* Feature test for freedesktop.org #21219.
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

#include "my-object.h"

GMainLoop *loop;

int
main (int argc, char **argv)
{
  DBusGConnection *connection;
  GError *error = NULL;
  GObject *obj;

  loop = g_main_loop_new (NULL, FALSE);

  g_type_init ();
  g_log_set_always_fatal (G_LOG_LEVEL_WARNING | G_LOG_LEVEL_CRITICAL);

  connection = dbus_g_bus_get (DBUS_BUS_SESSION, &error);

  if (connection == NULL)
    g_error ("failed to get bus: %s %d: %s", g_quark_to_string (error->domain),
        error->code, error->message);

  obj = g_object_new (MY_TYPE_OBJECT, NULL);
  dbus_g_connection_register_g_object (connection, "/foo", obj);
  g_assert (dbus_g_connection_lookup_g_object (connection, "/foo") == obj);
  dbus_g_connection_unregister_g_object (connection, obj);
  g_assert (dbus_g_connection_lookup_g_object (connection, "/foo") == NULL);

  g_object_unref (obj);

  return 0;
}
