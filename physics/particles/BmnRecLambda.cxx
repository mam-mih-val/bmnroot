/** 
 * File:   BmnRecLambda.cxx
 * Author: Alexander Lytaev
 * \brief supplementary class, extension of BmnParticlePair to store and operate the information about reconstructed MC lambda and it's properties
 * \author Sergey Merts <sergey.merts@gmail.com> - modifications for BMN experiment
 * \author Alexander Lytaev <sas-lyt@ya.ru> - modifications for BMN experiment 
 * \date  July, 2018
 */

#include "BmnRecLambda.h"
#include "BmnParticlePair.h"
#include <iostream>

BmnRecLambda::BmnRecLambda(): BmnParticlePair()
{
}

BmnRecLambda::BmnRecLambda(BmnParticlePair* pair = NULL, Double_t Px = 0, Double_t Py = 0, Double_t Pz = 0): 
BmnParticlePair(*pair),
fMomLambdaX(Px),       
fMomLambdaY(Py),
fMomLambdaZ(Pz),
fPart1ChargeSign(0),
fPart2ChargeSign(0),
fMCTrack1Id(0),
fMCTrack2Id(0)
{
    fVMomPart1.SetXYZ(0, 0, 0);
    fVMomPart2.SetXYZ(0, 0, 0);
}

BmnRecLambda::~BmnRecLambda() {
}

void BmnRecLambda::SetMomLambda(Double_t Px, Double_t Py, Double_t Pz){
    fMomLambdaX = Px;
    fMomLambdaY = Py;
    fMomLambdaZ = Pz;
}


Double_t BmnRecLambda::GetMomLambdaX(){
    return fMomLambdaX;
}

Double_t BmnRecLambda::GetMomLambdaY(){
    return fMomLambdaY;
}

Double_t BmnRecLambda::GetMomLambdaZ(){
    return fMomLambdaZ;
}

Int_t BmnRecLambda::GetPart1ChargeSign(){
    return fPart1ChargeSign;
}

Int_t BmnRecLambda::GetPart2ChargeSign(){
    return fPart2ChargeSign;
}

void BmnRecLambda::SetPairChargeSign(Int_t q1, Int_t q2){
    fPart1ChargeSign=q1;
    fPart2ChargeSign=q2;
}

TVector3 BmnRecLambda::GetVMomPart1(){
    return TVector3(fVMomPart1);
}

TVector3 BmnRecLambda::GetVMomPart2(){
    return TVector3(fVMomPart2);
}

void BmnRecLambda::SetVMomPart1(Double_t Px, Double_t Py, Double_t Pz){
    fVMomPart1 = TVector3(Px, Py, Pz);
}

void BmnRecLambda::SetVMomPart2(Double_t Px, Double_t Py, Double_t Pz){
    fVMomPart2 = TVector3(Px, Py, Pz);
}

void BmnRecLambda::SetMCTracksId(Int_t tr1Id, Int_t tr2Id){
    fMCTrack1Id = tr1Id;
    fMCTrack2Id = tr2Id;
}

Int_t BmnRecLambda::GetMCTrack1Id(){
    return fMCTrack1Id;
}

Int_t BmnRecLambda::GetMCTrack2Id(){
    return fMCTrack2Id;
}
