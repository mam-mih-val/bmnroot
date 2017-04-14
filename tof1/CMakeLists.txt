
# Create a library called "libTOF1" which includes the source files given in
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
${CMAKE_SOURCE_DIR}/fairtools
${CMAKE_SOURCE_DIR}/mcstack
${CMAKE_SOURCE_DIR}/bmndata
${CMAKE_SOURCE_DIR}/tof1
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(TOF1_SRCS
BmnTOF1.cxx 
BmnTOF1Point.cxx  
BmnTOF1ContFact.cxx  
BmnTOF1Geo.cxx 
BmnTOF1GeoPar.cxx
BmnTof1HitProducer.cxx
BmnTof1HitProducerIdeal.cxx
BmnTof1GeoUtils.cxx
BmnTOF1TDC.cxx
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h TOF1_HEADERS "${TOF1_SRCS}")

set(TOF1_LINKDEF BmnTOF1LinkDef.h)
set(TOF1_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/BmnTOF1Dict.cxx) 

ROOT_GENERATE_DICTIONARY("${TOF1_HEADERS}" "${TOF1_LINKDEF}" "${TOF1_DICTIONARY}" "${INCLUDE_DIRECTORIES}")


set(TOF1_SRCS ${TOF1_SRCS} ${TOF1_DICTIONARY})


add_library(TOF1 SHARED ${TOF1_SRCS})
target_link_libraries(TOF1 ${ROOT_LIBRARIES})
set_target_properties(TOF1 PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})


################ install ###################
install(TARGETS TOF1 DESTINATION ${CMAKE_BINARY_DIR}/lib)
