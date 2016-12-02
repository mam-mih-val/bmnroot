#ifndef BMNADC32DIGIT_H
#define BMNADC32DIGIT_H

#include "TNamed.h"

using namespace std;

class BmnADC32Digit : public TObject {

private:
    UInt_t fSerial;
    UInt_t fChannel;
    UInt_t fValue[32]; //[2048]; // 64 channels x 32 16-bit words
    
public:

    /** Default constructor **/
    BmnADC32Digit();

    /** Constructor to use **/
    BmnADC32Digit(UInt_t iSerial, UInt_t iChannel, UInt_t *iValue);

    UInt_t GetSerial() const {
        return fSerial;
    }

    UInt_t GetChannel() const {
        return fChannel;
    }
    
    UInt_t *GetValue() const {
        return (UInt_t *) fValue;
    }

    /** Destructor **/
    virtual ~BmnADC32Digit();

    ClassDef(BmnADC32Digit, 2);
};

#endif /* BMNADC32DIGIT_H */

