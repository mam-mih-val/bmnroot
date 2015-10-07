// ----------------------------------------------------------------------
//                    UniDbParameter header file 
//                      Generated 15-09-2015 
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

// enumeration 'enumParameterType' is corresponding parameter_type member UniDbParameter
// 0 - boolean, 1 - integer, 2 - double, 3 - string, 4 - int+int array
enum enumParameterType{BoolType, IntType, DoubleType, StringType, IIArrayType};

class UniDbParameter
{
 private:
	/* GENERATED PRIVATE MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	UniDbConnection* connectionUniDb;

	int i_parameter_id;
	TString str_parameter_name;
	int i_parameter_type;

	//Constructor
	UniDbParameter(UniDbConnection* connUniDb, int parameter_id, TString parameter_name, int parameter_type);
	/* END OF PRIVATE GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 public:
	/* GENERATED PUBLIC MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	virtual ~UniDbParameter(); // Destructor

	// static class functions
	static UniDbParameter* CreateParameter(TString parameter_name, int parameter_type);
	static UniDbParameter* GetParameter(int parameter_id);
	static UniDbParameter* GetParameter(TString parameter_name);
	static int DeleteParameter(int parameter_id);
	static int DeleteParameter(TString parameter_name);
	static int PrintAll();

	// Getters
	int GetParameterId() {return i_parameter_id;}
	TString GetParameterName() {return str_parameter_name;}
	int GetParameterType() {return i_parameter_type;}

	// Setters
	int SetParameterId(int parameter_id);
	int SetParameterName(TString parameter_name);
	int SetParameterType(int parameter_type);
	void Print();
	/* END OF PUBLIC GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 ClassDef(UniDbParameter,1);
};

#endif
