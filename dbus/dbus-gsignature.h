/* -*- mode: C; c-file-style: "gnu" -*- */
/*
 * Copyright (C) 2005-2006 Red Hat, Inc.
 *
 * SPDX-License-Identifier: AFL-2.1 OR GPL-2.0-or-later
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
 */

#ifndef DBUS_GOBJECT_SIGNATURE_H
#define DBUS_GOBJECT_SIGNATURE_H

#include <dbus/dbus.h>
#include <dbus/dbus-signature.h>
#include <glib.h>

GType          _dbus_gtype_from_basic_typecode (int typecode);

GType          _dbus_gtype_from_signature      (const char              *signature,
					       gboolean                 is_client);

GType          _dbus_gtype_from_signature_iter (DBusSignatureIter       *sigiter,
					       gboolean                 is_client);

GArray *       _dbus_gtypes_from_arg_signature (const char              *signature,
						gboolean                 is_client);

#endif
