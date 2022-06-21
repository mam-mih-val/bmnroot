// -------------------------------------------------------------------------
// -----                       ElogConnection header file               -----
// -------------------------------------------------------------------------

/** ElogConnection.h
 *@author K.Gertsenberger <gertsen@jinr.ru>
 **
 ** Class for connection to the experiment database: Electronic Logbook Database
 **/

#ifndef ELOGCONNECTION_H
#define ELOGCONNECTION_H 1

#include <TSQLServer.h>

#include "elog_db_settings.h"

#include <map>
#include <string>
#include <iostream>
using namespace std;

typedef map<string, TSQLServer*> mapSQLServer;
typedef pair<string, TSQLServer*> pairSQLServer;
typedef map<string, TSQLServer*>::iterator itSQLServer;

class ElogConnection : public TObject
{
  public:
    enum enumDBMS {MySQL, PgSQL};

    static ElogConnection* Open();
    static ElogConnection* Open(enumDBMS database_type, const char* strDBHost, const char* strDBName, const char* strUID, const char* strPassword);
    virtual ~ElogConnection(); // Destructor

    TSQLServer* GetSQLServer() { return elog_db_server; }

    static const char* DbToString() { return "ELOG_DB"; }

  protected:
    static mapSQLServer* mapConnection;

  private:
    TSQLServer* elog_db_server;
    ElogConnection(TSQLServer* pSQLServer);

  ClassDef(ElogConnection,2)
};

#endif
