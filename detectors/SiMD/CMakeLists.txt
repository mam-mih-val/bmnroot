# Create a library called "libSiMD"

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/base
 ${CMAKE_SOURCE_DIR}/base/data
 ${CMAKE_SOURCE_DIR}/base/raw
 ${CMAKE_SOURCE_DIR}/detectors/SiMD
)

Set(SYSTEM_INCLUDE_DIRECTORIES
 ${ROOT_INCLUDE_DIR} 
)

include_directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
 ${ROOT_LIBRARY_DIR}
 ${FAIRROOT_LIBRARY_DIR}
 ${Boost_LIBRARY_DIRS}
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
Set(LINKDEF SiMDLinkDef.h)
Set(LIBRARY_NAME SiMD)
Set(DEPENDENCIES BmnBase Base Core)

GENERATE_LIBRARY()
