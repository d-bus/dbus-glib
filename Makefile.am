ACLOCAL_AMFLAGS = -I m4
GLIB_PC=dbus-glib-1.pc

SUBDIRS=dbus tools test doc
DIST_SUBDIRS=dbus tools test doc m4

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

MAINTAINERCLEANFILES = ChangeLog

DISTCHECK_CONFIGURE_FLAGS=--enable-gtk-doc --enable-checks --enable-tests
EXTRA_DIST += ChangeLog

ChangeLog:
$(srcdir)/ChangeLog: FORCE
	@if test -d "$(srcdir)/.git"; then \
	  (cd "$(srcdir)" && \
	  ./missing --run git log --stat) | fmt --split-only > $@.tmp \
	  && mv -f $@.tmp $@ \
	  || ($(RM) $@.tmp; \
	      echo Failed to generate ChangeLog, your ChangeLog may be outdated >&2; \
	      (test -f $@ || echo git-log is required to generate this file >> $@)); \
	else \
	  test -f $@ || \
	  (echo A git checkout and git log are required to generate ChangeLog >&2 && \
	  echo A git checkout and git-log are required to generate this file >> $@); \
	fi

all-local: Doxyfile

if DBUS_GCOV_ENABLED
clean-gcov:
	find -name "*.da" -o -name "*.gcov" | xargs rm || true

clean-bbg:
	find -name "*.bbg" -o -name "*.bb" | xargs rm || true

GCOV_DIRS=$(GLIB_SUBDIR)

coverage-report.txt: FORCE
	echo "we just don't know" > $@

check-coverage: clean-gcov all check coverage-report.txt
	cat coverage-report.txt

else
coverage-report.txt:
	echo "Need to reconfigure with --enable-gcov"

check-coverage:
	echo "Need to reconfigure with --enable-gcov"

endif

FORCE:

