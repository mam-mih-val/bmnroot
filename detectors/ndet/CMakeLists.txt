# Create a library called "libNdet"

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/base
 ${CMAKE_SOURCE_DIR}/base/data
 ${CMAKE_SOURCE_DIR}/detectors/ndet
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
BmnNdet.cxx 
BmnNdetPoint.cxx  
BmnNdetContFact.cxx  
BmnNdetAddress.cxx
BmnNdetDigi.cxx
BmnNdetGeo.cxx 
BmnNdetGeoPar.cxx
BmnNdetDigit.cxx
BmnNdetDigitPar.cxx
BmnNdetDigitScheme.cxx
BmnNdetDigitizer.cxx
)


Set(HEADERS)
Set(LINKDEF NdetLinkDef.h)
Set(LIBRARY_NAME Ndet)
Set(DEPENDENCIES BmnBase Base Core)

GENERATE_LIBRARY()
