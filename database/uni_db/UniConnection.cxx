// -------------------------------------------------------------------------
// -----                       UniConnection source file               -----
// -------------------------------------------------------------------------
#include "UniConnection.h"

mapSQLServer* UniConnection::mapConnection = nullptr;

// -----   Constructor with connection   ----------------------
UniConnection::UniConnection(TSQLServer* pSQLServer)
{
    db_server = pSQLServer;
}

// -------------------------------------------------------------------
UniConnection* UniConnection::Open()
{
    TString conString = TString::Format("pgsql://%s/%s", UNI_DB_HOST, UNI_DB_NAME);

    if (UniConnection::mapConnection == nullptr)
        UniConnection::mapConnection = new mapSQLServer();

    TSQLServer* pSQLServer = nullptr;
    itSQLServer it = UniConnection::mapConnection->find(conString.Data());
    if (it != UniConnection::mapConnection->end())
        pSQLServer = it->second;
    else
    {
        pSQLServer = TSQLServer::Connect(conString, UNI_DB_USERNAME, UNI_DB_PASSWORD);
        if (pSQLServer == nullptr)
        {
            cout<<"ERROR: database connection was not established ("<<UNI_DB_HOST<<") for '"<<UNI_DB_USERNAME<<"' user"<<endl;
            return nullptr;
        }
        //cout<<"Server info: "<<pSQLServer->ServerInfo()<<endl;

        UniConnection::mapConnection->insert(pairSQLServer(conString.Data(), pSQLServer));
    }

    return new UniConnection(pSQLServer);
}

// -------------------------------------------------------------------
UniConnection* UniConnection::Open(enumDBMS database_type, const char* strDBHost, const char* strDBName, const char* strUID, const char* strPassword)
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
                return nullptr;
            }
    }
    TString conString = TString::Format("%s://%s/%s", db_type, strDBHost, strDBName);

    if (UniConnection::mapConnection == nullptr)
        UniConnection::mapConnection = new mapSQLServer();

    TSQLServer* pSQLServer = nullptr;
    itSQLServer it = UniConnection::mapConnection->find(conString.Data());
    if (it != UniConnection::mapConnection->end())
    {
        pSQLServer = it->second;
    }
    else
    {
        pSQLServer = TSQLServer::Connect(conString, strUID, strPassword);
        if (pSQLServer == nullptr)
        {
            cout<<"ERROR: database connection was not established ("<<strDBHost<<") for '"<<strUID<<"' user"<<endl;
            return nullptr;
        }
        //cout<<"Server info: "<<pSQLServer->ServerInfo()<<endl;

        UniConnection::mapConnection->insert(pairSQLServer(conString.Data(), pSQLServer));
    }

    return new UniConnection(pSQLServer);
}

// -------------------------------------------------------------------
UniConnection::~UniConnection()
{
    //if (db_server)
    //    delete db_server;
}

// -------------------------------------------------------------------
ClassImp(UniConnection);
