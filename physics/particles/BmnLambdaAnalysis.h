// @(#)bmnroot/physics/particles:$Id$
// Author: Pavel Batyuk <pavel.batyuk@jinr.ru> 2017-04-14

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// BmnLambdaAnalysis                                                           //
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
#include <TGraph.h>
#include <TH1I.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TMath.h>
#include <TString.h>
#include <TClonesArray.h>
#include <TLorentzVector.h>
#include <TVector2.h>
#include <TVector3.h>
#include <TF1.h>
#include <TFitResult.h>
#include "BmnGemTrack.h"
#include "BmnFieldMap.h"
#include "BmnNewFieldMap.h"
#include "CbmMCTrack.h"
#include "CbmStsPoint.h"
#include "CbmVertex.h"
#include "BmnGemStripStationSet.h"
#include "BmnGemStripStationSet_RunWinter2016.h"
#include "BmnGemStripStationSet_RunSpring2017.h"
#include "BmnGemStripHit.h"
#include "BmnKalmanFilter_tmp.h"
#include "BmnParticlePair.h"
#include <UniDbDetectorParameter.h>
#include <UniDbRun.h>

using namespace std;
using namespace TMath;

class BmnLambdaAnalysis : public FairTask {
private:
    UInt_t fEventCounter; //! event counter
    TDatabasePDG* fPDG; //!  PDG database

    TClonesArray* fGemHits;
    TClonesArray* fGemTracks;
    TClonesArray* fVertex;

    CbmVertex* fEventVertex;

    TString fBranchGemHits;
    TString fBranchGemTracks;
    TString fBranchVertex;

    BmnGemStripStationSet* fDetector; // Detector geometry
    BmnGemStripConfiguration::GEM_CONFIG fGeometry;

    Bool_t fUseMc; // Use evetest.root as an input file 
    TString fOutFileName; // output filename

    // Kinematic cuts
    Double_t fMom[2][2]; // [2] --> (proton, pion), [2] --> (min, max)
    Double_t fTx[2][2];
    Double_t fTy[2][2];
    Double_t fY[2][2]; // Cuts on rapidity 

    // Geometry cuts
    Double_t fVertexCuts[3][2]; // [3] --> (x, y, z), [2] --> (min, max)

    Double_t fV0VpDiff[2]; // [2] --> (min, max)
    Double_t fVpVpProton[2];
    Double_t fVpVpPion[2];
    Double_t fV0ProtonPion[2];

    Bool_t fDebugCalculations; // In case of debug calculations should be equal to 1

    Double_t fInputUncertainties[6]; // dp1, dp2, dTx1, dTx2, dTy1, dTy2 

    Int_t fPdgLambda;
    Int_t fPdgProton;
    Int_t fPdgPionMinus;

    FairField* fField;
    BmnFieldMap* fMagField;
    BmnKalmanFilter_tmp* fKalman;

    TClonesArray* fParticlePair;
    TClonesArray* fParticlePairCuts;

public:

    BmnLambdaAnalysis() {
    };
    BmnLambdaAnalysis(BmnGemStripConfiguration::GEM_CONFIG);
    virtual ~BmnLambdaAnalysis();

    virtual void Exec(Option_t * option);
    virtual InitStatus Init();
    virtual void Finish();

    void SetDebugCalculations(Bool_t flag) {
        fDebugCalculations = flag;
    }

    // Geometry cuts

    void SetVpCuts(Double_t xMin, Double_t xMax, Double_t yMin, Double_t yMax, Double_t zMin, Double_t zMax) {
        fVertexCuts[0][0] = xMin;
        fVertexCuts[0][1] = xMax;
        fVertexCuts[1][0] = yMin;
        fVertexCuts[1][1] = yMax;
        fVertexCuts[2][0] = zMin;
        fVertexCuts[2][1] = zMax;
    }

    void SetV0VpDiff(Double_t min, Double_t max) {
        fV0VpDiff[0] = min;
        fV0VpDiff[1] = max;
    }

    void SetVpVpProton(Double_t min, Double_t max) {
        fVpVpProton[0] = min;
        fVpVpProton[1] = max;
    }

    void SetVpVpPion(Double_t min, Double_t max) {
        fVpVpPion[0] = min;
        fVpVpPion[1] = max;
    }

    void SetV0ProtonPion(Double_t min, Double_t max) {
        fV0ProtonPion[0] = min;
        fV0ProtonPion[1] = max;
    }

    // Kinematical cuts

    void SetMomProtonRange(Double_t min, Double_t max) {
        fMom[0][0] = min;
        fMom[0][1] = max;
    }

    void SetMomPionRange(Double_t min, Double_t max) {
        fMom[1][0] = min;
        fMom[1][1] = max;
    }

    void SetTxProtonRange(Double_t min, Double_t max) {
        fTx[0][0] = min;
        fTx[0][1] = max;
    }

    void SetTyProtonRange(Double_t min, Double_t max) {
        fTy[0][0] = min;
        fTy[0][1] = max;
    }

    void SetTxPionRange(Double_t min, Double_t max) {
        fTx[1][0] = min;
        fTx[1][1] = max;
    }

    void SetTyPionRange(Double_t min, Double_t max) {
        fTy[1][0] = min;
        fTy[1][1] = max;
    }

    void SetYProtonRange(Double_t min, Double_t max) {
        fY[0][0] = min;
        fY[0][1] = max;
    }

    void SetYPionRange(Double_t min, Double_t max) {
        fY[1][0] = min;
        fY[1][1] = max;
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

private:
    void RecoAnalysis();
    void Analysis();
    void FindFirstPointOnMCTrack(Int_t, BmnGemTrack*, Int_t);
    TVector3 FitParabola(vector <TVector3>); // XZ-plane
    void CalculateMinDistance(TVector3, TVector3, Double_t*);
    TVector2 SecondaryVertexY(FairTrackParam*, FairTrackParam*); // YZ-plane
    vector <TVector3> KalmanTrackPropagation(BmnGemTrack*, Int_t);
    FairTrackParam KalmanTrackPropagation(BmnGemTrack* track, Int_t, Double_t);
    vector <Double_t> GeometryCuts(FairTrackParam, FairTrackParam, FairTrackParam, FairTrackParam);
    Bool_t CheckTrack(BmnGemTrack*, Int_t);

    inline Int_t CheckSign(Double_t val) {
        return (val > 0) ? 1 : ((val < 0) ? -1 : 0);
    }

    ClassDef(BmnLambdaAnalysis, 0)
};

#endif
