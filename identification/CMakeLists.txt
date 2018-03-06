# Create a library called "libBmnIdentification" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

Set(INCLUDE_DIRECTORIES
${CMAKE_SOURCE_DIR}/base
${CMAKE_SOURCE_DIR}/base/field
${CMAKE_SOURCE_DIR}/base/steer
${CMAKE_SOURCE_DIR}/base/sim
${CMAKE_SOURCE_DIR}/base/event
${CMAKE_SOURCE_DIR}/base/source
${CMAKE_SOURCE_DIR}/parbase
${CMAKE_SOURCE_DIR}/fairtools
${CMAKE_SOURCE_DIR}/bmndata
${CMAKE_SOURCE_DIR}/bmnfield
${CMAKE_SOURCE_DIR}/gem
${CMAKE_SOURCE_DIR}/dch
${CMAKE_SOURCE_DIR}/mwpc
)

Set(SYSTEM_INCLUDE_DIRECTORIES
 ${Boost_INCLUDE_DIRS}
 ${ROOT_INCLUDE_DIR}
)

include_directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

Set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)

link_directories(${LINK_DIRECTORIES})

# List of source files
Set(SRCS
)

If (Boost_FOUND)
  set(DEFINITIONS BOOST_FOUND)
EndIf (Boost_FOUND)

Set(HEADERS)
Set(LINKDEF LinkDef.h)
Set(LIBRARY_NAME BmnIdentification)
Set(DEPENDENCIES BmnData Base)

GENERATE_LIBRARY()
