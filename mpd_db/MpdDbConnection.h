// -------------------------------------------------------------------------
// -----                      MpdDbConnection header file              -----
// -----                  Created 28/01/13  by K. Gertsenberger        -----
// -------------------------------------------------------------------------

/** MpdDbConnection.h
 *@author K.Gertsenberger <gertsen@jinr.ru>
 **
 ** Class for connection to MPD database on PGSQL
 **/

#ifndef MPDDBCONNECTION_H
#define MPDDBCONNECTION_H 1

#include <TSQLServer.h>

enum MpdConnectionType{UNIFIED_DB};

class MpdDbConnection
{
 private:
    static const char* pcDBName;
    static const char* pcUID;
    static const char* pcPassword;
    TSQLServer* mpd_db;

    MpdDbConnection(TSQLServer* pSQLServer);

 public:
    virtual ~MpdDbConnection(); // Destructor

    static MpdDbConnection *Open(MpdConnectionType database_type);
    static MpdDbConnection* Open(TString strDBName, TString strUID, TString strPassword);

    TSQLServer* GetSQLServer(){return mpd_db;}


 ClassDef(MpdDbConnection,1) //MPDDBCONNECTION
};

#endif
