// @(#)bmnroot/physics/particles:$Id$
// Author: Pavel Batyuk <pavel.batyuk@jinr.ru> 2017-06-20

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// BmnParticlePair                                                            //
//                                                                            //
//  A class to consider selected pair of particles                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef BMNPARTICLEPAIR_H
#define BMNPARTICLEPAIR_H 1

#include <TNamed.h>
#include <TVector3.h>

class BmnParticlePair : public TNamed {
public:

    /** Default constructor **/
    BmnParticlePair();

    /** Destructor **/
    virtual ~BmnParticlePair();

    void SetV0X(Double_t val) {
        fV0X = val;
    }

    void SetV0Y(Double_t val) {
        fV0Y = val;
    }

    void SetV0Z(Double_t val) {
        fV0Z = val;
    }

    void SetDCA0(Double_t val) {
        fDCA0 = val;
    }
    
    void SetDCA1(Double_t val) {
        fDCA1 = val;
    }

    void SetDCA2(Double_t val) {
        fDCA2 = val;
    }

    void SetDCA12(Double_t val) {
        fDCA12 = val;
    }

    void SetPath(Double_t val) {
        fPath = val;
    }

    void SetInvMass(Double_t val) {
        fInvMass = val;
    }

    void SetMomPair(Double_t val1, Double_t val2) {
        fMomPart1 = val1;
        fMomPart2 = val2;
    }

    void SetEtaPair(Double_t val1, Double_t val2) {
        fEtaPart1 = val1;
        fEtaPart2 = val2;
    }
    
    void SetTxPair(Double_t val1, Double_t val2) {
        fTxPart1 = val1;
        fTxPart2 = val2;
    }
    
    void SetTyPair(Double_t val1, Double_t val2) {
        fTyPart1 = val1;
        fTyPart2 = val2;
    }

    void SetMCTrackIdPart1(Int_t id);
    void SetMCTrackIdPart2(Int_t id);
    void SetRecoTrackIdPart1(Int_t id); // actually it is GEM track Id
    void SetRecoTrackIdPart2(Int_t id); // actually it is GEM track Id

    void SetMCMomPart1(Double_t px, Double_t py, Double_t pz);
    void SetMCMomPart2(Double_t px, Double_t py, Double_t pz);

    void SetNHitsPair(Int_t nHits1, Int_t nHits2) {
        fNHitsPart1 = nHits1;
        fNHitsPart2 = nHits2;
    }

    Int_t GetNHitsPart1() {
        return fNHitsPart1;
    }

    Int_t GetNHitsPart2() {
        return fNHitsPart2;
    }

    Double_t GetInvMass() {
        return fInvMass;
    }

    Double_t GetDCA12() {
        return fDCA12;
    }

    Double_t GetPath() {
        return fPath;
    }

    Double_t GetDCA1() {
        return fDCA1;
    }

    Double_t GetDCA2() {
        return fDCA2;
    }
    
    Double_t GetDCA0() {
        return fDCA0;
    }

    Double_t GetMomPart1() {
        return fMomPart1;
    }

    Double_t GetMomPart2() {
        return fMomPart2;
    }

    Double_t GetEtaPart1() {
        return fEtaPart1;
    }

    Double_t GetEtaPart2() {
        return fEtaPart2;
    }

    Double_t GetV0X() {
        return fV0X;
    }

    Double_t GetV0Y() {
        return fV0Y;
    }

    Double_t GetV0Z() {
        return fV0Z;
    }

    Double_t GetTxPart1() {
        return fTxPart1;
    }

    Double_t GetTxPart2() {
        return fTxPart2;
    }

    Double_t GetTyPart1() {
        return fTyPart1;
    }

    Double_t GetTyPart2() {
        return fTyPart2;
    }

    void SetAlpha(Double_t val) {
        fAlpha = val;
    }

    void SetPtPodol(Double_t val) {
        fPtPodol = val;
    }

    Double_t GetAlpha() {
        return fAlpha;
    }

    Double_t GetPtPodol() {
        return fPtPodol;
    }

    TVector3 GetMCMomPart1();
    TVector3 GetMCMomPart2();

    Int_t GetMCTrackIdPart1();
    Int_t GetMCTrackIdPart2();

    Int_t GetRecoTrackIdPart1(); // actually it is GEM track Id
    Int_t GetRecoTrackIdPart2(); // actually it is GEM track Id

private:
    Double_t fInvMass; // Invariant mass of a considering pair

    Double_t fV0X;
    Double_t fV0Y;
    Double_t fV0Z;

    Double_t fDCA0;
    Double_t fDCA1;
    Double_t fDCA2;

    Double_t fDCA12;

    Double_t fPath;

    Double_t fMomPart1;
    Double_t fMomPart2;

    Double_t fEtaPart1;
    Double_t fEtaPart2;

    Int_t fNHitsPart1;
    Int_t fNHitsPart2;

    Double_t fTxPart1;
    Double_t fTxPart2;
    Double_t fTyPart1;
    Double_t fTyPart2;

    // Armenteros-Podolyansky ... 
    Double_t fAlpha;
    Double_t fPtPodol;

    TVector3 fMCMomPart1;
    TVector3 fMCMomPart2;

    TVector3 fRecoMomPart1;
    TVector3 fRecoMomPart2;

    Int_t fMCTrackIdPart1;
    Int_t fMCTrackIdPart2;
    Int_t fRecoTrackIdPart1;
    Int_t fRecoTrackIdPart2;

    ClassDef(BmnParticlePair, 1);

};

#endif