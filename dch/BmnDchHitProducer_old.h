//------------------------------------------------------------------------------------------------------------------------
#ifndef __BMN_DCH_HIT_PRODUCER_H
#define __BMN_DCH_HIT_PRODUCER_H 1

#include <map>
#include <math.h>
#include <iostream>
#include "FairTask.h"
#include "TList.h"
#include "TVector3.h"
#include "TString.h"
#include "TH1D.h"
#include "TH2D.h"
#include <TF1.h>
//#include "TGraph2DErrors.h"
#include "TGraph2D.h"
#include <TGraph.h>
#include <TSpline.h>
#include "ScalarD.h"
#include "ScalarI.h"
#include "ScalarUI.h"
#include "DchHitPlane.h"
#include "DchTrackCand.h"
#include "DchTrackManagerSim.h"
//#include "DCHgeometryconstants_bmn.h"
#include "TMath.h"
#include "Math/Point3D.h"
#include "Math/Vector3D.h"
#include "TMatrixD.h"

using namespace ROOT::Math;

class TClonesArray;
class TObjectArray;
class BmnDchHit;

class TRandom;
class TH1D;
class TH2D;
class ScalarD;
class ScalarI;
class ScalarUI;
class DchHitPlane;

//------------------------------------------------------------------------------------------------------------------------
class BmnDchHitProducer : public FairTask
{

        //TFile *dchtrackcands;
        DchTrackManagerSim *dchTracks;
        DchTrackCand *dchTrCand1;
        DchTrackCand *dchTrCand2;
        TString runType;
        TString fInputBranchName,fInputBranchName2;
        TString fOutputTestFileName;
        TString fOutputHitsBranchName0,fOutputHitsBranchName;
        //TString fOutputHitsBranchName0[numChambers],fOutputHitsBranchName[numChambers];
        /** Input array of Dch Points **/
        TClonesArray* fBmnDchPointsArray;
        TClonesArray* fBmnDchPointsArray2;
        /** Input array of MC Tracks **/
        TClonesArray* fMCTracksArray;
        /** Output array of Dch Hits **/
        //TClonesArray* fBmnDchHitsArray;
        //DchHitPlane *dchhitplane[numChambers][numLayers];
        DchHitPlane *dchhitplane[numLayers];
        //DchTrackCand *trackcand;
        TTree *tree; 
        Bool_t fOnlyPrimary;
        //UShort_t fDchNum;
        Bool_t fDchUsed;
        UShort_t calibMethod;
        UInt_t eventNum; 
        UInt_t trCand; 
        UShort_t Iter;
        //Double_t z3121,z4121,z4131;
        Double_t zCoeff1[2][2][2],zCoeff2[2][2][2];
        Double_t anglepar[numLayers][2];
        Double_t DCH1_ZlayerPos_global[numLayers],DCH2_ZlayerPos_global[numLayers];
        Double_t detXshift[numChambers],detYshift[numChambers];
  	TRandom		 *pRandom;
	TClonesArray	 *pHitCollection0[numChambers];
	TClonesArray	 *pHitCollection[numChambers];
	//TClonesArray			*pDchPoints;
	//TClonesArray			*pMCTracks;
	TList		 fList,gList,tList;	
	Bool_t		 fDoTest;
	//Bool_t		 useCalib;
        Bool_t           checkDch,fAngleCorrectionFill,checkGraphs;
	Double_t	 fRSigma, fRPhiSigma; // [cm] default 2mm in R, 200um in R-Phi

	TH1D		 *htOccup,  *htWireN, *htMCTime, *hX, *hY, *hZ, *hRadiusRange;
	TH2D		 *htXYlocal, *htRvsR, *hXYhit;
        TH2D             *hAngleVsWirepos[numLayers];	
        TH1D             *minDriftTime;
        TH1D             *minDriftTime1,*minDriftTime2;
        TH1D             *hOccup[numChambers];
        //TH1D             *xzAngle[numChambers],*yzAngle[numChambers];
	TH1D		 *htGasDrift,*htGasDriftA, *htTime, *htTimeA, *htPerp, *htPerpA;
	TH1D		 *hResolX, *hResolY, *hResolR, *hResolU, *hResolV, *hMinDriftTime, *hMinDriftTime_read, *hDCA, *hLength;
	TGraph2D	 *hXYZcombhits,*hXYZpoints;
        TF1              *fitrt,*fitdt;
        TH1D             *splinederivsmooth;
        //TGraph           *calib,*splinederiv;
        TGraph           *calib[2],*splinederiv;
        //TSpline5         *spline5,*spline5rt;
        TSpline5         *spline5,*spline5rt[2];
        TVector2         uOrt,vOrt;
        TMatrixD         C,C0;
	
	typedef multimap<Double_t, Int_t> 	occupMap;
	typedef occupMap::iterator 		occupIter;
	occupMap			fMapOccup;
        Bool_t          wireUsed[numLayers][numWiresPerLayer],planeUsed[numLayers];
        Double_t        zLayer[numChambers][numLayers],zLayerExtrap[numChambers][numLayers];
        Double_t        xyTolerance,radialRange;
        Double_t        ranmin[2],ranmax[2];// r-t calibration range
        Double_t        scaleSimExTDC[2];
	void		Rotate(UShort_t proj, Double_t x,Double_t y, Double_t& xRot, Double_t& yRot, Bool_t back=false);	
	Double_t	GetDriftLength(UShort_t proj, UShort_t gasgap, Double_t x, Double_t& wirePos);
        Double_t        GetDriftDist(FairMCPoint *dchPoint, UShort_t idch, UShort_t uidLocal, Double_t x, Double_t &wirePos);
	Double_t	GetTShift(Double_t driftLength, Double_t wirePos, Double_t R, Double_t&);
	Bool_t 		HitExist(Double_t delta);	
	Double_t	GetPhi(UShort_t proj);
	Int_t		WireID(UInt_t uid, Double_t wirePos, Double_t R);
        Double_t        wirePosition(UShort_t gasgap, UInt_t wirenum, UShort_t uidLocalproj);
        Double_t        rtCurve(const Double_t time, const UShort_t idch);
        UShort_t        PlanesTopology(UShort_t idch);
        void            ExtrapToDch(const Double_t x[],const Double_t y[],const Double_t zLayer[][numLayers], const UShort_t idch, const UShort_t ijk[], Int_t &jjgr2);
	
  	BmnDchHit* 	AddHit0(Int_t index, Int_t detID, const TVector3& posHit, const TVector3& posHitErr, Int_t trackIndex, Int_t pointIndex, Int_t flag, UInt_t dchlayer, UShort_t idch);
  	BmnDchHit* 	AddHit(Int_t index, Int_t detID, const TVector3& posHit, const TVector3& posHitErr, Int_t trackIndex, Int_t pointIndex, Int_t flag, UInt_t dchlayer, UShort_t idch);
  	//DchTrackCand* 	AddTrack(Int_t index, Int_t detID, const TVector3& posHit, const TVector3& posHitErr, Int_t trackIndex, Int_t pointIndex, Int_t flag, UInt_t dchlayer);

		
public:

  	//BmnDchHitProducer(UInt_t num, Int_t verbose = 1, Bool_t DoTest = false, TString runtype="run1", UShort_t iter=0);
  	BmnDchHitProducer(Int_t verbose = 1, Bool_t DoTest = false, TString runtype="run1");
  	~BmnDchHitProducer();

 	InitStatus	Init();
  	void 		Exec(Option_t* opt);
	void		Finish();

        void SetOnlyPrimary(Bool_t opt = kFALSE) { fOnlyPrimary = opt; }

	void		SetErrors(Double_t errR, Double_t errRphi){ fRSigma = errR; fRPhiSigma = errRphi; };
        void            InitDchParameters();
        void            BookHistograms();
        void            BookHistsAngleCorr();
        void            FitHistsAngleCorr();
        void            LRambiguity(UInt_t k, Double_t hittmp[2], Double_t hitx, Double_t driftdist, UInt_t hw, UShort_t &nhits);
        Double_t LineLineDCA(const Double_t x1, const Double_t y1, const Double_t z1, const Double_t dircosx1, const Double_t dircosy1, const Double_t dircosz1, const Double_t xst, const Double_t sty, const Double_t stz, const Double_t dircosstx, const Double_t dircossty, const Double_t dircosstz, Double_t &x2, Double_t &y2, Double_t &z2, Double_t &xdchwmin, Double_t  &ydchwmin, Double_t &zdchwmin, Double_t &length, Double_t &mu0, Double_t &lambda0);
        Double_t LineLength(const Double_t x1, const Double_t y1, const Double_t z1, const Double_t x2, const Double_t y2, const Double_t z2);
        Double_t        PointLineDistance(const Double_t x0, const Double_t y0, const Double_t z0, const Double_t x, const Double_t y, const Double_t z, const Double_t dircosx, const Double_t dircosy, const Double_t dircosz);
        void            RadialRange(FairMCPoint* dchPoint);
        void            HitFinder(UShort_t idch);
        void            CoordinateFinder(const Double_t y1,const Double_t x2,const Double_t k3,const Double_t q3,const Double_t k4,const Double_t q4,const UShort_t *ijk, Double_t x[],Double_t y[]);
        void            ReturnPointers(UInt_t ijk,UInt_t i,ScalarD* &hitX,ScalarI* &trackId, ScalarI* &detId,ScalarUI* &hitwire,ScalarD* &driftlen,ScalarD* &drifttim,ScalarI* &pointind);
        //void            rtCalibration();
        void            tdcInflexPoints(Double_t &inflX1,Double_t &inflX2);
        Double_t        SplineIntegral(const Double_t a, const Double_t b, const Int_t n);
        //void            rtCalibRead();
        void            rtCalibCorrection();
        void            MisAlignment(XYZPoint &xyzHit1,XYZPoint &xyzHit2,XYZPoint &xyzHit3,XYZPoint &xyzHit4,XYZPoint &xyzWire1,XYZPoint &xyzWire2,XYZPoint &xyzWire3,XYZPoint &xyzWire4);
        void            DchAlignment(XYZPoint &xyzHit1,XYZPoint &xyzHit2,XYZPoint &xyzHit3,XYZPoint &xyzHit4,XYZPoint &xyzWire1,XYZPoint &xyzWire2,XYZPoint &xyzWire3,XYZPoint &xyzWire4);
        static UShort_t GetProj(UShort_t uid){ return uid/2; }; //lsp [0-3] == [x,y,u,v]
        static UShort_t GetGasGap(UShort_t uid){ return uid%2; }; //lsp [0-1] == [inner,outer]
        UShort_t RunTypeToNumber(TString runType);
        //enum runtypes{run1=0,run2=1,run3=0};
        //runtypes current_run;

        static const Double_t cosPhi_45, sinPhi_45, cosPhi45, sinPhi45;
        static const Double_t Phi_45, Phi45, Phi90;
        static const Double_t sqrt2,tg3,tg4;
        static const Double_t clusdens_Ar,clusdens_CO2,clusmean,meanpath,meanpath_Ar,meanpath_CO2;
        static const Double_t gasDriftSpeed,wireDriftSpeed,driftTimeMax,WheelR_2;
        static const Double_t resolution;
        static const Double_t hitErr[3];
        static const UInt_t nintervals;
        static const TVector2 unitU,unitV;
  
ClassDef(BmnDchHitProducer,1); 
};
//------------------------------------------------------------------------------------------------------------------------
#endif
