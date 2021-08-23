#include "BmnPid.h"
#include "BmnMath.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TFitResult.h"
#include "TDatabasePDG.h"
#include "TParticlePDG.h"

using namespace std;
using namespace TMath;

BmnPid::BmnPid(vector<TParticlePDG*> particles, Int_t power) {
    fEventNo = 0;
    fGlobalTracksArray = NULL;
    fGlobalTracksBranchName = "BmnGlobalTrack";
    fParticles = particles;
    fModelPower = power;
}

BmnPid::~BmnPid() {
}

InitStatus BmnPid::Init(){

    if (fVerbose > 1) cout << "=========================== PID init started ====================" << endl;

    //Get ROOT Manager
    FairRootManager* ioman = FairRootManager::Instance();
    if (NULL == ioman) Fatal("Init", "FairRootManager is not instantiated");

    fGlobalTracksArray = (TClonesArray*) ioman->GetObject(fGlobalTracksBranchName); //in
    if (!fGlobalTracksArray) {
        cout << "PID::Init(): branch " << fGlobalTracksBranchName << " not found! Task will be deactivated" << endl;
        SetActive(kFALSE);
        return kERROR;
    }
   
    if (fVerbose > 1) cout << "=========================== PID init finished ===================" << endl;

    return kSUCCESS;
}

void BmnPid::Exec(Option_t* opt) {
    if (!IsActive())
        return;

    if (fVerbose > 1) cout << "======================== PID exec started  ======================" << endl;
    if (fVerbose > 1) cout << "Event number: " << fEventNo++ << endl;

    SetWeights();
    if (fVerbose > 1) cout << "\n======================== PID exec finished ======================" << endl;

}

void BmnPid::SetWeights(){
    Int_t size = fGlobalTracksArray->GetEntriesFast();

    Double_t rigidity, beta400, beta700, mass, charge, p;
    Int_t nSorts = fParticles.size(); // total number of particle sorts
    
    for (Int_t iTrack = 0; iTrack < size; ++iTrack){
        BmnGlobalTrack* track = (BmnGlobalTrack*) fGlobalTracksArray->UncheckedAt(iTrack); // get iTrack'th track
        track->ResizePidVectors(nSorts);
        for (Int_t iSort = 0; iSort < nSorts; ++iSort){
            track->AddPDGMatch(iSort, fParticles[iSort]->PdgCode());
            rigidity = track->GetP();
            beta400 = track->GetBeta(1);
            beta700 = track->GetBeta(2);
            mass = fParticles[iSort]->Mass();
            charge = fParticles[iSort]->Charge()/3;
            p = rigidity*charge;
            if (beta400 > -999) track->SetPidWeight(1, iSort, EvalSimpleWeight(p, beta400, mass, fModelPower));
            if (beta700 > -999) track->SetPidWeight(2, iSort, EvalSimpleWeight(p, beta700, mass, fModelPower));
        }
        track->NormalizeWeights();
    }
}

Double_t BmnPid::EvalSimpleWeight(Double_t p, Double_t beta, Double_t mass, Int_t power){
    return 1/pow(abs(beta - abs((p/sqrt(p*p + mass*mass)))), power);
}

void BmnPid::Finish() {
}

