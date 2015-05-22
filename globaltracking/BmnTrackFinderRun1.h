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

//TVector3 LineFit(TClonesArray* hits) {
//
//    //Least Square Method//
//    Float_t Xi = 0.0, Zi = 0.0; // coordinates of current track point
//    Float_t a = 0.0, b = 0.0; // parameters of line: x = a * z + b
//    Float_t SumX = 0.0, SumZ = 0.0, SumXZ = 0.0, SumZ2 = 0.0;
//    const Int_t nHits = hits->GetEntriesFast();
//    for (Int_t i = 0; i < nHits; ++i) {
//        BmnHit* hit = (BmnHit*) hits->At(i);
//        //        cout << "x = " << hit->GetX() << " z = " << hit->GetZ() << endl;
//        Xi = hit->GetX();
//        Zi = hit->GetZ();
//        SumX += Xi;
//        SumZ += Zi;
//        SumXZ += Xi * Zi;
//        SumZ2 += Zi * Zi;
//    }
//
//    a = (nHits * SumXZ - SumX * SumZ) / (nHits * SumZ2 - SumZ * SumZ);
//    b = (SumX - a * SumZ) / nHits;
//
//    return TVector3(a, b, 0.0);
//}

Float_t LineFit3D(vector<BmnDchHit*> hits, TVector3& vertex, TVector3& direction) {

    //    cout << "LineFit3D started!" << endl;
    const Int_t nHits = hits.size();

    if (nHits < 3) {
        cout << "WARNING: not enough hits for tracking! (Number of hits is " << nHits << ")" << endl;
        vertex = TVector3(0.0, 0.0, 0.0);
        direction = TVector3(0.0, 0.0, 0.0);
        return 1000.0;
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
        return 1000.0;
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
        cout << "Az = 0.0" << endl;
        vertex = TVector3(0.0, 0.0, 0.0);
        direction = TVector3(0.0, 0.0, 0.0);
        return 1000.0;
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
        cout << "koef = 0.0" << endl;
        vertex = TVector3(0.0, 0.0, 0.0);
        direction = TVector3(0.0, 0.0, 0.0);
        return 1000.0;
    }
    Float_t XV = (SumXW * SumC2 - SumC * SumXWC) / koef;
    Float_t YV = (SumYW * SumC2 - SumC * SumYWC) / koef;
    Float_t Ax = (SumXWC * SumW - SumC * SumXW) / koef;
    Float_t Ay = (SumYWC * SumW - SumC * SumYW) / koef;

    vertex = TVector3(XV, YV, ZV);
    direction = TVector3(Ax, Ay, Az);
    //    vertex.Print();
    //    direction.Print();

    //chi2 calculation

    Float_t sumChi2xy = 0.0;
    Float_t sumChi2xz = 0.0;
    Float_t sumChi2yz = 0.0;
    for (Int_t i = 0; i < nHits; ++i) {
        BmnDchHit* hit = (BmnDchHit*) hits.at(i);
        if (hit == NULL) continue;
        Xi = hit->GetX();
        Yi = hit->GetY();
        Zi = hit->GetZ();
        //XY
        Float_t Y_Xi = Ay / Ax * (Xi - XV) + YV;
        Float_t X_Zi = Ax / Az * (Zi - ZV) + XV;
        Float_t Y_Zi = Ay / Az * (Zi - ZV) + YV;
        sumChi2xy += ((Y_Xi - Yi) * (Y_Xi - Yi) / Yi / Yi);
        sumChi2xz += ((X_Zi - Xi) * (X_Zi - Xi) / Xi / Xi);
        sumChi2yz += ((Y_Zi - Yi) * (Y_Zi - Yi) / Yi / Yi);
    }
    sumChi2xy /= (nHits - 1);
    sumChi2xz /= (nHits - 1);
    sumChi2yz /= (nHits - 1);

    Float_t chi2 = Max(sumChi2xy, Max(sumChi2xz, sumChi2yz));
    if (chi2 < 1000) return chi2;
    else return -1000.0;
}

//BmnStatus FindSeed(TClonesArray* hits, TClonesArray* tracks) {
//
//    Int_t hitsThresh = 4;
//
//    const Int_t nBins = 200;
//    const Int_t nHits = hits->GetEntriesFast();
//    Float_t minY = -0.4;
//    Float_t maxY = -minY;
//    Float_t width = (maxY - minY) / nBins;
//
//    TH1F* h = new TH1F("yOverR", "yOverR", nBins, minY, maxY);
//
//    if (nHits < hitsThresh) {
//        cout << "WARNING: Too few hits: Nhits = " << nHits << endl;
//        return kBMNERROR;
//    }
//    Bool_t lay0 = kFALSE;
//    Bool_t lay1 = kFALSE;
//    Bool_t lay2 = kFALSE;
//    Bool_t lay3 = kFALSE;
//
//    for (Int_t i = 0; i < hits->GetEntriesFast(); ++i) {
//        BmnDchHit* hit = (BmnDchHit*) hits->At(i);
//        UInt_t lay = hit->GetLayer();
//        switch (lay) {
//            case 0: lay0 = kTRUE;
//                break;
//            case 1: lay1 = kTRUE;
//                break;
//            case 2: lay2 = kTRUE;
//                break;
//            case 3: lay3 = kTRUE;
//                break;
//        }
//    }
//    cout << lay0 << " " << lay1 << " " << lay2 << " " << lay3 << endl;
//
//    if (!(lay0 && lay1 && lay2 && lay3)) {
//        cout << "Hits for track not presented on each plane of DCHs" << endl;
//        return kBMNERROR;
//    }
//
//    for (Int_t iHit = 0; iHit < nHits; ++iHit) {
//        BmnDchHit* hit = (BmnDchHit*) hits->At(iHit);
//        const Float_t R = Sqrt(hit->GetX() * hit->GetX() + hit->GetY() * hit->GetY() + hit->GetZ() * hit->GetZ());
//        const Float_t newX = hit->GetX() / R;
//        const Float_t newY = hit->GetY() / R;
//        //        Int_t xAddr = ceil((newX - minY) / width);
//        Int_t yAddr = ceil((newY - minY) / width);
//        //        Long_t addr = yAddr * nBins + xAddr;
//        //        hit->SetAddr(addr);
//        //        hit->SetXaddr(xAddr);
//        hit->SetFlag(yAddr); //tmp storage
//        h->Fill(newY);
//        hit->SetIndex(iHit);
//    }
//
//    vector<Int_t> peaks; // vector of connected bins
//    for (Int_t iBin = 0; iBin < h->GetNbinsX(); iBin += (peaks.size() + 1)) {
//        peaks.clear();
//        peaks.resize(0);
//        if (h->GetBinContent(iBin) > 0.0) {
//            peaks.push_back(iBin);
//            for (Int_t iBinPeak = iBin + 1; iBinPeak < h->GetNbinsX(); ++iBinPeak) {
//                if (h->GetBinContent(iBinPeak) > 0.0) {
//                    peaks.push_back(iBinPeak);
//                } else {
//                    break;
//                }
//            }
//        }
//
//        vector<BmnDchHit*> hitsInTrack;
//        for (Int_t iBinPeak = 0; iBinPeak < peaks.size(); ++iBinPeak) {
//            for (Int_t iHit = 0; iHit < nHits; ++iHit) {
//                BmnDchHit* hit = (BmnDchHit*) hits->At(iHit);
//                if (hit->IsUsed()) continue;
//                if (hit->GetFlag() == peaks.at(iBinPeak)) {
//                    hit->SetUsing(kTRUE);
//                    hitsInTrack.push_back(hit);
//                }
//            }
//        }
//        TVector3 vertex;
//        TVector3 direction;
//
//        if (hitsInTrack.size() >= hitsThresh) {
//            cout << "Hits in track: " << endl;
//            //            for (Int_t iHit = 0; iHit < hitsInTrack.size(); ++iHit) {
//            //                BmnDchHit* hit = hitsInTrack.at(iHit);
//            //                cout << hit->GetX() << " " << hit->GetY() << " " << hit->GetZ() << endl;
//            //            }
//
//            LineFit3D(hitsInTrack, vertex, direction);
//            if (direction.Mag() == 0.0 && direction.Mag() == 0.0) continue;
//            new((*tracks)[tracks->GetEntriesFast()]) CbmTrack();
//            CbmTrack* track = (CbmTrack*) tracks->At(tracks->GetEntriesFast() - 1);
//            for (Int_t iHit = 0; iHit < hitsInTrack.size(); ++iHit) {
//                track->AddHit(hitsInTrack.at(iHit)->GetIndex(), HitType(0));
//                FairTrackParam* par = new FairTrackParam();
//                par->SetPosition(vertex);
//                par->SetTx(direction.X() / direction.Z());
//                par->SetTy(direction.Y() / direction.Z());
//                track->SetParamFirst(par);
//            }
//        }
//    }
//
//    TCanvas* c12 = new TCanvas("c12", "c12", 1600, 800);
//    h->Draw("");
//    c12->SaveAs("yR.png");
//    delete h;
//    return kBMNSUCCESS;
//}

//Float_t LineBy4Hits(BmnDchHit* hit0, BmnDchHit* hit1, BmnDchHit* hit2, BmnDchHit* hit3, TVector3& vertex, TVector3& direction) {
//    vector<BmnDchHit*> hits;
//    hits.push_back(hit0);
//    hits.push_back(hit1);
//    hits.push_back(hit2);
//    hits.push_back(hit3);
//    return LineFit3D(hits, vertex, direction);
//}

//BmnStatus FindSeedsOver4hits(TClonesArray* hits, TClonesArray* tracks, Float_t chi2Thresh) {
//    Int_t hitsThresh = 4;
//    Int_t nHits = hits->GetEntriesFast();
//
//    Bool_t lay0 = kFALSE;
//    Bool_t lay1 = kFALSE;
//    Bool_t lay2 = kFALSE;
//    Bool_t lay3 = kFALSE;
//
//    vector<BmnDchHit*> lay0hits;
//    vector<BmnDchHit*> lay1hits;
//    vector<BmnDchHit*> lay2hits;
//    vector<BmnDchHit*> lay3hits;
//
//    for (Int_t i = 0; i < nHits; ++i) {
//        BmnDchHit* hit = (BmnDchHit*) hits->At(i);
//        UInt_t lay = hit->GetLayer();
//        switch (lay) {
//            case 0:
//            {
//                lay0hits.push_back(hit);
//                lay0 = kTRUE;
//                break;
//            }
//            case 1:
//            {
//                lay1hits.push_back(hit);
//                lay1 = kTRUE;
//                break;
//            }
//            case 2:
//            {
//                lay2hits.push_back(hit);
//                lay2 = kTRUE;
//                break;
//            }
//            case 3:
//            {
//                lay3hits.push_back(hit);
//                lay3 = kTRUE;
//                break;
//            }
//        }
//    }
//
//    TVector3 vertex;
//    TVector3 direction;
//
//    for (Int_t i0 = 0; i0 < lay0hits.size(); ++i0) {
//        BmnDchHit* hit0 = (BmnDchHit*) lay0hits.at(i0);
//        //        if (hit0->IsUsed()) continue;
//        for (Int_t i1 = 0; i1 < lay1hits.size(); ++i1) {
//            BmnDchHit* hit1 = (BmnDchHit*) lay1hits.at(i1);
//            //            if (hit1->IsUsed()) continue;
//            for (Int_t i2 = 0; i2 < lay2hits.size(); ++i2) {
//                BmnDchHit* hit2 = (BmnDchHit*) lay2hits.at(i2);
//                //                if (hit2->IsUsed()) continue;
//                for (Int_t i3 = 0; i3 < lay3hits.size(); ++i3) {
//                    BmnDchHit* hit3 = (BmnDchHit*) lay3hits.at(i3);
//                    //                    if (hit3->IsUsed()) continue;
//                    vector<BmnDchHit*> hitsForLine;
//                    hitsForLine.push_back(hit0);
//                    hitsForLine.push_back(hit1);
//                    hitsForLine.push_back(hit2);
//                    hitsForLine.push_back(hit3);
//                    Float_t maxChi2 = LineFit3D(hitsForLine, vertex, direction);
//                    if (maxChi2 > chi2Thresh)
//                        continue;
//                    else {
//                        new((*tracks)[tracks->GetEntriesFast()]) CbmTrack(CreateTrack(direction, vertex, hitsForLine, maxChi2));
//                    }
//                }
//            }
//        }
//    }
//}

BmnStatus CreateTrack(TVector3 dir, TVector3 vert, vector<BmnDchHit*>& trackHits, CbmTrack& track, FairTrackParam& par, Float_t chi2) {
    par.SetPosition(vert);
    par.SetTx(dir.X() / dir.Z());
    par.SetTy(dir.Y() / dir.Z());
    par.SetQp(dir.Mag());
    track.SetParamFirst(&par);
    track.SetChiSq(chi2);
    for (Int_t i = 0; i < trackHits.size(); ++i) {
        track.AddHit(trackHits.at(i)->GetHitId(), HitType(0));
    }
    return kBMNSUCCESS;
}

BmnStatus TwoHitLine(BmnDchHit* hit1, BmnDchHit* hit2, TVector3& vertex, TVector3& direction) {

    Float_t x1 = hit1->GetX();
    Float_t x2 = hit2->GetX();
    Float_t y1 = hit1->GetY();
    Float_t y2 = hit2->GetY();
    Float_t z1 = hit1->GetZ();
    Float_t z2 = hit2->GetZ();

    vertex.SetXYZ(x1, y1, z1);
    direction.SetXYZ(x2 - x1, y2 - y1, z2 - z1);

    return kBMNSUCCESS;
}

BmnStatus SeedFinder(vector<BmnDchHit*> lay0hits, vector<BmnDchHit*> lay1hits, vector<CbmTrack>& seeds) {
    //function searches all segments on the first DCH
    TVector3 vertex;
    TVector3 direction;

    for (Int_t i0 = 0; i0 < lay0hits.size(); ++i0) {
        BmnDchHit* hit0 = (BmnDchHit*) lay0hits.at(i0);
        if (hit0->IsUsed()) continue;
        for (Int_t i1 = 0; i1 < lay1hits.size(); ++i1) {
            BmnDchHit* hit1 = (BmnDchHit*) lay1hits.at(i1);
            if (hit1->IsUsed()) continue;
            TwoHitLine(hit0, hit1, vertex, direction);
            vector<BmnDchHit*> trackHits;
            trackHits.push_back(hit0);
            trackHits.push_back(hit1);
            CbmTrack seed;
            FairTrackParam par;
            CreateTrack(direction, vertex, trackHits, seed, par, -100.0);
            seeds.push_back(seed);
        }
    }
    return kBMNSUCCESS;
}

BmnStatus TrackPropagation(vector<BmnDchHit*> &layHits, vector<CbmTrack>& seeds) {

    const Float_t kMINDIST = 10.0; //minimal distance for matching hits to track

    for (Int_t iSeed = 0; iSeed < seeds.size(); ++iSeed) {
        CbmTrack* seed = &(seeds.at(iSeed));
        Float_t minDist = 1000.0;
        BmnDchHit* minHit = NULL;
        for (Int_t iHit = 0; iHit < layHits.size(); ++iHit) {
            BmnDchHit* hit = (BmnDchHit*) layHits.at(iHit);
            if (hit->IsUsed()) continue;
            //hit information
            Float_t Hx = hit->GetX();
            Float_t Hy = hit->GetY();
            Float_t Hz = hit->GetZ();
            //seed information
            Float_t Tx = seed->GetParamFirst()->GetTx();
            Float_t Ty = seed->GetParamFirst()->GetTy();
            Float_t Vx = seed->GetParamFirst()->GetX();
            Float_t Vy = seed->GetParamFirst()->GetY();
            Float_t Vz = seed->GetParamFirst()->GetZ();
            //coordinates of seed on current layer
            Float_t xLay = Tx * (Hz - Vz) + Vx;
            Float_t yLay = Ty * (Hz - Vz) + Vy;
            Float_t dist = Sqrt((Hx - xLay) * (Hx - xLay) + (Hy - yLay) * (Hy - yLay));
            if (dist < minDist) {
                minDist = dist;
                minHit = hit;
            }
        }
        if (minDist <= kMINDIST) {
            seed->AddHit(minHit->GetHitId(), HitType(0));
        }
    }
}

BmnStatus TrackSelection(TClonesArray* hits, vector<CbmTrack>& seeds, TClonesArray* tracks, Int_t flag) {
    //select "long" tracks 
    for (Int_t iSeed = 0; iSeed < seeds.size(); ++iSeed) {
        CbmTrack seed = seeds.at(iSeed);
        vector<BmnDchHit*> hitsForRefit;
        if (seed.GetNofHits() >= 3) {
            for (Int_t iHit = 0; iHit < seed.GetNofHits(); ++iHit) {
                BmnDchHit* hit = (BmnDchHit*) hits->At(seed.GetHitIndex(iHit));
                hitsForRefit.push_back(hit);
            }
            TVector3 vert;
            TVector3 dir;
            Float_t chi2 = LineFit3D(hitsForRefit, vert, dir);
            if (Abs(chi2) > 100 ) continue;
            for (Int_t iHit = 0; iHit < seed.GetNofHits(); ++iHit) {
                BmnDchHit* hit = (BmnDchHit*) hits->At(seed.GetHitIndex(iHit));
                hit->SetUsing(kTRUE);
            }
            CbmTrack* track = new CbmTrack();
            FairTrackParam* par = new FairTrackParam();
            CreateTrack(dir, vert, hitsForRefit, *track, *par, chi2);
            //flag = +1 for forward seeding
            //flag = -1 for backward seeding
            track->SetFlag(flag);
            new((*tracks)[tracks->GetEntriesFast()]) CbmTrack(*track);
        }
    }
}

BmnStatus DchTrackFinder(TClonesArray* hits, TClonesArray* tracks) {
    //main function for tracking in DCH
    vector<BmnDchHit*> lay0hits;
    vector<BmnDchHit*> lay1hits;
    vector<BmnDchHit*> lay2hits;
    vector<BmnDchHit*> lay3hits;

    for (Int_t i = 0; i < hits->GetEntriesFast(); ++i) {
        BmnDchHit* hit = (BmnDchHit*) hits->At(i);
        if (hit->IsUsed()) continue;
        UShort_t lay = hit->GetLayer();
        if (lay == 0) lay0hits.push_back(hit);
        else if (lay == 1) lay1hits.push_back(hit);
        else if (lay == 2) lay2hits.push_back(hit);
        else if (lay == 3) lay3hits.push_back(hit);
        else continue;
    }

    vector<CbmTrack> seedsForward;
    SeedFinder(lay0hits, lay1hits, seedsForward);
    TrackPropagation(lay2hits, seedsForward);
    TrackPropagation(lay3hits, seedsForward);
    TrackSelection(hits, seedsForward, tracks, 1);

    vector<CbmTrack> seedsBack;
    SeedFinder(lay3hits, lay2hits, seedsBack);
    TrackPropagation(lay1hits, seedsBack);
    TrackPropagation(lay0hits, seedsBack);
    TrackSelection(hits, seedsBack, tracks, -1);
}

#endif	/* BMNTRACKFINDERRUN1_H */

