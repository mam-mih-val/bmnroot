#include <TNamed.h>
#include <TChain.h>
#include <TObject.h>
#include <TClonesArray.h>
#include <TGeoManager.h>
#include <TH1.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TLine.h>

#include <BmnInnerTrackerGeometryDraw.h>
#include <DstEventHeader.h>
#include <BmnGlobalTrack.h>
#include <BmnHit.h>
#include <BmnKalmanFilter.h>
#include <UniDbRun.h>
#include <CbmVertex.h>
#include <BmnVertex.h>
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

// A class to be used as a store for calculated residuals ...

class Residuals : public EffStore {
public:

    Residuals() : detector(""),
    station(-1), module(-1) {
        ;
    }

    Residuals(TString det, Int_t stat, Int_t mod) : EffStore(det, stat, mod), 
    detector(det), station(stat), module(mod) {
        ;
    }

    virtual ~Residuals() {
        ;
    }

    void SetXY(Double_t xR, Double_t yR) {
        x.push_back(xR);
        y.push_back(yR);
    }
    
    vector <Double_t> GetX() {
        return x;
    }
    
    vector <Double_t> GetY() {
        return y;
    }

private:
    TString detector; // GEM or SILICON
    Int_t station;
    Int_t module;

    vector <Double_t> x;
    vector <Double_t> y;

    ClassDef(Residuals, 1)
};

class BmnEfficiency : public TNamed {
public:

    BmnEfficiency() {
        dstChain = nullptr;
        fNEvents = 0;
        fKalman = nullptr;

        fHeader = nullptr;

        fInnerHits = nullptr;
        fGemHits = nullptr;
        fSiliconHits = nullptr;
        fGlobTracks = nullptr;
        fGemTracks = nullptr;
        fSiliconTracks = nullptr;
        fVertices = nullptr;

        fField = nullptr;
        fMagField = nullptr;
        fKalman = nullptr;

        effSilicon = nullptr;
        effGem = nullptr;

        BmnInnerTrackerGeometryDraw* fInnTracker = new BmnInnerTrackerGeometryDraw();

        gem = fInnTracker->GetGemGeometry();
        silicon = fInnTracker->GetSiliconGeometry();

        fNHits = 5;
    }

    BmnEfficiency(FairRunAna*, BmnInnerTrackerGeometryDraw*, TString, Int_t nEvents = 0);
    BmnEfficiency(FairRunAna*, TString, Int_t nEvents = 0);

    void SetMinNHitsPerGlobTrack(Int_t nHits) {
        fNHits = nHits;
    }

    void SetMinNHitsPerSiliconTrack(Int_t nHits) {
        fNHitsSilicon = nHits;
    }

    void SetMinNHitsPerGemTrack(Int_t nHits) {
        fNHitsGem = nHits;
    }

    void Efficiency(UInt_t&, TH1F*);

    virtual ~BmnEfficiency() {
        if (fMagField)
            delete fMagField;

        if (dstChain)
            delete dstChain;

        if (fKalman)
            delete fKalman;

        if (effSilicon)
            delete effSilicon;

        if (effGem)
            delete effGem;
    }

    TClonesArray* GetSiliconEfficiency() {
        return effSilicon;
    }

    TClonesArray* GetGemEfficiency() {
        return effGem;
    }

    TString GetGemZone(BmnHit*);

    // Overloading =  ...

    BmnEfficiency& operator=(const BmnEfficiency& eff) {
        // Check assignement to itself
        if (this == &eff)
            return *this;

        dstChain = eff.dstChain;
        fVertices = eff.fVertices;
        fGlobTracks = eff.fGlobTracks;

        fSiliconTracks = eff.fSiliconTracks;
        fSiliconHits = eff.fSiliconHits;

        fGemTracks = eff.fGemTracks;
        fGemHits = eff.fGemHits;

        fMagField = eff.fMagField;
        fField = eff.fField;
        fKalman = eff.fKalman;

        return *this;
    }

private:
    Bool_t isGoodDst;

    TClonesArray* effGem;
    TClonesArray* effSilicon;

    map <Int_t, Double_t> fStatZ;
    map <Int_t, vector <Double_t>> fStatAcceptance;


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

    Int_t GetSiliconStatModule(BmnHit*);
    Int_t GetGemStatModule(BmnHit*);

    inline Double_t FindZ(Int_t stat) {
        return fStatZ.find(stat)->second;
    }

    BmnHit* virtualHitIfInAcceptance(Int_t, vector <BmnHit*>, pair <FairTrackParam, FairTrackParam>);

protected:
    // Geometries
    BmnGemStripStationSet* gem;
    BmnSiliconStationSet* silicon;

    DstEventHeader* fHeader;

    TClonesArray* fInnerHits;
    TClonesArray* fGemHits;
    TClonesArray* fSiliconHits;
    TClonesArray* fGlobTracks;
    TClonesArray* fGemTracks;
    TClonesArray* fSiliconTracks;
    TClonesArray* fVertices;

    TChain* dstChain;
    Int_t fNEvents;

    FairField* fField;
    BmnNewFieldMap* fMagField;
    BmnKalmanFilter* fKalman;

    Int_t fNHits;

    ClassDef(BmnEfficiency, 1)
};

#endif