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

void LineFit3D(vector<BmnHit*> hits, TVector3& vertex, TVector3& direction) {

    const Int_t nHits = hits.size();

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
        cout << "WARNING: All its on the same DCH" << endl;
        vertex = TVector3(0.0, 0.0, 0.0);
        direction = TVector3(0.0, 0.0, 0.0);
        return;;
    }

    if (nHits < 3) {
        cout << "WARNING: not enough hits for tracking! (Number of hits is " << nHits << ")" << endl;
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
    Float_t ZV = ((BmnHit*) hits.at(0))->GetZ(); //Z-coordinate of vertex
    Float_t ZN = ((BmnHit*) hits.at(nHits - 1))->GetZ();
    Float_t Az = (ZN - ZV);
    for (Int_t i = 0; i < nHits; ++i) {
        BmnHit* hit = (BmnHit*) hits.at(i);
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
    Float_t koef = 1.0 / (SumC2 * SumW - SumC * SumC);
    Float_t XV = (SumXW * SumC2 - SumC * SumXWC) * koef;
    Float_t YV = (SumYW * SumC2 - SumC * SumYWC) * koef;
    Float_t Ax = (SumXWC * SumW - SumC * SumXW) * koef;
    Float_t Ay = (SumYWC * SumW - SumC * SumYW) * koef;

    vertex = TVector3(XV, YV, ZV);
    direction = TVector3(Ax, Ay, Az);
}

void FindSeed(TClonesArray* hits, TClonesArray* tracks) {

    const Int_t nBins = 400;
    const Int_t nHits = hits->GetEntriesFast();
    Float_t minY = -0.4;
    Float_t maxY = -minY;
    Float_t width = (maxY - minY) / nBins;

    TH1F* h = new TH1F("yOverR", "yOverR", nBins, minY, maxY);

    for (Int_t iHit = 0; iHit < nHits; ++iHit) {
        BmnHit* hit = (BmnHit*) hits->At(iHit);
        const Float_t R = Sqrt(hit->GetX() * hit->GetX() + hit->GetY() * hit->GetY() + hit->GetZ() * hit->GetZ());
        const Float_t newX = hit->GetX() / R;
        const Float_t newY = hit->GetY() / R;
        Int_t xAddr = ceil((newX - minY) / width);
        Int_t yAddr = ceil((newY - minY) / width);
        Long_t addr = yAddr * nBins + xAddr;
        hit->SetAddr(addr);
        hit->SetXaddr(xAddr);
        hit->SetYaddr(yAddr);
        h->Fill(newY);
        hit->SetIndex(iHit);
    }

    for (Int_t iBin = 0; iBin < h->GetNbinsX(); ++iBin) {
        vector<BmnHit*> hitsInTrack;
        for (Int_t iHit = 0; iHit < nHits; ++iHit) {
            BmnHit* hit = (BmnHit*) hits->At(iHit);
            if (hit->IsUsed()) continue;
            if (hit->GetYaddr() == iBin) {
                hit->SetUsing(kTRUE);
                hitsInTrack.push_back(hit);
            }
        }
        TVector3 vertex;
        TVector3 direction;
        if (hitsInTrack.size() > 2) {
            LineFit3D(hitsInTrack, vertex, direction);
            //            new((*tracks)[tracks->GetEntriesFast()]) CbmMCTrack(/*pdg*/ 0, /*motherId*/ -1, direction.X(), direction.Y(), direction.Z(), vertex.X(), vertex.Y(), vertex.Z(), /*start time*/0.0, hitsInTrack.size());
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

    delete h;
}

#endif	/* BMNTRACKFINDERRUN1_H */

