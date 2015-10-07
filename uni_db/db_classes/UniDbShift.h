// ----------------------------------------------------------------------
//                    UniDbShift header file 
//                      Generated 15-09-2015 
// ----------------------------------------------------------------------

/** db_classes/UniDbShift.h 
 ** Class for the table: shift_ 
 **/ 

#ifndef UNIDBSHIFT_H
#define UNIDBSHIFT_H 1

#include "TString.h"
#include "TDatime.h"

#include "UniDbConnection.h"

class UniDbShift
{
 private:
	/* GENERATED PRIVATE MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	UniDbConnection* connectionUniDb;

	int i_shift_id;
	int i_session_number;
	TString str_fio;
	TDatime dt_start_datetime;
	TDatime dt_end_datetime;
	TString* str_responsibility;

	//Constructor
	UniDbShift(UniDbConnection* connUniDb, int shift_id, int session_number, TString fio, TDatime start_datetime, TDatime end_datetime, TString* responsibility);
	/* END OF PRIVATE GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 public:
	/* GENERATED PUBLIC MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	virtual ~UniDbShift(); // Destructor

	// static class functions
	static UniDbShift* CreateShift(int session_number, TString fio, TDatime start_datetime, TDatime end_datetime, TString* responsibility);
	static UniDbShift* GetShift(int shift_id);
	static int DeleteShift(int shift_id);
	static int PrintAll();

	// Getters
	int GetShiftId() {return i_shift_id;}
	int GetSessionNumber() {return i_session_number;}
	TString GetFio() {return str_fio;}
	TDatime GetStartDatetime() {return dt_start_datetime;}
	TDatime GetEndDatetime() {return dt_end_datetime;}
	TString* GetResponsibility() {if (str_responsibility == NULL) return NULL; else return new TString(*str_responsibility);}

	// Setters
	int SetShiftId(int shift_id);
	int SetSessionNumber(int session_number);
	int SetFio(TString fio);
	int SetStartDatetime(TDatime start_datetime);
	int SetEndDatetime(TDatime end_datetime);
	int SetResponsibility(TString* responsibility);
	void Print();
	/* END OF PUBLIC GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 ClassDef(UniDbShift,1);
};

#endif
