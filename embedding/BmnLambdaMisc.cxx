#include <ratio>

#include "BmnLambdaMisc.h"

BmnLambdaMisc::BmnLambdaMisc() {
    GEM();
    SILICON();
}

// Process GEM mapping ...

void BmnLambdaMisc::GEM() {
    TString gPathConfig = gSystem->Getenv("VMCWORKDIR");
    TString gPathFull = gPathConfig + "/input/GEM_map_run7.txt";

    // Duplicate keys are possible due to such fucked format of mapping!
    multimap <vector <Int_t>, vector < Long_t>> corrMap; // <st, mod, id> ---> <low, high, serial>

    // Read GEM mapping (run7)
    ifstream g(gPathFull.Data());

    TString gemId = "", station = "", module = "", low = "", high = "", serial = "";
    Int_t linesOmitted = 4;

    Int_t counter = 0;
    for (string line; getline(g, line);) {
        counter++;
        if (counter > linesOmitted) {
            g >> serial >> low >> high >> gemId >> station >> module;
            vector <Int_t> key{station.Atoi(), module.Atoi(), gemId.Atoi()};
            vector <Long_t> value{low.Atoll(), high.Atoll(), (Long_t) serial.Atof()};
            corrMap.insert(make_pair(key, value));

            // Fill useful vector to be used outside of the function ...
            vector <Int_t> tmp{station.Atoi(), module.Atoi(), gemId.Atoi()};
            fGemStatModId.push_back(tmp);
        }
    }

    // Loop over corrMap to find duplicate keys ...
    const Int_t nStats = 6;
    const Int_t nMods = 4;
    const pair <Int_t, Int_t> gemIds = make_pair(1, 150); // FIXME !!!

    for (Int_t iStat = 0; iStat < nStats; iStat++)
        for (Int_t iMod = 0; iMod < nMods; iMod++)
            for (Int_t id = gemIds.first; id < gemIds.second; id++) {
                vector <Int_t> key{iStat, iMod, id};
                if (corrMap.count(key) == 0)
                    continue;

                Int_t nEntries = corrMap.count(key);

                if (nEntries > 1) {
                    // There are duplicated values!
                    vector <pair <Int_t, Long_t>> lowHighSerial;
                    for (auto it = corrMap.equal_range(key).first; it != corrMap.equal_range(key).second; ++it) {
                        vector <Long_t> value = (*it).second;
                        // Put channel ranges into the vector
                        lowHighSerial.push_back(make_pair(value[0], value[2]));
                        lowHighSerial.push_back(make_pair(value[1], value[2]));
                    }

                    // Sort vector by greater ordering ...
                    sort(lowHighSerial.begin(), lowHighSerial.end());
                    // Three ranges are available in the general case ...
                    Int_t chLow1 = lowHighSerial[0].first;
                    Int_t chHigh1 = lowHighSerial[1].first - 1;
                    Long_t serial1 = lowHighSerial[0].second;

                    Int_t chLow2 = lowHighSerial[1].first;
                    Int_t chHigh2 = lowHighSerial[2].first;
                    Long_t serial2 = lowHighSerial[1].second;

                    Int_t chLow3 = lowHighSerial[2].first + 1;
                    Int_t chHigh3 = lowHighSerial[3].first;
                    Long_t serial3 = lowHighSerial[3].second;

                    vector <Long_t> value1{chLow1, chHigh1, serial1};
                    vector <Long_t> value2{chLow2, chHigh2, serial2};
                    vector <Long_t> value3{chLow3, chHigh3, serial3};

                    fGem[value1] = key;
                    fGem[value2] = key;
                    fGem[value3] = key;

                    //                    cout << std::dec << key[0] << " " << key[1] << " " << key[2] << " ---> " <<
                    //                            chLow1 << " " << chHigh1 << " " << std::hex << serial1 << endl;
                    //                    cout << std::dec << key[0] << " " << key[1] << " " << key[2] << " ---> " <<
                    //                            chLow2 << " " << chHigh2 << " " << std::hex << serial2 << endl;
                    //                    cout << std::dec << key[0] << " " << key[1] << " " << key[2] << " ---> " <<
                    //                            chLow3 << " " << chHigh3 << " " << std::hex << serial3 << endl;
                    //
                    //                    cout << std::dec << fGem.size() << endl; //getchar();

                } else {
                    // No duplicated key values found!
                    vector <Long_t> value = corrMap.find(key)->second;
                    fGem[value] = key;

                    //                    cout << std::dec << key[0] << " " << key[1] << " " << key[2] << " ---> " << value[0] << " " << value[1] << " " << std::hex << value[2] << endl;
                    //                    cout << std::dec << fGem.size() << endl; //getchar();
                }
            }
}

void BmnLambdaMisc::SILICON() {

}

Long_t BmnLambdaMisc::GetGemSerial(Int_t stat, Int_t mod, Int_t id, Int_t channel) {
    for (auto it : fGem) {
        if (it.second[0] != stat || it.second[1] != mod || it.second[2] != id)
            continue;

        if (channel >= it.first[0] && channel <= it.first[1])
            return it.first[2];
    }
    return 0x0;
}

// Make correspondence <GEM-digi ---> channel>

Int_t BmnLambdaMisc::GemDigiToChannel(BmnGemStripDigit* dig) {
    Int_t stat = dig->GetStation();
    Int_t mod = dig->GetModule();
    Int_t lay = dig->GetStripLayer();
    Int_t strip = dig->GetStripNumber();

    TString layer = (lay == 0 || lay == 2) ? "X" : "Y";
    Int_t hotOrBig = -1;
    TString LeftOrRight = "";
    TString filename = "";

    for (auto it : fGemStatModId) {
        vector <Int_t> vect = it; // (stat, module, id)

        // Choosing station ...
        if (vect[0] != stat)
            continue;

        // Choosing left or right part to be got from GemId ...
        LeftOrRight = (vect[2] % 10 == 0) ? "Left" : "Right";

        // Defining hot or big zone by layer ...
        hotOrBig = (lay == 2 || lay == 3) ? 0 : 1;

        // Choosing module ...
        if (mod == 0) {
            if (vect[1] == 2 || vect[1] == 3)
                continue;

            // Skipping rest possible modules ...
            if (hotOrBig == 0 && vect[1] == 1)
                continue;
            else if (hotOrBig == 1 && vect[1] == 0)
                continue;
        } else {
            if (vect[1] == 0 || vect[1] == 1)
                continue;

            // Skipping rest possible modules ...
            if (hotOrBig == 0 && vect[1] == 3)
                continue;
            else if (hotOrBig == 1 && vect[1] == 2)
                continue;
        }
        
        filename = TString::Format("GEM_%s%d_Big_%s.txt", layer.Data(), hotOrBig, LeftOrRight.Data());
  
        break;
    }

    TString gPathConfig = gSystem->Getenv("VMCWORKDIR");
    TString gPathFull = gPathConfig + "/input/" + filename;

    ifstream f(gPathFull.Data());

    map <Int_t, Int_t> stripChannels; // Map to store read channel for each strip
    TString channel = "";

    Int_t stripCounter = 0;
    while (!f.eof()) {
        f >> channel;
        stripChannels[stripCounter] = channel.Atoi();
        stripCounter++;
    }
    return stripChannels.find(strip)->second;
}

Long_t BmnLambdaMisc::GemDigiChannelToSerial(pair <BmnGemStripDigit, Int_t> digiChannel) {
    // Make correspondence: st, mod, strip, channel --> serial
    Int_t stat = digiChannel.first.GetStation();
    Int_t mod = digiChannel.first.GetModule();   
    // Int_t strip = digiChannel.first->GetStripNumber();
    Int_t channel = digiChannel.second;

    Int_t layer = digiChannel.first.GetStripLayer(); // To define hot or big zone we are considering ...

    // Modules should be redefined in the mapping notation ...
    Bool_t isBig = (layer == 0 || layer == 1) ? kTRUE : kFALSE;
    Bool_t isHot = (layer == 2 || layer == 3) ? kTRUE : kFALSE;    

    if (isBig && mod == 1)
        mod = 3;
    else if (isBig && mod == 0)
        mod = 1;
    else if (isHot && mod == 1)
        mod = 2;
    else if (isHot && mod == 0)
        mod = 0;

    Int_t id = -1;
    
    for (auto it : fGemStatModId) {
        if (it[0] != stat || it[1] != mod)
            continue;
        
        id = it[2];
        
        break;
    }
    
    return GetGemSerial(stat, mod, id, channel);
}







