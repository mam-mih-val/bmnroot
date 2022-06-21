/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   read_tof1_inl.C
 * Author: mikhailr
 *
 * Created on August 31, 2021, 3:06 PM
 */

#include <cstdlib>

using namespace std;

/*
 * 
 */
void read_tof1_inl(Int_t period = 7, Int_t run = 4700) {

     // get parameter of TDC's map
    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::GetDetectorParameter("TOF1", "inl_test", period, run); //(detector_name, parameter_name, period_number, run_number)
    if (pDetectorParameter == nullptr) {
        cout << "\nMacro finished with errors" << endl;
        return;
    }

    //Print parameters
    vector<UniValue*> pValues;
    pDetectorParameter->GetValue(pValues);
    TdcInlValue* tempInl = new TdcInlValue;
    Double_t SumInl;
    for (int i = 0; i < pValues.size(); i++) {
        tempInl = (TdcInlValue*) pValues.at(i);
        cout << std::hex << tempInl->serial << std::dec << endl;
//        for (Int_t i = 0; i < 72; i++){
//            SumInl = 0;
//            for (Int_t j = 0; j < 1024; j++){
//                SumInl += tempInl->inl[i][j];
//            }
//            cout << " read " << i << " channel. InlSum = " << SumInl << endl;
//        }
    }

    // clean memory after work
    for (int i = 0; i < pValues.size(); i++) delete pValues[i];
    if (pDetectorParameter)
        delete pDetectorParameter;
    
    return;
}

