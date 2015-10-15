# Create a library called "libBmnKalmanFilter".

set(INCLUDE_DIRECTORIES
${BASE_INCLUDE_DIRECTORIES}
${ROOT_INCLUDE_DIR}
${Boost_INCLUDE_DIR}

${BMNROOT_SOURCE_DIR}/base
${BMNROOT_SOURCE_DIR}/parbase
${BMNROOT_SOURCE_DIR}/fairtools
${BMNROOT_SOURCE_DIR}/bmnfield
${BMNROOT_SOURCE_DIR}/bmnbase
${BMNROOT_SOURCE_DIR}/bmndata
${BMNROOT_SOURCE_DIR}/globaltracking
${BMNROOT_SOURCE_DIR}/BmnKalmanFilter
${BMNROOT_SOURCE_DIR}/gem
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)

link_directories( ${LINK_DIRECTORIES})

set(SRCS
BmnKalmanFilter_tmp.cxx
)

set(HEADERS
BmnKalmanFilter_tmp.h
)

IF (SSE_FOUND)
  ADD_DEFINITIONS(-DHAVE_SSE)
  SET_SOURCE_FILES_PROPERTIES(${SRCS} PROPERTIES COMPILE_FLAGS "-msse -O3 -w")
  Message(STATUS "Gem will be compiled with SSE support")
ELSE (SSE_FOUND)
  Message(STATUS "Gem will be compiled without SSE support")
  SET_SOURCE_FILES_PROPERTIES(${SRCS} PROPERTIES COMPILE_FLAGS "-O3 -w")
ENDIF (SSE_FOUND)

If (Boost_FOUND)
  set(DEFINITIONS BOOST_FOUND)
EndIf (Boost_FOUND)

set(LINKDEF  BmnKalmanFilterLinkDef.h)
Set(LIBRARY_NAME BmnKalmanFilter_tmp)
Set(DEPENDENCIES
    BmnBase BmnData Base
)

GENERATE_LIBRARY()

