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

    Bool_t fSiRequired; // select glob. tracks with at least one silicon hit
    Bool_t fUseMc; // Use evetest.root as an input file 
    Bool_t fIsUseRealVertex;
    TString fOutFileName; // output filename

    // Kinematic cuts
    Double_t fMom[2][2]; // [2] --> (proton, pion), [2] --> (min, max)
    Double_t fTx[2][2];
    Double_t fTy[2][2];
    Double_t fEta[2][2]; // Cuts on pseudorapidity 

    // Geometry cuts
    Double_t fVertexCuts[3][2]; // [3] --> (x, y, z), [2] --> (min, max)

    Double_t fPath[2];
    Double_t fDCA[2][2]; // [2] --> (proton, pion), [2] --> (min, max)
    Double_t fDCA12[2];

    FairField* fField;
    BmnFieldMap* fMagField;
    BmnKalmanFilter* fKalman;

    TClonesArray* fParticlePair;

    Int_t fPDG1, fPDG2, fPDGDecay, fPdgParticle1, fPdgParticle2;
    Double_t fLeftInvMass, fRightInvMass;
  
    Int_t fN, fN2, fN3, fN4;
    
    TH1F** hSim;
    TH1F** hReco;
    
    TH2F** h2Sim;
    TH2F** h2Reco; 

    TH2F*** h3Sim;
 
public:

    BmnTwoParticleDecay() {
    };
    BmnTwoParticleDecay(BmnGemStripConfiguration::GEM_CONFIG, Int_t r = 1209);
    virtual ~BmnTwoParticleDecay();

    virtual void Exec(Option_t * option);
    virtual InitStatus Init();
    virtual void Finish();

    void SetUseRealVertex(Bool_t flag) {
        fIsUseRealVertex = flag;
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

    // Kinematical cuts
    void SetTxParticle1Range(Double_t min, Double_t max) {
        fTx[0][0] = min;
        fTx[0][1] = max;
    }

    void SetTyParticle1Range(Double_t min, Double_t max) {
        fTy[0][0] = min;
        fTy[0][1] = max;
    }

    void SetTxParticle2Range(Double_t min, Double_t max) {
        fTx[1][0] = min;
        fTx[1][1] = max;
    }

    void SetTyParticle2Range(Double_t min, Double_t max) {
        fTy[1][0] = min;
        fTy[1][1] = max;
    }

    void SetParticlePDG(Int_t pdg1, Int_t pdg2) {
        fPDG1 = pdg1;
        fPDG2 = pdg2;
    }

    void SetSiRequired(Bool_t flag) {
        fSiRequired = flag;
    }

    void SetCuts(Double_t kin[][2], Double_t geom[][2]) {
        for (Int_t i = 0; i < 4; i++)
            for (Int_t j = 0; j < 2; j++) {
                if (i < 2) {
                    fMom[i][j] = kin[i][j];
                    fDCA[i][j] = geom[i][j];
                } else {
                    fEta[i - 2][j] = kin[i][j];
                    if (i == 2)
                        fDCA12[j] = geom[i][j];
                    else
                        fPath[j] = geom[i][j];
                }
            }
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

    void CalculateReconstuctableParticles(); // to be used in case of MC-data

    inline Int_t recoToMcIdx(Int_t iTrack) {
        BmnTrackMatch* globTrackMatch = (BmnTrackMatch*) (fGlobalMatches->UncheckedAt(iTrack));
        return globTrackMatch->GetMatchedLink().GetIndex();
    }

    inline Int_t CheckSign(Double_t val) {
        return (val > 0) ? 1 : ((val < 0) ? -1 : 0);
    }

    ClassDef(BmnTwoParticleDecay, 0)
};

#endif
