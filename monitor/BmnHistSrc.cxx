#include "BmnHistSrc.h"

BmnHistSrc::BmnHistSrc(TString title, TString path, Int_t periodID, BmnSetup setup) : BmnHist(periodID, setup),
hTDCTimes(nullptr){
    refPath = path;
    fTitle = title;
    fName = title + "_cl";
    canvas = NULL;
    //    TString PeriodSetupExt = Form("%d%s.txt", fPeriodID, ((fSetup == kBMNSETUP) ? "" : "_SRC"));
    //    TString MapFileName = TString("Trig_map_Run") + PeriodSetupExt;
    //    BmnTrigRaw2Digit *fTrigMapper = new BmnTrigRaw2Digit("0.txt", MapFileName);
    //    for (auto &map : *fTrigMapper->GetMap()) {
    //        TString trName = map.name;
    //        if (!strcmp(trName.Data(), "TDC"))
    //            tdcNames.push_back(trName);
    //    }
    //    TString name = fTitle + "_" + "TDC_mod_amp";
    //    Int_t trigCount = tdcNames.size();
    //    Double_t max_amp = 50;
    //    hTDCTimes = new TH2F(name, name, trigCount, 0, trigCount, 100, 0, max_amp);
    //    hTDCTimes->GetXaxis()->SetTitle("TDC #");
    //    hTDCTimes->GetYaxis()->SetTitle("Time, ns");
    //    hTDCTimes->GetXaxis()->SetTitleOffset(0.4);
    //    hTDCTimes->GetXaxis()->SetTitleColor(kOrange + 10);
    //    hTDCTimes->GetYaxis()->SetTitleOffset(0.6);
    //    hTDCTimes->GetYaxis()->SetTitleColor(kOrange + 10);
    //    hTDCTimes->SetDirectory(0);
    //    delete fTrigMapper;
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
    const Int_t MaxTimeTDC = 1000;
    const Int_t MaxAmpTDC = 100;
    const Int_t MaxTimeTQDC = 1000;
    const Int_t MaxAmpTQDC = 1000;
    const Int_t rows4Spectrum = 2;
    Int_t arLen = trigAr->size();
    TString name;
    fCols = SRC_COLS;
    fRows = arLen; // / fSrcCols;
    for (Int_t i = 0; i < fRows; ++i) {
        TClonesArray * ar = trigAr->at(i);
        if (ar->GetClass() == BmnTrigWaveDigit::Class())
            tqdcNames.push_back(TString(ar->GetName()));
        else
            tdcNames.push_back(TString(ar->GetName()));
    }
    // 2 dimensional time

    name = fTitle + "_Times_by_Trigger";
    hTDCTimes = new TH2F(name, name, tdcNames.size(), 0, tdcNames.size(), 300, 0, MaxTimeTDC);
    hTDCTimes->SetDirectory(fDir);
    hTDCTimes->GetXaxis()->SetTitle("Trigger Name");
    hTDCTimes->GetYaxis()->SetTitle("Time, ns");
    TAxis* xaTrigTimes = hTDCTimes->GetXaxis();
    for (Int_t i = 0; i < tdcNames.size(); ++i) {
        xaTrigTimes->SetBinLabel(i + 1, tdcNames[i]);
    }
    name = fTitle + "CanvasTimesByChannel";
    can2d = new TCanvas(name, name, PAD_WIDTH * fCols, PAD_HEIGHT * rows4Spectrum);
    can2d->Divide(fCols, rows4Spectrum);
    can2dPads.resize(fCols * rows4Spectrum);
    for (Int_t iPad = 0; iPad < rows4Spectrum * fCols; iPad++) {
        PadInfo* p = new PadInfo();
        p->opt = "colz ";
        can2dPads[iPad] = p;
        can2d->GetPad(iPad + 1)->SetGrid();
    }
    can2dPads[0]->current = hTDCTimes;
    can2dPads[1]->current = nullptr;
    can2dPads[2]->current = nullptr;
    can2dPads[3]->current = nullptr;
    for (PadInfo *pad : can2dPads) {
        TH1* h = pad->current;
        if (h) {
            h->SetTitleSize(0.06, "XY");
            h->SetLabelSize(0.08, "XY");
            TAxis *ax = h->GetYaxis();
            ax->SetTitleColor(kOrange + 10);
            ax->SetTitleOffset(1.1);
            ax->SetTitleFont(62);
            ax = h->GetXaxis();
            ax->SetTitleColor(kOrange + 10);
        }
    }

    // 1 dimensional
    hists.resize(fRows);
    for (Int_t i = 0; i < hists.size(); i++)
        hists[i].resize(fCols);

    for (Int_t iRow = 0; iRow < fRows; iRow++) { // 0 column - Time histograms from TDC by TQDC
        TClonesArray * ar = trigAr->at(iRow);
        Int_t maxTime =
                (ar->GetClass() == BmnTrigWaveDigit::Class()) ? MaxTimeTQDC : MaxTimeTDC;
        name = fTitle + "_" + ar->GetName() + "_Leading_Time";
        TH1F *h = new TH1F(name, name, 1000, 0, maxTime);
        h->SetTitleSize(0.06, "XY");
        h->SetLabelSize(0.08, "XY");
        h->GetXaxis()->SetTitle("Time, ns");
        h->GetXaxis()->SetTitleOffset(0.5);
        h->GetXaxis()->SetTitleColor(kOrange + 10);
        h->GetYaxis()->SetTitle("Activation Count");
        h->GetYaxis()->SetTitleOffset(0.5);
        h->GetYaxis()->SetTitleColor(kOrange + 10);
        hists[iRow][0] = h;
    }
    for (Int_t iRow = 0; iRow < fRows; iRow++) { // 1 column - Amplitude(TQDC -waveform ampl.,  TDC - sig. width)
        TClonesArray * ar = trigAr->at(iRow);
        Int_t maxAmp =
                (ar->GetClass() == BmnTrigWaveDigit::Class()) ? MaxAmpTQDC : MaxAmpTDC;
        name = fTitle + "_" + ar->GetName() + "_Amplitude";
        TH1F *h = new TH1F(name, name, 500, 0, maxAmp);
        h->SetTitleSize(0.06, "XY");
        h->SetLabelSize(0.08, "XY");
        h->GetXaxis()->SetTitle("Time, ns");
        h->GetXaxis()->SetTitleOffset(0.5);
        h->GetXaxis()->SetTitleColor(kOrange + 10);
        h->GetYaxis()->SetTitle("Activation Count");
        h->GetYaxis()->SetTitleOffset(0.5);
        h->GetYaxis()->SetTitleColor(kOrange + 10);
        hists[iRow][1] = h;
    }
    //    for (Int_t iRow = 0; iRow < fSrcRows; iRow++) { // 2 column - sampling summed
    //        name = fTitle + "_" + trigNames[iRow].Data() + "_QDC";
    //        TH1F *h = new TH1F(name, name, ADC_SAMPLING_LIMIT * 2 / 100, -ADC_SAMPLING_LIMIT, ADC_SAMPLING_LIMIT);
    //        h->SetTitleSize(0.06, "XY");
    //        h->SetLabelSize(0.08, "XY");
    //        h->GetXaxis()->SetTitle("QDC Channel, ");
    //        h->GetXaxis()->SetTitleColor(kOrange + 10);
    //        h->GetYaxis()->SetTitle("Activation Count");
    //        h->GetYaxis()->SetTitleOffset(1.1);
    //        h->GetYaxis()->SetTitleColor(kOrange + 10);
    //        hists[iRow][2] = h;
    //    }
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
    if (hTDCTimes) hTDCTimes->SetDirectory(fDir);
}

void BmnHistSrc::DrawBoth() {
    BmnHist::DrawRef(canvas, &canPads);
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
            }
            continue;
        }
        if (cl == BmnTrigDigit::Class()) {
            for (Int_t digIndex = 0; digIndex < a->GetEntriesFast(); digIndex++) {
                BmnTrigDigit *td = (BmnTrigDigit*) a->At(digIndex);
                hists[iTrig][0]->Fill(td->GetTime());
                hists[iTrig][1]->Fill(td->GetAmp());
                hTDCTimes->Fill(a->GetName(), td->GetTime(), 1);
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
    if (hTDCTimes) hTDCTimes->Reset();
}


ClassImp(BmnHistSrc);

