#! /bin/sh

set -e

libtool --mode=execute ./peer-server &

sleep 1

libtool --mode=execute ./peer-client
