// macro for writing links to the existed ROOT setup geometry for a selecting run period to the Unified Database
// in case of geometry_link is -1 - get geometry from the previous run
// e.g. root 'write_geometry_to_db.C(45, 7, 1, 7, 10000)'
void write_geometry_to_db(int geometry_link, int start_period, int start_run, int end_period, int end_run)
{
    gSystem->Load("libUniDb");

    if (geometry_link < -1)
    {
        cout<<"\nERROR: geometry link should be positive or be equal -1 to get geometry from the previous run"<<endl;
        return;
    }
    if (geometry_link == -1)
    {
        UniqueRunNumber* pPreviousRunNumber = UniDbRun::FindPreviousRun(start_period, start_run);
        if (pPreviousRunNumber == NULL)
            return;

        UniDbRun* pPreviosRun = UniDbRun::GetRun(pPreviousRunNumber->period_number, pPreviousRunNumber->run_number);
        if (pPreviosRun == NULL)
        {
            cout<<"ERROR: getting of previous run "<<pPreviousRunNumber->period_number<<":"<<pPreviousRunNumber->run_number<<" (period:number) was failed"<<endl;
            delete pPreviousRunNumber;

            return;
        }
        delete pPreviousRunNumber;

        if (pPreviosRun->GetGeometryId() == NULL)
        {
            cout<<"ERROR: no geometry id was found in the prevoius run "<<pPreviousRunNumber->period_number<<":"<<pPreviousRunNumber->run_number<<" (period:number)"<<endl;
            delete pPreviosRun;
            return;
        }
        geometry_link = *(pPreviosRun->GetGeometryId());

        delete pPreviosRun;
    }

    UniqueRunNumber* pUniqueRuns = NULL;
    int run_count = UniDbRun::GetRunNumbers(start_period, start_run, end_period, end_run, pUniqueRuns);
    if (run_count < 0)
    {
        cout<<"\nERROR: No runs were found in the given range"<<endl;
        return;
    }
    int res_code = 0;
    for (int i = 0; i < run_count; i++)
    {
        cout<<"Setting link to geometry for run_period:run   "<<pUniqueRuns[i].period_number<<":"<<pUniqueRuns[i].run_number<<"..."<<endl;

        UniDbRun* pCurRun = UniDbRun::GetRun(pUniqueRuns[i].period_number, pUniqueRuns[i].run_number);
        if (pCurRun == NULL)
        {
            cout<<"ERROR: getting of run "<<pUniqueRuns[i].period_number<<":"<<pUniqueRuns[i].run_number<<" (period:number) was failed"<<endl;
            res_code = -1;
            continue;
        }

        pCurRun->SetGeometryId(new int(geometry_link));

        delete pCurRun;
    }

    delete [] pUniqueRuns;
    if (res_code != 0)
    {
        cout<<"\nMacro finished with errors"<<endl;
        return;;
    }

    cout<<"\nMacro finished successfully"<<endl;
}

// macro for creating and writing ROOT file with full setup geometry to the Unified Database and creating links for the given runs (from start to end run)
// e.g. root 'write_geometry_to_db.C("$VMCWORKDIR/macro/run/geometry_run/geometry_src_run7.root", 7, 1, 7, 10000)'
void write_geometry_to_db(const char* root_file_path, int start_period, int start_run, int end_period, int end_run)
{
    gSystem->Load("libUniDb");

    // write ROOT file with detector geometry for run range
    int res_code = UniDbRun::WriteGeometryFile(start_period, start_run, end_period, end_run, root_file_path); //(int start_period, int start_run, int end_period, int end_run, char* geo_file_path)
    if (res_code != 0)
    {
        cout<<"\nMacro finished with errors"<<endl;
        exit(-1);
    }

    cout<<"\nMacro finished successfully"<<endl;
}
