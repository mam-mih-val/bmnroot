// @(#)bmnroot/physics/particles:$Id$
// Author: Pavel Batyuk <pavel.batyuk@jinr.ru> 2017-12-27

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// BmnTwoParticleDecay                                                        //
//                                                                            //
//  A supplementary class for two-body decay reconstruction                   //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef BMNTWOPARTICLEDECAY_H
#define BMNTWOPARTICLEDECAY_H 1

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
#include "BmnGlobalTrack.h"
#include "BmnFieldMap.h"
#include "BmnNewFieldMap.h"
#include "CbmMCTrack.h"
#include "CbmStsPoint.h"
#include "CbmVertex.h"
#include "BmnGemStripStationSet.h"
#include "BmnGemStripStationSet_RunWinter2016.h"
#include "BmnGemStripStationSet_RunSpring2017.h"
#include "BmnGemStripHit.h"
#include "BmnKalmanFilter.h"
#include "BmnParticlePair.h"
#include <UniDbDetectorParameter.h>
#include <UniDbRun.h>
#include "BmnTrackMatch.h"

using namespace std;
using namespace TMath;

class BmnTwoParticleDecay : public FairTask {
private:
    Int_t fRunPeriod; // run period (5, 6 ...) 6 is set by default
    Int_t fRunId; // runID
    UInt_t fEventCounter; //! event counter
    TDatabasePDG* fPDG; //!  PDG database

    TClonesArray* fGemPoints;
    TClonesArray* fGlobalTracks;
    TClonesArray* fMCTracks;
    TClonesArray* fGlobalMatches;
    TClonesArray* fVertex;

    TVector3 fMcVertex;
    CbmVertex* fEventVertex;

    TString fBranchGemPoints;
    TString fBranchGlobalTracks;
    TString fBranchMCTracks;
    TString fBranchGlobalMatch;
    TString fBranchVertex;

    BmnGemStripStationSet* fDetector; // Detector geometry
    BmnGemStripConfiguration::GEM_CONFIG fGeometry;

    Bool_t fIsUseRealVertex;

    // Kinematic cuts
    Double_t fMom[2][2]; // [2] --> (proton, pion), [2] --> (min, max)
    Double_t fTx[2][2];
    Double_t fTy[2][2];
    Double_t fEta[2][2]; // Cuts on pseudorapidity 

    Double_t fPath[2];
    Double_t fDCA[2][2]; // [2] --> (proton, pion), [2] --> (min, max)
    Double_t fDCA12[2];

    FairField* fField;
    BmnFieldMap* fMagField;
    BmnKalmanFilter* fKalman;
  
    TClonesArray* fParticlePairsInfo;
    
    TClonesArray* fParticlePair_MC_noCuts;
    TClonesArray* fParticlePair_MC_withCuts;
    
    TClonesArray* fParticlePair_RECO_noCuts;
    TClonesArray* fParticlePair_RECO_withCuts;
    
    TClonesArray* fParticlePair;

    Int_t fPDG1, fPDG2, fPDGDecay, fPdgParticle1, fPdgParticle2;

    vector <TString> fAnalType;

public:

    BmnTwoParticleDecay() {
    };
    BmnTwoParticleDecay(BmnGemStripConfiguration::GEM_CONFIG config);
    virtual ~BmnTwoParticleDecay();

    virtual void Exec(Option_t * option);
    virtual InitStatus Init();
    virtual void Finish();

    void SetUseRealVertex(Bool_t flag) {
        fIsUseRealVertex = flag;
    }

    // Geometry cuts
    void SetDCA1(Double_t min, Double_t max) {
        fDCA[0][0] = min;
        fDCA[0][1] = max;   
    }
    
    void SetDCA2(Double_t min, Double_t max) {
        fDCA[1][0] = min;
        fDCA[1][1] = max;
    }
    
    void SetDCA12(Double_t min, Double_t max) {
        fDCA12[0] = min;
        fDCA12[1] = max;
    }
    
    void SetPath(Double_t min, Double_t max) {
        fPath[0] = min;
        fPath[1] = max;
    }

    // Kinematical cuts
    void SetMom1(Double_t min, Double_t max) {
        fMom[0][0] = min;
        fMom[0][1] = max;
    }
    
    void SetMom2(Double_t min, Double_t max) {
        fMom[1][0] = min;
        fMom[1][1] = max;
    }
    
    void SetEta1(Double_t min, Double_t max) {
        fEta[0][0] = min;
        fEta[0][1] = max;
    }
    
    void SetEta2(Double_t min, Double_t max) {
        fEta[1][0] = min;
        fEta[1][1] = max;
    }
        
    void SetTx1(Double_t min, Double_t max) {
        fTx[0][0] = min;
        fTx[0][1] = max;
    }

    void SetTy1(Double_t min, Double_t max) {
        fTy[0][0] = min;
        fTy[0][1] = max;
    }

    void SetTx2(Double_t min, Double_t max) {
        fTx[1][0] = min;
        fTx[1][1] = max;
    }

    void SetTy2(Double_t min, Double_t max) {
        fTy[1][0] = min;
        fTy[1][1] = max;
    }

    void SetParticlePDG(Int_t pdg1, Int_t pdg2) {
        fPDG1 = pdg1;
        fPDG2 = pdg2;
    }

private:
    void Analysis();
    void FindFirstPointOnMCTrack(Int_t, BmnGlobalTrack*, Int_t);
    TVector3 FitParabola(vector <TVector3>); // XZ-plane
    void CalculateMinDistance(TVector3, TVector3, Double_t*);
    TVector2 SecondaryVertexY(FairTrackParam*, FairTrackParam*); // YZ-plane
    vector <TVector3> KalmanTrackPropagation(BmnGlobalTrack*, Int_t);
    FairTrackParam KalmanTrackPropagation(BmnGlobalTrack* track, Int_t, Double_t);
    vector <Double_t> GeomTopology(FairTrackParam, FairTrackParam, FairTrackParam, FairTrackParam);
    Bool_t CheckTrack(BmnGlobalTrack*, Int_t, Double_t&, Double_t&);

    inline Int_t recoToMcIdx(Int_t iTrack) {
        BmnTrackMatch* globTrackMatch = (BmnTrackMatch*) (fGlobalMatches->UncheckedAt(iTrack));
        return globTrackMatch->GetMatchedLink().GetIndex();
    }

    inline Int_t CheckSign(Double_t val) {
        return (val > 0) ? 1 : ((val < 0) ? -1 : 0);
    }

    TVector2 ArmenterosPodol(FairTrackParam, FairTrackParam);
    Double_t FindV0ByVirtualPlanes(BmnGlobalTrack*, BmnGlobalTrack*, Double_t, Double_t range = 50.);

    ClassDef(BmnTwoParticleDecay, 0)
};

#endif
