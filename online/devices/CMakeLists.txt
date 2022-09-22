# Create a library called "libBmnOnlineDevices"

set(INCLUDE_DIRECTORIES
  ${BASE_INCLUDE_DIRECTORIES}
  ${CMAKE_SOURCE_DIR}/database/unicom
  ${CMAKE_SOURCE_DIR}/database/uni_db
  ${CMAKE_SOURCE_DIR}/database/elog_db
  ${CMAKE_SOURCE_DIR}/database/tango
  ${CMAKE_SOURCE_DIR}/field
  ${CMAKE_SOURCE_DIR}/base
  ${CMAKE_SOURCE_DIR}/base/data
  ${CMAKE_SOURCE_DIR}/base/raw
  ${CMAKE_SOURCE_DIR}/base/header
  ${CMAKE_SOURCE_DIR}/base/math
  ${CMAKE_SOURCE_DIR}/base/cbm
  ${CMAKE_SOURCE_DIR}/decoder
  ${CMAKE_SOURCE_DIR}/detectors/SiBT
  ${CMAKE_SOURCE_DIR}/detectors/silicon
  ${CMAKE_SOURCE_DIR}/detectors/gem
  ${CMAKE_SOURCE_DIR}/detectors/csc
  ${CMAKE_SOURCE_DIR}/detectors/tof
  ${CMAKE_SOURCE_DIR}/detectors/tof1
  ${CMAKE_SOURCE_DIR}/detectors/dch
  ${CMAKE_SOURCE_DIR}/detectors/fhcal
  ${CMAKE_SOURCE_DIR}/detectors/sts
  ${CMAKE_SOURCE_DIR}/reconstruction/tof2
  ${CMAKE_SOURCE_DIR}/reconstruction/fhcal
  ${CMAKE_SOURCE_DIR}/services
  ${CMAKE_SOURCE_DIR}/online/devices
)

set(SYSTEM_INCLUDE_DIRECTORIES
  ${ROOT_INCLUDE_DIR}
)

set(LINK_DIRECTORIES
  ${ROOT_LIBRARY_DIR}
  ${FAIRROOT_LIBRARY_DIR}
)

include_directories(${INCLUDE_DIRECTORIES})
include_directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

link_directories(${LINK_DIRECTORIES})

set(SRCS
  BmnOnlineSampler.cxx
  BmnOnlineProcessor.cxx
  BmnOnlineSink.cxx
)

Set(HEADERS)
set(LIBRARY_NAME BmnOnlineDevices)
set(DEPENDENCIES BmnRecoFHCal BmnRecoTof2 Silicon Gem Sts CSC FHCal Ecal Dch TOF1 TOF Mwpc UniDb FairMQ Base Core rt)

GENERATE_LIBRARY()
