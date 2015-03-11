#ifndef BMNADCDIGIT_H
#define	BMNADCDIGIT_H

#include "TNamed.h"

using namespace std;

class BmnADCDigit : public TObject {

public:
    
/** Default constructor **/
    BmnADCDigit();

    /** Constructor to use **/
    BmnADCDigit(UInt_t iSerial,UChar_t iChannel,UChar_t iSamples,UShort_t *iValue);

    UInt_t  GetSerial()   const {return fSerial;}
    UInt_t  GetSamples()  const {return fSamples;}
    UInt_t  GetChannel()  const {return fChannel;}
    UShort_t *GetValue()  const {return (UShort_t *)fValue;}
    void    SetSample(UShort_t val);
    /** Destructor **/
    virtual ~BmnADCDigit();
private:
    UInt_t    fSerial; 
    UChar_t   fChannel;
    UInt_t    fSamples;
    UShort_t  fValue[200];
    
    ClassDef(BmnADCDigit, 1);
};

#endif	/* BMNADCDIGIT_H */

