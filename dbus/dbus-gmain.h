/* -*- mode: C; c-file-style: "gnu" -*- */
/* dbus-gmain.h — GLib main loop integration for libdbus
 *
 * Copyright (C) 2002, 2003  CodeFactory AB
 * Copyright (C) 2003, 2004 Red Hat, Inc.
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
#ifndef DBUS_GMAIN_H
#define DBUS_GMAIN_H

#include <dbus/dbus.h>
#include <glib.h>

G_BEGIN_DECLS

void            dbus_connection_setup_with_g_main (DBusConnection  *connection,
                                                   GMainContext    *context);
void            dbus_server_setup_with_g_main     (DBusServer      *server,
                                                   GMainContext    *context);

G_END_DECLS

#endif /* DBUS_GLIB_LOWLEVEL_H */




