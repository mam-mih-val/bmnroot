#include "BmnEfficiencyProbability.h"

BmnEfficiencyProbability::BmnEfficiencyProbability(TString f) :
fFile(nullptr),
hHisto(nullptr),
fDet(""),
fStat(-1),
fMod(-1),
fZone("") {
    fFile = new TFile(f.Data(), "read");

    if (!fFile->IsOpen() || fFile->IsZombie())
        return;
}

Double_t BmnEfficiencyProbability::GetProbability() {
    gRandom->SetSeed(0);

    TString ele = "";

    if (fDet.Contains("GEM")) {
        Int_t idxZone = fZone.Contains("hot") ? 0 : fZone.Contains("big") ? 1 : -1;
        if (idxZone == -1) {
            cout << "Zone is not defined!!! Exiting ..." << endl;
            throw;
        } else
            ele = TString::Format("Detector# %s, Station# %d, Module# %d, Zone# %d", fDet.Data(), fStat, fMod, idxZone);       
    } 
    
    else if (fDet.Contains("SILICON"))
        ele = TString::Format("Detector# %s, Station# %d, Module# %d", fDet.Data(), fStat, fMod);

    hHisto = (TH1F*) fFile->Get(ele.Data());

    if (TMath::Abs(hHisto->Integral() - 1.) > FLT_EPSILON)
        throw;

    Double_t x = -1., y = -1., content = -1.;

    do {
        x = gRandom->Rndm();
        //        y = hHisto->GetBinContent(hHisto->GetMaximumBin()) * gRandom->Rndm();
        y = 1. * gRandom->Rndm();

        // cout << x << " -- " << y << endl;

        // Looking for a bin corresponding to x ...
        Int_t bin = hHisto->FindBin(x);
        // cout << bin << endl;
        content = hHisto->GetBinContent(bin);
        // cout << content << endl;
    } while (y > content);

    // cout << "Eff# " << x << endl;
    return x;
}

