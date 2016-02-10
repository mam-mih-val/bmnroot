# Create a library called "libBase" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR}
${PLUTO_INCLUDE_DIR} 
${CMAKE_SOURCE_DIR}/fairtools
${CMAKE_SOURCE_DIR}/geobase 
${CMAKE_SOURCE_DIR}/field 
${CMAKE_SOURCE_DIR}/base 
${CMAKE_SOURCE_DIR}/generators 
${CMAKE_SOURCE_DIR}/gconfig
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${PLUTO_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS
FairAsciiGenerator.cxx
FairBoxGenerator.cxx
FairIonGenerator.cxx
FairParticleGenerator.cxx
FairShieldGenerator.cxx
FairPlutoReactionGenerator.cxx   
FairEvtGenGenerator.cxx

MpdMCEventHeader.cxx
MpdLAQGSMGenerator.cxx
MpdHypYPtGenerator.cxx
MpdRainGenerator.cxx
MpdPlutoGenerator.cxx
MpdPHSDGenerator.cxx
MpdUrqmdGenerator.cxx
MpdGetNumEvents.cxx
MpdHistoGenerator.cxx
)

Set(HEADERS )
Set(LINKDEF GenLinkDef.h)
Set(LIBRARY_NAME Gen)
Set(DEPENDENCIES Base EG MathCore Physics RIO Cint Core)

GENERATE_LIBRARY()

