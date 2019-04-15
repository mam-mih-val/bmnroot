#include <Rtypes.h>
#include <TString.h>
#include <TStopwatch.h>
#include <TFile.h>
#include <TKey.h>

R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"

void SplitDetectors(TString, TString, TString);  
void ComposeDigiFile(UInt_t);
        
// Valid for RUN6
// DIGITS_GEM_SILICON - in1
// DIGITS_ZDC - in2
// DIGITS_OTHER_DETECTORS - in3
void digiMergerBMN_RUN6(UInt_t runN = 1223,
        TString in1 = "/nica/mpd16/kapishin/run6/digit/bmn_run1223_sigemdigitthr2.root",
        TString in2 = "", 
        TString in3 = "/nica/mpd16/kapishin/run6/digit/bmn_run1223_digi.root") {

    bmnloadlibs(); // load BmnRoot libraries
    // -----   Timer   ---------------------------------------------------------
    TStopwatch timer;
    timer.Start();

    SplitDetectors(in1, in2, in3);
    ComposeDigiFile(runN);

}

void SplitDetectors(TString in1, TString in2, TString in3) {
    BmnDigiMergeTask* mergeTask = new BmnDigiMergeTask(in1, in2, in3);
    mergeTask->SplitToDetectors();
    delete mergeTask;
}

void ComposeDigiFile(UInt_t runN) { 
    TString run = TString::Format("%d", runN); 
    
    TString in1 = "GEM_" + run + ".root"; // GEM
    TString in2 = "SILICON_" + run + ".root"; // SILICON
    TString in3 = "ZDC_" + run + ".root"; // ZDC
    TString in4 = "T0_" + run + ".root"; // T0
    TString in5 = "BC1_" + run + ".root"; // BC1
    TString in6 = "BC2_" + run + ".root"; // BC2
    TString in7 = "VETO_" + run + ".root"; // VETO
    TString in8 = "FD_" + run + ".root"; // FD
    TString in9 = "BD_" + run + ".root"; // BD
    TString in10 = "MWPC_" + run + ".root"; // MWPC
    TString in11 = "DCH_" + run + ".root"; // DCH
    TString in12 = "TOF400_" + run + ".root"; // TOF400
    TString in13 = "TOF700_" + run + ".root"; // TOF700
    TString in14 = "ECAL_" + run + ".root"; // ECAL
    
    TString out = "bmn_run" + run + "_digi.root";

    const Int_t nFiles = 14;
    vector <TString> files;
    TString _files[nFiles] = {in1, in2, in3, in4, in5, in6, in7, in8, in9, in10, in11, in12, in13, in14};

    for (Int_t iFile = 0; iFile < nFiles; iFile++)
        files.push_back(_files[iFile]);

    BmnDigiMergeTask* mergeTask = new BmnDigiMergeTask(files, out);
    mergeTask->ProcessEvents();
    delete mergeTask;
}