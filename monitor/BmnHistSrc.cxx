#include <algorithm>
#include <numeric>

#include "BmnHistSrc.h"

BmnHistSrc::BmnHistSrc(TString title, TString path) : BmnHist() {
    refPath = path;
    fTitle = title;
    fName = title + "_cl";
    TString name;
    
    hists.resize(SRC_ROWS);
    for (Int_t i = 0; i < hists.size(); i++)
        hists[i].resize(SRC_COLS);

    for (Int_t iRow = 0; iRow < SRC_ROWS; iRow++) { // 0 column - Time histograms from TDC
        name = fTitle + "_" + trigNames[iRow].Data() + "_Leading_Time";
        TH1F *h = new TH1F(name, name, 700, 0, 700);
        h->SetTitleSize(0.06, "XY");
        h->SetLabelSize(0.08, "XY");
        h->GetXaxis()->SetTitle("Time, ns");
        h->GetXaxis()->SetTitleColor(kOrange + 10);
        h->GetYaxis()->SetTitle("Activation Count");
        h->GetYaxis()->SetTitleColor(kOrange + 10);
        hists[iRow][0] = h;
    }
    for (Int_t iRow = 0; iRow < SRC_ROWS; iRow++) { // 1 column - sampling summed
        name = fTitle + "_" + trigNames[iRow].Data() + "_Sampling";
        TH1F *h = new TH1F(name, name, 255, 0, 255);
        h->SetTitleSize(0.06, "XY");
        h->SetLabelSize(0.08, "XY");
        h->GetXaxis()->SetTitle("Amplitude, ");
        h->GetXaxis()->SetTitleColor(kOrange + 10);
        h->GetYaxis()->SetTitle("Activation Count");
        h->GetYaxis()->SetTitleColor(kOrange + 10);
        hists[iRow][1] = h;
    }

    // Create canvas
    name = fTitle + "Canvas";
    canvas = new TCanvas(name, name, PAD_WIDTH * SRC_COLS, PAD_HEIGHT * SRC_ROWS);
    canvas->Divide(SRC_COLS, SRC_ROWS);
    canPads.resize(SRC_COLS * SRC_ROWS);
    Names.resize(SRC_COLS * SRC_ROWS);
    for (Int_t iRow = 0; iRow < SRC_ROWS; iRow++)
        for (Int_t iCol = 0; iCol < SRC_COLS; iCol++) {
            PadInfo *p = new PadInfo();
            p->current = hists[iRow][iCol];
            Int_t iPad = iRow * SRC_COLS + iCol;
            canPads[iPad] = p;
            canvas->GetPad(iPad + 1)->SetGrid();
            if (p->current)
                Names[iPad] = p->current->GetName();
        }
}

BmnHistSrc::~BmnHistSrc() {
}

void BmnHistSrc::Register(THttpServer * serv) {
    fServer = serv;
    fServer->Register("/", this);
    TString path = "/" + fTitle + "/";
    fServer->Register(path, canvas);
    //    for (auto row : histGemStrip)
    //        for (auto col : row)
    //            for (auto el : col)
    //                fServer->Register(path, el);
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
    for (auto row : hists)
        for (auto el : row)
            if (el)
                el->SetDirectory(fDir);
}

void BmnHistSrc::DrawBoth() {
    BmnHist::DrawRef(canvas, &canPads);
}

void BmnHistSrc::FillFromDigi(DigiArrays *fDigiArrays) {
//    vector<TClonesArray*> *trigAr = new vector<TClonesArray*>(fDigiArrays->trigAr, fDigiArrays->trigAr + fDigiArrays->trigLen);
    vector<TClonesArray*> *trigAr = fDigiArrays->trigAr;
    for (UInt_t iTrig = 0; iTrig < trigAr->size(); iTrig++) {
        for (Int_t digIndex = 0; digIndex < (*trigAr)[iTrig]->GetEntriesFast(); digIndex++) {
            BmnTrigWaveDigit *tw = (BmnTrigWaveDigit*) (*trigAr)[iTrig]->At(digIndex);
//            Short_t module = tw->GetMod();
            Double_t time = tw->GetTime();
            UInt_t nSmpl = tw->GetNSamples();
            Short_t *sampling = tw->GetShortValue();
            hists[iTrig][0]->Fill(time);
            for (UInt_t iSmpl = 0; iSmpl < nSmpl; iSmpl++)
                hists[iTrig][1]->Fill(sampling[iSmpl]);
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
}

void BmnHistSrc::Reset() {
    for (auto row : hists)
        for (auto el : row)
            if (el)
                el->Reset();
}


ClassImp(BmnHistSrc);

