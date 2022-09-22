
#ifndef BMNRESCALE_H
#define BMNRESCALE_H

#include <vector>
#include <regex>
#include <math.h>

#include <TClass.h>
#include <TClonesArray.h>
#include <TBranch.h>
#include <TLeaf.h>
#include <TChain.h>
#include <TTree.h>
#include <TGeoTrack.h>
#include <TFile.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TStopwatch.h>

#include <FairMCEventHeader.h>
#include <DigiRunHeader.h>

//#include <CbmMCTrack.h>
//#include <CbmStsPoint.h>
//#include <BmnSiliconPoint.h>
//#include <BmnCSCPoint.h>
#include <BmnEnums.h>
#include <BmnMath.h>
#include <BmnEventHeader.h>
#include <DstEventHeader.h>
#include <BmnSiliconDigit.h>
#include <BmnGemStripDigit.h>
#include <BmnCSCDigit.h>
#include <BmnSiliconHit.h>
#include <BmnGemStripHit.h>
#include <BmnCSCHit.h>
#include <BmnGlobalTrack.h>

#include "BmnGemStripStationSet.h"
#include "BmnSiliconStationSet.h"
#include "BmnCSCStationSet.h"
#include "BmnGemStripConfiguration.h"
#include "BmnGemStripStationSet_RunSpring2017.h"
#include "BmnGemStripStationSet_RunSummer2016.h"
#include "BmnGemStripStationSet_RunWinter2016.h"

#define TRACK_HITS 1

using namespace std;
using namespace TMath;

class BmnSigInfo {
public:

    BmnSigInfo() {
        minVal = DBL_MAX;
        maxVal = DBL_MIN;
        hSig = NULL;
        hIntSig = NULL;
        func = NULL;
    }

    ~BmnSigInfo() {
        if (hSig) delete hSig;
        if (hIntSig) delete hIntSig;
        if (func) delete func;
        hSig = NULL;
        hIntSig = NULL;
        func = NULL;
    }
    Double_t minVal;
    Double_t maxVal;
    TH1D *hSig;
    TH1D *hIntSig;
    TF1* func;
private:
};

/**
 * BmnRescale - class implementing ADC signal rescaling
 *  specific for each strip plane
 * 
 * \author Ilnur Gabdrakhmanov <ilnur@jinr.ru>
 */
class BmnRescale {
public:

    /**
     * 
     * @param periodId
     * @param runId
     * @param lowThr lower cluster signal threshold
     * @param ClusterSizeThrlower cluster size threshold (in strip count)
     * @param nBins Histograms' binning
     */
    BmnRescale(
            UInt_t periodId, UInt_t runId,
            Double_t lowThr = 0,
            Double_t ClusterSizeThr = 0,
            Int_t nBins = 1e6);
    virtual ~BmnRescale();

    BmnStatus CreateRescales(TString fileNameMC, TString fileNameEx);
    TF1* GetRescaleFunc(TString name, TF1 *mc, TF1 *ex);

    vector<vector<vector<TF1* > > > GetGemRescaleVector() {
        return fStripGemRescale;
    }

    vector<vector<vector<TF1* > > > GetSilRescaleVector() {
        return fStripSilRescale;
    }

    vector<vector<vector<TF1* > > > GetCSCRescaleVector() {
        return fStripCSCRescale;
    }

    vector<vector<vector<BmnSigInfo* > > > GetGemInfoVectorExp() {
        return fStripGemInfoEx;
    }

    vector<vector<vector<BmnSigInfo* > > > GetSilInfoVectorExp() {
        return fStripSilInfoEx;
    }

    vector<vector<vector<BmnSigInfo* > > > GetCSCInfoVectorExp() {
        return fStripCSCInfoEx;
    }

    TCanvas * GetCanvasGem();
    TCanvas * GetCanvasSil();
    TCanvas * GetCanvasCSC();

private:
    static const UInt_t Pad_Width = 640;
    static const UInt_t Pad_Height = 360;
    BmnSetup fSetup;
    UInt_t fPeriodId;
    UInt_t fRunId;
    Double_t fLowThr;
    Double_t fClusterSizeThr;
    Int_t fNBins;

    BmnGemStripStationSet *fGemStationSet;
    BmnSiliconStationSet *fSilStationSet;
    BmnCSCStationSet *fCscStationSet;

    vector<vector<vector<BmnSigInfo* > > > fStripGemInfoMC;
    vector<vector<vector<BmnSigInfo* > > > fStripSilInfoMC;
    vector<vector<vector<BmnSigInfo* > > > fStripCSCInfoMC;
    vector<vector<vector<BmnSigInfo* > > > fStripGemInfoEx;
    vector<vector<vector<BmnSigInfo* > > > fStripSilInfoEx;
    vector<vector<vector<BmnSigInfo* > > > fStripCSCInfoEx;
    vector<vector<vector<TF1* > > > fStripGemRescale;
    vector<vector<vector<TF1* > > > fStripSilRescale;
    vector<vector<vector<TF1* > > > fStripCSCRescale;

    template<class ClStationSet, class ClSt, class ClMod>
    TCanvas * GetCanvas(ClStationSet * ss) {
        Int_t sumMods = 0;
        Int_t maxLayers = 0;
        for (Int_t iStation = 0; iStation < ss->GetNStations(); iStation++) {
            ClSt * st = ss->GetStation(iStation);
            sumMods += st->GetNModules();
            for (Int_t iModule = 0; iModule < st->GetNModules(); iModule++) {
                ClMod *mod = st->GetModule(iModule);
                if (maxLayers < mod->GetNStripLayers())
                    maxLayers = mod->GetNStripLayers();
            }
        }
        TString name = Form("Strip_Digit_%d", rand());
        TString title = "Canvas";
        TCanvas *can = new TCanvas(name, title, Pad_Width * maxLayers, Pad_Height * sumMods);
        can->Divide(maxLayers, sumMods);
        return can;
    }

    BmnStatus LoadDigitDistributionsFromFile(
            TString name,
            vector<TString> digiNames,
            vector<vector<vector<vector<BmnSigInfo* > > > > digiVecs);
    BmnStatus LoadClusterDistributionsFromFile(
            TString name,
            vector<TString> clusterNames,
            vector<vector<vector<vector<BmnSigInfo* > > > > clusterVecs);
    BmnStatus LoadClustersOfTrackDistributionsFromFile(
            TString fileName,
            TString globalTrackNames,
            vector<TString> subTrackNames,
            vector<TString> hitNames,
            vector<vector<vector<vector<BmnSigInfo* > > > > clusterVecs);
    void CreateStripVectors();

    template<class cl>
    void FreeVector3D(vector<vector<vector<cl*> > > vvv) {
        for (auto &row : vvv)
            for (auto &col : row)
                for (auto &el : col)
                    delete el;
    }

    /**
     * Template function for generating 3D vector of BmnSigInfo objects
     * @param ss station set for the specific detector subsystem
     * @param vvv empty 3D vector to fill
     */
    template<class ClStationSet, class ClSt, class ClMod>
    void FillInfoVector(ClStationSet *ss, vector<vector<vector<BmnSigInfo* > > > &vvv) {
        for (Int_t iStation = 0; iStation < ss->GetNStations(); iStation++) {
            vector<vector<BmnSigInfo*> > rowGEM;
            ClSt * st = ss->GetStation(iStation);
            for (Int_t iModule = 0; iModule < st->GetNModules(); iModule++) {
                vector<BmnSigInfo*> colGEM;
                ClMod *mod = st->GetModule(iModule);
                for (Int_t iLayer = 0; iLayer < mod->GetNStripLayers(); iLayer++) {
                    BmnSigInfo *r = new BmnSigInfo();
                    colGEM.push_back(r);
                }
                rowGEM.push_back(colGEM);
            }
            vvv.push_back(rowGEM);
        }
    }

    template<class ClStationSet, class ClSt, class ClMod>
    void FillRescaleVector(
            ClStationSet *ss,
            vector<vector<vector<BmnSigInfo* > > > &vMC,
            vector<vector<vector<BmnSigInfo* > > > &vEx,
            vector<vector<vector<TF1* > > > &vRescale) {
        for (Int_t iStation = 0; iStation < ss->GetNStations(); iStation++) {
            vector<vector<TF1*> > rowGEM;
            ClSt * st = ss->GetStation(iStation);
            for (Int_t iModule = 0; iModule < st->GetNModules(); iModule++) {
                vector<TF1*> colGEM;
                ClMod *mod = st->GetModule(iModule);
                for (Int_t iLayer = 0; iLayer < mod->GetNStripLayers(); iLayer++) {
                    TF1 *r = nullptr;
                    if ((vMC[iStation][iModule][iLayer]->func) && (vEx[iStation][iModule][iLayer]->func))
                        r = GetRescaleFunc(
                            Form("rescale_%s",
                            vEx[iStation][iModule][iLayer]->func->GetName()),
                            vMC[iStation][iModule][iLayer]->func,
                            vEx[iStation][iModule][iLayer]->func);
                    colGEM.push_back(r);
                    printf("st %d mod %d l %d  func %08X  of MC %08X  Ex %08X  \n",
                            iStation, iModule, iLayer, (UInt_t)(size_t) r,
                            (UInt_t)(size_t) vMC[iStation][iModule][iLayer]->func,
                            (UInt_t)(size_t) vEx[iStation][iModule][iLayer]->func);
                }
                rowGEM.push_back(colGEM);
            }
            vRescale.push_back(rowGEM);
        }
    }
    void CreateDigitDistributions(
            TTree *treeDig, TClonesArray *digits,
            vector<vector<vector<BmnSigInfo* > > > &infoVec);

    /**
     * Template function aimed to create exp cluster signal distribution function (Hits from tracks variant)
     * @param treeDST an experimental DST tree
     * @param globTracks 
     * @param subTracks  
     * @param gemHits inner tracker subdetector hit branch
     * @param ss subdetector station set
     * @param infoVec 3-d vector to store function info
     */
    template <class ClHit, class ClStationSet, class ClSt, class ClMod, class ClLay>
    void CreateClusterDistributions(
            TTree *treeDST, TClonesArray* globTracks, TClonesArray* subTracks, TClonesArray *gemHits, ClStationSet *ss,
            vector<vector<vector<BmnSigInfo* > > > &infoVec) {
        printf("CreateClusterDistributions started for %s\n", gemHits->GetClass()->GetName());
        TStopwatch timer;
        timer.Start();

        Bool_t isGem = (typeid (ss) == typeid (BmnGemStripStationSet*));
        Bool_t isSil = (typeid (ss) == typeid (BmnSiliconStationSet*));
        Bool_t isCSC = (typeid (ss) == typeid (BmnCSCStationSet*));

        // Find bounds
        for (Long64_t iEv = 0; iEv < treeDST->GetEntries(); iEv++) {
            treeDST->GetEntry(iEv);

            for (Int_t iTrack = 0; iTrack < globTracks->GetEntriesFast(); iTrack++) {
                BmnGlobalTrack* track = (BmnGlobalTrack*) globTracks->UncheckedAt(iTrack);
                BmnTrack* subTrack = nullptr;
                if (isGem && (track->GetGemTrackIndex() != -1))
                    subTrack = static_cast<BmnTrack*> (subTracks->UncheckedAt(track->GetGemTrackIndex()));
                if (isSil && (track->GetSilTrackIndex() != -1))
                    subTrack = static_cast<BmnTrack*> (subTracks->UncheckedAt(track->GetSilTrackIndex()));
                if ((!subTrack) && (!isCSC))
                    continue;
                vector<Int_t> vrange; // range of hit indices
                vrange.resize(
                        isCSC ? 1 : subTrack->GetNHits(),
                        isCSC ? track->GetCscHitIndex(0) : 0);
                if (!isCSC)
                    for (Int_t i = 0; i < vrange.size(); i++)
                        vrange[i] = subTrack->GetHitIndex(i);
                else
                    if (track->GetCscHitIndex(0) == -1)
                    continue;
                for (Int_t iHit : vrange) {
                    ClHit *hit = static_cast<ClHit*> (gemHits->UncheckedAt(iHit));
                    Int_t iSt = hit->GetStation();
                    Int_t iMod = hit->GetModule();
                    //                if (iMod % 2 == 0)
                    //                    printf("ist %d imod %d  ( %f : %f )\n", iSt, iMod, -hit->GetX(), hit->GetY());
                    ClMod * mod = ss->GetStation(iSt)->GetModule(iMod);
                    //                printf("iev %d\n\t iHit %d iSt %d  iMod %d\n",
                    //                        i, iHit, iSt, iMod);
                    for (Int_t iLay = 0; iLay < mod->GetStripLayers().size(); iLay++) {
                        ClLay lay = mod->GetStripLayer(iLay);
                        Double_t x = hit->GetX();
                        Double_t y = hit->GetY();
                        //                    if (iMod % 2 == 0)
                        //                        printf("iLay %d  xmin %f xmax %f ymin %f ymax %f\n", iLay,
                        //                            lay.GetXMinLayer(), lay.GetXMaxLayer(), lay.GetYMinLayer(), lay.GetYMaxLayer());
                        if ((lay.IsPointInsideStripLayer(x * (-1), y))) {
                            //                        printf("\t\tinside %d layer\n", iLay);
                            Int_t iLayCorr = (isSil && (iSt > 0)) ? (iLay / 2) : iLay; // For divided silicon layers
                            BmnSigInfo* info = infoVec[iSt][iMod][iLayCorr];
                            Double_t val = (lay.GetType() == UpperStripLayer) ? hit->GetStripTotalSignalInUpperLayer() : hit->GetStripTotalSignalInLowerLayer();
                            Double_t cs = (lay.GetType() == UpperStripLayer) ? hit->GetClusterSizeInUpperLayer() : hit->GetClusterSizeInLowerLayer();
                            //                        printf("\tiLay %d \n\t\tval %f fLowThr %f cs %f fClusterSizeThr %f\n",
                            //                                iLay, val, fLowThr, cs, fClusterSizeThr);
                            if ((val > fLowThr) && (cs > fClusterSizeThr)) {
                                if (info->maxVal < val)
                                    info->maxVal = val;
                                if (info->minVal > val)
                                    info->minVal = val;
                            }
                        }
                    }
                }
            }
        }
        // Create histograms
        for (Int_t iStation = 0; iStation < infoVec.size(); iStation++) {
            printf("iStation %d\n", iStation);
            for (Int_t iModule = 0; iModule < infoVec[iStation].size(); iModule++) {
                printf("\tiModule %d\n", iModule);
                for (Int_t iLayer = 0; iLayer < infoVec[iStation][iModule].size(); iLayer++) {
                    printf("\t\tiLayer %d\n", iLayer);
                    BmnSigInfo* info = infoVec[iStation][iModule][iLayer];
                    Double_t minVal = info->minVal;
                    Double_t maxVal = info->maxVal;
                    printf("\t\t\tMin  = %f max = %f\n", minVal, maxVal);
                    if (minVal > maxVal)
                        continue;
                    TString name = Form("Sig_exp_%s_%d_%d_%d",
                            gemHits->GetName(), iStation, iModule, iLayer);
                    info->hSig = new TH1D(name, name, fNBins,
                            minVal - 0.5 * (maxVal - minVal) / (Double_t) fNBins,
                            maxVal + 0.5 * (maxVal - minVal) / (Double_t) fNBins);
                    name = Form("IntSignal_%s_%d_%d_%d",
                            gemHits->GetName(), iStation, iModule, iLayer);
                    info->hIntSig = new TH1D(name, name, fNBins,
                            minVal - 0.5 * (maxVal - minVal) / (Double_t) fNBins,
                            maxVal + 0.5 * (maxVal - minVal) / (Double_t) fNBins);
                }
            }
        }
        // Fill signal hists
        for (Long64_t iEv = 0; iEv < treeDST->GetEntries(); iEv++) {
            treeDST->GetEntry(iEv);
            for (Int_t iTrack = 0; iTrack < globTracks->GetEntriesFast(); iTrack++) {
                BmnGlobalTrack* track = (BmnGlobalTrack*) globTracks->UncheckedAt(iTrack);
                BmnTrack* subTrack = nullptr;
                if (isGem && (track->GetGemTrackIndex() != -1))
                    subTrack = static_cast<BmnTrack*> (subTracks->UncheckedAt(track->GetGemTrackIndex()));
                if (isSil && (track->GetSilTrackIndex() != -1))
                    subTrack = static_cast<BmnTrack*> (subTracks->UncheckedAt(track->GetSilTrackIndex()));
                if ((!subTrack) && (!isCSC))
                    continue;
                vector<Int_t> vrange; // range of hit indices
                vrange.resize(
                        isCSC ? 1 : subTrack->GetNHits(),
                        isCSC ? track->GetCscHitIndex(0) : 0);
                if (!isCSC)
                    for (Int_t i = 0; i < vrange.size(); i++)
                        vrange[i] = subTrack->GetHitIndex(i);
                else
                    if (track->GetCscHitIndex(0) == -1)
                    continue;
                for (Int_t iHit : vrange) {
                    ClHit *hit = static_cast<ClHit*> (gemHits->UncheckedAt(iHit));
                    //                    printf("iev %lld ihit %d  flag %d\n     ", i, iHit, hit->GetFlag());
                    if (hit->GetFlag() == kFALSE) ///<- prevent multiple count
                    {
                        //                    printf("iev %lld ihit %d  flag %d\n     ", i, iHit, hit->GetFlag());
                        continue;
                    }
                    Int_t iSt = hit->GetStation();
                    Int_t iMod = hit->GetModule();
                    ClMod * mod = ss->GetStation(iSt)->GetModule(iMod);
                    for (Int_t iLay = 0; iLay < mod->GetStripLayers().size(); iLay++) {
                        ClLay lay = mod->GetStripLayer(iLay);
                        Double_t x = hit->GetX();
                        Double_t y = hit->GetY();
                        if (lay.IsPointInsideStripLayer(x * (-1), y)) {
                            hit->SetFlag(kFALSE);
                            //                    ClHit *hit2 = static_cast<ClHit*> (gemHits->UncheckedAt(hitIndex));
                            //                    printf("iev %lld itrack %d ihit %d  hitIndex %d set flag %d\n     ", i, iTrack, iHit, hitIndex, hit2->GetFlag());
                            Int_t iLayCorr = (isSil && (iSt > 0)) ? (iLay / 2) : iLay; // For divided silicon layers
                            BmnSigInfo* info = infoVec[iSt][iMod][iLayCorr];
                            if (!(info->hSig))
                                continue;
                            Double_t val = (lay.GetType() == UpperStripLayer) ? hit->GetStripTotalSignalInUpperLayer() : hit->GetStripTotalSignalInLowerLayer();
                            Int_t cs = (lay.GetType() == UpperStripLayer) ? hit->GetClusterSizeInUpperLayer() : hit->GetClusterSizeInLowerLayer();
                            if ((val > fLowThr) && (cs > fClusterSizeThr)) {
                                info->hSig->Fill(val);
                            }
                        }
                    }
                }
            }
            // clear marks
            for (Int_t iHit = 0; iHit < gemHits->GetEntriesFast(); iHit++) {
                BmnHit *hit = static_cast<BmnHit*> (gemHits->UncheckedAt(iHit));
                hit->SetFlag(kTRUE);
            }
        }
        // Fill integral hists & create funcs
        for (Int_t iStation = 0; iStation < infoVec.size(); iStation++) {
            for (Int_t iModule = 0; iModule < infoVec[iStation].size(); iModule++) {
                for (Int_t iLayer = 0; iLayer < infoVec[iStation][iModule].size(); iLayer++) {
                    BmnSigInfo* info = infoVec[iStation][iModule][iLayer];
                    if (!(info->hSig))
                        continue;
                    Double_t bc = 0;
                    for (Int_t i = 0; i < fNBins; i++) {
                        bc += info->hSig->GetBinContent(i);
                        info->hIntSig->SetBinContent(i, bc);
                    }
                    if (bc == 0)
                        continue;
                    info->hIntSig->Scale(1 / bc);
                    info->hIntSig->SetLineColor(kRed);
                    info->func = new TF1(
                            Form("%s_%d_%d_%d", gemHits->GetName(), iStation, iModule, iLayer),
                            [info, bc](Double_t *x, Double_t * p) {
                                //            Double_t val = (bc - x[0])/bc;//log(x[0]);
                                if (x[0] >= info->maxVal) return 1.0;
                                Int_t iBin = info->hIntSig->GetXaxis()->FindBin(x[0]);
                                Int_t jBin = iBin + ((x[0] > info->hIntSig->GetBinCenter(iBin)) ? 1 : -1);
                                Double_t v = info->hIntSig->GetBinContent(iBin) +
                                        (info->hIntSig->GetBinContent(jBin) - info->hIntSig->GetBinContent(iBin))*
                                        2 * abs(x[0] - info->hIntSig->GetBinCenter(iBin)) /
                                        (info->hIntSig->GetBinWidth(iBin) + info->hIntSig->GetBinWidth(jBin));
                                return v;
                            }, info->minVal, info->maxVal, 0);
                    info->func->SetNpx(fNBins);
                    info->func->SetLineColor(kBlue);
                }
            }
        }
        timer.Stop();
        Double_t rtime = timer.RealTime();
        Double_t ctime = timer.CpuTime();
        printf("Real time %f s,\n CPU time %f s\n", rtime, ctime);
    }

    /**
     * Template function aimed to create exp cluster signal distribution function (all hits)
     * @param treeDST an experimental DST tree
     * @param gemHits inner tracker subdetector hit branch
     * @param ss subdetector station set
     * @param infoVec 3-d vector to store function info
     */
    template <class ClHit, class ClStationSet, class ClSt, class ClMod, class ClLay>
    void CreateClusterDistributions(
            TTree *treeDST, TClonesArray *gemHits, ClStationSet *ss,
            vector<vector<vector<BmnSigInfo* > > > &infoVec) {
        printf("CreateClusterDistributions started for %s\n", gemHits->GetClass()->GetName());

        Bool_t isGem = (typeid (ss) == typeid (BmnGemStripStationSet*));
        printf("isGem = %d \n", isGem);
        Bool_t isSil = (typeid (ss) == typeid (BmnSiliconStationSet*));
        printf("isSil = %d \n", isSil);

        // Find bounds
        for (Long64_t iEv = 0; iEv < treeDST->GetEntries(); iEv++) {
            treeDST->GetEntry(iEv);
            for (Int_t iHit = 0; iHit < gemHits->GetEntriesFast(); iHit++) {
                //            for (Int_t iHit = 0; iHit < gemHits->GetEntriesFast(); iHit++) {
                ClHit *hit = static_cast<ClHit*> (gemHits->UncheckedAt(iHit));
                Int_t iSt = hit->GetStation();
                Int_t iMod = hit->GetModule();
                //                if (iMod % 2 == 0)
                //                    printf("ist %d imod %d  ( %f : %f )\n", iSt, iMod, -hit->GetX(), hit->GetY());
                ClMod * mod = ss->GetStation(iSt)->GetModule(iMod);
                //                printf("iev %d\n\t iHit %d iSt %d  iMod %d\n",
                //                        i, iHit, iSt, iMod);
                for (Int_t iLay = 0; iLay < mod->GetStripLayers().size(); iLay++) {
                    ClLay lay = mod->GetStripLayer(iLay);
                    Double_t x = hit->GetX();
                    Double_t y = hit->GetY();
                    //                    if (iMod % 2 == 0)
                    //                        printf("iLay %d  xmin %f xmax %f ymin %f ymax %f\n", iLay,
                    //                            lay.GetXMinLayer(), lay.GetXMaxLayer(), lay.GetYMinLayer(), lay.GetYMaxLayer());
                    if ((lay.IsPointInsideStripLayer(x * (-1), y))) {
                        //                        printf("\t\tinside %d layer\n", iLay);
                        Int_t iLayCorr = (isSil && (iSt > 0)) ? (iLay / 2) : iLay; // For divided silicon layers
                        BmnSigInfo* info = infoVec[iSt][iMod][iLayCorr];
                        Double_t val = (lay.GetType() == UpperStripLayer) ? hit->GetStripTotalSignalInUpperLayer() : hit->GetStripTotalSignalInLowerLayer();
                        Double_t cs = (lay.GetType() == UpperStripLayer) ? hit->GetClusterSizeInUpperLayer() : hit->GetClusterSizeInLowerLayer();
                        //                        printf("\tiLay %d \n\t\tval %f fLowThr %f cs %f fClusterSizeThr %f\n",
                        //                                iLay, val, fLowThr, cs, fClusterSizeThr);
                        if ((val > fLowThr) && (cs > fClusterSizeThr)) {
                            if (info->maxVal < val)
                                info->maxVal = val;
                            if (info->minVal > val)
                                info->minVal = val;
                        }
                    }
                }
            }
        }
        // Create histograms
        for (Int_t iStation = 0; iStation < infoVec.size(); iStation++) {
            printf("iStation %d\n", iStation);
            for (Int_t iModule = 0; iModule < infoVec[iStation].size(); iModule++) {
                printf("\tiModule %d\n", iModule);
                for (Int_t iLayer = 0; iLayer < infoVec[iStation][iModule].size(); iLayer++) {
                    printf("\t\tiLayer %d\n", iLayer);
                    BmnSigInfo* info = infoVec[iStation][iModule][iLayer];
                    Double_t minVal = info->minVal;
                    Double_t maxVal = info->maxVal;
                    printf("\t\t\tMin  = %f max = %f\n", minVal, maxVal);
                    if (minVal > maxVal)
                        continue;
                    TString name = Form("Sig_exp_%s_%d_%d_%d",
                            gemHits->GetName(), iStation, iModule, iLayer);
                    info->hSig = new TH1D(name, name, fNBins,
                            minVal - 0.5 * (maxVal - minVal) / (Double_t) fNBins,
                            maxVal + 0.5 * (maxVal - minVal) / (Double_t) fNBins);
                    name = Form("IntSignal_%s_%d_%d_%d",
                            gemHits->GetName(), iStation, iModule, iLayer);
                    info->hIntSig = new TH1D(name, name, fNBins,
                            minVal - 0.5 * (maxVal - minVal) / (Double_t) fNBins,
                            maxVal + 0.5 * (maxVal - minVal) / (Double_t) fNBins);
                }
            }
        }
        // Fill signal hists
        for (Long64_t iEv = 0; iEv < treeDST->GetEntries(); iEv++) {
            treeDST->GetEntry(iEv);
            for (Int_t iHit = 0; iHit < gemHits->GetEntriesFast(); iHit++) {
                ClHit *hit = static_cast<ClHit*> (gemHits->UncheckedAt(iHit));
                Int_t iSt = hit->GetStation();
                Int_t iMod = hit->GetModule();
                ClMod * mod = ss->GetStation(iSt)->GetModule(iMod);
                for (Int_t iLay = 0; iLay < mod->GetStripLayers().size(); iLay++) {
                    ClLay lay = mod->GetStripLayer(iLay);
                    Double_t x = hit->GetX();
                    Double_t y = hit->GetY();
                    if (lay.IsPointInsideStripLayer(x * (-1), y)) {
                        Int_t iLayCorr = (isSil && (iSt > 0)) ? (iLay / 2) : iLay; // For divided silicon layers
                        BmnSigInfo* info = infoVec[iSt][iMod][iLayCorr];
                        if (!(info->hSig))
                            continue;
                        Double_t val = (lay.GetType() == UpperStripLayer) ? hit->GetStripTotalSignalInUpperLayer() : hit->GetStripTotalSignalInLowerLayer();
                        Int_t cs = (lay.GetType() == UpperStripLayer) ? hit->GetClusterSizeInUpperLayer() : hit->GetClusterSizeInLowerLayer();
                        if ((val > fLowThr) && (cs > fClusterSizeThr)) {
                            info->hSig->Fill(val);
                        }
                    }
                }
            }
        }
        // Fill integral hists & create funcs
        for (Int_t iStation = 0; iStation < infoVec.size(); iStation++) {
            for (Int_t iModule = 0; iModule < infoVec[iStation].size(); iModule++) {
                for (Int_t iLayer = 0; iLayer < infoVec[iStation][iModule].size(); iLayer++) {
                    BmnSigInfo* info = infoVec[iStation][iModule][iLayer];
                    if (!(info->hSig))
                        continue;
                    Double_t bc = 0;
                    for (Int_t i = 0; i < fNBins; i++) {
                        bc += info->hSig->GetBinContent(i);
                        info->hIntSig->SetBinContent(i, bc);
                    }
                    if (bc == 0)
                        continue;
                    info->hIntSig->Scale(1 / bc);
                    info->hIntSig->SetLineColor(kRed);
                    info->func = new TF1(
                            Form("%s_%d_%d_%d", gemHits->GetName(), iStation, iModule, iLayer),
                            [info, bc](Double_t *x, Double_t * p) {
                                //            Double_t val = (bc - x[0])/bc;//log(x[0]);
                                if (x[0] >= info->maxVal) return 1.0;
                                Int_t iBin = info->hIntSig->GetXaxis()->FindBin(x[0]);
                                Int_t jBin = iBin + ((x[0] > info->hIntSig->GetBinCenter(iBin)) ? 1 : -1);
                                Double_t v = info->hIntSig->GetBinContent(iBin) +
                                        (info->hIntSig->GetBinContent(jBin) - info->hIntSig->GetBinContent(iBin))*
                                        2 * abs(x[0] - info->hIntSig->GetBinCenter(iBin)) /
                                        (info->hIntSig->GetBinWidth(iBin) + info->hIntSig->GetBinWidth(jBin));
                                return v;
                            }, info->minVal, info->maxVal, 0);
                    info->func->SetNpx(fNBins);
                    info->func->SetLineColor(kBlue);
                }
            }
        }
    }
    //    ClassDef(BmnRescale, 1);
};

#endif /* BMNRESCALE_H */

