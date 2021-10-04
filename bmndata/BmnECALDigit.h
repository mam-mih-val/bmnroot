#ifndef BMNECALDIGIT1_H
#define	BMNECALDIGIT1_H

#include "BmnCaloDigit.h"
#include <TGeoNode.h>


class BmnECALMapElement
{
    ULong_t fAdcId = 0;
    UShort_t fAdcChan = 0;
    Int_t fChan = -1;
    Double_t fX, fY, fZ;
    Double_t fLabX, fLabY, fLabZ;
    Double_t fCoeff = 1.;
    
  public:
    BmnECALMapElement() {};
    virtual ~BmnECALMapElement() {}
    
    void SetAdcMap(Int_t chan, ULong_t adcId, UShort_t adcChan);
    void SetCoeff(Int_t chan, Double_t coeff);
    void SetCoords(TGeoNode * cell, TGeoNode * mother);

    static void PrintTitle();
    void Print();
    
    Bool_t IsAdcMatch(ULong_t adcId, UShort_t adcChan) { return fChan > -1 && fAdcId == adcId && fAdcChan == adcChan+1; }
    
    Int_t GetChan() { return fChan; }
    Float_t GetX() { return fX; }
    Float_t GetY() { return fY; }
    
    Double_t GetLabX() { return fLabX; }
    Double_t GetLabY() { return fLabY; }
    Double_t GetLabZ() { return fLabZ; }
    
    Double_t GetCoeff() { return fCoeff; }
    
    Double_t * EcalCoords() {return &fX;}
    Double_t * LabCoords() {return &fLabX;}
};


class BmnECALDigit : public BmnCaloDigit
{
  public:
    /** Default constructor **/
    BmnECALDigit();

    /** Main constructor **/
    BmnECALDigit(Float_t x, Float_t y, UChar_t ch, Float_t amp);
    
    BmnECALDigit(BmnECALMapElement * e);

    void Set(BmnECALMapElement * e, Float_t amp = 0., Float_t peakAmp = 0., Float_t startTime = 0., Float_t peakTime = 0.);
    
    void SetIX(UChar_t ix)        {}
    void SetIY(UChar_t iy)        {}
    void SetSize(UChar_t size)  {}
    void SetPeakAmp(Float_t amp) { fPeakAmp = amp; }
    void SetPeakTime(Float_t ns) { fPeakTime = ns; }
    void SetStartTime(Float_t ns) { fStartTime = ns; }
    void SetLabCoords(Float_t x, Float_t y, Float_t z) { fLabX= x; fLabY = y; fLabZ = z; }

    UChar_t GetIX()         const  { return (Short_t) (fX / 40.);}
    UChar_t GetIY()         const  { return (Short_t) (fY / 40.);;}
    UChar_t GetSize()      const  { return 1;}
    Float_t GetPeakAmp() const { return fPeakAmp; }
    Float_t GetPeakTime() const { return fPeakTime; }
    Float_t GetStartTime() const { return fStartTime; }
    void GetLabCoords(Float_t &x, Float_t &y, Float_t &z) const { x = fLabX; y = fLabY; z = fLabZ; }
    
    /** Destructor **/
    virtual ~BmnECALDigit() {}

  private:
    Float_t  fPeakAmp;
    Float_t  fPeakTime;
    Float_t  fStartTime;
    Float_t  fLabX, fLabY, fLabZ;

    ClassDef(BmnECALDigit, 3);
};

#endif	/* BMNECALDIGIT1_H */
