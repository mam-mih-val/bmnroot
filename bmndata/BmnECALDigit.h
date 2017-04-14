#ifndef BMNECALDIGIT1_H
#define	BMNECALDIGIT1_H

#include "TNamed.h"
#include "TArrayS.h"

class BmnECALDigit : public TNamed
{
  public:
    /** Default constructor **/
    BmnECALDigit();

    /** Main constructor **/
    BmnECALDigit(UChar_t ix,UChar_t iy,Float_t x,Float_t y,UChar_t size,Short_t ch,Float_t amp);

    void SetIX(UChar_t ix)        { fIX = ix;        }
    void SetIY(UChar_t iy)        { fIY = iy;        }
    void SetX(Float_t x)        { fX = x;        }
    void SetY(Float_t y)        { fY = y;        }
    void SetSize(UChar_t size)  { fSize = size;  }
    void SetChannel(UChar_t ch) { fChannel = ch; }
    void SetAmp(Float_t amp) { fAmp = amp; }

    UChar_t GetIX()         const  { return fIX;}
    UChar_t GetIY()         const  { return fIY;}
    Float_t GetX()        const  { return fX;}
    Float_t GetY()        const  { return fY;}
    UChar_t GetSize()      const  { return fSize;}
    UChar_t GetChannel()   const  { return fChannel;}
    Float_t GetAmp()  const  { return fAmp;}

    /** Destructor **/
    virtual ~BmnECALDigit();

private:
    UChar_t  fIX;
    UChar_t  fIY;
    Float_t  fX;
    Float_t  fY;
    UChar_t  fSize;
    Short_t  fChannel;
    Float_t  fAmp;

    ClassDef(BmnECALDigit, 1);
};

#endif	/* BMNECALDIGIT1_H */
