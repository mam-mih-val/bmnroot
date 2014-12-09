/**
 * \file BmnTrackFitterImp.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de> - Original author. First version of code for CBM experiment.
 * \author Sergey Merts <Sergey.Merts@gmail.com> - Modification for BMN experiment.
 * \date 2007-2014
 **/
#include "BmnTrackFitter.h"

#include "FairTrackParam.h"
#include "CbmGlobalTrack.h"
#include "CbmStsHit.h"
#include "CbmStripHit.h"
#include "BmnFitNode.h"
#include "BmnMath.h"

#include <iostream>

using namespace std;

BmnTrackFitter::BmnTrackFitter(BmnTrackPropagator* prp, BmnKalmanFilter* upd, Int_t nItr, Float_t chi, Int_t nHits) :
fNofIterations(nItr),
fChiSqCut(chi),
fMinNofHits(nHits) {
    fPropagator = prp;
    fUpdate = upd;
}

BmnTrackFitter::BmnTrackFitter() :
fNofIterations(2),
fChiSqCut(15),
fMinNofHits(3) {
    fPropagator = NULL;
    fUpdate = NULL;
}

BmnTrackFitter::~BmnTrackFitter() {
}

//++++++++++++++++++++++++++++ITER++++++++++++++++++++++++++++++++++++//

BmnStatus BmnTrackFitter::FitImp(CbmGlobalTrack* glTr, TClonesArray* stsTracks, TClonesArray* stsHits, TClonesArray* tofHits, Bool_t downstream) { //FIXME now works only for stream from sts to tof!!!!!!!!!!!!!!!!!!!
    //    cout << "glTrIdx = " << glTr->GetUniqueID() << " StsTrIdx = " << glTr->GetStsTrackIndex() << " TofHitIdx = " << glTr->GetTofHitIndex() << endl;
    CbmStsTrack* stsTr = (CbmStsTrack*) stsTracks->At(glTr->GetStsTrackIndex());
    CbmTofHit* tofHit = NULL;
    if (glTr->GetFlag() == kBMNGOODMERGE) {tofHit = (CbmTofHit*) tofHits->At(glTr->GetTofHitIndex());}
    stsTr->SortHits();
    glTr->SetChi2(0.0);
    Int_t nofHits = stsTr->GetNStsHits();
    Int_t nNodes = (tofHit != NULL) ? nofHits + 1 : nofHits;
    vector<BmnFitNode> nodes(nNodes);
    FairTrackParam par;
    vector<Double_t> F(25);
    
    if (downstream) {
        glTr->SetParamLast(glTr->GetParamFirst());
        par = *glTr->GetParamLast();
    } else {
        glTr->SetParamFirst(glTr->GetParamLast());
        par = *glTr->GetParamFirst();
    }
    
    Float_t totalLength = 0.;
    for (Int_t iHit = 0; iHit < nofHits; iHit++) {
        Int_t stsHitIdx = stsTr->GetStsHitIndex(iHit);
        CbmStsHit* hit = (CbmStsHit*) stsHits->At(stsHitIdx);
        Float_t Ze = hit->GetZ();

        Float_t length = 0;
//        cout << par.GetX() << " " << par.GetY() << " " << par.GetTx() << " " << par.GetTy() << " " << par.GetQp() << endl;
        if (fPropagator->TGeoTrackPropagate(&par, Ze, 211/*glTr->GetPDG()*/, &F, &length, TString("field")) == kBMNERROR) {
            glTr->SetFlag(kBMNBAD);
            cout << "PROP ERROR: Ze = " << Ze << " length = " << length << " \npar = ";
            par.Print();
            return kBMNERROR;
        }
        
        totalLength += length;
        nodes[iHit].SetPredictedParam(&par);
        nodes[iHit].SetF(F);
        Float_t chi2Hit = 0.;
        if (fUpdate->Update(&par, hit, chi2Hit) == kBMNERROR) {
            glTr->SetFlag(kBMNBAD);
            cout << "UPD ERROR: Ze = " << Ze << " length = " << length << " \npar = ";
            par.Print();
            return kBMNERROR;
        }
//        cout << "Idx = " << iHit << " First q/p = " << glTr->GetParamFirst()->GetQp();
//        cout << " Last q/p = " << glTr->GetParamLast()->GetQp();
//        cout << " Par q/p = " << par.GetQp() << endl;
        
        nodes[iHit].SetUpdatedParam(&par);
        nodes[iHit].SetChiSqFiltered(chi2Hit);
        glTr->SetChi2(glTr->GetChi2() + chi2Hit);
        if (iHit == 0) {
            if (downstream) {
                glTr->SetParamFirst(&par);
            } else {
                glTr->SetParamLast(&par);
            }
        }
    }
   
    //&&&&&&&&& KOSTYL FOR TOF HIT &&&&&&&//
    if (tofHit != NULL) {
//        cout << "OK!\n";
        Float_t Ze = tofHit->GetZ();
        Float_t length = 0;
        if (fPropagator->TGeoTrackPropagate(&par, Ze, 211/*glTr->GetPDG()*/, &F, &length, TString("field")) == kBMNERROR) {
            glTr->SetFlag(kBMNBAD);
            //             cout << "PROP ERROR: Ze=" << Ze << " length=" << length << " par=" << par.Print();
            return kBMNERROR;
        }
        totalLength += length;
        nodes[nNodes - 1].SetPredictedParam(&par);
        nodes[nNodes - 1].SetF(F);
        Float_t chi2Hit = 0.;
//        cout << "FitImp Update\n";
        if (fUpdate->Update(&par, tofHit, chi2Hit) == kBMNERROR) {
            glTr->SetFlag(kBMNBAD);
            //             cout << "UPD ERROR: Ze=" << Ze << " length=" << length << " par=" << par.Print();
            return kBMNERROR;
        }
        nodes[nNodes - 1].SetUpdatedParam(&par);
        nodes[nNodes - 1].SetChiSqFiltered(chi2Hit);
        glTr->SetChi2(glTr->GetChi2() + chi2Hit);
    }
    //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&//

    if (downstream) {
        glTr->SetParamLast(&par);
    } else {
        glTr->SetParamFirst(&par);
    }

    glTr->SetFitNodes(nodes);
    glTr->SetNDF(lit::NDF(glTr));
    glTr->SetLength(totalLength);

    return kBMNSUCCESS;
}

//+++++++++++++++++++++++++++++++++ITER+++++++++++++++++++++++++++++++++++++++//

BmnStatus BmnTrackFitter::FitIter(CbmGlobalTrack* glTr, Bool_t downstream) { //FIXME infinite recurse
    cout << "FitIter is started\n";
    for (Int_t iter = 0; iter < fNofIterations; iter++) {
        Bool_t isRefit = kFALSE;

        if (FitIter(glTr) == kBMNERROR) {
            return kBMNERROR;
        }
        if (FitSmooth(glTr) == kBMNERROR) {
            return kBMNERROR;
        }
        if (iter < fNofIterations - 1) {
            for (Int_t i = 0; i < glTr->GetNofHits(); i++) {
                Float_t chiSq = glTr->GetFitNode(i)->GetChiSqSmoothed();
                if (chiSq > fChiSqCut) {
                    //                    glTr->RemoveHit(i); //FIXME remove hit from track!         //FIXME
                    isRefit = kTRUE;
                }
            }
        }

        if (glTr->GetNofHits() < fMinNofHits) {
            return kBMNERROR;
        }
        if (!isRefit) {
            return kBMNSUCCESS;
        }
    }
    cout << "FitIter is finished\n";
    return kBMNSUCCESS;
}

//++++++++++++++++++++++++++++SMOOTHER++++++++++++++++++++++++++++++++++++//

BmnStatus BmnTrackFitter::FitSmooth(CbmGlobalTrack* track, Bool_t downstream) { //FIXME

    const Int_t n = track->GetNofHits();

    vector<BmnFitNode> nodes = track->GetFitNodes();
    nodes[n - 1].SetSmoothedParam(nodes[n - 1].GetUpdatedParam());

    //        start with the before the last detector plane
    for (int i = n - 1; i > 0; i--) {
        Smooth(&nodes[i - 1], &nodes[i]);
    }

    // Calculate the chi2 of the track
    track->SetChi2(0.);
    for (int i = 0; i < n; i++) {
        //          Float_t chi2Hit = lit::ChiSq(nodes[i].GetSmoothedParam(), track->GetHit(i));  //FIXME
        //          nodes[i].SetChiSqSmoothed(chi2Hit);
        //          track->SetChi2(track->GetChi2() + chi2Hit);
    }

    track->SetParamFirst(nodes[0].GetSmoothedParam());
    track->SetFitNodes(nodes);
    track->SetNDF(lit::NDF(track));

    return kBMNSUCCESS;
}

// We are going in the upstream direction
// this Node (k) , prevNode (k+1)

void BmnTrackFitter::Smooth(//FIXME
        BmnFitNode* thisNode,
        const BmnFitNode* prevNode) {
    //// TMatrixDSym invPrevPredC(5);
    //// prevNode->GetPredictedParam()->GetCovMatrix(invPrevPredC);
    //// invPrevPredC.Invert();
    //   vector<Float_t> invPrevPredC(prevNode->GetPredictedParam()->GetCovMatrix());
    //   InvSym15(invPrevPredC);
    //
    //// TMatrixD Ft(5, 5);
    //// prevNode->GetF(Ft);
    //// Ft.T();
    //   vector<Float_t> Ft(prevNode->GetF());
    //   Transpose25(Ft);
    //
    //// TMatrixDSym thisUpdC(5);
    //// thisNode->GetUpdatedParam()->GetCovMatrix(thisUpdC);
    //   const vector<Float_t>& thisUpdC = thisNode->GetUpdatedParam()->GetCovMatrix();
    //
    //// TMatrixD A(5, 5);
    //// A = thisUpdC * Ft * invPrevPredC;
    //   vector<Float_t> A(25);
    //   vector<Float_t> temp1(25);
    //   Mult15On25(thisUpdC, Ft, temp1);
    //   Mult25On15(temp1, invPrevPredC, A);
    //
    //// TVectorD thisUpdX(5), prevSmoothedX(5), prevPredX(5);
    //// thisNode->GetUpdatedParam()->GetStateVector(thisUpdX);
    //// prevNode->GetSmoothedParam()->GetStateVector(prevSmoothedX);
    //// prevNode->GetPredictedParam()->GetStateVector(prevPredX);
    //// TVectorD thisSmoothedX(thisUpdX + A * (prevSmoothedX - prevPredX));
    //
    //   const vector<Float_t>& thisUpdX = thisNode->GetUpdatedParam()->GetStateVector();
    //   const vector<Float_t>& prevSmoothedX = prevNode->GetSmoothedParam()->GetStateVector();
    //   const vector<Float_t>& prevPredX = prevNode->GetPredictedParam()->GetStateVector();
    //
    //   vector<Float_t> temp2(5), temp3(5);
    //   Subtract(prevSmoothedX, prevPredX, temp2);
    //   Mult25On5(A, temp2, temp3);
    //   vector<Float_t> thisSmoothedX(5);
    //   Add(thisUpdX, temp3, thisSmoothedX);
    //
    //
    //// TMatrixDSym prevSmoothedC(5), prevPredC(5), Cdiff(5);
    //// prevNode->GetSmoothedParam()->GetCovMatrix(prevSmoothedC);
    //// prevNode->GetPredictedParam()->GetCovMatrix(prevPredC);
    //// Cdiff = prevSmoothedC - prevPredC;
    //
    //   const vector<Float_t>& prevSmoothedC = prevNode->GetSmoothedParam()->GetCovMatrix();
    //   const vector<Float_t>& prevPredC = prevNode->GetPredictedParam()->GetCovMatrix();
    //   vector<Float_t> temp4(15);
    //   Subtract(prevSmoothedC, prevPredC, temp4);
    //
    //
    //// TMatrixDSym thisSmoothedC(5);
    //// thisSmoothedC = thisUpdC + Cdiff.Similarity(A);
    //   vector<Float_t> temp5(15);
    //   Similarity(A, temp4, temp5);
    //   vector<Float_t> thisSmoothedC(15);
    //   Add(thisUpdC, temp5, thisSmoothedC);
    //
    //   FairTrackParam par;
    //
    //   par.SetStateVector(thisSmoothedX);
    //   par.SetCovMatrix(thisSmoothedC);
    //   par.SetZ(thisNode->GetUpdatedParam()->GetZ());
    //
    //   thisNode->SetSmoothedParam(&par);
}