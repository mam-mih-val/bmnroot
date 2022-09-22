/**
 * \file BmnSimulationReport.cxx
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
#include "BmnSimulationReport.h"
#include "BmnReportElement.h"
#include "BmnHistManager.h"
#include "BmnDrawHist.h"
#include "TFile.h"

#include <iostream>
#include <fstream>
#include <string>
using std::cout;
using std::ofstream;
using std::string;

BmnSimulationReport::BmnSimulationReport() :
BmnReport(),
fPrimes(kFALSE),
fHM(NULL) {

}

BmnSimulationReport::~BmnSimulationReport() {

}

void BmnSimulationReport::Create(BmnHistManager* histManager, const string& outputDir) {
    assert(histManager != NULL);
    fHM = histManager;
    SetOutputDir(outputDir);
    CreateReports();
}

void BmnSimulationReport::Create(const string& fileName, const string& outputDir) {
    assert(fHM == NULL);
    fHM = new BmnHistManager();
    TFile* file = new TFile(fileName.c_str());
    fHM->ReadFromFile(file);
    SetOutputDir(outputDir);
    CreateReports();
    //   delete fHM;
    //   delete file;
}

void BmnSimulationReport::DrawH1ByPattern(
        const string& histNamePattern) {
//    vector<TH1*> histos = HM()->H1Vector(histNamePattern);
//    Int_t nofHistos = histos.size();
//    if (nofHistos < 1) return;
//    for (UInt_t iHist = 0; iHist < nofHistos; iHist++) {
//        TH1* hist = histos[iHist];
//        string canvasName = GetReportName() + hist->GetName();
//        TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 800, 500);
//        DrawH1(hist, kLinear, kLinear);
//    }
}

void BmnSimulationReport::DrawH1ByPattern(
        const string& histNamePattern,
        string(*labelFormatter)(const string&, const BmnHistManager*)) {
//    vector<TH1*> histos = HM()->H1Vector(histNamePattern);
//    Int_t nofHistos = histos.size();
//    if (nofHistos < 1) return;
//    string canvasName = GetReportName() + histos[0]->GetName();
//    TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 600, 500);
//
//    vector<string> labels(nofHistos);
//    for (UInt_t iHist = 0; iHist < nofHistos; iHist++) {
//        string name = histos[iHist]->GetName();
//        labels[iHist] = labelFormatter(name, HM());
//    }
//
//    DrawH1(histos, labels, kLinear, kLinear, true, 0.3, 0.3, 0.85, 0.6, "PE1");
}

void BmnSimulationReport::DrawH2ByPattern(
        const string& histNamePattern,
        HistScale logx,
        HistScale logy,
        HistScale logz,
        const string& drawOpt) {
//    vector<TH2*> histos = HM()->H2Vector(histNamePattern);
//    Int_t nofHistos = histos.size();
//    if (nofHistos < 1) return;
//    for (UInt_t iHist = 0; iHist < nofHistos; iHist++) {
//        TH2* hist = histos[iHist];
//        string canvasName = GetReportName() + hist->GetName();
//        TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 800, 500);
//        DrawH2(hist, logx, logy, logz, drawOpt);
//    }
}

void BmnSimulationReport::DrawMuSigma(TVirtualPad* pad, TH1* h) {
    pad->cd();
    TF1* fit = h->GetFunction("gaus");
    if (!fit) return;
    Float_t xMax = h->GetXaxis()->GetXmax();
    Float_t yMax = h->GetMaximum();
    TPaveStats* ps = new TPaveStats(xMax / 2, yMax / 2, xMax, yMax);
    ps->SetFillColor(0);
    ps->SetShadowColor(0);
    ps->AddText(Form("#mu = %2.3f", fit->GetParameter(1)));
    ps->AddText(Form("#sigma = %2.3f", fit->GetParameter(2)));
    ps->Draw();
}

void BmnSimulationReport::DrawOneH1(const TString canvasName, const TString name1, const TString drawOpt) {
    TCanvas* canvas = CreateCanvas(canvasName.Data(), canvasName.Data(), baseW, baseH);
    if (!fInitCanvasesDone) {
    canvas->SetGrid();
    }
    canvas->cd();
    DrawH1(/*canvas,*/ HM()->H1(name1), kLinear, kLog, drawOpt.Data(), kRed, 1, 0.75, 1.1, 20);
}

void BmnSimulationReport::DrawTwoH1(const TString canvasName, const TString name1, const TString name2, const TString drawOpt, Bool_t doFit) {
    TCanvas* canvas = CreateCanvas(canvasName.Data(), canvasName.Data(), 2 * baseW, baseH);
    if (!fInitCanvasesDone) {
    canvas->SetGrid();
    canvas->Divide(2, 1);
    }
    canvas->cd(1);
    DrawH1(/*canvas,*/ HM()->H1(name1), kLinear, kLinear, drawOpt.Data(), kRed, 1, 0.75, 1.1, 20);
    if ((doFit) && (HM()->H1(name1)->GetEntries() > MinNEntries4Fit)) {
        HM()->H1(name1)->Fit("gaus", "WWQ");
        DrawMuSigma(canvas->cd(1), HM()->H1(name1));
    }
    canvas->cd(2);
    DrawH1(/*canvas,*/ HM()->H1(name2), kLinear, kLinear, drawOpt.Data(), kRed, 1, 0.75, 1.1, 20);
    if ((doFit) && (HM()->H1(name2)->GetEntries() > MinNEntries4Fit)) {
        HM()->H1(name2)->Fit("gaus", "WWQ");
        DrawMuSigma(canvas->cd(2), HM()->H1(name2));
    }
}

void BmnSimulationReport::DrawOneH2(const TString canvasName, const TString name1) {
    TCanvas* canvas = CreateCanvas(canvasName.Data(), canvasName.Data(), baseW, baseH);
    if (!fInitCanvasesDone) {
    canvas->SetGrid();
    }
    canvas->cd();
    DrawH2(/*canvas,*/ HM()->H2(name1), kLinear, kLinear, kLinear, "colz");
}

void BmnSimulationReport::DrawTwoH2(const TString canvasName, const TString name1, const TString name2, Int_t w, Int_t h) {
    TCanvas* canvas = CreateCanvas(canvasName.Data(), canvasName.Data(), w, h);
    if (!fInitCanvasesDone) {
        canvas->SetGrid();
        canvas->Divide(2, 1);
    }
    TVirtualPad *p = canvas->cd(1);
    DrawH2(/*canvas,*/ HM()->H2(name1), kLinear, kLinear, kLinear, "colz");
    canvas->cd(2);
    DrawH2(/*canvas,*/ HM()->H2(name2), kLinear, kLinear, kLinear, "colz");
//    canvas->Update();
//    canvas->Modified();
}

void BmnSimulationReport::DrawThreeH2(const TString canvasName, const TString name1, const TString name2, const TString name3) {
    TCanvas* canvas = CreateCanvas(canvasName.Data(), canvasName.Data(), 3 * baseW, baseH);
    if (!fInitCanvasesDone) {
        canvas->SetGrid();
        canvas->Divide(3, 1);
    }
    canvas->cd(1);
    DrawH2(/*canvas,*/ HM()->H2(name1), kLinear, kLinear, kLinear, "colz");
    canvas->cd(2);
    DrawH2(/*canvas,*/ HM()->H2(name2), kLinear, kLinear, kLinear, "colz");
    canvas->cd(3);
    DrawH2(/*canvas,*/ HM()->H2(name3), kLinear, kLinear, kLinear, "colz");
    canvas->Update();
    canvas->Modified();
}

void BmnSimulationReport::DrawThreeH1(const TString canvasName, const TString name1, const TString name2, const TString name3) {
    TCanvas* canvas = CreateCanvas(canvasName.Data(), canvasName.Data(), 3 * baseW, baseH);
    if (!fInitCanvasesDone) {
    canvas->SetGrid();
    canvas->Divide(3, 1);
    }
    canvas->cd(1);
    DrawH1(/*canvas,*/ HM()->H1(name1), kLinear, kLinear, "", kBlue, 1, 0.75, 1.1, 20);
    canvas->cd(2);
    DrawH1(/*canvas,*/ HM()->H1(name2), kLinear, kLinear, "", kBlue, 1, 0.75, 1.1, 20);
    canvas->cd(3);
    DrawH1(/*canvas,*/ HM()->H1(name3), kLinear, kLinear, "", kBlue, 1, 0.75, 1.1, 20);
}


ClassImp(BmnSimulationReport)
