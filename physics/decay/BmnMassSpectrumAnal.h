#include <TNamed.h>
#include <TChain.h>
#include <TSystemDirectory.h>
#include <TCollection.h>
#include <TClonesArray.h>
#include <TFile.h>
#include <TH1F.h>
#include <TF1.h>
#include <TFitResult.h>
#include <TLegend.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TLatex.h>

#include <BmnParticlePair.h>
#include <BmnParticlePairCut.h>
#include <UniDetectorParameter.h>
#include <UniRun.h>

#include <iostream>
#include <vector>

using namespace std;

#ifndef BMNMASSSPECTRUMANAL_H
#define BMNMASSSPECTRUMANAL_H 1

class BmnMassSpectrumAnal : public TNamed {
public:

    BmnMassSpectrumAnal();

    BmnMassSpectrumAnal(TString);

    vector <TString> createFilelist();

    void SetTarget(TString t) {
        fTarget.push_back(t);
    }

    void SetSpectrumRange(Double_t min, Double_t max) {
        xLow = min;
        xUp = max;
    }

    void SetSignalCut(Double_t s) {
        fSignal = s;
    }

    void SetBackgroungCut(Double_t b) {
        fBackground = b;
    }

    // Cuts to check optimal ones ...

    void SetCuts(vector <Double_t> DCA0,
            vector <Double_t> DCA1, vector <Double_t> DCA2, vector <Double_t> DCA12, vector <Double_t> PATH,
            vector <Int_t> nHitsPos, vector <Int_t> nHitsNeg) {

        // Looping over cut vectors ...
        for (auto dca0 : DCA0)
            for (auto dca1 : DCA1)
                for (auto dca2 : DCA2)
                    for (auto dca12 : DCA12)
                        for (auto path : PATH)
                            for (auto nPos : nHitsPos)
                                for (auto nNeg : nHitsNeg) {
                                    BmnParticlePairCut* pCut = new ((*fParticlePairCuts) [fParticlePairCuts->GetEntriesFast()]) BmnParticlePairCut();
                                    pCut->SetDca0(dca0);
                                    pCut->SetDca1(dca1);
                                    pCut->SetDca2(dca2);
                                    pCut->SetDca12(dca12);
                                    pCut->SetPath(path);
                                    pCut->SetNHitsGemPos(nPos);
                                    pCut->SetNHitsGemNeg(nNeg);
                                }

        if (fParticlePairCuts->GetEntriesFast())
            DoOptimization();
    }

    void SetNFiles(Int_t n) {
        fNFiles = n;
    }

    virtual ~BmnMassSpectrumAnal() {

    }

    void SetStartRun(Int_t start) {
        fStartRun = start;
    }

    void SetFinishRun(Int_t finish) {
        fFinishRun = finish;
    }

    vector <TString> GetFileList() {
        return fInFiles;
    }

private:
    void DoOptimization();
    Double_t ImproveCutValue(vector <TString>, BmnParticlePairCut*, TString);

    Int_t FinBin(map <Int_t, pair <Double_t, Double_t>> binMap, Double_t value) {
        for (auto it : binMap) {
            Int_t bin = it.first;
            Double_t min = it.second.first;
            Double_t max = it.second.second;

            if (value > min && value < max)
                return bin;
        }
        // If multBin not found ...
        return -1;
    }

    Bool_t isVectorOk(vector <Double_t> vector) {

        for (auto value : vector)
            if (TMath::Abs(value) > 100. || TMath::IsNaN(value))
                return kFALSE;

        return kTRUE;
    }

protected:
    void ReadFile(TString, BmnParticlePairCut* cut0 = nullptr, TClonesArray* triggEffInfo = nullptr, Double_t pathMin = 0., Double_t pathMax = 0.);

    TFitResultPtr fitSpectrum(TH1F*);
    void fitSpectrum(TH1F*, Double_t&, Double_t&, pair <Double_t, Double_t>&, pair <Double_t, Double_t>&);
    Bool_t checkFit(BmnParticlePairCut*);
    static Double_t background(Double_t*, Double_t*);
    static Double_t signal(Double_t*, Double_t*);
    static Double_t fitFunction(Double_t*, Double_t*);

    void GetPtY(vector <Double_t>, vector <Double_t>, Double_t&, Double_t&);

    Double_t deltaB(TF1* f, TFitResultPtr fitRes) {
        TH1F* h = new TH1F("h", "h", 75., xLow, xUp);

        Double_t min = h->GetBinCenter(fSignalBinMin);
        Double_t max = h->GetBinCenter(fSignalBinMax);

        if (fitRes.Get()) {
            Double_t err = f->IntegralError(min - .5 * h->GetBinWidth(1), max + .5 * h->GetBinWidth(1),
                    fitRes->GetParams(), fitRes->GetCovarianceMatrix().GetMatrixArray()) / h->GetBinWidth(1);
            delete h;
            return err;
        } else {
            delete h;
            return 0.;
        }
    }

protected:
    // Run info ...
    Int_t fPeriod;
    Int_t fStartRun;
    Int_t fFinishRun;

    vector <TString> fInFiles;

    // Beam, target info ...
    TString fBeam;

private:
    TClonesArray* fParticlePairCuts;

    // Spectrum histos ...
    TH1F** hSpectra;

    // S and B cuts if needed ...
    Double_t fSignal;
    Double_t fBackground;

protected:
    Double_t xLow;
    Double_t xUp;

    Int_t fSignalBinMin;
    Int_t fSignalBinMax;

    vector <TString> fTarget;
    Int_t fNFiles;

    // Spectrum for improved cut values ...
    TH1F* hSpectrumImproved;

    // Spectra in Pt-Y space ...
    Bool_t isPtY;
    TH1F** hSpectraPt;
    TH1F** hSpectraY;

    Int_t nPtBins;
    map <Int_t, pair <Double_t, Double_t>> fPtBinMap;

    Int_t nYBins;
    map <Int_t, pair <Double_t, Double_t>> fYBinMap;

    Int_t nPathBins;
    map <Int_t, pair <Double_t, Double_t>> fPathBins;

    Bool_t isMc;

    ClassDef(BmnMassSpectrumAnal, 0)
};


#endif