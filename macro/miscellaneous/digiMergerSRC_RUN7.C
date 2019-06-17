#include <Rtypes.h>
#include <TString.h>
#include <TStopwatch.h>
#include <TFile.h>
#include <TKey.h>
#include <vector>

using namespace std;

R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"

void SplitDetectors(TString, TString, TString);
void ComposeDigiFile(UInt_t);

// Valid for RUN7 (SRC)
// DIGITS_GEM_SILICON - in1
// DIGITS_TOF700_TRIGGERS - in2
// DIGIT_ALL_OTHER_DETS - in3

void digiMergerSRC_RUN7(UInt_t runN = 2320,
        TString in1 = "bmn_run2320_sigemdigitthr2src.root",
        TString in2 = "bmn_run2320_digi_tof700.root",
        TString in3 = "/nica/bmn1/lenivenko/run7/digit/bmn_run2320_digi.root") {

    bmnloadlibs(); // load BmnRoot libraries
    // -----   Timer   ---------------------------------------------------------
    TStopwatch timer;
    timer.Start();

    SplitDetectors(in1, in2, in3);
    ComposeDigiFile(runN);
}

void SplitDetectors(TString in1, TString in2, TString in3) {
    const Int_t nFiles = 3;
    TString _files[nFiles] = {in1, in2, in3};
    vector <TString> files;
    for (Int_t iFile = 0; iFile < nFiles; iFile++)
        files.push_back(_files[iFile]);

    BmnDigiMergeTask* mergeTask = new BmnDigiMergeTask(files);
    mergeTask->SplitToDetectors(kTRUE);
    delete mergeTask;
}

void ComposeDigiFile(UInt_t runN) {
    TString run = TString::Format("%d", runN);
   
    TString in1 = "GEM_" + run + ".root";
    TString in2 = "SILICON_" + run + ".root";
    TString in3 = "CSC_" + run + ".root";
    
    TString in4 = "BC1_" + run + ".root";
    TString in5 = "BC2_" + run + ".root";
    TString in6 = "BC3_" + run + ".root";
    TString in7 = "BC4_" + run + ".root";
    TString in8 = "TQDC_BC1_" + run + ".root";
    TString in9 = "TQDC_BC2_" + run + ".root";
    TString in10 = "TQDC_BC3_" + run + ".root";
    TString in11 = "TQDC_BC4_" + run + ".root";
    
    TString in12 = "X1L_" + run + ".root";
    TString in13 = "X2L_" + run + ".root";
    TString in14 = "Y1L_" + run + ".root";
    TString in15 = "Y2L_" + run + ".root";
    TString in16 = "X1R_" + run + ".root";
    TString in17 = "X2R_" + run + ".root";
    TString in18 = "Y1R_" + run + ".root";
    TString in19 = "Y2R_" + run + ".root";
    TString in20 = "TQDC_X1L_" + run + ".root";
    TString in21 = "TQDC_X2L_" + run + ".root";
    TString in22 = "TQDC_Y1L_" + run + ".root";
    TString in23 = "TQDC_Y2L_" + run + ".root";
    TString in24 = "TQDC_X1R_" + run + ".root";
    TString in25 = "TQDC_X2R_" + run + ".root";
    TString in26 = "TQDC_Y1R_" + run + ".root";
    TString in27 = "TQDC_Y2R_" + run + ".root";
    
    TString in28 = "VETO_" + run + ".root";
    TString in29 = "TQDC_VETO_" + run + ".root";
    
    TString in30 = "TOF700_" + run + ".root";
    TString in31 = "ZDC_" + run + ".root";
    TString in32 = "MWPC_" + run + ".root";
    TString in33 = "DCH_" + run + ".root";
    TString in34 = "TOF400_" + run + ".root";   
    TString in35 = "ECAL_" + run + ".root";
   
    TString out = "bmn_run" + run + "_digi.root";

    const Int_t nFiles = 35;
    vector <TString> files;
    TString _files[nFiles] = {in1, in2, in3, in4, in5, in6, in7, in8, in9, in10, in11, in12, in13, in14, in15,
        in16, in17, in18, in19, in20, in21, in22, in23, in24, in25, in26, in27, in28, in29, in30,
        in31, in32, in33, in34, in35};

    for (Int_t iFile = 0; iFile < nFiles; iFile++)
        files.push_back(_files[iFile]);

    BmnDigiMergeTask* mergeTask = new BmnDigiMergeTask(files, out);
    mergeTask->ProcessEvents();
    delete mergeTask;
}