# Find libxml2 library
#
# Usage:
#   find_package(XML2 [REQUIRED] [QUIET])
#     
# It sets the following variables:
#   XML2_FOUND              ... true if system has libxml2
#   XML2_INCLUDE_DIR        ... libxml2 include directory
#   XML2_LIBRARY_DIR        ... libxml2 library directory
#   XML2_LIBRARIES          ... libraries needed to use libxml2

if (XML2_INCLUDE_DIR OR XML2_LIBRARIES)
  SET (XML2_INCLUDE_DIR XML2_INCLUDE_DIR-NOTFOUND)
  SET (XML2_LIBRARIES XML2_LIBRARIES-NOTFOUND)
endif (XML2_INCLUDE_DIR OR XML2_LIBRARIES)

FIND_PATH(XML2_INCLUDE_DIR NAMES libxml/xmlversion.h PATHS
   $ENV{LIBXML2_ROOT}/include/libxml2
   /usr/include/libxml2
)

FIND_LIBRARY(XML2_LIBRARIES NAMES xml2 PATHS
   $ENV{LIBXML2_ROOT}/lib
)

if (XML2_INCLUDE_DIR AND XML2_LIBRARIES)
  set(XML2_FOUND TRUE)
  get_filename_component(XML2_LIBRARY_DIR ${XML2_LIBRARIES} DIRECTORY)
  SET(LD_LIBRARY_PATH ${LD_LIBRARY_PATH} ${XML2_LIBRARY_DIR})
  if (NOT XML2_FIND_QUIETLY)
    MESSAGE(STATUS "Looking for libxml2... found ${XML2_LIBRARIES}")
  endif (NOT XML2_FIND_QUIETLY)
else ()
  set(XML2_FOUND FALSE)
  if (XML2_FIND_REQUIRED)
    message(FATAL_ERROR "Looking for libxml2... not found")
  else (XML2_FIND_REQUIRED)
    if (NOT XML2_FIND_QUIETLY)
      message(STATUS "Looking for libxml2... not found")
    endif (NOT XML2_FIND_QUIETLY)
  endif (XML2_FIND_REQUIRED)
endif ()

