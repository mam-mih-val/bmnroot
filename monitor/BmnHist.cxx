#include "BmnHist.h"

BmnHist::BmnHist(Int_t PeriodID, BmnSetup setup) {
    refFile = NULL;
    frecoTree = NULL;
    fDir = NULL;
    fPeriodID = PeriodID;
    fSetup = setup;
    isShown = kFALSE;
}

BmnHist::~BmnHist() {
}

void BmnHist::DrawRef(TCanvas *canGemStrip, vector<PadInfo*> *canGemStripPads) {
    for (Int_t iPad = 0; iPad < canGemStripPads->size(); iPad++) {
        TVirtualPad *pad = canGemStrip->cd(iPad + 1);
        PadInfo* info = canGemStripPads->at(iPad);
        DrawPad(pad, info);
    }
    canGemStrip->Update();
    canGemStrip->Modified();
}

void BmnHist::DrawPad(TVirtualPad *pad, PadInfo *info) {
    if ((!pad) || (!info)) return;
    pad->Clear();
    Double_t maxy;
    Double_t k = 1;
    if (info->current) {
        //        maxy = info->current->GetMaximum(); //
        maxy = info->current->GetBinContent(info->current->GetMaximumBin());
        info->current->Draw(info->opt.Data());
        if (info->ref != NULL) {
            k = (info->ref->Integral() > 0) ?
                    info->current->Integral() / info->ref->Integral() : 1.0;
            if (info->ref->Integral() > 0)
                info->ref->DrawNormalized("same hist", info->current->Integral());
            if (k == 0) k = 1;
            k = k * info->ref->GetMaximum(); //GetBinContent(info->ref->GetMaximumBin());
            if (maxy < k)
                maxy = k;
        }
        //            info->current->GetYaxis()->SetRange(0, maxy * 1.4);
        info->current->SetMaximum(maxy * 1.2);
    }
    pad->Update();
    pad->Modified();
}

BmnStatus BmnHist::LoadRefRun(Int_t refID, TString FullName, TString fTitle, vector<PadInfo*> canPads, vector<TString> Names) {
    printf("Loading ref histos\n");
    TFile *refFile = new TFile(FullName, "read");
    if (refFile->IsOpen() == false) {
        printf("Cannot open file %s !\n", FullName.Data());
        return kBMNERROR;
    }
    TString refName = Form("ref%06d_", refID);
    TString name;
    for (Int_t iPad = 0; iPad < Names.size(); iPad++) {
        name = Names[iPad];
        if (name.Length() == 0)
            continue;
        delete canPads[iPad]->ref;
        canPads[iPad]->ref = NULL;
        TH1* tempH = NULL;
        tempH = static_cast<TH1*> (refFile->Get(refName + fTitle + "_hists/" + refName + name));
        if (tempH == NULL) {
            tempH = static_cast<TH1*> (refFile->Get(fTitle + "_hists/" + name));
        }
        if (tempH == NULL) {
            printf("Cannot load %s !\n", name.Data());
            continue;
        }
        canPads[iPad]->ref = static_cast<TH1*> (tempH->Clone(refName + name));
        canPads[iPad]->ref->SetLineColor(kRed);
        canPads[iPad]->ref->SetDirectory(0);
        printf("Loaded %s \n", canPads[iPad]->ref->GetName());
    }
    delete refFile;
    refFile = NULL;
    return kBMNSUCCESS;
}

BmnStatus BmnHist::DrawPadTree(BmnPadBranch* br) {
    if (!br)
        return kBMNERROR;
    if (PadInfo * info = br->GetPadInfo()) {
        TVirtualPad* pad = info->padPtr;
        DrawPad(pad, info);
    } else {
        for (auto &b : br->GetBranchesRef()) {
            DrawPadTree(b);
        }
    }
    return kBMNSUCCESS;
}


ClassImp(BmnHist);
