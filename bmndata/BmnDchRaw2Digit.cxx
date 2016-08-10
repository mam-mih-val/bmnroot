#include "BmnDchRaw2Digit.h"

BmnDchRaw2Digit::BmnDchRaw2Digit(Int_t period, Int_t run) {

    fEntriesInMap = 72;
    
    UniDbDetectorParameter* pDetectorParameter1 = UniDbDetectorParameter::GetDetectorParameter("DCH1", "DCH_mapping", period, run);
    if (pDetectorParameter1 != NULL)
        pDetectorParameter1->GetDchMapArray(fMap1, fEntriesInMap);
    UniDbDetectorParameter* pDetectorParameter2 = UniDbDetectorParameter::GetDetectorParameter("DCH2", "DCH_mapping", period, run);
    if (pDetectorParameter2 != NULL)
        pDetectorParameter2->GetDchMapArray(fMap2, fEntriesInMap);

}

void BmnDchRaw2Digit::FillEvent(TClonesArray *tdc, TClonesArray *sync, TClonesArray *dch) {

    for (Int_t i = 0; i < tdc->GetEntriesFast(); i++) {
        BmnTDCDigit *digit = (BmnTDCDigit*) tdc->At(i);
        if (digit->GetType() != DCH_TDC_TYPE) continue;
        if (FindInMap(fMap1, digit, dch) == kBMNERROR)
            FindInMap(fMap2, digit, dch);
    }
}

Int_t BmnDchRaw2Digit::GetChTDC64v(UInt_t tdc, UInt_t ch) {
    //this is some Vishnevsky's magic!
    //FIXME! What is going here?!
    const Int_t tdc64v_tdcch2ch[2][32] = {
        { 31, 15, 30, 14, 13, 29, 28, 12, 11, 27, 26, 10, 25, 9, 24, 8, 23, 7, 22, 6, 21, 5, 20, 4, 19, 3, 18, 2, 17, 1, 16, 0},
        { 31, 15, 30, 14, 29, 13, 28, 12, 27, 11, 26, 10, 25, 9, 24, 8, 23, 7, 22, 6, 21, 5, 20, 4, 19, 3, 18, 2, 17, 1, 16, 0}
    };
    Int_t val = tdc64v_tdcch2ch[tdc - 1][ch];
    if (tdc == 2) val += 32;
    return val;
};

BmnStatus BmnDchRaw2Digit::FindInMap(DchMapStructure* mapArr, BmnTDCDigit* dig, TClonesArray* arr) {
    for (Int_t iMap = 0; iMap < fEntriesInMap; ++iMap) {
        DchMapStructure map = mapArr[iMap];
        if (dig->GetSerial() != map.crate || dig->GetSlot() != map.slot) continue;
        UInt_t ch = GetChTDC64v(dig->GetHptdcId(), dig->GetChannel());
        if (ch > map.channel_high || ch < map.channel_low) continue;
        Float_t tm = dig->GetValue() / 10.0; // - (T0 + (t0time - digittime)); //FIXME!!! Why divide by 10???
        TClonesArray &ar_dch = *arr;
        new(ar_dch[arr->GetEntriesFast()]) BmnDchDigit(map.plane, map.group * 16 + ch - map.channel_low, tm, 0);
        return kBMNSUCCESS;
    }
    return kBMNERROR;
}

ClassImp(BmnDchRaw2Digit)

