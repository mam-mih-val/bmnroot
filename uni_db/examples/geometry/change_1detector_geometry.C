//#define DEBUG
bool CheckFileExist(TString fileName);
// Macro for changing single detector in facility geometry for a given run range in the database
//
// start_period - start number of the period to change single detector geometry in full geometry
// start_run - start number of the run range to change single detector geometry in full geometry
// end_period - end number of the period to change single detector geometry in full geometry
// end_run - end number of the run range to change single detector geometry in full geometry
// detector_name - volume name of the detector geoemtry to replace it with new geometry
// detector_file_path - path to the file with new subdetector geometry
void change_1detector_geometry(int start_period = 7, int start_run = 2041, int end_period = 7, int end_run = 3588,
                               const char* detector_name = "TOF400",
                               const char* detector_file_path = "$VMCWORKDIR/geometry/TOF400_RUN7_SRC_AllignmentZY_v2.root")
{
    // check existence of the file with new detector geometry
    TString strDetectorFilePath(detector_file_path);
    gSystem->ExpandPathName(strDetectorFilePath);
    if (!CheckFileExist(strDetectorFilePath))
    {
        cout<<"ERROR: File path with a new detector geometry does not exist: "<<strDetectorFilePath<<endl;
        exit(-5);
    }

    // generate output name for temporary file with old full geometry
    TString strFullGeoFileName;
    srand(time(NULL));
    strFullGeoFileName.Form("full_old_%x%x.root",//-%x-%x-%x-%x%x%x",
                            rand(), rand());//,                 // Generates a 64-bit Hex number
                          //    rand(),                         // Generates a 32-bit Hex number
                          //    ((rand() & 0x0fff) | 0x4000),   // Generates a 32-bit Hex number of the form 4xxx (4 indicates the UUID version)
                          //    rand() % 0x3fff + 0x8000,       // Generates a 32-bit Hex number in the range [0x8000, 0xbfff]
                          //    rand(), rand(), rand());        // Generates a 96-bit Hex number

    // get full geometry from database for a given run
    int res_code = UniDbRun::ReadGeometryFile(start_period, start_run, (char*)strFullGeoFileName.Data()); // FIX 3rd parameter - const char*
    if (res_code != 0)
    {
        cout<<"\nMacro finished with errors"<<endl;
        exit(-1);
    }

    // get gGeoManager from ROOT file
    TFile* fileFullGeo = new TFile(strFullGeoFileName, "READ");
    if (!fileFullGeo->IsOpen())
    {
        cout<<"ERROR: could not open ROOT file with full geometry!"<<endl;
        exit(-2);
    }

    TList* keyList = fileFullGeo->GetListOfKeys();
    TIter next(keyList);
    TKey* key = (TKey*)next();
    TString className(key->GetClassName());
    if (className.BeginsWith("TGeoManager"))
        key->ReadObj();
    else
    {
        cout<<"ERROR: TGeoManager is not top element in given file "<<strFullGeoFileName<<endl;
        delete fileFullGeo;
        exit(-3);
    }
    fileFullGeo->Close();
    delete fileFullGeo;
    gSystem->Unlink(strFullGeoFileName);

    // get old gGeoManager: name and title
    TString oldGeoManagerName = gGeoManager->GetName(), oldGeoManagerTitle = gGeoManager->GetTitle();

    // get top node
    TGeoNode* top_node = gGeoManager->GetTopNode();
#ifdef DEBUG
    cout<<"The top node of the old gGeoManager is "<<top_node->GetName()<<endl;
#endif

    // cycle for all detector volumes and exporting them to temporary files
    TObjArray* arrNodes = top_node->GetNodes();
    Int_t top_volume_number = top_node->GetNdaughters();
    TObjArray arrDetectorNames, arrDetectorGeoNames;
    for (int i = 0; i < top_volume_number; i++)
    {
        TGeoNode* cur_node = (TGeoNode*) arrNodes->At(i);
        TGeoVolume* cur_volume = cur_node->GetVolume();
        TString volume_name = cur_volume->GetName();
        arrDetectorNames.AddLast((TObject*)(new TObjString(cur_volume->GetName())));

        TString new_det = " ";
        TFile* fileDetectorGeo = NULL;
        if (volume_name == TString(detector_name))
        {
            // get detector volume from ROOT file
            fileDetectorGeo = new TFile(strDetectorFilePath, "READ");
            if (!fileDetectorGeo->IsOpen())
            {
                cout<<"ERROR: could not open ROOT file with detector geometry: "<<strDetectorFilePath<<endl;
                arrDetectorNames.Delete();
                arrDetectorGeoNames.Delete();
                exit(-7);
            }

            keyList = fileDetectorGeo->GetListOfKeys();
            TIter next2(keyList);
            key = (TKey*)next2();
            TString className(key->GetClassName());
            if (className.BeginsWith("TGeoVolume"))
            {
                cur_volume = (TGeoVolume*) key->ReadObj();
                if (!(cur_volume->GetName() == TString(detector_name)))
                {
                    cur_volume = cur_volume->GetNode(0)->GetVolume();
                    if (!(cur_volume->GetName() == TString(detector_name)))
                        cur_volume = NULL;
                }
            }
            if (!cur_volume)
            {
                cout<<"ERROR: detector volume was not found in the file: "<<strDetectorFilePath<<endl;
                arrDetectorNames.Delete();
                arrDetectorGeoNames.Delete();
                delete fileDetectorGeo;
                exit(-8);
            }

            new_det = " new ";
            cout<<"Replacing detector "<<detector_name<<"..."<<endl;
        }

        TObjString* strDetectorGeoName = new TObjString((TString::Format("%s_%x%x.root", volume_name.Data(), rand(), rand())).Data());
        arrDetectorGeoNames.AddLast((TObject*)strDetectorGeoName);

        // export top detector geometries to different files
#ifdef DEBUG
        cout<<"Exporting"<<new_det<<"detector '"<<volume_name<<"' to file: "<<strDetectorGeoName->String()<<"..."<<endl;
#endif
        cur_volume->Export(strDetectorGeoName->String(), volume_name);

        if (fileDetectorGeo != NULL)
        {
            fileDetectorGeo->Close();
            delete fileDetectorGeo;
        }
    }

    // create new gGeoManager and import all detector geometries
    TGeoManager* newGeoManager = new TGeoManager(oldGeoManagerName, oldGeoManagerTitle);
    TGeoMaterial* matAir = new TGeoMaterial("air",14.61,7.3,0.1205000E-02);
    matAir->SetUniqueID(15);
    TGeoMedium* medAir = new TGeoMedium("air",1,15,0,1,0.19,0.25,-1,-1,0.1000000E-02,-1);
    TGeoVolume* cave = newGeoManager->MakeBox("cave", medAir, 2000., 2000., 2000.);

    // cycle for all detector volumes and importing them from the temporary files
    for (int i = 0; i < top_volume_number; i++)
    {
        TObjString* impDetectorName = (TObjString*)arrDetectorNames.At(i);
        TObjString* impDetectorFile = (TObjString*)arrDetectorGeoNames.At(i);
#ifdef DEBUG
        cout<<"Importing detector '"<<impDetectorName->String()<<"' from file: "<<impDetectorFile->String()<<"..."<<endl;
#endif

        TGeoVolume* detector_volume = TGeoVolume::Import(impDetectorFile->String(), impDetectorName->String());
        if (detector_volume == NULL)
        {
            cout<<"ERROR: Importing detector from file: "<<impDetectorFile->String()<<endl;
            arrDetectorNames.Delete();
            arrDetectorGeoNames.Delete();
            exit(-6);
        }
        cave->AddNode(detector_volume, 0, 0);

        // delete temporary files with single detector geometry
        gSystem->Unlink(impDetectorFile->String());
    }
    newGeoManager->SetTopVolume(cave);
    newGeoManager->CloseGeometry();

    // clear arrays with temporary file names
    arrDetectorNames.Delete();
    arrDetectorGeoNames.Delete();

    // write new full geometry to the temporary file
    TString strNewGeoFile;
    strNewGeoFile.Form("full_new_%x%x.root", rand(), rand());
    newGeoManager->Export(strNewGeoFile);

    // write ROOT file with detector geometry for run range and then delete it
    res_code = UniDbRun::WriteGeometryFile(start_period, start_run, end_period, end_run, strNewGeoFile.Data());
    gSystem->Unlink(strNewGeoFile);
    if (res_code != 0)
    {
        cout << "\nMacro finished with errors" << endl;
        exit(-4);
    }

    cout << "\nMacro finished successfully" << endl;
}

// check whether file exists
bool CheckFileExist(TString fileName)
{
    gSystem->ExpandPathName(fileName);
    if (gSystem->AccessPathName(fileName.Data()) == true)
    {
        cout<<endl<<"no specified file: "<<fileName<<endl;
        return false;
    }

    return true;
}
