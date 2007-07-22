add_definitions(${KDE4_ENABLE_EXCEPTIONS})

include_directories(${KDE4_INCLUDES} ${QT4_INCLUDES})

# Note: This is necessary to be able to catch Error thrown from Exiv2.
# With visibility=hidden, the catch clause will not identify the thrown object.
# libexiv2 comes with default visibility.
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=default")

set(kexiv2_LIB_SRCS kexiv2.cpp)

kde4_add_library(kexiv2 SHARED ${kexiv2_LIB_SRCS})

target_link_libraries(kexiv2 ${KDE4_KDECORE_LIBS} ${QT_QTGUI_LIBRARY} exiv2)

set_target_properties(kexiv2 PROPERTIES VERSION 3.0.0 SOVERSION 3 )

install(TARGETS kexiv2 DESTINATION ${LIB_INSTALL_DIR} )

install(FILES kexiv2.h libkexiv2_export.h version.h DESTINATION ${INCLUDE_INSTALL_DIR}/libkexiv2)
