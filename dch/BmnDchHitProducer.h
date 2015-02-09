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
#include "TGraph2DErrors.h"
#include "ScalarD.h"
#include "ScalarI.h"
#include "ScalarUI.h"
#include "DchHitPlane.h"
#include "DCHgeometryconstants_bmn.h"

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
        DchHitPlane *dchhitplane[numLayers+ngaps];
        Bool_t fOnlyPrimary;
        UShort_t fDchNum;
        Bool_t fDchUsed;
        UInt_t eventNum; 
        Double_t z3121,z4121,z4131;
  	TRandom		 *pRandom;
	TClonesArray	 *pHitCollection0,*pHitCollection; 
	//TClonesArray			*pDchPoints;
	//TClonesArray			*pMCTracks;
	TList		 fList;	
	Bool_t		 fDoTest;
        Bool_t           checkDch;
	Double_t	 fRSigma, fRPhiSigma; // [cm] default 2mm in R, 200um in R-Phi

	TH1D		 *htOccup,  *htWireN, *htMCTime, *hX, *hY, *hZ, *hRadiusRange;
	TH2D		 *htXYlocal, *htRvsR, *hXYhit;	
	TH1D		 *htGasDrift,*htGasDriftA, *htTime, *htTimeA, *htPerp, *htPerpA;
	TH1D		 *hResolX,*hResolY,*hResolR;
	TGraph2D	 *hXYZcombhits,*hXYZpoints;
        BmnDchHit        *dchCombHit;
	
	typedef multimap<Double_t, Int_t> 	occupMap;
	typedef occupMap::iterator 		occupIter;
	occupMap			fMapOccup;
        Bool_t wireUsed[numLayers][numWiresPerLayer];
        UInt_t          ijkl2[ngaps];
        //Double_t        zgap[numChambers][ngaps];
        Double_t        zLayer[numLayers];
        Double_t        xyTolerance,radialRange;
	void		Rotate(UInt_t proj, Double_t x,Double_t y, Double_t& xRot, Double_t& yRot, Bool_t back=false);	
	Double_t	GetDriftLength(UInt_t proj, UInt_t gasgap, Double_t x, Double_t& wirePos);
	Double_t	GetTShift(Double_t driftLength, Double_t wirePos, Double_t R, Double_t&);
	Bool_t 		HitExist(Double_t delta);	
	Double_t	GetPhi(UInt_t proj);
	Int_t		WireID(UInt_t uid, Double_t wirePos, Double_t R);
	
  	BmnDchHit* 	AddHit0(Int_t index, Int_t detID, const TVector3& posHit, const TVector3& posHitErr, Int_t trackIndex, Int_t pointIndex, Int_t flag, UInt_t dchlayer);
  	BmnDchHit* 	AddHit(Int_t index, Int_t detID, const TVector3& posHit, const TVector3& posHitErr, Int_t trackIndex, Int_t pointIndex, Int_t flag, UInt_t dchlayer);

		
public:

  	BmnDchHitProducer(UInt_t num, Int_t verbose = 1, Bool_t DoTest = false);
  	~BmnDchHitProducer();

 	InitStatus	Init();
  	void 		Exec(Option_t* opt);
	void		Finish();

        void SetOnlyPrimary(Bool_t opt = kFALSE) { fOnlyPrimary = opt; }

	void		SetErrors(Double_t errR, Double_t errRphi){ fRSigma = errR; fRPhiSigma = errRphi; };
        void            InitDchParameters();
        void            BookHistograms();
        void            LRambiguity(UInt_t k, Double_t hittmp[2], Double_t hitx, Double_t driftdist, UInt_t hw, UShort_t &nhits);
        Double_t        PointLineDistance(const Double_t x0, const Double_t y0, const Double_t z0, const Double_t x, const Double_t y, const Double_t z, const Double_t dircosx, const Double_t dircosy, const Double_t dircosz);
        void            RadialRange(FairMCPoint* dchPoint);
        void            HitFinder();
        void            CoordinateFinder(Double_t y1,Double_t x2,Double_t k3,Double_t q3,Double_t k4,Double_t q4,Double_t x[],Double_t y[]);
        ScalarD* hitX[numLayers];
        void            ReturnPointers(UInt_t ijk,UInt_t i,ScalarD* &hitX,ScalarI* &trackId, ScalarI* &detId,ScalarUI* &hitwire,ScalarD* &driftlen,ScalarI* &pointind);
        static UInt_t GetProj(UInt_t uid){ return uid/2; }; //lsp [0-3] == [x,y,u,v]
        static UInt_t GetGasGap(UInt_t uid){ return uid%2; }; //lsp [0-1] == [inner,outer]

        static const Double_t detXshift[numChambers];
        static const Double_t cosPhi_45, sinPhi_45, cosPhi45, sinPhi45;
        static const Double_t Phi_45, Phi45, Phi90;
        static const Double_t sqrt2,tg3,tg4;
        static const Double_t gasDriftSpeed,wireDriftSpeed,WheelR_2;
        static const Double_t resolution;
        static const Double_t hitErr[3];
  
ClassDef(BmnDchHitProducer,1); 
};
//------------------------------------------------------------------------------------------------------------------------
#endif
