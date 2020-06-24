#include <ratio>

#include "BmnLambdaMisc.h"

BmnLambdaMisc::BmnLambdaMisc() :
fDetectorGEM(nullptr),
fDetectorSI(nullptr),
fDetectorCSC(nullptr) {
    TString gPathConfig = gSystem->Getenv("VMCWORKDIR");
    TString confGem = "GemRunSpring2018.xml";
    TString gPathGemConfig = gPathConfig + "/parameters/gem/XMLConfigs/";
    fDetectorGEM = new BmnGemStripStationSet(gPathGemConfig + confGem);

    TString confSi = "SiliconRunSpring2018.xml";
    TString gPathSiConfig = gPathConfig + "/parameters/silicon/XMLConfigs/";
    fDetectorSI = new BmnSiliconStationSet(gPathSiConfig + confSi);

    TString confCsc = "CSCRunSpring2018.xml";
    TString gPathCscConfig = gPathConfig + "/parameters/csc/XMLConfigs/";
    fDetectorCSC = new BmnCSCStationSet(gPathCscConfig + confCsc);

    GEM();
    SILICON();
    CSC();
}

BmnLambdaMisc::~BmnLambdaMisc() {
    if (fDetectorGEM)
        delete fDetectorGEM;

    if (fDetectorSI)
        delete fDetectorSI;

    if (fDetectorCSC)
        delete fDetectorCSC;
}

// Process GEM mapping ...

void BmnLambdaMisc::GEM() {
    TString gPathConfig = gSystem->Getenv("VMCWORKDIR");
    TString gPathFull = gPathConfig + "/input/GEM_map_run7.txt";

    Long_t commonSerial = 0x76D08B9;

    // Read GEM mapping (run7)
    ifstream g(gPathFull.Data());

    TString gemId = "", station = "", module = "", low = "", high = "", serial = "";
    Int_t linesOmitted = 4;

    Int_t counter = 0;
    for (string line; getline(g, line);) {
        counter++;
        if (counter > linesOmitted) {
            g >> serial >> low >> high >> gemId >> station >> module;
            Long_t ser = (Long_t) serial.Atof();
            if (ser == 0)
                continue;
            vector <Int_t> key{station.Atoi(), module.Atoi(), gemId.Atoi()};
            vector <Long_t> value{low.Atoll(), high.Atoll(), ser};
            if (ser == commonSerial)
                corrMapCommonSerial[key] = value;
            else
                corrMapNoCommonSerial[key] = value;

            // Fill useful vector to be used outside of the function ...
            fGemStatModId.push_back(key);
        }
    }
}

// Process SILICON mapping ...

void BmnLambdaMisc::SILICON() {
    TString gPathConfig = gSystem->Getenv("VMCWORKDIR");
    TString gPathFull = gPathConfig + "/input/SILICON_map_run7.txt";

    // Read GEM mapping (run7)
    ifstream g(gPathFull.Data());

    TString layer = "", station = "", modAdc = "", modGeo = "", low = "", high = "", serial = "";
    Int_t linesOmitted = 3;

    Int_t counter = 0;
    for (string line; getline(g, line);) {
        counter++;
        if (counter > linesOmitted) {
            g >> serial >> low >> high >> modAdc >> modGeo >> layer >> station;
            Long_t ser = (Long_t) serial.Atof();
            if (ser == 0)
                continue;
            vector <Int_t> key{station.Atoi(), modGeo.Atoi(), layer.Atoi(), modAdc.Atoi()};
            vector <Long_t> value{low.Atoll(), high.Atoll(), ser};

            serialsSilicon[key] = value;

            // Fill useful vector to be used outside of the function ...
            fSiliconStatModLayId.push_back(key);
        }
    }
}

// Processing CSC mapping ...

void BmnLambdaMisc::CSC() {
    TString gPathConfig = gSystem->Getenv("VMCWORKDIR");
    TString gPathFull = gPathConfig + "/input/CSC_map_period7.txt";

    // Read CSC mapping (run7)
    ifstream g(gPathFull.Data());

    TString zone = "", layer = "", station = "", module = "", low = "", high = "", serial = "";
    Int_t linesOmitted = 1;

    Int_t counter = 0;
    for (string line; getline(g, line);) {
        counter++;
        if (counter > linesOmitted) {
            g >> serial >> low >> high >> zone >> station >> module >> layer;
            Long_t ser = (Long_t) serial.Atof();
            if (ser == 0)
                continue;
            vector <Int_t> key{station.Atoi(), module.Atoi(), layer.Atoi()};
            vector <Long_t> value{low.Atoll(), high.Atoll(), ser};

            serialsCsc[key] = value;

            // Fill useful vector to be used outside of the function ...
            fCscStatModLay.push_back(key);
        }
    }
}

void BmnLambdaMisc::CheckStripOverlaps() {
    TClonesArray* mapInfo = new TClonesArray("MappingInfo");

    // Left and Right part for each station ...
    const Int_t nParts = 2;
    TString parts[nParts] = {"Left", "Right"};

    for (Int_t iStat = 0; iStat < fDetectorGEM->GetNStations(); iStat++) {

        for (Int_t iPart = 0; iPart < nParts; iPart++) {
            TString filename = (iStat == 0 || iStat == 1) ? TString::Format("GEM2_Y1_%s.txt", parts[iPart].Data()) :
                    TString::Format("GEM_Y1_Big_%s.txt", parts[iPart].Data());

            map <Int_t, Int_t> stripGlobChan;
            ParseStripChannelMapping(filename, iStat, stripGlobChan);

            // Trying to find min and max channel numbers ...
            vector <Int_t> channels;
            for (auto it : stripGlobChan)
                channels.push_back(it.second);

            Int_t min = *min_element(channels.begin(), channels.end());
            Int_t max = *max_element(channels.begin(), channels.end());

            // Create mapping info ...
            MappingInfo* info = new ((*mapInfo)[mapInfo->GetEntriesFast()]) MappingInfo(iStat, filename, stripGlobChan);
            info->channels = make_pair(min, max);
        }
    }

    // Looking for overlapping channels with main part of big zone ...
    // FIXME !!!
    const Int_t nStripsLeft = 1080;
    const Int_t nStripsRight = 1129;
    const Int_t addChanStart = 2048;

    TClonesArray* mapInfoMainPart = new TClonesArray("MappingInfo");

    // Loop over stations ..
    for (Int_t iStat = 0; iStat < fDetectorGEM->GetNStations(); iStat++) {

        // Loop over left and right mappings ...
        for (Int_t iPart = 0; iPart < nParts; iPart++) {
            TString filename = (iStat == 0 || iStat == 1) ? TString::Format("GEM2_Y1_%s.txt", parts[iPart].Data()) :
                    TString::Format("GEM_Y1_Big_%s.txt", parts[iPart].Data());

            // Loop over strips ...
            const Int_t nStrips = (filename.Contains("Left")) ? nStripsLeft : nStripsRight;

            for (Int_t iStrip = 0; iStrip < nStrips; iStrip++) {

                // Getting corr. channel ...
                UInt_t channel = FindChannelByStrip(filename, iStrip);

                // Skipping part that corresponds to common ADC ...
                if (channel > addChanStart - 1)
                    continue;

                new ((*mapInfoMainPart)[mapInfoMainPart->GetEntriesFast()]) MappingInfo(iStat, filename, channel, iStrip);
            }
        }
    }

    // Analyzing what is taken ... 
    BmnLambdaEmbeddingQa* qa = new BmnLambdaEmbeddingQa();
    qa->DrawHistos6(mapInfoMainPart, mapInfo);

    delete mapInfo;
    delete mapInfoMainPart;

    delete qa;
}

void BmnLambdaMisc::ParseStripChannelMapping(TString filename, Int_t stat, map <Int_t, Int_t>& stripsGlobChannels) {
    map <Int_t, Int_t> channelStrip; // channel --> strip correspondence

    TString gPathConfig = gSystem->Getenv("VMCWORKDIR");
    TString gPathFull = gPathConfig + "/input/" + filename;
    ifstream f(gPathFull.Data());

    TString channelCur = "";

    Int_t stripCounter = 0;
    while (!f.eof()) {
        f >> channelCur;
        channelStrip[channelCur.Atoi()] = stripCounter;
        stripCounter++;
    }

    // Left and Right files
    // Returns vector of pairs that correspond to blocks of channels connected to common ADC in the global encoding scheme

    // Channel shift for common ADC as a function of stat / mod number ...
    Int_t chanShift = 0;

    // <st, mod, id> ---> <low, high, serial>
    for (auto it : corrMapCommonSerial) {
        if (it.first[0] != stat)
            continue;

        // Choosing right or left part of station ...
        Int_t id = it.first[2]; // Current gemId
        if (filename.Contains("Left") && (id % 10 == 0))
            chanShift += it.second[0];

        else if (filename.Contains("Right") && (id % 10 != 0))
            chanShift += it.second[0];
    }

    // Trying to locate channels attributed to common ADC ...
    Int_t addChanStart = 2048;

    for (auto it : channelStrip) {
        Int_t chan = it.first;
        if (chan < addChanStart)
            continue;

        Int_t locChan = chan - addChanStart;
        Int_t globChan = locChan + chanShift;

        stripsGlobChannels[it.second] = globChan;
    }
}

Long_t BmnLambdaMisc::GetGemSerial(Int_t stat, Int_t mod, Int_t id, Int_t channel) {
    for (auto it : corrMapNoCommonSerial) {

        if (it.first[0] != stat || it.first[1] != mod || it.first[2] != id)
            continue;

        if (channel >= it.second[0] && channel <= it.second[1])
            return it.second[2];
    }
    return 0x0;
}

// Make correspondence <CSC-digi ---> channel(serial)>

Int_t BmnLambdaMisc::CscDigiToChannel(BmnStripDigit* dig, Long_t& ser) {
    Int_t stat = dig->GetStation();
    Int_t mod = dig->GetModule();
    Int_t lay = dig->GetStripLayer();
    Int_t strip = dig->GetStripNumber();

    TString filename = ""; // mapping with strip -> channel correspondence ...

    for (auto it : serialsCsc) {
        vector <Int_t> statModLay = it.first;

        if (stat != statModLay[0] || mod != statModLay[1] || lay != statModLay[2])
            continue;

        ser = it.second[2]; // get corr. serial ...

        filename = TString::Format("CSC_m%dl%d.txt", mod, lay); // get necessary file ...

        break;
    }

    // Let's get a corresp. chan. number for a current strip ...
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

    Int_t chan = stripChannels.find(strip)->second;

    return chan;
}

// Make correspondence <SILICON-digi ---> channel, sample, serial>

void BmnLambdaMisc::SiliconDigiToChannelSampleSerial(BmnStripDigit* dig, Int_t& chan, Int_t& sample, Long_t& serial) {
    Int_t stat = dig->GetStation();
    Int_t mod = dig->GetModule();
    Int_t lay = dig->GetStripLayer();
    Int_t strip = dig->GetStripNumber();

    for (auto it : serialsSilicon) {
        vector <Int_t> statModLay = it.first;

        if (stat != statModLay[0] || mod != statModLay[1] || lay != statModLay[2])
            continue;

        Int_t chanShift = Int_t(strip / 128);
        chan = it.second[0] + chanShift;
        sample = Int_t(strip % 128);
        serial = it.second[2];
    }
}

// Make correspondance GEM-digi ---> corresponding <strip-channel> mapping

TString BmnLambdaMisc::GemDigiToMapping(BmnStripDigit* dig) {
    Int_t stat = dig->GetStation();
    Int_t mod = dig->GetModule();
    Int_t lay = dig->GetStripLayer();
    Int_t strip = dig->GetStripNumber();

    TString layer = (lay == 0 || lay == 2) ? "X" : "Y";
    Int_t hotOrBig = -1;
    TString LeftOrRight = "";
    TString filename = "";

    Int_t modMakan = -1;

    for (auto it : fGemStatModId) {
        vector <Int_t> vect = it; // (stat, module, id)

        // Choosing station ...
        if (vect[0] != stat)
            continue;

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

        if (hotOrBig == 0) {
            if ((stat == 0 || stat == 3 || stat == 5))
                filename = TString::Format("GEM2_%s%d_%s.txt", layer.Data(), hotOrBig, LeftOrRight.Data());

            else
                filename = TString::Format("GEM_%s%d_Big_%s.txt", layer.Data(), hotOrBig, LeftOrRight.Data());
        } else {
            if ((stat == 0 || stat == 1))
                filename = TString::Format("GEM2_%s%d_%s.txt", layer.Data(), hotOrBig, LeftOrRight.Data());

            else
                filename = TString::Format("GEM_%s%d_Big_%s.txt", layer.Data(), hotOrBig, LeftOrRight.Data());
        }

        modMakan = vect[1];
        break;
    }

    return filename;
}


// Make correspondence <GEM-digi ---> channel>

Int_t BmnLambdaMisc::GemDigiToChannel(BmnStripDigit* dig, Long_t& serial) {
    Int_t stat = dig->GetStation();
    Int_t mod = dig->GetModule();
    Int_t lay = dig->GetStripLayer();
    Int_t strip = dig->GetStripNumber();

    // {0 ---> LeftToRight, 1 ---> RightToLeft};
    Int_t order = fDetectorGEM->GetStation(stat)->GetModule(mod)->GetStripLayer(lay).GetStripNumberingOrder();

    TString layer = (lay == 0 || lay == 2) ? "X" : "Y";
    Int_t hotOrBig = -1;
    TString LeftOrRight = "";
    TString filename = "";

    Int_t modMakan = -1;

    for (auto it : fGemStatModId) {
        vector <Int_t> vect = it; // (stat, module, id)

        // Choosing station ...
        if (vect[0] != stat)
            continue;

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

        if (hotOrBig == 0) {
            if ((stat == 0 || stat == 3 || stat == 5))
                filename = TString::Format("GEM2_%s%d_%s.txt", layer.Data(), hotOrBig, LeftOrRight.Data());

            else
                filename = TString::Format("GEM_%s%d_Big_%s.txt", layer.Data(), hotOrBig, LeftOrRight.Data());
        } else {
            if ((stat == 0 || stat == 1))
                filename = TString::Format("GEM2_%s%d_%s.txt", layer.Data(), hotOrBig, LeftOrRight.Data());

            else
                filename = TString::Format("GEM_%s%d_Big_%s.txt", layer.Data(), hotOrBig, LeftOrRight.Data());
        }

        modMakan = vect[1];
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

    UInt_t chan = stripChannels.find(strip)->second;
  
    // Check if we have a channel greater than 2047 ...
    UInt_t addChannelsStart = 2048;
    if (chan >= addChannelsStart) {
        for (auto it : corrMapCommonSerial) {
            Int_t station = it.first[0];
            Int_t module = it.first[1];

            if (station != stat || module != modMakan)
                continue;

            UInt_t low = it.second[0];
            UInt_t high = it.second[1];

            chan = chan - addChannelsStart + low;

            // Channel not found correctly ...
            if (chan > high)
                return -1;

            // Save this common serial ...
            for (auto itSerial : corrMapCommonSerial) {
                serial = itSerial.second[2];
                break;
            }
        }
    }
    return chan;
}

UInt_t BmnLambdaMisc::FindChannelByStrip(TString f, Int_t strip) {
    TString gPathConfig = gSystem->Getenv("VMCWORKDIR");
    TString gPathFull = gPathConfig + "/input/" + f;

    ifstream file(gPathFull.Data());
    map <Int_t, Int_t> stripChannels; // Map to store read channel for each strip
    TString channel = "";

    Int_t stripCounter = 0;
    while (!file.eof()) {
        file >> channel;
        stripChannels[stripCounter] = channel.Atoi();
        stripCounter++;
    }
    file.close();

    return stripChannels.find(strip)->second;
}

Long_t BmnLambdaMisc::GemDigiChannelToSerial(pair <BmnStripDigit, Int_t> digiChannel) {
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
