#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TClonesArray.h>
#include <Rtypes.h>
#include <TTimeStamp.h>
#include <TMathBase.h>

R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"

/**
 * readHeaders
 * Example of reading valuable content of the DigiRunHeader and BmnEventHeader
 * @param fileName
 * @param examples number of events to show
 */
void readHeaders(TString fileName = "", Int_t examples = 10) {
    if (fileName == "") {
        cout << "File not specified!" << endl;
        return;
    }
    bmnloadlibs(); // load libraries

    TFile *file = new TFile(fileName, "READ");
    if (file->IsOpen() == false) {
        printf("\nCannot open file %s !\n", fileName.Data());
        return;
    }
    /**
     * Reading Run Header for the whole run statistics:
     *  beam counters, run start/end time 
     */
    printf("Run Header content:\n");
    DigiRunHeader * rh = (DigiRunHeader *) file->Get("DigiRunHeader");
    
    UInt_t PeriodId =  rh->GetPeriodId();
    UInt_t RunId =  rh->GetRunId();
    printf("\tPeriod: %u  Run: %u\n", PeriodId, RunId);
    
    Double_t BT = rh->GetBT(); // Beam Trigger
    Double_t BTnBusy = rh->GetBTnBusy(); // BT & not Busy
        // (BT & not Busy) normalized by Before&After protection ratio
    Double_t TotalFlux = rh->GetBTAccepted();
    printf("\tBeam Trigger                       : %f\n", BT);
    printf("\tBeam Trigger and Not Busy          : %f\n", BTnBusy);
    printf("\tBeam Trigger Accepted Events (Flux): %f\n", TotalFlux);
    TTimeStamp RunStartTime = rh->GetRunStartTime();
    TTimeStamp RunEndTime = rh->GetRunEndTime();
    printf("\tSTART  : %s\n", RunStartTime.AsString());
    printf("\tFINISH : %s\n", RunEndTime.AsString());
    /**
     * Reading Event Header for each event in the range:
     *  current spill statistics, 
     */
    TTree * tree = (TTree *) file->Get("bmndata");
    BmnEventHeader * eh = nullptr;
    tree->SetBranchAddress("BmnEventHeader.", &eh);
    Long64_t NEvents = tree->GetEntries();
    Long64_t NExamples = examples; // Examples to show
    NExamples = Min(NEvents, NExamples);
    for (Int_t i = 0; i < NExamples; i++) {
        tree->GetEntry(i);
        BmnTrigInfo * ti = eh->GetTrigInfo();
        printf("Event %u\n", eh->GetEventId()); 
        printf("\tU40VE:\tCandidates %6u,  Accepted %6u, Before %4u,  After %6u, Rejected %6u\n",
                ti->GetTrigCand(),
                ti->GetTrigAccepted(),
                ti->GetTrigBefo(),
                ti->GetTrigAfter(),
                ti->GetTrigRjct());

    }
    
    printf(" %lld events shown.\n To see more events change the second argument of the macro.\n", NExamples); 


}
