//============================================================================
// Name        : show_simulation_files.cpp
// Author      : Konstantin Gertsenberger
// Copyright   : JINR
// Description : Get list of simulation files
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
            cout<<"Get list of simulation files satisfying the given parameters."<<endl<<"Parameters separated by comma:"<<endl<<"gen - generator name"<<endl
               <<"energy - collision energy (range is supported by '-' symbol)"<<endl<<"beam - first particle in collision (first beam)"<<endl
               <<"target - second particle in collision (second beam) OR target"<<endl<<"desc - text in description"<<endl
               <<"path - part of the file path"<<endl<<endl<<"Examples:"<<endl<<"show_simulation_files gen=QGSM,energy=9,beam=Au,target=Au"<<endl
               <<"show_simulation_files gen=urqmd,energy=5-9,desc=50K"<<endl;

            return 0;
        }
    }

    // parse parameter for DB searching
    bool isGen = false, isEnergy = false, isMinEnergy = false, isMaxEnergy = false, isBeam = false, isDesc = false, isTarget = false, isPath = false;
    string strGen, strBeam, strDesc, strTarget, strPath;
    double fEnergy, fMaxEnergy;
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

            // generator name parsing
            if ((token.length() > 4) && (token.substr(0,4) == "gen="))
            {
                isGen = true;
                strGen = token.substr(4);
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
                // beam particle's name in collision parsing
                else
                {
                    if ((token.length() > 5) && (token.substr(0,5) == "beam="))
                    {
                        isBeam = true;
                        strBeam = token.substr(5);
                    }
                    else
                    {
                        // search text in description string
                        if ((token.length() > 5) && (token.substr(0,5) == "desc="))
                        {
                            isDesc = true;
                            strDesc = token.substr(5);
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
                                // path parsing
                                if ((token.length() > 5) && (token.substr(0,5) == "path="))
                                {
                                    isPath = true;
                                    strPath = token.substr(5);
                                }
                            }//else TYPE of data
                        }//else DESC
                    }//else PARTICLE
                }//else ENERGY
            }//else GEN
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
                  "from simulation_file";

    bool isWhere = false;
    // if event generator selection
    if (isGen == true)
    {
        if (isWhere)
            sql += TString::Format(" AND lower(generator_name) = '%s'", strGen.data());
        else
        {
            isWhere = true;
            sql += TString::Format(" "
                                   "where lower(generator_name) = '%s'", strGen.data());
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
    if (isDesc == true)
    {
        if (isWhere)
            sql += TString::Format(" AND lower(file_desc) like '%%%s%%'", strDesc.data());
        else
        {
            isWhere = true;
            sql += TString::Format(" "
                                   "where lower(file_desc) like '%%%s%%'", strDesc.data());
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

    sql += " order by generator_name,energy";
    //cout<<sql<<endl;

    TSQLResult* res = pSQLServer->Query(sql);

    int nrows = res->GetRowCount();
    if (nrows == 0)
        cout<<"There are no simulation files for these parameters"<<endl;
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
