#include "BmnMassSpectrumAnal.h"

BmnMassSpectrumAnal::BmnMassSpectrumAnal(TString cutDir, TString spectraDir) :
fCutDir(cutDir),
fSpectraDir(spectraDir),
fOutput(nullptr) {

    fOutput = new TClonesArray("BmnParticlePairCut");

    TList *files = nullptr;

    // Composing cut list ...
    TSystemDirectory dirCut(cutDir.Data(), cutDir.Data());
    files = dirCut.GetListOfFiles();

    if (files) {
        TSystemFile *file;
        TIter next((TCollection*) files);

        while ((file = (TSystemFile*) next())) {

            TString fname = file->GetName();

            if (!file->IsDirectory() && fname.EndsWith(".root"))
                fCutFiles.push_back(file->GetName());
        }
    }

    // Composing spectra list ...
    TSystemDirectory dirSpectra(spectraDir.Data(), spectraDir.Data());
    files = dirSpectra.GetListOfFiles();

    if (files) {
        TSystemFile *file;
        TIter next((TCollection*) files);

        while ((file = (TSystemFile*) next())) {

            TString fname = file->GetName();

            if (!file->IsDirectory() && fname.EndsWith(".root"))
                fSpectraFiles.push_back(file->GetName());
        }
    }


    ProcessSpectrum();
}

void BmnMassSpectrumAnal::ProcessSpectrum() {
    TChain* chCut = nullptr;

    for (auto cutFile : fCutFiles) {

        chCut = new TChain("bmndata");
        TString f = fCutDir + "/" + cutFile;
        chCut->Add(f.Data());

        if (!chCut->GetFile() || chCut->IsZombie()) {
            delete chCut;
            continue;
        }

        TClonesArray* cuts = nullptr;
        chCut->SetBranchAddress("BmnParticlePairCut", &cuts);

        chCut->GetEntry();
        BmnParticlePairCut* cut = (BmnParticlePairCut*) cuts->UncheckedAt(0);

        Double_t DCA0 = cut->dca0();
        Double_t DCA12 = cut->dca12();
        Double_t DCA1 = cut->dca1();
        Double_t DCA2 = cut->dca2();

        enum {
            silicon, gem
        };

        Int_t nHitsPart1Sil = cut->nHits(0, silicon);
        Int_t nHitsPart1Gem = cut->nHits(0, gem);
        Int_t nHitsPart2Sil = cut->nHits(1, silicon);
        Int_t nHitsPart2Gem = cut->nHits(1, gem);

        pair <Int_t, Int_t> idx = make_pair(nHitsPart1Sil, nHitsPart2Sil); // FIXME

        TString patName = TString::Format("dca0_%G_dca12_%G_dca1_%G_dca2_%G_nHitsPart1_%d_nHitsPart2_%d",
                DCA0, DCA12, DCA1, DCA2, idx.first, idx.second);

        for (auto spectrum : fSpectraFiles) {
            if (!spectrum.Contains(patName.Data()))
                continue;

            // Opening chosen spectra ...
            TFile* file = new TFile(fSpectraDir + "/" + spectrum);
            TH1F* h = (TH1F*) file->Get("invMassSpectrum");

            if (AnalSpectrum(h)) {
                fSelectedSpectra.push_back(*h);
                new ((*fOutput)[fOutput->GetEntriesFast()]) BmnParticlePairCut(*cut);

                // cout << DCA0 << " " << DCA12 << " " << DCA1 << " " << DCA2 << " " << nHitsPart1Sil << " " << nHitsPart2Sil << endl;
            }

            delete file;
        }

        delete chCut;
    }
}

Bool_t BmnMassSpectrumAnal::AnalSpectrum(TH1F* h) {

    // 1. Checking num of entries in the histo ...
    const Int_t nEventsCut = 20;

    if (h->GetEntries() < nEventsCut)
        return kFALSE;

    // 2. Calculating ration MaxValue / MinValue (that is greater than zero)
    const Int_t maxMinRatio = 40;

    set <Double_t> binContents;
    for (Int_t iBin = 1; iBin < h->GetNbinsX() + 1; iBin++) {

        Double_t content = h->GetBinContent(iBin);
        if (content < .5)
            continue;
        else
            binContents.insert(content);
    }

    Double_t min = *min_element(binContents.begin(), binContents.end());
    Double_t max = *max_element(binContents.begin(), binContents.end());

    if (binContents.size() == 0)
        return kFALSE;

    if (Double_t(max / min) < maxMinRatio)
        return kFALSE;

    // 3. Looking deeply in the vicinity of interesting bin ...  
    Int_t binOfInterest = h->FindBin(1.1157); // FIXME
    Double_t binValue = h->GetBinContent(binOfInterest);

    // Getting bin content for neighbouring bins (+- 2 w.r.t. binOfInterest) ...
    binContents.clear();

    binContents.insert(h->GetBinContent(binOfInterest - 2));
    binContents.insert(h->GetBinContent(binOfInterest - 1));
    binContents.insert(h->GetBinContent(binOfInterest + 1));
    binContents.insert(h->GetBinContent(binOfInterest + 2));

    Double_t min1 = *min_element(binContents.begin(), binContents.end());
    Double_t max1 = *max_element(binContents.begin(), binContents.end());

    if (binContents.size() == 0)
        return kFALSE;

    if (Double_t(binValue / min1) < 1.1)
        return kFALSE;

    if (max1 > binValue)
        return kFALSE;

    return kTRUE;
}
