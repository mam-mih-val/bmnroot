#ifndef BMNHITFINDERRUN1_H
#define BMNHITFINDERRUN1_H 1

#include <vector>
#include "TGeoVolume.h"
#include "TGeoManager.h"
#include "TVector3.h"
#include "BmnDchHit.h"
#include "BmnDchDigit.h"
#include "BmnMwpcDigit.h"
#include "BmnMwpcHit.h"
#include "TClonesArray.h"
#include "TMath.h"
#include "TF1.h"
#include "TH1F.h"
#include "BmnEnums.h"

using namespace std;
using namespace TMath;


// constants for mwpc ===>
const Short_t kNPlanes = 6;
const Short_t kTimeBin = 8; // ns
const Short_t kNWires = 96; //in one plane
const Float_t kAngleStep = 60 * DegToRad();
const Float_t kWireStep = 0.25; // cm
const Float_t kPlaneHeight = 43.3; // cm
const Float_t kPlaneWidth = kNWires * kWireStep; //24.6; // cm
const Float_t kMwpcZpos = 350; //FIXME!!! Get coords from geometry

#define mwpc0_leftTime  0.0
#define mwpc0_rightTime  0.0
#define mwpc1_leftTime  0.0
#define mwpc1_rightTime  0.0
#define mwpc2_leftTime  0.0
#define mwpc2_rightTime  0.0
// <=== constants for mwpc

//Detector's position

//Detector's construct parameters
const Float_t ZLength_DCH1 = 20.0;
const Float_t ZLength_DCH1ActiveVolume = 0.6;
const Float_t HoleSize_DCH1 = 12.0;

const Float_t SideLengthOfOctagon = 120.0;
const Float_t InnerRadiusOfOctagon = (2.414 * SideLengthOfOctagon) / 2.0;

const UInt_t nWires = 240; // 0 - 255
const UInt_t nPlanes = 16; // 0 - 15const 
#define MaxRadiusOfActiveVolume  120.0
const Float_t MinRadiusOfActiveVolume = 12.0;
const Float_t WireStep = 2.0 * MaxRadiusOfActiveVolume / nWires;
const Float_t HalfStep = WireStep / 2.0;

inline void CheckHits(vector<TVector3> &vec1, vector<TVector3> &vec2) {

    Bool_t flag = kFALSE;
    Float_t delta = 10.0; // cm
    for (Int_t i = 0; i < vec1.size(); ++i) {
        TVector3* hit1 = &(vec1.at(i));
        for (Int_t j = 0; j < vec2.size(); ++j) {
            TVector3 hit2 = vec2.at(j);
            if (Abs(hit1->Perp() - hit2.Perp()) < delta) {
                flag = kTRUE;
                break;
            }
        }
        if (!flag) {
            hit1->SetXYZ(-1000.0, -1000.0, -1000.0);
        }
    }
}

inline void CombineHits(vector<TVector3> vec, TClonesArray* hits, Short_t plane) {

    TVector3 dchPos;

    //    TGeoVolume* pVolume = gGeoManager->GetVolume("cave");
    //    if (pVolume != NULL) {
    //        TString node_name = TString::Format("dch%d_0", plane / 2 + 1);
    //        TGeoNode* pNode = pVolume->FindNode(node_name);
    //        if (pNode != NULL) {
    //            TGeoMatrix* pMatrix = pNode->GetMatrix();
    //            dchPos = TVector3(pMatrix->GetTranslation()[0], pMatrix->GetTranslation()[1], pMatrix->GetTranslation()[2]);
    //        } else
    //            cout << "DCH detector (" << node_name << ") wasn't found." << endl;
    //    } else
    //        cout << "Cave volume wasn't found." << endl;

    Float_t delta = 2.0; //roughly
    for (Int_t i = 0; i < vec.size(); ++i) {
        TVector3* hitI = &(vec.at(i));
        for (Int_t j = i + 1; j < vec.size(); ++j) {
            TVector3* hitJ = &(vec.at(j));
            //            cout << "Abs(hitI.Mag() - hitJ.Mag()) = " << Abs(hitI.Mag() - hitJ.Mag()) << endl;
            if (Abs(hitI->Mag() - hitJ->Mag()) < delta) {
                hitI->SetZ(-1000);
                break;
            }
        }
    }
    for (Int_t i = 0; i < vec.size(); ++i) {
        TVector3 hit = vec.at(i);

        if (hit.Z() < -100.0) continue;
        if ((plane == 0) || (plane == 2)) {
            hit.RotateZ(45.0 * DegToRad());
            //hit.SetY(-1.0 * hit.Y());
        } else {
            hit.RotateZ(90.0 * DegToRad());
            //hit.SetY(-1.0 * hit.Y());
        }
        //        cout << hit.X() << " " << hit.Y() << " " << hit.Z() << " " << hit.Mag() << endl;
        new((*hits)[hits->GetEntriesFast()]) BmnDchHit(0, hit + dchPos, TVector3(0, 0, 0), 0, 0, 0, plane);
        BmnDchHit* dchHit = (BmnDchHit*) hits->At(hits->GetEntriesFast() - 1);
        dchHit->SetDchId(plane / 2 + 1);
        dchHit->SetHitId(hits->GetEntriesFast() - 1);
    }
}

inline vector<TVector3> CreateHitsByTwoPlanes(vector<Float_t> vec1, vector<Float_t> vec2, Float_t zPos) {
    vector<TVector3> v;
    for (Int_t i = 0; i < vec1.size(); ++i) {
        Float_t x = vec1.at(i);
        for (Int_t j = 0; j < vec2.size(); ++j) {
            Float_t y = vec2.at(j);
            TVector3 pos(x, y, zPos);
            v.push_back(pos);
        }
    }
    return v;
}

inline vector<Float_t> MergeSubPlanes(vector<BmnDchDigit*> vec1, vector<BmnDchDigit*> vec2) {
    vector<Float_t> v;
    for (Int_t i = 0; i < vec1.size(); ++i) {
        BmnDchDigit* d1 = (BmnDchDigit*) vec1.at(i);
        for (Int_t j = 0; j < vec2.size(); ++j) {
            BmnDchDigit* d2 = (BmnDchDigit*) vec2.at(j);
            if (Abs(d1->GetWireNumber() - d2->GetWireNumber()) > 1) continue;
            Float_t n = (d1->GetWireNumber() + d2->GetWireNumber()) / 2.0;
            //            Float_t coord = (InnerRadiusOfOctagon - WireStep) * (2.0 * n / (nWires - 1) - 1);
            Float_t coord = (MaxRadiusOfActiveVolume - WireStep) * (2.0 * n / (nWires - 1) - 1);
            v.push_back(coord);
        }
    }
    return v;
}

inline void ProcessDchDigits(TClonesArray* digits, TClonesArray * hitsArray) {

    BmnDchDigit* digit = NULL;

    //temporary containers
    vector<BmnDchDigit*> xa1;
    vector<BmnDchDigit*> xa2;
    vector<BmnDchDigit*> xb1;
    vector<BmnDchDigit*> xb2;
    vector<BmnDchDigit*> ya1;
    vector<BmnDchDigit*> ya2;
    vector<BmnDchDigit*> yb1;
    vector<BmnDchDigit*> yb2;
    vector<BmnDchDigit*> ua1;
    vector<BmnDchDigit*> ua2;
    vector<BmnDchDigit*> ub1;
    vector<BmnDchDigit*> ub2;
    vector<BmnDchDigit*> va1;
    vector<BmnDchDigit*> va2;
    vector<BmnDchDigit*> vb1;
    vector<BmnDchDigit*> vb2;

    //    const Float_t errX = kWireStep / Sqrt(12.0);
    //    const Float_t errY = kWireStep / Sqrt(12.0);
    //    const Float_t errZ = 1.0 / Sqrt(12.0); // zStep = 1.0 cm
    //    TVector3 errors = TVector3(errX, errY, errZ); //FIXME!!! Calculate by formulae

    for (Int_t iDig = 0; iDig < digits->GetEntriesFast(); ++iDig) {
        digit = (BmnDchDigit*) digits->At(iDig);
        if (digit == NULL) continue;
        //cout << "plane = " << digit->GetPlane() << " wire = " << digit->GetWireNumber() << endl;
        Short_t plane = digit->GetPlane();
        if (digit->GetTime() <= 450) continue;
        switch (plane) {
            case 0: va1.push_back(digit);
                break;
            case 1: vb1.push_back(digit);
                break;
            case 2: ua1.push_back(digit);
                break;
            case 3: ub1.push_back(digit);
                break;
            case 4: xa1.push_back(digit);
                break;
            case 5: xb1.push_back(digit);
                break;
            case 6: ya1.push_back(digit);
                break;
            case 7: yb1.push_back(digit);
                break;
            case 8: va2.push_back(digit);
                break;
            case 9: vb2.push_back(digit);
                break;
            case 10: ua2.push_back(digit);
                break;
            case 11: ub2.push_back(digit);
                break;
            case 12: xa2.push_back(digit);
                break;
            case 13: xb2.push_back(digit);
                break;
            case 14: ya2.push_back(digit);
                break;
            case 15: yb2.push_back(digit);
                break;
        }
    }

    vector<Float_t> v1 = MergeSubPlanes(va1, vb1);
    vector<Float_t> u1 = MergeSubPlanes(ua1, ub1);
    vector<Float_t> x1 = MergeSubPlanes(xa1, xb1);
    vector<Float_t> y1 = MergeSubPlanes(ya1, yb1);
    vector<Float_t> v2 = MergeSubPlanes(va2, vb2);
    vector<Float_t> u2 = MergeSubPlanes(ua2, ub2);
    vector<Float_t> x2 = MergeSubPlanes(xa2, xb2);
    vector<Float_t> y2 = MergeSubPlanes(ya2, yb2);

    vector<TVector3> u1v1 = CreateHitsByTwoPlanes(u1, v1, -5.0);
    vector<TVector3> x1y1 = CreateHitsByTwoPlanes(x1, y1, 5.0);
    vector<TVector3> u2v2 = CreateHitsByTwoPlanes(u2, v2, -5.0);
    vector<TVector3> x2y2 = CreateHitsByTwoPlanes(x2, y2, 5.0);

    //    CheckHits(u1v1, x1y1);
    //    CheckHits(x1y1, u1v1);
    //    CheckHits(u2v2, x2y2);
    //    CheckHits(x2y2, u2v2);

    CombineHits(u1v1, hitsArray, 0);
    CombineHits(x1y1, hitsArray, 1);
    CombineHits(u2v2, hitsArray, 2);
    CombineHits(x2y2, hitsArray, 3);

    //    cout << "Nuber of input digits = " << digits->GetEntriesFast() << endl;
    //    cout << "Nuber of output hits  = " << hitsArray->GetEntriesFast() << endl;

}


//MWPC

inline TVector3 CalcHitPosByTwoDigits(BmnMwpcDigit* dI, BmnMwpcDigit * dJ, Float_t zPos) {
    Float_t dWireI = dI->GetWireNumber();
    Float_t dWireJ = dJ->GetWireNumber();
    Short_t localPlaneI = dI->GetPlane() % kNPlanes;
    Short_t localPlaneJ = dJ->GetPlane() % kNPlanes;

    Float_t xI = kPlaneWidth * (dWireI * 1.0 / kNWires - 0.5); //local X by wire number
    Float_t xJ = kPlaneWidth * (dWireJ * 1.0 / kNWires - 0.5); //local X by wire number
    Float_t aI; //rotation angle by plane number
    Float_t aJ; //rotation angle by plane number

    switch (localPlaneI) {
        case 0:
            xI = xI;
            aI = 0.0;
            break;
        case 1:
            xI = xI;
            aI = -kAngleStep;
            break;
        case 2:
            xI = -xI;
            aI = kAngleStep;
            break;
        case 3:
            xI = -xI;
            aI = 0.0;
            break;
        case 4:
            xI = -xI;
            aI = -kAngleStep;
            break;
        case 5:
            xI = xI;
            aI = kAngleStep;
            break;
    }

    switch (localPlaneJ) {
        case 0:
            xJ = xJ;
            aJ = 0.0;
            break;
        case 1:
            xJ = xJ;
            aJ = -kAngleStep;
            break;
        case 2:
            xJ = -xJ;
            aJ = kAngleStep;
            break;
        case 3:
            xJ = -xJ;
            aJ = 0.0;
            break;
        case 4:
            xJ = -xJ;
            aJ = -kAngleStep;
            break;
        case 5:
            xJ = xJ;
            aJ = kAngleStep;
            break;
    }
    Float_t xGlob = (xI * Sin(aJ) - xJ * Sin(aI)) / Sin(aJ - aI);
    Float_t yGlob = (xI * Cos(aJ) - xJ * Cos(aI)) / Sin(aJ - aI);
    //    Float_t zGlob = Float_t(min(dI->GetPlane() % kNPlanes + 1, dJ->GetPlane() % kNPlanes + 1) - 3); //average position between two neighbor planes
    TVector3 pos(xGlob, yGlob, zPos);
    return pos;
}

inline vector<TVector3> CreateHitsByTwoPlanes(vector<BmnMwpcDigit*> x, vector<BmnMwpcDigit*> y, Float_t zPos) {
    vector<TVector3> v;
    for (Int_t i = 0; i < x.size(); ++i) {
        BmnMwpcDigit* dI = (BmnMwpcDigit*) x.at(i);
        for (Int_t j = 0; j < y.size(); ++j) {
            BmnMwpcDigit* dJ = (BmnMwpcDigit*) y.at(j);
            if (Abs(dI->GetTime() - dJ->GetTime()) > 6) continue;
            v.push_back(CalcHitPosByTwoDigits(dI, dJ, zPos));
        }
    }
    return v;
}

inline Float_t dist(Float_t x1, Float_t y1, Float_t x2, Float_t y2) {
    return Sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

inline void CreateMwpcHits(vector<TVector3> pos, TClonesArray* hits, Short_t mwpcId) {

    const Float_t errX = kWireStep / Sqrt(12.0);
    const Float_t errY = kWireStep / Sqrt(12.0);
    const Float_t errZ = 1.0 / Sqrt(12.0); // zStep = 1.0 cm
    TVector3 errors = TVector3(errX, errY, errZ); //FIXME!!! Calculate by formulae

    TVector3 globMwpcPos;
    //TGeoVolume* pVolume = gGeoManager->GetVolume("cave");
    //if (pVolume != NULL) {
    //    TString node_name = TString::Format("mwpc%d_0", mwpcId + 1);
    //    TGeoNode* pNode = pVolume->FindNode(node_name);
    //    if (pNode != NULL) {
    //        TGeoMatrix* pMatrix = pNode->GetMatrix();
    //        globMwpcPos = TVector3(pMatrix->GetTranslation()[0], pMatrix->GetTranslation()[1], pMatrix->GetTranslation()[2]);
    //    } else
    //        cout << "MWPC detector (" << node_name << ") wasn't found." << endl;
    //} else
    //    cout << "Cave volume wasn't found." << endl;

    for (Int_t i = 0; i < pos.size(); ++i) {
        new((*hits)[hits->GetEntriesFast()]) BmnMwpcHit(0, pos.at(i) + globMwpcPos, errors, -1);
        BmnMwpcHit* hit = (BmnMwpcHit*) hits->At(hits->GetEntriesFast() - 1);
        hit->SetMwpcId(mwpcId);
        hit->SetHitId(hits->GetEntriesFast() - 1);
    }
}

inline void FindNeighbour(BmnMwpcDigit* digiStart, vector<BmnMwpcDigit*> digits, vector<BmnMwpcDigit*> buffer) {
    for (Int_t i = 0; i < digits.size(); ++i) {
        BmnMwpcDigit* digi = digits.at(i);
        if (digi->IsUsed()) continue;
        if (Abs((Int_t) digiStart->GetWireNumber() - (Int_t) digi->GetWireNumber()) < 2) {
            digi->SetUsing(kTRUE);
            buffer.push_back(digi);
            FindNeighbour(digi, digits, buffer);
        }
    }
}

inline vector<BmnMwpcDigit*> CheckDigits(vector<BmnMwpcDigit*> digitsIn) {
    vector<BmnMwpcDigit*> digitsOut;
    if (digitsIn.size() > 3) return digitsOut;

    for (Int_t i = 0; i < digitsIn.size(); ++i) {
        BmnMwpcDigit* dI = digitsIn.at(i);
        vector<BmnMwpcDigit*> buffer;
        buffer.push_back(dI);
        FindNeighbour(dI, digitsIn, buffer);
        if (buffer.size() == 1) {
            digitsOut.push_back(dI); //just copy digit 
        } else {
            Float_t sumWires = 0.0;
            Float_t sumTimes = 0.0;
            for (Int_t j = 0; j < buffer.size(); ++j) {
                BmnMwpcDigit* digi = buffer.at(j);
                sumWires += digi->GetWireNumber();
                sumTimes += digi->GetTime();
            }
            BmnMwpcDigit averDigi(dI->GetPlane(), sumWires / buffer.size(), sumTimes / buffer.size(), dI->GetRefId());
            digitsOut.push_back(&averDigi);
        }
    }


    //    if (digitsIn.size() > 3) return digitsOut;
    //    for (Int_t i = 0; i < digitsIn.size(); ++i) {
    //        BmnMwpcDigit* digitIn = digitsIn.at(i);
    //        Bool_t same = kFALSE;
    //        for (Int_t j = 0; j < digitsOut.size(); ++j) {
    //            BmnMwpcDigit* digitOut = digitsOut.at(j);
    //            //            if (digitIn->GetWireNumber() == digitOut->GetWireNumber()) {
    //            if (Abs(digitIn->GetWireNumber() - digitOut->GetWireNumber()) < 2) {
    //                same = kTRUE;
    //                break;
    //            }
    //        }
    //        if (!same) {
    //            digitsOut.push_back(digitIn);
    //        }
    //    }

    return digitsOut;
}

inline BmnStatus DigitsTimeSelection(TH1F* h_times, Float_t& left, Float_t& right) {
    Float_t pedestal = h_times->GetBinContent(1);
    TF1* shiftedGaus = new TF1("ShiftedGaus", "gaus+[3]", 0.0, 40.0);
    shiftedGaus->SetParameters(pedestal, 20, 5, pedestal);
    h_times->Fit("ShiftedGaus", "SQww");
    Float_t mean = shiftedGaus->GetParameter(1);
    Float_t sigma = shiftedGaus->GetParameter(2);
    left = mean - 3 * sigma;
    right = mean + 3 * sigma;
    if (left > 50 || left < -10 || right > 50 || right < -10) return kBMNERROR;
    else return kBMNSUCCESS;
}

inline vector<TVector3> CreateHitsBy3Planes(vector<BmnMwpcDigit*> x, vector<BmnMwpcDigit*> u, vector<BmnMwpcDigit*> v, Float_t zPos) {
    vector<TVector3> hits;
    vector<TVector3> xu;
    vector<TVector3> xv;
    vector<TVector3> uv;
    if (x.size() == 0 || u.size() == 0 || v.size() == 0) return hits;
    for (Int_t i = 0; i < x.size(); ++i) {
        BmnMwpcDigit* dI = (BmnMwpcDigit*) x.at(i);
        for (Int_t j = 0; j < u.size(); ++j) {
            BmnMwpcDigit* dJ = (BmnMwpcDigit*) u.at(j);
            xu.push_back(CalcHitPosByTwoDigits(dI, dJ, zPos));
        }
    }
    for (Int_t i = 0; i < x.size(); ++i) {
        BmnMwpcDigit* dI = (BmnMwpcDigit*) x.at(i);
        for (Int_t j = 0; j < v.size(); ++j) {
            BmnMwpcDigit* dJ = (BmnMwpcDigit*) v.at(j);
            xv.push_back(CalcHitPosByTwoDigits(dI, dJ, zPos));
        }
    }
    for (Int_t i = 0; i < u.size(); ++i) {
        BmnMwpcDigit* dI = (BmnMwpcDigit*) u.at(i);
        for (Int_t j = 0; j < v.size(); ++j) {
            BmnMwpcDigit* dJ = (BmnMwpcDigit*) v.at(j);
            uv.push_back(CalcHitPosByTwoDigits(dI, dJ, zPos));
        }
    }
    
    const Float_t thDist = 1.0; //cm
    for (Int_t i = 0; i < xu.size(); ++i) {
        for (Int_t j = 0; j < xv.size(); ++j) {
            if (Abs(xu[i].Mag() - xv[j].Mag()) > thDist) continue;
            for (Int_t k = 0; k < uv.size(); ++k) {
                if (Abs(xu[i].Mag() - uv[k].Mag()) > thDist) continue;
                if (Abs(xv[j].Mag() - uv[k].Mag()) > thDist) continue;
                Float_t xAv = (xu[i].X() + xv[j].X() + uv[k].X()) / 3.0;
                Float_t yAv = (xu[i].Y() + xv[j].Y() + uv[k].Y()) / 3.0;
                hits.push_back(TVector3(xAv, yAv, zPos));
            }
        }
    }
    //printf("hits size = %d\n", (Int_t)hits.size());
    return hits;
}

inline void ProcessMwpcDigits(TClonesArray* digits, TClonesArray * hits) {

    //temporary containers
    vector<BmnMwpcDigit*> x1_mwpc0;
    vector<BmnMwpcDigit*> u1_mwpc0;
    vector<BmnMwpcDigit*> v1_mwpc0;
    vector<BmnMwpcDigit*> x2_mwpc0;
    vector<BmnMwpcDigit*> u2_mwpc0;
    vector<BmnMwpcDigit*> v2_mwpc0;
    vector<BmnMwpcDigit*> x1_mwpc1;
    vector<BmnMwpcDigit*> u1_mwpc1;
    vector<BmnMwpcDigit*> v1_mwpc1;
    vector<BmnMwpcDigit*> x2_mwpc1;
    vector<BmnMwpcDigit*> u2_mwpc1;
    vector<BmnMwpcDigit*> v2_mwpc1;
    vector<BmnMwpcDigit*> x1_mwpc2;
    vector<BmnMwpcDigit*> u1_mwpc2;
    vector<BmnMwpcDigit*> v1_mwpc2;
    vector<BmnMwpcDigit*> x2_mwpc2;
    vector<BmnMwpcDigit*> u2_mwpc2;
    vector<BmnMwpcDigit*> v2_mwpc2;

    for (Int_t i = 0; i < digits->GetEntriesFast(); ++i) {
        BmnMwpcDigit* digit = (BmnMwpcDigit*) digits->At(i);
        Short_t dTime = digit->GetTime();
        Short_t dPlane = digit->GetPlane();
        Int_t dRef = digit->GetRefId();

        //if ((dPlane > -1 && dPlane < 6) && (dTime < mwpc0_leftTime || dTime > mwpc0_rightTime)) continue;
        //if ((dPlane > 5 && dPlane < 12) && (dTime < mwpc1_leftTime || dTime > mwpc1_rightTime)) continue;
        //if ((dPlane > 11 && dPlane < 18) && (dTime < mwpc2_leftTime || dTime > mwpc2_rightTime)) continue;

        digit->SetUsing(kFALSE); //not used in hit finding yet
        //switch (dRef) {
        //  case 0x046F304E: x1_mwpc1.push_back(digit);
        // break;
        //  case 0x046F3043: u1_mwpc1.push_back(digit);
        // break;
        //  case 0x046F1A8D: v1_mwpc1.push_back(digit);
        // break;
        ///  case 0x046F4504: x2_mwpc1.push_back(digit);
        // break;
        // case 0x046F4514: u2_mwpc1.push_back(digit);
        //break;
        //  case 0x046F45DF: v2_mwpc1.push_back(digit);
        // break;
        //  case 0x046EFA53: x1_mwpc0.push_back(digit);
        // break;
        //  case 0x046F3F1D: u1_mwpc0.push_back(digit);
        // break;
        //  case 0x046F028B: v1_mwpc0.push_back(digit);
        //break;
        //case 0x046F3F97: x2_mwpc0.push_back(digit);
        //break;
        //case 0x046F4513: u2_mwpc0.push_back(digit);
        //break;
        //case 0x046F3F8E: v2_mwpc0.push_back(digit);
        //break;
        //case 0x046F47CB: x1_mwpc2.push_back(digit);
        //break;
        //case 0x046F3F8B: u1_mwpc2.push_back(digit);
        //break;
        //case 0x046F30B8: v1_mwpc2.push_back(digit);
        //break;
        //case 0x046F2950: x2_mwpc2.push_back(digit);
        // break;
        //case 0x046F2A79: u2_mwpc2.push_back(digit);
        // break;
        //case 0x046F2FFF: v2_mwpc2.push_back(digit);
        //    break;
        //}
        switch (dPlane) {
            case 0: u1_mwpc0.push_back(digit);
                break;
            case 1: v1_mwpc0.push_back(digit);
                break;
            case 2: x1_mwpc0.push_back(digit);
                break;
            case 3: u2_mwpc0.push_back(digit);
                break;
            case 4: v2_mwpc0.push_back(digit);
                break;
            case 5: x2_mwpc0.push_back(digit);
                break;
            case 6: u1_mwpc1.push_back(digit);
                break;
            case 7: v1_mwpc1.push_back(digit);
                break;
            case 8: x1_mwpc1.push_back(digit);
                break;
            case 9: u2_mwpc1.push_back(digit);
                break;
            case 10: v2_mwpc1.push_back(digit);
                break;
            case 11: x2_mwpc1.push_back(digit);
                break;
        }
    }

    vector<BmnMwpcDigit*> x1_mwpc0_filtered = CheckDigits(x1_mwpc0);
    vector<BmnMwpcDigit*> u1_mwpc0_filtered = CheckDigits(u1_mwpc0);
    vector<BmnMwpcDigit*> v1_mwpc0_filtered = CheckDigits(v1_mwpc0);
    vector<BmnMwpcDigit*> x2_mwpc0_filtered = CheckDigits(x2_mwpc0);
    vector<BmnMwpcDigit*> u2_mwpc0_filtered = CheckDigits(u2_mwpc0);
    vector<BmnMwpcDigit*> v2_mwpc0_filtered = CheckDigits(v2_mwpc0);
    vector<BmnMwpcDigit*> x1_mwpc1_filtered = CheckDigits(x1_mwpc1);
    vector<BmnMwpcDigit*> u1_mwpc1_filtered = CheckDigits(u1_mwpc1);
    vector<BmnMwpcDigit*> v1_mwpc1_filtered = CheckDigits(v1_mwpc1);
    vector<BmnMwpcDigit*> x2_mwpc1_filtered = CheckDigits(x2_mwpc1);
    vector<BmnMwpcDigit*> u2_mwpc1_filtered = CheckDigits(u2_mwpc1);
    vector<BmnMwpcDigit*> v2_mwpc1_filtered = CheckDigits(v2_mwpc1);
    vector<BmnMwpcDigit*> x1_mwpc2_filtered = CheckDigits(x1_mwpc2);
    vector<BmnMwpcDigit*> u1_mwpc2_filtered = CheckDigits(u1_mwpc2);
    vector<BmnMwpcDigit*> v1_mwpc2_filtered = CheckDigits(v1_mwpc2);
    vector<BmnMwpcDigit*> x2_mwpc2_filtered = CheckDigits(x2_mwpc2);
    vector<BmnMwpcDigit*> u2_mwpc2_filtered = CheckDigits(u2_mwpc2);
    vector<BmnMwpcDigit*> v2_mwpc2_filtered = CheckDigits(v2_mwpc2);

    //    vector<TVector3> x1u1_mwpc0 = CreateHitsByTwoPlanes(x1_mwpc0_filtered, u1_mwpc0_filtered);
    //    vector<TVector3> v1x2_mwpc0 = CreateHitsByTwoPlanes(v1_mwpc0_filtered, x2_mwpc0_filtered);
    //    vector<TVector3> u2v2_mwpc0 = CreateHitsByTwoPlanes(u2_mwpc0_filtered, v2_mwpc0_filtered);
    //
    //    vector<TVector3> x1u1_mwpc1 = CreateHitsByTwoPlanes(x1_mwpc1_filtered, u1_mwpc1_filtered);
    //    vector<TVector3> v1x2_mwpc1 = CreateHitsByTwoPlanes(v1_mwpc1_filtered, x2_mwpc1_filtered);
    //    vector<TVector3> u2v2_mwpc1 = CreateHitsByTwoPlanes(u2_mwpc1_filtered, v2_mwpc1_filtered);
    //
    //    vector<TVector3> x1u1_mwpc2 = CreateHitsByTwoPlanes(x1_mwpc2_filtered, u1_mwpc2_filtered);
    //    vector<TVector3> v1x2_mwpc2 = CreateHitsByTwoPlanes(v1_mwpc2_filtered, x2_mwpc2_filtered);
    //    vector<TVector3> u2v2_mwpc2 = CreateHitsByTwoPlanes(u2_mwpc2_filtered, v2_mwpc2_filtered);

//    vector<TVector3> u1v1_mwpc0 = CreateHitsByTwoPlanes(u1_mwpc0_filtered, v1_mwpc0_filtered, -2.0);
//    vector<TVector3> x2u2_mwpc0 = CreateHitsByTwoPlanes(x2_mwpc0_filtered, u2_mwpc0_filtered, 0.0);
//    vector<TVector3> v2x1_mwpc0 = CreateHitsByTwoPlanes(v2_mwpc0_filtered, x1_mwpc0_filtered, 2.0);
//
//    vector<TVector3> v2u2_mwpc1 = CreateHitsByTwoPlanes(v2_mwpc1_filtered, u2_mwpc1_filtered, -2.0);
//    vector<TVector3> x2v1_mwpc1 = CreateHitsByTwoPlanes(x2_mwpc1_filtered, v1_mwpc1_filtered, 0.0);
//    vector<TVector3> u1x1_mwpc1 = CreateHitsByTwoPlanes(u1_mwpc1_filtered, x1_mwpc1_filtered, 2.0);
//
//    vector<TVector3> x1u1_mwpc2 = CreateHitsByTwoPlanes(x1_mwpc2_filtered, u1_mwpc2_filtered, -2.0);
//    vector<TVector3> v1x2_mwpc2 = CreateHitsByTwoPlanes(v1_mwpc2_filtered, x2_mwpc2_filtered, 0.0);
//    vector<TVector3> u2v2_mwpc2 = CreateHitsByTwoPlanes(u2_mwpc2_filtered, v2_mwpc2_filtered, 2.0);
    
    vector<TVector3> xuv1_mwpc0 = CreateHitsBy3Planes(x1_mwpc0_filtered, u1_mwpc0_filtered, v1_mwpc0_filtered, 0.0);
    vector<TVector3> xuv2_mwpc0 = CreateHitsBy3Planes(x2_mwpc0_filtered, u2_mwpc0_filtered, v2_mwpc0_filtered, 0.0);
    vector<TVector3> xuv1_mwpc1 = CreateHitsBy3Planes(x1_mwpc1_filtered, u1_mwpc1_filtered, v1_mwpc1_filtered, 0.0);
    vector<TVector3> xuv2_mwpc1 = CreateHitsBy3Planes(x2_mwpc1_filtered, u2_mwpc1_filtered, v2_mwpc1_filtered, 0.0);

    //    CreateMwpcHits(x1u1_mwpc0, hits, 0);
    //    CreateMwpcHits(v1x2_mwpc0, hits, 0);
    //    CreateMwpcHits(u2v2_mwpc0, hits, 0);
    //    CreateMwpcHits(x1u1_mwpc1, hits, 1);
    //    CreateMwpcHits(v1x2_mwpc1, hits, 1);
    //    CreateMwpcHits(u2v2_mwpc1, hits, 1);
    //    CreateMwpcHits(x1u1_mwpc2, hits, 2);
    //    CreateMwpcHits(v1x2_mwpc2, hits, 2);
    //    CreateMwpcHits(u2v2_mwpc2, hits, 2);

//    CreateMwpcHits(u1v1_mwpc0, hits, 0);
//    CreateMwpcHits(x2u2_mwpc0, hits, 0);
//    CreateMwpcHits(v2x1_mwpc0, hits, 0);
//    CreateMwpcHits(v2u2_mwpc1, hits, 1);
//    CreateMwpcHits(x2v1_mwpc1, hits, 1);
//    CreateMwpcHits(u1x1_mwpc1, hits, 1);
//    CreateMwpcHits(x1u1_mwpc2, hits, 2);
//    CreateMwpcHits(v1x2_mwpc2, hits, 2);
//    CreateMwpcHits(u2v2_mwpc2, hits, 2);
    
    CreateMwpcHits(xuv1_mwpc0, hits, 0);
    CreateMwpcHits(xuv2_mwpc0, hits, 0);
    CreateMwpcHits(xuv1_mwpc1, hits, 1);
    CreateMwpcHits(xuv2_mwpc1, hits, 1);
}

#endif /* BMNHITFINDERRUN1_H 1*/
