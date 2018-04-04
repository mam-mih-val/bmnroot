
#include <TMath.h>
#include "TString.h"
#include <iostream>
#include "Riostream.h"
#include <string>
#include <fstream>
#include <sstream>

void CreateMapTof400_RUN7(std::string placementMapFile = "TOF400_PlaceMap_RUN7.txt",
        std::string mapFile = "TOF400_StripMap_RUN7.txt") {

    Int_t MapR[24] = {3, 0, 9, 6, 4, 1, 10, 7, 5, 2, 11, 8, 18, 21, 12, 15, 19, 22, 13, 16, 20, 23, 14, 17};
    Int_t MapL[24] = {20, 23, 14, 17, 19, 22, 13, 16, 18, 21, 12, 15, 5, 2, 11, 8, 4, 1, 10, 7, 3, 0, 9, 6};
    //Int_t MapL[24] = {4, 1, 10, 7, 5, 2, 11, 8, 6, 3, 12, 9, 19, 22, 13, 16, 20, 23, 14, 17, 21, 0, 15, 18};
    //Int_t MapR[24] = {21, 0, 15, 18, 20, 23, 14, 17, 19, 22, 13, 16, 6, 3, 12, 9, 5, 2, 11, 8, 4, 1, 10, 7};

    //UInt_t crate = 0x01690454;
    UInt_t crate = 0x76d3027;
    static const Int_t kNSlot = 23;
    UInt_t slot[kNSlot] = {0.};
    slot[1] = 0x076ccf6e;
    slot[2] = 0x06e9da91;
    slot[3] = 0x076caef6;
    slot[4] = 0x076cae7b;
    slot[5] = 0x046f332f;
    slot[6] = 0x06e9404b;
    slot[7] = 0x06e9e4b0;
    slot[14] = 0x046f408c;
    slot[15] = 0x06e94033;
    slot[16] = 0x076cae7a;
    slot[17] = 0x07a8deb2;
    slot[18] = 0x076cad52;
    slot[19] = 0x076ccf62;
    slot[20] = 0x06e940be;

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
    static const Int_t kNDet = 10 + 1;

    // Left arm
    Int_t MapDetL[kNDet][kNIt] = {
        {1202, 1104, 1001, 2203},
        {2112, 2014, 3211, 3113},
        {3022, 4224, 4121, 4023},
        {5232, 5134, 5031, 6233},
        {6142, 6044, 7241, 7143},
        {14252, 14154, 14051, 15253},
        {15162, 15064, 16261, 16163},
        {16072, 17274, 17171, 17073},
        {18282, 18184, 18081, 19283},
        {19192, 19094, 20291, 20193},
        {7005, 20005, 7005, 20005}
    };

    //first number - slot 1-22, second number - connector (0 - 0..23, 1 - 24..47, 2 - 48..71
    //third - detector , fourth - side (1,2 - L, 3,4 - R)


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
                ffStrip << GetDet(MapDetL[det][i]) << '\t';
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

    crate = 0x06ea9711;
    slot[1] = 0x076cad53;
    slot[2] = 0x06e9403f;
    slot[3] = 0x06e9da9d;
    slot[4] = 0x06e9f5cb;
    slot[5] = 0x06e9404a;
    slot[6] = 0x046f3872;
    slot[7] = 0x06e940b2;
    slot[14] = 0x06e9403e;
    slot[15] = 0x076ccf56;
    slot[16] = 0x06e940b1;
    slot[17] = 0x06e9043c;
    slot[18] = 0x06e9da86;
    slot[19] = 0x076ccf5f;
    slot[20] = 0x06e94032;


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

    Int_t MapDetR[kNDet][kNIt] = {
        {1201, 1103, 1002, 2204},
        {2111, 2013, 3212, 3114},
        {3021, 4223, 4122, 4024},
        {5231, 5133, 5032, 6234},
        {6141, 6043, 7242, 7144},
        {14251, 14153, 14052, 15254},
        {15161, 15063, 16262, 16164},
        {16071, 17273, 17172, 17074},
        {18281, 18183, 18082, 19284},
        {19191, 19093, 20292, 20194},
        {7005, 20005, 7005, 20005}
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
                ffStrip << GetDet(MapDetR[det][i]) + 10 << '\t';
                if (GetSide(MapDetR[det][i]) == 1 || GetSide(MapDetR[det][i]) == 2) {
                    if (det >= 5) {
                        Int_t strTemp = 47 - (MapL[j] + (GetSide(MapDetR[det][i]) - 1)*24);
                        if (strTemp < 24) ffStrip << 48-(23 - strTemp) - 1 << '\t';
                        else ffStrip << 48-(71 - strTemp) - 1 << '\t';
                    } else {
                        Int_t strTemp = MapL[j] + (GetSide(MapDetR[det][i]) - 1)*24;
                        if (strTemp < 24) ffStrip << 48-(23 - strTemp) - 1 << '\t';
                        else ffStrip << 48-(71 - strTemp) - 1 << '\t';
                    }
                    //ffStrip << MapL[j] + (GetSide(MapDet[det][i]) - 1)*24 << '\t';
                    ffStrip << 'L';
                    ffStrip << endl;
                } else if (GetSide(MapDetR[det][i]) == 3 || GetSide(MapDetR[det][i]) == 4) {
                    if (det >= 5) {
                        Int_t strTemp = 47 - (MapR[j] + (GetSide(MapDetR[det][i]) - 3)*24);
                        if (strTemp < 24) ffStrip << 48-(23 - strTemp) - 1 << '\t';
                        else ffStrip << 48-(71 - strTemp) - 1 << '\t';
                    } else {
                        Int_t strTemp = MapR[j] + (GetSide(MapDetR[det][i]) - 3)*24;
                        if (strTemp < 24) ffStrip << 48-(23 - strTemp) - 1 << '\t'; 
                        else ffStrip << 48-(71 - strTemp) - 1 << '\t';
                    }
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

}

Int_t GetSlot(Int_t number) {
    Int_t temp = static_cast<int> number / 1000.;
    return temp;
}

Int_t GetCon(Int_t number) {
    Int_t temp = static_cast<int> (number - GetSlot(number)*1000.) / 100.;
    return temp;
}

Int_t GetDet(Int_t number) {
    Int_t temp = static_cast<int> (number - GetSlot(number)*1000. - GetCon(number)*100.) / 10.;
    return temp;
}

Int_t GetSide(Int_t number) {
    Int_t temp = static_cast<int> (number - GetSlot(number)*1000. - GetCon(number)*100. - GetDet(number)*10.);
    return temp;
}
