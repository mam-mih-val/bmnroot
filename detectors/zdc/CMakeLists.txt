# Create a library called "libZdc"

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/bmndata
 ${CMAKE_SOURCE_DIR}/bmndata/raw
 ${CMAKE_SOURCE_DIR}/bmndata/gem
 ${CMAKE_SOURCE_DIR}/bmndata/header
 ${CMAKE_SOURCE_DIR}/bmndata/math
 ${CMAKE_SOURCE_DIR}/detectors/zdc
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
 BmnZdc.cxx 
 BmnZdcPoint.cxx  
 BmnZdcContFact.cxx  
 BmnZdcGeo.cxx 
 BmnZdcGeoPar.cxx
 BmnZdcDigi.cxx
 BmnZdcDigiPar.cxx
 BmnZdcDigiScheme.cxx
 BmnZdcDigiProducer.cxx
 BmnZdcBScanner.cxx
 BmnZDCDigit.cxx
 BmnZdcDigitizer.cxx
)

Set(HEADERS)
Set(LINKDEF BmnZdcLinkDef.h)
Set(LIBRARY_NAME Zdc)
Set(DEPENDENCIES BmnData Base Core)

GENERATE_LIBRARY()
