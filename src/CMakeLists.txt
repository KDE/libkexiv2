#
# Copyright (c) 2010-2015, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

kde_enable_exceptions()

include_directories(${CMAKE_CURRENT_BINARY_DIR}/..)

add_definitions(${EXIV2_DEFINITIONS})

if( WIN32 )
    add_definitions( -DEXV_HAVE_DLL )
endif()


set(kexiv2_LIB_SRCS kexiv2.cpp
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
                    libkexiv2_debug.cpp
)

ecm_generate_headers(kexiv2_CamelCase_HEADERS

                     HEADER_NAMES
                     KExiv2Data
                     KExiv2
                     KExiv2Previews
                     RotationMatrix

                     PREFIX           KExiv2
                     REQUIRED_HEADERS kexiv2_HEADERS
)

add_library(KF5KExiv2 SHARED ${kexiv2_LIB_SRCS})
add_library(KF5::KExiv2 ALIAS KF5KExiv2)

generate_export_header(KF5KExiv2 BASE_NAME libkexiv2 EXPORT_MACRO_NAME LIBKEXIV2_EXPORT)


target_include_directories(KF5KExiv2 INTERFACE
                           "$<INSTALL_INTERFACE:${KF5_INCLUDE_INSTALL_DIR}/KExiv2;${KF5_INCLUDE_INSTALL_DIR}/KExiv2/kexiv2>"
                           "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/..>"
)

target_link_libraries(KF5KExiv2
                      PRIVATE
                      LibExiv2::LibExiv2
                      PUBLIC
                      Qt5::Core
                      Qt5::Gui
)

if( WIN32 )
    target_link_libraries( KF5KExiv2 ${EXPAT_LIBRARIES} )
endif()

install(TARGETS KF5KExiv2
        EXPORT  KF5KExiv2Targets ${KF5_INSTALL_TARGETS_DEFAULT_ARGS}
)

set_target_properties(KF5KExiv2   PROPERTIES
                      VERSION     ${KEXIV2_VERSION_STRING}
                      SOVERSION   ${KEXIV2_SOVERSION}
                      EXPORT_NAME KExiv2
)

install(FILES       ${CMAKE_CURRENT_BINARY_DIR}/libkexiv2_export.h
        DESTINATION ${KF5_INCLUDE_INSTALL_DIR}/KExiv2/kexiv2
        COMPONENT   Devel
)

install(FILES       ${kexiv2_CamelCase_HEADERS}
        DESTINATION ${KF5_INCLUDE_INSTALL_DIR}/KExiv2/KExiv2
        COMPONENT   Devel
)

install(FILES       ${kexiv2_HEADERS}
        DESTINATION ${KF5_INCLUDE_INSTALL_DIR}/KExiv2/kexiv2
        COMPONENT   Devel
)

install(FILES       ${CMAKE_CURRENT_BINARY_DIR}/libkexiv2_version.h
        DESTINATION ${KF5_INCLUDE_INSTALL_DIR}
        COMPONENT   Devel
)
