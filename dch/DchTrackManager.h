#ifndef DCH_TRACK_MANAGER_H
#define DCH_TRACK_MANAGER_H

// *************************************************************************
// Author: Jan Fedorishin  e-mail: fedorisin@jinr.ru
//   
// Dch track manager, creation and fitting of track in both DCH's
//   
//
// Created: 28-09-2015
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
#include "Math/Vector3D.h"

//#include "DCHgeometryconstants_bmn.h"
#include "DchTrackCand.h"
#include "FairTask.h"
#include "BmnNewFieldMap.h"

class DchTrackManager : public FairTask {

 BmnNewFieldMap* magfield; 
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
 TH1D* mDCAy[2];
 TH1D* mDCAx[2];
 TH1D* mDCAu[2];
 TH1D* mDCAv[2];
 TH1D* fDCAy[2];
 TH1D* fDCAx[2];
 TH1D* fDCAu[2];
 TH1D* fDCAv[2];
 //TH1D* trackAngleX;TH1D* trackAngleY;
 TH1D* trackAngleXaf;TH1D* trackAngleYaf;
 TH1D* trackAngleXbf1;TH1D* trackAngleYbf1;TH1D* trackAngleXbf2;TH1D* trackAngleYbf2;
 TH2D* trckAngX2vs1; TH2D* trckAngY2vs1; 
 TH1D *hDeltaX[2][4],*hDeltaY[2][4];
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
  //void SetDchTrackManagerNumber(UInt_t n);
  //UInt_t GetDchTrackManagerNumber();
  Double_t meanDeltaPhi;

public:    
  TClonesArray  *fDchTrackManager1;           //-> pointer to Dch1 track candidate
  TClonesArray  *fDchTrackManager2;           //-> pointer to Dch2 track candidate
  //DchTrackManager(const DchTrackCand *dchTrCand1,const DchTrackCand *dchTrCand2);
  DchTrackManager(TString runtype);
  ~DchTrackManager();

  Bool_t checkDTM;
  //static const Double_t resolution;
  static const UInt_t ncols;

  //void SetDchTrackHits(UInt_t n, Double_t x0, Double_t y0, Double_t z0, Double_t x3, Double_t y3, Double_t z3);
  void InitDch(const UShort_t Iter, const Double_t ranmin[], const Double_t ranmax[], const Double_t Resolution, const UInt_t Nintervals, BmnNewFieldMap *magField);
  Double_t MagFieldIntegral(const Double_t *parFitL);
  void HistoBookDch(const Double_t ranmin[], const Double_t ranmax[]);
  //void DchTrackCandReader(DchTrackCand* &dchTrCand1,DchTrackCand* &dchTrCand2);
  //void DchTrackCandReader(const UInt_t numevents);
  void DchTrackMerger(DchTrackCand *dchTrCand1,DchTrackCand *dchTrCand2,const TMatrixD C0, Double_t parFitL[], Int_t &fitErrorcode); 
  double Matrix(float* matArr, const UInt_t k, const UInt_t l);
  void DchLinearTrackFitter(TGraph2DErrors* dchtrgr, Double_t *parFitL, TH1D* &chi2linfit, Int_t &fitErrorcode); 
  //void FinishDch(UShort_t fDchNum,TGraph *calib[fDchNum-1]); 
  //void FinishDch(TGraph* calib,TH1D* &hResidRMSvsIter, UShort_t Iter, TString datatype); 
  void FinishDch(TGraph* calib[],TGraph *calibPlane[][4],TH1D* &hResidRMSvsIter,TH1D* &hResidRMSvsIter0,TH1D* &hResidRMSvsIter1,UShort_t Iter,TString datatype,UShort_t runNumber, TString outDirectory, Double_t &locAngleX, Double_t &locAngleY); 
  Double_t PointLineDistance(const Double_t x0, const Double_t y0, const Double_t z0, const Double_t x, const Double_t y, const Double_t z, const Double_t dircosx, const Double_t dircosy, const Double_t dircosz);
  void DchAlignmentData(const DchTrackCand *dchTrCand1, const DchTrackCand *dchTrCand2, TClonesArray* &fDchTrCand1Align, TClonesArray* &fDchTrCand2Align, UInt_t &eventNum1track, const TMatrixD C0, const UShort_t Iter, const XYZVector XYZ1, Double_t &sumDeltaPhi);  
  Bool_t TracksDirectionTest(const TMatrix dch1Mat,const TMatrix dch2Mat, const TMatrixD C0, const XYZVector XYZ1, Double_t &delPhi);
  UShort_t RunTypeToNumber(TString runType);
  Double_t SetMeanDeltaPhi(Double_t meandelphi){meanDeltaPhi = meandelphi;};
  void FillAngles(const TGraph2DErrors* dchtrgr, const Double_t *parFitL);
  
  ClassDef(DchTrackManager,1)      // Dch Track Manager 
  

};

#endif
