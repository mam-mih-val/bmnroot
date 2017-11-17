// -------------------------------------------------------------------------
// -----                      UniDbConnection header file              -----
// -----                  Created 28/01/13 by K. Gertsenberger         -----
// -------------------------------------------------------------------------

/** UniDbConnection.h
 *@author K.Gertsenberger <gertsen@jinr.ru>
 **
 ** Class for connection to BM@N and MPD databases
 **/

#ifndef UNIDBCONNECTION_H
#define UNIDBCONNECTION_H 1

#include <TSQLServer.h>
#include "db_settings.h"

#include <map>
#include <string>
#include <iostream>
using namespace std;

enum UniDbType{MYSQL_DB, POSTGRESQL_DB};
enum UniConnectionType{UNIFIED_DB, TANGO_DB, ELOG_DB};

typedef map<string, TSQLServer*> mapSQLServer;
typedef pair<string, TSQLServer*> pairSQLServer;
typedef map<string, TSQLServer*>::iterator itSQLServer;

class UniDbConnection
{
 private:
    TSQLServer* server_db;

    UniDbConnection(TSQLServer* pSQLServer);

 protected:
    static mapSQLServer* mapConnection;

 public:
    virtual ~UniDbConnection(); // Destructor

    static UniDbConnection* Open(UniConnectionType connection_type);
    static UniDbConnection* Open(UniDbType database_type, const char* strDBHost, const char* strDBName, const char* strUID, const char* strPassword);

    TSQLServer* GetSQLServer() {return server_db;}

 ClassDef(UniDbConnection,1)
};

#endif
