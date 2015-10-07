// ----------------------------------------------------------------------
//                    UniDbSession header file 
//                      Generated 15-09-2015 
// ----------------------------------------------------------------------

/** db_classes/UniDbSession.h 
 ** Class for the table: session_ 
 **/ 

#ifndef UNIDBSESSION_H
#define UNIDBSESSION_H 1

#include "TString.h"
#include "TDatime.h"

#include "UniDbConnection.h"

class UniDbSession
{
 private:
	/* GENERATED PRIVATE MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	UniDbConnection* connectionUniDb;

	int i_session_number;
	TDatime dt_start_datetime;
	TDatime* dt_end_datetime;

	//Constructor
	UniDbSession(UniDbConnection* connUniDb, int session_number, TDatime start_datetime, TDatime* end_datetime);
	/* END OF PRIVATE GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 public:
	/* GENERATED PUBLIC MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	virtual ~UniDbSession(); // Destructor

	// static class functions
	static UniDbSession* CreateSession(int session_number, TDatime start_datetime, TDatime* end_datetime);
	static UniDbSession* GetSession(int session_number);
	static int DeleteSession(int session_number);
	static int PrintAll();

	// Getters
	int GetSessionNumber() {return i_session_number;}
	TDatime GetStartDatetime() {return dt_start_datetime;}
	TDatime* GetEndDatetime() {if (dt_end_datetime == NULL) return NULL; else return new TDatime(*dt_end_datetime);}

	// Setters
	int SetSessionNumber(int session_number);
	int SetStartDatetime(TDatime start_datetime);
	int SetEndDatetime(TDatime* end_datetime);
	void Print();
	/* END OF PUBLIC GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 ClassDef(UniDbSession,1);
};

#endif
