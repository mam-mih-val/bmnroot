#ifndef BMNMSCDIGIT_H
#define BMNMSCDIGIT_H

#include "TObject.h"

using namespace std;

class BmnMSCDigit : public TObject {
public:

    /** Default constructor **/
    BmnMSCDigit();

    /** Constructor 
     * \param[in] iSerial : crate serial
     * \param[in] iSlot : crate slot
     * \param[in] iValue : counters array ptr
     * \param[in] evId : spill's last eventID in case of EndOfSpill data, 
     *  or eventID in case of normal spill data
     * \param[in] iTime : timestamp for normal spill data
     */
    BmnMSCDigit(UInt_t iSerial, UChar_t iSlot, UInt_t *iValue, UInt_t evId, UInt_t iTime = 0);
    
    /** Constructor 
     * \param[in] iSerial : crate serial
     * \param[in] iSlot : crate slot
     * \param[in] evId : spill's last eventID in case of EndOfSpill data, 
     *  or eventID in case of normal spill data
     * \param[in] iTime : timestamp for normal spill data
     */
    BmnMSCDigit(UInt_t iSerial, UChar_t iSlot, UInt_t evId, UInt_t iTime = 0);

    UInt_t GetSerial() const {
        return fSerial;
    }

    UChar_t GetSlot() const {
        return fSlot;
    }

    UInt_t *GetValue() const {
        return (UInt_t *) fValue;
    }

    UInt_t GetTime() const {
        return fTime;
    }

    UInt_t GetLastEventId() const {
        return fEventId;
    }

    /** Destructor **/
    virtual ~BmnMSCDigit();
private:
    UInt_t fSerial;
    UChar_t fSlot;
    UInt_t fValue[16];
    UInt_t fTime;
    UInt_t fEventId;///< last EventID of the spill

    ClassDef(BmnMSCDigit, 1);
};

#endif /* BMNMSCDIGIT_H */

