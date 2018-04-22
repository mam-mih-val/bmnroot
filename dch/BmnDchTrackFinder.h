// @(#)bmnroot/dch:$Id$
// Author: Pavel Batyuk (VBLHEP) <pavel.batyuk@jinr.ru> 2017-01-21

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// BmnDchTrackFinder                                                          //
//                                                                            //
// Implementation of an algorithm developed by                                // 
// N.Voytishin and V.Paltchik (LIT)                                           //
// to the BmnRoot software                                                    //
//                                                                            //
// The algorithm serves for searching for track segments                      //
// in the Drift Chambers of the BM@N experiment                               //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef BMNDCHTRACKFINDER_H
#define BMNDCHTRACKFINDER_H 1

#include <fstream>

#include <TMath.h>
#include <TNamed.h>
#include <TString.h>
#include <TClonesArray.h>
#include <TVector2.h>
#include <Rtypes.h>
#include "FairTask.h"
#include "FairTrackParam.h"
#include "FairMCPoint.h"

#include "BmnDchDigit.h"
#include "BmnDchHit.h"
#include "BmnDchTrack.h"
#include "BmnMath.h"

using namespace std;
using namespace TMath;

class BmnDchTrackFinder : public FairTask {
public:

    BmnDchTrackFinder() {};
    BmnDchTrackFinder(Bool_t);
    virtual ~BmnDchTrackFinder();
    
    virtual InitStatus Init();

    virtual void Exec(Option_t* opt);

    virtual void Finish();
       
    void SetTransferFunction(TString name) {
        fTransferFunctionName = name;
    }
    
    struct PointPair
    {
    	Int_t idx1;
    	Int_t idx2;
    	PointPair(Int_t i1, Int_t i2){
    		idx1=i1;
    		idx2=i2;
    	}
    };
    struct Point
    {
    	Double_t x;
    	Double_t z;
    	Int_t number;
    	Int_t digiIdx;
    	Point(Double_t xP, Double_t zP, Int_t digiIdxP)
    	{
    		x=xP;
    		z=zP;
    		number=0;
    		digiIdx = digiIdxP;
    	}
    };

    typedef std::vector<Int_t>      Points;
    typedef std::vector<Point*>      Hits;
    struct Cluster
    {
    	vector<Int_t> hits;
    	Double_t xmin;
    	Double_t xmax;
    	Double_t ymin;
    	Double_t ymax;
    	Double_t parx[2];
    	Double_t pary[2];
    };

    struct Point3D
    {
    	Double_t x;
    	Double_t y;
    	Double_t z;
    	Int_t digiIdx;
    	Point3D(Double_t xP, Double_t yP, Double_t zP, Int_t digiIdxP)
    	{
    		x=xP;
    		y=yP;
    		z=zP;
    		digiIdx = digiIdxP;
    	}
    };
    struct RecoPoint3D
    {
    	Double_t x;
    	Double_t y;
    	Double_t z;
    	Int_t digiIdx;
    	Int_t lr;
    	RecoPoint3D(Double_t xP, Double_t yP, Double_t zP, Int_t digiIdxP, Int_t lrP)
    	{
    		x=xP;
    		y=yP;
    		z=zP;
    		digiIdx = digiIdxP;
    		lr = lrP;
    	}
    };
    struct RecoCluster
    {
    	Double_t xoz[2];
    	Double_t yoz[2];
    	Int_t nHits;
    	std::vector<RecoPoint3D*> hits;
    };
    struct Track
    {
    	Double_t xoz[2];
    	Double_t yoz[2];
    	//Int_t nHits;
    	std::vector<Point3D*> hits;
    };
    struct Point3DC
    {
    	Double_t x;
    	Double_t y;
    	Double_t z;
    	Int_t digiIdx1;
    	Int_t digiIdx2;
    	Point3DC(Double_t xP, Double_t yP, Double_t zP, Int_t digiIdxP1, Int_t digiIdxP2)
    	{
    		x=xP;
    		y=yP;
    		z=zP;
    		digiIdx1 = digiIdxP1;
    		digiIdx2 = digiIdxP2;
    	}
     Point3DC(){}
    };
    struct TrackC
    {
    	Double_t xoz[2];
    	Double_t yoz[2];
    	//Int_t nHits;
    	std::vector<Point3DC*> hits;
    };
    struct RecoPoint
    {
    	Double_t x;
    	Double_t y;
    	Double_t z;
    	Double_t driftTime;
    	Int_t plane;
    	Int_t wire;
    	RecoPoint(Double_t xp,Double_t yp,Double_t zp, Int_t planeP, Int_t wireP, Double_t driftTimeP)
    	{
    		x=xp;
    		y=yp;
    		z=zp;
    		plane=planeP;
    		wire=wireP;
    		driftTime=driftTimeP;
    	}
    };
     
private:
    Int_t N;
    Bool_t expData;
    UInt_t fEventNo; // event counter
    
    TString InputDigitsBranchName;
    TString tracksDch;
    
    TString fTransferFunctionName;
    
    TClonesArray* fBmnDchDigitsArray;
    TClonesArray* fDchTracks;    
    TClonesArray* fDchHits;    
    
    Int_t prev_wire;
    Int_t prev_time;
    
    Int_t nChambers; // dch1 or dch2
    Int_t nWires;    // corresponding to the found v(0), u(1), y(2), x(3)-pairs
    Int_t nLayers;   // a(0) or b(1)
    Int_t nSegmentsMax; // Max. number of found segments to store
    
    Double_t t_dc[5][16]; //[time interval][plane number]
    Double_t pol_par_dc[3][5][16]; //[polinom number][param number][plane number]
    Double_t scaling[16]; //[plane number]
    
    Double_t scale;  //scaling for transfer function coeffs
    
    Double_t rtRel[16][400];
    Double_t z_loc[8];   // z local xa->vb (cm)
    Double_t z_glob[16]; // z global dc 1 & dc 2 (cm)
    Double_t Z_dch1; //z coord in the middle of dch1	
    Double_t Z_dch2; //z coord in the middle of dch2
    Double_t Z_dch_mid; //z coord between the two chambers, this z is considered the global z for the matched dch-segment
    Double_t dZ_dch_mid; 
    Double_t dZ_dch;
    
    Double_t x1_sh; 
    Double_t x2_sh; 
    Double_t y1_sh;
    Double_t y2_sh;
    
    Double_t x1_slope_sh;
    Double_t y1_slope_sh; 
    Double_t x2_slope_sh;
    Double_t y2_slope_sh; 
    
    Bool_t* has7DC;
    
    Int_t nSegmentsToBeMatched;
    
    Double_t** x_global;
    Double_t** y_global;
    Double_t** Chi2;
        
    Double_t*** v;
    Double_t*** u;
    Double_t*** y;
    Double_t*** x;
    Double_t*** v_Single;
    Double_t*** u_Single;
    Double_t*** y_Single;
    Double_t*** x_Single;
    Double_t*** sigm_v;
    Double_t*** sigm_u;
    Double_t*** sigm_y;
    Double_t*** sigm_x;
    Double_t*** Sigm_v_single;
    Double_t*** Sigm_u_single;
    Double_t*** Sigm_y_single;
    Double_t*** Sigm_x_single;
    Double_t*** params;
    Double_t*** rh_segment;
    Double_t*** rh_sigm_segment;
    
    Int_t* nSegments;
    Int_t** pairs;
    Int_t** segment_size;
    Int_t*** singles;
    
    Double_t* x_mid; //x glob of matched segment in the z situated between the two DCH chambers
    Double_t* y_mid; //y glob of matched segment in the z situated between the two DCH chambers
    Double_t* aX; //x slope of the matched segment
    Double_t* aY; //y slope of the matched segment
    Double_t* imp; //reconstructed particle trajectory momentum 
    Double_t* leng; //the distance from z = 0 to the global point of the matched segment
    Double_t* Chi2_match; //chi2 of the matched seg

    std::vector<Int_t> planes[16];
    std::vector<BmnDchDigit*> digisDCH;
    std::vector<PointPair*> pairxa;
    std::vector<PointPair*> pairxb;
    std::vector<PointPair*> pairya;
    std::vector<PointPair*> pairyb;
    std::vector<PointPair*> pairua;
    std::vector<PointPair*> pairub;
    std::vector<PointPair*> pairva;
    std::vector<PointPair*> pairvb;
    std::vector<Int_t> excludedDidigs; // перевести на id хитов а не дигитов, тогда будет работать
    TClonesArray* fMCPointArray;
    TClonesArray* fMCTracks;
    TClonesArray* fBmnDchPointsArray;
	vector<Cluster*> aC;
	vector<Cluster*> bC;


    UChar_t  GetPlane(Double_t z);
    Double_t Distance(Double_t x1,Double_t y1,Double_t x2,Double_t y2,Double_t x0,Double_t y0);
    Double_t GetDistance(UChar_t plane, Double_t xD, Double_t yD, Int_t iWire);
    Short_t  GetClosestWireNumber(UChar_t plane, Double_t xD, Double_t yD, Double_t *dist); 
    Double_t  GetTime(Double_t* distance);
    Bool_t CheckPointGEO(Int_t plane, Double_t xD, Double_t yD, Double_t z);
    void ClustersPreparation();
    void ClustersPreparationRT();
    void RecoDCHCluster();
    Double_t  GetDDistance(Double_t time, Int_t plane);
    void GetCoordinatesWirePoint(UChar_t plane, Double_t xT, Double_t yT, Int_t iWire, Double_t* xW, Double_t* yW, Double_t ddist);
    void RotatePoint(Double_t xc, Double_t yc, Double_t x1, Double_t y1, Double_t* xW, Double_t* yW, Int_t angle);
    void GetBEPoints(Int_t idx1, Double_t* x1, Double_t* y1, Double_t* x2, Double_t* y2, Int_t lr);  
    void GetTrWPoints(Int_t idx1, Double_t xTr, Double_t yTr, Double_t* xr, Double_t* yr, Double_t xMC, Double_t yMC);
    void FillLR(Int_t idx1, Double_t xTr, Double_t yTr, Double_t xMC, Double_t yMC, Int_t global);
    void GetClusterHitsArray(Cluster* cluster, Int_t* ret);
    Bool_t FitItera(Int_t* clI, Double_t* parx, Double_t* pary, Double_t* parx1, Double_t* pary1, Double_t *zz11x, Double_t *zz11y, Bool_t fillHisto, FairMCPoint* pnt1, FairMCPoint* pnt2, Int_t sp, Double_t *chi2x, Double_t *chi2y);
    Bool_t FitIteraGlobal(Int_t* clIa, Int_t* clIb, Double_t* parx, Double_t* pary, Double_t* parx1, Double_t* pary1, Double_t *zz11x, Double_t *zz11y, Double_t *zz21x, Double_t *zz21y,Bool_t fillHisto, FairMCPoint* pnt1, FairMCPoint* pnt2, Int_t sp, Double_t *chi2x, Double_t *chi2y);
    void LRQA(Int_t* clI, Double_t* parx, Double_t* pary, Double_t *zz11x, Double_t *zz11y, FairMCPoint* pnt1, FairMCPoint* pnt2, Int_t global);
    Double_t GetCoordinateByTwoPoints(Double_t x1, Double_t x2, Double_t z1, Double_t z2, Double_t zc);
    Bool_t CombInitial(Int_t* clIa, Double_t* zz, Double_t* parx, Double_t *pary);
    void InitialHisto(Int_t* clI, Double_t* parx, Double_t* pary, Double_t *zz11x, Double_t *zz11y, FairMCPoint* pnt1, FairMCPoint* pnt2);
    Int_t  GetRTIndex(Double_t time);
    Double_t LinePropagation(Double_t x1, Double_t y1, Double_t x2, Double_t y2, Double_t xs);
    void GetWirePairBE(Int_t idx1, Int_t idx2, Double_t* x1b, Double_t* x1e, Double_t* y1b, Double_t* y1e, Double_t* x2b, Double_t* x2e, Double_t* y2b, Double_t* y2e, Int_t lr);


    Bool_t PrepareDigitsFromMC();
    void CreateDchTrack();
    void CreateDchTrack(Int_t, Double_t*, Double_t**, Int_t*);

    void SelectLongestAndBestSegments(Int_t, Int_t*, Double_t**, Double_t*);
    void CompareDaDb(Double_t, Double_t&);
    void CompareDaDb(Double_t, Double_t&, Double_t&);
    void PrepareArraysToProcessEvent();
    Bool_t FitDchSegments(Int_t, Int_t*, Double_t**, Double_t**, Double_t**, Double_t*, Double_t*, Double_t*);
    Double_t CalculateResidual(Int_t, Int_t, Double_t**, Double_t**);
    Int_t BuildUVSegments(Int_t, Int_t, Int_t, Int_t, Int_t, Int_t, Int_t, Int_t, Int_t,
        Double_t**, Double_t**, Double_t**, Double_t**,
        Double_t**, Double_t**, Double_t**, Double_t**,
        Double_t**, Double_t**, Double_t**,
        Double_t**, Double_t**, Double_t**);
    Int_t BuildXYSegments(Int_t, Int_t, Int_t, Int_t, Int_t, Int_t, Int_t, Int_t, Int_t,
        Double_t**, Double_t**, Double_t**, Double_t**,
        Double_t**, Double_t**, Double_t**, Double_t**,
        Double_t**, Double_t**, Double_t**,
        Double_t**, Double_t**, Double_t**);
    Int_t Reconstruction(Int_t, TString, Int_t, Int_t, Int_t,
        Double_t*, Double_t*, Double_t*, Double_t*,
        Bool_t*, Bool_t*,
        Double_t**, Double_t**);
    Int_t ReconstructionSingle(Int_t, TString, TString, Int_t, Int_t,
        Double_t*, Double_t*, Bool_t*,
        Double_t**, Double_t**);
    void SegmentsToBeMatched();
         
    ClassDef(BmnDchTrackFinder, 1)
};

#endif
