#include "BmnGemRaw2Digit.h"

BmnGemRaw2Digit::BmnGemRaw2Digit(Int_t period, Int_t run) {

    fPeriod = period;
    fRun = run;

    fEntriesInGlobMap = 17;
    fEntriesInPedMap = 20482;

    fNchXsmall = 256;
    fNchYsmall = 256;
    fNchX0mid = 190;
    fNchY0mid = 215;
    fNchX1mid = 825;
    fNchY1mid = 930;
    fNchX0big_l = 500;
    fNchX0big_r = 500;
    fNchX1big_l = 1019;
    fNchX1big_r = 1019;
    fNchY0big_l = 488;
    fNchY0big_r = 506;
    fNchY1big_l = 1081;
    fNchY1big_r = 1130;

    cout << "Loading the GEM Map from DB: Period " << period << ", Run " << run << "..." << endl;

    fSmall = new BmnGemMap[N_CH_IN_SMALL_GEM];
    fMid = new BmnGemMap[N_CH_IN_MID_GEM];
    fBigL = new BmnGemMap[N_CH_IN_BIG_GEM];
    fBigR = new BmnGemMap[N_CH_IN_BIG_GEM];
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

    ReadMap("GEM_X_small", fSmall, fNchXsmall, 0, 0);
    ReadMap("GEM_Y_small", fSmall, fNchYsmall, 1, 0);

    ReadMap("GEM_X0_middle", fMid, fNchX0mid, 0, 1);
    ReadMap("GEM_Y0_middle", fMid, fNchY0mid, 1, 1);
    ReadMap("GEM_X1_middle", fMid, fNchX1mid, 0, 0);
    ReadMap("GEM_Y1_middle", fMid, fNchY1mid, 1, 0);

    ReadMap("GEM_X0_Big_Left", fBigL, fNchX0big_l, 0, 3);
    ReadMap("GEM_Y0_Big_Left", fBigL, fNchY0big_l, 1, 3);
    ReadMap("GEM_X1_Big_Left", fBigL, fNchX1big_l, 0, 1);
    ReadMap("GEM_Y1_Big_Left", fBigL, fNchY1big_l, 1, 1);

    ReadMap("GEM_X0_Big_Right", fBigR, fNchX0big_r, 0, 2);
    ReadMap("GEM_Y0_Big_Right", fBigR, fNchY0big_r, 1, 2);
    ReadMap("GEM_X1_Big_Right", fBigR, fNchX1big_r, 0, 0);
    ReadMap("GEM_Y1_Big_Right", fBigR, fNchY1big_r, 1, 0);

    fPedArr = new BmnGemPed* [fNCrates];
    for (Int_t i = 0; i < fNCrates; ++i)
        fPedArr[i] = new BmnGemPed[N_CH_IN_CRATE];

    UniDbDetectorParameter* pedPar = UniDbDetectorParameter::GetDetectorParameter("GEM", "GEM_pedestal", fPeriod, fRun);
    if (pedPar != NULL) pedPar->GetGemPedestalArray(fPed, fEntriesInPedMap);

    for (Int_t i = 0; i < fEntriesInPedMap; ++i)
        for (Int_t iCr = 0; iCr < fNCrates; ++iCr)
            if (fPed[i].serial == fCrates[iCr])
                fPedArr[iCr][fPed[i].channel] = BmnGemPed(fPed[i].pedestal, fPed[i].noise);
}

BmnStatus BmnGemRaw2Digit::ReadMap(TString parName, BmnGemMap* m, Int_t size, Int_t lay, Int_t mod) {
    UniDbDetectorParameter* par = UniDbDetectorParameter::GetDetectorParameter("GEM", parName, fPeriod, fRun);
    IIStructure* iiArr;
    if (par != NULL) par->GetIIArray(iiArr, size);

    for (Int_t i = 0; i < size; ++i)
        m[iiArr[i].int_2] = BmnGemMap(iiArr[i].int_1, lay, mod);
    delete iiArr;
}

BmnGemRaw2Digit::~BmnGemRaw2Digit() {

    delete fMid;
    delete fBigR;
    delete fBigL;
    delete fSmall;
}

BmnStatus BmnGemRaw2Digit::FillEvent(TClonesArray *adc, TClonesArray *gem) {
    for (Int_t iAdc = 0; iAdc < adc->GetEntriesFast(); ++iAdc) {
        BmnADC32Digit* adcDig = (BmnADC32Digit*) adc->At(iAdc);
        for (Int_t iMap = 0; iMap < fEntriesInGlobMap; ++iMap) {
            GemMapStructure gemM = fMap[iMap];
            UInt_t ch = adcDig->GetChannel();

            if (adcDig->GetSerial() == gemM.serial && ch <= (gemM.channel_high / ADC_N_SAMPLES) && ch >= (gemM.channel_low / ADC_N_SAMPLES))
                ProcessDigit(adcDig, &gemM, gem);
        }
    }
}

void BmnGemRaw2Digit::ProcessDigit(BmnADC32Digit* adcDig, GemMapStructure* gemM, TClonesArray *gem) {
    const UInt_t nSmpl = ADC_N_SAMPLES;
    UInt_t ch = adcDig->GetChannel();

    BmnGemStripDigit candDig[nSmpl];

    for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
        Int_t strip = -1;
        Int_t lay = -1;
        Int_t mod = -1;
        Int_t ped = -1;
        Int_t noise = -1;
        Int_t ch2048 = ch * nSmpl + iSmpl;
        for (Int_t i = 0; i < fNCrates; ++i) {
            if (fCrates[i] == gemM->serial) {
                ped = fPedArr[i][ch2048].ped;
                noise = fPedArr[i][ch2048].noise;
                break;
            }
        }

        switch (gemM->id) {
            case 0: //small gem
            {
                UInt_t realChannel = ch2048 - gemM->channel_low;
                mod = fSmall[realChannel].mod;
                lay = fSmall[realChannel].lay;
                strip = fSmall[realChannel].strip;
                break;
            }
            case 6: //left big gem
            {
                //in one GEM we have 2176 channels, but in adc only 2048
                //so we use additional slot and we have to check is it an additional slot or not...
                //if additional, then channel number will be more than 2048
                UInt_t realChannel = ch2048;
                if (gemM->channel_high - gemM->channel_low < 128) realChannel += 2048;
                mod = fBigL[realChannel].mod;
                lay = fBigL[realChannel].lay;
                strip = fBigL[realChannel].strip;
                break;
            }
            case 7: //right big gem
            {
                //in one GEM we have 2176 channels, but in adc only 2048
                //so we use additional slot and we have to check is it an additional slot or not...
                //if additional, then channel number will be more than 2048
                UInt_t realChannel = ch2048;
                if (gemM->channel_high - gemM->channel_low < 128) realChannel += 2048;
                mod = fBigR[realChannel].mod;
                lay = fBigR[realChannel].lay;
                strip = fBigR[realChannel].strip;
                break;
            }
            default://middle gem's
            {
                //in ADC we have 64 channels with 32 samples, in FEE we have 2048 channels 
                //so ch2048 is just number of current sample in current adc-channel 
                //in the range [0..2048].
                //in one GEM we have 2176 channels, but in adc only 2048
                //so we use additional slot and we have to check is it an additional slot or not...
                //if additional, then channel number will be more than 2048
                UInt_t realChannel = ch2048;
                if (gemM->channel_high - gemM->channel_low < 128) realChannel += 128; //2048;
                mod = fMid[realChannel].mod;
                lay = fMid[realChannel].lay;
                strip = fMid[realChannel].strip;

            }
        }
        if (strip != -1) {
            Double_t sig = Abs(Double_t((adcDig->GetValue())[iSmpl] / 16 - ped));
            BmnGemStripDigit dig;
            dig.SetStation(gemM->station);
            dig.SetModule(mod);
            dig.SetStripLayer(lay);
            dig.SetStripNumber(strip);
            dig.SetStripSignal(sig);
            dig.SetStripSignalNoise(noise);
            candDig[iSmpl] = dig;
        }
    }

    const UShort_t kNITER = 4;

    UInt_t nStr = nSmpl;
    Double_t CMS = 0.0; //common mode shift

    BmnGemStripDigit updDig[nSmpl];
    for (Int_t iSmpl = 0; iSmpl < nStr; ++iSmpl)
        updDig[iSmpl] = candDig[iSmpl];

    for (Int_t itr = 0; itr < kNITER; ++itr) {
        if (nStr == 0) continue;
        Double_t cms = 0.0; //common mode shift
        for (Int_t iSmpl = 0; iSmpl < nStr; ++iSmpl)
            cms += Abs(Double_t((updDig[iSmpl]).GetStripSignal()));
        cms /= nStr;
        Float_t chNoise = 0.0; //chip noise
        for (Int_t iSmpl = 0; iSmpl < nStr; ++iSmpl)
            chNoise += ((Abs(Double_t((updDig[iSmpl]).GetStripSignal())) - cms) * (Abs(Double_t((updDig[iSmpl]).GetStripSignal())) - cms));
        chNoise = Sqrt(chNoise / nStr);
        UInt_t nOk = 0;
        for (Int_t iSmpl = 0; iSmpl < nStr; ++iSmpl)
            //if (Abs(Double_t((updDig[iSmpl]).GetStripSignal())) > 0.001 && Abs(Double_t((updDig[iSmpl]).GetStripSignal())) < 3 * chNoise) {
            if (Abs(Double_t((updDig[iSmpl]).GetStripSignal())) > 0.001) {
                updDig[nOk] = updDig[iSmpl];
                nOk++;
            }
        nStr = nOk;
        CMS = cms;
    }

    const Double_t kTHRESH = 15.0;

    for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
        if ((candDig[iSmpl]).GetStation() == -1) continue;
        BmnGemStripDigit * dig = &candDig[iSmpl];
        Double_t sig = Abs(dig->GetStripSignal()) - CMS;

        if (sig < kTHRESH) continue;
        TClonesArray & ar_gem = *gem;
        new(ar_gem[gem->GetEntriesFast()]) BmnGemStripDigit(dig->GetStation(), dig->GetModule(), dig->GetStripLayer(), dig->GetStripNumber(), dig->GetStripSignal(), dig->GetStripSignalNoise());
    }

}

BmnStatus BmnGemRaw2Digit::CalcGemPedestals(TClonesArray *adc, TTree *tree) {
    ofstream pedFile(Form("%s/input/GEM_pedestals.txt", getenv("VMCWORKDIR")));
    pedFile << "Serial\tCh_id\tPed\tComMode\tNoise" << endl;
    pedFile << "=====================================" << endl;
    const UShort_t nSmpl = 32;
    const UInt_t nDigs = fNCrates * 64; // 10 ADC x 64 ch
    Double_t pedestals[nDigs][nSmpl] = {};
    Double_t noises[nDigs][nSmpl] = {};
    Double_t comModes[nDigs] = {};

    UInt_t nEv = tree->GetEntries();
    for (Int_t iEv = 0; iEv < nEv; ++iEv) {
        if (iEv % 10 == 0) cout << "Read event, the first loop #" << iEv << endl;
        tree->GetEntry(iEv);
        for (Int_t iAdc = 0; iAdc < nDigs; ++iAdc) {
            BmnADC32Digit* adcDig = (BmnADC32Digit*) adc->At(iAdc);
            for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl)
                pedestals[iAdc][iSmpl] += ((adcDig->GetValue())[iSmpl] / 16);
        }
    }

    UInt_t comMode = 0;
    for (Int_t iAdc = 0; iAdc < nDigs; ++iAdc) {
        for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
            pedestals[iAdc][iSmpl] /= nEv;
            comMode += pedestals[iAdc][iSmpl];
        }
        comMode /= nSmpl;
        comModes[iAdc] = comMode;
    }

    for (Int_t iEv = 0; iEv < nEv; ++iEv) {
        if (iEv % 10 == 0) cout << "Read event, the second loop #" << iEv << endl;
        tree->GetEntry(iEv);
        for (Int_t iAdc = 0; iAdc < nDigs; ++iAdc) {
            BmnADC32Digit* adcDig = (BmnADC32Digit*) adc->At(iAdc);
            for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
                noises[iAdc][iSmpl] += (((adcDig->GetValue())[iSmpl] / 16) - comModes[iAdc]) * (((adcDig->GetValue())[iSmpl] / 16) - comModes[iAdc]);
            }
        }
    }

    for (Int_t iAdc = 0; iAdc < nDigs; ++iAdc) {
        BmnADC32Digit* adcDig = (BmnADC32Digit*) adc->At(iAdc);
        for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
            noises[iAdc][iSmpl] = Sqrt(noises[iAdc][iSmpl] / nEv);
            pedFile << hex << adcDig->GetSerial() << dec << "\t" << adcDig->GetChannel() * nSmpl + iSmpl << "\t" << Int_t(pedestals[iAdc][iSmpl]) << "\t" << Int_t(comModes[iAdc]) << "\t" << Int_t(noises[iAdc][iSmpl]) << endl;
        }
    }

    pedFile.close();
}

ClassImp(BmnGemRaw2Digit)
