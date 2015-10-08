// ----------------------------------------------------------------------
//                    UniDbDetectorParameter header file 
//                      Generated 15-09-2015 
// ----------------------------------------------------------------------

/** db_classes/UniDbDetectorParameter.h 
 ** Class for the table: detector_parameter 
 **/ 

#ifndef UNIDBDETECTORPARAMETER_H
#define UNIDBDETECTORPARAMETER_H 1

#include "TString.h"
#include "TDatime.h"

#include "UniDbConnection.h"
#include "UniDbParameter.h"

class UniDbDetectorParameter
{
 private:
	/* GENERATED PRIVATE MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	/// connection to the database
	UniDbConnection* connectionUniDb;

	/// run number
	int i_run_number;
	/// detector name
	TString str_detector_name;
	/// parameter id
	int i_parameter_id;
	/// parameter value
	unsigned char* blob_parameter_value;
	/// size of parameter value
	Long_t sz_parameter_value;

	//Constructor
	UniDbDetectorParameter(UniDbConnection* connUniDb, int run_number, TString detector_name, int parameter_id, unsigned char* parameter_value, Long_t size_parameter_value);
	/* END OF PRIVATE GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 public:
	/* GENERATED PUBLIC MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	virtual ~UniDbDetectorParameter(); // Destructor

	// static class functions
	/// add new detector parameter to the database
	static UniDbDetectorParameter* CreateDetectorParameter(int run_number, TString detector_name, int parameter_id, unsigned char* parameter_value, Long_t size_parameter_value);
	/// get detector parameter from the database
	static UniDbDetectorParameter* GetDetectorParameter(int run_number, TString detector_name, int parameter_id);
	/// delete detector parameter from the database
	static int DeleteDetectorParameter(int run_number, TString detector_name, int parameter_id);
	/// print all detector parameters
	static int PrintAll();

	// Getters
	/// get run number of the current detector parameter
	int GetRunNumber() {return i_run_number;}
	/// get detector name of the current detector parameter
	TString GetDetectorName() {return str_detector_name;}
	/// get parameter id of the current detector parameter
	int GetParameterId() {return i_parameter_id;}
	/// get parameter value of the current detector parameter
	unsigned char* GetParameterValue() {unsigned char* tmp_parameter_value = new unsigned char[sz_parameter_value]; memcpy(tmp_parameter_value, blob_parameter_value, sz_parameter_value); return tmp_parameter_value;}
	/// get size of parameter value of the current detector parameter
	Long_t GetParameterValueSize() {return sz_parameter_value;}

	// Setters
	/// set run number of the current detector parameter
	int SetRunNumber(int run_number);
	/// set detector name of the current detector parameter
	int SetDetectorName(TString detector_name);
	/// set parameter id of the current detector parameter
	int SetParameterId(int parameter_id);
	/// set parameter value of the current detector parameter
	int SetParameterValue(unsigned char* parameter_value, Long_t size_parameter_value);
	/// print information about current detector parameter
	void Print();
	/* END OF PUBLIC GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

    // get detector parameter by run number, detector name and parameter name
    static UniDbDetectorParameter* GetDetectorParameter(int run_number, TString detector_name, TString parameter_name);

    // add new record - detector parameter value as BOOL
    static UniDbDetectorParameter* CreateDetectorParameter(int run_number, TString detector_name, TString parameter_name, bool parameter_value);
    // add new records - detector parameter value as BOOL for run range
    static bool CreateDetectorParameters(int start_run_number, int end_run_number, TString detector_name, TString parameter_name, bool parameter_value);
    // get boolean value of parameter (for current run, detector and parameter)
    bool GetBool();
    // set boolean value to parameter
    int SetBool(bool parameter_value);

    // add new record - detector parameter value as INTEGER
    static UniDbDetectorParameter* CreateDetectorParameter(int run_number, TString detector_name, TString parameter_name, int parameter_value);
    // get integer value of parameter (for current run, detector and parameter)
    int GetInt();
    // set integer value to parameter
    int SetInt(int parameter_value);

    // add new record - detector parameter value as DOUBLE
    static UniDbDetectorParameter* CreateDetectorParameter(int run_number, TString detector_name, TString parameter_name, double parameter_value);
    // get double value of parameter (for current run, detector and parameter)
    double GetDouble();
    // set double value to parameter
    int SetDouble(double parameter_value);

    // add new record - detector parameter value as STRING
    static UniDbDetectorParameter* CreateDetectorParameter(int run_number, TString detector_name, TString parameter_name, TString parameter_value);
    // get string value of parameter (for current run, detector and parameter)
    TString GetString();
    // set string value to parameter
    int SetString(TString parameter_value);

    // add new record - detector parameter value as Int+Int Array (e.g. "noise" - Slot:Channel)
    static UniDbDetectorParameter* CreateDetectorParameter(int run_number, TString detector_name, TString parameter_name, IIStructure* parameter_value, int element_count);
    // get Int+Int array for parameter (for current run, detector and parameter)
    int GetIIArray(IIStructure*& parameter_value, int& element_count);
    // set Int+Int array for parameter
    int SetIIArray(IIStructure* parameter_value, int element_count);

 ClassDef(UniDbDetectorParameter,1);
};

#endif
