# Create a library called "libFHCal"

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/bmndata
 ${CMAKE_SOURCE_DIR}/database/unicom
 ${CMAKE_SOURCE_DIR}/database/uni_db
 ${CMAKE_SOURCE_DIR}/decoder
 ${CMAKE_SOURCE_DIR}/detectors/fhcal
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
BmnFHCal.cxx 
BmnFHCalPoint.cxx  
BmnFHCalContFact.cxx  
BmnFHCalGeo.cxx 
BmnFHCalGeoPar.cxx
BmnFHCalDigitizer.cxx
BmnFHCalDigit.cxx
BmnFHCalModule.cxx
BmnFHCalEvent.cxx
BmnFHCalReconstructor.cxx
)

Set(HEADERS)
Set(LINKDEF BmnFHCalLinkDef.h)
Set(LIBRARY_NAME FHCal)
Set(DEPENDENCIES BmnData Decoder UniDb Base Core)

GENERATE_LIBRARY()
