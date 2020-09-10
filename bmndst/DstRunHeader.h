#ifndef DSTRUNHEADER_H
#define DSTRUNHEADER_H

#include "TNamed.h"
#include "TTimeStamp.h"
#include "BmnEnums.h"

class DstRunHeader : public TNamed {
 private:
    /** Period Number **/
    UInt_t fPeriodNumber;
    /** Run Number **/
    UInt_t fRunNumber;
    /** Start time of run **/
    TTimeStamp fStartTime;
    /** Finish time of run **/
    TTimeStamp fFinishTime;

    /** Beam, A (atomic weight) **/
    Int_t fBeamA;
    /** Beam, Z (charge) **/
    Int_t fBeamZ;
    /** Beam energy, GeV **/
    Float_t fBeamEnergy;
    /** Target, A (atomic weight) **/
    Int_t fTargetA;
    /** Target, Z (charge) **/
    Int_t fTargetZ;
    /** Magnetic field, mV **/
    Double_t fMagneticField;

 public:
    /** Default constructor */
    DstRunHeader();

    /** Constructor (short) */
    DstRunHeader(UInt_t period_number, UInt_t run_number, TTimeStamp start_time, TTimeStamp end_time);

    /** Constructor (full) */
    DstRunHeader(UInt_t period_number, UInt_t run_number, TTimeStamp start_time, TTimeStamp end_time,
                 Int_t beam_a, Int_t beam_z, Float_t beam_energy, Int_t target_a, Int_t target_z, Double_t mag_field);

    /** Destructor */
    virtual ~DstRunHeader();

    /** Get the run ID for this run */
    UInt_t GetPeriodNumber() { return fPeriodNumber; }
    UInt_t GetRunNumber() { return fRunNumber; }
    TTimeStamp GetStartTime() { return fStartTime; }
    TTimeStamp GetFinishTime() { return fFinishTime; }
    Int_t GetBeamA() { return fBeamA; }
    Int_t GetBeamZ() { return fBeamZ; }
    Float_t GetBeamEnergy() { return fBeamEnergy; }
    Int_t GetTargetA() { return fTargetA; }
    Int_t GetTargetZ() { return fTargetZ; }
    Double_t GetMagneticField() { return fMagneticField; }

    /** Set the run number for this run **/
    void SetPeriodNumber(UInt_t period_number) { fPeriodNumber = period_number; }
    void SetRunNumber(UInt_t run_number) { fRunNumber = run_number; }
    void SetPeriodRun(UInt_t period_number, UInt_t run_number) { fPeriodNumber = period_number; fRunNumber = run_number; }
    void SetStartTime(TTimeStamp start_time) { fStartTime = start_time; }
    void SetFinishTime(TTimeStamp end_time) { fFinishTime = end_time; }
    void SetBeamA(Int_t beam_a) { fBeamA = beam_a; }
    void SetBeamZ(Int_t beam_z) { fBeamZ = beam_z; }
    void SetBeamEnergy(Float_t beam_energy) { fBeamEnergy = beam_energy; }
    void SetTargetA(Int_t target_a) { fTargetA = target_a; }
    void SetTargetZ(Int_t target_z) { fTargetZ = target_z; }
    void SetMagneticField(Double_t mag_field) { fMagneticField = mag_field; }

    ClassDef(DstRunHeader, 2)
};

#endif /* DSTRUNHEADER_H */
