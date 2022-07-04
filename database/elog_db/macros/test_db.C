void test_db(TString strDbHost, TString strDbName, TString strUserName, TString strDbPassword)
{
    TString conString = TString::Format("pgsql://%s/%s", strDbHost.Data(), strDbName.Data());
    TSQLServer* pSQLServer =TSQLServer::Connect(conString, strUserName.Data(), strDbPassword.Data());
    if (pSQLServer == nullptr)
    {
        cout<<"ERROR: database connection was not established ("<<strDbHost<<") for '"<<strUserName<<"' user"<<endl;
        return nullptr;
    }
    cout<<"Server info: "<<pSQLServer->ServerInfo()<<endl;
    cout<<"DB: "<<pSQLServer->GetDB()<<". Type: "<<pSQLServer->GetDBMS()<<"."<<endl;
}
