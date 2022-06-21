// -------------------------------------------------------------------------
// -----                       ElogConnection source file               -----
// -------------------------------------------------------------------------
#include "ElogConnection.h"

mapSQLServer* ElogConnection::mapConnection = nullptr;

// -----   Constructor with connection   ----------------------
ElogConnection::ElogConnection(TSQLServer* pSQLServer)
{
    elog_db_server = pSQLServer;
}

// -------------------------------------------------------------------
ElogConnection* ElogConnection::Open()
{
    TString conString = TString::Format("pgsql://%s/%s", ELOG_DB_HOST, ELOG_DB_NAME);

    if (ElogConnection::mapConnection == nullptr)
        ElogConnection::mapConnection = new mapSQLServer();

    TSQLServer* pSQLServer = nullptr;
    itSQLServer it = ElogConnection::mapConnection->find(conString.Data());
    if (it != ElogConnection::mapConnection->end())
        pSQLServer = it->second;
    else
    {
        pSQLServer = TSQLServer::Connect(conString, ELOG_DB_USERNAME, ELOG_DB_PASSWORD);
        if (pSQLServer == nullptr)
        {
            cout<<"ERROR: database connection was not established ("<<ELOG_DB_HOST<<") for '"<<ELOG_DB_USERNAME<<"' user"<<endl;
            return nullptr;
        }
        //cout<<"Server info: "<<pSQLServer->ServerInfo()<<endl;

        ElogConnection::mapConnection->insert(pairSQLServer(conString.Data(), pSQLServer));
    }

    return new ElogConnection(pSQLServer);
}

// -------------------------------------------------------------------
ElogConnection* ElogConnection::Open(enumDBMS database_type, const char* strDBHost, const char* strDBName, const char* strUID, const char* strPassword)
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

    if (ElogConnection::mapConnection == nullptr)
        ElogConnection::mapConnection = new mapSQLServer();

    TSQLServer* pSQLServer = nullptr;
    itSQLServer it = ElogConnection::mapConnection->find(conString.Data());
    if (it != ElogConnection::mapConnection->end())
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

        ElogConnection::mapConnection->insert(pairSQLServer(conString.Data(), pSQLServer));
    }

    return new ElogConnection(pSQLServer);
}

// -------------------------------------------------------------------
ElogConnection::~ElogConnection()
{
    //if (elog_db_server)
    //    delete elog_db_server;
}

// -------------------------------------------------------------------
ClassImp(ElogConnection);
