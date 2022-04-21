# Create a library called "libFHCal"

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/base
 ${CMAKE_SOURCE_DIR}/base/data
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
BmnFHCalAddress.cxx
BmnFHCalContFact.cxx
BmnFHCalGeo.cxx
BmnFHCalGeoPar.cxx
BmnFHCalDigitizer.cxx
BmnFHCalDigi.cxx
BmnFHCalDigit.cxx
BmnFHCalModule.cxx
BmnFHCalEvent.cxx
)

Set(HEADERS)
Set(LINKDEF FHCalLinkDef.h)
Set(LIBRARY_NAME FHCal)
Set(DEPENDENCIES BmnBase Base Core)

GENERATE_LIBRARY()
