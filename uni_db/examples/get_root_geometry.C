#include <stdio.h>

// macro for writing bytes of root geometry file to database
void get_root_geometry(char* root_file_path, int run_number)
{
    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();
    gSystem->Load("libUniDb");

    // get root geometry file's bytes for selected run
    unsigned char* buffer = NULL;
    Long_t file_size;
    int res_code = UniDbRun::GetRootGeometry(run_number, buffer, file_size); //(int run_number, unsigned char* root_geometry, Long_t size_root_geometry)
    if (res_code != 0)
    {
        cout << "\nMacro finished with errors" << endl;
        exit(-1);
    }

    FILE* root_file = fopen(root_file_path, "wb");
    if (root_file == NULL)
    {
        cout<<"Error: creating root file: "<<root_file_path<<endl;
        cout << "\nMacro finished with errors" << endl;
        exit(-2);
    }

    size_t bytes_write = fwrite(buffer, 1, file_size, root_file);
    if (bytes_write != file_size)
    {
        cout<<"Error: writing file: "<<root_file_path<<", put "<<bytes_write<<" bytes of "<<file_size<<endl;
        cout << "\nMacro finished with errors" << endl;
        delete [] buffer;
        fclose(root_file);
        exit(-3);
    }

    fclose(root_file);

    if (buffer)
        delete [] buffer;

    // get gGeoManager from ROOT file (if required)
    TFile* geoFile = new TFile(root_file_path, "READ");
    if (!geoFile->IsOpen())
    {
        cout<<"Error: could not open ROOT file with geometry!"<<endl;
        exit(-4);
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
        exit(-5);
    }

    TGeoNode* N = gGeoManager->GetTopNode();
    cout<<"The top node of gGeoManager is "<<N->GetName()<<endl;

    cout << "\nMacro finished successfully" << endl;
}
