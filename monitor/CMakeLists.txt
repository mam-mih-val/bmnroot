# Create a library called "libBmnMonitor"

Set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/database/uni_db
 ${CMAKE_SOURCE_DIR}/database/elog_db
 ${CMAKE_SOURCE_DIR}/database/tango
 ${CMAKE_SOURCE_DIR}/database/tools
 ${CMAKE_SOURCE_DIR}/field
 ${CMAKE_SOURCE_DIR}/base
 ${CMAKE_SOURCE_DIR}/base/data
 ${CMAKE_SOURCE_DIR}/base/raw
 ${CMAKE_SOURCE_DIR}/base/header
 ${CMAKE_SOURCE_DIR}/base/math
 ${CMAKE_SOURCE_DIR}/base/cbm
 ${CMAKE_SOURCE_DIR}/base/source
 ${CMAKE_SOURCE_DIR}/QA
 ${CMAKE_SOURCE_DIR}/QA/report
 ${CMAKE_SOURCE_DIR}/detectors/mwpc
 ${CMAKE_SOURCE_DIR}/detectors/csc
 ${CMAKE_SOURCE_DIR}/detectors/dch
 ${CMAKE_SOURCE_DIR}/detectors/silicon
 ${CMAKE_SOURCE_DIR}/detectors/gem
 ${CMAKE_SOURCE_DIR}/detectors/sts
 ${CMAKE_SOURCE_DIR}/detectors/tof1
 ${CMAKE_SOURCE_DIR}/detectors/tof
 ${CMAKE_SOURCE_DIR}/detectors/tofcal
 ${CMAKE_SOURCE_DIR}/detectors/ecal
 ${CMAKE_SOURCE_DIR}/detectors/zdc
 ${CMAKE_SOURCE_DIR}/detectors/hodo
 ${CMAKE_SOURCE_DIR}/detectors/scwall
 ${CMAKE_SOURCE_DIR}/detectors/fhcal
 ${CMAKE_SOURCE_DIR}/detectors/ndet
 ${CMAKE_SOURCE_DIR}/detectors/land
 ${CMAKE_SOURCE_DIR}/alignment
 ${CMAKE_SOURCE_DIR}/decoder
 ${CMAKE_SOURCE_DIR}/reconstruction/dst
 ${CMAKE_SOURCE_DIR}/reconstruction/tof2
 ${CMAKE_SOURCE_DIR}/reconstruction/KF
 ${CMAKE_SOURCE_DIR}/reconstruction/KF/Interface
 ${CMAKE_SOURCE_DIR}/reconstruction/tracking
 ${CMAKE_SOURCE_DIR}/reconstruction/globaltracking
 ${CMAKE_SOURCE_DIR}/monitor
 ${CMAKE_SOURCE_DIR}/steering
)

Set(SYSTEM_INCLUDE_DIRECTORIES
 ${ROOT_INCLUDE_DIR}
 ${XML2_INCLUDE_DIR}
 ${ZeroMQ_INCLUDE_DIR}
 ${Boost_INCLUDE_DIRS}
 ${FairMQ_PREFIX}/include/fairmq
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
BmnOnlineReco.cxx
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
#BmnMQRecoTaskProcessor.cxx
#BmnMQFileSaver.cxx
 BmnMQSource.cxx
 BmnMQSink.cxx
 #BmnMQRunDevice.cxx
)

add_executable(bmn-mon-prof BmnMonProf.cxx)
target_link_libraries(bmn-mon-prof PRIVATE
 ${ZeroMQ_LIBRARY_SHARED}
 Core
 RIO
 RHTTP
 Hist
 BmnDecoder
)

target_link_directories(bmn-mon-prof PRIVATE
 ${LINK_DIRECTORIES}
)

Set(HEADERS)
Set(LINKDEF MonitorLinkDef.h)
Set(LIBRARY_NAME BmnMonitor)
Set(DEPENDENCIES BmnQa BmnGlobalTracking BmnDecoder Alignment LAND FHCal ScWall Ndet Zdc Ecal TofCal TOF TOF1 Gem Silicon CSC Dch Mwpc
                 BmnBase BmnField BmnTracking DbTools ElogDb UniDb Base Core ${ZeroMQ_LIBRARY_SHARED} FairMQ)

GENERATE_LIBRARY()
