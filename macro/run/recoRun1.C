#include "TMath.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TChain.h"
#include "TClonesArray.h"
#include <vector>
#include "TVector3.h"

using namespace TMath;

void recoRun1() {

    /* Load basic libraries */
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load bmn libraries

    TChain *bmnTree = new TChain("BMN_DIGIT");
    //    bmnTree->Add("/home/merz/bmn_run0607_digit.root");
    bmnTree->Add("bmn_run0166_digit.root");

    TClonesArray *dchDigits;
    bmnTree->SetBranchAddress("bmn_dch_digit", &dchDigits);

    Int_t events = bmnTree->GetEntries();
    cout << "N events = " << events << endl;
    TClonesArray* hits = new TClonesArray("BmnDchHit");

    UInt_t nBins = 300;
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

    Float_t zMin = 540;
    Float_t zMax = 660;
    TH2F* hzx = new TH2F("hzx", "hzx", nBins, zMin, zMax, nBins, -bound, bound);
    Float_t bound1 = 0.2;
    TH2F* hxyR_1 = new TH2F("hxyR_1", "hxyR_1", nBins, -bound1, bound1, nBins, -bound1, bound1);
    TH2F* hxyR_2 = new TH2F("hxyR_2", "hxyR_2", nBins, -bound1, bound1, nBins, -bound1, bound1);

    Float_t angle = 45.0 * DegToRad();

    Float_t AlignmentDeltaX = 3.75; //very rough!!!
    TVector3 params;

    for (Int_t iEv = 0; iEv < events; iEv++) {
        bmnTree->GetEntry(iEv);
        hits->Clear();
        cout << "Event: " << iEv + 1 << "/" << events << endl;
        ProcessEvent(dchDigits, hits);
        if (hits->GetEntriesFast() != 4) continue;

        UInt_t prevLay = 100000;
        Bool_t flag = kTRUE;
        for (Int_t i = 0; i < hits->GetEntriesFast(); ++i) {
            BmnDchHit* hit = (BmnDchHit*) hits->At(i);
            UInt_t lay = hit->GetLayer();
            if (lay == prevLay) {
                flag = kFALSE;
                break;
            }
            prevLay = lay;
        }
        if (!flag) continue;
        for (Int_t i = 0; i < hits->GetEntriesFast(); ++i) {
            BmnDchHit* hit = (BmnDchHit*) hits->At(i);
            Float_t x = hit->GetX();
            Float_t y = hit->GetY();
            Float_t z = hit->GetZ();
            Float_t R = Sqrt(x * x + y * y + z * z);
            Float_t xR = x / R;
            Float_t yR = y / R;
            Float_t zR = z / R;

            UInt_t lay = hit->GetLayer();
            if (lay == 1) {
                hx1->Fill(x);
                hy1->Fill(y);
                hxy1->Fill(x, y);
                hxyR_1->Fill(xR, yR);
                hzx->Fill(z, x);
            }
            if (lay == 0) {
                hu1->Fill(x);
                hv1->Fill(y);
                huv1->Fill(x, y);
                hxyR_1->Fill(xR, yR);
                hzx->Fill(z, x);
            }
            if (lay == 3) {
                x += AlignmentDeltaX;
                hit->SetX(x);
                hx2->Fill(x);
                hy2->Fill(y);
                hxy2->Fill(x, y);
                hxyR_2->Fill(xR, yR);
                hxyR_1->Fill(xR, yR);
                hzx->Fill(z, x);
            }
            if (lay == 2) {
                x += AlignmentDeltaX;
                hit->SetX(x);
                hu2->Fill(x);
                hv2->Fill(y);
                huv2->Fill(x, y);
                hxyR_2->Fill(xR, yR);
                hxyR_1->Fill(xR, yR);
                hzx->Fill(z, x);
            }
        }
        params = LineFit(hits);
        //break;
        //params.Print();
    } // event loop
    
    Draw2x2histo(hx1, hy1, hu1, hv1, "prof_DCH1");
    Draw2x2histo(hx2, hy2, hu2, hv2, "prof_DCH2");
    Draw2x2histo(hxy1, huv1, hxy2, huv2, "2D");

    TCanvas* c5 = new TCanvas("c5", "c5", 1600, 800);
    hzx->Draw("colz");
    Float_t a = params.X();
    Float_t b = params.Y();
    //cout << "a = " << a << " b = " << b << endl;
    TLine* line = new TLine(zMin, a * zMin + b, zMax, a * zMax + b);
    line->SetLineColor(kBlue);
    line->Draw();
    c5->SaveAs("ZX.png");

    TCanvas* c6 = new TCanvas("c6", "c6", 1600, 800);
    c6->Divide(2, 1);
    c6->cd(1);
    hxyR_1->Draw("colz");
    c6->cd(2);
    hxyR_2->Draw("colz");
    c6->SaveAs("xyR.png");
}

void Draw2x2histo(TH1F* h1, TH1F* h2, TH1F* h3, TH1F* h4, TString name) {
    TCanvas* c1 = new TCanvas(name, name, 800, 800);
    c1->Divide(2, 2);
    c1->cd(1)->SetLogy();
    h1->Draw();
    c1->cd(2)->SetLogy();
    h2->Draw();
    c1->cd(3)->SetLogy();
    h3->Draw();
    c1->cd(4)->SetLogy();
    h4->Draw();
    c1->SaveAs(name + ".png");
}

void Draw2x2histo(TH2F* h1, TH2F* h2, TH2F* h3, TH2F* h4, TString name) {
    TCanvas* c1 = new TCanvas(name, name, 800, 800);
    c1->Divide(2, 2);
    c1->cd(1)->SetLogz();
    h1->Draw("colz");
    c1->cd(2)->SetLogz();
    h2->Draw("colz");
    c1->cd(3)->SetLogz();
    h3->Draw("colz");
    c1->cd(4)->SetLogz();
    h4->Draw("colz");
    c1->SaveAs(name + ".png");
}
