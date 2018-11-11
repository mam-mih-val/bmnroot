#include <Rtypes.h>
#include <TString.h>
#include <TStopwatch.h>
#include <TFile.h>
#include <TKey.h>
R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"

void digiConverter(TString in, TString out, Int_t nEvents) {
    bmnloadlibs(); // load BmnRoot libraries
    // -----   Timer   ---------------------------------------------------------
    TStopwatch timer;
    timer.Start();
    FairRunAna* fRunAna = new FairRunAna();
    FairSource* fFileSource = new BmnFileSource(in.Data());
    fRunAna->SetSource(fFileSource);
    
    fRunAna->SetOutputFile(out.Data());
    
    BmnDigiConverter* converter = new BmnDigiConverter();
    fRunAna->AddTask(converter);
    
    fRunAna->Init();
    fRunAna->Run(0, nEvents);
    
    delete converter;   
}