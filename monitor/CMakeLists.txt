# Create a library called "libBmnMonitor"

Set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/database/unicom
 ${CMAKE_SOURCE_DIR}/database/uni_db
 ${CMAKE_SOURCE_DIR}/database/elog_db
 ${CMAKE_SOURCE_DIR}/database/tango
 ${CMAKE_SOURCE_DIR}/bmnfield
 ${CMAKE_SOURCE_DIR}/bmndst
 ${CMAKE_SOURCE_DIR}/bmndata
 ${CMAKE_SOURCE_DIR}/bmndata/raw
 ${CMAKE_SOURCE_DIR}/bmndata/gem
 ${CMAKE_SOURCE_DIR}/bmndata/header
 ${CMAKE_SOURCE_DIR}/bmndata/math
 ${CMAKE_SOURCE_DIR}/bmndata/cbm
 ${CMAKE_SOURCE_DIR}/bmndata/common
 ${CMAKE_SOURCE_DIR}/QA
 ${CMAKE_SOURCE_DIR}/QA/report
 ${CMAKE_SOURCE_DIR}/detectors/mwpc
 ${CMAKE_SOURCE_DIR}/detectors/csc
 ${CMAKE_SOURCE_DIR}/detectors/dch
 ${CMAKE_SOURCE_DIR}/detectors/silicon
 ${CMAKE_SOURCE_DIR}/detectors/gem
 ${CMAKE_SOURCE_DIR}/detectors/tof1
 ${CMAKE_SOURCE_DIR}/detectors/tof
 ${CMAKE_SOURCE_DIR}/detectors/tofcal
 ${CMAKE_SOURCE_DIR}/detectors/ecal
 ${CMAKE_SOURCE_DIR}/detectors/zdc
 ${CMAKE_SOURCE_DIR}/detectors/hodo
 ${CMAKE_SOURCE_DIR}/detectors/scwall
 ${CMAKE_SOURCE_DIR}/detectors/fhcal
 ${CMAKE_SOURCE_DIR}/detectors/land
 ${CMAKE_SOURCE_DIR}/alignment
 ${CMAKE_SOURCE_DIR}/decoder
 ${CMAKE_SOURCE_DIR}/KF
 ${CMAKE_SOURCE_DIR}/KF/Interface
 ${CMAKE_SOURCE_DIR}/globaltracking
 ${CMAKE_SOURCE_DIR}/monitor
 ${CMAKE_SOURCE_DIR}/steering
 ${CMAKE_SOURCE_DIR}/tracking
 ${SIMPATH}/include/fairmq
)

Set(SYSTEM_INCLUDE_DIRECTORIES
 ${ROOT_INCLUDE_DIR}
 ${XML2_INCLUDE_DIR}
 ${ZeroMQ_INCLUDE_DIR}
 ${Boost_INCLUDE_DIRS}
)

include_directories(${INCLUDE_DIRECTORIES})
include_directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

Set(LINK_DIRECTORIES
 ${ROOT_LIBRARY_DIR}
 ${FAIRROOT_LIBRARY_DIR}
 ${Boost_LIBRARY_DIRS}
)

link_directories(${LINK_DIRECTORIES})

# List of source files
Set(SRCS
BmnMonitor.cxx
BmnDataReceiver.cxx
BmnOnlineDecoder.cxx
#BmnOnlineReco.cxx
BmnHist.cxx
BmnHistGem.cxx
BmnHistCsc.cxx
BmnHistSilicon.cxx
BmnHistDch.cxx
BmnHistMwpc.cxx
BmnHistZDC.cxx
BmnHistScWall.cxx
BmnHistECAL.cxx
BmnHistTrigger.cxx
BmnHistToF.cxx
BmnHistToF700.cxx
BmnHistSrc.cxx
BmnHistLAND.cxx
BmnHistTofCal.cxx
PadInfo.cxx
BmnPadBranch.cxx
BmnPadGenerator.cxx
#BmnMQRecoTaskProcessor.cxx
#BmnMQFileSaver.cxx
# BmnMQSink.cxx
# BmnMQRunDevice.cxx
)

Set(HEADERS)
Set(LINKDEF MonitorLinkDef.h)
Set(LIBRARY_NAME BmnMonitor)
Set(DEPENDENCIES BmnQa GlobalTracking Decoder Alignment LAND FHCal ScWall Zdc Ecal TofCal TOF TOF1 Gem Silicon CSC Dch Mwpc 
                 BmnData BmnField BmnDst Tracking UniDb Base Core ${ZeroMQ_LIBRARY_SHARED}
FairMQ)

GENERATE_LIBRARY()
