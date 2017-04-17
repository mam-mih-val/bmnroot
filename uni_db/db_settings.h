#ifndef DB_SETTINGS_H
#define DB_SETTINGS_H

//please, don't forget to recompile after changing of these parameters
const char* const UNI_DB_USERNAME = "db_detector";
const char* const UNI_DB_PASSWORD = "bmndb2detector";
const char* const UNI_DB_NAME = "bmn_db";               // for database adninistrator only
const char* const UNI_DB_HOST = "nc13.jinr.ru";         // for database adninistrator only

const char* const TANGO_DB_USERNAME = "tango";
const char* const TANGO_DB_PASSWORD = "tangompd";
const char* const TANGO_DB_NAME = "hdbpp";              // for database adninistrator only
const char* const TANGO_DB_HOST = "mpd-scdb.jinr.ru";   // for database adninistrator only

#endif // DB_SETTINGS_H
