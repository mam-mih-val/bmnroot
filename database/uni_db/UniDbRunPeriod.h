// ----------------------------------------------------------------------
//                    UniDbRunPeriod header file 
//                      Generated 05-11-2015 
// ----------------------------------------------------------------------

/** UniDbRunPeriod.h
 ** Class for the table: run_period 
 **/ 

#ifndef UNIDBRUNPERIOD_H 
#define UNIDBRUNPERIOD_H 1 

#include "TString.h"
#include "TDatime.h"

#include "UniConnection.h"
#include "db_structures.h"

class UniDbRunPeriod
{
 private:
	/* GENERATED PRIVATE MEMBERS (SHOULD NOT BE CHANGED MANUALLY) */
	/// connection to the database
	UniConnection* connectionUniDb;

	/// period number
	int i_period_number;
	/// start datetime
	TDatime ts_start_datetime;
	/// end datetime
	TDatime* ts_end_datetime;

	//Constructor
	UniDbRunPeriod(UniConnection* connUniDb, int period_number, TDatime start_datetime, TDatime* end_datetime);
	/* END OF PRIVATE GENERATED PART (SHOULD NOT BE CHANGED MANUALLY) */

 public:
	/* GENERATED PUBLIC MEMBERS (SHOULD NOT BE CHANGED MANUALLY) */
	virtual ~UniDbRunPeriod(); // Destructor

	// static class functions
	/// add new run period to the database
	static UniDbRunPeriod* CreateRunPeriod(int period_number, TDatime start_datetime, TDatime* end_datetime);
	/// get run period from the database
	static UniDbRunPeriod* GetRunPeriod(int period_number);
	/// check run period exists in the database
	static bool CheckRunPeriodExists(int period_number);
	/// delete run period from the database
	static int DeleteRunPeriod(int period_number);
	/// print all run periods
	static int PrintAll();

	// Getters
	/// get period number of the current run period
	int GetPeriodNumber() {return i_period_number;}
	/// get start datetime of the current run period
	TDatime GetStartDatetime() {return ts_start_datetime;}
	/// get end datetime of the current run period
	TDatime* GetEndDatetime() {if (ts_end_datetime == NULL) return NULL; else return new TDatime(*ts_end_datetime);}

	// Setters
	/// set period number of the current run period
	int SetPeriodNumber(int period_number);
	/// set start datetime of the current run period
	int SetStartDatetime(TDatime start_datetime);
	/// set end datetime of the current run period
	int SetEndDatetime(TDatime* end_datetime);

	/// print information about current run period
	void Print();
	/* END OF PUBLIC GENERATED PART (SHOULD NOT BE CHANGED MANUALLY) */

    /// get numbers of runs existing in the Database for a selected period
    /// \param[in] period_number period number for selected run numbers' range
    /// \param[out] run pairs (period number+run numbers) of the really existing runs for a selected range (from start to end)
    /// \return size of 'run_numbers' array. if size < 0, return value corresponds to error number
    static int GetRunNumbers(int period_number, UniqueRunNumber*& run_numbers);
    /// get first run number for a selected period
    /// \param[in] period_number a selected period number to find the minimal run number
    /// \return minimal run number for a selected period. if number < 0, the return value corresponds to the error number
    static int GetFirstRunNumber(int period_number);
    /// get last run number for a selected period
    /// \param[in] period_number a selected period number to find the maximal run number
    /// \return maximal run number for a selected period. if number < 0, the return value corresponds to the error number
    static int GetLastRunNumber(int period_number);

 ClassDef(UniDbRunPeriod,1);
};

#endif
