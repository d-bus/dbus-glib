INCLUDES=-I$(top_srcdir) $(DBUS_CFLAGS) $(DBUS_GLIB_CFLAGS) $(DBUS_X_CFLAGS) $(DBUS_GTK_THREADS_CFLAGS) -DDBUS_COMPILATION

nodist_libdbus_glib_HEADERS = dbus-glib-bindings.h
libdbus_glibdir = $(includedir)/dbus-1.0/dbus

dbus-glib-bindings.h: dbus-bus-introspect.xml $(top_builddir)/dbus/dbus-binding-tool$(EXEEXT)
	$(DBUS_BINDING_TOOL) --mode=glib-client --prefix=dbus_bus --output=dbus-glib-bindings.h dbus-bus-introspect.xml

BUILT_SOURCES = dbus-glib-bindings.h dbus-bus-introspect.xml

if USE_INTROSPECT_XML
dbus-bus-introspect.xml: $(INTROSPECT_XML_PATH)
	cp $(INTROSPECT_XML_PATH) dbus-bus-introspect.xml
else
dbus-bus-introspect.xml:
	DBUS_TOP_BUILDDIR=$(top_builddir) @DBUS_DAEMONDIR@dbus-daemon --introspect > dbus-bus-introspect.xml.tmp && mv dbus-bus-introspect.xml.tmp dbus-bus-introspect.xml
endif

EXTRA_DIST = run-with-tmp-session-bus.sh session.conf

CLEANFILES = 				\
	run-with-tmp-session-bus.conf   \
        dbus-bus-introspect.xml         \
        dbus-glib-bindings.h
