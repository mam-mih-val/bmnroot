#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TProfile.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TLegend.h"
#include "TSystem.h"
#include "TClonesArray.h"

using namespace std;

void testDCH(char *fname="bmn_run0166.root") {
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs();

    int run;
    sscanf(&fname[strlen(fname) - 8], "%d", &run);
    const char *mapping;
    if (run < 189) mapping = "DCH_map_Feb20_Feb25.txt";
    else mapping = "DCH_map_Mar4.txt";
    BmnDCHdetector DCH(mapping);
    cout << "RUN:  " << run << endl;
    DCH.print();

    TFile *_f = new TFile(fname, "READ");
    TTree *_t = (TTree *) _f->Get("BMN_RAW");

    TClonesArray *sync = new TClonesArray("BmnTDCDigit");
    TClonesArray *dch = new TClonesArray("BmnTDCDigit");
    TClonesArray *t0 = new TClonesArray("BmnTDCDigit");
    _t->SetBranchAddress("bmn_sync", &sync);
    _t->SetBranchAddress("bmn_dch", &dch);
    _t->SetBranchAddress("bmn_t0", &t0);

    /////////////////////////////////////////////////////////////////////////////////////
    TH1F * h[16];
    TH1F * t[16];
    for (int i = 0; i < 16; i++) {
        char str[100];
        sprintf(str, "Plane %s", DCH.getPlaneName(i));
        h[i] = new TH1F(str, str, 256, 0, 256);
        sprintf(str, "PlaneTime %s", DCH.getPlaneName(i));
        t[i] = new TH1F(str, str, 256, 0, 1000);
    }
    TH2F *Corr = new TH2F("corr", "corr", 100, 500, 600, 100, 500, 600);
    TH1F *T0 = new TH1F("t0", "t0", 200, 0, 500);
    /////////////////////////////////////////////////////////// //////////////////////////
    for (int ev = 0; ev < _t->GetEntries(); ev++) {
        if ((ev % 1000) == 0) printf("%d\n", ev);
        sync->Clear();
        dch->Clear();
        t0->Clear();
        _t->GetEntry(ev);
        DCH.fillEvent(dch, sync, t0);
        if (DCH.get_t0() == 0) continue;

        int tm[16], ch[16];
        for (int p = 0; p < 16; p++) tm[p] = ch[p] = 0;

        for (int p = 0; p < 16; p++)for (int c = 0; c < 256; c++) {
                unsigned short *d;
                int nHits = DCH.get(p, c, &d);
                if (nHits > 0) {
                    h[p]->Fill(c);
                    t[p]->Fill(d[0]);
                    tm[p] = d[0];
                    ch[p] = c;
                }
            }
        T0->Fill(DCH.get_t0());
        for (int p = 0; p < 16; p += 2) {
            if (DCH.getnhits(p) == 1 && DCH.getnhits(p + 1) == 1 && DCH.get_t0() > 0) {
                if (ch[p] == ch[p + 1]) Corr->Fill(tm[p], tm[p + 1]);
            }
        }
    }
    /////////////////////////////////////// /////////////////////////////////////////////
    Corr->Draw("COLZ");
    //T0->Draw();

    TLegend * l[8];
    for (int i = 0; i < 8; i++) {
        char str[100];
        l[i] = new TLegend(0.1, 0.7, 0.3, 0.9);
        l[i]->AddEntry(h[i * 2], h[i * 2]->GetName());
        l[i]->AddEntry(h[i * 2 + 1], h[i * 2 + 1]->GetName());
    }
    for (int i = 0; i < 16; i++) {
        h[i]->SetLineWidth(2);
        if ((i % 2) == 0) h[i]->SetLineColor(kRed);
        else h[i]->SetLineColor(kBlue);
        h[i]->GetXaxis()->SetTitle("Channel");
        h[i]->GetXaxis()->SetNdivisions(516, kFALSE);
        t[i]->SetLineWidth(2);
        if ((i % 2) == 0) t[i]->SetLineColor(kRed);
        else t[i]->SetLineColor(kBlue);
        t[i]->GetXaxis()->SetTitle("Channel");
    }
    TCanvas *c0 = new TCanvas("c00", "c00");
    c0->Divide(2, 2);
    for (int i = 1; i < 5; i++) {
        c0->cd(i)->SetLogy();
        c0->cd(i)->SetGridx();
    }
    c0->cd(1);
    h[0]->Draw();
    h[1]->Draw("SameS");
    l[0]->Draw();
    c0->cd(2);
    h[2]->Draw();
    h[3]->Draw("SameS");
    l[1]->Draw();
    c0->cd(3);
    h[4]->Draw();
    h[5]->Draw("SameS");
    l[2]->Draw();
    c0->cd(4);
    h[6]->Draw();
    h[7]->Draw("SameS");
    l[3]->Draw();
    TCanvas *c1 = new TCanvas("c01", "c00");
    c1->Divide(2, 2);
    for (int i = 1; i < 5; i++) {
        c1->cd(i)->SetLogy();
        c1->cd(i)->SetGridx();
    }
    c1->cd(1);
    h[8]->Draw();
    h[9]->Draw("SameS");
    l[4]->Draw();
    c1->cd(2);
    h[10]->Draw();
    h[11]->Draw("SameS");
    l[5]->Draw();
    c1->cd(3);
    h[12]->Draw();
    h[13]->Draw("SameS");
    l[6]->Draw();
    c1->cd(4);
    h[14]->Draw();
    h[15]->Draw("SameS");
    l[7]->Draw();

    TCanvas *c2 = new TCanvas("c20", "c00");
    c2->Divide(2, 2);
    for (int i = 1; i < 5; i++) {
        c2->cd(i)->SetGridx();
    }
    c2->cd(1);
    t[0]->Draw();
    t[1]->Draw("SameS");
    l[0]->Draw();
    c2->cd(2);
    t[2]->Draw();
    t[3]->Draw("SameS");
    l[1]->Draw();
    c2->cd(3);
    t[4]->Draw();
    t[5]->Draw("SameS");
    l[2]->Draw();
    c2->cd(4);
    t[6]->Draw();
    t[7]->Draw("SameS");
    l[3]->Draw();
    TCanvas *c3 = new TCanvas("c30", "c00");
    c3->Divide(2, 2);
    for (int i = 1; i < 5; i++) {
        c3->cd(i)->SetGridx();
    }
    c3->cd(1);
    t[8]->Draw();
    t[9]->Draw("SameS");
    l[4]->Draw();
    c3->cd(2);
    t[10]->Draw();
    t[11]->Draw("SameS");
    l[5]->Draw();
    c3->cd(3);
    t[12]->Draw();
    t[13]->Draw("SameS");
    l[6]->Draw();
    c3->cd(4);
    t[14]->Draw();
    t[15]->Draw("SameS");
    l[7]->Draw();

}
