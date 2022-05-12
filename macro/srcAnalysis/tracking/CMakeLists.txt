set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/base
 ${CMAKE_SOURCE_DIR}/base/data
 ${CMAKE_SOURCE_DIR}/detectors/mwpc
 ${CMAKE_SOURCE_DIR}/detectors/tof
 ${CMAKE_SOURCE_DIR}/detectors/gem
)

Set(SYSTEM_INCLUDE_DIRECTORIES
 ${ROOT_INCLUDE_DIR}
)

include_directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
 ${ROOT_LIBRARY_DIR}
 ${FAIRROOT_LIBRARY_DIR}
 ${GSL_LIBRARY_DIRS}
 ${Boost_LIBRARY_DIRS}
)

link_directories(${LINK_DIRECTORIES})

add_executable(track_arms track_arms.cpp SRCEvent.cpp)
target_link_libraries(track_arms Mwpc BmnBase ${ROOT_LIBRARIES} Minuit Spectrum GenVector MathMore)

INSTALL(PROGRAMS ${EXECUTABLE_OUTPUT_PATH}/track_arms DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/)
