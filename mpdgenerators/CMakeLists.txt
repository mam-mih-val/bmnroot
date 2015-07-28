# Create a library called "libMpdgenerators" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${PLUTO_INCLUDE_DIR}
${CMAKE_SOURCE_DIR}/fairtools
${CMAKE_SOURCE_DIR}/base 
${CMAKE_SOURCE_DIR}/geobase 
${CMAKE_SOURCE_DIR}/parbase 
${CMAKE_SOURCE_DIR}/base
${CMAKE_SOURCE_DIR}/mcstack 
${CMAKE_SOURCE_DIR}/mpdgenerators
${CMAKE_SOURCE_DIR}/mpddata
${CMAKE_SOURCE_DIR}/gconfig
)

include_directories( ${INCLUDE_DIRECTORIES})

find_library(Z_LIB z)
message(STATUS "Z_LIB: - ${Z_LIB}")

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${Z_LIB}
)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS
MpdMCEventHeader.cxx
MpdLAQGSMGenerator.cxx
MpdHypYPtGenerator.cxx
MpdRainGenerator.cxx
MpdPlutoGenerator.cxx
MpdPHSDGenerator.cxx
MpdUrqmdGenerator.cxx
MpdGetNumEvents.cxx
MpdHistoGenerator.cxx
)


Set(HEADERS
)

Set(LINKDEF MpdgeneratorsLinkDef.h)

Set(LIBRARY_NAME Mpdgenerators)

Set(DEPENDENCIES
    MpdData Base z
 )

GENERATE_LIBRARY()
