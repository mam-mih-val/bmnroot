
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
${CMAKE_SOURCE_DIR}/gem
${CMAKE_SOURCE_DIR}/uni_db
${CMAKE_SOURCE_DIR}/uni_db/db_classes
${CMAKE_SOURCE_DIR}/bmnfield
${CMAKE_SOURCE_DIR}/passive
)

include_directories( ${INCLUDE_DIRECTORIES})

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${ROOT_INCLUDE_DIR} 
  ${Boost_INCLUDE_DIRS}
  ${GEANT3_INCLUDE_DIR}
)

Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})


set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(Dch_SRCS
BmnDch.cxx 
BmnDchPoint.cxx  
BmnDchContFact.cxx  
BmnDchGeo.cxx 
BmnDchGeoPar.cxx

BmnDchHitProducer.cxx
BmnDchHitProducerTmp.cxx
BmnDchHitProducer_exp.cxx
#BmnDchHitProducerCalib_exp.cxx
DchTrackCand.cxx
DchTrackManager.cxx
ScalarD.cxx
ScalarI.cxx
ScalarUI.cxx
DchHitPlane.cxx
BmnDchTrackFinder.cxx
BmnDchTrack.cxx
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

