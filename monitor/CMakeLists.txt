
Set(INCLUDE_DIRECTORIES
${BASE_INCLUDE_DIRECTORIES}
${ROOT_INCLUDE_DIR}
${Boost_INCLUDE_DIRS}

${BMNROOT_SOURCE_DIR}/base
${CMAKE_SOURCE_DIR}/base/field
${CMAKE_SOURCE_DIR}/base/steer
${CMAKE_SOURCE_DIR}/base/sim
${CMAKE_SOURCE_DIR}/base/event
${CMAKE_SOURCE_DIR}/base/source
${BMNROOT_SOURCE_DIR}/parbase
${BMNROOT_SOURCE_DIR}/fairtools
${BMNROOT_SOURCE_DIR}/bmnbase
${BMNROOT_SOURCE_DIR}/bmndata
${BMNROOT_SOURCE_DIR}/bmnfield
${BMNROOT_SOURCE_DIR}/gem
${BMNROOT_SOURCE_DIR}/globaltracking
${BMNROOT_SOURCE_DIR}/monitor
/usr/include/libxml2
${CMAKE_SOURCE_DIR}/uni_db
${CMAKE_SOURCE_DIR}/uni_db/db_classes
#${BMNROOT_SOURCE_DIR}/sts
#${BMNROOT_SOURCE_DIR}/sts/setup

#${BMNROOT_SOURCE_DIR}/KF
#${BMNROOT_SOURCE_DIR}/KF/Interface
)

include_directories( ${INCLUDE_DIRECTORIES})

Set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)

link_directories( ${LINK_DIRECTORIES})

Set(SRCS
BmnMonitor.cxx
BmnDataReceiver.cxx
#BmnOnlineDecoder.cxx
BmnHistGem.cxx
BmnHistDch.cxx
BmnHistTrigger.cxx
BmnHistToF.cxx
BmnHistToF700.cxx

)

Set(HEADERS
BmnMonitor.h
BmnDataReceiver.h
#BmnOnlineDecoder.h
BmnHistGem.h
BmnHistDch.h
BmnHistTrigger.h
BmnHistToF.h
BmnHistToF700.h

)

If (Boost_FOUND)
  set(DEFINITIONS BOOST_FOUND)
EndIf (Boost_FOUND)

Set(LINKDEF  LinkDef.h)
Set(LIBRARY_NAME BmnMonitor)
Set(DEPENDENCIES
    KF Sts BmnBase BmnData Base 
)

#target_link_libraries(BmnMonitor ${INCLUDE_DIRECTORIES} zmq)

GENERATE_LIBRARY()

