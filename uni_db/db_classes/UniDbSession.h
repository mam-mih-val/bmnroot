// ----------------------------------------------------------------------
//                    UniDbSession header file 
//                      Generated 20-10-2015 
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
	/// connection to the database
	UniDbConnection* connectionUniDb;

	/// session number
	int i_session_number;
	/// start datetime
	TDatime dt_start_datetime;
	/// end datetime
	TDatime* dt_end_datetime;

	//Constructor
	UniDbSession(UniDbConnection* connUniDb, int session_number, TDatime start_datetime, TDatime* end_datetime);
	/* END OF PRIVATE GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 public:
	/* GENERATED PUBLIC MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	virtual ~UniDbSession(); // Destructor

	// static class functions
	/// add new session to the database
	static UniDbSession* CreateSession(int session_number, TDatime start_datetime, TDatime* end_datetime);
	/// get session from the database
	static UniDbSession* GetSession(int session_number);
	/// delete session from the database
	static int DeleteSession(int session_number);
	/// print all sessions
	static int PrintAll();

	// Getters
	/// get session number of the current session
	int GetSessionNumber() {return i_session_number;}
	/// get start datetime of the current session
	TDatime GetStartDatetime() {return dt_start_datetime;}
	/// get end datetime of the current session
	TDatime* GetEndDatetime() {if (dt_end_datetime == NULL) return NULL; else return new TDatime(*dt_end_datetime);}

	// Setters
	/// set session number of the current session
	int SetSessionNumber(int session_number);
	/// set start datetime of the current session
	int SetStartDatetime(TDatime start_datetime);
	/// set end datetime of the current session
	int SetEndDatetime(TDatime* end_datetime);
	/// print information about current session
	void Print();
	/* END OF PUBLIC GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 ClassDef(UniDbSession,1);
};

#endif
