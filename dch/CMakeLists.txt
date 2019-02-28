# Create a library called "libDch" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/mcstack
 ${CMAKE_SOURCE_DIR}/passive
 ${CMAKE_SOURCE_DIR}/bmndata
 ${CMAKE_SOURCE_DIR}/bmndata/math
 ${CMAKE_SOURCE_DIR}/bmnfield
 ${CMAKE_SOURCE_DIR}/gem
 ${CMAKE_SOURCE_DIR}/uni_db
 ${CMAKE_SOURCE_DIR}/uni_db/db_classes
 ${CMAKE_SOURCE_DIR}/dch
)

Set(SYSTEM_INCLUDE_DIRECTORIES
 ${Boost_INCLUDE_DIRS}
 ${ROOT_INCLUDE_DIR}
 ${GEANT3_INCLUDE_DIR}
)

include_directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
 ${ROOT_LIBRARY_DIR}
 ${FAIRROOT_LIBRARY_DIR}
)
 
link_directories(${LINK_DIRECTORIES})

# List of source files
set(SRCS
 BmnDch.cxx
 BmnDchPoint.cxx
 BmnDchContFact.cxx
 BmnDchGeo.cxx
 BmnDchGeoPar.cxx
 BmnDchHitProducer.cxx
 BmnDchHitProducerTmp.cxx
 BmnDchHitProducer_exp.cxx
 #BmnDchHitProducerCalib_exp.cxx
 DchTrackCand.cxx
 DchTrackManager.cxx
 DchTrackManagerSim.cxx
 ScalarD.cxx
 ScalarI.cxx
 ScalarUI.cxx
 DchHitPlane.cxx
 BmnDchTrackFinder.cxx
 BmnDchTrack.cxx

 DCHgeometryconstants_bmn.h
)

set(HEADERS)

Set(LINKDEF LinkDef.h)
Set(LIBRARY_NAME Dch)
Set(DEPENDENCIES BmnField BmnData Core Base)

GENERATE_LIBRARY()
