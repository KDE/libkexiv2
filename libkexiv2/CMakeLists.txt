# ===========================================================
#
# This file is a part of digiKam project
# <a href="http://www.digikam.org">http://www.digikam.org</a>
#
# @date   2006-09-15
# @brief  Exiv2 library interface for KDE
#
# @author Copyright (C) 2006-2012 by Gilles Caulier
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

IF( WIN32 )
    ADD_DEFINITIONS( -DEXV_HAVE_DLL )
ENDIF( WIN32 )

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/version.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/version.h)

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

KDE4_ADD_LIBRARY(kexiv2 SHARED ${kexiv2_LIB_SRCS})

TARGET_LINK_LIBRARIES(kexiv2 ${KDE4_KDEUI_LIBS} ${KDE4_KDECORE_LIBS} ${QT_QTGUI_LIBRARY}
                             ${QT_QTXML_LIBRARY} ${EXIV2_LIBRARIES})

IF( WIN32 )
    TARGET_LINK_LIBRARIES( kexiv2 ${EXPAT_LIBRARIES} )
ENDIF( WIN32 )

SET_TARGET_PROPERTIES(kexiv2 PROPERTIES VERSION ${KEXIV2_LIB_SO_VERSION_STRING} 
                             SOVERSION ${KEXIV2_LIB_SO_CUR_VERSION})

INSTALL(TARGETS kexiv2 ${INSTALL_TARGETS_DEFAULT_ARGS} )

INSTALL(FILES topicset.iptc-subjectcode.xml DESTINATION ${DATA_INSTALL_DIR}/libkexiv2/data)

# Check if compilation is done through digiKam SC or not. If yes, do not install header files.
IF(NOT DIGIKAMSC_USE_PRIVATE_KDEGRAPHICS)
    INSTALL(FILES kexiv2.h
                  kexiv2data.h
                  kexiv2previews.h
                  libkexiv2_export.h
                  msgtextedit.h
                  subjectwidget.h
                  altlangstredit.h
                  countryselector.h
                  rotationmatrix.h
                  ${CMAKE_CURRENT_BINARY_DIR}/version.h
            DESTINATION ${INCLUDE_INSTALL_DIR}/libkexiv2
            COMPONENT Devel
           )
ENDIF()
