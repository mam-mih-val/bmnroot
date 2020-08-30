#include <TNamed.h>
#include <TChain.h>
#include <TObject.h>
#include <TClonesArray.h>
#include <TGeoManager.h>
#include <TH1.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TLine.h>

#include "BmnInnerTrackerGeometryDraw.h"
#include <BmnGlobalTrack.h>
#include <BmnHit.h>
#include <BmnKalmanFilter.h>
#include <DstEventHeader.h>
#include <UniDbRun.h>
#include <CbmVertex.h>
#include "BmnNewFieldMap.h"

#include <vector>

#ifndef BMNEFF_H
#define BMNEFF_H 1

using namespace std;

// A class to be used as a store when calculating efficiency ...
class EffStore : public TObject {
public:

    EffStore() :
    zone(""), detector(""),
    station(-1), module(-1),
    nominator(0), denominator(0) {



    }

    EffStore(TString det, Int_t stat, Int_t mod, TString z = "") :
    zone(z), detector(det),
    station(stat), module(mod),
    nominator(0), denominator(0) {
        hitPositionsAndEff.resize(0);
    }

    void IncreaseNominatorByUnity() {
        nominator++;
    }

    void IncreaseDenominatorByUnity() {
        denominator++;
    }
    
    void AddHitCoordinates(BmnHit* hit, Double_t eff = 1.) {
        hitPositionsAndEff.push_back(TLorentzVector(hit->GetX(), hit->GetY(), hit->GetZ(), eff));
    }

    Double_t Efficiency() {
        Double_t eff = 0.;
        if (denominator != 0)
            eff = 1. * nominator / denominator;

        return eff;
    }
    
    TString Detector() {
        return detector;
    }
    
    Int_t Station() {
        return station;
    }
    
    Int_t Module() {
        return module;
    } 
    
    TString Zone() {
        return zone;
    }
    
    vector <TLorentzVector> CoordinatesAndEffs() {
        return hitPositionsAndEff;
    }

    virtual ~EffStore() {
        ;
    }

private:
    TString detector; // GEM or SILICON
    Int_t station;
    Int_t module;
    TString zone; // big or hot (valid for GEM only!)

    Int_t nominator;
    Int_t denominator;
    
    vector <TLorentzVector> hitPositionsAndEff;
                
    ClassDef(EffStore, 1)
};

class BmnEfficiency : public TNamed {
public:

    BmnEfficiency() {

    }

    BmnEfficiency(TString);

    void IsPrimaryTrack(Bool_t flag) {
        isTrackFromVp = flag;
    }
    
    void SetMinNHitsPerGlobTrack(Int_t nHits) {
        fNHits = nHits;
    }
    
    void SetMinNHitsPerSiliconTrack(Int_t nHits) {
        fNHitsSilicon = nHits;
    }
    
    void SetMinNHitsPerGemTrack(Int_t nHits) {
        fNHitsGem = nHits;
    }

    void Efficiency();

    virtual ~BmnEfficiency() {
        if (dstChain)
            delete dstChain;
        
        if (fKalman)
            delete fKalman;

    }

private:
    // Geometries
    BmnGemStripStationSet* gem;
    BmnSiliconStationSet* silicon;
    
    DstEventHeader* fHeader;

    TClonesArray* effGem;
    TClonesArray* effSilicon;

    TClonesArray* fInnerHits;
    TClonesArray* fGemHits;
    TClonesArray* fSiliconHits;
    TClonesArray* fGlobTracks;
    TClonesArray* fGemTracks;
    TClonesArray* fSiliconTracks;
    TClonesArray* fVertices;

    TChain* dstChain;
    
    FairField* fField;
    BmnFieldMap* fMagField;
    BmnKalmanFilter* fKalman;
    
    map <Int_t, Double_t> fStatZ;
    map <Int_t, vector <Double_t>> fStatAcceptance;
    
    Bool_t isTrackFromVp;
    Int_t fNHits;
    Int_t fNHitsSilicon;
    Int_t fNHitsGem;

    Bool_t isInAcceptance(Int_t, Double_t, Double_t);

    inline TString GetDetector(BmnHit* hit) {
        Double_t z = hit->GetZ();

        TString det = "";
        if (z > gem->GetStation(0)->GetZPosition() - 5.)
            det = "GEM";
        else
            det = "SILICON";

        return det;
    }
    
    TString GetGemZone(BmnHit*);
    Int_t GetSiliconStatModule(BmnHit*);
    Int_t GetGemStatModule(BmnHit*);
    
    inline Double_t FindZ(Int_t stat) {        
        return fStatZ.find(stat)->second;
    }

    ClassDef(BmnEfficiency, 1)
};

#endif