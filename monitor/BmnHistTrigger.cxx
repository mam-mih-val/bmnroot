/* 
 * File:   BmnHistTrigger.cxx
 * Author: ilnur
 * 
 * Created on November 27, 2016, 4:18 PM
 */

#include "BmnHistTrigger.h"
#include "BmnRawDataDecoder.h"

BmnHistTrigger::BmnHistTrigger(TString title, TString path) : BmnHist() {
    fTitle = title;
    fName = title + "_cl";
    fSelectedBDChannel = -1;
    BDEvents = new TClonesArray("BmnTrigDigit");
    can2d = NULL;
    canTimes = NULL;
    histBDTimeByChannel = NULL;
    histTrigTimeByChannel = NULL;
    histBDChannels = NULL;
    histBDSimult = NULL;
    histBDSpecific = NULL;
    histTriggers = NULL;
}

BmnHistTrigger::~BmnHistTrigger() {
    BDEvents->Clear();
    delete BDEvents;
}

void BmnHistTrigger::InitHistsFromArr(vector<TClonesArray*> *trigAr) {
    TString name;
    fCols = TRIG_COLS;
    const Int_t rows4Barrel = 1;
    const Int_t rows4Summary = 1;
    fRows = trigAr->size() / fCols + rows4Barrel + 1;
    Int_t trigCount = trigAr->size();
    for (Int_t i = 0; i < trigCount; ++i) {
        trigNames.push_back(TString(trigAr->at(i)->GetName()));
        name = fTitle + "_" + trigNames.back() + "_Time";
        TH1I* h = new TH1I(name, name, 300, 0, 1000);
        hists.push_back(h);
    }
    name = fTitle + "CanvasTimesByChannel";
    can2d = new TCanvas(name, name, PAD_WIDTH * fCols, PAD_HEIGHT * rows4Barrel);
    can2d->Divide(fCols, rows4Summary);
    can2dPads.resize(fCols * rows4Summary);
    for (Int_t iPad = 0; iPad < rows4Summary * fCols; iPad++) {
        PadInfo* p = new PadInfo();
        can2dPads[iPad] = p;
        can2d->GetPad(iPad + 1)->SetGrid();
    }
    name = fTitle + "CanvasTimes";
    canTimes = new TCanvas(name, name, PAD_WIDTH * fCols, PAD_HEIGHT * fRows);
    canTimes->Divide(fCols, fRows);
    canTimesPads.resize(fCols * fRows);
    histNames.resize(fCols * fRows);
    for (Int_t iPad = 0; iPad < fRows * fCols; iPad++) {
        PadInfo* p = new PadInfo();
        canTimesPads[iPad] = p;
        canTimes->GetPad(iPad + 1)->SetGrid();
    }
    name = fTitle + "_BD_Channels";
    histBDChannels = new TH1I(name, name, BD_CHANNELS, 0, BD_CHANNELS);
    histBDChannels->GetXaxis()->SetTitle("Channel #");
    histBDChannels->GetYaxis()->SetTitle("Activation Count");
    name = fTitle + "_BD_Time_by_Channels";
    histBDTimeByChannel = new TH2I(name, name, BD_CHANNELS, 0, BD_CHANNELS, 300, 0, 1000);
    histBDTimeByChannel->GetXaxis()->SetTitle("Channel #");
    histBDTimeByChannel->GetYaxis()->SetTitle("Time, ns");
    name = fTitle + "_BD_Simultaneous";
    histBDSimult = new TH1I(name, name, 10, 0, 10);
    histBDSimult->GetXaxis()->SetTitle("Channels #");
    histBDSimult->GetYaxis()->SetTitle("Activation Count");
    //    name = fTitle + "_BD_Specific_Channel";
    //    histBDSpecific = new TH1I(name, name, 300, 0, 1000);
    //    histBDSpecific->GetXaxis()->SetTitle("Time, ns");
    //    histBDSpecific->GetYaxis()->SetTitle("Activation Count");
    name = fTitle + "_Counter";
    histTriggers = new TH1I(name, name, trigCount, 0, trigCount);
    histTriggers->GetXaxis()->SetTitle("Trigger Name");
    histTriggers->GetYaxis()->SetTitle("Activation Count");
    TAxis* xaTriggers = histTriggers->GetXaxis();
    name = fTitle + "_Times_by_Trigger";
    histTrigTimeByChannel = new TH2I(name, name, trigCount, 0, trigCount, 300, 0, 1000);
    histTrigTimeByChannel->GetXaxis()->SetTitle("Trigger Name");
    histTrigTimeByChannel->GetYaxis()->SetTitle("Time, ns");
    TAxis* xaTrigTimes = histTrigTimeByChannel->GetXaxis();
    for (Int_t i = 0; i < trigCount; ++i) {
        xaTriggers->SetBinLabel(i + 1, trigNames[i]);
        xaTrigTimes->SetBinLabel(i + 1, trigNames[i]);
    }
    canTimesPads[0]->current = histTriggers;
    canTimesPads[1]->current = histBDChannels;
    canTimesPads[2]->current = histBDSimult;
    can2dPads[0]->current = histTrigTimeByChannel;
    can2dPads[0]->opt = "colz";
    can2dPads[1]->current = histBDTimeByChannel;
    can2dPads[1]->opt = "colz";
    for (Int_t i = 0; i < trigCount; ++i) {
        canTimesPads[i + rows4Barrel * fCols]->current = hists[i];
        hists[i]->GetYaxis()->SetTitle("Activation Count");
        hists[i]->GetXaxis()->SetTitle("Time, ns");
    }
    for (Int_t iPad = 0; iPad < canTimesPads.size(); ++iPad) {
        PadInfo* pad = canTimesPads[iPad];
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
            histNames[iPad] = h->GetName();
        }
    }
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
}

void BmnHistTrigger::FillFromDigi(DigiArrays *fDigiArrays) {
    vector<TClonesArray*> *trigAr = fDigiArrays->trigAr;
    if (trigAr == NULL)
        return;
    if (trigAr->size() == 0)
        return;
    if (canTimes == NULL) {
        InitHistsFromArr(trigAr);
        if (isShown) Register(fServer);
        SetDir(fDir);
    }
    Int_t bdCount = 0;
    BDEvents->Clear();
    for (auto &ar : *trigAr) {
        const char* arName = ar->GetName();
        Bool_t isBarrel = !strcmp(arName, "BD");
        for (Int_t i = 0; i < trigNames.size(); ++i) {
            if (!strcmp(arName, trigNames[i].Data())) {
                for (Int_t digIndex = 0; digIndex < ar->GetEntriesFast(); digIndex++) {
                    BmnTrigDigit* td = (BmnTrigDigit*) ar->At(digIndex);
                    histTriggers->Fill(arName, 1);
                    histTrigTimeByChannel->Fill(arName, td->GetTime(), 1);
                    hists[i]->Fill(td->GetTime());
                    if (isBarrel) {
                        bdCount++;
                        histBDChannels->Fill(td->GetMod());
                        histBDTimeByChannel->Fill(td->GetMod(), td->GetTime(), 1);
                        //                if (bd->GetMod() == fSelectedBDChannel) {
                        //                    histBDSpecific->Fill(bd->GetAmp());
                        //                    new ((*BDEvents)[BDEvents->GetEntriesFast()]) BmnTrigDigit(bd->GetMod(), bd->GetTime(), bd->GetAmp());
                        //                    //            frecoTree->Fill();
                        //                }
                    }
                }
                break;
            }
        }
    }
    histBDSimult->Fill(bdCount);
}

void BmnHistTrigger::SetBDChannel(Int_t iSelChannel) {
    TString title;
    if (iSelChannel > (histBDSpecific->GetNbinsX() - 1)) {
        printf("Wrong channel!\n");
        return;
    }
    printf("Set channel: %d\n", fSelectedBDChannel);
    fSelectedBDChannel = iSelChannel;
    TString command;
    if (fSelectedBDChannel >= 0)
        command = Form("fMod == %d", fSelectedBDChannel);
    if (iSelChannel == -1)
        title = Form("BD for All Channels");

    else
        title = "BD Time Length For: " + command;
    histBDSpecific->SetTitle(title);
    histBDSpecific->Reset();
    TString direction = "fTime>>" + TString(histBDSpecific->GetName());
    frecoTree->Draw(direction, command, "");

}

void BmnHistTrigger::Register(THttpServer * serv) {
    isShown = kTRUE;
    fServer = serv;
    if (canTimes == NULL)

        return;
    fServer->Register("/", this);
    TString path = "/" + fTitle + "/";
    fServer->Register(path, canTimes);
    fServer->Register(path, can2d);

    TString cmd = "/" + fName + "/->SetRefRun(%arg1%)";
    TString cmdTitle = path + "SetRefRun";
    fServer->RegisterCommand(cmdTitle.Data(), cmd.Data(), "button;");
    fServer->Restrict(cmdTitle.Data(), "visible=shift");
    fServer->Restrict(cmdTitle.Data(), "allow=shift");
    fServer->Restrict(cmdTitle.Data(), "deny=guest");
    cmdTitle = path + "ChangeBDChannel";
    fServer->RegisterCommand(cmdTitle.Data(), "/" + fName + "/->SetBDChannel(%arg1%)", "button;");
    fServer->Restrict(cmdTitle.Data(), "visible=shift");
    fServer->Restrict(cmdTitle.Data(), "allow=shift");
    fServer->Restrict(cmdTitle.Data(), "hidden=guest");
    fServer->Restrict(cmdTitle.Data(), "deny=guest");
    cmdTitle = path + "Reset";
    fServer->RegisterCommand(cmdTitle.Data(), "/" + fName + "/->Reset()", "button;");
    fServer->Restrict(cmdTitle.Data(), "visible=shift");
    fServer->Restrict(cmdTitle.Data(), "allow=shift");
    fServer->Restrict(cmdTitle.Data(), "deny=guest");
}

void BmnHistTrigger::SetDir(TFile *outFile = NULL, TTree *recoTree = NULL) {
    frecoTree = recoTree;
    fDir = NULL;
    if (outFile)
        fDir = outFile->mkdir(fTitle + "_hists");
    //        dir->cd();
    if (BDEvents)
        delete BDEvents;
    BDEvents = new TClonesArray("BmnTrigDigit");
    if (recoTree)
        recoTree->Branch(fTitle + "_BD", &BDEvents);
    if (!canTimes)
        return;
    SetDir(fDir);
}

void BmnHistTrigger::SetDir(TDirectory * Dir) {
    for (auto el : hists) {
        if (el)
            el->SetDirectory(fDir);
    }
    histBDChannels->SetDirectory(fDir);
    histBDSimult->SetDirectory(fDir);
    //    histBDSpecific->SetDirectory(fDir);
    histTriggers->SetDirectory(fDir);
    histBDTimeByChannel->SetDirectory(fDir);
    histTrigTimeByChannel->SetDirectory(fDir);
}

void BmnHistTrigger::Reset() {
    for (auto el : hists) {
        if (el)
            el->Reset();
    }
    if (histBDChannels) histBDChannels->Reset();
    if (histBDSimult) histBDSimult->Reset();
    //    if (histBDSpecific) histBDSpecific->Reset();
    if (histTriggers) histTriggers->Reset();
    if (histBDTimeByChannel) histBDTimeByChannel->Reset();
    if (histTrigTimeByChannel) histTrigTimeByChannel->Reset();

    if (BDEvents != NULL)
        BDEvents->Clear();
}

void BmnHistTrigger::DrawBoth() {
    if (canTimes) BmnHist::DrawRef(canTimes, &canTimesPads);
    if (can2d) BmnHist::DrawRef(can2d, &can2dPads);
}

BmnStatus BmnHistTrigger::SetRefRun(Int_t id) {
    if (refID != id) {
        TString FileName = Form("bmn_run%04d_hist.root", id);
        printf("SetRefRun: %s\n", FileName.Data());
        refRunName = FileName;
        refID = id;
        BmnHist::LoadRefRun(refID, refPath + FileName, fTitle, canTimesPads, histNames);
        DrawBoth();
    }
    return kBMNSUCCESS;
}

void BmnHistTrigger::ClearRefRun() {
    for (auto pad : canTimesPads){
        if (pad->ref) delete pad->ref;
        pad->ref = NULL;
    }
    refID = 0;
}

ClassImp(BmnHistTrigger);
