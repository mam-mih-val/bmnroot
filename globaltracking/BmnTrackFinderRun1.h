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

//const Double_t MWPC0_Zpos = -301.3;
//const Double_t MWPC1_Zpos = -158.5;
//const Double_t MWPC2_Zpos = 459.5;

Float_t LineFit3D(vector<FairHit*> hits, TVector3& vertex, TVector3& direction) {

    //    cout << "LineFit3D started!" << endl;
    const Int_t nHits = hits.size();

    Float_t Xi = 0.0, Yi = 0.0, Zi = 0.0; // coordinates of current track point
    Float_t Ci;
    Float_t SumXW = 0.0, SumYW = 0.0;
    Float_t SumXWC = 0.0, SumYWC = 0.0;
    Float_t SumW = 0.0;
    Float_t SumC = 0.0, SumC2 = 0.0;
    Float_t Wi = 1.0 / nHits; // weight    
    Float_t ZV = ((FairHit*) hits.at(0))->GetZ(); //Z-coordinate of vertex
    Float_t ZN = ((FairHit*) hits.at(nHits - 1))->GetZ();
    Float_t Az = (ZN - ZV);
    if (Az == 0.0) {
//        cout << "Az = 0.0" << endl;
        vertex = TVector3(0.0, 0.0, 0.0);
        direction = TVector3(0.0, 0.0, 0.0);
        return 1000.0;
    }
    for (Int_t i = 0; i < nHits; ++i) {
        FairHit* hit = (FairHit*) hits.at(i);
        if (hit == NULL) continue;
        Xi = hit->GetX();
        Yi = hit->GetY();
        Zi = hit->GetZ();
        //        if (Zi < 0.0) {
        //            cout << "WARNING: bad hit!" << endl;
        //            //            hit->Print();
        //        }
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
        FairHit* hit = (FairHit*) hits.at(i);
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

void AddHits(vector<BmnDchHit*>& trackHits, CbmTrack& track) {
    for (Int_t i = 0; i < trackHits.size(); ++i)
        track.AddHit(trackHits.at(i)->GetHitId(), HitType(0));
}

void AddHits(vector<BmnMwpcHit*>& trackHits, CbmTrack& track) {
    for (Int_t i = 0; i < trackHits.size(); ++i)
        track.AddHit(trackHits.at(i)->GetHitId(), HitType(0));
}

BmnStatus CreateTrack(TVector3 dir, TVector3 vert, CbmTrack& track, FairTrackParam& par, Float_t chi2, Int_t nHits) {
    par.SetPosition(vert);
    par.SetTx(dir.X() / dir.Z());
    par.SetTy(dir.Y() / dir.Z());
    par.SetQp(dir.Mag());
    track.SetParamFirst(&par);
    track.SetChiSq(chi2);
    track.SetNDF(nHits);
    return kBMNSUCCESS;
}

BmnStatus TwoHitLine(FairHit* hit1, FairHit* hit2, TVector3& vertex, TVector3& direction) {

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
            TwoHitLine((FairHit*) hit0, (FairHit*) hit1, vertex, direction);
            vector<BmnDchHit*> trackHits;
            trackHits.push_back(hit0);
            trackHits.push_back(hit1);
            CbmTrack seed;
            FairTrackParam par;
            AddHits(trackHits, seed);
            CreateTrack(direction, vertex, seed, par, -100.0, seed.GetNofHits());
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

BmnStatus TrackPropagation(vector<BmnMwpcHit*> &layHits, CbmTrack& seed) {

    const Float_t kMINDIST = 3.0; //minimal distance for matching hits to track

    Float_t minDist = 1000.0;
    BmnMwpcHit* minHit = NULL;
    for (Int_t iHit = 0; iHit < layHits.size(); ++iHit) {
        BmnMwpcHit* hit = (BmnMwpcHit*) layHits.at(iHit);
        if (hit->IsUsed()) continue;
        //hit information
        Float_t Hx = hit->GetX();
        Float_t Hy = hit->GetY();
        Float_t Hz = hit->GetZ();
        //seed information
        Float_t Tx = seed.GetParamFirst()->GetTx();
        Float_t Ty = seed.GetParamFirst()->GetTy();
        Float_t Vx = seed.GetParamFirst()->GetX();
        Float_t Vy = seed.GetParamFirst()->GetY();
        Float_t Vz = seed.GetParamFirst()->GetZ();
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
        seed.AddHit(minHit->GetHitId(), HitType(0));
    }
}

BmnStatus TrackSelection(TClonesArray* hits, vector<CbmTrack>& seeds, TClonesArray* tracks, Int_t flag) {
    //select "long" tracks 
    for (Int_t iSeed = 0; iSeed < seeds.size(); ++iSeed) {
        CbmTrack seed = seeds.at(iSeed);
        vector<FairHit*> hitsForRefit;
        vector<BmnDchHit*> resultTrackHits;
        if (seed.GetNofHits() >= 3) {
            for (Int_t iHit = 0; iHit < seed.GetNofHits(); ++iHit) {
                BmnDchHit* hit = (BmnDchHit*) hits->At(seed.GetHitIndex(iHit));
                hitsForRefit.push_back((FairHit*) hit);
                resultTrackHits.push_back(hit);
            }
            TVector3 vert;
            TVector3 dir;
            Float_t chi2 = LineFit3D(hitsForRefit, vert, dir);
            if (Abs(chi2) > 100) continue;
            for (Int_t iHit = 0; iHit < seed.GetNofHits(); ++iHit) {
                BmnDchHit* hit = (BmnDchHit*) hits->At(seed.GetHitIndex(iHit));
                hit->SetUsing(kTRUE);
            }
            CbmTrack* track = new CbmTrack();
            FairTrackParam* par = new FairTrackParam();
            AddHits(resultTrackHits, *track);
            CreateTrack(dir, vert, *track, *par, chi2, track->GetNofHits());
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

    return kBMNSUCCESS;
}

TVector3 GetMwpcPosition(Short_t MWPC_ID) {
    //    TVector3 mwpcPos;
    //    TGeoVolume* pVolume = gGeoManager->GetVolume("cave");
    //    if (pVolume != NULL) {
    //        TString node_name = TString::Format("mwpc%d_0", MWPC_ID + 1);
    //        TGeoNode* pNode = pVolume->FindNode(node_name);
    //        if (pNode != NULL) {
    //            TGeoMatrix* pMatrix = pNode->GetMatrix();
    //            mwpcPos = TVector3(pMatrix->GetTranslation()[0], pMatrix->GetTranslation()[1], pMatrix->GetTranslation()[2]);
    //        } else {
    //            cout << "MWPC detector (" << node_name << ") wasn't found." << endl;
    //            mwpcPos = TVector3(0.0, 0.0, -1000.0);
    //        }
    //    } else {
    //        cout << "Cave volume wasn't found." << endl;
    //        mwpcPos = TVector3(0.0, 0.0, -1000.0);
    //    }
    Float_t zPosMwpc = (MWPC_ID == 0) ? MWPC0_Zpos : (MWPC_ID == 1) ? MWPC1_Zpos : (MWPC_ID == 2) ? MWPC2_Zpos : -1000.0;
    TVector3 mwpcPos = TVector3(0.0, 0.0, zPosMwpc);
    return mwpcPos;
}

BmnStatus MwpcTrackFinder(TClonesArray* hits, TClonesArray* tracks, Short_t MWPC_ID) {
    //main function for tracking in MWPC

    vector<BmnMwpcHit*> lay0hits;
    vector<BmnMwpcHit*> lay1hits;
    vector<BmnMwpcHit*> lay2hits;

    TVector3 mwpcPos = GetMwpcPosition(MWPC_ID);

    for (Int_t i = 0; i < hits->GetEntriesFast(); ++i) {
        BmnMwpcHit* hit = (BmnMwpcHit*) hits->At(i);
        if (hit->IsUsed()) continue;
        // condition only for RUN2, MWPC0 & MWPC1 were placed before target and they detected beam ===>
        if (MWPC_ID != 2) {
            if (hit->GetX() < -3 || hit->GetX() > 3) continue;
            if (hit->GetY() < -10 || hit->GetY() > -6) continue;
        }
        // <===
        if (hit->GetMwpcId() != MWPC_ID) continue;
        Float_t zGlob = hit->GetZ();
        Float_t zLoc = zGlob - mwpcPos.Z();
        if (Abs(zLoc + 2) < 1e-3) lay0hits.push_back(hit);
        else if (Abs(zLoc) < 1e-3) lay1hits.push_back(hit);
        else if (Abs(zLoc - 2) < 1e-3) lay2hits.push_back(hit);
        else continue;
    }

    UInt_t s0 = lay0hits.size();
    UInt_t s1 = lay1hits.size();
    UInt_t s2 = lay2hits.size();

    //    if (!((s0 == 1) && (s1 == 1) && (s2 == 1))) return kBMNERROR; //ideal case 

    Bool_t s0_bool = Bool_t(s0);
    Bool_t s1_bool = Bool_t(s1);
    Bool_t s2_bool = Bool_t(s2);

    //        cout << "s0 = " << s0 << " s1 = " << s1 << " s2 = " << s2 << endl;

    if (s0_bool) {
        if (s1_bool) {
            for (Int_t i0 = 0; i0 < s0; ++i0) {
                BmnMwpcHit* hit0 = (BmnMwpcHit*) lay0hits.at(i0);
                if (hit0->IsUsed()) continue;
                for (Int_t i1 = 0; i1 < s1; ++i1) {
                    BmnMwpcHit* hit1 = (BmnMwpcHit*) lay1hits.at(i1);
                    if (hit1->IsUsed()) continue;
                    TVector3 vertex;
                    TVector3 direction;
                    TwoHitLine((FairHit*) hit0, (FairHit*) hit1, vertex, direction); //form line by 2 hits
                    vector<BmnMwpcHit*> trackHits;
                    trackHits.push_back(hit0);
                    trackHits.push_back(hit1);
                    CbmTrack seed;
                    FairTrackParam par;
                    AddHits(trackHits, seed);
                    CreateTrack(direction, vertex, seed, par, 0.0, seed.GetNofHits());
                    if (s2_bool) {
                        TrackPropagation(lay2hits, seed); //propagate line to hit on the third plane
                        vector<FairHit*> hitsForRefit;
                        for (Int_t iHit = 0; iHit < seed.GetNofHits(); ++iHit) {
                            BmnMwpcHit* hit = (BmnMwpcHit*) hits->At(seed.GetHitIndex(iHit));
                            hitsForRefit.push_back((FairHit*) hit);
                        }
                        Float_t chi2 = LineFit3D(hitsForRefit, vertex, direction);
                        if (Abs(chi2) > 100) continue;
                        for (Int_t iHit = 0; iHit < seed.GetNofHits(); ++iHit) {
                            BmnMwpcHit* hit = (BmnMwpcHit*) hits->At(seed.GetHitIndex(iHit));
                            hit->SetUsing(kTRUE);
                        }
                        CreateTrack(direction, vertex, seed, par, chi2, seed.GetNofHits());
                        new((*tracks)[tracks->GetEntriesFast()]) CbmTrack(seed); //make track and put it to output array
                    } else {
                        new((*tracks)[tracks->GetEntriesFast()]) CbmTrack(seed); //make track and put it to output array
                    }
                }
            }
        } else {
            return kBMNERROR;
        }
    } else if (s1_bool) {
        if (s2_bool) {
            for (Int_t i1 = 0; i1 < s1; ++i1) {
                BmnMwpcHit* hit1 = (BmnMwpcHit*) lay1hits.at(i1);
                if (hit1->IsUsed()) continue;
                for (Int_t i2 = 0; i2 < s2; ++i2) {
                    BmnMwpcHit* hit2 = (BmnMwpcHit*) lay2hits.at(i2);
                    if (hit2->IsUsed()) continue;
                    TVector3 vertex;
                    TVector3 direction;
                    TwoHitLine((FairHit*) hit1, (FairHit*) hit2, vertex, direction); //form line by 2 hits
                    vector<BmnMwpcHit*> trackHits;
                    trackHits.push_back(hit1);
                    trackHits.push_back(hit2);
                    CbmTrack seed;
                    FairTrackParam par;
                    AddHits(trackHits, seed);
                    CreateTrack(direction, vertex, seed, par, 0.0, seed.GetNofHits());
                    new((*tracks)[tracks->GetEntriesFast()]) CbmTrack(seed); //make track and put it to output array
                }
            }
        }
    } else {
        return kBMNERROR;
    }

    return kBMNSUCCESS;
}

BmnStatus MwpcTrackMatching(TClonesArray* hits, TClonesArray* outTracks, TClonesArray* Mwpc1Tracks, TClonesArray* Mwpc2Tracks) {

    const Float_t deltaTx = 10;
    const Float_t deltaTy = 10;
    const Float_t deltaX = 10;
    const Float_t deltaY = 10;

    const Float_t angThr = Tan(89.9 * DegToRad());

    TVector3 pos1 = GetMwpcPosition(0);
    TVector3 pos2 = GetMwpcPosition(1);

    Float_t zMid = (pos1.Z() + pos2.Z()) / 2.0;

    //    if ((Mwpc1Tracks->GetEntriesFast() != 1) && (Mwpc2Tracks->GetEntriesFast() != 1)) return kBMNERROR;

    for (Int_t i1 = 0; i1 < Mwpc1Tracks->GetEntriesFast(); ++i1) {
        CbmTrack* tr1 = (CbmTrack*) Mwpc1Tracks->At(i1);
        const FairTrackParam* par1 = tr1->GetParamFirst();
        Float_t Tx1 = par1->GetTx();
        Float_t Ty1 = par1->GetTy();
        Float_t x1 = par1->GetX();
        Float_t y1 = par1->GetY();
        Float_t z1 = par1->GetZ();
        for (Int_t i2 = 0; i2 < Mwpc2Tracks->GetEntriesFast(); ++i2) {
            CbmTrack* tr2 = (CbmTrack*) Mwpc2Tracks->At(i2);
            const FairTrackParam* par2 = tr2->GetParamFirst();
            Float_t Tx2 = par2->GetTx();
            Float_t Ty2 = par2->GetTy();
            Float_t x2 = par2->GetX();
            Float_t y2 = par2->GetY();
            Float_t z2 = par2->GetZ();
            //            if (Abs(Tx1 - Tx2) > deltaTx) continue;
            //            if (Abs(Ty1 - Ty2) > deltaTy) continue;
            if (Abs(Tx1) > angThr) continue;
            if (Abs(Tx2) > angThr) continue;
            if (Abs(Ty1) > angThr) continue;
            if (Abs(Ty2) > angThr) continue;
            Float_t x1_zMid = Tx1 * (zMid - z1) + x1;
            Float_t y1_zMid = Ty1 * (zMid - z1) + y1;
            Float_t x2_zMid = Tx2 * (zMid - z2) + x2;
            Float_t y2_zMid = Ty2 * (zMid - z2) + y2;
            //            if (Abs(x1_zMid - x2_zMid) > deltaX) continue;
            //            if (Abs(y1_zMid - y2_zMid) > deltaY) continue;
            //            cout << " deltaY = " << Abs(y2_zMid - y1_zMid) << " deltaX = " << Abs(x2_zMid - x1_zMid) << endl;
            CbmTrack matchedTrack;
            FairTrackParam matchedPar;
            //FOR TEST
            FairTrackParam tmp;
            tmp.SetX(x1_zMid - x2_zMid);
            tmp.SetY(y1_zMid - y2_zMid);
            //
            TVector3 vertex;
            TVector3 direction;
            vector<FairHit*> hitsForRefit;
            vector<BmnMwpcHit*> resultTrackHits;
            for (Int_t iHit = 0; iHit < tr1->GetNofHits(); ++iHit) {
                BmnMwpcHit* hit = (BmnMwpcHit*) hits->At(tr1->GetHitIndex(iHit));
                hitsForRefit.push_back((FairHit*) hit);
                resultTrackHits.push_back(hit);
            }
            for (Int_t iHit = 0; iHit < tr2->GetNofHits(); ++iHit) {
                BmnMwpcHit* hit = (BmnMwpcHit*) hits->At(tr2->GetHitIndex(iHit));
                hitsForRefit.push_back((FairHit*) hit);
                resultTrackHits.push_back(hit);
            }
            Float_t chi2 = LineFit3D(hitsForRefit, vertex, direction);
            if (Abs(chi2) > 100) continue;
            AddHits(resultTrackHits, matchedTrack);
            CreateTrack(direction, vertex, matchedTrack, matchedPar, chi2, matchedTrack.GetNofHits());
            matchedTrack.SetParamLast(&tmp);
            new((*outTracks)[outTracks->GetEntriesFast()]) CbmTrack(matchedTrack);
        }
    }

    return kBMNSUCCESS;
}

BmnStatus MwpcTrackMatchingByAllHits(TClonesArray* hits, TClonesArray* outTracks) {

    const Float_t angThr = Tan(89.9 * DegToRad());

    TVector3 pos1 = GetMwpcPosition(0);
    TVector3 pos2 = GetMwpcPosition(1);

    TVector3 vertex;
    TVector3 direction;
    vector<FairHit*> hitsForRefit;
    vector<BmnMwpcHit*> resultTrackHits;
    CbmTrack matchedTrack;
    FairTrackParam matchedPar;

    for (Int_t iHit = 0; iHit < hits->GetEntriesFast(); ++iHit) {
        BmnMwpcHit* hit = (BmnMwpcHit*) hits->At(iHit);
        if (hit->GetMwpcId() == 2) continue;
        if (hit->GetX() < -3 || hit->GetX() > 3) continue;
        if (hit->GetY() < -10 || hit->GetY() > -6) continue;
        hitsForRefit.push_back(hit);
    }
    if (hitsForRefit.size() < 3) return kBMNERROR;
    Float_t chi2 = LineFit3D(hitsForRefit, vertex, direction);
    if (Abs(chi2) > 100) return kBMNERROR;
    AddHits(resultTrackHits, matchedTrack);
    CreateTrack(direction, vertex, matchedTrack, matchedPar, chi2, matchedTrack.GetNofHits());
    new((*outTracks)[outTracks->GetEntriesFast()]) CbmTrack(matchedTrack);

    return kBMNSUCCESS;
}


#endif	/* BMNTRACKFINDERRUN1_H */

