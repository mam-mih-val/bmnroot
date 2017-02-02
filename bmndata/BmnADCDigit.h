#ifndef BMNADCDIGIT_H
#define BMNADCDIGIT_H

#include "TNamed.h"

using namespace std;

class BmnADCDigit : public TObject {

private:
    UInt_t fSerial;
    UInt_t fChannel;
    UInt_t fNsmpl;
    UShort_t* fValue; //[fNsmpl]
    
public:

    /** Default constructor **/
    BmnADCDigit();

    /** Constructor to use **/
    BmnADCDigit(UInt_t iSerial, UInt_t iChannel, UInt_t n, UShort_t *iValue);

    UInt_t GetSerial() const {
        return fSerial;
    }

    UInt_t GetChannel() const {
        return fChannel;
    }
    
    UInt_t GetNSamples() const {
        return fNsmpl;
    }
    
    UShort_t *GetValue() const {
        return (UShort_t *) fValue;
    }

    /** Destructor **/
    virtual ~BmnADCDigit();

    ClassDef(BmnADCDigit, 2);
};

#endif /* BMNADCDIGIT_H */

