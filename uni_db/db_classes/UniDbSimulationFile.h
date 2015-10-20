// ----------------------------------------------------------------------
//                    UniDbSimulationFile header file 
//                      Generated 20-10-2015 
// ----------------------------------------------------------------------

/** db_classes/UniDbSimulationFile.h 
 ** Class for the table: simulation_file 
 **/ 

#ifndef UNIDBSIMULATIONFILE_H 
#define UNIDBSIMULATIONFILE_H 1 

#include "TString.h"
#include "TDatime.h"

#include "UniDbConnection.h"

class UniDbSimulationFile
{
 private:
	/* GENERATED PRIVATE MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	/// connection to the database
	UniDbConnection* connectionUniDb;

	/// file id
	int i_file_id;
	/// file path
	TString str_file_path;
	/// generator name
	TString str_generator_name;
	/// beam particle
	TString str_beam_particle;
	/// target particle
	TString* str_target_particle;
	/// energy
	double* d_energy;
	/// centrality
	TString str_centrality;
	/// event count
	int* i_event_count;
	/// file desc
	TString* str_file_desc;
	/// file size
	double* d_file_size;

	//Constructor
	UniDbSimulationFile(UniDbConnection* connUniDb, int file_id, TString file_path, TString generator_name, TString beam_particle, TString* target_particle, double* energy, TString centrality, int* event_count, TString* file_desc, double* file_size);
	/* END OF PRIVATE GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 public:
	/* GENERATED PUBLIC MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	virtual ~UniDbSimulationFile(); // Destructor

	// static class functions
	/// add new simulation file to the database
	static UniDbSimulationFile* CreateSimulationFile(TString file_path, TString generator_name, TString beam_particle, TString* target_particle, double* energy, TString centrality, int* event_count, TString* file_desc, double* file_size);
	/// get simulation file from the database
	static UniDbSimulationFile* GetSimulationFile(int file_id);
	/// get simulation file from the database
	static UniDbSimulationFile* GetSimulationFile(TString file_path);
	/// delete simulation file from the database
	static int DeleteSimulationFile(int file_id);
	/// delete simulation file from the database
	static int DeleteSimulationFile(TString file_path);
	/// print all simulation files
	static int PrintAll();

	// Getters
	/// get file id of the current simulation file
	int GetFileId() {return i_file_id;}
	/// get file path of the current simulation file
	TString GetFilePath() {return str_file_path;}
	/// get generator name of the current simulation file
	TString GetGeneratorName() {return str_generator_name;}
	/// get beam particle of the current simulation file
	TString GetBeamParticle() {return str_beam_particle;}
	/// get target particle of the current simulation file
	TString* GetTargetParticle() {if (str_target_particle == NULL) return NULL; else return new TString(*str_target_particle);}
	/// get energy of the current simulation file
	double* GetEnergy() {if (d_energy == NULL) return NULL; else return new double(*d_energy);}
	/// get centrality of the current simulation file
	TString GetCentrality() {return str_centrality;}
	/// get event count of the current simulation file
	int* GetEventCount() {if (i_event_count == NULL) return NULL; else return new int(*i_event_count);}
	/// get file desc of the current simulation file
	TString* GetFileDesc() {if (str_file_desc == NULL) return NULL; else return new TString(*str_file_desc);}
	/// get file size of the current simulation file
	double* GetFileSize() {if (d_file_size == NULL) return NULL; else return new double(*d_file_size);}

	// Setters
	/// set file id of the current simulation file
	int SetFileId(int file_id);
	/// set file path of the current simulation file
	int SetFilePath(TString file_path);
	/// set generator name of the current simulation file
	int SetGeneratorName(TString generator_name);
	/// set beam particle of the current simulation file
	int SetBeamParticle(TString beam_particle);
	/// set target particle of the current simulation file
	int SetTargetParticle(TString* target_particle);
	/// set energy of the current simulation file
	int SetEnergy(double* energy);
	/// set centrality of the current simulation file
	int SetCentrality(TString centrality);
	/// set event count of the current simulation file
	int SetEventCount(int* event_count);
	/// set file desc of the current simulation file
	int SetFileDesc(TString* file_desc);
	/// set file size of the current simulation file
	int SetFileSize(double* file_size);
	/// print information about current simulation file
	void Print();
	/* END OF PUBLIC GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 ClassDef(UniDbSimulationFile,1);
};

#endif
