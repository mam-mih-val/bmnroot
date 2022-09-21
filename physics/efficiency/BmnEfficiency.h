#ifndef BMNEFF_H
#define BMNEFF_H 1

#include <TNamed.h>
#include <TChain.h>
#include <TObject.h>
#include <TClonesArray.h>
#include <TGeoManager.h>
#include <TH1.h>
#include <TH2.h>
#include <TEfficiency.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TLine.h>

#include <BmnInnerTrackerGeometryDraw.h>
#include <DstEventHeader.h>
#include <BmnGlobalTrack.h>
#include <BmnHit.h>
#include <BmnKalmanFilter.h>
#include <UniRun.h>
#include <CbmVertex.h>
#include <BmnVertex.h>
#include "BmnNewFieldMap.h"

#include <vector>

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

    EffStore(TString det, Int_t stat, TString z = "") :
    zone(z), detector(det),
    station(stat), module(-1),
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

class inputForEfficiency {
public:

    inputForEfficiency() : nGemCut(4), nSilCut(2), det("GEM") {
        eContainers = nullptr;
    }

    virtual ~inputForEfficiency() {
        ;
    }

    // Move assignment

    inputForEfficiency& operator=(inputForEfficiency&& input) {
        if (&input == this)
            return *this;

        det = input.det;
        nGemCut = input.nGemCut;
        nSilCut = input.nSilCut;

        sfIndices.first = input.sfIndices.first;
        sfIndices.second = input.sfIndices.second;

        flParams.first = input.flParams.first;
        flParams.second = input.flParams.second;

        gPairMap = input.gPairMap;
        sPairMap = input.sPairMap;

        eContainers = input.eContainers;

        for (auto it : input.usedStats)
            usedStats.push_back(it);

        for (auto it : input.hits)
            hits.push_back(it);

        input.hits.clear();
        input.usedStats.clear();

        if (input.eContainers)
            input.eContainers = nullptr;

        return *this;
    }

    // Detector specified, hit arrays and stats used ...
    TString det;
    vector <BmnHit*> hits;
    vector <Bool_t> usedStats;

    // Indices and track params ....
    pair <Int_t, Int_t> sfIndices;
    pair <FairTrackParam, FairTrackParam> flParams;

    TClonesArray* eContainers;

    // Y-ranges ...
    map <Int_t, vector <pair <Double_t, Double_t>>> gPairMap; // stat --> corresponding set of Y-pairs
    map <Int_t, vector <pair <Double_t, Double_t>>> sPairMap;

    // hit cuts ...
    Int_t nGemCut;
    Int_t nSilCut;
};

class InnerTrackerParams : public TObject {
public:

    InnerTrackerParams() {
        ;
    }

    InnerTrackerParams(Int_t stat, Double_t xMin, Double_t xMax, Double_t yMin, Double_t yMax) :
    fStation(stat), fModule(-1),
    fXMin(xMin), fXMax(xMax),
    fYMin(yMin), fYMax(yMax) {

    }

    InnerTrackerParams(Int_t stat, Int_t mod, Double_t xMin, Double_t xMax, Double_t yMin, Double_t yMax) :
    fStation(stat), fModule(mod),
    fXMin(xMin), fXMax(xMax),
    fYMin(yMin), fYMax(yMax) {

    }

    InnerTrackerParams(Int_t stat, Int_t mod, Double_t x, Double_t y, Double_t z) :
    fStation(stat), fModule(mod) {
        hitX = x;
        hitY = y;
        hitZ = z;
    }

    virtual ~InnerTrackerParams() {
        ;
    }

    Int_t station() {
        return fStation;
    }

    Int_t module() {
        return fModule;
    }

    pair <Double_t, Double_t> x() {
        return make_pair(fXMin, fXMax);
    }

    pair <Double_t, Double_t> y() {
        return make_pair(fYMin, fYMax);
    }

    void SetZ(Double_t z) {
        fZ = z;
    }

    Double_t z() {
        return fZ;
    }

    TVector3 xyz() {
        return TVector3(hitX, hitY, hitZ);
    }

protected:
    Int_t fStation;
    Int_t fModule;

    Double_t fXMin;
    Double_t fXMax;

    Double_t fYMin;
    Double_t fYMax;

    Double_t fZ; // position of station (at given module if defined) ...

    // 
    Double_t hitX;
    Double_t hitY;
    Double_t hitZ;

    ClassDef(InnerTrackerParams, 1)
};

class EffStore2D : public EffStore {
public:

    EffStore2D() {
        ;
    }

    EffStore2D(TString det, Int_t stat,
            map <Int_t, vector <pair <Double_t, Double_t>>> pairs) :
    EffStore(det, stat),
    hEffGem(nullptr), hEffGem1d(nullptr), hEffSil(nullptr), hEffSil1d(nullptr) {

        const Int_t nQp = 2; // positive and negative ...

        enum QpValue {
            pos, neg
        };

        hEffGem = new TEfficiency*[nQp];
        hEffSil = new TEfficiency*[nQp];
        hEffSilXP = new TEfficiency*[nQp];

        if (det.Contains("GEM")) {
            Int_t nBinsX = 160.; // 80.; // 160; 
            Int_t nBinsY = 45;

            pair <Double_t, Double_t> y = make_pair(0., 45.);

            Double_t xLow = -80.;
            Double_t xUp = +80.;

            for (Int_t iQp = 0; iQp < nQp; iQp++) {
                TString Qp = (iQp == QpValue::pos) ? "Q_{p} > 0" : "Q_{p} < 0";

                hEffGem[iQp] = new TEfficiency(Form("Detector# %s, Station# %d, %s", det.Data(), stat, Qp.Data()),
                        Form("Stat %d, %s; X [cm]; Y [cm]", stat, Qp.Data()), nBinsX, xLow, xUp, nBinsY, y.first, y.second);
            }

            // Preparing 1D-efficiencies (x for different y-ranges) ...
            if (pairs.size()) {
                nBinsX = 20.; // 80.; // 160;

                hEffGem1d = new TEfficiency**[nQp];

                const Int_t nYRanges = pairs.find(stat)->second.size();

                for (Int_t iQp = 0; iQp < nQp; iQp++) {
                    hEffGem1d[iQp] = new TEfficiency*[nYRanges];

                    TString Qp = (iQp == QpValue::pos) ? "Q_{p} > 0" : "Q_{p} < 0";

                    for (Int_t iRange = 0; iRange < nYRanges; iRange++) {
                        Double_t y0 = pairs.find(stat)->second.at(iRange).first;
                        Double_t y1 = pairs.find(stat)->second.at(iRange).second;

                        hEffGem1d[iQp][iRange] = new TEfficiency(Form("Detector# %s, Station# %d, (%G < y < %G [cm]), %s", det.Data(), stat, y0, y1, Qp.Data()),
                                Form("Stat %d, (%G < y < %G [cm]), %s; X [cm]; #varepsilon", stat, y0, y1, Qp.Data()), nBinsX, xLow, xUp);
                    }
                }
            }
        } else {
            Int_t nBinsX = 80;
            Int_t nBinsY = 45;

            TClonesArray* sil = new TClonesArray("InnerTrackerParams");

            // Adding necessary information ...
            // stat 0
            new ((*sil) [sil->GetEntriesFast()]) InnerTrackerParams(0, -6., +6., -5., +8.);

            // stat 1
            new ((*sil) [sil->GetEntriesFast()]) InnerTrackerParams(1, -6., +6., -5., +8.);

            // stat 2
            new ((*sil) [sil->GetEntriesFast()]) InnerTrackerParams(2, -12., +12., -11., +14.);

            for (Int_t iQp = 0; iQp < nQp; iQp++) {
                TString Qp = (iQp == QpValue::pos) ? "Q_{p} > 0" : "Q_{p} < 0";
                hEffSil[iQp] = new TEfficiency(Form("Detector# %s, Station# %d, %s", det.Data(), stat, Qp.Data()),
                        Form("Stat %d, %s; X [cm]; Y [cm]", stat, Qp.Data()),
                        nBinsX, getRanges(sil, stat)[0], getRanges(sil, stat)[1], nBinsY, getRanges(sil, stat)[2], getRanges(sil, stat)[3]);

                nBinsY = 55;

                hEffSilXP[iQp] = new TEfficiency(Form("Detector# %s, Station# %d (xP), %s", det.Data(), stat, Qp.Data()),
                        Form("Stat %d, %s; X [cm]; P [GeV/c]", stat, Qp.Data()),
                        nBinsX, getRanges(sil, stat)[0], getRanges(sil, stat)[1], nBinsY, 0., 5.5);
            }

            // Preparing 1D-efficiencies (x for different y-ranges) ...
            if (pairs.size()) {
                nBinsX = 20.; // 80.; // 160;

                hEffSil1d = new TEfficiency**[nQp];

                const Int_t nYRanges = pairs.find(stat)->second.size();

                for (Int_t iQp = 0; iQp < nQp; iQp++) {
                    hEffSil1d[iQp] = new TEfficiency*[nYRanges];

                    TString Qp = (iQp == QpValue::pos) ? "Q_{p} > 0" : "Q_{p} < 0";

                    for (Int_t iRange = 0; iRange < nYRanges; iRange++) {
                        Double_t y0 = pairs.find(stat)->second.at(iRange).first;
                        Double_t y1 = pairs.find(stat)->second.at(iRange).second;

                        hEffSil1d[iQp][iRange] = new TEfficiency(Form("Detector# %s, Station# %d, (%G < y < %G [cm]), %s", det.Data(), stat, y0, y1, Qp.Data()),
                                Form("Stat %d, (%G < y < %G [cm]), %s; X [cm]; #varepsilon", stat, y0, y1, Qp.Data()),
                                nBinsX, getRanges(sil, stat)[0], getRanges(sil, stat)[1]);
                    }
                }
            }

            delete sil;
        }
    }

    TEfficiency* efficiency(Int_t qpBin = 0, TString hType = "", TString det = "GEM") {
        // qpBin = 0 (positive), 1 (negative)
        if (hType.IsNull())
            return det.Contains("GEM") ? hEffGem[qpBin] : hEffSil[qpBin];
        else if (det.Contains("SILICON") && !hType.IsNull())
            return hEffSilXP[qpBin];
        else
            return nullptr;
    }

    TEfficiency* efficiency4range(Int_t range, Int_t qpBin = 0, TString det = "GEM") {
        // qpBin = 0 (positive), 1 (negative)

        if (det.Contains("GEM") && hEffGem1d[qpBin][range])
            return hEffGem1d[qpBin][range];
        else if (det.Contains("SIL") && hEffSil1d[qpBin][range])
            return hEffSil1d[qpBin][range];

        else
            return nullptr;
    }

    virtual ~EffStore2D() {
        // Should be implemented further ...
    }

private:
    TEfficiency** hEffGem;
    TEfficiency*** hEffGem1d;

    TEfficiency** hEffSil; // epslion(x, y)
    TEfficiency** hEffSilXP; // epsilon(x, P)
    TEfficiency*** hEffSil1d; // epsilon(x) at different y-slices 

private:

    vector <Double_t> getRanges(TClonesArray* sil, Int_t stat) {

        vector <Double_t> tmp;

        for (Int_t iEntry = 0; iEntry < sil->GetEntriesFast(); iEntry++) {
            InnerTrackerParams* silEle = (InnerTrackerParams*) sil->UncheckedAt(iEntry);

            if (silEle->station() == stat) {
                tmp.push_back(silEle->x().first);
                tmp.push_back(silEle->x().second);
                tmp.push_back(silEle->y().first);
                tmp.push_back(silEle->y().second);

                break;
            }
        }

        return tmp;
    }

    ClassDef(EffStore2D, 1)
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

    void Efficiency(TClonesArray*, TClonesArray* effSilicon,
            map <Int_t, vector <pair <Double_t, Double_t>>>, map <Int_t, vector <pair <Double_t, Double_t>>>);

    virtual ~BmnEfficiency() {
        if (fMagField)
            delete fMagField;

        if (dstChain)
            delete dstChain;

        if (fKalman)
            delete fKalman;
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
    void fillEfficiency(inputForEfficiency&& in);
    Bool_t isGoodDst;

    // Acceptance maps to be used ...
    map <pair <Int_t, Int_t>, Double_t> fStatZ; // (stat, mod) --> Z
    map <pair <Int_t, Int_t>, vector <Double_t>> fStatAcceptance; // (stat, mod) --> vector of boarders ...
    void FillAcceptanceMaps(TClonesArray*, TClonesArray* SILs = nullptr);

    Int_t fNHitsSilicon;
    Int_t fNHitsGem;

    inline TString GetDetector(BmnHit* hit) {
        Double_t z = hit->GetZ();

        TString det = "";
        if (z > gem->GetStation(0)->GetZPosition() - 5.)
            det = "GEM";
        else
            det = "SILICON";

        return det;
    }

    inline Double_t FindZ(Int_t stat) {
        return fStatZ.find(make_pair(stat, 0))->second;
    }

    Bool_t isVirtualHitInAcceptance(Int_t, vector <BmnHit*>, pair <FairTrackParam, FairTrackParam>, pair <Double_t, Double_t>&);

    Int_t FindBin(Double_t y, Int_t stat, map <Int_t, vector <pair <Double_t, Double_t>>> pairMap) {
        Int_t bin = -1;

        for (Int_t iRange = 0; iRange < pairMap.find(stat)->second.size(); iRange++) {
            Double_t min = pairMap.find(stat)->second.at(iRange).first;
            Double_t max = pairMap.find(stat)->second.at(iRange).second;

            if (y > min && y < max) {
                bin = iRange;
                break;
            }
        }

        return bin;
    }

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
