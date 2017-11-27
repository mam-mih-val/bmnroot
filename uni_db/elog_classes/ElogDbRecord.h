// ----------------------------------------------------------------------
//                    ElogDbRecord header file 
//                      Generated 27-11-2017 
// ----------------------------------------------------------------------

/** elog_classes/ElogDbRecord.h 
 ** Class for the table: record_ 
 **/ 

#ifndef ELOGDBRECORD_H 
#define ELOGDBRECORD_H 1 

#include "TString.h"
#include "TDatime.h"

#include "UniDbConnection.h"

class ElogDbRecord
{
 private:
	/* GENERATED PRIVATE MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	/// connection to the database
	UniDbConnection* connectionUniDb;

	/// record id
	int i_record_id;
	/// record date
	TDatime dt_record_date;
	/// author id
	int* i_author_id;
	/// type id
	int i_type_id;
	/// run number
	int* i_run_number;
	/// shift leader id
	int* i_shift_leader_id;
	/// trigger id
	int* i_trigger_id;
	/// daq status
	TString* str_daq_status;
	/// sp 41
	int* i_sp_41;
	/// field comment
	TString* str_field_comment;
	/// beam
	TString* str_beam;
	/// energy
	double* d_energy;
	/// target
	TString* str_target;
	/// target width
	double* d_target_width;
	/// record comment
	TString* str_record_comment;

	//Constructor
	ElogDbRecord(UniDbConnection* connUniDb, int record_id, TDatime record_date, int* author_id, int type_id, int* run_number, int* shift_leader_id, int* trigger_id, TString* daq_status, int* sp_41, TString* field_comment, TString* beam, double* energy, TString* target, double* target_width, TString* record_comment);
	/* END OF PRIVATE GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 public:
	/* GENERATED PUBLIC MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	virtual ~ElogDbRecord(); // Destructor

	// static class functions
	/// add new record to the database
	static ElogDbRecord* CreateRecord(TDatime record_date, int* author_id, int type_id, int* run_number, int* shift_leader_id, int* trigger_id, TString* daq_status, int* sp_41, TString* field_comment, TString* beam, double* energy, TString* target, double* target_width, TString* record_comment);
	/// get record from the database
	static ElogDbRecord* GetRecord(int record_id);
	/// check record exists in the database
	static bool CheckRecordExists(int record_id);
	/// delete record from the database
	static int DeleteRecord(int record_id);
	/// print all records
	static int PrintAll();

	// Getters
	/// get record id of the current record
	int GetRecordId() {return i_record_id;}
	/// get record date of the current record
	TDatime GetRecordDate() {return dt_record_date;}
	/// get author id of the current record
	int* GetAuthorId() {if (i_author_id == NULL) return NULL; else return new int(*i_author_id);}
	/// get type id of the current record
	int GetTypeId() {return i_type_id;}
	/// get run number of the current record
	int* GetRunNumber() {if (i_run_number == NULL) return NULL; else return new int(*i_run_number);}
	/// get shift leader id of the current record
	int* GetShiftLeaderId() {if (i_shift_leader_id == NULL) return NULL; else return new int(*i_shift_leader_id);}
	/// get trigger id of the current record
	int* GetTriggerId() {if (i_trigger_id == NULL) return NULL; else return new int(*i_trigger_id);}
	/// get daq status of the current record
	TString* GetDaqStatus() {if (str_daq_status == NULL) return NULL; else return new TString(*str_daq_status);}
	/// get sp 41 of the current record
	int* GetSp41() {if (i_sp_41 == NULL) return NULL; else return new int(*i_sp_41);}
	/// get field comment of the current record
	TString* GetFieldComment() {if (str_field_comment == NULL) return NULL; else return new TString(*str_field_comment);}
	/// get beam of the current record
	TString* GetBeam() {if (str_beam == NULL) return NULL; else return new TString(*str_beam);}
	/// get energy of the current record
	double* GetEnergy() {if (d_energy == NULL) return NULL; else return new double(*d_energy);}
	/// get target of the current record
	TString* GetTarget() {if (str_target == NULL) return NULL; else return new TString(*str_target);}
	/// get target width of the current record
	double* GetTargetWidth() {if (d_target_width == NULL) return NULL; else return new double(*d_target_width);}
	/// get record comment of the current record
	TString* GetRecordComment() {if (str_record_comment == NULL) return NULL; else return new TString(*str_record_comment);}

	// Setters
	/// set record date of the current record
	int SetRecordDate(TDatime record_date);
	/// set author id of the current record
	int SetAuthorId(int* author_id);
	/// set type id of the current record
	int SetTypeId(int type_id);
	/// set run number of the current record
	int SetRunNumber(int* run_number);
	/// set shift leader id of the current record
	int SetShiftLeaderId(int* shift_leader_id);
	/// set trigger id of the current record
	int SetTriggerId(int* trigger_id);
	/// set daq status of the current record
	int SetDaqStatus(TString* daq_status);
	/// set sp 41 of the current record
	int SetSp41(int* sp_41);
	/// set field comment of the current record
	int SetFieldComment(TString* field_comment);
	/// set beam of the current record
	int SetBeam(TString* beam);
	/// set energy of the current record
	int SetEnergy(double* energy);
	/// set target of the current record
	int SetTarget(TString* target);
	/// set target width of the current record
	int SetTargetWidth(double* target_width);
	/// set record comment of the current record
	int SetRecordComment(TString* record_comment);

	/// print information about current record
	void Print();
	/* END OF PUBLIC GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 ClassDef(ElogDbRecord,1);
};

#endif
