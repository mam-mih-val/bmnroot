// ----------------------------------------------------------------------
//                    UniDbSessionDcParameter header file 
//                      Generated 22-10-2015 
// ----------------------------------------------------------------------

/** db_classes/UniDbSessionDcParameter.h 
 ** Class for the table: session_dc_parameter 
 **/ 

#ifndef UNIDBSESSIONDCPARAMETER_H 
#define UNIDBSESSIONDCPARAMETER_H 1 

#include "TString.h"
#include "TDatime.h"

#include "UniDbConnection.h"
#include "UniDbParameter.h"

class UniDbSessionDcParameter
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
	/// dc serial
	int i_dc_serial;
	/// channel
	int i_channel;
	/// parameter value
	unsigned char* blob_parameter_value;
	/// size of parameter value
	Long_t sz_parameter_value;

	//Constructor
	UniDbSessionDcParameter(UniDbConnection* connUniDb, int session_number, TString detector_name, int parameter_id, int dc_serial, int channel, unsigned char* parameter_value, Long_t size_parameter_value);
	/* END OF PRIVATE GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 public:
	/* GENERATED PUBLIC MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	virtual ~UniDbSessionDcParameter(); // Destructor

	// static class functions
	/// add new session dc parameter to the database
	static UniDbSessionDcParameter* CreateSessionDcParameter(int session_number, TString detector_name, int parameter_id, int dc_serial, int channel, unsigned char* parameter_value, Long_t size_parameter_value);
	/// get session dc parameter from the database
	static UniDbSessionDcParameter* GetSessionDcParameter(int session_number, TString detector_name, int parameter_id, int dc_serial, int channel);
	/// delete session dc parameter from the database
	static int DeleteSessionDcParameter(int session_number, TString detector_name, int parameter_id, int dc_serial, int channel);
	/// print all session dc parameters
	static int PrintAll();

	// Getters
	/// get session number of the current session dc parameter
	int GetSessionNumber() {return i_session_number;}
	/// get detector name of the current session dc parameter
	TString GetDetectorName() {return str_detector_name;}
	/// get parameter id of the current session dc parameter
	int GetParameterId() {return i_parameter_id;}
	/// get dc serial of the current session dc parameter
	int GetDcSerial() {return i_dc_serial;}
	/// get channel of the current session dc parameter
	int GetChannel() {return i_channel;}
	/// get parameter value of the current session dc parameter
	unsigned char* GetParameterValue() {unsigned char* tmp_parameter_value = new unsigned char[sz_parameter_value]; memcpy(tmp_parameter_value, blob_parameter_value, sz_parameter_value); return tmp_parameter_value;}
	/// get size of parameter value of the current session dc parameter
	Long_t GetParameterValueSize() {return sz_parameter_value;}

	// Setters
	/// set session number of the current session dc parameter
	int SetSessionNumber(int session_number);
	/// set detector name of the current session dc parameter
	int SetDetectorName(TString detector_name);
	/// set parameter id of the current session dc parameter
	int SetParameterId(int parameter_id);
	/// set dc serial of the current session dc parameter
	int SetDcSerial(int dc_serial);
	/// set channel of the current session dc parameter
	int SetChannel(int channel);
	/// set parameter value of the current session dc parameter
	int SetParameterValue(unsigned char* parameter_value, Long_t size_parameter_value);
	/// print information about current session dc parameter
	void Print();
	/* END OF PUBLIC GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

    /// get detector parameter by session number, detector name, parameter name, TDC/ADC serial and channel number
    static UniDbSessionDcParameter* GetSessionDcParameter(int session_number, TString detector_name, TString parameter_name, int dc_serial, int channel);

    // common function for creating parameter
    static UniDbSessionDcParameter* CreateSessionDcParameter(int session_number, TString detector_name, TString parameter_name, int dc_serial, int channel, unsigned char* p_parameter_value, Long_t size_parameter_value, enumParameterType enum_parameter_type);
    // common function for getting parameter
    unsigned char* GetUNC(enumParameterType enum_parameter_type);
    // common function for setting parameter
    int SetUNC(unsigned char* p_parameter_value, Long_t size_parameter_value);

    /// add new record - TDC/ADC parameter value for one channel as BOOL
    static UniDbSessionDcParameter* CreateSessionDcParameter(int session_number, TString detector_name, TString parameter_name, int dc_serial, int channel, bool parameter_value);
    /// get parameter value (BOOL) for one channel (for current run, detector, TDC/ADC and channel)
    bool GetBool();
    /// set parameter value (BOOL) for one channel TDC/ADC
    int SetBool(bool parameter_value);

    /// add new record - TDC/ADC parameter value for one channel as INTEGER
    static UniDbSessionDcParameter* CreateSessionDcParameter(int session_number, TString detector_name, TString parameter_name, int dc_serial, int channel, int parameter_value);
    /// get parameter value (INTEGER) for one channel (for current run, detector, TDC/ADC and channel)
    int GetInt();
    /// set parameter value (INTEGER) for one channel TDC/ADC
    int SetInt(int parameter_value);

    /// add new record - TDC/ADC parameter value for one channel as DOUBLE
    static UniDbSessionDcParameter* CreateSessionDcParameter(int session_number, TString detector_name, TString parameter_name, int dc_serial, int channel, double parameter_value);
    /// get parameter value (DOUBLE) for one channel (for current run, detector, TDC/ADC and channel)
    double GetDouble();
    /// set parameter value (DOUBLEy) for one channel TDC/ADC
    int SetDouble(double parameter_value);

    /// add new record - TDC/ADC parameter value for one channel as STRING
    static UniDbSessionDcParameter* CreateSessionDcParameter(int session_number, TString detector_name, TString parameter_name, int dc_serial, int channel, TString parameter_value);
    /// get parameter value (STRING) for one channel (for current run, detector, TDC/ADC and channel)
    TString GetString();
    /// set parameter value (STRING) for one channel TDC/ADC
    int SetString(TString parameter_value);

    /// add new record - TDC/ADC parameter value for one channel as Integer Array
    static UniDbSessionDcParameter* CreateSessionDcParameter(int session_number, TString detector_name, TString parameter_name, int dc_serial, int channel, int* parameter_value, int element_count);
    /// get parameter value (Integer Array) for one channel (for current run, detector, TDC/ADC and channel)
    int GetIntArray(int*& parameter_value, int& element_count);
    /// set parameter value (Integer Array) for one channel TDC/ADC
    int SetIntArray(int* parameter_value, int element_count);

    /// add new record - TDC/ADC parameter value for one channel as Double Array (e.g. INL)
    static UniDbSessionDcParameter* CreateSessionDcParameter(int session_number, TString detector_name, TString parameter_name, int dc_serial, int channel, double* parameter_value, int element_count);
    /// get parameter value (Double Array) for one channel (for current run, detector, TDC/ADC and channel)
    int GetDoubleArray(double*& parameter_value, int& element_count);
    /// set parameter value (Double Array) for one channel TDC/ADC
    int SetDoubleArray(double* parameter_value, int element_count);

    /// add new record - TDC/ADC parameter value for one channel as Int+Int Array
    static UniDbSessionDcParameter* CreateSessionDcParameter(int session_number, TString detector_name, TString parameter_name, int dc_serial, int channel, IIStructure* parameter_value, int element_count);
    /// get parameter value (Int+Int Array) for one channel (for current run, detector, TDC/ADC and channel)
    int GetIIArray(IIStructure*& parameter_value, int& element_count);
    /// set parameter value (Int+Int Array) for one channel TDC/ADC
    int SetIIArray(IIStructure* parameter_value, int element_count);

 ClassDef(UniDbSessionDcParameter,1);
};

#endif
