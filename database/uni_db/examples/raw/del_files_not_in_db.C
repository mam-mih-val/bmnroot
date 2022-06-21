#include "../../function_set.h"

// function for recursively deleting all *.data files in a given directory (and all subdirectories)
// which are absent in the Unified Database for a given run
// returns the success code: 0 - without errors, error_code - otherwise
int del_files_not_in_db(TString dir_name, int period_number, double limit_size_in_mb = 0, bool isOnlyShow = true)
{
    gSystem->ExpandPathName(dir_name);
    TSystemDirectory dir(dir_name.Data(), dir_name.Data());
    TList* file_list = dir.GetListOfFiles();
    if (file_list == nullptr)
    {
        cout<<"WARNING: There are no files in the directory: "<<dir_name<<endl;
        return 0;
    }

    int del_count = 0;
    TSystemFile* cur_file;
    TIter iterFile(file_list);
    while ((cur_file = (TSystemFile*)iterFile.Next()))
    {
        TString file_name = cur_file->GetName();
        if ((file_name == ".") || (file_name == "..")) continue;

        TString file_dir = cur_file->GetTitle();
        if (cur_file->IsDirectory())
        {
            del_files_not_in_db(file_dir, period_number, limit_size_in_mb, isOnlyShow);
        }// if directory
        else
        {
            TString file_path = file_dir + "/" + file_name;
            if (file_name.EndsWith(".data"))
            {
                // parse run number from the current file name
                string strRunNumber = find_last_number(file_name.Data());
                if (strRunNumber == "")
                {
                    cout<<"WARNING: No run number was found in the file name: "<<file_path<<endl;
                    continue;
                }
                int run_number = stoi(strRunNumber);

                // get run from the Database
                UniDbRun* pRun = UniDbRun::GetRun(period_number, run_number);
                if (pRun == nullptr)
                {
                    Long64_t size = 0;
                    Long_t id = 0, flags = 0, modtime = 0;
                    gSystem->GetPathInfo(file_path.Data(), &id, &size, &flags, &modtime);

                    double dFileSize = size / 1024.0 / 1024.0;
                    if ((limit_size_in_mb != 0) && (dFileSize > limit_size_in_mb)) continue;

                    cout<<"ATTENTION: Run "<<period_number<<":"<<run_number<<" has not been found in the Database"
                       <<" ("<<file_path<<", "<<dFileSize<<" MB)"<<endl;
                    if (!isOnlyShow)
                    {
                        gSystem->Unlink(file_path);
                        cout<<"ATTENTION: The file has been deleted: "<<file_path<<endl<<endl;
                    }
                    del_count++;
                }
                else
                    delete pRun;
            }//if (file_name.EndsWith(".data"))
        }//if not a directory
    }//while file list

    if (del_count > 0)
    {
        if (isOnlyShow) cout<<endl<<del_count<<" files have been found which are absent in the Database (dir: "<<dir_name<<")"<<endl;
        else cout<<endl<<del_count<<" files have been deleted which are absent in the Database (dir: "<<dir_name<<")"<<endl;
    }

    return 0;
}
