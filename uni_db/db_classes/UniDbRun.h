// ----------------------------------------------------------------------
//                    UniDbRun header file 
//                      Generated 05-11-2015 
// ----------------------------------------------------------------------

/** db_classes/UniDbRun.h 
 ** Class for the table: run_
 **/

#ifndef UNIDBRUN_H 
#define UNIDBRUN_H 1 

#include "TString.h"
#include "TDatime.h"
#include "TObjArray.h"

#include "UniDbConnection.h"
#include "UniDbSearchCondition.h"
#include "db_structures.h"

class UniDbRun
{
 private:
	/* GENERATED PRIVATE MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	/// connection to the database
	UniDbConnection* connectionUniDb;

	/// period number
	int i_period_number;
	/// run number
	int i_run_number;
	/// file path
	TString str_file_path;
	/// beam particle
	TString str_beam_particle;
	/// target particle
	TString* str_target_particle;
	/// energy
	double* d_energy;
	/// start datetime
	TDatime dt_start_datetime;
	/// end datetime
	TDatime* dt_end_datetime;
	/// event count
	int* i_event_count;
	/// field voltage
	double* d_field_voltage;
	/// file size
	double* d_file_size;
	/// geometry id
	int* i_geometry_id;

	//Constructor
	UniDbRun(UniDbConnection* connUniDb, int period_number, int run_number, TString file_path, TString beam_particle, TString* target_particle, double* energy, TDatime start_datetime, TDatime* end_datetime, int* event_count, double* field_voltage, double* file_size, int* geometry_id);
	/* END OF PRIVATE GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 public:
	/* GENERATED PUBLIC MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	virtual ~UniDbRun(); // Destructor

	// static class functions
	/// add new run to the database
	static UniDbRun* CreateRun(int period_number, int run_number, TString file_path, TString beam_particle, TString* target_particle, double* energy, TDatime start_datetime, TDatime* end_datetime, int* event_count, double* field_voltage, double* file_size, int* geometry_id);
	/// get run from the database
	static UniDbRun* GetRun(int period_number, int run_number);
	/// get run from the database
	static UniDbRun* GetRun(TString file_path);
	/// check run exists in the database
	static bool CheckRunExists(int period_number, int run_number);
	/// check run exists in the database
	static bool CheckRunExists(TString file_path);
	/// delete run from the database
	static int DeleteRun(int period_number, int run_number);
	/// delete run from the database
	static int DeleteRun(TString file_path);
	/// print all runs
	static int PrintAll();

	// Getters
	/// get period number of the current run
	int GetPeriodNumber() {return i_period_number;}
	/// get run number of the current run
	int GetRunNumber() {return i_run_number;}
	/// get file path of the current run
	TString GetFilePath() {return str_file_path;}
	/// get beam particle of the current run
	TString GetBeamParticle() {return str_beam_particle;}
	/// get target particle of the current run
	TString* GetTargetParticle() {if (str_target_particle == NULL) return NULL; else return new TString(*str_target_particle);}
	/// get energy of the current run
	double* GetEnergy() {if (d_energy == NULL) return NULL; else return new double(*d_energy);}
	/// get start datetime of the current run
	TDatime GetStartDatetime() {return dt_start_datetime;}
	/// get end datetime of the current run
	TDatime* GetEndDatetime() {if (dt_end_datetime == NULL) return NULL; else return new TDatime(*dt_end_datetime);}
	/// get event count of the current run
	int* GetEventCount() {if (i_event_count == NULL) return NULL; else return new int(*i_event_count);}
	/// get field voltage of the current run
	double* GetFieldVoltage() {if (d_field_voltage == NULL) return NULL; else return new double(*d_field_voltage);}
	/// get file size of the current run
	double* GetFileSize() {if (d_file_size == NULL) return NULL; else return new double(*d_file_size);}
	/// get geometry id of the current run
	int* GetGeometryId() {if (i_geometry_id == NULL) return NULL; else return new int(*i_geometry_id);}

	// Setters
	/// set period number of the current run
	int SetPeriodNumber(int period_number);
	/// set run number of the current run
	int SetRunNumber(int run_number);
	/// set file path of the current run
	int SetFilePath(TString file_path);
	/// set beam particle of the current run
	int SetBeamParticle(TString beam_particle);
	/// set target particle of the current run
	int SetTargetParticle(TString* target_particle);
	/// set energy of the current run
	int SetEnergy(double* energy);
	/// set start datetime of the current run
	int SetStartDatetime(TDatime start_datetime);
	/// set end datetime of the current run
	int SetEndDatetime(TDatime* end_datetime);
	/// set event count of the current run
	int SetEventCount(int* event_count);
	/// set field voltage of the current run
	int SetFieldVoltage(double* field_voltage);
	/// set file size of the current run
	int SetFileSize(double* file_size);
	/// set geometry id of the current run
	int SetGeometryId(int* geometry_id);
	/// print information about current run
	void Print();
	/* END OF PUBLIC GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

	// static class functions (added by user request)
	/// get numbers of runs existing in the Database for a selected range
        /// \param[in] start_period start period number for selected run numbers' range
        /// \param[in] start_run start run number for selected run numbers' range
        /// \param[in] end_period end period number for selected run numbers' range
        /// \param[in] end_run end run number for selected run numbers' range
        /// \param[out] run pairs (period number+run numbers) of the really existing runs for a selected range (from start to end)
	/// \return size of 'run_numbers' array. if size < 0, return value corresponds to error number
        static int GetRunNumbers(int start_period, int start_run, int end_period, int end_run, UniqueRunNumber*& run_numbers);
	/// get all numbers of existing runs in the Database
        /// \param[out] run_numbers pairs (period number+run numbers) of the really existing runs for a selected range (from start to end)
	/// \return size of 'run_numbers' array. if size < 0, return value corresponds to error number
        static int GetRunNumbers(UniqueRunNumber*& run_numbers);
	/// set geometry binary data (geometry file's data) for runs from start_run_number to end_run_number
        static int SetRootGeometry(int start_period, int start_run, int end_period, int end_run, unsigned char* root_geometry, Long_t size_root_geometry);
	/// get geometry binary data (geometry file's data) for selected run number
        static int GetRootGeometry(int period_number, int run_number, unsigned char*& root_geometry, Long_t& size_root_geometry);
	/// write geometry file for runs from start_run_number to end_run_number to the database
        static int WriteGeometryFile(int start_period, int start_run, int end_period, int end_run, char* geo_file_path);
	/// read geometry file for selected run number from the database
        static int ReadGeometryFile(int period_number, int run_number, char* geo_file_path);
	/// get runs corresponding to the specified single condition
	static TObjArray* Search(const UniDbSearchCondition& search_condition);
	/// get runs corresponding to the specified (vector) conditions
	static TObjArray* Search(const TObjArray& search_conditions);

	ClassDef(UniDbRun,1);
};

#endif
