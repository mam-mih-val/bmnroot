# Create a library called "libBmnField"

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/field
)

Set(SYSTEM_INCLUDE_DIRECTORIES
 ${ROOT_INCLUDE_DIR}
)

include_directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
 ${ROOT_LIBRARY_DIR}
 ${FAIRROOT_LIBRARY_DIR}
) #${FairLogger_LIBDIR}
 
link_directories(${LINK_DIRECTORIES})

# List of source files
set(SRCS
 BmnNewFieldMap.cxx
 BmnFieldConst.cxx
 BmnFieldContFact.cxx
 BmnFieldMap.cxx
 BmnFieldMapData.cxx
 BmnFieldMapSym3.cxx
 BmnFieldPar.cxx
 BmnFieldCreator.cxx
 BmnSP41FieldMapCreator.cxx
 BmnRegion.cxx
 BmnConstPar.cxx
 BmnMapPar.cxx
 BmnMultiFieldPar.cxx
 BmnMultiField.cxx
)

Set(HEADERS)
Set(LINKDEF FieldLinkDef.h)
Set(LIBRARY_NAME BmnField)
Set(DEPENDENCIES Base Core) #FairLogger

GENERATE_LIBRARY()
