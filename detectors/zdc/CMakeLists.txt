# Create a library called "libZdc"

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/base
 ${CMAKE_SOURCE_DIR}/base/data
 ${CMAKE_SOURCE_DIR}/base/raw
 ${CMAKE_SOURCE_DIR}/base/header
 ${CMAKE_SOURCE_DIR}/base/math
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
Set(LINKDEF ZdcLinkDef.h)
Set(LIBRARY_NAME Zdc)
Set(DEPENDENCIES BmnBase Base Core)

GENERATE_LIBRARY()
