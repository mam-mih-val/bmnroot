//------------------------------------------------------------------------------------------------------------------------
#ifndef __BMN_DCH_HIT_PRODUCER_EXP_H
#define __BMN_DCH_HIT_PRODUCER_EXP_H

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
//#include "DCHgeometryconstants_bmn.h"

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
class BmnDchHitProducer_exp : public FairTask
{

        TString runType;
        TString fInputBranchName;
        TString fOutputTestFileName;
        TString fOutputHitsBranchName0,fOutputHitsBranchName;
        /** Input array of Dch Points **/
        TClonesArray* fBmnDchPointsArray;
        /** Input array of MC Tracks **/
        TClonesArray* fMCTracksArray;
        /** Output array of Dch Hits **/
        //TClonesArray* fBmnDchHitsArray;
        //DchHitPlane *dchhitplane[numChambers][numLayers];
        DchHitPlane *dchhitplane[numLayers];
        Bool_t fOnlyPrimary;
        UShort_t fDchNum;
        Bool_t fDchUsed;
        UInt_t eventNum; 
        Int_t nevents; 
        UInt_t neventsUsed; 
        //Double_t z3121,z4121,z4131;
        Double_t zCoeff1[2][2][2],zCoeff2[2][2][2];
        Double_t anglepar[numLayers][2];
        Double_t DCH1_ZlayerPos_global[numLayers],DCH2_ZlayerPos_global[numLayers];
        Double_t detXshift[numChambers];
  	TRandom		 *pRandom;
	TClonesArray	 *pHitCollection0,*pHitCollection; 
	//TClonesArray			*pMCTracks;
	TClonesArray     *dchDigits; 
	TList		 fList,gList;	
	Bool_t		 fDoTest;
	Bool_t		 fDoOccupancy;
        Bool_t           checkDch,fAngleCorrectionFill,checkGraphs;
	Double_t	 fRSigma, fRPhiSigma; // [cm] default 2mm in R, 200um in R-Phi

	TH1D		 *hX[numChambers], *hY[numChambers], *hZ[numChambers], *hRadiusRange[numChambers];
	TH2D		 *hXYhit[numChambers];
        //TH2D             *hAngleVsWirepos0,*hAngleVsWirepos2,*hAngleVsWirepos4,*hAngleVsWirepos6,;	
        TH2D             *hAngleVsWirepos[numLayers];	
        TH1D             *minDriftTime[numChambers];
        TH1D             *hOccup[numChambers];
	TH1D		 *hResolX, *hResolY, *hMinDriftTime;
	TGraph2D	 *hXYZcombhits;
        BmnDchHit        *dchCombHit;
        TF1              *fitrt,*fitdt;
        TH1D             *splinederivsmooth[numChambers]; 
        TGraph           *calib[numChambers],*splinederiv[numChambers];
        //TSpline3         *spline3,*spline3rt;
        TSpline5         *spline5[numChambers],*spline5rt[numChambers];
	
	typedef multimap<Double_t, Int_t> 	occupMap;
	typedef occupMap::iterator 		occupIter;
	occupMap			fMapOccup;
        Bool_t          wireUsed[numLayers][numWiresPerLayer],planeUsed[numLayers];
        Double_t        zLayer[numLayers],zLayerExtrap[numLayers];
        Double_t        xyTolerance,radialRange;
        Double_t        ranmin,ranmax;// r-t calibration range
	void		Rotate(UShort_t proj, Double_t x,Double_t y, Double_t& xRot, Double_t& yRot, Bool_t back=false);	
	Double_t	GetDriftLength(UShort_t gasgap, Double_t x, Double_t& wirePos);
        Double_t        GetDriftDist(FairMCPoint *dchPoint, UShort_t uidLocal, Double_t x, Double_t &wirePos);
	Double_t	GetTShift(Double_t driftLength, Double_t wirePos, Double_t R, Double_t&);
	Bool_t 		HitExist(Double_t delta);	
	Double_t	GetPhi(UShort_t proj);
	Int_t		WireID(UInt_t uid, Double_t wirePos, Double_t R);
        Double_t        wirePosition(UShort_t gasgap, UInt_t wirenum);
        Double_t        rtCurve(Double_t time);
        UShort_t        PlanesTopology();
        void            ExtrapToDch(const Double_t x[],const Double_t y[],const Double_t zLayer[],const UShort_t ijk[],Int_t &jjgr2);
	
  	BmnDchHit* 	AddHit0(Int_t index, Int_t detID, const TVector3& posHit, const TVector3& posHitErr, Int_t trackIndex, Int_t pointIndex, Int_t flag, UInt_t dchlayer);
  	BmnDchHit* 	AddHit(Int_t index, Int_t detID, const TVector3& posHit, const TVector3& posHitErr, Int_t trackIndex, Int_t pointIndex, Int_t flag, UInt_t dchlayer);

		
public:

  	BmnDchHitProducer_exp(UInt_t num, Int_t verbose = 1, Bool_t DoTest = false, TString runtype="run1");
  	~BmnDchHitProducer_exp();

 	//InitStatus	Init();
 	void	        InitDch(TChain *bmntree, TClonesArray *dchDigits, TTree* tReco);
  	void 		ExecDch(Int_t iev, TClonesArray *dchDigits);
	void		FinishDch();

        void SetOnlyPrimary(Bool_t opt = kFALSE) { fOnlyPrimary = opt; }

	void		SetErrors(Double_t errR, Double_t errRphi){ fRSigma = errR; fRPhiSigma = errRphi; };
        void            InitDchParameters();
        void            BookHistograms();
        void            BookHistsAngleCorr();
        void            FitHistsAngleCorr();
        int             NumericalRootFinder();
        void            tdcInflexPoints(Double_t &inflX1,Double_t &inflX2);
        Double_t        SplineIntegral(const Double_t a, const Double_t b, const Int_t n);
        void            LRambiguity(const UInt_t k, Double_t hittmp[2], const Double_t hitx, const Double_t driftdist, const UInt_t hw, UShort_t &nhits);
        Double_t LineLineDCA(const Double_t x1, const Double_t y1, const Double_t z1, const Double_t dircosx1, const Double_t dircosy1, const Double_t dircosz1, const Double_t xst, const Double_t sty, const Double_t stz, const Double_t dircosstx, const Double_t dircossty, const Double_t dircosstz, Double_t &x2, Double_t &y2, Double_t &z2, Double_t &xdchwmin, Double_t  &ydchwmin, Double_t &zdchwmin, Double_t &length, Double_t &mu0, Double_t &lambda0);
        Double_t LineLength(const Double_t x1, const Double_t y1, const Double_t z1, const Double_t x2, const Double_t y2, const Double_t z2);
        Double_t        PointLineDistance(const Double_t x0, const Double_t y0, const Double_t z0, const Double_t x, const Double_t y, const Double_t z, const Double_t dircosx, const Double_t dircosy, const Double_t dircosz);
        void            RadialRange(FairMCPoint* dchPoint);
        void            HitFinder();
        void            CoordinateFinder(const Double_t y1,const Double_t x2,const Double_t k3,const Double_t q3,const Double_t k4,const Double_t q4,const UShort_t *ijk, Double_t x[],Double_t y[]);
        void            ReturnPointers(UInt_t ijk,UInt_t i,ScalarD* &hitX,ScalarI* &trackId, ScalarI* &detId,ScalarUI* &hitwire,ScalarD* &driftlen,ScalarI* &pointind);
        void            rtCalibration(TChain *bmntree,TClonesArray *dchDigits);
        static UShort_t GetProj(UShort_t uidLocal){ return uidLocal/2; }; //lsp [0-3] == [x,y,u,v]
        static UShort_t GetGasGap(UShort_t uid){ return uid%2; }; //lsp [0-1] == [inner,outer]
        UShort_t RunTypeToNumber(const TString runType);
        enum runtypes{run1=0,run2=1,run3=0};
        runtypes current_run;

        //static const Double_t detXshift[numChambers];
        static const Double_t cosPhi_45, sinPhi_45, cosPhi45, sinPhi45;
        static const Double_t Phi_45, Phi45, Phi90;
        static const Double_t sqrt2,tg3,tg4;
        static const Double_t clusdens_Ar,clusdens_CO2,clusmean,meanpath,meanpath_Ar,meanpath_CO2;
        static const Double_t gasDriftSpeed,wireDriftSpeed,WheelR_2;
        static const Double_t resolution;
        static const Double_t hitErr[3];
  
ClassDef(BmnDchHitProducer_exp,1); 
};
//------------------------------------------------------------------------------------------------------------------------
#endif
