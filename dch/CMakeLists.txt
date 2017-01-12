
# Create a library called "libDch" which includes the source files given in
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
${CMAKE_SOURCE_DIR}/dch
${CMAKE_SOURCE_DIR}/uni_db
${CMAKE_SOURCE_DIR}/uni_db/db_classes
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(Dch_SRCS
BmnDch1.cxx 
BmnDch1Point.cxx  
BmnDch1ContFact.cxx  
BmnDch1Geo.cxx 
BmnDch1GeoPar.cxx

BmnDchHitProducer.cxx
BmnDchHitProducerTmp.cxx
BmnDchHitProducer_exp.cxx
ScalarD.cxx
ScalarI.cxx
ScalarUI.cxx
DchHitPlane.cxx
BmnDchTrackFinder.cxx
BmnDchTrack.cxx

BmnDch2.cxx 
BmnDch2Point.cxx  
BmnDch2ContFact.cxx  
BmnDch2Geo.cxx 
BmnDch2GeoPar.cxx
)


# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h Dch_HEADERS "${Dch_SRCS}")

set(Dch_LINKDEF LinkDef.h)
set(Dch_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/BmnDchDict.cxx) 

ROOT_GENERATE_DICTIONARY("${Dch_HEADERS}" "${Dch_LINKDEF}" "${Dch_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

set(Dch_SRCS ${Dch_SRCS} ${Dch_DICTIONARY})

add_library(Dch SHARED ${Dch_SRCS})
target_link_libraries(Dch ${ROOT_LIBRARIES})
set_target_properties(Dch PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})


################ install ###################
install(TARGETS Dch DESTINATION ${CMAKE_BINARY_DIR}/lib)

