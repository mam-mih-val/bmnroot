#ifndef DCH1_TRACK_CAND_H
#define DCH1_TRACK_CAND_H

// *************************************************************************
// Author: Jan Fedorishin  e-mail: fedorisin@jinr.ru
//   
// Dch track candidate creation
//   
//
// Created: 26-05-2015
// Modified:
//
// *************************************************************************

//#include "TObject.h"
#include "TClonesArray.h"

#include "FairTask.h"
#include "Math/Point3D.h"
#include "ScalarD.h"

using namespace ROOT::Math;

//class DchTrackCand : public FairTask {
//class DchTrackCand : public TObject {
class DchTrackCand : public TNamed {


protected:

  UInt_t nEvents;

public:    
  TClonesArray  *fDchTrackCand;           //-> pointer to Dch track candidate
  //TClonesArray  *fDchTrackCand2;          //-> pointer to Dch track candidate event number
  //TClonesArray  *fDchTrackCand3;          //-> pointer to Dch track candidate number
  //TClonesArray  *fDchTrackCand4;          //-> pointer to Dch number of track candidates in event
  DchTrackCand();
  //DchTrackCand(const char *name, const char *title="DCH Task");
  ~DchTrackCand();

  Bool_t checkDTC;
  
  /*void SetDchNumberOfTracksInEvent(UInt_t trcand,UInt_t n); 
  UInt_t GetDchNumberOfTracksInEvent(UInt_t trcand); 
  void SetDchTrackCandNumber(UInt_t trcand, UInt_t n);
  UInt_t GetDchTrackCandNumber(UInt_t trcand);
  void SetDchTrackCandEventNumber(UInt_t trcand, UInt_t n);
  UInt_t GetDchTrackCandEventNumber(UInt_t trcand);
  void SetNumberOfEvents(UInt_t n);
  UInt_t GetNumberOfEvents();*/

//void SetDchTrackHits(UInt_t n, Double_t x0, Double_t y0, Double_t z0, Double_t tdc0, Double_t hitX1, Double_t hitY1, Double_t hitX1_2, Double_t hitY1_2, UShort_t ijk0, Double_t x1, Double_t y1, Double_t z1, Double_t tdc1, Double_t hitX2, Double_t hitY2, Double_t hitX2_2, Double_t hitY2_2, UShort_t ijk1, Double_t x2, Double_t y2, Double_t z2, Double_t tdc2, Double_t hitX3, Double_t hitY3, Double_t hitX3_2, Double_t hitY3_2, UShort_t ijk2, Double_t x3, Double_t y3, Double_t z3, Double_t tdc3, Double_t hitX4, Double_t hitY4, Double_t hitX4_2, Double_t hitY4_2, UShort_t ijk3);
//void SetDchTrackHits(UInt_t n, Double_t x0, Double_t y0, Double_t z0, Double_t tdc0, Double_t hitX1, Double_t hitY1, Double_t hitZ1, Double_t hitX1_2, Double_t hitY1_2, Double_t hitZ1_2, UShort_t ijk0, Double_t x1, Double_t y1, Double_t z1, Double_t tdc1, Double_t hitX2, Double_t hitY2, Double_t hitZ2, Double_t hitX2_2, Double_t hitY2_2, Double_t hitZ2_2, UShort_t ijk1, Double_t x2, Double_t y2, Double_t z2, Double_t tdc2, Double_t hitX3, Double_t hitY3, Double_t hitZ3, Double_t hitX3_2, Double_t hitY3_2, Double_t hitZ3_2, UShort_t ijk2, Double_t x3, Double_t y3, Double_t z3, Double_t tdc3, Double_t hitX4, Double_t hitY4, Double_t hitZ4, Double_t hitX4_2, Double_t hitY4_2, Double_t hitZ4_2, UShort_t ijk3) {

void SetDchTrackHits(const UInt_t n, const XYZPoint xyzHit1, const XYZPoint xyzWire1, const XYZPoint xyzWire1_2, const XYZPoint xyzHit2, const XYZPoint xyzWire2, const XYZPoint xyzWire2_2, const XYZPoint xyzHit3, const XYZPoint xyzWire3, const XYZPoint xyzWire3_2, const XYZPoint xyzHit4, const XYZPoint xyzWire4, const XYZPoint xyzWire4_2, ScalarD* drifttim[], const UShort_t ijk[]);
ClassDef(DchTrackCand,1)      // Dch Track Candidate

};

#endif
