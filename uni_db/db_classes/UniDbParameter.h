// ----------------------------------------------------------------------
//                    UniDbParameter header file 
//                      Generated 05-11-2015 
// ----------------------------------------------------------------------

/** db_classes/UniDbParameter.h 
 ** Class for the table: parameter_ 
 **/ 

#ifndef UNIDBPARAMETER_H 
#define UNIDBPARAMETER_H 1 

#include "TString.h"
#include "TDatime.h"

#include "UniDbConnection.h"
#include "db_structures.h"

#include <iostream>
using namespace std;

class UniDbParameter
{
 private:
	/* GENERATED PRIVATE MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	/// connection to the database
	UniDbConnection* connectionUniDb;

	/// parameter id
	int i_parameter_id;
	/// parameter name
	TString str_parameter_name;
	/// parameter type
	int i_parameter_type;

	//Constructor
	UniDbParameter(UniDbConnection* connUniDb, int parameter_id, TString parameter_name, int parameter_type);
	/* END OF PRIVATE GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 public:
	/* GENERATED PUBLIC MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	virtual ~UniDbParameter(); // Destructor

	// static class functions
	/// add new parameter to the database
	static UniDbParameter* CreateParameter(TString parameter_name, int parameter_type);
	/// get parameter from the database
	static UniDbParameter* GetParameter(int parameter_id);
	/// get parameter from the database
	static UniDbParameter* GetParameter(TString parameter_name);
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

	// Setters
	/// set parameter name of the current parameter
	int SetParameterName(TString parameter_name);
	/// set parameter type of the current parameter
	int SetParameterType(int parameter_type);
	/// print information about current parameter
	void Print();
	/* END OF PUBLIC GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

    // enumeration 'enumParameterType' is corresponding parameter_type member UniDbParameter, you can see it in db_structures
    // enum enumParameterType{BoolType, IntType, DoubleType, StringType, IIArrayType, IntArrayType, DoubleArrayType, BinaryArrayType, ErrorType}
    static bool CheckAndGetParameterID(TSQLServer* uni_db, TString parameter_name, enumParameterType enum_parameter_type, int& parameter_id);

 ClassDef(UniDbParameter,1);
};

#endif
