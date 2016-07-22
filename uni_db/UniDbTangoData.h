/****************************************************************************
    Класс для доступа к информации детекторов в БД эксперимента Tango
    добавлено чтение информации по положению ZDC из CSV файлов
****************************************************************************/

#ifndef UNIDBTANGODATA_H
#define UNIDBTANGODATA_H 1

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

struct Tango_Double_Parameter
{
    //время
    TDatime parameter_time;
    //длина аттрибута (количество каналов)
    int parameter_length;
    //значения всех каналов одного аттрибута в один момент времени
    double* parameter_value;
};

struct Tango_Double_Data
{
    int dataCount;
    Tango_Double_Parameter* dataArray;

    Tango_Double_Data(int data_count)
    {
        dataCount = data_count;
        dataArray = new Tango_Double_Parameter[data_count];
    }
    ~Tango_Double_Data()
    {
        if (dataArray)
        {
            for (int i = 0; i < dataCount; i++)
                delete [] dataArray[i].parameter_value;

            delete [] dataArray;
        }
    }
};

class UniDbTangoData
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

    // Функция GetParameter получает доступ к базе Tango, совершает выбор, сделанный на основе заданных параметров.
    // Учитывая полученную информацию из базовой таблицы, извлекает название таблицы, в которой хранятся необходимые данные по параметру,
    // затем обращается к найденной таблице с данными и делает выборку по заданному временному периоду, результат заносит в структуру Tango_Double_Data.
    // Данная функция возвращает указатель на структуру с данными. Необходимые параметры для данной функции:
    //	detector_name - название детектора (например "zdc" или "gem")
    //	parameter_name - это название физического параметра из БД Tango (например, "uset" для ZDC или "u" для GEM)
    //	date_start - время, с которого начать считывать параметр (например, "2015-03-13 23:00:00")
    //	date_end - время окончания считавания параметра (например, "2015-03-13 24:00:00")
    Tango_Double_Data* GetTangoParameter(char* detector_name, char* parameter_name, char* date_start, char* date_end);

    // Функции PrintTangoData[Console,Surface,MultiGraph] выполняет вывод данных из заполненной ранее структуры
    // в консоль, графически как поверхность или графически в виде набора графиков,
    // на которых показано, как изменяются данные, например, напряжение на башнях ZDC в течении заданного периода.
    void PrintTangoDataConsole(Tango_Double_Data* tango_data);
    void PrintTangoDataSurface(Tango_Double_Data* tango_data);
    void PrintTangoDataMulti3D(Tango_Double_Data* tango_data);

 ClassDef(UniDbTangoData,1)
};

#endif

