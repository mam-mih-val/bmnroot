#include "BmnGemRaw2Digit.h"

BmnGemRaw2Digit::BmnGemRaw2Digit() {
    fSmall = NULL;
    fMid = NULL;
    fBigL0 = NULL;
    fBigL1 = NULL;
    fBigR0 = NULL;
    fBigR1 = NULL;
}

BmnGemRaw2Digit::BmnGemRaw2Digit(Int_t period, Int_t run, vector<UInt_t> vSer, TString mapFileName) : BmnAdcProcessor(period, run, "GEM", ADC_N_CHANNELS, ADC32_N_SAMPLES, vSer) {
    fSmall = NULL;
    fMid = NULL;
    fBigL0 = NULL;
    fBigL1 = NULL;
    fBigR0 = NULL;
    fBigR1 = NULL;

    cout << "Loading GEM Map: Period " << period << ", Run " << run << "..." << endl;

    fEventId = 0;

    fSmall = new BmnGemMap[N_CH_BUF];
    fMid = new BmnGemMap[N_CH_BUF];
    fBigL0 = new BmnGemMap[N_CH_BUF];
    fBigL1 = new BmnGemMap[N_CH_BUF];
    fBigR0 = new BmnGemMap[N_CH_BUF];
    fBigR1 = new BmnGemMap[N_CH_BUF];

    //    UniDbDetectorParameter* mapPar = UniDbDetectorParameter::GetDetectorParameter("GEM", "GEM_global_mapping", period, run);
    //    fEntriesInGlobMap = 0;
    //    if (mapPar != NULL) mapPar->GetGemMapArray(fMap, fEntriesInGlobMap);
    //    delete mapPar;
    //    for (Int_t i = 0; i < fEntriesInGlobMap; ++i)
    //        if (find(fSerials.begin(), fSerials.end(), fMap[i].serial) == fSerials.end())
    //            fSerials.push_back(fMap[i].serial);

    string dummy;
    UInt_t id = 0;
    UInt_t ser = 0;
    UInt_t ch_lo = 0;
    UInt_t ch_hi = 0;
    UInt_t station = 0;
    UInt_t hot = 0;
    //    TString name = TString(getenv("VMCWORKDIR")) + TString("/input/") + TString(Form("GEM_map_run%d_SRC.txt", period));
    TString name = TString(getenv("VMCWORKDIR")) + TString("/input/") + mapFileName;
    printf("%s\n", name.Data());
    ifstream inFile(name.Data());
    if (!inFile.is_open())
        cout << "Error opening map-file (" << name << ")!" << endl;
    for (Int_t i = 0; i < 5; ++i) getline(inFile, dummy); //comment line in input file

    while (!inFile.eof()) {
        inFile >> std::hex >> ser >> std::dec >> ch_lo >> ch_hi >> id >> station >> hot;
        if (!inFile.good()) break;
        GemMapStructure record;
        record.channel_high = ch_hi;
        record.serial = ser;
        record.channel_low = ch_lo;
        record.station = station;
        record.hotZone = hot;
        record.id = id;
        fMap.push_back(record);
    }
    fEntriesInGlobMap = fMap.size();

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

    const Int_t kNStations = 10;
    const Int_t kNStrips = 1300;

    fSigProf = new TH1F***[kNStations];
    fNoisyChannels = new Bool_t***[kNStations];
    for (Int_t iSt = 0; iSt < kNStations; ++iSt) {
        fSigProf[iSt] = new TH1F**[N_MODULES];
        fNoisyChannels[iSt] = new Bool_t**[N_MODULES];
        for (UInt_t iMod = 0; iMod < N_MODULES; ++iMod) {
            fSigProf[iSt][iMod] = new TH1F*[N_LAYERS];
            fNoisyChannels[iSt][iMod] = new Bool_t*[N_LAYERS];
            for (Int_t iLay = 0; iLay < N_LAYERS; ++iLay) {
                TString histName;
                histName.Form("GEM_%d_%d_%d", iSt, iMod, iLay);
                fSigProf[iSt][iMod][iLay] = new TH1F(histName, histName, kNStrips, 0, kNStrips);
                fNoisyChannels[iSt][iMod][iLay] = new Bool_t[kNStrips];
                for (Int_t iStrip = 0; iStrip < kNStrips; ++iStrip)
                    fNoisyChannels[iSt][iMod][iLay][iStrip] = kFALSE;
            }
        }
    }
}

BmnStatus BmnGemRaw2Digit::ReadMap(TString parName, BmnGemMap* m, Int_t lay, Int_t mod) {
    Int_t size = 0;
    UniDbDetectorParameter* par = UniDbDetectorParameter::GetDetectorParameter("GEM", parName, GetPeriod(), GetRun());
    IIStructure* iiArr;
    if (par != NULL) par->GetIIArray(iiArr, size);
    delete par;
    for (Int_t i = 0; i < size; ++i)
        m[iiArr[i].int_2] = BmnGemMap(iiArr[i].int_1, lay, mod);
    delete[] iiArr;
}

BmnGemRaw2Digit::~BmnGemRaw2Digit() {
    if (fSmall) delete[] fSmall;
    if (fMid) delete[] fMid;
    if (fBigL0) delete[] fBigL0;
    if (fBigL1) delete[] fBigL1;
    if (fBigR0) delete[] fBigR0;
    if (fBigR1) delete[] fBigR1;
    //    if (fMap) delete[] fMap;

    const Int_t kNStations = 10;

    for (Int_t iSt = 0; iSt < kNStations; ++iSt) {
        for (UInt_t iMod = 0; iMod < N_MODULES; ++iMod) {
            for (Int_t iLay = 0; iLay < N_LAYERS; ++iLay) {
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
}

BmnStatus BmnGemRaw2Digit::FillProfiles(TClonesArray *adc) {
    for (Int_t iAdc = 0; iAdc < adc->GetEntriesFast(); ++iAdc) {
        BmnADCDigit* adcDig = (BmnADCDigit*) adc->At(iAdc);
        UInt_t ch = adcDig->GetChannel() * adcDig->GetNSamples();
        for (Int_t iMap = 0; iMap < fEntriesInGlobMap; ++iMap) {
            GemMapStructure gemM = fMap[iMap];
            if (adcDig->GetSerial() == gemM.serial && ch <= gemM.channel_high && ch >= gemM.channel_low) {
                ProcessDigit(adcDig, &gemM, NULL, kTRUE);
                break;
            }
        }
    }
}

BmnStatus BmnGemRaw2Digit::FillNoisyChannels() {
    const Int_t kNStations = 10;
    const Int_t kNStrips = 1300;
    const Int_t kNStripsInBunch = 32;
    const Int_t kNBunches = kNStrips / kNStripsInBunch;
    const Int_t kNThresh = 3;

    for (Int_t iSt = 0; iSt < kNStations; ++iSt)
        for (UInt_t iMod = 0; iMod < N_MODULES; ++iMod)
            for (Int_t iLay = 0; iLay < N_LAYERS; ++iLay) {
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

BmnStatus BmnGemRaw2Digit::FillEvent(TClonesArray *adc, TClonesArray * gem) {
    fEventId++;
    for (Int_t iAdc = 0; iAdc < adc->GetEntriesFast(); ++iAdc) {
        BmnADCDigit* adcDig = (BmnADCDigit*) adc->At(iAdc);
        UInt_t ch = adcDig->GetChannel() * adcDig->GetNSamples();
        for (Int_t iMap = 0; iMap < fEntriesInGlobMap; ++iMap) {
            GemMapStructure gemM = fMap[iMap];
            if (adcDig->GetSerial() == gemM.serial && ch <= gemM.channel_high && ch >= gemM.channel_low) {
                ProcessDigit(adcDig, &gemM, gem, kFALSE);
                break;
            }
        }
    }
}

void BmnGemRaw2Digit::ProcessDigit(BmnADCDigit* adcDig, GemMapStructure* gemM, TClonesArray * gem, Bool_t doFill) {
    const UInt_t nSmpl = adcDig->GetNSamples();
    UInt_t ch = adcDig->GetChannel();
    UInt_t ser = adcDig->GetSerial();

    Int_t iSer = -1;
    for (iSer = 0; iSer < GetSerials().size(); ++iSer)
        if (ser == GetSerials()[iSer]) break;

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
            if (gemM->hotZone % 2 == 0) { //hot zone
                if (gemM->channel_low == 0)
                    realChannel += 1024;
                if (gemM->id % 10 == 0)
                    fBigMap = fBigL0;
                else
                    fBigMap = fBigR0;
            } else { //big zone
                if (gemM->id % 10 == 0)
                    fBigMap = fBigL1;
                else
                    fBigMap = fBigR1;
                if (gemM->channel_high - gemM->channel_low < 128) realChannel = (2048 + ch2048 - gemM->channel_low);
            }
        }
        lay = fBigMap[realChannel].lay;
        mod = gemM->hotZone >> 1;
        strip = fBigMap[realChannel].strip;

        if (strip > 0) {
            BmnGemStripDigit dig;
            dig.SetStation(gemM->station);
            dig.SetModule(mod);
            dig.SetStripLayer(lay);
            dig.SetStripNumber(strip);
            Double_t sig = (GetRun() > GetBoundaryRun(ADC32_N_SAMPLES)) ? ((Double_t) ((adcDig->GetShortValue())[iSmpl] / 16)) : ((Double_t) ((adcDig->GetUShortValue())[iSmpl] / 16));
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

    Double_t*** vPed = GetPedestals();
    Double_t*** vPedRMS = GetPedestalsRMS();

    for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
        if ((candDig[iSmpl]).GetStation() == -1) continue;

        BmnGemStripDigit * dig = &candDig[iSmpl];
        Double_t ped = vPed[iSer][ch][iSmpl];
        Double_t sig = Abs(dig->GetStripSignal() - CMS - ped);
        Float_t threshold = 15 + 4 * vPedRMS[iSer][ch][iSmpl]; //20;
        if (sig < threshold || sig == 0.0) continue; //FIXME: check cases with sig == 0
        if (doFill) {
            fSigProf[dig->GetStation()][dig->GetModule()][dig->GetStripLayer()]->Fill(dig->GetStripNumber());
        } else {
            BmnGemStripDigit * resDig = new((*gem)[gem->GetEntriesFast()]) BmnGemStripDigit(dig->GetStation(), dig->GetModule(), dig->GetStripLayer(), dig->GetStripNumber(), sig);
            if (fNoisyChannels[dig->GetStation()][dig->GetModule()][dig->GetStripLayer()][dig->GetStripNumber()])
                resDig->SetIsGoodDigit(kFALSE);
            else
                resDig->SetIsGoodDigit(kTRUE);
        }
    }
}

ClassImp(BmnGemRaw2Digit)

