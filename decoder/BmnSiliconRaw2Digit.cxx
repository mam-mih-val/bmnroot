#include <TStyle.h>

#include "BmnSiliconRaw2Digit.h"

BmnSiliconRaw2Digit::BmnSiliconRaw2Digit() {
    fEventId = -1;
    fMapFileName = "";
}

BmnSiliconRaw2Digit::BmnSiliconRaw2Digit(Int_t period, Int_t run, vector<UInt_t> vSer, TString MapFileName, BmnSetup bmnSetup, BmnADCDecoMode decoMode) : BmnAdcProcessor(period, run, "SILICON", ADC_N_CHANNELS, ADC128_N_SAMPLES, vSer) {
    fSetup = bmnSetup;
    cout << "Loading SILICON Map from FILE: Period " << period << ", Run " << run << "..." << endl;

    fEventId = -1;
    fMapFileName = MapFileName;
    ReadMapFile();

    if (decoMode == kBMNADCSM) {
        fSilStationSet = BmnAdcProcessor::GetSilStationSet(period, fSetup);

        Int_t kNStations = fSilStationSet->GetNStations();
        fSigProf = new TH1F***[kNStations];
        fNoisyChannels = new Bool_t***[kNStations];
        for (Int_t iSt = 0; iSt < kNStations; ++iSt) {
            auto * st = fSilStationSet->GetStation(iSt);
            Int_t kNModules = st->GetNModules();
            fSigProf[iSt] = new TH1F**[kNModules];
            fNoisyChannels[iSt] = new Bool_t**[kNModules];
            for (UInt_t iMod = 0; iMod < kNModules; ++iMod) {
                auto *mod = st->GetModule(iMod);
                Int_t kNLayers = 2; // mod->GetNStripLayers();
                fSigProf[iSt][iMod] = new TH1F*[kNLayers];
                fNoisyChannels[iSt][iMod] = new Bool_t*[kNLayers];
                for (Int_t iLay = 0; iLay < kNLayers; ++iLay) {
                    auto & lay = mod->GetStripLayer(iLay);
                    Int_t kNStrips = lay.GetNStrips();
                    TString histName;
                    histName.Form("SIL_%d_%d_%d", iSt, iMod, iLay);
                    fSigProf[iSt][iMod][iLay] = new TH1F(histName, histName, kNStrips, 0, kNStrips);
                    fSigProf[iSt][iMod][iLay]->SetDirectory(0);
                    fNoisyChannels[iSt][iMod][iLay] = new Bool_t[kNStrips + 1];
                    for (Int_t iStrip = 0; iStrip <= kNStrips; ++iStrip)
                        fNoisyChannels[iSt][iMod][iLay][iStrip] = kFALSE;
                }
            }
        }
        cmodcut = 100;
        thrMax = 420;
        thrDif = 80;
        niter = 4;
    }
}

BmnSiliconRaw2Digit::~BmnSiliconRaw2Digit() {
        Int_t kNStations = fSilStationSet->GetNStations();
        for (Int_t iSt = 0; iSt < kNStations; ++iSt) {
            auto * st = fSilStationSet->GetStation(iSt);
            Int_t kNModules = st->GetNModules();
            for (UInt_t iMod = 0; iMod < kNModules; ++iMod) {
                auto *mod = st->GetModule(iMod);
                Int_t kNLayers = 2; //mod->GetNStripLayers();
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
        if (canStrip) delete canStrip;
    //    for (auto &it : fMap) delete it;
    for (auto &it : fOuterMap)
        for (auto &inner : it.second)
            delete inner.second;
}

BmnStatus BmnSiliconRaw2Digit::ReadMapFile() {
    UInt_t ser = 0;
    Int_t ch_lo = 0;
    Int_t ch_hi = 0;
    Int_t mod_adc = 0;
    Int_t mod = 0;
    Int_t lay = 0;
    Int_t station = 0;
    string dummy;

    TString name = TString(getenv("VMCWORKDIR")) + TString("/input/") + fMapFileName;
    ifstream inFile(name.Data());
    if (!inFile.is_open()) {
        cout << "Error opening map-file (" << name << ")!" << endl;
        return kBMNERROR;
    }
    getline(inFile, dummy); //comment line in input file
    getline(inFile, dummy); //comment line in input file
    getline(inFile, dummy); //comment line in input file
    getline(inFile, dummy); //comment line in input file

    while (!inFile.eof()) {
        inFile >> std::hex >> ser >> std::dec >> ch_lo >> ch_hi >> mod_adc >> mod >> lay >> station;
        if (!inFile.good()) break;
        BmnSiliconMapping * record = new BmnSiliconMapping();
        record->layer = lay;
        record->serial = ser;
        record->module = mod;
        if (ch_lo < ch_hi) {
            record->channel_low = ch_lo;
            record->channel_high = ch_hi;
        } else {
            record->channel_low = ch_hi;
            record->channel_high = ch_lo;
            record->inverted = true;
        }
        record->station = station;
        fMap.push_back(record);
        auto it = fOuterMap.find(ser);
        if (it == fOuterMap.end()) { // create inner channel map for the serial 
            InChanMapSil inner;
            inner.insert(make_pair(record->channel_low - 1, nullptr));
            inner.insert(make_pair(record->channel_high, record));
            fOuterMap.insert(make_pair(ser, move(inner)));
        } else { // add range to the existing inner channel map
            InChanMapSil &inner = it->second;
            auto innerItHi = inner.find(record->channel_high);
            auto innerItLo = inner.find(record->channel_low - 1);
            if (innerItHi == inner.end()) {
                inner.insert(make_pair(record->channel_high, record));
            } else {
                if (innerItHi->second == nullptr) {
                    inner.erase(innerItHi);
                    inner.insert(make_pair(record->channel_high, record));
                } else {
                    fprintf(stderr, "Wrong %s map! Overlapping intervals for %08X!\n", fDetName.Data(), ser);
                    return kBMNERROR;
                }
            }
            if (innerItLo == inner.end()) {
                inner.insert(make_pair(record->channel_low - 1, nullptr));
            }
            //            auto innerIt = inner.find(record->channel_low);
            //            if (innerIt == inner.end()) {
            //                inner.insert(make_pair(record->channel_low - 1, nullptr));
            //                inner.insert(make_pair(record->channel_high, record));
            //            } else {
            //                if (innerIt->second == nullptr) {
            //                    innerIt->second = record;
            //                } else {
            //                    //                    fprintf(stderr, "Wrong CSC map!\n");
            //                    //                    return kBMNERROR;
            //                }
            //            }
        }
    }
    return kBMNSUCCESS;
}

BmnStatus BmnSiliconRaw2Digit::FillEvent(TClonesArray *adc, TClonesArray *silicon) {
    //    printf("Event %i\n", fEventId);
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
    ProcessAdc(silicon, kFALSE);
    timer.Stop();
    rtime = timer.RealTime();
    ctime = timer.CpuTime();
    //    printf("Real time %f s, CPU time %f s  ProcessAdc\n", rtime, ctime);
    return kBMNSUCCESS;
}

BmnStatus BmnSiliconRaw2Digit::FillProfiles(TClonesArray *adc) {
    (this->*PrecalcEventModsImp)(adc);
#ifdef BUILD_DEBUG
    CalcEventMods();
#else
    CalcEventMods_simd();
#endif
    ProcessAdc(nullptr, kTRUE);

    return kBMNSUCCESS;
}

BmnStatus BmnSiliconRaw2Digit::FillNoisyChannels() {
    //    const Int_t kNStations = 3;
    //    const Int_t kNModules = 8;
    //    const Int_t kNLayers = 2;
    //    const Int_t kNStrips = 640;
    const Int_t kNStripsInBunch = GetNSamples();
    //    const Int_t kNBunches = kNStrips / kNStripsInBunch;
    Int_t kNThresh = 3;
    // repeat noisy channels in the physical terms (station/module/layer)
    for (Int_t iCr = 0; iCr < GetNSerials(); ++iCr) {
        auto it = fOuterMap.find(GetSerials()[iCr]);
        if (it == fOuterMap.end())
            continue;
        for (Int_t iCh = 0; iCh < GetNChannels(); ++iCh) {
            InChanMapSil & inner = it->second;
            auto innerIt = inner.lower_bound(iCh);
            if (innerIt == inner.end())
                continue;
            BmnSiliconMapping* rec = innerIt->second;
            if (!rec)
                continue;
            for (Short_t iSmpl = 0; iSmpl < GetNSamples(); ++iSmpl) {
                if (GetNoisyChipChannels()[iCr][iCh][iSmpl] == kTRUE) {
                    Int_t iStrip = MapStrip(rec, iCh, iSmpl);
                    fNoisyChannels[rec->station][rec->module][rec->layer][iStrip] = kTRUE;
                }
            }
        }
    }
    // mark noisy
    for (Int_t iSt = 0; iSt < fSilStationSet->GetNStations(); ++iSt) {
        auto * st = fSilStationSet->GetStation(iSt);
        for (Int_t iMod = 0; iMod < st->GetNModules(); ++iMod) {
            auto *mod = st->GetModule(iMod);
            for (Int_t iLay = 0; iLay < 2/*mod->GetNStripLayers()*/; ++iLay) {
                TH1F* prof = fSigProf[iSt][iMod][iLay];
                /** Big module has 2 separate parts of each layer*/
                Int_t realILayer = (mod->GetNStripLayers() == 4) ? 2 * iLay : iLay;
                auto & lay = mod->GetStripLayer(realILayer);
                Int_t kNBunches = lay.GetNStrips() / kNStripsInBunch;
                for (Int_t iBunch = 0; iBunch < kNBunches; ++iBunch) {
                    Double_t mean = 0.0;
                    for (Int_t iStrip = 0; iStrip < kNStripsInBunch; ++iStrip) {
                        Int_t strip = iStrip + iBunch * kNStripsInBunch;
                        if (fNoisyChannels[iSt][iMod][iLay][strip] == kTRUE) continue;
                        Double_t curr = prof->GetBinContent(strip + 1);
                        Double_t next = prof->GetBinContent(strip);
                        mean += curr;
                    }
                    mean /= kNStripsInBunch;
                    for (Int_t iStrip = 0; iStrip < kNStripsInBunch; ++iStrip) {
                        Int_t strip = iStrip + iBunch * kNStripsInBunch;
                        if (fNoisyChannels[iSt][iMod][iLay][strip] == kTRUE) continue;
                        Double_t curr = prof->GetBinContent(strip + 1);
                        Double_t next = prof->GetBinContent(strip);
                        //                        if (kNThresh * meanDiff < next - curr)
                        if ((kNThresh * Abs(mean) < Abs(curr - mean))/* || (kNThresh * meanDiff < -next + curr)*/) {
                            if (fVerbose)
                                printf("profile noise on iSt %d iMod %d iLay %d strip %d\n",
                                    iSt, iMod, iLay, strip);
                            fNoisyChannels[iSt][iMod][iLay][strip] = kTRUE;
                            for (auto &it : fMap)
                                if (it->station == iSt && it->module == iMod && it->layer == iLay) {
                                    UInt_t iCr = 0;
                                    for (iCr = 0; iCr < GetSerials().size(); iCr++) {
                                        if (GetSerials()[iCr] == it->serial)
                                            break;
                                    }
                                    UInt_t iCh = it->channel_low + (strip) / GetNSamples();
                                    UInt_t iSmpl = (strip) % GetNSamples();
                                    GetNoisyChipChannels()[iCr][iCh][iSmpl] = kTRUE;
                                }
                        }
                    }
                }
            }
        }
    }
    // repeat noisy channels back into the electronics terms
    for (Int_t iCr = 0; iCr < GetNSerials(); ++iCr) {
        auto it = fOuterMap.find(GetSerials()[iCr]);
        if (it == fOuterMap.end())
            continue;
        for (Int_t iCh = 0; iCh < GetNChannels(); ++iCh) {
            InChanMapSil & inner = it->second;
            auto innerIt = inner.lower_bound(iCh);
            if (innerIt == inner.end())
                continue;
            BmnSiliconMapping* rec = innerIt->second;
            if (!rec)
                continue;
            for (Short_t iSmpl = 0; iSmpl < GetNSamples(); ++iSmpl) {
                    Int_t iStrip = MapStrip(rec, iCh, iSmpl);
                if (fNoisyChannels[rec->station][rec->module][rec->layer][iStrip] == kTRUE)
                    GetNoisyChipChannels()[iCr][iCh][iSmpl] = kTRUE;
            }
        }
    }
    return kBMNSUCCESS;
}

void BmnSiliconRaw2Digit::ProcessAdc(TClonesArray *silicon, Bool_t doFill) {
    cmodcut = 100;
    Double_t FinalThr = thrMax - (niter - 1) * thrDif;
    for (Int_t iCr = 0; iCr < GetNSerials(); ++iCr) {
        auto it = fOuterMap.find(GetSerials()[iCr]);
        if (it == fOuterMap.end())
            continue;
        for (Int_t iCh = 0; iCh < GetNChannels(); ++iCh) {
            InChanMapSil & inner = it->second;
            auto innerIt = inner.lower_bound(iCh);
            if (innerIt == inner.end())
                continue;
            BmnSiliconMapping* rec = innerIt->second;
            if (!rec)
                continue;
            //            for (auto &it : fMap)
            //                if (GetSerials()[iCr] == it.serial && iCh >= it.channel_low && iCh <= it.channel_high) {
            Short_t station = rec->station;
            Short_t module = rec->module;
            Short_t layer = rec->layer;
            for (Short_t iSmpl = 0; iSmpl < GetNSamples(); ++iSmpl) {
                if ((GetNoisyChipChannels()[iCr][iCh][iSmpl] == kTRUE)/* || (fPedVal[iCr][iCh][iSmpl] == 0.0)*/) continue;
                //                        Int_t strip = (iCh - it.channel_low) * GetNSamples() + iSmpl;
                Int_t strip = MapStrip(rec, iCh, iSmpl);
                Double_t sig = fAdc[iCr][iCh][iSmpl] - fPedVal[iCr][iCh][iSmpl] + fCMode[iCr][iCh] - fSMode[iCr][iCh];
                if (layer == 1)
                    sig = -sig;
                Double_t Asig = TMath::Abs(sig);
                Double_t thr = Max(FinalThr, 4 * GetPedestalsRMS()[iCr][iCh][iSmpl]);
                //                        if (layer && !doFill)
                //                            printf("%s signal %f thr %6f  prms %6f\n", it.inverted ? "inverted" : "normal", sig, thr, GetPedestalsRMS()[iCr][iCh][iSmpl]);
                if (sig > thr) {//[station][module][layer][strip] == kFALSE)) {
                    if (doFill) {
                        if (Abs(fCMode[iCr][iCh] - fSMode[iCr][iCh]) < cmodcut)
                            fSigProf[station][module][layer]->Fill(strip);
                    } else {
                        BmnSiliconDigit * resDig =
                                new((*silicon)[silicon->GetEntriesFast()])
                                BmnSiliconDigit(station, module, layer, strip, sig);
                        if ((Abs(fCMode[iCr][iCh] - fSMode[iCr][iCh]) > cmodcut))
                            resDig->SetIsGoodDigit(kFALSE);
                        else
                            resDig->SetIsGoodDigit(kTRUE);
                    }
                }
                //                    }
                //                    break;
            }
        }
    }
}

ClassImp(BmnSiliconRaw2Digit)
