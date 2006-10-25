#! /bin/bash

SCRIPTNAME=$0
MODE=$1

## so the tests can complain if you fail to use the script to launch them
export DBUS_TEST_GLIB_RUN_TEST_SCRIPT=1

export DBUS_TOP_SRCDIR=$(dirname "$0")/../..

# Rerun ourselves with tmp session bus if we're not already
if test -z "$DBUS_TEST_GLIB_IN_RUN_TEST"; then
  DBUS_TEST_GLIB_IN_RUN_TEST=1
  export DBUS_TEST_GLIB_IN_RUN_TEST
  exec $DBUS_TOP_SRCDIR/tools/run-with-tmp-session-bus.sh $SCRIPTNAME $MODE
fi  

echo "running test-client"
libtool --mode=execute $DEBUG $DBUS_TOP_BUILDDIR/test/interfaces/test-client || die "test-client failed"
