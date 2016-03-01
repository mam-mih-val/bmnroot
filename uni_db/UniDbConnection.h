// -------------------------------------------------------------------------
// -----                      UniDbConnection header file              -----
// -----                  Created 28/01/13 by K. Gertsenberger         -----
// -------------------------------------------------------------------------

/** UniDbConnection.h
 *@author K.Gertsenberger <gertsen@jinr.ru>
 **
 ** Class for connection to databases on PGSQL
 **/

#ifndef UNIDBCONNECTION_H
#define UNIDBCONNECTION_H 1

#include <TSQLServer.h>
#include "db_settings.h"

#include <map>
#include <string>
#include <iostream>
using namespace std;

enum UniConnectionType{UNIFIED_DB};

typedef map<string, TSQLServer*> mapSQLServer;
typedef pair<string, TSQLServer*> pairSQLServer;
typedef map<string, TSQLServer*>::iterator itSQLServer;

class UniDbConnection
{
 private:
    TSQLServer* uni_db;

    UniDbConnection(TSQLServer* pSQLServer);

 protected:
    static mapSQLServer* mapConnection;

 public:
    virtual ~UniDbConnection(); // Destructor

    static UniDbConnection* Open(UniConnectionType database_type);
    static UniDbConnection* Open(TString strDBName = "pgsql://nc13.jinr.ru/bmn_db", TString strUID = BMN_DB_USERNAME, TString strPassword = BMN_DB_PASSWORD);

    TSQLServer* GetSQLServer(){return uni_db;}


 ClassDef(UniDbConnection,1)
};

#endif
