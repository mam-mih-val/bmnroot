// ----------------------------------------------------------------------
//                    ElogDbRecord header file 
//                      Generated 27-11-2017 
// ----------------------------------------------------------------------

/** ElogDbRecord.h
 ** Class for the table: record_ 
 **/ 

#ifndef ELOGDBRECORD_H 
#define ELOGDBRECORD_H 1 

#include "TString.h"
#include "TDatime.h"
#include "TObjArray.h"

#include "UniConnection.h"
#include "UniSearchCondition.h"

class ElogDbRecord
{
 private:
	/* GENERATED PRIVATE MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	/// connection to the database
        UniConnection* connectionUniDb;

	/// record id
	int i_record_id;
	/// record date
	TDatime dt_record_date;
	/// shift leader id
	int* i_shift_leader_id;
	/// type id
	int i_type_id;
	/// period number
	int* i_period_number;
	/// run number
	int* i_run_number;
	/// trigger id
	int* i_trigger_id;
	/// daq status
	TString* str_daq_status;
	/// sp 41
	int* i_sp_41;
	/// sp 57
	int* i_sp_57;
	/// vkm2
	int* i_vkm2;
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
        ElogDbRecord(UniConnection* connUniDb, int record_id, TDatime record_date, int* shift_leader_id, int type_id, int* period_number, int* run_number, int* trigger_id, TString* daq_status, int* sp_41, int* sp_57, int* vkm2, TString* field_comment, TString* beam, double* energy, TString* target, double* target_width, TString* record_comment);
	/* END OF PRIVATE GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 public:
	/* GENERATED PUBLIC MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	virtual ~ElogDbRecord(); // Destructor

	// static class functions
	/// add new record to the database
	static ElogDbRecord* CreateRecord(TDatime record_date, int* shift_leader_id, int type_id, int* period_number, int* run_number, int* trigger_id, TString* daq_status, int* sp_41, int* sp_57, int* vkm2, TString* field_comment, TString* beam, double* energy, TString* target, double* target_width, TString* record_comment);
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
	/// get shift leader id of the current record
	int* GetShiftLeaderId() {if (i_shift_leader_id == NULL) return NULL; else return new int(*i_shift_leader_id);}
	/// get type id of the current record
	int GetTypeId() {return i_type_id;}
	/// get period number of the current record
	int* GetPeriodNumber() {if (i_period_number == NULL) return NULL; else return new int(*i_period_number);}
	/// get run number of the current record
	int* GetRunNumber() {if (i_run_number == NULL) return NULL; else return new int(*i_run_number);}
	/// get trigger id of the current record
	int* GetTriggerId() {if (i_trigger_id == NULL) return NULL; else return new int(*i_trigger_id);}
	/// get daq status of the current record
	TString* GetDaqStatus() {if (str_daq_status == NULL) return NULL; else return new TString(*str_daq_status);}
	/// get sp 41 of the current record
	int* GetSp41() {if (i_sp_41 == NULL) return NULL; else return new int(*i_sp_41);}
	/// get sp 57 of the current record
	int* GetSp57() {if (i_sp_57 == NULL) return NULL; else return new int(*i_sp_57);}
	/// get vkm2 of the current record
	int* GetVkm2() {if (i_vkm2 == NULL) return NULL; else return new int(*i_vkm2);}
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
	/// set shift leader id of the current record
	int SetShiftLeaderId(int* shift_leader_id);
	/// set type id of the current record
	int SetTypeId(int type_id);
	/// set period number of the current record
	int SetPeriodNumber(int* period_number);
	/// set run number of the current record
	int SetRunNumber(int* run_number);
	/// set trigger id of the current record
	int SetTriggerId(int* trigger_id);
	/// set daq status of the current record
	int SetDaqStatus(TString* daq_status);
	/// set sp 41 of the current record
	int SetSp41(int* sp_41);
	/// set sp 57 of the current record
	int SetSp57(int* sp_57);
	/// set vkm2 of the current record
	int SetVkm2(int* vkm2);
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

    /// get array of ElogDbRecord-s for a given run from the database
    static TObjArray* GetRecords(int period_number, int run_number);

    /// get ELOG records corresponding to the specified single condition
    static TObjArray* Search(const UniSearchCondition& search_condition);
    /// get ELOG records corresponding to the specified (vector) conditions
    static TObjArray* Search(const TObjArray& search_conditions);

 ClassDef(ElogDbRecord,1);
};

#endif
