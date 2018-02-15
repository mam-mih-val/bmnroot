set(INCLUDE_DIRECTORIES
  ${CMAKE_SOURCE_DIR}/base/field
  ${CMAKE_SOURCE_DIR}/base/steer
  ${CMAKE_SOURCE_DIR}/base/sim
  ${CMAKE_SOURCE_DIR}/base/event
  ${CMAKE_SOURCE_DIR}/base/source
  ${CMAKE_SOURCE_DIR}/geobase
  ${CMAKE_SOURCE_DIR}/parbase
  ${CMAKE_SOURCE_DIR}/fairtools
  ${CMAKE_SOURCE_DIR}/bmndata
  ${CMAKE_SOURCE_DIR}/bmnbase
  ${CMAKE_SOURCE_DIR}/globaltracking
  ${CMAKE_SOURCE_DIR}/gem
  ${CMAKE_SOURCE_DIR}/silicon
  ${CMAKE_SOURCE_DIR}/uni_db
  ${CMAKE_SOURCE_DIR}/uni_db/db_classes
  )

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${Boost_INCLUDE_DIRS}
  ${ROOT_INCLUDE_DIR}
  )

include_directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
  ${ROOT_LIBRARY_DIR}
  )

link_directories(${LINK_DIRECTORIES})

add_executable(track_arms track_arms.cpp SRCEvent.cpp)
target_link_libraries(track_arms BmnData Silicon TOF1 UniDb Gem GlobalTracking KF Mwpc Dch ${ROOT_LIBRARIES} XMLParser Minuit Spectrum GenVector MathMore)
