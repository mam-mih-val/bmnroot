#include "BmnTrigRaw2Digit.h"
#include <climits>

const UShort_t kNCHANNELS = 8; // number of channels in one HPTDC

BmnTrigRaw2Digit::BmnTrigRaw2Digit(TString mappingFile, TString INLFile) {
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
    readINLCorrections(INLFile);
    //==================================================//
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

BmnStatus BmnTrigRaw2Digit::FillEvent(TClonesArray *tdc, TClonesArray *t0, TClonesArray *bc1, TClonesArray *bc2, TClonesArray *veto, TClonesArray *fd, TClonesArray *bd, Double_t& t0time) {

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

                if (tM.name == "T0") {
                    t0time = (tdcDig1->GetValue() + fINLTable[rChannel1][tdcDig1->GetValue() % 1024]) * 24.0 / 1024; //ns
                    TClonesArray& ar_t0 = *t0;
                    if (t0) new(ar_t0[t0->GetEntriesFast()]) BmnTrigDigit(0, tL, tT - tL);
                } else if (tM.name == "BC1") {
                    TClonesArray& ar_bc1 = *bc1;
                    if (bc1) new(ar_bc1[bc1->GetEntriesFast()]) BmnTrigDigit(0, tL, tT - tL);
                } else if (tM.name == "BC2") {
                    TClonesArray& ar_bc2 = *bc2;
                    if (bc2) new(ar_bc2[bc2->GetEntriesFast()]) BmnTrigDigit(0, tL, tT - tL);
                } else if (tM.name == "VETO") {
                    TClonesArray& ar_veto = *veto;
                    if (veto) new(ar_veto[veto->GetEntriesFast()]) BmnTrigDigit(0, tL, tT - tL);
                } else if (tM.name == "FD") {
                    TClonesArray& ar_fd = *fd;
                    if (fd) new(ar_fd[fd->GetEntriesFast()]) BmnTrigDigit(0, tL, tT - tL);
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

ClassImp(BmnTrigRaw2Digit)

