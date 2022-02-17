R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "database/unicom/function_set.h"

// function for walking and checking ROOT files in a given directory (and all subdirectories),
// whether their event count is corresponding the number in BM@N Database
// returns the success code: 0 - without errors, error_code - otherwise
int check_event_count(TString dir_name, int period_number, double tolerance_percent = 10)
{
    gSystem->ExpandPathName(dir_name);
    TSystemDirectory dir(dir_name.Data(), dir_name.Data());
    TList* file_list = dir.GetListOfFiles();
    if (file_list == NULL)
    {
        cout<<"WARNING: There are no files in the directory: "<<dir_name<<endl;
        return 0;
    }

    int find_count = 0, total_count = 0;
    TSystemFile* cur_file;
    TIter iterFile(file_list);
    while ((cur_file = (TSystemFile*)iterFile.Next()))
    {
        TString file_name = cur_file->GetName();
        if ((file_name == ".") || (file_name == "..")) continue;

        TString file_dir = cur_file->GetTitle();
        if (cur_file->IsDirectory())
        {
            check_event_count(file_dir, period_number, tolerance_percent);
        }// if directory
        else
        {
            TString file_path = file_dir + "/" + file_name;
            if (file_name.EndsWith(".root"))
            {
                total_count++;

                // parse run number from the current file name
                string strRunNumber = find_last_number(file_name.Data());
                if (strRunNumber == "")
                {
                    cout<<"WARNING: No run number was found in the file name: "<<file_path<<endl;
                    continue;
                }
                int run_number = stoi(strRunNumber);

                // get number of entries (events) in the file
                int file_event_count = MpdGetNumEvents::GetNumROOTEvents(file_path.Data(),"",0);

                // get run from the Database
                UniDbRun* pRun = UniDbRun::GetRun(period_number, run_number);
                if (pRun != nullptr)
                {
                    int* p_db_event_count = pRun->GetEventCount();
                    if (p_db_event_count == nullptr)
                    {
                        cout<<"WARNING: The event count for run number "<<period_number<<":"<<run_number<<" is not stored in the database"<<endl;
                        delete pRun;
                        continue;
                    }
                    int db_event_count = *p_db_event_count;

                    if (TMath::Abs(db_event_count-file_event_count) > tolerance_percent/100.0*db_event_count)
                    {
                        cout<<"ATTENTION: The event count in the file ("<<file_path<<") = "<<file_event_count<<", but in BM@N database = "<<db_event_count<<endl;
                        find_count++;
                    }
                    delete pRun;
                }
                else
                    cout<<"WARNING: The run number "<<period_number<<":"<<run_number<<" was not found in the database for the file: "<<file_path<<endl;
            }//if (file_name.EndsWith(".data"))
        }//if not a directory
    }//while file list

    if (find_count > 0)
        cout<<endl<<find_count<<" files (from "<<total_count<<") have event count being different from the value in the Database (dir: "<<dir_name<<")"<<endl;
    else
        cout<<endl<<"There are no deviations in event count for "<<total_count<<" processed files (dir: "<<dir_name<<")"<<endl;

    return 0;
}
