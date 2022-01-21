#include "BmnHistSrc.h"

BmnHistSrc::BmnHistSrc(TString title, TString path) : BmnHist() {
    refPath = path;
    fTitle = title;
    fName = title + "_cl";
    canvas = NULL;
}

BmnHistSrc::~BmnHistSrc() {
}

void BmnHistSrc::InitHistsFromArr(vector<TClonesArray*> *trigAr) {
    Int_t arLen = trigAr->size();
    fSrcCols = SRC_COLS;
    fSrcRows = arLen;// / fSrcCols;
    for (Int_t i = 0; i < fSrcRows; ++i){
        trigNames.push_back(TString(trigAr->at(i)->GetName()));
    }
    TString name;

    hists.resize(fSrcRows);
    for (Int_t i = 0; i < hists.size(); i++)
        hists[i].resize(fSrcCols);

    for (Int_t iRow = 0; iRow < fSrcRows; iRow++) { // 0 column - Time histograms from TDC by TQDC
        name = fTitle + "_" + trigNames[iRow].Data() + "_Leading_Time";
        TH1F *h = new TH1F(name, name, 1000, 0, 2000);
        h->SetTitleSize(0.06, "XY");
        h->SetLabelSize(0.08, "XY");
        h->GetXaxis()->SetTitle("Time, ns");
        h->GetXaxis()->SetTitleOffset(0.4);
        h->GetXaxis()->SetTitleColor(kOrange + 10);
        h->GetYaxis()->SetTitle("Activation Count");
        h->GetYaxis()->SetTitleOffset(0.6);
        h->GetYaxis()->SetTitleColor(kOrange + 10);
        hists[iRow][0] = h;
    }
    for (Int_t iRow = 0; iRow < fSrcRows; iRow++) { // 1 column - Amplitude(TQDC -waveform ampl.,  TDC - sig. width)
        name = fTitle + "_" + trigNames[iRow].Data() + "_Amplitude";
        TH1F *h = new TH1F(name, name, 1000, 0, 2000);
        h->SetTitleSize(0.06, "XY");
        h->SetLabelSize(0.08, "XY");
        h->GetXaxis()->SetTitle("Time, ns");
        h->GetXaxis()->SetTitleOffset(0.4);
        h->GetXaxis()->SetTitleColor(kOrange + 10);
        h->GetYaxis()->SetTitle("Activation Count");
        h->GetYaxis()->SetTitleOffset(0.6);
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
    canvas = new TCanvas(name, name, PAD_WIDTH * fSrcCols, PAD_HEIGHT * fSrcRows);
    canvas->Divide(fSrcCols, fSrcRows,0.0001,0.0001);
            printf("rows %d cols %d  \n",  fSrcRows, fSrcCols);
    canPads.resize(fSrcCols * fSrcRows);
    Names.resize(fSrcCols * fSrcRows);
    for (Int_t iRow = 0; iRow < fSrcRows; iRow++)
        for (Int_t iCol = 0; iCol < fSrcCols; iCol++) {
            PadInfo *p = new PadInfo();
            p->current = hists[iRow][iCol];
            Int_t iPad = iRow * fSrcCols + iCol;
//            printf("ipad %d irow %d icol %d  %08X %08X\n", iPad, iRow, iCol, canvas, canvas->GetPad(iPad + 1));
            canPads[iPad] = p;
            canvas->GetPad(iPad + 1)->SetGrid();
            if (p->current){
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
    fDir = NULL;
    if (outFile != NULL)
        fDir = outFile->mkdir(fTitle + "_hists");
    if (canvas == NULL)
        return;
    SetDir(fDir);
}

void BmnHistSrc::SetDir(TDirectory* Dir) {
    for (auto row : hists)
        for (auto el : row)
            if (el)
                el->SetDirectory(fDir);
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
                hists[iTrig][0]->Fill(td->GetAmp());
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
}


ClassImp(BmnHistSrc);

