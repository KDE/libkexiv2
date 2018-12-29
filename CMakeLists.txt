#
# Copyright (c) 2010-2015, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

set(CMAKE_MIN_VERSION   "3.0.0")
set(ECM_MIN_VERSION     "1.1.0")
set(REQUIRED_QT_VERSION "5.2.0")
set(EXIV2_MIN_VERSION   "0.25")

cmake_minimum_required(VERSION ${CMAKE_MIN_VERSION})

project(libkexiv2 VERSION "5.0.0")

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

# Library ABI version used by linker.
# For details : http://www.gnu.org/software/libtool/manual/libtool.html#Updating-version-info
set(KEXIV2_LIB_SO_CUR_VERSION "15")
set(KEXIV2_LIB_SO_REV_VERSION "0")
set(KEXIV2_LIB_SO_AGE_VERSION "0")

set(LIBKEXIV2_SO_VERSION  "${KEXIV2_LIB_SO_CUR_VERSION}.${KEXIV2_LIB_SO_REV_VERSION}.${KEXIV2_LIB_SO_AGE_VERSION}")

############## ECM setup ######################

find_package(ECM ${ECM_MIN_VERSION} CONFIG REQUIRED)
set(CMAKE_MODULE_PATH ${libkexiv2_SOURCE_DIR}/cmake/modules ${ECM_MODULE_PATH})

include(ECMGenerateHeaders)
include(ECMGeneratePriFile)
include(ECMPackageConfigHelpers)
include(ECMSetupVersion)
include(ECMMarkNonGuiExecutable)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
include(GenerateExportHeader)
include(FeatureSummary)

ecm_setup_version(${libkexiv2_VERSION}
                  VARIABLE_PREFIX      KEXIV2
                  VERSION_HEADER       "src/libkexiv2_version.h"
                  PACKAGE_VERSION_FILE "KF5KExiv2ConfigVersion.cmake"
                  SOVERSION            ${LIBKEXIV2_SO_VERSION}
)

############## Find Packages ###################

find_package(Qt5 ${REQUIRED_QT_VERSION} REQUIRED NO_MODULE COMPONENTS
             Core
             Gui
)

find_package(LibExiv2 ${EXIV2_MIN_VERSION} REQUIRED)

############## Targets #########################

add_subdirectory(src)

if (BUILD_TESTING)
    add_subdirectory(tests)
endif()

############## CMake Config Files ##############

message("${CMAKECONFIG_INSTALL_DIR}")
set(CMAKECONFIG_INSTALL_DIR "${CMAKECONFIG_INSTALL_PREFIX}/KF5KExiv2")

ecm_configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/templates/KF5KExiv2Config.cmake.in"
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

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
