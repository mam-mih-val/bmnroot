
# Create a library called "libRecoil" which includes the source files given in
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
${CMAKE_SOURCE_DIR}/recoil
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(RECOIL_SRCS
BmnRecoil.cxx 
BmnRecoilPoint.cxx  
BmnRecoilContFact.cxx  
BmnRecoilGeo.cxx 
BmnRecoilGeoPar.cxx
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h RECOIL_HEADERS "${RECOIL_SRCS}")

set(RECOIL_LINKDEF BmnRecoilLinkDef.h)
set(RECOIL_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/BmnRecoilDict.cxx) 

ROOT_GENERATE_DICTIONARY("${RECOIL_HEADERS}" "${RECOIL_LINKDEF}" "${RECOIL_DICTIONARY}" "${INCLUDE_DIRECTORIES}")


set(RECOIL_SRCS ${RECOIL_SRCS} ${RECOIL_DICTIONARY})


add_library(Recoil SHARED ${RECOIL_SRCS})
target_link_libraries(Recoil ${ROOT_LIBRARIES})
set_target_properties(Recoil PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})


################ install ###################
install(TARGETS Recoil DESTINATION ${CMAKE_BINARY_DIR}/lib)
