// ----------------------------------------------------------------------
//                    UniDbDetector header file 
//                      Generated 05-11-2015 
// ----------------------------------------------------------------------

/** db_classes/UniDbDetector.h 
 ** Class for the table: detector_ 
 **/ 

#ifndef UNIDBDETECTOR_H 
#define UNIDBDETECTOR_H 1 

#include "TString.h"
#include "TDatime.h"

#include "UniDbConnection.h"

class UniDbDetector
{
 private:
	/* GENERATED PRIVATE MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	/// connection to the database
	UniDbConnection* connectionUniDb;

	/// detector name
	TString str_detector_name;
	/// manufacturer name
	TString* str_manufacturer_name;
	/// contact person
	TString* str_contact_person;
	/// description
	TString* str_description;

	//Constructor
	UniDbDetector(UniDbConnection* connUniDb, TString detector_name, TString* manufacturer_name, TString* contact_person, TString* description);
	/* END OF PRIVATE GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 public:
	/* GENERATED PUBLIC MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	virtual ~UniDbDetector(); // Destructor

	// static class functions
	/// add new detector to the database
	static UniDbDetector* CreateDetector(TString detector_name, TString* manufacturer_name, TString* contact_person, TString* description);
	/// get detector from the database
	static UniDbDetector* GetDetector(TString detector_name);
	/// delete detector from the database
	static int DeleteDetector(TString detector_name);
	/// print all detectors
	static int PrintAll();

	// Getters
	/// get detector name of the current detector
	TString GetDetectorName() {return str_detector_name;}
	/// get manufacturer name of the current detector
	TString* GetManufacturerName() {if (str_manufacturer_name == NULL) return NULL; else return new TString(*str_manufacturer_name);}
	/// get contact person of the current detector
	TString* GetContactPerson() {if (str_contact_person == NULL) return NULL; else return new TString(*str_contact_person);}
	/// get description of the current detector
	TString* GetDescription() {if (str_description == NULL) return NULL; else return new TString(*str_description);}

	// Setters
	/// set detector name of the current detector
	int SetDetectorName(TString detector_name);
	/// set manufacturer name of the current detector
	int SetManufacturerName(TString* manufacturer_name);
	/// set contact person of the current detector
	int SetContactPerson(TString* contact_person);
	/// set description of the current detector
	int SetDescription(TString* description);
	/// print information about current detector
	void Print();
	/* END OF PUBLIC GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 ClassDef(UniDbDetector,1);
};

#endif
