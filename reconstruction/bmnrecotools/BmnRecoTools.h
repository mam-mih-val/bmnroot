
#ifndef BMNRECOTOOLS_H
#define BMNRECOTOOLS_H

#include <vector>
#include <regex>
#include <dirent.h>
#include <math.h>

#include <TClonesArray.h>
#include <TBranch.h>
#include <TLeaf.h>
#include <TChain.h>
#include <TGeoTrack.h>
#include <TFile.h>
#include <TH2F.h>
#include <TCanvas.h>
#include <TStyle.h>

#include <TCutG.h>

#include <FairMCEventHeader.h>
#include <DigiRunHeader.h>

#include <CbmMCTrack.h>
#include <CbmStsPoint.h>
#include <BmnSiliconPoint.h>
#include <BmnCSCPoint.h>
#include <BmnMath.h>
#include <BmnEventHeader.h>
#include <DstEventHeader.h>
#include <BmnSiliconDigit.h>
#include <BmnGemStripDigit.h>
#include <BmnCSCDigit.h>
#include <BmnSiliconHit.h>
#include <BmnGemStripHit.h>
#include <BmnCSCHit.h>
#include <BmnSiliconStationSet.h>
#include <BmnGemStripStationSet.h>
#include <BmnGlobalTrack.h>

using namespace std;
using namespace TMath;

/**
 * BmnRecoTools - static functions for embedding & reconstruction quality check
 * 
 * \author Ilnur Gabdrakhmanov <ilnur@jinr.ru>
 */
class BmnRecoTools {
public:

    BmnRecoTools();
    virtual ~BmnRecoTools();

    static Bool_t IsReconstructable(
            TClonesArray* tracks, TClonesArray* gemPoints, TClonesArray* silPoints, TClonesArray* cscPoints,
            Int_t code, vector<Int_t> &outCodes, Int_t minHits = 4);

    static vector<TString> GetFileVecFromDir(TString dir);

    static void ShowHitMakerEff(TClonesArray * hits, BmnSiliconStationSet set);

    static void FillSetStsPoints(
            TClonesArray* pts,
            BmnGemStripStationSet* set,
            //            vector<TH2* > &hitVec);
            vector<vector<vector<TH2* > > > &hitVec);

    static void FillSetStsHits(
            TClonesArray* pts, TClonesArray* hits,
            BmnGemStripStationSet* set,
            //            vector<TH2* > &hitVec,
            //        vector<TH1D* > &hrx,
            //        vector<TH1D* > &hry);
            vector<vector<vector<TH2* > > > &hitVec);
    static TCutG* Ellipse2CutG(
            TString name,
            Double_t x, Double_t y, Double_t rx, Double_t ry = -1.0,
            Double_t theta = 0.0);
    static TF1* GetSignalDistribution(
            TTree *tree, TClonesArray *ar,
            TTree *treeDST = nullptr,
            TClonesArray *gemHits = nullptr,
            TClonesArray *gemTracks = nullptr,
            TClonesArray *tracks = nullptr,
            Double_t lowThr = 0,
            Int_t ClusterSizeThr = 0,
            Int_t nBins = 100000);
    static TF1* GetRescaleFunc(TString name, TF1 *mc, TF1 *ex);
    

private:


    //    ClassDef(BmnRecoTools, 1);
};

#endif /* BMNRECOTOOLS_H */

