#include "../../db_structures.h"
#include "TString.h"

int AssignMapStructure(IIStructure* pArray, int id, int i1, int i2) {
    pArray[id].int_1 = i1;
    pArray[id].int_2 = i2;
    return 0;
}

// macro for adding parameter value (if parameter exist - you could check existing parameters by 'UniDbParameter::PrintAll()' function)

void add_GEM_local_mapping() {
    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();
    gSystem->Load("libUniDb");

    bool return_error = false;

//    UniDbParameter::CreateParameter("GEM_X_small", IIArrayType);
//    UniDbParameter::CreateParameter("GEM_Y_small", IIArrayType);
//    UniDbParameter::CreateParameter("GEM_X0_middle", IIArrayType);
//    UniDbParameter::CreateParameter("GEM_Y0_middle", IIArrayType);
//    UniDbParameter::CreateParameter("GEM_X1_middle", IIArrayType);
//    UniDbParameter::CreateParameter("GEM_Y1_middle", IIArrayType);
//    UniDbParameter::CreateParameter("GEM_X0_Big_Left", IIArrayType);
//    UniDbParameter::CreateParameter("GEM_X0_Big_Right", IIArrayType);
//    UniDbParameter::CreateParameter("GEM_X1_Big_Left", IIArrayType);
//    UniDbParameter::CreateParameter("GEM_X1_Big_Right", IIArrayType);
//    UniDbParameter::CreateParameter("GEM_Y0_Big_Left", IIArrayType);
//    UniDbParameter::CreateParameter("GEM_Y0_Big_Right", IIArrayType);
//    UniDbParameter::CreateParameter("GEM_Y1_Big_Left", IIArrayType);
//    UniDbParameter::CreateParameter("GEM_Y1_Big_Right", IIArrayType);

    const int kNchXsmall = 256;
    const int kNchYsmall = 256;
    const int kNchX0mid = 190;
    const int kNchY0mid = 215;
    const int kNchX1mid = 825;
    const int kNchY1mid = 930;
    const int kNchX0big_l = 500;
    const int kNchX0big_r = 500;
    const int kNchX1big_l = 1019;
    const int kNchX1big_r = 1019;
    const int kNchY0big_l = 488;
    const int kNchY0big_r = 506;
    const int kNchY1big_l = 1081;
    const int kNchY1big_r = 1130;

    IIStructure* X_small = new IIStructure[kNchXsmall];
    IIStructure* Y_small = new IIStructure[kNchYsmall];
    IIStructure* X0_mid = new IIStructure[kNchX0mid];
    IIStructure* Y0_mid = new IIStructure[kNchY0mid];
    IIStructure* X1_mid = new IIStructure[kNchX1mid];
    IIStructure* Y1_mid = new IIStructure[kNchY1mid];
    IIStructure* X0_big_l = new IIStructure[kNchX0big_l];
    IIStructure* X0_big_r = new IIStructure[kNchX0big_r];
    IIStructure* X1_big_l = new IIStructure[kNchX1big_l];
    IIStructure* X1_big_r = new IIStructure[kNchX1big_r];
    IIStructure* Y0_big_l = new IIStructure[kNchY0big_l];
    IIStructure* Y0_big_r = new IIStructure[kNchY0big_r];
    IIStructure* Y1_big_l = new IIStructure[kNchY1big_l];
    IIStructure* Y1_big_r = new IIStructure[kNchY1big_r];

    TString path = TString(getenv("VMCWORKDIR")) + TString("/input/");
    ReadAndPut(path + TString("GEM_X_small.txt"), X_small);
    ReadAndPut(path + TString("GEM_Y_small.txt"), Y_small);
    ReadAndPut(path + TString("GEM_X0_middle.txt"), X0_mid);
    ReadAndPut(path + TString("GEM_Y0_middle.txt"), Y0_mid);
    ReadAndPut(path + TString("GEM_X1_middle.txt"), X1_mid);
    ReadAndPut(path + TString("GEM_Y1_middle.txt"), Y1_mid);
    ReadAndPut(path + TString("GEM_X0_Big_Left.txt"), X0_big_l);
    ReadAndPut(path + TString("GEM_X0_Big_Right.txt"), X0_big_r);
    ReadAndPut(path + TString("GEM_X1_Big_Left.txt"), X1_big_l);
    ReadAndPut(path + TString("GEM_X1_Big_Right.txt"), X1_big_r);
    ReadAndPut(path + TString("GEM_Y0_Big_Left.txt"), Y0_big_l);
    ReadAndPut(path + TString("GEM_Y0_Big_Right.txt"), Y0_big_r);
    ReadAndPut(path + TString("GEM_Y1_Big_Left.txt"), Y1_big_l);
    ReadAndPut(path + TString("GEM_Y1_Big_Right.txt"), Y1_big_r);

    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_X_small", 4, 61, 4, 84, X_small, kNchXsmall);
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_Y_small", 4, 61, 4, 84, Y_small, kNchYsmall);
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_X0_middle", 4, 61, 4, 84, X0_mid, kNchX0mid);
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_Y0_middle", 4, 61, 4, 84, Y0_mid, kNchY0mid);
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_X1_middle", 4, 61, 4, 84, X1_mid, kNchX1mid);
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_Y1_middle", 4, 61, 4, 84, Y1_mid, kNchY1mid);
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_X0_Big_Left", 4, 61, 4, 84, X0_big_l, kNchX0big_l);
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_X0_Big_Right", 4, 61, 4, 84, X0_big_r, kNchX0big_r);
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_X1_Big_Left", 4, 61, 4, 84, X1_big_l, kNchX1big_l);
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_X1_Big_Right", 4, 61, 4, 84, X1_big_r, kNchX1big_r);
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_Y0_Big_Left", 4, 61, 4, 84, Y0_big_l, kNchY0big_l);
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_Y0_Big_Right", 4, 61, 4, 84, Y0_big_r, kNchY0big_r);
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_Y1_Big_Left", 4, 61, 4, 84, Y1_big_l, kNchY1big_l);
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_Y1_Big_Right", 4, 61, 4, 84, Y1_big_r, kNchY1big_r);

    // clean memory after work
    delete [] X_small;
    delete [] Y_small;
    delete [] X0_mid;
    delete [] Y0_mid;
    delete [] X1_mid;
    delete [] Y1_mid;
    delete [] X0_big_l;
    delete [] X0_big_r;
    delete [] X1_big_l;
    delete [] X1_big_r;
    delete [] Y0_big_l;
    delete [] Y0_big_r;
    delete [] Y1_big_l;
    delete [] Y1_big_r;

    if (return_error)
        cout << "\nMacro finished with errors" << endl;
    else
        cout << "\nMacro finished successfully" << endl;
}

void ReadAndPut(TString fName, IIStructure* pValues) {
    UInt_t ch = 0;
    UInt_t strip = 0;
    ifstream inFile(fName.Data());
    if (!inFile.is_open())
        cout << "Error opening map-file (" << fName << ")!" << endl;
    while (!inFile.eof()) {
        inFile >> ch;
        if (!inFile.good()) break;
        AssignMapStructure(pValues, strip++, strip, ch);
    }
}