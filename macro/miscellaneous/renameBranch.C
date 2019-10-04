////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// renameBranch.C                                                             //
//                                                                            //
// An example macro how to rename Branch in the ROOT file                     //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

int renameBranch(TString filePath = "", TString treeName="bmndata", TString oldName="VC", TString newName = "VETO")
{
    if (filePath == "")
    {
        cout<<"ERROR: File path was not specified"<<endl;
        return -1;
    }

    gSystem->ExpandPathName(filePath);

    TFile* rootFile = new TFile(filePath.Data(), "UPDATE");
    if (!rootFile)
    {
        cout<<"ERROR: File was not found: "<<filePath<<endl;
        return -2;
    }

    TTree* oldTree = (TTree*) rootFile->Get(treeName.Data());
    if (!oldTree)
    {
        cout<<"ERROR: Tree was not found: "<<treeName<<endl;
        return -3;
    }
    TBranch* branch = nullptr;
    branch = oldTree->GetBranch(oldName.Data());
    if (branch)
        branch->SetName(newName.Data());
//    DigiRunHeader * rh = (DigiRunHeader *)rootFile->Get(oldObjectName.Data());    
//    rootFile->WriteObject(rh, newObjectName.Data());
    
    TString oldObjectName = "BmnDigiRunHeader";
    TString newObjectName = "DigiRunHeader";
    TKey* k = nullptr;
    k = rootFile->GetKey(oldObjectName.Data());
    if (k) {
        k->SetName(newObjectName.Data());
    }

    rootFile->Write();
    rootFile->Close();

    delete rootFile;
    return 0;
}

int renameBranch_dir(TString dirPath = "", TString treeName="bmndata", TString oldName="VC", TString newName = "VETO")
{
    if (dirPath == "")
    {
        cout<<"ERROR: Directory path was not specified"<<endl;
        return -1;
    }

    gSystem->ExpandPathName(dirPath);

    TSystemDirectory dir(dirPath.Data(), dirPath.Data());
    TList* file_list = dir.GetListOfFiles();
    if (file_list == NULL)
    {
        cout<<"WARNING: There are no files in the directory: "<<dirPath<<endl;
        return 0;
    }

    int rename_count = 0;
    TSystemFile* cur_file;
    TIter iterFile(file_list);
    while ((cur_file = (TSystemFile*)iterFile.Next()))
    {
        TString file_name = cur_file->GetName();
        if ((file_name == ".") ||  (file_name == "..")) continue;

        TString file_dir = cur_file->GetTitle();
        if (cur_file->IsDirectory())
        {
            // NOW - do nothing
            //del_files_not_in_db(file_dir, period_number, limit_size_in_mb, isOnlyShow);
        }// if directory
        else
        {
            TString file_path = file_dir + "/" + file_name;
            if (file_name.EndsWith("_digi.root"))
            {
                int error_code = renameBranch(file_path, treeName, oldName, newName);
                if (error_code == 0){
                    printf("successful rename of %s\n", file_path.Data());
                    rename_count++;
                }
            }//if (file_name.EndsWith(".root"))
        }//if not a directory
    }//while file list

    if (rename_count > 0)
        cout<<endl<<rename_count<<" files have been successfully processed where the tree was renamed (dir: "<<dirPath<<")"<<endl;

    return 0;
}
