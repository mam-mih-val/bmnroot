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
#include <vector>
#include "FairTask.h"
#include <TDatabasePDG.h>
#include <TH1I.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TMath.h>
#include <TString.h>
#include <TClonesArray.h>
#include <TLorentzVector.h>
#include <TVector2.h>
#include <TF1.h>

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

    // In order to obtain cuts
    TH1F* fLambdaMomenta;
    TH1F* fProtonMomenta; // Initial momenta
    TH1F* fPionMomenta;

    TH1F* fProtonTx; // Initial angles of tracks
    TH1F* fProtonTy;
    TH1F* fPionTx;
    TH1F* fPionTy;

    TH1F* fStartZ; // Z-position of Lambda decay

    TH1F* fEtaProton;
    TH1F* fEtaPion;

    TH1I* fNhitsPerProton;
    TH1I* fNhitsPerPion;
    TH2F* fNhitsPerProtonVsP;
    TH2F* fNhitsPerPionVsP;

    TString fBranchGemHits;
    TString fBranchGemTracks;

    BmnGemStripStationSet* fDetector; // Detector geometry
    BmnGemStripConfiguration::GEM_CONFIG fGeometry;

    Bool_t fUseMc; // Use evetest.root as an input file 
    TString fOutFileName; // output filename

    // Different kinematic cuts
    Double_t fMomProtMin, fMomProtMax;
    Double_t fMomPionMin, fMomPionMax;

    Double_t fTxProtMin, fTxProtMax;
    Double_t fTyProtMin, fTyProtMax;

    Double_t fTxPionMin, fTxPionMax;
    Double_t fTyPionMin, fTyPionMax;

    Double_t fYProtMin, fYProtMax; // Cuts on rapidity values
    Double_t fYPionMin, fYPionMax;

    Bool_t fDebugCalculations; // In case of debug calculations should be equal to 1
    Bool_t fCutsDistrOnly; // Is used in case of evetest-analysis to obtain some cuts used for further restrictions 

    Double_t fInputUncertainties[6]; // dp1, dp2, dTx1, dTx2, dTy1, dTy2 

    Int_t fPdgLambda;
    Int_t fPdgProton;
    Int_t fPdgPionMinus;

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
    void ObtainCuts();

    void SetObtainCutsOnly(Bool_t flag) {
        fCutsDistrOnly = flag;
    }

    void SetDebugCalculations(Bool_t flag) {
        fDebugCalculations = flag;
    }

    void SetMomProtonRange(Double_t m1, Double_t m2) {
        fMomProtMin = m1;
        fMomProtMax = m2;
    }

    void SetMomPionRange(Double_t m1, Double_t m2) {
        fMomPionMin = m1;
        fMomPionMax = m2;
    }

    void SetTxProtonRange(Double_t min, Double_t max) {
        fTxProtMin = min;
        fTxProtMax = max;
    }

    void SetTyProtonRange(Double_t min, Double_t max) {
        fTyProtMin = min;
        fTyProtMax = max;
    }

    void SetTxPionRange(Double_t min, Double_t max) {
        fTxPionMin = min;
        fTxPionMax = max;
    }

    void SetTyPionRange(Double_t min, Double_t max) {
        fTyPionMin = min;
        fTyPionMax = max;
    }

    void SetYProtonRange(Double_t min, Double_t max) {
        fYProtMin = min;
        fYProtMax = max;
    }

    void SetYPionRange(Double_t min, Double_t max) {
        fYPionMin = min;
        fYPionMax = max;
    }

    void SetInputAccuracy(Double_t dp1, Double_t dp2, Double_t dTx1, Double_t dTx2, Double_t dTy1, Double_t dTy2) {
        fInputUncertainties[0] = dp1;
        fInputUncertainties[1] = dp2;
        fInputUncertainties[2] = dTx1;
        fInputUncertainties[3] = dTx2;
        fInputUncertainties[4] = dTy1;
        fInputUncertainties[5] = dTy2;
    }

    vector <Double_t> DebugCalculations(BmnGemTrack*, BmnGemTrack*, vector <Double_t>, vector <Double_t>);

    ClassDef(BmnLambdaInvMass, 0)
};

#endif
