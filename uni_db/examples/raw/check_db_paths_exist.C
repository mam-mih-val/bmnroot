#include "../../function_set.h"

// function for checking existence of the files whose paths stored in the Database
// returns the success code: 0 - without errors, error_code - otherwise
int check_db_paths_exist(int period_number)
{
    UniqueRunNumber* run_numbers;
    int run_count = UniDbRunPeriod::GetRunNumbers(period_number, run_numbers);
    if (run_count <= 0)
    {
        cout<<"ERROR: Runs do not exist in the Database for period "<<period_number<<endl;
        return -1;
    }

    bool isAllIn = true;
    for (int i = 0; i < run_count; i++)
    {
        // get run time
        UniDbRun* pRun = UniDbRun::GetRun(run_numbers[i].period_number, run_numbers[i].run_number);
        if (pRun == NULL)
        {
            cout<<"The function encountered with errors: no experimental run was found ("<<run_numbers[i].period_number<<":"<<run_numbers[i].run_number<<"). This run will be skipped!"<<endl;
            continue;
        }

        TString strFilePath = pRun->GetFilePath();
        gSystem->ExpandPathName(strFilePath);

        if (gSystem->AccessPathName(strFilePath.Data()))
        {
            isAllIn = false;
            cout<<"ATTENTION: The file does not exist: "<<strFilePath<<endl;
        }
    }

    if (isAllIn) cout<<endl<<"All run paths exist"<<endl;
    else cout<<endl<<"There are absent run paths!"<<endl;

    return 0;
}
