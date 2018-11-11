/** 
 * File:   BmnRecLambda.h
 * Author: Alexander Lytaev
 * \brief supplementary class, extension of BmnParticlePair to store and operate the information about reconstructed MC lambda and it's properties
 * \author Sergey Merts <sergey.merts@gmail.com> - modifications for BMN experiment
 * \author Alexander Lytaev <sas-lyt@ya.ru> - modifications for BMN experiment 
 * \date  July, 2018
 */

#ifndef BMNRECLAMBDA_H
#define BMNRECLAMBDA_H

#include <TVector3.h>
#include "BmnParticlePair.h"
#include "CbmMCTrack.h"

class BmnRecLambda: public BmnParticlePair {
public:
    //Default constructor
    BmnRecLambda();
    BmnRecLambda(BmnParticlePair* pair, Double_t Px , Double_t Py , Double_t Pz);
    
    //BmnRecLambda(const BmnRecLambda& orig);
       
    //Default destructor
    virtual ~BmnRecLambda();
    
    void SetMomLambda(Double_t Px, Double_t Py, Double_t Pz); 
    
    void SetPairChargeSign(Int_t q1, Int_t q2);
    void SetVMomPart1(Double_t Px, Double_t Py, Double_t Pz);
    void SetVMomPart2(Double_t Px, Double_t Py, Double_t Pz);
    void SetMomMCLambda(Double_t Px, Double_t Py, Double_t Pz);
//    void SetMCTracks(CbmMCTrack* tr1, CbmMCTrack* tr2);
    void SetMCTracksId(Int_t tr1Id, Int_t tr2Id);
    
    Double_t GetMomLambdaX();
    Double_t GetMomLambdaY();
    Double_t GetMomLambdaZ();
    Int_t GetPart1ChargeSign();
    Int_t GetPart2ChargeSign();
    TVector3 GetVMomPart1();
    TVector3 GetVMomPart2();
    Double_t GetMomMCLambdaX();
    Double_t GetMomMCLambdaY();
    Double_t GetMomMCLambdaZ();
    Double_t GetMomMCLambda();
    Int_t GetMCTrack1Id();
    Int_t GetMCTrack2Id();
//    CbmMCTrack* GetMCTrack1();
//    CbmMCTrack* GetMCTrack2();
    
private:
    Double_t fMomLambdaX;
    Double_t fMomLambdaY;
    Double_t fMomLambdaZ;
    Double_t fMomMCLambdaX;
    Double_t fMomMCLambdaY;
    Double_t fMomMCLambdaZ;
    Int_t fMCTrack1Id;
    Int_t fMCTrack2Id;
//    CbmMCTrack* fMCTrack1;
//    CbmMCTrack* fMCTrack2;
    
    
    Int_t fPart1ChargeSign;
    Int_t fPart2ChargeSign;
    TVector3 fVMomPart1;
    TVector3 fVMomPart2;
    
    ClassDef(BmnRecLambda, 1);
};

#endif /* BMNRECLAMBDA_H */

