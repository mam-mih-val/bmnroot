#ifndef BMNADC128DIGIT_H
#define BMNADC128DIGIT_H

#include "TNamed.h"

using namespace std;

class BmnADC128Digit : public TObject {

private:
    UInt_t fSerial;
    UChar_t fChannel;
    UShort_t fValue[128];
    
public:

    /** Default constructor **/
    BmnADC128Digit();

    /** Constructor to use **/
    BmnADC128Digit(UInt_t iSerial, UChar_t iChannel, UShort_t *iValue);

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
    virtual ~BmnADC128Digit();

    ClassDef(BmnADC128Digit, 1);
};

#endif /* BMNADC128DIGIT_H */

