#include "db_settings.h"
#include "UniDbTangoData.h"

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

void UniDbTangoData::SplitString(TString str, TString delim, vector<TString> &v)
{
    v.clear();

    int stringLength = str.Length();
    int delimLength = delim.Length();

    int stop = 1;
    TString temp = "";
    while (stop != -1)
    {
        stop = str.First(delim);
        if (stop != -1)
        {
            temp = str(0, stop);
            TSubString newString = str(stop+delimLength, stringLength);
            str = newString;
            stringLength = str.Length();
        }
        else
        {
            stringLength = str.Length();
            temp = str(0, stringLength);
        }

        v.push_back(temp);
    }
}

UniDbTangoData::UniDbTangoData(){
}

UniDbTangoData::~UniDbTangoData(){
}

// перевод строки формата "DD.MM.YYYY HH:MM:SS" в класс TDatime
TDatime UniDbTangoData::StringToDatime(TString str_time)
{
    tm tmp;
    sscanf(str_time.Data(), "%2d.%2d.%4d %2d:%2d:%2d", &tmp.tm_mday, &tmp.tm_mon, &tmp.tm_year, &tmp.tm_hour, &tmp.tm_min, &tmp.tm_sec);
    TDatime ttime (tmp.tm_year, tmp.tm_mon, tmp.tm_mday, tmp.tm_hour, tmp.tm_min, tmp.tm_sec);

    return ttime;
}

// перевод строки формата "DD.MM.YYYY HH:MM:SS" в стандартный формат time_t
int UniDbTangoData::StringToTime(TString str_time)
{
    tm tmp;
    sscanf(str_time.Data(),"%2d.%2d.%4d %2d:%2d:%2d", &tmp.tm_mday, &tmp.tm_mon, &tmp.tm_year, &tmp.tm_hour, &tmp.tm_min, &tmp.tm_sec);
    TDatime ttime (tmp.tm_year, tmp.tm_mon, tmp.tm_mday, tmp.tm_hour, tmp.tm_min, tmp.tm_sec);

    return ttime.Convert();
}


vector<CSVElement>* UniDbTangoData::GetCSVData(string filename)
{
    ifstream fin(filename.c_str());
    if (!fin.is_open())
    {
        cout<<"Error was occured while opening the file!\n";
        return NULL;
    }

    vector<CSVElement>* zdcXY = new vector<CSVElement>();
    vector<TString> elements;
    TString temp, ttemp, delim = ";";
    string s;
    while (!fin.eof())
    {
        getline(fin, s);
        if (!s.empty())
        {
            temp = s;
            SplitString(temp, delim, elements);

            temp = "";
            temp = elements.at(0);
            int n = temp.Length();
            ttemp = temp(1, n-2);

            if (ttemp == "DB_Portal_Pos.PsA")
            {
                CSVElement par;
                par.varName = "ZDC_X";
                temp = "";
                ttemp = "";
                temp = elements.at(1);
                int n_in = temp.Length();
                ttemp = temp(1, n_in-2);
                par.runTime = StringToDatime(ttemp);
                temp = "";
                temp = elements.at(2);
                par.varValue = temp.Atoi();

                zdcXY->push_back(par);
            }
            if (ttemp == "DB_Klet_Pos.PsA")
            {
                CSVElement par;
                par.varName = "ZDC_Y";
                temp = "";
                ttemp = "";
                temp = elements.at(1);
                int n_in = temp.Length();
                ttemp = temp(1, n_in-2);
                par.runTime = StringToDatime(ttemp);
                temp = "";
                temp = elements.at(2);
                par.varValue = temp.Atoi();

                zdcXY->push_back(par);
            }
        }
    }

    fin.close();
    elements.clear();

    return zdcXY;
}

void UniDbTangoData::PrintCSVData(vector<CSVElement>* zdcXY, bool isGraphicPresentation, bool isTimeCut, TDatime* start_time, TDatime* end_time)
{
    if (isTimeCut)
    {
        if ((start_time == NULL) || (end_time == NULL))
        {
            cout<<"Error: Start and end cut time can't be equal NULL"<<endl;
            return;
        }
    }

    // if console presentation
    if (!isGraphicPresentation)
    {
        for (int i = 0; i < zdcXY->size(); i++)
        {
            if ((!isTimeCut) || ((zdcXY->at(i).runTime.Convert() >= start_time->Convert()) && (zdcXY->at(i).runTime.Convert() <= end_time->Convert())))
                cout<< zdcXY->at(i).varName<<" "<<zdcXY->at(i).runTime.AsSQLString()<<" "<<zdcXY->at(i).varValue<<" "<<endl;
        }

        return;
    }// if console presentation

    // if multigraph presentation
    const int N = (int) zdcXY->size()/2;
    int x[N], y[N], xx[N], yy[N];

    int count = 0;
    for (int i = 0; i < zdcXY->size(); i++)
    {
        if (zdcXY->at(i).varName == "ZDC_X")
        {
            int cur_time = zdcXY->at(i).runTime.Convert();
            if ((!isTimeCut) || ((cur_time > start_time->Convert()) && (cur_time <= end_time->Convert())))
            {
                x[count] = cur_time;
                y[count] = zdcXY->at(i).varValue;
                count++;
            }
        }
        if (zdcXY->at(i).varName == "ZDC_Y")
        {
            int cur_time = zdcXY->at(i).runTime.Convert();
            if ((!isTimeCut) || ((cur_time > start_time->Convert()) && (cur_time <= end_time->Convert())))
            {
                xx[count] = cur_time;
                yy[count] = zdcXY->at(i).varValue;
            }
        }
    }

    TCanvas *c1 = new TCanvas("c1", "Coord X and Y", 800, 600);

    TGraph* gr1 = new TGraph(count, x, y);
    TGraph* gr2 = new TGraph(count, xx, yy);

    gr1->SetMarkerColor(2); // красный - Х
    gr2->SetMarkerColor(3); // зеленый - Y

    TMultiGraph* mg = new TMultiGraph();
    mg->Add(gr1);
    mg->Add(gr2);
    mg->Draw("ap");

    //формирование оси с отрезками времени
    mg->GetXaxis()->SetTimeDisplay(1);
    mg->GetXaxis()->SetNdivisions(-503);
    mg->GetXaxis()->SetTimeFormat("%Y.%m.%d %H:%M");
    mg->GetXaxis()->SetTimeOffset(0,"gmt");

    gr1->SetMarkerStyle(21);
    gr2->SetMarkerStyle(21);

    TLegend* leg = new TLegend(.7, .9, .9, 1.);
    leg->AddEntry(gr1,"x coordinate","p");
    leg->AddEntry(gr2,"y coordinate","p");
    leg->Draw();

    return;
}

TObjArray* UniDbTangoData::GetTangoParameter(char* detector_name, char* parameter_name, char* date_start, char* date_end)
{
    // TANGO database connection
    TString strConnection = TString::Format("mysql://%s", TANGO_DB_HOST);
    TSQLServer* db = TSQLServer::Connect(strConnection, TANGO_DB_USERNAME, TANGO_DB_PASSWORD);
    db->GetTables(TANGO_DB_NAME);

    // searching for parameters with given parameter_name for detector with detector_name
    TString strStatement = TString::Format(
                "SELECT att_conf_id,data_type FROM att_conf ac join att_conf_data_type acdt on ac.att_conf_data_type_id = acdt.att_conf_data_type_id WHERE family=\"%s\" and name=\"%s\" ",
                detector_name, parameter_name);
    TSQLStatement* stmt_select = db->Statement(strStatement);
    if (!stmt_select->Process())
    {
            cout<<"Error: getting info about parameter from Tango has been failed"<<endl;

            delete stmt_select;
            delete db;
            return NULL;
    }

    stmt_select->StoreResult();
    if (!stmt_select->NextResultRow())
    {
        cout<<"Error: There is no parameter '"<<parameter_name<<"' for "<<detector_name<<" detector"<<endl;
        delete stmt_select;
        delete db;
        return NULL;
    }

    int data_id = stmt_select->GetInt(0);
    TString data_type(stmt_select->GetString(1));
    TString table_name = TString::Format("att_%s", data_type.Data());
    //cout<<"Data table name - "<<table_name<<endl;
    delete stmt_select;

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
                cout<<"Error: This Tango type is not supported: '"<<data_type<<"'"<<endl;
                delete db;
                return NULL;
            }
        }
    }

    // getting data from the table found by attribute type
    TString query_data = "";
    if (par_type > 10)
        query_data = TString::Format("SELECT data_time, value_r, dim_x_r, dim_x_w, idx FROM %s WHERE att_conf_id=\"%d\" and data_time>=\"%s\" and data_time<=\"%s\" ORDER BY data_time,idx",
                                     table_name.Data(), data_id, date_start, date_end);
    else
        query_data = TString::Format("SELECT data_time, value_r FROM %s WHERE att_conf_id=\"%d\" and data_time>=\"%s\" and data_time<=\"%s\" ORDER BY data_time",
                                      table_name.Data(), data_id, date_start, date_end);

    //cout<<"Query data: "<<query_data<<endl;
    stmt_select = db->Statement(query_data);
    if (!stmt_select->Process())
    {
            cout<<"Error: getting info about parameter values from Tango has been failed"<<endl;

            delete stmt_select;
            delete db;
            return NULL;
    }

    stmt_select->StoreResult();

    // write data to the result array
    TObjArray* tango_data = new TObjArray();
    tango_data->SetOwner(kTRUE);
    int i = 0;
    while (stmt_select->NextResultRow())
    {
        TDatime datetime = stmt_select->GetTimestamp(0);

        int i_par_len = 1;
        if (par_type > 10) i_par_len = stmt_select->GetInt(2);
        if (i_par_len == 0)
        {
            cout<<"Error: Parameter length can't be equal 0"<<endl;
            delete tango_data;
            delete stmt_select;
            delete db;
            return NULL;
        }

        int i_real_par_len = 1;
        if (par_type > 10) i_real_par_len = stmt_select->GetInt(3);
        if (i_real_par_len == 0)
        {
            cout<<"Error: Real parameter length can't be equal 0"<<endl;
            delete tango_data;
            delete stmt_select;
            delete db;
            return NULL;
        }

        //if (i++ == 0) cout<<"Parameter length: "<<i_par_len<<" (real: "<<i_real_par_len<<"). Number of time points: "<<stmt_select->GetNumAffectedRows()/i_par_len<<"."<<endl;

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
                cout<<"Error: idx should be equal index of the parameter array"<<endl;
                delete tango_data;
                delete stmt_select;
                delete db;
                return NULL;
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
    delete db;

    return tango_data;
}

// now it works only if channel count is constant during given time period
TObjArray* UniDbTangoData::SearchTangoIntervals(char* detector_name, char* parameter_name, char* date_start, char* date_end, enumConditions condition, bool value, int* mapChannel)
{
    TObjArray* tango_data = GetTangoParameter(detector_name, parameter_name, date_start, date_end);
    if (tango_data == NULL) return NULL;

    TObjArray* pTimeIntervals = new TObjArray();
    pTimeIntervals->SetOwner(kTRUE);

    // if there is no points in the Tango interval then return TimeIntervals collection without any elements
    if (tango_data->GetEntriesFast() == 0)
    {
        delete tango_data;
        return pTimeIntervals;
    }

    vector<int> vecStart;
    vector<bool> vecCondition;
    TangoTimeParameter* pParameter = NULL;
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
                    cout<<"Error: comparison operator in the searching of intervals is not appropriable for boolean type"<<endl;
                    delete tango_data;
                    delete pTimeIntervals;
                    return NULL;
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

                    TangoTimeInterval* pTimeInterval = new TangoTimeInterval(startInterval, endInterval);
                    int real_channel = j;
                    if (mapChannel) real_channel = mapChannel[j];
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
            if (mapChannel) real_channel = mapChannel[j];
            ((TObjArray*)pTimeIntervals->At(real_channel))->Add(pTimeInterval);
        }
    }//write the last period if neccessary

    delete tango_data;
    return pTimeIntervals;
}

void UniDbTangoData::PrintTangoDataConsole(TObjArray* tango_data)
{
    int def_precision = cout.precision();
    cout.precision(17);
    for (int i = 0; i < tango_data->GetEntriesFast(); i++)
    {
        TangoTimeParameter* pParameter = (TangoTimeParameter*) tango_data->At(i);
        cout<<pParameter->parameter_time.AsSQLString()<<endl;
        //cout<<TD->dataArray[i].parameter_length<<endl;

        for (int j = 0; j < pParameter->double_parameter_value.size(); j++)
        {
            cout<<pParameter->double_parameter_value[j]<<"  ";
        }
        cout<<""<<endl<<endl;
    }
    cout.precision(def_precision);

    return;
}

void UniDbTangoData::PrintTangoDataSurface(TObjArray* tango_data)
{
    TCanvas* c1 = new TCanvas("c1", "Tango Data", 800, 600);

    TGraph2D* gr2 = new TGraph2D();
    gr2->SetTitle("Tango Data (surface)");

    int par_length = 0;
    for (int i = 0; i < tango_data->GetEntriesFast(); i++)
    {
        TangoTimeParameter* pParameter = (TangoTimeParameter*) tango_data->At(i);
        par_length = pParameter->double_parameter_value.size();
        for (int j = 0; j < par_length; j++)
        {
            int cur_time = pParameter->parameter_time.Convert();
            double value = pParameter->double_parameter_value[j];
            //cout<<x<<" "<<y<<" "<<z<<endl;
            gr2->SetPoint(i*par_length+j, cur_time, j+1, value);
        }
    }

    gr2->Draw("SURF1");
    gPad->Update();

    gr2->GetXaxis()->SetTitle("time");
    gr2->GetXaxis()->CenterTitle();
    gr2->GetXaxis()->SetTitleOffset(1.9);
    gr2->GetXaxis()->SetLabelSize(0.025);
    gr2->GetXaxis()->SetLabelOffset(0.009);
    gr2->GetXaxis()->SetNdivisions(-503);

    gr2->GetYaxis()->SetTitle("index");
    gr2->GetYaxis()->CenterTitle();
    gr2->GetYaxis()->SetTitleOffset(1.9);
    gr2->GetYaxis()->SetLabelSize(0.025);
    gr2->GetYaxis()->SetLabelOffset(0.001);
    gr2->GetYaxis()->CenterLabels();
    gr2->GetYaxis()->SetNdivisions(par_length+1, 0, 0);

    gr2->GetZaxis()->SetLabelSize(0.025);

    // form X-axis with time ticks
    gr2->GetXaxis()->SetTimeDisplay(1);
    gr2->GetXaxis()->SetTimeFormat("%Y.%m.%d %H:%M");
    gr2->GetXaxis()->SetTimeOffset(0,"local");

    c1->Modified();

    return;
}

void UniDbTangoData::PrintTangoDataMulti3D(TObjArray* tango_data)
{
    TCanvas* c1 = new TCanvas("c1", "Tango Data", 800, 600);

    int par_length = ((TangoTimeParameter*)tango_data->At(0))->double_parameter_value.size();
    TGraph* gr = new TGraph[par_length]();
    gRandom->SetSeed();
    for (int j = 0; j < par_length; j++)
    {
        Int_t color = gRandom->Integer(50);
        if ((color == 0) || (color == 10) || ((color > 16) && (color < 20))) color += 20;
        gr[j].SetLineColor(color);
        gr[j].SetTitle(TString::Format("%d", j));
        gr[j].SetLineWidth(3);
    }

    for (int i = 0; i < tango_data->GetEntriesFast(); i++)
    {
        TangoTimeParameter* pParameter = (TangoTimeParameter*) tango_data->At(i);
        par_length = pParameter->double_parameter_value.size();
        for (int j = 0; j < par_length; j++)
        {
            //int cur_time = TD->dataArray[i].parameter_time.Convert();
            double value = pParameter->double_parameter_value[j];
            gr[j].SetPoint(i, i, value);
        }
    }

    TMultiGraph* mg = new TMultiGraph();
    mg->SetTitle("Tango Data (multigraph)");
    for (int j = 0; j < par_length; j++)
        mg->Add(&gr[j]);

    mg->Draw("AL3D");
    gPad->Update();

    c1->Modified();
    return;
}

void UniDbTangoData::PrintTangoIntervalConsole(TObjArray* tango_data, TString channel_name)
{
    for (int i = 0; i < tango_data->GetEntriesFast(); i++)
    {
        cout<<channel_name.Data()<<" "<<i<<":"<<endl;
        TObjArray* pChannel = (TObjArray*) tango_data->At(i);
        for (int j = 0; j < pChannel->GetEntriesFast(); j++)
        {
            TangoTimeInterval* pInterval = (TangoTimeInterval*) pChannel->At(j);
            cout<<"   "<<pInterval->start_time.AsSQLString();
            cout<<" - ";
            cout<<pInterval->end_time.AsSQLString();
        }
        if (pChannel->GetEntriesFast() == 0) cout<<"   No intervals correspond to the specified conditions";
        cout<<endl;
    }

    return;
}

vector<double> UniDbTangoData::GetAverageTangoData(TObjArray* tango_data)
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
ClassImp(UniDbTangoData);
