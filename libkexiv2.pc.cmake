prefix=${CMAKE_INSTALL_PREFIX}
exec_prefix=${BIN_INSTALL_DIR}
libdir=${LIB_INSTALL_DIR}
includedir=${INCLUDE_INSTALL_DIR}

Name: libkexiv2
Description: KDE wrapper library for Exiv2 library with some extras
Requires:
Version: 0.2.0
Libs: -L${LIB_INSTALL_DIR} -lkexiv2
Cflags: -I${INCLUDE_INSTALL_DIR}
