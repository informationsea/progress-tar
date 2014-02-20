EXTRA_DIST = run-test.sh testtar.tar.gz compare-zip.py

if WITH_CUTTER
TESTS = run-test.sh
TESTS_ENVIRONMENT = CUTTER="$(CUTTER)"

AM_LDFLAGS = -module -rpath $(libdir) -avoid-version -no-undefined
AM_CPPFLAGS = $(GCUTTER_CFLAGS) -O0 -g -I$(top_builddir)/src $(zlib_CFLAGS) $(libarchive_CFLAGS) -DTOPDIR="$(top_builddir)"
LIBS += $(GCUTTER_LIBS) $(top_builddir)/src/libprogresstar.la $(zlib_LIBS) $(libarchive_LIBS)

noinst_LTLIBRARIES = libmaketar.la libutiltest.la
libmaketar_la_SOURCES = maketar.c 
libutiltest_la_SOURCES = test-util.c test-optparse.c

echo-cutter:
	@echo $(CUTTER)

endif
