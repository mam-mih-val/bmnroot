/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   read_Tof1_mapping.C
 * Author: mikhailr
 *
 * Created on August 27, 2021, 11:51 AM
 */

#include <cstdlib>
#include <vector>

using namespace std;

void read_Tof1_mapping(Int_t period = 7, Int_t run = 4700) {

    bool return_error = false;

    // get parameter of TDC's map
    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::GetDetectorParameter("TOF1", "TOF1_TdcMap", period, run); //(detector_name, parameter_name, period_number, run_number)
    if (pDetectorParameter == NULL) {
        cout << "\nMacro finished with errors" << endl;
        return;
    }

    //Print parameters
    vector<UniValue*> pValues;
    pDetectorParameter->GetValue(pValues);
    MapIntValue* tempMap = new MapIntValue;
    for (int i = 0; i < pValues.size(); i++) {
        tempMap = (MapIntValue*) pValues.at(i);
        cout << std::hex << tempMap->serial << std::dec << " : " << tempMap->channel << " -> " << std::hex << tempMap->value << std::dec << endl;
    }

    // clean memory after work
    for (int i = 0; i < pValues.size(); i++) delete pValues[i];
    if (pDetectorParameter)
        delete pDetectorParameter;


    //--------------------------------------------------------------------------
    // get parameter of Strip's map 
    UniDbDetectorParameter* pDetectorParameter2 = UniDbDetectorParameter::GetDetectorParameter("TOF1", "TOF1_StripMap", period, run); //(detector_name, parameter_name, period_number, run_number)
    if (pDetectorParameter2 == NULL) {
        cout << "\nMacro finished with errors" << endl;
        return;
    }

    //Print parameters
    vector<UniValue*> pValues2;
    pDetectorParameter2->GetValue(pValues2);
    MapDVectorValue* tempMap2 = new MapDVectorValue;
    cout << pValues2.size() << " lines in the Strip map " << endl;
    for (int i = 0; i < pValues2.size(); i++) {
        tempMap2 = (MapDVectorValue*) pValues2.at(i);
        cout << std::hex << tempMap2->serial << std::dec << " ch " << tempMap2->channel << " -> " << tempMap2->value.at(0) << "  " << tempMap2->value.at(1) << "  " << tempMap2->value.at(2) << endl;
    }

    // clean memory after work
    for (int i = 0; i < pValues2.size(); i++) delete pValues2[i];
    if (pDetectorParameter2)
        delete pDetectorParameter2;

    else
        return_error = true;

    if (return_error)
        cout << "\nMacro finished with errors" << endl;
    else
        cout << "\nMacro finished successfully" << endl;
}
