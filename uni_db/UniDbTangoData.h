/****************************************************************************
           Class can be used to get hardware detector data
             from Tango (Slow Control System) database
****************************************************************************/

#ifndef UNIDBTANGODATA_H
#define UNIDBTANGODATA_H 1

#include "db_structures.h"

#include "TObject.h"
#include "TDatime.h"
#include "TObjArray.h"
#include "TString.h"

#include <vector>
#include <string>
using namespace std;

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

class UniDbTangoData
{
 public:
    // empty constructor
    UniDbTangoData(){}
    // empty destructor
    ~UniDbTangoData(){}

    // Функция SplitString разбивает строку на отдельные элементы по указанному разделителю.
    //	TString str - строка, которую будем редактировать.
    //	TString delim - разделитель.
    //	vector<TString> v - массив, в который заносятся отдельные элементы строки
    void SplitString(TString str, TString delim, vector<TString> &v);

    // Функция StringToTime конвертирует строковое значение вида "DD.MM.YYYY HH:MM:SS" в time_t
    int StringToTime(TString str_time);

    // Функция StringToTime конвертирует строковое значение вида "DD.MM.YYYY HH:MM:SS" в TDatime
    TDatime StringToDatime(TString str_time);

    // Функция GetCSVData считывает данные CSV файла, заносит их в структуру и возвращает указатель на нее
    // примечание: в таблицах CSV строки со временем сохранены в виде "DD.MM.YYYY HH:MM:SS", а в таблице запусков "Run1 summary table" - "YYYY-MM-DD HH:MM:SS".
    //	string filename - путь к файлу (например, Signals31.csv (run502.data 32944 строк))
    vector<CSVElement>* GetCSVData(string filename);

    // Функция PrintZDCXY выводит значения координат и время для массива данных
    //	CSVData *zdcXY - указатель на структуру.
    //  isGraphicPresentation: false - вывод в консоль,
    //      true - графическое представление (по оси Х откладываются отрезки времени, по оси Y - соответствующие координаты х и y калориметра)
    //  isTimeCut - выборка по времени данных из переданного массива
    //	TDatime* start_time - начало выборки по времени (например "2015-03-12 22:22:27" (строка 6132))
    //	TDatime* end_time - конец выборки по времени (например "2015-03-12 22:28:36" (строка 6881))
    void PrintCSVData(vector<CSVElement>* zdcXY, bool isGraphicPresentation=false, bool isTimeCut=false, TDatime* start_time=NULL, TDatime* end_time=NULL);

    // Функция GetTangoParameter получает доступ к базе Tango, совершает выбор, сделанный на основе заданных параметров.
    // Учитывая полученную информацию из базовой таблицы, извлекает название таблицы, в которой хранятся необходимые данные по параметру,
    // затем обращается к найденной таблице с данными и делает выборку по заданному временному периоду.
    // Возвращает массив TobjArray с объектами TangoTimeParameter (т.е. условно TObjArray<TangoTimeParameter*>), или NULL в случае ошибки.
    // Необходимые параметры для данной функции:
    //	detector_name - название детектора (например "zdc" или "gem")
    //	parameter_name - это название физического параметра из БД Tango (например, "uset" для ZDC или "u" для GEM)
    //	date_start - время, с которого начать считывать параметр (например, "2015-03-13 23:00:00")
    //	date_end - время окончания считавания параметра (например, "2015-03-13 24:00:00")
    TObjArray* GetTangoParameter(char* detector_name, char* parameter_name, char* date_start, char* date_end);

    // Функция SearchTangoParameter получает доступ к базе Tango, совершает выбор, сделанный на основе заданных параметров и дополнительных критериев.
    // Учитывая полученную информацию из базовой таблицы, извлекает название таблицы, в которой хранятся необходимые данные по параметру,
    // затем обращается к найденной таблице с данными и делает выборку по заданному временному периоду,
    // Возвращает массив TObjArray с объектми TObjArray, содержащими TangoTimeInterval (условно TObjArray<TObjArray<TangoTimeInterval*>>), или NULL в случае ошибки.
    // Необходимые параметры для данной функции:
    //	detector_name - название детектора (например "zdc" или "gem")
    //	parameter_name - это название физического параметра из БД Tango (например, "uset" для ZDC или "u" для GEM)
    //	date_start - время, с которого начать считывать параметр (например, "2015-03-13 23:00:00")
    //	date_end - время окончания считавания параметра (например, "2015-03-13 24:00:00")
    //  condition - условие выборки периодов времени
    //  value - значения для условия выборки, с которым происходит сравнение
    //  mapChannel - массив целочисленных значений для изменения порядка TObjArray-ев в результирующем массиве, если, например, каналы идут в другой последоватлеьности
    TObjArray* SearchTangoIntervals(char* detector_name, char* parameter_name, char* date_start, char* date_end,  enumConditions condition = conditionEqual, bool value = true, int* mapChannel = NULL);

    // Функции PrintTangoData[Console,Surface,MultiGraph] выполняет вывод данных из заполненной ранее структуры
    // в консоль, графически как поверхность или графически в виде набора графиков,
    // на которых показано, как изменяются данные, например, напряжение на башнях ZDC в течении заданного периода.
    void PrintTangoDataConsole(TObjArray* tango_data);
    void PrintTangoDataSurface(TObjArray* tango_data);
    void PrintTangoDataMulti3D(TObjArray* tango_data);
    void PrintTangoIntervalConsole(TObjArray* tango_data, TString channel_name = "Channel");

    // return average value for Tango data array (result vector with size greater than 1 is used in case of many channels)
    vector<double> GetAverageTangoData(TObjArray* tango_data);

 ClassDef(UniDbTangoData,1)
};

#endif

