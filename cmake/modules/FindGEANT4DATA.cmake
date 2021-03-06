 ################################################################################
 #    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    #
 #                                                                              #
 #              This software is distributed under the terms of the             # 
 #              GNU Lesser General Public Licence (LGPL) version 3,             #  
 #                  copied verbatim in the file "LICENSE"                       #
 ################################################################################
# - Try to find GEANT4 data files
#


MESSAGE(STATUS "Looking for GEANT4 DATA files...")

FIND_PATH(GEANT4_DATA NAMES $ENV{G4PIIDATA} G4PII G4PII1.3 PATHS
  ${SIMPATH}/share/Geant4/data
  $ENV{G4INSTALL_DATA}
  ${GEANT4_SHARE_DIR}/$ENV{GEANT4VERSION}/data
  NO_DEFAULT_PATH
)

#If(DEFINED ENV{G4LEDATA})
#  Set(G4LEDATA $ENV{G4LEDATA})
#Else()
  FIND_PATH(G4LEDATA NAMES README PATHS
    $ENV{G4LEDATA}
    ${GEANT4_DATA}/G4EMLOW
    NO_DEFAULT_PATH
  )
#EndIf()

If (G4LEDATA)
  Message(STATUS "Found G4EMLOW data")
Else (G4LEDATA)
  Message(STATUS "NOT Found G4EMLOW data")
EndIf (G4LEDATA)

#If(DEFINED ENV{G4LEVELGAMMADATA})
#  Set(G4LEVELGAMMADATA $ENV{G4LEVELGAMMADATA})
#Else()
  FIND_PATH(G4LEVELGAMMADATA  NAMES README-LevelGammaData PATHS
    $ENV{G4LEVELGAMMADATA}
    ${GEANT4_DATA}/PhotonEvaporation/
    NO_DEFAULT_PATH
  )
#EndIf()

If (G4LEVELGAMMADATA)
  Message(STATUS "Found PhotonEvaporation data")
Else (G4LEVELGAMMADATA)
  Message(STATUS "NOT Found PhotonEvaporation data")
EndIf (G4LEVELGAMMADATA)


#If(DEFINED ENV{G4NEUTRONHPDATA})
#  Set(G4NEUTRONHPDATA $ENV{G4NEUTRONHPDATA})
#Else()
  FIND_PATH(G4NEUTRONHPDATA NAMES README  PATHS
    $ENV{G4NEUTRONHPDATA}
    ${GEANT4_DATA}/G4NDL
    NO_DEFAULT_PATH
  )
#EndIf()

If (G4NEUTRONHPDATA)
  Message(STATUS "Found G4NDL data")
  SET(G4NeutronHPCrossSections ${G4NEUTRONHPDATA})
Else ()
  Message(STATUS "NOT Found G4NDL data")
EndIf ()

#If(DEFINED ENV{G4NEUTRONXSDATA})
#  Set(G4NEUTRONXSDATA $ENV{G4NEUTRONXSDATA})
#Else()
  FIND_PATH(G4NEUTRONXSDATA NAMES cap1  PATHS
    $ENV{G4NEUTRONXSDATA}
    ${GEANT4_DATA}/G4NEUTRONXS
    NO_DEFAULT_PATH
  )
#EndIf()

If (G4NEUTRONXSDATA)
  Message(STATUS "Found G4NEUTRONXS data")
Else (G4NEUTRONXSDATA)
  Message(STATUS "NOT Found G4NEUTRONXS data")
EndIf (G4NEUTRONXSDATA)

#If(DEFINED ENV{G4PARTICLEXSDATA})
#  Set(G4PARTICLEXSDATA $ENV{G4PARTICLEXSDATA})
#Else()
  FIND_PATH(G4PARTICLEXSDATA NAMES He3  PATHS
    $ENV{G4PARTICLEXSDATA}
    ${GEANT4_DATA}/G4PARTICLEXS
    NO_DEFAULT_PATH
  )
#EndIf()

If (G4PARTICLEXSDATA)
  Message(STATUS "Found G4PARTICLEXS data")
Else (G4PARTICLEXSDATA)
  Message(STATUS "NOT Found G4PARTICLEXS data")
EndIf (G4PARTICLEXSDATA)

Message("G4PIIDATA: $ENV{G4PIIDATA}")
#If(DEFINED ENV{G4PIIDATA})
#  Set(G4PIIDATA $ENV{G4PIIDATA})
#Else()
  FIND_PATH(G4PIIDATA NAMES PIXE_DataLibrary.pdf documentation.pdf PATHS
    $ENV{G4PIIDATA}
    ${GEANT4_DATA}/G4PII
    NO_DEFAULT_PATH
  )
#EndIf()


If (G4PIIDATA)
  Message(STATUS "Found G4PII data")
Else (G4PIIDATA)
  Message(STATUS "NOT Found G4PII data")
EndIf (G4PIIDATA)


#If(DEFINED ENV{G4RADIOACTIVEDATA})
#  Set(G4RADIOACTIVEDATA $ENV{G4RADIOACTIVEDATA})
#Else()
  FIND_PATH(G4RADIOACTIVEDATA NAMES README_RDM PATHS
    $ENV{G4RADIOACTIVEDATA}
    ${GEANT4_DATA}/RadioactiveDecay
    NO_DEFAULT_PATH
  )
#EndIf()

If (G4RADIOACTIVEDATA)
  Message(STATUS "Found RadioactiveDecay data")
Else (G4RADIOACTIVEDATA)
  Message(STATUS "NOT Found RadioactiveDecay data")
EndIf (G4RADIOACTIVEDATA)


#If(DEFINED ENV{G4REALSURFACEDATA})
#  Set(G4REALSURFACEDATA $ENV{G4REALSURFACEDATA})
#Else()
  FIND_PATH(G4REALSURFACEDATA NAMES EtchedTiO.dat Rough_LUT.dat PATHS
    $ENV{G4REALSURFACEDATA}
    ${GEANT4_DATA}/RealSurface
    NO_DEFAULT_PATH
  )
#EndIf()

If (G4REALSURFACEDATA)
  Message(STATUS "Found RealSurface data")
Else (G4REALSURFACEDATA)
  Message(STATUS "NOT Found RealSurface data")
EndIf (G4REALSURFACEDATA)

#If(DEFINED ENV{G4SAIDXSDATA})
#  Set(G4SAIDXSDATA $ENV{G4SAIDXSDATA})
#Else()
  FIND_PATH(G4SAIDXSDATA NAMES README_Eta PATHS
    $ENV{G4SAIDXSDATA}
    ${GEANT4_DATA}/G4SAIDDATA
    NO_DEFAULT_PATH
  )
#EndIf()

If (G4SAIDXSDATA)
  Message(STATUS "Found G4SAID data")
Else (G4SAIDXSDATA)
  Message(STATUS "NOT Found G4SAID data")
EndIf (G4SAIDXSDATA)

#If(DEFINED ENV{G4ENSDFSTATEDATA})
#  Set(G4ENSDFSTATEDATA $ENV{G4ENSDFSTATEDATA})
#Else()
  FIND_PATH(G4ENSDFSTATEDATA NAMES ENSDFSTATE.dat PATHS
    $ENV{G4ENSDFSTATEDATA}
    ${GEANT4_DATA}/G4ENSDFSTATE
    NO_DEFAULT_PATH
  )
#EndIf()

If (G4ENSDFSTATEDATA)
  Message(STATUS "Found G4ENSDFSTATE data")
Else (G4ENSDFSTATEDATA)
  Message(STATUS "NOT Found G4ENSDFSTATE data")
EndIf (G4ENSDFSTATEDATA)
