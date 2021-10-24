# Create a library called "libLargeCSC"

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/bmndata
 ${CMAKE_SOURCE_DIR}/large_csc
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
 BmnLargeCSC.cxx
 BmnLargeCSCPoint.cxx
 #BmnLargeCSCStationSet.cxx
 #BmnLargeCSCStation.cxx
 #BmnLargeCSCModule.cxx
 #BmnLargeCSCLayer.cxx
 #BmnLargeCSCDigitizer.cxx
 #BmnLargeCSCHitMaker.cxx
 #BmnLargeCSCTransform.cxx

 #BmnLargeCSCDigit.cxx
 #BmnLargeCSCHit.cxx

 #BmnLargeCSCConfiguration.h
)

Set(HEADERS)
Set(LINKDEF LargeCSCLinkDef.h)
Set(LIBRARY_NAME LargeCSC)
Set(DEPENDENCIES BmnData Base XMLParser Core)

GENERATE_LIBRARY()
