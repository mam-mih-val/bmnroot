// -------------------------------------------------------------------------
// -----                    BmnGlobalTrack source file                 -----
// -----                          Created S. Merts                     -----
// -------------------------------------------------------------------------
#include "BmnGlobalTrack.h"

#include <iostream>

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
BmnGlobalTrack::BmnGlobalTrack()
    : fGemTrack(-1),
      fSilTrack(-1),
      fSsdTrack(-1),
      fTof1Hit(-1),
      fTof2Hit(-1),
      fDch1Track(-1),
      fDch2Track(-1),
      fDchTrack(-1),
      fMwpc1Track(-1),
      fMwpc2Track(-1),
      fCscHit(-1),
      fUpstreamTrack(-1),
      fBeta400(-1000.0),
      fBeta700(-1000.0),
      fdQdNLower(0.0),
      fdQdNUpper(0.0),
      fA(-1),
      fZ(0),
      fPDG(0),
      fIsPrimary(kTRUE) {
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
BmnGlobalTrack::~BmnGlobalTrack() {}
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
//void BmnGlobalTrack::Print() const {

    //cout << endl << "MwpcTrack " << fMwpc1Track << ", SilHit " << fSilTrack << ", GemTrack " << fGemTrack << ", Tof1Hit " << fTof1Hit << ", Tof2Hit " << fTof2Hit << ", DchTrack " << fDchTrack << endl;
//}
// -------------------------------------------------------------------------

void BmnGlobalTrack::SetBeta(Double_t b, Int_t tofID) {
    if (tofID == 1)
        fBeta400 = b;
    else if (tofID == 2)
        fBeta700 = b;
}

Double_t BmnGlobalTrack::GetMass2(Int_t tofID) {
    // p^2/gamma^2/beta^2
    Double_t beta = this->GetBeta(tofID);
    if (beta < -999.0) return -1000.0;
    return TMath::Sq(this->GetP()) * (1 / beta / beta - 1);
}

Int_t BmnGlobalTrack::GetMaxWeightInd(Int_t tofID){
    Double_t maxWeight = 0;
    Int_t maxWeightInd = 0;

    if ((tofID == 1) || (tofID == 2))
        for(auto it = fPidWeights.begin(); it != fPidWeights.end(); ++it){
            if (maxWeight < (it->second)[tofID - 1]){
                maxWeight = (it->second)[tofID - 1];
                maxWeightInd = Int_t(it - fPidWeights.begin());
            }
        }
    else
         maxWeightInd = -1;
    return maxWeightInd;
}

Int_t BmnGlobalTrack::GetMostPossiblePDG(Int_t tofID){
    return fPidWeights[this->GetMaxWeightInd(tofID)].first;
}

Double_t BmnGlobalTrack::GetMaxWeight(Int_t tofID){
    if ((tofID == 1) || (tofID == 2))
        return fPidWeights[this->GetMaxWeightInd(tofID)].second[tofID-1];
    else
        return -1;
}

Double_t BmnGlobalTrack::GetSumWeight(Int_t tofID){
    Double_t sum = 0;
    Int_t i = 0;
    if ((tofID == 1) || (tofID == 2))
        for (auto it = fPidWeights.begin(); it != fPidWeights.end(); ++it)
            sum += it->second[tofID-1];
    else return -1;
    return sum;
}

Double_t BmnGlobalTrack::GetPidWeightByPDG(Int_t pdgCode, Int_t tofID){
    for (auto it = fPidWeights.begin(); it != fPidWeights.end(); ++it)
        if (it->first == pdgCode)
            return it->second[tofID-1];
    return -1;
}


void BmnGlobalTrack::NormalizeWeights(){
    Int_t i;
    Double_t sum400, sum700;
    sum400 = GetSumWeight(1);
    sum700 = GetSumWeight(2);
    for (auto it = fPidWeights.begin(); it != fPidWeights.end(); ++it){
        it->second[0] = it->second[0]/sum400;
        it->second[1] = it->second[1]/sum700;
    }
}

void BmnGlobalTrack::PrintWeights(Int_t tofID){
    for(auto it = fPidWeights.begin(); it != fPidWeights.end(); ++it){
        cout << it->second[tofID-1] << endl;
    }
}

void BmnGlobalTrack::ResizePidVectors(Int_t size){
    fPidWeights.resize(size);
}

ClassImp(BmnGlobalTrack)
