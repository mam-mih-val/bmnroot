//============================================================================
// Name        : get_mpd_prod.cpp
// Author      : Konstantin Gertsenberger
// Copyright   : JINR
// Description : Get list of production files
//============================================================================

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
            cout<<"Get list of production files"<<endl<<"parameters separated by comma:"<<endl<<"gen - generator"<<endl<<"energy - collision energy"<<endl
                <<"particle - particles in collision"<<endl<<"desc - text in description"<<endl<<"type - type of data in file"<<endl<<endl<<"Examples:"<<endl
                <<"get_mpd_prod gen=QGSM,energy=9,particle=AuAu"<<endl<<"get_mpd_prod gen=urqmd,energy=5-9,desc=50K"<<endl;

            return 0;
        }
    }

    // parse parameter for DB searching
    bool isGen = false, isEnergy = false, isMinEnergy = false, isMaxEnergy = false, isParts = false, isDesc = false, isType = false;
    string strGen, strParts, strDesc, strType;
    double fEnergy, fMaxEnergy;
    for (int i = 1; i < argc; i++){
        //TString strParameter(argv[i]);
        //TObjArray* arrSplit = strParameter.Tokenize(",");

        string input = argv[i];
        istringstream ss(input);
        string token;

        // parse tokens by comma separated
        while(getline(ss, token, ','))
        {
            // to lowercase
            transform(token.begin(), token.end(), token.begin(), ::tolower);

            // generator name parsing
            if ((token.length() > 4) && (token.substr(0,4) == "gen=")){
                isGen = true;
                strGen = token.substr(4);
                //cout<<strGen<<endl;
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
                            //cout<<dVal<<endl;
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
                                //cout<<dVal<<endl;
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
                            //cout<<dVal<<endl;
                        }
                    }//else
                }//if ((token.length() > 7) && (token.substr(0,7) == "energy="))
                else
                {
                    // particles' names in collision parsing
                    if ((token.length() > 9) && (token.substr(0,9) == "particle=")){
                        isParts = true;
                        strParts = token.substr(9);
                        //cout<<strParts<<endl;
                    }
                    else
                    {
                        // search text in description string
                        if ((token.length() > 5) && (token.substr(0,5) == "desc=")){
                            isDesc = true;
                            strDesc = token.substr(5);
                            //cout<<strDesc<<endl;
                        }
                        else
                        {
                            // type of data parsing
                            if ((token.length() > 5) && (token.substr(0,5) == "type=")){
                                isType = true;
                                strType = token.substr(5);
                                //cout<<strType<<endl;
                            }
                        }//else DESC
                    }//else PARTICLE
                }//else ENERGY
            }//else GEN
        }
    }//for (int i = 1; i < argc; i++)

    // generate query
    TSQLServer* pSQLServer = TSQLServer::Connect("mysql://mpd.jinr.ru/data4mpd", "data4mpd", "MixedPhase");
    if (pSQLServer == 0x00){
        cout<<"Connection to database wasn't established"<<endl;
        return 0x00;
    }

    TString sql = "select data4mpd_path "
                  "from events";

    bool isWhere = false;
    // if event generator selection
    if (isGen == true)
    {
        if (isWhere){
            sql += TString::Format(" AND data4mpd_generator = '%s'", strGen.data());
        }
        else
        {
            isWhere = true;
            sql += TString::Format(" "
                                   "where data4mpd_generator = '%s'", strGen.data());
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
            sql += TString::Format("data4mpd_energy >= %f", fEnergy);
            if (isMaxEnergy)
                sql += TString::Format(" AND data4mpd_energy <= %f", fMaxEnergy);
        }
        else
        {
            if (isMaxEnergy)
                sql += TString::Format("data4mpd_energy <= %f", fMaxEnergy);
            else
                sql += TString::Format("data4mpd_energy = %f", fEnergy);
        }
    }
    // if 'particles in collision' selection
    if (isParts == true)
    {
        if (isWhere)
            sql += TString::Format(" AND data4mpd_collision = '%s'", strParts.data());
        else
        {
            isWhere = true;
            sql += TString::Format(" "
                                   "where data4mpd_collision = '%s'", strParts.data());
        }
    }
    if (isDesc == true)
    {
        if (isWhere)
            sql += TString::Format(" AND data4mpd_description like '%%%s%%'", strDesc.data());
        else
        {
            isWhere = true;
            sql += TString::Format(" "
                                   "where data4mpd_description like '%%%s%%'", strDesc.data());
        }
    }
    if (isType == true)
    {
        if (isWhere)
            sql += TString::Format(" AND data4mpd_datatype = '%s'", strType.data());
        else
        {
            isWhere = true;
            sql += TString::Format(" "
                                   "where data4mpd_datatype = '%s'", strType.data());
        }
    }

    //cout<<sql<<endl;

    TSQLResult* res = pSQLServer->Query(sql);

    int nrows = res->GetRowCount();
    if (nrows == 0)
        cout<<"There are no records for these parameters"<<endl;
    else
    {
        TSQLRow* row;
        while (row = res->Next())
        {
            TString path = row->GetField(0);
            cout<<path<<endl;

            delete row;
        }

        cout<<"Total count: "<<nrows<<endl;
    }

    delete res;

    if (pSQLServer)
        delete pSQLServer;

    return 0;
}
