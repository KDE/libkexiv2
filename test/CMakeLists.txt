INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../libkexiv2)

SET(setiptcpreview_SRCS setiptcpreview.cpp)
KDE4_ADD_EXECUTABLE(setiptcpreview NOGUI ${setiptcpreview_SRCS})
TARGET_LINK_LIBRARIES(setiptcpreview kexiv2 ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})

SET(loadfromba_SRCS loadfromba.cpp)
KDE4_ADD_EXECUTABLE(loadfromba NOGUI ${loadfromba_SRCS})
TARGET_LINK_LIBRARIES(loadfromba kexiv2 ${KDE4_KDECORE_LIBS} ${QT_QTGUI_LIBRARY})
