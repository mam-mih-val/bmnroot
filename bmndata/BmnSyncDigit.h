#ifndef BMNSyncDIGIT_H
#define	BMNSyncDIGIT_H

#include "TNamed.h"

using namespace std;

class BmnSyncDigit : public TObject {

public:
    
/** Default constructor **/
    BmnSyncDigit();

    /** Constructor to use **/
    BmnSyncDigit(UInt_t iSerial,UInt_t iEvent,long long t_sec,long long t_ns);

    UInt_t  GetSerial()  const {return fSerial;}
    UInt_t  GetEvent()   const {return fEvent;}
    long long GetTime_sec()const {return fT_sec;}
    long long GetTime_ns() const {return fT_ns;}

    /** Destructor **/
    virtual ~BmnSyncDigit();

//private:
    UInt_t  fSerial; 
    UInt_t  fEvent;
    long long fT_sec; 
    long long fT_ns; 
    ClassDef(BmnSyncDigit, 1);
};

#endif	/* BMNSyncDIGIT_H */

