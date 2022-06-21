// ----------------------------------------------------------------------
//                    ElogDbType header file 
//                      Generated 27-11-2017 
// ----------------------------------------------------------------------

/** ElogDbType.h
 ** Class for the table: type_ 
 **/ 

#ifndef ELOGDBTYPE_H 
#define ELOGDBTYPE_H 1 

#include "TString.h"
#include "TDatime.h"

#include "ElogConnection.h"

class ElogDbType
{
 private:
	/* GENERATED PRIVATE MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	/// connection to the database
    ElogConnection* connectionDb;

	/// type id
	int i_type_id;
	/// type text
	TString str_type_text;

	//Constructor
    ElogDbType(ElogConnection* db_connect, int type_id, TString type_text);
	/* END OF PRIVATE GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 public:
	/* GENERATED PUBLIC MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	virtual ~ElogDbType(); // Destructor

	// static class functions
	/// add new type to the database
	static ElogDbType* CreateType(TString type_text);
	/// get type from the database
	static ElogDbType* GetType(int type_id);
	/// get type from the database
	static ElogDbType* GetType(TString type_text);
    /// check type exists in the database: 1 - true, 0 - false, <0 - database operation errors
    static int CheckTypeExists(int type_id);
    /// check type exists in the database: 1 - true, 0 - false, <0 - database operation errors
    static int CheckTypeExists(TString type_text);
	/// delete type from the database
	static int DeleteType(int type_id);
	/// delete type from the database
	static int DeleteType(TString type_text);
	/// print all types
	static int PrintAll();

	// Getters
	/// get type id of the current type
	int GetTypeId() {return i_type_id;}
	/// get type text of the current type
	TString GetTypeText() {return str_type_text;}

	// Setters
	/// set type text of the current type
	int SetTypeText(TString type_text);

	/// print information about current type
	void Print();
	/* END OF PUBLIC GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 ClassDef(ElogDbType,1);
};

#endif
