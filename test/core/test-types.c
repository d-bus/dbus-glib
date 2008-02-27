#include <stdio.h>
#include <stdlib.h>
#include <dbus-glib.h>
#include <dbus-glib-lowlevel.h>

static void
lose (const char *str, ...)
{
  va_list args;

  va_start (args, str);

  vfprintf (stderr, str, args);
  fputc ('\n', stderr);

  va_end (args);

  exit (1);
}

int
main (int argc, char **argv)
{
  DBusError derror;
  GError *gerror = NULL;
  DBusGConnection *gconn, *gconn2;
  DBusConnection *conn;

  g_type_init ();
  dbus_error_init (&derror);

  /* Check plain DBusConnection -> DBusGConnection fails. */
  conn = dbus_bus_get (DBUS_BUS_SESSION, &derror);
  if (!conn)
    lose ("Cannot get connection: %s", derror.message);
  g_print ("(expecting assertion...)\n");
  gconn = dbus_connection_get_g_connection (conn);
  if (gconn)
    lose ("Retrieved DBusGConection from plain DBusConnection");


  /* Check DBusGConnection -> DBusConnection -> DBusGConnection */
  gconn = dbus_g_bus_get (DBUS_BUS_SESSION, &gerror);
  if (!gconn)
    lose ("Cannot get connection: %s", gerror->message);
  
  conn = dbus_g_connection_get_connection (gconn);
  if (!conn)
    lose ("Cannot get DBusConnection from DBusGConnection");

  gconn2 = dbus_connection_get_g_connection (conn);
  if (gconn != gconn2)
    lose ("Retrieved DBusGConection != original DBusGConnection");
  
  dbus_g_connection_unref (gconn);


  /* Check NULL -> DBusGConnection */
  g_print ("(expecting assertion...)\n");
  gconn = dbus_connection_get_g_connection (NULL);
  if (gconn)
    lose ("Retrieved DBusGConection from NULL");


  /* Check NULL -> DBusConnection */
  g_print ("(expecting assertion...)\n");
  conn = dbus_g_connection_get_connection (NULL);
  if (conn)
    lose ("Retrieved DBusConection from NULL");
  
  g_print ("\nTest completed.\n");
  
  return 0;
}
