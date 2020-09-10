# Source files
#SRCS=eventdisplay.cxx
#SRCS=run_sim_bmn.cxx
#SRCS=run_reco_bmn.cxx
SRCS=$(wildcard *.cxx)

# Compiler
CC=g++

OBJECTS=$(patsubst %.cxx, %, $(SRCS))

ROOTCONFIG := root-config

CFLAGS := $(shell $(ROOTCONFIG) --cflags)
CFLAGS += -I$(VMCWORKDIR)/bmnfield
CFLAGS += -I$(VMCWORKDIR)/generators
CFLAGS += -I$(VMCWORKDIR)/passive
CFLAGS += -I$(VMCWORKDIR)/alignment
CFLAGS += -I$(VMCWORKDIR)/bd
CFLAGS += -I$(VMCWORKDIR)/bmndata
CFLAGS += -I$(VMCWORKDIR)/bmndata/cbm
CFLAGS += -I$(VMCWORKDIR)/bmndata/common
CFLAGS += -I$(VMCWORKDIR)/bmndata/math
CFLAGS += -I$(VMCWORKDIR)/cat
CFLAGS += -I$(VMCWORKDIR)/dch
CFLAGS += -I$(VMCWORKDIR)/ecal
CFLAGS += -I$(VMCWORKDIR)/gem
CFLAGS += -I$(VMCWORKDIR)/KF
CFLAGS += -I$(VMCWORKDIR)/sts
CFLAGS += -I$(VMCWORKDIR)/mwpc
CFLAGS += -I$(VMCWORKDIR)/psd
CFLAGS += -I$(VMCWORKDIR)/recoil
CFLAGS += -I$(VMCWORKDIR)/silicon
CFLAGS += -I$(VMCWORKDIR)/tof
CFLAGS += -I$(VMCWORKDIR)/tof1
CFLAGS += -I$(VMCWORKDIR)/zdc
CFLAGS += -I$(VMCWORKDIR)/land
CFLAGS += -I$(VMCWORKDIR)/csc
CFLAGS += -I$(VMCWORKDIR)/steering
CFLAGS += -I$(VMCWORKDIR)/bmndst
CFLAGS += -I$(VMCWORKDIR)/decoder
CFLAGS += -I$(VMCWORKDIR)/globaltracking
CFLAGS += -I$(VMCWORKDIR)/eventdisplay
CFLAGS += -I$(VMCWORKDIR)/eventdisplay/bmn
CFLAGS += -I$(VMCWORKDIR)/eventdisplay/mpd
CFLAGS += -I$(VMCWORKDIR)/uni_db
CFLAGS += -I$(VMCWORKDIR)/uni_db/db_classes
CFLAGS += -I${FAIRROOTPATH}/include
CFLAGS += -I$(SIMPATH)/include
CFLAGS += -I$(ROOT_INCLUDE_PATH)
CFLAGS += -I$(ROOT_INCLUDE_DIR)
CFLAGS += --std=c++11 -g -O0

LDFLAGS := $(shell $(ROOTCONFIG) --ldflags)
LDFLAGS += -lEG $(shell $(ROOTCONFIG) --glibs)
LDFLAGS += -L$(VMCWORKDIR)/build/lib
LDFLAGS += -L$(ROOT_LIBRARY_DIR) -L$(FAIRROOTPATH)/lib #-L/usr/lib64
LDFLAGS += -lGlobalTracking -lBmnAlignment -lEventDisplay -lDecoder -lBmnSteering -lBmnDst -lCSC -lLAND -lSilicon -lMwpc
LDFLAGS += -lSts -lBd -lEcal -lZdc -lRecoil -lPsd -lTOF -lGem -lTOF1 -lDch
LDFLAGS += -lCat -lKF -lBmnData -lBmnField -lMpdGen -lPassive -lGen -lUniDb
LDFLAGS += -lBase -lGeoBase -lParBase -lTrkBase -lGeane -lEve -lFairTools
LDFLAGS += -lXMLParser -lXMLIO -lSpectrum -lGenVector -lMinuit -lGeom
LDFLAGS += -lGeomPainter -lVMC -lpthread


all:	$(OBJECTS)

$(OBJECTS): % : $(SRCS)
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

clean:
	rm -vf $(basename $(SRCS)) *.o

