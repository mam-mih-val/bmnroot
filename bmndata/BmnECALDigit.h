#ifndef BMNECALDIGIT1_H
#define	BMNECALDIGIT1_H

#include <map>
#include <iostream>
#include <fstream>

#include "TNamed.h"
#include "TArrayS.h"

using namespace std;

class BmnECALMapElement {
    ULong_t fAdcId;
    UShort_t fAdcChan;
    Int_t fChan;
    Short_t fIX;
    Short_t fIY;
    Float_t fX;
    Float_t fY;
    
public:
    BmnECALMapElement() { fChan = -1; }
    virtual ~BmnECALMapElement() {}
    
    Bool_t Scan(ifstream &in);

    void Print();
    
    Int_t GetChan() { return fChan; }
    Float_t GetIX() { return fIX; }
    Float_t GetIY() { return fIY; }
    Float_t GetX() { return fX; }
    Float_t GetY() { return fY; }
};

class BmnECALMap {
    
    std::map<Int_t,BmnECALMapElement> fMap;
    
public:
    
    BmnECALMapElement& Get(Int_t chan)  { return fMap[chan]; }
    
    Bool_t Load();
};

class BmnECALDigit : public TNamed
{
  public:
    /** Default constructor **/
    BmnECALDigit();

    /** Main constructor **/
    BmnECALDigit(Float_t x,Float_t y,Short_t ch,Float_t amp);
    
    BmnECALDigit(BmnECALMapElement * e, Float_t amp);

    void SetIX(UChar_t ix)        {}
    void SetIY(UChar_t iy)        {}
    void SetX(Float_t x)        { fX = x;        }
    void SetY(Float_t y)        { fY = y;        }
    void SetSize(UChar_t size)  {}
    void SetChannel(Short_t ch) { fChannel = ch; }
    void SetAmp(Float_t amp) { fAmp = amp; }
    void SetPeakAmp(Float_t amp) { fPeakAmp = amp; }
    void SetPeakTime(Float_t ns) { fPeakTime = ns; }
    void SetStartTime(Float_t ns) { fStartTime = ns; }
    void SetLabCoords(Float_t x, Float_t y, Float_t z) { fLabX= x; fLabY = y; fLabZ = z; }

    UChar_t GetIX()         const  { return (Short_t) (fX / 40.);}
    UChar_t GetIY()         const  { return (Short_t) (fY / 40.);;}
    Float_t GetX()        const  { return fX;}
    Float_t GetY()        const  { return fY;}
    UChar_t GetSize()      const  { return 1;}
    Short_t GetChannel()   const  { return fChannel;}
    Float_t GetAmp()  const  { return fAmp;}
    Float_t GetPeakAmp() { return fPeakAmp; }
    Float_t GetPeakTime() { return fPeakTime; }
    Float_t GetStartTime() { return fStartTime; }
    void GetLabCoords(Float_t &x, Float_t &y, Float_t &z) { x = fLabX; y = fLabY; z = fLabZ; }
    
    /** Destructor **/
    virtual ~BmnECALDigit() {}

private:
    Short_t  fChannel;
    Float_t  fX;
    Float_t  fY;
    Float_t  fAmp;
    Float_t  fPeakAmp;
    Float_t  fPeakTime;
    Float_t  fStartTime;
    Float_t  fLabX, fLabY, fLabZ;

    ClassDef(BmnECALDigit, 2);
};

#endif	/* BMNECALDIGIT1_H */
