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

#include <vector>
#include <fstream>
#include <sstream>

#include <TSystem.h>
#include <TMath.h>
#include <TNamed.h>
#include <TClonesArray.h>
#include <TFitResult.h>
#include <TFitResultPtr.h>
#include <TGeoManager.h>
#include <TString.h>

#include "FairTask.h"
#include "FairRootManager.h"
#include "FairEventHeader.h"

#include <BmnGlobalTrack.h>
#include  "BmnGemStripHit.h"
#include  "BmnSiliconHit.h"
#include  "BmnMille.h"
#include  "BmnMilleContainer.h"
#include  "BmnGemAlignCorrections.h"
#include  "BmnSiliconAlignCorrections.h"

#include <BmnSiliconStationSet.h>
#include <BmnGemStripStationSet.h>

#include <UniDetectorParameter.h>
#include <UniRun.h>

#include <BmnFieldMap.h>
#include <BmnNewFieldMap.h>
#include <FairField.h>

using namespace std;
using namespace TMath;

class BmnGlobalAlignment : public FairTask {
public:

    BmnGlobalAlignment() {
    };
    BmnGlobalAlignment(Int_t, TString, Int_t, Int_t, TString misAlignFile = "misAlignment.root", Bool_t doTest = kFALSE);
    virtual ~BmnGlobalAlignment();

    virtual InitStatus Init();

    virtual void Exec(Option_t* opt);

    virtual void Finish();

    void SetDoTest(Bool_t flag) {
        fIsDoTest = flag;
    }

    void SetDetectors(Bool_t gem, Bool_t si) {
        fDetectorSet[0] = gem;
        fDetectorSet[1] = si;
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

    void SetGemFixedRun6(Bool_t st0_0, Bool_t st1_0, Bool_t st2_0,
            Bool_t st3_0, Bool_t st3_1, Bool_t st4_0,
            Bool_t st4_1, Bool_t st5_0, Bool_t st5_1) {

        cout << "GEM" << endl;
        const Int_t nModulMax = 2; // To be fixed         
        Bool_t** gems = new Bool_t*[fDetectorGEM->GetNStations()];
        for (Int_t iStat = 0; iStat < fDetectorGEM->GetNStations(); iStat++)
            gems[iStat] = new Bool_t[nModulMax];
        gems[0][0] = st0_0;
        gems[1][0] = st1_0;
        gems[2][0] = st2_0;
        gems[3][0] = st3_0;
        gems[3][1] = st3_1;
        gems[4][0] = st4_0;
        gems[4][1] = st4_1;
        gems[5][0] = st5_0;
        gems[5][1] = st5_1;

        for (Int_t iStat = 0; iStat < fDetectorGEM->GetNStations(); iStat++)
            for (Int_t iMod = 0; iMod < fDetectorGEM->GetGemStation(iStat)->GetNModules(); iMod++)
                if (gems[iStat][iMod])
                    fixedGemElements[iStat][iMod] = kTRUE;

        if (fDebug)
            for (Int_t iStat = 0; iStat < fDetectorGEM->GetNStations(); iStat++)
                for (Int_t iMod = 0; iMod < fDetectorGEM->GetGemStation(iStat)->GetNModules(); iMod++)
                    cout << "Stat = " << iStat << " Mod = " << iMod << " isFixed (true / false) " << fixedGemElements[iStat][iMod] << endl;
    }

    void SetSiFixedRun6(Bool_t st0_0, Bool_t st0_1, Bool_t st0_2, Bool_t st0_3,
            Bool_t st0_4, Bool_t st0_5, Bool_t st0_6, Bool_t st0_7) {
        cout << "SI" << endl;
        const Int_t nModulMax = 8; // FIXME         
        Bool_t** si = new Bool_t*[fDetectorSI->GetNStations()];
        for (Int_t iStat = 0; iStat < fDetectorSI->GetNStations(); iStat++)
            si[iStat] = new Bool_t[nModulMax];
        si[0][0] = st0_0;
        si[0][1] = st0_1;
        si[0][2] = st0_2;
        si[0][3] = st0_3;
        si[0][4] = st0_4;
        si[0][5] = st0_5;
        si[0][6] = st0_6;
        si[0][7] = st0_7;

        for (Int_t iStat = 0; iStat < fDetectorSI->GetNStations(); iStat++)
            for (Int_t iMod = 0; iMod < fDetectorSI->GetSiliconStation(iStat)->GetNModules(); iMod++)
                if (si[iStat][iMod])
                    fixedSiElements[iStat][iMod] = kTRUE;

        if (fDebug)
            for (Int_t iStat = 0; iStat < fDetectorSI->GetNStations(); iStat++)
                for (Int_t iMod = 0; iMod < fDetectorSI->GetSiliconStation(iStat)->GetNModules(); iMod++)
                    cout << "Stat = " << iStat << " Mod = " << iMod << " isFixed (true / false) " << fixedSiElements[iStat][iMod] << endl;
    }

    void SetSiFixedRun7(Bool_t st0_0, Bool_t st0_1, Bool_t st0_2, Bool_t st0_3,
            Bool_t st1_0, Bool_t st1_1,
            Bool_t st2_0, Bool_t st2_1, Bool_t st2_2, Bool_t st2_3, Bool_t st2_4, Bool_t st2_5, Bool_t st2_6, Bool_t st2_7) {
        cout << "SI" << endl;
        const Int_t nModulMax = 8; // FIXME         
        Bool_t** si = new Bool_t*[fDetectorSI->GetNStations()];
        for (Int_t iStat = 0; iStat < fDetectorSI->GetNStations(); iStat++)
            si[iStat] = new Bool_t[nModulMax];

        // Stat #0
        si[0][0] = st0_0;
        si[0][1] = st0_1;
        si[0][2] = st0_2;
        si[0][3] = st0_3;

        // Stat #1
        si[1][0] = st1_0;
        si[1][1] = st1_1;

        // Stat #2
        si[2][0] = st2_0;
        si[2][1] = st2_1;
        si[2][2] = st2_2;
        si[2][3] = st2_3;
        si[2][4] = st2_4;
        si[2][5] = st2_5;
        si[2][6] = st2_6;
        si[2][7] = st2_7;

        for (Int_t iStat = 0; iStat < fDetectorSI->GetNStations(); iStat++)
            for (Int_t iMod = 0; iMod < fDetectorSI->GetSiliconStation(iStat)->GetNModules(); iMod++)
                if (si[iStat][iMod])
                    fixedSiElements[iStat][iMod] = kTRUE;
    }

    void SetSiConfigSRC(Bool_t st0_0, Bool_t st0_1, Bool_t st0_2, Bool_t st0_3,
            Bool_t st1_0, Bool_t st1_1,
            Bool_t st2_0, Bool_t st2_1, Bool_t st2_2, Bool_t st2_3, Bool_t st2_4, Bool_t st2_5, Bool_t st2_6, Bool_t st2_7) {
        SetSiFixedRun7(st0_0, st0_1, st0_2, st0_3, st1_0, st1_1, st2_0, st2_1, st2_2, st2_3, st2_4, st2_5, st2_6, st2_7);
    }

    void SetGemConfigSRC(Bool_t st0_0, Bool_t st1_0, Bool_t st2_0, Bool_t st3_0,
            Bool_t st4_0, Bool_t st4_1,
            Bool_t st5_0, Bool_t st5_1,
            Bool_t st6_0, Bool_t st6_1,
            Bool_t st7_0, Bool_t st7_1,
            Bool_t st8_0, Bool_t st8_1,
            Bool_t st9_0, Bool_t st9_1) {

        cout << "GEM" << endl;
        const Int_t nModulMax = 2; // FIXME         
        Bool_t** gem = new Bool_t*[fDetectorGEM->GetNStations()];
        for (Int_t iStat = 0; iStat < fDetectorGEM->GetNStations(); iStat++)
            gem[iStat] = new Bool_t[nModulMax];

        // Stat #0
        gem[0][0] = st0_0;

        // Stat #1
        gem[1][0] = st1_0;

        // Stat #2
        gem[2][0] = st2_0;

        // Stat #3
        gem[3][0] = st3_0;

        // Stat #4
        gem[4][0] = st4_0;
        gem[4][1] = st4_1;

        // Stat #5
        gem[5][0] = st5_0;
        gem[5][1] = st5_1;

        // Stat #6
        gem[6][0] = st6_0;
        gem[6][1] = st6_1;

        // Stat #7
        gem[7][0] = st7_0;
        gem[7][1] = st7_1;

        // Stat #8
        gem[8][0] = st8_0;
        gem[8][1] = st8_1;

        // Stat #9
        gem[9][0] = st9_0;
        gem[9][1] = st9_1;

        for (Int_t iStat = 0; iStat < fDetectorGEM->GetNStations(); iStat++)
            for (Int_t iMod = 0; iMod < fDetectorGEM->GetGemStation(iStat)->GetNModules(); iMod++)
                if (gem[iStat][iMod])
                    fixedGemElements[iStat][iMod] = kTRUE;
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

    void SetUseConstraints(Bool_t flag) {
        fUseConstraints = flag;
    }

private:
    void CreateDetectorGeometries();
    const Int_t MakeBinFile();
    void MakeSteerFile();
    void MilleNoFieldRuns(BmnGlobalTrack*, Int_t, Int_t, vector <BmnMilleContainer*>&, vector <BmnMilleContainer*>&);
    void Pede();
    void ReadPedeOutput(ifstream&);
    void ExtractCorrValues(ifstream&, Double_t*);
    BmnMilleContainer* FillMilleContainer(BmnGlobalTrack*, BmnHit*);

    inline Int_t GemStatModLabel(Int_t st, Int_t mod) {
        // return 3 * (2 * st + mod + 1);    // FIXME!!!
        return gemStatModLabel.find(pair <Int_t, Int_t> (st, mod))->second;
    }

    inline Int_t SiliconStatModLabel(Int_t st, Int_t mod) {
        // Int_t coeff[3] = {0, 6, 3}; FIXME!!
        // return GemStatModLabel(st, mod) + coeff[st] * st;
        return silStatModLabel.find(pair <Int_t, Int_t> (st, mod))->second;
    }

    inline vector <Int_t> GetSiliconStatMod(Int_t label) {
        while (label % 3 != 0)
            label++;

        vector <Int_t> out;
        for (auto it : silStatModLabel) {
            if (it.second != label)
                continue;
            out.push_back(it.first.first);
            out.push_back(it.first.second);
            break;
        }
        return out;
    }

    inline vector <Int_t> GetGemStatMod(Int_t label) {
        while (label % 3 != 0)
            label++;

        vector <Int_t> out;
        for (auto it : gemStatModLabel) {
            if (it.second != label)
                continue;
            out.push_back(it.first.first);
            out.push_back(it.first.second);
            break;
        }
        return out;
    }

    void FillMaps() {
        // Choose appropriate configuration (BM@N or SRC)
        Bool_t isSRC = (fRunId < 3589) ? kTRUE : kFALSE; 
        
        Int_t srcGEM[10][2] = {
            {3, -1},
            {6, -1},
            {9, -1},
            {12, -1},
            {15, 18},
            {21, 24},
            {27, 30},
            {33, 36},
            {39, 42},
            {45, 48}
        };

        Int_t bmnGEM[6][2] = {
            {3, 6},
            {9, 12},
            {15, 18},
            {21, 24},
            {27, 30},
            {33, 36}
        };

        const Int_t nGemStats = isSRC ? 10 : 6;       
        const Int_t nGemMods = 2;

        Int_t** gem = new Int_t*[nGemStats];
        for (Int_t iStat = 0; iStat < nGemStats; iStat++) {
            gem[iStat] = new Int_t[nGemMods];
            for (Int_t iMod = 0; iMod < nGemMods; iMod++)
                gem[iStat][iMod] = isSRC ? srcGEM[iStat][iMod] : bmnGEM[iStat][iMod];
        }
        
        for (Int_t iStat = 0; iStat < fDetectorGEM->GetNStations(); iStat++)
            for (Int_t iMod = 0; iMod < fDetectorGEM->GetGemStation(iStat)->GetNModules(); iMod++)
                gemStatModLabel[pair <Int_t, Int_t> (iStat, iMod)] = gem[iStat][iMod];

        const Int_t nSilStats = 3; // FIXME!!
        const Int_t nSilMods = 8;

        Int_t bmnSIL[nSilStats][nSilMods] = {
            {39, 42, 45, 48, -1, -1, -1, -1},
            {51, 54, -1, -1, -1, -1, -1, -1},
            {57, 60, 63, 66, 69, 72, 75, 78}
        };
        
        Int_t srcSIL[nSilStats][nSilMods] = {
            {51, 54, 57, 60, -1, -1, -1, -1},
            {63, 66, -1, -1, -1, -1, -1, -1},
            {69, 72, 75, 78, 81, 84, 87, 90}
        };
        
        Int_t** sil = new Int_t*[nSilStats];
        for (Int_t iStat = 0; iStat < nSilStats; iStat++) {
            sil[iStat] = new Int_t[nSilMods];
            for (Int_t iMod = 0; iMod < nSilMods; iMod++)
                sil[iStat][iMod] = isSRC ? srcSIL[iStat][iMod] : bmnSIL[iStat][iMod];
        }
        
        for (Int_t iStat = 0; iStat < fDetectorSI->GetNStations(); iStat++)
            for (Int_t iMod = 0; iMod < fDetectorSI->GetSiliconStation(iStat)->GetNModules(); iMod++)
                silStatModLabel[pair <Int_t, Int_t> (iStat, iMod)] = sil[iStat][iMod];

        //        if (!fDebug)
        //            return;

        cout << "GEM" << endl;
        cout << gemStatModLabel.size() << endl;
        for (auto it : gemStatModLabel)
            cout << it.first.first << ", " << it.first.second << " --> " << it.second << endl;

        cout << "SILICON" << endl;
        for (auto it : silStatModLabel)
            cout << it.first.first << ", " << it.first.second << " --> " << it.second << endl;
    }

    void _Mille(Double_t*, Double_t*, BmnMille*, Int_t);

    static Int_t fCurrentEvent;
    Int_t fNEvents;
    Int_t fNTracks;
    map <Int_t, pair <vector <BmnMilleContainer*>, vector < BmnMilleContainer*>>> fCONTAINER;
    map <Int_t, pair <vector <BmnMilleContainer*>, vector < BmnMilleContainer*>>>::iterator fITERATOR;
    map <pair <Int_t, Int_t>, Int_t> gemStatModLabel;
    map <pair <Int_t, Int_t>, Int_t> silStatModLabel;
    Bool_t fIsField;
    Int_t fRunPeriod;
    Int_t fRunId;

    BmnGemStripStationSet* fDetectorGEM; // GEM-geometry
    BmnSiliconStationSet* fDetectorSI; // SI-geometry

    TClonesArray* fGemAlignCorr;
    TClonesArray* fSiAlignCorr;

    TString fBranchGemAlignCorr;
    TString fBranchSiAlignCorr;

    Bool_t* fDetectorSet;

    TString fBranchSiHits;
    TString fBranchGemHits;

    TString fBranchGemTracks;
    TString fBranchSilTracks;

    TString fBranchGlobalTracks;
    TString fBranchGemResiduals;
    TString fBranchFairEventHeader;

    TClonesArray* fSiHits;
    TClonesArray* fGemHits;

    TClonesArray* fGemTracks;
    TClonesArray* fSilTracks;
    TClonesArray* fGlobalTracks;

    TString fRecoFileName;
    TChain* fChain;

    FairEventHeader* fFairEventHeader;

    Bool_t fUseRealHitErrors; // errors are taken from hit finder algorithm

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

    // Int_t nSelectedTracks;

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
    Bool_t** fixedSiElements;

    Bool_t fDebug;
    Int_t* Labels; //array containing a fixed param. number for each detector. 

    // Use constraints or not
    Bool_t fUseConstraints;

    Bool_t fIsDoTest;
    TString fMisAlignFile;
    TClonesArray* fBmnGemMisalign;

    ClassDef(BmnGlobalAlignment, 1)
};

#endif
