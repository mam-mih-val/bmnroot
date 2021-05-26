// macro for adding parameter value (if parameter exist - you could check existing parameters by 'UniDbParameter::PrintAll()' function)
void ReadAndPut(TString fName, vector<UniValue*>& pValues);

void AssignMapStructure(vector<UniValue*>& pArray, int i1, int i2)
{
    IIValue* pValue = new IIValue;
    pValue->value1 = i1;
    pValue->value2 = i2;

    pArray.push_back(pValue);
}

void add_GEM_local_mapping()
{
    bool return_error = false;

//    UniDbParameter::CreateParameter("GEM_X_small", IIType);
//    UniDbParameter::CreateParameter("GEM_Y_small", IIType);
//    UniDbParameter::CreateParameter("GEM_X0_middle", IIType);
//    UniDbParameter::CreateParameter("GEM_Y0_middle", IIType);
//    UniDbParameter::CreateParameter("GEM_X1_middle", IIType);
//    UniDbParameter::CreateParameter("GEM_Y1_middle", IIType);
//    UniDbParameter::CreateParameter("GEM_X0_Big_Left", IIType);
//    UniDbParameter::CreateParameter("GEM_X0_Big_Right", IIType);
//    UniDbParameter::CreateParameter("GEM_X1_Big_Left", IIType);
//    UniDbParameter::CreateParameter("GEM_X1_Big_Right", IIType);
//    UniDbParameter::CreateParameter("GEM_Y0_Big_Left", IIType);
//    UniDbParameter::CreateParameter("GEM_Y0_Big_Right", IIType);
//    UniDbParameter::CreateParameter("GEM_Y1_Big_Left", IIType);
//    UniDbParameter::CreateParameter("GEM_Y1_Big_Right", IIType);

    vector<UniValue*> X_small;
    vector<UniValue*> Y_small;
    vector<UniValue*> X0_mid;
    vector<UniValue*> Y0_mid;
    vector<UniValue*> X1_mid;
    vector<UniValue*> Y1_mid;
    vector<UniValue*> X0_big_l;
    vector<UniValue*> X0_big_r;
    vector<UniValue*> X1_big_l;
    vector<UniValue*> X1_big_r;
    vector<UniValue*> Y0_big_l;
    vector<UniValue*> Y0_big_r;
    vector<UniValue*> Y1_big_l;
    vector<UniValue*> Y1_big_r;

    TString path = TString(getenv("VMCWORKDIR")) + TString("/input/");
    ReadAndPut(path + TString("GEM_X_small.txt"), X_small);
    ReadAndPut(path + TString("GEM_Y_small.txt"), Y_small);
    ReadAndPut(path + TString("GEM_X0_middle.txt"), X0_mid);
    ReadAndPut(path + TString("GEM_Y0_middle.txt"), Y0_mid);
    ReadAndPut(path + TString("GEM_X1_middle.txt"), X1_mid);
    ReadAndPut(path + TString("GEM_Y1_middle.txt"), Y1_mid);
    ReadAndPut(path + TString("GEM_X0_Big_Left_period_5.txt"), X0_big_l);
    ReadAndPut(path + TString("GEM_X0_Big_Right_period_5.txt"), X0_big_r);
    ReadAndPut(path + TString("GEM_X1_Big_Left_period_5.txt"), X1_big_l);
    ReadAndPut(path + TString("GEM_X1_Big_Right_period_5.txt"), X1_big_r);
    ReadAndPut(path + TString("GEM_Y0_Big_Left_period_5.txt"), Y0_big_l);
    ReadAndPut(path + TString("GEM_Y0_Big_Right_period_5.txt"), Y0_big_r);
    ReadAndPut(path + TString("GEM_Y1_Big_Left_period_5.txt"), Y1_big_l);
    ReadAndPut(path + TString("GEM_Y1_Big_Right_period_5.txt"), Y1_big_r);
    
    UInt_t period = 7;
    UInt_t runMin = 1;
    UInt_t runMax = 1e4;

    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_X_small", period, runMin, period, runMax, X_small);
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_Y_small", period, runMin, period, runMax, Y_small);
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_X0_middle", period, runMin, period, runMax, X0_mid);
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_Y0_middle", period, runMin, period, runMax, Y0_mid);
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_X1_middle", period, runMin, period, runMax, X1_mid);
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_Y1_middle", period, runMin, period, runMax, Y1_mid);
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_X0_Big_Left", period, runMin, period, runMax, X0_big_l);
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_X0_Big_Right", period, runMin, period, runMax, X0_big_r);
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_X1_Big_Left", period, runMin, period, runMax, X1_big_l);
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_X1_Big_Right", period, runMin, period, runMax, X1_big_r);
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_Y0_Big_Left", period, runMin, period, runMax, Y0_big_l);
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_Y0_Big_Right", period, runMin, period, runMax, Y0_big_r);
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_Y1_Big_Left", period, runMin, period, runMax, Y1_big_l);
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_Y1_Big_Right", period, runMin, period, runMax, Y1_big_r);

    // clean memory after work
    for (int i = 0; i < X_small.size(); i++) delete X_small[i];
    for (int i = 0; i < Y_small.size(); i++) delete Y_small[i];
    for (int i = 0; i < X0_mid.size(); i++) delete X0_mid[i];
    for (int i = 0; i < Y0_mid.size(); i++) delete Y0_mid[i];
    for (int i = 0; i < X1_mid.size(); i++) delete X1_mid[i];
    for (int i = 0; i < Y1_mid.size(); i++) delete Y1_mid[i];
    for (int i = 0; i < X0_big_l.size(); i++) delete X0_big_l[i];
    for (int i = 0; i < X0_big_r.size(); i++) delete X0_big_r[i];
    for (int i = 0; i < X1_big_l.size(); i++) delete X1_big_l[i];
    for (int i = 0; i < X1_big_r.size(); i++) delete X1_big_r[i];
    for (int i = 0; i < Y0_big_l.size(); i++) delete Y0_big_l[i];
    for (int i = 0; i < Y0_big_r.size(); i++) delete Y0_big_r[i];
    for (int i = 0; i < Y1_big_l.size(); i++) delete Y1_big_l[i];
    for (int i = 0; i < Y1_big_r.size(); i++) delete Y1_big_r[i];

    if (return_error)
        cout << "\nMacro finished with errors" << endl;
    else
        cout << "\nMacro finished successfully" << endl;
}

void ReadAndPut(TString fName, vector<UniValue*>& pValues)
{
    UInt_t ch = 0;
    UInt_t strip = 0;
    ifstream inFile(fName.Data());
    if (!inFile.is_open())
        cout << "Error opening map-file (" << fName << ")!" << endl;
    while (!inFile.eof()) {
        inFile >> ch;
        if (!inFile.good()) break;
        AssignMapStructure(pValues, strip++, ch);
    }
}
