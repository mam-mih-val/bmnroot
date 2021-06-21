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
    UShort_t slot, bt, btnbusy, l0, tp, bc1, bc2, bc3,
            BC1H, BC1BP, BC1xBC2, BC1nBusy, IntTrig, SRCTrig, TrignBusy;

    fMapFile >> dummy >> dummy >> dummy >> dummy >> dummy >> dummy >> dummy >>
            dummy >> dummy >> dummy >> dummy >> dummy >> dummy >> dummy >>
            dummy >> dummy;
    fMapFile >> dummy;
    while (!fMapFile.eof()) {
        fMapFile >> hex >> ser >>
                dec >> slot >> bt >> btnbusy >> l0 >> tp >> bc1 >> bc2 >> bc3 >>
                BC1H >> BC1BP >> BC1xBC2 >> BC1nBusy >> IntTrig >> SRCTrig >> TrignBusy;
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
        record.BC1H = BC1H;
        record.BC1BP = BC1BP;
        record.BC1xBC2 = BC1xBC2;
        record.BC1nBusy = BC1nBusy;
        record.IntTrig = IntTrig;
        record.SRCTrig = SRCTrig;
        record.TrignBusy = TrignBusy;
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
        rh->SetAccepted(fAccepted);
        rh->SetL0(fL0);
    }
}

BmnStatus BmnMscRaw2Digit::SumEvent(TClonesArray *msc, BmnEventHeader *hdr, BmnSpillHeader *sh, UInt_t &nPedEvBySpill) {
    sh->Clear();
    BmnTrigInfo *ti = hdr->GetTrigInfo();
    UInt_t iEv = hdr->GetEventId();
//    printf("iEv %u  iSpill %u\n", iEv, iSpill);
    for (Int_t iAdc = 0; iAdc < msc->GetEntriesFast(); ++iAdc) {
        BmnMSCDigit* dig = (BmnMSCDigit*) msc->At(iAdc);
//        printf("dig->GetLastEventId() %u  serial %08X\n", dig->GetLastEventId(), dig->GetSerial());
        if (dig->GetLastEventId() > iEv)
            break;
        if (dig->GetLastEventId() < iEv) {
            //            fprintf(stderr, "Spill %u last event %u lost! Curent evId %u \n",
            //                    iSpill, dig->GetLastEventId(), iEv);
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
                fProtection += arr[mRec.TriggerProtection];
                fL0 += arr[mRec.L0];
                UInt_t AcceptedReal = ti->GetTrigAccepted() - nPedEvBySpill;
                UInt_t den =
                        AcceptedReal +
                        ti->GetTrigBefo() +
                        ti->GetTrigAfter();
                if (den > 0)
                    fBTAccepted += arr[mRec.BTnBusy] * AcceptedReal / (Double_t) den;
                fAccepted += AcceptedReal;
                if (fDigSpillTree) {
                    // BM@N MSC16
                    sh->SetBC1(arr[mRec.BC1]);
                    sh->SetBC2(arr[mRec.BC2]);
                    sh->SetBC3(arr[mRec.BC3]);
                    sh->SetBT(arr[mRec.BT]);
                    sh->SetBTnBusy(arr[mRec.BTnBusy]);
                    sh->SetL0(arr[mRec.L0]);
                    sh->SetProt(arr[mRec.TriggerProtection]);
                    //SRC MSC16
                    sh->SetBC1H(arr[mRec.BC1H]);
                    sh->SetBC1BP(arr[mRec.BC1BP]);
                    sh->SetBC1xBC2(arr[mRec.BC1xBC2]);
                    sh->SetBC1nBusy(arr[mRec.BC1nBusy]);
                    sh->SetIntTrig(arr[mRec.IntTrig]);
                    sh->SetSRCTrig(arr[mRec.SRCTrig]);
                    sh->SetTrignBusy(arr[mRec.TrignBusy]);
                    // U40 
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

                printf("iSpill %4u   last EvId %7u  pedestals %d\n",
                        iSpill, dig->GetLastEventId(), nPedEvBySpill);
                hdr->GetEventTimeTS().Print();
                printf(ANSI_COLOR_BLUE " MSC16:" ANSI_COLOR_RESET"\t"
                        "BC1  %7u,    BC2 %7u,   BC3   %4u, BeamTrigger %7u,      L0 %7u, TrigProtection %7u, BT&Busy %7u\n"
                        "\tBC1H %7u,  BC1BP %7u, BC1xBC2 %4u,   BC1nBusy  %7u, IntTrig %7u,      SRCTrig %7u, TrignBusy %7u\n",
                        arr[mRec.BC1],
                        arr[mRec.BC2],
                        arr[mRec.BC3],
                        arr[mRec.BT],
                        arr[mRec.L0],
                        arr[mRec.TriggerProtection],
                        arr[mRec.BTnBusy],
                        arr[mRec.BC1H],
                        arr[mRec.BC1BP],
                        arr[mRec.BC1xBC2],
                        arr[mRec.BC1nBusy],
                        arr[mRec.IntTrig],
                        arr[mRec.SRCTrig],
                        arr[mRec.TrignBusy]
                        ); // BM@N
                printf(ANSI_COLOR_BLUE " U40VE:" ANSI_COLOR_RESET"\tcand %7u,   acc  %7u,   before  %4u,    after  %6u,  rjct %6u,  all %7u,  avail %7u\n\n",
                        ti->GetTrigCand(),
                        ti->GetTrigAccepted(),
                        ti->GetTrigBefo(),
                        ti->GetTrigAfter(),
                        ti->GetTrigRjct(),
                        ti->GetTrigAll(),
                        ti->GetTrigAvail());
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

