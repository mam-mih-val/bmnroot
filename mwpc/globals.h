#include "TMath.h"
#include "TVector3.h"
#include "../bmndata/BmnMwpcDigit.h"

using namespace TMath;
using namespace std;

const Short_t kTimeBin = 8; // ns
const Short_t kNWires = 102; //in one plane
const Float_t kAngleStep = 60; // degrees
const Float_t kWireStep = 0.25; // cm
const Float_t kPlaneHeight = 43.3; // cm
const Float_t kPlaneWidth = kNWires * kWireStep; // cm

const Float_t kMwpcZpos = 50.0; // z-position of the center of MWPC
const Int_t kMwpcNum = 1; //number of MWPC (from 1 to 3)

// nWire1 - number of the first wire      //from 0 to 102
// nWire2 - number of the second wire     //from 0 to 102
// nPlane1 - number of the first plane    //from 1 to 6
// nPlane2 - number of the second plane   //from 1 to 6

TVector3* CalcHitPosByTwoDigits(BmnMwpcDigit* dI, BmnMwpcDigit* dJ) {
    Short_t dWireI = dI->GetWireNumber();
    Short_t dWireJ = dJ->GetWireNumber();
    Float_t xI = kPlaneWidth * (dWireI * 1.0 / kNWires - 0.5); //local X by wire number
    Float_t xJ = kPlaneWidth * (dWireJ * 1.0 / kNWires - 0.5); //local X by wire number
    Float_t aI = (dI->GetPlane() - 1) * kAngleStep * DegToRad(); //rotation angle by plane number
    Float_t aJ = (dJ->GetPlane() - 1) * kAngleStep * DegToRad(); //rotation angle by plane number
    Float_t xGlob = (xI * Sin(aJ) - xJ * Sin(aI)) / Sin(aJ - aI);
    Float_t yGlob = (xI * Cos(aJ) - xJ * Cos(aI)) / Sin(aJ - aI);
    Float_t zGlob = kMwpcZpos - Float_t(min(dI->GetPlane(), dJ->GetPlane()) - 3); //average position between two neighbor planes
    TVector3* pos = new TVector3(xGlob, yGlob, zGlob);
    return pos;
}

vector<TVector3*> CreateHitsByTwoPlanes(vector<BmnMwpcDigit*> x, vector<BmnMwpcDigit*> y) {
    vector<TVector3*> v;
    for (Int_t i = 0; i < x.size(); ++i) {
        BmnMwpcDigit* dI = (BmnMwpcDigit*) x.at(i);
        for (Int_t j = 0; j < y.size(); ++j) {
            BmnMwpcDigit* dJ = (BmnMwpcDigit*) y.at(j);
            //if (dI->GetTime() != dJ->GetTime()) continue;
            v.push_back(CalcHitPosByTwoDigits(dI, dJ));
        }
    }
    return v;
}

vector<TVector3*> SearchHits(vector<BmnMwpcDigit*> x1, vector<BmnMwpcDigit*> u1, vector<BmnMwpcDigit*> v1, vector<BmnMwpcDigit*> x2, vector<BmnMwpcDigit*> u2, vector<BmnMwpcDigit*> v2) {

    Float_t x = 0.0;
    Float_t y = 0.0;
    Float_t z = 0.0;

    //temporary parameter for excluding fakes
    Float_t delta = (kMwpcNum == 1) ? 4.0 : (kMwpcNum == 2) ? 4.5 : 5.0; //cm

    vector<TVector3*> x1u1 = CreateHitsByTwoPlanes(x1, u1);
    vector<TVector3*> u1v1 = CreateHitsByTwoPlanes(u1, v1);
    vector<TVector3*> v1x2 = CreateHitsByTwoPlanes(v1, x2);
    vector<TVector3*> x2u2 = CreateHitsByTwoPlanes(x2, u2);
    vector<TVector3*> u2v2 = CreateHitsByTwoPlanes(u2, v2);
    vector<TVector3*> v2x1 = CreateHitsByTwoPlanes(v2, x1);

    TVector3* pos1 = NULL;
    TVector3* pos2 = NULL;
    TVector3* pos3 = NULL;

    vector<TVector3*> hits;


    for (Int_t i1 = 0; i1 < x1u1.size(); ++i1) {
        pos1 = x1u1.at(i1);
        if (pos1->z() < 0) continue;
        for (Int_t i2 = 0; i2 < v1x2.size(); ++i2) {
            pos2 = v1x2.at(i2);
            if (pos2->z() < 0 || pos1->z() < 0) continue;
            if (Sqrt((pos1->x() - pos2->x()) * (pos1->x() - pos2->x()) + (pos1->y() - pos2->y()) * (pos1->y() - pos2->y())) > delta) continue;
            for (Int_t i3 = 0; i3 < u2v2.size(); ++i3) {
                pos3 = u2v2.at(i3);
                if (pos3->z() < 0 || pos2->z() < 0 || pos1->z() < 0) continue;
                if (Sqrt((pos2->x() - pos3->x()) * (pos2->x() - pos3->x()) + (pos2->y() - pos3->y()) * (pos2->y() - pos3->y())) > delta) continue;
                //Int_t ref = (dI->GetRefId() == dJ->GetRefId() && dI->GetRefId() == dK->GetRefId()) ? dI->GetRefId() : -1;
                //cout << "ref = " << ref << " | x1:" << dI->GetRefId() << " | u1:" << dJ->GetRefId() << " | v1:" << dK->GetRefId() << endl;
                x = (pos1->x() + pos2->x() + pos3->x()) / 3;
                y = (pos1->y() + pos2->y() + pos3->y()) / 3;
                z = (pos1->z() + pos2->z() + pos3->z()) / 3;
                pos1->SetZ(-1.0);
                pos2->SetZ(-1.0);
                pos3->SetZ(-1.0);
                TVector3* coord = new TVector3(x, y, z);
                hits.push_back(coord);
            }
        }
    }

    Double_t k = Tan(kAngleStep / 2 * DegToRad());
    Double_t b = kPlaneWidth * k;
    delta = (kMwpcNum == 1) ? 0.5 : (kMwpcNum == 2) ? 0.7 : 1.0; //cm

    //Checking angle s between x1 and u1, x2 and u2
    for (Int_t i = 0; i < x1u1.size(); ++i) {
        TVector3* pos1 = x1u1.at(i);
        if (pos1->z() < 0) continue;
        if ((pos1->y() < (-k * pos1->x() + b)) && pos1->y() < (-k * pos1->x() - b)) continue;
        for (Int_t j = 0; j < x2u2.size(); ++j) {
            TVector3* pos2 = x2u2.at(j);
            if (pos2->z() < 0 || pos1->z() < 0) continue;
            if ((pos2->y() < (-k * pos2->x() + b)) && pos2->y() < (-k * pos2->x() - b)) continue;
            if (Sqrt((pos1->x() - pos2->x()) * (pos1->x() - pos2->x()) + (pos1->y() - pos2->y()) * (pos1->y() - pos2->y())) > delta) continue;
            x = (pos1->x() + pos2->x()) * 0.5;
            y = (pos1->y() + pos2->y()) * 0.5;
            z = (pos1->z() + pos2->z()) * 0.5;

            pos1->SetZ(-1.0);
            pos2->SetZ(-1.0);

            TVector3* coord = new TVector3(x, y, z);
            hits.push_back(coord);
        }
    }

    //Checking angle s between u1 and v1, u2 and v2
    for (Int_t i = 0; i < u1v1.size(); ++i) {
        TVector3* pos1 = u1v1.at(i);
        if (pos1->z() < 0) continue;
        if (pos1->x() > (-kPlaneWidth * 0.5) && pos1->x() < (kPlaneWidth * 0.5)) continue;
        for (Int_t j = 0; j < u2v2.size(); ++j) {
            TVector3* pos2 = u2v2.at(j);
            if (pos2->z() < 0 || pos1->z() < 0) continue;
            if (pos2->x() > (-kPlaneWidth * 0.5) && pos2->x() < (kPlaneWidth * 0.5)) continue;
            if (Sqrt((pos1->x() - pos2->x()) * (pos1->x() - pos2->x()) + (pos1->y() - pos2->y()) * (pos1->y() - pos2->y())) > delta) continue;
            x = (pos1->x() + pos2->x()) * 0.5;
            y = (pos1->y() + pos2->y()) * 0.5;
            z = (pos1->z() + pos2->z()) * 0.5;

            pos1->SetZ(-1.0);
            pos2->SetZ(-1.0);

            TVector3* coord = new TVector3(x, y, z);
            hits.push_back(coord);
        }
    }

    //Checking angle s between v1 and x2, v2 and x1
    for (Int_t i = 0; i < v1x2.size(); ++i) {
        TVector3* pos1 = v1x2.at(i);
        if (pos1->z() < 0) continue;
        if ((pos1->y() < (k * pos1->x() + b)) && pos1->y() < (k * pos1->x() - b)) continue;
        for (Int_t j = 0; j < v2x1.size(); ++j) {
            TVector3* pos2 = v2x1.at(j);
            if (pos2->z() < 0 || pos1->z() < 0) continue;
            if ((pos2->y() < (k * pos2->x() + b)) && pos2->y() < (k * pos2->x() - b)) continue;
            if (Sqrt((pos1->x() - pos2->x()) * (pos1->x() - pos2->x()) + (pos1->y() - pos2->y()) * (pos1->y() - pos2->y())) > delta) continue;
            x = (pos1->x() + pos2->x()) * 0.5;
            y = (pos1->y() + pos2->y()) * 0.5;
            z = (pos1->z() + pos2->z()) * 0.5;

            pos1->SetZ(-1.0);
            pos2->SetZ(-1.0);

            TVector3* coord = new TVector3(x, y, z);
            hits.push_back(coord);
        }
    }

    return hits;
}
