#ifndef BMNZDCDIGIT_H
#define	BMNZDCDIGIT_H

#include "BmnCaloDigit.h"

class BmnZDCDigit : public BmnCaloDigit
{
  public:
    /** Default constructor **/
    BmnZDCDigit();

    /** Main constructor **/
    BmnZDCDigit(UChar_t ix, UChar_t iy, Float_t x, Float_t y, UChar_t size, UChar_t ch, Float_t amp );

    BmnZDCDigit(UChar_t ix, UChar_t iy, Float_t x, Float_t y, UChar_t size, UChar_t ch, Float_t amp,
                Float_t sigMin, Float_t sigMax, Float_t sigPed, Float_t sigInt);

    void SetIX(UChar_t ix)        { fIX = ix;        }
    void SetIY(UChar_t iy)        { fIY = iy;        }
    void SetSize(UChar_t size)  { fSize = size;  }

    void SetSignalMin(Float_t value) { fSignalMin = value; }
    void SetSignalMax(Float_t value) { fSignalMax = value; }
    void SetSignalPed(Float_t value) { fSignalPed = value; }
    void SetSignalInt(Float_t value) { fSignalInt = value; }

    UChar_t GetIX()         const  { return fIX;}
    UChar_t GetIY()         const  { return fIY;}
    UChar_t GetSize()      const  { return fSize;}

    Float_t GetSignalMin()  const  { return fSignalMin;}
    Float_t GetSignalMax()  const  { return fSignalMax;}
    Float_t GetSignalPed()  const  { return fSignalPed;}
    Float_t GetSignalInt()  const  { return fSignalInt;}

    /** Destructor **/
    virtual ~BmnZDCDigit();

private:
    UChar_t  fIX;
    UChar_t  fIY;
    UChar_t  fSize;
    Float_t  fSignalMin;
    Float_t  fSignalMax;
    Float_t  fSignalPed;
    Float_t  fSignalInt;

    ClassDef(BmnZDCDigit, 5);
};

#endif	/* BMNZDCDIGIT_H */
