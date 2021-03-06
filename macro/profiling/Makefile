# Source files
#SRCS=eventdisplay.cxx

#SRCS=run8_sim_bmn.cxx
#SRCS=run_sim_bmn.cxx
#SRCS=run_sim_src.cxx

#SRCS=run8_reco_bmn.cxx
#SRCS=run_reco_bmn.cxx
#SRCS=run_reco_src.cxx

# Compiler
CC=g++

OBJECTS=$(patsubst %.cxx, %, $(SRCS))

ROOTCONFIG := root-config

CFLAGS := $(shell $(ROOTCONFIG) --cflags)
CFLAGS += -I$(VMCWORKDIR)/bmnfield
CFLAGS += -I$(VMCWORKDIR)/generators
CFLAGS += -I$(VMCWORKDIR)/passive
CFLAGS += -I$(VMCWORKDIR)/alignment
CFLAGS += -I$(VMCWORKDIR)/armTriggers
CFLAGS += -I$(VMCWORKDIR)/bd
CFLAGS += -I$(VMCWORKDIR)/bc
CFLAGS += -I$(VMCWORKDIR)/bmndata
CFLAGS += -I$(VMCWORKDIR)/bmndata/cbm
CFLAGS += -I$(VMCWORKDIR)/bmndata/common
CFLAGS += -I$(VMCWORKDIR)/bmndata/math
CFLAGS += -I$(VMCWORKDIR)/cat
CFLAGS += -I$(VMCWORKDIR)/cat/OffLineInterface
CFLAGS += -I$(VMCWORKDIR)/csc
CFLAGS += -I$(VMCWORKDIR)/dch
CFLAGS += -I$(VMCWORKDIR)/ecal
CFLAGS += -I$(VMCWORKDIR)/FD
CFLAGS += -I$(VMCWORKDIR)/fhcal
CFLAGS += -I$(VMCWORKDIR)/gem
CFLAGS += -I$(VMCWORKDIR)/gconfig
CFLAGS += -I$(VMCWORKDIR)/hodo
CFLAGS += -I$(VMCWORKDIR)/identification
CFLAGS += -I$(VMCWORKDIR)/KF
CFLAGS += -I$(VMCWORKDIR)/KF/Interface
CFLAGS += -I$(VMCWORKDIR)/KF/KFQA
CFLAGS += -I$(VMCWORKDIR)/sts
CFLAGS += -I$(VMCWORKDIR)/mwpc
CFLAGS += -I$(VMCWORKDIR)/scwall
CFLAGS += -I$(VMCWORKDIR)/silicon
CFLAGS += -I$(VMCWORKDIR)/SiBT
CFLAGS += -I$(VMCWORKDIR)/SiMD
CFLAGS += -I$(VMCWORKDIR)/tof
CFLAGS += -I$(VMCWORKDIR)/tracking
CFLAGS += -I$(VMCWORKDIR)/tof1
CFLAGS += -I$(VMCWORKDIR)/QA
CFLAGS += -I$(VMCWORKDIR)/zdc
CFLAGS += -I$(VMCWORKDIR)/land
CFLAGS += -I$(VMCWORKDIR)/csc
CFLAGS += -I$(VMCWORKDIR)/steering
#CFLAGS += -I$(VMCWORKDIR)/ssd/mc
CFLAGS += -I$(VMCWORKDIR)/bmndst
CFLAGS += -I$(VMCWORKDIR)/decoder
CFLAGS += -I$(VMCWORKDIR)/globaltracking
CFLAGS += -I$(VMCWORKDIR)/eventdisplay
CFLAGS += -I$(VMCWORKDIR)/eventdisplay/bmn
CFLAGS += -I$(VMCWORKDIR)/eventdisplay/mpd
CFLAGS += -I$(VMCWORKDIR)/database/uni_db
CFLAGS += -I$(VMCWORKDIR)/database/unicom
CFLAGS += -I$(VMCWORKDIR)/zdc
CFLAGS += -I${FAIRROOTPATH}/include
CFLAGS += -I$(SIMPATH)/include
CFLAGS += -I$(ROOT_INCLUDE_PATH)
CFLAGS += -I$(subst :, -I,$(ROOT_INCLUDE_DIR))
CFLAGS += -I$(Geant3_INCLUDE_DIRS)
CFLAGS += -I$(subst :, -I,$(GEANT4_INCLUDE_DIR))
CFLAGS += -I$(GEANT4VMC_INCLUDE_DIR)
CFLAGS += --std=c++17 -g -O0 

LDFLAGS := $(shell $(ROOTCONFIG) --ldflags)
LDFLAGS += -lEG $(shell $(ROOTCONFIG) --glibs)
LDFLAGS += -L$(VMCWORKDIR)/build/lib
LDFLAGS += -L$(GEANT3_LIBRARY_DIR) -L$(GEANT4_LIB_DIR) -L$(GEANT4VMC_LIBRARY_DIR)
LDFLAGS += -L$(ROOT_LIBRARY_DIR) -L$(FAIRROOTPATH)/lib #-L/usr/lib64
LDFLAGS += -lGlobalTracking -lBmnAlignment -lEventDisplay -lDecoder -lBmnSteering -lBmnDst -lCSC -lLAND -lSilicon -lMwpc
LDFLAGS += -lSts -lBd -lEcal -lZdc -lTOF -lGem -lTOF1 -lDch -lBmnPidSRC
LDFLAGS += -lCat -lKF -lBmnData -lBmnField -lMpdGen -lPassive -lGen -lUniDb
LDFLAGS += -lBase -lGeoBase -lParBase -lTrkBase -lGeane -lEve -lFairTools
LDFLAGS += -lXMLParser -lXMLIO -lSpectrum -lGenVector -lMinuit -lGeom
LDFLAGS += -lGeomPainter -lVMCLibrary -lpthread -lTracking -lBC -lArmTrig -lFD -lFHCal -lGdml
LDFLAGS += -lHodo -lBmnQa -lScWall -lSiBT -lSiMD -lgeant321 -lgeant4vmc -lCLHEP-2.4.4.0 -lpythia6 -lEGPythia6

all:	$(OBJECTS)

$(OBJECTS): % : $(SRCS)
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

clean:
	rm -vf $(basename $(SRCS)) *.o

