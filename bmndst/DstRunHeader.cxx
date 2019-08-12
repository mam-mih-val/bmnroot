#include "DstRunHeader.h"

// -----   Default constructor   ------------------------------------------
DstRunHeader::DstRunHeader() :
  TNamed("DstRunHeader",""),
  fPeriodNumber(0),
  fRunNumber(0),
  fStartTime(TTimeStamp()),
  fFinishTime(TTimeStamp()),
  fBeamA(-1),
  fBeamZ(-1),
  fBeamEnergy(0),
  fTargetA(-1),
  fTargetZ(-1),
  fMagneticField(0)
{}

DstRunHeader::DstRunHeader(UInt_t period_number, UInt_t run_number, TTimeStamp start_time, TTimeStamp end_time) :
  TNamed("DstRunHeader",""),
  fPeriodNumber(period_number),
  fRunNumber(run_number),
  fStartTime(start_time),
  fFinishTime(end_time),
  fBeamA(-1),
  fBeamZ(-1),
  fBeamEnergy(0),
  fTargetA(-1),
  fTargetZ(-1),
  fMagneticField(0)
{}

DstRunHeader::DstRunHeader(UInt_t period_number, UInt_t run_number, TTimeStamp start_time, TTimeStamp end_time,
                           Int_t beam_a, Int_t beam_z, Float_t beam_energy, Int_t target_a, Int_t target_z, Double_t mag_field) :
  TNamed("DstRunHeader",""),
  fPeriodNumber(period_number),
  fRunNumber(run_number),
  fStartTime(start_time),
  fFinishTime(end_time),
  fBeamA(beam_a),
  fBeamZ(beam_z),
  fBeamEnergy(beam_energy),
  fTargetA(target_a),
  fTargetZ(target_z),
  fMagneticField(mag_field)
{}

// -----   Destructor   ----------------------------------------------------
DstRunHeader::~DstRunHeader()
{}

ClassImp(DstRunHeader)
