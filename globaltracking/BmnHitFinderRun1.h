#ifndef BMNHITFINDERRUN1_H
#define	BMNHITFINDERRUN1_H

#include <vector>
#include "TGeoVolume.h"
#include "TGeoManager.h"
#include "TVector3.h"
#include "BmnDchDigit.h"
#include "BmnMwpcDigit.h"
#include "BmnMwpcHit.h"
#include "TClonesArray.h"
#include "TMath.h"

using namespace std;
using namespace TMath;


// constants for mwpc ===>
const Short_t kNPlanes = 6;
const Short_t kTimeBin = 8; // ns
const Short_t kNWires = 102; //in one plane
const Float_t kAngleStep = 60; // degrees
const Float_t kWireStep = 0.25; // cm
const Float_t kPlaneHeight = 43.3; // cm
const Float_t kPlaneWidth = kNWires * kWireStep; //24.6; // cm
const Float_t kMwpcZpos = 350; //FIXME!!! Get coords from geometry
// <=== constants for mwpc

//Detector's position
const Float_t tanpipeangle = 0.3 / 5.7; // tangent of pipe angle
//const Double_t DCH1_Xpos = 0.0;
const Float_t DCH1_Ypos = 0.0;
const Float_t DCH1_Zpos = 550.0; //cm
const Float_t DCH2_Zpos = 650.0; //cm
const Float_t DCH1_Xpos = DCH1_Zpos*tanpipeangle;

//Detector's construct parameters
const Float_t ZLength_DCH1 = 20.0;
const Float_t ZLength_DCH1ActiveVolume = 0.6;
const Float_t HoleSize_DCH1 = 12.0;

const Float_t SideLengthOfOctagon = 120.0;
const Float_t InnerRadiusOfOctagon = (2.414 * SideLengthOfOctagon) / 2.0;

const UInt_t nWires = 240; // 0 - 255
const UInt_t nPlanes = 16; // 0 - 15
const Float_t WireStep = 2.0 * InnerRadiusOfOctagon / nWires;
const Float_t HalfStep = WireStep / 2.0;

const Float_t MaxRadiusOfActiveVolume = 120.0;
const Float_t MinRadiusOfActiveVolume = 12.0;

void CombineHits(vector<TVector3> vec, TClonesArray* hits, Short_t plane) {
    
    TVector3 dchPos;

    TGeoVolume* pVolume = gGeoManager->GetVolume("cave");
    if (pVolume != NULL) {
        TString node_name = TString::Format("dch%d_0", plane / 2 + 1);
        TGeoNode* pNode = pVolume->FindNode(node_name);
        if (pNode != NULL) {
            TGeoMatrix* pMatrix = pNode->GetMatrix();
            dchPos = TVector3(pMatrix->GetTranslation()[0], pMatrix->GetTranslation()[1], pMatrix->GetTranslation()[2]);
        } else
            cout << "DCH detector (" << node_name << ") wasn't found." << endl;
    } else
        cout << "Cave volume wasn't found." << endl;
    
    for (Int_t i = 0; i < vec.size(); ++i) {
        TVector3 hit = vec.at(i);
        if ((plane == 0) || (plane == 2)) {
            hit.RotateZ(-135.0 * DegToRad());
            hit.SetY(-1.0 * hit.Y());
        }
        new((*hits)[hits->GetEntriesFast()]) BmnDchHit(0, hit + dchPos, TVector3(0, 0, 0), 0, 0, 0, plane);
        BmnDchHit* dchHit = (BmnDchHit*) hits->At(hits->GetEntriesFast() - 1);
        dchHit->SetDchId(plane / 2 + 1);
    }
}

vector<TVector3> CreateHitsByTwoPlanes(vector<Float_t> vec1, vector<Float_t> vec2, Float_t zPos) {
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

vector<Float_t> MergeSubPlanes(vector<BmnDchDigit*> vec1, vector<BmnDchDigit*> vec2) {
    vector<Float_t> v;
    for (Int_t i = 0; i < vec1.size(); ++i) {
        BmnDchDigit* d1 = (BmnDchDigit*) vec1.at(i);
        for (Int_t j = 0; j < vec2.size(); ++j) {
            BmnDchDigit* d2 = (BmnDchDigit*) vec2.at(j);
            if (Abs(d1->GetWireNumber() - d2->GetWireNumber()) > 1) continue;
            Float_t n = (d1->GetWireNumber() + d2->GetWireNumber()) / 2.0;
            Float_t coord = (InnerRadiusOfOctagon - WireStep) * (2.0 * n / (nWires - 1) - 1);
            v.push_back(coord);
        }
    }
    return v;
}

void ProcessDchDigits(TClonesArray* digits, TClonesArray* hitsArray) {

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

    CombineHits(u1v1, hitsArray, 0);
    CombineHits(x1y1, hitsArray, 1);
    CombineHits(u2v2, hitsArray, 2);
    CombineHits(x2y2, hitsArray, 3);

    //    cout << "Nuber of input digits = " << digits->GetEntriesFast() << endl;
    //    cout << "Nuber of output hits  = " << hitsArray->GetEntriesFast() << endl;

}

TVector3 CalcHitPosByTwoDigits(BmnMwpcDigit* dI, BmnMwpcDigit* dJ) {
    Short_t dWireI = dI->GetWireNumber();
    Short_t dWireJ = dJ->GetWireNumber();
    Float_t xI = kPlaneWidth * (dWireI * 1.0 / kNWires - 0.5); //local X by wire number
    Float_t xJ = kPlaneWidth * (dWireJ * 1.0 / kNWires - 0.5); //local X by wire number
    Float_t aI = (dI->GetPlane() - 1) * kAngleStep * DegToRad(); //rotation angle by plane number
    Float_t aJ = (dJ->GetPlane() - 1) * kAngleStep * DegToRad(); //rotation angle by plane number
    Float_t xGlob = (xI * Sin(aJ) - xJ * Sin(aI)) / Sin(aJ - aI);
    Float_t yGlob = (xI * Cos(aJ) - xJ * Cos(aI)) / Sin(aJ - aI);
    Float_t zGlob = Float_t(min(dI->GetPlane(), dJ->GetPlane()) - 3); //average position between two neighbor planes
    Float_t ref = -1;
    if (dI->GetRefId() == dI->GetRefId()) ref = dI->GetRefId();
    TVector3 pos(xGlob, yGlob, zGlob);
    return pos;
}

vector<TVector3> CreateHitsByTwoPlanes(vector<BmnMwpcDigit*> x, vector<BmnMwpcDigit*> y) {
    vector<TVector3> v;
    for (Int_t i = 0; i < x.size(); ++i) {
        BmnMwpcDigit* dI = (BmnMwpcDigit*) x.at(i);
        for (Int_t j = 0; j < y.size(); ++j) {
            BmnMwpcDigit* dJ = (BmnMwpcDigit*) y.at(j);
            if (dI->GetTime() != dJ->GetTime()) continue;
            v.push_back(CalcHitPosByTwoDigits(dI, dJ));
        }
    }
    return v;
}

Float_t dist(Float_t x1, Float_t y1, Float_t x2, Float_t y2) {
    return Sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

void CreateMwpcHits(vector<TVector3> pos, TClonesArray* hits, Short_t mwpcId) {

    const Float_t errX = kWireStep / Sqrt(12.0);
    const Float_t errY = kWireStep / Sqrt(12.0);
    const Float_t errZ = 1.0 / Sqrt(12.0); // zStep = 1.0 cm
    TVector3 errors = TVector3(errX, errY, errZ); //FIXME!!! Calculate by formulae
    TVector3 mwpcPos;

    TGeoVolume* pVolume = gGeoManager->GetVolume("cave");
    if (pVolume != NULL) {
        TString node_name = TString::Format("mwpc%d_0", mwpcId + 1);
        TGeoNode* pNode = pVolume->FindNode(node_name);
        if (pNode != NULL) {
            TGeoMatrix* pMatrix = pNode->GetMatrix();
            mwpcPos = TVector3(pMatrix->GetTranslation()[0], pMatrix->GetTranslation()[1], pMatrix->GetTranslation()[2]);
        } else
            cout << "MWPC detector (" << node_name << ") wasn't found." << endl;
    } else
        cout << "Cave volume wasn't found." << endl;


    for (Int_t i = 0; i < pos.size(); ++i) {
        new((*hits)[hits->GetEntriesFast()]) BmnMwpcHit(0, pos.at(i) + mwpcPos, errors, -1);
        BmnMwpcHit* hit = (BmnMwpcHit*) hits->At(hits->GetEntriesFast() - 1);
        hit->SetMwpcId(mwpcId);
    }



}

void ProcessMwpcDigits(TClonesArray* digits, TClonesArray* hits) {

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
        Short_t dPlane = digit->GetPlane();
        switch (dPlane) {
            case 0: x1_mwpc0.push_back(digit);
                break;
            case 1: u1_mwpc0.push_back(digit);
                break;
            case 2: v1_mwpc0.push_back(digit);
                break;
            case 3: x2_mwpc0.push_back(digit);
                break;
            case 4: u2_mwpc0.push_back(digit);
                break;
            case 5: v2_mwpc0.push_back(digit);
                break;
            case 6: x1_mwpc1.push_back(digit);
                break;
            case 7: u1_mwpc1.push_back(digit);
                break;
            case 8: v1_mwpc1.push_back(digit);
                break;
            case 9: x2_mwpc1.push_back(digit);
                break;
            case 10: u2_mwpc1.push_back(digit);
                break;
            case 11: v2_mwpc1.push_back(digit);
                break;
            case 12: x1_mwpc2.push_back(digit);
                break;
            case 13: u1_mwpc2.push_back(digit);
                break;
            case 14: v1_mwpc2.push_back(digit);
                break;
            case 15: x2_mwpc2.push_back(digit);
                break;
            case 16: u2_mwpc2.push_back(digit);
                break;
            case 17: v2_mwpc2.push_back(digit);
                break;
        }
    }

    //    Float_t delta = (fMwpcNum == 1) ? 1.0 : (fMwpcNum == 2) ? 1.5 : 2.00; //cm
    //    Float_t delta = 2.00; //cm //FIXME!

    vector<TVector3> x1u1_mwpc0 = CreateHitsByTwoPlanes(x1_mwpc0, u1_mwpc0);
    vector<TVector3> v1x2_mwpc0 = CreateHitsByTwoPlanes(v1_mwpc0, x2_mwpc0);
    vector<TVector3> u2v2_mwpc0 = CreateHitsByTwoPlanes(u2_mwpc0, v2_mwpc0);

    vector<TVector3> x1u1_mwpc1 = CreateHitsByTwoPlanes(x1_mwpc1, u1_mwpc1);
    vector<TVector3> v1x2_mwpc1 = CreateHitsByTwoPlanes(v1_mwpc1, x2_mwpc1);
    vector<TVector3> u2v2_mwpc1 = CreateHitsByTwoPlanes(u2_mwpc1, v2_mwpc1);

    vector<TVector3> x1u1_mwpc2 = CreateHitsByTwoPlanes(x1_mwpc2, u1_mwpc2);
    vector<TVector3> v1x2_mwpc2 = CreateHitsByTwoPlanes(v1_mwpc2, x2_mwpc2);
    vector<TVector3> u2v2_mwpc2 = CreateHitsByTwoPlanes(u2_mwpc2, v2_mwpc2);

    CreateMwpcHits(x1u1_mwpc0, hits, 0);
    CreateMwpcHits(v1x2_mwpc0, hits, 0);
    CreateMwpcHits(u2v2_mwpc0, hits, 0);
    CreateMwpcHits(x1u1_mwpc1, hits, 1);
    CreateMwpcHits(v1x2_mwpc1, hits, 1);
    CreateMwpcHits(u2v2_mwpc1, hits, 1);
    CreateMwpcHits(x1u1_mwpc2, hits, 2);
    CreateMwpcHits(v1x2_mwpc2, hits, 2);
    CreateMwpcHits(u2v2_mwpc2, hits, 2);
}

#endif	/* BMNHITFINDERRUN1_H */