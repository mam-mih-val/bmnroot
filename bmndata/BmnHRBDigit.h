#ifndef BMNHRBDIGIT_H
#define BMNHRBDIGIT_H

#include "TNamed.h"

using namespace std;

class BmnHRBDigit : public TObject {

private:
    UInt_t fSerial;
    UInt_t fChannel;
    UInt_t fSample; //sample per 8 ns
    UInt_t fTimeHigh;
    UInt_t fTimeLow;
    
    
public:

    /** Default constructor **/
    BmnHRBDigit();

    /** Constructor to use **/
    BmnHRBDigit(UInt_t iSerial, UInt_t iChannel, UInt_t iSample, UInt_t iTH, UInt_t iTL);

    UInt_t GetSerial() const {
        return fSerial;
    }

    UInt_t GetChannel() const {
        return fChannel;
    }
    
    UInt_t GetSample() const {
        return fSample;
    }

    UInt_t GetTimeHigh() const {
        return fTimeHigh;
    }
    
    UInt_t GetTimeLow() const {
        return fTimeLow;
    }

    /** Destructor **/
    virtual ~BmnHRBDigit();

    ClassDef(BmnHRBDigit, 1);
};

#endif /* BMNHRBDIGIT_H */

