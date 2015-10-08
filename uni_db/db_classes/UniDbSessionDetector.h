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
	/// connection to the database
	UniDbConnection* connectionUniDb;

	/// session number
	int i_session_number;
	/// detector name
	TString str_detector_name;
	/// map id
	int* i_map_id;

	//Constructor
	UniDbSessionDetector(UniDbConnection* connUniDb, int session_number, TString detector_name, int* map_id);
	/* END OF PRIVATE GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 public:
	/* GENERATED PUBLIC MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	virtual ~UniDbSessionDetector(); // Destructor

	// static class functions
	/// add new session detector to the database
	static UniDbSessionDetector* CreateSessionDetector(int session_number, TString detector_name, int* map_id);
	/// get session detector from the database
	static UniDbSessionDetector* GetSessionDetector(int session_number, TString detector_name);
	/// delete session detector from the database
	static int DeleteSessionDetector(int session_number, TString detector_name);
	/// print all session detectors
	static int PrintAll();

	// Getters
	/// get session number of the current session detector
	int GetSessionNumber() {return i_session_number;}
	/// get detector name of the current session detector
	TString GetDetectorName() {return str_detector_name;}
	/// get map id of the current session detector
	int* GetMapId() {if (i_map_id == NULL) return NULL; else return new int(*i_map_id);}

	// Setters
	/// set session number of the current session detector
	int SetSessionNumber(int session_number);
	/// set detector name of the current session detector
	int SetDetectorName(TString detector_name);
	/// set map id of the current session detector
	int SetMapId(int* map_id);
	/// print information about current session detector
	void Print();
	/* END OF PUBLIC GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 ClassDef(UniDbSessionDetector,1);
};

#endif
