#ifndef BMNECALDIGIT_H
#define	BMNECALDIGIT_H

#include "TNamed.h"
#include "TArrayS.h"

class BmnEcalDigit : public TNamed
{
  public:
    /** Default constructor **/
    BmnEcalDigit();

    /** Main constructor **/
    BmnEcalDigit(Char_t x,Char_t y,Char_t size,Char_t ch,Short_t samples,UShort_t *data);


    void SetX(Char_t x)        { fX = x;        }
    void SetY(Char_t y)        { fY = y;        }
    void SetSize(Char_t size)  { fSize = size;  }
    void SetChannel(Char_t ch) { fChannel = ch; }
    void SetSamples(Short_t samples);
    void SetWaveform(Short_t sample,Float_t val);

    Char_t GetX()         const  { return fX;}
    Char_t GetY()         const  { return fY;}
    Char_t GetSize()      const  { return fSize;}
    Char_t GetChannel()   const  { return fChannel;}
    Short_t GetSamples()  const  { return fSamples;}
    unsigned short *GetWaveform();


    /** Destructor **/
    virtual ~BmnEcalDigit();

private:
    Char_t  fX;
    Char_t  fY;
    Char_t  fSize;
    Char_t  fChannel;
    Short_t  fSamples;
    TArrayS fWaveform;

    ClassDef(BmnEcalDigit, 1);
};

#endif	/* BMNECALDIGIT_H */
