#include "BmnMscRaw2Digit.h"

BmnMscRaw2Digit::BmnMscRaw2Digit(TString MapFile, TTree *spillTree) {
    SetSpillTree(spillTree);
    ReadChannelMap(MapFile);
}

BmnStatus BmnMscRaw2Digit::ReadChannelMap(TString mappingFile) {
    fMapFileName = TString(getenv("VMCWORKDIR")) + TString("/input/") + mappingFile;
    printf("Reading MSC16 channel mapping file %s...\n", fMapFileName.Data());
    fMapFile.open((fMapFileName).Data());
    if (!fMapFile.is_open()) {
        fprintf(stderr, "Error opening map-file (%s)!\n", fMapFileName.Data());
        return kBMNERROR;
    }
    TString dummy;
    TString name;
    UInt_t ser;
    UShort_t slot, bt, btnbusy, l0, tp;

    fMapFile >> dummy >> dummy >> dummy >> dummy; // >> dummy >> dummy;
    fMapFile >> dummy;
    while (!fMapFile.eof()) {
        fMapFile >> hex >> ser >> dec >> slot >> bt >> btnbusy; // >> l0 >> tp;
        if (!fMapFile.good()) break;
        MscMap record;
        record.serial = ser;
        record.slot = slot;
        record.BT = bt;
        record.BTnBusy = btnbusy;
        record.L0 = l0;
        record.TriggerProtection = tp;
        fMap.push_back(record);
    }
    fMapFile.close();
    if (fMap.size() == 0) {
        fprintf(stderr, "Map file is empty!\n");
        return kBMNERROR;
    }
    return kBMNSUCCESS;
}

void BmnMscRaw2Digit::FillRunHeader(DigiRunHeader *rh) {
    if (rh) {
        rh->SetBT(fBT);
        rh->SetBTnBusy(fBTnBusy);
        rh->SetBTAccepted(fBTAccepted);
    }
}

BmnStatus BmnMscRaw2Digit::SumEvent(TClonesArray *msc, BmnTrigInfo *ti, UInt_t iEv, UInt_t &nPedEvBySpill) {
    //    printf("iEv %u  iSpill %u msc->GetEntriesFast() %d\n", iEv, iSpill, msc->GetEntriesFast());
    for (Int_t iAdc = 0; iAdc < msc->GetEntriesFast(); ++iAdc) {
        BmnMSCDigit* dig = (BmnMSCDigit*) msc->At(iAdc);
        //                printf("dig->GetLastEventId() %u \n", dig->GetLastEventId());
        if (dig->GetLastEventId() > iEv)
            break;
        if (dig->GetLastEventId() < iEv) {
            //            fprintf(stderr, "Spill %u last event lost!\n", iSpill);
            fRawTreeSpills->GetEntry(++iSpill);
            nPedEvBySpill = 0;
            return kBMNERROR;
        }
        UInt_t *arr = dig->GetValue();
        UInt_t serial = dig->GetSerial();
        for (auto &mRec : fMap) {
            if (mRec.serial == serial) {
                fBT += arr[mRec.BT];
                fBTnBusy += arr[mRec.BTnBusy];
                UInt_t AcceptedReal = ti->GetTrigAccepted() - nPedEvBySpill;
                UInt_t den =
                        AcceptedReal +
                        ti->GetTrigBefo() +
                        ti->GetTrigAfter();
                if (den > 0)
                    fBTAccepted += arr[mRec.BTnBusy] * AcceptedReal / (Double_t) den;
                //                printf("iEv %u  iSpill %u  BT %u BTnB %u  BTAcc %f\n", iEv, iSpill, fBT, fBTnBusy, fBTAccepted);
                fRawTreeSpills->GetEntry(++iSpill);
                nPedEvBySpill = 0;
                break;
            }
        }

    }
    return kBMNSUCCESS;
}


ClassImp(BmnMscRaw2Digit)

