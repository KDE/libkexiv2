#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

include_directories(${EXIV2_INCLUDE_DIR})

add_definitions(${EXIV2_DEFINITIONS})
add_definitions(${KDE4_ENABLE_EXCEPTIONS})

if(WIN32)
    add_definitions(-DEXV_HAVE_DLL)
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/version.h.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/version.h)

# This was used to enable catching of exceptions thrown by libexiv2.
# We use now #pragma GCC visibility push(default) around the headers
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=default")

set(kexiv2_LIB_SRCS kexiv2.cpp
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

target_link_libraries(kexiv2 ${KDE4_KDEUI_LIBS} ${KDE4_KDECORE_LIBS} ${QT_QTGUI_LIBRARY}
                             ${QT_QTXML_LIBRARY} ${EXIV2_LIBRARIES})

if(WIN32)
    TARGET_LINK_LIBRARIES( kexiv2 ${EXPAT_LIBRARIES} )
endif()

set_target_properties(kexiv2 PROPERTIES VERSION ${KEXIV2_LIB_SO_VERSION_STRING} 
                             SOVERSION ${KEXIV2_LIB_SO_CUR_VERSION})

install(TARGETS kexiv2 EXPORT Libkexiv2 ${INSTALL_TARGETS_DEFAULT_ARGS} )

install(FILES topicset.iptc-subjectcode.xml DESTINATION ${DATA_INSTALL_DIR}/libkexiv2/data)

# Check if compilation is done through digiKam SC or not. If yes, do not install header files.
if(NOT DIGIKAMSC_COMPILE_LIBKEXIV2)

    install(FILES kexiv2.h
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

    if(NOT WIN32)
            configure_file(${CMAKE_SOURCE_DIR}/cmake/templates/libkexiv2.pc.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/libkexiv2.pc)
            configure_file(${CMAKE_SOURCE_DIR}/cmake/templates/libkexiv2.lsm.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/libkexiv2.lsm)
            install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libkexiv2.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig)
    endif()

endif()
