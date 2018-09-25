# Find FairLogger module
#
# Usage:
#   find_package(FairLogger [REQUIRED] [QUIET])
#     
# It sets the following variables:
#   FAIRLOGGER_FOUND         ... true if FairLogger is found on the system
#   FairLogger_INCDIR        ... FairLogger include directory
#   FairLogger_LIBDIR        ... FairLogger library directory

find_path(FairLogger_INCDIR NAMES fairlogger/Logger.h PATHS
  ${FAIRLOGGER_ROOT}/include
  $ENV{FAIRLOGGER_ROOT}/include
  ${FAIRROOTPATH}/include
  ${SIMPATH}/include
  NO_DEFAULT_PATH
)

find_path(FairLogger_LIBDIR NAMES libFairLogger.so PATHS
  ${FAIRLOGGER_ROOT}/lib
  $ENV{FAIRLOGGER_ROOT}/lib
  ${FAIRROOTPATH}/lib
  ${SIMPATH}/lib
  NO_DEFAULT_PATH
)

if (FairLogger_INCDIR AND FairLogger_LIBDIR)
  set(FAIRLOGGER_FOUND TRUE)
  SET(LD_LIBRARY_PATH ${LD_LIBRARY_PATH} ${FairLogger_LIBDIR})
  SET(BASE_INCLUDE_DIRECTORIES ${BASE_INCLUDE_DIRECTORIES} ${FairLogger_INCDIR})
  if (NOT FAIRLOGGER_FIND_QUIETLY)
    message(STATUS "Looking for FairLogger... found at ${FairLogger_LIBDIR}")
  endif (NOT FAIRLOGGER_FIND_QUIETLY)
else (FairLogger_INCDIR AND FairLogger_LIBDIR)
  set(FAIRLOGGER_FOUND FALSE)
  if (FAIRLOGGER_FIND_REQUIRED)
    message(FATAL_ERROR "Looking for FairLogger... not found")
  else (FAIRLOGGER_FIND_REQUIRED)
    if (NOT FAIRLOGGER_FIND_QUIETLY)
      message(STATUS "Looking for FairLogger... not found")
    endif (NOT FAIRLOGGER_FIND_QUIETLY)
  endif (FAIRLOGGER_FIND_REQUIRED)
endif (FairLogger_INCDIR AND FairLogger_LIBDIR)
