# Create a library called "libMWPC" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${Boost_INCLUDE_DIRS}
${ROOT_INCLUDE_DIR}
${CMAKE_SOURCE_DIR}/base/field
${CMAKE_SOURCE_DIR}/base/steer
${CMAKE_SOURCE_DIR}/base/sim
${CMAKE_SOURCE_DIR}/base/event
${CMAKE_SOURCE_DIR}/base/source
${CMAKE_SOURCE_DIR}/geobase 
${CMAKE_SOURCE_DIR}/parbase
${CMAKE_SOURCE_DIR}/bmnbase
${CMAKE_SOURCE_DIR}/fairtools
${CMAKE_SOURCE_DIR}/mcstack
${CMAKE_SOURCE_DIR}/bmndata
${CMAKE_SOURCE_DIR}/mwpc
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(Mwpc_SRCS
BmnMwpcHitProducer.cxx
BmnMwpcHitFinder.cxx
BmnMwpcGeometry.cxx
BmnMwpcTrack.cxx
BmnMwpcTrackFinder.cxx
)


# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h Mwpc_HEADERS "${Mwpc_SRCS}")

set(Mwpc_LINKDEF LinkDef.h)
set(Mwpc_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/BmnMwpcDict.cxx) 

ROOT_GENERATE_DICTIONARY("${Mwpc_HEADERS}" "${Mwpc_LINKDEF}" "${Mwpc_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

set(Mwpc_SRCS ${Mwpc_SRCS} ${Mwpc_DICTIONARY})

add_library(Mwpc SHARED ${Mwpc_SRCS})
target_link_libraries(Mwpc ${ROOT_LIBRARIES})
set_target_properties(Mwpc PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})


################ install ###################
install(TARGETS Mwpc DESTINATION ${CMAKE_BINARY_DIR}/lib)
