#include "BmnTrigRaw2Digit.h"
#include <climits>

const UShort_t kNCHANNELS = 8; // number of channels in one HPTDC

BmnTrigRaw2Digit::BmnTrigRaw2Digit(TString mappingFile, TString INLFile) {
    readMap(mappingFile);
    readINLCorrections(INLFile);
}

BmnTrigRaw2Digit::BmnTrigRaw2Digit(TString mappingFile, TString INLFile, TTree *digiTree) {
    readMap(mappingFile);
    TPRegexp re("TQDC_(\\S+)");
    for (BmnTrigMapping &record : fMap) {
        TString chName = record.name; // channel name in mapping
        TString detName = chName;     // real detector name
        re.Substitute(detName, "$1"); // real detector name
        TBranch* br = digiTree->GetBranch(detName.Data());
        if (!br) {
            TClonesArray *ar = new TClonesArray(BmnTrigDigit::Class());
            ar->SetName(detName.Data());
            digiTree->Branch(detName.Data(), &ar);
            trigArrays.push_back(ar);
            record.branchRef = ar;
        } else
            for (auto tca : trigArrays)
                if (TString(tca->GetName()) == detName) {
                    record.branchRef = tca;
                    break;
                }
        if (chName.Contains("TQDC")) {
            TString adcName = chName;
            re.Substitute(adcName, "TQDC_ADC_$1");// name for corresponding ADC branch
            TBranch* brADC = digiTree->GetBranch(adcName.Data());
            if (!brADC) {
                TClonesArray *arADC = new TClonesArray(BmnTrigWaveDigit::Class());
                arADC->SetName(adcName.Data());
                digiTree->Branch(adcName.Data(), &arADC);
                trigArrays.push_back(arADC);
                record.branchRefADC = arADC;
            }
            for (auto tca : trigArrays)
                if (TString(tca->GetName()) == adcName) {
                    record.branchRefADC = tca;
                    break;
                }
        }
    }
    readINLCorrections(INLFile);
}

BmnStatus BmnTrigRaw2Digit::readMap(TString mappingFile) { // in mapping TQDC channels must appear earlier than TDC
    fMapFileName = TString(getenv("VMCWORKDIR")) + TString("/input/") + mappingFile;
    printf("Reading Triggers mapping file %s...\n", fMapFileName.Data());
    //========== read mapping file            ==========//
    fMapFile.open((fMapFileName).Data());
    if (!fMapFile.is_open()) {
        cout << "Error opening map-file (" << fMapFileName << ")!" << endl;
    }

    TString dummy;
    TString name;
    UInt_t ser;
    Short_t slot, ch, mod;

    fMapFile >> dummy >> dummy >> dummy >> dummy >> dummy;
    fMapFile >> dummy;
    while (!fMapFile.eof()) {
        fMapFile >> name >> mod >> hex >> ser >> dec >> slot >> ch;
        if (!fMapFile.good()) break;
        BmnTrigMapping record;
        record.branchRef = NULL;
        record.branchRefADC = NULL;
        record.name = name;
        record.serial = ser;
        record.module = mod;
        record.slot = slot;
        record.channel = ch;
        fMap.push_back(record);
    }
    fMapFile.close();
}

BmnStatus BmnTrigRaw2Digit::readINLCorrections(TString INLFile) {
    for (int i = 0; i < 72; i++)
        for (int j = 0; j < 1024; j++)
            fINLTable[i][j] = 0.;

    fINLFileName = TString(getenv("VMCWORKDIR")) + TString("/input/") + INLFile;
    //========== read INL file            ==========//
    fINLFile.open((fINLFileName).Data(), ios::in);
    if (!fINLFile.is_open()) {
        cout << "Error opening INL-file (" << fINLFileName << ")!" << endl;
        return kBMNERROR;
    }

    for (int i = 0; i < 72; i++)
        for (int j = 0; j < 1024; j++)
            fINLFile >> fINLTable[i][j];

    fINLFile.close();
    return kBMNSUCCESS;
}

BmnStatus BmnTrigRaw2Digit::FillEvent(TClonesArray *tdc, TClonesArray *adc) {
    for (Int_t iMap = 0; iMap < fMap.size(); ++iMap) {
        BmnTrigMapping tM = fMap[iMap];
        Short_t iMod = tM.module;
        TClonesArray *trigAr = tM.branchRef;
        TClonesArray *trigArADC = tM.branchRefADC;
        if (trigAr)
            for (Int_t iTdc = 0; iTdc < tdc->GetEntriesFast(); ++iTdc) {
                BmnTDCDigit* tdcDig = (BmnTDCDigit*) tdc->At(iTdc);
                if (tdcDig->GetSerial() != tM.serial || tdcDig->GetSlot() != tM.slot) continue;
                if (tdcDig->GetChannel() != tM.channel) continue;
                Double_t time = tdcDig->GetValue() * TDC_CLOCK / 1024;
                Double_t tdcTimestamp = tdcDig->GetTimestamp() * TDC_CLOCK;
                new ((*trigAr)[trigAr->GetEntriesFast()]) BmnTrigDigit(iMod, time, -1.0, tdcTimestamp);
            }
        if (trigArADC)
            for (Int_t iAdc = 0; iAdc < adc->GetEntriesFast(); iAdc++) {
                BmnTQDCADCDigit *adcDig = (BmnTQDCADCDigit*) adc->At(iAdc);
                if (adcDig->GetSerial() != tM.serial || adcDig->GetSlot() != tM.slot) continue;
                if (adcDig->GetChannel() != tM.channel) continue;
                Double_t adcTimestamp = adcDig->GetAdcTimestamp() * ADC_CLOCK_TQDC16VS;
                Double_t trgTimestamp = adcDig->GetTrigTimestamp() * ADC_CLOCK_TQDC16VS;
                new ((*trigArADC)[trigArADC->GetEntriesFast()]) BmnTrigWaveDigit(
                        iMod,
                        adcDig->GetShortValue(),
                        adcDig->GetNSamples(),
                        trgTimestamp,
                        adcTimestamp);
            }
    }
    return kBMNSUCCESS;
}

BmnStatus BmnTrigRaw2Digit::FillEvent(TClonesArray *tdc) {
    for (Int_t iMap = 0; iMap < fMap.size(); ++iMap) {
        BmnTrigMapping tM = fMap[iMap];
        Short_t iMod = tM.module;
        TClonesArray *trigAr = tM.branchRef;
        //        printf("tdc->GetEntriesFast() %d\n", tdc->GetEntriesFast());
        for (Int_t iTdc = 0; iTdc < tdc->GetEntriesFast(); ++iTdc) {
            BmnTDCDigit* tdcDig1 = (BmnTDCDigit*) tdc->At(iTdc);
            if (tdcDig1->GetSerial() != tM.serial || tdcDig1->GetSlot() != tM.slot) continue;
            if (!tdcDig1->GetLeading()) continue; // use only leading digits
            UShort_t rChannel1 = tdcDig1->GetHptdcId() * kNCHANNELS + tdcDig1->GetChannel();
            if (rChannel1 != tM.channel) continue;
            BmnTDCDigit* nearestDig = NULL;
            UInt_t nearestTime = UINT_MAX;
            for (Int_t jTdc = 0; jTdc < tdc->GetEntriesFast(); ++jTdc) {
                if (iTdc == jTdc) continue;
                BmnTDCDigit* tdcDig2 = (BmnTDCDigit*) tdc->At(jTdc);
                if (tdcDig2->GetSerial() != tM.serial || tdcDig2->GetSlot() != tM.slot) continue;
                if (tdcDig2->GetLeading()) continue; // use only trailing digits as a pair to leading one
                UShort_t rChannel2 = tdcDig2->GetHptdcId() * kNCHANNELS + tdcDig2->GetChannel();
                if (rChannel1 != rChannel2) continue; // we need the same hptdc & channel to create pair
                Int_t dTime = tdcDig2->GetValue() - tdcDig1->GetValue();
                if (dTime < 0) continue; //time should be positive
                if (dTime < nearestTime) {
                    nearestTime = dTime;
                    nearestDig = tdcDig2;
                }
            }
            if (nearestDig != NULL) {
                Double_t tL = (tdcDig1->GetValue() + fINLTable[rChannel1][tdcDig1->GetValue() % 1024]) * 24.0 / 1024;
                Double_t tT = (nearestDig->GetValue() + fINLTable[rChannel1][nearestDig->GetValue() % 1024]) * 24.0 / 1024;
                //                printf("OK:   tT = %f    tL = %f\n", tT, tL);
                new ((*trigAr)[trigAr->GetEntriesFast()]) BmnTrigDigit(iMod, tL, tT - tL);
            }
        }
    }
    return kBMNSUCCESS;
}

BmnStatus BmnTrigRaw2Digit::ClearArrays() {
    for (TClonesArray *ar : trigArrays)
        ar->Clear("C");
}

ClassImp(BmnTrigRaw2Digit)

