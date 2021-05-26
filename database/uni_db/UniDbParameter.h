// ----------------------------------------------------------------------
//                    UniDbParameter header file 
//                      Generated 05-11-2015 
// ----------------------------------------------------------------------

/** UniDbParameter.h
 ** Class for the table: parameter_ 
 **/ 

#ifndef UNIDBPARAMETER_H 
#define UNIDBPARAMETER_H 1 

#include "TString.h"
#include "TDatime.h"

#include "UniConnection.h"
#include "db_structures.h"

#include <iostream>
using namespace std;

class UniDbParameter
{
 private:
	/* GENERATED PRIVATE MEMBERS (SHOULD NOT BE CHANGED MANUALLY) */
	/// connection to the database
	UniConnection* connectionUniDb;

	/// parameter id
	int i_parameter_id;
	/// parameter name
	TString str_parameter_name;
	/// parameter type
	int i_parameter_type;
	/// is array
	bool b_is_array;

	//Constructor
	UniDbParameter(UniConnection* connUniDb, int parameter_id, TString parameter_name, int parameter_type, bool is_array);
	/* END OF PRIVATE GENERATED PART (SHOULD NOT BE CHANGED MANUALLY) */

 public:
	/* GENERATED PUBLIC MEMBERS (SHOULD NOT BE CHANGED MANUALLY) */
	virtual ~UniDbParameter(); // Destructor

	// static class functions
	/// add new parameter to the database
	static UniDbParameter* CreateParameter(TString parameter_name, int parameter_type, bool is_array);
	/// get parameter from the database
	static UniDbParameter* GetParameter(int parameter_id);
	/// get parameter from the database
	static UniDbParameter* GetParameter(TString parameter_name);
	/// check parameter exists in the database
	static bool CheckParameterExists(int parameter_id);
	/// check parameter exists in the database
	static bool CheckParameterExists(TString parameter_name);
	/// delete parameter from the database
	static int DeleteParameter(int parameter_id);
	/// delete parameter from the database
	static int DeleteParameter(TString parameter_name);
	/// print all parameters
	static int PrintAll();

	// Getters
	/// get parameter id of the current parameter
	int GetParameterId() {return i_parameter_id;}
	/// get parameter name of the current parameter
	TString GetParameterName() {return str_parameter_name;}
	/// get parameter type of the current parameter
	int GetParameterType() {return i_parameter_type;}
	/// get is array of the current parameter
	bool GetIsArray() {return b_is_array;}

	// Setters
	/// set parameter name of the current parameter
	int SetParameterName(TString parameter_name);
	/// set parameter type of the current parameter
	int SetParameterType(int parameter_type);
	/// set is array of the current parameter
	int SetIsArray(bool is_array);

	/// print information about current parameter
	void Print();
	/* END OF PUBLIC GENERATED PART (SHOULD NOT BE CHANGED MANUALLY) */

    // enumeration 'enumValueType' is corresponding parameter_type member UniDbParameter, you can see it in db_structures
    static bool CheckAndGetParameterID(TSQLServer* uni_db, TString parameter_name, enumValueType enum_parameter_type, int& parameter_id);

 ClassDef(UniDbParameter,1);
};

#endif
