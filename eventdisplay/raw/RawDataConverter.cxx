#include "RawDataConverter.h"
#include "BmnMwpcHit.h"

#include "TGeoManager.h"
#include "TMath.h"
using namespace TMath;

#include <iostream>
using namespace std;

RawDataConverter::RawDataConverter()
{
    kTimeBin = 8; // ns
    kNWires = 96; //in one plane
    kAngleStep = 60.0; // degrees
    kWireStep = 0.25; // cm
    kPlaneWidth = kNWires * kWireStep; // cm

    kMwpcZpos = 50.0; // z-position of the center of MWPC

    MwpcPosition[0] = NULL; MwpcPosition[1] = NULL; MwpcPosition[2] = NULL;
}

RawDataConverter::~RawDataConverter()
{
}

vector<TVector3*> RawDataConverter::MWPCEventToGeoVector(EventData* curEvent)
{
    bool isEmptyPlane = false;
    for (int j = 0; j< 6; j++)
    {
        if (curEvent->MWPC1Planes[j].size() == 0)
            isEmptyPlane = true;
    }

    if (isEmptyPlane)
        cout<<"WARNING!!! digits count 0"<<endl;

    cout<<"Search hits: "<<curEvent->MWPC1Planes[0].size()<<" : "<<curEvent->MWPC1Planes[1].size()<<" : "<<curEvent->MWPC1Planes[2].size()<<" : "
        <<curEvent->MWPC1Planes[3].size()<<" : "<<curEvent->MWPC1Planes[4].size()<<" : "<<curEvent->MWPC1Planes[5].size()<<endl;

    vector<TVector3*> event_hits = SearchHits(curEvent->MWPC1Planes[0], curEvent->MWPC1Planes[1], curEvent->MWPC1Planes[2],
                                              curEvent->MWPC1Planes[3], curEvent->MWPC1Planes[4], curEvent->MWPC1Planes[5]);

    return event_hits;
}

TEvePointSet* RawDataConverter::Vector2EvePoints(vector<TVector3*>* pPointVector, TString strSetName, Color_t fColor, Style_t fStyle, Int_t iMarkerSize, bool isDebug)
{
    Int_t npoints = pPointVector->size();
    TEvePointSet* q = new TEvePointSet(strSetName, npoints, TEvePointSelectorConsumer::kTVT_XYZ);

    q->SetOwnIds(kTRUE);
    q->SetMarkerColor(fColor);
    q->SetMarkerSize(iMarkerSize);
    q->SetMarkerStyle(fStyle);

    for (Int_t i = 0; i < npoints; i++)
    {
        TVector3* vec = (*pPointVector)[i];
        if (isDebug)
            cout<<"Point "<<i<<": x="<<vec->X()<<" y="<<vec->Y()<<" z="<<vec->Z()<<endl;

        q->SetNextPoint(vec->X(), vec->Y(), vec->Z());
        q->SetPointId(new TNamed(Form("Point %d", i), strSetName.Data()));

        // bug in ROOT with one point drawing
        if (npoints == 1)
        {
            q->SetNextPoint(vec->X()+0.0001, vec->Y(), vec->Z());
            q->SetPointId(new TNamed("Point 1", strSetName.Data()));
        }
    }

    return q;
}

void RawDataConverter::SetMwpcPosition(int mwpc_number, TVector3 mwpc_position)
{
    if ((mwpc_number > 0) && (mwpc_number < 4))
        MwpcPosition[mwpc_number-1] = new TVector3(mwpc_position);
    else
        cout<<"RawDataConverter::SetMwpcPosition function error: incorrect MWPC number - "<<mwpc_number<<endl;
}

void RawDataConverter::MwpcDigits2MwpcHits(TClonesArray* pMwpcDigits, TClonesArray* pMwpcHits)
{
    // divide digits by three MWPC
    vector<BmnMwpcDigit*> x[3][6];
    for (int i = 0; i < pMwpcDigits->GetEntriesFast(); i++)
    {
        BmnMwpcDigit* pDigit = (BmnMwpcDigit*)pMwpcDigits->At(i);
        int mwpc_number = pDigit->GetPlane() / 6;
        int plane_number = pDigit->GetPlane() - mwpc_number*6;
        pDigit->SetPlane(plane_number);
        x[mwpc_number][plane_number].push_back(pDigit);
    }

    // for three MWPC detectors convert to MWPC hits
    for (int i = 0; i < 3; i++)
    {
        vector<TVector3*> hits = SearchHits(x[i][0], x[i][1], x[i][2], x[i][3], x[i][4], x[i][5]);

        // correct MWPC position
        TVector3* pShift = MwpcPosition[i];
        if (pShift != NULL)
        {
            double dx = pShift->X(), dy = pShift->Y(), dz = pShift->Z();
            for (int j = 0; j < hits.size(); j++)
            {
                *hits[j] += TVector3(dx, dy, dz - kMwpcZpos);
            }
        }

        // write to MwpcHits array (TClonesArray)
        for (int j = 0; j < hits.size(); j++)
        {
            new ((*pMwpcHits)[pMwpcHits->GetEntriesFast()]) BmnMwpcHit(0, *hits[j], TVector3(0, 0, 0), i);
            //cout<<"Hit X: "<<hits[j]->X()<<" Y:"<<hits[j]->Y()<<" Z:"<<hits[j]->Z()<<endl;
        }
    }

    return;
}

TVector3* RawDataConverter::CalcHitPosByTwoDigits(BmnMwpcDigit* dI, BmnMwpcDigit* dJ)
{
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

vector<TVector3*> RawDataConverter::CreateHitsByTwoPlanes(vector<BmnMwpcDigit*> x, vector<BmnMwpcDigit*> y) {
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

vector<TVector3*> RawDataConverter::SearchHits(vector<BmnMwpcDigit*> x1, vector<BmnMwpcDigit*> u1, vector<BmnMwpcDigit*> v1, vector<BmnMwpcDigit*> x2, vector<BmnMwpcDigit*> u2, vector<BmnMwpcDigit*> v2)
{
    Float_t x = 0.0;
    Float_t y = 0.0;
    Float_t z = 0.0;

    //temporary parameter for excluding fakes
    Float_t delta = 4.0; //cm

    vector<TVector3*> x1u1 = CreateHitsByTwoPlanes(x1, u1);
    vector<TVector3*> u1v1 = CreateHitsByTwoPlanes(u1, v1);
    vector<TVector3*> v1x2 = CreateHitsByTwoPlanes(v1, x2);
    vector<TVector3*> x2u2 = CreateHitsByTwoPlanes(x2, u2);
    vector<TVector3*> u2v2 = CreateHitsByTwoPlanes(u2, v2);
    vector<TVector3*> v2x1 = CreateHitsByTwoPlanes(v2, x1);

    TVector3* pos3 = NULL;
    vector<TVector3*> hits;

    for (Int_t i1 = 0; i1 < x1u1.size(); ++i1) {
        TVector3* pos1 = x1u1.at(i1);
        if (pos1->z() < 0) continue;
        for (Int_t i2 = 0; i2 < v1x2.size(); ++i2) {
            TVector3* pos2 = v1x2.at(i2);
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
    delta = 0.5; //cm

    //Checking angle s between x1 and u1, x2 and u2
    for (Int_t i = 0; i < x1u1.size(); ++i)
    {
        TVector3* pos1 = x1u1.at(i);
        if (pos1->z() < 0) continue;
        if ((pos1->y() < (-k * pos1->x() + b)) && pos1->y() < (-k * pos1->x() - b)) continue;
        for (Int_t j = 0; j < x2u2.size(); ++j)
        {
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
