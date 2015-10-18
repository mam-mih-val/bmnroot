#include <stdio.h>

// macro for downloading ROOT file with detector geometry from database
void get_root_geometry(char* root_file_path, int run_number)
{
    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();
    gSystem->Load("libUniDb");

    int res_code = UniDbRun::ReadGeometryFile(run_number, root_file_path); //(int run_number, char* geo_file_path)
    if (res_code != 0)
    {
        cout << "\nMacro finished with errors" << endl;
        exit(-1);
    }

    // get gGeoManager from ROOT file (if required)
    TFile* geoFile = new TFile(root_file_path, "READ");
    if (!geoFile->IsOpen())
    {
        cout<<"Error: could not open ROOT file with geometry!"<<endl;
        exit(-2);
    }

    TList* keyList = geoFile->GetListOfKeys();
    TIter next(keyList);
    TKey* key = (TKey*)next();
    TString className(key->GetClassName());
    if (className.BeginsWith("TGeoManager"))
        key->ReadObj();
    else
    {
        cout<<"Error: TGeoManager isn't top element in given file "<<root_file_path<<endl;
        exit(-3);
    }

    TGeoNode* N = gGeoManager->GetTopNode();
    cout<<"The top node of gGeoManager is "<<N->GetName()<<endl;

    cout << "\nMacro finished successfully" << endl;
}
