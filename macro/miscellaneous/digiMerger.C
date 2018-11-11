#include <Rtypes.h>
#include <TString.h>
#include <TStopwatch.h>
#include <TFile.h>
#include <TKey.h>
R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"

void digiMerger(TString inGEM, TString inSIL, TString out, Int_t nEvents = 200000) {
    bmnloadlibs(); // load BmnRoot libraries
    // -----   Timer   ---------------------------------------------------------
    TStopwatch timer;
    timer.Start();
    FairRunAna* fRunAna = new FairRunAna();
    
    FairSource* fFileSource = new BmnFileSource(inGEM);
    fRunAna->SetSource(fFileSource);
  
    fRunAna->SetOutputFile(out.Data());
    
    BmnDigiMerger* merger = new BmnDigiMerger(inSIL);
    fRunAna->AddTask(merger);
    
    fRunAna->Init();
    fRunAna->Run(0, nEvents);
    
    delete merger;   
}