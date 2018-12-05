// --------------------------------------------------------------------------
// Macro for generating QA-reports
// --------------------------------------------------------------------------
#include <TString.h>
#include <Rtypes.h>

R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"

/*Important note: */        
/*Evetest.root must be passed obligatory!*/ 
/*All other files can be omitted*/       
void bmn_qa_generator_lambda(
        TString recoFile = "", // bmndst.root file with matches
        TString mcFile = "", // evetest.root 
        TString lambdaFile = "", // File got from LambdaAnal.root with DST-input
        TString lambdaEveFile = "", // File got from LambdaAnal.root with EVE-input
        TString outFile = "qa.root", Int_t nStartEvent = 0, Int_t nEvents = 1e6) {
    // ----  Load libraries   -------------------------------------------------
    
    bmnloadlibs(); // load bmn libraries
    if (mcFile.IsNull()) {
        cout << "No correct input specified! At least, evetest.root must be passed!" << endl;
        return;
    }

    // ------------------------------------------------------------------------

    FairRunAna *fRun = new FairRunAna();

    fRun->SetInputFile(mcFile);
    if (!lambdaEveFile.IsNull())
        fRun->AddFriend(lambdaEveFile);
    if (!lambdaFile.IsNull())
        fRun->AddFriend(lambdaFile);
    if (!recoFile.IsNull())
        fRun->AddFriend(recoFile);
    fRun->SetOutputFile(outFile);

    fRun->SetGenerateRunInfo(false);

    // ============ TASKS ============= //

    BmnMatchRecoToMC* mcMatching = new BmnMatchRecoToMC();
    if (!recoFile.IsNull() && !mcFile.IsNull())
        fRun->AddTask(mcMatching);

    BmnLambdaQa* lambdaQa = new BmnLambdaQa();
    
    // Geometry cuts if necessary
    lambdaQa->SetDCA1(0., 1000.);
    lambdaQa->SetDCA2(0., 1000.);
    lambdaQa->SetDCA12(0., 1000.);
    lambdaQa->SetPath(0., 1000.);

    // Kinematic cuts, if necessary
    lambdaQa->SetMom1(0., 10.);
    lambdaQa->SetMom2(0., 10.);
    lambdaQa->SetEta1(0., 10.);
    lambdaQa->SetEta2(0., 10.);
    
    fRun->AddTask(lambdaQa);

    // ============ TASKS ============= //

    // -----   Intialise and run   --------------------------------------------
    fRun->Init();
    cout << "Starting run" << endl;
    fRun->Run(nStartEvent, nStartEvent + nEvents);
    // ------------------------------------------------------------------------
}
