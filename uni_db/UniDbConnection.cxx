// -------------------------------------------------------------------------
// -----                      UniDbConnection header file              -----
// -----                  Created 28/01/13 by K. Gertsenberger         -----
// -------------------------------------------------------------------------
#include "UniDbConnection.h"
#include "db_settings.h"

#include <iostream>
using namespace std;

// -----   Constructor with connection   ----------------------
UniDbConnection::UniDbConnection(TSQLServer* pSQLServer)
{
    uni_db = pSQLServer;
}

// -------------------------------------------------------------------
UniDbConnection* UniDbConnection::Open(UniConnectionType database_type)
{
    TSQLServer* pSQLServer = 0x00;

    switch (database_type)
    {
        case UNIFIED_DB:
            pSQLServer = TSQLServer::Connect("pgsql://nc13.jinr.ru/bmn_db", BMN_DB_USERNAME, BMN_DB_PASSWORD);
            break;
        default:
            cout<<"Incorrect database connection type!"<<endl;
    }

    if (pSQLServer == 0x00)
    {
        cout<<"Connection wasn't established"<<endl;
        return 0x00;
    }
    //else
    //    cout<<"Server info: "<<pSQLServer->ServerInfo()<<endl;

    return new UniDbConnection(pSQLServer);
}

// -------------------------------------------------------------------
UniDbConnection* UniDbConnection::Open(TString strDBName, TString strUID, TString strPassword)
{
    TSQLServer* pSQLServer = TSQLServer::Connect(strDBName, strUID, strPassword);
    if (pSQLServer == 0x00)
    {
        cout<<"Connection wasn't established"<<endl;
        return 0x00;
    }
    else
        cout<<"Server info: "<<pSQLServer->ServerInfo()<<endl;

    return new UniDbConnection(pSQLServer);
}

// -------------------------------------------------------------------
UniDbConnection::~UniDbConnection()
{
    if (uni_db)
        delete uni_db;
}

// -------------------------------------------------------------------
ClassImp(UniDbConnection);
