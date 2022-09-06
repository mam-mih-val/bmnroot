# Create executable files for online processing flow

set(ONLINE_ED_DEVICES_CONFIG ${PROJECT_SOURCE_DIR}/config/devices-config.json)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/bmn-online-run.sh.in ${CMAKE_CURRENT_BINARY_DIR}/bmn-online-run.sh @ONLY)

set(INCLUDE_DIRECTORIES
  ${CMAKE_SOURCE_DIR}/base
  ${CMAKE_SOURCE_DIR}/online/devices
)

set(SYSTEM_INCLUDE_DIRECTORIES
  ${ROOT_INCLUDE_DIR}
  ${Boost_INCLUDE_DIRS}
)

set(LINK_DIRECTORIES
 ${ROOT_LIBRARY_DIR}
 ${FAIRROOT_LIBRARY_DIR}
 ${Boost_LIBRARY_DIRS}
)

include_directories(${INCLUDE_DIRECTORIES})
include_directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

link_directories(${LINK_DIRECTORIES})

set(EXE_DEPENDENCIES
  ${ROOT_LIBRARIES}
  ${Boost_PROGRAMM_OPTIONS_LIBRARY}
  ${Boost_INTERPROCESS_LIBRARY}
  rt
  BmnOnlineDevices
  BmnBase
  BmnField
  SiBT
  Silicon
  Gem
  CSC
  TOF
  TOF1
  BmnRecoTof2
  Dch
  FHCal
  BmnRecoFHCal
  Sts
  FairLogger
  FairMQ
)

add_executable(bmn-online-sampler runBmnOnlineSampler.cxx)
target_link_libraries(bmn-online-sampler PRIVATE ${EXE_DEPENDENCIES})

add_executable(bmn-online-processor runBmnOnlineProcessor.cxx)
target_link_libraries(bmn-online-processor PRIVATE ${EXE_DEPENDENCIES})

add_executable(bmn-online-sink runBmnOnlineSink.cxx)
target_link_libraries(bmn-online-sink PRIVATE ${EXE_DEPENDENCIES})

install(PROGRAMS ${EXECUTABLE_OUTPUT_PATH}/bmn-online-sampler DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/)
install(PROGRAMS ${EXECUTABLE_OUTPUT_PATH}/bmn-online-processor DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/)
install(PROGRAMS ${EXECUTABLE_OUTPUT_PATH}/bmn-online-sink DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/)
