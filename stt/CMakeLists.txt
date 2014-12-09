# Create a library called "libBase" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${CMAKE_SOURCE_DIR}/geobase 
${CMAKE_SOURCE_DIR}/parbase 
${CMAKE_SOURCE_DIR}/base 
${CMAKE_SOURCE_DIR}/field 
${CMAKE_SOURCE_DIR}/cbmbase 
${CMAKE_SOURCE_DIR}/mcstack 
${CMAKE_SOURCE_DIR}/fairtools
${CMAKE_SOURCE_DIR}/stt
#${CMAKE_SOURCE_DIR}/cpc

)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(STT_SRCS
BmnSttGeo.cxx 
BmnSttGeoPar.cxx 
BmnStt.cxx 
BmnSttContFact.cxx 
BmnSttHit.cxx 
BmnSttHitProducer.cxx 
BmnSttPoint.cxx
#BmnSttRec.cxx
#ScalarD.cxx
#ScalarI.cxx
#ScalarBool.cxx
#StrawttPointPlane.cxx
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h STT_HEADERS "${STT_SRCS}")

set(STT_LINKDEF BmnSttLinkDef.h)
set(STT_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/BmnSttDict.cxx) 

ROOT_GENERATE_DICTIONARY("${STT_HEADERS}" "${STT_LINKDEF}" "${STT_DICTIONARY}" "${INCLUDE_DIRECTORIES}")


set(STT_SRCS ${STT_SRCS} ${STT_DICTIONARY})


add_library(Stt SHARED ${STT_SRCS})
target_link_libraries(Stt ${ROOT_LIBRARIES})
set_target_properties(Stt PROPERTIES VERSION 0.0.1 SOVERSION 0)

################ install ###################
install(TARGETS Stt DESTINATION ${CMAKE_BINARY_DIR}/lib)
