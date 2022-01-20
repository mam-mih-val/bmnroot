#ifndef DB_SETTINGS_H
#define DB_SETTINGS_H

// connection settings for databases and REST API of the experiment
// set name and password of your privileged user if required, do not forget to recompile after any changes

const char* const UNI_DB_USERNAME = "db_reader";
const char* const UNI_DB_PASSWORD = "reader_pass";
const char* const UNI_DB_HOST = "nc13.jinr.ru";
const char* const UNI_DB_NAME = "bmn_db";

const char* const TANGO_DB_USERNAME = "tango";
const char* const TANGO_DB_PASSWORD = "tangompd";
const char* const TANGO_DB_HOST = "10.18.11.66";
const char* const TANGO_DB_NAME = "hdbpp";

const char* const ELOG_DB_USERNAME = "shift";
const char* const ELOG_DB_PASSWORD = "shift";
const char* const ELOG_DB_HOST = "nc13.jinr.ru";
const char* const ELOG_DB_NAME = "bmn_elog";

const char* const EVENT_API_USERNAME = "shift";
const char* const EVENT_API_PASSWORD = "shift";
const char* const EVENT_API_HOST = "http:///159.93.51.133:80";
const char* const EVENT_API_NAME = "event_api/v1/bmn/emd";

#endif // DB_SETTINGS_H
