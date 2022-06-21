/****************************************************************************
           This interface can be used to get hardware detector data
             from Tango (Slow Control System) database
****************************************************************************/

#ifndef TANGODATA_H
#define TANGODATA_H 1

#include "TDatime.h"
#include "TObjArray.h"
#include "TString.h"

#include <vector>
#include <string>
using namespace std;

enum enumConditions {conditionLess, conditionLessOrEqual, conditionEqual, conditionNotEqual, conditionGreater,
                     conditionGreaterOrEqual, conditionLike, conditionNull, conditionNotNull};

struct CSVElement
{
    TString varName;	// variable name (e.g. ZDC_X or ZDC_Y)
    TDatime runTime;	// corresponding time
    int varValue;		// value of the parameter
};

enum Tango_Parameter_Type {Tango_Bool = 1, Tango_Double = 2, Tango_Bool_Array = 11, Tango_Double_Array = 12};

class TangoTimeParameter : public TObject
{
  public:
    // parameter time
    TDatime parameter_time;
    // parameter type
    Tango_Parameter_Type parameter_type;
    // parameter values of one of the corresponding parameter_type
    vector<bool> bool_parameter_value;
    vector<double> double_parameter_value;

    /** Default constructor **/
    TangoTimeParameter();
    /** Initialization constructor **/
    TangoTimeParameter(TDatime par_time, Tango_Parameter_Type par_type);

    /** Destructor **/
    virtual ~TangoTimeParameter();

  ClassDef(TangoTimeParameter,1);
};

class TangoTimeInterval : public TObject
{
  public:
    TDatime start_time;
    TDatime end_time;

    /** Default constructor **/
    TangoTimeInterval();

    /** Initialization constructor **/
    TangoTimeInterval(TDatime start_time_interval, TDatime end_time_interval);

    /** Destructor **/
    virtual ~TangoTimeInterval();

  ClassDef(TangoTimeInterval,1);
};

// class used to get hardware detector data from Tango database and to display results in console or graphs
class TangoData
{
 public:
    // empty constructor
    TangoData();
    // empty destructor
    ~TangoData();

    // Function GetTangoParameter gets hardware data from the Tango database (MySQL connection defined in 'db_settings.h' file).
    // Parameters:
    //	detector_name - name of the detector (e.g. "zdc" or "gem")
    //	parameter_name - name of physical parameter stored in Tango (e.g. "uset" for ZDC or "u" for GEM)
    //	date_start - time from which to start reading the parameter, format: "YYYY-MM-DD HH:MM:SS" (e.g. "2015-03-13 23:00:00")
    //	date_end - end time of parameter reading, the same format (e.g. "2015-03-13 24:00:00")
    // Returns TObjArray with TangoTimeParameter objects (i.e. conditionally TObjArray<TangoTimeParameter*>), or nullptr in case errors.
    TObjArray* GetTangoParameter(const char* detector_name, const char* parameter_name, const char* date_start, const char* date_end);

    // Function SearchTangoIntervals gets time intervals for defined condition on parameter, from the Tango database (MySQL connection defined in 'db_settings.h' file).
    // NOTE: now it works only if channel count is constant during given time period
    // Parameters:
    //  detector_name - name of the detector (e.g. "zdc" or "gem")
    //  parameter_name - name of physical parameter stored in Tango (e.g. "uset" for ZDC or "u" for GEM)
    //  date_start - time from which to start searching for time intervals satisfied the condition, format: "YYYY-MM-DD HH:MM:SS" (e.g. "2015-03-13 23:00:00")
    //	date_end - end time of searching time intervals, the same format (e.g. "2015-03-13 24:00:00")
    //  condition - condition of time interval sampling, default: conditionEqual (the possible list in 'uni_db/uni_db_structures.h')
    //  value - boolean value for the condition with which the comparison is performed, default: true
    //  mapChannel - vector of integer values (map) to change the order of result TObjArray-s in the common result array, if, for example, channels go in a different sequence; nullptr - if not used
    // Returns common TObjArray with TObjArray objects containing TangoTimeInterval (i.e. conditionally TObjArray<TObjArray<TangoTimeInterval*>>),
    // if no intervals found - returns the common TObjArray with zero TObjArray elements; in case of errors - returns nullptr
    TObjArray* SearchTangoIntervals(const char* detector_name, const char* parameter_name, const char* date_start, const char* date_end,
                                    enumConditions condition = conditionEqual, bool value = true, vector<int>* mapChannel = nullptr);

    // Function PrintTangoDataConsole displays hardware data obtained from Tango, e.g. ZDC voltage in time interval, in console
    // Parameter: tango_data - TObjArray with TangoTimeParameter objects obtained from 'GetTangoParameter' function
    void PrintTangoDataConsole(TObjArray* tango_data);
    // Function PrintTangoDataSurface displays hardware vector data obtained from Tango, e.g. ZDC voltage in time interval, graphically as 2D Surface Graph
    // Parameters:
    //  tango_data - TObjArray with TangoTimeParameter objects obtained from 'GetTangoParameter' function
    //  y_axis - label of Y axis
    void PrintTangoDataSurface(TObjArray* tango_data, const char* y_axis = "parameter");
    // Function PrintTangoDataMulti3D displays hardware vector data obtained from Tango, e.g. ZDC voltage in time interval, graphically as a set of Line Graphs
    // Parameter: tango_data - TObjArray with TangoTimeParameter objects obtained from 'GetTangoParameter' function
    void PrintTangoDataMultiGraph(TObjArray* tango_data, const char* y_axis = "parameter", bool is3D = false);

    // Function PrintTangoIntervalConsole displays in console time intervals obtained from Tango for defined condition
    // Parameters:
    //  tango_intervals - TObjArray with TObjArray objects containing TangoTimeInterval objects obtained from 'SearchTangoIntervals' function
    //  channel_name - name of the dimension to display on the screen, default: Channel
    //  isShowOnlyExists - if true, print only intervals which satisfy the condition (skip empty intervals)
    void PrintTangoIntervalConsole(TObjArray* tango_intervals, TString channel_name = "Channel", bool isShowOnlyExists = false);

    // return average value for Tango data array (result vector with size greater than 1 is used in case of many channels)
    vector<double> GetAverageTangoData(TObjArray* tango_data);

 ClassDef(TangoData,1)
};

#endif
