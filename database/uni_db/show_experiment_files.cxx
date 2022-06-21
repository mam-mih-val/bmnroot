//============================================================================
// Name        : show_experiment_files.cpp
// Author      : Konstantin Gertsenberger
// Copyright   : JINR
// Description : Get list of experimental files for given runs
//============================================================================

// own headers
#include "uni_db_settings.h"

// ROOT includes
#include "TString.h"
#include "TSQLServer.h"
#include "TSQLResult.h"
#include "TSQLRow.h"

// C++ includes
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>

using namespace std;

int main(int argc, char** argv)
{
    // help information
    if (argc > 1){
        string first_par = argv[1];
        if ((first_par == "/?") || (first_par == "--help") || (first_par == "-h")){
            cout<<"Get list of experimental raw files satisfying the given parameters."<<endl<<"Parameters separated by comma:"<<endl
               <<"period - period number (range is supported by '-' symbol)"<<endl<<"run - run number (range is supported by '-' symbol)"<<endl
               <<"beam - first particle in collision (first beam)"<<endl<<"target - second particle in collision (second beam) OR target"<<endl
               <<"energy - collision energy (range is supported by '-' symbol)"<<endl<<"events - event count (range is supported by '-' symbol)"<<endl
               <<"time - experimental file (or files in case of the time interval) including event at this time (range is supported by '-' symbol), datetime format - yyyy-mm-dd 24hh:mm:ss"<<endl
               <<"field - field current (range is supported by '-' symbol)"<<endl<<"size - file size (range is supported by '-' symbol)"<<endl
               <<"path - part of the file path"<<endl<<endl<<"Examples:"<<endl<<"show_experiment_files period=5,energy=3-,beam=d,target=C"<<endl
               <<"show_experiment_files period=4-5,field=-800"<<endl;

            return 0;
        }
    }

    // parse parameter for DB searching
    bool isPeriod = false, isMinPeriod = false, isMaxPeriod = false, isRun = false, isMinRun = false, isMaxRun = false, isBeam = false, isTarget = false,
         isEnergy = false, isMinEnergy = false, isMaxEnergy = false, isEvents = false, isMinEvents = false, isMaxEvents = false, isTime = false, isMinTime = false, isMaxTime = false,
         isField = false, isMinField = false, isMaxField = false, isSize = false, isMinSize = false, isMaxSize = false, isPath = false;
    string strBeam, strTarget, strTime, strMaxTime, strPath;
    int iPeriod, iMaxPeriod, iRun, iMaxRun, iEvents, iMaxEvents, iField, iMaxField;
    double fEnergy, fMaxEnergy, fSize, fMaxSize;
    for (int i = 1; i < argc; i++){
        //TString strParameter(argv[i]);
        //TObjArray* arrSplit = strParameter.Tokenize(",");

        string input = argv[i];
        istringstream ss(input);
        string token;

        // parse tokens by comma separated
        while (getline(ss, token, ','))
        {
            // to lowercase
            transform(token.begin(), token.end(), token.begin(), ::tolower);

            // period number parsing
            if ((token.length() > 7) && (token.substr(0,7) == "period="))
            {
                token = token.substr(7);

                size_t indDash = token.find_first_of('-');
                if (indDash != string::npos)
                {
                    stringstream stream;
                    stream << token.substr(0, indDash);
                    int iVal;
                    if (stream >> iVal)
                    {
                        isPeriod = true;
                        isMinPeriod = true;
                        iPeriod = iVal;
                    }
                    if (token.length() > indDash)
                    {
                        stringstream stream2;
                        stream2 << token.substr(indDash+1);
                        if (stream2 >> iVal)
                        {
                            isPeriod = true;
                            isMaxPeriod = true;
                            iMaxPeriod = iVal;
                        }
                    }
                }//if (indDash > -1)
                // if exact period number
                else
                {
                    stringstream stream;
                    stream << token;
                    int iVal;
                    if (stream >> iVal)
                    {
                        isPeriod = true;
                        iPeriod = iVal;
                    }
                }//else
            }//if ((token.length() > 7) && (token.substr(0,7) == "period="))
            else
            {
                // run number parsing
                if ((token.length() > 4) && (token.substr(0,4) == "run="))
                {
                    token = token.substr(4);

                    size_t indDash = token.find_first_of('-');
                    if (indDash != string::npos)
                    {
                        stringstream stream;
                        stream << token.substr(0, indDash);
                        int iVal;
                        if (stream >> iVal)
                        {
                            isRun = true;
                            isMinRun = true;
                            iRun = iVal;
                        }
                        if (token.length() > indDash)
                        {
                            stringstream stream2;
                            stream2 << token.substr(indDash+1);
                            if (stream2 >> iVal)
                            {
                                isRun = true;
                                isMaxRun = true;
                                iMaxRun = iVal;
                            }
                        }
                    }//if (indDash > -1)
                    // if exact run number
                    else
                    {
                        stringstream stream;
                        stream << token;
                        int iVal;
                        if (stream >> iVal)
                        {
                            isRun = true;
                            iRun = iVal;
                        }
                    }//else
                }//if ((token.length() > 4) && (token.substr(0,4) == "run="))
                else
                {
                    // beam particle's name in collision parsing
                    if ((token.length() > 5) && (token.substr(0,5) == "beam="))
                    {
                        isBeam = true;
                        strBeam = token.substr(5);
                    }
                    else
                    {
                        // target particle's name in collision parsing
                        if ((token.length() > 7) && (token.substr(0,7) == "target="))
                        {
                            isTarget = true;
                            strTarget = token.substr(7);
                        }
                        else
                        {
                            // energy parsing
                            if ((token.length() > 7) && (token.substr(0,7) == "energy="))
                            {
                                token = token.substr(7);

                                size_t indDash = token.find_first_of('-');
                                if (indDash != string::npos)
                                {
                                    stringstream stream;
                                    stream << token.substr(0, indDash);
                                    double dVal;
                                    if (stream >> dVal)
                                    {
                                        isEnergy = true;
                                        isMinEnergy = true;
                                        fEnergy = dVal;
                                    }
                                    if (token.length() > indDash)
                                    {
                                        stringstream stream2;
                                        stream2 << token.substr(indDash+1);
                                        if (stream2 >> dVal)
                                        {
                                            isEnergy = true;
                                            isMaxEnergy = true;
                                            fMaxEnergy = dVal;
                                        }
                                    }
                                }//if (indDash > -1)
                                // if exact energy value
                                else
                                {
                                    stringstream stream;
                                    stream << token;
                                    double dVal;
                                    if (stream >> dVal)
                                    {
                                        isEnergy = true;
                                        fEnergy = dVal;
                                    }
                                }//else
                            }//if ((token.length() > 7) && (token.substr(0,7) == "energy="))
                            else
                            {
                                // event count parsing
                                if ((token.length() > 6) && (token.substr(0,6) == "events="))
                                {
                                    token = token.substr(6);

                                    size_t indDash = token.find_first_of('-');
                                    if (indDash != string::npos)
                                    {
                                        stringstream stream;
                                        stream << token.substr(0, indDash);
                                        int iVal;
                                        if (stream >> iVal)
                                        {
                                            isEvents = true;
                                            isMinEvents = true;
                                            iEvents = iVal;
                                        }
                                        if (token.length() > indDash)
                                        {
                                            stringstream stream2;
                                            stream2 << token.substr(indDash+1);
                                            if (stream2 >> iVal)
                                            {
                                                isEvents = true;
                                                isMaxEvents = true;
                                                iMaxEvents = iVal;
                                            }
                                        }
                                    }//if (indDash > -1)
                                    // if exact event count
                                    else
                                    {
                                        stringstream stream;
                                        stream << token;
                                        int iVal;
                                        if (stream >> iVal)
                                        {
                                            isEvents = true;
                                            iEvents = iVal;
                                        }
                                    }//else
                                }//if ((token.length() > 6) && (token.substr(0,6) == "events="))
                                else
                                {
                                    // time parsing
                                    if ((token.length() > 5) && (token.substr(0,5) == "time="))
                                    {
                                        token = token.substr(5);

                                        size_t indDash = token.find_first_of('-');
                                        if (indDash != string::npos)
                                        {
                                            if (indDash > 0)
                                            {
                                                isTime = true;
                                                isMinTime = true;
                                                strTime = token.substr(0, indDash);
                                            }
                                            if (token.length() > indDash)
                                            {
                                                isTime = true;
                                                isMaxTime = true;
                                                strMaxTime = token.substr(indDash+1);
                                            }
                                        }//if (indDash > -1)
                                        // if exact time
                                        else
                                        {
                                            isTime = true;
                                            strTime = token;
                                        }//else
                                    }//if ((token.length() > 5) && (token.substr(0,5) == "time="))
                                    else
                                    {
                                        // field current parsing
                                        if ((token.length() > 6) && (token.substr(0,6) == "field="))
                                        {
                                            token = token.substr(6);

                                            size_t indDash = token.find_first_of('-');
                                            if (indDash != string::npos)
                                            {
                                                stringstream stream;
                                                stream << token.substr(0, indDash);
                                                int iVal;
                                                if (stream >> iVal)
                                                {
                                                    isField = true;
                                                    isMinField = true;
                                                    iField = iVal;
                                                }
                                                if (token.length() > indDash)
                                                {
                                                    stringstream stream2;
                                                    stream2 << token.substr(indDash+1);
                                                    if (stream2 >> iVal)
                                                    {
                                                        isField = true;
                                                        isMaxField = true;
                                                        iMaxField = iVal;
                                                    }
                                                }
                                            }//if (indDash > -1)
                                            // if exact field current
                                            else
                                            {
                                                stringstream stream;
                                                stream << token;
                                                int iVal;
                                                if (stream >> iVal)
                                                {
                                                    isField = true;
                                                    iField = iVal;
                                                }
                                            }//else
                                        }//if ((token.length() > 6) && (token.substr(0,6) == "field="))
                                        else
                                        {
                                            // file size parsing
                                            if ((token.length() > 5) && (token.substr(0,5) == "size="))
                                            {
                                                token = token.substr(5);

                                                size_t indDash = token.find_first_of('-');
                                                if (indDash != string::npos)
                                                {
                                                    stringstream stream;
                                                    stream << token.substr(0, indDash);
                                                    double dVal;
                                                    if (stream >> dVal)
                                                    {
                                                        isSize = true;
                                                        isMinSize = true;
                                                        fSize = dVal;
                                                    }
                                                    if (token.length() > indDash)
                                                    {
                                                        stringstream stream2;
                                                        stream2 << token.substr(indDash+1);
                                                        if (stream2 >> dVal)
                                                        {
                                                            isSize = true;
                                                            isMaxSize = true;
                                                            fMaxSize = dVal;
                                                        }
                                                    }
                                                }//if (indDash > -1)
                                                // if exact file size
                                                else
                                                {
                                                    stringstream stream;
                                                    stream << token;
                                                    double dVal;
                                                    if (stream >> dVal)
                                                    {
                                                        isSize = true;
                                                        fSize = dVal;
                                                    }
                                                }//else
                                            }//if ((token.length() > 5) && (token.substr(0,5) == "size="))
                                            else
                                            {
                                                // path parsing
                                                if ((token.length() > 5) && (token.substr(0,5) == "path="))
                                                {
                                                    isPath = true;
                                                    strPath = token.substr(5);
                                                }
                                            }//else SIZE
                                        }//else FIELD
                                    }//else TIME
                                }//else EVENTS
                            }//else ENERGY
                        }//else TARGER
                    }//else BEAM
                }//else RUN
            }//else PERIOD
        }//while(getline(ss, token, ','))
    }//for (int i = 1; i < argc; i++)

    // generate query
    TString strConnection = "pgsql://" + TString(UNI_DB_HOST) + "/" + (TString)UNI_DB_NAME;
    TSQLServer* pSQLServer = TSQLServer::Connect(strConnection, UNI_DB_USERNAME, UNI_DB_PASSWORD);
    if (pSQLServer == nullptr)
    {
        cout<<"Connection to the database was not established"<<endl;
        return -1;
    }

    TString sql = "select file_path "
                  "from run_";

    bool isWhere = false;
    // if period selection
    if (isPeriod == true)
    {
        if (isWhere)
            sql += " AND ";
        else
        {
            isWhere = true;
            sql += " "
                   "where ";
        }

        if (isMinPeriod)
        {
            sql += TString::Format("period_number >= %d", iPeriod);
            if (isMaxPeriod)
                sql += TString::Format(" AND period_number <= %d", iMaxPeriod);
        }
        else
        {
            if (isMaxPeriod)
                sql += TString::Format("period_number <= %d", iMaxPeriod);
            else
                sql += TString::Format("period_number = %d", iPeriod);
        }
    }
    // if run selection
    if (isRun == true)
    {
        if (isWhere)
            sql += " AND ";
        else
        {
            isWhere = true;
            sql += " "
                   "where ";
        }

        if (isMinRun)
        {
            sql += TString::Format("run_number >= %d", iRun);
            if (isMaxRun)
                sql += TString::Format(" AND run_number <= %d", iMaxRun);
        }
        else
        {
            if (isMaxRun)
                sql += TString::Format("run_number <= %d", iMaxRun);
            else
                sql += TString::Format("run_number = %d", iRun);
        }
    }
    // if beam particle was selected
    if (isBeam == true)
    {
        if (isWhere)
            sql += TString::Format(" AND lower(beam_particle) = '%s'", strBeam.data());
        else
        {
            isWhere = true;
            sql += TString::Format(" "
                                   "where lower(beam_particle) = '%s'", strBeam.data());
        }
    }
    // if target particle was selected
    if (isTarget == true)
    {
        if (isWhere)
            sql += TString::Format(" AND lower(target_particle) = '%s'", strTarget.data());
        else
        {
            isWhere = true;
            sql += TString::Format(" "
                                   "where lower(target_particle) = '%s'", strTarget.data());
        }
    }
    // if energy selection
    if (isEnergy == true)
    {
        if (isWhere)
            sql += " AND ";
        else
        {
            isWhere = true;
            sql += " "
                   "where ";
        }

        if (isMinEnergy)
        {
            sql += TString::Format("energy >= %f", fEnergy);
            if (isMaxEnergy)
                sql += TString::Format(" AND energy <= %f", fMaxEnergy);
        }
        else
        {
            if (isMaxEnergy)
                sql += TString::Format("energy <= %f", fMaxEnergy);
            else
                sql += TString::Format("energy = %f", fEnergy);
        }
    }
    // if event count selection
    if (isEvents == true)
    {
        if (isWhere)
            sql += " AND ";
        else
        {
            isWhere = true;
            sql += " "
                   "where ";
        }

        if (isMinEvents)
        {
            sql += TString::Format("event_count >= %d", iEvents);
            if (isMaxEvents)
                sql += TString::Format(" AND event_count <= %d", iMaxEvents);
        }
        else
        {
            if (isMaxEvents)
                sql += TString::Format("event_count <= %d", iMaxEvents);
            else
                sql += TString::Format("event_count = %d", iEvents);
        }
    }
    // if time selection
    if (isTime == true)
    {
        if (isWhere)
            sql += " AND ";
        else
        {
            isWhere = true;
            sql += " "
                   "where ";
        }

        // check correct format
        struct tm tm;
        if (!strptime(strMaxTime.c_str(), "%Y-%m-%d %H:%M:%S", &tm))
        {
            isMaxTime = false;
            cout<<"ERROR: "<<strMaxTime<<" is not correct datetime. DateTime format should be yyyy-mm-dd 24hh:mm:ss."<<endl;
        }
        if (!strptime(strTime.c_str(), "%Y-%m-%d %H:%M:%S", &tm))
        {
            isMinTime = false;
            cout<<"ERROR: "<<strTime<<" is not correct datetime. DateTime format should be yyyy-mm-dd 24hh:mm:ss."<<endl;
            if (!isMaxTime) isTime = false;
        }

        if (isMinTime)
        {
            sql += TString::Format("end_datetime >= '%s'", strTime.c_str());
            if (isMaxTime)
                sql += TString::Format(" AND start_datetime <= '%s'", strMaxTime.c_str());
        }
        else
        {
            if (isMaxTime)
                sql += TString::Format("start_datetime <= '%s'", strMaxTime.c_str());
            else
            {
                if (isTime)
                    sql += TString::Format("start_datetime <= '%s' AND end_datetime >= '%s'", strTime.c_str(), strTime.c_str());
            }
        }
    }
    // if field current selection
    if (isField == true)
    {
        if (isWhere)
            sql += " AND ";
        else
        {
            isWhere = true;
            sql += " "
                   "where ";
        }

        if (isMinField)
        {
            sql += TString::Format("field_voltage >= %d", iField);
            if (isMaxField)
                sql += TString::Format(" AND field_voltage <= %d", iMaxField);
        }
        else
        {
            if (isMaxField)
                sql += TString::Format("field_voltage <= %d", iMaxField);
            else
                sql += TString::Format("field_voltage = %d", iField);
        }
    }
    // if file size selection
    if (isSize == true)
    {
        if (isWhere)
            sql += " AND ";
        else
        {
            isWhere = true;
            sql += " "
                   "where ";
        }

        if (isMinSize)
        {
            sql += TString::Format("file_size >= %f", fSize);
            if (isMaxSize)
                sql += TString::Format(" AND file_size <= %f", fMaxSize);
        }
        else
        {
            if (isMaxSize)
                sql += TString::Format("file_size <= %f", fMaxSize);
            else
                sql += TString::Format("file_size = %f", fSize);
        }
    }
    if (isPath == true)
    {
        if (isWhere)
            sql += TString::Format(" AND lower(file_path) like '%%%s%%'", strPath.data());
        else
        {
            isWhere = true;
            sql += TString::Format(" "
                                   "where lower(file_path) like '%%%s%%'", strPath.data());
        }
    }

    sql += " order by period_number,run_number";
    //cout<<sql<<endl;

    TSQLResult* res = pSQLServer->Query(sql);

    int nrows = res->GetRowCount();
    if (nrows == 0)
        cout<<"There are no experimental files for these parameters"<<endl;
    else
    {
        TSQLRow* row;
        while (row = res->Next())
        {
            TString path = row->GetField(0);
            cout<<path<<endl;

            delete row;
        }

        cout<<endl<<"Total count: "<<nrows<<endl;
    }

    delete res;

    if (pSQLServer)
        delete pSQLServer;

    return 0;
}
