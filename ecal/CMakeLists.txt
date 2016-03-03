
# Create a library called "libEcal" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${CMAKE_SOURCE_DIR}/base 
${CMAKE_SOURCE_DIR}/geobase 
${CMAKE_SOURCE_DIR}/parbase
${CMAKE_SOURCE_DIR}/fairtools
${CMAKE_SOURCE_DIR}/mcstack
${CMAKE_SOURCE_DIR}/bmndata
${CMAKE_SOURCE_DIR}/ecal
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(ECAL_SRCS
BmnEcal.cxx 
BmnEcalPoint.cxx  
BmnEcalContFact.cxx  
BmnEcalGeo.cxx 
BmnEcalGeoPar.cxx
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h ECAL_HEADERS "${ECAL_SRCS}")

set(ECAL_LINKDEF BmnEcalLinkDef.h)
set(ECAL_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/BmnEcalDict.cxx) 

ROOT_GENERATE_DICTIONARY("${ECAL_HEADERS}" "${ECAL_LINKDEF}" "${ECAL_DICTIONARY}" "${INCLUDE_DIRECTORIES}")


set(ECAL_SRCS ${ECAL_SRCS} ${ECAL_DICTIONARY})


add_library(Ecal SHARED ${ECAL_SRCS})
target_link_libraries(Ecal ${ROOT_LIBRARIES})
set_target_properties(Ecal PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})


################ install ###################
install(TARGETS Ecal DESTINATION ${CMAKE_BINARY_DIR}/lib)
