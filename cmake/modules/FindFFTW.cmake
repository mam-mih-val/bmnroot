# Find the FFTW includes and library
#
# Usage:
#   find_package(FFTW [REQUIRED] [QUIET])
#     
# It sets the following variables:
#   FFTW_FOUND               ... true if fftw is found on the system; if false, you cannot build anything that requires fftw3
#   FFTW_INCLUDE_DIR         ... fftw include directory where to locate fftw3.h file
#   FFTW_LIBRARY             ... full path to fftw library to link against to use fftw3
#
# The following variables will be checked by the function
#   FFTW_USE_STATIC_LIBS    ... if true, only static libraries are found

#If environment variable FFTWDIR is specified, it has same effect as FFTW_ROOT
if (NOT FFTW_ROOT AND ENV{FFTWDIR})
  set(FFTW_ROOT $ENV{FFTWDIR})
endif()

#find FFTW include directory
find_path(FFTW_INCLUDE_DIR NAMES fftw3.h
  ${FFTW_ROOT}/include
  $ENV{FFTW_ROOT}/include
  $ENV{FFTW3}
  $ENV{FFTW3}/include
  $ENV{FFTW3}/api
  /usr/local/include
  /usr/include
  /opt/fftw3/include
  DOC "Specify the directory containing fftw3.h"
)

#Check whether to search static or dynamic libs
set(CMAKE_FIND_LIBRARY_SUFFIXES_SAV ${CMAKE_FIND_LIBRARY_SUFFIXES})

if (${FFTW_USE_STATIC_LIBS})
  set(CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_STATIC_LIBRARY_SUFFIX})
else ()
  set(CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_SHARED_LIBRARY_SUFFIX})
endif ()

#find FFTW library
find_library(FFTW_LIBRARY NAMES fftw3 fftw3-3 PATHS
  ${FFTW_ROOT}/lib
  $ENV{FFTW_ROOT}/lib
  ${FFTW_ROOT}/lib64
  $ENV{FFTW3}
  $ENV{FFTW3}/lib
  $ENV{FFTW3}/.libs
  /usr/local/lib
  /usr/lib
  /opt/fftw3/lib
  DOC "Specify the fttw3 library here."
)

set(CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES_SAV})

if (FFTW_INCLUDE_DIR AND FFTW_LIBRARY)
  set(FFTW_FOUND 1)
  if (NOT FFTW_FIND_QUIETLY)
     #message(STATUS "Found fftw3 include directory at ${FFTW_INCLUDE_DIR}")
     message(STATUS "Looking for FFTW... found ${FFTW_LIBRARY}")
  endif (NOT FFTW_FIND_QUIETLY)
else ()
  set(FFTW_FOUND 0)
  if (FFTW_FIND_REQUIRED)
    message(FATAL_ERROR "Looking for FFTW... not found")
  else (FFTW_FIND_REQUIRED)
    if (NOT FFTW_FIND_QUIETLY)
      message(STATUS "Looking for FFTW... not found")
    endif (NOT FFTW_FIND_QUIETLY)
  endif (FFTW_FIND_REQUIRED)
endif ()

set(FFTW_LIBRARIES ${FFTW_LIBRARY})

mark_as_advanced(FFTW_FOUND FFTW_INCLUDE_DIR FFTW_LIBRARY FFTW_LIBRARIES)

