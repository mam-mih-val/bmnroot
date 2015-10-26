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
#include "UniDbParameter.h"

class UniDbSessionParameter
{
 private:
	/* GENERATED PRIVATE MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	/// connection to the database
	UniDbConnection* connectionUniDb;

	/// session number
	int i_session_number;
	/// detector name
	TString str_detector_name;
	/// parameter id
	int i_parameter_id;
	/// parameter value
	unsigned char* blob_parameter_value;
	/// size of parameter value
	Long_t sz_parameter_value;

	//Constructor
	UniDbSessionParameter(UniDbConnection* connUniDb, int session_number, TString detector_name, int parameter_id, unsigned char* parameter_value, Long_t size_parameter_value);
	/* END OF PRIVATE GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 public:
	/* GENERATED PUBLIC MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	virtual ~UniDbSessionParameter(); // Destructor

	// static class functions
	/// add new session parameter to the database
	static UniDbSessionParameter* CreateSessionParameter(int session_number, TString detector_name, int parameter_id, unsigned char* parameter_value, Long_t size_parameter_value);
	/// get session parameter from the database
	static UniDbSessionParameter* GetSessionParameter(int session_number, TString detector_name, int parameter_id);
	/// delete session parameter from the database
	static int DeleteSessionParameter(int session_number, TString detector_name, int parameter_id);
	/// print all session parameters
	static int PrintAll();

	// Getters
	/// get session number of the current session parameter
	int GetSessionNumber() {return i_session_number;}
	/// get detector name of the current session parameter
	TString GetDetectorName() {return str_detector_name;}
	/// get parameter id of the current session parameter
	int GetParameterId() {return i_parameter_id;}
	/// get parameter value of the current session parameter
	unsigned char* GetParameterValue() {unsigned char* tmp_parameter_value = new unsigned char[sz_parameter_value]; memcpy(tmp_parameter_value, blob_parameter_value, sz_parameter_value); return tmp_parameter_value;}
	/// get size of parameter value of the current session parameter
	Long_t GetParameterValueSize() {return sz_parameter_value;}

	// Setters
	/// set session number of the current session parameter
	int SetSessionNumber(int session_number);
	/// set detector name of the current session parameter
	int SetDetectorName(TString detector_name);
	/// set parameter id of the current session parameter
	int SetParameterId(int parameter_id);
	/// set parameter value of the current session parameter
	int SetParameterValue(unsigned char* parameter_value, Long_t size_parameter_value);
	/// print information about current session parameter
	void Print();
	/* END OF PUBLIC GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

    /// get detector parameter by run number, detector name and parameter name
    static UniDbSessionParameter* GetSessionParameter(int session_number, TString detector_name, TString parameter_name);

    // common function for creating parameter
    static UniDbSessionParameter* CreateSessionParameter(int session_number, TString detector_name, TString parameter_name, unsigned char* p_parameter_value, Long_t size_parameter_value, enumParameterType enum_parameter_type);
    // common function for getting parameter
    unsigned char* GetUNC(enumParameterType enum_parameter_type);
    // common function for setting parameter
    int SetUNC(unsigned char* p_parameter_value, Long_t size_parameter_value);

    /// add new record - detector parameter value as BOOL
    static UniDbSessionParameter* CreateSessionParameter(int session_number, TString detector_name, TString parameter_name, bool parameter_value);
    /// get boolean value of parameter (for current session, detector and parameter)
    bool GetBool();
    /// set boolean value to parameter
    int SetBool(bool parameter_value);

    /// add new record - detector parameter value as INTEGER
    static UniDbSessionParameter* CreateSessionParameter(int session_number, TString detector_name, TString parameter_name, int parameter_value);
    /// get integer value of parameter (for current session, detector and parameter)
    int GetInt();
    /// set integer value to parameter
    int SetInt(int parameter_value);

    /// add new record - detector parameter value as DOUBLE
    static UniDbSessionParameter* CreateSessionParameter(int session_number, TString detector_name, TString parameter_name, double parameter_value);
    /// get double value of parameter (for current session, detector and parameter)
    double GetDouble();
    /// set double value to parameter
    int SetDouble(double parameter_value);

    /// add new record - detector parameter value as STRING
    static UniDbSessionParameter* CreateSessionParameter(int session_number, TString detector_name, TString parameter_name, TString parameter_value);
    /// get string value of parameter (for current session, detector and parameter)
    TString GetString();
    /// set string value to parameter
    int SetString(TString parameter_value);

    /// add new record - detector parameter value as Integer Array
    static UniDbSessionParameter* CreateSessionParameter(int session_number, TString detector_name, TString parameter_name, int* parameter_value, int element_count);
    /// get Integer Array for parameter (for current session, detector and parameter)
    int GetIntArray(int*& parameter_value, int& element_count);
    /// set Integer Array array for parameter
    int SetIntArray(int* parameter_value, int element_count);

    /// add new record - detector parameter value as Double Array
    static UniDbSessionParameter* CreateSessionParameter(int session_number, TString detector_name, TString parameter_name, double* parameter_value, int element_count);
    /// get Double Array for parameter (for current session, detector and parameter)
    int GetDoubleArray(double*& parameter_value, int& element_count);
    /// set Double Array array for parameter
    int SetDoubleArray(double* parameter_value, int element_count);

    /// add new record - detector parameter value as Int+Int Array (e.g. "noise" - Slot:Channel)
    static UniDbSessionParameter* CreateSessionParameter(int session_number, TString detector_name, TString parameter_name, IIStructure* parameter_value, int element_count);
    /// get Int+Int array for parameter (for current session, detector and parameter)
    int GetIIArray(IIStructure*& parameter_value, int& element_count);
    /// set Int+Int array for parameter
    int SetIIArray(IIStructure* parameter_value, int element_count);

 ClassDef(UniDbSessionParameter,1);
};

#endif
