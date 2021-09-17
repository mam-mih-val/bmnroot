# Create a library called "libZdc"

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/bmndata
 ${CMAKE_SOURCE_DIR}/bmndata/math
 ${CMAKE_SOURCE_DIR}/zdc
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
BmnZdcDigitizer.cxx
BmnZdcAnalyzer.cxx
BmnFHCalDigitizer.cxx
)

Set(HEADERS)
Set(LINKDEF BmnZdcLinkDef.h)
Set(LIBRARY_NAME Zdc)
Set(DEPENDENCIES BmnData Base Core)

GENERATE_LIBRARY()
