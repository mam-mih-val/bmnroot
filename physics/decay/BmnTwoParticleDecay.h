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
#include "BmnSiliconPoint.h"
#include "BmnVertex.h"
#include "CbmVertex.h"
#include "BmnGemStripStationSet.h"
#include "BmnGemStripStationSet_RunWinter2016.h"
#include "BmnGemStripStationSet_RunSpring2017.h"
#include "BmnGemStripHit.h"
#include "BmnKalmanFilter.h"
#include <BmnParticlePair.h>
#include <UniDetectorParameter.h>
#include <UniRun.h>
#include "BmnTrackMatch.h"
#include <DstEventHeader.h>
#include <BmnEventHeader.h>

using namespace std;
using namespace TMath;

// Extenion of "standard" dst event header by info to be used in dec. particle anal ... 

class DstEventHeaderExtended : public DstEventHeader {
private:
    /** Event Header branch name **/
    TString fHeaderName; //!

    Int_t fRecoTracks; // track multiplicity
    Int_t fBD; // number of BD-counts per event
    Int_t fFD; // number of FD-counts per event

    TVector3 fVp; // reconstructed prim. vertex

public:

    DstEventHeaderExtended() {
        fHeaderName = "DstEventHeaderExtended.";

        fBD = 0;
        fFD = 0;
        fRecoTracks = 0;

        fVp.SetXYZ(-1., -1., -1.);
    }

    DstEventHeaderExtended(UInt_t run_id, UInt_t event_id) {
        fBD = 0;
        fFD = 0;
        fRecoTracks = 0;

        fVp.SetXYZ(-1., -1., -1.);
    }

    // Getters ...

    Int_t bd() {
        return fBD;
    }

    Int_t fd() {
        return fFD;
    }

    Int_t nTracks() {
        return fRecoTracks;
    }

    TVector3 Vp() {
        return fVp;
    }

    // Setters ...

    void SetBd(Int_t n) {
        fBD = n;
    }

    void SetFd(Int_t n) {
        fFD = n;
    }
    
    void SetVp(Double_t x, Double_t y, Double_t z) {
        fVp.SetXYZ(x, y, z);   
    }
    
     void SetNTracks(Int_t n) {
        fRecoTracks = n;   
    }

    virtual ~DstEventHeaderExtended() {
        ;
    }

    ClassDef(DstEventHeaderExtended, 1)
};

class BmnTwoParticleDecay : public FairTask {
private:
    Int_t fRunPeriod; // run period
    Int_t fRunId; // runID
    UInt_t fEventCounter; //! event counter
    TDatabasePDG* fPDG; //!  PDG database

    TClonesArray* fGemPoints;
    TClonesArray* fSilPoints;
    TClonesArray* fSilHits;

    TClonesArray* fGlobalTracks;
    TClonesArray* fGemTracks;
    TClonesArray* fSiliconTracks;

    TClonesArray* fMCTracks;
    TClonesArray* fGlobalMatches;
    TClonesArray* fVertex;

    TVector3 fMcVertex;
    BmnVertex* fEventVertex;

    TString fBranchGemPoints;
    TString fBranchSilPoints;
    TString fBranchGlobalTracks;
    TString fBranchMCTracks;
    TString fBranchGlobalMatch;
    TString fBranchVertex;

    BmnGemStripStationSet* fDetector; // Detector geometry
    BmnGemStripConfiguration::GEM_CONFIG fGeometry;

    FairField* fField;
    BmnFieldMap* fMagField;
    BmnKalmanFilter* fKalman;

    TClonesArray* fParticlePair;
    TClonesArray* fParticlePair_MC;
    TClonesArray* fParticlePair_RECO;

    Int_t fPDG1, fPDG2, fPDGDecay, fPdgParticle1, fPdgParticle2;

    vector <TString> fAnalType;
    vector <double> fWeightsModel;

    Bool_t fIsCbmDst;
    TString fGeomFile;
    TString fDigiDir;
    TString fEoSNode;
    
    // Dst event header ...
    DstEventHeader* fDstHeader;
    
    // Adding PhysInfo ...
    DstEventHeaderExtended* fPhysInfo;
    
    map <UInt_t, pair <Int_t, Int_t>> fTrigCountMap; // evId --> (bd, fd)

public:

    BmnTwoParticleDecay() {
    };
    BmnTwoParticleDecay(BmnGemStripConfiguration::GEM_CONFIG config, Int_t runId = 4649);
    virtual ~BmnTwoParticleDecay();

    virtual void Exec(Option_t * option);
    virtual InitStatus Init();
    virtual void Finish();

    void SetParticlePDG(Int_t pdg1, Int_t pdg2) {
        fPDG1 = pdg1;
        fPDG2 = pdg2;
    }

    void SetGeometryFile(TString file) {
        fGeomFile = file;
    }
    
    void SetDigiDir(TString dir) {
        fDigiDir = dir;
    }
    
    void SetEoSNode(TString node) {
        fEoSNode = node;
    }

private:
    void Analysis();
    BmnStatus FindFirstPointOnMCTrack(Int_t, BmnGlobalTrack*, Int_t);
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
