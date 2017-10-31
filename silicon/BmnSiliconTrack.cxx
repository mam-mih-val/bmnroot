#include "BmnSiliconTrack.h"

// -----   Default constructor   -------------------------------------------

BmnSiliconTrack::BmnSiliconTrack()
: BmnTrack(),
fUsed(kFALSE) {

}
// -------------------------------------------------------------------------

//// -----   Destructor   ----------------------------------------------------

BmnSiliconTrack::~BmnSiliconTrack() {
}

void BmnSiliconTrack::Print() {
    printf("\n==== TRACK INFORMATION ====\n");
    printf("Number of hits: %d\n", GetNHits());
    printf("Chi^2: \t\t%3.2f\n", GetChi2());
    printf("Length: \t%3.2f\n\n", GetLength());
    FairTrackParam* parFirst = GetParamFirst();
    FairTrackParam* parLast = GetParamLast();
    printf("\tFirst Track Param:\n");
    printf("Position: \t(%3.2f, %3.2f, %3.2f)\n", parFirst->GetX(), parFirst->GetY(), parFirst->GetZ());
    printf("Slopes: \tTx = %3.2f, Ty = %3.2f\n", parFirst->GetTx(), parFirst->GetTy());
    printf("Rigidity: \tp/q = %3.2f\n\n", 1.0 / parFirst->GetQp());
    printf("\tLast Track Param:\n");
    printf("Position: \t(%3.2f, %3.2f, %3.2f)\n", parLast->GetX(), parLast->GetY(), parLast->GetZ());
    printf("Slopes: \tTx = %3.2f, Ty = %3.2f\n", parLast->GetTx(), parLast->GetTy());
    printf("Rigidity: \tp/q = %3.2f\n\n", 1.0 / parLast->GetQp());
}

ClassImp(BmnSiliconTrack)
