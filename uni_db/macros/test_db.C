void test_db()
{
    TStopwatch timer;
    timer.Start();
    gDebug = 0;

    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load main libraries

    UniDbConnection* connectionUniDb = UniDbConnection::Open(0);

    if (connectionUniDb == 0x00)
    {
        cout<<"Error: connection to the database can't be established"<<endl;
        return -1;
    }

    TSQLServer* uni_db = connectionUniDb->GetSQLServer();

    cout<<"DB: "<<uni_db->GetDB()<<". Type: "<<uni_db->GetDBMS()<<"."<<endl;

    int iParameterID = 1;
    TSQLStatement* stmt = uni_db->Statement(TString::Format(
                                            "select parameter_name "
                                            "from parameter_ "
                                            "where parameter_id = %d", iParameterID));

    stmt->Process();
    // store result of statement in buffer
    stmt->StoreResult();

    // extract rows one after another
    if (!stmt->NextResultRow())
    {
        cout<<"Error: parameter with number 1 wasn't found!"<<endl;

        delete stmt;
        return;
    }


    TString parameter_name = stmt->GetString(0);

    delete stmt;
    delete connectionUniDb;

    cout<<"Parameter name: "<<parameter_name<<endl;

    timer.Stop();
    Double_t rtime = timer.RealTime(), ctime = timer.CpuTime();
    printf("RealTime=%f seconds, CpuTime=%f seconds\n", rtime, ctime);
}
