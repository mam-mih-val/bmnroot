
/* 
 * File:   BmnMwpcHitFinder.cxx
 * Author: Sergey Merts
 *
 * Created on October 17, 2014, 5:42 PM
 */

#include "BmnMwpcHitFinder.h"
#include "FairMCPoint.h"
#include "TGeoManager.h"
#include "TROOT.h"
#include "TString.h"
#include "BmnMwpcHit.h"
#include "TMath.h"
#include "TH2F.h"
#include "TCanvas.h"

using namespace std;
using namespace TMath;

const Short_t kTimeBin = 8; // ns
const Short_t kNWires = 102; //in one plane
const Float_t kAngleStep = 60; // degrees
const Float_t kWireStep = 0.25; // cm
const Float_t kPlaneHeight = 43.3; // cm
const Float_t kPlaneWidth = kNWires * kWireStep; //24.6; // cm

class BmnMwpcDigitizer;

Float_t dist(Float_t x1, Float_t y1, Float_t x2, Float_t y2);

BmnMwpcHitFinder::BmnMwpcHitFinder(Short_t num = 1) {
    fMwpcNum = num;
    fInputBranchName = Form("BmnMwpc%dDigit", fMwpcNum);
    fOutputBranchName = Form("BmnMwpc%dHit", fMwpcNum);
}

BmnMwpcHitFinder::~BmnMwpcHitFinder() {

}

InitStatus BmnMwpcHitFinder::Init() {

    //    cout << " BmnMwpcHitFinder::Init() " << endl;

    //Get ROOT Manager
    FairRootManager* ioman = FairRootManager::Instance();

    fBmnMwpcDigitArray = (TClonesArray*) ioman->GetObject(fInputBranchName);
    fBmnMwpcPointArray = (TClonesArray*) ioman->GetObject(Form("MWPC%dPoint", fMwpcNum));
    fBmnMwpcHitArray = new TClonesArray("BmnMwpcHit", 100);
    TString folder = Form("MWPC%d", fMwpcNum);
    ioman->Register(fOutputBranchName.Data(), folder.Data(), fBmnMwpcHitArray, kTRUE);

    TObjArray* nodes = gGeoManager->GetTopNode()->GetNodes();
    for (Int_t iNode = 0; iNode < nodes->GetEntriesFast(); iNode++) {
        TGeoNode* node = (TGeoNode*) nodes->At(iNode);
        TString nodeName = TString(node->GetName());
        if (nodeName.Contains(Form("mwpc%d", fMwpcNum))) {
            const Double_t* motherTr = node->GetMatrix()->GetTranslation();
            fMwpcZpos = motherTr[2];
            break;
        }
    }

    fHisto = new TH2F("h", "h", 600, -30, 30, 600, -30, 30);

    return kSUCCESS;
}

void BmnMwpcHitFinder::Exec(Option_t* opt) {

    cout << Form("BmnMwpc%dHitFinder::Exec() started!", fMwpcNum) << endl;
    cout << "Number of digits = " << fBmnMwpcDigitArray->GetEntriesFast() << endl;

    fBmnMwpcHitArray->Clear();

    if (!fBmnMwpcDigitArray) {
        Error("BmnMwpcHitFinder::Exec()", " !!! Unknown branch name !!! ");
        return;
    }

    //        SearchIn5Pairs();
    SearchIn3Pairs();

    cout << "Number of hits = " << fBmnMwpcHitArray->GetEntriesFast() << endl;



    //    //x-u-v
    //    //    TH2F* hTmp = new TH2F("hTmp", "hTmp", 500, -kPlaneWidth / 2, kPlaneWidth / 2, 500, -kPlaneHeight / 2, kPlaneHeight / 2);
    //    Float_t b0 = 49.8 / 2 / Sqrt(3);
    //    Float_t bStep = 2 * b0 / kNWires;
    //    for (Int_t i = 0; i < x1.size(); ++i) {
    //        Float_t z = fMwpcZpos - 1.25;
    //        BmnMwpcDigit* dI = (BmnMwpcDigit*) x1.at(i);
    //        Short_t dTimeI = dI->GetTime();
    //        Short_t dWireI = dI->GetWireNumber();
    //        Float_t b3 = kPlaneWidth * (dWireI * 1.0 / kNWires - 0.5); //coefficient for plane 1
    //        for (Int_t j = 0; j < u1.size(); ++j) {
    //            BmnMwpcDigit* dJ = (BmnMwpcDigit*) u1.at(j);
    //            Short_t dTimeJ = dJ->GetTime();
    //            if (dTimeI != dTimeJ) continue;
    //            Short_t dWireJ = dJ->GetWireNumber();
    //            Float_t b1 = b0 - dWireJ * bStep;
    //            for (Int_t k = 0; k < v1.size(); ++k) {
    //                BmnMwpcDigit* dK = (BmnMwpcDigit*) v1.at(k);
    //                Short_t dTimeK = dK->GetTime();
    //                if (dTimeJ != dTimeK) continue;
    //                Short_t dWireK = dK->GetWireNumber();
    //                Float_t b2 = b0 - dWireK * bStep;
    //                Float_t S = CalcSquare(b1, b2, b3);
    //                if (S < 0.05) {
    //                    //cout << "S = " << S << " b1 = " << b1 << " b2 = " << b2 << " b3 = " << b3 << endl;
    //                    Float_t x = b3 + Abs((b2 - b1 - 2 * Tan(30 * DegToRad()) * b3) / 2 / Sqrt(3));
    //                    Float_t y = (b2 + b1) / 2;
    //                    //hTmp->Fill(x, y, 500);
    //                    //cout << "x = " << x << " y = " << y << endl;
    //                    Int_t ref = (dI->GetRefId() == dJ->GetRefId() && dI->GetRefId() == dK->GetRefId()) ? dI->GetRefId() : -1;
    //                    //                    cout << "ref = " << ref << " | x1:" << dI->GetRefId() << " | u1:" << dJ->GetRefId() << " | v1:" << dK->GetRefId() << endl;
    //                    new((*fBmnMwpcHitArray)[fBmnMwpcHitArray->GetEntriesFast()]) BmnMwpcHit(0, TVector3(x, y, z), errors, ref);
    //                }
    //            }
    //        }
    //    }
    //
    //    //x'-u'-v'
    //    //    TH2F* hTmp = new TH2F("hTmp", "hTmp", 500, -kPlaneWidth / 2, kPlaneWidth / 2, 500, -kPlaneHeight / 2, kPlaneHeight / 2);
    //    for (Int_t i = 0; i < x2.size(); ++i) {
    //        Float_t z = fMwpcZpos + 1.25;
    //        BmnMwpcDigit* dI = (BmnMwpcDigit*) x2.at(i);
    //        Short_t dTimeI = dI->GetTime();
    //        Short_t dWireI = dI->GetWireNumber();
    //        Float_t b3 = kPlaneWidth * (dWireI * 1.0 / kNWires - 0.5); //coefficient for plane 1
    //        for (Int_t j = 0; j < u2.size(); ++j) {
    //            BmnMwpcDigit* dJ = (BmnMwpcDigit*) u2.at(j);
    //            Short_t dTimeJ = dJ->GetTime();
    //            if (dTimeI != dTimeJ) continue;
    //            Short_t dWireJ = dJ->GetWireNumber();
    //            Float_t b1 = b0 - dWireJ * bStep;
    //            for (Int_t k = 0; k < v2.size(); ++k) {
    //                BmnMwpcDigit* dK = (BmnMwpcDigit*) v2.at(k);
    //                Short_t dTimeK = dK->GetTime();
    //                if (dTimeJ != dTimeK) continue;
    //                Short_t dWireK = dK->GetWireNumber();
    //                Float_t b2 = b0 - dWireK * bStep;
    //                Float_t S = CalcSquare(b1, b2, b3);
    //                if (S < 0.05) {
    //                    //cout << "S = " << S << " b1 = " << b1 << " b2 = " << b2 << " b3 = " << b3 << endl;
    //                    Float_t x = b3 + Abs((b2 - b1 - 2 * Tan(30 * DegToRad()) * b3) / 2 / Sqrt(3));
    //                    Float_t y = (b2 + b1) / 2;
    //                    //hTmp->Fill(x, y, 500);
    //                    //cout << "x = " << x << " y = " << y << endl;
    //                    Int_t ref = (dI->GetRefId() == dJ->GetRefId() && dI->GetRefId() == dK->GetRefId()) ? dI->GetRefId() : -1;
    //                    //                    cout << "ref = " << ref << " | x1:" << dI->GetRefId() << " | u1:" << dJ->GetRefId() << " | v1:" << dK->GetRefId() << endl;
    //                    new((*fBmnMwpcHitArray)[fBmnMwpcHitArray->GetEntriesFast()]) BmnMwpcHit(0, TVector3(x, y, z), errors, ref);
    //                }
    //            }
    //        }
    //    }

    //    hTmp->Draw("colz");  
    FillHisto(fHisto);

    cout << Form("BmnMwpc%dHitFinder: ", fMwpcNum) << fBmnMwpcHitArray->GetEntriesFast() << " hits in output array" << endl;
    cout << Form("BmnMwpc%dHitFinder::Exec() finished!", fMwpcNum) << endl;
}

TVector3 BmnMwpcHitFinder::Global2Local(Float_t xGl, Float_t yGl, Short_t iPlane) {
    Float_t alpha = (iPlane - 1) * kAngleStep * DegToRad(); //setting rotation angle by plane number
    Float_t xLoc = xGl * Cos(alpha) - yGl * Sin(alpha);
    if (iPlane > 3) xLoc += kWireStep / 2.0;
    Float_t yLoc = xGl * Sin(alpha) + yGl * Cos(alpha);
    return TVector3(xLoc, yLoc, 0.0);
}

TVector3 BmnMwpcHitFinder::Local2Global(Float_t xLoc, Float_t yLoc, Short_t iPlane) {
    Float_t alpha = (iPlane - 1) * kAngleStep * DegToRad(); //setting rotation angle by plane number
    if (iPlane > 3) xLoc -= kWireStep / 2.0;
    Float_t xGl = xLoc * Cos(alpha) + yLoc * Sin(alpha);
    Float_t yGl = -xLoc * Sin(alpha) + yLoc * Cos(alpha);
    return TVector3(xGl, yGl, 0.0);
}

void BmnMwpcHitFinder::Finish() {
}

void BmnMwpcHitFinder::FillHisto(TH2F* h) {
    //    TH2F* hParams = new TH2F("params", "params", 100, -2, 2, 100, -15, 15);
    for (Int_t iDig = 0; iDig < fBmnMwpcDigitArray->GetEntriesFast(); ++iDig) {
        BmnMwpcDigit* digit = (BmnMwpcDigit*) fBmnMwpcDigitArray->At(iDig);
        Short_t dPlane = digit->GetPlane();
        //        if (dPlane > 3) continue;
        Short_t dWire = digit->GetWireNumber();
        Float_t b1 = kPlaneWidth * (dWire * 1.0 / kNWires - 0.5); //coefficient for plane 1
        Float_t b0 = 49.8 / 2 / Sqrt(3);
        Float_t bStep = 2 * b0 / kNWires;
        Float_t b = b0 - dWire * bStep;
        Float_t k = Tan(30 * DegToRad());
        const Int_t Ny = 1000;
        const Float_t yStep = kPlaneHeight / Ny;
        Float_t y = -kPlaneHeight / 2;
        for (Int_t iY = 0; iY < Ny; ++iY) {
            y += yStep;
            Float_t x = (dPlane == 1) ? b1 : (dPlane == 2) ? (y - (b0 - dWire * bStep)) / k : (dPlane == 3) ? ((b0 - dWire * bStep) - y) / k : (dPlane == 4) ? -b1 : (dPlane == 5) ? (y - (-b0 + dWire * bStep)) / k : ((-b0 + dWire * bStep) - y) / k;
            h->Fill(x, y);
            //            Float_t p = (dPlane == 1) ? 0.0 : (dPlane == 2) ? k : -k;
            //            Float_t t = (dPlane == 1) ? b1 : b;
            //            hParams->Fill(p, t);
        }

    }

    for (Int_t iPoint = 0; iPoint < fBmnMwpcPointArray->GetEntriesFast(); ++iPoint) {
        FairMCPoint* point = (FairMCPoint*) fBmnMwpcPointArray->At(iPoint);
        //        if (point->GetZ() > 40) continue;
        h->Fill(point->GetX(), point->GetY(), 100);
    }

    //    for (Int_t iHit = 0; iHit < fBmnMwpcHitArray->GetEntriesFast(); ++iHit) {
    //        BmnMwpcHit* hit = (BmnMwpcHit*) fBmnMwpcHitArray->At(iHit);
    //        if (hit->GetZ() > 40) continue;
    //        h->Fill(hit->GetX(), hit->GetY(), 200);
    //    }

//    TCanvas* ccc = new TCanvas("ccc", "ccc", 800, 800);
//    h->Draw("colz");
    //    hParams->Draw("colz");
}

Float_t BmnMwpcHitFinder::CalcSquare(Float_t b1, Float_t b2, Float_t b3) {
    Float_t k = Tan(30 * DegToRad()); // coefficient of line
    Float_t a = Abs(b2 - b1 - 2 * k * b3); // edge of triangle
    return a * a * Sqrt(3) / 4;
}

TLorentzVector* BmnMwpcHitFinder::CalcHitPosByTwoDigits(BmnMwpcDigit* dI, BmnMwpcDigit* dJ) {
    Short_t dWireI = dI->GetWireNumber();
    Short_t dWireJ = dJ->GetWireNumber();
    Float_t xI = kPlaneWidth * (dWireI * 1.0 / kNWires - 0.5); //local X by wire number
    Float_t xJ = kPlaneWidth * (dWireJ * 1.0 / kNWires - 0.5); //local X by wire number
    Float_t aI = (dI->GetPlane() - 1) * kAngleStep * DegToRad(); //rotation angle by plane number
    Float_t aJ = (dJ->GetPlane() - 1) * kAngleStep * DegToRad(); //rotation angle by plane number
    Float_t xGlob = (xI * Sin(aJ) - xJ * Sin(aI)) / Sin(aJ - aI);
    Float_t yGlob = (xI * Cos(aJ) - xJ * Cos(aI)) / Sin(aJ - aI);
    Float_t zGlob = fMwpcZpos - Float_t(min(dI->GetPlane(), dJ->GetPlane()) - 3); //average position between two neighbor planes
    Float_t ref = -1;
    if (dI->GetRefId() == dI->GetRefId()) ref = dI->GetRefId();
    TLorentzVector* pos = new TLorentzVector(xGlob, yGlob, zGlob, ref);
    return pos;
}

vector<TLorentzVector*> BmnMwpcHitFinder::CreateHitsByTwoPlanes(vector<BmnMwpcDigit*> x, vector<BmnMwpcDigit*> y) {
    vector<TLorentzVector*> v;
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

void BmnMwpcHitFinder::SearchIn5Pairs() {

    //temporary containers for storing 
    vector<BmnMwpcDigit*> x1;
    vector<BmnMwpcDigit*> u1;
    vector<BmnMwpcDigit*> v1;
    vector<BmnMwpcDigit*> x2;
    vector<BmnMwpcDigit*> u2;
    vector<BmnMwpcDigit*> v2;

    for (Int_t i = 0; i < fBmnMwpcDigitArray->GetEntriesFast(); ++i) {
        BmnMwpcDigit* digit = (BmnMwpcDigit*) fBmnMwpcDigitArray->At(i);
        Short_t dPlane = digit->GetPlane();
        if (dPlane == 1) x1.push_back(digit);
        else if (dPlane == 2) u1.push_back(digit);
        else if (dPlane == 3) v1.push_back(digit);
        else if (dPlane == 4) x2.push_back(digit);
        else if (dPlane == 5) u2.push_back(digit);
        else if (dPlane == 6) v2.push_back(digit);
    }

    const Float_t errX = kWireStep / Sqrt(12.0);
    const Float_t errY = kWireStep / Sqrt(12.0);
    const Float_t errZ = 1.0 / Sqrt(12.0); // zStep = 1.0 cm
    TVector3 errors = TVector3(errX, errY, errZ); //FIXME!!! Calculate by formulae

    vector<TLorentzVector*> x1u1 = CreateHitsByTwoPlanes(x1, u1);
    vector<TLorentzVector*> u1v1 = CreateHitsByTwoPlanes(u1, v1);
    vector<TLorentzVector*> v1x2 = CreateHitsByTwoPlanes(v1, x2);
    vector<TLorentzVector*> x2u2 = CreateHitsByTwoPlanes(x2, u2);
    vector<TLorentzVector*> u2v2 = CreateHitsByTwoPlanes(u2, v2);

    //    vector<TVector3*> p12 = CreateHitsByTwoPlanes(x1, u1);
    //    vector<TVector3*> p13 = CreateHitsByTwoPlanes(x1, v1);
    //    vector<TVector3*> p15 = CreateHitsByTwoPlanes(x1, u2);
    //    vector<TVector3*> p16 = CreateHitsByTwoPlanes(x1, v2);
    //    vector<TVector3*> p23 = CreateHitsByTwoPlanes(u1, v1);
    //    vector<TVector3*> p24 = CreateHitsByTwoPlanes(u1, x2);
    //    vector<TVector3*> p26 = CreateHitsByTwoPlanes(u1, v2);
    //    vector<TVector3*> p34 = CreateHitsByTwoPlanes(v1, x2);
    //    vector<TVector3*> p35 = CreateHitsByTwoPlanes(v1, u2);
    //    vector<TVector3*> p45 = CreateHitsByTwoPlanes(x2, u2);
    //    vector<TVector3*> p46 = CreateHitsByTwoPlanes(x2, v2);
    //    vector<TVector3*> p56 = CreateHitsByTwoPlanes(u2, v2);
    //
    //    Int_t Nbins = 240;
    //    Float_t xMin = -30.0;
    //    Float_t xMax = 30.0;
    //    TH2F* signals = new TH2F("signals", "signals", Nbins, xMin, xMax, Nbins, xMin, xMax);
    //    AccumulateSignals(p12, signals);
    //    AccumulateSignals(p13, signals);
    //    AccumulateSignals(p15, signals);
    //    AccumulateSignals(p16, signals);
    //    AccumulateSignals(p23, signals);
    //    AccumulateSignals(p24, signals);
    //    AccumulateSignals(p26, signals);
    //    AccumulateSignals(p34, signals);
    //    AccumulateSignals(p35, signals);
    //    AccumulateSignals(p45, signals);
    //    AccumulateSignals(p46, signals);
    //    AccumulateSignals(p56, signals);
    //
    //
    //    Float_t max = signals->GetMaximum();

    //    for (Int_t i = 0; i < signals->GetNbinsX(); ++i)
    //        for (Int_t j = 0; j < signals->GetNbinsY(); ++j)
    //            if (signals->GetBinContent(i, j) < 0.1 * max) signals->SetBinContent(i, j, 0);

    //    for (Int_t i = 0; i < signals->GetNbinsX(); ++i) {
    //        for (Int_t j = 0; j < signals->GetNbinsY(); ++j) {
    //            //            if (signals->GetBinContent(i, j) < 12) signals->SetBinContent(i, j, 0);
    //            Float_t si0 = 0.0;
    //            Float_t si1 = 0.0;
    //            Float_t si2 = 0.0;
    //            Float_t sj0 = 0.0;
    //            Float_t sj1 = 0.0;
    //            Float_t sj2 = 0.0;
    //            if (signals->GetBinContent(i, j) > max * 0.9) {
    //                for (Int_t i0 = -1; i0 < 1; ++i0) {
    //                    Float_t sigi = 0.0; // signal on i row
    //                    for (Int_t j0 = -1; j0 < 1; ++j0) {
    //                        sigi += signals->GetBinContent(i + i0, j + j0);
    //                    }
    //                    si0 += sigi;
    //                    si1 += sigi * i;
    //                    si2 += sigi * i * i;
    //                }
    //                for (Int_t j0 = -1; j0 < 1; ++j0) {
    //                    Float_t sigj = 0.0; // signal on j column
    //                    for (Int_t i0 = -1; i0 < 1; ++i0) {
    //                        sigj += signals->GetBinContent(i + i0, j + j0);
    //                        signals->SetBinContent(i + i0, j + j0, 0.0);
    //                    }
    //                    sj0 += sigj;
    //                    sj1 += sigj * j;
    //                    sj2 += sigj * j * j;
    //                }
    //                Float_t sigmaI = Sqrt(si0 * si2 - si1 * si1) / si0;
    //                Float_t sigmaJ = Sqrt(sj0 * sj2 - sj1 * sj1) / sj0;
    //                Float_t averX = sj1 / sj0;
    //                Float_t averY = si1 / si0;
    //                Float_t globX = (xMax - xMin) / Nbins * averX + xMin;
    //                Float_t globY = (xMax - xMin) / Nbins * averY + xMin;
    //                new((*fBmnMwpcHitArray)[fBmnMwpcHitArray->GetEntriesFast()]) BmnMwpcHit(0, TVector3(globX, globY, 40), errors, 0);
    //            }
    //        }
    //    }
    //    max = signals->GetMaximum();
    //    for (Int_t i = 0; i < signals->GetNbinsX(); ++i) {
    //        for (Int_t j = 0; j < signals->GetNbinsY(); ++j) {
    //            //            if (signals->GetBinContent(i, j) < 12) signals->SetBinContent(i, j, 0);
    //            Float_t si0 = 0.0;
    //            Float_t si1 = 0.0;
    //            Float_t si2 = 0.0;
    //            Float_t sj0 = 0.0;
    //            Float_t sj1 = 0.0;
    //            Float_t sj2 = 0.0;
    //            if (signals->GetBinContent(i, j) > max * 0.8) {
    //                for (Int_t i0 = -1; i0 < 1; ++i0) {
    //                    Float_t sigi = 0.0; // signal on i row
    //                    for (Int_t j0 = -1; j0 < 1; ++j0) {
    //                        sigi += signals->GetBinContent(i + i0, j + j0);
    //                    }
    //                    si0 += sigi;
    //                    si1 += sigi * i;
    //                    si2 += sigi * i * i;
    //                }
    //                for (Int_t j0 = -1; j0 < 1; ++j0) {
    //                    Float_t sigj = 0.0; // signal on j column
    //                    for (Int_t i0 = -1; i0 < 1; ++i0) {
    //                        sigj += signals->GetBinContent(i + i0, j + j0);
    //                        signals->SetBinContent(i + i0, j + j0, 0.0);
    //                    }
    //                    sj0 += sigj;
    //                    sj1 += sigj * j;
    //                    sj2 += sigj * j * j;
    //                }
    //                Float_t sigmaI = Sqrt(si0 * si2 - si1 * si1) / si0;
    //                Float_t sigmaJ = Sqrt(sj0 * sj2 - sj1 * sj1) / sj0;
    //                Float_t averX = sj1 / sj0;
    //                Float_t averY = si1 / si0;
    //                Float_t globX = (xMax - xMin) / Nbins * averX + xMin;
    //                Float_t globY = (xMax - xMin) / Nbins * averY + xMin;
    //                new((*fBmnMwpcHitArray)[fBmnMwpcHitArray->GetEntriesFast()]) BmnMwpcHit(0, TVector3(globX, globY, 40), errors, 0);
    //            }
    //        }
    //    }
    //    max = signals->GetMaximum();
    //    for (Int_t i = 0; i < signals->GetNbinsX(); ++i) {
    //        for (Int_t j = 0; j < signals->GetNbinsY(); ++j) {
    //            //            if (signals->GetBinContent(i, j) < 12) signals->SetBinContent(i, j, 0);
    //            Float_t si0 = 0.0;
    //            Float_t si1 = 0.0;
    //            Float_t si2 = 0.0;
    //            Float_t sj0 = 0.0;
    //            Float_t sj1 = 0.0;
    //            Float_t sj2 = 0.0;
    //            if (signals->GetBinContent(i, j) > max * 0.7) {
    //                for (Int_t i0 = -1; i0 < 1; ++i0) {
    //                    Float_t sigi = 0.0; // signal on i row
    //                    for (Int_t j0 = -1; j0 < 1; ++j0) {
    //                        sigi += signals->GetBinContent(i + i0, j + j0);
    //                    }
    //                    si0 += sigi;
    //                    si1 += sigi * i;
    //                    si2 += sigi * i * i;
    //                }
    //                for (Int_t j0 = -1; j0 < 1; ++j0) {
    //                    Float_t sigj = 0.0; // signal on j column
    //                    for (Int_t i0 = -1; i0 < 1; ++i0) {
    //                        sigj += signals->GetBinContent(i + i0, j + j0);
    //                        signals->SetBinContent(i + i0, j + j0, 0.0);
    //                    }
    //                    sj0 += sigj;
    //                    sj1 += sigj * j;
    //                    sj2 += sigj * j * j;
    //                }
    //                Float_t sigmaI = Sqrt(si0 * si2 - si1 * si1) / si0;
    //                Float_t sigmaJ = Sqrt(sj0 * sj2 - sj1 * sj1) / sj0;
    //                Float_t averX = sj1 / sj0;
    //                Float_t averY = si1 / si0;
    //                Float_t globX = (xMax - xMin) / Nbins * averX + xMin;
    //                Float_t globY = (xMax - xMin) / Nbins * averY + xMin;
    //                new((*fBmnMwpcHitArray)[fBmnMwpcHitArray->GetEntriesFast()]) BmnMwpcHit(0, TVector3(globX, globY, 40), errors, 0);
    //            }
    //        }
    //    }


    //    TCanvas* cc = new TCanvas("cc", "cc", 800, 800);
    //    signals->Draw("colz");
    //
    cout << "SIZES: " << x1u1.size() << " " << u1v1.size() << " " << v1x2.size() << " " << x2u2.size() << " " << u2v2.size() << endl;

    Float_t delta = (fMwpcNum == 1) ? 1.0 : (fMwpcNum == 2) ? 1.5 : 2.00; //cm
    Float_t x = 0.0;
    Float_t y = 0.0;
    Float_t z = 0.0;
    TLorentzVector* pos1 = NULL;
    TLorentzVector* pos2 = NULL;
    TLorentzVector* pos3 = NULL;
    TLorentzVector* pos4 = NULL;
    TLorentzVector* pos5 = NULL;
    for (Int_t i1 = 0; i1 < x1u1.size(); ++i1) {
        pos1 = x1u1.at(i1);
        if (pos1->Z() < 0) continue;
        for (Int_t i2 = 0; i2 < u1v1.size(); ++i2) {
            pos2 = u1v1.at(i2);
            if (pos2->Z() < 0 || pos1->Z() < 0) continue;
            if (dist(pos1->X(), pos1->Y(), pos2->X(), pos2->Y()) > delta) continue;
            for (Int_t i3 = 0; i3 < v1x2.size(); ++i3) {
                pos3 = v1x2.at(i3);
                if (pos3->Z() < 0 || pos2->Z() < 0 || pos1->Z() < 0) continue;
                if (dist(pos2->X(), pos2->Y(), pos3->X(), pos3->Y()) > delta) continue;
                for (Int_t i4 = 0; i4 < x2u2.size(); ++i4) {
                    pos4 = x2u2.at(i4);
                    if (pos4->Z() < 0 || pos3->Z() < 0 || pos2->Z() < 0 || pos1->Z() < 0) continue;
                    if (dist(pos3->X(), pos3->Y(), pos4->X(), pos4->Y()) > delta) continue;
                    for (Int_t i5 = 0; i5 < u2v2.size(); ++i5) {
                        pos5 = u2v2.at(i5);
                        if (pos5->Z() < 0 || pos4->Z() < 0 || pos3->Z() < 0 || pos2->Z() < 0 || pos1->Z() < 0) continue;
                        if (dist(pos4->X(), pos4->Y(), pos5->X(), pos5->Y()) > delta) continue;
                        //Int_t ref = (dI->GetRefId() == dJ->GetRefId() && dI->GetRefId() == dK->GetRefId()) ? dI->GetRefId() : -1;
                        //cout << "ref = " << ref << " | x1:" << dI->GetRefId() << " | u1:" << dJ->GetRefId() << " | v1:" << dK->GetRefId() << endl;
                        x = (pos1->X() + pos2->X() + pos3->X() + pos4->X() + pos5->X()) / 5;
                        y = (pos1->Y() + pos2->Y() + pos3->Y() + pos4->Y() + pos5->Y()) / 5;
                        z = (pos1->Z() + pos2->Z() + pos3->Z() + pos4->Z() + pos5->Z()) / 5;
                        pos1->SetZ(-1.0);
                        pos2->SetZ(-1.0);
                        pos3->SetZ(-1.0);
                        pos4->SetZ(-1.0);
                        pos5->SetZ(-1.0);
                        new((*fBmnMwpcHitArray)[fBmnMwpcHitArray->GetEntriesFast()]) BmnMwpcHit(0, TVector3(x, y, z), errors, 0);
                    }
                }
            }
        }
    }
}

void BmnMwpcHitFinder::AccumulateSignals(vector<TVector3*> vec, TH2F* h) {
    for (Int_t i = 0; i < vec.size(); ++i) {
        h->Fill(vec.at(i)->X(), vec.at(i)->Y());
    }
}

void BmnMwpcHitFinder::SearchIn3Pairs() {

    //temporary containers for storing 
    vector<BmnMwpcDigit*> x1;
    vector<BmnMwpcDigit*> u1;
    vector<BmnMwpcDigit*> v1;
    vector<BmnMwpcDigit*> x2;
    vector<BmnMwpcDigit*> u2;
    vector<BmnMwpcDigit*> v2;

    for (Int_t i = 0; i < fBmnMwpcDigitArray->GetEntriesFast(); ++i) {
        BmnMwpcDigit* digit = (BmnMwpcDigit*) fBmnMwpcDigitArray->At(i);
        Short_t dPlane = digit->GetPlane();
        if (dPlane == 1) x1.push_back(digit);
        else if (dPlane == 2) u1.push_back(digit);
        else if (dPlane == 3) v1.push_back(digit);
        else if (dPlane == 4) x2.push_back(digit);
        else if (dPlane == 5) u2.push_back(digit);
        else if (dPlane == 6) v2.push_back(digit);
    }

    const Float_t errX = kWireStep / Sqrt(12.0);
    const Float_t errY = kWireStep / Sqrt(12.0);
    const Float_t errZ = 1.0 / Sqrt(12.0); // zStep = 1.0 cm
    TVector3 errors = TVector3(errX, errY, errZ); //FIXME!!! Calculate by formulae

    Float_t x = 0.0;
    Float_t y = 0.0;
    Float_t z = 0.0;

    Float_t delta = (fMwpcNum == 1) ? 1.0 : (fMwpcNum == 2) ? 1.5 : 2.00; //cm

    vector<TLorentzVector*> x1u1 = CreateHitsByTwoPlanes(x1, u1);
    vector<TLorentzVector*> u1v1 = CreateHitsByTwoPlanes(u1, v1);
    vector<TLorentzVector*> v1x2 = CreateHitsByTwoPlanes(v1, x2);
    vector<TLorentzVector*> x2u2 = CreateHitsByTwoPlanes(x2, u2);
    vector<TLorentzVector*> u2v2 = CreateHitsByTwoPlanes(u2, v2);
    vector<TLorentzVector*> v2x1 = CreateHitsByTwoPlanes(v2, x1);

    //    vector<TVector3*> p12 = CreateHitsByTwoPlanes(x1, u1);
    //    vector<TVector3*> p13 = CreateHitsByTwoPlanes(x1, v1);
    //    vector<TVector3*> p15 = CreateHitsByTwoPlanes(x1, u2);
    //    vector<TVector3*> p16 = CreateHitsByTwoPlanes(x1, v2);
    //    vector<TVector3*> p23 = CreateHitsByTwoPlanes(u1, v1);
    //    vector<TVector3*> p24 = CreateHitsByTwoPlanes(u1, x2);
    //    vector<TVector3*> p26 = CreateHitsByTwoPlanes(u1, v2);
    //    vector<TVector3*> p34 = CreateHitsByTwoPlanes(v1, x2);
    //    vector<TVector3*> p35 = CreateHitsByTwoPlanes(v1, u2);
    //    vector<TVector3*> p45 = CreateHitsByTwoPlanes(x2, u2);
    //    vector<TVector3*> p46 = CreateHitsByTwoPlanes(x2, v2);
    //    vector<TVector3*> p56 = CreateHitsByTwoPlanes(u2, v2);

    //    vector<TVector3*> VectorOfPositions;

    //    for (Int_t i = 0; i < p12.size(); ++i) {
    //        VectorOfPositions.push_back(new TVector3(p12.at(i)->x(), p12.at(i)->y(), p12.at(i)->z()));
    //    }
    //    for (Int_t i = 0; i < p13.size(); ++i) {
    //        VectorOfPositions.push_back(new TVector3(p13.at(i)->x(), p13.at(i)->y(), p13.at(i)->z()));
    //    }
    //    for (Int_t i = 0; i < p15.size(); ++i) {
    //        VectorOfPositions.push_back(new TVector3(p15.at(i)->x(), p15.at(i)->y(), p15.at(i)->z()));
    //    }
    //    for (Int_t i = 0; i < p16.size(); ++i) {
    //        VectorOfPositions.push_back(new TVector3(p16.at(i)->x(), p16.at(i)->y(), p16.at(i)->z()));
    //    }
    //    for (Int_t i = 0; i < p23.size(); ++i) {
    //        VectorOfPositions.push_back(new TVector3(p23.at(i)->x(), p23.at(i)->y(), p23.at(i)->z()));
    //    }
    //    for (Int_t i = 0; i < p24.size(); ++i) {
    //        VectorOfPositions.push_back(new TVector3(p24.at(i)->x(), p24.at(i)->y(), p24.at(i)->z()));
    //    }
    //    for (Int_t i = 0; i < p26.size(); ++i) {
    //        VectorOfPositions.push_back(new TVector3(p26.at(i)->x(), p26.at(i)->y(), p26.at(i)->z()));
    //    }
    //    for (Int_t i = 0; i < p34.size(); ++i) {
    //        VectorOfPositions.push_back(new TVector3(p34.at(i)->x(), p34.at(i)->y(), p34.at(i)->z()));
    //    }
    //    for (Int_t i = 0; i < p35.size(); ++i) {
    //        VectorOfPositions.push_back(new TVector3(p35.at(i)->x(), p35.at(i)->y(), p35.at(i)->z()));
    //    }
    //    for (Int_t i = 0; i < p45.size(); ++i) {
    //        VectorOfPositions.push_back(new TVector3(p45.at(i)->x(), p45.at(i)->y(), p45.at(i)->z()));
    //    }
    //    for (Int_t i = 0; i < p46.size(); ++i) {
    //        VectorOfPositions.push_back(new TVector3(p46.at(i)->x(), p46.at(i)->y(), p46.at(i)->z()));
    //    }
    //    for (Int_t i = 0; i < p56.size(); ++i) {
    //        VectorOfPositions.push_back(new TVector3(p56.at(i)->x(), p56.at(i)->y(), p56.at(i)->z()));
    //    }

    //    vector<TVector3> nearest;
    //    Float_t deltaR = 0.7; //cm
    //    for (Int_t i = 0; i < p12.size(); ++i) {
    //        nearest.clear();
    //        TVector3* pos1 = p12.at(i);
    //        nearest.push_back(TVector3(pos1->x(), pos1->y(), pos1->z()));
    //        for (Int_t j = 0; j < VectorOfPositions.size(); ++j) {
    //            TVector3* pos2 = VectorOfPositions.at(j);
    //            if (pos2->z() < 0.0) continue;
    //            if (Sqrt((pos1->x() - pos2->x()) * (pos1->x() - pos2->x()) + (pos1->y() - pos2->y()) * (pos1->y() - pos2->y())) > deltaR) continue;
    //            nearest.push_back(TVector3(pos2->x(), pos2->y(), pos2->z()));
    //            pos2->SetZ(-1.0);
    //        }
    //        Float_t resX = 0.0;
    //        Float_t resY = 0.0;
    //        Float_t resZ = 0.0;
    //        if (nearest.size() < 4) continue;
    //        cout << "SIZE of nearest array = " << nearest.size() << endl;
    //        for (Int_t j = 0; j < nearest.size(); ++j) {
    //            TVector3 res = nearest.at(j);
    //            resX += res.X();
    //            resY += res.Y();
    //            resZ += res.Z();
    //        }
    //        resX /= nearest.size();
    //        resY /= nearest.size();
    //        resZ /= nearest.size();
    //        new((*fBmnMwpcHitArray)[fBmnMwpcHitArray->GetEntriesFast()]) BmnMwpcHit(0, TVector3(resX, resY, resZ), errors, 0);
    //    }

    //======================================================================================================================================================================================

    TLorentzVector* pos1 = NULL;
    TLorentzVector* pos2 = NULL;
    TLorentzVector* pos3 = NULL;

    for (Int_t i1 = 0; i1 < x1u1.size(); ++i1) {
        pos1 = x1u1.at(i1);
        if (pos1->Z() < 0) continue;
        for (Int_t i2 = 0; i2 < v1x2.size(); ++i2) {
            pos2 = v1x2.at(i2);
            if (pos2->Z() < 0 || pos1->Z() < 0) continue;
            if (dist(pos1->X(), pos1->Y(), pos2->X(), pos2->Y()) > delta) continue;
            for (Int_t i3 = 0; i3 < u2v2.size(); ++i3) {
                pos3 = u2v2.at(i3);
                if (pos3->Z() < 0 || pos2->Z() < 0 || pos1->Z() < 0) continue;
                if (dist(pos2->X(), pos2->Y(), pos3->X(), pos3->Y()) > delta) continue;
                x = (pos1->X() + pos2->X() + pos3->X()) / 3;
                y = (pos1->Y() + pos2->Y() + pos3->Y()) / 3;
                z = (pos1->Z() + pos2->Z() + pos3->Z()) / 3;
                Float_t ref = -1.0;
                if (pos1->T() == pos2->T() && pos1->T() == pos3->T()) ref = pos1->T();
                pos1->SetZ(-1.0);
                pos2->SetZ(-1.0);
                pos3->SetZ(-1.0);
                new((*fBmnMwpcHitArray)[fBmnMwpcHitArray->GetEntriesFast()]) BmnMwpcHit(0, TVector3(x, y, z), errors, ref);
            }
        }
    }

    Double_t k = Tan(kAngleStep / 2 * DegToRad());
    Double_t b = kPlaneWidth * k;
    delta = (fMwpcNum == 1) ? 1.0 : (fMwpcNum == 2) ? 1.5 : 2.0; //cm


    //Checking angle s between x1 and u1, x2 and u2
    //    for (Int_t i = 0; i < x1u1.size(); ++i) {
    //        TLorentzVector* pos1 = x1u1.at(i);
    //        if (pos1->Z() < 0) continue;
    //        if ((pos1->Y() < (-k * pos1->X() + b)) && pos1->Y() > (-k * pos1->X() - b)) continue;
    //        for (Int_t j = 0; j < x2u2.size(); ++j) {
    //            TLorentzVector* pos2 = x2u2.at(j);
    //            if (pos2->Z() < 0 || pos1->Z() < 0) continue;
    //            if ((pos2->Y() < (-k * pos2->X() + b)) && pos2->Y() > (-k * pos2->X() - b)) continue;
    //            if (dist(pos1->X(), pos1->Y(), pos2->X(), pos2->Y()) > delta) continue;
    //            x = (pos1->X() + pos2->X()) * 0.5;
    //            y = (pos1->Y() + pos2->Y()) * 0.5;
    //            z = (pos1->Z() + pos2->Z()) * 0.5;
    //            Float_t ref = -1.0;
    //            if (pos1->T() == pos2->T()) ref = pos1->T();
    //            pos1->SetZ(-1.0);
    //            pos2->SetZ(-1.0);
    //
    //            new((*fBmnMwpcHitArray)[fBmnMwpcHitArray->GetEntriesFast()]) BmnMwpcHit(0, TVector3(x, y, z), errors, ref);
    //        }
    //    }
    //
    //    //Checking angle s between u1 and v1, u2 and v2
    //    for (Int_t i = 0; i < u1v1.size(); ++i) {
    //        TLorentzVector* pos1 = u1v1.at(i);
    //        if (pos1->Z() < 0) continue;
    //        if (pos1->X() > (-kPlaneWidth * 0.5) && pos1->X() < (kPlaneWidth * 0.5)) continue;
    //        for (Int_t j = 0; j < u2v2.size(); ++j) {
    //            TLorentzVector* pos2 = u2v2.at(j);
    //            if (pos2->Z() < 0 || pos1->Z() < 0) continue;
    //            if (pos2->X() > (-kPlaneWidth * 0.5) && pos2->X() < (kPlaneWidth * 0.5)) continue;
    //            if (dist(pos1->X(), pos1->Y(), pos2->X(), pos2->Y()) > delta) continue;
    //            x = (pos1->X() + pos2->X()) * 0.5;
    //            y = (pos1->Y() + pos2->Y()) * 0.5;
    //            z = (pos1->Z() + pos2->Z()) * 0.5;
    //            Float_t ref = -1.0;
    //            if (pos1->T() == pos2->T()) ref = pos1->T();
    //            pos1->SetZ(-1.0);
    //            pos2->SetZ(-1.0);
    //
    //            new((*fBmnMwpcHitArray)[fBmnMwpcHitArray->GetEntriesFast()]) BmnMwpcHit(0, TVector3(x, y, z), errors, ref);
    //        }
    //    }
    //
    //    //Checking angle s between v1 and x2, v2 and x1
    //    for (Int_t i = 0; i < v1x2.size(); ++i) {
    //        TLorentzVector* pos1 = v1x2.at(i);
    //        if (pos1->Z() < 0) continue;
    //        if ((pos1->Y() < (k * pos1->X() + b)) && pos1->Y() > (k * pos1->X() - b)) continue;
    //        for (Int_t j = 0; j < v2x1.size(); ++j) {
    //            TLorentzVector* pos2 = v2x1.at(j);
    //            if (pos2->Z() < 0 || pos1->Z() < 0) continue;
    //            if ((pos2->Y() < (k * pos2->X() + b)) && pos2->Y() > (k * pos2->X() - b)) continue;
    //            if (dist(pos1->X(), pos1->Y(), pos2->X(), pos2->Y()) > delta) continue;
    //            x = (pos1->X() + pos2->X()) * 0.5;
    //            y = (pos1->Y() + pos2->Y()) * 0.5;
    //            z = (pos1->Z() + pos2->Z()) * 0.5;
    //            Float_t ref = -1.0;
    //            if (pos1->T() == pos2->T()) ref = pos1->T();
    //            pos1->SetZ(-1.0);
    //            pos2->SetZ(-1.0);
    //
    //            new((*fBmnMwpcHitArray)[fBmnMwpcHitArray->GetEntriesFast()]) BmnMwpcHit(0, TVector3(x, y, z), errors, ref);
    //        }
    //    }
}

Float_t dist(Float_t x1, Float_t y1, Float_t x2, Float_t y2) {
    return Sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

ClassImp(BmnMwpcHitFinder)