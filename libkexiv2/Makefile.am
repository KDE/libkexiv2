METASOURCES = AUTO
KDE_CXXFLAGS = $(USE_EXCEPTIONS) 

INCLUDES= $(EXIV2_CFLAGS) $(all_includes)

lib_LTLIBRARIES = libkexiv2.la

libkexiv2_la_SOURCES = kexiv2.cpp kexiv2private.cpp

libkexiv2_version_info = 5:0:0
libkexiv2_la_LDFLAGS = $(all_libraries) $(KDE_RPATH) -version-info $(libkexiv2_version_info) -no-undefined
libkexiv2_la_LIBADD  = $(EXIV2_LIBS) $(LIB_KDECORE) $(LIB_QT)

libkexiv2include_HEADERS = kexiv2.h libkexiv2_export.h version.h 

libkexiv2includedir = $(includedir)/libkexiv2

