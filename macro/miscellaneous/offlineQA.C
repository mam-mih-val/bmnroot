#include <Rtypes.h>
#include <TString.h>
#include <TStopwatch.h>
#include <TFile.h>
#include <TKey.h>

R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"

void offlineQA(TString digiFile = "", 
        TString dstBMN = "", 
        TString dstCBM = "", 
        TString out = "", 
        Int_t nEvents = 1e6) { 

    bmnloadlibs(); // load libraries

    // -----   Timer   ---------------------------------------------------------
    TStopwatch timer;
    timer.Start();
    FairRunAna* fRunAna = new FairRunAna();
    fRunAna->SetEventHeaderPersistence(kFALSE);
    //fRunAna->SetEventHeader(new BmnEventHeader());
    
    // fRunAna->SetInputFile(digiFile);
    
    // fRunAna->AddFriend(dstCBM);
    // fRunAna->SetOutputFile(out.Data());
    
    FairSource* fFileSource = new BmnFileSource(digiFile.Data());
    fRunAna->SetSource(fFileSource);
    // fRunAna->SetInputFile(dstBMN);
    // fRunAna->AddFriend(dstBMN);
    
    fRunAna->SetOutputFile(out.Data());
    
    BmnQaOffline* qaSystem = new BmnQaOffline(dstBMN);
    fRunAna->AddTask(qaSystem);
    
    fRunAna->Init();
    fRunAna->Run(0, nEvents);
    
    delete qaSystem;   
}

