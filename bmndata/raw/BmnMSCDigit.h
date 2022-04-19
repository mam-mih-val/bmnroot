#ifndef BMNMSCDIGIT_H
#define BMNMSCDIGIT_H

#include "TObject.h"
#include "TTimeStamp.h"

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
    BmnMSCDigit(UInt_t iSerial, UChar_t iSlot, UInt_t *iValue, UInt_t evId, TTimeStamp fTime = TTimeStamp(time_t(0), 0));

    /** Constructor 
     * \param[in] iSerial : crate serial
     * \param[in] iSlot : crate slot
     * \param[in] evId : spill's last eventID in case of EndOfSpill data, 
     *  or eventID in case of normal spill data
     * \param[in] iTime : timestamp for normal spill data
     */
    BmnMSCDigit(UInt_t iSerial, UChar_t iSlot, UInt_t evId, TTimeStamp fTime = TTimeStamp(time_t(0), 0));

    UInt_t GetSerial() const {
        return fSerial;
    }

    UChar_t GetSlot() const {
        return fSlot;
    }

    UInt_t *GetValue() const {
        return (UInt_t *) fValue;
    }

    TTimeStamp GetTime() const {
        return fTS;
    }

    UInt_t GetLastEventId() const {
        return fEventId;
    }
    
    constexpr static const uint8_t GetNVals(){
        return NVals;
    }
    /** Destructor **/
    virtual ~BmnMSCDigit();
private:
    static const uint8_t NVals = 16;
    UInt_t fSerial;
    UChar_t fSlot;
    UInt_t fValue[NVals];
    TTimeStamp fTS;
    UInt_t fEventId; ///< last EventID of the spill

    ClassDef(BmnMSCDigit, 1);
};

#endif /* BMNMSCDIGIT_H */

