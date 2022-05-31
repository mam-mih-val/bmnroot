/** @file BmnSsdDigitize.cxx
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 23.05.2014
 **/

// Include class header
#include "BmnSsdDigitize.h"

// Includes from C++
#include <cassert>
#include <cstring>
#include <fstream>
#include <sstream>
#include <sstream>
#include <iomanip>
#include <iostream>

// Includes from ROOT
#include "TClonesArray.h"
#include "TGeoBBox.h"
#include "TGeoMatrix.h"
#include "TGeoPhysicalNode.h"
#include "TGeoVolume.h"

// Includes from FairRoot
#include "FairEventHeader.h"
#include "FairField.h"
#include "FairLink.h"
#include "FairLogger.h"
#include "FairMCEventHeader.h"
#include "FairMCPoint.h"
#include "FairRunAna.h"
#include "FairRunSim.h"
#include "FairRuntimeDb.h"

//#include "BmnDaqBuffer.h"
#include "BmnDaqBuffer.h"
// Includes from BmnRoot
#include "CbmMCTrack.h"
#include "BmnSsdDigi.h"
#include "BmnSsdPoint.h"

// Includes from SSD
#include "setup/BmnSsdModule.h"
#include "setup/BmnSsdSensor.h"
#include "setup/BmnSsdSensorConditions.h"
#include "setup/BmnSsdSetup.h"
#include "digitize/BmnSsdPhysics.h"
#include "digitize/BmnSsdDigitizeParameters.h"

using std::fixed;
using std::right;
using std::setprecision;
using std::setw;
using std::string;

Bool_t fIsInitialised;   ///< kTRUE if Init() was called

Int_t  fEnergyLossModel;  ///< Energy loss model
Bool_t fUseLorentzShift;
Bool_t fUseDiffusion;
Bool_t fUseCrossTalk;
Bool_t fGenerateNoise;

BmnSsdDigitizeParameters* fDigiPar; ///< Digitisation parameters

// -----   Standard constructor   ------------------------------------------
BmnSsdDigitize::BmnSsdDigitize()
: BmnDigitize("SsdDigitize"),
  fIsInitialised(kFALSE),
  fEnergyLossModel(2),
  fUseLorentzShift(kTRUE),
  fUseDiffusion(kTRUE),
  fUseCrossTalk(kTRUE),
  fGenerateNoise(kFALSE),
  fDigiPar(NULL),
  fSetup(NULL),
  fPoints(NULL),
  fTracks(NULL),
  fDigis(NULL),
  fMatches(NULL),
  fTimer(),
  fSensorDinact(0.12),
  fSensorPitch(0.0058),
  fSensorStereoF(0.),
  fSensorStereoB(7.5),
  fSensorVdep(70.),
  fSensorVbias(140.),
  fSensorTemperature(268.),
  fSensorCcoupling(17.5),
  fSensorCinterstrip(1.),
  fModuleDynRange(75000.),
  fModuleThreshold(3000.),
  fModuleNofAdc(32),
  fModuleTresol(5.),
  fModuleTdead(800.),
  fModuleNoise(1000.),
  fModuleZeroNoiseRate(3.9789e-3),
  fModuleFracDeadChan(0.),
  fSensorParameterFile(),
  fSensorConditionFile(),
  fModuleParameterFile(),
  fTimePointLast(-1.),
  fTimeDigiFirst(-1.),
  fTimeDigiLast(-1.),
  fNofPoints(0),
  fNofSignalsF(0),
  fNofSignalsB(0),
  fNofDigis(0),
  fNofEvents(0),
  fNofPointsTot(0.),
  fNofSignalsFTot(0.),
  fNofSignalsBTot(0.),
  fNofDigisTot(0.),
  fNofNoiseTot(0.),
  fTimeTot()
{
  ResetCounters();
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
BmnSsdDigitize::~BmnSsdDigitize() {
  LOG(debug) << "Destructing " << fName;
  ResetArrays();
  delete fDigis;
  delete fMatches;
}
// -------------------------------------------------------------------------



// -----   Content of analogue buffers   -----------------------------------
Int_t BmnSsdDigitize::BufferSize() const {
  Int_t    nSignals =  0;
  Int_t    nSigModule;
  Double_t t1Module;
  Double_t t2Module;

  for (Int_t iModule = 0; iModule < fSetup->GetNofModules(); iModule++) {
    fSetup->GetModule(iModule)->BufferStatus(nSigModule, t1Module, t2Module);
    nSignals += nSigModule;
  } //# modules in setup

  return nSignals;
}
// -------------------------------------------------------------------------



// -----   Print the status of the analogue buffers   ----------------------
string BmnSsdDigitize::BufferStatus() const {

  Int_t    nSignals =  0;
  Double_t t1       = -1;
  Double_t t2       = -1.;

  Int_t    nSigModule;
  Double_t t1Module;
  Double_t t2Module;

  for (Int_t iModule = 0; iModule < fSetup->GetNofModules(); iModule++) {
    fSetup->GetModule(iModule)->BufferStatus(nSigModule, t1Module, t2Module);
    if ( nSigModule ) {
      nSignals += nSigModule;
      t1 = t1 < 0. ? t1Module : TMath::Min(t1, t1Module);
      t2  = TMath::Max(t2, t2Module);
    } //? signals in module buffer?
  } //# modules in setup

  std::stringstream ss;
  ss << nSignals << ( nSignals == 1 ? " signal " : " signals " )
		         << "in analogue buffers";
  if ( nSignals ) ss << " ( from " << fixed << setprecision(3)
			                       << t1 << " ns to " << t2 << " ns )";
  return ss.str();
}
// -------------------------------------------------------------------------



// -----   Create a digi object   ------------------------------------------
void BmnSsdDigitize::CreateDigi(Int_t address, UShort_t channel,
                                Long64_t time,
                                UShort_t adc,
                                const BmnMatch& match) {

  // Copy match object. Will be deleted in the digi destructor.
  BmnMatch* digiMatch = new BmnMatch(match);

  // Update times of first and last digi
  fTimeDigiFirst = fNofDigis ?
      TMath::Min(fTimeDigiFirst, Double_t(time)) : time;
  fTimeDigiLast  = TMath::Max(fTimeDigiLast, Double_t(time));

  // Create digi
  BmnSsdDigi* digi = new BmnSsdDigi(address, channel, time, adc);
  digi->SetMatch(digiMatch);

  // Send the digi to DAQ
  SendDigi(digi);

  fNofDigis++;
  LOG(debug3) << GetName() << ": created digi at " << time
      << " ns with ADC " << adc << " at address " << address
      << " (module "
      << fSetup->GetElement(address, kSsdModule)->GetName()
      << ", channel " << channel
      << ")";

}
// -------------------------------------------------------------------------



// -----   Task execution   ------------------------------------------------
void BmnSsdDigitize::Exec(Option_t* /*opt*/) {

  // --- Start timer and reset counters
  fTimer.Start();
  ResetCounters();

  // --- For debug: status of analogue buffers
  if ( gLogger->IsLogNeeded(DEBUG)) {
    std::cout << std::endl;
    LOG(debug) << GetName() << ": " << BufferStatus();
  }

  // --- Store previous event time.  Get current event time.
  Double_t eventTimePrevious = fCurrentEventTime;
  GetEventInfo();

  // --- Generate noise from previous to current event time
  if ( fDigiPar->GetGenerateNoise() ) {
    Int_t nNoise = 0;
    Double_t tNoiseStart = fNofEvents ? eventTimePrevious : 0.;
    Double_t tNoiseEnd   = fCurrentEventTime;
    for (Int_t iModule = 0; iModule < fSetup->GetNofModules(); iModule++)
      nNoise += fSetup->GetModule(iModule)->GenerateNoise(tNoiseStart,
                                                          tNoiseEnd);
    fNofNoiseTot += Double_t(nNoise);
    LOG(info) << "+ " << setw(20) << GetName() << ": Generated  " << nNoise
        << " noise signals from t = " << tNoiseStart << " ns to "
        << tNoiseEnd << " ns";
  }

  // --- Analogue response: Process the input array of SsdPoints
  ProcessMCEvent();
  LOG(debug) << GetName() << ": " << fNofSignalsF + fNofSignalsB
      << " signals generated ( "
      << fNofSignalsF << " / " << fNofSignalsB << " )"
     ;
  // --- For debug: status of analogue buffers
  if ( gLogger->IsLogNeeded(DEBUG)) {
    LOG(debug) << GetName() << ": " << BufferStatus();
  }

  // --- Readout time: in stream mode the time of the current event.
  // --- Analogue buffers will be digitised for signals at times smaller than
  // --- that time minus a safety margin depending on the module properties
  // --- (dead time and time resolution). In event mode, the readout time
  // --- is set to -1., meaning to digitise everything in the readout buffers.
  Double_t readoutTime = fEventMode ? -1. : fCurrentEventTime;

  // --- Digital response: Process buffers of all modules
  ProcessAnalogBuffers(readoutTime);

  // --- Check status of analogue module buffers
  if ( gLogger->IsLogNeeded(DEBUG)) {
    LOG(debug) << GetName() << ": " << BufferStatus();
  }

  // --- Event log
  LOG(info) << "+ " << setw(15) << GetName() << ": Event " << setw(6)
         << right << fCurrentEvent << " at " << fixed << setprecision(3)
         << fCurrentEventTime << " ns, points: " << fNofPoints
         << ", signals: " << fNofSignalsF << " / " << fNofSignalsB
         << ", digis: " << fNofDigis << ". Exec time " << setprecision(6)
         << fTimer.RealTime() << " s.";

  // --- Counters
  fTimer.Stop();
  fNofEvents++;
  fNofPointsTot   += fNofPoints;
  fNofSignalsFTot += fNofSignalsF;
  fNofSignalsBTot += fNofSignalsB;
  fNofDigisTot    += fNofDigis;
  fTimeTot        += fTimer.RealTime();

}
// -------------------------------------------------------------------------



// -----   Finish run    ---------------------------------------------------
void BmnSsdDigitize::Finish() {

  // --- Start timer and reset counters
  fTimer.Start();
  ResetCounters();
  Int_t nSignals;
  Double_t t1;
  Double_t t2;

  // --- In event-by-event mode, the analogue buffers should be empty.
  if ( fEventMode ) {
    if ( BufferSize() ) {
      LOG(info) << fName << BufferStatus();
      LOG(fatal) << fName << ": Non-empty analogue buffers at end of event "
          << " in event-by-event mode!";
    } //? buffers not empty
  } //? event-by-event mode

  // ---  In time-based mode: process the remaining signals in the buffers
  else {
    std::cout << std::endl;
    LOG(info) << GetName() << ": Finish run";
    LOG(info) << GetName() << ": " << BufferStatus()
               ;
    LOG(info) << GetName() << ": Processing analogue buffers"
       ;

    // --- Loop over all modules in the setup and process their buffers
    for (Int_t iModule = 0; iModule < fSetup->GetNofModules(); iModule++)
      fSetup->GetModule(iModule)->ProcessAnalogBuffer(-1.);

    // --- Screen output
    LOG(info) << GetName() << ": " << fNofDigis
        << ( fNofDigis == 1 ? " digi " :  " digis " )
        << "created and sent to DAQ ";
    if ( fNofDigis ) LOG(debug) << "( from " << fixed
        << setprecision(3) << fTimeDigiFirst << " ns to "
        << fTimeDigiLast << " ns )";
    LOG(info);
    LOG(info) << GetName() << ": " << BufferStatus()
               ;
  }

  fTimer.Stop();
  fNofPointsTot   += fNofPoints;
  fNofSignalsFTot += fNofSignalsF;
  fNofSignalsBTot += fNofSignalsB;
  fNofDigisTot    += fNofDigis;
  fTimeTot        += fTimer.RealTime();

  std::cout << std::endl;
  LOG(info) << "=====================================";
  LOG(info) << GetName() << ": Run summary";
  LOG(info) << "Events processed    : " << fNofEvents;
  LOG(info) << "SsdPoint / event    : " << setprecision(1)
			                  << fNofPointsTot / Double_t(fNofEvents)
			                 ;
  LOG(info) << "Signals / event     : "
      << fNofSignalsFTot / Double_t(fNofEvents)
      << " / " << fNofSignalsBTot / Double_t(fNofEvents)
     ;
  LOG(info) << "SsdDigi / event     : "
      << fNofDigisTot  / Double_t(fNofEvents);
  LOG(info) << "Digis per point     : " << setprecision(6)
			                  << fNofDigisTot / fNofPointsTot;
  LOG(info) << "Digis per signal    : "
      << fNofDigisTot / ( fNofSignalsFTot + fNofSignalsBTot )
     ;
  LOG(info) << "Noise digis / event : " << fNofNoiseTot / Double_t(fNofEvents)
	                     ;
  LOG(info) << "Noise fraction      : " << fNofNoiseTot / fNofDigisTot
     ;
  LOG(info) << "Real time per event : " << fTimeTot / Double_t(fNofEvents)
			                  << " s";
  LOG(info) << "=====================================";
}
// -------------------------------------------------------------------------


/** Is now in base class BmnDigitizer. Can be removed here after validation.
// -----   Get event information   -----------------------------------------
void BmnSsdDigitize::GetEventInfo(Int_t& inputNr, Int_t& eventNr,
                                  Double_t& eventTime) {

  // --- The event number is taken from the FairRootManager
  eventNr = FairRootManager::Instance()->GetEntryNr();

  // --- In a FairRunAna, take input number and time from FairEventHeader
  if ( FairRunAna::Instance() ) {
    FairEventHeader* event = FairRunAna::Instance()->GetEventHeader();
    assert ( event );
    inputNr   = event->GetInputFileId();
    eventTime = event->GetEventTime();
  } //? FairRunAna

  // --- In a FairRunSim, the input number and event time are always zero.
  else {
    if ( ! FairRunSim::Instance() )
      LOG(fatal) << GetName() << ": neither SIM nor ANA run."
     ;
    inputNr   = 0;
    eventTime = 0.;
  } //? Not FairRunAna

}
// -------------------------------------------------------------------------
**/

void BmnSsdDigitize::SetParContainers()
{
  fDigiPar = static_cast<BmnSsdDigitizeParameters*>(FairRunAna::Instance()->GetRuntimeDb()->getContainer("BmnSsdDigitizeParameters"));
}


// -----   Initialisation    -----------------------------------------------
InitStatus BmnSsdDigitize::Init() {

  // Get and initialise the SSD setup interface
  InitSetup();

  // Screen output
  std::cout << std::endl;
  LOG(info) << "==========================================================";
  LOG(info) << GetName() << ": Initialisation";

  // Deactivate noise in event-by-event mode
  if ( fEventMode ) {
    LOG(info) << GetName() << ": Using event mode.";
    SetGenerateNoise(kFALSE);
  }

  // Instantiate SsdPhysics
  BmnSsdPhysics::Instance();

  // --- Write physics settings to the parameter container. The default values
  // --- are defined in the constructor; they may have been changed from the
  // --- macro level by SetProcesses().
  fDigiPar->SetProcesses(fEnergyLossModel, fUseLorentzShift, fUseDiffusion,
                         fUseCrossTalk, fGenerateNoise);

  // --- Screen output of settings
  LOG(info) << GetName() << ": " << fDigiPar->ToString();

  // --- Get FairRootManager instance
  FairRootManager* ioman = FairRootManager::Instance();
  assert ( ioman );

  // --- Get input array (BmnSsdPoint)
  fPoints = (TClonesArray*) ioman->GetObject("SsdPoint");
  assert ( fPoints );

  // --- Get input array (CbmMCTrack)
  fTracks = (TClonesArray*) ioman->GetObject("MCTrack");
  assert ( fTracks );

  // Register output array (SsdDigi)
  fDigis = new TClonesArray("BmnSsdDigi",1000);
  ioman->Register("SsdDigi", "SSD", fDigis,
                  IsOutputBranchPersistent("SsdDigi"));

  // Register output array (SsdDigiMatch)
  // For backward compatibility only; the match object is already member
  // of BmnSsdDigi.
  fMatches = new TClonesArray("BmnMatch", 1000);
  ioman->Register("SsdDigiMatch", "SSD", fMatches,
                  IsOutputBranchPersistent("SsdDigiMatch"));


  // --- Screen output
  LOG(info) << GetName() << ": Initialisation successful"
     ;
  LOG(info) << "=========================================================="
     ;
  std::cout << std::endl;

  // Set static initialisation flag
  fIsInitialised = kTRUE;

  return kSUCCESS;
}
// -------------------------------------------------------------------------



// -----   Initialisation of setup    --------------------------------------
void BmnSsdDigitize::InitSetup() {

  // Get the SSD setup interface
  fSetup = BmnSsdSetup::Instance();

  // Register this task and the parameter container to the setup
  fSetup->SetDigitizer(this);
  fSetup->SetDigiParameters(fDigiPar);

  // Set or read sensor parameters
  fSetup->SetDefaultSensorParameters(fSensorDinact, fSensorPitch,
                                     fSensorStereoF, fSensorStereoB);
  if ( fSensorParameterFile.IsNull() ) fSetup->Init();
  else fSetup->Init(nullptr, fSensorParameterFile);

  // Set sensor conditions, global or from file
  if ( fSensorConditionFile.IsNull() )
    fSetup->SetSensorConditions(fSensorVdep, fSensorVbias,
                                fSensorTemperature, fSensorCcoupling,
                                fSensorCinterstrip);
  else
    fSetup->SetSensorConditions(fSensorConditionFile);

  // Set module parameters, global or from file
  if ( fModuleParameterFile.IsNull() )
    fSetup->SetModuleParameters(fModuleDynRange, fModuleThreshold,
                                fModuleNofAdc, fModuleTresol, fModuleTdead,
                                fModuleNoise, fModuleZeroNoiseRate,
                                fModuleFracDeadChan);
  else
    fSetup->SetModuleParameters(fModuleParameterFile);

}
// -------------------------------------------------------------------------



// -----   Process the analog buffers of all modules   ---------------------
void BmnSsdDigitize::ProcessAnalogBuffers(Double_t readoutTime) {

  // --- Debug
  LOG(debug) << GetName() << ": Processing analog buffers with readout "
      << "time " << readoutTime << " ns";

  // --- Loop over all modules in the setup and process their buffers
  for (Int_t iModule = 0; iModule < fSetup->GetNofModules(); iModule++)
    fSetup->GetModule(iModule)->ProcessAnalogBuffer(readoutTime);

  // --- Debug output
  LOG(debug) << GetName() << ": " << fNofDigis
      << ( fNofDigis == 1 ? " digi " :  " digis " )
      << "created and sent to DAQ ";
  if ( fNofDigis ) LOG(debug) << "( from " << fixed
      << setprecision(3) << fTimeDigiFirst << " ns to "
      << fTimeDigiLast << " ns )";
  LOG(debug);

}
// -------------------------------------------------------------------------



// -----   Process points from MC event    ---------------------------------
void BmnSsdDigitize::ProcessMCEvent() {

  // --- MC Event info (input file, entry number, start time)
  LOG(debug) << GetName() << ": Processing event " << fCurrentEvent
      << " from input " << fCurrentInput << " at t = " << fCurrentEventTime
      << " ns with " << fPoints->GetEntriesFast() << " SsdPoints "
     ;


  // --- Loop over all SsdPoints and execute the ProcessPoint method
  assert ( fPoints );
  for (Int_t iPoint=0; iPoint<fPoints->GetEntriesFast(); iPoint++) {
    const BmnSsdPoint* point = (const BmnSsdPoint*) fPoints->At(iPoint);
    BmnLink* link = new BmnLink(1., iPoint, fCurrentEvent, fCurrentInput);

    // --- Discard secondaries if the respective flag is set
    if ( fDigiPar->GetDiscardSecondaries() ) {
      Int_t iTrack = point->GetTrackID();
      if ( iTrack >= 0 ) {  // MC track is present
        CbmMCTrack* track = (CbmMCTrack*) fTracks->At(iTrack);
        assert ( track );
        if ( track->GetMotherId() >= 0 ) continue;
      } //? MC track present
    } //? discard secondaries

    ProcessPoint(point, fCurrentEventTime, link);
    fNofPoints++;
    delete link;
  }  //# SsdPoints

}
// -------------------------------------------------------------------------



// -----  Process a SsdPoint   ---------------------------------------------
void BmnSsdDigitize::ProcessPoint(const BmnSsdPoint* point,
                                  Double_t eventTime, BmnLink* link) {

  // Debug
  if ( FairLogger::GetLogger()->IsLogNeeded(DEBUG2) ) point->Print();
  LOG(debug2) << GetName() << ": Point coordinates: in (" << point->GetXIn()
			            << ", " << point->GetYIn() << ", " << point->GetZIn() << ")"
			            << ", out (" << point->GetXOut() << ", " << point->GetYOut()
			            << ", " << point->GetZOut() << ")";


  // --- Get the sensor the point is in
  Int_t address = point->GetDetectorID();
  BmnSsdSensor* sensor = dynamic_cast<BmnSsdSensor*>
  (fSetup->GetElement(address, kSsdSensor));
  if ( ! sensor ) {
    LOG(info) << GetName() << ": No sensor for address " << address
       ;
    LOG(info) << "Unit " << BmnSsdAddress::GetElementId(address, kSsdUnit);
    LOG(info) << " Ladder " << BmnSsdAddress::GetElementId(address, kSsdLadder);
    LOG(info) << " Half-ladder " << BmnSsdAddress::GetElementId(address, kSsdHalfLadder);
    LOG(info) << " Module " << BmnSsdAddress::GetElementId(address, kSsdModule);
    LOG(info) << " Sensor " << BmnSsdAddress::GetElementId(address, kSsdSensor);
    LOG(info);
  }
  if ( ! sensor ) LOG(error) << GetName() << ": Sensor of SsdPoint not found!"
     ;
  assert(sensor);
  LOG(debug2) << GetName() << ": Sending point to sensor "
      << sensor->GetName() << " ( " << sensor->GetAddress()
      << " ) ";

  // --- Process the point on the sensor
  Int_t status = sensor->ProcessPoint(point, eventTime, link);

  // --- Statistics
  Int_t nSignalsF = status / 1000;
  Int_t nSignalsB = status - 1000 * nSignalsF;
  LOG(debug2) << GetName() << ": Produced signals: "
      << nSignalsF + nSignalsB << " ( " << nSignalsF << " / "
      << nSignalsB << " )";
  fNofSignalsF += nSignalsF;
  fNofSignalsB += nSignalsB;

}
// -------------------------------------------------------------------------



// -----   Private method ReInit   -----------------------------------------
InitStatus BmnSsdDigitize::ReInit() {

  fSetup = BmnSsdSetup::Instance();

  return kERROR;

}
// -------------------------------------------------------------------------



// -----   Clear the output data arrays   ----------------------------------
void BmnSsdDigitize::ResetArrays() {
  if ( fDigis ) fDigis->Delete();
  if ( fMatches ) fMatches->Delete();
}
// -------------------------------------------------------------------------



// -----   Reset event counters   ------------------------------------------
void BmnSsdDigitize::ResetCounters() {
  fTimeDigiFirst = fTimeDigiLast = -1.;
  fNofPoints = fNofSignalsF = fNofSignalsB = fNofDigis = 0;
}
// -------------------------------------------------------------------------



// -----   Set the default module parameters   -----------------------------
void BmnSsdDigitize::SetDefaultModuleParameters(Double_t dynRange,
                                                Double_t threshold,
                                                Int_t nAdc,
                                                Double_t timeResolution,
                                                Double_t deadTime,
                                                Double_t noise,
                                                Double_t zeroNoiseRate,
                                                Double_t fracDeadChan) {
  assert( ! fIsInitialised );
  assert( nAdc > 0 );
  assert( fracDeadChan >= 0. && fracDeadChan <= 1.);
  fModuleDynRange      = dynRange;
  fModuleThreshold     = threshold;
  fModuleNofAdc        = nAdc;
  fModuleTresol        = timeResolution;
  fModuleTdead         = deadTime;
  fModuleNoise         = noise;
  fModuleZeroNoiseRate = zeroNoiseRate;
  fModuleFracDeadChan  = fracDeadChan;
}
// -------------------------------------------------------------------------



// -----   Set the default sensor conditions   -----------------------------
void BmnSsdDigitize::SetDefaultSensorConditions(Double_t vDep,
                                                Double_t vBias,
                                                Double_t temperature,
                                                Double_t cCoupling,
                                                Double_t cInterstrip) {
  assert( ! fIsInitialised );
  fSensorVdep        = vDep;
  fSensorVbias       = vBias;
  fSensorTemperature = temperature;
  fSensorCcoupling   = cCoupling;
  fSensorCinterstrip = cInterstrip;
}
// -------------------------------------------------------------------------



// -----   Set the default sensor parameters   -----------------------------
void BmnSsdDigitize::SetDefaultSensorParameters(Double_t dInact,
                                                Double_t pitch,
                                                Double_t stereoF,
                                                Double_t stereoB) {
  assert( ! fIsInitialised );
  assert( dInact >= 0.);
  assert( pitch >= 0. );
  fSensorDinact    = dInact;
  fSensorPitch     = pitch;
  fSensorStereoF   = stereoF;
  fSensorStereoB   = stereoB;
}
// -------------------------------------------------------------------------



// -----   Activate noise generation   -------------------------------------
void BmnSsdDigitize::SetGenerateNoise(Bool_t choice) {

  if ( fIsInitialised ) {
    LOG(error) << GetName() << ": physics processes must be set before "
        << "initialisation! Statement will have no effect."
       ;
    return;
  }

  fGenerateNoise = choice;
}
// -------------------------------------------------------------------------



// -----   Set sensor parameter file   -------------------------------------
void BmnSsdDigitize::SetModuleParameterFile(const char* fileName) {

  assert( ! fIsInitialised );
  fModuleParameterFile = fileName;

}
// -------------------------------------------------------------------------



// -----   Set the digitisation parameters   -------------------------------
void BmnSsdDigitize::SetParameters(Double_t dynRange, Double_t threshold,
                                   Int_t nAdc, Double_t timeResolution,
                                   Double_t deadTime, Double_t noise,
                                   Double_t zeroNoiseRate,
                                   Double_t deadChannelFrac) {

  if ( fIsInitialised ) {
    LOG(error) << GetName() << ": physics processes must be set before "
        << "initialisation! Statement will have no effect."
       ;
    return;
  }
  assert(fDigiPar);
  fDigiPar->SetModuleParameters(dynRange, threshold, nAdc, timeResolution,
                                deadTime, noise, zeroNoiseRate,
                                deadChannelFrac);
}
// -------------------------------------------------------------------------



// -----   Set the switches for physical processes for the analogue response
void BmnSsdDigitize::SetProcesses(Int_t eLossModel,
                                  Bool_t useLorentzShift,
                                  Bool_t useDiffusion,
                                  Bool_t useCrossTalk,
                                  Bool_t generateNoise) {
  if ( fIsInitialised ) {
    LOG(error) << GetName() << ": physics processes must be set before "
        << "initialisation! Statement will have no effect."
       ;
    return;
  }
  fEnergyLossModel = eLossModel;
  fUseLorentzShift = useLorentzShift;
  fUseDiffusion    = useDiffusion;
  fUseCrossTalk    = useCrossTalk;
  fGenerateNoise   = generateNoise;
}
// -------------------------------------------------------------------------



// -----   Set sensor condition file   -------------------------------------
void BmnSsdDigitize::SetSensorConditionFile(const char* fileName) {

  if ( fIsInitialised ) {
    LOG(fatal) << GetName()
            <<": sensor conditions must be set before initialisation!"
           ;
    return;
  }
  fSensorConditionFile = fileName;

}
// -------------------------------------------------------------------------



// -----   Set sensor parameter file   -------------------------------------
void BmnSsdDigitize::SetSensorParameterFile(const char* fileName) {

  if ( fIsInitialised ) {
    LOG(fatal) << GetName()
            <<": sensor parameters must be set before initialisation!"
           ;
    return;
  }
  fSensorParameterFile = fileName;

}
// -------------------------------------------------------------------------



// -----   Write a digi to the output array   ------------------------------
void BmnSsdDigitize::WriteDigi(BmnDigi* digi) {

  // --- Assert that it is a SsdDigi
  BmnSsdDigi* ssdDigi = dynamic_cast<BmnSsdDigi*>(digi);
  if ( ! ssdDigi ) LOG(fatal) << fName
      << ": not a valid SsdDigi pointer!";

  assert(fDigis);
  assert(fMatches);
  Int_t nDigis = fDigis->GetEntriesFast();
  new( (*fDigis)[nDigis] ) BmnSsdDigi(*ssdDigi);
  new( (*fMatches)[nDigis] ) BmnMatch(*(ssdDigi->GetMatch()));

}
// -------------------------------------------------------------------------



ClassImp(BmnSsdDigitize)

