/* 
 * File:   BmnZdcBScanner.cxx
 * Author: pnaleks <pnaleks@gmail.com>
 * 
 * Created on 25 октября 2019 г., 12:15
 */

#ifndef BMNZDCBSCANNER_H
#define BMNZDCBSCANNER_H

#include <TClonesArray.h>
#include <TH1D.h>
#include <BmnMath.h>
#include <BmnZDCDigit.h>

class ZdcMoment {
    Int_t fHits = 0;
    Double_t fM = 0;
    Double_t fMX = 0;
    Double_t fMY = 0;
    Double_t fMXX = 0;
    Double_t fMYY = 0;
    Double_t fXc = 0;
    Double_t fYc = 0;
    Double_t fMC = 0;
    Double_t fMD = 0;
public:
    void Reset() { fHits = 0; fM = fMX = fMY = fMXX = fMYY = fMC = fMD = 0.; }
    void SetCenter(Double_t x /*mm*/, Double_t y /*mm*/) {
        fXc = x; fYc = y;
    }
    void Add(BmnZDCDigit * p, Double_t scale = 1.) {
        Int_t chan = p->GetChannel();
        Double_t m = (scale * p->GetAmp())*1e-3; // MeV ->GeV
        if (chan > 104 || m == 0) return;
        
        Double_t x = (p->GetX() - fXc)*1e-3; // mm -> m
        Double_t y = (p->GetX() - fYc)*1e-3; // mm -> m
        
        fHits++;
        fM += m;
        fMX += m*x;
        fMY += m*y;
        fMXX += m*x*x;
        fMYY += m*y*y;
        
        if (chan < 69) fMD += m; else fMC += m;
    }

    Int_t GetHits() { return fHits; }
    Double_t GetE() { return fM; }
    
    Double_t GetXo() { return fM == 0. ? 0. : fMX/fM; }
    Double_t GetYo() { return fM == 0. ? 0. : fMY/fM; }
    Double_t GetR() {
        Double_t x = GetXo();
        Double_t y = GetYo();
        return TMath::Sqrt(x*x + y*y);
    }
    
    Double_t GetIx() {/*GeV*m^2*/
        Double_t Xo = GetXo();
        return (fMXX - 2.0*Xo*fMX + Xo*Xo*fM);
    }
    Double_t GetIy() {/*GeV*m^2*/
        Double_t Yo = GetYo();
        return (fMYY - 2.0*Yo*fMY + Yo*Yo*fM);
    }
    Double_t GetI() { return GetIx() + GetIy(); } /*GeV*m^2*/
    Double_t GetIo() { return (fMXX + fMYY); } /*GeV*m^2*/
    Double_t GetM() { return fM == 0. ? 0. : GetI() / fM; } /*m^2*/
    Double_t GetMo() { return fM == 0. ? 0. : GetIo() / fM; } /*m^2*/
    Double_t GetA() { return fM == 0. ? 0. : (fMC - fMD) / fM; }
};



class BmnZdcBScanner {
    TChain fInpChain;
    UInt_t fMaxEntries = 0;
    UInt_t fScanWidth = 1000;
    Double_t fSimScale = 1.;

    Bool_t fShowProgress = kFALSE;
    
    class ZdcMoment fEventData;

    struct Item {
        Double_t index;
        Double_t b;
    } * fIndex = 0;
    UInt_t fIndexSize = 0;
    
    static bool compareItem(Item i1, Item i2) { 
        return (i1.index < i2.index); 
    }
    
    void OnEvent(TClonesArray* pArrayOfBmnZDCDigit, char mode, Double_t b);
    void OnScan(Int_t n, Double_t scanValue, TH1D* pBH);
public:
    BmnZdcBScanner() : fInpChain("cbmsim") {}
    virtual ~BmnZdcBScanner() {
        if (fIndex) delete fIndex;
    }
    
    void SetShowProgress(Bool_t show) { fShowProgress = show; }
    void SetEventData(TClonesArray * pArrayOfBmnZDCDigit, Double_t scale = 1.);
    void AddSimDigit(const char * fileName) { fInpChain.Add(fileName); }
    void SetSimScale(Double_t scale) { fSimScale = scale; }
    
    void SetScanWidth(UInt_t entries) { fScanWidth = entries; }
    
    virtual TH1D * Scan(char mode);
};

#endif /* BMNZDCBSCANNER_H */

