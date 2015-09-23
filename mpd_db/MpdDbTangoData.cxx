#include "MpdDbTangoData.h"

#include <TSQLServer.h>
#include <TSQLResult.h>
#include <TSQLRow.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TGraph2D.h>
#include <TMultiGraph.h>
#include <TAxis.h>
#include <TLegend.h>

#include <fstream>
#include <iostream>

using namespace std;

void MpdDbTangoData::SplitString(TString str, TString delim, vector<TString> &v)
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
TDatime MpdDbTangoData::StringToDatime(TString str_time)
{
    tm tmp;
    sscanf(str_time.Data(), "%2d.%2d.%4d %2d:%2d:%2d", &tmp.tm_mday, &tmp.tm_mon, &tmp.tm_year, &tmp.tm_hour, &tmp.tm_min, &tmp.tm_sec);
    TDatime ttime (tmp.tm_year, tmp.tm_mon, tmp.tm_mday, tmp.tm_hour, tmp.tm_min, tmp.tm_sec);

    return ttime;
}

// перевод строки формата "DD.MM.YYYY HH:MM:SS" в стандартный формат time_t
int MpdDbTangoData::StringToTime(TString str_time)
{
    tm tmp;
    sscanf(str_time.Data(),"%2d.%2d.%4d %2d:%2d:%2d", &tmp.tm_mday, &tmp.tm_mon, &tmp.tm_year, &tmp.tm_hour, &tmp.tm_min, &tmp.tm_sec);
    TDatime ttime (tmp.tm_year, tmp.tm_mon, tmp.tm_mday, tmp.tm_hour, tmp.tm_min, tmp.tm_sec);

    return ttime.Convert();
}


CSVData* MpdDbTangoData::GetCSVData(string filename)
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
                int n = temp.Length();
                ttemp = temp(1, n-2);
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
                int n = temp.Length();
                ttemp = temp(1, n-2);
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

void MpdDbTangoData::PrintCSVData(CSVData* zdcXY, bool isGraphicPresentation, bool isTimeCut, TDatime* start_time, TDatime* end_time)
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

HV_Data* MpdDbTangoData::GetTangoParameter(char* detector_name, char* parameter_name, char* date_start, char* date_end)
{
    //подключение к базе данных
    TSQLServer* db = TSQLServer::Connect("mysql://159.93.120.66","sima","password");
    db->GetTables("hdb");

    //запрос на поиск таблицы, с учётом заданных параметров
    TString query_one = TString::Format("SELECT id FROM adt WHERE family=\"%s\" and att_name=\"%s\" ", detector_name, parameter_name);
    TSQLResult* res = db->Query(query_one.Data());
    TSQLRow* row = res->Next();
    TString att = TString::Format("att_%s", (char*)(row->GetField(0)));
    cout<<"Название таблицы - "<<att<<endl;

    //запрос к таблице, найденной по номеру атрибута
    TString query_two = TString::Format("SELECT * FROM %s WHERE time>=\"%s\" and time<=\"%s\" ",att.Data(), date_start, date_end);
    res = db->Query(query_two.Data());
    int nrows = res->GetRowCount();
    cout<<"Количество строк в таблице - "<<nrows<<endl;

    HV_Data* VC = new HV_Data(nrows);

    TDatime datetime;
    int yy, mm, dd, hour, min, sec;

    //запись данных в структуру
    for (int i = 0; i < nrows; i++)
    {
        row = res->Next();
        char* str = (char *)(row->GetField(0));
        sscanf(str, "%d-%d-%d %d:%d:%d", &yy, &mm, &dd, &hour, &min, &sec);
        datetime.Set(yy, mm, dd, hour, min, sec);
        VC->hvArray[i].time_run = datetime;
        //cout<<VC[i].time_run.AsString()<<endl;

        char* t = (char *)(row->GetField(1));
        int tow = atoi(t);
        VC->hvArray[i].tower_count = tow;
        VC->hvArray[i].tower_hv = new double[tow];
        //cout<<VC[i].towers<<endl;

        char* sp = (char *)(row->GetField(2));
        char* split = strtok(sp,",");
        double m;
        int k = 0;
        while (split != NULL)
        {
            m = atof(split);
            VC->hvArray[i].tower_hv[k++] = m;
            //cout<<VC[i].voltage_tower[k]<<endl;
            split = strtok(NULL,", ");
            if (k > tow)
            {
                cout<<"Critical error: HV string includes more than tower count values"<<endl;
                return NULL;
            }
        }
    }

    return VC;
}

void MpdDbTangoData::PrintHV(HV_Data* VC, bool isGraphicPresentation)
{
    if (!isGraphicPresentation)
    {
        for (int i = 0; i < VC->rowCount; i++)
        {
            cout<<VC->hvArray[i].time_run.AsSQLString()<<endl;
            cout<<VC->hvArray[i].tower_count<<endl;
            for (int j = 0; j < VC->hvArray[i].tower_count; j++)
                cout<<VC->hvArray[i].tower_hv[j]<<"  ";
            cout<<""<<endl;
        }

        return;
    }

    TCanvas* c = new TCanvas("c", "Graph2D example", 0, 0, 600, 400);
    TGraph2D* gr2 = new TGraph2D();

    int tower_count;
    for (int i = 1; i <= VC->rowCount; i++)
    {
        tower_count = VC->hvArray[i-1].tower_count;
        for (int j = 1; j <= tower_count; j++)
        {
            double value = VC->hvArray[i-1].tower_hv[j-1];
            //cout<<x<<" "<<y<<" "<<z<<endl;
            gr2->SetPoint((i-1)*tower_count+j-1, i, j, value);
        }
    }

    gr2->Draw("surf1");

    return;
}

// -------------------------------------------------------------------
ClassImp(MpdDbTangoData);
