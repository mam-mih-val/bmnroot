#include "BmnCscRaw2Digit.h"

BmnCscRaw2Digit::BmnCscRaw2Digit() {
    fEventId = -1;
    fMapFileName = "";
}

BmnCscRaw2Digit::BmnCscRaw2Digit(Int_t period, Int_t run, vector<UInt_t> vSer, TString MapFileName) : BmnAdcProcessor(period, run, "CSC", ADC_N_CHANNELS, ADC32_N_SAMPLES, vSer) {

    cout << "Loading CSC Map: Period " << period << ", Run " << run << "..." << endl;

    fEventId = 0;
    fMapFileName = MapFileName;
    for (Short_t iMod = 0; iMod < N_CSC_MODULES; ++iMod) {
        for (Short_t ich = 0; ich < N_CSC_CHANNELS; ++ich) {
            channel2layer[iMod][ich] = -1;
            channel2strip[iMod][ich] = -1;
        }
    }
    ReadMapFile();
    ReadMapLocalFile();

    fCscStationSetDer = BmnAdcProcessor::GetCSCStationSet(period, fSetup);

    Int_t kNStations = fCscStationSetDer->GetNStations();
    fSigProf = new TH1F***[kNStations];
    fNoisyChannels = new Bool_t***[kNStations];
    for (Int_t iSt = 0; iSt < kNStations; ++iSt) {
        auto * st = fCscStationSetDer->GetStation(iSt);
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
                histName.Form("CSC_%d_%d_%d", iSt, iMod, iLay);
                fSigProf[iSt][iMod][iLay] = new TH1F(histName, histName, kNStrips, 0, kNStrips);
                fSigProf[iSt][iMod][iLay]->SetDirectory(0);
                fNoisyChannels[iSt][iMod][iLay] = new Bool_t[kNStrips];
                for (Int_t iStrip = 0; iStrip < kNStrips; ++iStrip)
                    fNoisyChannels[iSt][iMod][iLay][iStrip] = kFALSE;
            }
        }
    }
    thrMax = 80;
    thrDif = 15;
    niter = 3;
}

BmnCscRaw2Digit::~BmnCscRaw2Digit() {
    for (Int_t iSt = 0; iSt < fCscStationSetDer->GetNStations(); ++iSt) {
        auto * st = fCscStationSetDer->GetStation(iSt);
        for (UInt_t iMod = 0; iMod < st->GetNModules(); ++iMod) {
            auto * mod = st->GetModule(iMod);
            for (Int_t iLay = 0; iLay < mod->GetNStripLayers(); ++iLay) {
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
    //    for (auto it : fMap) delete it;
    for (auto &it : fOuterMap)
        for (auto &inner : it.second)
            delete inner.second;
    if (fCscStationSetDer) delete fCscStationSetDer;
}

BmnStatus BmnCscRaw2Digit::ReadMapFile() {
    UInt_t ser = 0;
    Int_t ch_lo = 0;
    Int_t ch_hi = 0;
    Short_t zone = 0;
    Short_t mod = 0;
    Short_t lay = 0;
    Short_t station = 0;
    string dummy;

    TString name = TString(getenv("VMCWORKDIR")) + TString("/input/") + fMapFileName;
    ifstream inFile(name.Data());
    if (!inFile.is_open()) {
        printf(ANSI_COLOR_RED "\n[ERROR]" ANSI_COLOR_RESET);
        printf(" Error opening map-file (%s)", name.Data());
    }
    getline(inFile, dummy); //comment line in input file
    getline(inFile, dummy); //comment line in input file

    while (!inFile.eof()) {
        inFile >> std::hex >> ser >> std::dec >> ch_lo >> ch_hi >> zone >> station >> mod >> lay;
        if (!inFile.good()) break;
        BmnCscMapping* record = new BmnCscMapping();
        record->layer = lay;
        record->serial = ser;
        record->zone = zone;
        record->module = mod;
        record->channel_low = ch_lo / GetNSamples();
        record->channel_high = ch_hi / GetNSamples();
        record->station = station;
        //        fMap.push_back(record);
        auto it = fOuterMap.find(ser);
        if (it == fOuterMap.end()) { // create inner channel map for the serial 
            InChanMapCSC inner;
            inner.insert(make_pair(record->channel_low - 1, nullptr));
            inner.insert(make_pair(record->channel_high, record));
            fOuterMap.insert(make_pair(ser, move(inner)));
        } else { // add range to the existing inner channel map
            InChanMapCSC &inner = it->second;
            auto innerItHi = inner.find(record->channel_high);
            auto innerItLo = inner.find(record->channel_low - 1);
            if (innerItHi == inner.end()) {
                inner.insert(make_pair(record->channel_high, record));
            } else {
                if (innerItHi->second == nullptr) {
                    inner.erase(innerItHi);
                    inner.insert(make_pair(record->channel_high, record));
                } else {
                    delete record;
                    // fprintf(stderr, "Wrong %s map! Overlapping intervals for %08X!\n", fDetName.Data(), ser);
                    // return kBMNERROR;
                }
            }
            if (innerItLo == inner.end()) {
                inner.insert(make_pair(record->channel_low - 1, nullptr));
            }
        }
    }

    return kBMNSUCCESS;
}

BmnStatus BmnCscRaw2Digit::ReadMapLocalFile() {
    Short_t modules = 2;
    Short_t layers = 4;

    for (Short_t iMod = 0; iMod < modules; ++iMod) {
        for (Short_t iLay = 0; iLay < layers; ++iLay) {
            TString name = TString(getenv("VMCWORKDIR")) + TString("/input/") +
                    TString("CSC_m") + iMod +
                    TString("l") + (((iMod == 1) && (fPeriod < 8)) ? (iLay + 2) % 4 : iLay) +
                    TString(".txt");
            ifstream inFile(name.Data());
            if (!inFile.is_open()) {
                printf(ANSI_COLOR_RED "\n[ERROR]" ANSI_COLOR_RESET);
                printf(" Error opening map-file (%s)", name.Data());
            }
            Int_t strip = 0;
            while (!inFile.eof()) {
                Int_t ch;
                inFile >> ch;
                if (ch < N_CSC_CHANNELS) {
                    channel2layer[iMod][ch] = iLay;
                    channel2strip[iMod][ch] = strip;
                } else {
                    printf("Error! Wrong strip map in the %s !", name.Data());
                    return kBMNERROR;
                }
                strip++;
            }
        }
    }
    return kBMNSUCCESS;
}

BmnStatus BmnCscRaw2Digit::FillEvent(TClonesArray *adc, TClonesArray * csc) {
    fEventId++;
    //    for (Int_t iAdc = 0; iAdc < adc->GetEntriesFast(); ++iAdc) {
    //        BmnADCDigit* adcDig = (BmnADCDigit*) adc->At(iAdc);
    //        ProcessDigit(adcDig, FindMapEntry(adcDig), csc, kFALSE);
    //    }
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
    //    for (Int_t iAdc = 0; iAdc < adc->GetEntriesFast(); ++iAdc) {
    //        BmnADCDigit* adcDig = (BmnADCDigit*) adc->At(iAdc);
    //        ProcessDigit(adcDig, FindMapEntry(adcDig), csc, kFALSE);
    //    }
    ProcessAdc(adc, csc, kFALSE);
    timer.Stop();
    rtime = timer.RealTime();
    ctime = timer.CpuTime();
    //    printf("Real time %f s, CPU time %f s  ProcessAdc\n", rtime, ctime);

    return kBMNSUCCESS;
}

BmnStatus BmnCscRaw2Digit::FillProfiles(TClonesArray *adc) {
    //    for (Int_t iAdc = 0; iAdc < adc->GetEntriesFast(); ++iAdc) {
    //        BmnADCDigit* adcDig = (BmnADCDigit*) adc->At(iAdc);
    //        ProcessDigit(adcDig, FindMapEntry(adcDig), NULL, kTRUE);
    //    }
    (this->*PrecalcEventModsImp)(adc);
#ifdef BUILD_DEBUG
    CalcEventMods();
#else
    CalcEventMods_simd();
#endif
    //    for (Int_t iAdc = 0; iAdc < adc->GetEntriesFast(); ++iAdc) {
    //        BmnADCDigit* adcDig = (BmnADCDigit*) adc->At(iAdc);
    //        ProcessDigit(adcDig, FindMapEntry(adcDig), NULL, kTRUE);
    //    }
    ProcessAdc(adc, nullptr, kTRUE);

    return kBMNSUCCESS;
}

//BmnCscMapping* BmnCscRaw2Digit::FindMapEntry(BmnADCDigit* adcDig) {
//    for (Int_t iMap = 0; iMap < fMap.size(); ++iMap) {
//        BmnCscMapping* tM = fMap[iMap];
//        UInt_t ch = adcDig->GetChannel();
//        if (adcDig->GetSerial() == tM->serial && (ch >= tM->channel_low && ch <= tM->channel_high))
//            return tM;
//    }
//    return NULL;
//}

BmnStatus BmnCscRaw2Digit::FillNoisyChannels() {
    const Int_t kNStripsInBunch = GetNSamples();
    //    const Int_t kNBunches = kNStrips / kNStripsInBunch;
    const Int_t kNThresh = 3;

    for (Int_t iCr = 0; iCr < GetNSerials(); ++iCr)
        for (Int_t iCh = 0; iCh < GetNChannels(); ++iCh) {
            auto it = fOuterMap.find(GetSerials()[iCr]);
            if (it == fOuterMap.end())
                continue;
            InChanMapCSC & inner = it->second;
            auto innerIt = inner.lower_bound(iCh);
            if (innerIt == inner.end())
                continue;
            BmnCscMapping* rec = innerIt->second;
            if (!rec)
                continue;
            //            for (auto &it : fMap)
            //                if (GetSerials()[iCr] == it->serial && iCh >= it->channel_low && iCh <= it->channel_high) {
            for (Int_t iSmpl = 0; iSmpl < GetNSamples(); ++iSmpl)
                if (GetNoisyChipChannels()[iCr][iCh][iSmpl] == kTRUE) {
                    Int_t station = -1;
                    Int_t strip = -1;
                    Int_t layer = -1;
                    Int_t module = -1;
                    MapStrip(rec, iCh, iSmpl, station, module, layer, strip);
                    if (strip < 0)
                        continue;
                    fNoisyChannels[station][module][layer][strip] = kTRUE;
                }
        }
    for (Int_t iSt = 0; iSt < fCscStationSetDer->GetNStations(); ++iSt) {
        auto * st = fCscStationSetDer->GetStation(iSt);
        for (UInt_t iMod = 0; iMod < st->GetNModules(); ++iMod) {
            auto *mod = st->GetModule(iMod);
            for (Int_t iLay = 0; iLay < mod->GetNStripLayers(); ++iLay) {
                auto & lay = mod->GetStripLayer(iLay);
                Int_t kNBunches = lay.GetNStrips() / kNStripsInBunch;
                TH1F* prof = fSigProf[iSt][iMod][iLay];
                for (Int_t iBunch = 0; iBunch < kNBunches; ++iBunch) {
                    Double_t mean = 0.0;
                    for (Int_t iStrip = 0; iStrip < kNStripsInBunch - 1; ++iStrip) {
                        Int_t strip = iStrip + iBunch * kNStripsInBunch;
                        if (fNoisyChannels[iSt][iMod][iLay][strip] == kTRUE) continue;
                        Double_t curr = prof->GetBinContent(strip);
                        Double_t next = prof->GetBinContent(strip + 1);
                        mean += next;
                    }
                    mean /= kNStripsInBunch;
                    for (Int_t iStrip = 0; iStrip < kNStripsInBunch - 1; ++iStrip) {
                        Int_t strip = iStrip + iBunch * kNStripsInBunch;
                        if (fNoisyChannels[iSt][iMod][iLay][strip] == kTRUE) continue;
                        Double_t curr = prof->GetBinContent(strip);
                        Double_t next = prof->GetBinContent(strip + 1);
                        //                        if (kNThresh * meanDiff < next - curr)
                        if (kNThresh * mean < Abs(next - mean))
                            fNoisyChannels[iSt][iMod][iLay][strip] = kTRUE;
                    }
                }
            }
        }
    }
    for (Int_t iCr = 0; iCr < GetNSerials(); ++iCr)
        for (Int_t iCh = 0; iCh < GetNChannels(); ++iCh) {
            auto it = fOuterMap.find(GetSerials()[iCr]);
            if (it == fOuterMap.end())
                continue;
            InChanMapCSC & inner = it->second;
            auto innerIt = inner.lower_bound(iCh);
            if (innerIt == inner.end())
                continue;
            BmnCscMapping* rec = innerIt->second;
            if (!rec)
                continue;
            //            for (auto &it : fMap)
            //                if (GetSerials()[iCr] == it->serial && iCh >= it->channel_low && iCh <= it->channel_high) {
            for (Int_t iSmpl = 0; iSmpl < GetNSamples(); ++iSmpl) {
                Int_t station = -1;
                Int_t strip = -1;
                Int_t layer = -1;
                Int_t module = -1;
                MapStrip(rec, iCh, iSmpl, station, module, layer, strip);
                if (strip < 0)
                    continue;
                if (fNoisyChannels[station][module][layer][strip] == kTRUE)
                    GetNoisyChipChannels()[iCr][iCh][iSmpl] = kTRUE;
            }
        }

    return kBMNSUCCESS;
}

inline void BmnCscRaw2Digit::MapStrip(BmnCscMapping* cscM, UInt_t iCh, Int_t iSmpl, Int_t &station, Int_t &module, Int_t &layer, Int_t &strip) {
    station = cscM->station;
    module = cscM->module;
    Int_t ch2048 = iCh * GetNSamples() + iSmpl;
    layer = channel2layer[module][ch2048];
    strip = channel2strip[module][ch2048];
    //    printf("s %2d    m %2d    l %2d    ch %4d    s %4d\n", station, module, layer, ch2048, strip);
    return;
}

void BmnCscRaw2Digit::ProcessAdc(TClonesArray *adc, TClonesArray *csc, Bool_t doFill) {
    Double_t FinalThr = thrMax - (niter - 1) * thrDif;
    for (Int_t iCr = 0; iCr < fNSerials; ++iCr) {
        auto it = fOuterMap.find(GetSerials()[iCr]);
        if (it == fOuterMap.end())
            continue;
        for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
            auto innerIt = it->second.lower_bound(iCh);
            if (innerIt == it->second.end())
                continue;
            BmnCscMapping* rec = innerIt->second;
            if (!rec)
                continue;
            //            for (auto &it : fMap)
            //                if (GetSerials()[iCr] == it->serial && iCh >= it->channel_low && iCh <= it->channel_high) {
            Double_t cs = fCMode[iCr][iCh] - fSMode[iCr][iCh];
            for (Int_t iSmpl = 0; iSmpl < GetNSamples(); ++iSmpl) {
                if (GetNoisyChipChannels()[iCr][iCh][iSmpl] == kTRUE) continue;
                Int_t station = -1;
                Int_t strip = -1;
                Int_t layer = -1;
                Int_t module = -1;
                MapStrip(rec, iCh, iSmpl, station, module, layer, strip);
                if (strip < 0)
                    continue;
                Double_t sig = fAdc[iCr][iCh][iSmpl] - fPedVal[iCr][iCh][iSmpl] + cs;
                sig = -sig;
                Double_t Asig = TMath::Abs(sig);
                Double_t thr = Max(FinalThr, 3.5 * GetPedestalsRMS()[iCr][iCh][iSmpl]);
                if (sig > thr) {
                    if (doFill) {
                        fSigProf[station][module][layer]->Fill(strip);
                    } else {
                        BmnCSCDigit * resDig =
                                new((*csc)[csc->GetEntriesFast()])
                                BmnCSCDigit(station, module, layer, strip, sig);
                        //                                if (GetNoisyChipChannels()[iCr][iCh][iSmpl] == kTRUE)//((Abs(fCMode[iCr][iCh] - fSMode[iCr][iCh]) > cmodcut))
                        //                                    resDig->SetIsGoodDigit(kFALSE);
                        //                                else
                        resDig->SetIsGoodDigit(kTRUE);
                    }
                }
            }
            //                    break;
            //                }
        }
    }
}

ClassImp(BmnCscRaw2Digit)

