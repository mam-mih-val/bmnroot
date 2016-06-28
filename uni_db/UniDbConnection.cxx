// -------------------------------------------------------------------------
// -----                      UniDbConnection header file              -----
// -----                  Created 28/01/13 by K. Gertsenberger         -----
// -------------------------------------------------------------------------
#include "UniDbConnection.h"

mapSQLServer* UniDbConnection::mapConnection = 0x00;

// -----   Constructor with connection   ----------------------
UniDbConnection::UniDbConnection(TSQLServer* pSQLServer)
{
    uni_db = pSQLServer;
}

// -------------------------------------------------------------------
UniDbConnection* UniDbConnection::Open(UniConnectionType database_type)
{
    TString conString = "";
    switch (database_type)
    {
        case UNIFIED_DB:
            conString = "pgsql://nc13.jinr.ru/" + (TString)UNI_DB_NAME;
            break;
        default:
            {
                cout<<"Error: incorrect database connection type!"<<endl;
                return 0x00;
            }
    }

    if (UniDbConnection::mapConnection == NULL)
        UniDbConnection::mapConnection = new mapSQLServer();

    TSQLServer* pSQLServer = 0x00;
    itSQLServer it = UniDbConnection::mapConnection->find(conString.Data());
    if (it != UniDbConnection::mapConnection->end())
    {
        pSQLServer = it->second;
    }
    else
    {
        pSQLServer = TSQLServer::Connect(conString, UNI_DB_USERNAME, UNI_DB_PASSWORD);
        if (pSQLServer == 0x00)
        {
            cout<<"Error: connection wasn't established"<<endl;
            return 0x00;
        }
        //cout<<"Server info: "<<pSQLServer->ServerInfo()<<endl;

        UniDbConnection::mapConnection->insert(pairSQLServer(conString.Data(), pSQLServer));
    }

    return new UniDbConnection(pSQLServer);
}

// -------------------------------------------------------------------
UniDbConnection* UniDbConnection::Open(TString strDBConn, TString strDBName, TString strUID, TString strPassword)
{
    TString conString = strDBConn + strDBName;

    if (UniDbConnection::mapConnection == NULL)
        UniDbConnection::mapConnection = new mapSQLServer();

    TSQLServer* pSQLServer = 0x00;
    itSQLServer it = UniDbConnection::mapConnection->find(conString.Data());
    if (it != UniDbConnection::mapConnection->end())
    {
        pSQLServer = it->second;
    }
    else
    {
        pSQLServer = TSQLServer::Connect(conString, strUID, strPassword);
        if (pSQLServer == 0x00)
        {
            cout<<"Error: connection wasn't established"<<endl;
            return 0x00;
        }
        //cout<<"Server info: "<<pSQLServer->ServerInfo()<<endl;

        UniDbConnection::mapConnection->insert(pairSQLServer(conString.Data(), pSQLServer));
    }

    return new UniDbConnection(pSQLServer);
}

// -------------------------------------------------------------------
UniDbConnection::~UniDbConnection()
{
    //if (uni_db)
    //    delete uni_db;
}

// -------------------------------------------------------------------
ClassImp(UniDbConnection);
