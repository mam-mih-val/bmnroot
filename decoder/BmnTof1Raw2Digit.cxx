#include "BmnTof1Raw2Digit.h"

//	Written by JINR summer programme 2016 student Kurganov Alexander

//	Supervisors:
//	Vadim Babkin
//	Mikhail Rumyantsev
//	Vyatcheslav Golovatjuk

typedef std::map<std::pair<UInt_t, UChar_t>, UInt_t>::iterator Tof1PlMapIter;
typedef std::map<UInt_t, BmnTof1TDCParameters>::iterator Tof1TDCMapIter;
typedef std::pair<UInt_t, UChar_t> Tof1PlMapKey;
typedef std::pair<UInt_t, BmnTof1TDCParameters> Tof1TDCMapElem;

using namespace std;

int Tof1GlobalNameCounter = 0;

//Simple constructors for the BmnTof1Map2 element class

BmnTof1Map2::BmnTof1Map2(Short_t p, Short_t s, Bool_t sd) {
    plane = p;
    strip = s;
    side = sd;
}

BmnTof1Map2::BmnTof1Map2() {
    plane = 0;
    strip = 0;
    side = false;
}

//Main constructor for the BmnTof1TDCParameters()

BmnTof1TDCParameters::BmnTof1TDCParameters() {
    for (int i = 0; i < TOF1_CHANNEL_NUMBER; i++) {
        for (int j = 0; j < TOF1_BIN_NUMBER; j++) {
            INL[i][j] = 0;
        }
        ChannelMap[i] = BmnTof1Map2(0, 0, false);
        t[i] = -1;
    }
}

//A small functional class, which is used to compare two BmnTDCDigits
//It is used to sort by time the TDCDigits in the BmnTof1Raw2Digit::FillEvent()

bool _Tof1TDCCompare::operator()(const BmnTDCDigit& a, const BmnTDCDigit& b) const {
    return a.GetValue() < b.GetValue();
}

//A small function, which inserts in the placement map a key-value pair
//Just to make everything more clear

void BmnTof1Raw2Digit::plmap_insert(UInt_t Serial, UChar_t Slot, UInt_t TDCSerial) {
    PlacementMap.insert(std::pair<Tof1PlMapKey, UInt_t>(Tof1PlMapKey(Serial, Slot), TDCSerial));
}



//BmnTof1Raw2Digit main constructor

BmnTof1Raw2Digit::BmnTof1Raw2Digit() {
    init();
}

//BmnTof1Raw2Digit constructor, which also loads mapping from the DB

BmnTof1Raw2Digit::BmnTof1Raw2Digit(Int_t nPeriod, Int_t nRun, Int_t verbose = 0) {
    fVerbose = verbose;
    init();
    setRun(nPeriod, nRun);
}

//BmnTof1Raw2Digit destructor

BmnTof1Raw2Digit::~BmnTof1Raw2Digit() {
}

//BmnTof1Raw2Digit init function (called in BmnTof1Raw2Digit constructors)

void BmnTof1Raw2Digit::init() {
    //Nothing here
}

//A simple function to convert Hptdcid (0 -- 9) and Hptdc's channel (0 -- 8) to "global" tdc channel (0 -- 72)

UShort_t BmnTof1Raw2Digit::ToGlobalChannel(UChar_t HptdcId, UChar_t channel) {
    return HptdcId * 8 + channel;
}

//Loads mapping from the DB

Bool_t BmnTof1Raw2Digit::setRun(Int_t nPeriod, Int_t nRun) {

    if (fVerbose >= 1) cout << endl << "----------------------TOF400 RawToDigit initialization--------------------" << endl;
    if (fVerbose >= 1) cout << "Loading the TOF400 Data from DB: Period " << nPeriod << ", Run " << nRun << "..." << endl;
    PeriodIndex = nPeriod;
    RunIndex = nRun;
    //--------------------------------------------------------------------------
    //Load the TDC's map (placement map) from the DB
    if (fVerbose >= 1) cout << "\tLoading the placement map..." << endl;
    UniDetectorParameter* pLoadedPlacement = UniDetectorParameter::GetDetectorParameter("TOF1", "TOF1_TdcMap", nPeriod, nRun);
    if (pLoadedPlacement == 0) {
        printf(ANSI_COLOR_RED "\t\tTOF400 Crucial error: failed to load the placement map\n" ANSI_COLOR_RESET);
        return kFALSE;
    } else {
        //Get the placement map
        vector<UniValue*> tdcmap_array;
        pLoadedPlacement->GetValue(tdcmap_array);
        MapIntValue* temptdcMap = nullptr;
        //loop over TDCs stored in the DB
        for (int i = 0; i < tdcmap_array.size(); i++) {
            temptdcMap = (MapIntValue*) tdcmap_array.at(i);
            plmap_insert(temptdcMap->serial, temptdcMap->channel, temptdcMap->value);
            //create TDC in the map
            TDCMap.insert(Tof1TDCMapElem(temptdcMap->value, BmnTof1TDCParameters()));
        }

        //Print the result
        if (fVerbose >= 1) cout << "\tLoaded " << tdcmap_array.size() << " TDCs" << endl;
        //Free the memory
        if (!tdcmap_array.empty()) for (int i = 0; i < tdcmap_array.size(); i++) delete tdcmap_array[i];
        delete pLoadedPlacement;
        //      delete temptdcMap; //???
    }
    
    //--------------------------------------------------------------------------
    //Load the strip mapping from the DB (TDCs channel -> plane, strip, side).
    if (fVerbose >= 1) cout << "\tLoading strip map..." << endl;
    UniDetectorParameter* pLoadedStr = UniDetectorParameter::GetDetectorParameter("TOF1", "TOF1_StripMap", nPeriod, nRun);
    if (pLoadedStr == 0) {
        printf(ANSI_COLOR_RED "\t\tTOF400 Crucial error: failed to load the strip map\n" ANSI_COLOR_RESET);
        return kFALSE;
    } else {
        //Get the str map
        vector<UniValue*> stripmap_array;
        pLoadedStr->GetValue(stripmap_array);
        MapDVectorValue* tempstrMap = nullptr;
        for (int i = 0; i < stripmap_array.size(); i++) {
            tempstrMap = (MapDVectorValue*) stripmap_array.at(i);
            //Looking for TDC in the map
            Tof1TDCMapIter TDCPair = TDCMap.find(tempstrMap->serial);
            if (TDCPair == TDCMap.end()) {
                printf(ANSI_COLOR_RED "\t\tError: TDC # 0x%08x are not found in the strip map\n" ANSI_COLOR_RESET, tempstrMap->serial);
                //fill map for this TDC by under range value
                BmnTof1Map2* elem = &((TDCPair->second).ChannelMap[tempstrMap->channel]);
                elem->plane = -1;
                elem->strip = -1;
                elem ->side = TOF1_LEFT;
            }
            //get the channel map from
            BmnTof1Map2* elem = &((TDCPair->second).ChannelMap[tempstrMap->channel]);
            elem->plane = tempstrMap->value.at(0);
            elem->strip = tempstrMap->value.at(1);
            if (tempstrMap->value.at(2) == 0)
                elem -> side = TOF1_LEFT;
            else if (tempstrMap->value.at(2) == 1)
                elem -> side = TOF1_RIGHT;
            //        cout << std::hex << tempstrMap->serial << std::dec << " ch " << tempstrMap->channel << " -> " << tempstrMap->value.at(0) << "  " << tempstrMap->value.at(1) << "  " << tempstrMap->value.at(2) << endl;
        }

        //Print the result
        if (fVerbose >= 1) cout << "\tLoaded " << stripmap_array.size() << " strips" << endl;
        //Free the memory
        if (!stripmap_array.empty()) for (int i = 0; i < stripmap_array.size(); i++) delete stripmap_array[i];
        delete pLoadedStr;
        //        delete tempstrMap; // ???
    }
    
    //--------------------------------------------------------------------------
    //Load Inl from DB
    if (fVerbose >= 1) cout << "\tLoading the INL correction from DB... " << endl;
    UniDetectorParameter* pLoadedInl = UniDetectorParameter::GetDetectorParameter("TOF1", "TOF1_inl", nPeriod, nRun); //(detector_name, parameter_name, period_number, run_number)
    if (pLoadedInl == NULL) {
        printf(ANSI_COLOR_RED "\t\tINL are not exist in DB! Time of Flight fill be wrong!\n" ANSI_COLOR_RESET);
        return kTRUE; //
    }

    //get INL from DB to vector
    vector<UniValue*> inl_array;
    pLoadedInl->GetValue(inl_array);
    TdcInlValue* tempInl = nullptr;
    UInt_t serialtemp;
    Bool_t flagtemp = false;
    Tof1TDCMapIter tdcs = TDCMap.begin();
    // loop over TDCs in the map
    while (tdcs != TDCMap.end()) {
        serialtemp = tdcs->first;
        flagtemp = false;

        //looking for INL for current TDC in the INL_vector from DB
        for (Int_t iv = 0; iv < inl_array.size(); iv++) {
            tempInl = (TdcInlValue*) inl_array.at(iv);
            if (tempInl->serial == serialtemp) {
                if (fVerbose >= 2) cout << "\t\tINL for " << std::hex << serialtemp << std::dec << " are found" << endl;
                flagtemp = true;
                for (Int_t ich = 0; ich < TOF1_CHANNEL_NUMBER; ich++)
                    for (Int_t ibin = 0; ibin < TOF1_BIN_NUMBER; ibin++)
                        (tdcs->second).INL[ich][ibin] = tempInl->inl[ich][ibin];
            }
        }
        if (flagtemp == false) {
            printf(ANSI_COLOR_RED "\t\tError: INL for TDC # 0x%08x are not stored in DB\n" ANSI_COLOR_RESET, serialtemp);
            // fill INL for this TDC by zero
            for (Int_t ich = 0; ich < TOF1_CHANNEL_NUMBER; ich++)
                for (Int_t ibin = 0; ibin < TOF1_BIN_NUMBER; ibin++)
                    (tdcs->second).INL[ich][ibin] = 0.;
        }
        // increase iterator
        tdcs++;
    }

    //Free the memory
    if (!inl_array.empty()) for (int i = 0; i < inl_array.size(); i++) delete inl_array[i];
    delete pLoadedInl;

    if (fVerbose >= 1) cout << "\tLoading INL for TOF400 complete." << endl;

    //--------------------------------------------------------------------------
    if (fVerbose >= 1) cout << "----------------------TOF400 RawToDigit initialization complete--------------------" << endl << endl;
    return kTRUE;
}

//Load mapping from the file

void BmnTof1Raw2Digit::setMapFromFile(TString placementMapFile, TString mapFile) {
    printf(ANSI_COLOR_BLUE "Load TOF400 mapping from files \n" ANSI_COLOR_RESET);
    UInt_t fcrate, fserial, fslot, fchan, fplane, fstrip;
    char side_c;

    //1. Placement map
    fstream ff;
    ff.open(placementMapFile.Data(), std::fstream::in);
    if (ff.fail()) {
        printf(ANSI_COLOR_RED "\t\tError: Cannot open the file %s \n" ANSI_COLOR_RESET, placementMapFile.Data());
        return;
    } else if (fVerbose >= 1) cout << "\tReading TOF400 PlacementMap file " << placementMapFile << endl;
    while (!ff.eof()) {
        ff >> std::hex >> fcrate >> std::dec >> fslot >> std::hex >> fserial >> std::dec;
        if (ff.eof()) {
            break;
        }
        plmap_insert(fcrate, fslot, fserial);
        TDCMap.insert(Tof1TDCMapElem(fserial, BmnTof1TDCParameters()));
    }
    ff.close();

    //2. Main map
    ff.open(mapFile.Data(), std::fstream::in);
    if (ff.fail()) {
        printf(ANSI_COLOR_RED "\t\tError: Cannot open the file %s \n" ANSI_COLOR_RESET, mapFile.Data());
        return;
    } else if (fVerbose >= 1) cout << "\tReading TOF400 StripMap file " << mapFile << endl;
    while (!ff.eof()) {
        ff >> std::hex >> fserial >> std::dec >> fchan >> fplane >> fstrip >> side_c;
        if (ff.eof()) {
            break;
        }
        Tof1TDCMapIter TDCPair = TDCMap.find(fserial);
        if (TDCPair == TDCMap.end()) {
            printf(ANSI_COLOR_RED "\t\tError:  TDC # 0x%08x  not found\n" ANSI_COLOR_RESET, fserial);
        }
        BmnTof1Map2* elem = &((TDCPair->second).ChannelMap[fchan]);
        elem->plane = fplane;
        elem->strip = fstrip;
        if (side_c == 'L') {
            elem -> side = TOF1_LEFT;
        } else if (side_c == 'R') {
            elem->side = TOF1_RIGHT;
        } else {
            //cerr << "ToF1 Error: side is not L or R" << endl;
            continue;
        }
    }
    ff.close();
}

//Save mapping to file

void BmnTof1Raw2Digit::saveMapToFile(string placementMapFile, string mapFile) {
    fstream ff;

    //1.Placement map
    ff.open(placementMapFile.c_str(), std::fstream::out);
    if (ff.fail()) {
        cerr << "Cannot open the file " << placementMapFile << endl;
        return;
    }

    Tof1PlMapIter plit = PlacementMap.begin();
    while (plit != PlacementMap.end()) {
        ff << std::hex << std::setfill('0') << std::setw(8) << plit->first.first << '\t';
        ff << std::dec << int(plit->first.second) << '\t';
        ff << std::hex << std::setfill('0') << std::setw(8) << plit->second << endl;
        ++plit;
    }
    ff.close();

    //2. Main map
    ff.open(mapFile.c_str(), std::fstream::out);
    if (ff.fail()) {
        cerr << "Cannot open the file " << mapFile << endl;
        return;
    }

    Tof1TDCMapIter mapit = TDCMap.begin();
    while (mapit != TDCMap.end()) {
        for (int i = 0; i < TOF1_CHANNEL_NUMBER; i++) {
            ff << std::hex << std::setfill('0') << std::setw(8) << mapit->first << std::setfill(' ') << '\t';
            ff << std::dec << i << '\t';
            ff << mapit->second.ChannelMap[i].plane << '\t';
            ff << mapit->second.ChannelMap[i].strip << '\t';
            bool side = mapit->second.ChannelMap[i].side;
            if (side == TOF1_LEFT) {
                ff << 'L';
            } else if (side == TOF1_RIGHT) {
                ff << 'R';
            } else {
                cerr << "Side is not bool." << endl;
            } //I don't think this can even occur
            ff << endl;
        }
        ++mapit;
    }
}

//Save INL to file

void BmnTof1Raw2Digit::setINLFromFile(string INLFile) {
    fstream ff(INLFile.c_str(), std::fstream::in);
    if (ff.fail()) {
        cerr << "Failed to open " << INLFile << endl;
        return;
    }
    unsigned int TDCSerial;

    //Read the header from the file
    //The format of the header seems to be [TDC-THESERIAL-inl_corr]
    ff.ignore(10, '-');
    ff >> std::hex >> TDCSerial >> std::dec;
    ff.ignore(1000, '\n');

    //Find the TDC
    Tof1TDCMapIter TDCPair = TDCMap.find(TDCSerial);
    if (TDCPair == TDCMap.end()) {
        cerr << "Tof400: TDC " << std::setfill('0') << std::setw(8) << std::hex << TDCSerial << std::setfill(' ') << std::dec;
        cerr << " isn't in the placement map." << endl;
        ff.close();
        return;
    }

    unsigned int chan_id = 0;
    unsigned int lines_num = 0;

    while (!ff.eof()) {
        string line;
        char dummy;

        std::getline(ff, line, '\n');
        if (ff.eof()) {
            break;
        }
        if (line == "") {
            continue;
        }
        istringstream ss(line);

        ss >> chan_id >> dummy;
        if (dummy != '=') {
            cerr << "Tof400: Wrong INL file format." << endl;
            ff.close();
            return;
        }
        if (chan_id > TOF1_CHANNEL_NUMBER) {
            cerr << "Tof400: Wrong channel in in the INL file" << endl;
            ff.close();
            return;
        }

        double* INLelem = (TDCPair->second).INL[chan_id];

        unsigned int i_bin = 0;
        while (ss.tellg() != -1) {
            if (i_bin > TOF1_BIN_NUMBER) {
                cerr << "Tof400: INL File contains too many bins in channel." << endl;
                ff.close();
                return;
            }
            if (ss.peek() == ',') {
                ss.ignore();
            }
            ss >> INLelem[i_bin];
            i_bin++;
        }
        if (i_bin != TOF1_BIN_NUMBER) {
            cout << "Warning: wrong number of bins in the INL file for channel " << chan_id << " (" << i_bin << ")" << endl;
        }
        lines_num++;
    }

    if (lines_num != TOF1_CHANNEL_NUMBER) {
        cout << "Warning: wrong number of lines in the INL file (" << lines_num << endl;
    }
    cout << "Tof400: INL for TDC " << std::setfill('0') << std::setw(8) << std::hex << TDCSerial << std::setfill(' ') << std::dec << " loaded succesfully from INL file." << endl;
}

//Load INL from file

void BmnTof1Raw2Digit::saveINLToFile(string INLFile, unsigned int TDCSerial) {
    //Find the TDC
    Tof1TDCMapIter TDCPair = TDCMap.find(TDCSerial);
    if (TDCPair == TDCMap.end()) {
        cerr << "Tof400: TDC " << std::setfill('0') << std::setw(8) << std::hex << TDCSerial << std::setfill(' ') << std::dec;
        cerr << " isn't in the placement map." << endl;
        return;
    }

    fstream ff(INLFile.c_str(), std::fstream::out);
    ff << "[TDC-" << std::setfill('0') << std::setw(8) << std::hex << TDCSerial << std::dec << std::setfill(' ') << "-inl_corr]" << endl;

    for (int chan = 0; chan < TOF1_CHANNEL_NUMBER; chan++) {
        ff << chan << "=";
        double* INLelem = (TDCPair->second).INL[chan];
        for (int bin = 0; bin < TOF1_BIN_NUMBER; bin++) {
            ff << INLelem[bin];
            if (bin != TOF1_BIN_NUMBER - 1) {
                ff << ", ";
            }
        }
        if (chan != TOF1_CHANNEL_NUMBER - 1) {
            ff << endl;
        }
    }

    ff.close();
}

//Main function. "Converts" the TObjArray *data of BmnTDCDigit to the TObjArray *output of BmnTof1Digit

void BmnTof1Raw2Digit::FillEvent(TClonesArray *data, map<UInt_t, Long64_t> *mapTS, TClonesArray * output) {
    //0. Initialize: clear all the tempory times in the BmnTof1TDCParameters
    Tof1TDCMapIter tdci = TDCMap.begin();
    while (tdci != TDCMap.end()) {
        for (int i = 0; i < TOF1_CHANNEL_NUMBER; i++) {
            (tdci->second).t[i] = -1;
        }
        tdci++;
    }

    //1. Filter and sort the TDC Data
    //TODO: Well.. maybe it's better to do it NOT here?
    //Maybe both of these should be done in the BmnRawDataDecoder?

    int nOut = output->GetEntries();
    //nOut variable is needed. GetEntries is quite a slow function,
    //so storing the number of entries before filling the *output TObjArray works much faster

    multiset<BmnTDCDigit, _Tof1TDCCompare> temp; //Create a multiset
    //Multiset automaticly sorts the elements in it.
    //_Tof1TDCCompare functional class is used to compare two BmnTDCDigit (see above)

    //Fill the multiset from the input array (it sorts the input array)
    TIter it = data->begin();
    while (it != data->end()) {
        BmnTDCDigit* obj = static_cast<BmnTDCDigit*> (*it);
        if (obj->GetType() != TOF1_TDC_TYPE) {
            ++it;
            continue;
        } //Just skip it in case if TDC is not TDC72VHL (see TOF1_TDC_TYPE define in the .h file)
        temp.insert(*obj);
        //cout << std::hex << obj->GetSerial() << " "  << std::dec << int(obj->GetSlot()) << " " << int(obj->GetHptdcId()) << " " << int(obj->GetChannel()) << " " << (obj->GetLeading()?"L":"T") << obj->GetValue() << endl;
        ++it;
    }

    //Now a loop over the temp multiset using iterator si
    multiset<BmnTDCDigit, _Tof1TDCCompare>::iterator si = temp.begin();

    while (si != temp.end()) {
        //Look for the crate serial and slot in the placement map
        Tof1PlMapIter parPair = PlacementMap.find(std::make_pair<UInt_t, UChar_t>(si->GetSerial(), si->GetSlot()));

        //In case if nothing was found
        if (parPair == PlacementMap.end()) {
            //Just skip it
            si++;
            continue;
        }
        map<UInt_t, Long64_t>::iterator itTS = mapTS->find(si->GetSerial());
        if (itTS == mapTS->end()) {
            si++;
            continue;
        }
        Long64_t TimeShift = itTS->second;

        //Get the TDC Serial for this crate serial and slot
        UInt_t TDC_Serial = parPair -> second;
        //And look for this TDC Serial in the TDCMap to get the BmnTof1TDCParameters for this TDC
        Tof1TDCMapIter TDCPair = TDCMap.find(TDC_Serial);
        //In case if TDC Serial isn't in the TDCMap
        if (TDCPair == TDCMap.end()) {
            //Display a warning and skip. This is quite a serious error, actually
            cerr << "Warning: TDC with ID " << std::hex << TDC_Serial << std::dec << ": no data" << endl;
            si++;
            continue;
        }

        //Finally, get the BmnTof1TDCParameters instance for this TDC Serial
        BmnTof1TDCParameters* par = &(TDCPair->second);
        //And get it's channel using BmnTof1Raw2Digit::ToGlobalChannel (see above)
        UShort_t rchan = ToGlobalChannel(si->GetHptdcId(), si->GetChannel());

        //Get the time from the TDC value
        //t = (val + INL[channel][val % 1024]) * (24ns / 1024)
        //See TOF1_BIN_NUMBER and TOF1_MAX_TIME defines in the .h file
        /*
        if(par->INL[rchan][(si->GetValue()) % TOF1_BIN_NUMBER] == 0) {
                cout << std::hex << TDC_Serial << std::dec << rchan << ":" << ((si->GetValue()) % TOF1_BIN_NUMBER) << " - " << par->INL[rchan][(si->GetValue()) % TOF1_BIN_NUMBER] << endl;
        }
         */
        double timeFromDigit = (si->GetValue() + par->INL[rchan][(si->GetValue()) % TOF1_BIN_NUMBER]) * TOF1_MAX_TIME / double(TOF1_BIN_NUMBER);

        if (si->GetLeading()) {
            //If this is a leading TDC digit, just fill the temporary time in the BmnTof1TDCParameters.
            par->t[rchan] = timeFromDigit;
        } else {
            //If this is NOT a leading TDC Digit...
            if (par->t[rchan] != -1) {
                //BmbTof1TDCParameters' temporary time (par->t[rchan]) has been already set
                //This temporary time should store the leading time
                if (timeFromDigit < (par->t[rchan])) {
                    //This is impossible:
                    //Leading value is larger than the trailing time
                    //The BmnTDCDigit array has already been sorted, so this shouldn't happen
                    //But let's just make sure, this could be a serious error
                    cout << "Error: negative dt" << endl;

                    //Print all the TDC Digits we have loaded in the sorted order
                    multiset<BmnTDCDigit, _Tof1TDCCompare>::iterator si2 = temp.begin();
                    while (si2 != temp.end()) {
                        cout << std::hex << si2->GetSerial() << " " << int(si2->GetSlot()) << " " << std::dec << int(si2->GetHptdcId()) << " " << int(si2->GetChannel()) << " " << (si2->GetLeading() ? "L" : "T") << si2->GetValue() << endl;
                        si2++;
                    }
                    //Print where exactly the error has occured
                    cout << "Error at: " << endl;
                    cout << std::hex << si->GetSerial() << " " << int(si->GetSlot()) << " " << std::dec << int(si->GetHptdcId()) << " " << int(si->GetChannel()) << " " << (si->GetLeading() ? "L" : "T") << si->GetValue() << endl;
                    cout << std::hex << TDC_Serial << std::dec << " " << rchan << " " << (par->t[rchan]) << "--" << timeFromDigit << endl;
                } else {
                    //So we've got the Leading and Trail times and everything seems to be okay
                    //Find the BmnTof1Map2 mapping element, which stores the plane, strip and side.
                    BmnTof1Map2* elem = &(par->ChannelMap[rchan]);

                    //Create a new BmnTof1Digit:
                    //Plane, strip, and side are loaded from the found BmnTof1Map2
                    //Leading time is loaded from the temporary storage in the BmnTof1TDCParameters class
                    //Trailing time is just the time in current TDC Digit
                    //(See above: this piece of code is executed only if the current BmnTDCDigit is NOT leading)

                    new((*output)[nOut]) BmnTof1Digit(elem->plane, elem->strip, elem->side, par->t[rchan] + TimeShift, timeFromDigit - (par->t[rchan]));
                    nOut++;
                }
            }
            //We don't need to do anything in case if no leading time has occured yet (if par->t[rchan] == -1)

            //Finally, reset the temporary storage to prevent errors
            par->t[rchan] = -1;
        }
        si++;
    }
}

ClassImp(BmnTof1Raw2Digit)
