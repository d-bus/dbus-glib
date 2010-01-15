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
	BBG_FILES=`find $(GCOV_DIRS) -name "*.bbg" -o -name "*.gcno"` ;			\
	C_FILES= ;									\
	for F in $$BBG_FILES ; do							\
		F_nolibs=`echo $$F | sed -e 's/.libs\///g'` ;				\
		C=`echo $$F_nolibs | sed -e 's/.bbg/.c/g' | sed -e 's/.gcno/.c/g'`  ;	\
		B=`basename $$F .bbg` ;							\
		D=`dirname $$F` ;							\
		DA=`echo $$F | sed -e 's/.bbg/.da/g'` ;					\
		DA_libs=`echo $$D/.libs/$$B/.da` ;					\
		if test -e $$DA || test -e $$DA_libs; then				\
			C_FILES="$$C_FILES $$C" ;					\
		fi ;									\
	done ;										\
	echo $$C_FILES ;								\
	$(top_builddir)/test/decode-gcov --report $$C_FILES > coverage-report.txt

check-coverage: clean-gcov all check coverage-report.txt
	cat coverage-report.txt

else
coverage-report.txt:
	echo "Need to reconfigure with --enable-gcov"

check-coverage:
	echo "Need to reconfigure with --enable-gcov"

endif

FORCE:

