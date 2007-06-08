project(libkexiv2)

subdirs(libkexiv2)

add_definitions(${KDE4_ENABLE_EXCEPTIONS})

# Detect Exiv2 library properlly with CMake script from 'cmake/modules'
SET( CMAKE_MODULE_PATH    ${CMAKE_MODULE_PATH}  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules )
FIND_PACKAGE(Exiv2 REQUIRED)

include_directories(${KDE4_INCLUDES} ${QT4_INCLUDES})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/libkexiv2.pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/libkexiv2.pc)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libkexiv2.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig )


