#include "BmnTrigRaw2Digit.h"
#include <climits>

const UShort_t kNCHANNELS = 8; // number of channels in one HPTDC

BmnTrigRaw2Digit::BmnTrigRaw2Digit(TString mappingFile, TString INLFile) {
    readMap(mappingFile);
    readINLCorrections(INLFile);
    //==================================================//
}

BmnTrigRaw2Digit::BmnTrigRaw2Digit(TString mappingFile, TString INLFile, TTree *digiTree) {
    readMap(mappingFile);
    //    fDir = digiTree->mkdir(dirName.Data());
    for (BmnTrigMapping record : fMap) {
        TClonesArray *ar = new TClonesArray(BmnTrigWaveDigit::Class_Name());
        digiTree->Branch(record.name.Data(), &ar);
        trigArrays.push_back(ar);
    }
}

BmnStatus BmnTrigRaw2Digit::readMap(TString mappingFile){
    fMapFileName = TString(getenv("VMCWORKDIR")) + TString("/input/") + mappingFile;
    printf("Reading Triggers mapping file ...\n");
    //========== read mapping file            ==========//
    fMapFile.open((fMapFileName).Data());
    if (!fMapFile.is_open()) {
        cout << "Error opening map-file (" << fMapFileName << ")!" << endl;
    }

    TString dummy;
    TString name;
    UInt_t ser;
    Short_t slot, ch;

    fMapFile >> dummy >> dummy >> dummy >> dummy;
    fMapFile >> dummy;
    while (!fMapFile.eof()) {
        fMapFile >> name >> hex >> ser >> dec >> slot >> ch;
        if (!fMapFile.good()) break;
        BmnTrigMapping record;
        record.name = name;
        record.serial = ser;
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

BmnStatus BmnTrigRaw2Digit::FillEvent(TClonesArray *tdc, TClonesArray *trigger, TClonesArray *t0, TClonesArray *bc1, TClonesArray *bc2, TClonesArray *veto, TClonesArray *fd, TClonesArray *bd, Double_t& t0time, Double_t *t0width) {

    for (Int_t iMap = 0; iMap < fMap.size(); ++iMap) {
        BmnTrigMapping tM = fMap[iMap];
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
                if (tdcDig2->GetValue() < tdcDig1->GetValue()) continue; //time should be positive
                if (tdcDig2->GetValue() - tdcDig1->GetValue() < nearestTime) {
                    nearestTime = tdcDig2->GetValue() - tdcDig1->GetValue();
                    nearestDig = tdcDig2;
                }
            }

            if (nearestDig != NULL) {
                Double_t tL = (tdcDig1->GetValue() + fINLTable[rChannel1][tdcDig1->GetValue() % 1024]) * 24.0 / 1024;
                Double_t tT = (nearestDig->GetValue() + fINLTable[rChannel1][nearestDig->GetValue() % 1024]) * 24.0 / 1024;
                Int_t iMod = tM.name.Contains("_0") ? 0 : (tM.name.Contains("_1")) ? 1 : -1; //FIXME!!! add "module" into map for T0, FD, BC, VC and use it here without checking name!
                if (tM.name == "T0_0" || tM.name == "T0_1") {
                    t0time = tL; //ns
                    if (t0width) *t0width = tT - tL;
                    TClonesArray& ar_t0 = *t0;
                    if (t0) new(ar_t0[t0->GetEntriesFast()]) BmnTrigDigit(iMod, tL, tT - tL);
                } else if (tM.name == "TRIGGER") {
                    TClonesArray& ar_trig = *trigger;
                    if (trigger) new(ar_trig[trigger->GetEntriesFast()]) BmnTrigDigit(0, tL, tT - tL);
                } else if (tM.name == "BC1") {
                    TClonesArray& ar_bc1 = *bc1;
                    if (bc1) new(ar_bc1[bc1->GetEntriesFast()]) BmnTrigDigit(iMod, tL, tT - tL);
                } else if (tM.name == "BC2_0" || tM.name == "BC2_1") {
                    TClonesArray& ar_bc2 = *bc2;
                    if (bc2) new(ar_bc2[bc2->GetEntriesFast()]) BmnTrigDigit(iMod, tL, tT - tL);
                } else if (tM.name == "VETO_0" || tM.name == "VETO_1") {
                    TClonesArray& ar_veto = *veto;
                    if (veto) new(ar_veto[veto->GetEntriesFast()]) BmnTrigDigit(iMod, tL, tT - tL);
                } else if (tM.name == "FD_0" || tM.name == "FD_1") {
                    TClonesArray& ar_fd = *fd;
                    if (fd) new(ar_fd[fd->GetEntriesFast()]) BmnTrigDigit(iMod, tL, tT - tL);
                } else {
                    for (Int_t i = 0; i < KNBDCHANNELS; ++i)
                        if (tM.name == TString(Form("BD%d", i))) {
                            TClonesArray& ar_bd = *bd;
                            if (bd) new(ar_bd[bd->GetEntriesFast()]) BmnTrigDigit(i, tL, tT - tL);
                        }
                }
            }
        }
    }
}

BmnStatus BmnTrigRaw2Digit::FillEvent(TClonesArray *tdc, TClonesArray *adc) {
    for (Int_t iMap = 0; iMap < fMap.size(); ++iMap) {
        BmnTrigMapping tM = fMap[iMap];
        Short_t iMod = 0;
        Short_t iTime = 0;
        Short_t iAmp = 0;
        for (Int_t iTdc = 0; iTdc < tdc->GetEntriesFast(); ++iTdc) {
            BmnTDCDigit* tdcDig1 = (BmnTDCDigit*) tdc->At(iTdc);
            iTime = tdcDig1->GetValue();
            UInt_t iChannel = tdcDig1->GetChannel();
//            if (tdcDig1->GetSerial() != tM.serial || tdcDig1->GetSlot() != tM.slot) continue;
            if (iChannel != tM.channel) continue;
            for (Int_t iAdc = 0; iAdc < adc->GetEntriesFast(); iAdc++){
                BmnADCSRCDigit *adcDig = (BmnADCSRCDigit*)adc->At(iAdc);
                if (iChannel == adcDig->GetChannel()){
                    TClonesArray *trigAr = trigArrays[iMap];
                    if (trigAr)
                        new ((*trigAr)[trigAr->GetEntriesFast()])
                                BmnTrigWaveDigit(iMod, iTime, iAmp, adcDig);
                    break;
                }
            }
            // @TODO What if corresponding adc digit not found??
        }
    }
    return kBMNSUCCESS;
}

BmnStatus BmnTrigRaw2Digit::ClearArrays() {
    for (TClonesArray *ar : trigArrays)
        ar->Delete();
}

ClassImp(BmnTrigRaw2Digit)

