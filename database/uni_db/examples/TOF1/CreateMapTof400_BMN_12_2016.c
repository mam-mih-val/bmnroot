
#include <TMath.h>
#include "TString.h"
#include <iostream>
#include "Riostream.h"
#include <string>
#include <fstream>
#include <sstream>

void CreateMapTof400_BMN_12_2016(std::string placementMapFile, std::string mapFile)
{

    Int_t MapR[24] = {3, 0, 9, 6, 4, 1, 10, 7, 5, 2, 11, 8, 18, 21, 12, 15, 19, 22, 13, 16, 20, 23, 14, 17};
    Int_t MapL[24] = {20, 23, 14, 17, 19, 22, 13, 16, 18, 21, 12, 15, 5, 2, 11, 8, 4, 1, 10, 7, 3, 0, 9, 6};
    //Int_t MapL[24] = {4, 1, 10, 7, 5, 2, 11, 8, 6, 3, 12, 9, 19, 22, 13, 16, 20, 23, 14, 17, 21, 0, 15, 18};
    //Int_t MapR[24] = {21, 0, 15, 18, 20, 23, 14, 17, 19, 22, 13, 16, 6, 3, 12, 9, 5, 2, 11, 8, 4, 1, 10, 7};

    //UInt_t crate = 0x01690454;
    UInt_t crate = 0x06ea9711;
    static const Int_t kNSlot = 23;
    UInt_t slot[kNSlot] = {0.};
    slot[1] = 0x06e9da86;
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

    fstream ff;

    //1.Placement map
    ff.open(placementMapFile.c_str(), std::fstream::out);
    if (ff.fail())
    {
        cerr << "Cannot open the file " << placementMapFile << endl;
        return;
    }
    for (Int_t i = 0; i < kNSlot; i++)
        if (slot[i] != 0)
        {
            ff << std::hex << std::setfill('0') << std::setw(8) << crate << '\t';
            ff << std::dec << i << '\t';
            ff << std::hex << std::setfill('0') << std::setw(8) << slot[i] << endl;
        }
    ff.close();


    Int_t Num = 14516;
    cout << "Num = " << Num << endl;
    cout << "GetSlot(Num) = " << GetSlot(Num) << " GetCon(Num) = " << GetCon(Num) << endl;
    cout << "GetDet(Num) = " << GetDet(Num) << " GetSide(Num) = " << GetSide(Num) << endl;


    //create Map for Cosmic Test Setup
    ff.open(mapFile.c_str(), std::fstream::out);
    if (ff.fail())
    {
        cerr << "Cannot open the file " << mapFile << endl;
        return;
    }

    static const Int_t kNIt = 4;
    static const Int_t kNDet = 10+1;
    Int_t MapDet[kNDet][kNIt] = {
        {1002, 1101, 1204, 2003},
        {2112, 2211, 3014, 3113},
        {3222, 4021, 4124, 4223},
        {5032, 5131, 5234, 6033},
        {6142, 6241, 7044, 7143},
        {14052, 14151, 14254, 15053},
        {15162, 15261, 16064, 16163},
        {16272, 17071, 17174, 17273},
        {18082, 18181, 18284, 19083},
        {19192, 19291, 20094, 20193},
        {7205, 20205, 7205, 20205}
    };

    //first number - slot 1-22, second number - connector (0 - 0..23, 1 - 24..47, 2 - 48..71
    //third - detector , fourth - side (1,2 - L, 3,4 - R)


    for (Int_t det = 0; det < kNDet; det++)
    {
        for (Int_t i = 0; i < kNIt; i++)
        {
            for (Int_t j = 0; j < 24; j++)
            {
                /*
                                cout << std::dec << "MapDet = " << MapDet[det][i] << "; Det = " << det << "; i = " << i << "; j = " << j << "; slot = " << slot[GetSlot(MapDet[det][i])]
                                        << "; chTDC = " << j + 24 * GetCon(MapDet[det][i]) << "; GetDet = " << GetDet(MapDet[det][i])
                                        << "; GetSide = " << GetSide(MapDet[det][i]) << endl;
                                getchar();
                 */

                ff << std::hex << std::setfill('0') << std::setw(8) << slot[GetSlot(MapDet[det][i])] << std::setfill(' ') << '\t';
                ff << std::dec << j + 24 * GetCon(MapDet[det][i]) << '\t';
                ff << GetDet(MapDet[det][i]) << '\t';
                if (GetSide(MapDet[det][i]) == 1 || GetSide(MapDet[det][i]) == 2)
                {
                    if (det < 5) ff << 47 - (MapL[j] + (GetSide(MapDet[det][i]) - 1)*24) << '\t';
                    else ff << MapL[j] + (GetSide(MapDet[det][i]) - 1)*24 << '\t';
                    //ff << MapL[j] + (GetSide(MapDet[det][i]) - 1)*24 << '\t';
                    ff << 'L';
                    ff << endl;
                }
                else if (GetSide(MapDet[det][i]) == 3 || GetSide(MapDet[det][i]) == 4)
                {
                    if (det < 5) ff << 47 - (MapR[j] + (GetSide(MapDet[det][i]) - 3)*24) << '\t';
                    else ff << MapR[j] + (GetSide(MapDet[det][i]) - 3)*24 << '\t';
                    //ff << MapR[j] + (GetSide(MapDet[det][i]) - 3)*24 << '\t';
                    ff << 'R';
                    ff << endl;
                }
                else
                {
                    ff << -1 << '\t';
                    ff << 'L';
                    ff << endl;
                }
            }
        }
    }

}

Int_t GetSlot(Int_t number)
{
    Int_t temp = static_cast<int> number / 1000.;
    return temp;
}

Int_t GetCon(Int_t number)
{
    Int_t temp = static_cast<int>(number - GetSlot(number)*1000.) / 100.;
    return temp;
}

Int_t GetDet(Int_t number)
{
    Int_t temp = static_cast<int>(number - GetSlot(number)*1000. - GetCon(number)*100.) / 10.;
    return temp;
}

Int_t GetSide(Int_t number)
{
    Int_t temp = static_cast<int>(number - GetSlot(number)*1000. - GetCon(number)*100. - GetDet(number)*10.);
    return temp;
}
