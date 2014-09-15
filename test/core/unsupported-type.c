/*
 * Copyright © 2009-2014 Collabora Ltd. <http://www.collabora.co.uk/>
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

#include <config.h>

#include <glib.h>
#include <glib/gstdio.h>
#include <dbus/dbus.h>
#include <dbus/dbus-glib.h>
#include <dbus/dbus-glib-lowlevel.h>

#ifdef G_OS_UNIX
# include <errno.h>
# include <fcntl.h>
# include <sys/stat.h>
# include <unistd.h>
#endif

#include "my-object.h"

GMainLoop *loop = NULL;

typedef struct {
    DBusGConnection *bus;
    GObject *object;
    DBusGProxy *proxy;
    DBusGProxyCall *call;
    gsize in_flight;
    int fd;
} Fixture;

#ifdef G_OS_UNIX
static void
call_cb (DBusGProxy *proxy,
    DBusGProxyCall *call,
    gpointer user_data)
{
  Fixture *f = user_data;

  g_assert (proxy == f->proxy);
  g_assert (call == f->call);

  f->in_flight--;
}

static void
setup (Fixture *f,
    gconstpointer path_to_use)
{
  DBusConnection *libdbus;

  f->bus = dbus_g_bus_get_private (DBUS_BUS_SESSION,
      g_main_context_default (), NULL);
  g_assert (f->bus != NULL);
  libdbus = dbus_g_connection_get_connection (f->bus);
  dbus_connection_set_exit_on_disconnect (libdbus, FALSE);

  f->object = g_object_new (MY_TYPE_OBJECT, NULL);
  g_assert (MY_IS_OBJECT (f->object));
  dbus_g_connection_register_g_object (f->bus, "/object",
      (GObject *) f->object);

  f->proxy = dbus_g_proxy_new_for_name (f->bus,
      dbus_bus_get_unique_name (libdbus),
      "/notdbusglib", "org.freedesktop.DBus.GLib.Tests.Example");

  f->fd = g_open ("/dev/null", O_RDONLY, 0);

  if (f->fd < 0)
    g_error ("unable to open /dev/null: %s", g_strerror (errno));
}

static void
teardown (Fixture *f,
    gconstpointer test_data G_GNUC_UNUSED)
{
  g_clear_object (&f->proxy);

  if (f->object != NULL)
    {
      dbus_g_connection_unregister_g_object (f->bus, f->object);
      g_object_unref (f->object);
    }

  if (f->bus != NULL)
    {
      dbus_connection_close (dbus_g_connection_get_connection (f->bus));
      dbus_g_connection_unref (f->bus);
    }

  if (f->fd >= 0)
    {
      close (f->fd);
      f->fd = -1;
    }
}

static void
pending_call_store_reply (DBusPendingCall *pc,
    void *data)
{
  DBusMessage **message_p = data;

  g_assert (*message_p == NULL);
  *message_p = dbus_pending_call_steal_reply (pc);
  g_assert (*message_p != NULL);
}

/*
 * Exercise a call to a dbus-glib method with an unsupported parameter
 * type. The only unsupported parameter type we know is 'h', a Unix fd,
 * which is why this test is Unix-only.
 */
static void
test_call (Fixture *f,
    gconstpointer test_data G_GNUC_UNUSED)
{
  DBusConnection *libdbus = dbus_g_connection_get_connection (f->bus);
  const char *unique = dbus_bus_get_unique_name (libdbus);
  DBusMessage *m = dbus_message_new_method_call (unique,
      "/object", "org.freedesktop.DBus.GLib.Tests.MyObject",
      "EchoVariant");
  DBusPendingCall *pc;
  DBusMessageIter iter;
  DBusMessageIter var_iter;
  DBusMessage *reply = NULL;

  if (m == NULL)
    g_error ("OOM");

  dbus_message_iter_init_append (m, &iter);

  if (!dbus_message_iter_open_container (&iter, DBUS_TYPE_VARIANT,
        DBUS_TYPE_UNIX_FD_AS_STRING, &var_iter))
    g_error ("OOM");

  if (!dbus_message_iter_append_basic (&var_iter, DBUS_TYPE_UNIX_FD,
        &f->fd))
    {
      /* FIXME: cannot distinguish between inability to use fd and OOM */
      dbus_message_iter_abandon_container (&iter, &var_iter);
      dbus_message_unref (m);
      g_test_skip ("Unix fd-passing does not appear to be supported");
      return;
    }

  if (!dbus_message_iter_close_container (&iter, &var_iter))
    g_error ("OOM");

  if (!dbus_connection_send_with_reply (libdbus, m, &pc, -1) || pc == NULL)
    g_error ("OOM");

  if (dbus_pending_call_get_completed (pc))
    pending_call_store_reply (pc, &reply);
  else if (!dbus_pending_call_set_notify (pc, pending_call_store_reply,
        &reply, NULL))
    g_error ("OOM");

  dbus_message_unref (m);

  while (reply == NULL)
    g_main_context_iteration (NULL, TRUE);

  dbus_pending_call_unref (pc);

  /* it didn't work */
  /* this is what it should be */
  g_assert_cmpstr (dbus_message_get_error_name (reply), ==,
      DBUS_ERROR_INVALID_SIGNATURE);
  /* it didn't call the into the user-supplied C code */
  g_assert_cmpuint (MY_OBJECT (f->object)->echo_variant_called, ==, 0);

  dbus_message_unref (reply);
}

/*
 * Exercise a call from dbus-glib to a method returning an unsupported
 * parameter type. Again, the only unsupported parameter type we know is 'h'.
 */
static void
unregister_cb (DBusConnection *conn,
    void *user_data)
{
}

static DBusHandlerResult
message_cb (DBusConnection *conn,
    DBusMessage *m,
    void *user_data)
{
  Fixture *f = user_data;
  DBusMessage *reply = dbus_message_new_method_return (m);
  DBusMessageIter iter;
  DBusMessageIter var_iter;

  if (reply == NULL)
    g_error ("OOM");

  dbus_message_iter_init_append (reply, &iter);

  if (!dbus_message_iter_open_container (&iter, DBUS_TYPE_VARIANT,
        DBUS_TYPE_UNIX_FD_AS_STRING, &var_iter))
    g_error ("OOM");

  if (dbus_message_iter_append_basic (&var_iter, DBUS_TYPE_UNIX_FD,
        &f->fd))
    {
      if (!dbus_message_iter_close_container (&iter, &var_iter))
        g_error ("OOM");
    }
  else
    {
      /* FIXME: cannot distinguish between inability to use fd and OOM */
      dbus_message_iter_abandon_container (&iter, &var_iter);
      dbus_message_unref (reply);
      reply = dbus_message_new_error (m,
          DBUS_ERROR_NOT_SUPPORTED, "Unable to append Unix fd");

      if (reply == NULL)
        g_error ("OOM");
    }

  if (!dbus_connection_send (conn, reply, NULL))
    g_error ("OOM");

  return DBUS_HANDLER_RESULT_HANDLED;
}

static DBusObjectPathVTable vtable = {
    unregister_cb,
    message_cb,
    NULL,
    NULL,
    NULL,
    NULL
};

static void
test_ret (Fixture *f,
    gconstpointer test_data G_GNUC_UNUSED)
{
  DBusConnection *libdbus = dbus_g_connection_get_connection (f->bus);
  GError *error = NULL;
  GValue value;

  dbus_connection_register_object_path (libdbus, "/notdbusglib",
      &vtable, f);

  f->in_flight = 1;
  f->call = dbus_g_proxy_begin_call (f->proxy, "ReturnUnixFd",
      call_cb, f, NULL,
      G_TYPE_INVALID);

  while (f->in_flight > 0)
    g_main_context_iteration (NULL, TRUE);

  if (dbus_g_proxy_end_call (f->proxy, f->call, &error,
        G_TYPE_VALUE, &value,
        G_TYPE_INVALID))
    {
      g_error ("that shouldn't have worked!");
    }

  if (g_error_matches (error, DBUS_GERROR, DBUS_GERROR_NOT_SUPPORTED))
    {
      g_test_skip ("couldn't send a Unix fd");
    }
  else
    {
      g_assert_error (error, DBUS_GERROR, DBUS_GERROR_INVALID_ARGS);
    }

  g_clear_error (&error);
  dbus_connection_unregister_object_path (libdbus, "/notdbusglib");
}
#endif

int
main (int argc,
    char **argv)
{
  g_type_init ();
  dbus_g_type_specialized_init ();

  g_test_init (&argc, &argv, NULL);

#ifdef G_OS_UNIX
  g_test_add ("/unsupported-type/call",
      Fixture, NULL, setup, test_call, teardown);
  g_test_add ("/unsupported-type/ret",
      Fixture, NULL, setup, test_ret, teardown);
#endif

  return g_test_run ();
}
