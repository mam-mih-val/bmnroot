// ----------------------------------------------------------------------
//                    UniDbElogRecord header file 
//                      Generated 23-11-2017 
// ----------------------------------------------------------------------

/** db_classes/UniDbElogRecord.h 
 ** Class for the table: elog_record 
 **/ 

#ifndef UNIDBELOGRECORD_H 
#define UNIDBELOGRECORD_H 1 

#include "TString.h"
#include "TDatime.h"

#include "UniDbConnection.h"

class UniDbElogRecord
{
 private:
	/* GENERATED PRIVATE MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	/// connection to the database
	UniDbConnection* connectionUniDb;

	/// record id
	int i_record_id;
	/// record date
	TDatime dt_record_date;
	/// author
	int* i_author;
	/// record type
	int i_record_type;
	/// run number
	int* i_run_number;
	/// shift leader
	int* i_shift_leader;
	/// trigger config
	int* i_trigger_config;
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
	UniDbElogRecord(UniDbConnection* connUniDb, int record_id, TDatime record_date, int* author, int record_type, int* run_number, int* shift_leader, int* trigger_config, TString* daq_status, int* sp_41, TString* field_comment, TString* beam, double* energy, TString* target, double* target_width, TString* record_comment);
	/* END OF PRIVATE GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 public:
	/* GENERATED PUBLIC MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	virtual ~UniDbElogRecord(); // Destructor

	// static class functions
	/// add new elog record to the database
	static UniDbElogRecord* CreateElogRecord(int* author, int record_type, int* run_number, int* shift_leader, int* trigger_config, TString* daq_status, int* sp_41, TString* field_comment, TString* beam, double* energy, TString* target, double* target_width, TString* record_comment);
	/// get elog record from the database
	static UniDbElogRecord* GetElogRecord(int record_id);
	/// check elog record exists in the database
	static bool CheckElogRecordExists(int record_id);
	/// delete elog record from the database
	static int DeleteElogRecord(int record_id);
	/// print all elog records
	static int PrintAll();

	// Getters
	/// get record id of the current elog record
	int GetRecordId() {return i_record_id;}
	/// get record date of the current elog record
	TDatime GetRecordDate() {return dt_record_date;}
	/// get author of the current elog record
	int* GetAuthor() {if (i_author == NULL) return NULL; else return new int(*i_author);}
	/// get record type of the current elog record
	int GetRecordType() {return i_record_type;}
	/// get run number of the current elog record
	int* GetRunNumber() {if (i_run_number == NULL) return NULL; else return new int(*i_run_number);}
	/// get shift leader of the current elog record
	int* GetShiftLeader() {if (i_shift_leader == NULL) return NULL; else return new int(*i_shift_leader);}
	/// get trigger config of the current elog record
	int* GetTriggerConfig() {if (i_trigger_config == NULL) return NULL; else return new int(*i_trigger_config);}
	/// get daq status of the current elog record
	TString* GetDaqStatus() {if (str_daq_status == NULL) return NULL; else return new TString(*str_daq_status);}
	/// get sp 41 of the current elog record
	int* GetSp41() {if (i_sp_41 == NULL) return NULL; else return new int(*i_sp_41);}
	/// get field comment of the current elog record
	TString* GetFieldComment() {if (str_field_comment == NULL) return NULL; else return new TString(*str_field_comment);}
	/// get beam of the current elog record
	TString* GetBeam() {if (str_beam == NULL) return NULL; else return new TString(*str_beam);}
	/// get energy of the current elog record
	double* GetEnergy() {if (d_energy == NULL) return NULL; else return new double(*d_energy);}
	/// get target of the current elog record
	TString* GetTarget() {if (str_target == NULL) return NULL; else return new TString(*str_target);}
	/// get target width of the current elog record
	double* GetTargetWidth() {if (d_target_width == NULL) return NULL; else return new double(*d_target_width);}
	/// get record comment of the current elog record
	TString* GetRecordComment() {if (str_record_comment == NULL) return NULL; else return new TString(*str_record_comment);}

	// Setters
	/// set author of the current elog record
	int SetAuthor(int* author);
	/// set record type of the current elog record
	int SetRecordType(int record_type);
	/// set run number of the current elog record
	int SetRunNumber(int* run_number);
	/// set shift leader of the current elog record
	int SetShiftLeader(int* shift_leader);
	/// set trigger config of the current elog record
	int SetTriggerConfig(int* trigger_config);
	/// set daq status of the current elog record
	int SetDaqStatus(TString* daq_status);
	/// set sp 41 of the current elog record
	int SetSp41(int* sp_41);
	/// set field comment of the current elog record
	int SetFieldComment(TString* field_comment);
	/// set beam of the current elog record
	int SetBeam(TString* beam);
	/// set energy of the current elog record
	int SetEnergy(double* energy);
	/// set target of the current elog record
	int SetTarget(TString* target);
	/// set target width of the current elog record
	int SetTargetWidth(double* target_width);
	/// set record comment of the current elog record
	int SetRecordComment(TString* record_comment);

        /// print information about current elog record
	void Print();
	/* END OF PUBLIC GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 ClassDef(UniDbElogRecord,1);
};

#endif
