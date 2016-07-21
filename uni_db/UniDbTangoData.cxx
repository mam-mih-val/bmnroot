#include "UniDbTangoData.h"

#include <TSQLServer.h>
#include <TSQLResult.h>
#include <TSQLRow.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TGraph2D.h>
#include <TMultiGraph.h>
#include <TAxis.h>
#include <TLegend.h>
#include <TStyle.h>
#include <TH2D.h>

#include <fstream>
#include <iostream>

using namespace std;

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


CSVData* UniDbTangoData::GetCSVData(string filename)
{
    int size = 0;
    string s;
    ifstream fin(filename.c_str());

    if (!fin.is_open())
    {
        cout << "Файл не может быть открыт!\n";
        return NULL;
    }
    else
    {
        while (!fin.eof())
        {
            getline (fin, s, '"');
            if ((s == "DB_Klet_Pos.PsA") || (s == "DB_Portal_Pos.PsA"))
                size++;
        }
    }
    fin.seekg (0, fin.beg);

    CSVData* zdcXY = new CSVData(size);
    int count = 0;
    vector<TString> elements;
    TString temp, ttemp, delim = ";";

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
                zdcXY->dataArray[count].varName = "ZDC_X";
                temp = "";
                ttemp = "";
                temp = elements.at(1);
                int n_in = temp.Length();
                ttemp = temp(1, n_in-2);
                zdcXY->dataArray[count].runTime = StringToDatime(ttemp);
                temp = "";
                temp = elements.at(2);
                zdcXY->dataArray[count].varValue = temp.Atoi();
                count++;
            }
            if (ttemp == "DB_Klet_Pos.PsA")
            {
                zdcXY->dataArray[count].varName = "ZDC_Y";
                temp = "";
                ttemp = "";
                temp = elements.at(1);
                int n_in = temp.Length();
                ttemp = temp(1, n_in-2);
                zdcXY->dataArray[count].runTime = StringToDatime(ttemp);
                temp = "";
                temp = elements.at(2);
                zdcXY->dataArray[count].varValue = temp.Atoi();
                count++;
            }
        }
    }

    fin.close();
    elements.clear();

    return zdcXY;
}

void UniDbTangoData::PrintCSVData(CSVData* zdcXY, bool isGraphicPresentation, bool isTimeCut, TDatime* start_time, TDatime* end_time)
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
        for (int i = 0; i < zdcXY->dataCount; i++)
        {
            if ((!isTimeCut) || ((zdcXY->dataArray[i].runTime.Convert() >= start_time->Convert()) && (zdcXY->dataArray[i].runTime.Convert() <= end_time->Convert())))
                cout<< zdcXY->dataArray[i].varName<<" "<<zdcXY->dataArray[i].runTime.AsSQLString()<<" "<<zdcXY->dataArray[i].varValue<<" "<<endl;
        }

        return;
    }// if console presentation

    // if multigraph presentation
    const int N = (int) zdcXY->dataCount/2;
    int x[N], y[N], xx[N], yy[N];

    int count = 0;
    for (int i = 0; i < zdcXY->dataCount; i++)
    {
        if (zdcXY->dataArray[i].varName == "ZDC_X")
        {
            int cur_time = zdcXY->dataArray[i].runTime.Convert();
            if ((!isTimeCut) || ((cur_time > start_time->Convert()) && (cur_time <= end_time->Convert())))
            {
                x[count] = cur_time;
                y[count] = zdcXY->dataArray[i].varValue;
                count++;
            }
        }
        if (zdcXY->dataArray[i].varName == "ZDC_Y")
        {
            int cur_time = zdcXY->dataArray[i].runTime.Convert();
            if ((!isTimeCut) || ((cur_time > start_time->Convert()) && (cur_time <= end_time->Convert())))
            {
                xx[count] = cur_time;
                yy[count] = zdcXY->dataArray[i].varValue;
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

Tango_Double_Data* UniDbTangoData::GetTangoParameter(char* detector_name, char* parameter_name, char* date_start, char* date_end)
{
    //подключение к базе данных
    TSQLServer* db = TSQLServer::Connect("mysql://159.93.120.43","tango","tangompd");
    db->GetTables("hdbpp");

    //запрос на поиск в базовых таблицах, с учётом заданных параметров
    TString query_info = TString::Format(
                "SELECT att_conf_id,data_type FROM att_conf ac join att_conf_data_type acdt on ac.att_conf_data_type_id = acdt.att_conf_data_type_id WHERE family=\"%s\" and name=\"%s\" ",
                detector_name, parameter_name);
    TSQLResult* res = db->Query(query_info.Data());
    int nrows = res->GetRowCount();
    if (nrows < 1)
    {
        cout<<"Error: There is no parameter '"<<parameter_name<<"' for "<<detector_name<<" detector"<<endl;
        return NULL;
    }

    TSQLRow* row = res->Next();
    TString data_id((char*) row->GetField(0));
    TString table_name = TString::Format("att_%s", (char*)(row->GetField(1)));
    //cout<<"Data table name - "<<table_name<<endl;

    //запрос к таблице с данными, найденной по типу атрибута
    TString query_data = TString::Format("SELECT data_time, dim_x_r, dim_x_w, idx, value_r FROM %s WHERE att_conf_id=\"%s\" and data_time>=\"%s\" and data_time<=\"%s\" ORDER BY data_time,idx",
                                        table_name.Data(), data_id.Data(), date_start, date_end);
    //cout<<"Query data: "<<query_data<<endl;
    res = db->Query(query_data.Data());

    nrows = res->GetRowCount();
    if (nrows == 0)
    {
        cout<<"Warning: There are no parameters for the given conditions"<<endl;
        return NULL;
    }

    Tango_Double_Data* TD = NULL;
    TDatime datetime;
    int yy, mm, dd, hour, min, sec;

    //запись данных в структуру
    int data_ind = 0;
    for (int i = 0; i < nrows; i++)
    {
        row = res->Next();

        char* data_time = (char*) (row->GetField(0));
        sscanf(data_time, "%d-%d-%d %d:%d:%d", &yy, &mm, &dd, &hour, &min, &sec);
        datetime.Set(yy, mm, dd, hour, min, sec);

        char* par_len = (char*) row->GetField(1);
        int i_par_len = atoi(par_len);
        if (i_par_len == 0)
        {
            cout<<"Critical error: Parameter length can't be equal 0"<<endl;
            if (TD != NULL) delete TD;
            return NULL;
        }

        char* real_par_len = (char*) row->GetField(2);
        int i_real_par_len = atoi(real_par_len);
        if (i_real_par_len == 0)
        {
            cout<<"Critical error: Real parameter length can't be equal 0"<<endl;
            if (TD != NULL) delete TD;
            return NULL;
        }

        if (i == 0)
        {
            cout<<"Parameter length: "<<i_par_len<<" (real: "<<i_real_par_len<<"). Number of parameter values: "<<nrows/i_par_len<<"."<<endl;
            TD = new Tango_Double_Data(nrows/i_par_len);
        }

        TD->dataArray[data_ind].parameter_time = datetime;
        //cout<<TD[i].parameter_time.AsString()<<endl;

        TD->dataArray[data_ind].parameter_length = i_par_len;
        TD->dataArray[data_ind].parameter_value = new double[i_par_len];
        //cout<<TD[i].parameter_length<<endl;

        for (int ind = 0; ind < i_par_len; ind++)
        {
            if (ind > 0)
            {
                row = res->Next();
                i++;
            }

            char* idx = (char*) row->GetField(3);
            int i_idx = atoi(idx);
            //cout<<"idx:ind - "<<idx<<":"<<ind<<endl;
            if (i_idx != ind)
            {
                cout<<"Critical error: idx should be equal index of the parameter array"<<endl;
                if (TD != NULL) delete TD;
                return NULL;
            }

            char* val = (char*) row->GetField(4);
            double d_val = atof(val);
            TD->dataArray[data_ind].parameter_value[ind] = d_val;
            //cout<<TD[i].parameter_value[ind]<<endl;
        }

        data_ind++;
    }

    return TD;
}

void UniDbTangoData::PrintTangoData(Tango_Double_Data* TD, bool isGraphicPresentation)
{
    if (!isGraphicPresentation)
    {
        int def_precision = cout.precision();
        cout.precision(17);
        for (int i = 0; i < TD->dataCount; i++)
        {
            cout<<TD->dataArray[i].parameter_time.AsSQLString()<<endl;
            //cout<<TD->dataArray[i].parameter_length<<endl;
            for (int j = 0; j < TD->dataArray[i].parameter_length; j++)
                cout<<TD->dataArray[i].parameter_value[j]<<"  ";
            cout<<""<<endl<<endl;
        }
        cout.precision(def_precision);

        return;
    }

    TGraph2D* gr2 = new TGraph2D();
    gr2->SetTitle("2D Tango Data");

    int par_length;
    for (int i = 0; i < TD->dataCount; i++)
    {
        par_length = TD->dataArray[i].parameter_length;
        for (int j = 0; j < par_length; j++)
        {
            int cur_time = TD->dataArray[i].parameter_time.Convert();
            double value = TD->dataArray[i].parameter_value[j];
            //cout<<x<<" "<<y<<" "<<z<<endl;
            gr2->SetPoint(i*par_length+j, cur_time, j+1, value);
        }
    }

    TCanvas* c = new TCanvas("c", "2D Tango Data", 800, 600);

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

    c->Modified();

    return;
}

// -------------------------------------------------------------------
ClassImp(UniDbTangoData);
