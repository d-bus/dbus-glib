ACLOCAL_AMFLAGS = -I m4
GLIB_PC=dbus-glib-1.pc

SUBDIRS = dbus-gmain dbus tools test doc
DIST_SUBDIRS = dbus-gmain dbus tools test doc m4

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = $(GLIB_PC)

DISTCLEANFILES = 		\
	$(GLIB_PC)

EXTRA_DIST =			\
	HACKING			\
	NEWS			\
	dbus-bus-introspect.xml \
	dbus-glib-1.pc.in

# Creating ChangeLog from git log:

DISTCHECK_CONFIGURE_FLAGS=--enable-gtk-doc --enable-checks --enable-tests

%.tar.gz.asc: %.tar.gz
	$(AM_V_GEN)gpg --detach-sign --armor $@

maintainer-upload-release:
	test -f @PACKAGE@-@VERSION@.tar.gz
	test -f @PACKAGE@-@VERSION@.tar.gz.asc
	gpg --verify @PACKAGE@-@VERSION@.tar.gz.asc
	rsync -vzP @PACKAGE@-@VERSION@.tar.gz dbus.freedesktop.org:/srv/dbus.freedesktop.org/www/releases/@PACKAGE@/@PACKAGE@-@VERSION@.tar.gz
	rsync -vzP @PACKAGE@-@VERSION@.tar.gz.asc dbus.freedesktop.org:/srv/dbus.freedesktop.org/www/releases/@PACKAGE@/@PACKAGE@-@VERSION@.tar.gz.asc
	rsync -rvzPp --chmod=Dg+s,ug+rwX,o=rX doc/reference/html/ \
	dbus.freedesktop.org:/srv/dbus.freedesktop.org/www/doc/@PACKAGE@/

check-valgrind: all
	$(MAKE) -C test check-valgrind

include tools/lcov.am
