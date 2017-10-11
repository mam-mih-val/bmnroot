# Create a library called "libMpdField" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${CMAKE_SOURCE_DIR}/base/field
${CMAKE_SOURCE_DIR}/base/steer
${CMAKE_SOURCE_DIR}/base/sim
${CMAKE_SOURCE_DIR}/base/event
${CMAKE_SOURCE_DIR}/base/source
${CMAKE_SOURCE_DIR}/parbase 
${CMAKE_SOURCE_DIR}/fairtools
${CMAKE_SOURCE_DIR}/bmnfield 
)

Set(SYSTEM_INCLUDE_DIRECTORIES
 ${ROOT_INCLUDE_DIR} 
)

include_directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

# List of source files
set(SRCS
BmnNewFieldMap.cxx
BmnFieldConst.cxx     
BmnFieldContFact.cxx    
BmnFieldMap.cxx
BmnFieldMapCreator.cxx         
BmnFieldMapData.cxx     
#BmnFieldMapSym3.cxx     
BmnFieldPar.cxx         
BmnFieldCreator.cxx
BmnSP41FieldMapCreator.cxx
)

Set(HEADERS)
Set(LINKDEF FieldLinkDef.h)
Set(LIBRARY_NAME BmnField)
Set(DEPENDENCIES Core Base)

GENERATE_LIBRARY()
