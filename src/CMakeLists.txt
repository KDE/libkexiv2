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

kde_enable_exceptions()

include_directories(${EXIV2_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR}/..)

add_definitions(${EXIV2_DEFINITIONS})

remove_definitions(-DQT_NO_CAST_FROM_ASCII -DQT_NO_CAST_TO_ASCII)

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
                    msgtextedit.cpp
                    altlangstredit.cpp
                    countryselector.cpp
                    subjectwidget.cpp
)

ecm_generate_headers(kexiv2_CamelCase_HEADERS

                     HEADER_NAMES
                     AltLangStrEdit
                     CountrySelector
                     KExiv2Data
                     KExiv2
                     KExiv2Previews
                     MsgTextEdit
                     RotationMatrix
                     SubjectWidget

                     PREFIX           KEXIV2
                     REQUIRED_HEADERS kexiv2_HEADERS
)

install(FILES       ${kexiv2_CamelCase_HEADERS}
        DESTINATION ${KF5_INCLUDE_INSTALL_DIR}/KEXIV2/KExiv2
        COMPONENT   Devel
)

install(FILES       ${kexiv2_HEADERS}
        DESTINATION ${KF5_INCLUDE_INSTALL_DIR}/KEXIV2/kexiv2
        COMPONENT   Devel
)

add_library(KF5Kexiv2 SHARED ${kexiv2_LIB_SRCS})

generate_export_header(KF5Kexiv2 BASE_NAME kexiv2)

install(FILES       ${CMAKE_CURRENT_BINARY_DIR}/kexiv2_export.h
        DESTINATION ${KF5_INCLUDE_INSTALL_DIR}/KEXIV2
        COMPONENT   Devel
)

add_library(KF5::Kexiv2 ALIAS KF5Kexiv2)

target_include_directories(KF5Kexiv2 INTERFACE 
                           "$<INSTALL_INTERFACE:${KF5_INCLUDE_INSTALL_DIR}/KEXIV2;${KF5_INCLUDE_INSTALL_DIR}/KEXIV2/kexiv2>"
)

target_link_libraries(KF5Kexiv2
                      KF5::KDELibs4Support
                      Qt5::Xml
                      ${EXIV2_LIBRARIES}
)

if( WIN32 )
    target_link_libraries( kexiv2 ${EXPAT_LIBRARIES} )
endif()

install(TARGETS KF5Kexiv2
        EXPORT  KF5KExiv2Targets ${KF5_INSTALL_TARGETS_DEFAULT_ARGS}
)

set_target_properties(KF5Kexiv2   PROPERTIES
                      VERSION     ${KEXIV2_VERSION_STRING}
                      SOVERSION   ${KEXIV2_SOVERSION}
                      EXPORT_NAME Kexiv2
)

install(FILES       ${CMAKE_CURRENT_SOURCE_DIR}/../data/topicset.iptc-subjectcode.xml
        DESTINATION ${DATA_INSTALL_DIR}/libkexiv2/data)
