/** @file BmnSsdSensorTypeDssd.cxx
 ** @author Volker Friese <v.friese@gsi.de>
 ** @author D. Baranov
 ** @date 19.12.2018
 **/

#include "BmnSsdSensorDssd.h"

#include <cassert>
#include "BmnSsdDigitize.h"
#include "BmnSsdDigitizeParameters.h"
#include "BmnSsdModule.h"
#include "BmnSsdPhysics.h"
#include "BmnSsdSensorPoint.h"
#include "BmnSsdSetup.h"


using namespace std;


// -----   Constructor   ---------------------------------------------------
BmnSsdSensorDssd::BmnSsdSensorDssd(Int_t address, TGeoPhysicalNode* node,
                                   BmnSsdElement* mother) :
              BmnSsdSensor(address, node, mother),
              fDx(0.), fDy(0.), fDz(0.), fIsSet(kFALSE)
{
}
// -------------------------------------------------------------------------



// -----   Cross talk   ----------------------------------------------------
void BmnSsdSensorDssd::CrossTalk(Double_t ctcoeff) {

  for (Int_t side = 0; side < 2; side++) {  // front and back side

    // Number of strips for this side
    Int_t nStrips = GetNofStrips(side);

    // First strip
    Double_t qLeft    = 0.;
    Double_t qCurrent = fStripCharge[side][0];
    fStripCharge[side][0] =
        (1. - ctcoeff ) * qCurrent + ctcoeff * fStripCharge[side][1];

    // Strips 1 to n-2
    for (Int_t strip = 1; strip < nStrips - 1; strip++) {
      qLeft    = qCurrent;
      qCurrent = fStripCharge[side][strip];
      fStripCharge[side][strip] =
          ctcoeff * ( qLeft + fStripCharge[side][strip+1] ) +
          ( 1. - 2. * ctcoeff ) * qCurrent;
    } //# strips

    // Last strip
    qLeft = qCurrent;
    qCurrent = fStripCharge[side][nStrips-1];
    fStripCharge[side][nStrips-1] =
        ctcoeff * qLeft + ( 1. - ctcoeff ) * qCurrent;

  } //# front and back side

}
// -------------------------------------------------------------------------



// -----  Hit finding   ----------------------------------------------------
Int_t BmnSsdSensorDssd::FindHits(std::vector<BmnSsdCluster*>& clusters,
                                 TClonesArray* hitArray, BmnEvent* event,
                                 Double_t dTime) {

  fHits = hitArray;
  fEvent = event;
  Int_t nHits = 0;

  Int_t nClusters = clusters.size();

  Int_t nClustersF = 0;
  Int_t nClustersB = 0;

  // --- Sort clusters into front and back side
  vector<Int_t> frontClusters;
  vector<Int_t> backClusters;
  Int_t side  = -1;         // front or back side
  for (Int_t iCluster = 0; iCluster < nClusters; iCluster++) {
    BmnSsdCluster* cluster = clusters[iCluster];
    side = GetSide( cluster->GetPosition() );

    if ( side == 0) {
      frontClusters.push_back(iCluster);
      nClustersF++;
    }
    else if ( side == 1 ) {
      backClusters.push_back(iCluster);
      nClustersB++;
    }
    else
      LOG(FATAL) << GetName() << ": Illegal side qualifier "
      << side;
  }  // Loop over clusters in module
  LOG(DEBUG3) << GetName() << ": " << nClusters << " clusters (front "
      << frontClusters.size() << ", back " << backClusters.size()
      << ") ";

  // --- Loop over front and back side clusters
  for (Int_t iClusterF = 0; iClusterF < nClustersF; iClusterF++) {
    BmnSsdCluster* clusterF = clusters[frontClusters[iClusterF]];
    for (Int_t iClusterB = 0; iClusterB < nClustersB;   iClusterB++) {
      BmnSsdCluster* clusterB = clusters[backClusters[iClusterB]];

      Double_t sigma = TMath::Sqrt( clusterF->GetTimeError() * clusterF->GetTimeError()
                                    + clusterB->GetTimeError() * clusterB->GetTimeError() );
      if ( fabs(clusterF->GetTime() - clusterB->GetTime()) > 4. * sigma ) continue;

      // --- Calculate intersection points
      Int_t nOfHits = IntersectClusters(clusterF, clusterB);
      LOG(DEBUG4) << GetName() << ": Cluster front " << iClusterF
          << ", cluster back " << iClusterB
          << ", intersections " << nOfHits;
      nHits += nOfHits;

    }  // back side clusters

  }  // front side clusters

  LOG(DEBUG3) << GetName() << ": Clusters " << nClusters << " ( "
      << nClustersF << " / " << nClustersB << " ), hits: "
      << nHits;


  return nHits;
}
// -------------------------------------------------------------------------



// -----   Get cluster position at read-out edge   -------------------------
void BmnSsdSensorDssd::GetClusterPosition(Double_t centre,
                                          Double_t& xCluster,
                                          Int_t& side) {

  // Take integer channel
  Int_t iChannel = Int_t(centre);
  Double_t xDif = centre - Double_t(iChannel);

  // Calculate corresponding strip on sensor
  Int_t iStrip = -1;
  pair<Int_t, Int_t> stripSide = GetStrip(iChannel, GetIndex());
  iStrip = stripSide.first;
  side = stripSide.second;

  // Re-add difference to integer channel. Convert channel to
  // coordinate
  xCluster = (Double_t(iStrip) + xDif + 0.5 ) * GetPitch(side);

  // Correct for Lorentz-Shift
  // Simplification: The correction uses only the y component of the
  // magnetic field. The shift is calculated using the mid-plane of the
  // sensor, which is not correct for tracks not traversing the entire
  // sensor thickness (i.e., are created or stopped somewhere in the sensor).
  // However, this is the best one can do in reconstruction.
  //Double_t mobility = (side == 0 ? 0.1650 : 0.0310 );  // in m^2/(Vs)
  //Double_t tanLorentz = mobility * sensor->GetConditions().GetBy();
  //xCluster -= tanLorentz * fDz / 2.;
  assert (BmnSsdSetup::Instance()->GetDigiParameters());
  assert ( GetConditions() );
  if ( BmnSsdSetup::Instance()->GetDigiParameters()->GetUseLorentzShift() ) {
    xCluster -= GetConditions()->GetMeanLorentzShift(side);
  }

  LOG(DEBUG4) << GetName() << ": Cluster centre " << centre
      << ", sensor index " << GetIndex() << ", side "
      << side << ", cluster position " << xCluster
     ;
  return;
}
// -------------------------------------------------------------------------



// -----   Check whether a point is inside the active area   ---------------
Bool_t BmnSsdSensorDssd::IsInside(Double_t x, Double_t y) {
  if ( x < -fDx/2. ) return kFALSE;
  if ( x >  fDx/2. ) return kFALSE;
  if ( y < -fDy/2. ) return kFALSE;
  if ( y >  fDy/2. ) return kFALSE;
  return kTRUE;
}
// -------------------------------------------------------------------------



// -----   Lorentz shift   -------------------------------------------------
Double_t BmnSsdSensorDssd::LorentzShift(Double_t z, Int_t chargeType,
                                        Double_t bY) const {

  // --- Drift distance to readout plane
  // Electrons drift to the front side (z = d/2), holes to the back side (z = -d/2)
  Double_t driftZ = 0.;
  if      ( chargeType == 0 ) driftZ = fDz / 2. - z;  // electrons
  else if ( chargeType == 1 ) driftZ = fDz / 2. + z;  // holes
  else {
    LOG(ERROR) << GetName() << ": illegal charge type " << chargeType
       ;
    return 0.;
  }

  // --- Hall mobility
  Double_t vBias = GetConditions()->GetVbias();
  Double_t vFd   = GetConditions()->GetVfd();
  Double_t eField = BmnSsdPhysics::ElectricField(vBias, vFd, fDz, z + fDz/2.);
  Double_t eFieldMax = BmnSsdPhysics::ElectricField(vBias, vFd, fDz, fDz);
  Double_t eFieldMin = BmnSsdPhysics::ElectricField(vBias, vFd, fDz, 0.);

  Double_t muHall;
  if (chargeType == 0) muHall = GetConditions()->HallMobility((eField + eFieldMax)/2., chargeType);
  if (chargeType == 1) muHall = GetConditions()->HallMobility((eField + eFieldMin)/2., chargeType);

  // --- The direction of the shift is the same for electrons and holes.
  // --- Holes drift in negative z direction, the field is in
  // --- positive y direction, thus the Lorentz force v x B acts in positive
  // --- x direction. Electrons drift in the opposite (positive z) direction,
  // --- but the have also the opposite charge sign, so the Lorentz force
  // --- on them is also in the positive x direction.
  Double_t shift = muHall * bY * driftZ * 1.e-4;
  LOG(DEBUG4) << GetName() << ": Drift " << driftZ
      << " cm, mobility " << muHall
      << " cm**2/(Vs), field " << bY
      << " T, shift " << shift << " cm";
  // The factor 1.e-4 is because bZ is in T = Vs/m**2, but muHall is in
  // cm**2/(Vs) and z in cm.

  return shift;
}
// -------------------------------------------------------------------------



// -----   Print charge status   -------------------------------------------
void BmnSsdSensorDssd::PrintChargeStatus() const {
  LOG(INFO) << GetName() << ": Charge status: \n";
  for (Int_t side = 0; side < 2; side++) {
    for (Int_t strip = 0; strip < GetNofStrips(side); strip++) {
      if ( fStripCharge[side][strip] > 0. )
        LOG(INFO) << "          " << (side ? "Back  " : "Front ") << "strip "
        << strip << "  charge " << fStripCharge[side][strip] << "\n";
    } //# strips
  } //# front and back side
  LOG(INFO) << "          Total: front side "
      << (fStripCharge[0]).GetSum() << ", back side "
      << (fStripCharge[1]).GetSum();
}
// -------------------------------------------------------------------------



// -----   Process one MC Point  -------------------------------------------
Int_t BmnSsdSensorDssd::CalculateResponse(BmnSsdSensorPoint* point) {

  // --- Catch if parameters are not set
  if ( ! fIsSet ) {
    LOG(FATAL) << fName << ": sensor is not initialised!"
       ;
    return -1;
  }

  // --- Debug
  LOG(DEBUG3) << ToString();
  LOG(DEBUG3) << GetName() << ": Processing point " << point->ToString()
                       ;

  // --- Number of created charge signals (coded front/back side)
  Int_t nSignals = 0;

  // --- Reset the strip charge arrays
  fStripCharge[0].Reset();   // front side
  fStripCharge[1].Reset();   // back side

  // --- Produce charge and propagate it to the readout strips
  ProduceCharge(point);

  // --- Cross talk
  if ( BmnSsdSetup::Instance()->GetDigiParameters()->GetUseCrossTalk() ) {
    if ( FairLogger::GetLogger()->IsLogNeeded(DEBUG4) ) {
      LOG(DEBUG4) << GetName() << ": Status before cross talk"
         ;
      PrintChargeStatus();
    }
    Double_t ctcoeff =  GetConditions()->GetCrossTalk();
    LOG(DEBUG4) << GetName() << ": Cross-talk coefficient is "
        << ctcoeff;
    CrossTalk(ctcoeff);
  }

  // --- Debug
  if ( FairLogger::GetLogger()->IsLogNeeded(DEBUG3) )
    PrintChargeStatus();

  // --- Stop here if no module is connected (e.g. for test purposes)
  if ( ! GetModule() ) return 0;

  // --- Register charges in strips to the module
  Int_t nCharges[2] = { 0, 0 };
  for (Int_t side = 0; side < 2; side ++) {  // front and back side

    for (Int_t strip = 0; strip < GetNofStrips(side); strip++) {
      if ( fStripCharge[side][strip] > 0. ) {
        RegisterCharge(side, strip, fStripCharge[side][strip],
                       point->GetTime());
        nCharges[side]++;
      } //? charge in strip
    } //# strips

  } //# front and back side

  // Code number of signals
  nSignals = 1000 * nCharges[0] + nCharges[1];

  return nSignals;
}
// -------------------------------------------------------------------------



// -----   Produce charge and propagate it to the readout strips   ---------
void BmnSsdSensorDssd::ProduceCharge(BmnSsdSensorPoint* point) {

  // Total charge created in the sensor: is calculated from the energy loss
  Double_t chargeTotal =
      point->GetELoss() / BmnSsdPhysics::PairCreationEnergy();  // in e

  // For ideal energy loss, just have all charge in the mid-point of the
  // trajectory
  if ( BmnSsdSetup::Instance()->GetDigitizer()->GetELossModel() == 0 ) {
    Double_t xP = 0.5 * ( point->GetX1() + point->GetX2() );
    Double_t yP = 0.5 * ( point->GetY1() + point->GetY2() );
    Double_t zP = 0.5 * ( point->GetZ1() + point->GetZ2() );
    PropagateCharge(xP, yP, zP, chargeTotal, point->GetBy(), 0); // front side (n)
    PropagateCharge(xP, yP, zP, chargeTotal, point->GetBy(), 1); // back side (p)
    return;
  }

  // Kinetic energy
  Double_t mass = BmnSsdPhysics::ParticleMass(point->GetPid());
  Double_t eKin = TMath::Sqrt( point->GetP() * point->GetP() + mass * mass )
  - mass;

  // Length of trajectory inside sensor and its projections
  Double_t trajLx = point->GetX2() - point->GetX1();
  Double_t trajLy = point->GetY2() - point->GetY1();
  Double_t trajLz = point->GetZ2() - point->GetZ1();
  Double_t trajLength = TMath::Sqrt( trajLx*trajLx +
                                     trajLy*trajLy +
                                     trajLz*trajLz );

  // The trajectory is sub-divided into equidistant steps, with a step size
  // close to 3 micrometer.
  Double_t stepSizeTarget = 3.e-4;   // targeted step size is 3 micrometer
  Int_t nSteps = TMath::Nint( trajLength / stepSizeTarget );
  if ( nSteps == 0 ) nSteps = 1;     // assure at least one step
  Double_t stepSize  = trajLength / nSteps;
  Double_t stepSizeX = trajLx / nSteps;
  Double_t stepSizeY = trajLy / nSteps;
  Double_t stepSizeZ = trajLz / nSteps;

  // Average charge per step, used for uniform distribution
  Double_t chargePerStep = chargeTotal / nSteps;
  LOG(DEBUG3) << GetName() << ": Trajectory length " << trajLength
      << " cm, steps " << nSteps << ", step size " << stepSize * 1.e4
      << " mu, charge per step " << chargePerStep;

  // Stopping power, needed for energy loss fluctuations
  Double_t dedx = 0.;
  if ( BmnSsdSetup::Instance()->GetDigitizer()->GetELossModel() == 2 )
    dedx = BmnSsdPhysics::Instance()->StoppingPower(eKin, point->GetPid());

  // Stepping over the trajectory
  Double_t chargeSum = 0.;
  Double_t xStep = point->GetX1() - 0.5 * stepSizeX;
  Double_t yStep = point->GetY1() - 0.5 * stepSizeY;
  Double_t zStep = point->GetZ1() - 0.5 * stepSizeZ;
  for (Int_t iStep = 0; iStep < nSteps; iStep++ ) {
    xStep += stepSizeX;
    yStep += stepSizeY;
    zStep += stepSizeZ;

    // Charge for this step
    Double_t chargeInStep = chargePerStep;  // uniform energy loss
    if ( BmnSsdSetup::Instance()->GetDigitizer()->GetELossModel() == 2 ) // energy loss fluctuations
      chargeInStep = BmnSsdPhysics::Instance()->EnergyLoss(stepSize, mass, eKin, dedx)
      / BmnSsdPhysics::PairCreationEnergy();
    chargeSum += chargeInStep;

    // Propagate charge to strips
    PropagateCharge(xStep, yStep, zStep, chargeInStep,
                    point->GetBy(), 0);  // front
    PropagateCharge(xStep, yStep, zStep, chargeInStep,
                    point->GetBy(), 1);  // back

  } //# steps of the trajectory

  // For fluctuations: normalise to the total charge from GEANT.
  // Since the number of steps is finite (about 100), the average
  // charge per step does not coincide with the expectation value.
  // In order to be consistent with the transport, the charges are
  // re-normalised.
  if ( BmnSsdSetup::Instance()->GetDigitizer()->GetELossModel() == 2) {
    for (Int_t side = 0; side < 2; side++) {  // front and back side
      for (Int_t strip = 0; strip < GetNofStrips(side); strip++)
        fStripCharge[side][strip] *= ( chargeTotal / chargeSum );
    } //# front and back side
  } //? E loss fluctuations

}
// -------------------------------------------------------------------------



// -----   Register charge to the module  ----------------------------------
void BmnSsdSensorDssd::RegisterCharge(Int_t side, Int_t strip,
                                      Double_t charge,
                                      Double_t time) const {

  // --- Check existence of module
  if ( ! GetModule() ) {
    LOG(ERROR) << GetName() << ": No module connected to sensor "
        << GetName() << ", side " << side << ", strip "
        << strip << ", time " << time << ", charge " << charge
       ;
    return;
  }

  // --- Determine module channel for given sensor strip
  Int_t channel = GetModuleChannel(strip, side, GetSensorId() );

  // --- Debug output
  LOG(DEBUG4) << fName << ": Registering charge: side " << side
      << ", strip " << strip << ", time " << time
      << ", charge " << charge
      << " to channel " << channel
      << " of module " << GetModule()->GetName()
     ;

  // --- Get the MC link information
  Int_t index = -1;
  Int_t entry = -1;
  Int_t file  = -1;
  if ( GetCurrentLink() ) {
    index = GetCurrentLink()->GetIndex();
    entry = GetCurrentLink()->GetEntry();
    file  = GetCurrentLink()->GetFile();
  }

  // --- Send signal to module
  GetModule()->AddSignal(channel, time, charge, index, entry, file);

}
// -------------------------------------------------------------------------



// -----   Self test   -----------------------------------------------------
Bool_t BmnSsdSensorDssd::SelfTest() {

  for (Int_t sensorId = 0; sensorId < 3; sensorId++ ) {
    for (Int_t side = 0; side < 2; side ++ ) {
      for (Int_t strip = 0; strip < GetNofStrips(side); strip++ ) {
        Int_t channel = GetModuleChannel(strip, side, sensorId);
        pair<Int_t, Int_t> test = GetStrip(channel, sensorId);
        if ( test.first != strip || test.second != side ) {
          LOG(ERROR) << fName << "Self test failed! Sensor " << sensorId
              << " side " << side << " strip " << strip
              << " gives channel " << channel << " gives strip "
              << test.first << " side " << test.second
             ;
          return kFALSE;
        }
      } // strip loop
    } // side loop
  } // sensor loop

  return kTRUE;
}
// -------------------------------------------------------------------------



ClassImp(BmnSsdSensorDssd)
