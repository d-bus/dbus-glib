#!/bin/bash

# Copyright © 2015-2018 Collabora Ltd.
#
# Permission is hereby granted, free of charge, to any person
# obtaining a copy of this software and associated documentation files
# (the "Software"), to deal in the Software without restriction,
# including without limitation the rights to use, copy, modify, merge,
# publish, distribute, sublicense, and/or sell copies of the Software,
# and to permit persons to whom the Software is furnished to do so,
# subject to the following conditions:
#
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
# BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
# ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

set -euo pipefail
set -x

NULL=

# ci_buildsys:
# Build system under test: autotools is the only option right now
: "${ci_buildsys:=autotools}"

# ci_docker:
# If non-empty, this is the name of a Docker image. ci-install.sh will
# fetch it with "docker pull" and use it as a base for a new Docker image
# named "ci-image" in which we will do our testing.
#
# If empty, we test on "bare metal".
# Typical values: ubuntu:xenial, debian:jessie-slim
: "${ci_docker:=}"

# ci_host:
# See ci-install.sh
: "${ci_host:=native}"

# ci_parallel:
# A number of parallel jobs, passed to make -j
: "${ci_parallel:=1}"

# ci_sudo:
# If yes, assume we can get root using sudo; if no, only use current user
: "${ci_sudo:=no}"

# ci_test:
# If yes, run tests; if no, just build
: "${ci_test:=yes}"

# ci_test_fatal:
# If yes, test failures break the build; if no, they are reported but ignored
: "${ci_test_fatal:=yes}"

# ci_variant:
# One of debug, reduced, legacy, production
: "${ci_variant:=production}"

if [ -n "$ci_docker" ]; then
    exec docker run \
        --env=ci_buildsys="${ci_buildsys}" \
        --env=ci_docker="" \
        --env=ci_host="${ci_host}" \
        --env=ci_parallel="${ci_parallel}" \
        --env=ci_sudo=yes \
        --env=ci_test="${ci_test}" \
        --env=ci_test_fatal="${ci_test_fatal}" \
        --env=ci_variant="${ci_variant}" \
        --privileged \
        ci-image \
        tools/ci-build.sh
fi

maybe_fail_tests () {
    if [ "$ci_test_fatal" = yes ]; then
        exit 1
    fi
}

NOCONFIGURE=1 ./autogen.sh

srcdir="$(pwd)"
mkdir ci-build-${ci_variant}-${ci_host}
cd ci-build-${ci_variant}-${ci_host}

make="make -j${ci_parallel} V=1 VERBOSE=1"

case "$ci_buildsys" in
    (autotools)
        case "$ci_variant" in
            (debug)
                set _ "$@"
                set "$@" --enable-tests
                shift
                ;;

            (*)
                ;;
        esac

        ../configure \
            --enable-installed-tests \
            --enable-maintainer-mode \
            "$@"

        ${make}
        [ "$ci_test" = no ] || ${make} check || maybe_fail_tests
        cat test/test-suite.log || :
        [ "$ci_test" = no ] || ${make} distcheck || maybe_fail_tests

        ${make} install DESTDIR=$(pwd)/DESTDIR
        ( cd DESTDIR && find . )

        if [ "$ci_sudo" = yes ] && [ "$ci_test" = yes ]; then
            sudo ${make} install
        fi
        ;;
esac

# vim:set sw=4 sts=4 et:
