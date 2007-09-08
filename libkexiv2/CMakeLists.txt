ADD_DEFINITIONS(${KDE4_ENABLE_EXCEPTIONS})

INCLUDE_DIRECTORIES(${KDE4_INCLUDES} ${QT4_INCLUDES})

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/version.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/version.h)

# This was used to enable catching of exceptions thrown by libexiv2.
# We use now #pragma GCC visibility push(default) around the headers
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=default")

SET(kexiv2_LIB_SRCS kexiv2.cpp 
                    kexiv2private.cpp
                    kexiv2image.cpp
                    kexiv2comments.cpp
                    kexiv2exif.cpp 
                    kexiv2iptc.cpp
                    kexiv2gps.cpp 
                    kexiv2xmp.cpp 
   )

KDE4_ADD_LIBRARY(kexiv2 SHARED ${kexiv2_LIB_SRCS})

TARGET_LINK_LIBRARIES(kexiv2 ${KDE4_KDECORE_LIBS} ${QT_QTGUI_LIBRARY} exiv2)

SET_TARGET_PROPERTIES(kexiv2 PROPERTIES VERSION ${KEXIV2_LIB_SO_VERSION_STRING} 
                             SOVERSION ${KEXIV2_LIB_SO_CUR_VERSION})

INSTALL(TARGETS kexiv2 DESTINATION ${LIB_INSTALL_DIR} )

INSTALL(FILES kexiv2.h libkexiv2_export.h ${CMAKE_CURRENT_BINARY_DIR}/version.h 
        DESTINATION ${INCLUDE_INSTALL_DIR}/libkexiv2)
