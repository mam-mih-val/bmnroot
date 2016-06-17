# Create a library called "libUniDb" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR}
/usr/include/libxml2
${CMAKE_SOURCE_DIR}/uni_db
${CMAKE_SOURCE_DIR}/uni_db/db_classes
)

include_directories(${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories(${LINK_DIRECTORIES})

set(UNIDB_SRCS
UniDbConnection.cxx
UniDbGeoConverter.cxx
UniDbGenerateClasses.cxx
UniDbTangoData.cxx
UniDbParser.cxx
UniDbSearchCondition.cxx

db_classes/UniDbRunPeriod.cxx
db_classes/UniDbRun.cxx
db_classes/UniDbRunGeometry.cxx
db_classes/UniDbDetector.cxx
db_classes/UniDbParameter.cxx
db_classes/UniDbDetectorParameter.cxx
db_classes/UniDbSimulationFile.cxx
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h UNIDB_HEADERS "${UNIDB_SRCS}")

set(UNIDB_LINKDEF UniDbLinkDef.h)
set(UNIDB_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/UniDbDict.cxx) 

ROOT_GENERATE_DICTIONARY("${UNIDB_HEADERS}" "${UNIDB_LINKDEF}" "${UNIDB_DICTIONARY}" "${INCLUDE_DIRECTORIES}")


set(UNIDB_SRCS ${UNIDB_SRCS} ${UNIDB_DICTIONARY})


add_library(UniDb SHARED ${UNIDB_SRCS})
target_link_libraries(UniDb ${ROOT_LIBRARIES} xml2)
set_target_properties(UniDb PROPERTIES VERSION 0.0.1 SOVERSION 0)

add_executable (get_mpd_prod get_mpd_prod.cxx)
target_link_libraries(get_mpd_prod ${ROOT_LIBRARIES})

################ install ###################
install(TARGETS UniDb DESTINATION ${CMAKE_BINARY_DIR}/lib)
