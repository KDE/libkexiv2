project(libkexiv2)

include_directories(${KDE4_INCLUDES} ${QT4_INCLUDES})

add_definitions(${KDE4_ENABLE_EXCEPTIONS})

set(CMAKE_VERBOSE_MAKEFILE ON)

# FIXME: Detect Exiv2 library properlly with CMake script from 'cmake/modules'
#
SET( CMAKE_MODULE_PATH    ${CMAKE_MODULE_PATH}  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules )
FIND_PACKAGE(Exiv2 REQUIRED)

########### next target ###############

set(kexiv2_LIB_SRCS kexiv2.cpp)

kde4_automoc(${kexiv2_LIB_SRCS})

kde4_add_library(kexiv2 SHARED ${kexiv2_LIB_SRCS})

target_link_libraries(kexiv2 ${KDE4_KDECORE_LIBS} exiv2)

set_target_properties(kexiv2 PROPERTIES VERSION 2.1.0 SOVERSION 2 )

install(TARGETS kexiv2 DESTINATION ${LIB_INSTALL_DIR} )

########### install files ###############

install(FILES kexiv2.h libkexiv2_export.h version.h DESTINATION ${INCLUDES_INSTALL_DIR}/libkexiv2)

