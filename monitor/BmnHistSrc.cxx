#include "BmnHistSrc.h"

BmnHistSrc::BmnHistSrc(TString title, TString path, Int_t periodID, BmnSetup setup) : BmnHist(periodID, setup),
hTDCTimes(nullptr),
hTDCAmps(nullptr),
hTQDCTimes(nullptr),
hTQDCAmps(nullptr) {
    refPath = path;
    fTitle = title;
    fName = title + "_cl";
    canvas = NULL;
}

BmnHistSrc::~BmnHistSrc() {
    delete canvas;
    delete can2d;
    if (fDir)
        return;
    //    if (hTDC) delete hTDC;
    for (auto pad : canPads)
        delete pad;
    for (auto pad : can2dPads)
        delete pad;
}

void BmnHistSrc::InitHistsFromArr(vector<TClonesArray*> *trigAr) {
    const Int_t MaxTimeTDC = 600;
    const Int_t MaxAmpTDC = 50;
    const Int_t MaxTimeTQDC = 600;
    const Int_t MaxAmpTQDC = 10000;
    const Int_t MaxAmpTQDC_x10 = 33000;
    regex reX10("TQDC_.+_X10.*");
    const Int_t rows4Spectrum = 2;
    Int_t arLen = trigAr->size();
    TString name;
    fCols = SRC_COLS;
    fRows = arLen; // / fSrcCols;
    for (Int_t i = 0; i < fRows; ++i) {
        TClonesArray * ar = trigAr->at(i);
        if (ar->GetClass() == BmnTrigWaveDigit::Class()) {
            regex re("TQDC_(.+)");
            //            tqdcNames.push_back(TString(regex_replace(ar->GetName(), re, "$1")));
            tqdcNames.push_back(TString(ar->GetName()));
            //            cout <<regex_replace(ar->GetName(), re, "$1") << endl;
        } else
            tdcNames.push_back(TString(ar->GetName()));
    }
    // 2 dimensional time

    name = fTitle + "_TDC_Times_by_Trigger";
    hTDCTimes = new TH2F(name, name, tdcNames.size(), 0, tdcNames.size(), 300, 0, MaxTimeTDC);
    hTDCTimes->SetDirectory(fDir);
    hTDCTimes->GetXaxis()->SetTitle("Trigger Name");
    hTDCTimes->GetYaxis()->SetTitle("Time, ns");
    TAxis* xaTrigTimes = hTDCTimes->GetXaxis();
    for (Int_t i = 0; i < tdcNames.size(); ++i) {
        xaTrigTimes->SetBinLabel(i + 1, tdcNames[i]);
    }
    name = fTitle + "_TDC_Amps_by_Trigger";
    hTDCAmps = new TH2F(name, name, tdcNames.size(), 0, tdcNames.size(), 300, 0, MaxAmpTDC);
    hTDCAmps->SetDirectory(fDir);
    hTDCAmps->GetXaxis()->SetTitle("Trigger Name");
    hTDCAmps->GetYaxis()->SetTitle("Amplitude, ns");
    xaTrigTimes = hTDCAmps->GetXaxis();
    for (Int_t i = 0; i < tdcNames.size(); ++i) {
        xaTrigTimes->SetBinLabel(i + 1, tdcNames[i]);
    }
    name = fTitle + "_TQDC_Times_by_Trigger";
    hTQDCTimes = new TH2F(name, name, tqdcNames.size(), 0, tqdcNames.size(), 300, 0, MaxTimeTQDC);
    hTQDCTimes->SetDirectory(fDir);
    hTQDCTimes->GetXaxis()->SetTitle("Trigger Name");
    hTQDCTimes->GetYaxis()->SetTitle("Time, ns");
    xaTrigTimes = hTQDCTimes->GetXaxis();
    for (Int_t i = 0; i < tqdcNames.size(); ++i) {
        xaTrigTimes->SetBinLabel(i + 1, tqdcNames[i]);
    }
    name = fTitle + "_TQDC_Amps_by_Trigger";
    hTQDCAmps = new TH2F(name, name, tqdcNames.size(), 0, tqdcNames.size(), 300, 0, MaxAmpTQDC);
    hTQDCAmps->SetDirectory(fDir);
    hTQDCAmps->GetXaxis()->SetTitle("Trigger Name");
    hTQDCAmps->GetYaxis()->SetTitle("Amplitude");
    xaTrigTimes = hTQDCAmps->GetXaxis();
    for (Int_t i = 0; i < tqdcNames.size(); ++i) {
        xaTrigTimes->SetBinLabel(i + 1, tqdcNames[i]);
    }

    name = fTitle + "CanvasTimesByChannel";
    can2d = new TCanvas(name, name, PAD_WIDTH * fCols, PAD_HEIGHT * rows4Spectrum);
    can2d->Divide(fCols, rows4Spectrum, 0.001, 0.001);
    can2dPads.resize(fCols * rows4Spectrum);
    for (Int_t iPad = 0; iPad < rows4Spectrum * fCols; iPad++) {
        PadInfo* p = new PadInfo();
        p->opt = "colz ";
        can2dPads[iPad] = p;
        can2d->GetPad(iPad + 1)->SetGrid();
    }
    can2dPads[0]->current = hTDCTimes;
    can2dPads[1]->current = hTQDCTimes;
    can2dPads[2]->current = hTDCAmps;
    can2dPads[3]->current = hTQDCAmps;
    // set style props
    for (PadInfo *pad : can2dPads) {
        TH1* h = pad->current;
        if (h) {
            h->SetTitleSize(0.07, "XY");
            h->SetLabelSize(0.045, "XY");
            TAxis *ax = h->GetXaxis();
            ax->SetTitleColor(kOrange + 10);
            ax->SetTitleOffset(0.7);
            ax->SetTitleFont(62);
            TAxis *ay = h->GetYaxis();
            ay->SetTitleColor(kOrange + 10);
            ay->SetTitleOffset(0.6);
            ay->SetTitleFont(62);
        }
    }

    // 1 dimensional
    hists.resize(fRows);
    histsAux.resize(fRows);
    for (Int_t i = 0; i < hists.size(); i++) {
        hists[i].resize(fCols, nullptr);
        histsAux[i].resize(fCols, nullptr);
    }

    for (Int_t iRow = 0; iRow < fRows; iRow++) { // 0 column - Time histograms from TDC by TQDC
        TClonesArray * ar = trigAr->at(iRow);
        Int_t maxTime =
                (ar->GetClass() == BmnTrigWaveDigit::Class()) ? MaxTimeTQDC : MaxTimeTDC;
        name = fTitle + "_" + ar->GetName() + "_Leading_Time";
        TH1F *h = new TH1F(name, name, 800, 0, maxTime);
        h->SetTitleSize(0.06, "XY");
        h->SetLabelSize(0.08, "XY");
        h->GetXaxis()->SetTitle("Time, ns");
        h->GetXaxis()->SetTitleOffset(0.6);
        h->GetXaxis()->SetTitleColor(kOrange + 10);
        h->GetYaxis()->SetTitle("Activation Count");
        h->GetYaxis()->SetTitleOffset(0.6);
        h->GetYaxis()->SetTitleColor(kOrange + 10);
        hists[iRow][0] = h;
        if (ar->GetClass() == BmnTrigWaveDigit::Class()) {
            name = fTitle + "_" + ar->GetName() + "_Leading_Time_filtered";
            h = new TH1F(name, name, 800, 0, maxTime);
            h->SetTitleSize(0.06, "XY");
            h->SetLabelSize(0.08, "XY");
            h->GetXaxis()->SetTitle("Time, ns");
            h->GetXaxis()->SetTitleOffset(0.6);
            h->GetXaxis()->SetTitleColor(kOrange + 10);
            h->GetYaxis()->SetTitle("Activation Count");
            h->GetYaxis()->SetTitleOffset(0.6);
            h->GetYaxis()->SetTitleColor(kOrange + 10);
            h->SetLineColor(kMagenta);
            histsAux[iRow][0] = h;
        }
    }
    for (Int_t iRow = 0; iRow < fRows; iRow++) { // 1 column - Amplitude(TQDC -waveform ampl.,  TDC - sig. width)
        TClonesArray * ar = trigAr->at(iRow);
        Int_t maxAmp =
                (ar->GetClass() == BmnTrigWaveDigit::Class()) ?
                (regex_match(ar->GetName(), reX10) ? MaxAmpTQDC_x10 : MaxAmpTQDC) :
                MaxAmpTDC;
        name = fTitle + "_" + ar->GetName() + "_Amplitude";
        TH1F *h = new TH1F(name, name, 300, 0, maxAmp);
        h->SetTitleSize(0.06, "XY");
        h->SetLabelSize(0.08, "XY");
        h->GetXaxis()->SetTitle(
                (ar->GetClass() == BmnTrigWaveDigit::Class()) ? "Amplitude" : "Width, ns");
        h->GetXaxis()->SetTitleOffset(0.6);
        h->GetXaxis()->SetTitleColor(kOrange + 10);
        h->GetYaxis()->SetTitle("Activation Count");
        h->GetYaxis()->SetTitleOffset(0.5);
        h->GetYaxis()->SetTitleColor(kOrange + 10);
        hists[iRow][1] = h;
        if (ar->GetClass() == BmnTrigWaveDigit::Class()) {
            name = fTitle + "_" + ar->GetName() + "_Amplitude_filtered";
            h = new TH1F(name, name, 300, 0, maxAmp);
            h->SetTitleSize(0.06, "XY");
            h->SetLabelSize(0.08, "XY");
            h->GetXaxis()->SetTitle("Amplitude");
            h->GetXaxis()->SetTitleOffset(0.6);
            h->GetXaxis()->SetTitleColor(kOrange + 10);
            h->GetYaxis()->SetTitle("Activation Count");
            h->GetYaxis()->SetTitleOffset(0.5);
            h->GetYaxis()->SetTitleColor(kOrange + 10);
            h->SetLineColor(kMagenta);
            histsAux[iRow][1] = h;
        }
    }
    // Create canvas
    name = fTitle + "Canvas";
    canvas = new TCanvas(name, name, PAD_WIDTH * fCols, PAD_HEIGHT * fRows);
    canvas->Divide(fCols, fRows, 0.0001, 0.0001);
    canPads.resize(fCols * fRows);
    Names.resize(fCols * fRows);
    for (Int_t iRow = 0; iRow < fRows; iRow++)
        for (Int_t iCol = 0; iCol < fCols; iCol++) {
            PadInfo *p = new PadInfo();
            p->current = hists[iRow][iCol];
            p->aux.push_back(histsAux[iRow][iCol]);
            Int_t iPad = iRow * fCols + iCol;
            //            printf("ipad %d irow %d icol %d  %08X %08X\n", iPad, iRow, iCol, canvas, canvas->GetPad(iPad + 1));
            canPads[iPad] = p;
            canvas->GetPad(iPad + 1)->SetGrid();
            if (p->current) {
                Names[iPad] = p->current->GetName();
            }
        }
}

void BmnHistSrc::Register(THttpServer * serv) {
    isShown = kTRUE;
    fServer = serv;
    if (canvas == NULL)
        return;
    fServer->Register("/", this);
    TString path = "/" + fTitle + "/";
    fServer->Register(path, canvas);
    fServer->Register(path, can2d);
    //    fServer->Register(path, hTDCTimes);
    fServer->SetItemField(path, "_monitoring", "2000");
    fServer->SetItemField(path, "_layout", "grid3x3");
    TString cmd = "/" + fName + "/->Reset()";
    TString cmdTitle = path + "Reset";
    fServer->RegisterCommand(cmdTitle.Data(), cmd.Data(), "button;");
    fServer->Restrict(cmdTitle.Data(), "visible=shift");
    fServer->Restrict(cmdTitle.Data(), "allow=shift");
    fServer->Restrict(cmdTitle.Data(), "deny=guest");
    cmd = "/" + fName + "/->SetRefRun(%arg1%)";
    cmdTitle = path + "SetRefRun";
    fServer->RegisterCommand(cmdTitle.Data(), cmd.Data(), "button;");
    //    fServer->Restrict(cmdTitle.Data(), "deny=guest");

}

void BmnHistSrc::SetDir(TFile *outFile, TTree * recoTree) {
    frecoTree = recoTree;
    fDir = nullptr;
    if (outFile != nullptr)
        fDir = outFile->mkdir(fTitle + "_hists");
    if (canvas == nullptr)
        return;
    SetDir(fDir);
}

void BmnHistSrc::SetDir(TDirectory* Dir) {
    fDir = Dir;
    for (auto row : hists)
        for (auto el : row)
            if (el)
                el->SetDirectory(fDir);
    for (auto row : histsAux)
        for (auto el : row)
            if (el)
                el->SetDirectory(fDir);
    if (hTDCTimes) hTDCTimes->SetDirectory(fDir);
}

void BmnHistSrc::DrawBoth() {
    BmnHist::DrawRef(canvas, &canPads);
    BmnHist::DrawRef(can2d, &can2dPads);
}

void BmnHistSrc::FillFromDigi(DigiArrays *fDigiArrays) {
    vector<TClonesArray*> *trigAr = fDigiArrays->trigAr;
    if (!trigAr)
        return;
    if (!canvas) {
        InitHistsFromArr(trigAr);
        if (isShown) Register(fServer);
        SetDir(fDir);
    }
    for (Int_t iTrig = 0; iTrig < trigAr->size(); ++iTrig) {
        TClonesArray *a = (*trigAr)[iTrig];
        TClass *cl = trigAr->at(iTrig)->GetClass();
        if (cl == BmnTrigWaveDigit::Class()) {
            for (Int_t digIndex = 0; digIndex < a->GetEntriesFast(); digIndex++) {
                BmnTrigWaveDigit *tw = (BmnTrigWaveDigit*) a->At(digIndex);
                hists[iTrig][0]->Fill(tw->GetTime());
                hists[iTrig][1]->Fill(tw->GetPeak());
                if (tw->GetTime() > -900) {
                    histsAux[iTrig][0]->Fill(tw->GetTime());
                    histsAux[iTrig][1]->Fill(tw->GetPeak());
                }
                hTQDCTimes->Fill(a->GetName(), tw->GetTime(), 1);
                hTQDCAmps->Fill(a->GetName(), tw->GetPeak(), 1);
                //                printf("%s %5.2f %5d\n",a->GetName(),tw->GetTime(),tw->GetPeak());
            }
            continue;
        }
        if (cl == BmnTrigDigit::Class()) {
            for (Int_t digIndex = 0; digIndex < a->GetEntriesFast(); digIndex++) {
                BmnTrigDigit *td = (BmnTrigDigit*) a->At(digIndex);
                hists[iTrig][0]->Fill(td->GetTime());
                hists[iTrig][1]->Fill(td->GetAmp());
                hTDCTimes->Fill(a->GetName(), td->GetTime(), 1);
                hTDCAmps->Fill(a->GetName(), td->GetAmp(), 1);
                //                printf("%s %5.2f %5.2f\n",a->GetName(),td->GetTime(),td->GetAmp());
            }
        }
    }
}

BmnStatus BmnHistSrc::SetRefRun(Int_t id) {
    if (refID != id) {
        TString FileName = Form("bmn_run%04d_hist.root", id);
        printf("SetRefRun: %s\n", FileName.Data());
        refRunName = FileName;
        refID = id;
        BmnHist::LoadRefRun(refID, refPath + FileName, fTitle, canPads, Names);
        DrawBoth();
    }

    return kBMNSUCCESS;
}

void BmnHistSrc::ClearRefRun() {
    for (auto pad : canPads) {
        if (pad->ref) delete pad->ref;
        pad->ref = NULL;
    }
    refID = 0;
}

void BmnHistSrc::Reset() {
    for (auto row : hists)
        for (auto el : row)
            if (el)
                el->Reset();
    for (auto row : histsAux)
        for (auto el : row)
            if (el)
                el->Reset();
    if (hTDCTimes) hTDCTimes->Reset();
    if (hTDCAmps) hTDCAmps->Reset();
    if (hTQDCTimes) hTQDCTimes->Reset();
    if (hTQDCAmps) hTQDCAmps->Reset();
}


ClassImp(BmnHistSrc);

