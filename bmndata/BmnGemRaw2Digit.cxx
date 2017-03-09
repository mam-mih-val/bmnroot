#include "BmnGemRaw2Digit.h"
#include "TH1F.h"
#include "TCanvas.h"

BmnGemRaw2Digit::BmnGemRaw2Digit() {
}

BmnGemRaw2Digit::BmnGemRaw2Digit(Int_t period, Int_t run) {

    fPeriod = period;
    fRun = run;
    fEventId = 0;

    cout << "Loading the GEM Map from DB: Period " << period << ", Run " << run << "..." << endl;

    fSmall = new BmnGemMap[N_CH_BUF];
    fMid = new BmnGemMap[N_CH_BUF];
    fBigL0 = new BmnGemMap[N_CH_BUF];
    fBigL1 = new BmnGemMap[N_CH_BUF];
    fBigR0 = new BmnGemMap[N_CH_BUF];
    fBigR1 = new BmnGemMap[N_CH_BUF];

    UniDbDetectorParameter* mapPar = UniDbDetectorParameter::GetDetectorParameter("GEM", "GEM_global_mapping", fPeriod, fRun);
    fEntriesInGlobMap = 0;
    if (mapPar != NULL) mapPar->GetGemMapArray(fMap, fEntriesInGlobMap);
    delete mapPar;
    for (Int_t i = 0; i < fEntriesInGlobMap; ++i)
        if (find(fSerials.begin(), fSerials.end(), fMap[i].serial) == fSerials.end())
            fSerials.push_back(fMap[i].serial);
    fNSerials = fSerials.size();

    ReadMap("GEM_X_small", fSmall, 0, 0);
    ReadMap("GEM_Y_small", fSmall, 1, 0);

    ReadMap("GEM_X0_middle", fMid, 2, 0);
    ReadMap("GEM_Y0_middle", fMid, 3, 0);
    ReadMap("GEM_X1_middle", fMid, 0, 0);
    ReadMap("GEM_Y1_middle", fMid, 1, 0);

    ReadMap("GEM_X0_Big_Left", fBigL0, 2, 1);
    ReadMap("GEM_Y0_Big_Left", fBigL0, 3, 1);

    ReadMap("GEM_X1_Big_Left", fBigL1, 0, 1);
    ReadMap("GEM_Y1_Big_Left", fBigL1, 1, 1);

    ReadMap("GEM_X0_Big_Right", fBigR0, 2, 0);
    ReadMap("GEM_Y0_Big_Right", fBigR0, 3, 0);

    ReadMap("GEM_X1_Big_Right", fBigR1, 0, 0);
    ReadMap("GEM_Y1_Big_Right", fBigR1, 1, 0);

    fPedVal = new Float_t**[fNSerials];
    fPedRms = new Float_t**[fNSerials];
    fAdcProfiles = new UInt_t**[fNSerials];
    fNoiseChannels = new Bool_t**[fNSerials];
    for (Int_t iCr = 0; iCr < fNSerials; ++iCr) {
        fPedVal[iCr] = new Float_t*[ADC_N_CHANNELS];
        fPedRms[iCr] = new Float_t*[ADC_N_CHANNELS];
        fAdcProfiles[iCr] = new UInt_t*[ADC_N_CHANNELS];
        fNoiseChannels[iCr] = new Bool_t*[ADC_N_CHANNELS];
        for (Int_t iCh = 0; iCh < ADC_N_CHANNELS; ++iCh) {
            fPedVal[iCr][iCh] = new Float_t[ADC32_N_SAMPLES];
            fPedRms[iCr][iCh] = new Float_t[ADC32_N_SAMPLES];
            fAdcProfiles[iCr][iCh] = new UInt_t[ADC32_N_SAMPLES];
            fNoiseChannels[iCr][iCh] = new Bool_t[ADC32_N_SAMPLES];
            for (Int_t iSmpl = 0; iSmpl < ADC32_N_SAMPLES; ++iSmpl) {
                fPedVal[iCr][iCh][iSmpl] = 0.0;
                fPedRms[iCr][iCh][iSmpl] = 0.0;
                fAdcProfiles[iCr][iCh][iSmpl] = 0;
                fNoiseChannels[iCr][iCh][iSmpl] = kFALSE;
            }
        }
    }

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
    while (!inFile.eof()) {
        inFile >> hex >> ser >> dec >> ch >> ped >> rms;
        if (!inFile.good()) break;
        for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
            if (ser == fSerials[iCr]) {
                //                fPedArr[iCr][ch] = BmnGemPed(ped, rms);
                fPedVal[iCr][ch / ADC32_N_SAMPLES][ch % ADC32_N_SAMPLES] = ped;
                fPedRms[iCr][ch / ADC32_N_SAMPLES][ch % ADC32_N_SAMPLES] = rms;
            }
    }

    path = TString(getenv("VMCWORKDIR")) + TString("/input/GEM_noisy_Channels.txt");
    ifstream noiseFile(path.Data());
    if (!noiseFile.is_open())
        cout << "Error opening map-file (" << path << ")!" << endl;
    noiseFile >> dummy >> dummy >> dummy;
    noiseFile >> dummy;
    Bool_t isNoise;
    while (!noiseFile.eof()) {
        noiseFile >> hex >> ser >> dec >> ch >> isNoise;
        if (!noiseFile.good()) break;
        for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
            if (ser == fSerials[iCr])
                fNoiseChannels[iCr][ch / ADC32_N_SAMPLES][ch % ADC32_N_SAMPLES] = isNoise;
    }

    fPedDat = new UInt_t***[fNSerials];
    for (Int_t iCr = 0; iCr < fNSerials; ++iCr) {
        fPedDat[iCr] = new UInt_t**[N_EV_FOR_PEDESTALS];
        for (UInt_t iEv = 0; iEv < N_EV_FOR_PEDESTALS; ++iEv) {
            fPedDat[iCr][iEv] = new UInt_t*[ADC_N_CHANNELS];
            for (Int_t iCh = 0; iCh < ADC_N_CHANNELS; ++iCh) {
                fPedDat[iCr][iEv][iCh] = new UInt_t[ADC32_N_SAMPLES];
                for (Int_t iSmpl = 0; iSmpl < ADC32_N_SAMPLES; ++iSmpl)
                    fPedDat[iCr][iEv][iCh][iSmpl] = 0;
            }
        }
    }
}

BmnStatus BmnGemRaw2Digit::ReadMap(TString parName, BmnGemMap* m, Int_t lay, Int_t mod) {
    Int_t size = 0;
    UniDbDetectorParameter* par = UniDbDetectorParameter::GetDetectorParameter("GEM", parName, fPeriod, fRun);
    IIStructure* iiArr;
    if (par != NULL) par->GetIIArray(iiArr, size);
    delete par;
    for (Int_t i = 0; i < size; ++i)
        m[iiArr[i].int_2] = BmnGemMap(iiArr[i].int_1, lay, mod);
    delete[] iiArr;
}

BmnGemRaw2Digit::~BmnGemRaw2Digit() {
    //    delete [] fPedArr;
    //    delete fCrates;
    //    delete fMid;
    //    delete fBigR;
    //    delete fBigL;
    //    delete fSmall;
}

BmnStatus BmnGemRaw2Digit::FillEvent(TClonesArray *adc, TClonesArray * gem) {
    fEventId++;
    for (Int_t iAdc = 0; iAdc < adc->GetEntriesFast(); ++iAdc) {
        BmnADCDigit* adcDig = (BmnADCDigit*) adc->At(iAdc);
        UInt_t ch = adcDig->GetChannel() * adcDig->GetNSamples();
        for (Int_t iMap = 0; iMap < fEntriesInGlobMap; ++iMap) {
            GemMapStructure gemM = fMap[iMap];
            if (adcDig->GetSerial() == gemM.serial && ch <= gemM.channel_high && ch >= gemM.channel_low) {
                ProcessDigit(adcDig, &gemM, gem);
                break;
            }
        }
    }
}

void BmnGemRaw2Digit::ProcessDigit(BmnADCDigit* adcDig, GemMapStructure* gemM, TClonesArray * gem) {
    const UInt_t nSmpl = adcDig->GetNSamples();
    UInt_t ch = adcDig->GetChannel();
    UInt_t ser = adcDig->GetSerial();

    Int_t iSer = -1;
    for (iSer = 0; iSer < fNSerials; ++iSer)
        if (ser == fSerials[iSer]) break;

    BmnGemStripDigit candDig[nSmpl];

    for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
        Int_t strip = -1;
        Int_t lay = -1;
        Int_t mod = -1;
        Int_t ch2048 = ch * nSmpl + iSmpl;
        UInt_t realChannel = ch2048;
        BmnGemMap* fBigMap = NULL;

        if (gemM->id < 10) {
            if ((gemM->channel_high - gemM->channel_low) < 128) realChannel = (2048 + ch2048 - gemM->channel_low);
            fBigMap = fMid;
        } else {
            if (gemM->hotZone % 2 == 0) {
                if (gemM->id % 10 == 0) fBigMap = fBigL0;
                else fBigMap = fBigR0;
            } else {
                if (gemM->id % 10 == 0) fBigMap = fBigL1;
                else fBigMap = fBigR1;
                if (gemM->channel_high - gemM->channel_low < 128) realChannel = (2048 + ch2048 - gemM->channel_low);
            }
        }
        mod = (gemM->hotZone < 2) ? 0 : 1;
        lay = fBigMap[realChannel].lay;
        strip = fBigMap[realChannel].strip;

        if (strip > 0) {
            BmnGemStripDigit dig;
            dig.SetStation(gemM->station);
            dig.SetModule(mod);
            dig.SetStripLayer(lay);
            dig.SetStripNumber(strip);
            dig.SetStripSignal((adcDig->GetValue())[iSmpl] / 16);
            candDig[iSmpl] = dig;
        }
    }

    Double_t signals[ADC32_N_SAMPLES];
    Int_t nOk = 0;
    for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
        if ((candDig[iSmpl]).GetStripSignal() == 0) continue;
        signals[iSmpl] = (candDig[iSmpl]).GetStripSignal();
        nOk++;
    }
    Double_t CMS = CalcCMS(signals, nOk);

    for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
        if ((candDig[iSmpl]).GetStation() == -1) continue;

        if (fNoiseChannels[iSer][ch][iSmpl]) continue;
        BmnGemStripDigit * dig = &candDig[iSmpl];
        Double_t ped = fPedVal[iSer][ch][iSmpl];
        Double_t rms = fPedRms[iSer][ch][iSmpl];
        Double_t sig = dig->GetStripSignal() - CMS - ped;
        Float_t threshold = 20;
        if (sig < threshold) continue;
        new((*gem)[gem->GetEntriesFast()]) BmnGemStripDigit(dig->GetStation(), dig->GetModule(), dig->GetStripLayer(), dig->GetStripNumber(), sig);
    }
}

BmnStatus BmnGemRaw2Digit::CalcGemPedestals(TClonesArray *adc, TTree * tree) {
    ofstream pedFile(Form("%s/input/GEM_pedestals.txt", getenv("VMCWORKDIR")));
    pedFile << "Serial\tCh_id\tPed\tRMS" << endl;
    pedFile << "============================================" << endl;
    const UShort_t nSmpl = ADC32_N_SAMPLES;
    const UInt_t nDigs = 640; //fNCrates * 64; // 10 ADC x 64 ch

    Double_t** pedestals = new Double_t* [nDigs];
    Double_t** noises = new Double_t* [nDigs];
    for (Int_t i = 0; i < nDigs; ++i) {
        pedestals[i] = new Double_t[nSmpl];
        noises[i] = new Double_t[nSmpl];
    }

    cout << "Pedestals calculation..." << endl;
    UInt_t nEv = tree->GetEntries();
    for (Int_t iEv = 0; iEv < nEv; ++iEv) {
        //        if (iEv % 100 == 0) cout << "Pedestals calculation: read event #" << iEv << endl;
        tree->GetEntry(iEv);
        for (Int_t iAdc = 0; iAdc < nDigs; ++iAdc) {
            BmnADCDigit* adcDig = (BmnADCDigit*) adc->At(iAdc);
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

    cout << "RMS calculation..." << endl;
    for (Int_t iEv = 0; iEv < nEv; ++iEv) {
        //        if (iEv % 100 == 0) cout << "RMS calculation: read event #" << iEv << endl;
        tree->GetEntry(iEv);
        for (Int_t iAdc = 0; iAdc < nDigs; ++iAdc) {
            BmnADCDigit* adcDig = (BmnADCDigit*) adc->At(iAdc);
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
        BmnADCDigit* adcDig = (BmnADCDigit*) adc->At(iAdc);
        for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
            noises[iAdc][iSmpl] = Sqrt(noises[iAdc][iSmpl] / nEv);
            if (noises[iAdc][iSmpl])
                pedFile << hex << adcDig->GetSerial() << dec << "\t" << adcDig->GetChannel() * nSmpl + iSmpl << "\t" << pedestals[iAdc][iSmpl] << "\t" << noises[iAdc][iSmpl] << endl;
        }
    }

    pedFile.close();
}

BmnStatus BmnGemRaw2Digit::FindNoisyStrips() {
    const Short_t kNBUNCHES = 8;
    const Short_t kNSAMPLES = ADC32_N_SAMPLES / kNBUNCHES;
    const Short_t kNITER = 4;
    const Float_t coeff[kNITER] = {2, 2, 2, 2};

    //            TH1F* h_prof = new TH1F("h_prof", "prof", 20480, 0.0, 20480.0);
    //            TCanvas* canv = new TCanvas("c", "c", 1000, 1000);
    //            for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
    //                for (Int_t iCh = 0; iCh < ADC_N_CHANNELS; ++iCh)
    //                    for (Int_t iSmpl = 0; iSmpl < ADC32_N_SAMPLES; ++iSmpl)
    //                        h_prof->SetBinContent(iSmpl + iCh * ADC32_N_SAMPLES + iCr * ADC_N_CHANNELS * ADC32_N_SAMPLES, fAdcProfiles[iCr][iCh][iSmpl]);
    //        
    //            h_prof->Draw();
    //            canv->SaveAs("ADC_profile.root");
    //            Fatal("", "STOP ADC FILLING");

    for (Int_t iCr = 0; iCr < fNSerials; ++iCr) {
        for (Int_t iCh = 0; iCh < ADC_N_CHANNELS; ++iCh) {
            Bool_t channelOk[ADC32_N_SAMPLES];
            Short_t nOk[kNBUNCHES];
            for (Short_t i = 0; i < kNBUNCHES; ++i) nOk[i] = kNSAMPLES;
            for (Short_t i = 0; i < ADC32_N_SAMPLES; ++i) channelOk[i] = kTRUE;

            for (Short_t itr = 0; itr < kNITER; ++itr) {
                for (Int_t iBunch = 0; iBunch < kNBUNCHES; ++iBunch) {
                    Double_t mean = 0.0;
                    for (Short_t i = 0; i < kNSAMPLES; ++i) {
                        Short_t idx = i + iBunch * kNSAMPLES;
                        //                        cout << fAdcProfiles[iCr][iCh][idx] << " ";
                        if (fAdcProfiles[iCr][iCh][idx] > 0.7 * N_EV_FOR_PEDESTALS && channelOk[idx] == kTRUE) {
                            nOk[iBunch]--;
                            channelOk[idx] = kFALSE;
                            continue;
                        }
                        if (channelOk[idx] == kTRUE && fAdcProfiles[iCr][iCh][idx] == 0) {
                            //channelOk[idx] = kFALSE;
                            nOk[iBunch]--;
                            continue;
                        }
                        mean += (((Int_t) channelOk[idx]) * fAdcProfiles[iCr][iCh][idx]);
                    }
                    if (nOk[iBunch] == 0) continue;
                    mean /= nOk[iBunch];
                    for (Short_t i = 0; i < kNSAMPLES; ++i) {
                        Short_t idx = i + iBunch * kNSAMPLES;
                        if (((Int_t) channelOk[idx]) * fAdcProfiles[iCr][iCh][idx] > coeff[itr] * mean) {
                            channelOk[idx] = kFALSE;
                            nOk[iBunch]--;
                        }
                    }
                }
            }
            for (Short_t i = 0; i < ADC32_N_SAMPLES; ++i)
                fNoiseChannels[iCr][iCh][i] = !(channelOk[i]);
        }
    }

    ofstream pedFile(Form("%s/input/GEM_noisy_Channels.txt", getenv("VMCWORKDIR")));
    pedFile << "Serial\tCh_id\tIsNoisy" << endl;
    pedFile << "============================================" << endl;
    for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
        for (Int_t iCh = 0; iCh < ADC_N_CHANNELS; ++iCh)
            for (Int_t iSmpl = 0; iSmpl < ADC32_N_SAMPLES; ++iSmpl)
                pedFile << hex << fSerials[iCr] << dec << "\t" << iCh * ADC32_N_SAMPLES + iSmpl << "\t" << fNoiseChannels[iCr][iCh][iSmpl] << endl;
    pedFile.close();

    return kBMNSUCCESS;
}

BmnStatus BmnGemRaw2Digit::RecalculatePedestals() {
    const UShort_t nSmpl = ADC32_N_SAMPLES;

    for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
        for (Int_t iCh = 0; iCh < ADC_N_CHANNELS; ++iCh) {
            for (Int_t iSmpl = 0; iSmpl < ADC32_N_SAMPLES; ++iSmpl) {
                fNoiseChannels[iCr][iCh][iSmpl] = kFALSE;
                fPedVal[iCr][iCh][iSmpl] = 0.0;
                fPedRms[iCr][iCh][iSmpl] = 0.0;
                fAdcProfiles[iCr][iCh][iSmpl] = 0;
            }
        }
    cout << "Pedestals calculation..." << endl;
    for (Int_t iEv = 0; iEv < N_EV_FOR_PEDESTALS; ++iEv) {
        //        if (iEv % 100 == 0) cout << "Pedestals calculation: read event #" << iEv << endl;
        for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
            for (Int_t iCh = 0; iCh < ADC_N_CHANNELS; ++iCh) {
                Double_t signals[nSmpl];
                for (Int_t i = 0; i < nSmpl; ++i) signals[i] = 0.0;
                Int_t nOk = 0;
                for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
                    if (fPedDat[iCr][iEv][iCh][iSmpl] == 0) continue;
                    signals[iSmpl] = fPedDat[iCr][iEv][iCh][iSmpl];
                    nOk++;
                }
                Double_t CMS = CalcCMS(signals, nOk);
                for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
                    fPedVal[iCr][iCh][iSmpl] += ((signals[iSmpl] - CMS) / N_EV_FOR_PEDESTALS);
                }
            }
    }

    cout << "RMS calculation..." << endl;
    for (Int_t iEv = 0; iEv < N_EV_FOR_PEDESTALS; ++iEv) {
        //        if (iEv % 100 == 0) cout << "RMS calculation: read event #" << iEv << endl;
        for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
            for (Int_t iCh = 0; iCh < ADC_N_CHANNELS; ++iCh) {
                Double_t signals[nSmpl];
                for (Int_t i = 0; i < nSmpl; ++i) signals[i] = 0.0;
                Int_t nOk = 0;
                for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
                    if (fPedDat[iCr][iEv][iCh][iSmpl] == 0) continue;
                    signals[iSmpl] = fPedDat[iCr][iEv][iCh][iSmpl];
                    nOk++;
                }
                Double_t CMS = CalcCMS(signals, nOk);
                for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
                    Float_t ped = fPedVal[iCr][iCh][iSmpl];
                    fPedRms[iCr][iCh][iSmpl] += ((signals[iSmpl] - CMS - ped) * (signals[iSmpl] - CMS - ped));
                }
            }
    }

    for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
        for (Int_t iCh = 0; iCh < ADC_N_CHANNELS; ++iCh)
            for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl)
                fPedRms[iCr][iCh][iSmpl] = Sqrt(fPedRms[iCr][iCh][iSmpl] / N_EV_FOR_PEDESTALS);

    cout << "Profile filling..." << endl;
    for (Int_t iEv = 0; iEv < N_EV_FOR_PEDESTALS; ++iEv) {
        //        if (iEv % 100 == 0) cout << "Profile filling: read event #" << iEv << endl;
        for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
            for (Int_t iCh = 0; iCh < ADC_N_CHANNELS; ++iCh) {
                Double_t signals[nSmpl];
                Int_t nOk = 0;
                for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
                    if (fPedDat[iCr][iEv][iCh][iSmpl] == 0) continue;
                    signals[iSmpl] = fPedDat[iCr][iEv][iCh][iSmpl];
                    nOk++;
                }
                Double_t CMS = CalcCMS(signals, nOk);
                for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {

                    if (fPedDat[iCr][iEv][iCh][iSmpl] == 0 || CMS == 0.0) continue;
                    if (fPedDat[iCr][iEv][iCh][iSmpl] - CMS - fPedVal[iCr][iCh][iSmpl] > 20/*3 * fPedRms[iCr][iCh][iSmpl]*/) {
                        fAdcProfiles[iCr][iCh][iSmpl]++;
                    }
                }
            }
    }

    ofstream pedFile(Form("%s/input/GEM_pedestals.txt", getenv("VMCWORKDIR")));
    pedFile << "Serial\tCh_id\tPed\tRMS" << endl;
    pedFile << "============================================" << endl;
    for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
        for (Int_t iCh = 0; iCh < ADC_N_CHANNELS; ++iCh)
            for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl)
                pedFile << hex << fSerials[iCr] << dec << "\t" << iCh * nSmpl + iSmpl << "\t" << fPedVal[iCr][iCh][iSmpl] << "\t" << fPedRms[iCr][iCh][iSmpl] << endl;
    pedFile.close();

    FindNoisyStrips();

    return kBMNSUCCESS;
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

ClassImp(BmnGemRaw2Digit)

