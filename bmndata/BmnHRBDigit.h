#ifndef BMNHRBDIGIT_H
#define BMNHRBDIGIT_H

#include "TNamed.h"

using namespace std;

class BmnHRBDigit : public TObject {

private:
    UInt_t fSerial;
    UInt_t fChannel;
    UInt_t fSample; //sample per 8 ns
    
public:

    /** Default constructor **/
    BmnHRBDigit();

    /** Constructor to use **/
    BmnHRBDigit(UInt_t iSerial, UInt_t iChannel, UInt_t iSample);

    UInt_t GetSerial() const {
        return fSerial;
    }

    UInt_t GetChannel() const {
        return fChannel;
    }
    
    UInt_t GetSample() const {
        return fSample;
    }

    /** Destructor **/
    virtual ~BmnHRBDigit();

    ClassDef(BmnHRBDigit, 1);
};

#endif /* BMNHRBDIGIT_H */

