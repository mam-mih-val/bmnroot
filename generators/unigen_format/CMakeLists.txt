set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/generators/unigen_format
)

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${ROOT_INCLUDE_DIR}
)

include_directories(${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
 ${ROOT_LIBRARY_DIR}
)

link_directories(${LINK_DIRECTORIES})

set(SRCS
  UEvent.cxx
  UParticle.cxx
  URun.cxx
)

Set(HEADERS
  UEvent.h
  UParticle.h
  URun.h
)

Set(HEADERS)
Set(LINKDEF UniGenLinkDef.h)
Set(LIBRARY_NAME UniGenFormat)
Set(DEPENDENCIES Core)

GENERATE_LIBRARY()
