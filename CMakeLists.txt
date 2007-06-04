project(libkexiv2)

include_directories(${KDE4_INCLUDES} ${QT4_INCLUDES})

add_definitions(${KDE4_ENABLE_EXCEPTIONS})

set(CMAKE_VERBOSE_MAKEFILE ON)

link_directories(${KDE4_LIB_DIR})


# FIXME: Detect Exiv2 library properlly with CMake script from 'cmake/modules'
#
#FIND_PACKAGE(Exiv2 REQUIRED)
#
#SET( CMAKE_MODULE_PATH    ${CMAKE_MODULE_PATH}
#                          ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules )

########### next target ###############

set(kexiv2_LIB_SRCS kexiv2.cpp)

kde4_automoc(${kexiv2_LIB_SRCS})

kde4_add_library(kexiv2 SHARED ${kexiv2_LIB_SRCS})

target_link_libraries(kexiv2 ${KDE4_KDECORE_LIBS} exiv2)

set_target_properties(kexiv2 PROPERTIES VERSION 2.1.0 SOVERSION 2 )

install(TARGETS kexiv2 DESTINATION ${LIB_INSTALL_DIR}} )

########### install files ###############

install(FILES kexiv2.h libkexiv2_export.h version.h DESTINATION ${INCLUDES_INSTALL_DIR}/libkexiv2)


####################################################################################################"

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
