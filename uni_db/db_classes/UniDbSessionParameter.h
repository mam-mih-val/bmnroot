// ----------------------------------------------------------------------
//                    UniDbSessionParameter header file 
//                      Generated 20-10-2015 
// ----------------------------------------------------------------------

/** db_classes/UniDbSessionParameter.h 
 ** Class for the table: session_parameter 
 **/ 

#ifndef UNIDBSESSIONPARAMETER_H 
#define UNIDBSESSIONPARAMETER_H 1 

#include "TString.h"
#include "TDatime.h"

#include "UniDbConnection.h"

class UniDbSessionParameter
{
 private:
	/* GENERATED PRIVATE MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	/// connection to the database
	UniDbConnection* connectionUniDb;

	/// detector name
	TString str_detector_name;
	/// session number
	int i_session_number;
	/// parameter id
	int i_parameter_id;
	/// parameter value
	unsigned char* blob_parameter_value;
	/// size of parameter value
	Long_t sz_parameter_value;

	//Constructor
	UniDbSessionParameter(UniDbConnection* connUniDb, TString detector_name, int session_number, int parameter_id, unsigned char* parameter_value, Long_t size_parameter_value);
	/* END OF PRIVATE GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 public:
	/* GENERATED PUBLIC MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	virtual ~UniDbSessionParameter(); // Destructor

	// static class functions
	/// add new session parameter to the database
	static UniDbSessionParameter* CreateSessionParameter(TString detector_name, int session_number, int parameter_id, unsigned char* parameter_value, Long_t size_parameter_value);
	/// get session parameter from the database
	static UniDbSessionParameter* GetSessionParameter(TString detector_name, int session_number, int parameter_id);
	/// delete session parameter from the database
	static int DeleteSessionParameter(TString detector_name, int session_number, int parameter_id);
	/// print all session parameters
	static int PrintAll();

	// Getters
	/// get detector name of the current session parameter
	TString GetDetectorName() {return str_detector_name;}
	/// get session number of the current session parameter
	int GetSessionNumber() {return i_session_number;}
	/// get parameter id of the current session parameter
	int GetParameterId() {return i_parameter_id;}
	/// get parameter value of the current session parameter
	unsigned char* GetParameterValue() {unsigned char* tmp_parameter_value = new unsigned char[sz_parameter_value]; memcpy(tmp_parameter_value, blob_parameter_value, sz_parameter_value); return tmp_parameter_value;}
	/// get size of parameter value of the current session parameter
	Long_t GetParameterValueSize() {return sz_parameter_value;}

	// Setters
	/// set detector name of the current session parameter
	int SetDetectorName(TString detector_name);
	/// set session number of the current session parameter
	int SetSessionNumber(int session_number);
	/// set parameter id of the current session parameter
	int SetParameterId(int parameter_id);
	/// set parameter value of the current session parameter
	int SetParameterValue(unsigned char* parameter_value, Long_t size_parameter_value);
	/// print information about current session parameter
	void Print();
	/* END OF PUBLIC GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 ClassDef(UniDbSessionParameter,1);
};

#endif
