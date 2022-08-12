#include "BmnPid.h"
#include "BmnMath.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TFitResult.h"
#include <TStopwatch.h>

static Double_t workTime = 0.0;

using namespace std;
using namespace TMath;

BmnPid::BmnPid(Int_t power) {
    fEventNo = 0;
    fGlobalTracksArray = NULL;
    fGlobalTracksBranchName = "BmnGlobalTrack";
    fModelPower = power;
}

BmnPid::~BmnPid() {
}

InitStatus BmnPid::Init()
{
    if (fVerbose > 1) cout << "=========================== PID init started ====================" << endl;

    db = TDatabasePDG::Instance();

    //Add several types of particles to the database.
    // p, pi, K, e are already in it
    db->AddParticle("D","D",1.876123928, true, 0, 3, "Core", 1000010020, 1000010020, 1000010020);
    db->AddParticle("T","T",2.809432115, true, 0, 3, "Core", 1000010030, 1000010030, 1000010030);
    db->AddParticle("He3","He3",2.809413523, true, 0, 6, "Core", 1000020030, 1000020030, 1000020030);
    db->AddParticle("He4","He4",3.728401326, true, 0, 6, "Core", 1000020040, 1000020040, 1000020040);

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

    TStopwatch sw;
    sw.Start();
    
    if (!IsActive())
        return;

    if (fVerbose > 1) cout << "======================== PID exec started  ======================" << endl;
    if (fVerbose > 1) cout << "Event number: " << fEventNo++ << endl;

    SetVector();
    if (fVerbose > 1) cout << "\n======================== PID exec finished ======================" << endl;

    sw.Stop();
    workTime += sw.RealTime();
}

void BmnPid::SetVector(){
    Int_t size = fGlobalTracksArray->GetEntriesFast();
    Double_t rigidity, beta400, beta700, mass, charge, p;

    for (Int_t iTrack = 0; iTrack < size; ++iTrack){
        BmnGlobalTrack* track = (BmnGlobalTrack*) fGlobalTracksArray->UncheckedAt(iTrack); // get iTrack'th track
        vector<Double_t> vectorTof400;
        vector<Double_t> vectorTof700;        

        for(PidParticles iSort=(PidParticles)0; iSort!=EndPidEnum; iSort=(PidParticles)(iSort+1)){
            Int_t pdg = EnumToPdg(iSort);
            TParticlePDG* iParticle = db->GetParticle(pdg);
            rigidity = track->GetP();
            beta400 = track->GetBeta(1);
            beta700 = track->GetBeta(2);
            mass = iParticle->Mass();
            charge = iParticle->Charge()/3;
            p = rigidity*charge;
            if (beta400 > -999) vectorTof400.push_back(EstimateProbability(p, beta400, mass, fModelPower));
            if (beta700 > -999) vectorTof700.push_back(EstimateProbability(p, beta700, mass, fModelPower));   
        }
        NormalizeVector(vectorTof400);
        NormalizeVector(vectorTof700);

        track->SetPidVectorTof400(vectorTof400);
        track->SetPidVectorTof700(vectorTof700);
    }
}

Double_t BmnPid::EstimateProbability(Double_t p, Double_t beta, Double_t mass, Int_t power){
    return 1/pow(abs(beta - abs((p/sqrt(p*p + mass*mass)))), power);
}

Int_t BmnPid::EnumToPdg(PidParticles part){
    switch(part)
    {
        case PidProton:
            return 2212;
        case PidPion:
            return 211;
        case PidKaon:
            return 321;
        case PidElectron:
            return 11;
        case PidDeuteron:
            return 1000010020;
        case PidTriton:
            return 1000010030;
       case PidHelium3:
            return 1000020030;
        case PidHelium4:
            return 1000020040;
        default:
            cout << "No such particle in PidParticles\n";
            return -1;
    }
}

void BmnPid::NormalizeVector(vector<Double_t>& vec){
    Double_t sum = GetSum(vec);
    for(auto iter = vec.begin(); iter != vec.end(); iter++)  *iter=*iter/sum;
}

Double_t BmnPid::GetSum(const vector<Double_t>& vec){
    Double_t sum = 0;
    for(auto i:vec) 
        sum+=i;
    return sum;    
}

void BmnPid::Finish() {
    printf("Work time of BmnPid: %4.2f sec.\n", workTime);
}

