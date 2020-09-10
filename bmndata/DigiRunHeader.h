#ifndef BMNDIGIRUNHEADER_H
#define BMNDIGIRUNHEADER_H

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
    UInt_t fBeamTrigger;
    UInt_t fBTnBusy;
    UInt_t fBTAccepted;


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
    UInt_t GetPeriodId() { return fPeriodId; }
    /** Get the run ID */
    UInt_t GetRunId() { return fRunId; }
    /** Get the BeamTrigger counter */
    UInt_t GetBT() { return fBeamTrigger; }
    /** Get the BT and not Busy  */
    UInt_t GetBTnBusy() { return fBTnBusy; }
    /** Get the (BT and not Busy) by live time  */
    UInt_t GetBTAccepted() { return fBTAccepted; }

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
    void SetBT(UInt_t cntr) { fBeamTrigger = cntr; }
    /** Set the BT and Busy counter
     * \param[in] cntr : BT and Busy
     */
    void SetBTnBusy(UInt_t cntr) { fBTnBusy = cntr; }
    /** Set the (BT and not Busy) by live time
     * \param[in] cntr : BT Accepted
     */
    void SetBTAccepted(UInt_t cntr) { fBTAccepted = cntr; }

    ClassDef(DigiRunHeader, 1)
};

#endif /* BMNDIGIRUNHEADER_H */
