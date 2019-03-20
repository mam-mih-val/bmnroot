# Create a library called "libUniDb"

set(INCLUDE_DIRECTORIES
 ${ROOT_INCLUDE_DIR}
 ${XML2_INCLUDE_DIR}
 ${CMAKE_SOURCE_DIR}/uni_db
 ${CMAKE_SOURCE_DIR}/uni_db/db_classes
 ${CMAKE_SOURCE_DIR}/uni_db/elog_classes
)

Set(SYSTEM_INCLUDE_DIRECTORIES
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
UniDbConnection.cxx
UniDbGeoConverter.cxx
UniDbGenerateClasses.cxx
UniDbTangoData.cxx
UniDbSearchCondition.cxx
UniDbParser.cxx

db_classes/UniDbRunPeriod.cxx
db_classes/UniDbRun.cxx
db_classes/UniDbRunGeometry.cxx
db_classes/UniDbDetector.cxx
db_classes/UniDbParameter.cxx
db_classes/UniDbDetectorParameter.cxx
db_classes/UniDbSimulationFile.cxx

elog_classes/ElogDbPerson.cxx
elog_classes/ElogDbType.cxx
elog_classes/ElogDbTrigger.cxx
elog_classes/ElogDbBeam.cxx
elog_classes/ElogDbTarget.cxx
elog_classes/ElogDbRecord.cxx
elog_classes/ElogDbAttachment.cxx
)

set(HEADERS
UniDbConnection.h
UniDbGeoConverter.h
UniDbGenerateClasses.h
UniDbTangoData.h
UniDbSearchCondition.h
UniDbParser.h

db_settings.h
# add definitions of the possible database structures and enums
db_structures.h

db_classes/UniDbRunPeriod.h
db_classes/UniDbRun.h
db_classes/UniDbRunGeometry.h
db_classes/UniDbDetector.h
db_classes/UniDbParameter.h
db_classes/UniDbDetectorParameter.h
db_classes/UniDbSimulationFile.h

elog_classes/ElogDbPerson.h
elog_classes/ElogDbType.h
elog_classes/ElogDbTrigger.h
elog_classes/ElogDbBeam.h
elog_classes/ElogDbTarget.h
elog_classes/ElogDbRecord.h
elog_classes/ElogDbAttachment.h
)

Set(LINKDEF UniDbLinkDef.h)
Set(LIBRARY_NAME UniDb)
Set(DEPENDENCIES Core Geom Net xml2)

GENERATE_LIBRARY()

add_executable (show_simulation_files show_simulation_files.cxx)
target_link_libraries(show_simulation_files ${ROOT_LIBRARIES})

add_executable (show_experiment_files show_experiment_files.cxx)
target_link_libraries(show_experiment_files ${ROOT_LIBRARIES})

INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/docs/ DESTINATION ${CMAKE_INSTALL_PREFIX}/uni_db/docs)
INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/examples/ DESTINATION ${CMAKE_INSTALL_PREFIX}/uni_db/examples)
INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/macros/ DESTINATION ${CMAKE_INSTALL_PREFIX}/uni_db/macros)
INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/scheme/ DESTINATION ${CMAKE_INSTALL_PREFIX}/uni_db/scheme)
