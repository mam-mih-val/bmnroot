#include <Rtypes.h>
#include <TString.h>
#include <TStopwatch.h>
#include <TFile.h>
#include <TKey.h>

R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"

void offlineQA(TString digiFile = "/nica/mpd22/batyuk/digi/run6/bmn_run1558_digi.root",
//        TString dstBMN = "/nica/mpd22/batyuk/srcDST/srcdst_3387.root",
        TString dstBMN = "",
        Int_t periodId = 6,
        Int_t runId = 1558,
        TString out = "qa_1558.root",
        Int_t nEvents = 1e6) {

    bmnloadlibs(); // load libraries

    // -----   Timer   ---------------------------------------------------------
    TStopwatch timer;
    timer.Start();
    FairRunAna* fRunAna = new FairRunAna();
    fRunAna->SetEventHeaderPersistence(kFALSE);

    FairSource* fFileSource = new BmnFileSource(digiFile.Data());
    fRunAna->SetSource(fFileSource);

    fRunAna->SetOutputFile(out.Data());

    UniRun* pCurrentRun = UniRun::GetRun(periodId, runId); // FIXME
    Double_t* field_voltage = pCurrentRun->GetFieldVoltage();
   
    Double_t map_current = 55.87;
    Double_t fieldScale = (*field_voltage < 10.) ? 0 : (*field_voltage) / map_current;

    BmnFieldMap* magField = new BmnNewFieldMap("field_sp41v4_ascii_Extrap.root");
    magField->SetScale(fieldScale);
    magField->Init();
    fRunAna->SetField(magField);

    BmnQaOffline* qaSystem = new BmnQaOffline(dstBMN);
    fRunAna->AddTask(qaSystem);

    fRunAna->Init();
    fRunAna->Run(0, nEvents);

    delete qaSystem;
}

