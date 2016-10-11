#include "BmnGemRaw2Digit.h"

BmnGemRaw2Digit::BmnGemRaw2Digit() {
}

BmnGemRaw2Digit::BmnGemRaw2Digit(Int_t period, Int_t run) {

    fPeriod = period;
    fRun = run;

    fEntriesInGlobMap = 17;

    cout << "Loading the GEM Map from DB: Period " << period << ", Run " << run << "..." << endl;

    fSmall = new BmnGemMap[N_CH_IN_SMALL_GEM];
    fMid = new BmnGemMap[N_CH_IN_MID_GEM];
    fBigL0 = new BmnGemMap[N_CH_IN_BIG_GEM_0];
    fBigL1 = new BmnGemMap[N_CH_IN_BIG_GEM_1];
    fBigR0 = new BmnGemMap[N_CH_IN_BIG_GEM_0];
    fBigR1 = new BmnGemMap[N_CH_IN_BIG_GEM_1];
    fCrates = new UInt_t[fEntriesInGlobMap];
    for (Int_t iCr = 0; iCr < fEntriesInGlobMap; ++iCr) {
        fCrates[iCr] = 0;
    }

    UniDbDetectorParameter* mapPar = UniDbDetectorParameter::GetDetectorParameter("GEM", "GEM_global_mapping", fPeriod, fRun);
    if (mapPar != NULL) mapPar->GetGemMapArray(fMap, fEntriesInGlobMap);

    fNCrates = 0;
    //read and store crates numbers
    for (Int_t i = 0; i < fEntriesInGlobMap; ++i) {
        UInt_t ser = fMap[i].serial;
        for (Int_t iCr = 0; iCr < fEntriesInGlobMap; ++iCr) {
            if (fCrates[iCr] == ser) break;
            if (fCrates[iCr] == 0) {
                fCrates[iCr] = ser;
                fNCrates++;
                break;
            }
        }
    }

    ReadMap("GEM_X_small", "GEM_N_ch_X_small", fSmall, 0, 0);
    ReadMap("GEM_Y_small", "GEM_N_ch_Y_small", fSmall, 1, 0);
    //    UniDbDetectorParameter* cPar = UniDbDetectorParameter::GetDetectorParameter("GEM", "GEM_N_ch_X_small", fPeriod, fRun);
    //    Int_t size = (cPar != NULL) ? cPar->GetInt() : 0;
    //    for (Int_t i = 0; i < size; ++i)
    //        cout << fSmall[i].strip << " " << i << endl;


    ReadMap("GEM_X0_middle", "GEM_N_ch_X0_middle", fMid, 2, 0);
    ReadMap("GEM_Y0_middle", "GEM_N_ch_Y0_middle", fMid, 3, 0);
    ReadMap("GEM_X1_middle", "GEM_N_ch_X1_middle", fMid, 0, 0);
    ReadMap("GEM_Y1_middle", "GEM_N_ch_Y1_middle", fMid, 1, 0);

    ReadMap("GEM_X0_Big_Left", "GEM_N_ch_X0_big_l", fBigL0, 2, 0);
    ReadMap("GEM_Y0_Big_Left", "GEM_N_ch_Y0_big_l", fBigL0, 3, 0);

    ReadMap("GEM_X1_Big_Left", "GEM_N_ch_X1_big_l", fBigL1, 0, 0);
    ReadMap("GEM_Y1_Big_Left", "GEM_N_ch_Y1_big_l", fBigL1, 1, 0);

    ReadMap("GEM_X0_Big_Right", "GEM_N_ch_X0_big_r", fBigR0, 2, 1);
    ReadMap("GEM_Y0_Big_Right", "GEM_N_ch_Y0_big_r", fBigR0, 3, 1);

    ReadMap("GEM_X1_Big_Right", "GEM_N_ch_X1_big_r", fBigR1, 0, 1);
    ReadMap("GEM_Y1_Big_Right", "GEM_N_ch_Y1_big_r", fBigR1, 1, 1);

    fPedArr = new BmnGemPed* [fNCrates];
    for (Int_t i = 0; i < fNCrates; ++i)
        fPedArr[i] = new BmnGemPed[N_CH_IN_CRATE];

    GemPedestalStructure* pedMap;
    UniDbDetectorParameter* pedSizePar = UniDbDetectorParameter::GetDetectorParameter("GEM", "GEM_size_ped_map", fPeriod, fRun);
    Int_t sizePedMap = (pedSizePar != NULL) ? pedSizePar->GetInt() : 0;

    //    UniDbDetectorParameter* pedPar = UniDbDetectorParameter::GetDetectorParameter("GEM", "GEM_pedestal", fPeriod, fRun);
    //    if (pedPar != NULL) pedPar->GetGemPedestalArray(pedMap, sizePedMap);
    //
    //    for (Int_t i = 0; i < sizePedMap; ++i)
    //        for (Int_t iCr = 0; iCr < fNCrates; ++iCr)
    //            if (pedMap[i].serial == fCrates[iCr])
    //                fPedArr[iCr][pedMap[i].channel] = BmnGemPed(pedMap[i].pedestal, pedMap[i].noise);

    UInt_t ser = 0;
    Int_t ch = 0;
    Double_t ped = 0;
    Double_t rms = 0;
    TString dummy;
    TString path = TString(getenv("VMCWORKDIR")) + TString("/input/GEM_pedestals.txt");

    ifstream inFile(path.Data());
    if (!inFile.is_open())
        cout << "Error opening map-file (" << path << ")!" << endl;
    inFile >> dummy >> dummy >> dummy >> dummy;
    inFile >> dummy;
    Int_t i = 0;
    while (!inFile.eof()) {
        inFile >> hex >> ser >> dec >> ch >> ped >> rms;
        if (!inFile.good()) break;
        for (Int_t iCr = 0; iCr < fNCrates; ++iCr)
            if (ser == fCrates[iCr])
                fPedArr[iCr][ch] = BmnGemPed(ped, rms);
    }

}

BmnStatus BmnGemRaw2Digit::ReadMap(TString parName, TString parNameSize, BmnGemMap* m, Int_t lay, Int_t mod) {

    UniDbDetectorParameter* cPar = UniDbDetectorParameter::GetDetectorParameter("GEM", parNameSize, fPeriod, fRun);
    Int_t size = (cPar != NULL) ? cPar->GetInt() : 0;
    if (size == 0) return kBMNERROR;
    UniDbDetectorParameter* par = UniDbDetectorParameter::GetDetectorParameter("GEM", parName, fPeriod, fRun);
    IIStructure* iiArr;
    if (par != NULL) par->GetIIArray(iiArr, size);
    for (Int_t i = 0; i < size; ++i)
        m[iiArr[i].int_2] = BmnGemMap(iiArr[i].int_1, lay, mod);
}

BmnGemRaw2Digit::~BmnGemRaw2Digit() {
    //    delete [] fPedArr;
    //    delete fCrates;
    //    delete fMid;
    //    delete fBigR;
    //    delete fBigL;
    //    delete fSmall;
}

BmnStatus BmnGemRaw2Digit::FillEvent(TClonesArray *adc, TClonesArray *gem) {
    for (Int_t iAdc = 0; iAdc < adc->GetEntriesFast(); ++iAdc) {
        BmnADC32Digit* adcDig = (BmnADC32Digit*) adc->At(iAdc);
        UInt_t ch = adcDig->GetChannel() * ADC_N_SAMPLES;
        for (Int_t iMap = 0; iMap < fEntriesInGlobMap; ++iMap) {
            GemMapStructure gemM = fMap[iMap];
//            if (gemM.station != 0) continue;
            if (adcDig->GetSerial() == gemM.serial && ch <= gemM.channel_high && ch >= gemM.channel_low) {
                ProcessDigit(adcDig, &gemM, gem);
                break;
            }
        }
    }
}

void BmnGemRaw2Digit::ProcessDigit(BmnADC32Digit* adcDig, GemMapStructure* gemM, TClonesArray *gem) {
    const UInt_t nSmpl = ADC_N_SAMPLES;
    UInt_t ch = adcDig->GetChannel();

    BmnGemStripDigit candDig[nSmpl];
    Double_t pedestals[nSmpl];
    Double_t pedNoises[nSmpl];

    for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
        Int_t strip = -1;
        Int_t lay = -1;
        Int_t mod = -1;
        Int_t ch2048 = ch * nSmpl + iSmpl;
        UInt_t realChannel = 0;
        for (Int_t i = 0; i < fNCrates; ++i) {
            if (fCrates[i] == gemM->serial) {
                pedestals[iSmpl] = fPedArr[i][ch2048].ped;
                pedNoises[iSmpl] = fPedArr[i][ch2048].noise;
                break;
            }
        }

        switch (gemM->id) {
            case 0: //small gem
            {
                realChannel = ch2048 - gemM->channel_low;
                mod = fSmall[realChannel].mod;
                lay = fSmall[realChannel].lay;
                strip = fSmall[realChannel].strip;
                break;
            }
            case 6: //left big gem
            {
                realChannel = ch2048;
                if (gemM->hotZone == 1) {
                    mod = fBigL0[realChannel].mod;
                    lay = fBigL0[realChannel].lay;
                    strip = fBigL0[realChannel].strip;
                } else {
                    if (gemM->channel_high - gemM->channel_low < 128) realChannel = (2048 + ch2048 - gemM->channel_low);
                    mod = fBigL1[realChannel].mod;
                    lay = fBigL1[realChannel].lay;
                    strip = fBigL1[realChannel].strip;
                }
                break;
            }
            case 7: //right big gem
            {
                realChannel = ch2048;
                if (gemM->hotZone == 1) {
                    mod = fBigR0[realChannel].mod;
                    lay = fBigR0[realChannel].lay;
                    strip = fBigR0[realChannel].strip;
                } else {
                    if (gemM->channel_high - gemM->channel_low < 128) realChannel = (2048 + ch2048 - gemM->channel_low);
                    mod = fBigR1[realChannel].mod;
                    lay = fBigR1[realChannel].lay;
                    strip = fBigR1[realChannel].strip;
                }
                break;
            }
            default://middle gem's
            {
                realChannel = ch2048;
                if ((gemM->channel_high - gemM->channel_low) < 128) realChannel = (2048 + ch2048 - gemM->channel_low);
                mod = fMid[realChannel].mod;
                lay = fMid[realChannel].lay;
                strip = fMid[realChannel].strip;
                break;
            }
        }
        if (strip > 0) {
            BmnGemStripDigit dig;
            dig.SetStation(gemM->station);
            dig.SetModule(mod);
            dig.SetStripLayer(lay);
            dig.SetStripNumber(strip);
            dig.SetStripSignal(Double_t((adcDig->GetValue())[iSmpl] / 16));
            dig.SetStripSignalNoise(pedNoises[iSmpl]);
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

    for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
        if ((candDig[iSmpl]).GetStation() == -1) continue;
        BmnGemStripDigit * dig = &candDig[iSmpl];
        Double_t sig = dig->GetStripSignal() - CMS - pedestals[iSmpl];
        Float_t threshold = (dig->GetStation() == 0) ? 100 : 5.0 * pedNoises[iSmpl];
        if (sig < threshold) continue;
        if (IsStripNoisy(dig->GetStation(), dig->GetStripLayer(), dig->GetModule(), dig->GetStripNumber())) continue;
        if (dig->GetModule() < 0 || dig->GetModule() >= N_MODULES) continue;
        if (dig->GetStripLayer() < 0 || dig->GetStripLayer() >= N_LAYERS) continue;
        new((*gem)[gem->GetEntriesFast()]) BmnGemStripDigit(dig->GetStation(), dig->GetModule(), dig->GetStripLayer(), dig->GetStripNumber(), sig, dig->GetStripSignalNoise());
    }

}

BmnStatus BmnGemRaw2Digit::CalcGemPedestals(TClonesArray *adc, TTree *tree) {
    ofstream pedFile(Form("%s/input/GEM_pedestals.txt", getenv("VMCWORKDIR")));
    pedFile << "Serial\tCh_id\tPed\tRMS" << endl;
    pedFile << "============================================" << endl;
    const UShort_t nSmpl = ADC_N_SAMPLES;
    const UInt_t nDigs = 640; //fNCrates * 64; // 10 ADC x 64 ch

    Double_t** pedestals = new Double_t* [nDigs];
    Double_t** noises = new Double_t* [nDigs];
    for (Int_t i = 0; i < nDigs; ++i) {
        pedestals[i] = new Double_t[nSmpl];
        noises[i] = new Double_t[nSmpl];
    }

    UInt_t nEv = tree->GetEntries();
    for (Int_t iEv = 0; iEv < nEv; ++iEv) {
        if (iEv % 100 == 0) cout << "Pedestals calculation: read event #" << iEv << endl;
        tree->GetEntry(iEv);
        for (Int_t iAdc = 0; iAdc < nDigs; ++iAdc) {
            BmnADC32Digit* adcDig = (BmnADC32Digit*) adc->At(iAdc);
            Double_t signals[nSmpl];
            Int_t nOk = 0;
            for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
                if ((adcDig->GetValue())[iSmpl] / 16 == 0) continue;
                signals[iSmpl] = (adcDig->GetValue())[iSmpl] / 16;
                nOk++;
            }
            Double_t CMS = CalcCMS(signals, nOk);
            for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl)
                pedestals[iAdc][iSmpl] += (signals[iSmpl] - CMS);
        }
    }

    for (Int_t iAdc = 0; iAdc < nDigs; ++iAdc)
        for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl)
            pedestals[iAdc][iSmpl] /= nEv;


    for (Int_t iEv = 0; iEv < nEv; ++iEv) {
        if (iEv % 100 == 0) cout << "RMS calculation: read event #" << iEv << endl;
        tree->GetEntry(iEv);
        for (Int_t iAdc = 0; iAdc < nDigs; ++iAdc) {
            BmnADC32Digit* adcDig = (BmnADC32Digit*) adc->At(iAdc);
            Double_t signals[nSmpl];
            Int_t nOk = 0;
            for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
                if ((adcDig->GetValue())[iSmpl] / 16 == 0) continue;
                signals[iSmpl] = (adcDig->GetValue())[iSmpl] / 16;
                nOk++;
            }
            Double_t CMS = CalcCMS(signals, nSmpl);
            for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl)
                noises[iAdc][iSmpl] += (((signals[iSmpl] - CMS) - pedestals[iAdc][iSmpl]) * ((signals[iSmpl] - CMS) - pedestals[iAdc][iSmpl]));
        }
    }

    for (Int_t iAdc = 0; iAdc < nDigs; ++iAdc) {
        BmnADC32Digit* adcDig = (BmnADC32Digit*) adc->At(iAdc);
        for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
            noises[iAdc][iSmpl] = Sqrt(noises[iAdc][iSmpl] / nEv);
            if (noises[iAdc][iSmpl])
                pedFile << hex << adcDig->GetSerial() << dec << "\t" << adcDig->GetChannel() * nSmpl + iSmpl << "\t" << pedestals[iAdc][iSmpl] << "\t" << noises[iAdc][iSmpl] << endl;
        }
    }

    pedFile.close();
}

Double_t BmnGemRaw2Digit::CalcCMS(Double_t* samples, Int_t size) {

    const UShort_t kNITER = 4;
    Double_t CMS = 0.0;
    UInt_t nStr = size;
    Double_t upd[size];
    for (Int_t iSmpl = 0; iSmpl < size; ++iSmpl)
        upd[iSmpl] = samples[iSmpl];

    for (Int_t itr = 0; itr < kNITER; ++itr) {
        if (nStr == 0) break;
        Double_t cms = 0.0; //common mode shift
        for (Int_t iSmpl = 0; iSmpl < nStr; ++iSmpl)
            cms += upd[iSmpl];
        cms /= nStr;
        Double_t rms = 0.0; //chip noise
        for (Int_t iSmpl = 0; iSmpl < nStr; ++iSmpl)
            rms += (upd[iSmpl] - cms) * (upd[iSmpl] - cms);
        rms = Sqrt(rms / nStr);

        UInt_t nOk = 0;
        for (Int_t iSmpl = 0; iSmpl < nStr; ++iSmpl)
            if (Abs(upd[iSmpl] - cms) < 3 * rms)
                upd[nOk++] = upd[iSmpl];
        nStr = nOk;
        CMS = cms;
    }
    return CMS;
}

Bool_t BmnGemRaw2Digit::IsStripNoisy(Int_t station, Int_t lay, Int_t mod, Int_t strip) {
    //killing noisy strips...
    if (station == 1 && mod == 0 && lay == 0)
        if (strip == 398 || strip == 487 || strip == 488 || strip == 825)
            return kTRUE;

    if (station == 1 && mod == 0 && lay == 1)
        if (strip == 523 || strip == 525 || strip == 651 || strip == 653 || strip == 693 || strip == 698)
            return kTRUE;

    if (station == 2 && mod == 0 && lay == 0)
        if (strip == 6 || strip == 16 || strip == 183 || strip == 238 || strip == 240 || strip == 509 || strip == 647 || strip == 662 || strip == 666 || strip == 691 || strip == 693 || strip == 695 || strip == 696 || strip == 698 || strip == 744 || strip == 745 || strip == 746 || strip == 747 || strip == 751 || strip == 777 || strip == 825)
            return kTRUE;

    if (station == 2 && mod == 0 && lay == 1)
        if (strip == 354 || strip == 517 || strip == 643 || strip == 648 || strip == 652 || strip == 653 || strip == 695 || strip == 698)
            return kTRUE;

    if (station == 3 && mod == 0 && lay == 0)
        if (strip == 825)
            return kTRUE;

    if (station == 3 && mod == 0 && lay == 1)
        if (strip == 750)
            return kTRUE;

    if (station == 4 && mod == 0 && lay == 0)
        if (strip == 124 || strip == 311 || strip == 395 || strip == 401 || (strip >= 737 && strip <= 769) || strip == 825)
            return kTRUE;

    if (station == 4 && mod == 0 && lay == 1)
        if (strip == 384 || strip == 389)
            return kTRUE;

    if (station == 5 && mod == 0 && lay == 1)
        if (strip == 523 || strip == 532 || strip == 654 || strip == 705 || strip == 723 || strip == 825)
            return kTRUE;
    //killing noisy strips...

    return kFALSE;
}

ClassImp(BmnGemRaw2Digit)
