#
# SPDX-FileCopyrightText: 2010-2014 Gilles Caulier <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

add_executable(setiptcpreview)
target_sources(setiptcpreview PRIVATE setiptcpreview.cpp)
target_link_libraries(setiptcpreview KExiv2)

add_executable(loadfromba)
target_sources(loadfromba PRIVATE loadfromba.cpp)
target_link_libraries(loadfromba KExiv2)

add_executable(erasetag)
target_sources(erasetag PRIVATE erasetag.cpp)
target_link_libraries(erasetag KExiv2)

add_executable(printtagslist)
target_sources(printtagslist PRIVATE printtagslist.cpp)
target_link_libraries(printtagslist KExiv2)

add_executable(usexmpsidecar)
target_sources(usexmpsidecar PRIVATE usexmpsidecar.cpp)
target_link_libraries(usexmpsidecar KExiv2)

add_executable(readimagewritexmpsidecar)
target_sources(readimagewritexmpsidecar PRIVATE readimagewritexmpsidecar.cpp)
target_link_libraries(readimagewritexmpsidecar KExiv2)

add_executable(setxmpface)
target_sources(setxmpface PRIVATE setxmpface.cpp)
target_link_libraries(setxmpface KExiv2)
