// @(#)bmnroot/physics/particles:$Id$
// Author: Pavel Batyuk <pavel.batyuk@jinr.ru> 2017-04-14

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// BmnLambdaInvMass                                                           //
//                                                                            //
//  A supplementary class for Lambda-reconstruction                           //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef BMNLAMBDAINVMASS_H
#define BMNLAMBDAINVMASS_H 1

#include <iostream>
#include "FairTask.h"
#include <TDatabasePDG.h>
#include <TH1F.h>
#include <TMath.h>
#include <TString.h>
#include <TClonesArray.h>
#include <TLorentzVector.h>
#include <TVector2.h>

#include "BmnGemTrack.h"
#include "CbmMCTrack.h"
#include "BmnGemStripStationSet.h"
#include "BmnGemStripStationSet_RunWinter2016.h"
#include "BmnGemStripStationSet_RunSpring2017.h"
#include "BmnGemStripHit.h"

using namespace std;
using namespace TMath;

class BmnLambdaInvMass : public FairTask {
private:
    UInt_t fEventCounter; //! event counter
    TDatabasePDG* fPDG; //!  PDG database

    TClonesArray* fGemHits;
    TClonesArray* fGemTracks;

    TH1F* fLambdaInvMass;

    TString fBranchGemHits;
    TString fBranchGemTracks;

    BmnGemStripStationSet* fDetector; // Detector geometry
    BmnGemStripConfiguration::GEM_CONFIG fGeometry;

    Bool_t fUseMc; // Use evetest.root as an input file 
    TString fOutFileName; // output filename

    // Different kinematic cuts
    Double_t fMomProtMin, fMomProtMax;
    Double_t fMomPionMin, fMomPionMax;


public:

    BmnLambdaInvMass() {
    };
    BmnLambdaInvMass(BmnGemStripConfiguration::GEM_CONFIG);
    virtual ~BmnLambdaInvMass();

    virtual void Exec(Option_t * option);
    virtual InitStatus Init();
    virtual void Finish();

    void RecoAnalysis();
    void McAnalysis();

    void SetMomProtonRange(Double_t m1, Double_t m2) {
        fMomProtMin = m1;
        fMomProtMax = m2;
    }

    void SetMomPionRange(Double_t m1, Double_t m2) {
        fMomPionMin = m1;
        fMomPionMax = m2;
    }

    ClassDef(BmnLambdaInvMass, 0)
};

#endif
