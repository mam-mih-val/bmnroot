/* 
 * File:   BmnSeedFinder.h
 * Author: Sergey Merts
 *
 * Created on October 28, 2014, 12:07 PM
 */

#ifndef BMNSEEDFINDER_H
#define	BMNSEEDFINDER_H


// Base Class Headers ----------------

#include "FairTask.h"
#include "TClonesArray.h"
#include "TString.h"
#include "BmnGemTrack.h"
#include <iostream>
#include <vector>
#include "TMath.h"
#include "TVector3.h"
#include "FairMCPoint.h"
#include "CbmMCTrack.h"
#include "CbmStsPoint.h"
#include "BmnEnums.h"
#include "FairRunAna.h"
#include "FairField.h"
#include "BmnHit.h"
#include "BmnDetectorSetup.h"
#include "TH1F.h"

using namespace std;

class BmnSeedFinder : public FairTask {
public:

    // Constructors/Destructors ---------
    BmnSeedFinder();
    virtual ~BmnSeedFinder();

    void FindXYRSeeds(TH1F* h);
    void FindSeeds(Int_t station, Int_t gate, Bool_t isIdeal);
    UInt_t SearchTrackCandidates(Int_t startStation, Int_t gate, Bool_t isIdeal, Bool_t isLeft);
    void SearchTrackCandInLine(const Int_t i, const Int_t y, BmnGemTrack* tr, Int_t* hitCntr, Int_t* maxDist, Int_t* dist, Int_t* startBin, Int_t* prevStation, Int_t gate, Bool_t isIdeal);
    Bool_t CalculateTrackParams(BmnGemTrack* tr, vector<BmnHit*> hits, TVector3 circPar, TVector3 linePar);
    BmnStatus DoSeeding();
    BmnStatus DoSeedingRun1(Int_t nBins);
    TVector3 CircleFit(vector<BmnHit*> hits);
    TVector3 LineFit(vector<BmnHit*> hits);

    void SetRun1(Bool_t run) {
        isRUN1 = run;
    }

    void SetMakeQA(Bool_t qa) {
        fMakeQA = qa;
    }

    //some useful functions
    Float_t Dist(Float_t x1, Float_t y1, Float_t x2, Float_t y2);
    Float_t Sqr(Float_t x);


    virtual InitStatus Init();
    virtual void Exec(Option_t* opt);
    virtual void Finish();

private:

    // Private Data Members ------------
    TString fHitsBranchName;
    TString fSeedsBranchName;

    Bool_t isRUN1; // temporary flag for switching between full geometry and prototype

    Bool_t fMakeQA; // create or not in output tree branch with QA histograms
    Bool_t isHistogramsInitialized; // is QA histograms initialized or not

    Int_t fNBins; // number of bins in histogram  
    Float_t fMin;
    Float_t fMax;
    Float_t fWidth;

    Int_t fEventNo; // event counter
    BmnDetectorSetup fDet; // Detector presence information

    TClonesArray* fGemHitsArray;
    TClonesArray* fMwpc1HitsArray;
    TClonesArray* fMwpc2HitsArray;
    TClonesArray* fMwpc3HitsArray;
    vector<BmnHit*> fSeedHits;
    TClonesArray* fSeedsArray;
    TClonesArray* fMCTracksArray;
    TClonesArray* fGemPointsArray;
    TClonesArray* fMwpc1PointsArray;
    TClonesArray* fMwpc2PointsArray;
    TClonesArray* fMwpc3PointsArray;

    FairField* fField;

    ClassDef(BmnSeedFinder, 1);
};


#endif	/* BMNSEEDFINDER_H */

