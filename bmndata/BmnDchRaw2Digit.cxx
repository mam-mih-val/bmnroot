#include "BmnDchRaw2Digit.h"

BmnDchRaw2Digit::BmnDchRaw2Digit(Int_t period, Int_t run) {

    fEntriesInMap1 = 0;
    fEntriesInMap2 = 0;
    
    cout << "Loading the DCH Map from DB: Period " << period << ", Run " << run << "..." << endl;
    
    UniDbDetectorParameter* pDetectorParameter1 = UniDbDetectorParameter::GetDetectorParameter("DCH1", "DCH_mapping", period, run);
    if (pDetectorParameter1 != NULL)
        pDetectorParameter1->GetDchMapArray(fMap1, fEntriesInMap1);
    UniDbDetectorParameter* pDetectorParameter2 = UniDbDetectorParameter::GetDetectorParameter("DCH2", "DCH_mapping", period, run);
    if (pDetectorParameter2 != NULL)
        pDetectorParameter2->GetDchMapArray(fMap2, fEntriesInMap2);
    delete pDetectorParameter1;
    delete pDetectorParameter2;
}

void BmnDchRaw2Digit::FillEvent(TClonesArray *tdc, map<UInt_t, Long64_t> *ts, TClonesArray *dch, Double_t t0) {
    
    for (Int_t i = 0; i < tdc->GetEntriesFast(); i++) {
        BmnTDCDigit *digit = (BmnTDCDigit*) tdc->At(i);
        if (digit->GetType() != DCH_TDC_TYPE) continue;
        map<UInt_t, Long64_t>::iterator it = ts->find(digit->GetSerial());
        Long64_t timeShift = (it != ts->end()) ? it->second : 0;
        if (FindInMap(digit, dch, timeShift, t0, 1) == kBMNERROR)
            FindInMap(digit, dch, timeShift, t0, 2);
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

BmnStatus BmnDchRaw2Digit::FindInMap(BmnTDCDigit* dig, TClonesArray* arr, Long64_t ts, Double_t t0, Int_t dchId) {
    DchMapStructure* mapArr = (dchId == 1) ? fMap1 : fMap2; 
    Int_t nEntriesInMap = (dchId == 1) ? fEntriesInMap1 : fEntriesInMap2; 
    for (Int_t iMap = 0; iMap < nEntriesInMap; ++iMap) {
        DchMapStructure map = mapArr[iMap];
        if (dig->GetSerial() != map.crate || dig->GetSlot() != map.slot) continue;
        UInt_t ch = GetChTDC64v(dig->GetHptdcId(), dig->GetChannel());
        if (ch > map.channel_high || ch < map.channel_low) continue;
        Double_t tm = dig->GetValue() / 10.0 - t0 + ts; //divide by 10 for conversion (100 ps -> ns)
        new((*arr)[arr->GetEntriesFast()]) BmnDchDigit(map.plane, map.group * 16 + ch - map.channel_low, tm, 0);
        return kBMNSUCCESS;
    }
    return kBMNERROR;
}

ClassImp(BmnDchRaw2Digit)

