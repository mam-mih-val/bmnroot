// -------------------------------------------------------------------------
// -----                     CbmStsDigiPar source file                 -----
// -----                  Created 28/06/05  by V. Friese               -----
// -------------------------------------------------------------------------

#include "CbmStsDigiPar.h"

#include "CbmStsStationDigiPar.h"

#include "FairDetParIo.h"
#include "FairParIo.h"

#include "TString.h"
#include "TMath.h"

#include <iostream>


// -----   Standard constructor   ------------------------------------------
CbmStsDigiPar::CbmStsDigiPar(const char* name, const char* title,
			     const char* context)
  : FairParSet(name, title, context),
  fStations(NULL),
  fStationMap()
{
  fStations = new TObjArray();
  detName="STS";
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmStsDigiPar::~CbmStsDigiPar() {
  clear();
}
// -------------------------------------------------------------------------



// -----   Public method init   --------------------------------------------
Bool_t CbmStsDigiPar::init(FairParIo* parIo) {
  if ( ! parIo ) {
    cout << "-W- CbmStsDigiPar::init "
	 << "No input given; could not initialise parameters." << endl;
    return kFALSE;
  }
  FairDetParIo* input = parIo->getDetParIo("CbmStsParIo");
  if (input) return (input->init(this));
  return kFALSE;
}
// -------------------------------------------------------------------------



// -----   Public method write   -------------------------------------------
Int_t CbmStsDigiPar::write(FairParIo* parIo) {
  if ( ! parIo ) {
    cout << "-W- CbmStsDigiPar::write "
	 << "No output given; could not write parameters." << endl;
    return kFALSE;;
  }
  FairDetParIo* output = parIo->getDetParIo("CbmStsParIo");
  if (output) return (output->write(this));
  return kFALSE;
}
// -------------------------------------------------------------------------



// -----   Public method clear   -------------------------------------------
void CbmStsDigiPar::clear() {
  status = kFALSE;
  resetInputVersions();
  fStations->Delete();
  fStationMap.clear();
}
// -------------------------------------------------------------------------



// -----   Public method readline   ----------------------------------------
void CbmStsDigiPar::readline(const char* buffer, Int_t* set, fstream* f) {

  // First line: Station ID and type
  Int_t    stationNr   = -1;
  Float_t  frotation   =  0.;
  Int_t    nSectors    =  0;
  Int_t    nSensors    =  0;
  sscanf(buffer, "%d%f%d", &stationNr, &frotation, &nSectors);
  if ( stationNr == -1 ) return;
  Double_t rotation = Double_t(frotation);

  // Create new station
  CbmStsStationDigiPar* station = new CbmStsStationDigiPar(stationNr,
							   rotation);

  // Read the sector lines
  Int_t    sectorNr = -1;
  Int_t    sensorNr = -1;
  Int_t    iType    = 0;
  Double_t x0       = 0.;
  Double_t y0       = 0.;
  Double_t z0       = 0.;
  Double_t angle    = 0.;
  Double_t lx       = 0.;
  Double_t ly       = 0.;
  Double_t lz       = 0.;
  Double_t dx       = 0.;
  Double_t dy       = 0.;
  Double_t stereoF  = 0.;
  Double_t stereoB  = 0.;
  for (Int_t iSector=0; iSector<nSectors; iSector++) {
    *f >> sectorNr >> nSensors; 

    CbmStsSectorDigiPar* sector = new CbmStsSectorDigiPar(sectorNr);

    for (Int_t iSensor=0; iSensor<nSensors; iSensor++) {

      *f >> sensorNr >> iType >> x0 >> y0 >> z0 >> angle >> lx >> ly >> lz
	 >> dx >> dy  >> stereoF >> stereoB; 
      
      angle  = angle  * TMath::Pi() / 180.;
      stereoB = stereoB * TMath::Pi() / 180.;
      stereoF = stereoF * TMath::Pi() / 180.;
      
      CbmStsSensorDigiPar* sensor = new CbmStsSensorDigiPar(sensorNr, iType,
							    x0, y0, z0, angle,
							    lx, ly, lz, dx, dy,
							    stereoF, stereoB);
      sector->AddSensor(sensor);
    }
    station->AddSector(sector);
  }   // Loop over sector lines
  fStations->Add(station);

}
// -------------------------------------------------------------------------
/*void CbmStsDigiPar::readline(const char* buffer, Int_t* set, fstream* f) {

  // First line: Station ID and type
  Int_t    stationNr   = -1;
  Float_t  frotation   =  0.;
  Int_t    nSectors    =  0;
  sscanf(buffer, "%d%f%d", &stationNr, &frotation, &nSectors);
  if ( stationNr == -1 ) return;
  Double_t rotation = Double_t(frotation);

  // Create new station
  CbmStsStationDigiPar* station = new CbmStsStationDigiPar(stationNr,
							   rotation);

  // Read the sector lines
  Int_t    sectorNr = -1;
  Int_t    iType    = 0;
  Double_t x0       = 0.;
  Double_t y0       = 0.;
  Double_t angle    = 0.;
  Double_t lx       = 0.;
  Double_t ly       = 0.;
  Double_t dx       = 0.;
  Double_t dy       = 0.;
  Double_t stereo   = 0.;
  for (Int_t iSector=0; iSector<nSectors; iSector++) {
    *f >> sectorNr >> iType >> x0 >> y0 >> angle >> lx >> ly 
	  >> dx >> dy  >> stereo; 
    angle  = angle  * TMath::Pi() / 180.;
    stereo = stereo * TMath::Pi() / 180.;
    CbmStsSectorDigiPar* sector = new CbmStsSectorDigiPar(sectorNr, iType,
							  x0, y0, angle,
							  lx, ly, dx, dy,
							  stereo);
    station->AddSector(sector);
  }   // Loop over sector lines
  fStations->Add(station);

}
*/// -------------------------------------------------------------------------



// -----   Public method putAsciiHeader   ----------------------------------
void CbmStsDigiPar::putAsciiHeader(TString& header) {
  header = "Header is not yet defined";
}
// -------------------------------------------------------------------------



// -----   Public method GetStation   --------------------------------------
CbmStsStationDigiPar* CbmStsDigiPar::GetStation(Int_t iStation) {
  return (CbmStsStationDigiPar*) fStations->At(iStation);
}
// -------------------------------------------------------------------------



// -------   Public method GetStationById   --------------------------------
CbmStsStationDigiPar* CbmStsDigiPar::GetStationByNr(Int_t stationNr) {
  Int_t index = fStationMap[stationNr];
  CbmStsStationDigiPar* station = 
    (CbmStsStationDigiPar*) fStations->At(index);
  if ( ! station ) {
    cout << "-W- CbmStsDigiPar::GetStationDigiPar: "
	 << "No parameters found for station " << stationNr << endl;
    return NULL;
  }
  return station;
}
// -------------------------------------------------------------------------





ClassImp(CbmStsDigiPar)
