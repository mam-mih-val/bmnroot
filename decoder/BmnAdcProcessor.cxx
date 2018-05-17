#include "BmnAdcProcessor.h"
#include <BmnMath.h>

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

    fPedVal = new Double_t**[fNSerials];
    fPedRms = new Double_t**[fNSerials];
    fAdcProfiles = new UInt_t**[fNSerials];
    for (Int_t iCr = 0; iCr < fNSerials; ++iCr) {
        fPedVal[iCr] = new Double_t*[fNChannels];
        fPedRms[iCr] = new Double_t*[fNChannels];
        fAdcProfiles[iCr] = new UInt_t*[fNChannels];
        for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
            fPedVal[iCr][iCh] = new Double_t[fNSamples];
            fPedRms[iCr][iCh] = new Double_t[fNSamples];
            fAdcProfiles[iCr][iCh] = new UInt_t[fNSamples];
            for (Int_t iSmpl = 0; iSmpl < fNSamples; ++iSmpl) {
                fPedVal[iCr][iCh][iSmpl] = 0.0;
                fPedRms[iCr][iCh][iSmpl] = 0.0;
                fAdcProfiles[iCr][iCh][iSmpl] = 0;
            }
        }
    }

    fPedDat = new Double_t***[fNSerials];
    for (Int_t iCr = 0; iCr < fNSerials; ++iCr) {
        fPedDat[iCr] = new Double_t**[N_EV_FOR_PEDESTALS];
        for (UInt_t iEv = 0; iEv < N_EV_FOR_PEDESTALS; ++iEv) {
            fPedDat[iCr][iEv] = new Double_t*[fNChannels];
            for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
                fPedDat[iCr][iEv][iCh] = new Double_t[fNSamples];
                for (Int_t iSmpl = 0; iSmpl < fNSamples; ++iSmpl)
                    fPedDat[iCr][iEv][iCh][iSmpl] = 0;
            }
        }
    }
}

BmnAdcProcessor::~BmnAdcProcessor() {
    for (Int_t iCr = 0; iCr < fNSerials; iCr++) {
        for (Int_t iEv = 0; iEv < N_EV_FOR_PEDESTALS; iEv++) {
            for (Int_t iCh = 0; iCh < fNChannels; iCh++)
                delete[] fPedDat[iCr][iEv][iCh];
            delete[] fPedDat[iCr][iEv];
        }
        delete[] fPedDat[iCr];
    }
    delete[] fPedDat;
    for (Int_t iCr = 0; iCr < fNSerials; ++iCr) {
        for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
            delete[] fPedVal[iCr][iCh];
            delete[] fPedRms[iCr][iCh];
            delete[] fAdcProfiles[iCr][iCh];
        }
        delete[] fPedVal[iCr];
        delete[] fPedRms[iCr];
        delete[] fAdcProfiles[iCr];
    }
    delete[] fPedVal;
    delete[] fPedRms;
    delete[] fAdcProfiles;
}

BmnStatus BmnAdcProcessor::RecalculatePedestals() {
//    printf("\n[INFO]");
//    printf(ANSI_COLOR_BLUE " ADC pedestals recalculation\n" ANSI_COLOR_RESET);
    const UShort_t nSmpl = fNSamples;

    for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
        for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
            for (Int_t iSmpl = 0; iSmpl < fNSamples; ++iSmpl) {
                fPedVal[iCr][iCh][iSmpl] = 0.0;
                fPedRms[iCr][iCh][iSmpl] = 0.0;
                fAdcProfiles[iCr][iCh][iSmpl] = 0;
            }
        }
    //cout << fDetName << " pedestals calculation..." << endl;
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

    //cout << fDetName << " RMS calculation..." << endl;
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

    ofstream pedFile(Form("%s/input/%s_pedestals_%d.txt", getenv("VMCWORKDIR"), fDetName.Data(), fRun));
    pedFile << "Serial\tCh_id\tPed\tRMS" << endl;
    pedFile << "============================================" << endl;
    for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
        for (Int_t iCh = 0; iCh < fNChannels; ++iCh)
            for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl)
                pedFile << hex << fSerials[iCr] << dec << "\t" << iCh * nSmpl + iSmpl << "\t" << fPedVal[iCr][iCh][iSmpl] << "\t" << fPedRms[iCr][iCh][iSmpl] << endl;
    pedFile.close();
    
    return kBMNSUCCESS;
}

Double_t BmnAdcProcessor::CalcCMS(Double_t* samples, Int_t size) {

    const UShort_t kNITER = 10;
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
