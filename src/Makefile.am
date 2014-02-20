lib_LTLIBRARIES = libprogresstar.la
libprogresstar_la_SOURCES = progresstar.c fileutil.c optparse.c
bin_PROGRAMS = ptar
ptar_SOURCES = ptar.c
ptar_LDADD = libprogresstar.la

AM_CPPFLAGS = $(zlib_CFLAGS) $(libarchive_CFLAGS)
libprogresstar_la_LIBADD = $(zlib_LIBS) $(libarchive_LIBS)
