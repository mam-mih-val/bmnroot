#ifndef BMNTTBDIGIT_H
#define	BMNTTBDIGIT_H

#include "TNamed.h"

using namespace std;

class BmnTTBDigit : public TObject {

public:
    
/** Default constructor **/
    BmnTTBDigit();

    /** Constructor to use **/
    BmnTTBDigit(UInt_t iSerial,UInt_t iEvent,UInt_t iSlot,ULong_t lo,ULong_t hi);

    UInt_t  GetSerial()  const {return fSerial;}
    UInt_t  GetEvent()   const {return fEvent;}
    UInt_t  GetSlot()    const {return fSlot;}
    ULong_t GetTime_lo() const {return fT_lo;}
    ULong_t GetTime_hi() const {return fT_hi;}

    /** Destructor **/
    virtual ~BmnTTBDigit();

//private:
    UInt_t  fSerial; 
    UInt_t  fEvent;
    UInt_t  fSlot;
    ULong_t fT_lo; 
    ULong_t fT_hi; 
    ClassDef(BmnTTBDigit, 1);
};

#endif	/* BMNSyncDIGIT_H */

