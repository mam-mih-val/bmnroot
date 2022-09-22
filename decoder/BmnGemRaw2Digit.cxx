#include "BmnGemRaw2Digit.h"
#include "BmnEventHeader.h"
#include <UniRun.h>

BmnGemRaw2Digit::BmnGemRaw2Digit() {
    fSmall = nullptr;
    fMid = nullptr;
}

BmnGemRaw2Digit::BmnGemRaw2Digit(Int_t period, Int_t run, vector<UInt_t> vSer, TString mapFileName, BmnSetup bmnSetup, BmnADCDecoMode decoMode) : BmnAdcProcessor(period, run, "GEM", ADC_N_CHANNELS, ADC32_N_SAMPLES, vSer) {

    fSetup = bmnSetup;
    fSmall = nullptr;
    fMid = nullptr;

    cout << "Loading GEM Map: Period " << period << ", Run " << run << "..." << endl;

    fEventId = 0;

    ReadGlobalMap(mapFileName);

    fSmall = new BmnGemMap[N_CH_BUF];
    fMid = new BmnGemMap[N_CH_BUF];
    Byte_t nMods = 4;
    for (Byte_t i = 0; i < nMods; i++) {
        fBigHot.push_back(new BmnGemMap[N_CH_BUF]);
        fBig.push_back(new BmnGemMap[N_CH_BUF]);
    }

    if ((fPeriod >= 8) && fSetup != kSRCSETUP) {
        ReadLocalMap("X0_Bottom_Left.txt", fBigHot[2], 2, 2);
        ReadLocalMap("X0_Bottom_Right.txt", fBigHot[3], 2, 3);
        ReadLocalMap("X_Bottom_Left.txt", fBig[2], 0, 2);
        ReadLocalMap("X_Bottom_Right.txt", fBig[3], 0, 3);
        ReadLocalMap("Y0_Bottom_Left.txt", fBigHot[2], 3, 2);
        ReadLocalMap("Y0_Bottom_Right.txt", fBigHot[3], 3, 3);
        ReadLocalMap("Y_Bottom_Left.txt", fBig[2], 1, 2);
        ReadLocalMap("Y_Bottom_Right.txt", fBig[3], 1, 3);

        ReadLocalMap("X0_Top_Left.txt", fBigHot[0], 2, 0);
        ReadLocalMap("X0_Top_Right.txt", fBigHot[1], 2, 1);
        ReadLocalMap("X_Top_Left.txt", fBig[0], 0, 0);
        ReadLocalMap("X_Top_Right.txt", fBig[1], 0, 1);
        ReadLocalMap("Y0_Top_Left.txt", fBigHot[0], 3, 0);
        ReadLocalMap("Y0_Top_Right.txt", fBigHot[1], 3, 1);
        ReadLocalMap("Y_Top_Left.txt", fBig[0], 1, 0);
        ReadLocalMap("Y_Top_Right.txt", fBig[1], 1, 1);
    } else {
        ReadMap("GEM_X_small", fSmall, 0, 0);
        ReadMap("GEM_Y_small", fSmall, 1, 0);

        ReadMap("GEM_X0_middle", fMid, 2, 0);
        ReadMap("GEM_Y0_middle", fMid, 3, 0);
        ReadMap("GEM_X1_middle", fMid, 0, 0);
        ReadMap("GEM_Y1_middle", fMid, 1, 0);

        ReadMap("GEM_X0_Big_Left", fBigHot[1], 2, 1);
        ReadMap("GEM_Y0_Big_Left", fBigHot[1], 3, 1);
        ReadMap("GEM_X1_Big_Left", fBig[1], 0, 1);
        ReadMap("GEM_Y1_Big_Left", fBig[1], 1, 1);
        ReadMap("GEM_X0_Big_Right", fBigHot[0], 2, 0);
        ReadMap("GEM_Y0_Big_Right", fBigHot[0], 3, 0);
        ReadMap("GEM_X1_Big_Right", fBig[0], 0, 0);
        ReadMap("GEM_Y1_Big_Right", fBig[0], 1, 0);

        // ReadLocalMap("GEM2_X0_Left.txt", fBigHot[1], 2, 1);
        // ReadLocalMap("GEM2_Y0_Left.txt", fBigHot[1], 3, 1);

        // ReadLocalMap("GEM2_X1_Left.txt", fBig[1], 0, 1);
        // ReadLocalMap("GEM2_Y1_Left.txt", fBig[1], 1, 1);

        // ReadLocalMap("GEM2_X0_Right.txt", fBigHot[0], 2, 0);
        // ReadLocalMap("GEM2_Y0_Right.txt", fBigHot[0], 3, 0);

        // ReadLocalMap("GEM2_X1_Right.txt", fBig[0], 0, 0);
        // ReadLocalMap("GEM2_Y1_Right.txt", fBig[0], 1, 0);
    }

    if (decoMode == kBMNADCSM) {
        //        fGemStationSetDer = new BmnGemStripStationSet(period, fSetup);
        fGemStationSetDer = BmnAdcProcessor::GetGemStationSet(period, fSetup);
        //                    printf("fGemStationSetDer %08X\n", fGemStationSetDer);
        //                    printf("\tstations %d\n", fGemStationSetDer->GetNStations());

        Int_t kNStations = fGemStationSetDer->GetNStations();
        fSigProf = new TH1F * **[kNStations];
        fNoisyChannels = new Bool_t * **[kNStations];
        for (Int_t iSt = 0; iSt < kNStations; ++iSt) {
            auto* st = fGemStationSetDer->GetStation(iSt);
            Int_t nModules = st->GetNModules();
            fSigProf[iSt] = new TH1F * *[nModules];
            fNoisyChannels[iSt] = new Bool_t * *[nModules];
            for (UInt_t iMod = 0; iMod < nModules; ++iMod) {
                auto* mod = st->GetModule(iMod);
                Int_t nLayers = mod->GetNStripLayers();
                fSigProf[iSt][iMod] = new TH1F * [nLayers];
                fNoisyChannels[iSt][iMod] = new Bool_t * [nLayers];
                for (Int_t iLay = 0; iLay < nLayers; ++iLay) {
                    auto& lay = mod->GetStripLayer(iLay);
                    Int_t nStrips = lay.GetNStrips();
                    TString histName;
                    histName.Form("GEM_%d_%d_%d", iSt, iMod, iLay);
                    fSigProf[iSt][iMod][iLay] = new TH1F(histName, histName, nStrips, 0, nStrips);
                    fSigProf[iSt][iMod][iLay]->SetDirectory(0);
                    fNoisyChannels[iSt][iMod][iLay] = new Bool_t[nStrips];
                    for (Int_t iStrip = 0; iStrip < nStrips; ++iStrip)
                        fNoisyChannels[iSt][iMod][iLay][iStrip] = kFALSE;
                }
            }
        }

        switch (GetPeriod()) {
        case 8:
            thrMax = 30;
            thrDif = 10;
            niter = 3;
            break;
        case 7:
            thrMax = 40;
            thrDif = 10;
            niter = 3;
            break;
        case 6:
            thrMax = 40;
            thrDif = 10;
            niter = 3;
            break;
        default:
            fprintf(stderr, "Unsupported period %d !\n", GetPeriod());
            break;
        }
    }
}

BmnStatus BmnGemRaw2Digit::ReadGlobalMap(TString FileName) {
    string dummy;
    UInt_t id = 0;
    UInt_t ser = 0;
    UInt_t ch_lo = 0;
    UInt_t ch_hi = 0;
    UInt_t station = 0;
    UInt_t mod = 0;
    UInt_t zone = 0;
    UInt_t side = 0;
    UInt_t type = 0;
    TString name = TString(getenv("VMCWORKDIR")) + TString("/input/") + FileName;
    printf("%s\n", name.Data());
    ifstream inFile(name.Data());
    if (!inFile.is_open()) {
        cout << "Error opening map-file (" << name << ")!" << endl;
        return kBMNERROR;
    }
    for (Int_t i = 0; i < 8; ++i) getline(inFile, dummy); //comment line in input file

    while (!inFile.eof()) {
        inFile >> std::hex >> ser >> std::dec >> ch_lo >> ch_hi >> id >> side >> type >> station >> mod >> zone;
        if (!inFile.good()) break;
        GemMapLine* record = new GemMapLine();
        record->Ch_hi = ch_hi / GetNSamples();
        record->Serial = ser;
        record->Ch_lo = ch_lo / GetNSamples();
        record->Station = station;
        record->Zone = zone;
        record->GEM_id = id;
        record->Side = side;
        record->Type = type;
        record->Module = mod;
        fMap.push_back(record);
    }
    fEntriesInGlobMap = fMap.size();
    return kBMNSUCCESS;
}

BmnStatus BmnGemRaw2Digit::ReadLocalMap(TString FileName, BmnGemMap* m, Int_t lay, Int_t mod) {
    TString name = TString(getenv("VMCWORKDIR")) + TString("/input/") + FileName;
    printf("%s\n", name.Data());
    ifstream inFile(name.Data());
    if (!inFile.is_open()) {
        cout << "Error opening map-file (" << name << ")!" << endl;
        return kBMNERROR;
    }
    Int_t iStrip = 0;
    Int_t chan = 0;
    while (!inFile.eof()) {
        inFile >> chan;
        if (!inFile.good()) break;
        m[chan] = BmnGemMap(iStrip++, lay, mod);
        //        printf("\t\t strip %4d  lay %4d mod %4d  chan %4d\n", iStrip, lay, mod, chan);
    }
    return kBMNSUCCESS;
}

BmnStatus BmnGemRaw2Digit::ReadMap(TString parName, BmnGemMap* m, Int_t lay, Int_t mod) {
    Int_t size = 0;
    UniDetectorParameter* par = UniDetectorParameter::GetDetectorParameter("GEM", parName, GetPeriod(), GetRun());
    vector<UniValue*> iiArr;
    if (par != NULL) par->GetValue(iiArr);
    delete par;
    //    printf("%20s  mod %d lay %d\n", parName.Data(), mod, lay);
    for (Int_t i = 0; i < iiArr.size(); ++i) {
        IIValue* pValue = (IIValue*)iiArr[i];
        m[pValue->value2] = BmnGemMap(pValue->value1 - 1, lay, mod); // Strip begins from 0
        //        printf("\t\t strip %4d  lay %4d mod %4d  chan %4d %s\n", pValue->value1, lay, mod, pValue->value2, (pValue->value1 == 0) ? "WFT!!!" : "");
    }

    if (!iiArr.empty()) for (int i = 0; i < iiArr.size(); i++) delete iiArr[i];
    return kBMNSUCCESS;
}

BmnGemRaw2Digit::~BmnGemRaw2Digit() {
    if (fSmall) delete[] fSmall;
    if (fMid) delete[] fMid;
    for (size_t i = 0; i < fBig.size(); i++) {
        delete[] fBigHot[i];
        delete[] fBig[i];
    }
 
    if (!fMap.empty()) for (int i = 0; i < fMap.size(); i++) delete fMap[i];

    Int_t kNStations = fGemStationSetDer->GetNStations();
    for (Int_t iSt = 0; iSt < kNStations; ++iSt) {
        auto* st = fGemStationSetDer->GetStation(iSt);
        for (UInt_t iMod = 0; iMod < st->GetNModules(); ++iMod) {
            auto* mod = st->GetModule(iMod);
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
    if (fGemStationSetDer) delete fGemStationSetDer;
}

BmnStatus BmnGemRaw2Digit::FillProfiles(TClonesArray* adc) {
    //    for (Int_t iAdc = 0; iAdc < adc->GetEntriesFast(); ++iAdc) {
    //        BmnADCDigit* adcDig = (BmnADCDigit*) adc->At(iAdc);
    //        UInt_t ch = adcDig->GetChannel() * adcDig->GetNSamples();
    //        for (Int_t iMap = 0; iMap < fEntriesInGlobMap; ++iMap) {
    //            GemMapLine* gemM = fMap[iMap];
    //            if (adcDig->GetSerial() == gemM->serial && ch <= gemM->channel_high && ch >= gemM->channel_low) {
    //                ProcessDigit(adcDig, gemM, NULL, kTRUE);
    //                break;
    //            }
    //        }
    //    }
    (this->*PrecalcEventModsImp)(adc);
#ifdef BUILD_DEBUG
    CalcEventMods();
#else
    CalcEventMods_simd();
#endif
    ProcessAdc(nullptr, kTRUE);
    return kBMNSUCCESS;
}

BmnStatus BmnGemRaw2Digit::FillNoisyChannels() {
    //    const Int_t kNStations = 10;
    //    const Int_t kNStrips = 1300;
    const Int_t kNStripsInBunch = fNSamples;
    //    const Int_t kNBunches = kNStrips / kNStripsInBunch;
    const Int_t kNThresh = 3;

    for (Int_t iCr = 0; iCr < GetNSerials(); ++iCr)
        for (Int_t iCh = 0; iCh < GetNChannels(); ++iCh)
            for (auto& it : fMap)
                if (GetSerials()[iCr] == it->Serial && iCh >= it->Ch_lo && iCh <= it->Ch_hi) {
                    for (Int_t iSmpl = 0; iSmpl < GetNSamples(); ++iSmpl)
                        if (GetNoisyChipChannels()[iCr][iCh][iSmpl] == kTRUE) {
                            Int_t station = -1;
                            Int_t strip = -1;
                            Int_t layer = -1;
                            Int_t mod = -1;
                            MapStrip(it, iCh, iSmpl, station, mod, layer, strip);
                            if (strip < 0)
                                continue;
                            fNoisyChannels[station][mod][layer][strip] = kTRUE;
                        }
                }
    for (Int_t iSt = 0; iSt < fGemStationSetDer->GetNStations(); ++iSt) {
        auto* st = fGemStationSetDer->GetStation(iSt);
        for (UInt_t iMod = 0; iMod < st->GetNModules(); ++iMod) {
            auto* mod = st->GetModule(iMod);
            for (Int_t iLay = 0; iLay < mod->GetNStripLayers(); ++iLay) {
                TH1F* prof = fSigProf[iSt][iMod][iLay];
                auto& lay = mod->GetStripLayer(iLay);
                Int_t kNBunches = lay.GetNStrips() / kNStripsInBunch;
                for (Int_t iBunch = 0; iBunch < kNBunches; ++iBunch) {
                    Double_t mean = 0.0;
                    for (Int_t iStrip = 0; iStrip < kNStripsInBunch; ++iStrip) {
                        Int_t strip = iStrip + iBunch * kNStripsInBunch;
                        if (fNoisyChannels[iSt][iMod][iLay][strip] == kTRUE)
                            continue;
                        Double_t curr = prof->GetBinContent(strip + 1);
                        Double_t prev = prof->GetBinContent(strip);
                        mean += curr;
                    }
                    mean /= kNStripsInBunch;
                    for (Int_t iStrip = 0; iStrip < kNStripsInBunch; ++iStrip) {
                        Int_t strip = iStrip + iBunch * kNStripsInBunch;
                        if (fNoisyChannels[iSt][iMod][iLay][strip] == kTRUE)
                            continue;
                        Double_t curr = prof->GetBinContent(strip + 1);
                        Double_t prev = prof->GetBinContent(strip);
                        //                        if (kNThresh * meanDiff < curr - prev)
                        if (kNThresh * mean < Abs(curr - mean))
                            fNoisyChannels[iSt][iMod][iLay][strip] = kTRUE;
                    }
                }
            }
        }
    }
    for (Int_t iCr = 0; iCr < GetNSerials(); ++iCr)
        for (Int_t iCh = 0; iCh < GetNChannels(); ++iCh)
            for (auto& it : fMap)
                if (GetSerials()[iCr] == it->Serial && iCh >= it->Ch_lo && iCh <= it->Ch_hi)
                    for (Int_t iSmpl = 0; iSmpl < GetNSamples(); ++iSmpl) {
                        Int_t station = -1;
                        Int_t strip = -1;
                        Int_t lay = -1;
                        Int_t mod = -1;
                        MapStrip(it, iCh, iSmpl, station, mod, lay, strip);
                        if (strip < 0)
                            continue;
                        if (fNoisyChannels[station][mod][lay][strip] == kTRUE)
                            GetNoisyChipChannels()[iCr][iCh][iSmpl] = kTRUE;
                    }

    return kBMNSUCCESS;
}

void BmnGemRaw2Digit::ProcessAdc(TClonesArray* gem, Bool_t doFill) {

    Double_t FinalThr = thrMax - (niter - 1) * thrDif;
   // cout << FinalThr << " " << fNSerials << " " << fNChannels << endl;
    
    for (Int_t iCr = 0; iCr < fNSerials; ++iCr) {
        for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
            for (auto& it : fMap) {
                //it->Print();
                if (GetSerials()[iCr] == it->Serial && iCh >= it->Ch_lo && iCh <= it->Ch_hi) {
                    for (Int_t iSmpl = 0; iSmpl < GetNSamples(); ++iSmpl) {
                        if (GetNoisyChipChannels()[iCr][iCh][iSmpl] == kTRUE) continue;

                        Int_t station = -1;
                        Int_t strip = -1;
                        Int_t layer = -1;
                        Int_t mod = -1;
                        MapStrip(it, iCh, iSmpl, station, mod, layer, strip);
                        //cout << "DECODER INFO: " <<  station << " " << mod << " " << layer << " " << strip << endl;
                        if (strip < 0)
                            continue;
                        //                        if (strip == 0)
                        //                            printf("iCr %d  serial %08X ch %4d\n\n", iCr, GetSerials()[iCr], iCh);
                        //                        if (Abs(fCMode[iCr][iCh] - fSMode[iCr][iCh]) > cmodcut)
                        //                            continue;

                        Double_t sig = fAdc[iCr][iCh][iSmpl] - fPedVal[iCr][iCh][iSmpl] + fCMode[iCr][iCh] - fSMode[iCr][iCh];
                        Double_t Asig = TMath::Abs(sig);
                        //                        printf("gem thrMax  %4.2f niter %d dthr %4.2f FinalThr %4.2f\n", thrMax, niter, thrDif, FinalThr);
                        Double_t thr = Max(FinalThr, 3.5 * GetPedestalsRMS()[iCr][iCh][iSmpl]);
                        //                        printf("signal %6.2f  thr %6f  prms %6f\n", sig, thr, GetPedestalsRMS()[iCr][iCh][iSmpl]);
                        if (sig > thr) {//[station][mod][layer][strip] == kFALSE)) {
                            if (doFill/* && (Abs(fCMode[iCr][iCh] - fSMode[iCr][iCh]) < cmodcut)*/) {
                                fSigProf[station][mod][layer]->Fill(strip);
                            } else {
                                BmnGemStripDigit* resDig =
                                    new((*gem)[gem->GetEntriesFast()])
                                    BmnGemStripDigit(station, mod, layer, strip, sig);
                                //                                if ((Abs(fCMode[iCr][iCh] - fSMode[iCr][iCh]) > cmodcut))
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
}

BmnStatus BmnGemRaw2Digit::FillEvent(TClonesArray* adc, TClonesArray* gem) {
    fEventId++;
    //    for (Int_t iAdc = 0; iAdc < adc->GetEntriesFast(); ++iAdc) {
    //        BmnADCDigit* adcDig = (BmnADCDigit*) adc->At(iAdc);
    //        UInt_t ch = adcDig->GetChannel() * adcDig->GetNSamples();
    //        for (Int_t iMap = 0; iMap < fEntriesInGlobMap; ++iMap) {
    //            GemMapLine* gemM = fMap[iMap];
    //            if (adcDig->GetSerial() == gemM->serial && ch <= gemM->channel_high && ch >= gemM->channel_low) {
    //                ProcessDigit(adcDig, gemM, gem, kFALSE);
    //                break;
    //            }
    //        }
    //    }
    (this->*PrecalcEventModsImp)(adc);
#ifdef BUILD_DEBUG
    CalcEventMods();
#else
    CalcEventMods_simd();
#endif
    ProcessAdc(gem, kFALSE);
    return kBMNSUCCESS;
}

inline void BmnGemRaw2Digit::MapStrip(GemMapLine* gemM, UInt_t ch, Int_t iSmpl, Int_t& station, Int_t& mod, Int_t& lay, Int_t& strip) {
    Int_t ch2048 = ch * GetNSamples() + iSmpl;
    UInt_t realChannel = ch2048;
    BmnGemMap* fBigMap = nullptr;
    mod = gemM->Module;
    UInt_t side = gemM->Side;
    if (gemM->GEM_id < 5) {
        if ((gemM->Ch_hi - gemM->Ch_lo) < 128 / GetNSamples()) realChannel = (2048 + ch2048 - gemM->Ch_lo * GetNSamples());
        fBigMap = fMid;
    } else {
        if (gemM->Zone == 0) { //hot zone
            Int_t chShift = (gemM->Ch_lo == 0) ? 0 : -1024;
            if(fPeriod < 8)
                chShift+=1024;
            //For 6-7 run
            //Int_t chShift = (gemM->Ch_lo == 0) ? 1024 : 0;
            realChannel += chShift;
            fBigMap = fBigHot[side];
        } else { //big zone
            fBigMap = fBig[side];
            if (gemM->Ch_hi - gemM->Ch_lo < 128 / GetNSamples()) {
                realChannel = 2048 + ch2048 - gemM->Ch_lo * GetNSamples();
            }
        }
    }
    station = gemM->Station;
    lay = fBigMap[realChannel].lay;
    strip = fBigMap[realChannel].strip;
    return;
}

void BmnGemRaw2Digit::ProcessDigit(BmnADCDigit* adcDig, GemMapLine* gemM, TClonesArray* gem, Bool_t doFill) {
    const UInt_t nSmpl = adcDig->GetNSamples();
    UInt_t ch = adcDig->GetChannel();
    UInt_t ser = adcDig->GetSerial();

    Int_t iSer = -1;
    for (iSer = 0; iSer < GetSerials().size(); ++iSer)
        if (ser == GetSerials()[iSer]) break;
    if (iSer == GetSerials().size())
        return; // serial not found

    BmnGemStripDigit candDig[nSmpl];

    for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
        Int_t station = -1;
        Int_t strip = -1;
        Int_t lay = -1;
        Int_t mod = -1;
        MapStrip(gemM, ch, iSmpl, station, mod, lay, strip);
        if (strip < 0)
            continue;

        if (strip > 0) {
            BmnGemStripDigit dig;
            dig.SetStation(station);
            dig.SetModule(mod);
            dig.SetStripLayer(lay);
            dig.SetStripNumber(strip);
            Double_t sig = (GetRun() > GetBoundaryRun(ADC32_N_SAMPLES)) ? ((Double_t)((adcDig->GetShortValue())[iSmpl] / 16)) : ((Double_t)((adcDig->GetUShortValue())[iSmpl] / 16));
            dig.SetStripSignal(sig);
            candDig[iSmpl] = dig;
        }
    }

    Double_t signals[nSmpl];
    Int_t nOk = 0;
    for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
        if ((candDig[iSmpl]).GetStripSignal() == 0) continue;
        signals[iSmpl] = (candDig[iSmpl]).GetStripSignal();
        nOk++;
    }
    Double_t CMS = CalcCMS(signals, nOk);
    Double_t SCMS = CalcSCMS(signals, nSmpl, iSer, ch);

    Float_t*** vPed = GetPedestals();
    Double_t*** vPedRMS = GetPedestalsRMS();

    for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
        if ((candDig[iSmpl]).GetStation() == -1) continue;

        BmnGemStripDigit* dig = &candDig[iSmpl];
        Double_t ped = vPed[iSer][ch][iSmpl];
        //        Double_t sig = Abs(dig->GetStripSignal() - CMS - ped);
        Double_t sig = dig->GetStripSignal() - SCMS - ped;
        Float_t threshold = Max(20.0, 3.5 * vPedRMS[iSer][ch][iSmpl]); //15 + 4 * vPedRMS[iSer][ch][iSmpl]; //20;
        //        if (vPedRMS[iSer][ch][iSmpl] != 0)
        //            printf(" iSer %d, ch %d, iSmpl %d, vPedRMS %f\n", iSer, ch ,iSmpl, vPedRMS[iSer][ch][iSmpl]);
        //    printf("ProcessDigit candDig[iSmpl]).GetStation() = %i sig = %f threshold = %f\n", candDig[iSmpl].GetStation(), sig, threshold);
        if (sig < threshold || sig == 0.0) continue; //FIXME: check cases with sig == 0
        if (doFill) {
            fSigProf[dig->GetStation()][dig->GetModule()][dig->GetStripLayer()]->Fill(dig->GetStripNumber());
        } else {
            BmnGemStripDigit* resDig = new((*gem)[gem->GetEntriesFast()]) BmnGemStripDigit(dig->GetStation(), dig->GetModule(), dig->GetStripLayer(), dig->GetStripNumber(), sig);
            //            printf("st %d  mod %d lay %d strip %d\n",dig->GetStation(), dig->GetModule(), dig->GetStripLayer(), dig->GetStripNumber() );
            if (fNoisyChannels[dig->GetStation()][dig->GetModule()][dig->GetStripLayer()][dig->GetStripNumber()])
                resDig->SetIsGoodDigit(kFALSE);
            else
                resDig->SetIsGoodDigit(kTRUE);
        }
    }
}

ClassImp(BmnGemRaw2Digit)

