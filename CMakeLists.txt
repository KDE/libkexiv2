#
# Copyright (c) 2010-2015, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

cmake_minimum_required(VERSION 2.8.9)
project(libkexiv2)

message(STATUS "----------------------------------------------------------------------------------")
message(STATUS "Starting CMake configuration for: libkexiv2")

find_package(KDE4 REQUIRED)

include(KDE4Defaults)
include(MacroLibrary)
include(MacroOptionalAddSubdirectory)
include(MacroOptionalFindPackage)
include(FindPackageHandleStandardArgs)

add_definitions(${QT_DEFINITIONS} ${QT_QTDBUS_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(${QDBUS_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES})

set(LIBKEXIV2_AREA_CODE_GENERAL 51003)
add_definitions(-DKDE_DEFAULT_DEBUG_AREA=${LIBKEXIV2_AREA_CODE_GENERAL})

set(EXIV2_MIN_VERSION "0.24")
find_package(Exiv2)
MACRO_LOG_FEATURE(EXIV2_FOUND "Exiv2" "Required to build libkexiv2." "http://www.exiv2.org"
                  TRUE ${EXIV2_MIN_VERSION} "")

# =======================================================
# Information to update before to release this library.

# Library version history:
# API      ABI
# 0.1.0 => 0.1.0
# 0.1.1 => 0.2.0
# 0.1.2 => 1.0.1
# 0.1.3 => 1.0.1
# 0.1.4 => 2.0.2
# 0.1.5 => 2.1.1
# 0.1.6 => 3.0.0
# 0.1.7 => 4.0.1
# 0.1.8 => 5.0.0
# 0.2.0 => 6.0.0     (released with KDE 4.1.0)
# 0.3.0 => 7.0.0     (released with KDE 4.1.2)
# 0.4.0 => 7.1.0
# 0.5.0 => 7.2.0     (Released with KDE 4.2.0)
# 0.6.0 => 7.3.0     (Released with KDE 4.3.0)
# 1.0.0 => 8.0.0     (Released with KDE 4.4.0)
# 1.1.0 => 8.1.0     (Released with KDE 4.5.0)
# 1.2.0 => 9.0.0     (Released with KDE 4.6.0  - Including XMP sidecar support
# 2.0.0 => 10.0.0    (Released with KDE 4.7.0)
# 2.1.0 => 10.0.1    (Released with KDE 4.7.1) - Add AltLangStrEdit visible lines API
# 2.1.1 => 10.0.2    (Released with KDE 4.7.4) - Add AltLangStrEdit::setCurrentLanguageCode()
# 2.2.0 => 11.0.0    (Released with KDE 4.8.1) - Remove deprecated methods
# 2.3.0 => 11.1.0    (Released with KDE 4.8.2) - Add new static methods about XMP sidecar file management.
# 2.3.1 => 11.2.0                              - Add new method to set specific XMP tag string
# 2.4.0 => 11.3.0                              - Add new method to access on text edit widget from AltLangStrEdit.

# Library API version
set(KEXIV2_LIB_MAJOR_VERSION "2")
set(KEXIV2_LIB_MINOR_VERSION "4")
set(KEXIV2_LIB_PATCH_VERSION "0")

# Suffix to add at end of version string. Usual values are:
# "-git"   : alpha code unstable from git. Do not use in production
# "-beta1" : beta1 release.
# "-beta2" : beta2 release.
# "-beta3" : beta3 release.
# "-rc"    : release candidate.
# ""       : final relase. Can be used in production.
set(KEXIV2_LIB_SUFFIX_VERSION "")

# Library ABI version used by linker.
# For details : http://www.gnu.org/software/libtool/manual/libtool.html#Updating-version-info
set(KEXIV2_LIB_SO_CUR_VERSION "11")
set(KEXIV2_LIB_SO_REV_VERSION "3")
set(KEXIV2_LIB_SO_AGE_VERSION "0")

# =======================================================
# Set env. variables accordinly.

set(KEXIV2_LIB_VERSION_STRING    "${KEXIV2_LIB_MAJOR_VERSION}.${KEXIV2_LIB_MINOR_VERSION}.${KEXIV2_LIB_PATCH_VERSION}${KEXIV2_LIB_SUFFIX_VERSION}")
set(KEXIV2_LIB_VERSION_ID        "0x0${KEXIV2_LIB_MAJOR_VERSION}0${KEXIV2_LIB_MINOR_VERSION}0${KEXIV2_LIB_PATCH_VERSION}")
set(KEXIV2_LIB_SO_VERSION_STRING "${KEXIV2_LIB_SO_CUR_VERSION}.${KEXIV2_LIB_SO_REV_VERSION}.${KEXIV2_LIB_SO_AGE_VERSION}")

# =======================================================

if(EXIV2_FOUND)

    add_subdirectory(libkexiv2)

    if(KDE4_BUILD_TESTS)
        MACRO_OPTIONAL_ADD_SUBDIRECTORY(tests)
    endif()

    include(CMakePackageConfigHelpers)

    CONFIGURE_PACKAGE_CONFIG_FILE(${CMAKE_CURRENT_SOURCE_DIR}/cmake/templates/libkexiv2-config.cmake.in libkexiv2-config.cmake
                                  INSTALL_DESTINATION "${LIB_INSTALL_DIR}/cmake/libkexiv2-${KEXIV2_LIB_VERSION_STRING}"
                                  PATH_VARS INCLUDE_INSTALL_DIR
                                  NO_CHECK_REQUIRED_COMPONENTS_MACRO)

    WRITE_BASIC_PACKAGE_VERSION_FILE(libkexiv2-config-version.cmake
                                     VERSION "${KEXIV2_LIB_MAJOR_VERSION}.${KEXIV2_LIB_MINOR_VERSION}.${KEXIV2_LIB_PATCH_VERSION}"
                                     COMPATIBILITY SameMajorVersion)
    install(EXPORT Libkexiv2
            DESTINATION "${LIB_INSTALL_DIR}/cmake/libkexiv2-${KEXIV2_LIB_VERSION_STRING}"
            NAMESPACE Libkexiv2::
            FILE libkexiv2-export.cmake)

    install(FILES
            ${CMAKE_CURRENT_BINARY_DIR}/libkexiv2-config.cmake
            ${CMAKE_CURRENT_BINARY_DIR}/libkexiv2-config-version.cmake
            DESTINATION
            "${LIB_INSTALL_DIR}/cmake/libkexiv2-${KEXIV2_LIB_VERSION_STRING}")

endif()

MACRO_DISPLAY_FEATURE_LOG()
