#include <Rtypes.h>
#include <TString.h>
#include <TFile.h>
#include <TH1.h>
#include <TF1.h>
#include <TCanvas.h>
#include <vector>

#include <TString.h>
#include <Rtypes.h>
R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"

using namespace std;

// Background function ...

Double_t background(Double_t *xx, Double_t *p) {
    Double_t fA = 1.07;
    Double_t fB = 1.22;

    vector <Double_t> fT;
    fT.resize(6); // polynomial

    Double_t x = (2.0 * xx[0] - fA - fB) / (fB - fA);
    int order = fT.size();
    if (order == 1)
        return p[0];
    if (order == 2)
        return p[0] + x * p[1];
    // build the polynomials
    fT[0] = 1;
    fT[1] = x;
    for (Int_t i = 1; i < order; ++i) {
        fT[i + 1] = 2 * x * fT[i] - fT[i - 1];
    }
    Double_t sum = p[0] * fT[0];
    for (int i = 1; i <= order; ++i) {
        sum += p[i] * fT[i];
    }
    return sum;
}

// Signal function ...

Double_t signal(Double_t *xx, Double_t *p) {
    Double_t arg = (xx[0] - p[1]) / p[2];
    Double_t fitval = p[0] * TMath::Exp(-0.5 * arg * arg);
    return fitval;
}

// Lorentzian Peak function

Double_t lorentzianPeak(Double_t *x, Double_t *par) {
    return (0.5 * par[0] * par[1] / TMath::Pi()) / TMath::Max(1.e-10,
            (x[0] - par[2])*(x[0] - par[2]) + .25 * par[1] * par[1]);
}

// Sum of background and peak function

Double_t fitFunction(Double_t *x, Double_t *par) {
    return background(x, &par[0]) + signal(x, &par[7]);
}

// A useful macro that can be used for fit of mass spectrum to extract signal and background ...
// All necessary procedures are described there and can be used in chain just passing a file containing spectrum ...
// inFile is a .root file where spectrum is saved ...

void fitSpectrum(TString inFile, TString spectrumName) {
    TFile* f = new TFile(inFile.Data());
    if (!f->IsOpen())
        return;
    
    TH1F* h = (TH1F*) f->Get(spectrumName.Data()); // get histo with spectrum ...
    
    if (!h)
        return;
    h->GetXaxis()->SetLabelSize(.05);
    h->GetXaxis()->SetTitleOffset(-.6);
    h->GetYaxis()->SetLabelSize(.05);
    h->GetYaxis()->SetTitle("");
    h->GetXaxis()->SetTitle("M_{(p + #pi^{-})} [GeV/c^{2}]");
    h->GetXaxis()->SetTitleSize(0.06);
    h->GetXaxis()->CenterTitle();

    // Bin signal range ...
    const Int_t binMin = 20; //20;
    const Int_t binMax = 26; //27;

    vector <Double_t> binErrorsAll;
    vector <Double_t> binErrorsSignal;

    // Saving bin errors for all bins ...
    for (Int_t iBin = 1; iBin < h->GetNbinsX() + 1; iBin++)
        binErrorsAll.push_back(h->GetBinError(iBin));

    // Saving bin errors for signal region ...
    for (Int_t iBin = binMin; iBin < binMax + 1; iBin++)
        binErrorsSignal.push_back(h->GetBinError(iBin));

    // Setting zero errors for signal region ...
    for (Int_t iBin = binMin; iBin < binMax + 1; iBin++)
        h->SetBinError(iBin, 0.);

    Double_t xmin = 1.08;
    Double_t xmax = 1.22;
    Double_t n = 6;

    TF1* fitBackground = new TF1("f1", background, xmin, xmax, n + 1);
    fitBackground->SetLineColor(kBlue);
    fitBackground->SetLineStyle(kDashed);
    TFitResultPtr backRes = h->Fit(fitBackground, "SR+", "", xmin, xmax);

    // Setting zero error for all range ...
    for (Int_t iBin = 1; iBin < h->GetNbinsX() + 1; iBin++)
        h->SetBinError(iBin, 0.);

    // Restoring errors for signal range ...
    for (Int_t iBin = binMin; iBin < binMax + 1; iBin++)
        h->SetBinError(iBin, binErrorsSignal[iBin - binMin]);

    TF1* fitSignal = new TF1("f2", signal, xmin, xmax, 3);

    fitSignal->SetParameter(0, 2000.);
    fitSignal->SetParameter(1, 1.115);
    fitSignal->SetParameter(2, 0.002);
    fitSignal->SetNpx(500);

    fitSignal->SetLineColor(kRed);
    TFitResultPtr sigRes = h->Fit(fitSignal, "SR0+", "", h->GetBinCenter(binMin), h->GetBinCenter(binMax));

    // Restoring all bin errors ...
    for (Int_t iBin = 1; iBin < h->GetNbinsX() + 1; iBin++)
        h->SetBinError(iBin, binErrorsAll[iBin]);


    TF1* fitSpectrum = new TF1("f", fitFunction, xmin, xmax, 10);
    fitSpectrum->SetLineColor(kMagenta);
    fitSpectrum->SetNpx(500);
    fitSpectrum->SetParameter(0, backRes->Parameter(0));
    fitSpectrum->SetParameter(1, backRes->Parameter(1));
    fitSpectrum->SetParameter(2, backRes->Parameter(2));
    fitSpectrum->SetParameter(3, backRes->Parameter(3));
    fitSpectrum->SetParameter(4, backRes->Parameter(4));
    fitSpectrum->SetParameter(5, backRes->Parameter(5));
    fitSpectrum->SetParameter(6, backRes->Parameter(6));
    fitSpectrum->SetParameter(7, sigRes->Parameter(0));
    fitSpectrum->SetParameter(8, sigRes->Parameter(1));
    fitSpectrum->SetParameter(9, sigRes->Parameter(2));

    TFitResultPtr res = h->Fit(fitSpectrum, "SR+", "", xmin, xmax);

    h->Draw("PE1X0");

    TLegend *legend = new TLegend(0.6, 0.25, 0.61, 0.45);
    legend->SetTextFont(72);
    legend->SetTextSize(0.04);

    Double_t mean = res->Parameter(8);
    Double_t sigma = TMath::Abs(res->Parameter(9));

    legend->AddEntry("", Form("Mass = %.4f", mean), 0);
    legend->AddEntry("", Form("Sigma = %.4f", sigma), 0);

    // Calculating signal ...
    // Double_t T = h->Integral(h->FindBin(mean - 3 * sigma), h->FindBin(mean + 3 * sigma));
    Double_t T = fitSpectrum->Integral(mean - 3 * sigma, mean + 3 * sigma) / h->GetBinWidth(1);
    Double_t B = fitBackground->Integral(mean - 3 * sigma, mean + 3 * sigma) / h->GetBinWidth(1);

    legend->AddEntry("", Form("All, %d", Int_t(T)), 0);
    legend->AddEntry("", Form("Background, %d", Int_t(B)), 0);

    legend->AddEntry("", Form("Numb. of #Lambda^{0}  = %d", Int_t(T - B)), 0);
    legend->SetLineColor(0);
    legend->Draw();
}
