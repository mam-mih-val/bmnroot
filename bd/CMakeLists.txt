
# Create a library called "libBd" which includes the source files given in
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
${CMAKE_SOURCE_DIR}/bd
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(BD_SRCS
BmnBd.cxx 
BmnBdPoint.cxx  
BmnBdContFact.cxx  
BmnBdGeo.cxx 
BmnBdGeoPar.cxx
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h BD_HEADERS "${BD_SRCS}")

set(BD_LINKDEF BmnBdLinkDef.h)
set(BD_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/BmnBdDict.cxx) 

ROOT_GENERATE_DICTIONARY("${BD_HEADERS}" "${BD_LINKDEF}" "${BD_DICTIONARY}" "${INCLUDE_DIRECTORIES}")


set(BD_SRCS ${BD_SRCS} ${BD_DICTIONARY})


add_library(Bd SHARED ${BD_SRCS})
target_link_libraries(Bd ${ROOT_LIBRARIES})
set_target_properties(Bd PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})


################ install ###################
install(TARGETS Bd DESTINATION ${CMAKE_BINARY_DIR}/lib)
