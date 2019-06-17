void test_db()
{
    TStopwatch timer;
    timer.Start();
    gDebug = 0;

    UniDbConnection* connectionUniDb = UniDbConnection::Open(UNIFIED_DB);
    if (connectionUniDb == 0x00)
        return;

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
        cout<<"ERROR: parameter with number 1 wasn't found!"<<endl;
        delete stmt;
        return;
    }

    TString parameter_name = stmt->GetString(0);
    delete stmt;
    delete connectionUniDb;

    timer.Stop();
    Double_t rtime = timer.RealTime(), ctime = timer.CpuTime();
    printf("RealTime=%f seconds, CpuTime=%f seconds\n", rtime, ctime);

    cout<<"Test was successfull"<<endl;
}

void test_db(TString strDbHost, TString strDbName, TString strUserName, TString strDbPassword)
{
    TString conString = TString::Format("pgsql://%s/%s", strDbHost.Data(), strDbName.Data());
    TSQLServer* pSQLServer =TSQLServer::Connect(conString, strUserName.Data(), strDbPassword.Data());
    if (pSQLServer == 0x00)
    {
        cout<<"ERROR: database connection was not established (m.b. wrong login or password)"<<endl;
        return 0x00;
    }
    cout<<"Server info: "<<pSQLServer->ServerInfo()<<endl;
    cout<<"DB: "<<pSQLServer->GetDB()<<". Type: "<<pSQLServer->GetDBMS()<<"."<<endl;
}
