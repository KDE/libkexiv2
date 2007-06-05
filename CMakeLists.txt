project(libkexiv2)

add_definitions(${KDE4_ENABLE_EXCEPTIONS})

# Detect Exiv2 library properlly with CMake script from 'cmake/modules'
SET( CMAKE_MODULE_PATH    ${CMAKE_MODULE_PATH}  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules )
FIND_PACKAGE(Exiv2 REQUIRED)

include_directories(${KDE4_INCLUDES} ${QT4_INCLUDES})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/libkexiv2.pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/libkexiv2.pc)

########### next target ###############

set(kexiv2_LIB_SRCS kexiv2.cpp)

kde4_automoc(${kexiv2_LIB_SRCS})

kde4_add_library(kexiv2 SHARED ${kexiv2_LIB_SRCS})

target_link_libraries(kexiv2 ${KDE4_KDECORE_LIBS} ${QT_QTGUI_LIBRARY} exiv2)

set_target_properties(kexiv2 PROPERTIES VERSION 3.0.0 SOVERSION 3 )

install(TARGETS kexiv2 DESTINATION ${LIB_INSTALL_DIR} )

########### install files ###############

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libkexiv2.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig )

install(FILES kexiv2.h libkexiv2_export.h version.h DESTINATION ${INCLUDE_INSTALL_DIR}/libkexiv2)

