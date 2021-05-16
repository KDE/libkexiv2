#
# SPDX-FileCopyrightText: 2010-2015 Gilles Caulier <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

set(KEXIV2_INSTALL_INCLUDEDIR "${KDE_INSTALL_INCLUDEDIR_KF5}/KExiv2")
set(KEXIV2_CMAKECONFIG_NAME "KF5KExiv2")

ecm_setup_version(${libkexiv2_VERSION}
    VARIABLE_PREFIX      KEXIV2
    VERSION_HEADER       "libkexiv2_version.h"
    PACKAGE_VERSION_FILE "${KEXIV2_CMAKECONFIG_NAME}ConfigVersion.cmake"
    SOVERSION            ${LIBKEXIV2_SO_VERSION}
)

kde_enable_exceptions()

add_library(KExiv2 SHARED)

if( WIN32 )
    target_compile_definitions(KExiv2 PRIVATE -DEXV_HAVE_DLL)
endif()

target_sources(KExiv2 PRIVATE
    kexiv2.cpp
    kexiv2_p.cpp
    kexiv2data.cpp
    kexiv2image.cpp
    kexiv2comments.cpp
    kexiv2exif.cpp
    kexiv2iptc.cpp
    kexiv2gps.cpp
    kexiv2xmp.cpp
    kexiv2previews.cpp
    rotationmatrix.cpp
)
ecm_qt_declare_logging_category(KExiv2
    HEADER libkexiv2_debug.h
    IDENTIFIER LIBKEXIV2_LOG
    CATEGORY_NAME "libKExiv2"
    DESCRIPTION "libKExiv2"
    EXPORT kexiv2
)

ecm_generate_headers(kexiv2_CamelCase_HEADERS
    HEADER_NAMES
        KExiv2Data
        KExiv2
        KExiv2Previews
        RotationMatrix
    PREFIX KExiv2
    REQUIRED_HEADERS kexiv2_HEADERS
)

generate_export_header(KExiv2
    BASE_NAME libkexiv2
)

target_include_directories(KExiv2
    INTERFACE
        "$<INSTALL_INTERFACE:${KEXIV2_INSTALL_INCLUDEDIR};${KEXIV2_INSTALL_INCLUDEDIR}/kexiv2>"
)

target_link_libraries(KExiv2
    PUBLIC
        Qt5::Gui
        Qt5::Core
    PRIVATE
        LibExiv2::LibExiv2
)

if( WIN32 )
    target_link_libraries(KExiv2 ${EXPAT_LIBRARIES})
endif()

set_target_properties(KExiv2  PROPERTIES
    OUTPUT_NAME KF5KExiv2
    VERSION     ${KEXIV2_VERSION}
    SOVERSION   ${KEXIV2_SOVERSION}
)

install(TARGETS KExiv2
    EXPORT  KExiv2Targets ${KF5_INSTALL_TARGETS_DEFAULT_ARGS}
)

install(FILES
    ${kexiv2_CamelCase_HEADERS}
    DESTINATION ${KEXIV2_INSTALL_INCLUDEDIR}/KExiv2
    COMPONENT   Devel
)

install(FILES
    ${kexiv2_HEADERS}
    ${CMAKE_CURRENT_BINARY_DIR}/libkexiv2_export.h
    DESTINATION ${KEXIV2_INSTALL_INCLUDEDIR}/kexiv2
    COMPONENT   Devel
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/libkexiv2_version.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}
    COMPONENT   Devel
)

ecm_qt_install_logging_categories(
    EXPORT kexiv2
    FILE libkexiv2.categories
    DESTINATION "${KDE_INSTALL_LOGGINGCATEGORIESDIR}"
)

############## CMake Config Files ##############

set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/${KEXIV2_CMAKECONFIG_NAME}")

configure_package_config_file(
    "${CMAKE_SOURCE_DIR}/cmake/templates/${KEXIV2_CMAKECONFIG_NAME}Config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${KEXIV2_CMAKECONFIG_NAME}Config.cmake"
    INSTALL_DESTINATION ${CMAKECONFIG_INSTALL_DIR}
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/${KEXIV2_CMAKECONFIG_NAME}Config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/${KEXIV2_CMAKECONFIG_NAME}ConfigVersion.cmake"
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    COMPONENT   Devel
)

install(EXPORT KExiv2Targets
    FILE        ${KEXIV2_CMAKECONFIG_NAME}Targets.cmake
    NAMESPACE   KF5::
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    COMPONENT   Devel
)
