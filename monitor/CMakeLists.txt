# Create a library called "libBmnMonitor" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

Set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/bmndata
 ${CMAKE_SOURCE_DIR}/bmndata/math
 ${CMAKE_SOURCE_DIR}/bmnfield
 ${CMAKE_SOURCE_DIR}/gem
 ${CMAKE_SOURCE_DIR}/silicon
 ${CMAKE_SOURCE_DIR}/dch
 ${CMAKE_SOURCE_DIR}/globaltracking
 ${CMAKE_SOURCE_DIR}/decoder
 ${CMAKE_SOURCE_DIR}/monitor
 ${CMAKE_SOURCE_DIR}/uni_db
 ${CMAKE_SOURCE_DIR}/uni_db/db_classes
)

Set(SYSTEM_INCLUDE_DIRECTORIES
 ${Boost_INCLUDE_DIRS}
 ${ROOT_INCLUDE_DIR}
 ${XML2_INCLUDE_DIR}
)

include_directories(${INCLUDE_DIRECTORIES})
include_directories(${SYSTEM_INCLUDE_DIRECTORIES})

Set(LINK_DIRECTORIES
 ${ROOT_LIBRARY_DIR}
 ${FAIRROOT_LIBRARY_DIR}
)

link_directories(${LINK_DIRECTORIES})

# List of source files
Set(SRCS
BmnRunInfo.cxx
BmnMonitor.cxx
BmnDataReceiver.cxx
BmnOnlineDecoder.cxx
BmnHist.cxx
BmnHistGem.cxx
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
)

If (Boost_FOUND)
  set(DEFINITIONS BOOST_FOUND)
EndIf (Boost_FOUND)

Set(HEADERS)
Set(LINKDEF MonitorLinkDef.h)
Set(LIBRARY_NAME BmnMonitor)
Set(DEPENDENCIES Dch Sts BmnData Base Gem Silicon Decoder)

GENERATE_LIBRARY()
