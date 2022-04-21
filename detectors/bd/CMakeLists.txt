# Create a library called "libBd"

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/base
 ${CMAKE_SOURCE_DIR}/base/data
 ${CMAKE_SOURCE_DIR}/base/raw
 ${CMAKE_SOURCE_DIR}/detectors/bd
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
 BmnBd.cxx
 BmnBdPoint.cxx
 BmnBdContFact.cxx
 BmnBdGeo.cxx
 BmnBdGeoPar.cxx
 BmnBdHit.cxx
 BmnBdHitProducer.cxx
 BmnBdDigitizer.cxx
)

Set(HEADERS)
Set(LINKDEF BdLinkDef.h)
Set(LIBRARY_NAME Bd)
Set(DEPENDENCIES BmnBase Base Core)

GENERATE_LIBRARY()
