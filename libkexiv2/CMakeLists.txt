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

INCLUDE_DIRECTORIES(${EXIV2_INCLUDE_DIR})

ADD_DEFINITIONS(${EXIV2_DEFINITIONS})
ADD_DEFINITIONS(${KDE4_ENABLE_EXCEPTIONS})
remove_definitions(-DQT_NO_CAST_FROM_ASCII -DQT_NO_CAST_TO_ASCII)

IF( WIN32 )
    ADD_DEFINITIONS( -DEXV_HAVE_DLL )
ENDIF( WIN32 )


CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/version.h.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/version.h)

# This was used to enable catching of exceptions thrown by libexiv2.
# We use now #pragma GCC visibility push(default) around the headers
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=default")

SET(kexiv2_LIB_SRCS kexiv2.cpp
                    kexiv2_p.cpp
                    kexiv2image.cpp
                    kexiv2comments.cpp
                    kexiv2exif.cpp
                    kexiv2iptc.cpp
                    kexiv2gps.cpp
                    kexiv2xmp.cpp
                    kexiv2previews.cpp
                    altlangstredit.cpp
                    msgtextedit.cpp
                    countryselector.cpp
                    subjectwidget.cpp
                    rotationmatrix.cpp
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
    #SubjectData
    SubjectWidget
    PREFIX KEXIV2
    REQUIRED_HEADERS kexiv2_HEADERS
)


add_library(KF5Kexiv2 ${kexiv2_LIB_SRCS})

generate_export_header(KF5Kexiv2 BASE_NAME kexiv2)

add_library(KF5::Kexiv2 ALIAS KF5Kexiv2)

target_include_directories(KF5Kexiv2 INTERFACE "$<INSTALL_INTERFACE:${KF5_INCLUDE_INSTALL_DIR}/KEXIV2;${KF5_INCLUDE_INSTALL_DIR}/KEXIV2/kexiv2>")

TARGET_LINK_LIBRARIES(KF5Kexiv2 KF5::KHtml KF5::KDELibs4Support Qt5::Xml ${EXIV2_LIBRARIES})

IF( WIN32 )
    TARGET_LINK_LIBRARIES( kexiv2 ${EXPAT_LIBRARIES} )
ENDIF( WIN32 )


install(TARGETS
    KF5Kexiv2
    EXPORT KF5KExiv2Targets ${KF5_INSTALL_TARGETS_DEFAULT_ARGS}
)


set_target_properties(KF5Kexiv2 PROPERTIES
    VERSION ${KEXIV2_VERSION_STRING}
    SOVERSION ${KEXIV2_SOVERSION}
    EXPORT_NAME Kexiv2
)



INSTALL(FILES topicset.iptc-subjectcode.xml DESTINATION ${DATA_INSTALL_DIR}/libkexiv2/data)

