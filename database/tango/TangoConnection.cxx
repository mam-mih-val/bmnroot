// -------------------------------------------------------------------------
// -----                       TangoConnection source file               -----
// -------------------------------------------------------------------------
#include "TangoConnection.h"

mapSQLServer* TangoConnection::mapConnection = nullptr;

// -----   Constructor with connection   ----------------------
TangoConnection::TangoConnection(TSQLServer* pSQLServer)
{
    tango_db_server = pSQLServer;
}

// -------------------------------------------------------------------
TangoConnection* TangoConnection::Open()
{
    TString conString = TString::Format("pgsql://%s/%s", TANGO_DB_HOST, TANGO_DB_NAME);

    if (TangoConnection::mapConnection == nullptr)
        TangoConnection::mapConnection = new mapSQLServer();

    TSQLServer* pSQLServer = nullptr;
    itSQLServer it = TangoConnection::mapConnection->find(conString.Data());
    if (it != TangoConnection::mapConnection->end())
        pSQLServer = it->second;
    else
    {
        pSQLServer = TSQLServer::Connect(conString, TANGO_DB_USERNAME, TANGO_DB_PASSWORD);
        if (pSQLServer == nullptr)
        {
            cout<<"ERROR: database connection was not established ("<<TANGO_DB_HOST<<") for '"<<TANGO_DB_USERNAME<<"' user"<<endl;
            return nullptr;
        }
        //cout<<"Server info: "<<pSQLServer->ServerInfo()<<endl;

        TangoConnection::mapConnection->insert(pairSQLServer(conString.Data(), pSQLServer));
    }

    return new TangoConnection(pSQLServer);
}

// -------------------------------------------------------------------
TangoConnection* TangoConnection::Open(enumDBMS database_type, const char* strDBHost, const char* strDBName, const char* strUID, const char* strPassword)
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

    if (TangoConnection::mapConnection == nullptr)
        TangoConnection::mapConnection = new mapSQLServer();

    TSQLServer* pSQLServer = nullptr;
    itSQLServer it = TangoConnection::mapConnection->find(conString.Data());
    if (it != TangoConnection::mapConnection->end())
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

        TangoConnection::mapConnection->insert(pairSQLServer(conString.Data(), pSQLServer));
    }

    return new TangoConnection(pSQLServer);
}

// -------------------------------------------------------------------
TangoConnection::~TangoConnection()
{
    //if (tango_db_server)
    //    delete tango_db_server;
}

// -------------------------------------------------------------------
ClassImp(TangoConnection);
