#include "BmnSiliconRaw2Digit.h"

BmnSiliconRaw2Digit::BmnSiliconRaw2Digit() {
    fEventId = -1;
}

BmnSiliconRaw2Digit::BmnSiliconRaw2Digit(Int_t period, Int_t run, vector<UInt_t> vSer) : BmnAdcProcessor(period, run, "SILICON", ADC_N_CHANNELS, ADC128_N_SAMPLES, vSer) {

    cout << "Loading SILICON Map from FILE: Period " << period << ", Run " << run << "..." << endl;

    fEventId = 0;
    ReadMapFile();
}

BmnSiliconRaw2Digit::~BmnSiliconRaw2Digit() {
}

BmnStatus BmnSiliconRaw2Digit::ReadMapFile() {
    UInt_t ser = 0;
    Int_t ch = 0;
    Int_t mod = 0;
    Int_t start = 0;
    TString type = "";
    string dummy;

    TString fMapFileName = TString(getenv("VMCWORKDIR")) + TString("/input/SILICON_map_run6.txt");

    ifstream inFile(fMapFileName.Data());
    if (!inFile.is_open())
        cout << "Error opening map-file (" << fMapFileName << ")!" << endl;
    getline(inFile, dummy); //comment line in input file
    getline(inFile, dummy); //comment line in input file

    while (!inFile.eof()) {
        inFile >> ch >> mod >> type >> std::hex >> ser >> std::dec >> start;
        if (!inFile.good()) break;
        BmnSiliconMapping record;
        record.layer = (type == "X") ? 0 : 1;
        record.serial = ser;
        record.module = mod;
        record.channel = ch;
        record.start_strip = start;
        fMap.push_back(record);
    }
}

BmnStatus BmnSiliconRaw2Digit::FillEvent(TClonesArray *adc, TClonesArray *silicon) {
    fEventId++;
    for (Int_t iMap = 0; iMap < fMap.size(); ++iMap) {
        BmnSiliconMapping tM = fMap[iMap];
        for (Int_t iAdc = 0; iAdc < adc->GetEntriesFast(); ++iAdc) {
            BmnADCDigit* adcDig = (BmnADCDigit*) adc->At(iAdc);
            if (adcDig->GetSerial() == tM.serial && adcDig->GetChannel() == tM.channel) {
                ProcessDigit(adcDig, &tM, silicon);
                break;
            }
        }
    }
}

void BmnSiliconRaw2Digit::ProcessDigit(BmnADCDigit* adcDig, BmnSiliconMapping* silM, TClonesArray *silicon) {
    const UInt_t nSmpl = adcDig->GetNSamples();
    UInt_t ch = adcDig->GetChannel();
    UInt_t ser = adcDig->GetSerial();

    Int_t iSer = -1;
    for (iSer = 0; iSer < GetSerials().size(); ++iSer)
        if (ser == GetSerials()[iSer]) break;

    BmnSiliconDigit candDig[nSmpl];

    for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
        BmnSiliconDigit dig;
        dig.SetStation(0);
        dig.SetModule(silM->module - 1);
        dig.SetStripLayer(silM->layer);
        dig.SetStripNumber(silM->start_strip + iSmpl);
        if (GetRun() > 1542)
            dig.SetStripSignal((adcDig->GetShortValue())[iSmpl] / 16);
        else
            dig.SetStripSignal((adcDig->GetUShortValue())[iSmpl] / 16);
        candDig[iSmpl] = dig;
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
        BmnSiliconDigit * dig = &candDig[iSmpl];
        Double_t ped = vPed[iSer][ch][iSmpl];
        Double_t sig = Abs(dig->GetStripSignal() - CMS - ped);
        Float_t threshold = 160;
        if (sig < threshold) continue;
        new((*silicon)[silicon->GetEntriesFast()]) BmnSiliconDigit(dig->GetStation(), dig->GetModule(), dig->GetStripLayer(), dig->GetStripNumber(), sig);
    }

}

ClassImp(BmnSiliconRaw2Digit)
