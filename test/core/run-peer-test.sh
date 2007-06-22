#! /bin/sh

set -e

libtool --mode=execute ./peer-server &

libtool --mode=execute ./peer-client

kill %1
