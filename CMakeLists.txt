#
# SPDX-FileCopyrightText: 2010-2015 Gilles Caulier <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

cmake_minimum_required(VERSION 3.16)

set(ECM_MIN_VERSION     "5.91.0")
set(QT_MIN_VERSION      "5.15.0")
set(EXIV2_MIN_VERSION   "0.25")

project(libkexiv2 VERSION "5.1.0")

set(CMAKE_CXX_STANDARD 14) # exiv is not compatible with C++ > 14 yet
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message(STATUS "----------------------------------------------------------------------------------")
message(STATUS "Starting CMake configuration for: ${PROJECT_NAME}")

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
# 2.4.0 => 11.3.0                              - Add new method to access on text edit widget from AltLangStrEdit
# 5.0.0 => 15.0.0    (Released with KDE 5.x)
# 5.1.0 => 15.0.0    (Released with KDE Gear) - Add QTransform-varianst methods RotationMatrix

# Library ABI version used by linker.
# For details : https://www.gnu.org/software/libtool/manual/libtool.html#Updating-version-info
set(KEXIV2_LIB_SO_CUR_VERSION "15")
set(KEXIV2_LIB_SO_REV_VERSION "0")
set(KEXIV2_LIB_SO_AGE_VERSION "0")
# TODO Qt6 or next ABI break: the very SO version is usually a single number, not a triple.
# The library non-SO version as decribed in the link above is a separate thing
# (cmp. VVERSION & SOVERSION args of set_target_properties(KF5KExiv2)
# See below (where QT_MAJOR_VERSION is available) for resetting LIBKEXIV2_SO_VERSION to 0 with Qt6 builds
set(LIBKEXIV2_SO_VERSION  "${KEXIV2_LIB_SO_CUR_VERSION}.${KEXIV2_LIB_SO_REV_VERSION}.${KEXIV2_LIB_SO_AGE_VERSION}")

############## ECM setup ######################

find_package(ECM ${ECM_MIN_VERSION} CONFIG REQUIRED)
set(CMAKE_MODULE_PATH ${libkexiv2_SOURCE_DIR}/cmake/modules ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

include(ECMGenerateHeaders)
include(ECMSetupVersion)
include(ECMMarkNonGuiExecutable)
include(ECMQtDeclareLoggingCategory)
include(ECMGenerateExportHeader)
include(ECMDeprecationSettings)
include(CMakePackageConfigHelpers)
include(FeatureSummary)

# reset to 0 once all deprecated API has been removed on full switch to Qt6
if (QT_MAJOR_VERSION STREQUAL "6")
set(EXCLUDE_DEPRECATED_BEFORE_AND_AT 5.1.0 CACHE STRING "Control the range of deprecated API excluded from the build [default=5.1.0].")
else()
set(EXCLUDE_DEPRECATED_BEFORE_AND_AT 0 CACHE STRING "Control the range of deprecated API excluded from the build [default=0].")
endif()

if (QT_MAJOR_VERSION STREQUAL "6")
    # starting with 0 for the new Qt6 series
    set(LIBKEXIV2_SO_VERSION "0")
endif()

############## Find Packages ###################

if (QT_MAJOR_VERSION STREQUAL "6")
    set(QT_MIN_VERSION "6.5.0")
endif()

find_package(Qt${QT_MAJOR_VERSION} ${QT_MIN_VERSION} REQUIRED NO_MODULE
    COMPONENTS
        Core
        Gui
)

find_package(LibExiv2 ${EXIV2_MIN_VERSION} REQUIRED)

############## Targets #########################

if (QT_MAJOR_VERSION STREQUAL "6")
    ecm_set_disabled_deprecation_versions(
        QT 6.5
    )
else()
    ecm_set_disabled_deprecation_versions(
        QT 5.14 # QTransform::toAffine
    )
endif()

add_subdirectory(src)

if (BUILD_TESTING)
    add_subdirectory(tests)
endif()

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
