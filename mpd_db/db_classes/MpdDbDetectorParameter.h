// ----------------------------------------------------------------------
//                    MpdDbDetectorParameter header file 
//                      Generated 15-09-2015 
// ----------------------------------------------------------------------

/** db_classes/MpdDbDetectorParameter.h 
 ** Class for the table: detector_parameter 
 **/ 

#ifndef MPDDBDETECTORPARAMETER_H 
#define MPDDBDETECTORPARAMETER_H 1 

#include "TString.h"
#include "TDatime.h"

#include "MpdDbConnection.h"
#include "MpdDbParameter.h"

class MpdDbDetectorParameter
{
 private:
	/* GENERATED PRIVATE MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	MpdDbConnection* connectionUniDb;

	int i_run_number;
	TString str_detector_name;
	int i_parameter_id;
	unsigned char* blob_parameter_value;
	Long_t sz_parameter_value;

	//Constructor
	MpdDbDetectorParameter(MpdDbConnection* connUniDb, int run_number, TString detector_name, int parameter_id, unsigned char* parameter_value, Long_t size_parameter_value);
	/* END OF PRIVATE GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 public:
	/* GENERATED PUBLIC MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	virtual ~MpdDbDetectorParameter(); // Destructor

	// static class functions
	static MpdDbDetectorParameter* CreateDetectorParameter(int run_number, TString detector_name, int parameter_id, unsigned char* parameter_value, Long_t size_parameter_value);
	static MpdDbDetectorParameter* GetDetectorParameter(int run_number, TString detector_name, int parameter_id);
	static int DeleteDetectorParameter(int run_number, TString detector_name, int parameter_id);
	static int PrintAll();

	// Getters
	int GetRunNumber() {return i_run_number;}
	TString GetDetectorName() {return str_detector_name;}
	int GetParameterId() {return i_parameter_id;}
	unsigned char* GetParameterValue() {unsigned char* tmp_parameter_value = new unsigned char[sz_parameter_value]; memcpy(tmp_parameter_value, blob_parameter_value, sz_parameter_value); return tmp_parameter_value;}
	Long_t GetParameterValueSize() {return sz_parameter_value;}

	// Setters
	int SetRunNumber(int run_number);
	int SetDetectorName(TString detector_name);
	int SetParameterId(int parameter_id);
	int SetParameterValue(unsigned char* parameter_value, Long_t size_parameter_value);
	void Print();
	/* END OF PUBLIC GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

    // get detector parameter by run number, detector name and parameter name
    static MpdDbDetectorParameter* GetDetectorParameter(int run_number, TString detector_name, TString parameter_name);

    // add new record - detector parameter value as BOOL
    static MpdDbDetectorParameter* CreateDetectorParameter(int run_number, TString detector_name, TString parameter_name, bool parameter_value);
    // add new records - detector parameter value as BOOL for run range
    static bool CreateDetectorParameters(int start_run_number, int end_run_number, TString detector_name, TString parameter_name, bool parameter_value);
    // get boolean value of parameter (for current run, detector and parameter)
    bool GetBool();
    // set boolean value to parameter
    int SetBool(bool parameter_value);

    // add new record - detector parameter value as INTEGER
    static MpdDbDetectorParameter* CreateDetectorParameter(int run_number, TString detector_name, TString parameter_name, int parameter_value);
    // get integer value of parameter (for current run, detector and parameter)
    int GetInt();
    // set integer value to parameter
    int SetInt(int parameter_value);

    // add new record - detector parameter value as DOUBLE
    static MpdDbDetectorParameter* CreateDetectorParameter(int run_number, TString detector_name, TString parameter_name, double parameter_value);
    // get double value of parameter (for current run, detector and parameter)
    double GetDouble();
    // set double value to parameter
    int SetDouble(double parameter_value);

    // add new record - detector parameter value as STRING
    static MpdDbDetectorParameter* CreateDetectorParameter(int run_number, TString detector_name, TString parameter_name, TString parameter_value);
    // get string value of parameter (for current run, detector and parameter)
    TString GetString();
    // set string value to parameter
    int SetString(TString parameter_value);

    // add new record - detector parameter value as Int+Int Array (e.g. "noise" - Slot:Channel)
    static MpdDbDetectorParameter* CreateDetectorParameter(int run_number, TString detector_name, TString parameter_name, IIStructure* parameter_value, int element_count);
    // get Int+Int array for parameter (for current run, detector and parameter)
    int GetIIArray(IIStructure*& parameter_value, int& element_count);
    // set Int+Int array for parameter
    int SetIIArray(IIStructure* parameter_value, int element_count);

 ClassDef(MpdDbDetectorParameter,1);
};

#endif
