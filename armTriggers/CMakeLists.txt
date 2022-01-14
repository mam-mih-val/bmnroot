# Create a library called "libArmTrig"

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/bmndata
 ${CMAKE_SOURCE_DIR}/armTriggers
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
BmnArmTrig.cxx 
BmnArmTrigPoint.cxx
BmnArmTrigHit.cxx
BmnArmTrigHitProducer.cxx
#BmnArmTrigContFact.cxx  
#BmnArmTrigGeo.cxx 
#BmnArmTrigGeoPar.cxx
)

Set(HEADERS)
Set(LINKDEF BmnArmTrigLinkDef.h)
Set(LIBRARY_NAME ArmTrig)
Set(DEPENDENCIES BmnData Base Core)

GENERATE_LIBRARY()
