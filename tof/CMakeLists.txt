# Create a library called "libBmnTof" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${CMAKE_SOURCE_DIR}/base 
${CMAKE_SOURCE_DIR}/geobase 
${CMAKE_SOURCE_DIR}/parbase
${CMAKE_SOURCE_DIR}/fairtools
${CMAKE_SOURCE_DIR}/bmndata
${CMAKE_SOURCE_DIR}/tof
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(TOF_SRCS
CbmGeoTof.cxx 
CbmGeoTofPar.cxx 
CbmTof.cxx 
CbmTofContFact.cxx 
CbmTofDetectorId.cxx
CbmTofDetectorId_v07a.cxx
CbmTofDetectorId_v12b.cxx
CbmTofGeoHandler.cxx 
CbmTofCell.cxx
CbmTofDigiPar.cxx
CbmTofHitProducer.cxx 
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h TOF_HEADERS "${TOF_SRCS}")

set(TOF_LINKDEF TofLinkDef.h)
set(TOF_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/BmnTofDict.cxx) 

ROOT_GENERATE_DICTIONARY("${TOF_HEADERS}" "${TOF_LINKDEF}" "${TOF_DICTIONARY}" "${INCLUDE_DIRECTORIES}")


set(TOF_SRCS ${TOF_SRCS} ${TOF_DICTIONARY})


add_library(Tof SHARED ${TOF_SRCS})
target_link_libraries(Tof ${ROOT_LIBRARIES})
set_target_properties(Tof PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})

################ install ###################
install(TARGETS Tof DESTINATION ${CMAKE_BINARY_DIR}/lib)
