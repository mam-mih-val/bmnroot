// macro for adding parameter value (if parameter exist - you could check existing parameters by 'UniDbParameter::PrintAll()' function)
void add_GEM_constants() {
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

    IntValue iValue;
    iValue.value = 256;
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_N_ch_X_small", 5, 1, 5, 1e4, &iValue);
    iValue.value = 256;
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_N_ch_Y_small", 5, 1, 5, 1e4, &iValue);
    iValue.value = 190;
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_N_ch_X0_middle", 5, 1, 5, 1e4, &iValue);
    iValue.value = 215;
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_N_ch_Y0_middle", 5, 1, 5, 1e4, &iValue);
    iValue.value = 825;
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_N_ch_X1_middle", 5, 1, 5, 1e4, &iValue);
    iValue.value = 930;
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_N_ch_Y1_middle", 5, 1, 5, 1e4, &iValue);
    iValue.value = 500;
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_N_ch_X0_big_l", 5, 1, 5, 1e4, &iValue);
    iValue.value = 488;
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_N_ch_Y0_big_l", 5, 1, 5, 1e4, &iValue);
    iValue.value = 1019;
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_N_ch_X1_big_l", 5, 1, 5, 1e4, &iValue);
    iValue.value = 1081;
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_N_ch_Y1_big_l", 5, 1, 5, 1e4, &iValue);
    iValue.value = 500;
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_N_ch_X0_big_r", 5, 1, 5, 1e4, &iValue);
    iValue.value = 506;
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_N_ch_Y0_big_r", 5, 1, 5, 1e4, &iValue);
    iValue.value = 1019;
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_N_ch_X1_big_r", 5, 1, 5, 1e4, &iValue);
    iValue.value = 1130;
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_N_ch_Y1_big_r", 5, 1, 5, 1e4, &iValue);

    if (return_error)
        cout << "\nMacro finished with errors" << endl;
    else
        cout << "\nMacro finished successfully" << endl;
}
