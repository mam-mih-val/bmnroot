/* 
 * File:   BmnZdcBScanner.cxx
 * Author: pnaleks <pnaleks@gmail.com>
 * 
 * Created on 25 октября 2019 г., 12:15
 */

#include <BmnEventHeader.h>
#include <FairMCEventHeader.h>

#include "BmnZdcBScanner.h"

void BmnZdcBScanner::SetEventData(TClonesArray* pArrayOfBmnZDCDigit, Double_t scale) {
    Int_t N = pArrayOfBmnZDCDigit->GetEntries();
    printf("SetEventData from %d entries\n",N);
    fEventData.Reset();
    for (Int_t i = 0; i < N; i++) {
        BmnZDCDigit * p = (BmnZDCDigit *)pArrayOfBmnZDCDigit->At(i);
        if (p == 0) continue;
        fEventData.Add(p,scale);
    }
}

TH1D* BmnZdcBScanner::Scan(char mode) {
    printf("Starting scan\n");
    TString name = TString::Format("BSpectrum_%c",mode);
    TString title = TString::Format("b-spectrum from %c",mode);
    TH1D * pH = new TH1D(name.Data(),title.Data(),100,0,20);
    
    TClonesArray * pArrayOfBmnZDCDigit = new TClonesArray("BmnZDCDigit");
    fInpChain.SetBranchAddress("ZDC",&pArrayOfBmnZDCDigit);

    FairMCEventHeader * pHeader = new FairMCEventHeader();
    fInpChain.SetBranchAddress("MCEventHeader.",&pHeader);
    
    UInt_t entries = fInpChain.GetEntries();
    if (fMaxEntries && entries > fMaxEntries) entries = fMaxEntries;
    
    printf("Started for %u entries\n", entries);

    if (fIndex) delete fIndex;
    
    fIndexSize = 0;
    fIndex = new Item[entries];
    if (!fIndex) throw;

    UInt_t percent = entries / 100;
    for (UInt_t i = 0; i < entries; i++) {
        fInpChain.GetEntry(i);

        OnEvent(pArrayOfBmnZDCDigit,mode,pHeader->GetB());
        
        if (fShowProgress && percent > 0 && i%percent == 0) DrawBar(i, entries);
    }

    std::sort(fIndex, fIndex + fIndexSize, BmnZdcBScanner::compareItem);

    Double_t scanValue;
    switch (mode) {
        case 'A':
            scanValue = fEventData.GetA();
            break;
        case 'E':
            scanValue = fEventData.GetE();
            break;
        case 'H':
            scanValue = fEventData.GetHits();
            break;
        case 'M':
            scanValue = fEventData.GetM();
            break;
        default:
            printf("Unexpected mode '%c'\n",mode);
            throw;
    }
    
    OnScan(fScanWidth,scanValue,pH);
    
    return pH;
}

void BmnZdcBScanner::OnEvent(TClonesArray* pArrayOfBmnZDCDigit, char mode, Double_t b) {
    Int_t N = pArrayOfBmnZDCDigit->GetEntries();
    
    ZdcMoment MoI;
    
    for (Int_t i = 0; i < N; i++) {
        BmnZDCDigit * p = (BmnZDCDigit*)pArrayOfBmnZDCDigit->At(i);
        if (p == 0) continue;
        MoI.Add(p,fSimScale);
    }
    
    if (MoI.GetE() == 0.) return;
    
    Item * p = fIndex + fIndexSize++;
    switch (mode) {
        case 'A':
            p->index = MoI.GetA();
            break;
        case 'E':
            p->index = MoI.GetE();
            break;
        case 'H':
            p->index = MoI.GetHits();
            break;
        case 'M':
            p->index = MoI.GetM();
            break;
        default:
            printf("Unexpected mode '%c'\n",mode);
            throw;
    }
    
    p->b = b;
}

void BmnZdcBScanner::OnScan(Int_t n, Double_t scanValue, TH1D* pBH) {
    if (n > fIndexSize) n = fIndexSize;        
    Int_t cnt = 0;

    UInt_t iFwd = 0;
    UInt_t iRew = 0;
    Double_t dPos = fIndex[iFwd].index - scanValue;
    Double_t dNeg = 0;

    Double_t bS = 0;
    Double_t bSS = 0;

    while (dPos <= 0) {
        iFwd++;
        if (iFwd >= fIndexSize) return;
        dPos = fIndex[iFwd].index - scanValue;
    }

    if (iFwd > 0) {
        iRew = iFwd-1;
        dNeg = scanValue - fIndex[iRew].index;
    }

    while (cnt < n) {
        Int_t i = -1;
        if (dNeg > 0 && (dNeg < dPos || iFwd >= fIndexSize-1)) {
            if (iRew > 0) {
                iRew--;
                i = iRew;
                dNeg = scanValue - fIndex[iRew].index;
            }
        } else {
            if (iFwd < fIndexSize-1) {
                iFwd++;
                i = iFwd;
                dPos = fIndex[iFwd].index - scanValue;
            }
        }
        if (i < 0) {
            return;
        } else {
            cnt++;
            Double_t b = fIndex[i].b;
            if (pBH) pBH->Fill(b);
        }
    }
}
