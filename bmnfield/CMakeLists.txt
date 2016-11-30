# Create a library called "libMpdField" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${CMAKE_SOURCE_DIR}/base/field
${CMAKE_SOURCE_DIR}/base/steer
${CMAKE_SOURCE_DIR}/base/sim
${CMAKE_SOURCE_DIR}/base/event
${CMAKE_SOURCE_DIR}/base/source
${CMAKE_SOURCE_DIR}/parbase 
${CMAKE_SOURCE_DIR}/fairtools
${CMAKE_SOURCE_DIR}/bmnfield 
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(FIELD_SRCS
BmnNewFieldMap.cxx
BmnFieldConst.cxx     
BmnFieldContFact.cxx    
BmnFieldMap.cxx
BmnFieldMapCreator.cxx         
BmnFieldMapData.cxx     
#BmnFieldMapSym3.cxx     
BmnFieldPar.cxx         
BmnFieldCreator.cxx
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h FIELD_HEADERS "${FIELD_SRCS}")

set(FIELD_LINKDEF FieldLinkDef.h)
set(FIELD_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/FieldDict.cxx) 

ROOT_GENERATE_DICTIONARY("${FIELD_HEADERS}" "${FIELD_LINKDEF}" "${FIELD_DICTIONARY}" "${INCLUDE_DIRECTORIES}")


set(FIELD_SRCS ${FIELD_SRCS} ${FIELD_DICTIONARY})

add_library(BmnField SHARED ${FIELD_SRCS})
target_link_libraries(BmnField ${ROOT_LIBRARIES})
set_target_properties(BmnField PROPERTIES VERSION 0.0.1 SOVERSION 0 )

################ install ###################
install(TARGETS BmnField DESTINATION ${CMAKE_BINARY_DIR}/lib)


