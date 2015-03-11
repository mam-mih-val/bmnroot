#ifndef BMNTDCDIGIT_H
#define	BMNTDCDIGIT_H

#include "TObject.h"

using namespace std;

class BmnTDCDigit : public TObject {

public:
    
/** Default constructor **/
    BmnTDCDigit();

    /** Constructor to use **/
    BmnTDCDigit(UInt_t iSerial,UChar_t iType,UChar_t iSlot,Bool_t iLeading,UChar_t iChannel,UInt_t iValue);

    UInt_t  GetSerial()  const {return fSerial;}
    UChar_t GetType()    const {return fType;}
    UChar_t GetSlot()    const {return fSlot;}
    Bool_t  GetLeading() const {return fLeading;}
    UChar_t GetChannel() const {return fChannel;}
    UInt_t  GetValue()   const {return fValue;}

    /** Destructor **/
    virtual ~BmnTDCDigit();
private:
    UInt_t  fSerial; 
    UChar_t fType;
    UChar_t fSlot;
    Bool_t  fLeading;
    UChar_t fChannel;
    UInt_t  fValue;
  
    ClassDef(BmnTDCDigit, 1);
};

#endif	/* BMNTDCDIGIT_H */

