#
# SPDX-FileCopyrightText: 2010-2015 Gilles Caulier <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

kde_enable_exceptions()

include_directories(${CMAKE_CURRENT_BINARY_DIR}/..)

add_definitions(${EXIV2_DEFINITIONS})

if( WIN32 )
    add_definitions( -DEXV_HAVE_DLL )
endif()


set(kexiv2_LIB_SRCS
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
ecm_qt_declare_logging_category(kexiv2_LIB_SRCS
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

add_library(KF5KExiv2 SHARED ${kexiv2_LIB_SRCS})
add_library(KF5::KExiv2 ALIAS KF5KExiv2)

generate_export_header(KF5KExiv2
    BASE_NAME libkexiv2
)

target_include_directories(KF5KExiv2
    INTERFACE
        "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR_KF5}/KExiv2;${KDE_INSTALL_INCLUDEDIR_KF5}/KExiv2/kexiv2>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/..>"
)

target_link_libraries(KF5KExiv2
    PUBLIC
        Qt5::Gui
        Qt5::Core
    PRIVATE
        LibExiv2::LibExiv2
)

if( WIN32 )
    target_link_libraries( KF5KExiv2 ${EXPAT_LIBRARIES} )
endif()

set_target_properties(KF5KExiv2  PROPERTIES
    VERSION     ${KEXIV2_VERSION_STRING}
    SOVERSION   ${KEXIV2_SOVERSION}
    EXPORT_NAME KExiv2
)

install(TARGETS KF5KExiv2
    EXPORT  KF5KExiv2Targets ${KF5_INSTALL_TARGETS_DEFAULT_ARGS}
)

install(FILES
    ${kexiv2_CamelCase_HEADERS}
    DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}/KExiv2/KExiv2
    COMPONENT   Devel
)

install(FILES
    ${kexiv2_HEADERS}
    ${CMAKE_CURRENT_BINARY_DIR}/libkexiv2_export.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}/KExiv2/kexiv2
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
