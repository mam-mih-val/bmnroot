// ----------------------------------------------------------------------
//                    UniDbRunPeriod header file 
//                      Generated 05-11-2015 
// ----------------------------------------------------------------------

/** db_classes/UniDbRunPeriod.h 
 ** Class for the table: run_period 
 **/ 

#ifndef UNIDBRUNPERIOD_H 
#define UNIDBRUNPERIOD_H 1 

#include "TString.h"
#include "TDatime.h"

#include "UniDbConnection.h"

class UniDbRunPeriod
{
 private:
	/* GENERATED PRIVATE MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	/// connection to the database
	UniDbConnection* connectionUniDb;

	/// period number
	int i_period_number;
	/// start datetime
	TDatime dt_start_datetime;
	/// end datetime
	TDatime* dt_end_datetime;
	/// contact person
	TString* str_contact_person;

	//Constructor
	UniDbRunPeriod(UniDbConnection* connUniDb, int period_number, TDatime start_datetime, TDatime* end_datetime, TString* contact_person);
	/* END OF PRIVATE GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 public:
	/* GENERATED PUBLIC MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	virtual ~UniDbRunPeriod(); // Destructor

	// static class functions
	/// add new run period to the database
	static UniDbRunPeriod* CreateRunPeriod(int period_number, TDatime start_datetime, TDatime* end_datetime, TString* contact_person);
	/// get run period from the database
	static UniDbRunPeriod* GetRunPeriod(int period_number);
	/// delete run period from the database
	static int DeleteRunPeriod(int period_number);
	/// print all run periods
	static int PrintAll();

	// Getters
	/// get period number of the current run period
	int GetPeriodNumber() {return i_period_number;}
	/// get start datetime of the current run period
	TDatime GetStartDatetime() {return dt_start_datetime;}
	/// get end datetime of the current run period
	TDatime* GetEndDatetime() {if (dt_end_datetime == NULL) return NULL; else return new TDatime(*dt_end_datetime);}
	/// get contact person of the current run period
	TString* GetContactPerson() {if (str_contact_person == NULL) return NULL; else return new TString(*str_contact_person);}

	// Setters
	/// set period number of the current run period
	int SetPeriodNumber(int period_number);
	/// set start datetime of the current run period
	int SetStartDatetime(TDatime start_datetime);
	/// set end datetime of the current run period
	int SetEndDatetime(TDatime* end_datetime);
	/// set contact person of the current run period
	int SetContactPerson(TString* contact_person);
	/// print information about current run period
	void Print();
	/* END OF PUBLIC GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 ClassDef(UniDbRunPeriod,1);
};

#endif
