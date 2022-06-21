#include "TangoData.h"
#include "TangoConnection.h"

#include <TSQLServer.h>
#include <TSQLStatement.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TGraph2D.h>
#include <TMultiGraph.h>
#include <TAxis.h>
#include <TLegend.h>
#include <TStyle.h>
#include <TH2D.h>
#include "TRandom.h"
#include "TColor.h"
#include "TROOT.h"

#include <fstream>
#include <iostream>

TangoTimeParameter::TangoTimeParameter()
{
}

TangoTimeParameter::TangoTimeParameter(TDatime par_time, Tango_Parameter_Type par_type)
{
    parameter_time = par_time;
    parameter_type = par_type;
}

TangoTimeParameter::~TangoTimeParameter()
{
}
ClassImp(TangoTimeParameter);

TangoTimeInterval::TangoTimeInterval()
{
}

TangoTimeInterval::TangoTimeInterval(TDatime start_time_interval, TDatime end_time_interval)
{
    start_time = start_time_interval;
    end_time = end_time_interval;
}

TangoTimeInterval::~TangoTimeInterval()
{
}
ClassImp(TangoTimeInterval);

// CLASS TangoData
TangoData::TangoData() {}

TangoData::~TangoData() {}

// Function GetTangoParameter gets hardware data from the Tango database (MySQL connection defined in 'db_settings.h' file).
// First, name of the target table with required parameter is extracted from the Tango Base Table,
// then required data are selected from the target table corresponding the given time interval.
// Parameters:
//	detector_name - name of the detector (e.g. "zdc" or "gem")
//	parameter_name - name of physical parameter stored in Tango (e.g. "uset" for ZDC or "u" for GEM)
//	date_start - time from which to start reading the parameter, format: "YYYY-MM-DD HH:MM:SS" (e.g. "2015-03-13 23:00:00")
//	date_end - end time of parameter reading, the same format (e.g. "2015-03-13 24:00:00")
// Returns TobjArray with TangoTimeParameter objects (i.e. conditionally TObjArray<TangoTimeParameter*>), or nullptr in case errors.
TObjArray* TangoData::GetTangoParameter(const char* detector_name, const char* parameter_name, const char* date_start, const char* date_end)
{
    if ((date_start == nullptr) || (date_end == nullptr))
    {
        cout<<"ERROR: date start and date end should be not nullptr!"<<endl;
        return nullptr;
    }

    // TANGO database connection
    TangoConnection* connDb = TangoConnection::Open();
    if (connDb == nullptr) return nullptr;
    TSQLServer* db_server = connDb->GetSQLServer();
    if (db_server == nullptr) return nullptr;

    if (db_server->GetTables(db_server->GetDB()) == nullptr)
        return nullptr;

    // searching for parameters with given parameter_name for detector with detector_name
    TString strStatement = TString::Format(
                "SELECT att_conf_id,data_type FROM att_conf ac join att_conf_data_type acdt on ac.att_conf_data_type_id = acdt.att_conf_data_type_id WHERE family=\"%s\" and name=\"%s\" ",
                detector_name, parameter_name);
    TSQLStatement* stmt_select = db_server->Statement(strStatement);
    if (!stmt_select->Process())
    {
            cout<<"ERROR: getting info about parameter from Tango has been failed: detector_name = "<<detector_name<<", parameter_name = "<<parameter_name<<endl;
            delete stmt_select;
            return nullptr;
    }

    stmt_select->StoreResult();
    if (!stmt_select->NextResultRow())
    {
        cout<<"ERROR: There is no parameter '"<<parameter_name<<"' for "<<detector_name<<" detector"<<endl;
        delete stmt_select;
        return nullptr;
    }
    // Now Tango team duplicates parameters for every Nuclotron session
    if ((stmt_select->GetNumAffectedRows() > 1) && (strcmp(date_end, "2018-01-01") > 0))
    {
        if (!stmt_select->NextResultRow())
        {
            cout<<"ERROR: There is no second row for parameter '"<<parameter_name<<"' for "<<detector_name<<" detector"<<endl;
            delete stmt_select;
            return nullptr;
        }
    }

    int data_id = stmt_select->GetInt(0);
    TString data_type(stmt_select->GetString(1));
    TString table_name = TString::Format("att_%s", data_type.Data());
    //cout<<"Data table name - "<<table_name<<endl;
    delete stmt_select;

    // parse parameter type, support for array_devdouble, array_devboolean, scalar_devdouble
    Tango_Parameter_Type par_type;
    if ((data_type == "array_devdouble_rw") || (data_type == "array_devdouble_ro"))
        par_type = Tango_Double_Array;
    else
    {
        if ((data_type == "array_devboolean_rw") || (data_type == "array_devboolean_ro"))
            par_type = Tango_Bool_Array;
        else
        {
            if ((data_type == "scalar_devdouble_rw") || (data_type == "scalar_devdouble_ro"))
                par_type = Tango_Double;
            else
            {
                cout<<"ERROR: This Tango type is not supported: '"<<data_type<<"'"<<endl;
                return nullptr;
            }
        }
    }

    // getting data from the table found by attribute type
    TString query_data = "";
    if (par_type > 10)
        query_data = TString::Format("SELECT data_time, value_r, dim_x_r, dim_x_w, idx FROM %s WHERE att_conf_id=\"%d\" and data_time>=\"%s\" and data_time<=\"%s\" and value_r is not null ORDER BY data_time,idx",
                                     table_name.Data(), data_id, date_start, date_end);
    else
        query_data = TString::Format("SELECT data_time, value_r FROM %s WHERE att_conf_id=\"%d\" and data_time>=\"%s\" and data_time<=\"%s\" and value_r is not null ORDER BY data_time",
                                      table_name.Data(), data_id, date_start, date_end);
    //cout<<"Query data: "<<query_data<<endl;

    stmt_select = db_server->Statement(query_data);
    if (!stmt_select->Process())
    {
            cout<<"ERROR: getting info about parameter values from Tango has been failed"<<endl;
            delete stmt_select;
            return nullptr;
    }

    stmt_select->StoreResult();

    // write data to the result array
    TObjArray* tango_data = new TObjArray();
    tango_data->SetName(TString::Format("'%s' parameter for %s detector", parameter_name, detector_name));
    tango_data->SetOwner(kTRUE);
    while (stmt_select->NextResultRow())
    {
        TDatime datetime = stmt_select->GetTimestamp(0);

        int i_par_len = 1;
        if (par_type > 10) i_par_len = stmt_select->GetInt(2);
        if (i_par_len == 0)
        {
            cout<<"ERROR: Parameter length can't be equal 0"<<endl;
            delete tango_data;
            delete stmt_select;
            return nullptr;
        }

        int i_real_par_len = 1;
        if (par_type > 10) i_real_par_len = stmt_select->GetInt(3);
        if (i_real_par_len == 0)
        {
            cout<<"ERROR: Real parameter length can't be equal 0"<<endl;
            delete tango_data;
            delete stmt_select;
            return nullptr;
        }

        TangoTimeParameter* par = new TangoTimeParameter(datetime, par_type);
        //cout<<par.parameter_time.AsString()<<endl;
        for (int ind = 0; ind < i_par_len; ind++)
        {
            if (ind > 0)
                stmt_select->NextResultRow();

            int i_idx = 0;
            if (par_type > 10)
                 i_idx = stmt_select->GetInt(4);
            //cout<<"idx:ind - "<<idx<<":"<<ind<<endl;
            if (i_idx != ind)
            {
                cout<<"ERROR: idx should be equal index of the parameter array"<<endl;
                delete tango_data;
                delete stmt_select;
                return nullptr;
            }

            if (ind >= i_real_par_len)
                continue;

            if ((par_type == Tango_Bool_Array) || (par_type == Tango_Bool))
            {
                bool b_val = (bool) stmt_select->GetInt(1);
                par->bool_parameter_value.push_back(b_val);
                par->double_parameter_value.push_back(b_val);
                //cout<<par.bool_parameter_value[par.bool_parameter_value.size()-1]<<endl;
            }
            else
            {
                double d_val = stmt_select->GetDouble(1);
                par->double_parameter_value.push_back(d_val);
                //cout<<par.double_parameter_value[par.double_parameter_value.size()-1]<<endl;
            }

        }
        //cout<<par.bool_parameter_value.size().size()<<endl;
        //cout<<par.double_parameter_value.size().size()<<endl;

        tango_data->Add(par);
    }

    delete stmt_select;

    return tango_data;
}

// Function SearchTangoIntervals gets time intervals for defined condition on parameter, from the Tango database (MySQL connection defined in 'db_settings.h' file).
// NOTE: now it works only if channel count is constant during given time period
// First, name of the target table with required parameter is extracted from the Tango Base Table,
// then time intervals are selected from the target table corresponding the given condition and time interval.
// Parameters:
//	detector_name - name of the detector (e.g. "zdc" or "gem")
//	parameter_name - name of physical parameter stored in Tango (e.g. "uset" for ZDC or "u" for GEM)
//      date_start - time from which to start searching for time intervals satisfied the condition, format: "YYYY-MM-DD HH:MM:SS" (e.g. "2015-03-13 23:00:00")
//	date_end - end time of searching time intervals, the same format (e.g. "2015-03-13 24:00:00")
//      condition - condition of time interval sampling, default: conditionEqual (the possible list in 'uni_db/uni_db_structures.h')
//      value - boolean value for the condition with which the comparison is performed, default: true
//      mapChannel - vector of integer values (map) to change the order of result TObjArray-s in the common result array, if, for example, channels go in a different sequence; nullptr - if not used
// Returns common TObjArray with TObjArray objects containing TangoTimeInterval (i.e. conditionally TObjArray<TObjArray<TangoTimeInterval*>>),
// if no intervals found - returns the common TObjArray with zero TObjArray elements; in case of errors - returns nullptr
TObjArray* TangoData::SearchTangoIntervals(const char* detector_name, const char* parameter_name, const char* date_start, const char* date_end,
                                           enumConditions condition, bool value, vector<int>* mapChannel)
{
    // get Tango parameter values for the given time period
    TObjArray* tango_data = GetTangoParameter(detector_name, parameter_name, date_start, date_end);
    if (tango_data == nullptr) return nullptr;

    TObjArray* pTimeIntervals = new TObjArray();
    pTimeIntervals->SetOwner(kTRUE);

    // if there is no points in the Tango period then return empty common TObjArray (without any elements)
    if (tango_data->GetEntriesFast() == 0)
    {
        delete tango_data;
        return pTimeIntervals;
    }

    vector<int> vecStart;
    vector<bool> vecCondition;
    TangoTimeParameter* pParameter = nullptr;
    //cout<<"tango_data->GetEntriesFast(): "<<tango_data->GetEntriesFast()<<endl;
    for (int i = 0; i < tango_data->GetEntriesFast(); i++)
    {
        pParameter = (TangoTimeParameter*) tango_data->At(i);
        if (i == 0)
        {
            for (int j = 0; j < pParameter->bool_parameter_value.size(); j++)
            {
                vecCondition.push_back(false);
                vecStart.push_back(0);

                TObjArray* pChannel = new TObjArray();
                pChannel->SetOwner(kTRUE);
                pTimeIntervals->Add(pChannel);
            }
        }

        for (int j = 0; j < pParameter->bool_parameter_value.size(); j++)
        {
            bool isCondition;
            switch (condition)
            {
                case conditionLess:             isCondition = pParameter->bool_parameter_value[j] < value; break;
                case conditionLessOrEqual:      isCondition = pParameter->bool_parameter_value[j] <= value; break;
                case conditionEqual:            isCondition = pParameter->bool_parameter_value[j] == value; break;
                case conditionNotEqual:         isCondition = pParameter->bool_parameter_value[j] != value; break;
                case conditionGreater:          isCondition = pParameter->bool_parameter_value[j] > value; break;
                case conditionGreaterOrEqual:   isCondition = pParameter->bool_parameter_value[j] >= value; break;
                default:
                {
                    cout<<"ERROR: comparison operator in the searching of intervals is not appropriable for boolean type"<<endl;
                    delete tango_data;
                    delete pTimeIntervals;
                    return nullptr;
                }
            }

            if (isCondition)
            {
                // save start interval
                if (!vecCondition[j])
                {
                    vecCondition[j] = true;
                    vecStart[j] = i;
                    //cout<<"vecCondition "<<j<<": true"<<endl<<"Start: "<<i<<endl<<endl;
                }
            }
            else
            {
                // write interval
                if (vecCondition[j])
                {
                    vecCondition[j] = false;

                    int start_i = vecStart[j];
                    TDatime startInterval = ((TangoTimeParameter*)tango_data->At(start_i))->parameter_time;
                    if (start_i > 0)
                    {
                        TDatime previousTime = ((TangoTimeParameter*)tango_data->At(start_i-1))->parameter_time;
                        TDatime middleTime((startInterval.Convert() + previousTime.Convert()) / 2);
                        startInterval = middleTime;
                    }
                    TDatime endInterval((((TangoTimeParameter*)tango_data->At(i))->parameter_time.Convert() + ((TangoTimeParameter*)tango_data->At(i-1))->parameter_time.Convert()) / 2);

                    // create TangoTimeInterval for the satisfied condition
                    TangoTimeInterval* pTimeInterval = new TangoTimeInterval(startInterval, endInterval);
                    int real_channel = j;
                    if (mapChannel)
                        if (mapChannel->size() > j)
                        {
                            real_channel = mapChannel->at(j);
                            if (pTimeIntervals->GetEntriesFast() <= real_channel)
                            {
                                cout<<"CRITICAL WARNING: Tango array size is less than the corrected channel: "<<real_channel<<endl;
                                continue;
                            }
                        }
                        else cout<<"CRITICAL WARNING: map channel size is less than the current channel: "<<j<<endl;
                    //cout<<"j = "<<j<<", real_channel = "<<real_channel<<", mapChannel->size() = "<<mapChannel->size()<<", pTimeIntervals->GetEntriesFast() = "<<pTimeIntervals->GetEntriesFast()<<endl;
                    ((TObjArray*)pTimeIntervals->At(real_channel))->Add(pTimeInterval);
                }
            }
        }//for (int j = 0; j < pParameter->bool_parameter_value.size(); j++)
    }//for (int i = 0; i < tango_data->GetEntriesFast(); i++)

    // write the last period if neccessary
    for (int j = 0; j < pParameter->bool_parameter_value.size(); j++)
    {
        if (vecCondition[j])
        {
            vecCondition[j] = false;

            int start_i = vecStart[j];
            TDatime startInterval = ((TangoTimeParameter*)tango_data->At(start_i))->parameter_time;
            if (start_i > 0)
            {
                TDatime previousTime = ((TangoTimeParameter*)tango_data->At(start_i-1))->parameter_time;
                TDatime middleTime((startInterval.Convert() + previousTime.Convert()) / 2);
                startInterval = middleTime;
            }
            TDatime endInterval = ((TangoTimeParameter*)tango_data->At(tango_data->GetEntriesFast()-1))->parameter_time;

            TangoTimeInterval* pTimeInterval = new TangoTimeInterval(startInterval, endInterval);
            int real_channel = j;
            if (mapChannel)
                if (mapChannel->size() > j)
                {
                    real_channel = mapChannel->at(j);
                    if (pTimeIntervals->GetEntriesFast() <= real_channel)
                    {
                        cout<<"CRITICAL WARNING: Tango array size is less than the corrected channel: "<<real_channel<<endl;
                        continue;
                    }
                }
                else cout<<"CRITICAL WARNING: map channel size is less than the current channel: "<<j<<endl;
            //cout<<"j = "<<j<<", real_channel = "<<real_channel<<", mapChannel->size() = "<<mapChannel->size()<<", pTimeIntervals->GetEntriesFast() = "<<pTimeIntervals->GetEntriesFast()<<endl;
            ((TObjArray*)pTimeIntervals->At(real_channel))->Add(pTimeInterval);
        }
    }//write the last period if neccessary

    delete tango_data;
    return pTimeIntervals;
}

// Function PrintTangoDataConsole display hardware data obtained from Tango, e.g. ZDC voltage in time interval, in console
//      tango_data - TObjArray with TangoTimeParameter objects obtained from 'GetTangoParameter' function
void TangoData::PrintTangoDataConsole(TObjArray* tango_data)
{
    int def_precision = cout.precision();
    cout.precision(17);

    int par_length = 0;
    for (int i = 0; i < tango_data->GetEntriesFast(); i++)
    {
        TangoTimeParameter* pParameter = (TangoTimeParameter*) tango_data->At(i);
        cout<<pParameter->parameter_time.AsSQLString()<<endl;

        //cout<<TD->dataArray[i].parameter_length<<endl;
        par_length = pParameter->double_parameter_value.size();
        for (int j = 0; j < par_length; j++)
            cout<<pParameter->double_parameter_value[j]<<"  ";
        cout<<""<<endl<<endl;
    }

    cout.precision(def_precision);
    return;
}

// Function PrintTangoDataSurface display hardware vector data obtained from Tango, e.g. ZDC voltage in time interval, graphically as 2D Surface Graph
// Parameters:
//  tango_data - TObjArray with TangoTimeParameter objects obtained from 'GetTangoParameter' function
//  y_axis - label of Y axis
void TangoData::PrintTangoDataSurface(TObjArray* tango_data, const char* y_axis)
{
    TCanvas* c1 = new TCanvas("c1", "Tango Data", 800, 600);
    TGraph2D* tango_graph = new TGraph2D();
    tango_graph->SetTitle(tango_data->GetName());

    int par_length = 0;
    for (int i = 0; i < tango_data->GetEntriesFast(); i++)
    {
        TangoTimeParameter* pParameter = (TangoTimeParameter*) tango_data->At(i);

        par_length = pParameter->double_parameter_value.size();
        //cout<<"Parameter length = "<<par_length<<endl;
        for (int j = 0; j < par_length; j++)
        {
            int cur_time = pParameter->parameter_time.Convert();
            double value = pParameter->double_parameter_value[j];
            //cout<<x<<" "<<y<<" "<<z<<endl;
            tango_graph->SetPoint(i*par_length+j, cur_time, j+1, value);
        }
    }

    tango_graph->Draw("SURF1");
    gPad->Update();

    tango_graph->GetXaxis()->SetTitle("time");
    tango_graph->GetXaxis()->CenterTitle();
    tango_graph->GetXaxis()->SetTitleOffset(1.9);
    tango_graph->GetXaxis()->SetLabelSize(0.025);
    tango_graph->GetXaxis()->SetLabelOffset(0.009);
    tango_graph->GetXaxis()->SetNdivisions(-503);

    tango_graph->GetYaxis()->SetTitle(y_axis);
    tango_graph->GetYaxis()->CenterTitle();
    tango_graph->GetYaxis()->SetTitleOffset(1.9);
    tango_graph->GetYaxis()->SetLabelSize(0.025);
    tango_graph->GetYaxis()->SetLabelOffset(0.001);
    tango_graph->GetYaxis()->CenterLabels();
    tango_graph->GetYaxis()->SetNdivisions(par_length+1, 0, 0);

    tango_graph->GetZaxis()->SetLabelSize(0.025);

    // form X-axis with time ticks
    tango_graph->GetXaxis()->SetTimeDisplay(1);
    tango_graph->GetXaxis()->SetTimeFormat("%Y.%m.%d %H:%M");
    tango_graph->GetXaxis()->SetTimeOffset(0,"local");

    c1->Modified();

    return;
}

// Function PrintTangoDataMulti3D display hardware vector data obtained from Tango, e.g. ZDC voltage in time interval, graphically as a set of Line Graphs
// Parameter: tango_data - TObjArray with TangoTimeParameter objects obtained from 'GetTangoParameter' function
void TangoData::PrintTangoDataMultiGraph(TObjArray* tango_data, const char* y_axis, bool is3D)
{
    TCanvas* c1 = new TCanvas("c1", "Tango Data", 800, 600);

    int par_length = ((TangoTimeParameter*)tango_data->At(0))->double_parameter_value.size();
    TGraph* tango_graphs = new TGraph[par_length]();
    gRandom->SetSeed();
    for (int j = 0; j < par_length; j++)
    {
        Int_t color = gRandom->Integer(50);
        if ((color == 0) || (color == 10) || ((color > 16) && (color < 20))) color += 20;
        tango_graphs[j].SetLineColor(color);
    }

    for (int i = 0; i < tango_data->GetEntriesFast(); i++)
    {
        TangoTimeParameter* pParameter = (TangoTimeParameter*) tango_data->At(i);
        par_length = pParameter->double_parameter_value.size();
        for (int j = 0; j < par_length; j++)
        {
            int cur_time = pParameter->parameter_time.Convert();
            double value = pParameter->double_parameter_value[j];
            tango_graphs[j].SetPoint(i, cur_time, value);
        }
    }

    TMultiGraph* tango_multi = new TMultiGraph();
    tango_multi->SetTitle(tango_data->GetName());
    for (int j = 0; j < par_length; j++)
    {
        tango_multi->Add(&tango_graphs[j]);
        tango_graphs[j].SetTitle(TString::Format("%d", j));
        tango_graphs[j].SetLineWidth(3);
    }

    TString draw_par = "AL";
    if (is3D) draw_par += "3D";
    tango_multi->Draw(draw_par.Data());
    gPad->Update();

    if (!is3D)
    {
        tango_multi->GetXaxis()->SetTitle("time");
        tango_multi->GetXaxis()->CenterTitle();
        tango_multi->GetXaxis()->SetTitleOffset(1.3);
        tango_multi->GetXaxis()->SetLabelSize(0.025);
        tango_multi->GetXaxis()->SetLabelOffset(0.009);
        tango_multi->GetXaxis()->SetNdivisions(-503);
        // form X-axis with time ticks
        tango_multi->GetXaxis()->SetTimeDisplay(1);
        tango_multi->GetXaxis()->SetTimeFormat("%Y.%m.%d %H:%M");
        tango_multi->GetXaxis()->SetTimeOffset(0,"local");

        tango_multi->GetYaxis()->SetTitle(y_axis);
        tango_multi->GetYaxis()->CenterTitle();
        tango_multi->GetYaxis()->SetTitleOffset(1.3);
        tango_multi->GetYaxis()->SetLabelSize(0.025);
        tango_multi->GetYaxis()->SetLabelOffset(0.001);
        tango_multi->GetYaxis()->CenterLabels();
        //gr->GetYaxis()->SetNdivisions(10, 0, 0);
    }

    TLegend* pLegend = c1->BuildLegend(0.92, 0.77, 0.99, 0.98, "");
    pLegend->SetMargin(0.80);

    c1->Modified();
    return;
}

// Function PrintTangoIntervalConsole displays in console time intervals obtained from Tango for defined condition
// Parameters:
//  tango_intervals - TObjArray with TObjArray objects containing TangoTimeInterval objects obtained from 'SearchTangoIntervals' function
//  channel_name - name of the dimension to display on the screen, default: Channel
//  isShowOnlyExists - if true, print only intervals which satisfy the condition (skip empty intervals)
void TangoData::PrintTangoIntervalConsole(TObjArray* tango_intervals, TString channel_name, bool isShowOnlyExists)
{
    for (int i = 0; i < tango_intervals->GetEntriesFast(); i++)
    {
        TObjArray* pChannel = (TObjArray*) tango_intervals->At(i);
        if (pChannel->GetEntriesFast() == 0)
        {
            if (!isShowOnlyExists)
                cout<<channel_name.Data()<<" "<<i<<":"<<endl<<"   No intervals correspond to the specified conditions"<<endl;
            continue;
        }
        else
            cout<<channel_name.Data()<<" "<<i<<":"<<endl;
        for (int j = 0; j < pChannel->GetEntriesFast(); j++)
        {
            TangoTimeInterval* pInterval = (TangoTimeInterval*) pChannel->At(j);
            cout<<"   "<<pInterval->start_time.AsSQLString();
            cout<<" - ";
            cout<<pInterval->end_time.AsSQLString();
        }
        cout<<endl;
    }

    return;
}

// return average value for Tango data array (result vector with size greater than 1 is used in case of many channels)
vector<double> TangoData::GetAverageTangoData(TObjArray* tango_data)
{
    vector<double> result;
    int time_count = tango_data->GetEntriesFast();
    for (int i = 0; i < time_count; i++)
    {
        TangoTimeParameter* pParameter = (TangoTimeParameter*) tango_data->At(i);
        for (int j = 0; j < pParameter->double_parameter_value.size(); j++)
        {
            if (i == 0)
                result.push_back(pParameter->double_parameter_value[j]);
            else
                result[j] += pParameter->double_parameter_value[j];
        }
    }

    for (int j = 0; j < result.size(); j++)
       result[j] /= time_count;

    return result;
}

// -------------------------------------------------------------------
ClassImp(TangoData);
