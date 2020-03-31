
#ifndef BMNRECOTOOLS_H
#define BMNRECOTOOLS_H

#include <vector>
#include <regex>
#include <dirent.h>

#include <TClonesArray.h>
#include <TBranch.h>
#include <TLeaf.h>
#include <TChain.h>
#include <TGeoTrack.h>
#include <TFile.h>
#include <TH2F.h>

#include <FairMCEventHeader.h>
#include <DigiRunHeader.h>

#include <CbmMCTrack.h>
#include <CbmStsPoint.h>
#include <BmnSiliconPoint.h>
#include <BmnCSCPoint.h>
#include <BmnMath.h>
#include <BmnEventHeader.h>
#include <DstEventHeader.h>
#include <BmnFieldPar.h>
#include <BmnMatch.h>
#include <BmnSiliconDigit.h>
#include <BmnGemStripDigit.h>
#include <BmnCSCDigit.h>
#include <BmnSiliconHit.h>
#include <BmnGemStripHit.h>
#include <BmnCSCHit.h>
#include "BmnSiliconStationSet.h"
#include "BmnGemStripStationSet.h"

using namespace std;
using namespace TMath;

class BmnRecoTools {
public:

    BmnRecoTools();
    virtual ~BmnRecoTools();

    BmnStatus Embed(
            TString inSourceName = "eve-lam-box.root", TString inBaseName = "evetest-p.root", TString destName = "merged-digi.root",
            Int_t code = 3122, vector<Int_t> outCodes = {2212, -211},
    Bool_t turnOffBaseDigits = kFALSE);
    
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

private:
    void RescaleFunc(TClonesArray* base, TClonesArray* src);
    //    void HighlightDecay(TClonesArray* src);
    BmnStatus GetNextValidSourceEvent();
    BmnStatus CloneSelected(TString BaseName, TString TempBaseName);

    TTree * fInTreeSource;
    TTree * fInTreeBase;
    TTree * fDestTree;

    UInt_t iSourceEvent = 0;

    //    const Int_t fMinHits = 4;
    Int_t fCode = 0;
    vector<Int_t> fOutCodes;

    vector<TClonesArray*> digiSourceArs; ///<- source digi arrays
    vector<TClonesArray*> digiBaseArs; ///<- base digi arrays
    vector<TClonesArray*> digiDestArs; ///<- destination digi arrays
    vector<TBranch*> digiBaseBrs; ///<- base digi branches
    vector<TBranch*> digiDestBrs; ///<- destination digi branches
    vector<TClonesArray*> matchSourceArs; ///<- source match arrays
    vector<TClonesArray*> matchBaseArs; ///<- source base arrays
    vector<TClonesArray*> matchDestArs; ///<- destination match arrays
    vector<TBranch*> matchDestBrs; ///<- destination match branches
    TClonesArray * mcTracks = nullptr;
    TClonesArray* stsPoints = nullptr;
    TClonesArray* silPoints = nullptr;
    TClonesArray* cscPoints = nullptr;
    DstEventHeader * copyBaseEH = nullptr;
    DstEventHeader * copyDestEH = nullptr;
    FairMCEventHeader * mcEH = nullptr;
    TBranch *EHBranch = nullptr;
    const TString EHMCName = "MCEventHeader.";
    const TString EHDigiName = "BmnEventHeader.";
    const TString RHDigiName = "DigiRunHeader";
    const TString FieldParName = "BmnFieldPar";



    ClassDef(BmnRecoTools, 1);
};

#endif /* BMNRECOTOOLS_H */

