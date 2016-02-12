
# Create a library called "libBarrel" which includes the source files given in
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
${CMAKE_SOURCE_DIR}/barrel
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(BARREL_SRCS
BmnBarrel.cxx 
BmnBarrelPoint.cxx  
BmnBarrelContFact.cxx  
BmnBarrelGeo.cxx 
BmnBarrelGeoPar.cxx
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h BARREL_HEADERS "${BARREL_SRCS}")

set(BARREL_LINKDEF BmnBarrelLinkDef.h)
set(BARREL_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/BmnBarrelDict.cxx) 

ROOT_GENERATE_DICTIONARY("${BARREL_HEADERS}" "${BARREL_LINKDEF}" "${BARREL_DICTIONARY}" "${INCLUDE_DIRECTORIES}")


set(BARREL_SRCS ${BARREL_SRCS} ${BARREL_DICTIONARY})


add_library(Barrel SHARED ${BARREL_SRCS})
target_link_libraries(Barrel ${ROOT_LIBRARIES})
set_target_properties(Barrel PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})


################ install ###################
install(TARGETS Barrel DESTINATION ${CMAKE_BINARY_DIR}/lib)
