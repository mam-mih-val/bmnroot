// ----------------------------------------------------------------------
//                    MpdDbRun header file 
//                      Generated 15-09-2015 
// ----------------------------------------------------------------------

/** db_classes/MpdDbRun.h 
 ** Class for the table: run_ 
 **/ 

#ifndef MPDDBRUN_H 
#define MPDDBRUN_H 1 

#include "TString.h"
#include "TDatime.h"

#include "MpdDbConnection.h"

class MpdDbRun
{
 private:
	/* GENERATED PRIVATE MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	MpdDbConnection* connectionUniDb;

	int i_run_number;
	int* i_session_number;
	TString str_file_path;
	TString str_beam_particle;
	TString* str_target_particle;
	double* d_energy;
	TDatime dt_start_datetime;
	TDatime* dt_end_datetime;
	int* i_event_count;
	int* i_field_current;
	double* d_file_size;
	int* i_geometry_id;

	//Constructor
	MpdDbRun(MpdDbConnection* connUniDb, int run_number, int* session_number, TString file_path, TString beam_particle, TString* target_particle, double* energy, TDatime start_datetime, TDatime* end_datetime, int* event_count, int* field_current, double* file_size, int* geometry_id);
	/* END OF PRIVATE GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 public:
	/* GENERATED PUBLIC MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	virtual ~MpdDbRun(); // Destructor

	// static class functions
	static MpdDbRun* CreateRun(int run_number, int* session_number, TString file_path, TString beam_particle, TString* target_particle, double* energy, TDatime start_datetime, TDatime* end_datetime, int* event_count, int* field_current, double* file_size, int* geometry_id);
	static MpdDbRun* GetRun(int run_number);
	static MpdDbRun* GetRun(TString file_path);
	static int DeleteRun(int run_number);
	static int DeleteRun(TString file_path);
	static int PrintAll();

	// Getters
	int GetRunNumber() {return i_run_number;}
	int* GetSessionNumber() {if (i_session_number == NULL) return NULL; else return new int(*i_session_number);}
	TString GetFilePath() {return str_file_path;}
	TString GetBeamParticle() {return str_beam_particle;}
	TString* GetTargetParticle() {if (str_target_particle == NULL) return NULL; else return new TString(*str_target_particle);}
	double* GetEnergy() {if (d_energy == NULL) return NULL; else return new double(*d_energy);}
	TDatime GetStartDatetime() {return dt_start_datetime;}
	TDatime* GetEndDatetime() {if (dt_end_datetime == NULL) return NULL; else return new TDatime(*dt_end_datetime);}
	int* GetEventCount() {if (i_event_count == NULL) return NULL; else return new int(*i_event_count);}
	int* GetFieldCurrent() {if (i_field_current == NULL) return NULL; else return new int(*i_field_current);}
	double* GetFileSize() {if (d_file_size == NULL) return NULL; else return new double(*d_file_size);}
	int* GetGeometryId() {if (i_geometry_id == NULL) return NULL; else return new int(*i_geometry_id);}

	// Setters
	int SetRunNumber(int run_number);
	int SetSessionNumber(int* session_number);
	int SetFilePath(TString file_path);
	int SetBeamParticle(TString beam_particle);
	int SetTargetParticle(TString* target_particle);
	int SetEnergy(double* energy);
	int SetStartDatetime(TDatime start_datetime);
	int SetEndDatetime(TDatime* end_datetime);
	int SetEventCount(int* event_count);
	int SetFieldCurrent(int* field_current);
	int SetFileSize(double* file_size);
	int SetGeometryId(int* geometry_id);
	void Print();
	/* END OF PUBLIC GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

    // static class functions (added by user request)
    // add geometry data (geometry file's data) for runs from start_run_number to end_run_number
    static int SetRootGeometry(int start_run_number, int end_run_number, unsigned char* root_geometry, Long_t size_root_geometry);

 ClassDef(MpdDbRun,1);
};

#endif
