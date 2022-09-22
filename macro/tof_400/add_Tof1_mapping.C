//Uploads Tof1 mapping from two text files
#include <fstream>
#include <iostream>
#include <string>

using namespace std;
//sP is start period
//sR is start run
//eP is end period
//eR is end run


//FILES FORMAT:

//Placement map:
//Each line:
//[HEX: Crate serial] [Slot id] [HEX: TDC Serial]

//Main map:
//Each line:
//[HEX: TDC Serial] [Channel] [Plane id] [Strip id] [Side: L or R]

void add_Tof1_mapping(string placement_map_filename = "/home/mikhailr/bmnroot/input/TOF400_PlaceMap_Period5_v3.txt", string main_map_filename = "/home/mikhailr/bmnroot/input/TOF400_StripMap_Period5_v3.txt", int sP = 5, int sR = 419, int eP = 5, int eR = 10000)
{
    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();
    gSystem->Load("libUniDb");

    char anwser;
    int nLines;
    fstream ff;

    unsigned int fserial;
    int fchan;
    int fplane;
    int fstrip;
    char side_c;
    unsigned int fcrate;
    int fslot;
    bool side;

    //Placement map
    //Stored in 1D UInt_t array format
    //(HEX: Create serial), (Slot), (HEX: TDC Serial), (HEX: Crate serial)...

    unsigned int map1[100];
    int map1Size = 0;

    nLines = 0;
    ff.open(placement_map_filename.c_str(), std::fstream::in);
    if (ff.fail())
    {
        cout << "Failed to open " << placement_map_filename << endl;
        return;
    }
    while (!ff.eof())
    {
        ff >> std::hex >> fcrate >> std::dec >> fslot >> std::hex >> fserial;
        if (ff.eof() || fcrate == 0)
            break;

        cout << std::hex << fcrate << " : " << std::dec << fslot << std::hex << " -> " << fserial << std::dec << endl;
        map1[nLines * 3 + 0] = fcrate;
        map1[nLines * 3 + 1] = fslot;
        map1[nLines * 3 + 2] = fserial;
        nLines++;
    }
    ff.close();

    int nTDC = nLines;
    map1Size = nTDC * 3;
    cout << nTDC << " TDCs in the mapping " << endl;

    //Load the main map

    nLines = 0;
    ff.open(main_map_filename.c_str(), std::fstream::in);
    if (ff.fail())
    {
        cout << "Failed to open " << main_map_filename << endl;
        return;
    }
    while (!ff.eof())
    {
        ff >> std::hex >> fserial >> std::dec >> fchan >> fplane >> fstrip >> side_c;
        if (ff.eof() || fserial == 0)
            break;

        if (side_c != 'L' && side_c != 'R')
            cout << "Wrong mapping file: Side is not L or R." << endl;

        //cout << nLines << std::hex << fserial << std::dec << " " << fchan << " " << fplane << " " << fstrip << " " << side_c << endl;
        nLines++;
    }

    ff.close();

    //Check if number of lines is correct (should be 72 * nTDC)
    cout << nLines << " lines in the main mapping file" << endl;
    cout << (double(nLines) / 72.0) << " TDCs in the main mapping file" << endl;
    /*if (double(nLines) / 72.0 != nTDC)
    {
        cout << "Error!" << endl;
        return;
    }*/

    cout << "Resulting array of unsigned int: " << endl;
    for (int i = 0; i < map1Size; i++)
    {
        cout << map1[i] << " (" << std::hex << map1[i] << std::dec << endl;
    }

    //Upload the placement map
    cout << "\nOkay to upload it? (y/N)" << endl;
    cin >> anwser;
    if (anwser == 'y' || anwser == 'Y')
    {
        unsigned char* pt = reinterpret_cast<unsigned char*> (map1);
        UniDetectorParameter* t = UniDetectorParameter::CreateDetectorParameter("TOF1", "placementmap", sP, sR, eP, eR, pt, map1Size * sizeof (unsigned int), UIntArrayType);
        if (t == NULL)
        {
            cout << "Cannot upload the placement map" << endl;
            return;
        }
        delete t;
    }

    //Upload the main mapping
    int nLines = 0;
    cout << "Okay to upload the main map? (y/N)" << endl;
    cin >> anwser;
    if (anwser == 'y' || anwser == 'Y')
    {
        ff.open(main_map_filename.c_str(), std::fstream::in);
        while (!ff.eof())
        {
            cout << "Uploading line " << nLines << endl;
            ff >> std::hex >> fserial >> std::dec >> fchan >> fplane >> fstrip >> side_c;
            if (side_c == 'L')
            {
                side = true;
            }
            else if (side_c == 'R')
            {
                side = false;
            }
            else
            {
                cout << "Error: side is not L or R" << endl;
                return;
            }
            cout << std::hex << fserial << " " << std::dec << fchan << " " << fplane << " " << fstrip << " " << side_c;


            if (ff.eof() || fserial == 0)
            {
                break;
            }

            UniDetectorParameter* t;
            t = UniDetectorParameter::CreateDetectorParameter("TOF1", "plane", sP, sR, eP, eR, fserial, fchan, fplane);
            if (t == 0x00)
            {
                cout << "Cannot upload the placement map" << endl;
                break;
            }
            else
            {
                delete t;
            }
            t = UniDetectorParameter::CreateDetectorParameter("TOF1", "strip", sP, sR, eP, eR, fserial, fchan, fstrip);
            if (t == 0x00)
            {
                cout << "Cannot upload the placement map" << endl;
                break;
            }
            else
            {
                delete t;
            }
            t = UniDetectorParameter::CreateDetectorParameter("TOF1", "side", sP, sR, eP, eR, fserial, fchan, side);
            if (t == 0x00)
            {
                cout << "Cannot upload the placement map" << endl;
                break;
            }
            else
            {
                delete t;
            }

            nLines++;
        }
        ff.close();
    }
}
