void test_db()
{
    TStopwatch timer;
    timer.Start();
    gDebug = 0;

    gROOT->LoadMacro("$VMCWORKDIR/macro/mpd/mpdloadlibs.C");
    mpdloadlibs(1, 1); // load main libraries

    MpdDbConnection* connectionUniDb = MpdDbConnection::Open(0);

    if (connectionUniDb == 0x00)
    {
        cout<<"Error: connection to the database can't be established"<<endl;
        return -1;
    }

    TSQLServer* uni_db = connectionUniDb->GetSQLServer();

    cout<<"DB: "<<uni_db->GetDB()<<". Type: "<<uni_db->GetDBMS()<<"."<<endl;

    int iSessionNumber = 1;
    TSQLStatement* stmt = uni_db->Statement(TString::Format(
                                            "select start_datetime "
                                            "from session_ "
                                            "where session_number = %d", iSessionNumber));

    stmt->Process();
    // store result of statement in buffer
    stmt->StoreResult();

    // extract rows one after another
    if (!stmt->NextResultRow())
    {
        cout<<"1: session wasn't found!"<<endl;

        delete stmt;
        return;
    }


    TDatime start_date = stmt->GetDatime(0);

    delete stmt;
    delete connectionUniDb;

    cout<<"Start date: "<<start_date.AsSQLString()<<endl;

    timer.Stop();
    Double_t rtime = timer.RealTime(), ctime = timer.CpuTime();
    printf("RealTime=%f seconds, CpuTime=%f seconds\n", rtime, ctime);

    //cout << "Macro finished succesfully." << endl;
}
