// ----------------------------------------------------------------------
//                    UniDbDetector header file 
//                      Generated 05-11-2015 
// ----------------------------------------------------------------------

/** UniDbDetector.h
 ** Class for the table: detector_ 
 **/ 

#ifndef UNIDBDETECTOR_H 
#define UNIDBDETECTOR_H 1 

#include "TString.h"
#include "TDatime.h"

#include "UniConnection.h"

class UniDbDetector
{
 private:
	/* GENERATED PRIVATE MEMBERS (SHOULD NOT BE CHANGED MANUALLY) */
	/// connection to the database
	UniConnection* connectionUniDb;

	/// detector name
	TString str_detector_name;
	/// description
	TString* str_description;

	//Constructor
	UniDbDetector(UniConnection* connUniDb, TString detector_name, TString* description);
	/* END OF PRIVATE GENERATED PART (SHOULD NOT BE CHANGED MANUALLY) */

 public:
	/* GENERATED PUBLIC MEMBERS (SHOULD NOT BE CHANGED MANUALLY) */
	virtual ~UniDbDetector(); // Destructor

	// static class functions
	/// add new detector to the database
	static UniDbDetector* CreateDetector(TString detector_name, TString* description);
	/// get detector from the database
	static UniDbDetector* GetDetector(TString detector_name);
	/// check detector exists in the database
	static bool CheckDetectorExists(TString detector_name);
	/// delete detector from the database
	static int DeleteDetector(TString detector_name);
	/// print all detectors
	static int PrintAll();

	// Getters
	/// get detector name of the current detector
	TString GetDetectorName() {return str_detector_name;}
	/// get description of the current detector
	TString* GetDescription() {if (str_description == NULL) return NULL; else return new TString(*str_description);}

	// Setters
	/// set detector name of the current detector
	int SetDetectorName(TString detector_name);
	/// set description of the current detector
	int SetDescription(TString* description);

	/// print information about current detector
	void Print();
	/* END OF PUBLIC GENERATED PART (SHOULD NOT BE CHANGED MANUALLY) */

 ClassDef(UniDbDetector,1);
};

#endif
