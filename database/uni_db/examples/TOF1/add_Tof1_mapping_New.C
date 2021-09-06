/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   add_Tof1_mapping_New.C
 * Author: mikhailr
 *
 * Created on August 24, 2021, 3:25 PM
 */

#include <cstdlib>
#include <vector>

using namespace std;

/*
 * 
 */
void add_Tof1_mapping_New(TString placement_map_filename = "TOF400_PlaceMap_RUN7.txt", TString main_map_filename = "TOF400_StripMap_RUN7.txt", int sP = 7, int sR = 2041, int eP = 7, int eR = 5184) {
    fstream ff;

    unsigned int fserial;
    int fchan;
    int fplane;
    int fstrip;
    char side_c;
    unsigned int fcrate;
    int fslot;
    bool side;

    // array for store in DB
    vector<UniValue*> PlMap;

    //Read Placement map
    //Stored in 1D UInt_t array format
    //(HEX: Create serial), (Slot), (HEX: TDC Serial), (HEX: Crate serial)...


    TString dir = Form("%s%s%s", getenv("VMCWORKDIR"), "/input/", placement_map_filename.Data());
    ff.open(dir.Data(), std::fstream::in);
    if (ff.fail()) {
        cout << "Failed to open " << placement_map_filename << endl;
        return;
    }
    while (!ff.eof()) {
        ff >> std::hex >> fcrate >> std::dec >> fslot >> std::hex >> fserial;
        if (ff.eof() || fcrate == 0)
            break;

        cout << std::hex << fcrate << " : " << std::dec << fslot << std::hex << " -> " << fserial << std::dec << endl;
        //Fill db value
        MapIntValue* tempMap = new MapIntValue;
        tempMap->serial = fcrate;
        tempMap->channel = fslot;
        tempMap->value = fserial;
        PlMap.push_back(tempMap);
    }
    ff.close();

    cout << PlMap.size() << " TDCs in the mapping " << endl;

    bool return_error = false;
    //create parameter in DB
//    UniDbParameter* pParameter = UniDbParameter::CreateParameter("TOF1_TdcMap", MapIntType, 0); //(TString parameter_name, int parameter_type)
//    delete pParameter;
    //write detector parameters in DB
    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter("TOF1", "TOF1_TdcMap", sP, sR, eP, eR, PlMap);
    if (pDetectorParameter == NULL)
        return_error = true;
    //
    //    // clean memory after work
    for (int i = 0; i < PlMap.size(); i++) delete PlMap[i];
    if (pDetectorParameter)
        delete pDetectorParameter;


    // read strip map    
    vector<UniValue*> StrMap;
    dir = Form("%s%s%s", getenv("VMCWORKDIR"), "/input/", main_map_filename.Data());
    ff.open(dir.Data(), std::fstream::in);
    if (ff.fail()) {
        cout << "Failed to open " << main_map_filename << endl;
        return;
    }


    //for (Int_t i = 0; i < 48; i++)
    while (!ff.eof()) {
        MapDVectorValue* tempMap2 = new MapDVectorValue;
        vector<Double_t> strmaptemp;
        ff >> std::hex >> fserial >> std::dec >> fchan >> fplane >> fstrip >> side_c;
        cout << std::hex << fserial << " : " << std::dec << fchan << "  " << fplane << "  " << fstrip << "  " << side_c << endl;

        strmaptemp.clear();
        //strmaptemp.resize(3);

        if (ff.eof() || fserial == 0 || fstrip == -1)
            continue;

        if (side_c != 'L' && side_c != 'R') {
            cout << "Wrong mapping file: Side is not L or R." << endl;
            continue;
        }
        tempMap2->serial = fserial;
        tempMap2->channel = fchan;
        //tempMap2->value.push_back(fplane);
        //tempMap2->value.push_back(fstrip);
        strmaptemp.push_back(fplane);
        strmaptemp.push_back(fstrip);
        if (side_c == 'L') {
            strmaptemp.push_back(0);
            //tempMap2->value.push_back(0);
        } else {
            strmaptemp.push_back(1);
            //tempMap2->value.push_back(1);
        }
        //       cout << "Write to strmaptemp: " << strmaptemp.at(0) << "  " << strmaptemp.at(1) << "  " << strmaptemp.at(2) << endl;
        tempMap2->value = strmaptemp;
        StrMap.push_back(tempMap2);

        //     cout << std::hex << tempMap2->serial << std::dec << " ch " << tempMap2->channel << " -> " << tempMap2->value.at(0) << "  " << tempMap2->value.at(1) << "  " << tempMap2->value.at(2) << endl;

    }
    ff.close();
    //
    //    MapDVectorValue* tempMap2 = new MapDVectorValue;
    //    for (int i = 0; i < StrMap.size(); i++) {
    //        tempMap2 = (MapDVectorValue*) StrMap.at(i);
    //        cout << std::hex << tempMap2->serial << std::dec << " ch " << tempMap2->channel << " -> " << tempMap2->value.at(0) << "  " << tempMap2->value.at(1) << "  " << tempMap2->value.at(2) << endl;
    //    }

    //create parameter in DB
//    UniDbParameter* pParameter2 = UniDbParameter::CreateParameter("TOF1_StripMap", MapDVectorType, 0); //(TString parameter_name, int parameter_type)
//    delete pParameter2;
    //write detector parameters in DB
    cout << "Write to DB " << endl;
    UniDbDetectorParameter* pDetectorParameter2 = UniDbDetectorParameter::CreateDetectorParameter("TOF1", "TOF1_StripMap", sP, sR, eP, eR, StrMap);
    if (pDetectorParameter2 == NULL)
        return_error = true;

    //clean memory after work
    cout << "clear memory " << endl;
    //    for (int i = 0; i < StrMap.size(); i++) delete StrMap[i];
    //    if (pDetectorParameter2)
    //        delete pDetectorParameter2;

    if (return_error)
        cout << "\nMacro finished with errors" << endl;
    else
        cout << "\nMacro finished successfully" << endl;
}

