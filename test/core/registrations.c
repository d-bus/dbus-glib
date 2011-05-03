/* Regression test for object registration and unregistration
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
  /* we close the connection before releasing the object, to test fd.o #5688
   * in test_lookup() */
  if (f->bus != NULL)
    {
      dbus_connection_close (dbus_g_connection_get_connection (f->bus));
      dbus_g_connection_unref (f->bus);
    }

  if (f->object != NULL)
    {
      g_object_unref (f->object);
    }
}

static void
test_lookup (Fixture *f,
    gconstpointer test_data G_GNUC_UNUSED)
{
  /* teardown() closes the connection before f->object is destroyed, which
   * used to be broken */
  g_test_bug ("5688");

  dbus_g_connection_register_g_object (f->bus, "/foo", f->object);
  g_assert (dbus_g_connection_lookup_g_object (f->bus, "/foo") ==
      f->object);
  /* this was briefly broken while fixing fd.o#5688 */
  g_assert (dbus_g_connection_lookup_g_object (f->bus, "/bar") == NULL);
}

static void
test_unregister (Fixture *f,
    gconstpointer test_data G_GNUC_UNUSED)
{
  /* feature test: objects can be unregistered */
  g_test_bug ("21219");

  dbus_g_connection_register_g_object (f->bus, "/foo", f->object);
  g_assert (dbus_g_connection_lookup_g_object (f->bus, "/foo") ==
      f->object);
  dbus_g_connection_unregister_g_object (f->bus, f->object);
  g_assert (dbus_g_connection_lookup_g_object (f->bus, "/foo") == NULL);
}

static void
test_unregister_on_last_unref (Fixture *f,
    gconstpointer test_data G_GNUC_UNUSED)
{
  gpointer weak_pointer;

  weak_pointer = f->object;
  g_object_add_weak_pointer (weak_pointer, &weak_pointer);

  dbus_g_connection_register_g_object (f->bus, "/foo", f->object);
  g_assert (dbus_g_connection_lookup_g_object (f->bus, "/foo") ==
      f->object);
  /* implicit unregistration by the last-unref of the object */
  g_object_unref (f->object);
  f->object = NULL;

  g_assert (weak_pointer == NULL);

  g_assert (dbus_g_connection_lookup_g_object (f->bus, "/foo") == NULL);
}

static void
test_unregister_on_forced_dispose (Fixture *f,
    gconstpointer test_data G_GNUC_UNUSED)
{
  dbus_g_connection_register_g_object (f->bus, "/foo", f->object);
  g_assert (dbus_g_connection_lookup_g_object (f->bus, "/foo") ==
      f->object);
  /* implicit unregistration by dispose() of the object (don't try
   * this at home) */
  g_object_run_dispose (f->object);

  g_assert (dbus_g_connection_lookup_g_object (f->bus, "/foo") == NULL);
}

static void
test_reregister (Fixture *f,
    gconstpointer test_data G_GNUC_UNUSED)
{
  dbus_g_connection_register_g_object (f->bus, "/foo", f->object);
  g_assert (dbus_g_connection_lookup_g_object (f->bus, "/foo") ==
      f->object);

  /* Before 0.82, re-registering the same object path was leaky but successful.
   * 0.82 disallowed this behaviour. Since 0.84 it was meant to be allowed
   * again, and a no-op, but it actually had the effect of removing all
   * record of the registrations (while leaving the object registered with
   * libdbus). */
  dbus_g_connection_register_g_object (f->bus, "/foo", f->object);
  g_assert (dbus_g_connection_lookup_g_object (f->bus, "/foo") ==
      f->object);

  /* This would critical in 0.84. */
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

  g_test_add ("/registrations/lookup", Fixture, NULL,
      setup, test_lookup, teardown);
  g_test_add ("/registrations/unregister", Fixture, NULL,
      setup, test_unregister, teardown);
  g_test_add ("/registrations/unregister-on-last-unref", Fixture, NULL,
      setup, test_unregister_on_last_unref, teardown);
  g_test_add ("/registrations/unregister-on-forced-dispose", Fixture, NULL,
      setup, test_unregister_on_forced_dispose, teardown);
  g_test_add ("/registrations/reregister", Fixture, NULL,
      setup, test_reregister, teardown);

  return g_test_run ();
}
