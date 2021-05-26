// ----------------------------------------------------------------------
//                    UniDbDetectorParameter header file
// ----------------------------------------------------------------------

/** UniDbDetectorParameter.h
 ** Class for the table: detector_parameter
 **/

#ifndef UNIDBDETECTORPARAMETER_H
#define UNIDBDETECTORPARAMETER_H 1

#include "TString.h"
#include "TDatime.h"
#include "TObjArray.h"

#include "UniConnection.h"
#include "UniSearchCondition.h"
#include "UniDbParameter.h"

#include <iostream>
using namespace std;

class UniDbDetectorParameter
{
 private:
	/* GENERATED PRIVATE MEMBERS (SHOULD NOT BE CHANGED MANUALLY) */
	/// connection to the database
	UniConnection* connectionUniDb;

	/// value id
	int i_value_id;
	/// detector name
	TString str_detector_name;
	/// parameter id
	int i_parameter_id;
	/// start period
	int i_start_period;
	/// start run
	int i_start_run;
	/// end period
	int i_end_period;
	/// end run
	int i_end_run;
	/// value key
	int i_value_key;
	/// parameter value
	unsigned char* blob_parameter_value;
	/// size of parameter value
	Long_t sz_parameter_value;

	//Constructor
	UniDbDetectorParameter(UniConnection* connUniDb, int value_id, TString detector_name, int parameter_id, int start_period, int start_run, int end_period, int end_run, int value_key, unsigned char* parameter_value, Long_t size_parameter_value);
	/* END OF PRIVATE GENERATED PART (SHOULD NOT BE CHANGED MANUALLY) */

 public:
	/* GENERATED PUBLIC MEMBERS (SHOULD NOT BE CHANGED MANUALLY) */
	virtual ~UniDbDetectorParameter(); // Destructor

	// static class functions
	/// add new detector parameter to the database
	static UniDbDetectorParameter* CreateDetectorParameter(TString detector_name, int parameter_id, int start_period, int start_run, int end_period, int end_run, int value_key, unsigned char* parameter_value, Long_t size_parameter_value);
	/// get detector parameter from the database
	static UniDbDetectorParameter* GetDetectorParameter(int value_id);
	/// check detector parameter exists in the database
	static bool CheckDetectorParameterExists(int value_id);
	/// delete detector parameter from the database
	static int DeleteDetectorParameter(int value_id);
	/// print all detector parameters
	static int PrintAll();

	// Getters
	/// get value id of the current detector parameter
	int GetValueId() {return i_value_id;}
	/// get detector name of the current detector parameter
	TString GetDetectorName() {return str_detector_name;}
	/// get parameter id of the current detector parameter
	int GetParameterId() {return i_parameter_id;}
	/// get start period of the current detector parameter
	int GetStartPeriod() {return i_start_period;}
	/// get start run of the current detector parameter
	int GetStartRun() {return i_start_run;}
	/// get end period of the current detector parameter
	int GetEndPeriod() {return i_end_period;}
	/// get end run of the current detector parameter
	int GetEndRun() {return i_end_run;}
	/// get value key of the current detector parameter
	int GetValueKey() {return i_value_key;}
	/// get parameter value of the current detector parameter
	unsigned char* GetParameterValue() {unsigned char* tmp_parameter_value = new unsigned char[sz_parameter_value]; memcpy(tmp_parameter_value, blob_parameter_value, sz_parameter_value); return tmp_parameter_value;}
	/// get size of parameter value of the current detector parameter
	Long_t GetParameterValueSize() {return sz_parameter_value;}

	// Setters
	/// set detector name of the current detector parameter
	int SetDetectorName(TString detector_name);
	/// set parameter id of the current detector parameter
	int SetParameterId(int parameter_id);
	/// set start period of the current detector parameter
	int SetStartPeriod(int start_period);
	/// set start run of the current detector parameter
	int SetStartRun(int start_run);
	/// set end period of the current detector parameter
	int SetEndPeriod(int end_period);
	/// set end run of the current detector parameter
	int SetEndRun(int end_run);
	/// set value key of the current detector parameter
	int SetValueKey(int value_key);
	/// set parameter value of the current detector parameter
	int SetParameterValue(unsigned char* parameter_value, Long_t size_parameter_value);

	/// print information about current detector parameter
	void Print();
	/* END OF PUBLIC GENERATED PART (SHOULD NOT BE CHANGED MANUALLY) */

    /// write detector parameter value presented by an array (integer value key is optional, default, 0)
    static UniDbDetectorParameter* CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run,
                                                       vector<UniValue*> parameter_value, int value_key = 0);
    /// write detector parameter value presented by a single value (integer value key is optional, default, 0)
    static UniDbDetectorParameter* CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run,
                                                       UniValue* parameter_value, int value_key = 0);

    /// get detector parameter value (integer value key is optional, default, 0)
    static UniDbDetectorParameter* GetDetectorParameter(TString detector_name, TString parameter_name, int period_number, int run_number, int value_key = 0);

    /// delete detector parameter value (integer value key is optional, default, 0)
    static int DeleteDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run, int value_key = 0);

    /// get value of detector parameter presented by an array
    int GetValue(vector<UniValue*>& parameter_value);
    /// get value of detector parameter presented by a single value
    UniValue* GetValue();
    /// set value to detector parameter presented by an array
    int SetValue(vector<UniValue*> parameter_value);
    /// set value to detector parameter presented by a single value
    int SetValue(UniValue* parameter_value);

    /// write a whole file with parameter value (with given detector and parameter names) to the Database from start run to end one
    static int WriteFile(const char* detector_name, const char* parameter_name, int start_period, int start_run, int end_period, int end_run, const char* file_path);
    /// read a whole file with parameter value (with given detector and parameter names) from the Database for the selected run
    static int ReadFile(const char* detector_name, const char* parameter_name, int period_number, int run_number, const char* file_path);

    /// get parameters' values corresponding to the specified single condition and set owner for search_condition to kTRUE
    static TObjArray* Search(UniSearchCondition& search_condition);
    /// get parameters' values corresponding to the specified (vector) conditions and set owner for search_condition to kTRUE
    static TObjArray* Search(TObjArray& search_conditions);

    /// get parameter name of the current parameter value
    TString GetParameterName();
    /// get parameter type of the current parameter value
    enumValueType GetParameterType();

    /// get start period and run of the current parameter value
    void GetStart(int& start_period, int& start_run);
    /// get end period and run of the current parameter value
    void GetEnd(int& end_period, int& end_run);
    /// set start period and run of the current parameter value
    int SetStart(int start_period, int start_run);
    /// set end period and run of the current parameter value
    int SetEnd(int end_period, int end_run);

    /// Parse detector parameter's values and write them to the database:
    /// txtFile - path to the text file with parameter values separated by spaces or tabs, e.g.: "value1 value2..."
    ///           if line is started with '#' symbol - it will be skipped (as comments)
    /// detector_name - detector corresponding to the parameter values
    /// parameter name - parameter filled by values
    /// start_period - start period of validity range for the parameter values
    /// start_run - start run of validity range for the parameter values
    /// end_period - end period of validity range for the parameter values
    /// end_run - end run of validity range for the parameter values.
    /// Function returns row count added to the database.
    //static int ParseTxt(TString text_file, TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run);

 protected:
    // non-user function for getting parameter value as a binary (char) array
    unsigned char* GetUNC(enumValueType enum_parameter_type = UndefinedType);

    // non-user function for setting parameter value as a binary (char) array
    int SetUNC(unsigned char* p_parameter_value, Long_t size_parameter_value);

    // non-user function for writing parameter value (integer value key is optional, default, 0)
    static UniDbDetectorParameter* CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run,
                                                              unsigned char* parameter_value, Long_t size_parameter_value, enumValueType enum_parameter_type, int value_key = 0);

    ClassDef(UniDbDetectorParameter,1);
};

#endif
