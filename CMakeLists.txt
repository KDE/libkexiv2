# ===========================================================
#
# This file is a part of digiKam project
# <a href="http://www.digikam.org">http://www.digikam.org</a>
#
# @date   2006-09-15
# @brief  Exiv2 library interface for KDE
#
# @author Copyright (C) 2006-2010 by Gilles Caulier
#         <a href="mailto:caulier dot gilles at gmail dot com">caulier dot gilles at gmail dot com</a>
#
# This program is free software; you can redistribute it
# and/or modify it under the terms of the GNU General
# Public License as published by the Free Software Foundation;
# either version 2, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# ============================================================

PROJECT(libkexiv2)

FIND_PACKAGE(KDE4 REQUIRED)

INCLUDE(KDE4Defaults)
INCLUDE(MacroLibrary)
INCLUDE(MacroOptionalAddSubdirectory)
INCLUDE(MacroOptionalFindPackage)
INCLUDE(FindPackageHandleStandardArgs)

ADD_DEFINITIONS (${QT_DEFINITIONS} ${QT_QTDBUS_DEFINITIONS} ${KDE4_DEFINITIONS})
INCLUDE_DIRECTORIES (${QDBUS_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES})

SET(LIBKEXIV2_AREA_CODE_GENERAL 51003)
ADD_DEFINITIONS(-DKDE_DEFAULT_DEBUG_AREA=${LIBKEXIV2_AREA_CODE_GENERAL})

SET(EXIV2_MIN_VERSION "0.20")
MACRO_OPTIONAL_FIND_PACKAGE(Exiv2)
MACRO_LOG_FEATURE(EXIV2_FOUND "Exiv2" "A library to access image metadata" "http://www.exiv2.org"
                  FALSE ${EXIV2_MIN_VERSION} "Required to build libkexiv2.")

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
# 1.2.0 => 9.0.0     (Released with KDE 4.6.0 - Including XMP sidecar support)
# 2.0.0 => 10.0.0    (included into digiKam 2.0.0 collection)

# Library API version
SET(KEXIV2_LIB_MAJOR_VERSION "2")
SET(KEXIV2_LIB_MINOR_VERSION "0")
SET(KEXIV2_LIB_PATCH_VERSION "0")

# Suffix to add at end of version string. Usual values are:
# "-svn"   : alpha code unstable from svn. Do not use in production
# "-beta1" : beta1 release.
# "-beta2" : beta2 release.
# "-beta3" : beta3 release.
# "-rc"    : release candidate.
# ""       : final relase. Can be used in production.
SET(KEXIV2_LIB_SUFFIX_VERSION "")

# Library ABI version used by linker.
# For details : http://www.gnu.org/software/libtool/manual/libtool.html#Updating-version-info
SET(KEXIV2_LIB_SO_CUR_VERSION "10")
SET(KEXIV2_LIB_SO_REV_VERSION "0")
SET(KEXIV2_LIB_SO_AGE_VERSION "0")

# =======================================================
# Set env. variables accordinly.

SET(KEXIV2_LIB_VERSION_STRING "${KEXIV2_LIB_MAJOR_VERSION}.${KEXIV2_LIB_MINOR_VERSION}.${KEXIV2_LIB_PATCH_VERSION}${KEXIV2_LIB_SUFFIX_VERSION}")
SET(KEXIV2_LIB_VERSION_ID "0x0${KEXIV2_LIB_MAJOR_VERSION}0${KEXIV2_LIB_MINOR_VERSION}0${KEXIV2_LIB_PATCH_VERSION}")
SET(KEXIV2_LIB_SO_VERSION_STRING "${KEXIV2_LIB_SO_CUR_VERSION}.${KEXIV2_LIB_SO_REV_VERSION}.${KEXIV2_LIB_SO_AGE_VERSION}")

# =======================================================

IF (EXIV2_FOUND)
  SUBDIRS(libkexiv2)
  SUBDIRS(test)

  IF(NOT WIN32)
    CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/libkexiv2.pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/libkexiv2.pc)
    CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/libkexiv2.lsm.cmake ${CMAKE_CURRENT_BINARY_DIR}/libkexiv2.lsm)
    INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/libkexiv2.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig )
  ENDIF(NOT WIN32)
ENDIF (EXIV2_FOUND)
