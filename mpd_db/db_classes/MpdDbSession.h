// ----------------------------------------------------------------------
//                    MpdDbSession header file 
//                      Generated 15-09-2015 
// ----------------------------------------------------------------------

/** db_classes/MpdDbSession.h 
 ** Class for the table: session_ 
 **/ 

#ifndef MPDDBSESSION_H 
#define MPDDBSESSION_H 1 

#include "TString.h"
#include "TDatime.h"

#include "MpdDbConnection.h"

class MpdDbSession
{
 private:
	/* GENERATED PRIVATE MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	MpdDbConnection* connectionUniDb;

	int i_session_number;
	TDatime dt_start_datetime;
	TDatime* dt_end_datetime;

	//Constructor
	MpdDbSession(MpdDbConnection* connUniDb, int session_number, TDatime start_datetime, TDatime* end_datetime);
	/* END OF PRIVATE GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 public:
	/* GENERATED PUBLIC MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	virtual ~MpdDbSession(); // Destructor

	// static class functions
	static MpdDbSession* CreateSession(int session_number, TDatime start_datetime, TDatime* end_datetime);
	static MpdDbSession* GetSession(int session_number);
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

 ClassDef(MpdDbSession,1);
};

#endif
