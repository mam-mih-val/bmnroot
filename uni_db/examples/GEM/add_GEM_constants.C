#include "TString.h"

// macro for adding parameter value (if parameter exist - you could check existing parameters by 'UniDbParameter::PrintAll()' function)

void add_GEM_constants() {
    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();
    gSystem->Load("libUniDb");

    bool return_error = false;

//    UniDbParameter::CreateParameter("GEM_N_ch_X_small", IntType);
//    UniDbParameter::CreateParameter("GEM_N_ch_Y_small", IntType);
//    UniDbParameter::CreateParameter("GEM_N_ch_X0_middle", IntType);
//    UniDbParameter::CreateParameter("GEM_N_ch_Y0_middle", IntType);
//    UniDbParameter::CreateParameter("GEM_N_ch_X1_middle", IntType);
//    UniDbParameter::CreateParameter("GEM_N_ch_Y1_middle", IntType);
//    UniDbParameter::CreateParameter("GEM_N_ch_X0_big_l", IntType);
//    UniDbParameter::CreateParameter("GEM_N_ch_Y0_big_l", IntType);
//    UniDbParameter::CreateParameter("GEM_N_ch_X1_big_l", IntType);
//    UniDbParameter::CreateParameter("GEM_N_ch_Y1_big_l", IntType);
//    UniDbParameter::CreateParameter("GEM_N_ch_X0_big_r", IntType);
//    UniDbParameter::CreateParameter("GEM_N_ch_Y0_big_r", IntType);
//    UniDbParameter::CreateParameter("GEM_N_ch_X1_big_r", IntType);
//    UniDbParameter::CreateParameter("GEM_N_ch_Y1_big_r", IntType);
    
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_N_ch_X_small", 5, 1, 5, 1e4, 256);
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_N_ch_Y_small", 5, 1, 5, 1e4, 256);
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_N_ch_X0_middle", 5, 1, 5, 1e4, 190);
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_N_ch_Y0_middle", 5, 1, 5, 1e4, 215);
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_N_ch_X1_middle", 5, 1, 5, 1e4, 825);
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_N_ch_Y1_middle", 5, 1, 5, 1e4, 930);
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_N_ch_X0_big_l", 5, 1, 5, 1e4, 500);
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_N_ch_Y0_big_l", 5, 1, 5, 1e4, 488);
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_N_ch_X1_big_l", 5, 1, 5, 1e4, 1019);
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_N_ch_Y1_big_l", 5, 1, 5, 1e4, 1081);
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_N_ch_X0_big_r", 5, 1, 5, 1e4, 500);
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_N_ch_Y0_big_r", 5, 1, 5, 1e4, 506);
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_N_ch_X1_big_r", 5, 1, 5, 1e4, 1019);
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_N_ch_Y1_big_r", 5, 1, 5, 1e4, 1130);

    if (return_error)
        cout << "\nMacro finished with errors" << endl;
    else
        cout << "\nMacro finished successfully" << endl;
}