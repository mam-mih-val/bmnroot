/* 
 * File:   BmnHistTrigger.cxx
 * Author: ilnur
 * 
 * Created on November 27, 2016, 4:18 PM
 */

#include "BmnHistTrigger.h"
#include "BmnRawDataDecoder.h"

BmnHistTrigger::BmnHistTrigger(TString title) : BmnHist() {
    fTitle = title;
    fName = title + "_cl";
    fSelectedBDChannel = -1;
    BDEvents = new TClonesArray("BmnTrigDigit");
    TString name;
    name = fTitle + "_BC1_Time";
    histBC1TimeLen = new TH1D(name, name, 300, 0, 1000);
    //                histBC1TimeLen->SetTitleSize(0.08, "XY");
    //                histBC1TimeLen->SetLabelSize(0.08, "XY");
    //                histBC1TimeLen->SetLabelColor(kOrange+5, "XY");
    histBC1TimeLen->GetXaxis()->SetTitle("Time, ns");
    histBC1TimeLen->GetYaxis()->SetTitle("Activation Count");
    name = fTitle + "_BC2_Time";
    histBC2TimeLen = new TH1D(name, name, 300, 0, 1000);
    histBC2TimeLen->GetXaxis()->SetTitle("Time, ns");
    histBC2TimeLen->GetYaxis()->SetTitle("Activation Count");
    name = fTitle + "_FD_Time";
    histFDTimeLen = new TH1D(name, name, 300, 0, 1000);
    histFDTimeLen->GetXaxis()->SetTitle("Time, ns");
    histFDTimeLen->GetYaxis()->SetTitle("Activation Count");
    name = fTitle + "_T0_Time";
    histSDTimeLen = new TH1D(name, name, 300, 0, 1000);
    histSDTimeLen->GetXaxis()->SetTitle("Time, ns");
    histSDTimeLen->GetYaxis()->SetTitle("Activation Count");
    name = fTitle + "_VETO_Time";
    histVDTimeLen = new TH1D(name, name, 300, 0, 1000);
    histVDTimeLen->GetXaxis()->SetTitle("Time, ns");
    histVDTimeLen->GetYaxis()->SetTitle("Activation Count");
    name = fTitle + "_BD_Channels";
    histBDChannels = new TH1I(name, name, BD_CHANNELS, 0, BD_CHANNELS);
    histBDChannels->GetXaxis()->SetTitle("Channel #");
    histBDChannels->GetYaxis()->SetTitle("Activation Count");
    name = fTitle + "_BD_Simultaneous";
    histBDSimult = new TH1I(name, name, 10, 0, 10);
    histBDSimult->GetXaxis()->SetTitle("Channels #");
    histBDSimult->GetYaxis()->SetTitle("Activation Count");
    name = fTitle + "_BD_Time";
    histBDTime = new TH1I(name, name, 300, 0, 1000);
    histBDTime->GetXaxis()->SetTitle("Time, ns");
    histBDTime->GetYaxis()->SetTitle("Activation Count");
    name = fTitle + "_BD_Specific_Channel";
    histBDSpecific = new TH1D(name, name, 300, 0, 1000);
    histBDSpecific->GetXaxis()->SetTitle("Time, ns");
    histBDSpecific->GetYaxis()->SetTitle("Activation Count");
    TString triggerNames[6] = {"BC1", "T0", "BC2", "VD", "FD", "BD"};
    name = fTitle + "_Counter";
    histTriggers = new TH1I(name, name, 6, 0, 6);
    histTriggers->GetXaxis()->SetTitle("Trigger Name");
    histTriggers->GetYaxis()->SetTitle("Activation Count");
    TAxis* xa = histTriggers->GetXaxis();
    for (Int_t i = 0; i < sizeof (triggerNames) / sizeof (*triggerNames); i++) {
        xa->SetBinLabel(i + 1, triggerNames[i]);
    }
    name = fTitle + "CanvasTimes";
    canTimes = new TCanvas(name, name, PAD_WIDTH * TRIG_ROWS, PAD_HEIGHT * TRIG_COLS);
    canTimes->Divide(TRIG_ROWS, TRIG_COLS);
    canTimesPads.resize(TRIG_ROWS * TRIG_COLS);
    for (Int_t iPad = 0; iPad < TRIG_ROWS * TRIG_COLS; iPad++) {
        PadInfo* p = new PadInfo();
        canTimesPads[iPad] = p;
        canTimes->GetPad(iPad + 1)->SetGrid();
    }
    canTimesPads[0]->current = histTriggers;
    canTimesPads[1]->current = histBDChannels;
    canTimesPads[2]->current = histBDSimult;
    canTimesPads[3]->current = histBC1TimeLen;
    canTimesPads[4]->current = histBC2TimeLen;
    canTimesPads[5]->current = histFDTimeLen;
    canTimesPads[6]->current = histVDTimeLen;
    canTimesPads[7]->current = histSDTimeLen;
    canTimesPads[8]->current = histBDTime;
    for (Int_t iPad = 0; iPad < canTimesPads.size(); iPad++)
        if (canTimesPads[iPad]->current) {
            Names.push_back(canTimesPads[iPad]->current->GetName());
            canTimesPads[iPad]->current->SetTitleSize(0.06, "XY");
            canTimesPads[iPad]->current->SetLabelSize(0.08, "XY");
            TAxis *ax = canTimesPads[iPad]->current->GetYaxis();
            ax->SetTitleColor(kOrange + 10);
            ax->SetTitleOffset(1.8);
            ax->SetTitleFont(62);
            ax = canTimesPads[iPad]->current->GetXaxis();
            ax->SetTitleColor(kOrange + 10);
        }
}

BmnHistTrigger::~BmnHistTrigger() {
    BDEvents->Clear();
    delete BDEvents;
}

void BmnHistTrigger::FillFromDigi(DigiArrays *fDigiArrays) {
    BDEvents->Clear();
    TClonesArray *BC1digits = fDigiArrays->bc1;
    TClonesArray *SDdigits = fDigiArrays->t0;
    TClonesArray *BC2digits = fDigiArrays->bc2;
    TClonesArray *VDdigits = fDigiArrays->veto;
    TClonesArray *FDdigits = fDigiArrays->fd;
    TClonesArray *BDdigits = fDigiArrays->bd;
    for (Int_t digIndex = 0; digIndex < BC1digits->GetEntriesFast(); digIndex++) {
        BmnTrigDigit* td0 = (BmnTrigDigit*) BC1digits->At(digIndex);
        histBC1TimeLen->Fill(td0->GetTime());
        histTriggers->Fill("BC1", 1);
    }
    for (Int_t digIndex = 0; digIndex < SDdigits->GetEntriesFast(); digIndex++) {
        BmnTrigDigit* td1 = (BmnTrigDigit*) SDdigits->At(digIndex);
        histSDTimeLen->Fill(td1->GetTime());
        histTriggers->Fill("T0", 1);
    }
    for (Int_t digIndex = 0; digIndex < BC2digits->GetEntriesFast(); digIndex++) {
        BmnTrigDigit* td2 = (BmnTrigDigit*) BC2digits->At(digIndex);
        histBC2TimeLen->Fill(td2->GetTime());
        histTriggers->Fill("BC2", 1);
    }
    for (Int_t digIndex = 0; digIndex < VDdigits->GetEntriesFast(); digIndex++) {
        BmnTrigDigit* tv = (BmnTrigDigit*) VDdigits->At(digIndex);
        histVDTimeLen->Fill(tv->GetTime());
        histTriggers->Fill("VD", 1);
    }
    for (Int_t digIndex = 0; digIndex < FDdigits->GetEntriesFast(); digIndex++) {
        BmnTrigDigit* td3 = (BmnTrigDigit*) FDdigits->At(digIndex);
        histFDTimeLen->Fill(td3->GetTime());
        histTriggers->Fill("FD", 1);
    }
    Int_t bdCount = 0;
    for (Int_t digIndex = 0; digIndex < BDdigits->GetEntriesFast(); digIndex++) {
        bdCount++;
        BmnTrigDigit* bd = (BmnTrigDigit*) BDdigits->At(digIndex);
        histBDChannels->Fill(bd->GetMod());
        histBDTime->Fill(bd->GetTime());
        histTriggers->Fill("BD", 1);
        if (bd->GetMod() == fSelectedBDChannel){
            histBDSpecific->Fill(bd->GetAmp());
        new ((*BDEvents)[BDEvents->GetEntriesFast()]) BmnTrigDigit(bd->GetMod(), bd->GetTime(), bd->GetAmp());
//            frecoTree->Fill();
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

void BmnHistTrigger::Register(THttpServer *serv) {
    fServer = serv;
    fServer->Register("/", this);
    TString path = "/" + fTitle + "/";
    fServer->Register(path, canTimes);
    //    fServer->Register(path, histBC1TimeLen);
    //    fServer->Register(path, histBC2TimeLen);
    //    fServer->Register(path, histFDTimeLen);
    //    fServer->Register(path, histVDTimeLen);
    //    fServer->Register(path, histSDTimeLen);
    //    fServer->Register(path, histTriggers);
    //    fServer->Register(path, histBDChannels);
    //    fServer->Register(path, histBDSimult);
    //    fServer->Register(path, histBDTime);
    //    fServer->Register(path, histBDSpecific);
    TString examples = TString("[") +
            histBC1TimeLen->GetTitle() + TString(",") +
            histBC2TimeLen->GetTitle() + TString(",") +
            histFDTimeLen->GetTitle() + TString(",") +
            histVDTimeLen->GetTitle() + TString(",") +
            histSDTimeLen->GetTitle() + TString(",") +
            histBDSpecific->GetTitle() + TString("]");
    fServer->SetItemField(path.Data(), "_monitoring", "2000");
    fServer->SetItemField(path.Data(), "_layout", "grid3x2");
    fServer->SetItemField(path, "_drawitem", examples);

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
    if (outFile != NULL)
        fDir = outFile->mkdir(fTitle + "_hists");
    //        dir->cd();
    histBC1TimeLen->SetDirectory(fDir);
    histBC2TimeLen->SetDirectory(fDir);
    histFDTimeLen->SetDirectory(fDir);
    histSDTimeLen->SetDirectory(fDir);
    histVDTimeLen->SetDirectory(fDir);
    histBDChannels->SetDirectory(fDir);
    histBDSimult->SetDirectory(fDir);
    histBDTime->SetDirectory(fDir);
    histBDSpecific->SetDirectory(fDir);
    histTriggers->SetDirectory(fDir);
    if (BDEvents != NULL)
        delete BDEvents;
    BDEvents = new TClonesArray("BmnTrigDigit");
    if (recoTree != NULL)
        recoTree->Branch(fTitle + "_BD", &BDEvents);
}

void BmnHistTrigger::Reset() {
    histBC1TimeLen->Reset();
    histBC2TimeLen->Reset();
    histSDTimeLen->Reset();
    histVDTimeLen->Reset();
    histFDTimeLen->Reset();
    histBDChannels->Reset();
    histBDSimult->Reset();
    histBDTime->Reset();
    histBDSpecific->Reset();
    histTriggers->Reset();
    if (BDEvents != NULL)
        BDEvents->Clear();
}

void BmnHistTrigger::DrawBoth() {
    BmnHist::DrawRef(canTimes, &canTimesPads);
}

BmnStatus BmnHistTrigger::SetRefRun(Int_t id) {
    if (refID != id) {
        TString FileName = Form("bmn_run%04d_hist.root", id);
        printf("SetRefRun: %s\n", FileName.Data());
        refRunName = FileName;
        refID = id;
        BmnHist::LoadRefRun(refID, refPath + FileName, fTitle, canTimesPads, Names);
        DrawBoth();
    }
}

ClassImp(BmnHistTrigger);