// ----------------------------------------------------------------------
//                    MpdDbDetector header file 
//                      Generated 15-09-2015 
// ----------------------------------------------------------------------

/** db_classes/MpdDbDetector.h 
 ** Class for the table: detector_ 
 **/ 

#ifndef MPDDBDETECTOR_H 
#define MPDDBDETECTOR_H 1 

#include "TString.h"
#include "TDatime.h"

#include "MpdDbConnection.h"

class MpdDbDetector
{
 private:
	/* GENERATED PRIVATE MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	MpdDbConnection* connectionUniDb;

	TString str_detector_name;
	TString* str_manufacturer_name;
	TString* str_responsible_person;
	TString* str_description;

	//Constructor
	MpdDbDetector(MpdDbConnection* connUniDb, TString detector_name, TString* manufacturer_name, TString* responsible_person, TString* description);
	/* END OF PRIVATE GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 public:
	/* GENERATED PUBLIC MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	virtual ~MpdDbDetector(); // Destructor

	// static class functions
	static MpdDbDetector* CreateDetector(TString detector_name, TString* manufacturer_name, TString* responsible_person, TString* description);
	static MpdDbDetector* GetDetector(TString detector_name);
	static int DeleteDetector(TString detector_name);
	static int PrintAll();

	// Getters
	TString GetDetectorName() {return str_detector_name;}
	TString* GetManufacturerName() {if (str_manufacturer_name == NULL) return NULL; else return new TString(*str_manufacturer_name);}
	TString* GetResponsiblePerson() {if (str_responsible_person == NULL) return NULL; else return new TString(*str_responsible_person);}
	TString* GetDescription() {if (str_description == NULL) return NULL; else return new TString(*str_description);}

	// Setters
	int SetDetectorName(TString detector_name);
	int SetManufacturerName(TString* manufacturer_name);
	int SetResponsiblePerson(TString* responsible_person);
	int SetDescription(TString* description);
	void Print();
	/* END OF PUBLIC GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 ClassDef(MpdDbDetector,1);
};

#endif
