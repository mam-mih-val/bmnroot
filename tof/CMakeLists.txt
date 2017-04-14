
# Create a library called "libTOF" which includes the source files given in
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
${CMAKE_SOURCE_DIR}/tof
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(TOF_SRCS
BmnTOF.cxx 
BmnTOFPoint.cxx  
BmnTOFContFact.cxx  
BmnTOFGeo.cxx 
BmnTOFGeoPar.cxx
BmnTofHitProducer.cxx
BmnTofHitProducerIdeal.cxx
BmnTofGeoUtils.cxx
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h TOF_HEADERS "${TOF_SRCS}")

set(TOF_LINKDEF BmnTOFLinkDef.h)
set(TOF_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/BmnTOFDict.cxx) 

ROOT_GENERATE_DICTIONARY("${TOF_HEADERS}" "${TOF_LINKDEF}" "${TOF_DICTIONARY}" "${INCLUDE_DIRECTORIES}")


set(TOF_SRCS ${TOF_SRCS} ${TOF_DICTIONARY})


add_library(TOF SHARED ${TOF_SRCS})
target_link_libraries(TOF ${ROOT_LIBRARIES})
set_target_properties(TOF PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})


################ install ###################
install(TARGETS TOF DESTINATION ${CMAKE_BINARY_DIR}/lib)
