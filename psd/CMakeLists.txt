# Create a library called "libPsd" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${CMAKE_SOURCE_DIR}/base/field
${CMAKE_SOURCE_DIR}/base/steer
${CMAKE_SOURCE_DIR}/base/sim
${CMAKE_SOURCE_DIR}/base/event
${CMAKE_SOURCE_DIR}/base/source
${CMAKE_SOURCE_DIR}/geobase 
${CMAKE_SOURCE_DIR}/parbase
${CMAKE_SOURCE_DIR}/fairtools
${CMAKE_SOURCE_DIR}/bmndata
${CMAKE_SOURCE_DIR}/psd
)

Set(SYSTEM_INCLUDE_DIRECTORIES
 ${Boost_INCLUDE_DIRS}
 ${ROOT_INCLUDE_DIR}
)

include_directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories(${LINK_DIRECTORIES})

# List of source files
set(SRCS
CbmPsd.cxx  
CbmGeoPsd.cxx  
CbmGeoPsdPar.cxx  
CbmPsdv1.cxx 
CbmPsdIdealDigitizer.cxx
CbmPsdHitProducer.cxx
CbmPsdReactionPlaneMaker.cxx
)

Set(HEADERS)
Set(LINKDEF PsdLinkDef.h)
Set(LIBRARY_NAME Psd)
Set(DEPENDENCIES Core Base BmnData)

GENERATE_LIBRARY()
