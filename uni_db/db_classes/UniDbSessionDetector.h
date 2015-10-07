// ----------------------------------------------------------------------
//                    UniDbSessionDetector header file 
//                      Generated 15-09-2015 
// ----------------------------------------------------------------------

/** db_classes/UniDbSessionDetector.h 
 ** Class for the table: session_detector 
 **/ 

#ifndef UNIDBSESSIONDETECTOR_H
#define UNIDBSESSIONDETECTOR_H 1

#include "TString.h"
#include "TDatime.h"

#include "UniDbConnection.h"

class UniDbSessionDetector
{
 private:
	/* GENERATED PRIVATE MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	UniDbConnection* connectionUniDb;

	int i_session_number;
	TString str_detector_name;
	int* i_map_id;

	//Constructor
	UniDbSessionDetector(UniDbConnection* connUniDb, int session_number, TString detector_name, int* map_id);
	/* END OF PRIVATE GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 public:
	/* GENERATED PUBLIC MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	virtual ~UniDbSessionDetector(); // Destructor

	// static class functions
	static UniDbSessionDetector* CreateSessionDetector(int session_number, TString detector_name, int* map_id);
	static UniDbSessionDetector* GetSessionDetector(int session_number, TString detector_name);
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

 ClassDef(UniDbSessionDetector,1);
};

#endif
