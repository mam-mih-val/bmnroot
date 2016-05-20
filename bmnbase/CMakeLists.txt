# Create a library called "libBmnBase" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${Boost_INCLUDE_DIRS}
${ROOT_INCLUDE_DIR} 
${CMAKE_SOURCE_DIR}/fairtools
${CMAKE_SOURCE_DIR}/base
${CMAKE_SOURCE_DIR}/base/field
${CMAKE_SOURCE_DIR}/base/steer
${CMAKE_SOURCE_DIR}/base/sim
${CMAKE_SOURCE_DIR}/base/event
${CMAKE_SOURCE_DIR}/base/source
${CMAKE_SOURCE_DIR}/bmnbase
${CMAKE_SOURCE_DIR}/bmndata
${CMAKE_SOURCE_DIR}/gem
${CMAKE_SOURCE_DIR}/globaltracking
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(BMNBASE_SRCS
CbmStsTrackFinder.cxx
CbmStsTrackFitter.cxx
CbmTofMerger.cxx
CbmTrackMerger.cxx
CbmPrimaryVertexFinder.cxx
CbmFindPrimaryVertex.cxx
)

EXEC_PROGRAM( ${ROOT_CONFIG_EXECUTABLE} 
    ARGS "--has-gdml"
    OUTPUT_VARIABLE ROOT_HAS_GDML )

If ( ${ROOT_HAS_GDML} STREQUAL yes )
  Message(STATUS "Build CbmModule with Gdml support")
  Add_Definitions(-DROOT_HAS_GDML)
EndIf ( ${ROOT_HAS_GDML} STREQUAL yes )


# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h BMNBASE_HEADERS "${BMNBASE_SRCS}")

set(BMNBASE_LINKDEF BmnBaseLinkDef.h)
set(BMNBASE_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/BmnBaseDict.cxx) 

ROOT_GENERATE_DICTIONARY("${BMNBASE_HEADERS}" "${BMNBASE_LINKDEF}" "${BMNBASE_DICTIONARY}" "${INCLUDE_DIRECTORIES}")


set(BMNBASE_SRCS ${BMNBASE_SRCS} ${BMNBASE_DICTIONARY})

If ( ${ROOT_HAS_GDML} STREQUAL yes )
 Set (DEPENDENCIES ${DEPENDENCIES} Gdml)
EndIf ( ${ROOT_HAS_GDML} STREQUAL yes )

add_library(BmnBase SHARED ${BMNBASE_SRCS})
target_link_libraries(BmnBase ${ROOT_LIBRARIES})
set_target_properties(BmnBase PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})

################ install ###################
install(TARGETS BmnBase DESTINATION ${CMAKE_BINARY_DIR}/lib)


