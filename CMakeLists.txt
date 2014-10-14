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

cmake_minimum_required(VERSION 2.8.12)

PROJECT(libkexiv2)

############## ECM setup ######################

find_package(ECM 1.1.0 CONFIG REQUIRED)
set(CMAKE_MODULE_PATH ${libkexiv2_SOURCE_DIR}/cmake/modules ${ECM_MODULE_PATH})

include(GenerateExportHeader)
include(ECMGenerateHeaders)
include(ECMGeneratePriFile)
include(ECMPackageConfigHelpers)
include(ECMSetupVersion)
include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDEFrameworkCompilerSettings)

set(KF5_VERSION "5.0.0")
set(LIBKEXIV2_LIB_VERSION "2.70.0")

############## Find Packages ###################

set(REQUIRED_QT_VERSION "5.2.0")
find_package(KF5KDELibs4Support ${KF5_VERSION} CONFIG REQUIRED)

SET(EXIV2_MIN_VERSION "0.24")
FIND_PACKAGE(Exiv2)
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
                  SOVERSION            3
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
