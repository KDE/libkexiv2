PROJECT(libkexiv2)

# =======================================================
# Information to update before to release this library.

# Library version history:
# API      ABI
# 0.1.0 => 0.1.0
# 0.1.1 => 0.2.0
# 0.1.2 => 1.0.1
# 0.1.3 => 1.0.1
# 0.1.4 => 2.0.2
# 0.1.5 => 2.1.1
# 0.1.6 => 3.0.0
# 0.1.7 => 4.0.1
# 0.1.8 => 5.0.0
# ... here we can have new releases from KDE3 branch
# 0.2.0 => 6.0.0     (released with KDE 4.1.0)
# 0.3.0 => 7.0.0     (released with KDE 4.1.2)
# 0.4.0 => 7.1.0
# 0.5.0 => 7.2.0     (Released with KDE 4.2.0)
# 0.6.0 => 7.3.0     (Released with KDE 4.3.0)

# Library API version
SET(KEXIV2_LIB_MAJOR_VERSION "0")
SET(KEXIV2_LIB_MINOR_VERSION "6")
SET(KEXIV2_LIB_PATCH_VERSION "0")

# Suffix to add at end of version string. Usual values are:
# "-svn"   : alpha code unstable from svn. Do not use in production
# "-beta1" : beta1 release.
# "-beta2" : beta2 release.
# "-beta3" : beta3 release.
# "-rc"    : release candidate.
# ""       : final relase. Can be used in production.
SET(KEXIV2_LIB_SUFFIX_VERSION "")

# Library ABI version used by linker.
# For details : http://www.gnu.org/software/libtool/manual.html#Updating-version-info
SET(KEXIV2_LIB_SO_CUR_VERSION "7")
SET(KEXIV2_LIB_SO_REV_VERSION "3")
SET(KEXIV2_LIB_SO_AGE_VERSION "0")

# =======================================================
# Set env. variables accordinly.

SET(KEXIV2_LIB_VERSION_STRING "${KEXIV2_LIB_MAJOR_VERSION}.${KEXIV2_LIB_MINOR_VERSION}.${KEXIV2_LIB_PATCH_VERSION}${KEXIV2_LIB_SUFFIX_VERSION}")
SET(KEXIV2_LIB_VERSION_ID "0x0${KEXIV2_LIB_MAJOR_VERSION}0${KEXIV2_LIB_MINOR_VERSION}0${KEXIV2_LIB_PATCH_VERSION}")

SET(KEXIV2_LIB_SO_VERSION_STRING "${KEXIV2_LIB_SO_CUR_VERSION}.${KEXIV2_LIB_SO_REV_VERSION}.${KEXIV2_LIB_SO_AGE_VERSION}")

# =======================================================

if (EXIV2_FOUND)
  SUBDIRS(libkexiv2)
  SUBDIRS(test)

  IF(NOT WIN32)
    CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/libkexiv2.pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/libkexiv2.pc)
    CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/libkexiv2.lsm.cmake ${CMAKE_CURRENT_BINARY_DIR}/libkexiv2.lsm)
    INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/libkexiv2.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig )
  ENDIF(NOT WIN32)
endif (EXIV2_FOUND)
