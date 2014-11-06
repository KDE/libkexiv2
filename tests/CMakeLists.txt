#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../libkexiv2)

set(setiptcpreview_SRCS setiptcpreview.cpp)
KDE4_ADD_EXECUTABLE(setiptcpreview NOGUI ${setiptcpreview_SRCS})
target_link_libraries(setiptcpreview kexiv2 ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})

set(loadfromba_SRCS loadfromba.cpp)
KDE4_ADD_EXECUTABLE(loadfromba NOGUI ${loadfromba_SRCS})
target_link_libraries(loadfromba kexiv2 ${KDE4_KDECORE_LIBS} ${QT_QTGUI_LIBRARY})

set(erasetag_SRCS erasetag.cpp)
KDE4_ADD_EXECUTABLE(erasetag NOGUI ${erasetag_SRCS})
target_link_libraries(erasetag kexiv2 ${KDE4_KDECORE_LIBS} ${QT_QTGUI_LIBRARY})

set(printtagslist_SRCS printtagslist.cpp)
KDE4_ADD_EXECUTABLE(printtagslist NOGUI ${printtagslist_SRCS})
target_link_libraries(printtagslist kexiv2 ${KDE4_KDECORE_LIBS} ${QT_QTGUI_LIBRARY})

set(usexmpsidecar_SRCS usexmpsidecar.cpp)
KDE4_ADD_EXECUTABLE(usexmpsidecar NOGUI ${usexmpsidecar_SRCS})
target_link_libraries(usexmpsidecar kexiv2 ${KDE4_KDECORE_LIBS} ${QT_QTGUI_LIBRARY})

set(readimagewritexmpsidecar_SRCS readimagewritexmpsidecar.cpp)
KDE4_ADD_EXECUTABLE(readimagewritexmpsidecar NOGUI ${readimagewritexmpsidecar_SRCS})
target_link_libraries(readimagewritexmpsidecar kexiv2 ${KDE4_KDECORE_LIBS} ${QT_QTGUI_LIBRARY})

set(setxmpface_SRCS setxmpface.cpp)
KDE4_ADD_EXECUTABLE(setxmpface NOGUI ${setxmpface_SRCS})
target_link_libraries(setxmpface kexiv2 ${KDE4_KDECORE_LIBS} ${QT_QTGUI_LIBRARY})
