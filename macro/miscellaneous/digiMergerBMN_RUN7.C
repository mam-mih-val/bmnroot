#include <Rtypes.h>
#include <TString.h>
#include <TStopwatch.h>
#include <TFile.h>
#include <TKey.h>

using namespace std;

R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"

void SplitDetectors(TString, TString, TString);  
void ComposeDigiFile(UInt_t);
        
// Valid for RUN7
// DIGITS_OTHER_DETECTORS - in1
// DIGITS_ZDC_TRIGGERS - in2
// DIGITS_TOF700 - in3

void digiMergerBMN_RUN7(UInt_t runN = 4649,
        TString in1 = "/nica/mpd20/kapishin/run7/digit/bmn_run4649_sigemdigittrigcmode.root",
        TString in2 = "/nica/mpd22/lenivenko/run7/digit/bmn_run4649_digi_zdc.root", 
        TString in3 = "/nica/mpd22/lenivenko/run7/digit/bmn_run4649_digi_tof700.root") {

    bmnloadlibs(); // load BmnRoot libraries
    // -----   Timer   ---------------------------------------------------------
    TStopwatch timer;
    timer.Start();

    SplitDetectors(in1, in2, in3);
    ComposeDigiFile(runN);
}

void SplitDetectors(TString in1, TString in2, TString in3) {
    TString input[3] = {in1, in2, in3};
    BmnDigiMergeTask* mergeTask = new BmnDigiMergeTask(input);
    mergeTask->SplitToDetectors();
    delete mergeTask;
}

void ComposeDigiFile(UInt_t runN) { 
    TString run = TString::Format("%d", runN); 
    
    TString in1 = "BC1_" + run + ".root"; // BC1
    TString in2 = "BC2_" + run + ".root"; // BC2
    TString in3 = "BC3_" + run + ".root"; // BC3
    TString in4 = "VC_" + run + ".root"; // VETO
    TString in5 = "Si_" + run + ".root"; // Si
    TString in6 = "BD_" + run + ".root"; // BD
    TString in7 = "ZDC_" + run + ".root"; // ZDC
    TString in8 = "GEM_" + run + ".root"; // GEM
    TString in9 = "SILICON_" + run + ".root"; // SILICON
    TString in10 = "MWPC_" + run + ".root"; // MWPC
    TString in11 = "DCH_" + run + ".root"; // DCH
    TString in12 = "TOF400_" + run + ".root"; // TOF400
    TString in13 = "ECAL_" + run + ".root"; // TOF700
    TString in14 = "CSC_" + run + ".root"; // ECAL
    TString in15 = "TOF700_" + run + ".root"; // CSC
    
    TString out = "bmn_run" + run + "_digi.root";

    const Int_t nFiles = 15;
    vector <TString> files;
    TString _files[nFiles] = {in1, in2, in3, in4, in5, in6, in7, in8, in9, in10, in11, in12, in13, in14, in15};

    for (Int_t iFile = 0; iFile < nFiles; iFile++)
        files.push_back(_files[iFile]);

    BmnDigiMergeTask* mergeTask = new BmnDigiMergeTask(files, out);
    mergeTask->ProcessEvents();
    delete mergeTask;
}