# Create a library called "libPsd" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${CMAKE_SOURCE_DIR}/base 
${CMAKE_SOURCE_DIR}/geobase 
${CMAKE_SOURCE_DIR}/parbase
${CMAKE_SOURCE_DIR}/fairtools
${CMAKE_SOURCE_DIR}/bmndata
${CMAKE_SOURCE_DIR}/psd
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(PSD_SRCS
CbmPsd.cxx  
CbmGeoPsd.cxx  
CbmGeoPsdPar.cxx  
CbmPsdv1.cxx 
CbmPsdIdealDigitizer.cxx
CbmPsdHitProducer.cxx
CbmPsdReactionPlaneMaker.cxx
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h PSD_HEADERS "${PSD_SRCS}")

set(PSD_LINKDEF PsdLinkDef.h)
set(PSD_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/BmnPsdDict.cxx) 

ROOT_GENERATE_DICTIONARY("${PSD_HEADERS}" "${PSD_LINKDEF}" "${PSD_DICTIONARY}" "${INCLUDE_DIRECTORIES}")


set(PSD_SRCS ${PSD_SRCS} ${PSD_DICTIONARY})


add_library(Psd SHARED ${PSD_SRCS})
target_link_libraries(Psd ${ROOT_LIBRARIES})
set_target_properties(Psd PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})

################ install ###################
install(TARGETS Psd DESTINATION ${CMAKE_BINARY_DIR}/lib)
