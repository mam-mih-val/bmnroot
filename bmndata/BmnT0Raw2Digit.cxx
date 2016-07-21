#include "BmnT0Raw2Digit.h"
#include <climits>

BmnT0Raw2Digit::BmnT0Raw2Digit(TString mappingFile) {
    fMapFileName = TString(getenv("VMCWORKDIR")) + TString("/input/") + mappingFile;
    //========== read mapping file            ==========//
    fMapFile.open((fMapFileName).Data());
    if (!fMapFile.is_open()) {
        cout << "Error opening map-file (" << fMapFileName << ")!" << endl;
    }

    TString dummy;
    UInt_t ser, slot;

    fMapFile >> dummy >> dummy;
    fMapFile >> dummy;
    while (!fMapFile.eof()) {
        fMapFile >> hex >> ser >> dec >> slot;
        if (!fMapFile.good()) break;
        BmnT0Mapping record;
        record.serial = ser;
        record.slot = slot;
        fMap.push_back(record);
    }
    fMapFile.close();
    //==================================================//
}

BmnStatus BmnT0Raw2Digit::FillEvent(TClonesArray *tdc, TClonesArray *t0) {
    for (Int_t iMap = 0; iMap < fMap.size(); ++iMap) {
        BmnT0Mapping t0M = fMap[iMap];
        for (Int_t iTdc = 0; iTdc < tdc->GetEntriesFast(); ++iTdc) {
            BmnTDCDigit* tdcDig1 = (BmnTDCDigit*) tdc->At(iTdc);
            if (tdcDig1->GetSerial() != t0M.serial || tdcDig1->GetSlot() != t0M.slot) continue;
            if (!tdcDig1->GetLeading()) continue; // use only leading digits
            BmnTDCDigit* nearestDig = NULL;
            UInt_t nearestTime = UINT_MAX;

            for (Int_t jTdc = 0; jTdc < tdc->GetEntriesFast(); ++jTdc) {
                BmnTDCDigit* tdcDig2 = (BmnTDCDigit*) tdc->At(jTdc);
                if (tdcDig2->GetSerial() != t0M.serial || tdcDig2->GetSlot() != t0M.slot) continue;
                if (tdcDig2->GetLeading()) continue; // use only trailing digits as a pair to leading one
                if (tdcDig1->GetHptdcId() != tdcDig2->GetHptdcId() || tdcDig1->GetChannel() != tdcDig2->GetChannel()) continue; // we need the same hptdc & channel to create pair
                if (tdcDig2->GetValue() < tdcDig1->GetValue()) continue; //time should be positive
                if (tdcDig2->GetValue() - tdcDig1->GetValue() < nearestTime) {
                    nearestTime = tdcDig2->GetValue() - tdcDig1->GetValue();
                    nearestDig = tdcDig2;
                }
            }
            if (nearestDig != NULL) {
                TClonesArray& ar_t0 = *t0;
                new(ar_t0[t0->GetEntriesFast()]) BmnT0Digit(0, tdcDig1->GetChannel(), tdcDig1->GetValue(), nearestDig->GetValue() - tdcDig1->GetValue());
            }
        }
    }
}

ClassImp(BmnT0Raw2Digit)

