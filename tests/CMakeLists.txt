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
add_executable(setiptcpreview NOGUI ${setiptcpreview_SRCS})
TARGET_LINK_LIBRARIES(setiptcpreview kexiv2  )

SET(loadfromba_SRCS loadfromba.cpp)
add_executable(loadfromba NOGUI ${loadfromba_SRCS})
TARGET_LINK_LIBRARIES(loadfromba kexiv2  )

SET(erasetag_SRCS erasetag.cpp)
add_executable(erasetag NOGUI ${erasetag_SRCS})
TARGET_LINK_LIBRARIES(erasetag kexiv2  )

SET(printtagslist_SRCS printtagslist.cpp)
add_executable(printtagslist NOGUI ${printtagslist_SRCS})
TARGET_LINK_LIBRARIES(printtagslist kexiv2  )

SET(usexmpsidecar_SRCS usexmpsidecar.cpp)
add_executable(usexmpsidecar NOGUI ${usexmpsidecar_SRCS})
TARGET_LINK_LIBRARIES(usexmpsidecar kexiv2  )

SET(readimagewritexmpsidecar_SRCS readimagewritexmpsidecar.cpp)
add_executable(readimagewritexmpsidecar NOGUI ${readimagewritexmpsidecar_SRCS})
TARGET_LINK_LIBRARIES(readimagewritexmpsidecar kexiv2  )

SET(setxmpface_SRCS setxmpface.cpp)
add_executable(setxmpface NOGUI ${setxmpface_SRCS})
TARGET_LINK_LIBRARIES(setxmpface kexiv2  )
