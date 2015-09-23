/****************************************************************************
    Класс для доступа к информации детекторов в БД эксперимента Tango
    добавлено чтение информации по положению ZDC из CSV файлов
****************************************************************************/

#ifndef MPDDBTANGODATA_H
#define MPDDBTANGODATA_H 1

#include "TDatime.h"
#include "TString.h"

#include <vector>
#include <string>

struct CSVElement
{
    TString varName;	// имя переменной (ZDC_X или ZDC_Y)
    TDatime runTime;	// время
    int varValue;		// значение
};

struct CSVData
{
    int dataCount;
    CSVElement* dataArray;

    CSVData(int size)
    {
        dataCount = size;
        dataArray = new CSVElement[size];
    }
    ~CSVData()
    {
        if (dataArray)
            delete [] dataArray;
    }
};

struct HV_Parameter
{
    //время запуска
    TDatime time_run;
    //количество башен
    int tower_count;
    //напряжение на башнях
    double* tower_hv;
};

struct HV_Data
{
    int rowCount;
    HV_Parameter* hvArray;

    HV_Data(int count)
    {
        rowCount = count;
        hvArray = new HV_Parameter[count];
    }
    ~HV_Data()
    {
        if (hvArray)
        {
            for (int i = 0; i < rowCount; i++)
                delete [] hvArray[i].tower_hv;

            delete [] hvArray;
        }
    }
};

class MpdDbTangoData
{
 public:
    // Функция SplitString разбивает строку на отдельные элементы по указанному разделителю.
    //	TString str - строка, которую будем редактировать.
    //	TString delim - разделитель.
    //	vector<TString> v - массив, в который заносятся отдельные элементы строки
    void SplitString(TString str, TString delim, std::vector<TString> &v);

    // Функция StringToTime конвертирует строковое значение вида "DD.MM.YYYY HH:MM:SS" в time_t
    int StringToTime(TString str_time);

    // Функция StringToTime конвертирует строковое значение вида "DD.MM.YYYY HH:MM:SS" в TDatime
    TDatime StringToDatime(TString str_time);

    // Функция GetCSVData считывает данные CSV файла, заносит их в структуру и возвращает указатель на нее
    // примечание: в таблицах CSV строки со временем сохранены в виде "DD.MM.YYYY HH:MM:SS", а в таблице запусков "Run1 summary table" - "YYYY-MM-DD HH:MM:SS".
    //	string filename - путь к файлу (например, Signals31.csv (run502.data 32944 строк))
    CSVData* GetCSVData(std::string filename);

    // Функция PrintZDCXY выводит значения координат и время для массива данных
    //	CSVData *zdcXY - указатель на структуру.
    //  isGraphicPresentation: false - вывод в консоль,
    //      true - графическое представление (по оси Х откладываются отрезки времени, по оси Y - соответствующие координаты х и y калориметра)
    //  isTimeCut - выборка по времени данных из переданного массива
    //	TDatime* start_time - начало выборки по времени (например "2015-03-12 22:22:27" (строка 6132))
    //	TDatime* end_time - конец выборки по времени (например "2015-03-12 22:28:36" (строка 6881))
    void PrintCSVData(CSVData* zdcXY, bool isGraphicPresentation=false, bool isTimeCut=false, TDatime* start_time=NULL, TDatime* end_time=NULL);

    // функция GetParameter получает доступ к базе Tango, совершает выбор, сделанный на основе заданных параметров, необходимой строки из таблицы hdb,
    // учитывая полученную строку, извлекает название таблицы, в которой хранятся данные по напряжению питания калориметра,
    // затем обращается к данной таблице и делает выборку по заданному временному периоду, результат заносит в структуру Voltage_Calorimeter.
    // Данный класс расчитан на построение графиков со 128 башнями.
    // Данная функция возвращает указатель на структуру. Необходимые параметры для данной функции:
    //	detector_name - название детектора (например "zdc")
    //	parameter_name - это название физического параметра из БД Tango (например, "Vset")
    //	date_start- время, с которого начать считывать параметр (например, "2015-03-13 23:00:00")
    //	date_end - время окончания считавания параметра (например, "2015-03-13 24:00:00")
    HV_Data* GetTangoParameter(char* detector_name, char* parameter_name, char* date_start, char* date_end);

    // функция PrintZDCHV выполняет вывод данных из заполненной ранее структуры в консоль, а также вывод графиков, посредством класса TCanvas,
    // на которых показано, как изменяется напряжение на башнях в течении заданного периода.
    void PrintHV(HV_Data* hv_parameter, bool isGraphicPresentation=false);

 ClassDef(MpdDbTangoData,1) //MpdDbTangoData
};

#endif

