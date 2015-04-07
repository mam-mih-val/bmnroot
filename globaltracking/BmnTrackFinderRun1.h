#ifndef BMNTRACKFINDERRUN1_H
#define	BMNTRACKFINDERRUN1_H

#include "TVector3.h"
#include "TClonesArray.h"
#include "BmnHit.h"

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

void LineFit3D(TClonesArray* hits, TVector3& vertex, TVector3& direction) {

    Float_t Xi = 0.0, Yi = 0.0, Zi = 0.0; // coordinates of current track point
    Float_t Ci;
    Float_t SumXW = 0.0, SumYW = 0.0;
    Float_t SumXWC = 0.0, SumYWC = 0.0;
    Float_t SumW = 0.0;
    Float_t SumC = 0.0, SumC2 = 0.0;
    const Int_t nHits = hits->GetEntriesFast();
    Float_t Wi = 1.0 / nHits; // weight    
    Float_t ZV = ((BmnHit*) hits->At(0))->GetZ(); //Z-coordinate of vertex
    Float_t ZN = ((BmnHit*) hits->At(nHits - 1))->GetZ();
    Float_t Az = (ZN - ZV);
    for (Int_t i = 0; i < nHits; ++i) {
        BmnHit* hit = (BmnHit*) hits->At(i);
        Xi = hit->GetX();
        Yi = hit->GetY();
        Zi = hit->GetZ();
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

#endif	/* BMNTRACKFINDERRUN1_H */

