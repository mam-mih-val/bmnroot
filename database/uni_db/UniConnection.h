// -------------------------------------------------------------------------
// -----                       UniConnection header file               -----
// -------------------------------------------------------------------------

/** UniConnection.h
 *@author K.Gertsenberger <gertsen@jinr.ru>
 **
 ** Class for connection to the experiment database: Unified Database
 **/

#ifndef UNICONNECTION_H
#define UNICONNECTION_H 1

#include <TSQLServer.h>

#include "uni_db_settings.h"

#include <map>
#include <string>
#include <iostream>
using namespace std;

typedef map<string, TSQLServer*> mapSQLServer;
typedef pair<string, TSQLServer*> pairSQLServer;
typedef map<string, TSQLServer*>::iterator itSQLServer;

class UniConnection : public TObject
{
  public:
    enum enumDBMS {MySQL, PgSQL};

    static UniConnection* Open();
    static UniConnection* Open(enumDBMS database_type, const char* strDBHost, const char* strDBName, const char* strUID, const char* strPassword);
    virtual ~UniConnection(); // Destructor

    TSQLServer* GetSQLServer() { return db_server; }

    static const char* DbToString() { return "UNIFIED_DB"; }

  protected:
    static mapSQLServer* mapConnection;

  private:
    TSQLServer* db_server;
    UniConnection(TSQLServer* pSQLServer);


  ClassDef(UniConnection,2)
};

#endif
