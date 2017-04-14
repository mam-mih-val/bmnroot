#include "BmnGemRaw2Digit.h"

BmnGemRaw2Digit::BmnGemRaw2Digit() {
}

BmnGemRaw2Digit::BmnGemRaw2Digit(Int_t period, Int_t run, vector<UInt_t> vSer) : BmnAdcProcessor(period, run, "GEM", ADC_N_CHANNELS, ADC32_N_SAMPLES, vSer) {

    cout << "Loading GEM Map from DB: Period " << period << ", Run " << run << "..." << endl;
    
    fEventId = 0;

    fSmall = new BmnGemMap[N_CH_BUF];
    fMid = new BmnGemMap[N_CH_BUF];
    fBigL0 = new BmnGemMap[N_CH_BUF];
    fBigL1 = new BmnGemMap[N_CH_BUF];
    fBigR0 = new BmnGemMap[N_CH_BUF];
    fBigR1 = new BmnGemMap[N_CH_BUF];

    UniDbDetectorParameter* mapPar = UniDbDetectorParameter::GetDetectorParameter("GEM", "GEM_global_mapping", period, run);
    fEntriesInGlobMap = 0;
    if (mapPar != NULL) mapPar->GetGemMapArray(fMap, fEntriesInGlobMap);
    delete mapPar;
//    for (Int_t i = 0; i < fEntriesInGlobMap; ++i)
//        if (find(fSerials.begin(), fSerials.end(), fMap[i].serial) == fSerials.end())
//            fSerials.push_back(fMap[i].serial);
    
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
            if (gemM->hotZone % 2 == 0) {
                if (gemM->id % 10 == 0)
                    fBigMap = fBigL0;
                else
                    fBigMap = fBigR0;
            } else {
                if (gemM->id % 10 == 0) fBigMap = fBigL1;
                else fBigMap = fBigR1;
                if (gemM->channel_high - gemM->channel_low < 128) realChannel = (2048 + ch2048 - gemM->channel_low);
            }
        }
        mod = (gemM->hotZone < 2) ? 0 : 1;
        lay = fBigMap[realChannel].lay;
        strip = fBigMap[realChannel].strip;

        if (gemM->station == 6 && lay > 1)
            printf("strip = %d\n", strip);

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

    Double_t signals[nSmpl];
    Int_t nOk = 0;
    for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
        if ((candDig[iSmpl]).GetStripSignal() == 0) continue;
        signals[iSmpl] = (candDig[iSmpl]).GetStripSignal();
        nOk++;
    }
    Double_t CMS = CalcCMS(signals, nOk);

    Bool_t*** nc = GetNoiseChannels();
    Float_t*** vPed = GetPedestals();
    
    for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
        if ((candDig[iSmpl]).GetStation() == -1) continue;

        if (nc[iSer][ch][iSmpl]) continue;
        BmnGemStripDigit * dig = &candDig[iSmpl];
        Double_t ped = vPed[iSer][ch][iSmpl];
        Double_t sig = dig->GetStripSignal() - CMS - ped;
        Float_t threshold = 20;
        if (sig < threshold) continue;
        new((*gem)[gem->GetEntriesFast()]) BmnGemStripDigit(dig->GetStation(), dig->GetModule(), dig->GetStripLayer(), dig->GetStripNumber(), sig);
    }
}

ClassImp(BmnGemRaw2Digit)

