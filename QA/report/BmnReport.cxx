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
#include <TError.h>

BmnReport::BmnReport() :
fReportName("qa_report"),
fReportTitle("QA report"),
fOutputDir("./"),
fR(NULL),
fOut(NULL),
fMonitorMode(kFALSE),
fServer(nullptr),
fInitCanvasesDone(kFALSE) {
}

BmnReport::~BmnReport() {
    for (auto h: fCanvases){
        if (h)
            delete h;
    }
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
    if (!fMonitorMode) {
        SaveCanvasesAsImages();
        WriteCanvases();

        CreateReportElement();
        Create(); // User has to implement this function!
        DeleteReportElement();
    }
}

/**
 * @brief Create or just find already created
 * @param name
 * @param title
 * @param ww
 * @param wh
 * @return 
 */
TCanvas* BmnReport::CreateCanvas(const char* name, const char* title, Int_t ww, Int_t wh) {
    TCanvas* canvas = nullptr;
    string sname(name);
    auto it = fCanvasMap.find(sname);
    if (it == fCanvasMap.end()){
    canvas = new TCanvas(name, title, ww, wh);
    fCanvases.push_back(canvas);
    fCanvasMap.insert(make_pair(sname, canvas));
    } else
        canvas = it->second;
    return canvas;
}

void BmnReport::SaveCanvasesAsImages() const {
    if (GetOutputDir() == "") return;
    Int_t nofCanvases = fCanvases.size();
    cout << "nofCanvases = " << nofCanvases << endl;
    gErrorIgnoreLevel = kWarning;
    for (Int_t i = 0; i < nofCanvases; i++) {
        TCanvas* canvas = fCanvases[i];
        canvas->SaveAs(TString(GetOutputDir() + string(canvas->GetTitle()) + ".png").Data());
    }
    gErrorIgnoreLevel = kPrint;
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

void BmnReport::Register(string path){
    for (auto  can : fCanvases){
//        printf("reg %s\n", can->GetName());
        fServer->Register(path.c_str(), can);
    }
}

ClassImp(BmnReport)
