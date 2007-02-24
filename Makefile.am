METASOURCES = AUTO
KDE_CXXFLAGS = $(USE_EXCEPTIONS) 

INCLUDES= $(all_includes)

lib_LTLIBRARIES = libkexiv2.la

libkexiv2_la_SOURCES = kexiv2.cpp

libkexiv2_la_LDFLAGS = $(all_libraries) $(KDE_RPATH) -version-info 0:2:0 -no-undefined
libkexiv2_la_LIBADD  = $(EXIV2_LIBS) $(LIB_KDECORE) $(LIB_QT)

libkexiv2include_HEADERS = kexiv2.h libkexiv2_export.h version.h 

libkexiv2includedir = $(includedir)/libkexiv2

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libkexiv2.pc
EXTRA_DIST = libkexiv2.pc.in

