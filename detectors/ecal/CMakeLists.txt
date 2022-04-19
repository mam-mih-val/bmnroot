# Create a library called "libEcal"

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/bmndata
 ${CMAKE_SOURCE_DIR}/detectors/ecal
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
 BmnEcal.cxx 
 BmnEcalPoint.cxx  
 BmnEcalContFact.cxx  
 BmnEcalGeo.cxx 
 BmnEcalGeoPar.cxx
 BmnECALDigit.cxx
 BmnEcalDigitizer.cxx
)

Set(HEADERS)
Set(LINKDEF BmnEcalLinkDef.h)
Set(LIBRARY_NAME Ecal)
Set(DEPENDENCIES BmnData Base Core)

GENERATE_LIBRARY()
