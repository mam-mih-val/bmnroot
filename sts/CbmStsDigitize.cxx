//* $Id: */

// -------------------------------------------------------------------------
// -----                    CbmStsDigitize source file             -----
// -----                  Created 08/07/2008  by R. Karabowicz         -----
// -------------------------------------------------------------------------

// Includes from ROOT
#include "TClonesArray.h"
#include "TGeoBBox.h"
#include "TObjArray.h"
#include "TMath.h"
#include "TFile.h"
#include "TF1.h"
#include "TRandom3.h"

#include "TGeoManager.h"
#include "TGeoNode.h"

// Includes from base
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

// Includes from STS
#include "CbmGeoStsPar.h"
#include "CbmStsDigi.h"
#include "CbmStsDigiMatch.h"
#include "CbmStsDigiPar.h"
#include "CbmStsDigiScheme.h"
#include "CbmStsDigitize.h"
#include "CbmStsPoint.h"
#include "CbmStsSensor.h"
#include "CbmStsSector.h"
#include "CbmStsStation.h"
#include "CbmMCTrack.h" //AZ
#include <TDatabasePDG.h>

#include <iostream>
#include <iomanip>
#include <map>
#include <vector>
#include <algorithm>

#include "TCanvas.h"

using std::cerr;
using std::cout;
using std::endl;
using std::fixed;
using std::flush;
using std::ios_base;
using std::left;
using std::map;
using std::pair;
using std::right;
using std::set;
using std::setprecision;
using std::setw;
using std::vector;

#include "TKey.h"


// -----   Default constructor   ------------------------------------------
CbmStsDigitize::CbmStsDigitize()
   : FairTask("STS Digitizer", 1), fGeoPar(NULL), fDigiPar(NULL), fPoints(NULL), fDigis(NULL), fDigiMatches(NULL),
     fRealistic(kFALSE), fDigiScheme(NULL), fNDigis(0), fNMulti(0), fNEvents(0), fNPoints(0), fNDigisFront(0),
     fNDigisBack(0), fStep(0.001), fEnergyLossToSignal(0.), fFThreshold(4.0), fBThreshold(4.0), fFNoiseWidth(0.1),
     fBNoiseWidth(0.1), fStripDeadTime(10), fFNofBits(11), fBNofBits(11), fFNofElPerAdc(10.), fBNofElPerAdc(10.),
     fFNofSteps(0), fBNofSteps(0), fStripSignalF(NULL), fStripSignalB(NULL), fTime(0.), fTimer(), fFChannelPointsMap(),
     fBChannelPointsMap(), fPointMap()
{
   fDigiScheme = new CbmStsDigiScheme();
   Reset();
      collision_points.reserve(10000);
      collPoint.reserve(10000);

}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmStsDigitize::CbmStsDigitize(Int_t iVerbose)
   : FairTask("STSDigitize", iVerbose), fGeoPar(NULL), fDigiPar(NULL), fPoints(NULL), fDigis(NULL), fDigiMatches(NULL),
     fRealistic(kFALSE), fDigiScheme(NULL), fNDigis(0), fNMulti(0), fNEvents(0), fNPoints(0), fNDigisFront(0),
     fNDigisBack(0), fStep(0.001), fEnergyLossToSignal(0.), fFThreshold(4.0), fBThreshold(4.0), fFNoiseWidth(0.1),
     fBNoiseWidth(0.1), fStripDeadTime(10), fFNofBits(11), fBNofBits(11), fFNofElPerAdc(10.), fBNofElPerAdc(10.),
     fFNofSteps(0), fBNofSteps(0), fStripSignalF(NULL), fStripSignalB(NULL), fTime(0.), fTimer(), fFChannelPointsMap(),
     fBChannelPointsMap(), fPointMap()
{
   fDigiScheme = new CbmStsDigiScheme();
   Reset();
      collision_points.reserve(100000);
            collPoint.reserve(10000);


}
// -------------------------------------------------------------------------


// -----   Constructor with name   -----------------------------------------
CbmStsDigitize::CbmStsDigitize(const char *name, Int_t iVerbose)
   : FairTask(name, iVerbose), fGeoPar(NULL), fDigiPar(NULL), fPoints(NULL), fDigis(NULL), fDigiMatches(NULL),
     fRealistic(kFALSE), fDigiScheme(NULL), fNDigis(0), fNMulti(0), fNEvents(0), fNPoints(0), fNDigisFront(0),
     fNDigisBack(0), fStep(0.001), fEnergyLossToSignal(0.), fFThreshold(4.0), fBThreshold(4.0), fFNoiseWidth(0.1),
     fBNoiseWidth(0.1), fStripDeadTime(10), fFNofBits(11), fBNofBits(11), fFNofElPerAdc(10.), fBNofElPerAdc(10.),
     fFNofSteps(0), fBNofSteps(0), fStripSignalF(NULL), fStripSignalB(NULL), fTime(0.), fTimer(), fFChannelPointsMap(),
     fBChannelPointsMap(), fPointMap()
{
  
   fGeoPar      = NULL;
   fDigiPar     = NULL;
   fPoints      = NULL;
   fDigis       = NULL;
   fDigiMatches = NULL;
   fRealistic   = kFALSE;
   fDigiScheme  = new CbmStsDigiScheme();
   Reset();

   fStep = 0.001;

   fFThreshold  = 4.0;
   fBThreshold  = 4.0;
   fFNoiseWidth = 0.1;
   fBNoiseWidth = 0.1;

   fFNofBits      = 11;
   fBNofBits      = 11;
   fFNofElPerAdc  = 10.;
   fBNofElPerAdc  = 10.;
   fStripDeadTime = 10;
   fNEvents       = 0.;

   collision_points.reserve(100000);
         collPoint.reserve(10000);

}

// -----   Destructor   ----------------------------------------------------
CbmStsDigitize::~CbmStsDigitize()
{

   if (fGeoPar) delete fGeoPar;
   if (fDigiPar) delete fDigiPar;
   if (fDigis) {
      fDigis->Delete();
      delete fDigis;
   }
   if (fDigiMatches) {
      fDigiMatches->Delete();
      delete fDigiMatches;
   }
   if (fDigiScheme) delete fDigiScheme;
   Reset();
}
// -------------------------------------------------------------------------
// -----   Public method Exec   --------------------------------------------
void CbmStsDigitize::Exec(Option_t *opt)
{ 
   // Reset all eventwise counters
   fTimer.Start();
   Reset();
   // Verbose screen output
   if (fVerbose > 2) {
      cout << endl << "-I- " << fName << ": executing event" << endl;
      cout << "----------------------------------------------" << endl;
   }

   Int_t nPoints = 0;
   Int_t nDigisF = 0;
   Int_t nDigisB = 0;

   // Loop over all StsPoints
   if (!fPoints) {
      cerr << "-W- " << fName << "::Exec: No input array (STSPoint) " << endl;
      cout << "- " << fName << endl;
      return;
   }
   if (fFNofBits > CbmStsDigi::GetNofAdcBits() || fBNofBits > CbmStsDigi::GetNofAdcBits()) {
      cerr << "-W- " << fName << "::Exec: Number of AdcBits(" << fFNofBits << ") during digitization exceeds ADC range("
           << CbmStsDigi::GetNofAdcBits() << ") defined in data class " << endl;
      cout << "- " << fName << endl;
      return;
   }
   // AZ if (fFNofBits<=CbmStsDigi::GetNofAdcBits()&& fFNofElPerAdc!=10*TMath::Power(2,(12-fFNofBits))) {
   if (0 && fFNofBits <= CbmStsDigi::GetNofAdcBits() && fFNofElPerAdc != 10 * TMath::Power(2, (12 - fFNofBits))) {
      cerr << "-W- " << fName << "::Exec: Number of electrons per AdcChannel does not match Adc range " << endl;
      cout << "- " << fName << endl;
      return;
   }
   // AZ if (fBNofBits<=CbmStsDigi::GetNofAdcBits()&& fBNofElPerAdc!=10*TMath::Power(2,(12-fFNofBits))) {
   if (0 && fBNofBits <= CbmStsDigi::GetNofAdcBits() && fBNofElPerAdc != 10 * TMath::Power(2, (12 - fFNofBits))) {
      cerr << "-W- " << fName << "::Exec: Number of electrons per AdcChannel does not match Adc range " << endl;
      cout << "- " << fName << endl;
      return;
   }
   map<CbmStsSensor *, set<Int_t>>::iterator mapIt;
   for (mapIt = fPointMap.begin(); mapIt != fPointMap.end(); mapIt++) ((*mapIt).second).clear();
  
   for (Int_t iPoint = 0; iPoint < fPoints->GetEntriesFast(); iPoint++) {
      CbmStsPoint *point = (CbmStsPoint *)fPoints->At(iPoint);
   
      Double_t xin = point->GetXIn();
      Double_t yin = point->GetYIn();
      Double_t zin = point->GetZIn();

      gGeoManager->FindNode(xin, yin, zin);
      TGeoNode *curNode = gGeoManager->GetCurrentNode();
      if (TString(curNode->GetName()).Contains("part")) {
         // AZ - Test beam silicon
         // cout << curNode->GetName() << " " << gGeoManager->GetPath() << endl;
         gGeoManager->CdUp();
         curNode = gGeoManager->GetCurrentNode();
         // cout << curNode->GetName() << " " << gGeoManager->GetPath() << endl;
      }

      if(!TString(curNode->GetName()).Contains("Sensor") && !TString(curNode->GetName()).Contains("module") )
      continue; //skip invalid mc-points. i.e. inside cave (air) or frame (lead).

      CbmStsSensor *sensor = NULL;
      if (fDigiScheme->IsNewGeometry()) {
         TString curPath = fDigiScheme->GetCurrentPath();
         // if (!curPath.Contains("Si") && CrossSpacer(curNode, point)) continue; //AZ - emulate spacers
         sensor = fDigiScheme->GetSensorByName(curPath);
      } else
         sensor = fDigiScheme->GetSensorByName(curNode->GetName());

      if (fPointMap.find(sensor) == fPointMap.end()) {
         cerr << "-E- " << fName << "::Exec:: sensor " << curNode->GetName() << " not found in digi scheme!"
              << " PAth: " << gGeoManager->GetPath() << " x,y,z: " << xin << ", " << yin << ", " << zin << endl;
         continue;
      }
      fPointMap[sensor].insert(iPoint);
      nPoints++;
   }

//Int_t i_sigma = (Int_t)(((fBNoiseWidth / 0.0001) * 4000 - 4000));// + fast_rndm->Rndm() * 4000);
//Int_t noisech=0;
   for (Int_t iStation = fDigiScheme->GetNStations(); iStation > 0;) {
      CbmStsStation *station = fDigiScheme->GetStation(--iStation);
      for (Int_t iSector = station->GetNSectors(); iSector > 0;) {
         CbmStsSector *sector = station->GetSector(--iSector);

         map<Int_t, set<Int_t>>::iterator mapCh;

         for (mapCh = fFChannelPointsMap.begin(); mapCh != fFChannelPointsMap.end(); mapCh++) ((*mapCh).second).clear();
         for (mapCh = fBChannelPointsMap.begin(); mapCh != fBChannelPointsMap.end(); mapCh++) ((*mapCh).second).clear();

         // simulating detector+cables+electronics noise
         // should be more sophisticated...
         // the question is: sectorwise or sensorwise???
         Int_t nChannels = sector->GetNChannelsFront();



         //-----aplying noise on every channel-----
         for (Int_t iChannel = nChannels; iChannel > 0;) {
            // 	fStripSignalF[--iChannel] = fGen->Landau(.1,.02);
            // 	fStripSignalB[  iChannel] = fGen->Landau(.1,.02);
            // 	fStripSignalF[--iChannel] = 0.;
            // 	fStripSignalB[  iChannel] = 0.;
             	fStripSignalF[--iChannel] = TMath::Abs(gRandom->Gaus(0.,fFNoiseWidth));
           
            // AZ fStripSignalB[  iChannel] = TMath::Abs(gRandom->Gaus(0.,fBNoiseWidth));
         }
         // AZ
         Int_t nChannelsB = sector->GetNChannelsBack();
         //-----aplying noise on every channel-----
         
         for (Int_t iChannel = nChannelsB; iChannel > 0;) {
                 	fStripSignalB[--iChannel] = TMath::Abs(gRandom->Gaus(0.,fBNoiseWidth));
         }

         for (Int_t iSensor = sector->GetNSensors(); iSensor > 0;) {
            CbmStsSensor *sensor = sector->GetSensor(--iSensor);

            if (sensor->GetDx() < 0.02)
               ProduceHitResponseSi(sensor); 
            else
               ProduceHitResponse(sensor);
         }

         Int_t stationNr   = sector->GetStationNr();
         Int_t sectorNr    = sector->GetSectorNr();
         Int_t sectorDetId = sector->GetDetectorId();

         for (Int_t ifstr = 0; ifstr < nChannels; ifstr++) {
            if (fStripSignalF[ifstr] < (fFThreshold * 1000.)) continue; // threshold cut
       
            //-----random strip inefficiency-----

            Double_t generator;
            generator = gRandom->Rndm() * 100.;
            if (generator < (fStripDeadTime / 100.) * occupancy[iStation][iSector][ifstr / 125]) continue;
            //-----------------------------------
            Int_t digiFSignal = 1 + (Int_t)((fStripSignalF[ifstr]) / fFNofElPerAdc);
            if (digiFSignal >= fFNofSteps) digiFSignal = fFNofSteps - 1;

            new ((*fDigis)[fNDigis]) CbmStsDigi(stationNr, sectorNr, 0, ifstr, digiFSignal, 0);
            set<Int_t>::iterator it1;
            set<Int_t>           chPnt = fFChannelPointsMap[ifstr];
            Int_t                pnt;
            CbmStsDigiMatch *    match;
            if (chPnt.size() == 0) {
               new ((*fDigiMatches)[fNDigis]) CbmStsDigiMatch(-666);
            } else {
               for (it1 = chPnt.begin(); it1 != chPnt.end(); it1++) {
                  pnt = (*it1);
                  if (it1 == chPnt.begin())
                     match = new ((*fDigiMatches)[fNDigis]) CbmStsDigiMatch(pnt);
                  else {
                     match->AddPoint(pnt);
                     fNMulti++;
                  }
               }
            }
            fNDigis++;
            nDigisF++;

         }

         nChannels = sector->GetNChannelsBack(); // AZ

         for (Int_t ibstr = 0; ibstr < nChannels; ibstr++) {
            if (fStripSignalB[ibstr] < (fBThreshold * 1000.)) continue; // threshold cut
     
            //-----random strip inefficiency-----
            Double_t generator;
            generator = gRandom->Rndm() * 100.;
            
            if (generator < (fStripDeadTime / 100.) * occupancy[iStation][iSector][ibstr / 125]) continue;
            //-----------------------------------

            Int_t digiBSignal = 1 + (Int_t)((fStripSignalB[ibstr]) / fBNofElPerAdc);
            if (digiBSignal >= fBNofSteps) digiBSignal = fBNofSteps - 1;

            new ((*fDigis)[fNDigis]) CbmStsDigi(stationNr, sectorNr, 1, ibstr, digiBSignal, 0);
            set<Int_t>::iterator it1;
            set<Int_t>           chPnt = fBChannelPointsMap[ibstr];
            Int_t                pnt;
            CbmStsDigiMatch *    match;
            if (chPnt.size() == 0) {
               new ((*fDigiMatches)[fNDigis]) CbmStsDigiMatch(-666);
            } else {
               for (it1 = chPnt.begin(); it1 != chPnt.end(); it1++) {
                  pnt = (*it1);
                  if (it1 == chPnt.begin())
                     match = new ((*fDigiMatches)[fNDigis]) CbmStsDigiMatch(pnt);
                  else {
                     match->AddPoint(pnt);
                     fNMulti++;
                  }
               }
            }
            fNDigis++;
            nDigisB++;

         }
      }
   }

   fTimer.Stop();
   cout << "+ " << flush;
   cout << setw(15) << left << fName << ": " << setprecision(4) << setw(8) << fixed << right << fTimer.RealTime()
        << " s, digis " << nDigisF << " / " << nDigisB << endl;

   fNEvents += 1.;
   fNPoints += Double_t(nPoints);
   fNDigisFront += Double_t(nDigisF);
   fNDigisBack += Double_t(nDigisB);
   fTime += fTimer.RealTime();
}

// -----   Private method CrossSpacer   ------------------------------------
Bool_t CbmStsDigitize::CrossSpacer(const TGeoNode *node, const CbmStsPoint *point)
{
   // AZ - Check if particle goes thru the spacer (dead space)

   TString name = node->GetName();
   // if (name.Contains("SensorSV")) return kFALSE; // short strip area

   Double_t width = 0.1, pitch = 10.0; // spacer width/2 and pitch
   Double_t xyzloc[3], xyz[3]  = {(point->GetXIn() + point->GetXOut()) / 2, (point->GetYIn() + point->GetYOut()) / 2,
                                 (point->GetZIn() + point->GetZOut()) / 2};
   gGeoManager->MasterToLocal(xyz, xyzloc);

   TGeoVolume *vol = node->GetVolume();
   TGeoBBox *  box = (TGeoBBox *)vol->GetShape();
   Double_t    dx = box->GetDX(), dy = box->GetDY();
   gGeoManager->CdUp();
   TGeoNode *      nodeM  = gGeoManager->GetCurrentNode();
   const Double_t *transl = nodeM->GetMatrix()->GetTranslation();

   // X-spacers: check distance from the beam pipe corner
   Double_t dist = 0;
   if (transl[0] > -1)
      dist = xyzloc[0] + dx;
   else
      dist = dx - xyzloc[0];
   Double_t dscaled = dist / pitch;
   Int_t    nspacer = TMath::Nint(dscaled);
   if (nspacer == 0) return kFALSE; // at the border
   if (TMath::Abs(dist - nspacer * pitch) < width) return kTRUE;

   // Y-spacers: check distance from the beam pipe corner
   if (transl[1] > 0)
      dist = xyzloc[1] + dy;
   else
      dist = dy - xyzloc[1];
   dscaled = dist / pitch;
   nspacer = TMath::Nint(dscaled);
   if (nspacer == 0) return kFALSE;                              // at the border
   if (TMath::Abs(dist - nspacer * pitch) < width) return kTRUE; // Y-spacer

   return kFALSE;
}
// -------------------------------------------------------------------------

// -----   Private method ProduceHitResponse   --------------------------------
void CbmStsDigitize::ProduceHitResponseSi(CbmStsSensor *sensor)
{
   // Produce  response in Silicon

   set<Int_t> pSet;
   if (fPointMap.find(sensor) == fPointMap.end()) {
      cerr << "-E- " << fName << "::ProduceHitResponse:: sensor"
           << " not found in digi scheme!" << endl;
      return;
   }
   pSet = fPointMap[sensor];

   Int_t        iPoint = -1;
   CbmStsPoint *point  = NULL;

   set<Int_t>::iterator it1;

   Double_t dPitch, step = fStep;                                                           // AZ
   if (TString(sensor->GetName()).Contains("Si") || sensor->GetDx() < 0.0200) step = 0.001; // AZ - silicon

   for (it1 = pSet.begin(); it1 != pSet.end(); it1++) {
      iPoint = (*it1);
      point  = (CbmStsPoint *)fPoints->At(iPoint);

      Double_t xin = point->GetXIn();
      Double_t yin = point->GetYIn();
      Double_t zin = point->GetZIn();

      Double_t xvec = point->GetXOut() - xin;
      Double_t yvec = point->GetYOut() - yin;
      Double_t zvec = point->GetZOut() - zin;

      Int_t nofSteps = (Int_t)(TMath::Sqrt(xvec * xvec + yvec * yvec + zvec * zvec) / step + 1);

      Double_t stepEL = 0.0; // AZ
      if (sensor->GetDx() < 0.0200)
         stepEL = 280000000 * 1.5 * point->GetEnergyLoss() / (nofSteps + 1); // AZ - Si
      else
         stepEL = fEnergyLossToSignal * point->GetEnergyLoss() / (nofSteps + 1); // AZ

      xvec = xvec / ((Double_t)nofSteps);
      yvec = yvec / ((Double_t)nofSteps);
      zvec = zvec / ((Double_t)nofSteps);

      Int_t interval = 0;

  Double_t xinS = xin, yinS = yin, sigma = 0.01; // 100um
//Int_t i_sigma = (Int_t)(((sigma / 0.0001) * 4000 - 4000));// + fast_rndm->Rndm() * 4000);

      for (Int_t istep = 0; istep <= nofSteps; istep++) {

         // AZ - introduce smearing
       
         if (step < 0.0009) {
            // Smearing only for GEMs
             xinS += gRandom->Gaus(0.0,sigma);
             yinS += gRandom->Gaus(0.0,sigma);

         }
         // AZ

         // AZ Int_t   iIChan = sensor->GetFrontChannel(xin,yin,zin);
         Int_t iIChan = sensor->GetFrontChannel(xinS, yinS, zin, dPitch);

         // AZ!!! Exclude 50% of channels for large stations
         // if (sensor->GetNChannelsFront() > 900 && sensor->GetSectorNr() == 1 && iIChan > 500) iIChan = -1;
         // else if (sensor->GetNChannelsFront() > 900 && sensor->GetSectorNr() == 3 && iIChan < 500) iIChan = -1;

         if (iIChan != -1) {
            fStripSignalF[iIChan] += stepEL;
            fFChannelPointsMap[iIChan].insert(iPoint);
         }

         // AZ iIChan = sensor->GetBackChannel (xin,yin,zin);
         iIChan = sensor->GetBackChannel(xinS, yinS, zin, dPitch);

         // AZ!!! Exclude 50% of channels for large stations
         // if (sensor->GetNChannelsFront() > 900 && sensor->GetSectorNr() == 1 && iIChan > 600) iIChan = -1;
         // else if (sensor->GetNChannelsFront() > 900 && sensor->GetSectorNr() == 3 && iIChan < 600) iIChan = -1;

         if (iIChan != -1) {
            fStripSignalB[iIChan] += stepEL;
            fBChannelPointsMap[iIChan].insert(iPoint);
         }

         xin += xvec;
         yin += yvec;
         zin += zvec;
      }
   }
}
// -------------------------------------------------------------------------

//----------------------------new bete blockh ionizing------------------------
Double_t CbmStsDigitize::GetNumberOfClusters(Double_t beta, Double_t gamma, Double_t p0, Double_t p1)
{ // ES
   Double_t beta2  = beta * beta;
   Double_t gamma2 = gamma * gamma;
   Double_t val    = p0 / beta2 * (p1 + TMath::Log(beta2 * gamma2) - beta2);
   return val;
}

// -----   Private method ProduceHitResponse   --------------------------------
void CbmStsDigitize::ProduceHitResponse(CbmStsSensor *sensor)
{
   set<Int_t> pSet;

   if (fPointMap.find(sensor) == fPointMap.end()) {
      cerr << "-E- " << fName << "::ProduceHitResponse:: sensor"
           << " not found in digi scheme!" << endl;
      return;
   }


   pSet = fPointMap[sensor];

   Int_t        iPoint = -1;
   CbmStsPoint *point  = NULL;

   set<Int_t>::iterator it1;

   Double_t dPitch = fStep; // AZ

   Double_t x_shift    = 0;
   Int_t    x_meanColl = 0;

   for (it1 = pSet.begin(); it1 != pSet.end(); it1++) {

      // AZ Apply overall efficiency
      // const Double_t eff0 = 0.65;
      // if (gRandom->Rndm() > eff0) continue;

      iPoint = (*it1);
      // AZ point  = (CbmStsPoint*)fPoints->At(iPoint);
      point = (CbmStsPoint *)fPoints->UncheckedAt(iPoint); // AZ
      TVector3 mom;
      point->Momentum(mom);

      CbmMCTrack *  mcTr = (CbmMCTrack *)fMCTracks->UncheckedAt(point->GetTrackID()); // ES
      TParticlePDG *part = TDatabasePDG::Instance()->GetParticle(mcTr->GetPdgCode());
      Double_t      mass = 3.7283999; // He4
      if (part) mass = part->Mass();
      TLorentzVector lorv;
      lorv.SetVectM(mom, mass);
      Double_t beta  = lorv.Beta();
      Double_t gamma = lorv.Gamma();

      //memory leak cut
      if(beta>=1 || beta<=0.005 ) continue; //GP

      Double_t mnoc  = GetNumberOfClusters(beta, gamma, 1.787, 12.33); // ES

      Double_t moduleThickness = 
         0.3 + 0.25 + 0.2 +
         0.15; // DriftGapThickness + FirstTransferGapThickness + SecondTransferGapThickness + InductionGapThickness;
      Int_t stationNr = sensor->GetStationNr();


      Double_t xin = point->GetXIn();
      Double_t yin = point->GetYIn();
      Double_t zin = point->GetZIn();



      Double_t xvec = point->GetXOut() - xin;
      Double_t yvec = point->GetYOut() - yin;
      Double_t zvec;

      if (point->GetPz() > 0.0)
         zvec = point->GetZOut() - zin;
      else
         zvec = zin - point->GetZOut();

      // length of track
      Double_t track_length = TMath::Sqrt(xvec * xvec + yvec * yvec + zvec * zvec);

      // Energy loss step for cluster, for electron
      // Double_t stepEL, stepEl_current;

      // Exponential dependency on a distance between clusters
      Double_t current_length       = 0.0; // traversed length (counter)
      Double_t current_length_ratio = 0.0; // ratio of the traversed length to common track length (counter)

      Int_t    collisions_cnt = 0;   // total collision counter
      Double_t current_step   = 0.0; // current distance between two collision points

      // Collection of collision points
    //  std::vector<std::vector<Double_t>> collision_points;
    collision_points.clear();
      ////mean collision distance (mean free flight path) [cm]
      Double_t mcd = 1 / mnoc; // ES

      Int_t iladn_ch = 0, i_mdc_ch = 0, i_exp = 0, i_g = 0;
      while (current_length < track_length) {

         current_step = gRandom->Exp(mcd); 

         current_length += current_step;

         if (current_length > track_length) break;

         current_length_ratio = current_length / track_length;

         // In terms of distance
         Double_t current_x = xin + current_length_ratio * xvec;
         Double_t current_y = yin + current_length_ratio * yvec;
         Double_t current_z = zin + current_length_ratio * zvec;

  /*       std::vector<Double_t> collPoint;
         collPoint.push_back(current_x);
         collPoint.push_back(current_y);
         collPoint.push_back(current_z);
         collision_points.push_back(collPoint);
*/
         collPoint.clear();
         collPoint.emplace_back(current_x);
         collPoint.emplace_back(current_y);
         collPoint.emplace_back(current_z);
         collision_points.emplace_back(collPoint);

         collisions_cnt++;
      }

      // Each level - distance to the readout plane
      const Double_t level1 = 0.15;       // InductionGapThickness;
      const Double_t level2 = 0.2 + 0.15; // InductionGapThickness+SecondTransferGapThickness;
      const Double_t level3 =0.2 + 0.15 + 0.25;
       // InductionGapThickness+SecondTransferGapThickness+FirstTransferGapThickness;

      // Mean electron shift along x-axis (under the influence of the Lorentz force)
      Double_t       xmean; // the dependence fitted by polynomial: f(x) = (p0 + p1*x + p2*x^2 + p3*x^3)
      const Double_t p0_xmean = +0.000227984; //+0.000118365;
      const Double_t p1_xmean = +0.0614758;   //+0.0551321;
      const Double_t p2_xmean = +0.157119;    //+0.110804;
      const Double_t p3_xmean = -0.0799265;   //-0.0530758;

      // Sigma electron smearing
      Double_t sigma; // depends on the distance from current z-position to the readout plane

      for (Int_t icoll = 0; icoll < collision_points.size(); ++icoll) {

         Double_t xcoll = collision_points[icoll][0]; // x
         Double_t ycoll = collision_points[icoll][1]; // y
         Double_t zcoll = collision_points[icoll][2]; // z

         Double_t zdist; // current z-distance to the readout

         // Find z-distance to the readout depending on the electron drift direction
         // Consider that electron drift direction equals to forward Z axis drift

        //reverse order
        if(stationNr!=5 && stationNr!=6 && stationNr!=8)
            zdist = moduleThickness - (zcoll - zin);
         else
            zdist = zcoll - zin;

         Double_t zdist2 = zdist * zdist;

         xmean = p0_xmean + p1_xmean * zdist + p2_xmean * zdist2 + p3_xmean * zdist2 * zdist;

         // Condition: beacause we have piecewise fitting function (different polynomials on each gap)
         if (zdist < 0.1) {
            sigma = std::sqrt(0.000663416 * zdist);
         }
         if (zdist >= 0.1 && zdist < 0.3) {
            sigma = 0.003897 + 0.045375 * zdist + (-0.03355) * zdist2;
         }
         if (zdist >= 0.3 && zdist < 0.6) {
            sigma = 0.004095 + 0.0424 * zdist + (-0.026) * zdist2;
         }
         if (zdist >= 0.6) {
            sigma = 0.0118343 + 0.0200945 * zdist + (-0.010325) * zdist2;
         }

         if (std::isnan(sigma)) continue;//validation
           
         Int_t n_electrons_cluster =gRandom->Landau(1.027, 0.11);

         if (n_electrons_cluster < 1) n_electrons_cluster = 1; // min
         if (n_electrons_cluster > 6) n_electrons_cluster = 6; // max

         for (Int_t ielectron = 0; ielectron < n_electrons_cluster; ++ielectron) {

            // Electron gain in each GEM cascade
            // Polya distribution is better, but Exponential is good too in our case
         //...->Exp(V), where V is the mean value of the exponential distribution
          
           Double_t gain_gem1 =gRandom->Exp(15); 
            Double_t gain_gem2 = gRandom->Exp(15); 
            Double_t gain_gem3 = gRandom->Exp(15);

            int total_gain = 0;

            if (zdist < level1) {
               total_gain = 1.0;
            } else if (zdist >= level1 && zdist < level2) {
               if (gain_gem3 < 1.0) gain_gem3 = 1.0;
               total_gain = gain_gem3;
            } else if (zdist >= level2 && zdist < level3) {
               if (gain_gem3 < 1.0) gain_gem3 = 1.0;
               if (gain_gem2 < 1.0) gain_gem2 = 1.0;
               total_gain = gain_gem3 * gain_gem2;
            } else if (zdist >= level3) {
               if (gain_gem3 < 1.0) gain_gem3 = 1.0;
               if (gain_gem2 < 1.0) gain_gem2 = 1.0;
               if (gain_gem1 < 1.0) gain_gem1 = 1.0;
               total_gain = gain_gem3 * gain_gem2 * gain_gem1;
            } 
       
            // Projection of the current electron on the readout (x,y-coordinates)
            double x_readout, y_readout;

            Int_t interval = 0;
            for (int igain = 0; igain < total_gain; ++igain) {

               // x-shift of the electon depends on the electron drift direction
               // Consider that electron drift direction equals to forward Z axis drift

               //reverse order
               /* if(stationNr!=5 && stationNr!=6 && stationNr!=8) //should uncomment for field
                  x_readout = gRandom->Gaus(xcoll-xmean, sigma);
                    else x_readout = gRandom->Gaus(xcoll+xmean, sigma);
                */
                x_readout = gRandom->Gaus(xcoll, sigma);

                    y_readout = gRandom->Gaus(ycoll, sigma);

               Int_t iIChan = sensor->GetFrontChannel(x_readout, y_readout, zcoll, dPitch);

               // AZ!!! Exclude 50% of channels for large stations
               // if (sensor->GetNChannelsFront() > 900 && sensor->GetSectorNr() == 1 && iIChan > 500) iIChan = -1;
               // else if (sensor->GetNChannelsFront() > 900 && sensor->GetSectorNr() == 3 && iIChan < 500) iIChan = -1;

               if (iIChan != -1) {
                  fStripSignalF[iIChan] += 1.0; // stepEl_current;
                  fFChannelPointsMap[iIChan].insert(iPoint);
               }

               // AZ iIChan = sensor->GetBackChannel (xin,yin,zin);
               iIChan = sensor->GetBackChannel(x_readout, y_readout, zcoll, dPitch);

               // AZ!!! Exclude 50% of channels for large stations
               // if (sensor->GetNChannelsFront() > 900 && sensor->GetSectorNr() == 1 && iIChan > 600) iIChan = -1;
               // else if (sensor->GetNChannelsFront() > 900 && sensor->GetSectorNr() == 3 && iIChan < 600) iIChan = -1;

               if (iIChan != -1) {
                  fStripSignalB[iIChan] += 1.0; // stepEl_current;
                  fBChannelPointsMap[iIChan].insert(iPoint);
               }
            }
         } 
      }
   }
}
//----------------------------new bete blockh ionizing------------------------

// -----   Private method SetParContainers   -------------------------------
void CbmStsDigitize::SetParContainers()
{

   // Get run and runtime database
   FairRunAna *run = FairRunAna::Instance();
   if (!run) Fatal("SetParContainers", "No analysis run");

   FairRuntimeDb *db = run->GetRuntimeDb();
   if (!db) Fatal("SetParContainers", "No runtime database");

   // Get STS geometry parameter container
   fGeoPar = (CbmGeoStsPar *)db->getContainer("CbmGeoStsPar");

   // Get STS digitisation parameter container
   fDigiPar = (CbmStsDigiPar *)db->getContainer("CbmStsDigiPar");
}
// -------------------------------------------------------------------------

// -----   Private method Init   -------------------------------------------
InitStatus CbmStsDigitize::Init()
{

   // Get input array
   FairRootManager *ioman = FairRootManager::Instance();
   if (!ioman) Fatal("Init", "No FairRootManager");
   fPoints   = (TClonesArray *)ioman->GetObject("StsPoint");
   fMCTracks = (TClonesArray *)ioman->GetObject("MCTrack"); // AZ

   // Register output array StsDigi
   fDigis = new TClonesArray("CbmStsDigi", 1000);
   ioman->Register("StsDigi", "Digital response in STS", fDigis, kTRUE);

   // Register output array StsDigiMatches
   fDigiMatches = new TClonesArray("CbmStsDigiMatch", 1000);
   ioman->Register("StsDigiMatch", "Digi Match in STS", fDigiMatches, kTRUE);


   fStripSignalF = new Double_t[2000];
   fStripSignalB = new Double_t[2000];

   // AZ fEnergyLossToSignal    = 280000000.;

   fFNofSteps = (Int_t)TMath::Power(2, (Double_t)fFNofBits);
   fBNofSteps = (Int_t)TMath::Power(2, (Double_t)fBNofBits);
 
   // Build digitisation scheme
   if (fDigiScheme->Init(NULL, fDigiPar)) {
      MakeSets();

      if (fVerbose == 1 || fVerbose == 2)
         fDigiScheme->Print(kFALSE);
      else if (fVerbose > 2)
         fDigiScheme->Print(kTRUE);
      cout << "-I- " << fName << "::Init: "
           << "STS digitisation scheme succesfully initialised" << endl;
      if (fDigiScheme->IsNewGeometry()) cout << "-I- Using new geometry" << endl;
      cout << "    Stations: " << fDigiScheme->GetNStations() << ", Sectors: " << fDigiScheme->GetNSectors()
           << ", Channels: " << fDigiScheme->GetNChannels() << endl;
      return kSUCCESS;
   }

   return kERROR;
}
// -------------------------------------------------------------------------

// -----   Private method ReInit   -----------------------------------------
InitStatus CbmStsDigitize::ReInit()
{

   // Clear digitisation scheme
   fDigiScheme->Clear();

   // Build new digitisation scheme
   if (fDigiScheme->Init(fGeoPar, fDigiPar)) {
      MakeSets();
      return kSUCCESS;
   }

   return kERROR;
}
// -------------------------------------------------------------------------

// -----   Private method MakeSets   ---------------------------------------
void CbmStsDigitize::MakeSets()
{

   fPointMap.clear();
   Int_t nStations = fDigiScheme->GetNStations();

   Double_t fSectorWidth = 0.;

   for (Int_t iStation = 0; iStation < nStations; iStation++) {
      CbmStsStation *station  = fDigiScheme->GetStation(iStation);
      Int_t          nSectors = station->GetNSectors();
      for (Int_t iSector = 0; iSector < nSectors; iSector++) {
         CbmStsSector *sector   = station->GetSector(iSector);
         Int_t         nSensors = sector->GetNSensors();
         for (Int_t iSensor = 0; iSensor < nSensors; iSensor++) {
            CbmStsSensor *sensor = sector->GetSensor(iSensor);
            set<Int_t>    a;
            fPointMap[sensor] = a;
            fSectorWidth      = 10. * sensor->GetLx();

            Int_t nofChips =
               (Int_t)(TMath::Ceil(fSectorWidth / 7.5)); // fwidth in mm, 7.5mm = 125(channels)*60mum(pitch)
            Int_t lastChip = (Int_t)(TMath::Ceil(10. * fSectorWidth));
            lastChip       = lastChip % 75;
            lastChip       = (Int_t)(lastChip / .6);
            //     cout << nofChips << " chips on " << iStation+1 << " " << iSector+1 << endl;
            TString addInfo = "";
            if (nofChips != 8) {
               addInfo = Form(", only %d strips", lastChip);
               //	cout << fSectorWidth << " -> " << addInfo.Data() << endl;
            }

            for (Int_t iChip = 0; iChip < nofChips; iChip++) {
               occupancy[iStation][iSector][iChip] = 3.;
               //           cout << "OCCUPANCY [" << iStation+1 << "][" << iSector+1 << "][" << iChip << "] "<<
               //           occupancy [iStation][iSector][iChip] << "%" << endl;
            }
         }
      }
   }
   fFChannelPointsMap.clear();
   fBChannelPointsMap.clear();
   for (Int_t ichan = 2000; ichan > 0;) {
      set<Int_t> a;
      fFChannelPointsMap[--ichan] = a;
      set<Int_t> b;
      fBChannelPointsMap[ichan] = b;
   }
}
// -------------------------------------------------------------------------
void CbmStsDigitize::MakeSets1()
{ // with occupancy file - default not used

   fPointMap.clear();
   Int_t nStations = fDigiScheme->GetNStations();

   TH1F *  fhFNofDigisPChip[10][1000][20];
   TH1F *  fhBNofDigisPChip[10][1000][20];
   TString qaFileName;
   qaFileName = "occup.sts.reco.root";
   cout << "Occupancy read from file: \"" << qaFileName.Data() << "\"" << endl;
   TFile *occuF = TFile::Open(qaFileName.Data());

   TString directoryName = "STSFindHitsQA";

   Double_t fSectorWidth = 0.;

   for (Int_t iStation = 0; iStation < nStations; iStation++) {
      CbmStsStation *station  = fDigiScheme->GetStation(iStation);
      Int_t          nSectors = station->GetNSectors();

      for (Int_t iSector = 0; iSector < nSectors; iSector++) {
         CbmStsSector *sector    = station->GetSector(iSector);
         Int_t         nSensors  = sector->GetNSensors();
         Int_t         nChannels = sector->GetNChannelsFront();

         for (Int_t iSensor = 0; iSensor < nSensors; iSensor++) {
            CbmStsSensor *sensor = sector->GetSensor(iSensor);
            set<Int_t>    a;
            fPointMap[sensor] = a;
            fSectorWidth      = 10. * sensor->GetLx();

            Int_t nofChips =
               (Int_t)(TMath::Ceil(fSectorWidth / 7.5)); // fwidth in mm, 7.5mm = 125(channels)*60mum(pitch)
            Int_t lastChip = (Int_t)(TMath::Ceil(10. * fSectorWidth));
            lastChip       = lastChip % 75;
            lastChip       = (Int_t)(lastChip / .6);
            //     cout << nofChips << " chips on " << iStation+1 << " " << iSector+1 << endl;
            TString addInfo = "";
            if (nofChips != 8) {
               addInfo = Form(", only %d strips", lastChip);
               //	cout << fSectorWidth << " -> " << addInfo.Data() << endl;
            }

            for (Int_t iChip = 0; iChip < nofChips; iChip++) {
               fhFNofDigisPChip[iStation][iSector][iChip] =
                  (TH1F *)occuF->Get(Form("%s/Station%d/hNofFiredDigisFSt%dSect%dChip%d", directoryName.Data(),
                                          iStation + 1, iStation + 1, iSector + 1, iChip + 1));
               fhBNofDigisPChip[iStation][iSector][iChip] =
                  (TH1F *)occuF->Get(Form("%s/Station%d/hNofFiredDigisBSt%dSect%dChip%d", directoryName.Data(),
                                          iStation + 1, iStation + 1, iSector + 1, iChip + 1));
               occupancy[iStation][iSector][iChip] =
                  100. * fhFNofDigisPChip[iStation][iSector][iChip]->GetMean() / 125.;
               occupancy[iStation][iSector][iChip] =
                  100. * fhBNofDigisPChip[iStation][iSector][iChip]->GetMean() / 125.;
               // 	    if ( !occuF ) {
               // 	      occupancy [iStation][iSector][iChip] = 3.;
               // 	    }
               //           cout << "OCCUPANCY [" << iStation+1 << "][" << iSector+1 << "][" << iChip << "] "<<
               //           occupancy [iStation][iSector][iChip] << "%" << endl;
            }
         }
      }
   }
   fFChannelPointsMap.clear();
   fBChannelPointsMap.clear();
   for (Int_t ichan = 2000; ichan > 0;) {
      set<Int_t> a;
      fFChannelPointsMap[--ichan] = a;
      set<Int_t> b;
      fBChannelPointsMap[ichan] = b;
   }
}
// -------------------------------------------------------------------------

// -----   Private method Reset   ------------------------------------------
void CbmStsDigitize::Reset()
{
   //  fNPoints = fNDigisFront = fNDigisBack = fTime = 0.;
   fNDigis = fNMulti = 0;
   fFChannelPointsMap.clear();
   fBChannelPointsMap.clear();
   //  if ( fDigis ) fDigis->Clear();
   //  if ( fDigiMatches ) fDigiMatches->Clear();
   if (fDigis) fDigis->Delete();
   if (fDigiMatches) fDigiMatches->Delete();
}
// -------------------------------------------------------------------------

// -----   Virtual method Finish   -----------------------------------------
void CbmStsDigitize::Finish()
{
   cout << endl;
   cout << "============================================================" << endl;
   cout << "===== " << fName << ": Run summary " << endl;
   cout << "===== " << endl;
   cout << "===== Events processed          : " << setw(8) << fNEvents << endl;
   cout.setf(ios_base::fixed, ios_base::floatfield);
   cout << "===== Real time per event       : " << setw(8) << setprecision(4) << fTime / fNEvents << " s" << endl;
   cout << "===== StsPoints per event       : " << setw(8) << setprecision(2) << fNPoints / fNEvents << endl;
   cout << "===== StsDigis per event        : " << setw(8) << setprecision(2) << (fNDigisFront + fNDigisBack) / fNEvents
        << endl;
   cout << "===== Front digis per point     : " << setw(8) << setprecision(2) << fNDigisFront / (fNPoints) << endl;
   cout << "===== Back digis per point      : " << setw(8) << setprecision(2) << fNDigisBack / (fNPoints) << endl;
   cout << "============================================================" << endl;
   
}
// -------------------------------------------------------------------------
Int_t CbmStsDigitize::GetNofModules(TGeoNode *station)
{

   Int_t nModules = 0; // station->GetNdaughters();

   // --- Modules

   //*
   for (Int_t iModule = 0; iModule < station->GetNdaughters(); iModule++) {
      TGeoNode *module = station->GetDaughter(iModule);
      if (TString(module->GetName()).Contains("module")) nModules++;
   }
   //*/

   return nModules;
}

ClassImp(CbmStsDigitize)
