# Create a library called "libBmnBase" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${CMAKE_SOURCE_DIR}/fairtools
${CMAKE_SOURCE_DIR}/base/field
${CMAKE_SOURCE_DIR}/base/steer
${CMAKE_SOURCE_DIR}/base/sim
${CMAKE_SOURCE_DIR}/base/event
${CMAKE_SOURCE_DIR}/base/source
${CMAKE_SOURCE_DIR}/parbase
${CMAKE_SOURCE_DIR}/bmnbase
${CMAKE_SOURCE_DIR}/bmndata
${CMAKE_SOURCE_DIR}/gem
${CMAKE_SOURCE_DIR}/globaltracking
)

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${ROOT_INCLUDE_DIR} 
  ${Boost_INCLUDE_DIRS}
)

Include_Directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories(${LINK_DIRECTORIES})

set(SRCS
CbmStsTrackFinder.cxx
CbmStsTrackFitter.cxx
CbmTofMerger.cxx
CbmTrackMerger.cxx
CbmPrimaryVertexFinder.cxx
CbmFindPrimaryVertex.cxx
BmnFileSource.cxx
BmnMath.cxx
)

Set(HEADERS)

EXEC_PROGRAM( ${ROOT_CONFIG_EXECUTABLE} 
    ARGS "--has-gdml"
    OUTPUT_VARIABLE ROOT_HAS_GDML )

If ( ${ROOT_HAS_GDML} STREQUAL yes )
  Message(STATUS "Build CbmModule with Gdml support")
  Add_Definitions(-DROOT_HAS_GDML)
EndIf ( ${ROOT_HAS_GDML} STREQUAL yes )

If ( ${ROOT_HAS_GDML} STREQUAL yes )
 Set (DEPENDENCIES ${DEPENDENCIES} Gdml)
EndIf ( ${ROOT_HAS_GDML} STREQUAL yes )

Set(LINKDEF BmnBaseLinkDef.h)
Set(LIBRARY_NAME BmnBase)

GENERATE_LIBRARY()
