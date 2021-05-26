// function to calculate MD5 values for files whose paths stored in the Database
// returns the success code: 0 - without errors, error_code - otherwise
int write_md5_db(int period_number, bool isSkipExisted = true)
{
    UniqueRunNumber* run_numbers;
    int run_count = UniDbRunPeriod::GetRunNumbers(period_number, run_numbers), iter_count = 0;
    if (run_count <= 0)
    {
        cout<<"ERROR: runs do not exist in the Database for period "<<period_number<<endl;
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
        if ((isSkipExisted) && (pRun->GetFileMd5() != NULL)) continue;

        TString strFilePath = pRun->GetFilePath();
        gSystem->ExpandPathName(strFilePath);

        if (gSystem->AccessPathName(strFilePath.Data()))
        {
            isError = true;
            cout<<"ERROR: raw file does not exist: "<<strFilePath<<endl;
        }
        else
        {
            TMD5* pMD5 = TMD5::FileChecksum(strFilePath.Data());
            if (!pMD5)
            {
                isError = true;
                cout<<"ERROR: a problem occured while opening file: "<<strFilePath<<endl;
                continue;
            }
            cout<<"File: "<<strFilePath<<". MD5 = "<<pMD5->AsString()<<endl;
            TString strMD5(pMD5->AsString());
            int res_code = pRun->SetFileMd5(&strMD5);
            if (res_code != 0) isError = true;
            else iter_count++;
            delete pMD5;
        }
    }

    if (isError) cout<<endl<<"There are some errors occured!"<<endl;
    cout<<"Checksums were written for "<<iter_count<<" of "<<run_count<<" files"<<endl;

    return 0;
}

int write_sim_md5_db(bool isSkipExisted = true)
{
    TObjArray* simulation_files = UniDbSimulationFile::GetSimulationFiles();

    bool isError = false;
    int file_count = simulation_files->GetEntriesFast(), iter_count = 0;
    for (int i = 0; i < file_count; i++)
    {
        UniDbSimulationFile* pFile = (UniDbSimulationFile*) simulation_files->At(i);
        if ((isSkipExisted) && (pFile->GetFileMd5() != NULL)) continue;

        TString strFilePath = pFile->GetFilePath();
        gSystem->ExpandPathName(strFilePath);

        if (gSystem->AccessPathName(strFilePath.Data()))
        {
            isError = true;
            cout<<"ERROR: simulation file does not exist: "<<strFilePath<<endl;
        }
        else
        {
            TMD5* pMD5 = TMD5::FileChecksum(strFilePath.Data());
            if (!pMD5)
            {
                isError = true;
                cout<<"ERROR: a problem occured while opening file: "<<strFilePath<<endl;
                continue;
            }
            cout<<"File: "<<strFilePath<<". MD5 = "<<pMD5->AsString()<<endl;
            TString strMD5(pMD5->AsString());
            int res_code = pFile->SetFileMd5(&strMD5);
            if (res_code != 0) isError = true;
            else iter_count++;
            delete pMD5;
        }
    }

    // clean memory after work - deleting the array of simulation files
    simulation_files->Delete();
    delete simulation_files;

    if (isError) cout<<endl<<"There are some errors occured!"<<endl;
    cout<<"Checksums were written for "<<iter_count<<" of "<<file_count<<" files"<<endl;

    return 0;
}
