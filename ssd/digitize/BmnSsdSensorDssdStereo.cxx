/** @file BmnSsdSensorTypeStereo.cxx
 ** @author Volker Friese <v.friese@gsi.de>
 ** @author D. Baranov
 ** @date 20.12.2018
 **/

#include "BmnSsdSensorDssdStereo.h"

#include "TGeoBBox.h"
#include "TMath.h"

#include "BmnMatch.h"
#include "BmnSsdDigitizeParameters.h"
#include "BmnSsdPhysics.h"
#include "BmnSsdSetup.h"


using namespace std;


// -----   Constructor   ---------------------------------------------------
BmnSsdSensorDssdStereo::BmnSsdSensorDssdStereo(UInt_t address,
                                               TGeoPhysicalNode* node,
                                               BmnSsdElement* mother) :
             BmnSsdSensorDssd(address, node, mother),
             fNofStrips(0), fPitch(0.), fStereoF(100.), fStereoB(100.),
             fTanStereo(), fCosStereo(), fStripShift(), fErrorFac(0.)
{
  SetTitle("DssdStereo");
}
// -------------------------------------------------------------------------



// -----   Constructor   ---------------------------------------------------
BmnSsdSensorDssdStereo::BmnSsdSensorDssdStereo(Double_t dy, Int_t nStrips,
                                               Double_t pitch,
                                               Double_t stereoF,
                                               Double_t stereoB) :
    BmnSsdSensorDssd(),
    fNofStrips(nStrips),
    fPitch(pitch),
    fStereoF(stereoF),
    fStereoB(stereoB),
    fTanStereo(),
    fCosStereo(),
    fStripShift(),
    fErrorFac(0.)
{
  SetTitle("DssdStereo");
  fDy = dy;
}
// -------------------------------------------------------------------------



// -----   Diffusion   -----------------------------------------------------
void BmnSsdSensorDssdStereo::Diffusion(Double_t x, Double_t y,
                                       Double_t sigma, Int_t side,
                                       Double_t& fracL, Double_t& fracC,
                                       Double_t& fracR) {

  // Check side qualifier
  assert( side == 0 || side == 1);

  // x coordinate at the readout edge (y = fDy/2 )
  // This x is counted from the left edge.
  Double_t xRo = x + fDx / 2. - ( fDy / 2. - y ) * fTanStereo[side];

  // Centre strip number (w/o cross connection; may be negative or large than
  // the number of strips)
  Int_t iStrip = TMath::FloorNint(xRo / fPitch);

  // Strip boundaries at the readout edge (y = fDy/2)
  Double_t xLeftRo  = Double_t(iStrip) * fPitch;
  Double_t xRightRo = xLeftRo + fPitch;

  // Distance from strip boundaries across the strip
  Double_t dLeft  = ( xRo - xLeftRo )  * fCosStereo[side];
  Double_t dRight = ( xRightRo - xRo ) * fCosStereo[side];

  // Charge fractions
  // The value 0.707107 is 1/sqrt(2)
  fracL = 0.;
  if ( dLeft < 3. * sigma )
    fracL = 0.5 * ( 1. - TMath::Erf( 0.707107 * dLeft  / sigma) );
  fracR = 0.;
  if ( dRight < 3. * sigma )
    fracR = 0.5 * ( 1. - TMath::Erf( 0.707107 * dRight / sigma) );
  fracC = 1. - fracL - fracR;

  LOG(DEBUG4) << GetName() << ": Distances to next strip " << dLeft << " / "
      << dRight << ", charge fractions " << fracL << " / " << fracC
      << " / " << fracR;
}
// -------------------------------------------------------------------------



// -----   Get channel number in module   ----------------------------------
Int_t BmnSsdSensorDssdStereo::GetModuleChannel(Int_t strip, Int_t side,
                                               Int_t sensorId) const {

  // --- Check side argument
  assert( side == 0 || side == 1);

  // --- Account for offset due to stereo angle
  Int_t channel = strip - sensorId * fStripShift[side];

  // --- Account for horizontal cross-connection of strips
  while ( channel < 0 )           channel += fNofStrips;
  while ( channel >= fNofStrips ) channel -= fNofStrips;

  // --- Account for front or back side
  if ( side ) channel += fNofStrips;

  return channel;
}
// -------------------------------------------------------------------------



// -----   Get strip and side from channel number   ------------------------
pair<Int_t, Int_t> BmnSsdSensorDssdStereo::GetStrip(Int_t channel,
                                                    Int_t sensorId) const  {

  Int_t stripNr = -1;
  Int_t side    = -1;

  // --- Determine front or back side
  if ( channel < fNofStrips ) {          // front side
    side = 0;
    stripNr = channel;
  }
  else {                                 // back side
    side = 1;
    stripNr = channel - fNofStrips;
  }

  // --- Offset due to stereo angle
  stripNr += sensorId * fStripShift[side];

  // --- Horizontal cross-connection
  while ( stripNr < 0 )           stripNr += fNofStrips;
  while ( stripNr >= fNofStrips ) stripNr -= fNofStrips;

  return ( pair<Int_t, Int_t>(stripNr, side) );
}
// -------------------------------------------------------------------------



// -----   Get strip number from coordinates   -----------------------------
Int_t BmnSsdSensorDssdStereo::GetStripNumber(Double_t x, Double_t y,
                                             Int_t side) const {

  // Cave: This implementation assumes that the centre of the sensor volume
  // is also the centre of the active area, i.e. that the inactive borders
  // (guard ring) are symmetric both and x and y (not necessarily the same
  // in x and y).

  // Check side
  assert( side == 0 || side == 1);

  // Check whether in active area (should have been caught before)
  if ( TMath::Abs(x) > fDx / 2. ) {
    LOG(ERROR) << GetName() << ": Outside active area : x = "
        << x << " cm";
    return -1;
  }
  if ( TMath::Abs(y) > fDy / 2. ) {
    LOG(ERROR) << GetName() << ": Outside active area : y = "
        << y << " cm";
    return -1;
  }

  // Calculate distance from lower left corner of the active area.
  // Note: the coordinates are given w.r.t. the centre of the volume.
  Double_t xdist = x + 0.5 * fDx;
  Double_t ydist = y + 0.5 * fDy;

  // Project coordinates to readout (top) edge
  Double_t xro = xdist - ( fDy - ydist ) * fTanStereo[side];

  // Calculate corresponding strip number
  Int_t iStrip = TMath::FloorNint( xro / fPitch );

  // Account for horizontal cross-connection of strips
  // not extending to the top edge
  while ( iStrip < 0 )                 iStrip += fNofStrips;
  while ( iStrip >= fNofStrips )       iStrip -= fNofStrips;

  return iStrip;
}
// -------------------------------------------------------------------------



// -----   Initialise   ----------------------------------------------------
Bool_t BmnSsdSensorDssdStereo::Init() {

  // Check presence of node
  if ( ! fNode ) {
    LOG(ERROR) << GetName() << ": No node assigned!";
    return kFALSE;
  }

  // Check whether parameters are assigned
  if ( fNofStrips <= 0 ) {
    LOG(ERROR) << GetName() << ": Parameters are not set!"
       ;
    return kFALSE;
  }

  // Geometric shape of the sensor volume
  TGeoBBox* shape = dynamic_cast<TGeoBBox*>(fNode->GetShape());
  assert(shape);

  // Active size in x coordinate
  fDx = Double_t(fNofStrips) * fPitch;
  if ( fDx >= 2. * shape->GetDX() ) {
    LOG(ERROR) << GetName() << ": Active size in x ( " << fNofStrips << " x "
        << fPitch << " cm exceeds volume extension " << 2. * shape->GetDX()
       ;
    return kFALSE;
  }

  // Active size in y coordinate
  if ( fDy >= 2. * shape->GetDY() ) {
    LOG(ERROR) << GetName() << ": Active size in y ( " << fDy
        << " cm exceeds volume extension " << 2. * shape->GetDY()
       ;
    return kFALSE;
  }

  // Active size in z coordinate
  fDz = 2. * shape->GetDZ();

  // Stereo angle front side must be between -85 and 85 degrees
  if ( TMath::Abs(fStereoF) > 85. ) {
    LOG(ERROR) << GetName() << ": Stereo angle front side ( " << fStereoF
        << "?? exceeds maximum 85?? ";
    return kFALSE;
  }

  // Stereo angle back side must be between -85 and 85 degrees
  if ( TMath::Abs(fStereoB) > 85. ) {
    LOG(ERROR) << GetName() << ": Stereo angle back side ( " << fStereoB
        << "?? exceeds maximum 85?? ";
    return kFALSE;
  }

  // Derived variables
  fTanStereo[0]  = TMath::Tan( fStereoF * TMath::DegToRad() );
  fCosStereo[0]  = TMath::Cos( fStereoF * TMath::DegToRad() );
  fStripShift[0] = TMath::Nint(fDy * fTanStereo[0] / fPitch);
  fTanStereo[1]  = TMath::Tan( fStereoB * TMath::DegToRad() );
  fCosStereo[1]  = TMath::Cos( fStereoB * TMath::DegToRad() );
  fStripShift[1] = TMath::Nint(fDy * fTanStereo[1] / fPitch);

  // Set size of charge arrays
  fStripCharge[0].Set(fNofStrips);
  fStripCharge[1].Set(fNofStrips);

  // Factor for the hit position error
  fErrorFac = 1. / ( fTanStereo[1] - fTanStereo[0] )
                     / ( fTanStereo[1] - fTanStereo[0] );

  // --- Flag parameters to be set if test is OK
  fIsSet = SelfTest();

  return fIsSet;
}
// -------------------------------------------------------------------------



// -----   Intersection of two lines along the strips   --------------------
Bool_t BmnSsdSensorDssdStereo::Intersect(Double_t xF, Double_t exF,
                                         Double_t xB, Double_t exB,
                                         Double_t& x, Double_t& y,
                                         Double_t& varX, Double_t& varY,
                                         Double_t& varXY) {

  // In the coordinate system with origin at the bottom left corner,
  // a line along the strips with coordinate x0 at the top edge is
  // given by the function y(x) = Dy - ( x - x0 ) / tan(phi), if
  // the stereo angle phi does not vanish. Two lines yF(x), yB(x) with top
  // edge coordinates xF, xB intersect at
  // x = ( tan(phiB)*xF - tan(phiF)*xB ) / (tan(phiB) - tan(phiF)
  // y = Dy + ( xB - xF ) / ( tan(phiB) - tan(phiF) )
  // For the case that one of the stereo angles vanish (vertical strips),
  // the calculation of the intersection is straightforward.

  // --- First check whether stereo angles are different. Else there is
  // --- no intersection.
  if ( TMath::Abs(fStereoF-fStereoB) < 0.5 ) {
    x = -1000.;
    y = -1000.;
    return kFALSE;
  }

  // --- Now treat vertical front strips
  if ( TMath::Abs(fStereoF) < 0.001 ) {
    x = xF;
    y = fDy - ( xF - xB ) / fTanStereo[1];
    varX = exF * exF;
    varY = ( exF * exF + exB * exB ) / fTanStereo[1] / fTanStereo[1];
    varXY = -1. * exF * exF / fTanStereo[1];
    return IsInside(x-fDx/2., y-fDy/2.);
  }

  // --- Maybe the back side has vertical strips?
  if ( TMath::Abs(fStereoB) < 0.001 ) {
    x = xB;
    y = fDy - ( xB - xF ) / fTanStereo[0];
    varX = exB * exB;
    varY = ( exF * exF + exB * exB) / fTanStereo[0] / fTanStereo[0];
    varXY = -1. * exB * exB / fTanStereo[0];
    return IsInside(x-fDx/2., y-fDy/2.);
  }

  // --- OK, both sides have stereo angle
  x = ( fTanStereo[1] * xF - fTanStereo[0] * xB ) /
      ( fTanStereo[1] - fTanStereo[0]);
  y = fDy + ( xB - xF ) / ( fTanStereo[1] - fTanStereo[0]);
  varX = fErrorFac * ( exF * exF * fTanStereo[1] * fTanStereo[1]
                                                              + exB * exB * fTanStereo[0] * fTanStereo[0] );
  varY = fErrorFac * ( exF * exF + exB * exB );
  varXY = -1. * fErrorFac * ( exF * exF * fTanStereo[1]
                                                     + exB * exB * fTanStereo[0] );

  // --- Check for being in active area.
  return IsInside(x-fDx/2., y-fDy/2.);

}
// -------------------------------------------------------------------------



// -----   Create hits from two clusters   ---------------------------------
Int_t BmnSsdSensorDssdStereo::IntersectClusters(BmnSsdCluster* clusterF,
                                                BmnSsdCluster* clusterB) {
  // --- Check pointer validity
  assert(clusterF);
  assert(clusterB);

  // --- Ideal hit finder
  if ( kFALSE ){  //TODO Proper implementation
    LOG(DEBUG3) << GetName() << ": ideal model of Hit Finder";

    const BmnMatch *clusterFMatch, *clusterBMatch;

    clusterFMatch = static_cast<const BmnMatch*>(clusterF -> GetMatch());
    if (clusterFMatch){
      LOG(DEBUG4) << GetName() << ": front cluster exists";
      if ((clusterFMatch -> GetNofLinks()) != 1) {
        LOG(DEBUG4) << GetName() << ": front cluster has more or less than 1 BmnLink";
        return 0;
      } else LOG(DEBUG4) << GetName() << ": front cluster has " <<  clusterFMatch -> GetNofLinks() << " BmnLink";
    } else return 0;

    clusterBMatch = static_cast<const BmnMatch*> (clusterB -> GetMatch());
    if (clusterBMatch){
      LOG(DEBUG4) << GetName() << ": back cluster exists";
      if ((clusterBMatch -> GetNofLinks()) != 1){
        LOG(DEBUG4) << GetName() << ": back cluster has more or less than 1 BmnLink";
        return 0;
      } else LOG(DEBUG4) << GetName() << ": back cluster has " <<  clusterBMatch -> GetNofLinks() << " BmnLink";
    } else return 0;

    if (clusterBMatch -> GetLink(0).GetIndex() != clusterFMatch -> GetLink(0).GetIndex()){
      LOG(DEBUG4) << GetName() << ": back and front clusters have different index of BmnLink";
      return 0;
    } else LOG(DEBUG4) << GetName() << ": back and front clusters have the same index of BmnLink";
  }

  // --- Calculate cluster centre position on readout edge
  Int_t side  = -1;
  Double_t xF = -1.;
  Double_t xB = -1.;
  GetClusterPosition(clusterF->GetPosition(), xF, side);
  if ( side != 0 )
    LOG(FATAL) << GetName() << ": Inconsistent side qualifier " << side
    << " for front side cluster! ";
  Double_t exF = clusterF->GetPositionError() * fPitch;
  Double_t du = exF * TMath::Cos(TMath::DegToRad() * fStereoF);
  GetClusterPosition(clusterB->GetPosition(), xB, side);
  if ( side != 1 )
    LOG(FATAL) << GetName() << ": Inconsistent side qualifier " << side
    << " for back side cluster! ";
  Double_t exB = clusterB->GetPositionError() * fPitch;
  Double_t dv = exB * TMath::Cos(TMath::DegToRad() * fStereoB);

  // --- Should be inside active area
  if ( ! ( xF >= 0. || xF <= fDx) ) return 0;
  if ( ! ( xB >= 0. || xB <= fDx) ) return 0;

  // --- Hit counter
  Int_t nHits = 0;

  // --- Calculate number of line segments due to horizontal
  // --- cross-connection. If x(y=0) does not fall on the bottom edge,
  // --- the strip is connected to the one corresponding to the line
  // --- with top edge coordinate xF' = xF +/- Dx. For odd combinations
  // --- of stereo angle and sensor dimensions, this could even happen
  // --- multiple times. For each of these lines, the intersection with
  // --- the line on the other side is calculated. If inside the active area,
  // --- a hit is created.
  Int_t nF = Int_t( (xF + fDy * fTanStereo[0]) / fDx );
  Int_t nB = Int_t( (xB + fDy * fTanStereo[1]) / fDx );

  // --- If n is positive, all lines from 0 to n must be considered,
  // --- if it is negative (phi negative), all lines from n to 0.
  Int_t nF1 = TMath::Min(0, nF);
  Int_t nF2 = TMath::Max(0, nF);
  Int_t nB1 = TMath::Min(0, nB);
  Int_t nB2 = TMath::Max(0, nB);

  // --- Double loop over possible lines
  Double_t xC = -1.;   // x coordinate of intersection point
  Double_t yC = -1.;   // y coordinate of intersection point
  Double_t varX = 0.;  // variance of xC
  Double_t varY = 0.;  // variance of yC
  Double_t varXY = 0.; // covariance xC-yC
  for (Int_t iF = nF1; iF <= nF2; iF++) {
    Double_t xFi = xF - Double_t(iF) * fDx;
    for (Int_t iB = nB1; iB <= nB2; iB++) {
      Double_t xBi = xB - Double_t(iB) * fDx;

      // --- Intersect the two lines
      Bool_t found = Intersect(xFi, exF, xBi, exB, xC, yC, varX, varY, varXY);
      LOG(DEBUG4) << GetName() << ": Trying " << xFi << ", " << xBi
          << ", intersection ( " << xC << ", " << yC
          << " ) " << ( found ? "TRUE" : "FALSE" )
         ;
      if ( found ) {

        // --- Transform into sensor system with origin at sensor centre
        xC -= 0.5 * fDx;
        yC -= 0.5 * fDy;
        // --- Create the hit
        CreateHit(xC, yC, varX, varY, varXY, clusterF, clusterB, du, dv);
        nHits++;

      }  //? Intersection of lines
    }  // lines on back side
  }  // lines on front side

  return nHits;
}
// -------------------------------------------------------------------------



// -----   Modify the strip pitch   ----------------------------------------
void BmnSsdSensorDssdStereo::ModifyStripPitch(Double_t pitch) {

  assert(fIsSet);  // Parameters should have been set before

  // Set new pitch and re-calculate number of strips
  fPitch = pitch;
  fNofStrips = Int_t( fDx / pitch );
  fDx = Double_t(fNofStrips) * pitch;

  // Set size of charge arrays
  fStripCharge[0].Set(fNofStrips);
  fStripCharge[1].Set(fNofStrips);

}
// -------------------------------------------------------------------------



// -----   Propagate charge to the readout strips   ------------------------
void BmnSsdSensorDssdStereo::PropagateCharge(Double_t x, Double_t y,
                                             Double_t z, Double_t charge,
                                             Double_t bY, Int_t side) {

  // Check side qualifier
  assert( side == 0 || side == 1);

  Double_t xCharge = x;
  Double_t yCharge = y;
  Double_t zCharge = z;

  // Debug
  LOG(DEBUG4) << GetName() << ": Propagating charge " << charge
      << " from (" << x << ", " << y << ", " << z
      << ") on side " << side << " of sensor " << GetName()
     ;

  // Lorentz shift on the drift to the readout plane
  if ( BmnSsdSetup::Instance()->GetDigiParameters()->GetUseLorentzShift() ) {
    xCharge += LorentzShift(z, side, bY);
    LOG(DEBUG4) << GetName() << ": After Lorentz shift: (" << xCharge << ", "
        << yCharge << ", " << zCharge << ") cm";
  }

  // Stop is the charge after Lorentz shift is not in the active area.
  // Diffusion into the active area is not treated.
  if ( ! IsInside(xCharge, yCharge) ) {
    LOG(DEBUG4) << GetName() << ": Charge outside active area"
       ;
    return;
  }

  // No diffusion: all charge is in one strip
  if ( ! BmnSsdSetup::Instance()->GetDigiParameters()->GetUseDiffusion() ) {
    Int_t iStrip = GetStripNumber(xCharge, yCharge, side);
    fStripCharge[side][iStrip] += charge;
    LOG(DEBUG4) << GetName() << ": Adding charge " << charge << " to strip "
        << iStrip;
  } //? Do not use diffusion

  // Diffusion: charge is distributed over centre strip and neighbours
  else {
    // Calculate diffusion width
    Double_t diffusionWidth =
        BmnSsdPhysics::DiffusionWidth(z + fDz / 2.,  // distance from back side
                                      fDz,
                                      GetConditions()->GetVbias(),
                                      GetConditions()->GetVfd(),
                                      GetConditions()->GetTemperature(),
                                      side);
    assert (diffusionWidth >= 0.);
    LOG(DEBUG4) << GetName() << ": Diffusion width = " << diffusionWidth
        << " cm";
    // Calculate charge fractions in strips
    Double_t fracL = 0.;  // fraction of charge in left neighbour
    Double_t fracC = 1.;  // fraction of charge in centre strip
    Double_t fracR = 0.;  // fraction of charge in right neighbour
    Diffusion(xCharge, yCharge, diffusionWidth, side, fracL, fracC, fracR);
    // Calculate strip numbers
    // Note: In this implementation, charge can diffuse out of the sensitive
    // area only for vertical strips. In case of stereo angle (cross-connection
    // of strips), all charge is assigned to some strip, so the edge effects
    // are not treated optimally.
    Int_t iStripC  = GetStripNumber(xCharge, yCharge, side);  // centre strip
    Int_t iStripL  = 0;                                     // left neighbour
    Int_t iStripR  = 0;                                    // right neighbour
    if ( fTanStereo[side] < 0.0001 )  {   // vertical strips, no cross connection
      iStripL = iStripC - 1;  // might be = -1
      iStripR = iStripC + 1;  // might be = nOfStrips
    }
    else {   // stereo angle, cross connection
      iStripL = ( iStripC == 0 ? fNofStrips - 1 : iStripC - 1);
      iStripR = ( iStripC == fNofStrips - 1 ? 0 : iStripC + 1);
    }
    // Collect charge on the readout strips
    if ( fracC > 0. ) {
      fStripCharge[side][iStripC] += charge * fracC;    // centre strip
      LOG(DEBUG4) << GetName() << ": Adding charge " << charge * fracC
          << " to strip " << iStripC;
    }
    if ( fracL > 0. && iStripL >= 0 ) {
      fStripCharge[side][iStripL] += charge * fracL;  // right neighbour
      LOG(DEBUG4) << GetName() << ": Adding charge " << charge * fracL
          << " to strip " << iStripL;
    }
    if ( fracR > 0. && iStripR < fNofStrips ) {
      fStripCharge[side][iStripR] += charge * fracR;  // left neighbour
      LOG(DEBUG4) << GetName() << ": Adding charge " << charge * fracR
          << " to strip " << iStripR;
    }
  } //? Use diffusion

}
// -------------------------------------------------------------------------



// -----   String output   -------------------------------------------------
std::string BmnSsdSensorDssdStereo::ToString() const {
  stringstream ss;
  ss << "Sensor " << fName << " (" << GetTitle() << "): ";
  if ( ! GetPnode() ) ss << "no node assigned; ";
  else {
    TGeoBBox* shape = dynamic_cast<TGeoBBox*>(GetPnode()->GetShape());
    assert(shape);
    ss << "Dimension (" << 2. * shape->GetDX() << ", "
        << 2. * shape->GetDY() << ", " << 2. * shape->GetDZ() << ") cm, ";
  }
  ss << "dy " << fDy << " cm, ";
  ss << "# strips " << fNofStrips << ", pitch " << fPitch << " cm, ";
  ss << "stereo " << fStereoF << "/" << fStereoB << " degrees";
  if ( fConditions) ss << "\n Conditions: " << fConditions->ToString();
  return ss.str();
}
// -------------------------------------------------------------------------



ClassImp(BmnSsdSensorDssdStereo)
