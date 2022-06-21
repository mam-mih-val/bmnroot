void test_db()
{
    TStopwatch timer;
    timer.Start();
    gDebug = 0;

    UniConnection* connDb = UniConnection::Open();
    if (connectionUniDb == nullptr)
        return;

    TSQLServer* db_server = connectionUniDb->GetSQLServer();

    cout<<"Host: "<<uni_db->GetHost()<<". DB: "<<uni_db->GetDB()<<". Type: "<<uni_db->GetDBMS()<<"."<<endl;

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
