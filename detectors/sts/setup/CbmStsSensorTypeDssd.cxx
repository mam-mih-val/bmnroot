/** @file CbmStsSensorTypeDssd.cxx
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 02.05.2013
 **/

#include <iomanip>

#include "TMath.h"

#include "FairLogger.h"

#include "setup/CbmStsAddress.h"
#include "setup/CbmStsModule.h"
#include "setup/CbmStsSenzor.h"
#include "setup/CbmStsSensorPoint.h"
#include "setup/CbmStsSensorTypeDssd.h"


using namespace std;


// --- Energy for creation of an electron-hole pair in silicon [GeV]  ------
const double kPairEnergy = 3.6e-9;



// -----   Constructor   ---------------------------------------------------
CbmStsSensorTypeDssd::CbmStsSensorTypeDssd()
    : CbmStsSensorType(), 
      fDx(-1.), fDy(-1.), fDz(-1.),
      fPitch(), fStereo(), fIsSet(kFALSE),
      fNofStrips(), fCosStereo(), fSinStereo(), fStripShift()
{
}
// -------------------------------------------------------------------------



// -----   Get channel number in module   ----------------------------------
Int_t CbmStsSensorTypeDssd::GetModuleChannel(Int_t strip, Int_t side,
                                             Int_t sensorId) const {

  // --- Check side
  if ( side < 0 || side > 1 ) {
    LOG(error) << "Illegal side qualifier " << side;
    return -1;
  }

  // --- Account for offset due to stereo angle
  Int_t channel = strip - sensorId * fStripShift[side];

  // --- Account for horizontal cross-connection of strips
  while ( channel < 0 ) channel += fNofStrips[side];
  while ( channel >= fNofStrips[side] ) channel -= fNofStrips[side];

  // --- Account for front or back side
  if ( side ) channel += fNofStrips[0];

  return channel;
}
// -------------------------------------------------------------------------



// -----   Get strip and side from channel number   ------------------------
void CbmStsSensorTypeDssd::GetStrip(Int_t channel, Int_t sensorId,
                                    Int_t& strip, Int_t& side) {

  Int_t stripNr = -1;
  Int_t sideNr  = -1;


  // --- Determine front or back side
  if ( channel < fNofStrips[0] ) {          // front side
    sideNr = 0;
    stripNr = channel;
  }
  else {
    sideNr = 1;
    stripNr = channel - fNofStrips[0];      // back side
  }
  side = sideNr;

  // --- Offset due to stereo angle
  stripNr += sensorId * fStripShift[side];

  // --- Horizontal cross-connection
  while ( stripNr < 0 ) stripNr += fNofStrips[side];
  while ( stripNr >= fNofStrips[side] ) stripNr -= fNofStrips[side];

  strip = stripNr;
  return;
}
// -------------------------------------------------------------------------




// -----   Print parameters   ----------------------------------------------
void CbmStsSensorTypeDssd::Print(Option_t* opt) const {

  LOG(info) << "Properties of sensor type " << GetName() << ": ";
            << "\t  Dimensions: (" << fixed << setprecision(4)
            << fDx << ", " << fDy << ", " << fDz << ") cm";
            << "\t  Front side: pitch = "
            << setprecision(0) << fPitch[0]*1.e4 << " mum, "
            << fNofStrips[0] << " strips, stereo angle "
            << setprecision(1) << fStereo[0] << " degrees";
            << "\t  Back side:  pitch = "
            << setprecision(0) << fPitch[1]*1.e4 << " mum, "
            << fNofStrips[1] << " strips, stereo angle "
            << setprecision(1) << fStereo[1] << " degrees";

}
// -------------------------------------------------------------------------



// -----   Process an MC Point  --------------------------------------------
void CbmStsSensorTypeDssd::ProcessPoint(CbmStsSensorPoint* point,
                                        const CbmStsSenzor* sensor) const {

  // --- Catch if parameters are not set
  if ( ! fIsSet ) {
    LOG(fatal) << fName << ": parameters are not set!";
    return;
  }

  ProduceCharge(point, 0, sensor);
  ProduceCharge(point, 1, sensor);

}
// -------------------------------------------------------------------------



// -----   Produce charge on the strips   ----------------------------------
void CbmStsSensorTypeDssd::ProduceCharge(CbmStsSensorPoint* point,
                                         Int_t side,
                                         const CbmStsSenzor* sensor) const {

  // --- Protect against being called without parameters being set
  if ( ! fIsSet ) LOG(fatal) << "Parameters of sensor " << fName
                             << " are not set!";

  // This implementation assumes a straight trajectory in the sensor
  // and a constant charge distribution along it.

  // Check for side qualifier
  if ( side < 0 || side > 1 )  {
    LOG(error) << "Illegal side qualifier!";
    return;
  }

  // Total produced charge
  Double_t qtot = point->GetELoss() / kPairEnergy;

  // Stereo angle and strip pitch
  Double_t tanphi = fSinStereo[side] / fCosStereo[side];
  Double_t pitch  = fPitch[side];
  Int_t nStrips   = fNofStrips[side];


  // Project point coordinates (in / out) to readout (top) edge
  // Keep in mind that the SensorPoint gives coordinates with
  // respect to the centre of the active area/
  Double_t x1 = point->GetX1() - 0.5 * fDx
                - ( 0.5 * fDy - point->GetY1() ) * tanphi;
  Double_t x2 = point->GetX2() - 0.5 * fDx
                - ( 0.5 * fDy - point->GetY2() ) * tanphi;


  // Calculate corresponding strip numbers
  // This can be negative or larger than the number of channels if the
  // strip does not extend to the top edge.
  Int_t i1 = TMath::FloorNint( x1 / pitch );
  Int_t i2 = TMath::FloorNint( x2 / pitch );


  // --- More than one strip: sort strips
  if ( i1 > i2 ) {
    Int_t tempI = i1;
    i1 = i2;
    i2 = tempI;
    Double_t tempX = x1;
    x1 = x2;
    x2 = tempX;
  }


  // --- Loop over fired strips
  for (Int_t iStrip = i1; iStrip <= i2; iStrip++) {

    // --- Calculate charge in strip
    Double_t y1 = TMath::Max(x1, Double_t(iStrip) * pitch);  // start in strip
    Double_t y2 = TMath::Min(x2, Double_t(iStrip+1) * pitch); // stop in strip
    Double_t charge = (y2 - y1) * qtot / ( x2 - x1 );

    // --- Register charge to module
    RegisterCharge(sensor, side, iStrip, charge, point->GetTime());

  } // Loop over fired strips

}
// -------------------------------------------------------------------------



// -----   Register charge   -----------------------------------------------
void CbmStsSensorTypeDssd::RegisterCharge(const CbmStsSenzor* sensor,
                                          Int_t side, Int_t strip,
                                          Double_t charge,
                                          Double_t time) const {

  // --- Determine module channel for given sensor strip
  Int_t channel = GetModuleChannel(strip, side, sensor->GetSensorId() );

  // --- Send signal to module
  sensor->GetModule()->AddSignal(channel, time, charge);

  // --- Debug output
  LOG(debug3) << fName << ": Registering charge: side " << side
              << ", strip " << strip << ", time " << time
              << ", charge " << charge;

}
// -------------------------------------------------------------------------



// -----   Self test   -----------------------------------------------------
Bool_t CbmStsSensorTypeDssd::SelfTest() {

  for (Int_t sensorId = 0; sensorId < 3; sensorId++ ) {
    for (Int_t side = 0; side < 2; side ++ ) {
      for (Int_t strip = 0; strip < fNofStrips[side]; strip++ ) {
        Int_t channel = GetModuleChannel(strip, side, sensorId);
        Int_t testStrip, testSide;
        GetStrip(channel, sensorId, testStrip, testSide);
        if ( testStrip != strip || testSide != side ) {
          LOG(error) << fName << "Self test failed! Sensor " << sensorId
                     << " side " << side << " strip " << strip
                     << " gives channel " << channel << " gives strip "
                     << testStrip << " side " << testSide;
          return kFALSE;
        }
      } // strip loop
    } // side loop
  } // sensor loop

  LOG(info) << fName << ": self test passed";
  return kTRUE;
}
// -------------------------------------------------------------------------



// -----   Set the parameters   --------------------------------------------
void CbmStsSensorTypeDssd::SetParameters(Double_t dx, Double_t dy,
                                         Double_t dz, Int_t nStripsF,
                                         Int_t nStripsB, Double_t stereoF,
                                         Double_t stereoB) {

  // --- Check stereo angles
  if ( TMath::Abs(stereoF) > 85. )
    LOG(fatal) << "Stereo angle for front side " << stereoF
               << "outside allowed limits! "
               << "Must be between -85 and 85 degrees.";
  if ( TMath::Abs(stereoB) > 85. )
    LOG(fatal) << "Stereo angle for back side " << stereoB
               << "outside allowed limits! "
               << "Must be between -85 and 85 degrees.";

  // --- Set members
  fDx           = dx;
  fDy           = dy;
  fDz           = dz;
  fNofStrips[0] = nStripsF;
  fNofStrips[1] = nStripsB;
  fStereo[0]    = stereoF;
  fStereo[1]    = stereoB;

  // --- Calculate parameters for front and back
  for (Int_t side = 0; side < 2; side++) {
    fPitch[side] = fDx / Double_t(fNofStrips[side]);
    fCosStereo[side] = TMath::Cos( fStereo[side] * TMath::DegToRad() );
    fSinStereo[side] = TMath::Sin( fStereo[side] * TMath::DegToRad() );
    Double_t tanPhi = fSinStereo[side] / fCosStereo[side];
    fStripShift[side] = TMath::Nint(fDy * tanPhi / fPitch[side]);
  }

  // --- Flag parameters to be set if test is ok
  fIsSet = SelfTest();

}
// -------------------------------------------------------------------------



ClassImp(CbmStsSensorTypeDssd)
