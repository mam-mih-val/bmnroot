#ifndef BMNSyncDIGIT_H
#define	BMNSyncDIGIT_H

#include "TNamed.h"

using namespace std;

class BmnSyncDigit : public TObject {

public:
    
/** Default constructor **/
    BmnSyncDigit();

    /** Constructor to use **/
    BmnSyncDigit(UInt_t iSerial,UInt_t iEvent,ULong_t t_sec,ULong_t t_ns);

    UInt_t  GetSerial()  const {return fSerial;}
    UInt_t  GetEvent()   const {return fEvent;}
    ULong_t GetTime_sec()const {return fT_sec;}
    ULong_t GetTime_ns() const {return fT_ns;}

    /** Destructor **/
    virtual ~BmnSyncDigit();

//private:
    UInt_t  fSerial; 
    UInt_t  fEvent;
    ULong_t fT_sec; 
    ULong_t fT_ns; 
    ClassDef(BmnSyncDigit, 1);
};

#endif	/* BMNSyncDIGIT_H */

