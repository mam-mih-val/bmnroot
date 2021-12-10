# Create a library called "libSiMD"

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/bmndata
 ${CMAKE_SOURCE_DIR}/SiMD
)

Set(SYSTEM_INCLUDE_DIRECTORIES
 ${Boost_INCLUDE_DIRS}
 ${ROOT_INCLUDE_DIR} 
)

include_directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
 ${ROOT_LIBRARY_DIR}
 ${FAIRROOT_LIBRARY_DIR}
)
 
link_directories(${LINK_DIRECTORIES})

# List of source files
set(SRCS
BmnSiMD.cxx 
BmnSiMDPoint.cxx  
BmnSiMDContFact.cxx  
BmnSiMDGeo.cxx 
BmnSiMDGeoPar.cxx
BmnSiMDDigitizer.cxx
)

Set(HEADERS)
Set(LINKDEF BmnSiMDLinkDef.h)
Set(LIBRARY_NAME SiMD)
Set(DEPENDENCIES BmnData Base Core)

GENERATE_LIBRARY()
