/* Feature test for freedesktop.org #21219 and similar.
 *
 * Copyright © 2009 Collabora Ltd. <http://www.collabora.co.uk/>
 * Copyright © 2009-2011 Nokia Corporation
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

#include "my-object.h"

GMainLoop *loop = NULL;

typedef struct {
    DBusGConnection *bus;
    GObject *object;
} Fixture;

static void
setup (Fixture *f,
    gconstpointer path_to_use)
{
  f->bus = dbus_g_bus_get_private (DBUS_BUS_SESSION, NULL, NULL);
  g_assert (f->bus != NULL);

  f->object = g_object_new (MY_TYPE_OBJECT, NULL);
  g_assert (MY_IS_OBJECT (f->object));
}

static void
teardown (Fixture *f,
    gconstpointer test_data G_GNUC_UNUSED)
{
  if (f->object != NULL)
    {
      g_object_unref (f->object);
    }

  if (f->bus != NULL)
    {
      dbus_connection_close (dbus_g_connection_get_connection (f->bus));
      dbus_g_connection_unref (f->bus);
    }
}

static void
test_unregister (Fixture *f,
    gconstpointer test_data G_GNUC_UNUSED)
{
  dbus_g_connection_register_g_object (f->bus, "/foo", f->object);
  g_assert (dbus_g_connection_lookup_g_object (f->bus, "/foo") ==
      f->object);
  dbus_g_connection_unregister_g_object (f->bus, f->object);
  g_assert (dbus_g_connection_lookup_g_object (f->bus, "/foo") == NULL);
}

int
main (int argc, char **argv)
{
  loop = g_main_loop_new (NULL, FALSE);

  g_type_init ();
  g_log_set_always_fatal (G_LOG_LEVEL_WARNING | G_LOG_LEVEL_CRITICAL);
  dbus_g_type_specialized_init ();
  g_test_bug_base ("https://bugs.freedesktop.org/show_bug.cgi?id=");
  g_test_init (&argc, &argv, NULL);

  g_test_add ("/registrations/unregister", Fixture, NULL,
      setup, test_unregister, teardown);

  return g_test_run ();
}
