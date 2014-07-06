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

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../libkexiv2)

SET(setiptcpreview_SRCS setiptcpreview.cpp)
KDE4_ADD_EXECUTABLE(setiptcpreview NOGUI ${setiptcpreview_SRCS})
TARGET_LINK_LIBRARIES(setiptcpreview kexiv2 ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})

SET(loadfromba_SRCS loadfromba.cpp)
KDE4_ADD_EXECUTABLE(loadfromba NOGUI ${loadfromba_SRCS})
TARGET_LINK_LIBRARIES(loadfromba kexiv2 ${KDE4_KDECORE_LIBS} ${QT_QTGUI_LIBRARY})

SET(erasetag_SRCS erasetag.cpp)
KDE4_ADD_EXECUTABLE(erasetag NOGUI ${erasetag_SRCS})
TARGET_LINK_LIBRARIES(erasetag kexiv2 ${KDE4_KDECORE_LIBS} ${QT_QTGUI_LIBRARY})

SET(printtagslist_SRCS printtagslist.cpp)
KDE4_ADD_EXECUTABLE(printtagslist NOGUI ${printtagslist_SRCS})
TARGET_LINK_LIBRARIES(printtagslist kexiv2 ${KDE4_KDECORE_LIBS} ${QT_QTGUI_LIBRARY})

SET(usexmpsidecar_SRCS usexmpsidecar.cpp)
KDE4_ADD_EXECUTABLE(usexmpsidecar NOGUI ${usexmpsidecar_SRCS})
TARGET_LINK_LIBRARIES(usexmpsidecar kexiv2 ${KDE4_KDECORE_LIBS} ${QT_QTGUI_LIBRARY})

SET(readimagewritexmpsidecar_SRCS readimagewritexmpsidecar.cpp)
KDE4_ADD_EXECUTABLE(readimagewritexmpsidecar NOGUI ${readimagewritexmpsidecar_SRCS})
TARGET_LINK_LIBRARIES(readimagewritexmpsidecar kexiv2 ${KDE4_KDECORE_LIBS} ${QT_QTGUI_LIBRARY})

SET(setxmpface_SRCS setxmpface.cpp)
KDE4_ADD_EXECUTABLE(setxmpface NOGUI ${setxmpface_SRCS})
TARGET_LINK_LIBRARIES(setxmpface kexiv2 ${KDE4_KDECORE_LIBS} ${QT_QTGUI_LIBRARY})
