#include "BmnGemRaw2Digit.h"

//list of GEM's serial id
//Int_t kNentries = 17; //number of entries in mapping
const UChar_t kNSER = 10;
const UInt_t kNCH = 2048;
const UInt_t kSERIALS[kNSER] = {0x76CBA8B, 0x76CD410, 0x76C8320, 0x76CB9C0, 0x76CA266, 0x76D08B9, 0x76C8321, 0x76CE3EE, 0x76CE3E5, 0x4E983C1};

BmnGemRaw2Digit::BmnGemRaw2Digit(Int_t period, Int_t run) {

    fEntriesInGlobMap = 17;
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

    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::GetDetectorParameter("GEM", "GEM_global_mapping", period, run);
    if (pDetectorParameter != NULL) pDetectorParameter->GetGemMapArray(fMap, fEntriesInGlobMap);

    UniDbDetectorParameter* pX_small = UniDbDetectorParameter::GetDetectorParameter("GEM", "GEM_X_small", period, run);
    if (pX_small != NULL) pX_small->GetIIArray(fX_small, fNchXsmall);
    UniDbDetectorParameter* pY_small = UniDbDetectorParameter::GetDetectorParameter("GEM", "GEM_Y_small", period, run);
    if (pY_small != NULL) pY_small->GetIIArray(fY_small, fNchYsmall);
    UniDbDetectorParameter* pX0_mid = UniDbDetectorParameter::GetDetectorParameter("GEM", "GEM_X0_middle", period, run);
    if (pX0_mid != NULL) pX0_mid->GetIIArray(fX0_mid, fNchX0mid);
    UniDbDetectorParameter* pY0_mid = UniDbDetectorParameter::GetDetectorParameter("GEM", "GEM_Y0_middle", period, run);
    if (pY0_mid != NULL) pY0_mid->GetIIArray(fY0_mid, fNchY0mid);
    UniDbDetectorParameter* pX1_mid = UniDbDetectorParameter::GetDetectorParameter("GEM", "GEM_X1_middle", period, run);
    if (pX1_mid != NULL) pX1_mid->GetIIArray(fX1_mid, fNchX1mid);
    UniDbDetectorParameter* pY1_mid = UniDbDetectorParameter::GetDetectorParameter("GEM", "GEM_Y1_middle", period, run);
    if (pY1_mid != NULL) pY1_mid->GetIIArray(fY1_mid, fNchY1mid);
    UniDbDetectorParameter* pX0_big_l = UniDbDetectorParameter::GetDetectorParameter("GEM", "GEM_X0_Big_Left", period, run);
    if (pX0_big_l != NULL) pX0_big_l->GetIIArray(fX0_big_l, fNchX0big_l);
    UniDbDetectorParameter* pX0_big_r = UniDbDetectorParameter::GetDetectorParameter("GEM", "GEM_X0_Big_Right", period, run);
    if (pX0_big_r != NULL) pX0_big_r->GetIIArray(fX0_big_r, fNchX0big_r);
    UniDbDetectorParameter* pX1_big_l = UniDbDetectorParameter::GetDetectorParameter("GEM", "GEM_X1_Big_Left", period, run);
    if (pX1_big_l != NULL) pX1_big_l->GetIIArray(fX1_big_l, fNchX1big_l);
    UniDbDetectorParameter* pX1_big_r = UniDbDetectorParameter::GetDetectorParameter("GEM", "GEM_X1_Big_Right", period, run);
    if (pX1_big_r != NULL) pX1_big_r->GetIIArray(fX1_big_r, fNchX1big_r);
    UniDbDetectorParameter* pY0_big_l = UniDbDetectorParameter::GetDetectorParameter("GEM", "GEM_Y0_Big_Left", period, run);
    if (pY0_big_l != NULL) pY0_big_l->GetIIArray(fY0_big_l, fNchY0big_l);
    UniDbDetectorParameter* pY0_big_r = UniDbDetectorParameter::GetDetectorParameter("GEM", "GEM_Y0_Big_Right", period, run);
    if (pY0_big_r != NULL) pY0_big_r->GetIIArray(fY0_big_r, fNchY0big_r);
    UniDbDetectorParameter* pY1_big_l = UniDbDetectorParameter::GetDetectorParameter("GEM", "GEM_Y1_Big_Left", period, run);
    if (pY1_big_l != NULL) pY1_big_l->GetIIArray(fY1_big_l, fNchY1big_l);
    UniDbDetectorParameter* pY1_big_r = UniDbDetectorParameter::GetDetectorParameter("GEM", "GEM_Y1_Big_Right", period, run);
    if (pY1_big_r != NULL) pY1_big_r->GetIIArray(fY1_big_r, fNchY1big_r);

    FillMaps();
}

BmnStatus BmnGemRaw2Digit::FillMaps() {

    TString dummy;
    UInt_t ser;

    //========= read pedestal file into vector =========//
    ifstream pedFile(Form("%s/input/GEM_pedestals.txt", getenv("VMCWORKDIR")));
    if (!pedFile.is_open()) {
        cout << "Error opening pedestal-file!" << endl;
        return kBMNERROR;
    }
    UInt_t ped, com, chan, noise;
    fPedMap = new UInt_t* [kNSER];
    fCMMap = new UInt_t* [kNSER];
    fNoiseMap = new UInt_t* [kNSER];
    for (Int_t i = 0; i < kNSER; ++i) {
        fPedMap[i] = new UInt_t[kNCH];
        fCMMap[i] = new UInt_t[kNCH];
        fNoiseMap[i] = new UInt_t[kNCH];
    }


    pedFile >> dummy >> dummy >> dummy >> dummy >> dummy;
    pedFile >> dummy;
    while (!pedFile.eof()) {
        pedFile >> hex >> ser >> dec >> chan >> ped >> com >> noise;
        if (!pedFile.good()) break;
        Int_t i = 0;
        for (i = 0; i < kNSER; ++i)
            if (ser == kSERIALS[i]) break;
        fPedMap[i][chan] = ped;
        fCMMap[i][chan] = com;
        fNoiseMap[i][chan] = noise;
    }
    pedFile.close();

    return kBMNSUCCESS;
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

        switch (gemM->id) {
            case 0: //small gem
            {
                UInt_t realChannel = ch * nSmpl + iSmpl - gemM->channel_low;
                ped = SearchPed(ch * nSmpl + iSmpl, gemM->serial);
                noise = SearchNoise(ch * nSmpl + iSmpl, gemM->serial);
                mod = 0;
                strip = SearchInMap(fX_small, fNchXsmall, realChannel);
                if (strip != -1) {
                    lay = 0;
                    break;
                }
                strip = SearchInMap(fY_small, fNchYsmall, realChannel);
                if (strip != -1) {
                    lay = 1;
                    break;
                }
            }
            case 6: //left big gem
            {
                UInt_t ch2048 = ch * nSmpl + iSmpl;
                //in one GEM we have 2176 channels, but in adc only 2048
                //so we use additional slot and we have to check is it an additional slot or not...
                //if additional, then channel number will be more than 2048
                UInt_t realChannel = ch2048;
                if (gemM->channel_high - gemM->channel_low < 128) realChannel += 2048;
                ped = SearchPed(ch2048, gemM->serial);
                noise = SearchNoise(ch2048, gemM->serial);
                if (gemM->hotZone) {
                    mod = 3;
                    strip = SearchInMap(fX0_big_l, fNchX0big_l, realChannel);
                    if (strip != -1) {
                        lay = 0;
                        break;
                    }
                    strip = SearchInMap(fY0_big_l, fNchY0big_l, realChannel);
                    if (strip != -1) {
                        lay = 1;
                        break;
                    }
                } else {
                    mod = 1;
                    strip = SearchInMap(fX1_big_l, fNchX1big_l, realChannel);
                    if (strip != -1) {
                        lay = 0;
                        break;
                    }
                    strip = SearchInMap(fY1_big_l, fNchY1big_l, realChannel);
                    if (strip != -1) {
                        lay = 1;
                        break;
                    }
                }
            }
            case 7: //right big gem
            {
                UInt_t ch2048 = ch * nSmpl + iSmpl;
                //in one GEM we have 2176 channels, but in adc only 2048
                //so we use additional slot and we have to check is it an additional slot or not...
                //if additional, then channel number will be more than 2048
                UInt_t realChannel = ch2048;
                if (gemM->channel_high - gemM->channel_low < 128) realChannel += 2048;
                ped = SearchPed(ch2048, gemM->serial);
                noise = SearchNoise(ch2048, gemM->serial);
                if (gemM->hotZone) {
                    mod = 2;
                    strip = SearchInMap(fX0_big_r, fNchX0big_r, realChannel);
                    if (strip != -1) {
                        lay = 0;
                        break;
                    }
                    strip = SearchInMap(fY0_big_r, fNchY0big_r, realChannel);
                    if (strip != -1) {
                        lay = 1;
                        break;
                    }
                } else {
                    mod = 0;
                    strip = SearchInMap(fX1_big_r, fNchX1big_r, realChannel);
                    if (strip != -1) {
                        lay = 0;
                        break;
                    }
                    strip = SearchInMap(fY1_big_r, fNchY1big_r, realChannel);
                    if (strip != -1) {
                        lay = 1;
                        break;
                    }
                }
            }
            default://middle gem's
            {
                //in ADC we have 64 channels with 32 samples, in FEE we have 2048 channels 
                //so ch2048 is just number of current sample in current adc-channel 
                //in the range [0..2048].
                UInt_t ch2048 = ch * nSmpl + iSmpl;
                //in one GEM we have 2176 channels, but in adc only 2048
                //so we use additional slot and we have to check is it an additional slot or not...
                //if additional, then channel number will be more than 2048
                UInt_t realChannel = ch2048;
                if (gemM->channel_high - gemM->channel_low < 128) realChannel += 2048;
                ped = SearchPed(ch2048, gemM->serial);
                noise = SearchNoise(ch2048, gemM->serial);
		strip = SearchInMap(fX1_mid, fNchX1mid, realChannel);
                if (strip != -1) {
                    mod = 0;
                    lay = 0;
                    break;
                }
                strip = SearchInMap(fY1_mid, fNchY1mid, realChannel);
                if (strip != -1) {
                    mod = 0;
                    lay = 1;
                    break;
                }
                strip = SearchInMap(fX0_mid, fNchX0mid, realChannel);
                if (strip != -1) {
                    mod = 1;
                    lay = 0;
                    break;
                }
                strip = SearchInMap(fY0_mid, fNchY0mid, realChannel);
                if (strip != -1) {
                    mod = 1;
                    lay = 1;
                    break;
                }
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
        Double_t cms = 0.0; //common mode shift
        for (Int_t iSmpl = 0; iSmpl < nStr; ++iSmpl)
            cms += Abs(Double_t((updDig[iSmpl]).GetStripSignal()));
        if (nStr == 0) continue;
        cms /= nStr;
        Float_t chNoise = 0.0; //chip noise
        for (Int_t iSmpl = 0; iSmpl < nStr; ++iSmpl)
            chNoise += ((Abs(Double_t((updDig[iSmpl]).GetStripSignal())) - cms) * (Abs(Double_t((updDig[iSmpl]).GetStripSignal())) - cms));
        chNoise = Sqrt(chNoise / nStr);
        UInt_t nOk = 0;
        for (Int_t iSmpl = 0; iSmpl < nStr; ++iSmpl)
            if (Abs(Double_t((updDig[iSmpl]).GetStripSignal())) < 3 * chNoise) {
                updDig[nOk] = updDig[iSmpl];
                nOk++;
            }
        nStr = nOk;
        CMS = cms;
    }

    const Double_t kTHRESH = 15.0;

    for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
        if ((candDig[iSmpl]).GetStation() == -1) continue;
        BmnGemStripDigit* dig = &candDig[iSmpl];
        Double_t sig = Abs(dig->GetStripSignal()) - CMS;
        if (sig < kTHRESH) continue;
        //        if (sig > 4 * CMSiSmpl) continue; //check it!!!
        TClonesArray& ar_gem = *gem;
        new(ar_gem[gem->GetEntriesFast()]) BmnGemStripDigit(dig->GetStation(), dig->GetModule(), dig->GetStripLayer(), dig->GetStripNumber(), dig->GetStripSignal(), dig->GetStripSignalNoise());
    }

}

Int_t BmnGemRaw2Digit::SearchInMap(IIStructure* m, Int_t size, UInt_t ch) {
    for (Int_t i = 0; i < size; ++i)
        if (m[i].int_2 == ch)
            return m[i].int_1;
    return -1;
}

UInt_t BmnGemRaw2Digit::SearchPed(UInt_t chn, UInt_t ser) {
    Int_t i = 0;
    for (i = 0; i < kNSER; ++i)
        if (ser == kSERIALS[i]) break;
    return fPedMap[i][chn];
}

UInt_t BmnGemRaw2Digit::SearchNoise(UInt_t chn, UInt_t ser) {
    Int_t i = 0;
    for (i = 0; i < kNSER; ++i)
        if (ser == kSERIALS[i]) break;
    return fNoiseMap[i][chn];
}

UInt_t BmnGemRaw2Digit::SearchComMod(UInt_t chn, UInt_t ser) {
    Int_t i = 0;
    for (i = 0; i < kNSER; ++i)
        if (ser == kSERIALS[i]) break;
    return fCMMap[i][chn];
}

BmnStatus BmnGemRaw2Digit::CalcGemPedestals(TClonesArray *adc, TTree *tree) {
    ofstream pedFile(Form("%s/input/GEM_pedestals.txt", getenv("VMCWORKDIR")));
    pedFile << "Serial\tCh_id\tPed\tComMode\tNoise" << endl;
    pedFile << "=====================================" << endl;
    const UShort_t nSmpl = 32;
    const UInt_t nDigs = kNSER * 64; // 10 ADC x 64 ch
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
