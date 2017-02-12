// @(#)bmnroot/mwpc:$Id$
// Author: Pavel Batyuk (VBLHEP) <pavel.batyuk@jinr.ru> 2017-02-12

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// BmnMwpcTrackFinder                                                         //
//                                                                            //
//                                                                            //
// The algorithm serves for searching for track segments                      //
// in the MWPC of the BM@N experiment                                         //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
#include "BmnMwpcTrackFinder.h"

static Float_t workTime = 0.0;

BmnMwpcTrackFinder::BmnMwpcTrackFinder(Bool_t isExp) :
fEventNo(0),
expData(isExp) {
    fInputBranchName = "BmnMwpcHit";
    fOutputBranchName = "BmnMwpcTrack";
}

BmnMwpcTrackFinder::~BmnMwpcTrackFinder() {

}

void BmnMwpcTrackFinder::Exec(Option_t* opt) {
    clock_t tStart = clock();
    cout << "\n======================== MWPC track finder exec started ====================\n" << endl;
    cout << "Event number: " << fEventNo++ << endl;

    cout << "\n======================== MWPC track finder exec finished ===================" << endl;
    clock_t tFinish = clock();
    workTime += ((Float_t) (tFinish - tStart)) / CLOCKS_PER_SEC;
}

InitStatus BmnMwpcTrackFinder::Init() {
    if (!expData)
        return kERROR;
    cout << "BmnMwpcTrackFinder::Init()" << endl;
    FairRootManager* ioman = FairRootManager::Instance();

    fBmnMwpcHitsArray = (TClonesArray*) ioman->GetObject(fInputBranchName);

    fBmnMwpcTracksArray = new TClonesArray(fOutputBranchName.Data());
    ioman->Register(fOutputBranchName.Data(), "MWPC", fBmnMwpcTracksArray, kTRUE);
}

void BmnMwpcTrackFinder::Finish() {
    cout << "Work time of the MWPC track finder: " << workTime << " s" << endl;
}

ClassImp(BmnMwpcTrackFinder)

