// macro for adding file IDs defining files with corresponding TOF-700 slewing
// it reads a file with file list storing run numbers prepared by Yury Petukhov

struct RunPair
{
    int iStartRun;
    int iEndRun;
    RunPair(int start_run, int end_run) { iStartRun = start_run; iEndRun = end_run; }
};
ostream& operator<<(ostream& stream, const RunPair& obj)
{
    stream<<obj.iStartRun<<"-"<<obj.iEndRun;
    return stream;
}

void add_slewing_file_id(TString strFileList, int file_id, int period_number = 7)
{
    fstream fList(strFileList, std::fstream::in);
    if (fList.fail())
    {
        cout<<"Failed to open "<<strFileList<<endl;
        return;
    }

    TString strCurrentFile;
    int number;
    vector<int> vecRuns;
    while (!fList.eof())
    {
        fList >> strCurrentFile;
        if (strCurrentFile[0] == 0x00) continue;
        cout<<"Current file: "<<strCurrentFile<<endl;

        // open current file
        fstream fFile(strCurrentFile.Data(), std::fstream::in);
        if (fFile.fail())
        {
            cout<<"Failed to open "<<strCurrentFile<<endl;
            return;
        }

        while (!fFile.eof())
        {
            fFile >> number;
            if (number < 0)
            {
                cout<<"Numbers should be positive: "<<number<<". Something goes wrong. Exiting..."<<endl;
                return;
            }
            vecRuns.push_back(number);
        }
        fFile.close();
    }
    fList.close(); cout<<endl;

    cout<<"Vector before sorting:"<<endl; copy(vecRuns.begin(), vecRuns.end(), ostream_iterator<int>(cout, " ")); cout<<endl<<endl;
    sort(vecRuns.begin(), vecRuns.end());
    cout<<"Vector after sorting:"<<endl; copy(vecRuns.begin(), vecRuns.end(), ostream_iterator<int>(cout, " ")); cout<<endl<<endl;

    if (vecRuns. size() < 1)
    {
        cout<<"There are no numbers in output vector. Exiting..."<<endl;
        return;
    }

    //generate run intervals
    vector<RunPair> vecRunPairs;
    vector<int>::iterator it = vecRuns.begin();
    int fix_number = *it, prev_number = *it, cur_number;
    for (it++; it != vecRuns.end(); it++)
    {
        cur_number = *it;
        if (cur_number != (prev_number+1))
        {
            RunPair new_pair(fix_number, prev_number);
            vecRunPairs.push_back(new_pair);
            fix_number = cur_number;
        }

        prev_number = cur_number;
    }
    RunPair new_pair(fix_number, prev_number);
    vecRunPairs.push_back(new_pair);
    cout<<"Final Run Intervals:"<<endl; copy(vecRunPairs.begin(), vecRunPairs.end(), ostream_iterator<RunPair>(cout, " ")); cout<<endl<<endl;

    // create parameter in the Database (only once)
    /*UniDbParameter* pParameter = UniDbParameter::CreateParameter("slewing_file_id", IntType);
    if (pParameter == NULL)
    {
        cout << "\nMacro finished with errors" << endl;
        return;
    }
    // clean memory after work
    delete pParameter;*/

    // write parameter values to the Database
    cout<<"Writing file ID ("<<file_id<<") to the Database... ";
    for (vector<RunPair>::iterator it = vecRunPairs.begin(); it != vecRunPairs.end(); it++)
    {
        IntValue iValue; iValue.value = file_id;
        UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter("TOF2", "slewing_file_id", period_number, (*it).iStartRun, period_number, (*it).iEndRun, &iValue);
        if (pDetectorParameter == NULL)
        {
            cout<<"Macro finished with errors"<<endl;
            return;
        }
        // clean memory after work
        delete pDetectorParameter;
    }

    cout<<"Macro finished successfully"<<endl;
}
