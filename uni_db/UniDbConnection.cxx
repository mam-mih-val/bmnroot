// -------------------------------------------------------------------------
// -----                      UniDbConnection source file              -----
// -----                  Created 28/01/13 by K. Gertsenberger         -----
// -------------------------------------------------------------------------
#include "UniDbConnection.h"

mapSQLServer* UniDbConnection::mapConnection = 0x00;

// -----   Constructor with connection   ----------------------
UniDbConnection::UniDbConnection(TSQLServer* pSQLServer)
{
    server_db = pSQLServer;
}

// -------------------------------------------------------------------
UniDbConnection* UniDbConnection::Open(UniConnectionType connection_type)
{
    TString conString = "";
    switch (connection_type)
    {
        case UNIFIED_DB:
            conString = TString::Format("pgsql://%s/%s", UNI_DB_HOST, UNI_DB_NAME);
            break;
        case TANGO_DB:
            conString = TString::Format("mysql://%s/%s", TANGO_DB_HOST, TANGO_DB_NAME);
            break;
        case ELOG_DB:
            conString = TString::Format("pgsql://%s/%s", ELOG_DB_HOST, ELOG_DB_NAME);
            break;
        default:
            {
                cout<<"ERROR: incorrect database connection type!"<<endl;
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
        switch (connection_type)
        {
            case UNIFIED_DB:
                pSQLServer = TSQLServer::Connect(conString, UNI_DB_USERNAME, UNI_DB_PASSWORD);
                break;
            case TANGO_DB:
                pSQLServer = TSQLServer::Connect(conString, TANGO_DB_USERNAME, TANGO_DB_PASSWORD);
                break;
            case ELOG_DB:
                pSQLServer = TSQLServer::Connect(conString, ELOG_DB_USERNAME, ELOG_DB_PASSWORD);
                break;
        }
        if (pSQLServer == 0x00)
        {
            cout<<"ERROR: database connection was not established (m.b. wrong login or password)"<<endl;
            return 0x00;
        }
        //cout<<"Server info: "<<pSQLServer->ServerInfo()<<endl;

        UniDbConnection::mapConnection->insert(pairSQLServer(conString.Data(), pSQLServer));
    }

    return new UniDbConnection(pSQLServer);
}

// -------------------------------------------------------------------
UniDbConnection* UniDbConnection::Open(enumDBMS database_type, const char* strDBHost, const char* strDBName, const char* strUID, const char* strPassword)
{
    char* db_type;
    switch (database_type)
    {
        case MySQL:
            db_type = (char*)"mysql";
            break;
        case PgSQL:
            db_type = (char*)"pgsql";
            break;
        default:
            {
                cout<<"ERROR: incorrect database type!"<<endl;
                return 0x00;
            }
    }
    TString conString = TString::Format("%s://%s/%s", db_type, strDBHost, strDBName);

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
            cout<<"ERROR: database connection was not established (m.b. wrong login or password)"<<endl;
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
