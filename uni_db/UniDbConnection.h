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

enum UniConnectionType{UNIFIED_DB};

class UniDbConnection
{
 private:
    static const char* pcDBName;
    static const char* pcUID;
    static const char* pcPassword;
    TSQLServer* uni_db;

    UniDbConnection(TSQLServer* pSQLServer);

 public:
    virtual ~UniDbConnection(); // Destructor

    static UniDbConnection *Open(UniConnectionType database_type);
    static UniDbConnection* Open(TString strDBName, TString strUID, TString strPassword);

    TSQLServer* GetSQLServer(){return uni_db;}


 ClassDef(UniDbConnection,1)
};

#endif
