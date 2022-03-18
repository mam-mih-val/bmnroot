#ifndef BMNDIGIRUNHEADER_H
#define BMNDIGIRUNHEADER_H

#include <map>
#include "TObject.h"
#include "TTimeStamp.h"

using namespace std;

class DigiRunHeader : public TObject {
private:
    /** Period Id **/
    UInt_t fPeriodId;
    /** Run Id **/
    UInt_t fRunId;
    TTimeStamp fRunStartTime;
    TTimeStamp fRunEndTime;
    ULong64_t fBeamTrigger;
    ULong64_t fBTnBusy;
    Double_t fBTAccepted; ///< BT&notBusy normalized according to Acc/(Acc + BP + AP) | for SRC => sum [BT * (DAQ_Busy -peds)/ (DAQ_TRigger - peds)]
    ULong64_t fAccepted;
    ULong64_t fProtection;
    ULong64_t fL0;
    ULong64_t fBP;
    ULong64_t fAP;
    map<UInt_t, vector<uint64_t> > fBoardSums;///<  (MSC serial, raw sums of the counters)


public:
    /** Default constructor */
    DigiRunHeader();

    /** Constructor 
     * \param[in] period_id : unique period id
     * \param[in] run_id : unique run id
     * \param[in] run_start_time : run start time
     * \param[in] run_end_time : run end time 
     */
    DigiRunHeader(UInt_t period_id, UInt_t run_id, TTimeStamp run_start_time, TTimeStamp run_end_time);
    
    /** Destructor */
    virtual ~DigiRunHeader();


    /** Get the period ID for this run */
    ULong64_t GetPeriodId() { return fPeriodId; }
    /** Get the run ID */
    ULong64_t GetRunId() { return fRunId; }
    /** Get the BeamTrigger counter */
    ULong64_t GetBT() { return fBeamTrigger; }
    /** Get the BT and not Busy  */
    ULong64_t GetBTnBusy() { return fBTnBusy; }
    /** Get the (BT and not Busy) by live time  */
    Double_t GetBTAccepted() { return fBTAccepted; }
    /** Get the Accepted by live time  */
    ULong64_t GetAccepted() { return fAccepted; }
    /** Get the Trigger Protection  */
    ULong64_t GetProtection() { return fProtection; }
    /** Get the L0  */
    ULong64_t GetL0() { return fL0; }
    /** Get the raw MSC counters  */
    map<UInt_t, vector<uint64_t> >& GetRawMSC() { return fBoardSums; }


    TTimeStamp GetRunStartTime() { return fRunStartTime; }
    TTimeStamp GetRunEndTime() { return fRunEndTime; }


    /** Set the period ID for this run
     * \param[in] period_id : unique period id
     */
    void SetPeriodId(UInt_t period_id) { fPeriodId = period_id; }
    
    /** Set the period ID for this run
     * \param[in] run_id : unique run id
     */
    void SetRunId(UInt_t run_id) { fRunId = run_id; }
    
    /** Set the period ID for this run
     * \param[in] run_start_time : run start time
     */
    void SetRunStartTime(TTimeStamp run_start_time) { fRunStartTime = run_start_time; }
    
    /** Set the period ID for this run
     * \param[in] run_end_time : run end time
     */
    void SetRunEndTime(TTimeStamp run_end_time) { fRunEndTime = run_end_time; }
    
    /** Set the Beam Trigger counter
     * \param[in] cntr : Beam Trigger counter
     */
    void SetBT(ULong64_t cntr) { fBeamTrigger = cntr; }
    /** Set the BT and Busy counter
     * \param[in] cntr : BT and Busy
     */
    void SetBTnBusy(ULong64_t cntr) { fBTnBusy = cntr; }
    /** Set the (BT and not Busy) by live time
     * \param[in] cntr : BT Accepted
     */
    void SetBTAccepted(Double_t cntr) { fBTAccepted = cntr; }
    /** Set the Accepted counter
     * \param[in] cntr : Accepted counter
     */
    void SetAccepted(ULong64_t cntr) { fAccepted = cntr; }
    /** Set the Trigger Protection counter
     * \param[in] cntr : Trigger Protection counter
     */
    void SetProtection(ULong64_t cntr) { fProtection = cntr; }
    /** Set the L0 counter
     * \param[in] cntr : L0 counter
     */
    void SetL0(ULong64_t cntr) { fL0 = cntr; }
    /** Set the raw MSC counters
     * \param[in] v : map < MSC serial, counters vector>
     */
    void SetRawMSC(map<UInt_t, vector<uint64_t> >  v) { fBoardSums = v;
//    printf("inner len %lu\n", fBoardSums.size());
    }

    ClassDef(DigiRunHeader, 2)
};

#endif /* BMNDIGIRUNHEADER_H */
