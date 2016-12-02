#include "TString.h"

// macro for adding parameter value (if parameter exist - you could check existing parameters by 'UniDbParameter::PrintAll()' function)

void add_GEM_constants() {
    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();
    gSystem->Load("libUniDb");

    bool return_error = false;

    UniDbParameter::CreateParameter("GEM_N_ch_X_small", IntType);
    UniDbParameter::CreateParameter("GEM_N_ch_Y_small", IntType);
    UniDbParameter::CreateParameter("GEM_N_ch_X0_middle", IntType);
    UniDbParameter::CreateParameter("GEM_N_ch_Y0_middle", IntType);
    UniDbParameter::CreateParameter("GEM_N_ch_X1_middle", IntType);
    UniDbParameter::CreateParameter("GEM_N_ch_Y1_middle", IntType);
    UniDbParameter::CreateParameter("GEM_N_ch_X0_big_l", IntType);
    UniDbParameter::CreateParameter("GEM_N_ch_Y0_big_l", IntType);
    UniDbParameter::CreateParameter("GEM_N_ch_X1_big_l", IntType);
    UniDbParameter::CreateParameter("GEM_N_ch_Y1_big_l", IntType);
    UniDbParameter::CreateParameter("GEM_N_ch_X0_big_r", IntType);
    UniDbParameter::CreateParameter("GEM_N_ch_Y0_big_r", IntType);
    UniDbParameter::CreateParameter("GEM_N_ch_X1_big_r", IntType);
    UniDbParameter::CreateParameter("GEM_N_ch_Y1_big_r", IntType);
    
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_N_ch_X_small", 4, 61, 4, 84, 256);
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_N_ch_Y_small", 4, 61, 4, 84, 256);
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_N_ch_X0_middle", 4, 61, 4, 84, 190);
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_N_ch_Y0_middle", 4, 61, 4, 84, 215);
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_N_ch_X1_middle", 4, 61, 4, 84, 825);
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_N_ch_Y1_middle", 4, 61, 4, 84, 930);
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_N_ch_X0_big_l", 4, 61, 4, 84, 500);
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_N_ch_Y0_big_l", 4, 61, 4, 84, 488);
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_N_ch_X1_big_l", 4, 61, 4, 84, 1019);
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_N_ch_Y1_big_l", 4, 61, 4, 84, 1081);
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_N_ch_X0_big_r", 4, 61, 4, 84, 500);
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_N_ch_Y0_big_r", 4, 61, 4, 84, 506);
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_N_ch_X1_big_r", 4, 61, 4, 84, 1019);
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_N_ch_Y1_big_r", 4, 61, 4, 84, 1130);

    if (return_error)
        cout << "\nMacro finished with errors" << endl;
    else
        cout << "\nMacro finished successfully" << endl;
}