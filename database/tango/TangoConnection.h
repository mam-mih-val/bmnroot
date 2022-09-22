// -------------------------------------------------------------------------
// -----                       TangoConnection header file               -----
// -------------------------------------------------------------------------

/** TangoConnection.h
 *@author K.Gertsenberger <gertsen@jinr.ru>
 **
 ** Class for connection to the experiment database: Tango slow control Database
 **/

#ifndef TANGOCONNECTION_H
#define TANGOCONNECTION_H 1

#include <TSQLServer.h>

#include "tango_db_settings.h"

#include <map>
#include <string>
#include <iostream>
using namespace std;

typedef map<string, TSQLServer*> mapSQLServer;
typedef pair<string, TSQLServer*> pairSQLServer;
typedef map<string, TSQLServer*>::iterator itSQLServer;

class TangoConnection
{
  public:
    enum enumDBMS {MySQL, PgSQL};

    static TangoConnection* Open();
    static TangoConnection* Open(enumDBMS database_type, const char* strDBHost, const char* strDBName, const char* strUID, const char* strPassword);
    virtual ~TangoConnection(); // Destructor

    TSQLServer* GetSQLServer() { return tango_db_server; }

    static const char* DbToString() { return "TANGO_DB"; }

  protected:
    static mapSQLServer* mapConnection;

  private:
    TSQLServer* tango_db_server;
    TangoConnection(TSQLServer* pSQLServer);

  ClassDef(TangoConnection,2)
};

#endif
