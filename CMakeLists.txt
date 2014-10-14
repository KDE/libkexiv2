# ===========================================================
#
# This file is a part of digiKam project
# <a href="http://www.digikam.org">http://www.digikam.org</a>
#
# @date   2006-09-15
# @brief  Exiv2 library interface for KDE
#
# @author Copyright (C) 2006-2014 by Gilles Caulier
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

set(CMAKE_MIN_VERSION   "2.8.12")
set(ECM_MIN_VERSION     "1.1.0")
set(KF5_VERSION         "5.0.0")
set(REQUIRED_QT_VERSION "5.2.0")
set(EXIV2_MIN_VERSION   "0.24")

cmake_minimum_required(VERSION ${CMAKE_MIN_VERSION})

project(libkexiv2)

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
# 1.2.0 => 9.0.0     (Released with KDE 4.6.0) - Including XMP sidecar support
# 2.0.0 => 10.0.0    (Released with KDE 4.7.0)
# 2.1.0 => 10.0.1    (Released with KDE 4.7.1) - Add AltLangStrEdit visible lines API
# 2.1.1 => 10.0.2    (Released with KDE 4.7.4) - Add AltLangStrEdit::setCurrentLanguageCode()
# 2.2.0 => 11.0.0    (Released with KDE 4.8.1) - Remove deprecated methods
# 2.3.0 => 11.1.0    (Released with KDE 4.8.2) - Add new static methods about XMP sidecar file management.
# 2.3.1 => 11.2.0                              - Add new method to set specific XMP tag string
# 2.3.2 => 11.3.0                              - Add new method to access on text edit widget from AltLangStrEdit
# 3.0.0 => 15.0.0    (Released with KDE 5.x.0)

# Library API version
set(KEXIV2_LIB_MAJOR_VERSION "2")
set(KEXIV2_LIB_MINOR_VERSION "7")
set(KEXIV2_LIB_PATCH_VERSION "0")

# Library ABI version used by linker.
# For details : http://www.gnu.org/software/libtool/manual/libtool.html#Updating-version-info
set(KEXIV2_LIB_SO_CUR_VERSION "15")
set(KEXIV2_LIB_SO_REV_VERSION "0")
set(KEXIV2_LIB_SO_AGE_VERSION "0")

set(LIBKEXIV2_LIB_VERSION "${KEXIV2_LIB_MAJOR_VERSION}.${KEXIV2_LIB_MINOR_VERSION}.${KEXIV2_LIB_PATCH_VERSION}")
set(LIBKEXIV2_SO_VERSION  "${KEXIV2_LIB_SO_CUR_VERSION}.${KEXIV2_LIB_SO_REV_VERSION}.${KEXIV2_LIB_SO_AGE_VERSION}")

############## ECM setup ######################

find_package(ECM ${ECM_MIN_VERSION} CONFIG REQUIRED)
set(CMAKE_MODULE_PATH ${libkexiv2_SOURCE_DIR}/cmake/modules ${ECM_MODULE_PATH})

include(ECMGenerateHeaders)
include(ECMGeneratePriFile)
include(ECMPackageConfigHelpers)
include(ECMSetupVersion)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDEFrameworkCompilerSettings)
include(GenerateExportHeader)
include(FeatureSummary)

############## Find Packages ###################

find_package(KF5KDELibs4Support     ${KF5_VERSION} CONFIG REQUIRED)
# find_package(KF5CoreAddons ${KF5_VERSION} CONFIG REQUIRED)
# find_package(KF5Config ${KF5_VERSION} CONFIG REQUIRED)
# find_package(KF5I18n ${KF5_VERSION} CONFIG REQUIRED)

find_package(Exiv2)
set_package_properties("Exiv2"     PROPERTIES
                       DESCRIPTION "Required to build libkexiv2."
                       URL         "http://www.exiv2.org"
                       TYPE        RECOMMENDED
                       PURPOSE     "Library to manage image metadata"
)

ecm_setup_version(${LIBKEXIV2_LIB_VERSION}
                  VARIABLE_PREFIX      KEXIV2
                  VERSION_HEADER       "${CMAKE_CURRENT_BINARY_DIR}/libkexiv2_version.h"
                  PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KF5KExiv2ConfigVersion.cmake"
                  SOVERSION            ${LIBKEXIV2_SO_VERSION}
)

############## Targets #########################

add_definitions(-DTRANSLATION_DOMAIN=\"libkexiv2\")

add_subdirectory(src)
add_subdirectory(tests)

############## CMake Config Files ##############

message("${CMAKECONFIG_INSTALL_DIR}")
set(CMAKECONFIG_INSTALL_DIR "${CMAKECONFIG_INSTALL_PREFIX}/KF5Kexiv2")

ecm_configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/KF5KExiv2Config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/KF5KExiv2Config.cmake"
    INSTALL_DESTINATION ${CMAKECONFIG_INSTALL_DIR}
)

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/KF5KExiv2Config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/KF5KExiv2ConfigVersion.cmake"
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT   Devel
)

install(EXPORT      KF5KExiv2Targets
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        FILE        KF5KExiv2Targets.cmake
        NAMESPACE   KF5::
)

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/libkexiv2_version.h"
        DESTINATION "${KF5_INCLUDE_INSTALL_DIR}"
        COMPONENT   Devel
)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
