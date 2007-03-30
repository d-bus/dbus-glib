#! /bin/sh

set -e

libtool --mode=execute ./peer-server &

libtool --mode=execute ./peer-client $ADDRESS

kill %1
