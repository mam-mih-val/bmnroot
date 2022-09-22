#include "BmnMscRaw2Digit.h"

BmnMscRaw2Digit::BmnMscRaw2Digit(Int_t period, Int_t run, TString MapFile, TTree *spillTree, TTree *digiSpillTree) {
    fBmnSetup = MapFile.Contains("SRC") ? kSRCSETUP : kBMNSETUP;
    fPeriodId = period;
    fRunId = run;
    SetRawSpillTree(spillTree);
    SetDigSpillTree(digiSpillTree);
    ReadChannelMap(MapFile);
    if ((fBmnSetup == kSRCSETUP) && (fPeriodId == 7)) {// @TODO extend for BM@N
        BmnStatus status = ParseTxtSpillLog(
                fBmnSetup == kSRCSETUP ?
                "$VMCWORKDIR/database/uni_db/macros/parse_schemes/spill_run7/SRC_Data.txt" :
                "$VMCWORKDIR/database/uni_db/macros/parse_schemes/spill_run7/summary_corr_v2.txt",
                fBmnSetup == kSRCSETUP ?
                "$VMCWORKDIR/database/uni_db/macros/parse_schemes/spill_run7/spill_run7_src_full.xslt" :
                "$VMCWORKDIR/database/uni_db/macros/parse_schemes/spill_run7/spill_run7_bmn_full.xslt");
        isValidSpillLog = (status == kBMNSUCCESS);
    }
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

BmnStatus BmnMscRaw2Digit::ParseTxtSpillLog(TString LogName, TString SchemeName) {
    TString beam = "";
    TString target = "";
    Int_t log_shift_bmn = 6;
    Int_t log_shift_src = -3;
    fLogShift = fBmnSetup == kBMNSETUP ? log_shift_bmn : log_shift_src;

    map<TDatime, vector < Int_t>> temp_spill_map;
    UniParser parser;
    vector<structParseValue*> parse_values;
    vector<structParseSchema> vecElements;
    int res_code = parser.ParseTxt2Struct(LogName, SchemeName, parse_values, vecElements, fVerbose);
    if (res_code != 0) {
        cout << endl << "Error: parser error (" << res_code << ")" << endl;
        return kBMNERROR;
    }

    if (parse_values.size() < 1) {
        cout << endl << "Error: there are no lines to parse" << endl;
        return kBMNERROR;
    }

    TObjArray* pRunArray = NULL;
    // select only with a given condition, e.g. target particle
    TObjArray arrayConditions;
    arrayConditions.SetOwner(kTRUE);

    UniSearchCondition* searchCondition = new UniSearchCondition(UniSearchCondition::columnPeriodNumber, UniSearchCondition::conditionEqual, fPeriodId);
    arrayConditions.Add((TObject*) searchCondition);
    if (target != "") {
        searchCondition = new UniSearchCondition(UniSearchCondition::columnTargetParticle, UniSearchCondition::conditionEqual, target); // one can choose any condition: beam, energy...
        arrayConditions.Add((TObject*) searchCondition);
    }
    if (beam != "") {
        searchCondition = new UniSearchCondition(UniSearchCondition::columnBeamParticle, UniSearchCondition::conditionEqual, beam);
        arrayConditions.Add((TObject*) searchCondition);
    }

    int sum_size = parse_values[0]->arrValues.size();

    for (int ind = 0; ind < parse_values.size(); ind++) {
        structParseValue* st = parse_values.at(ind);
        vector<Int_t> vals;
        for (Int_t iValue = 0; iValue < sum_size; iValue++)
            vals.push_back(boost::any_cast<Int_t>(st->arrValues[iValue]));
        st->dtSpillEnd.Set(st->dtSpillEnd.Convert() - fLogShift);
        temp_spill_map.insert(pair<TDatime, vector < Int_t >> (st->dtSpillEnd, vals));
    }

    vector<Long64_t> total_columns;
    for (int i_total = 0; i_total < sum_size; i_total++)
        total_columns.push_back(0);
    // get run time
    UniRun* pRun = UniRun::GetRun(fPeriodId, fRunId);
    if (pRun == NULL) {
        cout << endl << "Error: no experimental run was found " << fPeriodId << " : " << fRunId << endl;
        return kBMNERROR;
    }
    dtStart = pRun->GetStartDatetime();
    TDatime* dateEnd = pRun->GetEndDatetime();
    if (dateEnd == NULL) {
        cout << "Error: no end datetime in the database for this run" << endl;
        delete pRun;
        return kBMNERROR;
    }
    dtEnd = *dateEnd;
    delete pRun;
    //    fSpillMapIter = spill_map.lower_bound(dtStart);
    // check for presence in ELOG
    TObjArray* recs = ElogRecord::GetRecords(fPeriodId, fRunId);
    if (recs == NULL) {
        fprintf(stderr, "ELOG Error!\n");
        return kBMNERROR;
    } else
        if (recs->GetEntries() == 0) {
        fprintf(stderr, "Run %d not found in ELOG!\n", fRunId);
        return kBMNERROR;
    }
    printf("Run %d  %s", fRunId, dtStart.AsSQLString());
    printf(" - %s\n", dtEnd.AsSQLString());
    spill_map.insert(temp_spill_map.lower_bound(dtStart), temp_spill_map.upper_bound(dtEnd));

    return kBMNSUCCESS;

}

void BmnMscRaw2Digit::FillRunHeader(DigiRunHeader *rh) {
    if (rh) {
        rh->SetBT(fBT);
        rh->SetBTnBusy(fBTnBusy);
        rh->SetBTAccepted(fBTAccepted);
        rh->SetAccepted(fAccepted);
        rh->SetL0(fL0);
        rh->SetRawMSC(move(fBoardSums));
//        printf("map len %lu\n",fBoardSums.size());
    }   
}

BmnStatus BmnMscRaw2Digit::SumEvent(TClonesArray *msc, BmnEventHeader *hdr, BmnSpillHeader *shdr, UInt_t &nPedEvBySpill) {
    //    sh->Clear();
    //    if (fRawSpillTree->GetEntriesFast() <= iSpill)
    //        return kBMNERROR;
    BmnTrigInfo *ti = hdr->GetTrigInfo();
    //    if (msc->GetEntriesFast() == 0)
    //        fTempTI = BmnTrigInfo(hdr->GetTrigInfo());
    UInt_t iEv = hdr->GetEventId();
    for (Int_t iAdc = 0; iAdc < msc->GetEntriesFast(); ++iAdc) {
        BmnMSCDigit* dig = (BmnMSCDigit*) msc->At(iAdc);
//        fVerbose = 1;
        if (fVerbose)
            printf("iAdc %d Spill %u last event %u  Curent evId %u \n",
                iAdc, iSpill, dig->GetLastEventId(), iEv);
        if (fPeriodId == 8) {
            auto it = fBoardSums.find(dig->GetSerial());
            if (it == fBoardSums.end()) {
                vector<uint64_t> v(dig->GetValue(), dig->GetValue() + BmnMSCDigit::GetNVals());
                fBoardSums.insert(make_pair(dig->GetSerial(), move(v)));
            } else {
                for (uint8_t i = 0; i < BmnMSCDigit::GetNVals(); i++)
                    it->second[i] += dig->GetValue()[i];
            }
        } else {
            if (dig->GetLastEventId() > iEv)
                return kBMNFINISH;
            if ((dig->GetLastEventId() < iEv) && (dig->GetLastEventId() > 0)) {
                fprintf(stderr, "Spill %u last event %u lost! Curent evId %u \n",
                        iSpill, dig->GetLastEventId(), iEv);
                //            fRawSpillTree->GetEntry(++iSpill);
                ++iSpillMap;
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
                    if ((fBmnSetup == kSRCSETUP) && (fPeriodId == 7) && (isValidSpillLog)) {
                        if (iSpillMap < spill_map.size()) {
                            map<TDatime, vector < Int_t>>::iterator SpillMapIter = spill_map.begin();
                            advance(SpillMapIter, iSpillMap);
                            if (fVerbose)
                                printf(" spill  %s\n", SpillMapIter->first.AsSQLString());
                            vector<Int_t> v = SpillMapIter->second;
                            /** flux ~= sum [BT * (DAQ_Busy -peds)/ (DAQ_TRigger - peds)]*/
                            Double_t BT = v[15];
                            Double_t DAQ_Busy = v[21];
                            Double_t DAQ_Trigger = v[22];
                            if (DAQ_Trigger - nPedEvBySpill != 0)
                                fBTAccepted += BT * (DAQ_Busy - nPedEvBySpill) / (DAQ_Trigger - nPedEvBySpill);
                            if (fVerbose)
                                printf("BT %f DAQ_Busy %f  DAQ_Trigger %f  flux %f\n",
                                    BT, DAQ_Busy, DAQ_Trigger, fBTAccepted);
                        }
                    } else { // BM@N setup or U40 is present
                        UInt_t den =
                                AcceptedReal +
                                ti->GetTrigBefo() +
                                ti->GetTrigAfter();
                        if (den > 0)
                            fBTAccepted += arr[mRec.BTnBusy] * AcceptedReal / (Double_t) den;
                    }
                    fAccepted += AcceptedReal;
//                    //                if (shdr->GetEntriesFast()) {
//                    //                    BmnSpillHeader *sh = static_cast<BmnSpillHeader*> (shdr->At(0));
//                    BmnSpillHeader *sh = new ((*shdr)[shdr->GetEntriesFast()]) BmnSpillHeader();
//                    // BM@N MSC16
//                    sh->SetBC1(arr[mRec.BC1]);
//                    sh->SetBC2(arr[mRec.BC2]);
//                    sh->SetBC3(arr[mRec.BC3]);
//                    sh->SetBT(arr[mRec.BT]);
//                    sh->SetBTnBusy(arr[mRec.BTnBusy]);
//                    sh->SetL0(arr[mRec.L0]);
//                    sh->SetProt(arr[mRec.TriggerProtection]);
//                    //SRC MSC16
//                    sh->SetBC1H(arr[mRec.BC1H]);
//                    sh->SetBC1BP(arr[mRec.BC1BP]);
//                    sh->SetBC1xBC2(arr[mRec.BC1xBC2]);
//                    sh->SetBC1nBusy(arr[mRec.BC1nBusy]);
//                    sh->SetIntTrig(arr[mRec.IntTrig]);
//                    sh->SetSRCTrig(arr[mRec.SRCTrig]);
//                    sh->SetTrignBusy(arr[mRec.TrignBusy]);
//                    // U40 
//                    sh->SetAccepted(AcceptedReal);
//                    sh->SetAfter(ti->GetTrigAfter());
//                    sh->SetBefo(ti->GetTrigBefo());
//                    sh->SetCand(ti->GetTrigCand());
//                    sh->SetAll(ti->GetTrigAll());
//                    sh->SetAvail(ti->GetTrigAvail());
//                    sh->SetRjct(ti->GetTrigRjct());
//
//                    sh->SetLastEventId(iEv);
//                    sh->SetPeriodId(hdr->GetPeriodId());
//                    sh->SetEventTimeTS(hdr->GetEventTimeTS());
//                    //                    fDigSpillTree->Fill();
//                    //                }

                    if (fVerbose) {
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
                    }
                    ++iSpill;
                    ++iSpillMap;
                    //                Int_t r = fRawSpillTree->GetEntry(iSpill);
                    //                //                printf("Get entry %u returned %d\n", iSpill, r);
                    //                if (r <= 0) {
                    //                    //                    fprintf(stderr, "Spill %u read error!\n", iSpill);
                    //                    return kBMNFINISH;
                    //                }
                    nPedEvBySpill = 0;
                    break;
                }
            }
        }
    }
    return kBMNSUCCESS;
}

// for period 7
BmnStatus BmnMscRaw2Digit::SumEvent7(TClonesArray *msc, BmnEventHeader *hdr, BmnSpillHeader *sh, UInt_t &nPedEvBySpill) {
    sh->Clear();
    if (fRawSpillTree->GetEntriesFast() <= iSpill)
        return kBMNERROR;
    BmnTrigInfo *ti = hdr->GetTrigInfo();
    //    if (msc->GetEntriesFast() == 0)
    //        fTempTI = BmnTrigInfo(hdr->GetTrigInfo());
    UInt_t iEv = hdr->GetEventId();
    for (Int_t iAdc = 0; iAdc < msc->GetEntriesFast(); ++iAdc) {
        BmnMSCDigit* dig = (BmnMSCDigit*) msc->At(iAdc);
        if (dig->GetLastEventId() > iEv)
            return kBMNFINISH;
        if ((dig->GetLastEventId() < iEv) && (dig->GetLastEventId() > 0)) {
            fprintf(stderr, "Spill %u last event %u lost! Curent evId %u \n",
                    iSpill, dig->GetLastEventId(), iEv);
            fRawSpillTree->GetEntry(++iSpill);
            ++iSpillMap;
            nPedEvBySpill = 0;
            return kBMNERROR;
        }
                fVerbose = 1;
        if (fVerbose)
            printf("Spill %u last event %u  Curent evId %u \n",
                iSpill, dig->GetLastEventId(), iEv);
        UInt_t *arr = dig->GetValue();
        UInt_t serial = dig->GetSerial();
        for (auto &mRec : fMap) {
            if (mRec.serial == serial) {
                fBT += arr[mRec.BT];
                fBTnBusy += arr[mRec.BTnBusy];
                fProtection += arr[mRec.TriggerProtection];
                fL0 += arr[mRec.L0];
                UInt_t AcceptedReal = ti->GetTrigAccepted() - nPedEvBySpill;
                if ((fBmnSetup == kSRCSETUP) && (fPeriodId == 7) && (isValidSpillLog)) {
                    if (iSpillMap < spill_map.size()) {
                        map<TDatime, vector < Int_t>>::iterator SpillMapIter = spill_map.begin();
                        advance(SpillMapIter, iSpillMap);
                        if (fVerbose)
                            printf(" spill  %s\n", SpillMapIter->first.AsSQLString());
                        vector<Int_t> v = SpillMapIter->second;
                        /** flux ~= sum [BT * (DAQ_Busy -peds)/ (DAQ_TRigger - peds)]*/
                        Double_t BT = v[15];
                        Double_t DAQ_Busy = v[21];
                        Double_t DAQ_Trigger = v[22];
                        if (DAQ_Trigger - nPedEvBySpill != 0)
                            fBTAccepted += BT * (DAQ_Busy - nPedEvBySpill) / (DAQ_Trigger - nPedEvBySpill);
                        if (fVerbose)
                            printf("BT %f DAQ_Busy %f  DAQ_Trigger %f  flux %f\n",
                                BT, DAQ_Busy, DAQ_Trigger, fBTAccepted);
                    }
                } else { // BM@N setup or U40 is present
                    UInt_t den =
                            AcceptedReal +
                            ti->GetTrigBefo() +
                            ti->GetTrigAfter();
                    if (den > 0)
                        fBTAccepted += arr[mRec.BTnBusy] * AcceptedReal / (Double_t) den;
                }
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
                    sh->SetEventTimeTS(hdr->GetEventTimeTS());
                    fDigSpillTree->Fill();
                }

                if (fVerbose) {
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
                }
                ++iSpill;
                ++iSpillMap;
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

