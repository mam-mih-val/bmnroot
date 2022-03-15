/* 
 * File:   BmnHistTrigger.cxx
 * Author: ilnur
 * 
 * Created on November 27, 2016, 4:18 PM
 */

#include "BmnHistTrigger.h"
#include "BmnRawDataDecoder.h"

BmnHistTrigger::BmnHistTrigger(TString title, TString path, Int_t periodID, BmnSetup setup) : BmnHist(periodID,setup) {
    
    fTitle = title;
    fName = title + "_cl";
    fSelectedBDChannel = -1;
    BDEvents = new TClonesArray("BmnTrigDigit");
    can2d = NULL;
    canTimes = NULL;
    histTriggers = NULL;
    histTrigTimeByChannel = NULL;
    histBDTimeByChannel = NULL;
    histBDChannels = NULL;
    histBDSimult = NULL;
    histSiTimeByChannel = NULL;
    histSiChannels = NULL;
    histSiSimult = NULL;
    histCorrSiBD = NULL;
    histSiBSum = NULL;
    TString PeriodSetupExt = Form("%d%s.txt", fPeriodID, ((fSetup == kBMNSETUP) ? "" : "_SRC"));
    TString MapFileName = TString("Trig_map_Run") + PeriodSetupExt;
//    TString("Trig_PlaceMap_Run") + PeriodSetupExt; 
    BmnTrigRaw2Digit *fTrigMapper = new BmnTrigRaw2Digit("0.txt", MapFileName);
    TString name;
    fCols = TRIG_COLS;
    const Int_t rows4Summary = 2;
    const Int_t rows4Spectrum = 2;
    TPRegexp re("TQDC_(\\S+)");
    for (auto &map : *fTrigMapper->GetMap()) {
        TString trName = map.name;
        re.Substitute(trName, "$1");
        //        printf("trname %s mod %d\n", map.name.Data(), map.module);
        if (trName.Contains("BD") || trName.Contains("SI")) {
            Bool_t dupl = kFALSE;
            for (auto &tr : trigNames) {
                if (!strcmp(tr.Data(), trName.Data())) {
                    dupl = kTRUE;
                    break;
                }
            }
            if (dupl)
                continue;
            trigNames.push_back(trName);
            shortNames.push_back(trName);
            name = fTitle + "_" + trName + "_Time";
            TH1I* h = new TH1I(name, name, 300, 0, TRIG_TIME_WIN);
            hists.push_back(h);
        } else {
            name = Form("%s_%d", trName.Data(), map.module);
            trigNames.push_back(name);
            name = fTitle + "_" + name + "_Time";
            TH1I* h2 = new TH1I(name, name, 300, 0, TRIG_TIME_WIN);
            hists.push_back(h2);
            Bool_t dupl = kFALSE;
            for (auto &tr : shortNames) {
                if (!strcmp(tr.Data(), trName.Data())) {
                    dupl = kTRUE;
                    break;
                }
            }
            if (dupl)
                continue;
            shortNames.push_back(trName);

        }
    }
    fRows = hists.size() / fCols + rows4Summary + 1;
    name = fTitle + "CanvasTimesByChannel";
    can2d = new TCanvas(name, name, PAD_WIDTH * fCols, PAD_HEIGHT * rows4Spectrum);
    can2d->Divide(fCols, rows4Spectrum);
    can2dPads.resize(fCols * rows4Spectrum);
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
    histBDTimeByChannel = new TH2I(name, name, BD_CHANNELS, 0, BD_CHANNELS, 300, 0, TRIG_TIME_WIN);
    histBDTimeByChannel->GetXaxis()->SetTitle("Channel #");
    histBDTimeByChannel->GetYaxis()->SetTitle("Time, ns");
    name = fTitle + "_BD_Simultaneous";
    histBDSimult = new TH1I(name, name, TRIG_MULTIPL, 1, TRIG_MULTIPL + 1);
    histBDSimult->GetXaxis()->SetTitle("Channels #");
    histBDSimult->GetYaxis()->SetTitle("Activation Count");
    name = fTitle + "_Si_Channels";
    histSiChannels = new TH1I(name, name, SI_CHANNELS, 0, SI_CHANNELS);
    histSiChannels->GetXaxis()->SetTitle("Channel #");
    histSiChannels->GetYaxis()->SetTitle("Activation Count");
    name = fTitle + "_Si_Time_by_Channels";
    histSiTimeByChannel = new TH2I(name, name, SI_CHANNELS, 0, SI_CHANNELS, 300, 0, TRIG_TIME_WIN);
    histSiTimeByChannel->GetXaxis()->SetTitle("Channel #");
    histSiTimeByChannel->GetYaxis()->SetTitle("Time, ns");
    name = fTitle + "_Si_Simultaneous";
    histSiSimult = new TH1I(name, name, TRIG_MULTIPL, 1, TRIG_MULTIPL + 1);
    histSiSimult->GetXaxis()->SetTitle("Channels #");
    histSiSimult->GetYaxis()->SetTitle("Activation Count");
    name = fTitle + "_BDxSi_Correlation";
    Int_t corrLen = max(BD_CHANNELS, SI_CHANNELS);
    histCorrSiBD = new TH2I(name, name, SI_CHANNELS, 0, SI_CHANNELS, BD_CHANNELS, 0, BD_CHANNELS);
    histCorrSiBD->GetXaxis()->SetTitle("Si Channels");
    histCorrSiBD->GetYaxis()->SetTitle("BD Channels");
    name = fTitle + "_BCxBC_Amplitude_Correlation";
    histCorrBCBC = new TH2I(name, name, 200, 0, AMP_WIN, 200, 0, AMP_WIN);
    histCorrBCBC->GetXaxis()->SetTitle("BC1 Amplitude");
    histCorrBCBC->GetYaxis()->SetTitle("BC2 Amplitude");
    name = fTitle + "_BCxVC_Amplitude_Correlation";
    histCorrBCVC = new TH2I(name, name, 200, 0, AMP_WIN, 200, 0, AMP_WIN);
    histCorrBCVC->GetXaxis()->SetTitle("BC1 Amplitude");
    histCorrBCVC->GetYaxis()->SetTitle("VC Amplitude");
    name = fTitle + "_Si+BD_Simultaneous";
    histSiBSum = new TH1I(name, name, TRIG_MULTIPL, 1, TRIG_MULTIPL + 1);
    histSiBSum->GetXaxis()->SetTitle("Channels #");
    histSiBSum->GetYaxis()->SetTitle("Activation Count");
    //    name = fTitle + "_BD_Specific_Channel";
    //    histBDSpecific = new TH1I(name, name, 300, 0, TRIG_TIME_WIN);
    //    histBDSpecific->GetXaxis()->SetTitle("Time, ns");
    //    histBDSpecific->GetYaxis()->SetTitle("Activation Count");
    name = fTitle + "_Counter";
    Int_t trigCount = shortNames.size();
    histTriggers = new TH1I(name, name, trigCount, 0, trigCount);
    histTriggers->GetXaxis()->SetTitle("Trigger Name");
    histTriggers->GetYaxis()->SetTitle("Activation Count");
    TAxis* xaTriggers = histTriggers->GetXaxis();
    name = fTitle + "_Times_by_Trigger";
    histTrigTimeByChannel = new TH2I(name, name, trigCount, 0, trigCount, 300, 0, TRIG_TIME_WIN);
    histTrigTimeByChannel->GetXaxis()->SetTitle("Trigger Name");
    histTrigTimeByChannel->GetYaxis()->SetTitle("Time, ns");
    TAxis* xaTrigTimes = histTrigTimeByChannel->GetXaxis();
    for (Int_t i = 0; i < trigCount; ++i) {
        xaTriggers->SetBinLabel(i + 1, shortNames[i]);
        xaTrigTimes->SetBinLabel(i + 1, shortNames[i]);
    }
    canTimesPads[0]->current = histTriggers;
    canTimesPads[1]->current = histBDChannels;
    canTimesPads[4]->current = histBDSimult;
    canTimesPads[2]->current = histSiChannels;
    canTimesPads[5]->current = histSiSimult;
    canTimesPads[3]->current = histSiBSum;
    for (Int_t iPad = 0; iPad < rows4Spectrum * fCols; iPad++) {
        PadInfo* p = new PadInfo();
        p->opt = "colz logz nostat";
        can2dPads[iPad] = p;
        can2d->GetPad(iPad + 1)->SetGrid();
    }
    can2dPads[0]->current = histCorrBCBC;
    can2dPads[0]->opt = "col";
    can2dPads[1]->current = histCorrBCVC;
    can2dPads[1]->opt = "col";
    can2dPads[2]->current = histCorrSiBD;
    can2dPads[2]->opt = "col";
    can2dPads[3]->current = histTrigTimeByChannel;
    can2dPads[4]->current = histBDTimeByChannel;
    can2dPads[5]->current = histSiTimeByChannel;
    for (Int_t i = 0; i < hists.size(); ++i) {
        canTimesPads[i + rows4Summary * fCols]->current = hists[i];
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

BmnHistTrigger::~BmnHistTrigger() {
    BDEvents->Clear();
    delete BDEvents;
    delete canTimes;
    delete canProfile;
    delete can2d;
    if (fDir)
        return;
    for (auto pad : canProfilePads)
        delete pad;
    for (auto pad : canTimesPads)
        delete pad;
    for (auto pad : can2dPads)
        delete pad;
}

void BmnHistTrigger::FillFromDigi(DigiArrays *fDigiArrays) {
    vector<TClonesArray*> *trigAr = fDigiArrays->trigAr;
    Int_t bdCount = 0;
    Int_t SiCount = 0;
    Int_t bc1Amp = 0;
    Int_t bc2Amp = 0;
    Int_t vcAmp = 0;
    BDEvents->Clear();
    for (auto &ar : *trigAr) {
        const char* arName = ar->GetName();
        Bool_t isBarrel = !strcmp(arName, "BD");
        Bool_t isSilicon = !strcmp(arName, "Si");
        if (ar->GetClass() == BmnTrigDigit::Class()) { // TDC data
            for (Int_t digIndex = 0; digIndex < ar->GetEntriesFast(); digIndex++) {
                BmnTrigDigit* td = (BmnTrigDigit*) ar->At(digIndex);
                TString trName = Form("%s_%d", ar->GetName(), td->GetMod());
                if (isBarrel) {
                    bdCount++;
                    histBDChannels->Fill(td->GetMod());
                    histBDTimeByChannel->Fill(td->GetMod(), td->GetTime(), 1);
                }
                if (isSilicon) {
                    SiCount++;
                    histSiChannels->Fill(td->GetMod());
                    histSiTimeByChannel->Fill(td->GetMod(), td->GetTime(), 1);
                }
                histTriggers->Fill(arName, 1);
                histTrigTimeByChannel->Fill(arName, td->GetTime(), 1);
                for (Int_t i = 0; i < trigNames.size(); ++i) {
                    if (
                            !strcmp(trName.Data(), trigNames[i].Data()) ||
                            !strcmp(arName, trigNames[i].Data())) {
                        hists[i]->Fill(td->GetTime());
                    }
                }
            }
        }
        if (ar->GetClass() == BmnTrigWaveDigit::Class()) { // TQDC ADC data
            for (Int_t digIndex = 0; digIndex < ar->GetEntriesFast(); digIndex++) {
                BmnTrigWaveDigit* td = (BmnTrigWaveDigit*) ar->At(digIndex);
                if (!strcmp(arName, "TQDC_ADC_BC1") && td->GetMod() == 2)
                    bc1Amp = td->GetPeak();
                if (!strcmp(arName, "TQDC_ADC_BC2") && td->GetMod() == 2)
                    bc2Amp = td->GetPeak();
                if (!strcmp(arName, "TQDC_ADC_VC") && td->GetMod() == 2)
                    vcAmp = td->GetPeak();
            }
        }
    }
    if (bdCount) histBDSimult->Fill(bdCount);
    if (SiCount) histSiSimult->Fill(SiCount);
    if (bdCount || SiCount) {
        histCorrSiBD->Fill(SiCount, bdCount);
        histSiBSum->Fill(SiCount + bdCount);
    }
    if (bc1Amp && bc2Amp)
        histCorrBCBC->Fill(bc1Amp, bc2Amp, 1);
    if (bc1Amp && vcAmp)
        histCorrBCVC->Fill(bc1Amp, vcAmp, 1);
}

void BmnHistTrigger::SetBDChannel(Int_t iSelChannel) {
    //    TString title;
    //    if (iSelChannel > (histBDSpecific->GetNbinsX() - 1)) {
    //        printf("Wrong channel!\n");
    //        return;
    //    }
    //    printf("Set channel: %d\n", fSelectedBDChannel);
    //    fSelectedBDChannel = iSelChannel;
    //    TString command;
    //    if (fSelectedBDChannel >= 0)
    //        command = Form("fMod == %d", fSelectedBDChannel);
    //    if (iSelChannel == -1)
    //        title = Form("BD for All Channels");
    //
    //    else
    //        title = "BD Time Length For: " + command;
    //    histBDSpecific->SetTitle(title);
    //    histBDSpecific->Reset();
    //    TString direction = "fTime>>" + TString(histBDSpecific->GetName());
    //    frecoTree->Draw(direction, command, "");

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
    fDir = Dir;
    for (auto &el : can2dPads) {
        if (el->current)
            el->current->SetDirectory(fDir);
    }
    for (auto &el : canTimesPads) {
        if (el->current)
            el->current->SetDirectory(fDir);
    }
}

void BmnHistTrigger::Reset() {
    for (auto &el : can2dPads) {
        if (el->current)
            el->current->Reset();
    }
    for (auto &el : canTimesPads) {
        if (el->current)
            el->current->Reset();
    }
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
    for (auto pad : canTimesPads) {
        if (pad->ref) delete pad->ref;
        pad->ref = NULL;
    }
    refID = 0;
}

ClassImp(BmnHistTrigger);
