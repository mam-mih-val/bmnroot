#include "BmnCscRaw2Digit.h"

BmnCscRaw2Digit::BmnCscRaw2Digit() {
    fEventId = -1;
    fMapFileName = "";
}

BmnCscRaw2Digit::BmnCscRaw2Digit(Int_t period, Int_t run, vector<UInt_t> vSer) : BmnAdcProcessor(period, run, "CSC", ADC_N_CHANNELS, ADC32_N_SAMPLES, vSer) {

    cout << "Loading CSC Map: Period " << period << ", Run " << run << "..." << endl;

    fEventId = 0;
    fMapFileName = Form("CSC_map_period%d.txt", period);
    ReadMapFile();

    const Int_t kNStations = 1;
    const Int_t kNStrips = 4096; //FIXME

    fSigProf = new TH1F***[kNStations];
    fNoisyChannels = new Bool_t***[kNStations];
    for (Int_t iSt = 0; iSt < kNStations; ++iSt) {
        fSigProf[iSt] = new TH1F**[N_CSC_MODULES];
        fNoisyChannels[iSt] = new Bool_t**[N_CSC_MODULES];
        for (UInt_t iMod = 0; iMod < N_CSC_MODULES; ++iMod) {
            fSigProf[iSt][iMod] = new TH1F*[N_CSC_LAYERS];
            fNoisyChannels[iSt][iMod] = new Bool_t*[N_CSC_LAYERS];
            for (Int_t iLay = 0; iLay < N_CSC_LAYERS; ++iLay) {
                TString histName;
                histName.Form("CSC_%d_%d_%d", iSt, iMod, iLay);
                fSigProf[iSt][iMod][iLay] = new TH1F(histName, histName, kNStrips, 0, kNStrips);
                fNoisyChannels[iSt][iMod][iLay] = new Bool_t[kNStrips];
                for (Int_t iStrip = 0; iStrip < kNStrips; ++iStrip)
                    fNoisyChannels[iSt][iMod][iLay][iStrip] = kFALSE;
            }
        }
    }
}

BmnCscRaw2Digit::~BmnCscRaw2Digit() {
    const Int_t kNStations = 1;

    for (Int_t iSt = 0; iSt < kNStations; ++iSt) {
        for (UInt_t iMod = 0; iMod < N_CSC_MODULES; ++iMod) {
            for (Int_t iLay = 0; iLay < N_CSC_LAYERS; ++iLay) {
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
        record->channel_low = ch_lo;
        record->channel_high = ch_hi;
        record->station = station;
        fMap.push_back(record);
    }
}

BmnStatus BmnCscRaw2Digit::FillEvent(TClonesArray *adc, TClonesArray * csc) {
    fEventId++;
    for (Int_t iAdc = 0; iAdc < adc->GetEntriesFast(); ++iAdc) {
        BmnADCDigit* adcDig = (BmnADCDigit*) adc->At(iAdc);
        ProcessDigit(adcDig, FindMapEntry(adcDig), csc, kFALSE);
    }
}

BmnStatus BmnCscRaw2Digit::FillProfiles(TClonesArray *adc) {
    for (Int_t iAdc = 0; iAdc < adc->GetEntriesFast(); ++iAdc) {
        BmnADCDigit* adcDig = (BmnADCDigit*) adc->At(iAdc);
        ProcessDigit(adcDig, FindMapEntry(adcDig), NULL, kTRUE);
    }
}

BmnCscMapping* BmnCscRaw2Digit::FindMapEntry(BmnADCDigit* adcDig) {
    for (Int_t iMap = 0; iMap < fMap.size(); ++iMap) {
        BmnCscMapping* tM = fMap[iMap];
        UInt_t ch = adcDig->GetChannel() * adcDig->GetNSamples();
        if (adcDig->GetSerial() == tM->serial && (ch >= tM->channel_low && ch <= tM->channel_high))
            return tM;
    }
    return NULL;
}

BmnStatus BmnCscRaw2Digit::FillNoisyChannels() {
    const Int_t kNStations = 1;
    const Int_t kNStrips = 2048; //FIXME
    const Int_t kNStripsInBunch = 32;
    const Int_t kNBunches = kNStrips / kNStripsInBunch;
    const Int_t kNThresh = 3;

    for (Int_t iSt = 0; iSt < kNStations; ++iSt)
        for (UInt_t iMod = 0; iMod < N_CSC_MODULES; ++iMod)
            for (Int_t iLay = 0; iLay < N_CSC_LAYERS; ++iLay) {
                TH1F* prof = fSigProf[iSt][iMod][iLay];
                for (Int_t iBunch = 0; iBunch < kNBunches; ++iBunch) {
                    Double_t meanDiff = 0.0;
                    for (Int_t iStrip = 0; iStrip < kNStripsInBunch - 1; ++iStrip) {
                        Int_t strip = iStrip + iBunch * kNStripsInBunch;
                        Double_t curr = prof->GetBinContent(strip);
                        Double_t next = prof->GetBinContent(strip + 1);
                        meanDiff += Abs(next - curr);
                    }
                    meanDiff /= kNStripsInBunch;
                    for (Int_t iStrip = 0; iStrip < kNStripsInBunch - 1; ++iStrip) {
                        Int_t strip = iStrip + iBunch * kNStripsInBunch;
                        Double_t curr = prof->GetBinContent(strip);
                        Double_t next = prof->GetBinContent(strip + 1);

                        if (kNThresh * meanDiff < next - curr)
                            fNoisyChannels[iSt][iMod][iLay][strip] = kTRUE;
                    }
                }
            }
}

void BmnCscRaw2Digit::ProcessDigit(BmnADCDigit* adcDig, BmnCscMapping* cscM, TClonesArray * csc, Bool_t doFill) {
    if (!adcDig || !cscM || !csc) return;
    const UInt_t nSmpl = adcDig->GetNSamples();
    UInt_t ch = adcDig->GetChannel();
    UInt_t ser = adcDig->GetSerial();

    Int_t iSer = -1;
    for (iSer = 0; iSer < GetSerials().size(); ++iSer)
        if (ser == GetSerials()[iSer]) break;

    BmnCscDigit candDig[nSmpl];

    for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
        BmnCscDigit dig;
        dig.SetStation(cscM->station);
        dig.SetModule(cscM->module);
        dig.SetStripLayer(cscM->layer);
        //        dig.SetStripNumber((ch - cscM->channel_low) * nSmpl + 1 + iSmpl); //FIXME
        dig.SetStripNumber(0); //FIXME
        Double_t sig = (Double_t) ((adcDig->GetShortValue())[iSmpl] / 16);
        dig.SetStripSignal(sig);
        candDig[iSmpl] = dig;
    }

    Double_t signals[nSmpl];
    for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) signals[iSmpl] = 0.0;
    Int_t nOk = 0;
    for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
        if ((candDig[iSmpl]).GetStripSignal() == 0) continue;
        signals[iSmpl] = (candDig[iSmpl]).GetStripSignal();
        nOk++;
    }
    Double_t CMS = CalcCMS(signals, nOk);

    Double_t*** vPed = GetPedestals();
    Double_t*** vPedRMS = GetPedestalsRMS();

    for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
        if ((candDig[iSmpl]).GetStation() == -1) continue;
        BmnCscDigit * dig = &candDig[iSmpl];
        Double_t ped = vPed[iSer][ch][iSmpl];
        Double_t sig = Abs(dig->GetStripSignal() - CMS - ped);
        //        Double_t threshold = 120; // * vPedRMS[iSer][ch][iSmpl]; //50;//120;//160;
        //        if (sig < threshold || sig == 0.0) continue;
        if (doFill) {
            fSigProf[dig->GetStation()][dig->GetModule()][dig->GetStripLayer()]->Fill(dig->GetStripNumber());
        } else {
            BmnCscDigit * resDig = new((*csc)[csc->GetEntriesFast()]) BmnCscDigit(dig->GetStation(), dig->GetModule(), dig->GetStripLayer(), dig->GetStripNumber(), sig);
            if (fNoisyChannels[dig->GetStation()][dig->GetModule()][dig->GetStripLayer()][dig->GetStripNumber()])
                resDig->SetIsGoodDigit(kFALSE);
            else
                resDig->SetIsGoodDigit(kTRUE);
        }
    }
}

ClassImp(BmnCscRaw2Digit)

