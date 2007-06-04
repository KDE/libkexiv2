# - Try to find the Exiv2 library
# Once done this will define
#
#  EXIV2_FOUND - system has libexiv2
#  EXIV2_INCLUDE_DIR - the libexiv2 include directory
#  EXIV2_LIBRARIES - Link these to use libexiv2
#  EXIV2_DEFINITIONS - Compiler switches required for using libexiv2
#

if (EXIV2_INCLUDE_DIR AND EXIV2_LIBRARIES)

  # in cache already
  SET(EXIV2_FOUND TRUE)

else (EXIV2_INCLUDE_DIR AND EXIV2_LIBRARIES)

  # use pkg-config to get the directories and then use these values
  # in the FIND_PATH() and FIND_LIBRARY() calls
  INCLUDE(UsePkgConfig)
  
  PKGCONFIG(Exiv2 _EXIV2IncDir _EXIV2LinkDir _EXIV2LinkFlags _EXIV2Cflags)
  
  set(EXIV2_DEFINITIONS ${_EXIV2Cflags})
 
  FIND_PATH(EXIV2_INCLUDE_DIR exiv2/exiv2_version.h
    ${_EXIV2IncDir}
    /usr/include
    /usr/local/include
  )
  
  FIND_LIBRARY(EXIV2_LIBRARIES NAMES libexiv2
    PATHS
    ${_EXIV2LinkDir}
    /usr/lib
    /usr/local/lib
  )
  
  if (EXIV2_INCLUDE_DIR AND EXIV2_LIBRARIES)
     set(EXIV2_FOUND TRUE)
  endif (EXIV2_INCLUDE_DIR AND EXIV2_LIBRARIES)
  
  if (EXIV2_FOUND)
    if (NOT Exiv2_FIND_QUIETLY)
      message(STATUS "Found Exiv2: ${EXIV2_LIBRARIES}")
    endif (NOT Exiv2_FIND_QUIETLY)
  else (EXIV2_FOUND)
    if (Exiv2_FIND_REQUIRED)
      message(FATAL_ERROR "Could NOT find Exiv2")
    endif (Exiv2_FIND_REQUIRED)
  endif (EXIV2_FOUND)
  
  MARK_AS_ADVANCED(EXIV2_INCLUDE_DIR EXIV2_LIBRARIES)
  
endif (EXIV2_INCLUDE_DIR AND EXIV2_LIBRARIES)

