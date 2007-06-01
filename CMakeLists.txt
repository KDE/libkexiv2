PROJECT(libkexiv2)

SET(CMAKE_VERBOSE_MAKEFILE ON)

FIND_PACKAGE(Exiv2 REQUIRED)
FIND_PACKAGE(KDE3 REQUIRED)


ADD_DEFINITIONS(${QT_DEFINITIONS} ${KDE3_DEFINITIONS} ${EXIV2_DEFINITIONS})

LINK_DIRECTORIES(${KDE3_LIB_DIR})

INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${KDE3_INCLUDE_DIR} ${QT_INCLUDE_DIR} ${EXIV2_INCLUDE_DIR} )

SET( CMAKE_MODULE_PATH    ${CMAKE_MODULE_PATH}
                          ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules )

########### next target ###############

SET(kexiv2_LIB_SRCS
   kexiv2.cpp
   )

KDE3_AUTOMOC(${kexiv2_LIB_SRCS})

ADD_LIBRARY(kexiv2 SHARED ${kexiv2_LIB_SRCS})

TARGET_LINK_LIBRARIES(kexiv2 ${QT_AND_KDECORE_LIBS} exiv2)

SET_TARGET_PROPERTIES(kexiv2 PROPERTIES VERSION 2.1.0 SOVERSION 2 )
INSTALL(TARGETS kexiv2 DESTINATION lib} )

########### install files ###############

INSTALL_FILES( FILES  libkexiv2.pc )
INSTALL_FILES( /include/libkexiv2 FILES kexiv2.h libkexiv2_export.h version.h )



#original Makefile.am contents follow:

#METASOURCES = AUTO
#KDE_CXXFLAGS = $(USE_EXCEPTIONS) 
#
#INCLUDES= $(all_includes)
#
#lib_LTLIBRARIES = libkexiv2.la
#
#libkexiv2_la_SOURCES = kexiv2.cpp
#
#libexiv2_version_info = 2:1:1
#libkexiv2_la_LDFLAGS = $(all_libraries) $(KDE_RPATH) -version-info $(libexiv2_version_info) -no-undefined
#libkexiv2_la_LIBADD  = $(EXIV2_LIBS) $(LIB_KDECORE) $(LIB_QT)
#
#libkexiv2include_HEADERS = kexiv2.h libkexiv2_export.h version.h 
#
#libkexiv2includedir = $(includedir)/libkexiv2
#
#pkgconfigdir = $(libdir)/pkgconfig
#pkgconfig_DATA = libkexiv2.pc
#EXTRA_DIST = libkexiv2.pc.in
#
