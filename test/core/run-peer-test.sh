#! /bin/sh

set -e

${DBUS_TOP_BUILDDIR}/libtool --mode=execute ./peer-server &

sleep 1

${DBUS_TOP_BUILDDIR}/libtool --mode=execute ./peer-client
