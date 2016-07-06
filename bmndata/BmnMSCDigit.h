#ifndef BMNMSCDIGIT_H
#define BMNMSCDIGIT_H

#include "TObject.h"

using namespace std;

class BmnMSCDigit : public TObject {
public:

    /** Default constructor **/
    BmnMSCDigit();

    /** Constructor to use **/
    BmnMSCDigit(UInt_t iSerial, UChar_t iType, UChar_t iSlot, UShort_t *iValue, UInt_t iTime);

    UInt_t GetSerial() const {
        return fSerial;
    }

    UChar_t GetType() const {
        return fType;
    }

    UChar_t GetSlot() const {
        return fSlot;
    }

    UShort_t *GetValue() const {
        return (UShort_t *) fValue;
    }

    UInt_t GetTime() const {
        fTime;
    }

    /** Destructor **/
    virtual ~BmnMSCDigit();
private:
    UInt_t fSerial;
    UChar_t fType;
    UChar_t fSlot;
    UShort_t fValue[16];
    UInt_t fTime;

    ClassDef(BmnMSCDigit, 1);
};

#endif /* BMNMSCDIGIT_H */

