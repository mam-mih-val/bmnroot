// @(#)bmnroot/mwpc:$Id$
// Author: Pavel Batyuk (VBLHEP) <pavel.batyuk@jinr.ru> 2017-02-10

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// BmnMwpcHitFinder                                                           //
//                                                                            //
// Implementation of an algorithm developed by                                // 
// S. Merts and P. Batyuk                                                     //
// to the BmnRoot software                                                    //
//                                                                            //
// The algorithm serves for searching for hits                                //
// in the Multi Wire Prop. Chambers of the BM@N experiment                    //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
#include <Rtypes.h>
#include <climits>

#include "BmnMwpcHitFinder.h"
static Float_t workTime = 0.0;

using namespace std;
using namespace TMath;

BmnMwpcHitFinder::BmnMwpcHitFinder(Bool_t isExp) :
fEventNo(0),
fUseDigitsInTimeBin(kTRUE),
expData(isExp) {
    fInputBranchName = "MWPC";
    fOutputBranchName = "BmnMwpcHit";

    thDist = 1.;
    nInputDigits = 3;
    fMinTime = 0;
    fMaxTime = 352;
}

BmnMwpcHitFinder::~BmnMwpcHitFinder() {

}

InitStatus BmnMwpcHitFinder::Init() {
    if (!expData)
        return kERROR;
    cout << " BmnMwpcHitFinder::Init() " << endl;

    FairRootManager* ioman = FairRootManager::Instance();
    fBmnMwpcDigitArray = (TClonesArray*) ioman->GetObject(fInputBranchName);
    fBmnMwpcHitArray = new TClonesArray(fOutputBranchName);
    ioman->Register(fOutputBranchName.Data(), "MWPC", fBmnMwpcHitArray, kTRUE);

    fMwpcGeometry = new BmnMwpcGeometry();

    return kSUCCESS;
}

void BmnMwpcHitFinder::Exec(Option_t* opt) {
    clock_t tStart = clock();
    cout << "\n======================== MWPC hit finder exec started ====================\n" << endl;
    cout << "Event number: " << fEventNo++ << endl;

    fBmnMwpcHitArray->Clear();
    Short_t kNChambers = fMwpcGeometry->GetNChambers();
    Short_t kNPlanes = fMwpcGeometry->GetNPlanes();

    vector <BmnMwpcDigit*> digits[kNChambers][kNPlanes];
    vector <BmnMwpcDigit*> digits_filtered[kNChambers][kNPlanes];

    for (Int_t iDigit = 0; iDigit < fBmnMwpcDigitArray->GetEntriesFast(); iDigit++) {
        BmnMwpcDigit* digit = (BmnMwpcDigit*) fBmnMwpcDigitArray->UncheckedAt(iDigit);
        digit->SetUsing(kFALSE);

        digits[digit->GetPlane() / kNPlanes][digit->GetPlane() % kNPlanes].push_back(digit);
    }

    for (Int_t iChamber = 0; iChamber < kNChambers; iChamber++) {
        for (Int_t iPlane = 0; iPlane < kNPlanes; iPlane++)
            digits_filtered[iChamber][iPlane] = CheckDigits(digits[iChamber][iPlane]);

        CreateMwpcHits(CreateHitsBy3Planes(digits_filtered[iChamber][0], digits_filtered[iChamber][1], digits_filtered[iChamber][2], 0.0), fBmnMwpcHitArray, iChamber);
        CreateMwpcHits(CreateHitsBy3Planes(digits_filtered[iChamber][3], digits_filtered[iChamber][4], digits_filtered[iChamber][5], 0.0), fBmnMwpcHitArray, iChamber);
    }
    cout << "\n======================== MWPC hit finder exec finished ===================" << endl;
    clock_t tFinish = clock();
    workTime += ((Float_t) (tFinish - tStart)) / CLOCKS_PER_SEC;
}

void BmnMwpcHitFinder::CreateMwpcHits(vector <TVector3> pos, TClonesArray* hits, Short_t mwpcId) {
    Double_t kWireStep = fMwpcGeometry->GetWireStep();
    TVector3 errors(kWireStep / Sqrt(12.0), kWireStep / Sqrt(12.0), 1.0 / Sqrt(12.0));

    if (pos.size() == 0)
        return;

    for (Int_t iSize = 0; iSize < pos.size(); iSize++) {
        BmnMwpcHit* hit = new((*hits)[hits->GetEntriesFast()]) BmnMwpcHit(0, pos.at(iSize), errors, -1);
        hit->SetMwpcId(mwpcId);
    }
}

vector <BmnMwpcDigit*> BmnMwpcHitFinder::CheckDigits(vector <BmnMwpcDigit*> digitsIn) {
    vector <BmnMwpcDigit*> digitsOut;
    if (digitsIn.size() > nInputDigits)
        return digitsOut;

    vector <BmnMwpcDigit*> digitsOrderedInTime;

    multimap <UInt_t, BmnMwpcDigit*> digitsToBeOrderedInTime;
    for (Int_t iSize = 0; iSize < digitsIn.size(); iSize++)
        digitsToBeOrderedInTime.insert(pair <UInt_t, BmnMwpcDigit*> (digitsIn[iSize]->GetTime(), digitsIn[iSize]));

    for (multimap <UInt_t, BmnMwpcDigit*>::iterator it = digitsToBeOrderedInTime.begin(); it != digitsToBeOrderedInTime.end(); it++)
        digitsOrderedInTime.push_back(it->second);
    //
    //    for (Int_t iSize = 0; iSize < digitsIn.size(); iSize++) {
    //        cout << "digitsIn: plane = " << digitsIn[iSize]->GetPlane() << " time = " << digitsIn[iSize]->GetTime();
    //        // cout << " digitsSorted: plane = " << digitsOrderedInTime[iSize]->GetPlane() << " time = " << digitsOrderedInTime[iSize]->GetTime() << endl;
    //    }

//    if (digitsIn.size() != digitsOrderedInTime.size()) {
//       
//        for (Int_t iSize = 0; iSize < digitsIn.size(); iSize++)
//            cout << "1: " << digitsIn[iSize]->GetTime() << " " << digitsIn[iSize]->GetPlane() << " " << digitsIn[iSize]->GetWireNumber() << endl;
//        
//        for (Int_t iSize = 0; iSize < digitsOrderedInTime.size(); iSize++)
//            cout << "2: " << digitsOrderedInTime[iSize]->GetTime() << " " << digitsOrderedInTime[iSize]->GetPlane() << " " << digitsIn[iSize]->GetWireNumber() << endl;
//        
//        getchar();
//    }

    for (Int_t iDigit = 0; iDigit < digitsIn.size(); iDigit++) {
        BmnMwpcDigit* dI = digitsIn[iDigit];
        vector <BmnMwpcDigit*> buffer;
        buffer.push_back(dI);

        FindNeighbour(dI, digitsIn, buffer);

        if (buffer.size() == 1)
            digitsOut.push_back(dI); //just copy digit 
        else {
            Float_t sumWires = 0.0;
            Float_t sumTimes = 0.0;
            for (Int_t iDigiInBuffer = 0; iDigiInBuffer < buffer.size(); iDigiInBuffer++) {
                BmnMwpcDigit* digi = buffer[iDigiInBuffer];
                sumWires += digi->GetWireNumber();
                sumTimes += digi->GetTime();
            }
            BmnMwpcDigit averDigi(dI->GetPlane(), sumWires / buffer.size(), sumTimes / buffer.size(), dI->GetRefId());
            digitsOut.push_back(&averDigi);
        }
    }
    return digitsOut;
}

void BmnMwpcHitFinder::FindNeighbour(BmnMwpcDigit* digiStart, vector <BmnMwpcDigit*> digits, vector <BmnMwpcDigit*> buffer) {
    for (Int_t iDigit = 0; iDigit < digits.size(); iDigit++) {
        BmnMwpcDigit* digi = digits[iDigit];
        if (digi->IsUsed())
            continue;

        if (Abs((Int_t) digiStart->GetWireNumber() - (Int_t) digi->GetWireNumber()) < 2) {
            digi->SetUsing(kTRUE);
            buffer.push_back(digi);
            FindNeighbour(digi, digits, buffer);
        }
    }
}

vector <TVector3> BmnMwpcHitFinder::CreateHitsBy3Planes(vector <BmnMwpcDigit*> x, vector <BmnMwpcDigit*> u, vector <BmnMwpcDigit*> v, Float_t zPos) {
    vector <TVector3> hits;
    if (x.size() == 0 || u.size() == 0 || v.size() == 0)
        return hits;

    vector <TVector3> xu;
    vector <TVector3> xv;
    vector <TVector3> uv;

    FindPairs(x, u, xu);
    FindPairs(x, v, xv);
    FindPairs(u, v, uv);

    Double_t mag = 0.;
    for (Int_t iXU = 0; iXU < xu.size(); iXU++)
        for (Int_t iXV = 0; iXV < xv.size(); iXV++) {
            if (Abs(xu[iXU].Mag() - xv[iXV].Mag()) > thDist)
                continue;
            for (Int_t iUV = 0; iUV < uv.size(); iUV++) {
                if (Abs(xu[iXU].Mag() - uv[iUV].Mag()) > thDist)
                    continue;
                if (Abs(xv[iXV].Mag() - uv[iUV].Mag()) > thDist)
                    continue;
                Float_t xAv = (xu[iXU].X() + xv[iXV].X() + uv[iUV].X()) / 3.0;
                Float_t yAv = (xu[iXU].Y() + xv[iXV].Y() + uv[iUV].Y()) / 3.0;

                // Remove totally duplicated hits if occurred
                TVector3 hitCandidate(xAv, yAv, zPos);
                if (Abs(hitCandidate.Mag() - mag) < LDBL_EPSILON)
                    continue;
                else
                    mag = hitCandidate.Mag();
                hits.push_back(TVector3(xAv, yAv, zPos));
            }
        }
    return hits;
}

void BmnMwpcHitFinder::FindPairs(vector <BmnMwpcDigit*> in1, vector <BmnMwpcDigit*> in2, vector <TVector3>& out) {
    Short_t kTimeBin = fMwpcGeometry->GetTimeBin();
    for (Int_t iIn1 = 0; iIn1 < in1.size(); iIn1++) {
        BmnMwpcDigit* digX = (BmnMwpcDigit*) in1[iIn1];
        for (Int_t iIn2 = 0; iIn2 < in2.size(); iIn2++) {
            BmnMwpcDigit* digU = (BmnMwpcDigit*) in2[iIn2];
            if (fUseDigitsInTimeBin && Abs((Int_t) digX->GetTime() - (Int_t) digU->GetTime()) > kTimeBin)
                continue;
            out.push_back(CalcHitPosByTwoDigits(digX, digU, 0.0));
        }
    }
}

TVector3 BmnMwpcHitFinder::CalcHitPosByTwoDigits(BmnMwpcDigit* dI, BmnMwpcDigit* dJ, Float_t zPos) {
    Short_t kNPlanes = fMwpcGeometry->GetNPlanes();
    Short_t kNWires = fMwpcGeometry->GetNWires();
    Double_t kPlaneWidth = fMwpcGeometry->GetPlaneWidth();
    UInt_t dWireI = dI->GetWireNumber();
    UInt_t dWireJ = dJ->GetWireNumber();
    Short_t localPlaneI = dI->GetPlane() % kNPlanes;
    Short_t localPlaneJ = dJ->GetPlane() % kNPlanes;

    Double_t xI = kPlaneWidth * (dWireI * 1.0 / kNWires - 0.5); //local X by wire number
    Double_t xJ = kPlaneWidth * (dWireJ * 1.0 / kNWires - 0.5); //local X by wire number
    Double_t aI; //rotation angle by plane number
    Double_t aJ; //rotation angle by plane number

    DefineCoordinateAngle(localPlaneI, xI, aI);
    DefineCoordinateAngle(localPlaneJ, xJ, aJ);

    Float_t xGlob = (xI * Sin(aJ) - xJ * Sin(aI)) / Sin(aJ - aI);
    Float_t yGlob = (xI * Cos(aJ) - xJ * Cos(aI)) / Sin(aJ - aI);
    Float_t zGlob = Float_t(min(dI->GetPlane() % kNPlanes + 1, dJ->GetPlane() % kNPlanes + 1) - 3); //average position between two neighbor planes

    TVector3 pos(xGlob, yGlob, zGlob);
    return pos;
}

void BmnMwpcHitFinder::Finish() {
    delete fMwpcGeometry;
    cout << "Work time of the MWPC hit finder: " << workTime << " s" << endl;
}

void BmnMwpcHitFinder::DefineCoordinateAngle(Short_t localPlane, Double_t& x, Double_t& a) {
    Double_t kAngleStep = fMwpcGeometry->GetAngleStep();
    switch (localPlane) {
        case 2:
            x = -x;
            a = 0.0;
            break;
        case 4:
            x = x;
            a = -kAngleStep;
            break;
        case 3:
            x = -x;
            a = +kAngleStep;
            break;
        case 5:
            x = x;
            a = 0.0;
            break;
        case 1:
            x = -x;
            a = -kAngleStep;
            break;
        case 0:
            x = x;
            a = +kAngleStep;
            break;
    }
}

ClassImp(BmnMwpcHitFinder)