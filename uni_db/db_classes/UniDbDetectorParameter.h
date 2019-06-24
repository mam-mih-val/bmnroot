// ----------------------------------------------------------------------
//                    UniDbDetectorParameter header file 
//                      Generated 05-11-2015 
// ----------------------------------------------------------------------

/** db_classes/UniDbDetectorParameter.h 
 ** Class for the table: detector_parameter
 **/

#ifndef UNIDBDETECTORPARAMETER_H 
#define UNIDBDETECTORPARAMETER_H 1 

#include "TString.h"
#include "TDatime.h"
#include "TObjArray.h"

#include "UniDbConnection.h"
#include "UniDbSearchCondition.h"
#include "UniDbParameter.h"

// for templates
//#include <string>
//#include <cstdlib>
//#include <cxxabi.h>

#include <iostream>
using namespace std;

class UniDbDetectorParameter
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
	/// dc serial
	unsigned int* ui_dc_serial;
	/// channel
	int* i_channel;
	/// parameter value
	unsigned char* blob_parameter_value;
	/// size of parameter value
	Long_t sz_parameter_value;

	//Constructor
	UniDbDetectorParameter(UniDbConnection* connUniDb, int value_id, TString detector_name, int parameter_id, int start_period, int start_run, int end_period, int end_run, unsigned int* dc_serial, int* channel, unsigned char* parameter_value, Long_t size_parameter_value);
	/* END OF PRIVATE GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

	// for templates
	enumParameterType GetParameterTypeByString(string type_name, bool is_array);

 public:
	/* GENERATED PUBLIC MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	virtual ~UniDbDetectorParameter(); // Destructor

	// static class functions
	/// add new detector parameter to the database
	static UniDbDetectorParameter* CreateDetectorParameter(TString detector_name, int parameter_id, int start_period, int start_run, int end_period, int end_run, unsigned int* dc_serial, int* channel, unsigned char* parameter_value, Long_t size_parameter_value);
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
	/// get dc serial of the current detector parameter
	unsigned int* GetDcSerial() {if (ui_dc_serial == NULL) return NULL; else return new unsigned int(*ui_dc_serial);}
	/// get channel of the current detector parameter
	int* GetChannel() {if (i_channel == NULL) return NULL; else return new int(*i_channel);}
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
	/// set dc serial of the current detector parameter
	int SetDcSerial(unsigned int* dc_serial);
	/// set channel of the current detector parameter
	int SetChannel(int* channel);
	/// set parameter value of the current detector parameter
	int SetParameterValue(unsigned char* parameter_value, Long_t size_parameter_value);

	/// print information about current detector parameter
	void Print();
	/* END OF PUBLIC GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

    /// get common detector parameter value
    static UniDbDetectorParameter* GetDetectorParameter(TString detector_name, TString parameter_name, int period_number, int run_number);
	/// get TDC/ADC parameter value
    static UniDbDetectorParameter* GetDetectorParameter(TString detector_name, TString parameter_name, int period_number, int run_number, unsigned int dc_serial, int channel);

    /// delete common detector parameter value
    static int DeleteDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run);
    /// delete TDC/ADC parameter value
    static int DeleteDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run, unsigned int dc_serial, int channel);

	/// get channel count for TDC/ADC parameter value
    static int GetChannelCount(TString detector_name, TString parameter_name, int period_number, int run_number, unsigned int dc_serial);

    /// get parameter name of the current detector parameter
    TString GetParameterName();

    /// get start period and run of the current detector parameter
    void GetStart(int& start_period, int& start_run);
    /// get end period and run of the current detector parameter
    void GetEnd(int& end_period, int& end_run);
    /// set start period and run of the current detector parameter
    int SetStart(int start_period, int start_run);
    /// set end period and run of the current detector parameter
    int SetEnd(int end_period, int end_run);

    /// write ROOT file with binary detector parameter (with given detector and parameter names)
    /// to the database from start run_number to end run_number
    static int WriteRootFile(int start_period, int start_run, int end_period, int end_run, const char* detector_name, const char* parameter_name, const char* root_file_path);
    /// read ROOT file with binary detector parameter (with given detector and parameter names)
    /// from the database for a selected run number
    static int ReadRootFile(int period_number, int run_number, const char* detector_name, const char* parameter_name, const char* root_file_path);

	// common function for adding common parameter value
    static UniDbDetectorParameter* CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run,
                                                               unsigned char* parameter_value, Long_t size_parameter_value, enumParameterType enum_parameter_type);
	// common function for adding TDC/ADC parameter value
    static UniDbDetectorParameter* CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run,
                                                               unsigned int dc_serial, int channel, unsigned char* parameter_value, Long_t size_parameter_value, enumParameterType enum_parameter_type);

	/* template function for adding common parameter value as single value
		template<class T>
                static UniDbDetectorParameter* CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run, T parameter_value)
		{
			// define string name of T class type
			int status;
			string tname = typeid(T).name();
			char* demangled_name = abi::__cxa_demangle(tname.c_str(), NULL, NULL, &status);
			if (status == 0)
			{
				tname = demangled_name;
				free(demangled_name);
			}
			enumParameterType enum_parameter_type = GetParameterTypeByString(tname, false);

			// copy parameter value to new binary array
			Long_t size_parameter_value = sizeof(T);
			T* p_parameter_value = new T[1];
			p_parameter_value[0] = parameter_value;

			UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name, start_run, end_run,
																										 (unsigned char*)p_parameter_value, size_parameter_value, enum_parameter_type);
			if (pDetectorParameter == 0x00)
				delete [] p_parameter_value;

			return pDetectorParameter;
		}*/

	/* template function for adding common parameter value presented as array (common)
		template<class T>
                static UniDbDetectorParameter* CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run, T* parameter_value, Long_t element_count, enumParameterType enum_parameter_type)
		{
			// define string name of T class type
			int status;
			string tname = typeid(T).name();
			char* demangled_name = abi::__cxa_demangle(tname.c_str(), NULL, NULL, &status);
			if (status == 0)
			{
				tname = demangled_name;
				free(demangled_name);
			}
			enumParameterType enum_parameter_type = GetParameterTypeByString(tname, true);

			Long_t size_parameter_value = element_count * sizeof(T);
			unsigned char* p_parameter_value = new unsigned char[size_parameter_value];
			memcpy(p_parameter_value, parameter_value, size_parameter_value);

			UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name, start_run, end_run,
																										 p_parameter_value, size_parameter_value, enum_parameter_type);
			if (pDetectorParameter == 0x00)
				delete [] p_parameter_value;

			return pDetectorParameter;
		}*/

	// common function for getting parameter value
    unsigned char* GetUNC(enumParameterType enum_parameter_type, bool isCheckType = false);

	// common function for setting parameter value
	int SetUNC(unsigned char* p_parameter_value, Long_t size_parameter_value);

	// function set with different types
    /// add new record - detector parameter value as BOOL
    static UniDbDetectorParameter* CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run,
                                                               bool parameter_value);
    /// add new record - TDC/ADC parameter value as BOOL
    static UniDbDetectorParameter* CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run,
                                                               unsigned int dc_serial, int channel, bool parameter_value);
	/// get boolean value of parameter
	bool GetBool();
	/// set boolean value to parameter
	int SetBool(bool parameter_value);

    /// add new record - detector parameter value as INTEGER
    static UniDbDetectorParameter* CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run,
                                                               int parameter_value);
    /// add new record - TDC/ADC parameter value as INTEGER
    static UniDbDetectorParameter* CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run,
                                                               unsigned int dc_serial, int channel, int parameter_value);
	/// get integer value of parameter
	int GetInt();
	/// set integer value to parameter
	int SetInt(int parameter_value);

	/// add new record - detector parameter value as DOUBLE
    static UniDbDetectorParameter* CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run,
                                                               double parameter_value);
	/// add new record - TDC/ADC parameter value as DOUBLE
    static UniDbDetectorParameter* CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run,
                                                               unsigned int dc_serial, int channel, double parameter_value);
	/// get double value of parameter
	double GetDouble();
	/// set double value to parameter
	int SetDouble(double parameter_value);

	/// add new record - detector parameter value as STRING
    static UniDbDetectorParameter* CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run,
                                                               TString parameter_value);
	/// add new record - detector parameter value as STRING
    static UniDbDetectorParameter* CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run,
                                                               unsigned int dc_serial, int channel, TString parameter_value);
	/// get string value of parameter
	TString GetString();
	/// set string value to parameter
	int SetString(TString parameter_value);

	/// add new record - detector parameter value as Integer Array
    static UniDbDetectorParameter* CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run,
                                                               int* parameter_value, int element_count);
	/// add new record - detector parameter value as Integer Array
    static UniDbDetectorParameter* CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run,
                                                               unsigned int dc_serial, int channel, int* parameter_value, int element_count);
	/// get Integer Array for parameter
	int GetIntArray(int*& parameter_value, int& element_count);
	/// set Integer Array array for parameter
	int SetIntArray(int* parameter_value, int element_count);

	/// add new record - detector parameter value as Double Array
    static UniDbDetectorParameter* CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run,
                                                               double* parameter_value, int element_count);
	/// add new record - detector parameter value as Double Array
    static UniDbDetectorParameter* CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run,
                                                               unsigned int dc_serial, int channel, double* parameter_value, int element_count);
	/// get Double Array for parameter
	int GetDoubleArray(double*& parameter_value, int& element_count);
	/// set Double Array array for parameter
	int SetDoubleArray(double* parameter_value, int element_count);

    /// add new record - detector parameter value as Binary Array
    static UniDbDetectorParameter* CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run,
                                                               unsigned char* parameter_value, int byte_count);
    /// add new record - detector parameter value as Binary Array
    static UniDbDetectorParameter* CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run,
                                                               unsigned int dc_serial, int channel, unsigned char* parameter_value, int byte_count);
    /// get Binary Array for parameter
    int GetBinaryArray(unsigned char*& parameter_value, size_t& byte_count);
    /// set Binary Array array for parameter
    int SetBinaryArray(unsigned char* parameter_value, size_t byte_count);

	/// add new record - detector parameter value as Int+Int Array
    static UniDbDetectorParameter* CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run,
                                                               IIStructure* parameter_value, int element_count);
	/// add new record - detector parameter value as Int+Int Array
    static UniDbDetectorParameter* CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run,
                                                               unsigned int dc_serial, int channel, IIStructure* parameter_value, int element_count);
	/// get Int+Int array for parameter
	int GetIIArray(IIStructure*& parameter_value, int& element_count);
	/// set Int+Int array for parameter
	int SetIIArray(IIStructure* parameter_value, int element_count);

    /// add new record - detector parameter value as Unsigned Integer Array
    static UniDbDetectorParameter* CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run,
                                                               unsigned int* parameter_value, int element_count);
    /// add new record - detector parameter value as Unsigned Integer Array
    static UniDbDetectorParameter* CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run,
                                                               unsigned int dc_serial, int channel, unsigned int* parameter_value, int element_count);
    /// get Unsigned Integer Array for parameter
    int GetUIntArray(unsigned int*& parameter_value, int& element_count);
    /// set Unsigned Integer Array array for parameter
    int SetUIntArray(unsigned int* parameter_value, int element_count);

    /// add new record - detector parameter value as DCH mapping Array
    static UniDbDetectorParameter* CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run,
                                                               DchMapStructure* parameter_value, int element_count);
    /// add new record - detector parameter value as DCH mapping Array
    static UniDbDetectorParameter* CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run,
                                                               unsigned int dc_serial, int channel, DchMapStructure* parameter_value, int element_count);
    /// get DCH mapping Array for parameter
    int GetDchMapArray(DchMapStructure*& parameter_value, int& element_count);
    /// set DCH mapping Array for parameter
    int SetDchMapArray(DchMapStructure* parameter_value, int element_count);

    /// add new record - detector parameter value as GEM mapping Array
    static UniDbDetectorParameter* CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run,
                                                               GemMapStructure* parameter_value, int element_count);
    /// add new record - detector parameter value as GEM mapping Array
    static UniDbDetectorParameter* CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run,
                                                               unsigned int dc_serial, int channel, GemMapStructure* parameter_value, int element_count);
    /// get GEM mapping Array for parameter
    int GetGemMapArray(GemMapStructure*& parameter_value, int& element_count);
    /// set GEM mapping Array for parameter
    int SetGemMapArray(GemMapStructure* parameter_value, int element_count);

    /// add new record - detector parameter value as GEM pedestal map Array
    static UniDbDetectorParameter* CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run,
                                                           GemPedestalStructure* parameter_value, int element_count);
    /// add new record - detector parameter value as GEM pedestal map Array
    static UniDbDetectorParameter* CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run,
                                                           unsigned int dc_serial, int channel, GemPedestalStructure* parameter_value, int element_count);
    /// get GEM pedestal map Array for parameter
    int GetGemPedestalArray(GemPedestalStructure*& parameter_value, int& element_count);
    /// set GEM pedestal map Array for parameter
    int SetGemPedestalArray(GemPedestalStructure* parameter_value, int element_count);

    /// add new record - detector parameter value as Trigger mapping Array
    static UniDbDetectorParameter* CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run,
                                                           TriggerMapStructure* parameter_value, int element_count);
    /// add new record - detector parameter value as Trigger mapping Array
    static UniDbDetectorParameter* CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run,
                                                           unsigned int dc_serial, int channel, TriggerMapStructure* parameter_value, int element_count);
    /// get Trigger mapping Array for parameter
    int GetTriggerMapArray(TriggerMapStructure*& parameter_value, int& element_count);
    /// set Trigger mapping Array for parameter
    int SetTriggerMapArray(TriggerMapStructure* parameter_value, int element_count);

    /// add new record - detector parameter value as Lorents Shift Array
    static UniDbDetectorParameter* CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run,
                                                           LorentzShiftStructure* parameter_value, int element_count);
    /// add new record - detector parameter value as Lorents Shift Array
    static UniDbDetectorParameter* CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run,
                                                           unsigned int dc_serial, int channel, LorentzShiftStructure* parameter_value, int element_count);
    /// get Lorents Shift Array for parameter
    int GetLorentzShiftArray(LorentzShiftStructure*& parameter_value, int& element_count);
    /// set Lorents Shift Array for parameter
    int SetLorentzShiftArray(LorentzShiftStructure* parameter_value, int element_count);

	/// get parameters' values corresponding to the specified single condition
	static TObjArray* Search(const UniDbSearchCondition& search_condition);
	/// get parameters' values corresponding to the specified (vector) conditions
	static TObjArray* Search(const TObjArray& search_conditions);

    /// parse detector parameter's values and write them to the database:
    /// txtFile - path to the text file with parameter values separated by spaces or tabs, e.g.: "value1 value2..."
    ///           if line is started with '#' symbol - it will be skipped (as comments)
    /// detector_name - detector corresponding to the parameter values
    /// parameter name - parameter filled by values
    /// start_period - start period of validity range for the parameter values
    /// start_run - start run of validity range for the parameter values
    /// end_period - end period of validity range for the parameter values
    /// end_run - end run of validity range for the parameter values
    /// function returns row count added to the database
    static int ParseTxt(TString text_file, TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run, bool isSerialChannel = false);

	ClassDef(UniDbDetectorParameter,1);
};

/*// for template
//template<class T>
//static void template_example(T a)
//{
//    std::cout<<a<<std::endl;
//    return;
//}

#ifdef __CINT__
  #pragma link C++ function UniDbDetectorParameter::CreateDetectorParameter(TString, TString, int, int, bool);
#else
  template UniDbDetectorParameter* UniDbDetectorParameter::CreateDetectorParameter(TString, TString, int, int, bool);
#endif*/

#endif
