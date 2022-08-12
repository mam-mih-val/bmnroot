#ifndef DSTEVENTHEADER_H
#define DSTEVENTHEADER_H

#include "BmnEnums.h"

#include "FairEventHeader.h"
#include "FairRootManager.h"

#include "TTimeStamp.h"

class DstEventHeader : public FairEventHeader
{
   private:
    /** Event Header branch name **/
    TString fHeaderName;

    /** Event Identifier **/
    UInt_t fEventId;
    /** Event Time in TTimeStamp **/
    TTimeStamp fEventTimeTS;
    /** Trigger Type (beam = 6 or target = 1) **/
    BmnTriggerType fTriggerType;
    /** Impact parameter [fm] **/
    Double_t fB;
    /** Total charge before and after the target **/
    Double_t fZ2in;
    Double_t fZ2out;
    Double_t fADCin;
    Double_t fADCout;
    Double_t fZ1, fZ2, fZ3, fZ4;          // for charge calculation from single scintilator, currently not used
    Double_t fADC1, fADC2, fADC3, fADC4;  // for 2D charge cuts
    Short_t fZin;

   public:
    /** Default constructor */
    DstEventHeader();

    /** Constructor with parameters */
    DstEventHeader(UInt_t run_id, UInt_t event_id, TTimeStamp event_time, BmnTriggerType trigger_type, Double_t b);

    /** Destructor */
    virtual ~DstEventHeader();

    virtual void Register(Bool_t Persistence = kTRUE) {
        FairRootManager::Instance()->Register(fHeaderName.Data(), "EvtHeader", this, Persistence);
    }

    /** Get Event Header branch name */
    TString GetHeaderName() { return fHeaderName; }

    /** Get run ID for this run */
    UInt_t GetEventId() { return fEventId; }

    /** Get time for this event */
    TTimeStamp GetEventTimeTS() { return fEventTimeTS; }

    /** Get type of this event */
    BmnTriggerType GetTriggerType() { return fTriggerType; }

    /** Get impact parameter - b, fm */
    Double_t GetB() { return fB; }

    /** Get total charge before and after the target */
    Double_t GetZ2in() { return fZ2in; }
    Double_t GetZ2out() { return fZ2out; }
    Double_t GetADCin() { return fADCin; }
    Double_t GetADCout() { return fADCout; }
    Double_t GetZ1() { return fZ1; }
    Double_t GetZ2() { return fZ2; }
    Double_t GetZ3() { return fZ3; }
    Double_t GetZ4() { return fZ4; }

    Double_t GetADC1() { return fADC1; }
    Double_t GetADC2() { return fADC2; }
    Double_t GetADC3() { return fADC3; }
    Double_t GetADC4() { return fADC4; }

    Short_t GetZin() { return fZin; }
    /** Set Event Header branch name */
    void SetHeaderName(TString header_name) { fHeaderName = header_name; }

    /** Set event ID for this run */
    void SetEventId(UInt_t event_id) { fEventId = event_id; }

    /** Set time for this event in */
    void SetEventTimeTS(TTimeStamp event_time) { fEventTimeTS = event_time; }

    /** Set trigger type for this event
     * @param trigger_type : trigger type (beam = 6 or target = 1)
     */
    void SetTriggerType(BmnTriggerType trigger_type) { fTriggerType = trigger_type; }

    /** Set impact parameter - b, fm */
    void SetB(Double_t b) { fB = b; }

    /** Set total charge before and after the target */
    void SetZ2in(Double_t z2in) { fZ2in = z2in; }
    void SetZ2out(Double_t z2out) { fZ2out = z2out; }
    void SetADCin(Double_t adcin) { fADCin = adcin; }
    void SetADCout(Double_t adcout) { fADCout = adcout; }

    void SetZ1(Double_t z1) { fZ1 = z1; }
    void SetZ2(Double_t z2) { fZ2 = z2; }
    void SetZ3(Double_t z3) { fZ3 = z3; }
    void SetZ4(Double_t z4) { fZ4 = z4; }
    void SetADC1(Double_t adc1) { fADC1 = adc1; }
    void SetADC2(Double_t adc2) { fADC2 = adc2; }
    void SetADC3(Double_t adc3) { fADC3 = adc3; }
    void SetADC4(Double_t adc4) { fADC4 = adc4; }

    void SetZin(Short_t Zin) { fZin = Zin; }

    void CopyFrom(DstEventHeader * hdr);
    ClassDef(DstEventHeader, 2);
};

#endif /* DstEventHeader_H */
