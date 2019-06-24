// ----------------------------------------------------------------------
//                    UniDbDetectorParameterNew header file
// ----------------------------------------------------------------------

/** db_classes/UniDbDetectorParameterNew.h
 ** Class for the table: detector_parameter
 **/

#ifndef UNIDBDETECTORPARAMETERNEW_H
#define UNIDBDETECTORPARAMETERNEW_H 1

#include "TString.h"
#include "TDatime.h"
#include "TObjArray.h"

#include "UniDbConnection.h"
#include "UniDbSearchCondition.h"
#include "UniDbParameter.h"

#include <iostream>
using namespace std;

class UniDbDetectorParameterNew
{
 private:
	/* GENERATED PRIVATE MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	/// connection to the database
	UniDbConnection* connectionUniDb;

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
	UniDbDetectorParameterNew(UniDbConnection* connUniDb, int value_id, TString detector_name, int parameter_id, int start_period, int start_run, int end_period, int end_run, int value_key, unsigned char* parameter_value, Long_t size_parameter_value);
	/* END OF PRIVATE GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 public:
	/* GENERATED PUBLIC MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	virtual ~UniDbDetectorParameterNew(); // Destructor

	// static class functions
	/// add new detector parameter new to the database
	static UniDbDetectorParameterNew* CreateDetectorParameterNew(TString detector_name, int parameter_id, int start_period, int start_run, int end_period, int end_run, int value_key, unsigned char* parameter_value, Long_t size_parameter_value);
	/// get detector parameter new from the database
	static UniDbDetectorParameterNew* GetDetectorParameterNew(int value_id);
	/// check detector parameter new exists in the database
	static bool CheckDetectorParameterNewExists(int value_id);
	/// delete detector parameter new from the database
	static int DeleteDetectorParameterNew(int value_id);
	/// print all detector parameter news
	static int PrintAll();

	// Getters
	/// get value id of the current detector parameter new
	int GetValueId() {return i_value_id;}
	/// get detector name of the current detector parameter new
	TString GetDetectorName() {return str_detector_name;}
	/// get parameter id of the current detector parameter new
	int GetParameterId() {return i_parameter_id;}
	/// get start period of the current detector parameter new
	int GetStartPeriod() {return i_start_period;}
	/// get start run of the current detector parameter new
	int GetStartRun() {return i_start_run;}
	/// get end period of the current detector parameter new
	int GetEndPeriod() {return i_end_period;}
	/// get end run of the current detector parameter new
	int GetEndRun() {return i_end_run;}
	/// get value key of the current detector parameter new
	int GetValueKey() {return i_value_key;}
	/// get parameter value of the current detector parameter new
	unsigned char* GetParameterValue() {unsigned char* tmp_parameter_value = new unsigned char[sz_parameter_value]; memcpy(tmp_parameter_value, blob_parameter_value, sz_parameter_value); return tmp_parameter_value;}
	/// get size of parameter value of the current detector parameter new
	Long_t GetParameterValueSize() {return sz_parameter_value;}

	// Setters
	/// set detector name of the current detector parameter new
	int SetDetectorName(TString detector_name);
	/// set parameter id of the current detector parameter new
	int SetParameterId(int parameter_id);
	/// set start period of the current detector parameter new
	int SetStartPeriod(int start_period);
	/// set start run of the current detector parameter new
	int SetStartRun(int start_run);
	/// set end period of the current detector parameter new
	int SetEndPeriod(int end_period);
	/// set end run of the current detector parameter new
	int SetEndRun(int end_run);
	/// set value key of the current detector parameter new
	int SetValueKey(int value_key);
	/// set parameter value of the current detector parameter new
	int SetParameterValue(unsigned char* parameter_value, Long_t size_parameter_value);

	/// print information about current detector parameter new
	void Print();
	/* END OF PUBLIC GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

    // non-user function for writing parameter value (integer value key is optional, default, 0)
    static UniDbDetectorParameterNew* CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run,
                                                           unsigned char* parameter_value, Long_t size_parameter_value, enumParameterTypeNew enum_parameter_type, int value_key = 0);

    // non-user function for getting parameter value as a binary (char) array
    unsigned char* GetUNC(enumParameterTypeNew enum_parameter_type = UndefinedType);

    // non-user function for setting parameter value as a binary (char) array
    int SetUNC(unsigned char* p_parameter_value, Long_t size_parameter_value);


    /// write detector parameter value (integer value key is optional, default, 0)
    UniDbDetectorParameterNew* CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run,
                                                    vector<UniDbParameterValue*> parameter_value, int value_key = 0);

    /// get detector parameter value (integer value key is optional, default, 0)
    static UniDbDetectorParameterNew* GetDetectorParameter(TString detector_name, TString parameter_name, int period_number, int run_number, int value_key = 0);

    /// delete detector parameter value (integer value key is optional, default, 0)
    static int DeleteDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run, int value_key = 0);

    /// get value of detector parameter
    int GetParameterValue(vector<UniDbParameterValue*>& parameter_value);
    /// set value to detector parameter
    int SetParameterValue(vector<UniDbParameterValue*> parameter_value);

    /// write a whole file with parameter value (with given detector and parameter names) to the Database from start run to end one
    static int WriteFile(const char* detector_name, const char* parameter_name, int start_period, int start_run, int end_period, int end_run, const char* file_path);
    /// read a whole file with parameter value (with given detector and parameter names) from the Database for the selected run
    static int ReadFile(const char* detector_name, const char* parameter_name, int period_number, int run_number, const char* file_path);

    /// get parameters' values corresponding to the specified single condition
    static TObjArray* Search(const UniDbSearchCondition& search_condition);
    /// get parameters' values corresponding to the specified (vector) conditions
    static TObjArray* Search(const TObjArray& search_conditions);

    /// get parameter name of the current parameter value
    TString GetParameterName();
    /// get parameter type of the current parameter value
    enumParameterTypeNew GetParameterType();

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

    ClassDef(UniDbDetectorParameterNew,1);
};

#endif
