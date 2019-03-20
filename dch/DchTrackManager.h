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
#include "TMatrixFSym.h"
//#include "DCHgeometryconstants_bmn.h"
#include "DchTrackCand.h"
#include "BmnDchTrack.h"
#include "FairTask.h"
#include "BmnNewFieldMap.h"

class DchTrackManager : public FairTask {

 //BmnNewFieldMap* magfield; 
 //DchTrackCand *dchTrCand1;
 //DchTrackCand *dchTrCand2;
 //TFile *fdstread1;
 //TFile *fdstread2;
 TH1F  *hResidRMSvsIter,*hResidRMSvsIter0,*hResidRMSvsIter1;
 TH1D* chi2linfitHist;
 TH1F *distDch1,*distDch2;
 TH1F *distxDch1,*distxDch2,*distyDch1,*distyDch2;
 TH1F *diffAngleTracks,*diffAngleTracksX,*diffAngleTracksY;
 TH2D* residVsTDCy;    
 TH2D* residVsTDCx;
 TH2D* residVsTDCu;
 TH2D* residVsTDCv;
 TH2D* residVsTDC;
 TH2D* ResidVsTDCy[2];    
 TH2D* ResidVsTDCx[2];
 TH2D* ResidVsTDCu[2];
 TH2D* ResidVsTDCv[2];
 TH2D* ResidVsTDCx_y[2];    
 TH2D* ResidVsTDCx_u[2];    
 TH2D* ResidVsTDCx_v[2];    
 TH2D* ResidVsTDCy_x[2];    
 TH2D* ResidVsTDCy_u[2];    
 TH2D* ResidVsTDCy_v[2];    
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
 //const static UInt_t nintervals; 
 //const Double_t ranmin;
 //const Double_t ranmax;
 Double_t resolution; 
 Double_t chi2linfit;
 UInt_t nintervals; 
 UShort_t iter,itermax; 
 //TString runType;
 UShort_t runPeriod;
 Option_t* opt;
 Bool_t doCheck;

protected:

  //UInt_t nTrCand;
  //void SetDchTrackManagerNumber(UInt_t n);
  //UInt_t GetDchTrackManagerNumber();
  Double_t meanDeltaPhi;

public:    
  TClonesArray  *fDchTrackManager1;           //-> pointer to Dch1 track candidate
  TClonesArray  *fDchTrackManager2;           //-> pointer to Dch2 track candidate
  //DchTrackManager(const DchTrackCand *dchTrCand1,const DchTrackCand *dchTrCand2);
  DchTrackManager(UShort_t runperiod);
  ~DchTrackManager();

  Bool_t checkDTM;
  //static const Double_t resolution;
  static const UInt_t ncols;

  //void SetDchTrackHits(UInt_t n, Double_t x0, Double_t y0, Double_t z0, Double_t x3, Double_t y3, Double_t z3);
  void InitDch(const UShort_t Iter, const UShort_t Itermax, const Double_t ranmin[][4], const Double_t ranmax[][4], const Double_t ranmin_cham[], const Double_t ranmax_cham[], const Double_t Resolution, const UInt_t Nintervals, TList &hList, const Bool_t fDoCheck, const TString outDirectory, const TString residualsFileName_old, const Option_t* Opt);
  //Double_t MagFieldIntegral(const Double_t *parFitL);
  void HistoBookDch(const Double_t ranmin[][4], const Double_t ranmax[][4], const Double_t ranmin_cham[], const Double_t ranmax_cham[], TList &hList, const TString outDirectory, const TString residualsFileName_old);
  //void DchTrackCandReader(DchTrackCand* &dchTrCand1,DchTrackCand* &dchTrCand2);
  //void DchTrackCandReader(const UInt_t numevents);
  void DchTrackMerger(DchTrackCand *dchTrCand1, DchTrackCand *dchTrCand2, const TMatrixD C0, Int_t DCHtrackIDglob[], const Int_t eventnum, const Option_t* Opt, TClonesArray *pTrackCollection[], const TMatrixFSym covMat[], Double_t parFitL[], Int_t &fitErrorcode); 
  double Matrix(const float* matArr, const UInt_t k, const UInt_t l);
  //void DchLinearTrackFitter(TGraph2DErrors* dchtrgr, Double_t *parFitL, TH1D* &chi2linfitHist_, Int_t &fitErrorcode); 
  void DchLinearTrackFitter(TGraph2DErrors* dchtrgr, Double_t *parFitL, Int_t &fitErrorcode); 
  void DchAverageLinearTrack(TGraph2DErrors* dchtrgr, Double_t *parFitL);
  //void FinishDch(UShort_t fDchNum,TGraph *calib[fDchNum-1]); 
  //void FinishDch(TGraph* calib,TH1D* &hResidRMSvsIter, UShort_t Iter, TString datatype); 
  void FinishDch(TGraph* calib_cham[],TGraph *calib[][4], UShort_t runNumber, TString outDirectory, TString residualsFileName, Double_t &locAngleX, Double_t &locAngleY, Bool_t &noError); 
  Double_t PointLineDistance(const Double_t x0, const Double_t y0, const Double_t z0, const Double_t x, const Double_t y, const Double_t z, const Double_t dircosx, const Double_t dircosy, const Double_t dircosz);
  Float_t tracksAngle(const float* matArr,const float*matArr2);
  void DchAlignmentData(const DchTrackCand *dchTrCand1, const DchTrackCand *dchTrCand2, TClonesArray* &fDchTrCand1Align, TClonesArray* &fDchTrCand2Align, UInt_t &eventNum1track, const TMatrixD C0, const XYZVector XYZ1,  Double_t &sumDeltaPhi);  
  Bool_t TracksDirectionTest(const TMatrix dch1Mat, const TMatrix dch2Mat, const TMatrixD C0, const XYZVector XYZ1, Double_t &delPhi);
  Bool_t TracksDirectionTestReco(const float* matArr,const float* matArr2); 
  //UShort_t RunTypeToNumber(TString runType);
  void SetMeanDeltaPhi(Double_t meandelphi){meanDeltaPhi = meandelphi;};
  void SetOpt(Option_t *Opt){opt = Opt;};
  void FillAngles(const TGraph2DErrors* dchtrgr, const Double_t *parFitL);
  BmnDchTrack* AddTrack(const Int_t DCHtrID, const Int_t DCHtrIDglob, TClonesArray *pTrackCollection, const TVector3 linePointFirst, TVector3 linePointLast, const Double_t slopeX1, const Double_t slopeY1, const Double_t slopeX2, const Double_t slopeY2, const Int_t eventnum, const TMatrixFSym covMat1, const TMatrixFSym covMat2, const TString whichDCH);

  static const Double_t sqrt2;
  static const Double_t sqrt2half;
  
  ClassDef(DchTrackManager,1)      // Dch Track Manager 
  

};

#endif
