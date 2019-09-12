#ifndef BMNDIGIRUNHEADER_H
#define BMNDIGIRUNHEADER_H

#include "TObject.h"
#include "TTimeStamp.h"

using namespace std;

class BmnDigiRunHeader : public TObject {
private:
    /** Period Id **/
    UInt_t fPeriodId;
    /** Run Id **/
    UInt_t fRunId;
    TTimeStamp fRunStartTime;
    TTimeStamp fRunEndTime;


public:
    /** Default constructor */
    BmnDigiRunHeader();

    /** Constructor 
     * \param[in] period_id : unique period id
     * \param[in] run_id : unique run id
     * \param[in] run_start_time : run start time
     * \param[in] run_end_time : run end time 
     */
    BmnDigiRunHeader(UInt_t period_id, UInt_t run_id, TTimeStamp run_start_time, TTimeStamp run_end_time);
    
    /** Destructor */
    virtual ~BmnDigiRunHeader();


    /** Get the period ID for this run */
    UInt_t GetPeriodId() { return fPeriodId; }
    /** Get the run ID */
    UInt_t GetRunId() { return fRunId; }

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


    ClassDef(BmnDigiRunHeader, 1)
};

#endif /* BMNDIGIRUNHEADER_H */
