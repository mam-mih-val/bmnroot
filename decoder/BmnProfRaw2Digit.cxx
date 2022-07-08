#include "BmnProfRaw2Digit.h"

BmnProfRaw2Digit::BmnProfRaw2Digit() {
    fEventId = -1;
}

BmnProfRaw2Digit::BmnProfRaw2Digit(Int_t period, Int_t run /*, vector<UInt_t> vSer, TString MapFileName*/) : BmnAdcProcessor(period, run, "Prof", 1, ADC32_N_SAMPLES) {

    cout << "Loading Profilometer Map: Period " << period << ", Run " << run << "..." << endl;

    fEventId = 0;
    string dir = string(getenv("VMCWORKDIR")) + "/input/";
    fLocalMapFileName = dir + string(Form("Prof_map_run_%d.json", period));
    fGlobalMapFileName = dir + string(Form("Prof_map_run_%d_global.txt", period));
    ReadLocalMapFile(fLocalMapFileName);
    ReadGlobalMapFile(fGlobalMapFileName);

    fSiBTStationSet = GetProfStationSet(period);

    Int_t kNStations = fSiBTStationSet->GetNStations();
    fSigProf = new TH1F***[kNStations];
    fNoisyChannels = new Bool_t***[kNStations];
    for (Int_t iSt = 0; iSt < kNStations; ++iSt) {
        auto * st = fSiBTStationSet->GetStation(iSt);
        Int_t kNModules = st->GetNModules();
        fSigProf[iSt] = new TH1F**[kNModules];
        fNoisyChannels[iSt] = new Bool_t**[kNModules];
        for (UInt_t iMod = 0; iMod < kNModules; ++iMod) {
            auto *mod = st->GetModule(iMod);
            Int_t kNLayers = mod->GetNStripLayers();
            fSigProf[iSt][iMod] = new TH1F*[kNLayers];
            fNoisyChannels[iSt][iMod] = new Bool_t*[kNLayers];
            for (Int_t iLay = 0; iLay < kNLayers; ++iLay) {
                auto & lay = mod->GetStripLayer(iLay);
                Int_t kNStrips = lay.GetNStrips();
                TString histName;
                histName.Form("Prof_%d_%d_%d", iSt, iMod, iLay);
                fSigProf[iSt][iMod][iLay] = new TH1F(histName, histName, kNStrips, 0, kNStrips);
                fSigProf[iSt][iMod][iLay]->SetDirectory(0);
                fNoisyChannels[iSt][iMod][iLay] = new Bool_t[kNStrips + 1];
                for (Int_t iStrip = 0; iStrip <= kNStrips; ++iStrip)
                    fNoisyChannels[iSt][iMod][iLay][iStrip] = kFALSE;
            }
        }
    }
    thrMax = 50;
    thrDif = 10;
    niter = 4;
}

BmnProfRaw2Digit::~BmnProfRaw2Digit() {
    for (Int_t iSt = 0; iSt < fSiBTStationSet->GetNStations(); ++iSt) {
        auto * st = fSiBTStationSet->GetStation(iSt);
        Int_t kNModules = st->GetNModules();
        for (UInt_t iMod = 0; iMod < kNModules; ++iMod) {
            auto *mod = st->GetModule(iMod);
            Int_t kNLayers = mod->GetNStripLayers();
            for (Int_t iLay = 0; iLay < kNLayers; ++iLay) {
                delete fSigProf[iSt][iMod][iLay];
                delete[] fNoisyChannels[iSt][iMod][iLay];
            }
            delete[] fSigProf[iSt][iMod];
            delete[] fNoisyChannels[iSt][iMod];
        }
        delete[] fSigProf[iSt];
        delete[] fNoisyChannels[iSt];
    }
    delete[] fNoisyChannels;
    delete[] fSigProf;
}

unique_ptr<BmnSiBTStationSet> BmnProfRaw2Digit::GetProfStationSet(Int_t period) {
    TString gPathConfig = getenv("VMCWORKDIR");
    TString xmlConfFileName;
    switch (period) {
        case 8:
            xmlConfFileName = "ProfRun8.xml";
            break;
        default:
            printf("Error! Unknown config!\n");
            return nullptr;
            break;
    }
    TString gPathSiliconConfig = gPathConfig + "/parameters/profilometer/XMLConfigs/";
        return unique_ptr<BmnSiBTStationSet>(new BmnSiBTStationSet(gPathSiliconConfig + xmlConfFileName));
//    return std::make_unique<BmnSiBTStationSet>(gPathSiliconConfig + xmlConfFileName);
}

BmnStatus BmnProfRaw2Digit::ReadGlobalMapFile(string name) {
    string dummy;
    UInt_t ser = 0;
    UInt_t ch = 0;
    UInt_t station = 0;
    UInt_t mod = 0;
    ifstream inFile(name);
    if (!inFile.is_open())
        cout << "Error opening map-file (" << name << ")!" << endl;
    for (Int_t i = 0; i < 2; ++i) getline(inFile, dummy); //comment line in input file
    while (!inFile.eof()) {
        inFile >> std::hex >> ser >> std::dec >> ch >> station >> mod;
        if (!inFile.good()) break;
        auto it = fChannelMaps.find(mod);
        if (it == fChannelMaps.end())
            continue;
        ProfiMap copy = it->second; // a bit of memory waste
        copy.StationId = station;
        copy.LayerId = copy.LayerType != 'p';
        fGlobalMap.insert(make_pair(make_pair(ser, ch), copy));
    }
    return kBMNSUCCESS;
}

BmnStatus BmnProfRaw2Digit::ReadLocalMapFile(string name) {
    try {
        pt::ptree conf;
        pt::read_json(name, conf);
        pt::ptree pads = conf.get_child("modules");
        for (auto v = pads.begin(); v != pads.end(); v++) {
//            cout << " channels " << (*v).second.get_optional<Int_t>("nAsicChannels") << endl;
//            cout << " mod      " << (*v).second.get_optional<uint16_t>("moduleNumber") << endl;
            uint16_t mod = (*v).second.get<uint16_t>("moduleNumber");
            pt::ptree maps = (*v).second.get_child("channelMapping");
            for (auto m = maps.begin(); m != maps.end(); m++) {
                fChannelMaps.insert(make_pair(
                        mod,
                        ProfiMap{
                    .ChannelName = (*m).second.get<char>("adcChName"),
                    .LayerType = (*m).second.get<char>("layerType"),
                    .ModuleId = mod
                }
                ));
                pt::ptree strips = (*m).second.get_child("stripsMapping");
                for (auto stripNode = strips.begin(); stripNode != strips.end(); stripNode++) {
                    auto it = (*stripNode).second.begin();
                    //                    cout << it->second.get_value<int>() << " : " <<
                    //                            (++it)->second.get_value<int>()<< endl;
                    int stripId = (it)->second.get_value<int>();
                    int chanlId = (++it)->second.get_value<int>();
                    fChannelMaps[mod].StripMap[chanlId] = stripId;
                }


            }
        }
    } catch (boost::exception &e) {
        cerr << boost::diagnostic_information(e);
        cout << "Unable to parse the channel map!\n" << endl;
        return kBMNERROR;
    }
    return kBMNSUCCESS;
}

BmnStatus BmnProfRaw2Digit::FillEvent(TClonesArray *adc, TClonesArray * csc) {
    fEventId++;
    TStopwatch timer;
    Double_t rtime;
    Double_t ctime;
    timer.Start();
    (this->*PrecalcEventModsImp)(adc);
    timer.Stop();
    rtime = timer.RealTime();
    ctime = timer.CpuTime();
    //    printf("\nReal time %f s, CPU time %f s  PrecalcEventMods\n", rtime, ctime);
    timer.Start();
#ifdef BUILD_DEBUG
    CalcEventMods();
#else
    CalcEventMods_simd();
#endif
    timer.Stop();
    rtime = timer.RealTime();
    ctime = timer.CpuTime();
    //    printf("Real time %f s, CPU time %f s  CalcEventMods\n", rtime, ctime);
    timer.Start();
    ProcessAdc(adc, csc, kFALSE);
    timer.Stop();
    rtime = timer.RealTime();
    ctime = timer.CpuTime();
    //    printf("Real time %f s, CPU time %f s  ProcessAdc\n", rtime, ctime);

    return kBMNSUCCESS;
}

BmnStatus BmnProfRaw2Digit::FillProfiles(TClonesArray *adc) {
    (this->*PrecalcEventModsImp)(adc);
#ifdef BUILD_DEBUG
    CalcEventMods();
#else
    CalcEventMods_simd();
#endif
    ProcessAdc(adc, nullptr, kTRUE);

    return kBMNSUCCESS;
}

BmnStatus BmnProfRaw2Digit::FillNoisyChannels() {
    const Int_t kNStripsInBunch = GetNSamples();
    //    const Int_t kNBunches = kNStrips / kNStripsInBunch;
    const Int_t kNThresh = 3;

    for (Int_t iCr = 0; iCr < GetNSerials(); ++iCr)
        for (Int_t iCh = 0; iCh < GetNChannels(); ++iCh) {
            auto it = fGlobalMap.find(make_pair(iCr, iCh));
            if (it == fGlobalMap.end())
                continue;
            ProfiMap & map = it->second;
            for (Int_t iSmpl = 0; iSmpl < GetNSamples(); ++iSmpl)
                if (GetNoisyChipChannels()[iCr][iCh][iSmpl] == kTRUE) {
                    uint16_t station = map.StationId;
                    uint16_t layer = map.LayerId;
                    uint16_t module = map.ModuleId;
                    Int_t strip = map.StripMap[iSmpl];
                    if (strip < 0)
                        continue;
                    fNoisyChannels[station][module][layer][strip] = kTRUE;
                }
        }
    for (Int_t iSt = 0; iSt < fSiBTStationSet->GetNStations(); ++iSt) {
        auto * st = fSiBTStationSet->GetStation(iSt);
        for (Int_t iMod = 0; iMod < st->GetNModules(); ++iMod) {
            auto *mod = st->GetModule(iMod);
            for (Int_t iLay = 0; iLay < mod->GetNStripLayers(); ++iLay) {
                auto &lay = mod->GetStripLayer(iLay);
                TH1F* prof = fSigProf[iSt][iMod][iLay];
                Double_t mean = 0.0;
                for (Int_t iStrip = 0; iStrip < lay.GetNStrips(); ++iStrip) {
                    if (fNoisyChannels[iSt][iMod][iLay][iStrip] == kTRUE) continue;
                    Double_t curr = prof->GetBinContent(iStrip);
                    Double_t next = prof->GetBinContent(iStrip + 1);
                    mean += next;
                }
                if (mean > 0)
                    mean /= lay.GetNStrips();
                for (Int_t iStrip = 0; iStrip < lay.GetNStrips(); ++iStrip) {
                    if (fNoisyChannels[iSt][iMod][iLay][iStrip] == kTRUE) continue;
                    Double_t curr = prof->GetBinContent(iStrip);
                    Double_t next = prof->GetBinContent(iStrip + 1);
                    //                        if (kNThresh * meanDiff < next - curr)
                    if (kNThresh * mean < Abs(next - mean))
                        fNoisyChannels[iSt][iMod][iLay][iStrip] = kTRUE;
                }
            }
        }
    }
    for (Int_t iCr = 0; iCr < GetNSerials(); ++iCr)
        for (Int_t iCh = 0; iCh < GetNChannels(); ++iCh) {
            auto it = fGlobalMap.find(make_pair(iCr, iCh));
            if (it == fGlobalMap.end())
                continue;
            ProfiMap & map = it->second;
            for (Int_t iSmpl = 0; iSmpl < GetNSamples(); ++iSmpl)
                if (GetNoisyChipChannels()[iCr][iCh][iSmpl] == kTRUE) {
                    uint16_t station = map.StationId;
                    uint16_t layer = map.LayerId;
                    uint16_t module = map.ModuleId;
                    Int_t strip = map.StripMap[iSmpl];
                    if (strip < 0)
                        continue;
                    if (fNoisyChannels[station][module][layer][strip] == kTRUE)
                        GetNoisyChipChannels()[iCr][iCh][iSmpl] = kTRUE;
                }
        }

    return kBMNSUCCESS;
}

void BmnProfRaw2Digit::ProcessAdc(TClonesArray *adc, TClonesArray *arr, Bool_t doFill) {
    Double_t FinalThr = thrMax - (niter - 1) * thrDif;
    for (Int_t iCr = 0; iCr < fNSerials; ++iCr) {
        for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
            auto it = fGlobalMap.find(make_pair(iCr, iCh));
            if (it == fGlobalMap.end())
                continue;
            ProfiMap & map = it->second;
            Double_t cs = fCMode[iCr][iCh] - fSMode[iCr][iCh];
            for (Int_t iSmpl = 0; iSmpl < GetNSamples(); ++iSmpl) {
                if (GetNoisyChipChannels()[iCr][iCh][iSmpl] == kTRUE) continue;
                uint16_t station = map.StationId;
                uint16_t layer = map.LayerId;
                uint16_t module = map.ModuleId;
                Int_t strip = map.StripMap[iSmpl];
                if (strip < 0)
                    continue;
                Double_t sig = fAdc[iCr][iCh][iSmpl] - fPedVal[iCr][iCh][iSmpl] + cs;
                Double_t Asig = TMath::Abs(sig);
                Double_t thr = Max(FinalThr, 3.5 * GetPedestalsRMS()[iCr][iCh][iSmpl]);
                if (Asig > thr) {
                    if (doFill) {
                        fSigProf[station][module][layer]->Fill(strip);
                    } else {
                        BmnSiBTDigit * resDig =
                                new((*arr)[arr->GetEntriesFast()])
                                BmnSiBTDigit(station, module, layer, strip, sig);
                        resDig->SetIsGoodDigit(kTRUE);
                    }
                }
            }
        }
    }
}

ClassImp(BmnProfRaw2Digit)

