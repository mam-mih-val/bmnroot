////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// renameTree.C                                                               //
//                                                                            //
// An example macro how to rename Tree in the ROOT file                       //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

int renameTree(TString filePath = "", TString oldName="cbmsim", TString newName = "bmndata", TString newTitle = "bmndigit")
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

    TTree* oldTree = (TTree*) rootFile->Get(oldName.Data());
    if (!oldTree)
    {
        cout<<"ERROR: Tree was not found: "<<oldName<<endl;
        return -3;
    }

    //oldTree->SetNameTitle(newName.Data(), newName.Data());
    oldTree->SetObject(newName.Data(), newTitle.Data());
    gDirectory->Delete(TString::Format("%s;*", oldName.Data()));

    rootFile->Write();
    rootFile->Close();

    delete rootFile;
    return 0;
}

int renameTree_dir(TString dirPath = "", TString oldName="cbmsim", TString newName = "bmndata", TString newTitle = "bmndigit")
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
            if (file_name.EndsWith(".root"))
            {
                int error_code = renameTree(file_path, oldName, newName, newTitle);
                if (error_code == 0) rename_count++;
            }//if (file_name.EndsWith(".root"))
        }//if not a directory
    }//while file list

    if (rename_count > 0)
        cout<<endl<<rename_count<<" files have been successfully processed where the tree was renamed (dir: "<<dirPath<<")"<<endl;

    return 0;
}
