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

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../libkexiv2)

SET(setiptcpreview_SRCS setiptcpreview.cpp)
add_executable(setiptcpreview ${setiptcpreview_SRCS})
TARGET_LINK_LIBRARIES(setiptcpreview KF5Kexiv2)

SET(loadfromba_SRCS loadfromba.cpp)
add_executable(loadfromba ${loadfromba_SRCS})
TARGET_LINK_LIBRARIES(loadfromba KF5Kexiv2)

SET(erasetag_SRCS erasetag.cpp)
add_executable(erasetag ${erasetag_SRCS})
TARGET_LINK_LIBRARIES(erasetag KF5Kexiv2)

SET(printtagslist_SRCS printtagslist.cpp)
add_executable(printtagslist ${printtagslist_SRCS})
TARGET_LINK_LIBRARIES(printtagslist KF5Kexiv2)

SET(usexmpsidecar_SRCS usexmpsidecar.cpp)
add_executable(usexmpsidecar ${usexmpsidecar_SRCS})
TARGET_LINK_LIBRARIES(usexmpsidecar KF5Kexiv2)

SET(readimagewritexmpsidecar_SRCS readimagewritexmpsidecar.cpp)
add_executable(readimagewritexmpsidecar ${readimagewritexmpsidecar_SRCS})
TARGET_LINK_LIBRARIES(readimagewritexmpsidecar KF5Kexiv2)

SET(setxmpface_SRCS setxmpface.cpp)
add_executable(setxmpface ${setxmpface_SRCS})
TARGET_LINK_LIBRARIES(setxmpface KF5Kexiv2)
