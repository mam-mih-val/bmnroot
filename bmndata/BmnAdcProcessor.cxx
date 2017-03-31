#include "BmnAdcProcessor.h"

BmnAdcProcessor::BmnAdcProcessor() {
}

BmnAdcProcessor::BmnAdcProcessor(Int_t period, Int_t run, TString det, Int_t nCh, Int_t nSmpl, vector<UInt_t> vSer) {

    fPeriod = period;
    fRun = run;
    fDetName = det;
    fNSerials = vSer.size();
    fNChannels = nCh;
    fNSamples = nSmpl;
    fSerials = vSer;
    
    fPedVal = new Float_t**[fNSerials];
    fPedRms = new Float_t**[fNSerials];
    fAdcProfiles = new UInt_t**[fNSerials];
    fNoiseChannels = new Bool_t**[fNSerials];
    for (Int_t iCr = 0; iCr < fNSerials; ++iCr) {
        fPedVal[iCr] = new Float_t*[fNChannels];
        fPedRms[iCr] = new Float_t*[fNChannels];
        fAdcProfiles[iCr] = new UInt_t*[fNChannels];
        fNoiseChannels[iCr] = new Bool_t*[fNChannels];
        for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
            fPedVal[iCr][iCh] = new Float_t[fNSamples];
            fPedRms[iCr][iCh] = new Float_t[fNSamples];
            fAdcProfiles[iCr][iCh] = new UInt_t[fNSamples];
            fNoiseChannels[iCr][iCh] = new Bool_t[fNSamples];
            for (Int_t iSmpl = 0; iSmpl < fNSamples; ++iSmpl) {
                fPedVal[iCr][iCh][iSmpl] = 0.0;
                fPedRms[iCr][iCh][iSmpl] = 0.0;
                fAdcProfiles[iCr][iCh][iSmpl] = 0;
                fNoiseChannels[iCr][iCh][iSmpl] = kFALSE;
            }
        }
    }

    fPedDat = new UInt_t***[fNSerials];
    for (Int_t iCr = 0; iCr < fNSerials; ++iCr) {
        fPedDat[iCr] = new UInt_t**[N_EV_FOR_PEDESTALS];
        for (UInt_t iEv = 0; iEv < N_EV_FOR_PEDESTALS; ++iEv) {
            fPedDat[iCr][iEv] = new UInt_t*[fNChannels];
            for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
                fPedDat[iCr][iEv][iCh] = new UInt_t[fNSamples];
                for (Int_t iSmpl = 0; iSmpl < fNSamples; ++iSmpl)
                    fPedDat[iCr][iEv][iCh][iSmpl] = 0;
            }
        }
    }
}

BmnAdcProcessor::~BmnAdcProcessor() {
}

BmnStatus BmnAdcProcessor::FindNoisyStrips() {
    const Short_t kNBUNCHES = 8;
    const Short_t kNSAMPLES = fNSamples / kNBUNCHES;
    const Short_t kNITER = 4;
    const Float_t coeff[kNITER] = {2, 2, 2, 2};

    for (Int_t iCr = 0; iCr < fNSerials; ++iCr) {
        for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
            Bool_t channelOk[fNSamples];
            Short_t nOk[kNBUNCHES];
            for (Short_t i = 0; i < kNBUNCHES; ++i) nOk[i] = kNSAMPLES;
            for (Short_t i = 0; i < fNSamples; ++i) channelOk[i] = kTRUE;

            for (Short_t itr = 0; itr < kNITER; ++itr) {
                for (Int_t iBunch = 0; iBunch < kNBUNCHES; ++iBunch) {
                    Double_t mean = 0.0;
                    for (Short_t i = 0; i < kNSAMPLES; ++i) {
                        Short_t idx = i + iBunch * kNSAMPLES;
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
            for (Short_t i = 0; i < fNSamples; ++i)
                fNoiseChannels[iCr][iCh][i] = !(channelOk[i]);
        }
    }

    ofstream pedFile(Form("%s/input/%s_noisy_Channels_%d.txt", getenv("VMCWORKDIR"), fDetName.Data(), fRun));
    pedFile << "Serial\tCh_id\tIsNoisy" << endl;
    pedFile << "============================================" << endl;
    for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
        for (Int_t iCh = 0; iCh < fNChannels; ++iCh)
            for (Int_t iSmpl = 0; iSmpl < fNSamples; ++iSmpl)
                pedFile << hex << fSerials[iCr] << dec << "\t" << iCh * fNSamples + iSmpl << "\t" << fNoiseChannels[iCr][iCh][iSmpl] << endl;
    pedFile.close();

    return kBMNSUCCESS;
}

BmnStatus BmnAdcProcessor::RecalculatePedestals() {
    const UShort_t nSmpl = fNSamples;

    for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
        for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
            for (Int_t iSmpl = 0; iSmpl < fNSamples; ++iSmpl) {
                fNoiseChannels[iCr][iCh][iSmpl] = kFALSE;
                fPedVal[iCr][iCh][iSmpl] = 0.0;
                fPedRms[iCr][iCh][iSmpl] = 0.0;
                fAdcProfiles[iCr][iCh][iSmpl] = 0;
            }
        }
    cout << fDetName << " pedestals calculation..." << endl;
    for (Int_t iEv = 0; iEv < N_EV_FOR_PEDESTALS; ++iEv) {
        for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
            for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
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

    cout << fDetName << " RMS calculation..." << endl;
    for (Int_t iEv = 0; iEv < N_EV_FOR_PEDESTALS; ++iEv) {
        for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
            for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
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
        for (Int_t iCh = 0; iCh < fNChannels; ++iCh)
            for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl)
                fPedRms[iCr][iCh][iSmpl] = Sqrt(fPedRms[iCr][iCh][iSmpl] / N_EV_FOR_PEDESTALS);

    cout << fDetName << " profile filling..." << endl;
    for (Int_t iEv = 0; iEv < N_EV_FOR_PEDESTALS; ++iEv) {
        for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
            for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
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

    ofstream pedFile(Form("%s/input/%s_pedestals_%d.txt", getenv("VMCWORKDIR"), fDetName.Data(), fRun));
    pedFile << "Serial\tCh_id\tPed\tRMS" << endl;
    pedFile << "============================================" << endl;
    for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
        for (Int_t iCh = 0; iCh < fNChannels; ++iCh)
            for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl)
                pedFile << hex << fSerials[iCr] << dec << "\t" << iCh * nSmpl + iSmpl << "\t" << fPedVal[iCr][iCh][iSmpl] << "\t" << fPedRms[iCr][iCh][iSmpl] << endl;
    pedFile.close();

    FindNoisyStrips();

    return kBMNSUCCESS;
}

Double_t BmnAdcProcessor::CalcCMS(Double_t* samples, Int_t size) {

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

ClassImp(BmnAdcProcessor)
