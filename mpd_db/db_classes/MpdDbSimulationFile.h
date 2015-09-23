// ----------------------------------------------------------------------
//                    MpdDbSimulationFile header file 
//                      Generated 15-09-2015 
// ----------------------------------------------------------------------

/** db_classes/MpdDbSimulationFile.h 
 ** Class for the table: simulation_file 
 **/ 

#ifndef MPDDBSIMULATIONFILE_H 
#define MPDDBSIMULATIONFILE_H 1 

#include "TString.h"
#include "TDatime.h"

#include "MpdDbConnection.h"

class MpdDbSimulationFile
{
 private:
	/* GENERATED PRIVATE MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	MpdDbConnection* connectionUniDb;

	int i_file_id;
	TString str_file_path;
	TString str_generator_name;
	TString str_beam_particle;
	TString* str_target_particle;
	double* d_energy;
	TString str_centrality;
	int* i_event_count;
	TString* str_file_desc;
	double* d_file_size_kb;

	//Constructor
	MpdDbSimulationFile(MpdDbConnection* connUniDb, int file_id, TString file_path, TString generator_name, TString beam_particle, TString* target_particle, double* energy, TString centrality, int* event_count, TString* file_desc, double* file_size_kb);
	/* END OF PRIVATE GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 public:
	/* GENERATED PUBLIC MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	virtual ~MpdDbSimulationFile(); // Destructor

	// static class functions
	static MpdDbSimulationFile* CreateSimulationFile(TString file_path, TString generator_name, TString beam_particle, TString* target_particle, double* energy, TString centrality, int* event_count, TString* file_desc, double* file_size_kb);
	static MpdDbSimulationFile* GetSimulationFile(int file_id);
	static MpdDbSimulationFile* GetSimulationFile(TString file_path);
	static int DeleteSimulationFile(int file_id);
	static int DeleteSimulationFile(TString file_path);
	static int PrintAll();

	// Getters
	int GetFileId() {return i_file_id;}
	TString GetFilePath() {return str_file_path;}
	TString GetGeneratorName() {return str_generator_name;}
	TString GetBeamParticle() {return str_beam_particle;}
	TString* GetTargetParticle() {if (str_target_particle == NULL) return NULL; else return new TString(*str_target_particle);}
	double* GetEnergy() {if (d_energy == NULL) return NULL; else return new double(*d_energy);}
	TString GetCentrality() {return str_centrality;}
	int* GetEventCount() {if (i_event_count == NULL) return NULL; else return new int(*i_event_count);}
	TString* GetFileDesc() {if (str_file_desc == NULL) return NULL; else return new TString(*str_file_desc);}
	double* GetFileSizeKb() {if (d_file_size_kb == NULL) return NULL; else return new double(*d_file_size_kb);}

	// Setters
	int SetFileId(int file_id);
	int SetFilePath(TString file_path);
	int SetGeneratorName(TString generator_name);
	int SetBeamParticle(TString beam_particle);
	int SetTargetParticle(TString* target_particle);
	int SetEnergy(double* energy);
	int SetCentrality(TString centrality);
	int SetEventCount(int* event_count);
	int SetFileDesc(TString* file_desc);
	int SetFileSizeKb(double* file_size_kb);
	void Print();
	/* END OF PUBLIC GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 ClassDef(MpdDbSimulationFile,1);
};

#endif
