# Create a library called "libBmnMonitor" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

Set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/uni_db
 ${CMAKE_SOURCE_DIR}/uni_db/db_classes
 ${CMAKE_SOURCE_DIR}/bmnfield
 ${CMAKE_SOURCE_DIR}/bmndata
 ${CMAKE_SOURCE_DIR}/bmndata/math
 ${CMAKE_SOURCE_DIR}/bmndata/cbm
 ${CMAKE_SOURCE_DIR}/QA
 ${CMAKE_SOURCE_DIR}/QA/report
 ${CMAKE_SOURCE_DIR}/mwpc
 ${CMAKE_SOURCE_DIR}/csc
 ${CMAKE_SOURCE_DIR}/dch
 ${CMAKE_SOURCE_DIR}/silicon
 ${CMAKE_SOURCE_DIR}/gem
 ${CMAKE_SOURCE_DIR}/alignment
 ${CMAKE_SOURCE_DIR}/decoder
 ${CMAKE_SOURCE_DIR}/globaltracking
 ${CMAKE_SOURCE_DIR}/monitor
)

Set(SYSTEM_INCLUDE_DIRECTORIES
 ${ROOT_INCLUDE_DIR}
 ${XML2_INCLUDE_DIR}
 ${ZeroMQ_INCLUDE_DIR}
 ${Boost_INCLUDE_DIRS}
)

include_directories(${INCLUDE_DIRECTORIES})
include_directories(SYSTEM  ${SYSTEM_INCLUDE_DIRECTORIES})

Set(LINK_DIRECTORIES
 ${ROOT_LIBRARY_DIR}
 ${FAIRROOT_LIBRARY_DIR}
)

link_directories(${LINK_DIRECTORIES})

# List of source files
Set(SRCS
BmnMonitor.cxx
BmnDataReceiver.cxx
BmnOnlineDecoder.cxx
BmnOnlineReco.cxx
BmnHist.cxx
BmnHistGem.cxx
BmnHistCsc.cxx
BmnHistSilicon.cxx
BmnHistDch.cxx
BmnHistMwpc.cxx
BmnHistZDC.cxx
BmnHistECAL.cxx
BmnHistTrigger.cxx
BmnHistToF.cxx
BmnHistToF700.cxx
BmnHistSrc.cxx
BmnHistLAND.cxx
PadInfo.cxx
BmnPadBranch.cxx
BmnPadGenerator.cxx
)

If (Boost_FOUND)
  set(DEFINITIONS BOOST_FOUND)
EndIf (Boost_FOUND)

Set(HEADERS)
Set(LINKDEF MonitorLinkDef.h)
Set(LIBRARY_NAME BmnMonitor)
Set(DEPENDENCIES GlobalTracking Decoder Alignment Gem Silicon CSC Dch Mwpc BmnData BmnField UniDb Base Core ${ZeroMQ_LIBRARY_SHARED} BmnQa)

GENERATE_LIBRARY()
