# Create a library called "libUniDb"

set(INCLUDE_DIRECTORIES
 ${CMAKE_SOURCE_DIR}/database/unicom
 ${CMAKE_SOURCE_DIR}/database/uni_db
)

Set(SYSTEM_INCLUDE_DIRECTORIES
 ${Boost_INCLUDE_DIRS}
 ${ROOT_INCLUDE_DIR}
 ${XML2_INCLUDE_DIR}
)

include_directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
 ${ROOT_LIBRARY_DIR}
 ${XML2_LIBRARY_DIR}
)
 
link_directories(${LINK_DIRECTORIES})

# List of source files
set(SRCS
 UniDbRunPeriod.cxx
 UniDbRun.cxx
 UniDbRunGeometry.cxx
 UniDbDetector.cxx
 UniDbParameter.cxx
 UniDbDetectorParameter.cxx
 UniDbSimulationFile.cxx
 
 UniParser.cxx
)

set(HEADERS)
Set(LINKDEF UniDbLinkDef.h)
Set(LIBRARY_NAME UniDb)
Set(DEPENDENCIES UniCommon Core Geom Gpad Net xml2)

GENERATE_LIBRARY()

add_executable (show_simulation_files show_simulation_files.cxx)
target_link_libraries(show_simulation_files ${ROOT_LIBRARIES})

add_executable (show_experiment_files show_experiment_files.cxx)
target_link_libraries(show_experiment_files ${ROOT_LIBRARIES})

INSTALL(PROGRAMS ${EXECUTABLE_OUTPUT_PATH}/show_simulation_files DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/)
INSTALL(PROGRAMS ${EXECUTABLE_OUTPUT_PATH}/show_experiment_files DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/)
INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/docs/ DESTINATION ${CMAKE_INSTALL_PREFIX}/database/uni_db/docs)
INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/examples/ DESTINATION ${CMAKE_INSTALL_PREFIX}/database/uni_db/examples)
INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/macros/ DESTINATION ${CMAKE_INSTALL_PREFIX}/database/uni_db/macros)
INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/scheme/ DESTINATION ${CMAKE_INSTALL_PREFIX}/database/uni_db/scheme)
