#ifndef DCH_TRACK_MANAGER_SIM_H
#define DCH_TRACK_MANAGER_SIM_H

// *************************************************************************
// Author: Jan Fedorishin  e-mail: fedorisin@jinr.ru
//   
// Dch track manager, creation and fitting of track in both DCH's
//   
//
// Created: 23-05-2015
// Modified:
//
// *************************************************************************

//#include "TObject.h"
#include "TClonesArray.h"
#include "TGraph2DErrors.h"
#include "TGraph.h"
#include "TVector2.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"
#include "TMatrix.h"
#include "TMatrixD.h"

#include "DchTrackCand.h"
#include "FairTask.h"


class DchTrackManagerSim : public FairTask {

 //DchTrackCand *dchTrCand1;
 //DchTrackCand *dchTrCand2;
 //TFile *fdstread1;
 //TFile *fdstread2;
 TH1D* chi2linfit;
 TH2D* residVsTDCy;
 TH2D* residVsTDCx;
 TH2D* residVsTDCu;
 TH2D* residVsTDCv;
 TH2D* residVsTDC;
 TH2D* ResidVsTDCy[2];
 TH2D* ResidVsTDCx[2];
 TH2D* ResidVsTDCu[2];
 TH2D* ResidVsTDCv[2];
 TH2D* ResidVsTDC[2];
 TH1D* trackAngleX;TH1D* trackAngleY;
 TList vList; 
 //const static UInt_t nintervals; 
 //const Double_t ranmin;
 //const Double_t ranmax;
 Double_t resolution; 
 UInt_t nintervals; 
 UShort_t iter; 
 TString runType;

protected:

  //UInt_t nTrCand;
  //void SetDchTrackManagerSimNumber(UInt_t n);
  //UInt_t GetDchTrackManagerSimNumber();
  Double_t meanDeltaPhi;

public:    
  TClonesArray  *fDchTrackManagerSim1;           //-> pointer to Dch1 track candidate
  TClonesArray  *fDchTrackManagerSim2;           //-> pointer to Dch2 track candidate
  //DchTrackManagerSim(const DchTrackCand *dchTrCand1,const DchTrackCand *dchTrCand2);
  DchTrackManagerSim(TString runtype);
  ~DchTrackManagerSim();

  Bool_t checkDTM;
  //static const Double_t resolution;
  static const UInt_t ncols;

  //void SetDchTrackHits(UInt_t n, Double_t x0, Double_t y0, Double_t z0, Double_t x3, Double_t y3, Double_t z3);
  void InitDch(const UShort_t Iter, const Double_t ranmin[], const Double_t ranmax[], const Double_t Resolution, const UInt_t Nintervals);
  void HistoBookDch(const Double_t ranmin[], const Double_t ranmax[]);
  //void DchTrackCandReader(DchTrackCand* &dchTrCand1,DchTrackCand* &dchTrCand2);
  //void DchTrackCandReader(const UInt_t numevents);
  void DchTrackMerger(DchTrackCand *dchTrCand1,DchTrackCand *dchTrCand2,const TMatrixD C0); 
  double Matrix(float* matArr, const UInt_t k, const UInt_t l);
  void DchLinearTrackFitter(TGraph2DErrors* dchtrgr, Double_t *parFitL, TH1D* &chi2linfit); 
  //void FinishDch(TGraph* calib,TH1D* &hResidRMSvsIter, UShort_t Iter, TString datatype); 
  void FinishDch(TGraph* calib[],TH1D* &hResidRMSvsIter, TH1D* &hResidRMSvsIter0, TH1D* &hResidRMSvsIter1, UShort_t Iter, TString datatype); 
  Double_t PointLineDistance(const Double_t x0, const Double_t y0, const Double_t z0, const Double_t x, const Double_t y, const Double_t z, const Double_t dircosx, const Double_t dircosy, const Double_t dircosz);
  void DchAlignmentData(const DchTrackCand *dchTrCand1, const DchTrackCand *dchTrCand2, TClonesArray* &fDchTrCand1Align, TClonesArray* &fDchTrCand2Align, UInt_t &eventNum1track, const TMatrixD C0, const UShort_t Iter, Double_t &sumDeltaPhi);  
  Bool_t TracksDirectionTest(const TMatrix dch1Mat,const TMatrix dch2Mat, const TMatrixD C0, Double_t &delPhi);
  UShort_t RunTypeToNumber(TString runType);
  Double_t SetMeanDeltaPhi(Double_t meandelphi){meanDeltaPhi = meandelphi;};
  
  ClassDef(DchTrackManagerSim,1)      // Dch Track Manager 
  

};

#endif
