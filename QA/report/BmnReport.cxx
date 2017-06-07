/**
 * \file BmnReport.cxx
 * \brief Base class for reports.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
#include "BmnReport.h"
#include "BmnHtmlReportElement.h"
#include "TCanvas.h"
#include <fstream>

BmnReport::BmnReport() :
fReportName("qa_report"),
fReportTitle("QA report"),
fOutputDir("./"),
fR(NULL),
fOut(NULL),
fCanvases() {
}

BmnReport::~BmnReport() {
}

void BmnReport::CreateReportElement() {
    if (NULL != fR) delete fR;
    if (NULL != fOut) delete fOut;
    fR = new BmnHtmlReportElement();
    fOut = new ofstream(TString(GetOutputDir() + fReportName + ".html").Data());
}

void BmnReport::DeleteReportElement() {
    if (NULL != fR) delete fR;
    if (NULL != fOut) delete fOut;
}

void BmnReport::CreateReports() {
    Draw(); // User has to implement this function!
    SaveCanvasesAsImages();
    WriteCanvases();

    CreateReportElement();
    Create(); // User has to implement this function!
    DeleteReportElement();
}

TCanvas* BmnReport::CreateCanvas(const char* name, const char* title, Int_t ww, Int_t wh) {
    TCanvas* canvas = new TCanvas(name, title, ww, wh);
    fCanvases.push_back(canvas);
    return canvas;
}

void BmnReport::SaveCanvasesAsImages() const {
    if (GetOutputDir() == "") return;
    Int_t nofCanvases = fCanvases.size();
    cout << "nofCanvases = " << nofCanvases << endl;
    for (Int_t i = 0; i < nofCanvases; i++) {
        TCanvas* canvas = fCanvases[i];
        canvas->SaveAs(TString(GetOutputDir() + string(canvas->GetTitle()) + ".png").Data());
    }
}

void BmnReport::WriteCanvases() const {
    if (GetOutputDir() == "") return;
    Int_t nofCanvases = fCanvases.size();
    for (Int_t i = 0; i < nofCanvases; i++) {
        fCanvases[i]->Write();
    }
}

void BmnReport::PrintCanvases() const {
    Int_t nofCanvases = fCanvases.size();
    for (Int_t i = 0; i < nofCanvases; i++) {
        TCanvas* canvas = fCanvases[i];
        Out() << R()->Image(canvas->GetName(), canvas->GetName());
    }
}

ClassImp(BmnReport)
