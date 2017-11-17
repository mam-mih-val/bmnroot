#ifndef DB_SETTINGS_H
#define DB_SETTINGS_H

const char* const UNI_DB_USERNAME = "db_reader";        // set your priviledge user if required (don't forget to recompile after changing)
const char* const UNI_DB_PASSWORD = "reader_pass";      // set your priviledge user if required (don't forget to recompile after changing)
const char* const UNI_DB_NAME = "bmn_db";               // option for DB adninistrator only
const char* const UNI_DB_HOST = "vm221-53.jinr.ru";     // option for DB adninistrator only

const char* const TANGO_DB_USERNAME = "tango";          // option for DB adninistrator only
const char* const TANGO_DB_PASSWORD = "tangompd";       // option for DB adninistrator only
const char* const TANGO_DB_NAME = "hdbpp";              // option for DB adninistrator only
const char* const TANGO_DB_HOST = "10.18.11.66";        // option for DB adninistrator only

const char* const ELOG_DB_USERNAME = "";         // option for DB adninistrator only
const char* const ELOG_DB_PASSWORD = "";   // option for DB adninistrator only
const char* const ELOG_DB_NAME = "bmn_elog";             // option for DB adninistrator only
const char* const ELOG_DB_HOST = "vm221-53.jinr.ru";    // option for DB adninistrator only

#endif // DB_SETTINGS_H
