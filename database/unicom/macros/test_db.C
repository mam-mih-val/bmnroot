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
