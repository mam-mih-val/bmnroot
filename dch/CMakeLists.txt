
# Create a library called "libDch" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${CMAKE_SOURCE_DIR}/base 
${CMAKE_SOURCE_DIR}/geobase 
${CMAKE_SOURCE_DIR}/parbase
${CMAKE_SOURCE_DIR}/bmnbase
${CMAKE_SOURCE_DIR}/fairtools
${CMAKE_SOURCE_DIR}/mcstack
${CMAKE_SOURCE_DIR}/bmndata
${CMAKE_SOURCE_DIR}/dch
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(Dch1_SRCS
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
${CMAKE_SOURCE_DIR}/bmndata/BmnDchRaw2Digit.cxx
)

set(Dch2_SRCS
BmnDch2.cxx 
BmnDch2Point.cxx  
BmnDch2ContFact.cxx  
BmnDch2Geo.cxx 
BmnDch2GeoPar.cxx
#BmnDchHitProducer.cxx
#ScalarD.cxx
#ScalarI.cxx
#ScalarUI.cxx
#DchHitPlane.cxx
)


# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h Dch1_HEADERS "${Dch1_SRCS}")
CHANGE_FILE_EXTENSION(*.cxx *.h Dch2_HEADERS "${Dch2_SRCS}")

set(Dch1_LINKDEF BmnDch1LinkDef.h)
set(Dch1_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/BmnDch1Dict.cxx) 

set(Dch2_LINKDEF BmnDch2LinkDef.h)
set(Dch2_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/BmnDch2Dict.cxx) 

ROOT_GENERATE_DICTIONARY("${Dch1_HEADERS}" "${Dch1_LINKDEF}" "${Dch1_DICTIONARY}" "${INCLUDE_DIRECTORIES}")
ROOT_GENERATE_DICTIONARY("${Dch2_HEADERS}" "${Dch2_LINKDEF}" "${Dch2_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

set(Dch1_SRCS ${Dch1_SRCS} ${Dch1_DICTIONARY})
set(Dch2_SRCS ${Dch2_SRCS} ${Dch2_DICTIONARY})


add_library(Dch1 SHARED ${Dch1_SRCS})
target_link_libraries(Dch1 ${ROOT_LIBRARIES})
set_target_properties(Dch1 PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})

add_library(Dch2 SHARED ${Dch2_SRCS})
target_link_libraries(Dch2 ${ROOT_LIBRARIES})
set_target_properties(Dch2 PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})


################ install ###################
install(TARGETS Dch1 DESTINATION ${CMAKE_BINARY_DIR}/lib)
install(TARGETS Dch2 DESTINATION ${CMAKE_BINARY_DIR}/lib)

