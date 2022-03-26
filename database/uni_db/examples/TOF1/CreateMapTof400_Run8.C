/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */


#include <TMath.h>
#include "TString.h"
#include <iostream>
#include "Riostream.h"
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

Int_t GetSlot(Int_t number) {
    Int_t temp = static_cast<int> (number / 1000.);
    return temp;
}

Int_t GetCon(Int_t number) {
    Int_t temp = static_cast<int> ((number - GetSlot(number)*1000.) / 100.);
    return temp;
}

Int_t GetDet(Int_t number) {
    Int_t temp = static_cast<int> ((number - GetSlot(number)*1000. - GetCon(number)*100.) / 10.);
    return temp;
}

Int_t GetSide(Int_t number) {
    Int_t temp = static_cast<int> ((number - GetSlot(number)*1000. - GetCon(number)*100. - GetDet(number)*10.));
    return temp;
}

void CreateMapTof400_Run8(std::string placementMapFile = "../../../../input/TOF400_PlaceMap_RUN8_SRC.txt",
        std::string mapFile = "../../../../input/TOF400_StripMap_RUN8_SRC.txt") {

    Int_t MapR[24] = {3, 0, 9, 6, 4, 1, 10, 7, 5, 2, 11, 8, 18, 21, 12, 15, 19, 22, 13, 16, 20, 23, 14, 17};
    Int_t MapL[24] = {20, 23, 14, 17, 19, 22, 13, 16, 18, 21, 12, 15, 5, 2, 11, 8, 4, 1, 10, 7, 3, 0, 9, 6};
    //Int_t MapL[24] = {4, 1, 10, 7, 5, 2, 11, 8, 6, 3, 12, 9, 19, 22, 13, 16, 20, 23, 14, 17, 21, 0, 15, 18};
    //Int_t MapR[24] = {21, 0, 15, 18, 20, 23, 14, 17, 19, 22, 13, 16, 6, 3, 12, 9, 5, 2, 11, 8, 4, 1, 10, 7};

    UInt_t crate = 0x76d3027; //Right side slot 13
    static const Int_t kNSlot = 23;
    UInt_t slot[kNSlot] = {0};
    memset(slot, 0, sizeof (slot));

    // box2 det 15-19
    slot[1] = 0x06e9f5cb;
    slot[2] = 0x06e9da9d;
    slot[3] = 0x07a9e7d0;
    slot[4] = 0x076cad53;
    slot[5] = 0x06e940b2;
    slot[6] = 0x046f3872;
    slot[7] = 0x06e9404a;
    // box3 det 10-14
    slot[14] = 0x06e9403e;
    slot[15] = 0x06e9da86;
    slot[16] = 0x06e9043c;
    slot[17] = 0x06e940b1;
    slot[18] = 0x076ccf56;
    slot[19] = 0x06e94032;
    slot[20] = 0x076ccf5f;

    fstream ff, ffStrip;

    //1. Placement map
    ff.open(placementMapFile.c_str(), std::fstream::out);
    if (ff.fail()) {
        cerr << "Cannot open the file " << placementMapFile << endl;
        return;
    }
    for (Int_t i = 0; i < kNSlot; i++)
        if (slot[i] != 0) {
            ff << std::hex << std::setfill('0') << std::setw(8) << crate << '\t';
            ff << std::dec << i << '\t';
            ff << std::hex << std::setfill('0') << std::setw(8) << slot[i] << endl;
        }
    ff.close();


    Int_t Num = 14516;
    cout << "Num = " << Num << endl;
    cout << "GetSlot(Num) = " << GetSlot(Num) << " GetCon(Num) = " << GetCon(Num) << endl;
    cout << "GetDet(Num) = " << GetDet(Num) << " GetSide(Num) = " << GetSide(Num) << endl;


    //1. Fill Strip Map
    ffStrip.open(mapFile.c_str(), std::fstream::out);
    if (ffStrip.fail()) {
        cerr << "Cannot open the file " << mapFile << endl;
        return;
    }

    static const Int_t kNIt = 4;
    static const Int_t kNDet = 10;

    // Right arm
    Int_t MapDetL[kNDet][kNIt] = {
        //box2
        {1053, 1154, 1251, 2052}, //det 15
        {2163, 2264, 3061, 3162}, //det 16
        {3273, 4074, 4171, 4372}, //det 17
        {5083, 5184, 5281, 6082}, //det 18
        {6193, 6294, 7091, 7192}, //det 19
        //box3
        {14003, 14104, 14201, 15002}, //det 10
        {15113, 15214, 16011, 16112}, //det 11
        {16223, 17024, 17121, 17222}, //det 12
        {18033, 18134, 18231, 19032}, //det 13
        {19142, 19244, 20041, 20142}, //det 14
    };

    //first number - slot 1-22, second number - connector (0 - 0..23, 1 - 24..47, 2 - 48..71
    //third - detector , fourth - side (1,2 - L (Down) , 3,4 - R (Up))


    for (Int_t det = 0; det < kNDet; det++) {
        for (Int_t i = 0; i < kNIt; i++) {
            for (Int_t j = 0; j < 24; j++) {

                /*                  cout << std::dec << "MapDet = " << MapDet[det][i] << "; Det = " << det << "; i = " << i << "; j = " << j << "; slot = " << slot[GetSlot(MapDet[det][i])]
                                          << "; chTDC = " << j + 24 * GetCon(MapDet[det][i]) << "; GetDet = " << GetDet(MapDet[det][i])
                                          << "; GetSide = " << GetSide(MapDet[det][i]) << endl;
                                  getchar();
                 */

                ffStrip << std::hex << std::setfill('0') << std::setw(8) << slot[GetSlot(MapDetL[det][i])] << std::setfill(' ') << '\t';
                ffStrip << std::dec << j + 24 * GetCon(MapDetL[det][i]) << '\t';
                ffStrip << GetDet(MapDetL[det][i]) + 10 << '\t';
                if (GetSide(MapDetL[det][i]) == 1 || GetSide(MapDetL[det][i]) == 2) {
                    if (det < 5) ffStrip << 47 - (MapL[j] + (GetSide(MapDetL[det][i]) - 1)*24) << '\t';
                    else ffStrip << MapL[j] + (GetSide(MapDetL[det][i]) - 1)*24 << '\t';
                    //ffStrip << MapL[j] + (GetSide(MapDet[det][i]) - 1)*24 << '\t';
                    ffStrip << 'L';
                    ffStrip << endl;
                } else if (GetSide(MapDetL[det][i]) == 3 || GetSide(MapDetL[det][i]) == 4) {
                    if (det < 5) ffStrip << 47 - (MapR[j] + (GetSide(MapDetL[det][i]) - 3)*24) << '\t';
                    else ffStrip << MapR[j] + (GetSide(MapDetL[det][i]) - 3)*24 << '\t';
                    //ffStrip << MapR[j] + (GetSide(MapDet[det][i]) - 3)*24 << '\t';
                    ffStrip << 'R';
                    ffStrip << endl;
                } else {
                    ffStrip << -1 << '\t';
                    ffStrip << 'L';
                    ffStrip << endl;
                }
            }
        }
    }
    ffStrip.close();
    // 

    //Left side
    memset(slot, 0, sizeof (slot));
    //box 1
    crate = 0x06ea9711; // slot 10

    //box1
    slot[2] = 0x076ccf6e;
    slot[3] = 0x06e9404b;
    slot[4] = 0x076cae7b;
    slot[5] = 0x076caef6;
    slot[6] = 0x06e9da91;
    slot[7] = 0x076cae7a;
    slot[8] = 0x07a8deb2;
    //box4
    slot[15] = 0x06e94033;
    slot[16] = 0x046f408c;
    slot[17] = 0x06e9e4b0;
    slot[18] = 0x06e940be;
    slot[19] = 0x076ccf62;
    slot[20] = 0x076cad52;
    slot[21] = 0x07a9fbf8;


    //2. Fill Placement map
    ff.open(placementMapFile.c_str(), std::fstream::in | std::fstream::out | std::fstream::app);
    if (ff.fail()) {
        cerr << "2. Cannot open the file " << placementMapFile << endl;
        return;
    }
    for (Int_t i = 0; i < kNSlot; i++)
        if (slot[i] != 0) {
            ff << std::hex << std::setfill('0') << std::setw(8) << crate << '\t';
            ff << std::dec << i << '\t';
            ff << std::hex << std::setfill('0') << std::setw(8) << slot[i] << endl;
        }
    ff.close();

    //fill it !!!!!!!!!!!!!!!!!

    Int_t MapDetR[kNDet][kNIt] = {
        //Box1 det 0-4
        {2003, 2104, 2201, 5202},
        {3113, 3214, 4011, 4112},
        {4223, 5024, 5121, 3022},
        {6033, 6134, 6231, 7032},
        {7143, 7244, 8041, 8142},
        //box4 det 5-9
        {15053, 15154, 15251, 16052},
        {16163, 16264, 17061, 17162},
        {17273, 18074, 18171, 18272},
        {19083, 19184, 19281, 20082},
        {20193, 20294, 21091, 21192},
    };

    //2. Fill Strip Map
    ffStrip.open(mapFile.c_str(), std::fstream::in | std::fstream::out | std::fstream::app);
    if (ffStrip.fail()) {
        cerr << "2. Cannot open the file " << mapFile << endl;
        return;
    }

    //first number - slot 1-22, second number - connector (0 - 0..23, 1 - 24..47, 2 - 48..71
    //third - detector , fourth - side (1,2 - L, 3,4 - R)


    for (Int_t det = 0; det < kNDet; det++) {
        for (Int_t i = 0; i < kNIt; i++) {
            for (Int_t j = 0; j < 24; j++) {
                /*
                                cout << std::dec << "MapDet = " << MapDet[det][i] << "; Det = " << det << "; i = " << i << "; j = " << j << "; slot = " << slot[GetSlot(MapDet[det][i])]
                                        << "; chTDC = " << j + 24 * GetCon(MapDet[det][i]) << "; GetDet = " << GetDet(MapDet[det][i])
                                        << "; GetSide = " << GetSide(MapDet[det][i]) << endl;
                                getchar();
                 */

                ffStrip << std::hex << std::setfill('0') << std::setw(8) << slot[GetSlot(MapDetR[det][i])] << std::setfill(' ') << '\t';
                ffStrip << std::dec << j + 24 * GetCon(MapDetR[det][i]) << '\t';
                ffStrip << GetDet(MapDetR[det][i]) << '\t';
                if (GetSide(MapDetR[det][i]) == 1 || GetSide(MapDetR[det][i]) == 2) {
                    Int_t str_temp = -1;
                    Int_t str_temp2 = -1;
                    if (det >= 5) {

                        str_temp = 47 - (MapL[j] + (GetSide(MapDetL[det][i]) - 1)*24);
                        if (str_temp < 24) str_temp2 = 48 - (23 - str_temp) - 1;
                        else str_temp2 = 48 - (71 - str_temp) - 1;

                    } else {
                        cout << "Det " << det << endl;
                        str_temp = MapL[j] + (GetSide(MapDetL[det][i]) - 1)*24;
                        if (str_temp < 24) str_temp2 = 48 - (23 - str_temp) - 1;
                        else str_temp2 = 48 - (71 - str_temp) - 1;
                        cout << "str_temp = " << str_temp2 << "; str_temp2 = " << str_temp << endl;
                    }

                    if (str_temp2 < 24) str_temp = 23 - str_temp2;
                    else str_temp = 71 - str_temp2;


                    ffStrip << str_temp << '\t';
                    ffStrip << 'L';
                    ffStrip << endl;
                } else if (GetSide(MapDetR[det][i]) == 3 || GetSide(MapDetR[det][i]) == 4) {
                    Int_t str_temp = -1;
                    Int_t str_temp2 = -1;
                    if (det >= 5) {
                        str_temp = 47 - (MapR[j] + (GetSide(MapDetR[det][i]) - 3)*24);
                        if (str_temp < 24) str_temp2 = 48 - (23 - str_temp) - 1;
                        else str_temp2 = 48 - (71 - str_temp) - 1;
                    } else {
                        str_temp = MapR[j] + (GetSide(MapDetR[det][i]) - 3)*24;
                        if (str_temp < 24) str_temp2 = 48 - (23 - str_temp) - 1;
                        else str_temp2 = 48 - (71 - str_temp) - 1;
                    }

                    if (str_temp2 < 24) str_temp = 23 - str_temp2;
                    else str_temp = 71 - str_temp2;

                    ffStrip << str_temp << '\t';
                    ffStrip << 'R';
                    ffStrip << endl;
                } else {
                    ffStrip << -1 << '\t';
                    ffStrip << 'L';
                    ffStrip << endl;
                }
            }
        }
    }
    ffStrip.close();
}

