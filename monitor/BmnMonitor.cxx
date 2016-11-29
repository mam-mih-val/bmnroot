#include <iterator>

#include "BmnMonitor.h"
#include "BmnHistToF.h"
#include "BmnHistTrigger.h"

BmnMonitor::BmnMonitor() {

}

BmnMonitor::~BmnMonitor() {

}

void BmnMonitor::ProcessRun(TString digiName){
        
    // DCH config
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
    
    TChain *digiTree = new TChain("cbmsim");
    digiTree->Add(digiName);
    Int_t nEvents = digiTree->GetEntries();
    cout << "Events: " << nEvents << endl;

    TString name;
    TString outHistName = Form("bmn_run%04d_hist.root", runIndex);
    TFile *fHistOut = new TFile(outHistName, "recreate");
    TTree *recoTree = new TTree("BmnMon", "BmnMon");
    // ====================================================================== //
    // Create server
    // ====================================================================== //
    THttpServer* serv = new THttpServer("fastcgi:9000");
    
    // ====================================================================== //
    // GEM histograms init
    // ====================================================================== //
    TClonesArray * gemDigits = NULL;
    digiTree->SetBranchAddress("GEM", &gemDigits);
    TDirectory *dirGEM = fHistOut->mkdir("GEM_hists");
    BmnHistGem *bhGem = new BmnHistGem("GEM");
    bhGem->Register(serv, dirGEM, recoTree);

    // ====================================================================== //
    // ToF histograms init
    // ====================================================================== //
    TClonesArray *ToF4Digits = NULL;    
    digiTree->SetBranchAddress("TOF400", &ToF4Digits);
//    TDirectory *dirToF400 = fHistOut->mkdir("ToF400_hists");
    BmnHistToF *bhToF400 = new BmnHistToF("ToF400");
    bhToF400->Register(serv, fHistOut, recoTree);
    // ====================================================================== //
    // DCH histograms init
    // ====================================================================== //
    TClonesArray *DchDigits = NULL;
    digiTree->SetBranchAddress("DCH", &DchDigits);
    TDirectory *dirDrift = fHistOut->mkdir("DCH_hists");
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
    //    TCanvas* chmb1 = new TCanvas("DCH_1", "DCH_1", 1000, 1000);

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
    BmnHistTrigger *bhTrig = new BmnHistTrigger("Triggers");
    bhTrig->Register(serv, dirTrig, recoTree);

    // Register objects
    cout << " histograms registered" << endl;
//
//    serv->SetItemField("/Triggers/", "_monitoring", "2000");
//    serv->SetItemField("/Triggers/", "_layout", "grid3x2");
//    serv->SetItemField("/Triggers/", "_drawitem",
//            "[BC1_Time_Length,BC2_Time_Length,FD_Time_Length,VETO_Time_Length,SD_Time_Length,BD_Specific_Channel]");

//    serv->SetItemField("/ToF400/", "_layout", "grid3x2");
//    serv->SetItemField("/ToF400/", "_drawitem",
//            "[ToF400_Leading_Time,ToF400_Amplitude]");

    // Start events
    Int_t res = 0;
//    while (kTRUE) {
        for (Long64_t iEv = 0; iEv < nEvents; iEv++) {
            digiTree->GetEntry(iEv);
            // ====================================================================== //
            // Triggers histograms fill
            // ====================================================================== //
            bhTrig->FillFromDigi(
                    trigBC1Digits,
                    NULL,
                    trigBC2Digits,
                    trigVDDigits,
                    NULL,
                    trigBDDigits);
            // ====================================================================== //
            // GEM histograms fill
            // ====================================================================== // 
            bhGem->FillFromDigi(gemDigits);
            // ====================================================================== //
            // ToF histograms fill
            // ====================================================================== //
            bhToF400->FillFromDigi(ToF4Digits);
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
            // ====================================================================== //
            // Update canvases //
            // ====================================================================== //
            recoTree->Fill();            
//            bmh.histToF400LeadingTimeSpecific = (TH1D*)gPad->GetPrimitive("htemp");   

    
            if (iEv % itersToUpdate == 0) {
//                bhTrig->SaveHists(imgSavePath);
            }
            if (res = gSystem->ProcessEvents()) break;
            cout << "Event " << iEv << " processed with result " << res << endl;
            usleep(1e5);
        }
//    }

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

    recoTree->Write();
    fHistOut->Write();
    fHistOut->Close();
//    delete serv;
}

void BmnMonitor::ProcessStreamRun(TString digiName){
    
//    BmnDataReceiver *dr = new BmnDataReceiver();
//    BmnRawDataDecoder *rdd = new BmnRawDataDecoder();
////    FILE *data_stream = istream_iterator<UInt_t>(dr->data_queue);
////    rdd->SetRawFileIn(data_stream);
//    rdd->InitConverter();
//    rdd->InitDecoder();
    
    // DCH config
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
    
    TChain *digiTree = new TChain("cbmsim");
    digiTree->Add(digiName);
    Int_t nEvents = digiTree->GetEntries();
    cout << "Events: " << nEvents << endl;

    TString name;
    TString outHistName = Form("bmn_run%04d_hist.root", runIndex);
    TFile *fHistOut = new TFile(outHistName, "recreate");
    TTree *recoTree = new TTree("BmnMon", "BmnMon");
    // ====================================================================== //
    // Create server
    // ====================================================================== //
    THttpServer* serv = new THttpServer("fastcgi:9000");
    // ====================================================================== //
    // GEM histograms init
    // ====================================================================== //
    TClonesArray * gemDigits = NULL;
    digiTree->SetBranchAddress("GEM", &gemDigits);
    TDirectory *dirGEM = fHistOut->mkdir("GEM_hists");
    BmnHistGem *bhGem = new BmnHistGem("GEM");
    bhGem->Register(serv, dirGEM, recoTree);

    // ====================================================================== //
    // ToF histograms init
    // ====================================================================== //
    TClonesArray *ToF4Digits = NULL;    
    digiTree->SetBranchAddress("TOF400", &ToF4Digits);
//    TDirectory *dirToF400 = fHistOut->mkdir("ToF400_hists");
    BmnHistToF *bhToF400 = new BmnHistToF("ToF400");
    bhToF400->Register(serv, fHistOut, recoTree);
    // ====================================================================== //
    // DCH histograms init
    // ====================================================================== //
    TClonesArray *DchDigits = NULL;
    digiTree->SetBranchAddress("DCH", &DchDigits);
    TDirectory *dirDrift = fHistOut->mkdir("DCH_hists");
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
    //    TCanvas* chmb1 = new TCanvas("DCH_1", "DCH_1", 1000, 1000);

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
    BmnHistTrigger *bhTrig = new BmnHistTrigger("Triggers");
    bhTrig->Register(serv, dirTrig, recoTree);

    // Register objects
    cout << " histograms registered" << endl;
//
//    serv->SetItemField("/Triggers/", "_monitoring", "2000");
//    serv->SetItemField("/Triggers/", "_layout", "grid3x2");
//    serv->SetItemField("/Triggers/", "_drawitem",
//            "[BC1_Time_Length,BC2_Time_Length,FD_Time_Length,VETO_Time_Length,SD_Time_Length,BD_Specific_Channel]");

//    serv->SetItemField("/ToF400/", "_layout", "grid3x2");
//    serv->SetItemField("/ToF400/", "_drawitem",
//            "[ToF400_Leading_Time,ToF400_Amplitude]");

    // Start events
    Int_t res = 0;
//    while (kTRUE) {
        for (Long64_t iEv = 0; iEv < nEvents; iEv++) {
            digiTree->GetEntry(iEv);
            // ====================================================================== //
            // Triggers histograms fill
            // ====================================================================== //
            bhTrig->FillFromDigi(
                    trigBC1Digits,
                    NULL,
                    trigBC2Digits,
                    trigVDDigits,
                    NULL,
                    trigBDDigits);
            // ====================================================================== //
            // GEM histograms fill
            // ====================================================================== // 
            bhGem->FillFromDigi(gemDigits);
            // ====================================================================== //
            // ToF histograms fill
            // ====================================================================== //
            bhToF400->FillFromDigi(ToF4Digits);
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
            // ====================================================================== //
            // Update canvases //
            // ====================================================================== //
            recoTree->Fill();            
//            bmh.histToF400LeadingTimeSpecific = (TH1D*)gPad->GetPrimitive("htemp");   

    
            if (iEv % itersToUpdate == 0) {
//                bhTrig->SaveHists(imgSavePath);
            }
            if (res = gSystem->ProcessEvents()) break;
            serv->ProcessRequests();
            cout << "Event " << iEv << " processed with result " << res << endl;
            usleep(1e6);
        }
//    }

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

    recoTree->Write();
    fHistOut->Write();
    fHistOut->Close();
}


ClassImp(BmnMonitor);