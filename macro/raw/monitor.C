#include "TH1I.h"
#include "TH1F.h"
#include "TH1D.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TChain.h"
#include "TClonesArray.h"
#include <TMemFile.h>
#include "TGraph.h"
#include "TMath.h"
#include "TImage.h"
#include <stdlib.h>
#include "/home/ilnur/bmnroot/monitor/BmnMonHists.h"
#include <unistd.h>
#include <vector>
#include "THttpServer.h"
#include "TSystem.h"
#include "TFolder.h"
//#include "../../bmndata/BmnTof2Raw2Digit.h"

#define TOF400_PLANE_COUNT 3
#define TOF400_STRIP_COUNT 48
#define MAX_STATIONS 40
#define MAX_MODULES 8
#define BD_CHANNELS 40

void monitor(TString digiName = "$VMCWORKDIR/macro/raw/bmn_run0084_digi.root") {

    Int_t runIndex = 84;
    // GEM config
    Int_t hitBins = 100;
    const UInt_t gemLayers = 4;
    const UInt_t stationCount = 7;
    const UInt_t moduleCount[stationCount] = {1, 1, 1, 1, 1, 1, 2};
    const UInt_t nStrips[stationCount] = {256, 825, 825, 825, 825, 825, 1019};

    // ToF config
    const UInt_t kNPLANES = 16;
    const UInt_t kNREALWIRES = 240;
    const UInt_t kNWIRES = 256;
    const Int_t Number = 4;
    const Int_t nb = 100;
    const Float_t angles[kNPLANES] = {
        135, 135, 45, 45, 90, 90, 180, 180,
        135, 135, 45, 45, 90, 90, 180, 180
    };
    const TString names[kNPLANES] = {
        "VA_1", "VB_1", "UA_1", "UB_1", "YA_1", "YB_1", "XA_1", "XB_1",
        "VA_2", "VB_2", "UA_2", "UB_2", "YA_2", "YB_2", "XA_2", "XB_2"
    };
    /* Load basic libraries */
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs();
    //gROOT->LoadMacro("$VMCWORKDIR/macro/raw/DchDigitsAnalysis.C");

    TChain *digiTree = new TChain("cbmsim");
    digiTree->Add(digiName);
    Int_t nEvents = digiTree->GetEntries();
    Int_t layerIndex = 0;
    cout << "Events: " << nEvents << endl;

    TString name;
    BmnMonHists bmh = BmnMonHists();
    bmh.SetName("bmh");
    bmh.SetTitle("bmh");
    // ====================================================================== //
    // Create server
    // ====================================================================== //
    THttpServer* serv = new THttpServer(Form("fastcgi:9000", "BM@N Monitor"));


    TString outHistName = Form("bmn_run%04d_hist.root", runIndex);
    TFile *fHistOut = new TFile(outHistName, "recreate");

    // ====================================================================== //
    // GEM histograms init
    // ====================================================================== //
    TClonesArray * gemDigits = NULL;
    digiTree->SetBranchAddress("GEM", &gemDigits);

    TDirectory *dirGEM = fHistOut->mkdir("GEM_hists");
    dirGEM->cd();
    for (Int_t stationIndex = 0; stationIndex < stationCount; stationIndex++) {
        vector<vector<TH1F*> > rowGEM;
        for (Int_t moduleIndex = 0; moduleIndex < moduleCount[stationIndex]; moduleIndex++) {
            vector<TH1F*> colGEM;
            for (Int_t layerIndex = 0; layerIndex < gemLayers; layerIndex++) {
                name = Form("GEM_Station_%d_module_%d_layer_%d", stationIndex, moduleIndex, layerIndex);
                TH1F *h = new TH1F(name, name, hitBins, 0, nStrips[stationIndex]);
                serv->Register("/GEM/", h);
                colGEM.push_back(h);
            }
            rowGEM.push_back(colGEM);
        }
        bmh.histGemStrip.push_back(rowGEM);
    }

    // ====================================================================== //
    // ToF histograms init
    // ====================================================================== //
    TClonesArray *ToF4Digits = NULL;
    digiTree->SetBranchAddress("TOF400", &ToF4Digits);
    TDirectory *dirToF400 = fHistOut->mkdir("ToF400_hists");
    dirToF400->cd();
    name = "ToF400_Leading_Time";
    bmh.histToF400LeadingTime = new TH1D(name, name, 500, 0, 1000);
    name = "ToF400_Amplitude";
    bmh.histToF400Amp = new TH1D(name, name, 4096, 0, 96);
    name = "ToF400_Strip";
    bmh.histToF400Strip = new TH1I(name, name, TOF400_STRIP_COUNT, 0, TOF400_STRIP_COUNT);
    name = "ToF400_StripSimult";
    bmh.histToF400StripSimult = new TH1I(name, name, TOF400_STRIP_COUNT, 0, TOF400_STRIP_COUNT);
    name = "ToF400_State";
    bmh.histToF400State = new TH2F(name, name, TOF400_STRIP_COUNT, 0, TOF400_STRIP_COUNT, 2, 0, 2);

    TH1I *histL = new TH1I("", "", TOF400_STRIP_COUNT, 0, TOF400_STRIP_COUNT);
    TH1I *histR = new TH1I("", "", TOF400_STRIP_COUNT, 0, TOF400_STRIP_COUNT);
    TH1I histSimultaneous;
    histSimultaneous.SetDirectory(0);
    histL->SetDirectory(0);
    histR->SetDirectory(0);
    // ====================================================================== //
    // DCH histograms init
    // ====================================================================== //
    TClonesArray *DchDigits = NULL;
    digiTree->SetBranchAddress("DCH", &DchDigits);
    TDirectory *dirDrift = fHistOut->mkdir("DCH_hists");
    TTree *recoTree = new TTree("BmnMon", "BmnMon");
    dirDrift->cd();
    TH1F * h_wires[kNPLANES];
    Float_t v_wires[kNPLANES][kNWIRES] = {};
    Int_t myPalette[nb];
    Double_t R[Number] = {0.00, 0.00, 1.00, 1.00};
    Double_t G[Number] = {0.00, 1.00, 0.65, 0.00};
    Double_t B[Number] = {1.00, 0.00, 0.00, 0.00};
    Double_t Length[Number] = {0.0, 0.33, 0.66, 1.0};

    Int_t FI = TColor::CreateGradientColorTable(Number, Length, R, G, B, nb);
    for (Int_t i = 0; i < nb; ++i) {
        myPalette[i] = FI + i;
    }
    for (Int_t i = 0; i < kNPLANES; ++i) {
        h_wires[i] = new TH1F(names[i], names[i], kNREALWIRES, 0, kNREALWIRES);
        serv->Register("/DCH/", h_wires[i]);

    }
    TCanvas* chmb1 = new TCanvas("DCH_1", "DCH_1", 1000, 1000);

    // ====================================================================== //
    // Triggers histograms init
    // ====================================================================== //
    TClonesArray * trigBC1Digits = NULL;
    digiTree->SetBranchAddress("BC1", &trigBC1Digits);
    TClonesArray * trigBC2Digits = NULL;
    digiTree->SetBranchAddress("BC2", &trigBC2Digits);
    TClonesArray * trigVDDigits = NULL;
    digiTree->SetBranchAddress("VETO", &trigVDDigits);
    TClonesArray * trigBDDigits = NULL;
   // digiTree->SetBranchAddress("BD", &trigBDDigits);
    //    TClonesArray * trigBC1Digits = NULL;
    //    digiTree->SetBranchAddress("GEM", &trigBC1Digits);
    //    TClonesArray * trigBC1Digits = NULL;
    //    digiTree->SetBranchAddress("GEM", &trigBC1Digits);
    TDirectory *dirTrig = fHistOut->mkdir("Trigger_hists");
    dirTrig->cd();
    bmh.BDEvents = new TClonesArray("BmnTrigDigit");
    recoTree->Branch("BmnTrigBD", &(bmh.BDEvents));
    name = "BC1_Time_Length";
    bmh.histBC1TimeLen = new TH1D(name, name, 300, 0, 20000);
    name = "BC2_Time_Length";
    bmh.histBC2TimeLen = new TH1D(name, name, 300, 0, 2000);
    name = "FD_Time_Length";
    bmh.histFDTimeLen = new TH1D(name, name, 300, 0, 2000);
    name = "SD_Time_Length";
    bmh.histSDTimeLen = new TH1D(name, name, 300, 0, 2000);
    name = "VETO_Time_Length";
    bmh.histVDTimeLen = new TH1D(name, name, 300, 0, 2000);
    name = "BD_Channels";
    bmh.histBDChannels = new TH1I(name, name, BD_CHANNELS, 0, BD_CHANNELS);
    name = "BD_Specific_Channel";
    bmh.histBDSpecific = new TH1D(name, name, 300, 0, 2000);
    name = "Triggers_Counter";
    bmh.histTriggers = new TH1I(name, name, 5, 0, 5);
    TString triggerNames[5] = {"BC1", "SD", "BC2", "VD", "FD"};
    TAxis* xa = bmh.histTriggers->GetXaxis();
    for (Int_t i = 0; i < sizeof (triggerNames) / sizeof (*triggerNames); i++) {
        xa->SetBinLabel(i + 1, triggerNames[i]);
    }

    cout << "trigger histograms initialized" << endl;

    serv->AddLocation("img/", "/home/ilnur/bmnroot/monitor/img");

    // Register objects
    serv->Register("/", &bmh);
    serv->Register("/Triggers/", bmh.histBC1TimeLen);
    serv->Register("/Triggers/", bmh.histBC2TimeLen);
    serv->Register("/Triggers/", bmh.histFDTimeLen);
    serv->Register("/Triggers/", bmh.histVDTimeLen);
    serv->Register("/Triggers/", bmh.histSDTimeLen);
    serv->Register("/Triggers/", bmh.histTriggers);
    serv->Register("/Triggers/", bmh.histBDChannels);
    bmh.histBDSpecific->SetDirectory(0);
    serv->Register("/Triggers/", bmh.histBDSpecific);
    serv->Register("/ToF400/", bmh.histToF400LeadingTime);
    serv->Register("/ToF400/", bmh.histToF400Amp);
    serv->Register("/ToF400/", bmh.histToF400Strip);
    serv->Register("/ToF400/", bmh.histToF400StripSimult);
    serv->Register("/ToF400/", bmh.histToF400State);
    cout << " histograms registered" << endl;

    serv->SetItemField("/Triggers/", "_monitoring", "2000");
    serv->SetItemField("/Triggers/", "_layout", "grid3x2");
    serv->SetItemField("/Triggers/", "_drawitem",
            "[BC1_Time_Length,BC2_Time_Length,FD_Time_Length,VETO_Time_Length,SD_Time_Length,BD_Specific_Channel]");
    serv->SetItemField("/Triggers/", "_drawopt", "colz");
    // Register commands
    serv->RegisterCommand("/ResetGEM", "/bmh/->ClearGEM()", "button;img/reset.png");
    serv->RegisterCommand("/ResetToF400", "/bmh/->ClearToF400()", "button;img/reset.png");
    serv->RegisterCommand("/ResetToF700", "/bmh/->ClearToF700()", "button;img/reset.png");
    serv->RegisterCommand("//Triggers/ChangeBDChannel", "/bmh/->SetSelBDChannel(%arg1%)", "button;img/reset.png");
    serv->RegisterCommand("/Triggers/ResetTriggers", "/bmh/->ClearTriggers()", "button;img/reset.png");
    serv->RegisterCommand("/ResetAll", "/bmh/->Clear()", "button;img/reset.png");
    //serv->Hide("/ResetAll");

    // Start events
    Int_t res = 0;
    Int_t station = 0;
    Int_t module = 0;
    Int_t layer = 0;
    Int_t gemStrip = 0;
    Int_t ToF400strip = 0;
    Int_t ToF400selectedPlane = 2;
    while (kTRUE) {
        for (Long64_t iEv = 0; iEv < nEvents; iEv++) {
            digiTree->GetEntry(iEv);
            // ====================================================================== //
            // Triggers histograms fill
            // ====================================================================== //
            cout << "Event " << iEv << " found BC1 digits: " << trigBC1Digits->GetEntriesFast() << endl;
            for (Int_t digIndex = 0; digIndex < trigBC1Digits->GetEntriesFast(); digIndex++) {
                BmnTrigDigit* td1 = (BmnTrigDigit*) trigBC1Digits->At(digIndex);
                bmh.histBC1TimeLen->Fill(td1->GetAmp());
                bmh.histTriggers->Fill(0);
            }
            for (Int_t digIndex = 0; digIndex < trigBC2Digits->GetEntriesFast(); digIndex++) {
                BmnTrigDigit* td2 = (BmnTrigDigit*) trigBC2Digits->At(digIndex);
                bmh.histBC2TimeLen->Fill(td2->GetAmp());
                bmh.histTriggers->Fill(2);
            }
            for (Int_t digIndex = 0; digIndex < trigVDDigits->GetEntriesFast(); digIndex++) {
                BmnTrigDigit* tv = (BmnTrigDigit*) trigVDDigits->At(digIndex);
                bmh.histVDTimeLen->Fill(tv->GetAmp());
                bmh.histTriggers->Fill(3);
            }
//            for (Int_t digIndex = 0; digIndex < trigVDDigits->GetEntriesFast(); digIndex++) {
//                BmnTrigDigit* bd = (BmnTrigDigit*) trigBDDigits->At(digIndex);
//                bmh.histBDChannels->Fill(bd->GetMod());
//                BmnTrigDigit *bd1 = new (*(bmh.BDEvents))[bmh.BDEvents->GetEntriesFast()] BmnTrigDigit();
//                bd1->SetAmp(bd->GetAmp());
//                bd1->SetDet(bd->GetDet());
//                bd1->SetMod(bd->GetMod());
//                bd1->SetTime(bd->GetTime());
//            }
            // ====================================================================== //
            // GEM histograms fill
            // ====================================================================== // 
            for (Int_t digIndex = 0; digIndex < gemDigits->GetEntriesFast(); digIndex++) {
                BmnGemStripDigit* gs = (BmnGemStripDigit*) gemDigits->At(digIndex);
                module = gs->GetModule();
                station = gs->GetStation();
                layer = gs->GetStripLayer();
                gemStrip = gs->GetStripNumber();
                if (gs->GetStripLayer() != 0) continue;
                bmh.histGemStrip[station][module][layer]->Fill(gemStrip);
            }
            // ====================================================================== //
            // ToF histograms fill
            // ====================================================================== //
            histL->Reset();
            histR->Reset();
            histSimultaneous.Reset();
            bmh.histToF400State->Reset();
            cout << "Event " << iEv << " found tof digits: " << ToF4Digits->GetEntriesFast() << endl;
            for (Int_t digIndex = 0; digIndex < ToF4Digits->GetEntriesFast(); digIndex++) {
                BmnTof1Digit *td = (BmnTof1Digit *) ToF4Digits->At(digIndex);
                ToF400strip = td->GetStrip();
                bmh.histToF400LeadingTime->Fill(td->GetTime());
                bmh.histToF400Amp->Fill(td->GetAmplitude());
                bmh.histToF400Strip->Fill(ToF400strip);
                if (td->GetPlane() == ToF400selectedPlane)
                    bmh.histToF400State->Fill(td->GetStrip(), td->GetSide(), td->GetAmplitude());
                if (td->GetSide() == 0)
                    histL->Fill(ToF400strip);
                else
                    histR->Fill(ToF400strip);
            }
            //histSimultaneous = (*histL) * (*histR);
            Int_t s;
            for (Int_t binIndex = 1; binIndex < TOF400_STRIP_COUNT; binIndex++) {
                s = ((histL->GetBinContent(binIndex) * histR->GetBinContent(binIndex)) != 0) ? 1 : 0;
                bmh.histToF400StripSimult->AddBinContent(s);
            }
            // ====================================================================== //
            // DCH histograms fill
            // ====================================================================== //
            for (Int_t iDig = 0; iDig < DchDigits->GetEntriesFast(); ++iDig) {
                BmnDchDigit* dig = (BmnDchDigit*) DchDigits->At(iDig);
                Int_t plane = dig->GetPlane();
                Int_t wire = dig->GetWireNumber();

                v_wires[plane][wire] += 1;
                if (wire > kNREALWIRES - 1) {
                    wire -= 128; //8 * 16 last preamplifier setup behind hole, so move signal in correct place
                }
                h_wires[plane]->Fill(wire);
            }
            //        if (iEv%10 == 0){
            //            memset(v_wires, (Float_t)0, sizeof(v_wires));
            //            for (Int_t ih = 0; ih < kNPLANES; ih++)
            //                h_wires[ih]->Reset();
            //            for (Int_t innerEv = 0; innerEv < iEv; innerEv++) {
            //                digiTree->GetEntry(innerEv);
            //                for (Int_t iDig = 0; iDig < DchDigits->GetEntriesFast(); ++iDig) {
            //                    BmnDchDigit* dig = (BmnDchDigit*) DchDigits->At(iDig);
            //                    Int_t plane = dig->GetPlane();
            //                    Int_t wire = dig->GetWireNumber();
            //                    
            //                    v_wires[plane][wire] += 1;
            //                    if (wire > kNREALWIRES - 1) {
            //                        wire -= 128; //8 * 16 last preamplifier setup behind hole, so move signal in correct place
            //                    }
            //                    h_wires[plane]->Fill(wire);
            //                }
            //            }
            //            for (Int_t i = 0; i < kNPLANES; ++i) {
            //                Float_t maxSig = 0.0;
            //                for (Int_t j = 0; j < kNWIRES; ++j) {
            //                    Float_t sig = v_wires[i][j];
            //                    if (sig > maxSig) maxSig = sig;
            //                }
            //                for (Int_t j = 0; j < kNWIRES; ++j) {
            //                    v_wires[i][j] /= maxSig;
            //                }
            //            }
            //            chmb1->Clear();
            //            chmb1->cd();
            //            cout << "Event " << iEv << " begin to draw wires: " << endl; 
            //            for (Int_t i = 0; i < kNPLANES / 2; ++i) {
            //                if (res=gSystem->ProcessEvents()) break;
            //                DrawAllWires(chmb1, myPalette, v_wires[i], angles[i], i);
            //            }
            //        }
            // ====================================================================== //
            // Update canvases //
            // ====================================================================== //
            //            canvasToF400State->Modified();
            //            canvasToF400State->Update();
            chmb1->Modified();
            chmb1->Update();
            if (res = gSystem->ProcessEvents()) break;
            cout << "Event " << iEv << " processed with result " << res << endl;
            usleep(3e6);
        }
    }
    /*//if (ToF4Digits->GetEntriesFast() > 0)
    for (Int_t evIndex = 0; evIndex < 10; evIndex++){
        digiTree->GetEntry(evIndex);
    for (Int_t digIndex = 0; digIndex < ToF4Digits->GetEntriesFast(); digIndex++){
        BmnTof1Digit *td = (BmnTof1Digit *)ToF4Digits->At(digIndex);
        if (td->GetPlane() == ToF400selectedPlane)
            bmh.histToF400State->Fill(td->GetStrip(), td->GetSide(), td->GetAmplitude());
    }}*/

    /*const Int_t MaxPlane = 
    for (int i=0; i<MaxPlane; i++) // MaxPlane - число камер в системе
    {
        // гистогаммы срабатывания стрипов для всех камер
        // сработал стрип в событии - в соответствующий бин единичка
        sprintf(name, "Rate_vs_Strip_Chamber_%d",i+1);
        sprintf(title, "Rate vs Strip Chamber %d",i+1);
        RvsS[i] = new TH1F(name,title,TOF2_MAX_STRIPS_IN_CHAMBER,0,TOF2_MAX_STRIPS_IN_CHAMBER);
    }
     */

    fHistOut->Write();
    fHistOut->Close();
    //chmb1->SaveAs("DCH1_wires.png");
    /*TImage * img = TImage::Open("DCH1_wires.png");    
    TCanvas * png_canvas = new TCanvas("DCH-1_pic", "DCH-1_pic", 100, 100);
    png_canvas->cd();
    img->Draw();
    png_canvas->Modified();
    png_canvas->Update();
    img->SetName("DCH_img");
   serv->Register("/", img);*/
    //delete img;

    //TFile *hfile = new TMemFile("GEM online","RECREATE","Demo ROOT file with histograms");
    //hfile->Write();

}

inline void FillEvent() {

}

inline void DrawAllWires(TCanvas* c, Int_t* palette, Float_t * wires, Float_t ang, Short_t planeId) {
    const UInt_t kNANGLE = 8;
    Double_t l = c->GetAbsWNDC();
    Double_t a = l / (1 + Sqrt2());
    Double_t b = l / (2 + Sqrt2());
    Double_t xPol[kNANGLE] = {b, 0, 0, b, b + a, l, l, b + a};
    Double_t yPol[kNANGLE] = {0, b, b + a, l, l, b + a, b, 0};
    TGraph* gr = new TGraph(kNANGLE, xPol, yPol);
    gr->GetXaxis()->SetRangeUser(0.0, 1.0);
    gr->GetYaxis()->SetRangeUser(0.0, 1.0);
    gr->SetTitle("oct");
    gr->SetLineWidth(3);
    gr->GetXaxis()->SetLabelColor(0);
    gr->GetXaxis()->SetTickLength(0);
    gr->GetYaxis()->SetLabelColor(0);
    gr->GetYaxis()->SetTickLength(0);
    gr->SetFillStyle(4000);
    //    gr->Draw("la");
    Float_t w = l / kNREALWIRES;
    Float_t x0 = 0.0;
    Float_t x1 = 0.0;
    Float_t y0 = 0.0;
    Float_t y1 = 0.0;

    TLine line;
    line.SetLineWidth(2);
    for (Int_t j = 0; j < kNWIRES; ++j) {

        if (j >= 112 && j < 128) {
            x0 = w * j;
            x1 = x0;
            y0 = 0;
            y1 = l / 2;
        } else if (j > kNREALWIRES - 1) {
            x0 = w * (j - 128);
            x1 = x0;
            y0 = l / 2;
            y1 = l;
        } else {
            x0 = w * j;
            x1 = x0;
            if (x0 <= b) {
                y0 = -x0 + b;
                y1 = x0 + a + b;
            } else if (x0 >= a + b) {
                y0 = -x0 + 2 * a + 3 * b;
                y1 = x0 - a - b;
            } else {
                y0 = 0;
                y1 = l;
            }
        }

        Int_t colorId = Int_t(wires[j] * (nb - 1));
        line.SetLineColor(palette[colorId]);
        line.SetLineColorAlpha(palette[colorId], 0.9 - 0.9 / 7 * planeId); //transparency
        if (colorId == 0) {
            line.SetLineColor(kWhite);
            line.SetLineColorAlpha(kWhite, planeId * 0.1);
        }
        Float_t sinPhi = Sin(ang * DegToRad());
        Float_t cosPhi = Cos(ang * DegToRad());
        Float_t x0_ = (x0 - l / 2) * cosPhi - (y0 - l / 2) * sinPhi + l / 2;
        Float_t y0_ = (x0 - l / 2) * sinPhi + (y0 - l / 2) * cosPhi + l / 2;
        Float_t x1_ = (x1 - l / 2) * cosPhi - (y1 - l / 2) * sinPhi + l / 2;
        Float_t y1_ = (x1 - l / 2) * sinPhi + (y1 - l / 2) * cosPhi + l / 2;
        line.DrawLine(x0_, y0_, x1_, y1_);
        x0 += w;
        x1 = x0;
    }
    TEllipse* hole = new TEllipse((a + 2 * b) / 2, (a + 2 * b) / 2, (a + 2 * b) / 30);
    hole->Draw("same");
}

void DrawWires(TPad* pad, Int_t* palette, Float_t * wires, Float_t ang) {

    pad->Draw();
    pad->cd();
    const UInt_t kNANGLE = 8;
    Double_t l = pad->GetAbsWNDC();
    Double_t a = l / (1 + Sqrt2());
    Double_t b = l / (2 + Sqrt2());
    Double_t xPol[kNANGLE] = {b, 0, 0, b, b + a, l, l, b + a};
    Double_t yPol[kNANGLE] = {0, b, b + a, l, l, b + a, b, 0};
    TGraph* gr = new TGraph(kNANGLE, xPol, yPol);
    gr->GetXaxis()->SetRangeUser(0.0, 1.0);
    gr->GetYaxis()->SetRangeUser(0.0, 1.0);
    gr->SetTitle("");
    gr->SetLineWidth(3);
    gr->GetXaxis()->SetLabelColor(0);
    gr->GetXaxis()->SetTickLength(0);
    gr->GetYaxis()->SetLabelColor(0);
    gr->GetYaxis()->SetTickLength(0);
    gr->Draw("la");
    Float_t w = l / kNREALWIRES;
    Float_t x0 = 0.0;
    Float_t x1 = 0.0;
    Float_t y0 = 0.0;
    Float_t y1 = 0.0;

    TLine line;
    line.SetLineWidth(3);
    for (Int_t j = 0; j < kNWIRES; ++j) {

        if (j >= 112 && j < 128) {
            x0 = w * j;
            x1 = x0;
            y0 = 0;
            y1 = l / 2;
        } else if (j > kNREALWIRES - 1) {
            x0 = w * (j - 128);
            x1 = x0;
            y0 = l / 2;
            y1 = l;
        } else {
            x0 = w * j;
            x1 = x0;
            if (x0 <= b) {
                y0 = -x0 + b;
                y1 = x0 + a + b;
            } else if (x0 >= a + b) {
                y0 = -x0 + 2 * a + 3 * b;
                y1 = x0 - a - b;
            } else {
                y0 = 0;
                y1 = l;
            }
        }

        Int_t colorId = Int_t(wires[j] * (nb - 1));
        line.SetLineColor(palette[colorId]);
        if (colorId == 0)
            line.SetLineColor(kWhite);
        Float_t sinPhi = Sin(ang * DegToRad());
        Float_t cosPhi = Cos(ang * DegToRad());
        Float_t x0_ = (x0 - l / 2) * cosPhi - (y0 - l / 2) * sinPhi + l / 2;
        Float_t y0_ = (x0 - l / 2) * sinPhi + (y0 - l / 2) * cosPhi + l / 2;
        Float_t x1_ = (x1 - l / 2) * cosPhi - (y1 - l / 2) * sinPhi + l / 2;
        Float_t y1_ = (x1 - l / 2) * sinPhi + (y1 - l / 2) * cosPhi + l / 2;
        line.DrawLine(x0_, y0_, x1_, y1_);
        x0 += w;
        x1 = x0;
    }
    TEllipse* hole = new TEllipse((a + 2 * b) / 2, (a + 2 * b) / 2, (a + 2 * b) / 30);
    hole->Draw("same");
}
