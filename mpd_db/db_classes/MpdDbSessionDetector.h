// ----------------------------------------------------------------------
//                    MpdDbSessionDetector header file 
//                      Generated 15-09-2015 
// ----------------------------------------------------------------------

/** db_classes/MpdDbSessionDetector.h 
 ** Class for the table: session_detector 
 **/ 

#ifndef MPDDBSESSIONDETECTOR_H 
#define MPDDBSESSIONDETECTOR_H 1 

#include "TString.h"
#include "TDatime.h"

#include "MpdDbConnection.h"

class MpdDbSessionDetector
{
 private:
	/* GENERATED PRIVATE MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	MpdDbConnection* connectionUniDb;

	int i_session_number;
	TString str_detector_name;
	int* i_map_id;

	//Constructor
	MpdDbSessionDetector(MpdDbConnection* connUniDb, int session_number, TString detector_name, int* map_id);
	/* END OF PRIVATE GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 public:
	/* GENERATED PUBLIC MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	virtual ~MpdDbSessionDetector(); // Destructor

	// static class functions
	static MpdDbSessionDetector* CreateSessionDetector(int session_number, TString detector_name, int* map_id);
	static MpdDbSessionDetector* GetSessionDetector(int session_number, TString detector_name);
	static int DeleteSessionDetector(int session_number, TString detector_name);
	static int PrintAll();

	// Getters
	int GetSessionNumber() {return i_session_number;}
	TString GetDetectorName() {return str_detector_name;}
	int* GetMapId() {if (i_map_id == NULL) return NULL; else return new int(*i_map_id);}

	// Setters
	int SetSessionNumber(int session_number);
	int SetDetectorName(TString detector_name);
	int SetMapId(int* map_id);
	void Print();
	/* END OF PUBLIC GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 ClassDef(MpdDbSessionDetector,1);
};

#endif
