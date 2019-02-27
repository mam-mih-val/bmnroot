//------------------------------------------------------------------------------------------------------------------------
#ifndef __BMN_DCH_HIT_PRODUCER_EXP_H
#define __BMN_DCH_HIT_PRODUCER_EXP_H 1

#include <map>
#include <math.h>
#include <iostream>
#include <FairRunSim.h>
#include "FairTask.h"
#include "TList.h"
#include "TVector3.h"
#include "TString.h"
#include "TH1D.h"
#include "TH1I.h"
#include "TH2D.h"
#include "TH3D.h"
#include <TF1.h>
#include "TTree.h"
//#include "TGraph2DErrors.h"
#include "TGraph2D.h"
#include <TGraph.h>
#include <TSpline.h>
#include "ScalarD.h"
#include "ScalarI.h"
#include "ScalarUI.h"
#include "DchHitPlane.h"
#include "DchTrackCand.h"
#include "DchTrackManager.h"
//#include "DCHgeometryconstants_bmn.h"
#include "TMath.h"
#include "Math/Point3D.h"
#include "Math/Vector3D.h"
//#include "TMatrixD.h"
//#include "TMatrixFSym.h"
#include "BmnNewFieldMap.h"
#include "BmnFieldConst.h"
//#include "/opt/fairsoft/install/include/TGeant3/TGeant3.h"                    // for TGeant3
#include "TGeant3.h"                    // for TGeant3
//#include "TGeant3TGeo.h"
//#include "FairGeanePro.h"

using namespace ROOT::Math;

class TClonesArray;
class TObjectArray;
class BmnDchHit;
class TRandom;
class TH1D;
class TH1I;
class TH2D;
class TH3D;
class TTree;
class ScalarD;
class ScalarI;
class ScalarUI;
class DchHitPlane;

//------------------------------------------------------------------------------------------------------------------------
class BmnDchHitProducer_exp 
{

        //TFile *dchtrackcands;
        TFile *fReco;
        TGeant3* gMC3;
        FairRunSim* fRun; 
        BmnNewFieldMap* magField;
        //BmnFieldConst* magField;
        DchTrackManager *dchTracks;
        DchTrackCand *dchTrCand1;
        DchTrackCand *dchTrCand2;
        //TString runPeriod;
        UShort_t runPeriod;
        UShort_t runNumber;
        Bool_t isTarget,zeroField;
        TString outDirectory,inDirectory;
        TString fInputBranchName,fInputBranchName2;
        TString fOutputFileName,fOutputFileNameReco;
        TString residualsFileName,residualsFileName_old,residualsFileNameReco;
        TString fileName0;
        TString DCHpar;
        TString fOutputHitsBranchName, fOutputTracksBranchName[3];
        TString runPeriodStr;
        /** Input array of Dch Digits **/
        TClonesArray* fBmnDchDigitsArray;
        TChain* fBmnTree; 
        TBranch *hitBranch,*trackBranch[3];
        /** Output array of Dch Hits **/
        //TClonesArray* fBmnDchHitsArray;
        //DchHitPlane *dchhitplane[numChambers][numLayers];
        DchHitPlane *dchhitplane[numLayers];
        //DchTrackCand *trackcand;
        TTree *tReco; 
        Bool_t fOnlyPrimary;
        //UShort_t fDchNum;
        Bool_t fDchUsed;
        UShort_t calibMethod;
        UInt_t eventNum1track; 
        Int_t eventNum; 
        Int_t hitGlobID,DCHtrackIDglob[3];   
        UInt_t trCand; 
        UShort_t Iter,fileNumber,itermax;
        Short_t topol[numChambers]; 
        //UInt_t goodRuns1[],goodRuns2[],goodRuns3[],goodRuns4[],goodRuns5[],goodRuns6[];
        UInt_t DCHtracks[numChambers],DCHwireHits[numChambers]; 
        //Double_t z3121,z4121,z4131;
        //Double_t zCoeff1[4][2][6],zCoeff2[4][2][8];
        Double_t zDif21[2][2],zDif31[2][2],zDif32[2][2],zDif42[2][2],zDif41[2][2],zDif43[2][2];
        Double_t zMult12[2][2],zMult34[2][2],zMult1234[2][2][2][2];
        Double_t anglepar[numLayers][2];
        Double_t dx[numChambers],dy[numChambers],du[numChambers],dv[numChambers];
        Double_t dx_y[numChambers],dx_u[numChambers],dx_v[numChambers];
        Double_t dy_x[numChambers],dy_u[numChambers],dy_v[numChambers];
        Double_t DCH1_ZlayerPos_global[numLayers],DCH2_ZlayerPos_global[numLayers];
        Double_t detXshift[numChambers],detYshift[numChambers],detZshift[numChambers];
        XYZVector XYZ1,XYZ2,XYZ0,XYZ0beam,DXYZ1,XYZshift1,XYZshift2;
        TClonesArray     *fDchTrCand1Align,*fDchTrCand2Align;
  	TRandom		 *pRandom;
	TClonesArray	 *pHitCollection,*pTrackCollection[3]; 
	//TClonesArray			*pDchPoints;
	TList		 hList,gList,calibList;
	Bool_t		 fDoCheck,isGeant,geantCheck;
	//Bool_t		 useCalib;
        Bool_t           checkDch,checkDchHist,checkMatrices,fAngleCorrectionFill,checkGraphs;
        TH1D             *hXhit[numChambers][numLayers_half], *hYhit[numChambers][numLayers_half], *hZhit[numChambers][numLayers_half];
        TH1D             *hRadialRange[numChambers],*hRadialRangeAll[numChambers];
        TH2D             *hXYhit[numChambers][numLayers_half];
        TH2D             *hAngleVsWirepos[numLayers];	
        TH1D             *minDriftTimeAll;
        //TH1D             *minDriftTime[numChambers];
        TH1D             *minDriftTime[numChambers],*minDriftTimePlane[numChambers][numLayers_half];
        TH1D             *trackAngleXdch1,*trackAngleYdch1,*trackAngleXdch2,*trackAngleYdch2;
        TH2D             *trackAngleYvsXdch1,*trackAngleYvsXdch2;
        TH1D             *deflectPointX,*deflectPointY,*deflectPointPx,*deflectPointPy,*deflectPointPz,*magfieldinteg;
        //TH1I             *hWirePlaneChamber[numChambers][numLayers_half];
        TH1I             *hWireNumberInPlane[numChambers][numLayers];
        TH1I             *hDigits;
        TH1F             *hWireCoordinateInPlane[numChambers][numLayers];
        TH1F             *hHitCoordinateInPlane[numChambers][numLayers];
        TH1I             *hTracksPerEvent[numChambers];
        TH1I             *hYwires[numChambers];
        //TH2I             *hHitsVsPlanePerEvent[numChambers];
        TH2I             *hWireHitsVsTracksPerEvent[numChambers];
        TH2I             *hWireHitsVsPlanePerEvent[numChambers];
        TH2I             *hDch1WireHitsVsDch2WireHitsPerEvent;
        TH2I             *hDch1TracksVsDch2TracksPerEvent;
        TH1D             *hOccup[numChambers];
        TH1D             *hExtrapMeasDiffX,*hExtrapMeasDiffY;
        TH2D             *hExtrapMeasDiffXY;
        TH1D             *hAliMisaliX,*hAliMisaliY;
        TH2D             *hAliMisaliXYdist;
        TH1D             *hAliX,*hAliY;
        TH2D             *hAliXYdist;
        TH1D             *hMisaliX,*hMisaliY;
        TH2D             *hMisaliXYdist;
        TH1D             *hXplanesAngle[numChambers],*hYplanesAngle[numChambers];
        TH1D             *hYplanesHitDist[numChambers],*hXplanesHitDist[numChambers],*hUplanesHitDist[numChambers],*hVplanesHitDist[numChambers];
        //TH1D             *xzAngle[numChambers],*yzAngle[numChambers];
	TH1D		 *htGasDrift,*htGasDriftA, *htTime, *htTimeA, *htPerp, *htPerpA;
	TH1D		 *hResolX, *hResolY, *hResolR, *hResolU, *hResolV, *hMinDriftTime, *hMinDriftTime_read, *hDCA, *hLength;
	TGraph2D	 *hXYZcombhits;
	TGraph2D	 *grBy,*grBx;
        TF1              *fitrt,*fitdt;
        //TH1D             *splinederivsmooth;
        //TH1D             *splinederivsmooth[numChambers];
        TH1D             *splinederivsmooth_cham[numChambers];
        TH1D             *splinederivsmooth[numChambers][numLayers_half];
        //TGraph           *calib,*splinederiv;
        //TGraph           *calib_cham[numChambers],*splinederiv;
        TGraph           *calib_cham[numChambers];
        TGraph           *calib[numChambers][numLayers_half];
        //TSpline5         *spline5,*spline5rt;
        //TSpline5         *spline5[numChambers],*spline5rt[numChambers],*spline5Plane[numChambers],*spline5rtPlane[numChambers];
        TSpline5         *spline5_cham[numChambers],*spline5rt_cham[numChambers],*spline5[numChambers][numLayers_half],*spline5rt[numChambers][numLayers_half];
        TVector2         uOrt,vOrt;
        //TMatrixD         C(4,4);
        TMatrixD         C,C0,D,D0;
        TMatrixFSym      covMat[6];
        Double_t         locAngleX,locAngleY;
        //Double_t         ranmin[2],ranmax[2];// r-t calibration range
        Double_t         ranmin_cham[numChambers],ranmax_cham[numChambers];// r-t calibration range
        Double_t         ranmin[numChambers][numLayers_half],ranmax[numChambers][numLayers_half];// r-t calibration range
        //TMatrixD         C[4][4];
        Double_t x5extrapBeam[4],y5extrapBeam[4],x6extrapBeam[4],y6extrapBeam[4],x7extrapBeam[4],y7extrapBeam[4],x8extrapBeam[4],y8extrapBeam[4],beamMomentum,beamMomentum2;
         Int_t           GeantBeamCode;
         Int_t           geantCode(TString particle);
         Double_t        geantMass(Int_t geantcode);
	//typedef multimap<Double_t, Int_t> 	occupMap;
	//typedef occupMap::iterator 		occupIter;
	//occupMap			fMapOccup;
	typedef map<UInt_t,UInt_t> 	occupMap;
	occupMap			fMapOccup[numLayers];
        Bool_t          wireUsed[numLayers][numWiresPerLayer],planeUsed[numLayers];
        Bool_t          areSame(double a, double b);
        Double_t        zLayer[numChambers][numLayers],zLayerExtrap[numChambers][numLayers],zCenter[numChambers];
        Double_t        xyTolerance,radialRange[numRunPeriods];
        //Double_t        ranmin[2],ranmax[2];// r-t calibration range
        //Double_t        scaleSimExTDC;
        TH1D            *ydt[numChambers],*xdt[numChambers],*udt[numChambers],*vdt[numChambers];
        TH2D            *ydtcorrel[numChambers],*xdtcorrel[numChambers],*udtcorrel[numChambers],*vdtcorrel[numChambers];
        TH2D            *uvcorrel[numChambers],*uxvxcorrel[numChambers];
        TH1D            *difxu[numChambers],*difuv[numChambers],*difxv[numChambers];
        TH1D            *difxu_2[numChambers],*difuv_2[numChambers],*difxv_2[numChambers];
        Double_t        sumDeltaPhi;
        //Double_t        xDifSE[numLayers_half],yDifSE[numLayers_half],xDifSE2[numLayers_half],yDifSE2[numLayers_half]; //difference of simulated and experimental track coordinates needed for the alignement
	void		Rotate(UShort_t proj, Double_t x,Double_t y, Double_t& xRot, Double_t& yRot, Bool_t back=false);	
	Double_t	GetDriftLength(UShort_t proj, UShort_t gasgap, Double_t x, Double_t& wirePos);
        Double_t        GetDriftDist(FairMCPoint *dchPoint, UShort_t idch, UShort_t uidLocal, Double_t x, Double_t &wirePos);
	Double_t	GetTShift(Double_t driftLength, Double_t wirePos, Double_t R, Double_t&);
        Double_t        alphaxzBeam[numChambers],alphayzBeam[numChambers];
        Double_t        endBeamPos[numChambers][3];
        Double_t        meanShiftX[numChambers],meanShiftY[numChambers];
        Double_t        magFieldIntegSimul;
	Bool_t 		HitExist(Double_t delta);	
	Double_t	GetPhi(UShort_t proj);
	Int_t		WireID(UInt_t uid, Double_t wirePos, Double_t R);
        Double_t        wirePosition(UShort_t gasgap, UInt_t wirenum, UShort_t uidLocalproj, const UShort_t idch);
        //Double_t        rtCurve(const Double_t time);
        Double_t        rtCurve(const Double_t time,const UShort_t idch, const UShort_t iplane);
        UShort_t        PlanesTopology(const UShort_t idch);
        void            magFieldScale();
        Double_t        MagFieldIntegral(const Bool_t backward, const Double_t parFitL[]);
        //void            ExtrapToDch(const Double_t x[],const Double_t y[],const Double_t zLayer[][numLayers], const UShort_t idch, const UShort_t ijk[], Int_t &jjgr2);
        void            ExtrapToDch(const Double_t x0, const Double_t x1, const Double_t y0, const Double_t y1, const Double_t zLayer[][numLayers], const UShort_t idch, const UShort_t ijk[], Int_t &jjgr2);
	
  	BmnDchHit* 	AddHit0(Int_t index, Int_t detID, const TVector3& posHit, const TVector3& posHitErr, Int_t trackIndex, Int_t pointIndex, Int_t flag, UInt_t dchlayer);
  	BmnDchHit* 	AddHit(const Int_t hitGlobId, const Int_t detID, const TVector3& posHit, const TVector3& posHitErr, const Int_t trackIndex, const Int_t pointIndex, const Int_t flag, const UShort_t dchlayerNumber, const UShort_t idch, const Double_t driftlen, const Double_t hitx, const Int_t hitID);
  	//DchTrackCand* 	AddTrack(Int_t index, Int_t detID, const TVector3& posHit, const TVector3& posHitErr, Int_t trackIndex, Int_t pointIndex, Int_t flag, UInt_t dchlayer);

protected: 

        Double_t meanDeltaPhi;
        //Option_t* opt;
		
public:
        Option_t* opt;

  	BmnDchHitProducer_exp(Bool_t checkDch = false, Bool_t DoCheck = false, UShort_t runperiod=1, UShort_t runnumber=0, UShort_t iter=0, UShort_t filenumber=0);
  	~BmnDchHitProducer_exp();

 	InitStatus	Init(TChain *bmnTree, TClonesArray *dchDigits);
  	void 		Exec(Int_t eventnum);
	//void		Finish(TH1D* &hResidRMSvsIter);
	void		Finish(Bool_t &noError);
        void            SetOnlyPrimary(Bool_t opt1 = kFALSE) { fOnlyPrimary = opt1; }
        void            SetfDoCheck(Bool_t fdocheck) {fDoCheck = fdocheck;}
        void            SetRunPeriod(UShort_t runperiod) {runPeriod = runperiod;}
        void            SetRunNumber(Int_t runnumber) { runNumber = runnumber; }
        void            SetIterationNumber(UShort_t iter) { Iter = iter; }
        void            SetMaxIterationNumber(UShort_t Itermax) { itermax = Itermax; }
        void            SetFileNumber(UShort_t filenumber) {fileNumber = filenumber;}
        void            InitDchParameters();
        void            FileManagement();
        void            MagFieldInit();
        void            BookCheckHistograms();
        void            BookAnalysisHistograms();
        void            BookHistsAngleCorr();
        void            FitHistsAngleCorr();
        //void            LRambiguity(UInt_t k, Double_t hittmp[2], Double_t hitx, Double_t driftdist, UInt_t hw, UShort_t &nhits, Double_t drifttim);
        void            LRambiguity(const UInt_t k, const Double_t hitx, const Double_t driftdist, const UInt_t hw, Double_t drifttim, UShort_t &nhits, Double_t hittmp[2], UShort_t &nwires_nextplane, UInt_t hw_nextplane[2]);
        Double_t LineLineDCA(const Double_t x1, const Double_t y1, const Double_t z1, const Double_t dircosx1, const Double_t dircosy1, const Double_t dircosz1, const Double_t xst, const Double_t sty, const Double_t stz, const Double_t dircosstx, const Double_t dircossty, const Double_t dircosstz, Double_t &x2, Double_t &y2, Double_t &z2, Double_t &xdchwmin, Double_t  &ydchwmin, Double_t &zdchwmin, Double_t &length, Double_t &mu0, Double_t &lambda0);
        Double_t LineLength(const Double_t x1, const Double_t y1, const Double_t z1, const Double_t x2, const Double_t y2, const Double_t z2);
        Double_t        PointLineDistance(const Double_t x0, const Double_t y0, const Double_t z0, const Double_t x, const Double_t y, const Double_t z, const Double_t dircosx, const Double_t dircosy, const Double_t dircosz);
        void LineLineIntersection2D(const Double_t x1, const Double_t y1, const Double_t x2, const Double_t y2, const Double_t x3, const Double_t y3, const Double_t x4, const Double_t y4, Double_t &xp, Double_t &yp); 
        void            RadialRange(FairMCPoint* dchPoint);
        void            HitFinder(const UShort_t idch, const Option_t *opt);
        //void            CoordinateFinder(const Double_t y1,const Double_t x2,const Double_t k3,const Double_t q3,const Double_t k4,const Double_t q4,const UShort_t *ijk, Double_t x[],Double_t y[]);
        //void            CoordinateFinder2(const Double_t x1,const Double_t y2,const Double_t k3,const Double_t q3,const Double_t k4,const Double_t q4,const UShort_t *ijk, Double_t x[],Double_t y[]);
        void            CoordinateFinder3(const Double_t x1,const Double_t y2,const Double_t k3,const Double_t q3,const Double_t k4,const Double_t q4,const UShort_t *ijk, Double_t x[],Double_t y[]);
        void            ReturnPointers(UInt_t ijk,UInt_t i,ScalarD* &hitX,ScalarI* &trackId, ScalarI* &detId,ScalarUI* &hitwire,ScalarD* &driftlen,ScalarD* &drifttim,ScalarI* &pointind);
        //void            rtCalibration(TChain *bmnTree);
        void            rtCalibration();
        //void            tdcInflexPoints(Double_t &inflX1,Double_t &inflX2,const UShort_t idch);
        void            tdcInflexPoints(Double_t &inflX1,Double_t &inflX2,const UShort_t idch,const UShort_t iplane,const Bool_t cham);
        Double_t        SplineIntegral(const Double_t a, const Double_t b, const Int_t n, const TSpline5 *Spline5);
        void            rtCalibCorrection();
        void            rtCalibrationReader();
        void            AlignmentMatrix();
        void            GlobalAlignmentParameters(Bool_t &noError);
        void            AlignmentTracksFilter(Double_t &Xmin,Double_t &Xmax,Double_t &Ymin,Double_t &Ymax,UInt_t &filterTracks,Bool_t aliTrIndex[]);
        void            FinishAlignment(const Bool_t noError);
        void            DchAlignment(XYZPoint &xyzHit1,XYZPoint &xyzHit2,XYZPoint &xyzHit3,XYZPoint &xyzHit4,XYZPoint &xyzWire1,XYZPoint &xyzWire2,XYZPoint &xyzWire3,XYZPoint &xyzWire4,XYZPoint &xyzWire1_2,XYZPoint &xyzWire2_2,XYZPoint &xyzWire3_2,XYZPoint &xyzWire4_2, UShort_t idch);
        void            MisAlignment(XYZPoint &xyzHit1,XYZPoint &xyzHit2,XYZPoint &xyzHit3,XYZPoint &xyzHit4,XYZPoint &xyzWire1,XYZPoint &xyzWire2,XYZPoint &xyzWire3,XYZPoint &xyzWire4);
  	UInt_t 		GetNumberOfAlignmentTracks();
        UInt_t          runSelection();
  	Double_t 	GetSumOfDeltaPhi();
        void            SetMeanDeltaPhi(Double_t meandelphi){meanDeltaPhi = meandelphi;};
        //void            backgroundRemoval();
        void            backgroundRemoval(TH1D *mindrifttime);
        void            SetOpt(Option_t *Opt){opt = Opt;};
        //void            RunBeamTarget(TString &beam, TString &target);
        UShort_t        mapPlaneID(UShort_t Uid);
        static UShort_t GetProj(UShort_t uid){ return uid/2; }; //lsp [0-3] == [x,y,u,v]
        static UShort_t GetGasGap(UShort_t uid){ return uid%2; }; //lsp [0-1] == [inner,outer]
        //UShort_t RunPeriodToNumber(TString runPeriod);
        Double_t minDel(Double_t coor1, Double_t driftdist1,Double_t coor2, Double_t driftdist2);
        double HWHM(const TH1D *h1);
        //void inFieldTracking();
        void GeantInit();
        void GeantBeamTracker();
        void GeantTracker(const Double_t parFitL[]);
        //void GeantStepTracker(Float_t x1[], Float_t p1[], Float_t pli[], Float_t plo[], Int_t nepred, Float_t ein[], Int_t GeantCode, const Bool_t backward, Float_t (&x2)[3], Float_t (&p2)[3]);
        void GeantStepTracker(Float_t x1[], Float_t p1[], Float_t pli[], Float_t plo[], Int_t nepred, Float_t ein[], Int_t geantCode, const Bool_t backward, Float_t x2[], Float_t p2[]);
        //enum runtypes{run1=0,run2=1,run3=0};
        //runtypes current_run;

        static const Double_t cosPhi_45, sinPhi_45, cosPhi45, sinPhi45;
        static const Double_t Phi_45, Phi45, Phi90;
        static const Double_t sqrt2,tg3,tg4;
        static const Double_t WheelR_2;
        static const Double_t resolution;
        static const Double_t hitErr[3];
        //static const Float_t hmintime,hmaxtime;	
        //static const Int_t hnbins;
        static const Float_t hmintime[numRunPeriods],hmaxtime[numRunPeriods];	
        static const Int_t hnbins[numRunPeriods];
        static const UInt_t nintervals;
        static const TVector2 unitU,unitV;
        //static const Double_t Dmomentum; 
        //static const Int_t GeantBeamCode;
  
ClassDef(BmnDchHitProducer_exp,1); 
};
//------------------------------------------------------------------------------------------------------------------------
#endif
