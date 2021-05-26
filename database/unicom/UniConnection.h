// -------------------------------------------------------------------------
// -----                       UniConnection header file               -----
// -----                  Created 28/01/13 by K. Gertsenberger         -----
// -------------------------------------------------------------------------

/** UniConnection.h
 *@author K.Gertsenberger <gertsen@jinr.ru>
 **
 ** Class for connection to the experiment databases: Unified Database, e-Log Database, Tango Database
 **/

#ifndef UNICONNECTION_H
#define UNICONNECTION_H 1

#include <TSQLServer.h>
#include "db_settings.h"

#include <map>
#include <string>
#include <iostream>
using namespace std;

enum enumDBMS{MySQL, PgSQL};
enum UniConnectionType{UNIFIED_DB, TANGO_DB, ELOG_DB};

typedef map<string, TSQLServer*> mapSQLServer;
typedef pair<string, TSQLServer*> pairSQLServer;
typedef map<string, TSQLServer*>::iterator itSQLServer;

class UniConnection
{
 private:
    TSQLServer* server_db;

    UniConnection(TSQLServer* pSQLServer);

 protected:
    static mapSQLServer* mapConnection;

 public:
    virtual ~UniConnection(); // Destructor

    static UniConnection* Open(UniConnectionType connection_type);
    static UniConnection* Open(enumDBMS database_type, const char* strDBHost, const char* strDBName, const char* strUID, const char* strPassword);

    TSQLServer* GetSQLServer() {return server_db;}

    static const char* DbToString(UniConnectionType v)
    {
        switch (v)
        {
            case UNIFIED_DB:    return "UNIFIED_DB";
            case TANGO_DB:      return "TANGO_DB";
            case ELOG_DB:       return "ELOG_DB";
            default:            return "[Unknown DB type]";
        }
    }

 ClassDef(UniConnection,1)
};

#endif
