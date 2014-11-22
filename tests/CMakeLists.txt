#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

set(setiptcpreview_SRCS setiptcpreview.cpp)
add_executable(setiptcpreview ${setiptcpreview_SRCS})
target_link_libraries(setiptcpreview KF5KExiv2)

set(loadfromba_SRCS loadfromba.cpp)
add_executable(loadfromba ${loadfromba_SRCS})
target_link_libraries(loadfromba KF5KExiv2)

set(erasetag_SRCS erasetag.cpp)
add_executable(erasetag ${erasetag_SRCS})
target_link_libraries(erasetag KF5KExiv2)

set(printtagslist_SRCS printtagslist.cpp)
add_executable(printtagslist ${printtagslist_SRCS})
target_link_libraries(printtagslist KF5KExiv2)

set(usexmpsidecar_SRCS usexmpsidecar.cpp)
add_executable(usexmpsidecar ${usexmpsidecar_SRCS})
target_link_libraries(usexmpsidecar KF5KExiv2)

set(readimagewritexmpsidecar_SRCS readimagewritexmpsidecar.cpp)
add_executable(readimagewritexmpsidecar ${readimagewritexmpsidecar_SRCS})
target_link_libraries(readimagewritexmpsidecar KF5KExiv2)

set(setxmpface_SRCS setxmpface.cpp)
add_executable(setxmpface ${setxmpface_SRCS})
target_link_libraries(setxmpface KF5KExiv2)
