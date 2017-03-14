
#include "BmnTofHit.h"

BmnTofHit::BmnTofHit() : FairHit() {
 }

BmnTofHit::BmnTofHit(Int_t detID, TVector3 pos, TVector3 dpos, Int_t index)
: FairHit(detID, pos, dpos, index){
    fUsing = kFALSE;
    fLength = -1.;
}

BmnTofHit::~BmnTofHit() {
 }

Bool_t BmnTofHit::IsUsed() const{
    return fUsing;
}

void BmnTofHit::SetUsing(Bool_t use){
    fUsing = use;
}

void BmnTofHit::SetLength(Double_t Length){
    fLength = Length;
}

Double_t BmnTofHit::GetLength(){
    return fLength;
}

ClassImp(BmnTofHit)