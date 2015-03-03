/* 
 * File:   BmnMwpcDigitizer.cxx
 * Author: Sergey Merts
 *
 * Created on October 17, 2014, 5:42 PM
 */

#include "BmnMwpcDigitizer.h"
#include "FairMCPoint.h"
#include "TGeoManager.h"
#include "TROOT.h"
#include "TString.h"
#include "TGeoBBox.h"
#include "TMath.h"

using namespace std;
using namespace TMath;

const Short_t kTimeBin = 8; // ns
const Short_t kNWires = 102; //in one plane
const Float_t kAngleStep = 60; // degrees
const Float_t kWireStep = 0.25; // cm
const Float_t kPlaneHeight = 43.3; // cm
const Float_t kPlaneWidth = kNWires * kWireStep; //24.6; // cm

BmnMwpcDigitizer::BmnMwpcDigitizer(Short_t num = 1) {
    fMwpcNum = num;
    fInputBranchName = Form("MWPC%dPoint", fMwpcNum);
    fOutputBranchName = "BmnMwpcDigit";
}

BmnMwpcDigitizer::~BmnMwpcDigitizer() {

}

InitStatus BmnMwpcDigitizer::Init() {

    //    cout << " BmnMwpcDigitizer::Init() " << endl;

    //Get ROOT Manager
    FairRootManager* ioman = FairRootManager::Instance();

    fBmnMwpcPointArray = (TClonesArray*) ioman->GetObject(fInputBranchName);
    fBmnMwpcDigitArray = new TClonesArray(fOutputBranchName, 100);
    TString name = Form("BmnMwpc%dDigit", fMwpcNum);
    TString folder = Form("MWPC%d", fMwpcNum);
    ioman->Register(name.Data(), folder.Data(), fBmnMwpcDigitArray, kTRUE);

    return kSUCCESS;
}

void BmnMwpcDigitizer::Exec(Option_t* opt) {

    cout << Form("BmnMwpc%dDigitizer::Exec() started!", fMwpcNum) << endl;
    cout << "number of MC points is " << fBmnMwpcPointArray->GetEntriesFast() << endl;
    if (!fBmnMwpcPointArray) {
        Error("BmnMwpcDigitizer::Exec()", " !!! Unknown branch name !!! ");
        return;
    }
    fBmnMwpcDigitArray->Clear();

    if (!fBmnMwpcPointArray) {
        Error("BmnMwpcDigitizer::Exec()", " !!! Unknown branch name !!! ");
        return;
    }
    
    for (Int_t i = 0; i < fBmnMwpcPointArray->GetEntriesFast(); i++) {

        FairMCPoint* point = (FairMCPoint*) fBmnMwpcPointArray->At(i);
        Float_t pTime = point->GetTime();
        Short_t dTime = Short_t(pTime / kTimeBin) + 1;
        Float_t pZ = point->GetZ();
        Float_t pX = point->GetX();
        Float_t pY = point->GetY();
        TGeoNode* curNode = gGeoManager->FindNode(pX, pY, pZ);
        TString nameNode = TString(curNode->GetName());
        Short_t planeNum = TString(nameNode(Int_t(nameNode.Length() - 1), 1)).Atoi();

        TVector3 locPos = Global2Local(TVector3(pX, pY, pZ), planeNum);
        
        Short_t iWire = Short_t(kNWires * (locPos.X() / kPlaneWidth + 0.5));
        if ((iWire > kNWires - 3) || (iWire < 3)) continue; //three wires on each edges are excluded
        new((*fBmnMwpcDigitArray)[fBmnMwpcDigitArray->GetEntriesFast()])BmnMwpcDigit(planeNum, iWire, dTime, point->GetTrackID());

//        if (planeNum > 3) continue;
//        Float_t b1 = kPlaneWidth * (iWire * 1.0 / kNWires - 0.5); //coefficient for plane 1
//        Float_t b0 = 49.8 / 2 / Sqrt(3);
//        Float_t bStep = 2 * b0 / kNWires;
//        Float_t b = b0 - iWire * bStep;
//        Float_t k = Tan(30 * DegToRad());
        //cout << "i = " << iWire << " b = " << b << " xG = " << pX << " yG = " << pY << " xL = " << locPos.X() << " yL = " << locPos.Y() << endl;
//        const Int_t Ny = 1000;
//        const Float_t yStep = kPlaneHeight / Ny;
//        Float_t y = -kPlaneHeight / 2;
//        for (Int_t iY = 0; iY < Ny; ++iY) {
//            y += yStep;
//            Float_t x = (planeNum == 1) ? b1 : (planeNum == 2) ? (y - b) / k : (b - y) / k;
//            h->Fill(x, y);
//        }
//        if (point->GetZ() > 40) continue;
//        TVector3 pos = Global2Local(TVector3(pX, pY, 0.0), 1);
//        h->Fill(pos.X(), pos.Y(), 100);
//        h->Fill(pX, pY, 100);
        
    }
//    h->Draw("colz");
    cout << Form("BmnMwpc%dDigitizer::Exec() finished!", fMwpcNum) << endl;
}

TVector3 BmnMwpcDigitizer::Global2Local(TVector3 gl, Short_t iPlane) {
    Float_t x = gl.X();
    Float_t y = gl.Y();
    Float_t alpha = (iPlane - 1) * kAngleStep * DegToRad(); //setting rotation angle by plane number
    Float_t xLoc = x * Cos(alpha) - y * Sin(alpha);
    Float_t yLoc = x * Sin(alpha) + y * Cos(alpha);
    if (iPlane > 3) xLoc += kWireStep / 2.0;
    return TVector3(xLoc, yLoc, gl.Z());
}

void BmnMwpcDigitizer::Finish() {
}

ClassImp(BmnMwpcDigitizer)