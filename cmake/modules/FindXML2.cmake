# Searching libxml2 installation
# Once done this will define
#  XML2_FOUND - system has libxml2
#  XML2_INCLUDE_DIR - the libxml2 include directory
#  XML2_LIBRARY_DIR - the libxml2 library directory
#  XML2_LIBRARIES - the libraries needed to use libxml2

if (XML2_INCLUDE_DIR OR XML2_LIBRARIES)
  SET (XML2_INCLUDE_DIR XML2_INCLUDE_DIR-NOTFOUND)
  SET (XML2_LIBRARIES XML2_LIBRARIES-NOTFOUND)
endif (XML2_INCLUDE_DIR OR XML2_LIBRARIES)

MESSAGE(STATUS "Looking for libxml2...")

FIND_PATH(XML2_INCLUDE_DIR NAMES libxml/xmlversion.h PATHS /usr/include/libxml2)

FIND_LIBRARY(XML2_LIBRARIES NAMES xml2)

if (XML2_INCLUDE_DIR AND XML2_LIBRARIES)
   set(XML2_FOUND TRUE)
   get_filename_component(XML2_LIBRARY_DIR ${XML2_LIBRARIES} DIRECTORY)
endif (XML2_INCLUDE_DIR AND XML2_LIBRARIES)

if (XML2_FOUND)
  if (NOT XML2_FIND_QUIETLY)
    MESSAGE(STATUS "Looking for libxml2... - found ${XML2_LIBRARIES}")
    SET(LD_LIBRARY_PATH ${LD_LIBRARY_PATH} ${XML2_LIBRARY_DIR})
  endif (NOT XML2_FIND_QUIETLY)
else (XML2_FOUND)
  if (XML2_FIND_REQUIRED)
    message(FATAL_ERROR "Looking for libxml2... - Not found")
  endif (XML2_FIND_REQUIRED)
endif (XML2_FOUND)

