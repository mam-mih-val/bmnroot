#ifndef BMNTRACKFINDERRUN1_H
#define	BMNTRACKFINDERRUN1_H

#include "FairTrackParam.h"
#include "TVector3.h"
#include "CbmMCTrack.h"
#include "CbmTrack.h"
#include "TClonesArray.h"
#include "BmnHit.h"
#include "TH1.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TLine.h"
#include "BmnEnums.h"
#include "TMath.h"

using namespace std;
using namespace TMath;

TVector3 LineFit(TClonesArray* hits) {

    //Least Square Method//
    Float_t Xi = 0.0, Zi = 0.0; // coordinates of current track point
    Float_t a = 0.0, b = 0.0; // parameters of line: x = a * z + b
    Float_t SumX = 0.0, SumZ = 0.0, SumXZ = 0.0, SumZ2 = 0.0;
    const Int_t nHits = hits->GetEntriesFast();
    for (Int_t i = 0; i < nHits; ++i) {
        BmnHit* hit = (BmnHit*) hits->At(i);
        //        cout << "x = " << hit->GetX() << " z = " << hit->GetZ() << endl;
        Xi = hit->GetX();
        Zi = hit->GetZ();
        SumX += Xi;
        SumZ += Zi;
        SumXZ += Xi * Zi;
        SumZ2 += Zi * Zi;
    }

    a = (nHits * SumXZ - SumX * SumZ) / (nHits * SumZ2 - SumZ * SumZ);
    b = (SumX - a * SumZ) / nHits;

    return TVector3(a, b, 0.0);
}

void LineFit3D(vector<BmnDchHit*> hits, TVector3& vertex, TVector3& direction) {

    const Int_t nHits = hits.size();

    if (nHits < 3) {
        cout << "WARNING: not enough hits for tracking! (Number of hits is " << nHits << ")" << endl;
        vertex = TVector3(0.0, 0.0, 0.0);
        direction = TVector3(0.0, 0.0, 0.0);
        return;
    }

    //Rough checking for hits. They shouldn't be on the same DCH
    BmnDchHit* hit0 = (BmnDchHit*) hits.at(0);
    UInt_t dchId0 = hit0->GetLayer();
    Bool_t flag = kFALSE;
    for (Int_t i = 1; i < hits.size(); ++i) {
        BmnDchHit* hit = (BmnDchHit*) hits.at(i);
        UInt_t dchId = hit->GetDchId();
        if (dchId != dchId0) {
            flag = kTRUE;
            break;
        }
    }
    if (!flag) {
        cout << "WARNING: All hits on the same DCH" << endl;
        vertex = TVector3(0.0, 0.0, 0.0);
        direction = TVector3(0.0, 0.0, 0.0);
        return;
    }

    Float_t Xi = 0.0, Yi = 0.0, Zi = 0.0; // coordinates of current track point
    Float_t Ci;
    Float_t SumXW = 0.0, SumYW = 0.0;
    Float_t SumXWC = 0.0, SumYWC = 0.0;
    Float_t SumW = 0.0;
    Float_t SumC = 0.0, SumC2 = 0.0;
    Float_t Wi = 1.0 / nHits; // weight    
    Float_t ZV = ((BmnDchHit*) hits.at(0))->GetZ(); //Z-coordinate of vertex
    Float_t ZN = ((BmnDchHit*) hits.at(nHits - 1))->GetZ();
    Float_t Az = (ZN - ZV);
    if (Az == 0.0) {
        vertex = TVector3(0.0, 0.0, 0.0);
        direction = TVector3(0.0, 0.0, 0.0);
        return;
    }
    for (Int_t i = 0; i < nHits; ++i) {
        BmnDchHit* hit = (BmnDchHit*) hits.at(i);
        if (hit == NULL) continue;
        Xi = hit->GetX();
        Yi = hit->GetY();
        Zi = hit->GetZ();
        if (Zi < 0.0) {
            cout << "WARNING: bad hit!" << endl;
            //            hit->Print();
        }
        Ci = (Zi - ZV) / Az;
        SumXW += (Xi * Wi);
        SumYW += (Yi * Wi);
        SumXWC += (Xi * Wi * Ci);
        SumYWC += (Yi * Wi * Ci);
        SumC += (Ci * Wi);
        SumC2 += (Ci * Ci * Wi);
        SumW += Wi;
    }
    Float_t koef = (SumC2 * SumW - SumC * SumC);
    if (koef == 0.0) {
        vertex = TVector3(0.0, 0.0, 0.0);
        direction = TVector3(0.0, 0.0, 0.0);
        return;
    }
    Float_t XV = (SumXW * SumC2 - SumC * SumXWC) / koef;
    Float_t YV = (SumYW * SumC2 - SumC * SumYWC) / koef;
    Float_t Ax = (SumXWC * SumW - SumC * SumXW) / koef;
    Float_t Ay = (SumYWC * SumW - SumC * SumYW) / koef;

    vertex = TVector3(XV, YV, ZV);
    direction = TVector3(Ax, Ay, Az);
    vertex.Print();
    direction.Print();
}

BmnStatus FindSeed(TClonesArray* hits, TClonesArray* tracks) {

    Int_t hitsThresh = 4;

    const Int_t nBins = 200;
    const Int_t nHits = hits->GetEntriesFast();
    Float_t minY = -0.4;
    Float_t maxY = -minY;
    Float_t width = (maxY - minY) / nBins;

    TH1F* h = new TH1F("yOverR", "yOverR", nBins, minY, maxY);

    if (nHits < hitsThresh) {
        cout << "WARNING: Too few hits: Nhits = " << nHits << endl;
        return kBMNERROR;
    }
    Bool_t lay0 = kFALSE;
    Bool_t lay1 = kFALSE;
    Bool_t lay2 = kFALSE;
    Bool_t lay3 = kFALSE;

    for (Int_t i = 0; i < hits->GetEntriesFast(); ++i) {
        BmnDchHit* hit = (BmnDchHit*) hits->At(i);
        UInt_t lay = hit->GetLayer();
        switch (lay) {
            case 0: lay0 = kTRUE;
                break;
            case 1: lay1 = kTRUE;
                break;
            case 2: lay2 = kTRUE;
                break;
            case 3: lay3 = kTRUE;
                break;
        }
    }
    cout << lay0 << " " << lay1 << " " << lay2 << " " << lay3 << endl;

    if (!(lay0 && lay1 && lay2 && lay3)) {
        cout << "Hits for track not presented on each plane of DCHs" << endl;
        return kBMNERROR;
    }

    for (Int_t iHit = 0; iHit < nHits; ++iHit) {
        BmnDchHit* hit = (BmnDchHit*) hits->At(iHit);
        const Float_t R = Sqrt(hit->GetX() * hit->GetX() + hit->GetY() * hit->GetY() + hit->GetZ() * hit->GetZ());
        const Float_t newX = hit->GetX() / R;
        const Float_t newY = hit->GetY() / R;
        //        Int_t xAddr = ceil((newX - minY) / width);
        Int_t yAddr = ceil((newY - minY) / width);
        //        Long_t addr = yAddr * nBins + xAddr;
        //        hit->SetAddr(addr);
        //        hit->SetXaddr(xAddr);
        hit->SetFlag(yAddr); //tmp storage
        h->Fill(newY);
        hit->SetIndex(iHit);
    }

    vector<Int_t> peaks; // vector of connected bins
    for (Int_t iBin = 0; iBin < h->GetNbinsX(); iBin += (peaks.size() + 1)) {
        peaks.clear();
        peaks.resize(0);
        if (h->GetBinContent(iBin) > 0.0) {
            peaks.push_back(iBin);
            for (Int_t iBinPeak = iBin + 1; iBinPeak < h->GetNbinsX(); ++iBinPeak) {
                if (h->GetBinContent(iBinPeak) > 0.0) {
                    peaks.push_back(iBinPeak);
                } else {
                    break;
                }
            }
        }

        vector<BmnDchHit*> hitsInTrack;
        for (Int_t iBinPeak = 0; iBinPeak < peaks.size(); ++iBinPeak) {
            for (Int_t iHit = 0; iHit < nHits; ++iHit) {
                BmnDchHit* hit = (BmnDchHit*) hits->At(iHit);
                if (hit->IsUsed()) continue;
                if (hit->GetFlag() == peaks.at(iBinPeak)) {
                    hit->SetUsing(kTRUE);
                    hitsInTrack.push_back(hit);
                }
            }
        }
        TVector3 vertex;
        TVector3 direction;

        if (hitsInTrack.size() >= hitsThresh) {
            cout << "Hits in track: " << endl;
//            for (Int_t iHit = 0; iHit < hitsInTrack.size(); ++iHit) {
//                BmnDchHit* hit = hitsInTrack.at(iHit);
//                cout << hit->GetX() << " " << hit->GetY() << " " << hit->GetZ() << endl;
//            }

            LineFit3D(hitsInTrack, vertex, direction);
            if (direction.Mag() == 0.0 && direction.Mag() == 0.0) continue;
            new((*tracks)[tracks->GetEntriesFast()]) CbmTrack();
            CbmTrack* track = (CbmTrack*) tracks->At(tracks->GetEntriesFast() - 1);
            for (Int_t iHit = 0; iHit < hitsInTrack.size(); ++iHit) {
                track->AddHit(hitsInTrack.at(iHit)->GetIndex(), HitType(0));
                FairTrackParam* par = new FairTrackParam();
                par->SetPosition(vertex);
                par->SetTx(direction.X() / direction.Z());
                par->SetTy(direction.Y() / direction.Z());
                track->SetParamFirst(par);
            }
        }
    }

    TCanvas* c12 = new TCanvas("c12", "c12", 1600, 800);
    h->Draw("");
    c12->SaveAs("yR.png");
    delete h;
    return kBMNSUCCESS;
}


#endif	/* BMNTRACKFINDERRUN1_H */

