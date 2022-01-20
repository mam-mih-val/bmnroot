#include "BmnCscRaw2Digit.h"

BmnCscRaw2Digit::BmnCscRaw2Digit() {
    fEventId = -1;
    fMapFileName = "";
}

BmnCscRaw2Digit::BmnCscRaw2Digit(Int_t period, Int_t run, vector<UInt_t> vSer, TString MapFileName) : BmnAdcProcessor(period, run, "CSC", ADC_N_CHANNELS, ADC32_N_SAMPLES, vSer) {

    cout << "Loading CSC Map: Period " << period << ", Run " << run << "..." << endl;

    fEventId = 0;
    fMapFileName = MapFileName;
    ReadMapFile();
    ReadMapLocalFile();

    TString gPathConfig = getenv("VMCWORKDIR");
    TString xmlConfFileName;
    switch (period) {
        case 8:
            if (fSetup == kBMNSETUP) {
                xmlConfFileName = "CSCRun8.xml";
            } else {
                xmlConfFileName = "CSCRunSRC2021.xml";
            }
            break;
        case 7:
            if (fSetup == kBMNSETUP) {
                xmlConfFileName = "CSCRunSpring2018.xml";
            } else {
                xmlConfFileName = "CSCRunSRCSpring2018.xml";
            }
            break;
        default:
            printf("Error! Unknown config!\n");
            return;
            break;

    }
    TString gPathCscConfig = gPathConfig + "/parameters/csc/XMLConfigs/";
    fCscStationSet = new BmnCSCStationSet(gPathCscConfig + xmlConfFileName);

    Int_t kNStations = fCscStationSet->GetNStations();
    fSigProf = new TH1F***[kNStations];
    fNoisyChannels = new Bool_t***[kNStations];
    for (Int_t iSt = 0; iSt < kNStations; ++iSt) {
        auto * st = fCscStationSet->GetStation(iSt);
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
    for (Int_t iSt = 0; iSt < fCscStationSet->GetNStations(); ++iSt) {
        auto * st = fCscStationSet->GetStation(iSt);
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
    for (auto it : fMap) delete it;
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
        fMap.push_back(record);
    }

    return kBMNSUCCESS;
}

BmnStatus BmnCscRaw2Digit::ReadMapLocalFile() {
    Int_t modules = 2;
    Int_t layers = 4;

    for (Short_t iMod = 0; iMod < modules; ++iMod) {
        vector< vector<Int_t> > mVec;
        vector< vector<Int_t> > mChannel;
        for (Short_t iLay = 0; iLay < layers; ++iLay) {
            vector<Int_t> lVec(2048);
            std::fill(lVec.begin(), lVec.end(), -1);
            vector<Int_t> lChannel;
            TString name = TString(getenv("VMCWORKDIR")) + TString("/input/") + TString("CSC_m") + (Long_t) iMod + TString("l") + (Long_t) iLay + TString(".txt");
            ifstream inFile(name.Data());
            if (!inFile.is_open()) {
                printf(ANSI_COLOR_RED "\n[ERROR]" ANSI_COLOR_RESET);
                printf(" Error opening map-file (%s)", name.Data());
            }
            Int_t strip = 0;
            while (!inFile.eof()) {
                Int_t ch;
                inFile >> ch;
                lChannel.push_back(ch);
                lVec[ch] = strip;
                strip++;
            }
            mVec.push_back(lVec);
            mChannel.push_back(lChannel);
        }
        localMap.push_back(mVec);
        channelMap.push_back(mChannel);
    }

    return kBMNSUCCESS;
}

BmnStatus BmnCscRaw2Digit::FillEvent(TClonesArray *adc, TClonesArray * csc) {
    fEventId++;
    //    for (Int_t iAdc = 0; iAdc < adc->GetEntriesFast(); ++iAdc) {
    //        BmnADCDigit* adcDig = (BmnADCDigit*) adc->At(iAdc);
    //        ProcessDigit(adcDig, FindMapEntry(adcDig), csc, kFALSE);
    //    }
    PrecalcEventMods(adc);
    CalcEventMods();
    //    for (Int_t iAdc = 0; iAdc < adc->GetEntriesFast(); ++iAdc) {
    //        BmnADCDigit* adcDig = (BmnADCDigit*) adc->At(iAdc);
    //        ProcessDigit(adcDig, FindMapEntry(adcDig), csc, kFALSE);
    //    }
    ProcessAdc(csc, kFALSE);

    return kBMNSUCCESS;
}

BmnStatus BmnCscRaw2Digit::FillProfiles(TClonesArray *adc) {
    //    for (Int_t iAdc = 0; iAdc < adc->GetEntriesFast(); ++iAdc) {
    //        BmnADCDigit* adcDig = (BmnADCDigit*) adc->At(iAdc);
    //        ProcessDigit(adcDig, FindMapEntry(adcDig), NULL, kTRUE);
    //    }
    PrecalcEventMods(adc);
    CalcEventMods();
    //    for (Int_t iAdc = 0; iAdc < adc->GetEntriesFast(); ++iAdc) {
    //        BmnADCDigit* adcDig = (BmnADCDigit*) adc->At(iAdc);
    //        ProcessDigit(adcDig, FindMapEntry(adcDig), NULL, kTRUE);
    //    }
    ProcessAdc(nullptr, kTRUE);

    return kBMNSUCCESS;
}

BmnCscMapping* BmnCscRaw2Digit::FindMapEntry(BmnADCDigit* adcDig) {
    for (Int_t iMap = 0; iMap < fMap.size(); ++iMap) {
        BmnCscMapping* tM = fMap[iMap];
        UInt_t ch = adcDig->GetChannel();
        if (adcDig->GetSerial() == tM->serial && (ch >= tM->channel_low && ch <= tM->channel_high))
            return tM;
    }
    return NULL;
}

BmnStatus BmnCscRaw2Digit::FillNoisyChannels() {
//    const Int_t kNStations = 1;
//    const Int_t kNStrips = 640; //2048; //FIXME
    const Int_t kNStripsInBunch = GetNSamples();
//    const Int_t kNBunches = kNStrips / kNStripsInBunch;
    const Int_t kNThresh = 3;

    for (Int_t iCr = 0; iCr < GetNSerials(); ++iCr)
        for (Int_t iCh = 0; iCh < GetNChannels(); ++iCh)
            for (auto &it : fMap)
                if (GetSerials()[iCr] == it->serial && iCh >= it->channel_low && iCh <= it->channel_high) {
                    for (Int_t iSmpl = 0; iSmpl < GetNSamples(); ++iSmpl)
                        if (GetNoisyChipChannels()[iCr][iCh][iSmpl] == kTRUE) {
                            Int_t station = -1;
                            Int_t strip = -1;
                            Int_t layer = -1;
                            Int_t module = -1;
                            MapStrip(it, iCh, iSmpl, station, module, layer, strip);
                            if (strip < 0)
                                continue;
                            fNoisyChannels[station][module][layer][strip] = kTRUE;
                        }
                }
    for (Int_t iSt = 0; iSt < fCscStationSet->GetNStations(); ++iSt) {
        auto * st = fCscStationSet->GetStation(iSt);
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
        for (Int_t iCh = 0; iCh < GetNChannels(); ++iCh)
            for (auto &it : fMap)
                if (GetSerials()[iCr] == it->serial && iCh >= it->channel_low && iCh <= it->channel_high) {
                    for (Int_t iSmpl = 0; iSmpl < GetNSamples(); ++iSmpl) {
                        Int_t station = -1;
                        Int_t strip = -1;
                        Int_t layer = -1;
                        Int_t module = -1;
                        MapStrip(it, iCh, iSmpl, station, module, layer, strip);
                        if (strip < 0)
                            continue;
                        if (fNoisyChannels[station][module][layer][strip] == kTRUE)
                            GetNoisyChipChannels()[iCr][iCh][iSmpl] = kTRUE;
                    }
                }

    return kBMNSUCCESS;
}

inline Int_t BmnCscRaw2Digit::LayerPrediction(Int_t module, Int_t x) {
    Int_t layer = -1;
    bool found = false;
    for (Int_t j = 0; j < channelMap[module].size(); j++) {
        layer = j;
        auto result = std::find(channelMap[module][j].begin(), channelMap[module][j].end(), x);
        if (result != std::end(channelMap[module][j])) {
            found = true;
            break;
        }
    }
    if (found) return layer;
    else return -1;
}

inline void BmnCscRaw2Digit::MapStrip(BmnCscMapping* cscM, UInt_t iCh, Int_t iSmpl, Int_t &station, Int_t &module, Int_t &layer, Int_t &strip) {
    station = cscM->station;
    module = cscM->module;
    Int_t ch2048 = iCh * GetNSamples() + iSmpl;
    layer = LayerPrediction(module, ch2048);
    if (layer == -1) return;
    strip = localMap[module][layer][ch2048];
    return;
}

void BmnCscRaw2Digit::ProcessAdc(TClonesArray *csc, Bool_t doFill) {
    Double_t FinalThr = thrMax - (niter - 1) * thrDif;
    for (Int_t iCr = 0; iCr < fNSerials; ++iCr) {
        for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
            for (auto &it : fMap)
                if (GetSerials()[iCr] == it->serial && iCh >= it->channel_low && iCh <= it->channel_high) {
                    for (Int_t iSmpl = 0; iSmpl < GetNSamples(); ++iSmpl) {
                        if (GetNoisyChipChannels()[iCr][iCh][iSmpl] == kTRUE) continue;
                        Int_t station = -1;
                        Int_t strip = -1;
                        Int_t layer = -1;
                        Int_t module = -1;
                        MapStrip(it, iCh, iSmpl, station, module, layer, strip);
                        if (strip < 0)
                            continue;
                        Double_t sig = fAdc[iCr][iCh][iSmpl] - fPedVal[iCr][iCh][iSmpl] + fCMode[iCr][iCh] - fSMode[iCr][iCh];
                        Double_t Asig = TMath::Abs(sig);
                        Double_t thr = Max(FinalThr, 3.5 * GetPedestalsRMS()[iCr][iCh][iSmpl]);
                        if (Asig > thr) {
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
                    break;
                }
        }
    }
}
//
//void BmnCscRaw2Digit::ProcessDigit(BmnADCDigit* adcDig, BmnCscMapping* cscM, TClonesArray * csc, Bool_t doFill) {
//    if (!adcDig || !cscM) return;
//    const UInt_t nSmpl = adcDig->GetNSamples();
//    UInt_t ch = adcDig->GetChannel();
//    UInt_t ser = adcDig->GetSerial();
//
//    Int_t iSer = -1;
//    for (iSer = 0; iSer < GetSerials().size(); ++iSer)
//        if (ser == GetSerials()[iSer]) break;
//    if (iSer == GetSerials().size())
//        return; // serial not found
//    BmnCSCDigit candDig[nSmpl];
//
//    Short_t cscStation = cscM->station;
//    Short_t cscModule = cscM->module;
//    Int_t ch2048 = ch * nSmpl;
//    Int_t counter = -1;
//    for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
//        ch2048 = ch * nSmpl + iSmpl;
//        Short_t cscLayer = LayerPrediction(cscModule, ch2048);
//        if (cscLayer == -1) continue;
//        BmnCSCDigit dig;
//        if (localMap[cscModule][cscLayer][ch2048] == -1) continue;
//        else counter++;
//        dig.SetStripNumber(localMap[cscModule][cscLayer][ch2048]);
//        dig.SetStation(cscStation);
//        dig.SetModule(cscModule);
//        dig.SetStripLayer(cscLayer);
//        Double_t sig = (Double_t) ((adcDig->GetShortValue())[iSmpl] / 16);
//        dig.SetStripSignal(sig);
//        candDig[counter] = dig;
//    }
//    Double_t signals[counter + 1];
//    for (Int_t iSmpl = 0; iSmpl < counter; ++iSmpl) signals[iSmpl] = 0.0;
//    Int_t nOk = 0;
//    for (Int_t iSmpl = 0; iSmpl < counter; ++iSmpl) {
//        if ((candDig[iSmpl]).GetStripSignal() == 0 ||
//                fNoisyChannels[candDig[iSmpl].GetStation()][candDig[iSmpl].GetModule()][candDig[iSmpl].GetStripLayer()][candDig[iSmpl].GetStripNumber()] == kTRUE) continue;
//        signals[iSmpl] = (candDig[iSmpl]).GetStripSignal();
//        nOk++;
//    }
//    Double_t CMS = CalcCMS(signals, nOk);
//    Double_t SCMS = CalcSCMS(signals, counter, iSer, ch);
//    Double_t*** vPed = GetPedestals();
//    Double_t*** vPedRMS = GetPedestalsRMS();
//    for (Int_t iSmpl = 0; iSmpl < counter; ++iSmpl) {
//        if ((candDig[iSmpl]).GetStation() == -1) continue;
//        BmnCSCDigit * dig = &candDig[iSmpl];
//        Double_t ped = vPed[iSer][ch][iSmpl];
//        Double_t sig = dig->GetStripSignal() - CMS - ped; // + fCMode[iSer][ch] - fSMode[iSer][ch]);
//        Double_t Asig = Abs(sig);
//        //cout << "strip " << iSer << " CMS " << ch << " ped " << iSmpl << endl;
//        Float_t threshold = Max(50.0, 4 * vPedRMS[iSer][ch][iSmpl]); //50 + 4 * vPedRMS[iSer][ch][iSmpl]; //20;
//        //if(threshold != 0) cout << threshold << endl;
//        if (sig < threshold || Asig == 0.0) continue; //FIXME: check cases with sig == 0
//        if (doFill) {
//            fSigProf[dig->GetStation()][dig->GetModule()][dig->GetStripLayer()]->Fill(dig->GetStripNumber());
//        } else {
//            BmnCSCDigit * resDig = new((*csc)[csc->GetEntriesFast()]) BmnCSCDigit(dig->GetStation(), dig->GetModule(), dig->GetStripLayer(), dig->GetStripNumber(), sig);
//            if (fNoisyChannels[dig->GetStation()][dig->GetModule()][dig->GetStripLayer()][dig->GetStripNumber()]) {
//                resDig->SetIsGoodDigit(kFALSE);
//            } else {
//                resDig->SetIsGoodDigit(kTRUE);
//            }
//        }
//    }
//}

ClassImp(BmnCscRaw2Digit)

