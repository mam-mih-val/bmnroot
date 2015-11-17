
# Create a library called "libZdc" which includes the source files given in
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
${CMAKE_SOURCE_DIR}/zdc
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(ZDC_SRCS
BmnZdc.cxx 
BmnZdcPoint.cxx  
BmnZdcContFact.cxx  
BmnZdcGeo.cxx 
BmnZdcGeoPar.cxx
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h ZDC_HEADERS "${ZDC_SRCS}")

set(ZDC_LINKDEF BmnZdcLinkDef.h)
set(ZDC_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/BmnZdcDict.cxx) 

ROOT_GENERATE_DICTIONARY("${ZDC_HEADERS}" "${ZDC_LINKDEF}" "${ZDC_DICTIONARY}" "${INCLUDE_DIRECTORIES}")


set(ZDC_SRCS ${ZDC_SRCS} ${ZDC_DICTIONARY})


add_library(Zdc SHARED ${ZDC_SRCS})
target_link_libraries(Zdc ${ROOT_LIBRARIES})
set_target_properties(Zdc PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})


################ install ###################
install(TARGETS Zdc DESTINATION ${CMAKE_BINARY_DIR}/lib)
