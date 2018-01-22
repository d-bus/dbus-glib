AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_builddir) \
	$(DBUS_CFLAGS) \
	$(DBUS_GLIB_CFLAGS) \
	$(NULL)

noinst_LTLIBRARIES = libdbus-gmain.la

libdbus_gmain_la_SOURCES = \
	dbus-gmain.c \
	dbus-gmain.h \
	$(NULL)

libdbus_gmain_la_LIBADD = $(DBUS_LIBS) $(DBUS_GLIB_LIBS)
libdbus_gmain_la_LDFLAGS = -no-undefined
