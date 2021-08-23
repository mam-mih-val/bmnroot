// --------------------------------------------------------------------------
// Macro for generating QA-reports
// --------------------------------------------------------------------------
//#include <Rtypes.h>
R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"

void bmn_qa_generator(        
        TString recoFile,        
		TString mcFile) {


    
    // ----  Load libraries   -------------------------------------------------
    Int_t num(TString(recoFile(34, 3)).Atoi());


    TString out = "/home/konst/bmnroot/macro/run/input_stat_" + to_string(num) + ".root";
    TString output = "/home/konst/bmnroot/macro/run/data" + to_string(num);



    bmnloadlibs(); // load bmn libraries
    Int_t nStartEvent = 0;
    Bool_t isPrimary = kFALSE;
    Int_t nEvents = 100;

    // ------------------------------------------------------------------------

    FairRunAna *fRun = new FairRunAna();
    fRun->SetInputFile(recoFile);
    fRun->AddFriend(mcFile);
    fRun->SetOutputFile(out);
    fRun->SetGenerateRunInfo(false);

    Int_t period = 7;

    TString gPathConfig = gSystem->Getenv("VMCWORKDIR");
    TString gPathGemConfig = gPathConfig + "/parameters/gem/XMLConfigs/";
    TString gPathSilConfig = gPathConfig + "/parameters/silicon/XMLConfigs/";
    TString confGem = gPathGemConfig + ((period == 7) ? "GemRunSpring2018.xml" : (period == 6) ? "GemRunSpring2017.xml" : "GemRunSpring2017.xml");
    TString confSil = gPathSilConfig + ((period == 7) ? "SiliconRunSpring2018.xml" : (period == 6) ? "SiliconRunSpring2017.xml" : "SiliconRunSpring2017.xml");

    // ============ TASKS ============= //

    BmnMatchRecoToMC* mcMatching = new BmnMatchRecoToMC();
    fRun->AddTask(mcMatching);

    //  BmnClusteringQa* clQa = new BmnClusteringQa();
    //  clQa->SetOnlyPrimes(isPrimary);
    //  fRun->AddTask(clQa);

    BmnTrackingQa* trQaAll = new BmnTrackingQa(0, "tracking_qa", confGem, confSil);
    trQaAll->SetDetectorPresence(kSILICON, kTRUE);
    trQaAll->SetDetectorPresence(kSSD, kFALSE);
    trQaAll->SetDetectorPresence(kGEM, kTRUE);
    trQaAll->SetOnlyPrimes(isPrimary);
    fRun->AddTask(trQaAll);
//
//    BmnTrackingQa* trQaPos = new BmnTrackingQa(+1, "tracking_qa_positive", confGem, confSil);
//    trQaPos->SetOnlyPrimes(isPrimary);
//    fRun->AddTask(trQaPos);
//
//    BmnTrackingQa* trQaNeg = new BmnTrackingQa(-1, "tracking_qa_negative", confGem, confSil);
//    trQaNeg->SetOnlyPrimes(isPrimary);
//    fRun->AddTask(trQaNeg);
	
/*	TDatabasePDG* db = TDatabasePDG::Instance();
    TParticlePDG* e = db->GetParticle(11);
    TParticlePDG* pi = db->GetParticle(211);
    TParticlePDG* K = db->GetParticle(321);
    TParticlePDG* p = db->GetParticle(2212);
	

	
    TParticlePDG* D = new TParticlePDG("D","D",1.876123928, true, 0, 3, "Core", 1000010020, 1000010020, 1000010020);
    TParticlePDG* T = new TParticlePDG("T","T",2.809432115, true, 0, 3, "Core", 1000010030, 1000010030, 1000010030);
    TParticlePDG* He3 = new TParticlePDG("He3","He3",2.809413523, true, 0, 6, "Core", 1000020030, 1000020030, 1000020030);
    TParticlePDG* He4 = new TParticlePDG("He4","He4",3.728401326, true, 0, 6, "Core", 1000020040, 1000020040, 1000020040);
    
    vector<TParticlePDG*> particles{e, pi, K, p, D, T, He3 , He4};

    BmnPidQa* pidQaAll = new BmnPidQa("pid_qa", particles, output);
    pidQaAll->SetQuota(0.1);
    pidQaAll->SetOnlyPrimes(isPrimary);
    fRun->AddTask(pidQaAll);
*/
    // ============ TASKS ============= //

    // -----   Intialise and run   --------------------------------------------
    fRun->Init();
    cout << "Starting run" << endl;
    fRun->Run(nStartEvent, nStartEvent + nEvents);
    // ------------------------------------------------------------------------
}
