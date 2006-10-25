#include <stdlib.h>
#include "test-song-bindings.h"
#include "test-hello-bindings.h"
#include "test-goodbye-bindings.h"

#define TEST_NAMESPACE "org.freedesktop.DBus.GLib.Test.Interfaces"
#define TEST_OBJECT_PATH "/org/freedesktop/DBus/GLib/Test/Interfaces"

int
main (int    argc,
      char **argv)
{
  DBusGConnection *connection;
  DBusGProxy *proxy;
  GError *error = NULL;
  gchar *str;
  gboolean success;

  g_type_init ();

  connection = dbus_g_bus_get (DBUS_BUS_SESSION, &error);
  if (connection == NULL)
    {
      g_error ("Failed to make connection to session bus: %s", error->message);
      g_error_free (error);
      exit(1);
    }

  proxy = dbus_g_proxy_new_for_name (connection, TEST_NAMESPACE, TEST_OBJECT_PATH,
                                     "org.freedesktop.DBus.GLib.Test.Interfaces.Song");
  success = org_freedesktop_DBus_GLib_Test_Interfaces_Song_get_title (proxy, &str, &error);
  g_object_unref (proxy);

  if (!success)
    {
      g_print ("Error while calling Parent object method: %s\n", error->message);
      g_error_free (error);
      exit(1);
    }
  else
    {
      g_free (str);
      g_print ("Called Parent object method with success\n");
    }

  proxy = dbus_g_proxy_new_for_name (connection, TEST_NAMESPACE, TEST_OBJECT_PATH,
                                     "org.freedesktop.DBus.GLib.Test.Interfaces.Hello");
  g_assert (proxy != NULL);
  success = org_freedesktop_DBus_GLib_Test_Interfaces_Hello_say_hello (proxy, &str, &error);
  g_object_unref (proxy);

  if (!success)
    {
      g_print ("Error while calling Parent Interface object method: %s\n", error->message);
      g_error_free (error);
      exit(1);
    }
  else
    {
      g_free (str);
      g_print ("Called Parent Interface object method with success\n");
    }

  proxy = dbus_g_proxy_new_for_name (connection, TEST_NAMESPACE, TEST_OBJECT_PATH,
                                     "org.freedesktop.DBus.GLib.Test.Interfaces.Goodbye");
  success = org_freedesktop_DBus_GLib_Test_Interfaces_Goodbye_say_goodbye (proxy, &str, &error);
  g_object_unref (proxy);

  if (!success)
    {
      g_print ("Error while calling Object Interface object method: %s\n", error->message);
      g_error_free (error);
      exit(1);
    }
  else
    {
      g_free (str);
      g_print ("Called Object Interface object method with success\n");
    }

  exit(0);
}

/* ex:ts=2:et: */

