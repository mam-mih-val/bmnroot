#include "../../function_set.h"

// function to calculate MD5 values for files whose paths stored in the Database
// returns the success code: 0 - without errors, error_code - otherwise
int write_md5_db(int period_number)
{
    UniqueRunNumber* run_numbers;
    int run_count = UniDbRunPeriod::GetRunNumbers(period_number, run_numbers);
    if (run_count <= 0)
    {
        cout<<"ERROR: Runs do not exist in the Database for period "<<period_number<<endl;
        return -1;
    }

    bool isError = false;
    for (int i = 0; i < run_count; i++)
    {
        // get run time
        UniDbRun* pRun = UniDbRun::GetRun(run_numbers[i].period_number, run_numbers[i].run_number);
        if (pRun == NULL)
        {
            cout<<"The function encountered with errors: no experimental run was found ("<<run_numbers[i].period_number<<":"<<run_numbers[i].run_number<<"). This run will be skipped!"<<endl;
            isError = true;
            continue;
        }

        TString strFilePath = pRun->GetFilePath();
        gSystem->ExpandPathName(strFilePath);

        if (gSystem->AccessPathName(strFilePath.Data()))
        {
            isError = true;
            cout<<"ERROR: The file does not exist: "<<strFilePath<<endl;
        }
        else
        {
            UChar_t digest[16];
            TMD5::FileChecksum(strFilePath.Data(), digest);
            TString strFileMd5((char*)digest);
            int res_code = pRun->SetFileMd5(&strFileMd5);
            if (res_code != 0) isError = true;
        }
    }

    if (isError) cout<<endl<<"There are some errors occured!"<<endl;
    else cout<<endl<<"All checksum were written"<<endl;

    return 0;
}
