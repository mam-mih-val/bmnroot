#include "BmnMscRaw2Digit.h"

BmnMscRaw2Digit::BmnMscRaw2Digit(TString MapFile, TTree *spillTree, TTree *digiSpillTree) {
    SetRawSpillTree(spillTree);
    SetDigSpillTree(digiSpillTree);
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
    UShort_t slot, bt, btnbusy, l0, tp, bc1, bc2, bc3;

    fMapFile >> dummy >> dummy >> dummy >> dummy >> dummy >> dummy >> dummy >> dummy >> dummy;
    fMapFile >> dummy;
    while (!fMapFile.eof()) {
        fMapFile >> hex >> ser >> dec >> slot >> bt >> btnbusy >> l0 >> tp >> bc1 >> bc2 >> bc3;
        if (!fMapFile.good()) break;
        MscMap record;
        record.serial = ser;
        record.slot = slot;
        record.BT = bt;
        record.BTnBusy = btnbusy;
        record.L0 = l0;
        record.TriggerProtection = tp;
        record.BC1 = bc1;
        record.BC2 = bc2;
        record.BC3 = bc3;
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

BmnStatus BmnMscRaw2Digit::SumEvent(TClonesArray *msc, BmnEventHeader *hdr, BmnSpillHeader *sh, UInt_t &nPedEvBySpill) {
    //    printf("iEv %u  iSpill %u msc->GetEntriesFast() %d\n", iEv, iSpill, msc->GetEntriesFast());
    sh->Clear();
    BmnTrigInfo *ti = hdr->GetTrigInfo();
    UInt_t iEv = hdr->GetEventId();
    for (Int_t iAdc = 0; iAdc < msc->GetEntriesFast(); ++iAdc) {
        BmnMSCDigit* dig = (BmnMSCDigit*) msc->At(iAdc);
        if (dig->GetLastEventId() > iEv)
            break;
        if (dig->GetLastEventId() < iEv) {
            fprintf(stderr, "Spill %u last event lost!\n", iSpill);
            fRawSpillTree->GetEntry(++iSpill);
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
                if (fDigSpillTree) {
                    sh->SetBC1(arr[mRec.BC1]);
                    sh->SetBC2(arr[mRec.BC2]);
                    sh->SetBC3(arr[mRec.BC3]);
                    sh->SetBT(arr[mRec.BT]);
                    sh->SetBTnBusy(arr[mRec.BTnBusy]);
                    sh->SetL0(arr[mRec.L0]);
                    sh->SetProt(arr[mRec.TriggerProtection]);
                    sh->SetAccepted(AcceptedReal);
                    sh->SetAfter(ti->GetTrigAfter());
                    sh->SetBefo(ti->GetTrigBefo());
                    sh->SetCand(ti->GetTrigCand());
                    sh->SetAll(ti->GetTrigAll());
                    sh->SetAvail(ti->GetTrigAvail());
                    sh->SetRjct(ti->GetTrigRjct());
                    sh->SetLastEventId(iEv);
                    sh->SetPeriodId(hdr->GetPeriodId());
                    fDigSpillTree->Fill();
                }

//                printf("iEv %7u  iSpill %4u   last EvId %7u\n", iEv, iSpill, dig->GetLastEventId());
//                hdr->GetEventTimeTS().Print();
//                printf(ANSI_COLOR_BLUE " MSC16:" ANSI_COLOR_RESET"\tBC1  %7u,    BC2  %7u,   BC3  %4u, BeamTrigger %7u,   L0 %7u, TrigProtection %7u, BT&Busy %7u\n",
//                        arr[0],
//                        arr[2],
//                        arr[4],
//                        arr[6],
//                        arr[8],
//                        arr[10],
//                        arr[12]
//                        ); // BM@N
//                printf(ANSI_COLOR_BLUE " U40VE:" ANSI_COLOR_RESET"\t cand %7u,   acc  %7u,   before  %4u,    after  %6u,  rjct %6u,  all %7u,  avail %7u\n\n",
//                        ti->GetTrigCand(),
//                        ti->GetTrigAccepted(),
//                        ti->GetTrigBefo(),
//                        ti->GetTrigAfter(),
//                        ti->GetTrigRjct(),
//                        ti->GetTrigAll(),
//                        ti->GetTrigAvail());
                ++iSpill;
                Int_t r = fRawSpillTree->GetEntry(iSpill);
                //                printf("Get entry %u returned %d\n", iSpill, r);
                if (r <= 0) {
//                    fprintf(stderr, "Spill %u read error!\n", iSpill);
                    return kBMNFINISH;
                }
                nPedEvBySpill = 0;
                break;
            }
        }
    }
    return kBMNSUCCESS;
}


ClassImp(BmnMscRaw2Digit)

