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
    vector<TH1*> histos = HM()->H1Vector(histNamePattern);
    Int_t nofHistos = histos.size();
    if (nofHistos < 1) return;
    for (UInt_t iHist = 0; iHist < nofHistos; iHist++) {
        TH1* hist = histos[iHist];
        string canvasName = GetReportName() + hist->GetName();
        TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 800, 500);
        DrawH1(hist, kLinear, kLinear);
    }
}

void BmnSimulationReport::DrawH1ByPattern(
        const string& histNamePattern,
        string(*labelFormatter)(const string&, const BmnHistManager*)) {
    vector<TH1*> histos = HM()->H1Vector(histNamePattern);
    Int_t nofHistos = histos.size();
    if (nofHistos < 1) return;
    string canvasName = GetReportName() + histos[0]->GetName();
    TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 600, 500);

    vector<string> labels(nofHistos);
    for (UInt_t iHist = 0; iHist < nofHistos; iHist++) {
        string name = histos[iHist]->GetName();
        labels[iHist] = labelFormatter(name, HM());
    }

    DrawH1(histos, labels, kLinear, kLinear, true, 0.3, 0.3, 0.85, 0.6, "PE1");
}

void BmnSimulationReport::DrawH2ByPattern(
        const string& histNamePattern,
        HistScale logx,
        HistScale logy,
        HistScale logz,
        const string& drawOpt) {
    vector<TH2*> histos = HM()->H2Vector(histNamePattern);
    Int_t nofHistos = histos.size();
    if (nofHistos < 1) return;
    for (UInt_t iHist = 0; iHist < nofHistos; iHist++) {
        TH2* hist = histos[iHist];
        string canvasName = GetReportName() + hist->GetName();
        TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 800, 500);
        DrawH2(hist, logx, logy, logz, drawOpt);
    }
}

ClassImp(BmnSimulationReport)
