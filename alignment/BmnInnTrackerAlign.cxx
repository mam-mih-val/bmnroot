// @(#)bmnroot/alignment:$Id$
// Author: Pavel Batyuk <pavel.batyuk@jinr.ru> 2018-11-02

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  BmnInnTrackerAlign                                                        //
//                                                                            //
// Interface to get align. corrections for inn. tracker                       //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include "BmnInnTrackerAlign.h"
#include "UniDetectorParameter.h"
#include "TRandom.h"

BmnInnTrackerAlign::BmnInnTrackerAlign(Int_t period, Int_t run, TString fileName) {
    fBranchGemCorrs = "BmnGemAlignCorrections";
    fBranchSilCorrs = "BmnSiliconAlignCorrections";

    //  TString alCorrFileName = "";

    if (fileName.Contains("default")) {
        gRandom->SetSeed(0);
        fFilename = Form("alignment_innTracker_%d.root", UInt_t(gRandom->Integer(UINT32_MAX)));
        UniDetectorParameter::ReadFile("BM@N", "alignment", period, run, (Char_t*) fFilename.Data());
    } else
        fFilename = fileName;

    // Choose appropriate configuration (BM@N or SRC)
    Bool_t isSRC = (run < 3589) ? kTRUE : kFALSE;
    TString gPathConfig = gSystem->Getenv("VMCWORKDIR");
    TString confSi = "SiliconRun" + TString(isSRC ? "SRC" : "") + "Spring2018.xml";
    TString confGem = "GemRun" + TString(isSRC ? "SRC" : "") + "Spring2018.xml";

    /// SI
    TString gPathSiliconConfig = gPathConfig + "/parameters/silicon/XMLConfigs/";
    fDetectorSI = new BmnSiliconStationSet(gPathSiliconConfig + confSi);

    /// GEM
    TString gPathGemConfig = gPathConfig + "/parameters/gem/XMLConfigs/";
    fDetectorGEM = new BmnGemStripStationSet(gPathGemConfig + confGem);

    // Get alignment corrections ...
    TFile* f = nullptr;
    for (Int_t iOpen = 0; iOpen < 2; iOpen++) {
        f = new TFile(fFilename.Data());
        if (iOpen == 0)
            fCorrsGem = GetGemCorrs(f);
        else
            fCorrsSil = GetSiliconCorrs(f);
        delete f;
    }
    remove(fFilename.Data());

    // Get Lorentz corrections ...
    const Int_t nStat = fDetectorGEM->GetNStations();
    nStations = fDetectorGEM->GetNStations();
    fLorCorrs = new Double_t*[nStat];
    UniDetectorParameter* coeffLorCorrs = UniDetectorParameter::GetDetectorParameter("GEM", "lorentz_shift", period, run);
    vector<UniValue*> shifts;
    if (coeffLorCorrs)
        coeffLorCorrs->GetValue(shifts); //
    for (Int_t iEle = 0; iEle < nStat; iEle++) {
        const Int_t nParams = 3; // Parabolic approximation is used
        fLorCorrs[iEle] = new Double_t[nParams];
        for (Int_t iParam = 0; iParam < nParams; iParam++)
            fLorCorrs[iEle][iParam] = (coeffLorCorrs) ? ((LorentzShiftValue*)shifts[iEle])->ls[iParam] : 0.;
    }
    
    for(auto shift:shifts)
        delete shift;
    delete coeffLorCorrs;
    
}

void BmnInnTrackerAlign::Print() {
    cout << "GEM corrections: " << endl;
    for (Int_t iStat = 0; iStat < fDetectorGEM->GetNStations(); iStat++)
        for (Int_t iMod = 0; iMod < fDetectorGEM->GetGemStation(iStat)->GetNModules(); iMod++) {
            cout << "Stat# " << iStat << " Mod# " << iMod << " Corrs# ";
            for (Int_t iCorr = 0; iCorr < 3; iCorr++)
                cout << fCorrsGem[iStat][iMod][iCorr] << " ";
            cout << endl;
        }

    cout << "Lorentz coefficients: " << endl;
    for (Int_t iStat = 0; iStat < fDetectorGEM->GetNStations(); iStat++)
        for (Int_t iCoeff = 0; iCoeff < 3; iCoeff++)
            cout << fLorCorrs[iStat][iCoeff] << " ";
    cout << endl;

    cout << "SILICON corrections: " << endl;
    for (Int_t iStat = 0; iStat < fDetectorSI->GetNStations(); iStat++)
        for (Int_t iMod = 0; iMod < fDetectorSI->GetSiliconStation(iStat)->GetNModules(); iMod++) {
            cout << "Stat# " << iStat << " Mod# " << iMod << " Corrs# ";
            for (Int_t iCorr = 0; iCorr < 3; iCorr++)
                cout << fCorrsSil[iStat][iMod][iCorr] << " ";
            cout << endl;
        }
}

BmnInnTrackerAlign::~BmnInnTrackerAlign() {
    
    for (Int_t iStat = 0; iStat < fDetectorGEM->GetNStations(); iStat++){
        for (Int_t iMod = 0; iMod < fDetectorGEM->GetGemStation(iStat)->GetNModules(); iMod++) {
            delete[] fCorrsGem[iStat][iMod];
        }
        delete[] fCorrsGem[iStat];
        delete[] fLorCorrs[iStat];
    }
    delete[] fCorrsGem;
    delete[] fLorCorrs;

    for (Int_t iStat = 0; iStat < fDetectorSI->GetNStations(); iStat++){
        for (Int_t iMod = 0; iMod < fDetectorSI->GetSiliconStation(iStat)->GetNModules(); iMod++) {
            delete[] fCorrsSil[iStat][iMod];
        }
        delete[] fCorrsSil[iStat];
    }
    delete[] fCorrsSil;

    delete fDetectorGEM;
    delete fDetectorSI;
}

Double_t*** BmnInnTrackerAlign::GetGemCorrs(TFile* file) {
    const Int_t nStat = fDetectorGEM->GetNStations();
    const Int_t nParams = 3;
    Double_t*** corr = new Double_t**[nStat];

    for (Int_t iStat = 0; iStat < nStat; iStat++) {
        Int_t nModul = fDetectorGEM->GetGemStation(iStat)->GetNModules();
        corr[iStat] = new Double_t*[nModul];

        for (Int_t iMod = 0; iMod < nModul; iMod++) {
            corr[iStat][iMod] = new Double_t[nParams];

            for (Int_t iPar = 0; iPar < nParams; iPar++)
                corr[iStat][iMod][iPar] = 0.;
        }
    }

    TTree* t = (TTree*) file->Get("cbmsim");
    TClonesArray* corrs = nullptr;
    t->SetBranchAddress(fBranchGemCorrs.Data(), &corrs);

    for (Int_t iEntry = 0; iEntry < t->GetEntries(); iEntry++) {
        t->GetEntry(iEntry);
        for (Int_t iCorr = 0; iCorr < corrs->GetEntriesFast(); iCorr++) {
            BmnGemAlignCorrections* align = (BmnGemAlignCorrections*) corrs->UncheckedAt(iCorr);
            Int_t iStat = align->GetStation();
            Int_t iMod = align->GetModule();
            corr[iStat][iMod][0] = align->GetCorrections().X();
            corr[iStat][iMod][1] = align->GetCorrections().Y();
            corr[iStat][iMod][2] = align->GetCorrections().Z();
        }
    }
    delete corrs;
    t->Delete();
    return corr;
}

Double_t*** BmnInnTrackerAlign::GetSiliconCorrs(TFile* file) {
    const Int_t nStat = fDetectorSI->GetNStations();
    const Int_t nParams = 3;
    Double_t*** corr = new Double_t**[nStat];

    for (Int_t iStat = 0; iStat < nStat; iStat++) {
        Int_t nModul = fDetectorSI->GetSiliconStation(iStat)->GetNModules();
        corr[iStat] = new Double_t*[nModul];

        for (Int_t iMod = 0; iMod < nModul; iMod++) {
            corr[iStat][iMod] = new Double_t[nParams];

            for (Int_t iPar = 0; iPar < nParams; iPar++)
                corr[iStat][iMod][iPar] = 0.;
        }
    }

    TTree* t = (TTree*) file->Get("cbmsim");
    TClonesArray* corrs = nullptr;
    t->SetBranchAddress(fBranchSilCorrs.Data(), &corrs);

    for (Int_t iEntry = 0; iEntry < t->GetEntries(); iEntry++) {
        t->GetEntry(iEntry);
        for (Int_t iCorr = 0; iCorr < corrs->GetEntriesFast(); iCorr++) {
            BmnSiliconAlignCorrections* align = (BmnSiliconAlignCorrections*) corrs->UncheckedAt(iCorr);
            Int_t iStat = align->GetStation();
            Int_t iMod = align->GetModule();
            corr[iStat][iMod][0] = align->GetCorrections().X();
            corr[iStat][iMod][1] = align->GetCorrections().Y();
            corr[iStat][iMod][2] = align->GetCorrections().Z();
        }
    }
    delete corrs;
    t->Delete();
    return corr;
}


