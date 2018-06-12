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

class BmnParticlePair : public TNamed {
public:

    /** Default constructor **/
    BmnParticlePair();

    /** Destructor **/
    virtual ~BmnParticlePair();

    void SetV0XZ(Double_t val) {
        fV0XZ = val;
    }

    void SetV0YZ(Double_t val) {
        fV0YZ = val;
    }

    void SetDCA1(Double_t val) {
        fDCA1 = val;
    }

    void SetDCA2(Double_t val) {
        fDCA2 = val;
    }

    void SetDCA12(Double_t val1, Double_t val2) {
        fDCA12X = val1;
        fDCA12Y = val2;
    }

    void SetPath(Double_t val1, Double_t val2) {
        fPathX = val1;
        fPathY = val2;
    }

    void SetInvMass(Double_t val1, Double_t val2) {
        fInvMassX = val1;
        fInvMassY = val2;
    }

    void SetMomPair(Double_t val1, Double_t val2) {
        fMomPart1 = val1;
        fMomPart2 = val2;
    }

    void SetEtaPair(Double_t val1, Double_t val2) {
        fEtaPart1 = val1;
        fEtaPart2 = val2;
    }
    
    void SetPartOrigB(Double_t bx, Double_t by) {
        fPartOrigBX = bx;
        fPartOrigBY = by;
    }

    Double_t GetInvMass(TString flag) {
        return flag.Contains("X") ? fInvMassX : flag.Contains("Y") ? fInvMassY : throw;
    }

    Double_t GetDCA12(TString flag) {
        return flag.Contains("X") ? fDCA12X : flag.Contains("Y") ? fDCA12Y : throw;
    }

    Double_t GetPath(TString flag) {
        return flag.Contains("X") ? fPathX : flag.Contains("Y") ? fPathY : throw;
    }

    Double_t GetDCA1() {
        return fDCA1;
    }

    Double_t GetDCA2() {
        return fDCA2;
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

    Double_t GetV0XZ() {
        return fV0XZ;
    }

    Double_t GetV0YZ() {
        return fV0YZ;
    }
    
    Double_t GetPartOrigBX() {
        return fPartOrigBX;
    }

    Double_t GetPartOrigBY() {
        return fPartOrigBY;
    }

    void SetAlpha(Double_t val1, Double_t val2) {
        fAlphaX = val1;
        fAlphaY = val2;
    }

    void SetPtPodol(Double_t val1, Double_t val2) {
        fPtPodolX = val1;
        fPtPodolY = val2;
    }

    Double_t GetAlpha(TString flag) {
        return flag.Contains("X") ? fAlphaX : flag.Contains("Y") ? fAlphaY : throw;
    }
    
    Double_t GetPtPodol(TString flag) {
        return flag.Contains("X") ? fPtPodolX : flag.Contains("Y") ? fPtPodolY : throw;
    }

private:
    Double_t fInvMassX; // Invariant mass of a considering pair
    Double_t fInvMassY;

    Double_t fV0XZ; // Zv of V0 obtained in XZ-plane
    Double_t fV0YZ; // Zv of V0 obtained in YZ-plane

    Double_t fDCA1; // spatial distance between Vp and extrap. track of Part1 to Vp  
    Double_t fDCA2; // spatial distance between Vp and extrap. track of Part2 to Vp

    Double_t fDCA12X; // spatial distance between the two particles at V0
    Double_t fDCA12Y;

    Double_t fPathX; // distance between Vp and V0 along beam axis
    Double_t fPathY;

    Double_t fMomPart1;
    Double_t fMomPart2;

    Double_t fEtaPart1;
    Double_t fEtaPart2;
    
    //impact param of orig. particle (Lambda, K0s, ...)
    Double_t fPartOrigBX;
    Double_t fPartOrigBY; 

    // Armenteros-Podolyansky ... 
    Double_t fAlphaX;
    Double_t fAlphaY;
    Double_t fPtPodolX;
    Double_t fPtPodolY;

    ClassDef(BmnParticlePair, 1);

};

#endif