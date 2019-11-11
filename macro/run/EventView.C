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
#include <Fit/FitResult.h>
#include <FairTrackParam.h>
#include <Rtypes.h>
#include <root/TVirtualPad.h>
#include <ncurses.h>

#else
#include "gconfig/basiclibs.C"
#include "bmnloadlibs.C"

#endif


#define PAD_WIDTH   640
#define PAD_HEIGHT  360
#define ROWS  2
#define COLS  3
#define EV_LIMIT  4
using namespace std;
using namespace TMath;

void EventView() {
#if !(!defined(__CINT__) || defined(__MAKECINT__))
    bmnloadlibs(); // load BmnRoot libraries
#endif
    gStyle->SetOptStat(0);
    const Int_t nColors = 16;
    Int_t ColorMap[nColors] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 49, 11, 12, 13, 14, 15, 16};
    Int_t runID = 4649;
    const Int_t boxWidth = 40;
    TString inFileNameCbm = Form("/home/ilnur/nica/cbm/%04d_Ext_v5.root", runID);
    TString inFileNameCbm2Bmn = "dst-emb.root"; //Form("cbmdst_%d.root", runID);
    TString inFileNameBmn = "dst-emb.root"; //Form("dst_MK_%d.root", runID); //~/BMNDST_%d.root

    TString fnames[COLS] = {inFileNameCbm, inFileNameCbm2Bmn, inFileNameBmn};
    TString treeNames[COLS] = {"bmndata", "bmndata", "bmndata"}; //{"BMN_DIGIT", "cbmsim", "cbmsim"};
    TString hitNames[COLS] = {"StsHit", "BmnInnerHits", "BmnInnerHits"};
    TString trackNames[COLS] = {"StsTrack", "BmnGlobalTrack", "BmnGlobalTrack"};
    TFile * files[COLS] = {NULL};
    TTree * trees[COLS] = {NULL};
    TClonesArray * hits[COLS] = {NULL};
    TClonesArray * tracks[COLS] = {NULL};
    TClonesArray * silTrack[COLS] = {NULL};
    TClonesArray * gemTrack[COLS] = {NULL};
    TClonesArray * silHits[COLS] = {NULL};
    TClonesArray * gemHits[COLS] = {NULL};
    TH2D * h[COLS][ROWS][nColors] = {NULL};
    for (Int_t ic = 0; ic < nColors; ic++) {
        h[0][0][ic] = new TH2D(Form("h0-0-%d", ic), "Hits XZ CBM", 400, -boxWidth, boxWidth, 400, 0, 200);
        h[0][1][ic] = new TH2D(Form("h0-1-%d", ic), "Hits YZ CBM", 400, -boxWidth, boxWidth, 400, 0, 200);
        //        h[0][2] = new TH1I("h0-2", "Track 1st TX CBM", 200, -0.5, 0.5);
        //        h[0][3] = new TH1I("h0-3", "Track 1st TY CBM", 200, -0.5, 0.5);
        h[1][0][ic] = new TH2D(Form("h1-0-%d", ic), "Hits XZ CBM2BMN", 400, -boxWidth, boxWidth, 400, 0, 200);
        h[1][1][ic] = new TH2D(Form("h1-1-%d", ic), "Hits YZ CBM2BMN", 400, -boxWidth, boxWidth, 400, 0, 200);
        //        h[1][2] = new TH1I("h0-2", "Track 1st TX CBM2BMN", 200, -0.5, 0.5);
        //        h[1][3] = new TH1I("h0-3", "Track 1st TY CBM2BMN", 200, -0.5, 0.5);
        h[2][0][ic] = new TH2D(Form("h2-0-%d", ic), "Hits XZ BMN", 400, -boxWidth, boxWidth, 400, 0, 200);
        h[2][1][ic] = new TH2D(Form("h2-1-%d", ic), "Hits YZ BMN", 400, -boxWidth, boxWidth, 400, 0, 200);
        //        h[2][2] = new TH1I("h0-2", "Track 1st TX BMN", 200, -0.5, 0.5);
        //        h[2][3] = new TH1I("h0-3", "Track 1st TY BMN", 200, -0.5, 0.5);
    }
    TCanvas* can = new TCanvas("canHits", "", PAD_WIDTH * COLS, PAD_HEIGHT * ROWS);
    can->Divide(COLS, ROWS);
    //        can->Size(PAD_WIDTH * 2, PAD_HEIGHT * 2);

    for (Int_t i = 1; i < COLS; i++) {
        files[i] = new TFile(fnames[i], "READ");
        if (files[i]->IsOpen())
            printf("file %s opened\n", fnames[i].Data());
        else {
            fprintf(stderr, "file %s open error\n", fnames[i].Data());
            continue;
            //            return;
        }
        trees[i] = (TTree*) files[i]->Get(treeNames[i]);
        trees[i]->SetBranchAddress(hitNames[i], &hits[i]);
        trees[i]->SetBranchAddress(trackNames[i], &tracks[i]);
        for (Int_t j = 0; j < ROWS; j++) {
            for (Int_t ic = 0; ic < nColors; ic++) {
                h[i][j][ic]->SetMarkerStyle(ic > 0 ? 43 : 2);
                h[i][j][ic]->SetMarkerSize(ic > 0 ? 2 : 1);
                h[i][j][ic]->SetMarkerColor(ColorMap[ic]);
            }
        }
    }
    if (trees[1]) {
        trees[1]->SetBranchAddress("BmnSiliconTrack", &silTrack[1]);
        trees[1]->SetBranchAddress("BmnGemTrack", &gemTrack[1]);
        trees[1]->SetBranchAddress("BmnSiliconHit", &silHits[1]);
        trees[1]->SetBranchAddress("BmnGemStripHit", &gemHits[1]);
    }
    if (trees[2]) {
        trees[2]->SetBranchAddress("BmnSiliconTrack", &silTrack[2]);
        trees[2]->SetBranchAddress("BmnGemTrack", &gemTrack[2]);
        trees[2]->SetBranchAddress("BmnSiliconHit", &silHits[2]);
        trees[2]->SetBranchAddress("BmnGemStripHit", &gemHits[2]);
    }

    const UInt_t nEvs = trees[1]->GetEntries();
    cout << "#recorded entries = " << nEvs << " in file" << endl;
    Int_t ch = 0;
    Int_t iEv = 0;
    do {
        Int_t hitCount[COLS] = {0};
        for (Int_t i = 1; i < COLS; i++) {
            for (Int_t row = 0; row < ROWS; row++)
                for (Int_t ic = 0; ic < nColors; ic++)
                    h[i][row][ic]->Reset();
            trees[i]->GetEntry(iEv);
            for (Int_t iHit = 1; iHit < hits[i]->GetEntriesFast(); iHit++) {
                BmnHit *hit = (BmnHit *) hits[i]->UncheckedAt(iHit);
                h[i][0][0]->Fill(hit->GetX(), hit->GetZ());
                h[i][1][0]->Fill(hit->GetY(), hit->GetZ());
            }
            //                                hitCount[i] += hits[i]->GetEntriesFast();

            if (tracks[1]->GetEntriesFast() < 2) {
                iEv++;
                continue;
            }
            if (i > 0) {
                printf("%d branch %d tracks\n", i, tracks[i]->GetEntriesFast());
                for (Int_t iTrack = 0; iTrack < tracks[i]->GetEntriesFast(); iTrack++) {
                    BmnGlobalTrack* track = (BmnGlobalTrack*) tracks[i]->UncheckedAt(iTrack);
                    if (track->GetSilTrackIndex() != -1) {
                        BmnTrack * silTr = (BmnTrack*) silTrack[i]->UncheckedAt(track->GetSilTrackIndex());
                        //                        if (silTr->GetNHits() < 4)
                        //                            continue;
                        hitCount[i] += silTr->GetNHits();
                        for (Int_t iHit = 0; iHit < silTr->GetNHits(); iHit++) {
                            BmnHit *hit = (BmnHit *) silHits[i]->UncheckedAt(silTr->GetHitIndex(iHit));
                            h[i][0][iTrack + 1]->Fill(hit->GetX(), hit->GetZ());
                            h[i][1][iTrack + 1]->Fill(hit->GetY(), hit->GetZ());
                        }
                    }
                    if (track->GetGemTrackIndex() != -1) {
                        BmnTrack * gemTr = (BmnTrack*) gemTrack[i]->UncheckedAt(track->GetGemTrackIndex());
                        //                        if (gemTr->GetNHits() < 4)
                        //                            continue;
                        hitCount[i] += gemTr->GetNHits();
                        for (Int_t iHit = 0; iHit < gemTr->GetNHits(); iHit++) {
                            BmnHit *hit = (BmnHit *) gemHits[i]->UncheckedAt(gemTr->GetHitIndex(iHit));
                            h[i][0][iTrack + 1]->Fill(hit->GetX(), hit->GetZ());
                            h[i][1][iTrack + 1]->Fill(hit->GetY(), hit->GetZ());
                        }
                    }
                    //                    FairTrackParam* first = track->GetParamFirst();
                    //                    h[i][2][iTrack]->Fill(first->GetTx());
                    //                    h[i][3][iTrack]->Fill(first->GetTy());
                }
            } else {
                for (Int_t iTrack = 0; iTrack < tracks[i]->GetEntriesFast(); iTrack++) {
                    CbmStsTrack* track = (CbmStsTrack*) tracks[i]->UncheckedAt(iTrack);
                    //                    if (track->GetNStsHits() < 4)
                    //                        continue;
                    hitCount[i] += track->GetNStsHits();
                    for (Int_t iHit = 0; iHit < track->GetNStsHits(); iHit++) {
                        CbmStsHit* hit = (CbmStsHit*) hits[i]->UncheckedAt(track->GetStsHitIndex(iHit));
                        h[i][0][iTrack + 1]->Fill(hit->GetX(), hit->GetZ());
                        h[i][1][iTrack + 1]->Fill(hit->GetY(), hit->GetZ());
                    }
                    //                    FairTrackParam* first = track->GetParamFirst();
                    //                    h[i][2][iTrack]->Fill(first->GetTx());
                    //                    h[i][3][iTrack]->Fill(first->GetTy());
                }
            }
        }
        if (/*hitCount[0] < EV_LIMIT || */hitCount[1] < EV_LIMIT || hitCount[2] < EV_LIMIT) {
            iEv++;
            continue;
        }
        if (ch != '\n')
            for (Int_t col = 0; col < COLS; col++)
                for (Int_t row = 0; row < ROWS; row++) {
                    TVirtualPad *pad = can->cd(row * 3 + col + 1);
                    pad->Clear();
                    //                if (row > 2)
                    //                    h[col][row]->Draw();
                    //                else
                    for (Int_t iTrack = 0; iTrack < nColors; iTrack++)
                        if (iTrack)
                            h[col][row][iTrack]->Draw("SAME");
                        else
                            h[col][row][iTrack]->Draw("");
                }
        can->Update();
        can->Modified();
        can->SaveAs(Form("can-%d.pdf", runID));
        cout << " Ev# " << iEv << endl;
        ch = getchar();
        switch (ch) {
            case 'a':
                iEv = (iEv == 0) ? iEv : iEv - 1;
                break;
            case 'd':
                iEv = (iEv < nEvs) ? iEv + 1 : iEv;
                break;
            default:
                //                iEv++;
                break;
        }
    } while (ch != 'q');


    for (Int_t i = 0; i < COLS; i++) {
        for (Int_t j = 0; j < ROWS; j++) {
            for (Int_t ic = 0; ic < nColors; ic++) {
                delete h[i][j][ic];
            }
        }
    }

    return;
}
