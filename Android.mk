LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

DBUS_GLIB_BUILT_SOURCES := \
	dbus-glib-1.pc \
	dbus/Android.mk

dbus-glib-configure-real:
	cd $(DBUS_GLIB_TOP) ; \
	CC="$(CONFIGURE_CC)" \
	CFLAGS="$(CONFIGURE_CFLAGS)" \
	LD=$(TARGET_LD) \
	LDFLAGS="$(CONFIGURE_LDFLAGS)" \
	CPP=$(CONFIGURE_CPP) \
	CPPFLAGS="$(CONFIGURE_CPPFLAGS)" \
	PKG_CONFIG_LIBDIR="$(CONFIGURE_PKG_CONFIG_LIBDIR)" \
	PKG_CONFIG_TOP_BUILD_DIR=$(PKG_CONFIG_TOP_BUILD_DIR) \
	ac_cv_have_abstract_sockets=no \
	ac_cv_lib_expat_XML_ParserCreate_MM=yes \
	$(DBUS_GLIB_TOP)/$(CONFIGURE) --host=arm-linux-androideabi \
		--disable-nls --disable-gtk-doc && \
	for file in $(DBUS_GLIB_BUILT_SOURCES); do \
		rm -f $$file && \
		make -C $$(dirname $$file) $$(basename $$file) ; \
	done

dbus-glib-configure: dbus-glib-configure-real

.PHONY: dbus-glib-configure

CONFIGURE_TARGETS += dbus-glib-configure

#include all the subdirs...
-include $(DBUS_GLIB_TOP)/dbus/Android.mk
