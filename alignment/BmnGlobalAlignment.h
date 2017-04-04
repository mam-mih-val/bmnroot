// @(#)bmnroot/alignment:$Id$
// Author: Pavel Batyuk <pavel.batyuk@jinr.ru> 2017-03-31

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// BmnGlobalAlignment                                                         //
//                                                                            //
// Alignment of tracking detectors.                                           //
//                                                                            //
// Uses Volker Blobel and Claus Kleinwort Millepede II                        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
#ifndef BMNGLOBALALIGNMENT_H
#define BMNGLOBALALIGNMENT_H 1

#include <fstream>
#include <sstream>
#include <TMath.h>
#include <TNamed.h>
#include <TClonesArray.h>
#include <TString.h>
#include "FairTask.h"
#include "FairRootManager.h"

#include  "BmnGlobalTrack.h"
#include  "BmnMwpcTrack.h"
#include  "BmnMwpcGeometry.h"
#include  "BmnGemTrack.h"
#include  "BmnMwpcHit.h"
#include  "BmnDchHit.h"
#include  "BmnGemStripHit.h"
#include  "BmnDchTrack.h"
#include  "BmnMille.h"
#include  "BmnMwpcAlignCorrections.h"
#include  "BmnGemAlignCorrections.h"
#include  "BmnDchAlignCorrections.h"

#include  "BmnGemStripStationSet.h"
#include  "BmnGemStripStationSet_RunWinter2016.h"
#include  "BmnGemStripStationSet_RunSpring2017.h"

using namespace std;
using namespace TMath;

class BmnGlobalAlignment : public FairTask {
public:

    BmnGlobalAlignment(BmnGemStripConfiguration::GEM_CONFIG);
    virtual ~BmnGlobalAlignment();

    virtual InitStatus Init();

    virtual void Exec(Option_t* opt);

    virtual void Finish() {
    };

    void SetDetectors(TString det1, TString det2) {
        fDetectorSet[0] = "YES";
        fDetectorSet[1] = det1;
        fDetectorSet[2] = det2;
    }

    void SetUseRealHitErrors(Bool_t flag) {
        fUseRealHitErrors = flag;
    }

    void SetPreSigma(Double_t presigma) {
        fPreSigma = presigma;
    }

    void SetAccuracy(Double_t accuracy) {
        fAccuracy = accuracy;
    }

    void SetUseRegularization(Bool_t flag) {
        fUseRegularization = flag;
    }

    void SetOutlierDownweighting(Int_t n) {
        fOutlierdownweighting = n;
    }

    void SetDwfractionCut(Double_t fraction) {
        fDwfractioncut = fraction;
    }

    void SetChi2MaxPerNDF(Double_t val) {
        fChi2MaxPerNDF = val;
    }

    void SetMinHitsAccepted(Int_t val) {
        fMinHitsAccepted = val;
    }

    void SetTxMinMax(Double_t min, Double_t max) {
        fTxMin = min;
        fTxMax = max;
    }

    void SetTyMinMax(Double_t min, Double_t max) {
        fTyMin = min;
        fTyMax = max;
    }

    void SetDebug(Bool_t flag) {
        fDebug = flag;
    }

    void SetHugecut(Double_t val) {
        fHugecut = val;
    }

    void SetChisqcut(Double_t val1, Double_t val2) {
        fChisqcut[0] = val1;
        fChisqcut[1] = val2;
    }

    void SetEntriesPerParam(Int_t entries) {
        fEntries = entries;
    }

    void SetGemFixedRun6(TString st0_0, TString st1_0, TString st2_0,
            TString st3_0, TString st3_1, TString st4_0,
            TString st4_1, TString st5_0, TString st5_1) {

        const Int_t nModulMax = 2; // To be fixed         
        TString gems[fDetector->GetNStations()][nModulMax];
        gems[0][0] = st0_0;
        gems[1][0] = st1_0;
        gems[2][0] = st2_0;
        gems[3][0] = st3_0;
        gems[3][1] = st3_1;
        gems[4][0] = st4_0;
        gems[4][1] = st4_1;
        gems[5][0] = st5_0;
        gems[5][1] = st5_1;

        for (Int_t iStat = 0; iStat < fDetector->GetNStations(); iStat++)
            for (Int_t iMod = 0; iMod < fDetector->GetGemStation(iStat)->GetNModules(); iMod++)
                if (gems[iStat][iMod] != "")
                    fixedGemElements[iStat][iMod] = kTRUE;

        if (fDebug)
            for (Int_t iStat = 0; iStat < fDetector->GetNStations(); iStat++)
                for (Int_t iMod = 0; iMod < fDetector->GetGemStation(iStat)->GetNModules(); iMod++)
                    cout << "Stat = " << iStat << " Mod = " << iMod << " isFixed (true / false) " << fixedGemElements[iStat][iMod] << endl;
    }

    void SetUseTrackWithMinChi2(Bool_t flag) {
        fUseTrackWithMinChi2 = flag;
    }

    void SetExclusionRangeTx(Double_t min, Double_t max) {
        fIsExcludedTx = kTRUE;
        fTxLeft = min;
        fTxRight = max;
    }

    void SetExclusionRangeTy(Double_t min, Double_t max) {
        fIsExcludedTy = kTRUE;
        fTyLeft = min;
        fTyRight = max;
    }

private:
    void PrintToFullFormat(TString, Char_t*);
    const Int_t MakeBinFile();
    void MakeSteerFile();
    void Mille(Int_t, Int_t, Char_t*);
    void Pede();
    void ReadPedeOutput(ifstream&);
    void ExtractCorrValues(ifstream&, Double_t*);

    static Int_t fCurrentEvent;
    static Int_t trackCounter;

    BmnMwpcGeometry* mwpcGeo;
    BmnGemStripStationSet* fDetector; // Detector geometry
    BmnGemStripConfiguration::GEM_CONFIG fGeometry;
    TClonesArray* fMwpcAlignCorr;
    TClonesArray* fGemAlignCorr;
    TClonesArray* fTofAlignCorr;
    TClonesArray* fDchAlignCorr;

    TString fBranchMwpcAlignCorr;
    TString fBranchGemAlignCorr;
    TString fBranchTofAlignCorr;
    TString fBranchDchAlignCorr;

    TString* fDetectorSet;

    TString fBranchMwpcHits;
    TString fBranchGemHits;
    TString fBranchTof1Hits;
    TString fBranchDchHits;

    TString fBranchMwpcTracks;
    TString fBranchGemTracks;
    TString fBranchDchTracks;

    TString fBranchGlobalTracks;

    TClonesArray* fMwpcHits;
    TClonesArray* fGemHits;
    TClonesArray* fTof1Hits;
    TClonesArray* fDchHits;

    TClonesArray* fMwpcTracks;
    TClonesArray* fGemTracks;
    TClonesArray* fDchTracks;

    TClonesArray* fGlobalTracks;

    FILE* fin_txt;
    TString fRecoFileName;
    TChain* fChain;

    Bool_t fUseRealHitErrors; // errors are taken from hit finder algorithm
    Bool_t fUseTrackWithMinChi2; // in case of target select track with minimal value of chi2

    // Restrictions on track params
    Double_t fChi2MaxPerNDF;
    Double_t fMinHitsAccepted;
    Double_t fTxMin;
    Double_t fTxMax;
    Double_t fTyMin;
    Double_t fTyMax;
    // Range to be exluded in case of "beam-target" alignment...
    Bool_t fIsExcludedTx; // do exclusion (true) or not, manipulated by SetExclusionRangeTx
    Bool_t fIsExcludedTy; // do exclusion (true) or not, manipulated by SetExclusionRangeTy
    Double_t fTxLeft;
    Double_t fTxRight;
    Double_t fTyLeft;
    Double_t fTyRight;

    Int_t nSelectedTracks;

    UInt_t fNumOfIterations;
    Double_t fAccuracy;
    Double_t fPreSigma;

    // Millepede params
    Bool_t fUseRegularization; // use regularization or not
    Double_t fHugecut; // cut factor in iteration 0
    Double_t fChisqcut[2]; // cut factor in iterations 1 and 2
    Int_t fEntries; // lower limit on number of entries/parameter
    Int_t fOutlierdownweighting; // number of internal iterations (> 1)
    Double_t fDwfractioncut; // reject all cases with a down-weight fraction >= val

    Int_t fNGL;
    Int_t fNLC;
    Int_t nDetectors;

    vector <Int_t> fFixedStats;
    Bool_t** fixedGemElements;

    Bool_t fDebug;
    Int_t* Labels; //array containing a fixed param. number for each detector. 
    // GEMs: 1 - 27; MWPC: 28 - 30; DCH: 31 - 33

    ClassDef(BmnGlobalAlignment, 1)
};

#endif