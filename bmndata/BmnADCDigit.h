#ifndef BMNADCDIGIT_H
#define BMNADCDIGIT_H

#include "TNamed.h"

#define ADC_SAMPLING_LIMIT 8192

using namespace std;

class BmnADCDigit : public TObject {
protected:
    UInt_t fSerial;
    UInt_t fChannel;
    UInt_t fNsmpl;
    UShort_t* fValueU; //[fNsmpl]
    Short_t* fValueI; //[fNsmpl]

public:

    /** Default constructor **/
    BmnADCDigit();

    /** Constructor to use **/
    BmnADCDigit(UInt_t iSerial, UInt_t iChannel, UInt_t n, UShort_t *iValue);
    BmnADCDigit(UInt_t iSerial, UInt_t iChannel, UInt_t n, Short_t *iValue);

    UInt_t GetSerial() const {
        return fSerial;
    }

    UInt_t GetChannel() const {
        return fChannel;
    }

    UInt_t GetNSamples() const {
        return fNsmpl;
    }

    UShort_t *GetUShortValue() const {
        return (UShort_t *) fValueU;
    }
    
    Short_t *GetShortValue() const {
        return (Short_t *) fValueI;
    }

    void SetShortValue(Short_t *iValue) const {
        for (Int_t i = 0; i < fNsmpl; ++i)
            fValueI[i] = iValue[i];
    }
    
    void SetUShortValue(UShort_t *iValue) const {
        for (Int_t i = 0; i < fNsmpl; ++i)
            fValueU[i] = iValue[i];
    }

    /** Destructor **/
    virtual ~BmnADCDigit();

    ClassDef(BmnADCDigit, 2);
};

#endif /* BMNADCDIGIT_H */

