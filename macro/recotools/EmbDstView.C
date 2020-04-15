/**
 * Macro for tracks versions comparison
 */
R__ADD_INCLUDE_PATH($VMCWORKDIR)

#if !defined(__CINT__) || defined(__MAKECINT__)

#include <TClass.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TClonesArray.h>
#include <TChain.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TH2.h>
#include <TH1.h>
#include <TF1.h>
#include <TPad.h>
#include <TVirtualPad.h>
#include <TString.h>
#include <TMath.h>
#include <TDatime.h>
#include <TROOT.h>
#include <TStyle.h>
#include <FairTrackParam.h>
#include <FairMCPoint.h>
#include <Rtypes.h>
#include <ncurses.h>
#include <FairMCEventHeader.h>

#include <DstEventHeader.h>
#include <CbmMCTrack.h>

#include <silicon/BmnSiliconStationSet.h>
#include <gem/BmnGemStripStationSet.h>
#include <csc/BmnCSCStationSet.h>
#include <BmnHit.h>
#include <BmnGlobalTrack.h>
//#include <BmnHist.h>
#include <physics/bmnrecotools/BmnRecoTools.h>

#else
#include "gconfig/basiclibs.C"
#include "bmnloadlibs.C"

#endif


//#define PAD_WIDTH_EMB   1920
//#define PAD_HEIGHT_EMB  1200
#define PAD_WIDTH_EMB   1200
#define PAD_HEIGHT_EMB  700
#define ROWS  2
#define COLS  3
#define EV_LIMIT  4
using namespace std;
using namespace TMath;

void EmbDstView() {
#if !(!defined(__CINT__) || defined(__MAKECINT__))
    bmnloadlibs(); // load BmnRoot libraries
#endif
    gStyle->SetOptStat(1);
    const Int_t nColors = 16;
    Int_t ColorMap[nColors] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 49, 11, 12, 13, 14, 15, 16};
    Int_t runID = 4649;
    Int_t fPeriodID = 7;
    const Int_t boxWidth = 40;
    const Int_t boxHeigth = 200;
    const Int_t Bins = 400;
//    TString inFileNameEve = "eve-p-nofield.root";
        TString inFileNameEve = Form("eve-%d-isAll-nocurl.root", runID);
        TString inFileNameDigi = Form("../run/bmn_run%d_digi.root", runID);
        TString inFileNameEmb = Form("../run/dst-%d.root", runID);
        TString inFileNameAna = Form("../run/an-%d.root", runID);
//        TString inFileNameEmb = Form("dst-%d-emb-qgsm-4h-isAll-nocurl-BaseOff.root", runID);

//    TString fnames[COLS] = {inFileNameEve, inFileNameBase, inFileNameEmb};
    TString fnames[COLS] = {inFileNameEve, inFileNameDigi, inFileNameEmb};
    TString treeNames[COLS] = {"bmndata", "bmndata", "bmndata"}; //{"BMN_DIGIT", "cbmsim", "cbmsim"};
    TString EHNames[COLS] = {"MCEventHeader.", "BmnEventHeader.", "DstEventHeader."};
    TClass* EHClasses[COLS] =
    {FairMCEventHeader::Class(), BmnEventHeader::Class(), DstEventHeader::Class()};
    TString hitNames[COLS] = {"StsPoint", "BmnInnerHits", "BmnInnerHits"};
    TString trackNames[COLS] = {"MCTrack", "BmnGlobalTrack", "BmnGlobalTrack"};
    TString hitNamesGEM[COLS] = {"StsPoint", "BmnGemStripHit", "BmnGemStripHit"};
    TString hitNamesSil[COLS] = {"SiliconPoint", "BmnSiliconHit", "BmnSiliconHit"};
    TString hitNamesCSC[COLS] = {"CSCPoint", "BmnCSCHit", "BmnCSCHit"};
    TString strVertex = "BmnVertex";
    TFile * files[COLS] = {NULL};
    TTree * trees[COLS] = {NULL};
    FairMCEventHeader * MCEH = NULL;
    TNamed * EHs[COLS] = {NULL};
    TClonesArray * hits[COLS] = {NULL};
    TClonesArray * tracks[COLS] = {NULL};
    TClonesArray * silTrack[COLS] = {NULL};
    TClonesArray * gemTrack[COLS] = {NULL};
    TClonesArray * silHits[COLS] = {NULL};
    TClonesArray * cscHits[COLS] = {NULL};
    TClonesArray * gemHits[COLS] = {NULL};
    TClonesArray * vertex = NULL;
    TClonesArray * tqdc_bc1 = NULL;
    TClonesArray * tqdc_bc2 = NULL;
//    TCanvas* can = new TCanvas("canHits", "", PAD_WIDTH_EMB * 1, PAD_HEIGHT_EMB * ROWS);
//    can->Divide(1, ROWS);

    /** Open exp files */
    for (Int_t i = 1; i < COLS; i++) {
        files[i] = new TFile(fnames[i], "READ");
        if (files[i]->IsOpen() == kTRUE){
            trees[i] = (TTree*) files[i]->Get(treeNames[i]);
            printf("file %s contains %lld events\n", fnames[i].Data(), trees[i]->GetEntries());
        }
        else {
            fprintf(stderr, "file %s open error\n", fnames[i].Data());
            continue;
            //            return;
        }
        trees[i]->SetBranchAddress(EHNames[i], &EHs[i]);
    }
    MCEH = (FairMCEventHeader*)EHs[0];
    
    const UInt_t nEvs = trees[2]->GetEntries();
    
    trees[1]->SetBranchAddress("TQDC_BC1", &tqdc_bc1);
    trees[1]->SetBranchAddress("TQDC_BC2", &tqdc_bc2);
    trees[2]->SetBranchAddress(strVertex, &vertex);
    
    TCanvas* canDST = new TCanvas("canCut", "", PAD_WIDTH_EMB * 2, PAD_HEIGHT_EMB * 2);
    canDST->Divide(2,2);
//    TString n = "Vertex-BC1";
//    TH2D* hvc1 = new TH2D(n.Data(), n.Data(), PAD_HEIGHT_EMB, 0, 2200, PAD_WIDTH_EMB, -4, 4);
//    n = "Vertex-BC1-notFound";
//    TH2D* hvc1Fail = new TH2D(n.Data(), n.Data(), PAD_HEIGHT_EMB, 0, 2200, 5, -1002, -998);
//    n = "Vertex-BC2";
//    TH2D* hvc2 = new TH2D(n.Data(), n.Data(), PAD_HEIGHT_EMB, 0, 2200, PAD_WIDTH_EMB, -4, 4);
//    n = "Vertex-BC2-notFound";
//    TH2D* hvc2Fail = new TH2D(n.Data(), n.Data(), PAD_HEIGHT_EMB, 0, 2200, 5, -1002, -998);
    
    
//    UInt_t iTreeDST = 2;
//    Long64_t iExpEvent = -1;
//    for (Int_t iEv = 0; iEv < nEvs; iEv++) {
//        DrawBar(iEv, nEvs);
//        trees[iTreeDST]->GetEntry(iEv);
//        UInt_t EventId = ((DstEventHeader*)EHs[iTreeDST])->GetEventId();
////                printf("iEv = %d  EventId = %u \n", iEv, EventId);
//        UInt_t i = 1;
//        while (iExpEvent < nEvs) {
//            //            printf("iExpEvent %lld\n", iExpEvent);
//            trees[i]->GetEntry(++iExpEvent);
////            printf("EHs[%lld]->GetEventId() = %u \n", iExpEvent, ((BmnEventHeader*)EHs[i])->GetEventId());
//            if (EventId == ((BmnEventHeader*)EHs[i])->GetEventId())
//                break;
//            if (EventId < ((BmnEventHeader*)EHs[i])->GetEventId()) {
////                printf("\n%d tree %u event lost!\n", i, EventId);
//                break;
//            }
//        }
//        Double_t vz = 0.0;
//        Double_t peak1 = 0.0;
//        Double_t peak2 = 0.0;
//        for (Int_t iTrack = 0; iTrack < vertex->GetEntriesFast(); iTrack++) {
//            CbmVertex* v = (CbmVertex*) vertex->UncheckedAt(iTrack);
//            vz = v->GetZ();
//        }
////        printf("tqdc_bc1->GetEntriesFast() = %d\n", tqdc_bc1->GetEntriesFast());
//        for (Int_t iTrack = 0; iTrack < tqdc_bc1->GetEntriesFast(); iTrack++) {
//            BmnTrigWaveDigit* dig = (BmnTrigWaveDigit*) tqdc_bc1->UncheckedAt(iTrack);
//            peak1 = dig->GetPeak();
//        }
//        for (Int_t iTrack = 0; iTrack < tqdc_bc2->GetEntriesFast(); iTrack++) {
//            BmnTrigWaveDigit* dig = (BmnTrigWaveDigit*) tqdc_bc2->UncheckedAt(iTrack);
//            peak2 = dig->GetPeak();
//        }
////        printf("\t\tvz = %f   peak = %f\n", vz , peak);
//        hvc1->Fill(peak1, vz);
//        hvc2->Fill(peak2, vz);
//        hvc1Fail->Fill(peak1, vz);
//        hvc2Fail->Fill(peak2, vz);
//    }
//    canDST->cd(1);
//    hvc1->Draw("colz");
//    canDST->cd(2);
//    hvc2->Draw("colz");
//    canDST->cd(3);
//    hvc1Fail->Draw("colz");
//    canDST->cd(4);
//    hvc2Fail->Draw("colz");
    
    //canDST->Divide(1, ROWS);
//    trees[2]->SetName("df");
    TFriendElement * fel = trees[2]->AddFriend(treeNames[0], inFileNameEmb); // DST tree
    TFriendElement * fAna = trees[2]->AddFriend(treeNames[0], inFileNameAna);
//    TFriendElement * fel = trees[0]->AddFriend(trees[2], "fa");
    printf("friend element %s\n", fel->GetTreeName());
//    printf("friend element %s\n", fAna->GetTreeName());
//    trees[0]->Draw("fa.ParticlePair.fInvMass", "fa.EventHeader.fEventId==bmndata.BmnEventHeader.fEventId");
    
    /** BC1 ADC amplitude vs Inv Mass diagram */
    canDST->cd(1);
    trees[2]->Draw("DstEventHeader.GetADC1():ParticlePair.fInvMass",
            "DstEventHeader.GetADC1() > 0 && ParticlePair.fInvMass > 1.08 && ParticlePair.fInvMass < 1.22", "colz");
    
    /** BC1 ADC amplitude vs Q/p */
    canDST->cd(2);
    trees[2]->Draw("DstEventHeader.GetADC1():BmnGemTrack.fParamFirst.fQp","DstEventHeader.GetADC1() > 0", "colz");
    
    canDST->cd(3);
    trees[2]->Draw("ParticlePair.fInvMass","ParticlePair.fInvMass > 1.08 && ParticlePair.fInvMass < 1.22", "");
    trees[2]->Draw("ParticlePair.fInvMass",
            "ParticlePair.fInvMass > 1.08 && ParticlePair.fInvMass < 1.22 && DstEventHeader.GetADC1() > 1200 && DstEventHeader.GetADC1() < 1600", "same");
    
    canDST->cd(4);
    trees[2]->Draw("BmnGemTrack.fParamFirst.fQp","DstEventHeader.fADC1 > 0", "");
    trees[2]->Draw("BmnGemTrack.fParamFirst.fQp",
            "DstEventHeader.GetADC1() > 0 && DstEventHeader.GetADC1() > 1200 && DstEventHeader.GetADC1() < 1600", "same");
    
    canDST->Update();
    canDST->Modified();
    canDST->SaveAs(Form("can-%d-cut-adc.pdf", runID));
    canDST->SaveAs(Form("can-%d-cut-adc.png", runID));



    return;
}

