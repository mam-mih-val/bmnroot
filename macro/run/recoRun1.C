
#include "TMath.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TChain.h"

using namespace TMath;

void recoRun1() {

    /* Load basic libraries */
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load bmn libraries

    TChain *bmnTree = new TChain("BMN_DIGIT");
    //    bmnTree->Add("/home/merz/bmn_run0607_digit.root");
    bmnTree->Add("bmn_run0258_digit.root");

    TClonesArray *dchDigits;
    bmnTree->SetBranchAddress("bmn_dch_digit", &dchDigits);

    Int_t events = bmnTree->GetEntries();
    cout << "N events = " << events << endl;
    TClonesArray* hits = new TClonesArray("BmnDchHit");

    UInt_t nBins = 600;
    Float_t bound = InnerRadiusOfOctagon;

    TH1F* hu1 = new TH1F("hu_DCH1", "hu_DCH1", nBins, -bound, bound);
    TH1F* hv1 = new TH1F("hv_DCH1", "hv_DCH1", nBins, -bound, bound);
    TH1F* hx1 = new TH1F("hx_DCH1", "hx_DCH1", nBins, -bound, bound);
    TH1F* hy1 = new TH1F("hy_DCH1", "hy_DCH1", nBins, -bound, bound);

    TH1F* hu2 = new TH1F("hu_DCH2", "hu_DCH2", nBins, -bound, bound);
    TH1F* hv2 = new TH1F("hv_DCH2", "hv_DCH2", nBins, -bound, bound);
    TH1F* hx2 = new TH1F("hx_DCH2", "hx_DCH2", nBins, -bound, bound);
    TH1F* hy2 = new TH1F("hy_DCH2", "hy_DCH2", nBins, -bound, bound);

    TH2F* hxy1 = new TH2F("hxy_DCH1", "hxy_DCH1", nBins, -bound, bound, nBins, -bound, bound);
    TH2F* huv1 = new TH2F("huv_DCH1", "huv_DCH1", nBins, -bound, bound, nBins, -bound, bound);
    TH2F* hxy2 = new TH2F("hxy_DCH2", "hxy_DCH2", nBins, -bound, bound, nBins, -bound, bound);
    TH2F* huv2 = new TH2F("huv_DCH2", "huv_DCH2", nBins, -bound, bound, nBins, -bound, bound);

    TH2F* hxu1 = new TH2F("hxu1", "hxu1", nBins, -bound, bound, nBins, -bound, bound);
    TH2F* hyv1 = new TH2F("hyv1", "hyv1", nBins, -bound, bound, nBins, -bound, bound);
    TH2F* hxu2 = new TH2F("hxu2", "hxu2", nBins, -bound, bound, nBins, -bound, bound);
    TH2F* hyv2 = new TH2F("hyv2", "hyv2", nBins, -bound, bound, nBins, -bound, bound);

    TH2F* hzx = new TH2F("hzx", "hzx", nBins, 540, 660, nBins, -bound, bound);

    Float_t angle = -45.0 * DegToRad();

    for (Int_t iEv = 0; iEv < events; iEv++) {
        bmnTree->GetEntry(iEv);
        hits->Clear();
        cout << "Event: " << iEv + 1 << "/" << events << endl;
        ProcessEvent(dchDigits, hits);
        for (Int_t i = 0; i < hits->GetEntriesFast(); ++i) {
            BmnDchHit* hit = (BmnDchHit*) hits->At(i);
            Float_t x = hit->GetX();
            Float_t y = hit->GetY();
            Float_t z = hit->GetZ();
            UInt_t lay = hit->GetLayer();
            if (lay == 1) {
                hx1->Fill(x);
                hy1->Fill(y);
                hxy1->Fill(x, y);
            }
            if (lay == 0) {
                hu1->Fill(x);
                hv1->Fill(y);
                huv1->Fill(x, y);
            }
            if (lay == 3) {
                hx2->Fill(x);
                hy2->Fill(y);
                hxy2->Fill(x, y);
            }
            if (lay == 2) {
                hu2->Fill(x);
                hv2->Fill(y);
                huv2->Fill(x, y);
            }
            hzx->Fill(z, x);
//            for (Int_t j = 0; j < hits->GetEntriesFast(); ++j) {
//                BmnDchHit* hit1 = (BmnDchHit*) hits->At(j);
//                Float_t x1 = hit1->GetX();
//                Float_t y1 = hit1->GetY();
//
//                Float_t xx = x1 * Cos(angle) - y1 * Sin(angle);
//                Float_t yy = x1 * Sin(angle) + y1 * Cos(angle);
//                
//                UInt_t lay1 = hit1->GetLayer();
//
//                if (lay == 1 && lay1 == 0) {
//                    hxu1->Fill(x, xx);
//                    hyv1->Fill(y, yy);
//                }
//                if (lay == 3 && lay1 == 2) {
//                    hxu2->Fill(x, xx);
//                    hyv2->Fill(y, yy);
//                }
//            }
        }
    } // event loop
    TCanvas* c1 = new TCanvas("c1", "c1", 1600, 800);
    c1->Divide(2, 2);
    c1->cd(1)->SetLogy();
    hx1->Draw();
    c1->cd(2)->SetLogy();
    hy1->Draw();
    c1->cd(3)->SetLogy();
    hu1->Draw();
    c1->cd(4)->SetLogy();
    hv1->Draw();
    c1->SaveAs("prof_DCH1.png");

    TCanvas* c2 = new TCanvas("c2", "c2", 1600, 800);
    c2->Divide(2, 2);
    c2->cd(1)->SetLogy();
    hx2->Draw();
    c2->cd(2)->SetLogy();
    hy2->Draw();
    c2->cd(3)->SetLogy();
    hu2->Draw();
    c2->cd(4)->SetLogy();
    hv2->Draw();
    c2->SaveAs("prof_DCH2.png");

    TCanvas* c3 = new TCanvas("c3", "c3", 1000, 1000);
    c3->Divide(2, 2);
    c3->cd(1)->SetLogz();
    hxy1->Draw("colz");
    c3->cd(2)->SetLogz();
    huv1->Draw("colz");
    c3->cd(3)->SetLogz();
    hxy2->Draw("colz");
    c3->cd(4)->SetLogz();
    huv2->Draw("colz");
    c3->SaveAs("2D.png");

//    TCanvas* c4 = new TCanvas("c4", "c4", 1600, 800);
//    c4->Divide(2, 2);
//    c4->cd(1)->SetLogz();
//    hxu1->Draw("colz");
//    c4->cd(2)->SetLogz();
//    hyv1->Draw("colz");
//    c4->cd(3)->SetLogz();
//    hxu2->Draw("colz");
//    c4->cd(4)->SetLogz();
//    hyv2->Draw("colz");
//    c4->SaveAs("Corr.png");

    TCanvas* c5 = new TCanvas("c5", "c5", 1600, 800);
    hzx->Draw("colz");
    c5->SaveAs("ZX.png");
}
