#ifndef BMNADC32DIGIT_H
#define BMNADC32DIGIT_H

#include "TNamed.h"

using namespace std;

class BmnADC32Digit : public TObject {

private:
    Bool_t fPed; //is pedestal digit or payload
    UInt_t fSerial;
    UChar_t fChannel;
    UShort_t fValue[32]; //[2048]; // 64 channels x 32 16-bit words
    
public:

    /** Default constructor **/
    BmnADC32Digit();

    /** Constructor to use **/
    BmnADC32Digit(UInt_t iSerial, UChar_t iChannel, UShort_t *iValue);

    UInt_t GetSerial() const {
        return fSerial;
    }

    UInt_t GetChannel() const {
        return fChannel;
    }
    
    UShort_t *GetValue() const {
        return (UShort_t *) fValue;
    }

    /** Destructor **/
    virtual ~BmnADC32Digit();

    ClassDef(BmnADC32Digit, 1);
};

#endif /* BMNADC32DIGIT_H */

