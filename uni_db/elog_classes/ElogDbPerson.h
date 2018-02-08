// ----------------------------------------------------------------------
//                    ElogDbPerson header file 
//                      Generated 27-11-2017 
// ----------------------------------------------------------------------

/** elog_classes/ElogDbPerson.h 
 ** Class for the table: person_ 
 **/ 

#ifndef ELOGDBPERSON_H 
#define ELOGDBPERSON_H 1 

#include "TString.h"
#include "TDatime.h"

#include "UniDbConnection.h"

class ElogDbPerson
{
 private:
	/* GENERATED PRIVATE MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	/// connection to the database
	UniDbConnection* connectionUniDb;

	/// person id
	int i_person_id;
	/// person name
	TString str_person_name;
	/// is active
	int i_is_active;

	//Constructor
	ElogDbPerson(UniDbConnection* connUniDb, int person_id, TString person_name, int is_active);
	/* END OF PRIVATE GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 public:
	/* GENERATED PUBLIC MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	virtual ~ElogDbPerson(); // Destructor

	// static class functions
	/// add new person to the database
	static ElogDbPerson* CreatePerson(TString person_name, int is_active);
	/// get person from the database
	static ElogDbPerson* GetPerson(int person_id);
	/// get person from the database
	static ElogDbPerson* GetPerson(TString person_name);
	/// check person exists in the database
	static bool CheckPersonExists(int person_id);
	/// check person exists in the database
	static bool CheckPersonExists(TString person_name);
	/// delete person from the database
	static int DeletePerson(int person_id);
	/// delete person from the database
	static int DeletePerson(TString person_name);
	/// print all persons
	static int PrintAll();

	// Getters
	/// get person id of the current person
	int GetPersonId() {return i_person_id;}
	/// get person name of the current person
	TString GetPersonName() {return str_person_name;}
	/// get is active of the current person
	int GetIsActive() {return i_is_active;}

	// Setters
	/// set person name of the current person
	int SetPersonName(TString person_name);
	/// set is active of the current person
	int SetIsActive(int is_active);

	/// print information about current person
	void Print();
	/* END OF PUBLIC GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 ClassDef(ElogDbPerson,1);
};

#endif
