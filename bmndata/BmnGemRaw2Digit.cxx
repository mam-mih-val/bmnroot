#include "BmnGemRaw2Digit.h"

//list of GEM's serial id
const UChar_t kNSER = 10;
const UInt_t kNCH = 2048;
const UInt_t kSERIALS[kNSER] = {0x76CBA8B, 0x76CD410, 0x76C8320, 0x76CB9C0, 0x76CA266, 0x76D08B9, 0x76C8321, 0x76CE3EE, 0x76CE3E5, 0x4E983C1};

BmnGemRaw2Digit::BmnGemRaw2Digit(TString mappingFile) {
    fMapFileName = TString(getenv("VMCWORKDIR")) + TString("/input/") + mappingFile;
    FillMaps();
}

BmnStatus BmnGemRaw2Digit::FillMaps() {

    //========== read mapping file into vector ==========//
    fMapFile.open((fMapFileName).Data());
    if (!fMapFile.is_open()) {
        cout << "Error opening map-file (" << fMapFileName << ")!" << endl;
        return kBMNERROR;
    }

    TString dummy;
    UInt_t ser, ch_l, ch_h, gId, stId, adc_l, adc_h;
    Bool_t hotZ = 0;

    fMapFile >> dummy;
    fMapFile >> dummy >> dummy >> dummy >> dummy >> dummy >> dummy >> dummy >> dummy;
    fMapFile >> dummy;
    while (!fMapFile.eof()) {
        fMapFile >> hex >> ser >> dec >> ch_l >> ch_h >> gId >> stId >> adc_l >> adc_h >> hotZ;
        if (!fMapFile.good()) break;
        BmnGemMapping record;
        record.serial = ser;
        record.gemId = gId;
        record.station = stId;
        record.adcChLo = adc_l;
        record.adcChHi = adc_h;
        record.gemChLo = ch_l;
        record.gemChHi = ch_h;
        record.hotZone = hotZ;
        fMap.push_back(record);
    }
    fMapFile.close();
    //==================================================//

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
    //==================================================//    

    //====== read strip-channel maps into vector =======//
    TString path = TString(getenv("VMCWORKDIR")) + TString("/input/");
    ReadAndPut(path + TString("GEM_X_small.txt"), X_small);
    ReadAndPut(path + TString("GEM_Y_small.txt"), Y_small);
    ReadAndPut(path + TString("GEM_X0_middle.txt"), X0_mid);
    ReadAndPut(path + TString("GEM_Y0_middle.txt"), Y0_mid);
    ReadAndPut(path + TString("GEM_X1_middle.txt"), X1_mid);
    ReadAndPut(path + TString("GEM_Y1_middle.txt"), Y1_mid);
    ReadAndPut(path + TString("GEM_X0_Big_Left.txt"), X0_big_l);
    ReadAndPut(path + TString("GEM_X0_Big_Right.txt"), X0_big_r);
    ReadAndPut(path + TString("GEM_X1_Big_Left.txt"), X1_big_l);
    ReadAndPut(path + TString("GEM_X1_Big_Right.txt"), X1_big_r);
    ReadAndPut(path + TString("GEM_Y0_Big_Left.txt"), Y0_big_l);
    ReadAndPut(path + TString("GEM_Y0_Big_Right.txt"), Y0_big_r);
    ReadAndPut(path + TString("GEM_Y1_Big_Left.txt"), Y1_big_l);
    ReadAndPut(path + TString("GEM_Y1_Big_Right.txt"), Y1_big_r);
    //==================================================//

    return kBMNSUCCESS;
}

void BmnGemRaw2Digit::ReadAndPut(TString fName, map<UInt_t, UInt_t>& chMap) {
    UInt_t ch = 0;
    UInt_t strip = 0;
    ifstream inFile(fName.Data());
    if (!inFile.is_open()) {
        cout << "Error opening map-file (" << fName << ")!" << endl;
    }
    while (!inFile.eof()) {
        inFile >> ch;
        if (!inFile.good()) break;
        strip++;
        chMap.insert(pair<UInt_t, UInt_t>(ch, strip));
    }
}

BmnStatus BmnGemRaw2Digit::FillEvent(TClonesArray *adc, TClonesArray *gem) {

    for (Int_t iAdc = 0; iAdc < adc->GetEntriesFast(); ++iAdc) {
        BmnADC32Digit* adcDig = (BmnADC32Digit*) adc->At(iAdc);
        for (Int_t iMap = 0; iMap < fMap.size(); ++iMap) {
            BmnGemMapping gemM = fMap[iMap];
            UInt_t ch = adcDig->GetChannel();
            if (adcDig->GetSerial() == gemM.serial && ch <= gemM.adcChHi && ch >= gemM.adcChLo) {
                ProcessDigit(adcDig, &gemM, gem);
            }
        }
    }
}

void BmnGemRaw2Digit::ProcessDigit(BmnADC32Digit* adcDig, BmnGemMapping* gemM, TClonesArray *gem) {
    const UInt_t nSmpl = 32;
    UInt_t ch = adcDig->GetChannel();

    BmnGemStripDigit candDig[nSmpl];

    for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
        Int_t strip = -1;
        Int_t lay = -1;
        Int_t mod = -1;
        Int_t ped = -1;
        Int_t noise = -1;

        switch (gemM->gemId) {
            case 0: //small gem
            {
                UInt_t realChannel = ch * nSmpl + iSmpl - gemM->gemChLo;
                if (SearchInMap(&X_small, strip, realChannel) == kBMNSUCCESS) {
                    mod = 0;
                    lay = 0;
                    ped = SearchPed(ch * nSmpl + iSmpl, gemM->serial);
                    noise = SearchNoise(ch * nSmpl + iSmpl, gemM->serial);
                    break;
                }
                if (SearchInMap(&Y_small, strip, realChannel) == kBMNSUCCESS) {
                    mod = 0;
                    lay = 1;
                    ped = SearchPed(ch * nSmpl + iSmpl, gemM->serial);
                    noise = SearchNoise(ch * nSmpl + iSmpl, gemM->serial);
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
                if (gemM->gemChHi - gemM->gemChLo < 128) realChannel += 2048;
                if (gemM->hotZone) {
                    if (SearchInMap(&X0_big_l, strip, realChannel) == kBMNSUCCESS) {
                        mod = 2;
                        lay = 0;
                        ped = SearchPed(ch2048, gemM->serial);
                        noise = SearchNoise(ch2048, gemM->serial);
                        break;
                    }
                    if (SearchInMap(&Y0_big_l, strip, realChannel) == kBMNSUCCESS) {
                        mod = 2;
                        lay = 1;
                        ped = SearchPed(ch2048, gemM->serial);
                        noise = SearchNoise(ch2048, gemM->serial);
                        break;
                    }
                } else {
                    if (SearchInMap(&X1_big_l, strip, realChannel) == kBMNSUCCESS) {
                        mod = 0;
                        lay = 0;
                        ped = SearchPed(ch2048, gemM->serial);
                        noise = SearchNoise(ch2048, gemM->serial);
                        break;
                    }

                    if (SearchInMap(&Y1_big_l, strip, realChannel) == kBMNSUCCESS) {
                        mod = 0;
                        lay = 1;
                        ped = SearchPed(ch2048, gemM->serial);
                        noise = SearchNoise(ch2048, gemM->serial);
                        break;
                    }
                    break;
                }
            }
            case 7: //right big gem
            {
                UInt_t ch2048 = ch * nSmpl + iSmpl;
                //in one GEM we have 2176 channels, but in adc only 2048
                //so we use additional slot and we have to check is it an additional slot or not...
                //if additional, then channel number will be more than 2048
                UInt_t realChannel = ch2048;
                if (gemM->gemChHi - gemM->gemChLo < 128) realChannel += 2048;
                if (gemM->hotZone) {
                    if (SearchInMap(&X0_big_r, strip, realChannel) == kBMNSUCCESS) {
                        mod = 3;
                        lay = 0;
                        ped = SearchPed(ch2048, gemM->serial);
                        noise = SearchNoise(ch2048, gemM->serial);
                        break;
                    }
                    if (SearchInMap(&Y0_big_r, strip, realChannel) == kBMNSUCCESS) {
                        mod = 3;
                        lay = 1;
                        ped = SearchPed(ch2048, gemM->serial);
                        noise = SearchNoise(ch2048, gemM->serial);
                        break;
                    }
                } else {
                    if (SearchInMap(&X1_big_r, strip, realChannel) == kBMNSUCCESS) {
                        mod = 1;
                        lay = 0;
                        ped = SearchPed(ch2048, gemM->serial);
                        noise = SearchNoise(ch2048, gemM->serial);
                        break;
                    }
                    if (SearchInMap(&Y1_big_r, strip, realChannel) == kBMNSUCCESS) {
                        mod = 1;
                        lay = 1;
                        ped = SearchPed(ch2048, gemM->serial);
                        noise = SearchNoise(ch2048, gemM->serial);
                        break;
                    }
                    break;
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
                if (gemM->gemChHi - gemM->gemChLo < 128) realChannel += 2048;
                if (SearchInMap(&X1_mid, strip, realChannel) == kBMNSUCCESS) {
                    mod = 0;
                    lay = 0;
                    ped = SearchPed(ch2048, gemM->serial);
                    noise = SearchNoise(ch2048, gemM->serial);
                    break;
                }
                if (SearchInMap(&Y1_mid, strip, realChannel) == kBMNSUCCESS) {
                    mod = 0;
                    lay = 1;
                    ped = SearchPed(ch2048, gemM->serial);
                    noise = SearchNoise(ch2048, gemM->serial);
                    break;
                }
                if (SearchInMap(&X0_mid, strip, realChannel) == kBMNSUCCESS) {
                    mod = 1;
                    lay = 0;
                    ped = SearchPed(ch2048, gemM->serial);
                    noise = SearchNoise(ch2048, gemM->serial);
                    break;
                }
                if (SearchInMap(&Y0_mid, strip, realChannel) == kBMNSUCCESS) {
                    mod = 1;
                    lay = 1;
                    ped = SearchPed(ch2048, gemM->serial);
                    noise = SearchNoise(ch2048, gemM->serial);
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

BmnStatus BmnGemRaw2Digit::SearchInMap(map<UInt_t, UInt_t>* m, Int_t& strip, UInt_t ch) {
    map<UInt_t, UInt_t>::iterator it = m->find(ch);
    if (it != m->end()) {
        strip = it->second;
        return kBMNSUCCESS;
    }
    return kBMNERROR;
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

