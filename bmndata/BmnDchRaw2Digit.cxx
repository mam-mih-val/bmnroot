#include "BmnDchRaw2Digit.h"

const UChar_t kNSERIALS = 2;
const UInt_t kSERIALS[kNSERIALS] = {0x46f1f79, 0x168e5c5};

BmnDchRaw2Digit::BmnDchRaw2Digit(TString mappingFile) {
    fType = 0x10; //modId of TDC64V (for DCH)
    fMapFileName = TString(getenv("VMCWORKDIR")) + TString("/input/") + mappingFile;
    static const Char_t * tmp[16] = {
        "VA_1", "VB_1", "UA_1", "UB_1", "YA_1", "YB_1", "XA_1", "XB_1",
        "VA_2", "VB_2", "UA_2", "UB_2", "YA_2", "YB_2", "XA_2", "XB_2"
    };
    for (Int_t i = 0; i < 16; i++)
        sprintf(Bmn_DCH_names[i], "%s", tmp[i]);
    TString dummy, plane;
    UInt_t id;
    Int_t group, slot, chan_hi, chan_lo;

    fMapFile.open(fMapFileName.Data());
    fMapFile >> dummy >> dummy >> dummy >> dummy >> dummy >> dummy;
    fMapFile >> dummy;
    while (!fMapFile.eof()) {
        fMapFile >> plane >> group >> hex >> id >> dec >> slot >> chan_lo >> chan_hi;
        if (!fMapFile.good()) break;

        UInt_t idx = 0;
        for (idx = 0; idx < kNSERIALS; ++idx)
            if (kSERIALS[idx] == id) break;
        if (idx == 2) continue;

        Int_t i;
        for (i = 0; i < 16; i++)
            if (plane.Contains(Bmn_DCH_names[i])) break;
        if (i == 16) continue;
        for (Int_t j = chan_lo; j <= chan_hi; j++)
            dhc_map_element[i][group][j % 16].set(idx, (UChar_t) slot, (UChar_t) j);
    }
    fMapFile.close();

    for (Int_t iSer = 0; iSer < kNSERIALS; ++iSer)
        for (Int_t iSlot = 0; iSlot < 21; ++iSlot)
            for (Int_t iCh = 0; iCh < 64; ++iCh)
                rel[iSer][iSlot][iCh] = -1;
    for (Int_t i = 0; i < 16; i++)
        for (Int_t j = 0; j < 16; j++)
            for (Int_t k = 0; k < 16; k++)
                rel[dhc_map_element[i][j][k].id][dhc_map_element[i][j][k].slot][dhc_map_element[i][j][k].chan] = (i << 8) | (j << 4) | k;
}

void BmnDchRaw2Digit::Clear() {
    for (Int_t i = 0; i < 16; i++)
        for (Int_t j = 0; j < 16; j++)
            for (Int_t k = 0; k < 16; k++)
                data[i][j][k].clear();
    for (Int_t i = 0; i < 16; i++)
        digits[i] = 0;
    T0 = 0;
    EVENT = TIME_SEC = TIME_NS = 0;
}

Char_t *BmnDchRaw2Digit::GetPlaneName(Int_t i) {
    return (Char_t *) Bmn_DCH_names[i & 0xF];
}

void BmnDchRaw2Digit::GetEventInfo(Long64_t *ev, Long64_t *t1, Long64_t *t2) {
    *ev = EVENT;
    *t1 = TIME_SEC;
    *t2 = TIME_NS;
}

Int_t BmnDchRaw2Digit::Set(ULong_t id, UChar_t slot, UChar_t ch, Int_t val) {
    //    if (rel[slot][ch] == 0) return 0;
    //    Int_t i = (rel[slot][ch] >> 8) & 0xF;
    //    Int_t j = (rel[slot][ch] >> 4) & 0xF;
    //    Int_t k = (rel[slot][ch]) & 0xF;
    //    data[i][j][k].set(val);
    //    digits[i]++;
    //    return 1;
}

void BmnDchRaw2Digit::Print() {
    cout << "Plane\tGroup\tCrate#\t\tSlot\tCh_Lo\tCh_hi\n";
    cout << "=====================================================\n";
    for (Int_t i = 0; i < 16; i++) {
        for (Int_t j = 0; j < 16; j++)
            for (Int_t k = 0; k < 1; k++) {
                if (dhc_map_element[i][j][k].id) {
                    cout << Bmn_DCH_names[i] << "\t" << j << "\t0x" << std::hex << dhc_map_element[i][j][k].id << std::dec << "\t";
                    cout << (Int_t) dhc_map_element[i][j][k].slot << "\t" << (Int_t) dhc_map_element[i][j][k].chan << "\t" << (Int_t) dhc_map_element[i][j][k].chan + 15 << "\n";
                }
            }
        cout << "\n";
    }
}

//void BmnDchRaw2Digit::FillEvent(TClonesArray *dataArray) {
//    Clear();
//    for (Int_t i = 0; i < dataArray->GetEntriesFast(); i++) {
//        BmnTDCDigit *digit = (BmnTDCDigit*) dataArray->At(i);
//        set(digit->GetSerial(), digit->GetSlot(), digit->GetChannel(), digit->GetValue() / 10.0);
//    }
//}

//void BmnDchRaw2Digit::FillEvent(TClonesArray *dataArray, TClonesArray *sync, TClonesArray *t0) {
//    UInt_t t0id = 0;
//    Long64_t t0time = 0, digittime = 0;
//    Clear();
//    for (Int_t i = 0; i < t0->GetEntriesFast(); i++) {
//        BmnTDCDigit *digit = (BmnTDCDigit*) t0->At(i);
//        if (digit->GetLeading()) {
//            T0 = digit->GetValue() / 42.6666;
//            t0id = digit->GetSerial();
//            break;
//        }
//    }
//    for (Int_t i = 0; i < dataArray->GetEntriesFast(); i++) {
//        BmnTDCDigit *digit = (BmnTDCDigit*) dataArray->At(i);
//        for (Int_t j = 0; j < sync->GetEntriesFast(); j++) {
//            BmnSyncDigit *rec = (BmnSyncDigit*) sync->At(j);
//            if (rec->GetSerial() == t0id) {
//                EVENT = rec->GetEvent();
//                TIME_SEC = rec->GetTime_sec();
//                TIME_NS = rec->GetTime_ns();
//                t0time = TIME_SEC * 1000000000LL + TIME_NS;
//            }
//            if (rec->GetSerial() == digit->GetSerial()) digittime = rec->GetTime_sec() * 1000000000LL + rec->GetTime_ns();
//        }
//        set(digit->GetSerial(), digit->GetSlot(), digit->GetChannel(), digit->GetValue() / 10.0 - (T0 + (t0time - digittime)));
//    }
//}

//void BmnDchRaw2Digit::FillEvent(TClonesArray *dataArray, TClonesArray *sync, TClonesArray *t0, TClonesArray *dhcdigit) {
//    UInt_t t0id = 0;
//    Long64_t t0time = 0, digittime = 0;
//    Clear();
//    for (Int_t i = 0; i < t0->GetEntriesFast(); i++) {
//        BmnTDCDigit *digit = (BmnTDCDigit*) t0->At(i);
//        if (digit->GetLeading()) {
//            T0 = digit->GetValue() / 42.6666;
//            t0id = digit->GetSerial();
//            break;
//        }
//    }
//    for (Int_t i = 0; i < dataArray->GetEntriesFast(); i++) {
//        BmnTDCDigit *digit = (BmnTDCDigit*) dataArray->At(i);
//        for (Int_t j = 0; j < sync->GetEntriesFast(); j++) {
//            BmnSyncDigit *rec = (BmnSyncDigit*) sync->At(j);
//            if (rec->GetSerial() == t0id) {
//                EVENT = rec->GetEvent();
//                TIME_SEC = rec->GetTime_sec();
//                TIME_NS = rec->GetTime_ns();
//                t0time = TIME_SEC * 1000000000LL + TIME_NS;
//            }
//            if (rec->GetSerial() == digit->GetSerial()) digittime = rec->GetTime_sec() * 1000000000LL + rec->GetTime_ns();
//        }
//        Int_t plane = (rel[digit->GetSlot()][digit->GetChannel()] >> 8)&0xF;
//        Int_t group = (rel[digit->GetSlot()][digit->GetChannel()] >> 4)&0xF;
//        Int_t chan = (rel[digit->GetSlot()][digit->GetChannel()])&0xF;
//        Float_t tm = digit->GetValue() / 10.0 - (T0 + (t0time - digittime));
//        TClonesArray &ar_dch = *dhcdigit;
//        new(ar_dch[dhcdigit->GetEntriesFast()]) BmnDchDigit(plane, group * 16 + chan, tm, 0);
//    }
//}

void BmnDchRaw2Digit::FillEvent(TClonesArray *tdc, TClonesArray *sync, TClonesArray *dch) {
    UInt_t t0id = 0;
    Long64_t t0time = 0, digittime = 0;
    Clear();
    for (Int_t i = 0; i < tdc->GetEntriesFast(); i++) {
        BmnTDCDigit *digit = (BmnTDCDigit*) tdc->At(i);
        if (digit->GetType() != fType) continue;
    
        UInt_t idx = 0;
        for (idx = 0; idx < kNSERIALS; ++idx)
            if (kSERIALS[idx] == digit->GetSerial()) break;
        if (idx == 2) continue;

        //        for (Int_t j = 0; j < sync->GetEntriesFast(); j++) {
        //            BmnSyncDigit *rec = (BmnSyncDigit*) sync->At(j);
        //            if (rec->GetSerial() == t0id) {
        //                EVENT = rec->GetEvent();
        //                TIME_SEC = rec->GetTime_sec();
        //                TIME_NS = rec->GetTime_ns();
        //                t0time = TIME_SEC * 1000000000LL + TIME_NS;
        //            }
        //            if (rec->GetSerial() == digit->GetSerial()) digittime = rec->GetTime_sec() * 1000000000LL + rec->GetTime_ns();
        //        }
        //        cout << (Int_t)digit->GetChannel() << endl;
        //cout << bitset<32>(rel[digit->GetSlot()][digit->GetChannel()]) << endl;
        UChar_t sl = digit->GetSlot();
        UChar_t ch = digit->GetChannel();
        if (digit->GetHptdcId() == 2) ch += 32;
        UInt_t code = rel[idx][sl][ch];
        Int_t plane = (code >> 8) & 0xF;
        Int_t group = (code >> 4) & 0xF;
        Int_t chan = code & 0xF;
        Float_t tm = digit->GetValue() / 10.0; // - (T0 + (t0time - digittime));
        TClonesArray &ar_dch = *dch;
        new(ar_dch[dch->GetEntriesFast()]) BmnDchDigit(plane, group * 16 + chan, tm, 0);
    }
}

ClassImp(BmnDchRaw2Digit)

