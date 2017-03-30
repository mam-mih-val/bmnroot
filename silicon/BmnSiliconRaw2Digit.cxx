#include "BmnSiliconRaw2Digit.h"

BmnSiliconRaw2Digit::BmnSiliconRaw2Digit() {
    fPeriod = -1;
    fRun = -1;
    fEventId = -1;
}

BmnSiliconRaw2Digit::BmnSiliconRaw2Digit(Int_t period, Int_t run, TString name) {

    fPeriod = period;
    fRun = run;
    fEventId = 0;
    ReadMapFile(name);
}

BmnSiliconRaw2Digit::~BmnSiliconRaw2Digit() {
}

BmnStatus BmnSiliconRaw2Digit::ReadMapFile(TString fName) {
    UInt_t ser = 0;
    Int_t ch = 0;
    Int_t mod = 0;
    Int_t start = 0;
    TString type = "";
    string dummy;

    printf("Reading Silicon mapping file ...\n");
    TString fMapFileName = TString(getenv("VMCWORKDIR")) + TString("/input/") + fName;

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
                for (Int_t iSmpl = 0; iSmpl < ADC128_N_SAMPLES; ++iSmpl) {
                    new((*silicon)[silicon->GetEntriesFast()]) BmnSiliconDigit(0, tM.module, tM.layer, tM.start_strip + iSmpl, (adcDig->GetValue())[iSmpl]);
                }
                break;
            }
        }
    }
}

ClassImp(BmnSiliconRaw2Digit)
