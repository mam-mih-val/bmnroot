#include "TRandom3.h"
#include "TH2F.h"
#include "TH1F.h"
#include "TGraph.h"

const Int_t kN = 6;
const Int_t kM = 49;
const Int_t my[kN] = {1, 2, 3, 4, 5, 6};
//const Int_t my[kN] = {1, 2};

TH1F* randoms = new TH1F("randoms", "randoms", kM, 1, kM);
TH2F* slope = new TH2F("slope", "slope", 500, 1, 1000000, 500, 1, 1000);
TGraph* gr = new TGraph();
Int_t nGood = 0;
Int_t nAll = 0;

Bool_t check(Int_t* lot) {
    Bool_t ok[kN];
    for (Int_t i = 0; i < kN; ++i) {
        ok[i] = kFALSE;
        for (Int_t j = 0; j < kN; ++j) {
            if (lot[i] == my[j]) {
                ok[i] = kTRUE;
                break;
            }
        }
        if (!ok[i]) return kFALSE;
    }
    return kTRUE;
}

void prob6of49() {
    Int_t cntr = 0;
    while (cntr < 1e9) {
        cntr++;
        TRandom3 r(0); // generate a number in interval ]0,1] (0 is excluded)
        r.Rndm();
        Float_t x[kN];
        Int_t lot[kN];
        r.RndmArray(kN, x); // generate an array of random numbers in ]0,1]
        for (Int_t i = 0; i < kN; ++i) {
            lot[i] = Int_t(x[i] * kM);
        }
        Bool_t copy = kFALSE;
        for (Int_t i = 0; i < kN; ++i) {
            for (Int_t j = 0; j < kN; ++j) {
                if ((lot[i] == lot[j]) && (i != j)) {
                    copy = kTRUE;
                    break;
                }
            }
            if (copy) break;
//            cout << lot[i] << " ";
        }
        if (copy) continue;
//        cout << endl;
        nAll++;
        for (Int_t i = 0; i < kN; ++i) {
            randoms->Fill(lot[i]);
        }
        if (check(lot)) {
            cout << "OK! ";
            for (Int_t i = 0; i < kN; ++i) {
                cout << lot[i] << " ";
            }
            cout << endl;
            nGood++;
            cout << nAll << " " << nGood << " " << nGood * 100.0 / nAll << endl;
            slope->Fill(nAll, nGood);
            gr->SetPoint(nGood, nAll, nGood * 100.0 / nAll);
        }
    }
    TCanvas* c1 = new TCanvas("c1", "c1", 500, 500);
    randoms->Draw();
    TCanvas* c2 = new TCanvas("c2", "c2", 500, 500);
    slope->Draw("colz");
    TCanvas* c3 = new TCanvas("c3", "c3", 500, 500);
    gr->Draw("Al");
    
}

