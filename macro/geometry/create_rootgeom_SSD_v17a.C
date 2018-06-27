/******************************************************************************
 ** Creation of STS geometry in ROOT format (TGeo).
 **
 ** @file create_stsgeo_v17a.C
 ** @author Volker Friese <v.friese@gsi.de>
 ** @since 15 June 2012
 ** @date 09.05.2014
 ** @author Tomas Balog <T.Balog@gsi.de>
 **
 ** v16c: like v16b, but senors of ladders beampipe next to beampipe
 **       shifted closer to the pipe, like in the CAD model
 ** v16b: like v16a, but yellow sensors removed
 ** v16a: derived from v15c (no cones), but with sensor types renamed:
 ** 2 -> 1, 3 -> 2, 4 -> 3, 5 -> 4, 1 -> 5
 **
 ** v15c: as v15b without cones
 ** v15b: introduce modified carbon ladders from v13z
 ** v15a: with flipped ladder orientation for stations 0,2,4,6 to match CAD design
 **
 ** TODO:
 **
 ** DONE:
 ** v15b - use carbon macaroni as ladder support
 ** v15b - introduce a small gap between lowest sensor and carbon ladder
 ** v15b - build small cones for the first 2 stations
 ** v15b - within a station the ladders of adjacent units should not touch eachother - set gkLadderGapZ to 10 mm
 ** v15b - for all ladders set an even number of ladder elements
 ** v15b - z offset of cones to ladders should not be 0.3 by default, but 0.26
 ** v15b - within a station the ladders should be aligned in z, defined either by the unit or the ladder with most sensors
 ** v15b - get rid of cone overlap in stations 7 and 8 - done by adapting rHole size
 **
 ** The geometry hierarachy is:
 **
 ** 1. Sensors  (see function CreateSensors)
 **    The sensors are the active volumes and the lowest geometry level.
 **    They are built as TGeoVolumes, shape box, material silicon.
 **    x size is determined by strip pitch 58 mu and 1024 strips
 **    plus guard ring of 1.3 mm at each border -> 6.1992 cm.
 **    Sensor type 1 is half of that (3.0792 cm).
 **    y size is determined by strip length (2.2 / 4.2 / 6.3 cm) plus
 **    guard ring of 1.3 mm at top and bottom -> 2.46 / 4.46 / 6.46 cm.
 **    z size is a parameter, to be set by gkSensorThickness.
 **
 ** 2. Sectors  (see function CreateSectors)
 **    Sectors consist of several chained sensors. These are arranged
 **    vertically on top of each other with a gap to be set by
 **    gkChainGapY. Sectors are constructed as TGeoVolumeAssembly.
 **    The sectors are auxiliary volumes used for proper placement
 **    of the sensor(s) in the module. They do not show up in the
 **    final geometry.
 **
 ** 3. Modules (see function ConstructModule)
 **    A module is a readout unit, consisting of one sensor or
 **    a chain of sensors (see sector) and a cable.
 **    The cable extends from the top of the sector vertically to the
 **    top of the halfladder the module is placed in. The cable and module
 **    volume thus depend on the vertical position of the sector in
 **    the halfladder. The cables consist of silicon with a thickness to be
 **    set by gkCableThickness.
 **    Modules are constructed as TGeoVolume, shape box, medium gStsMedium.
 **    The module construction can be switched off (gkConstructCables)
 **    to reproduce older geometries.
 **
 ** 4. Halfladders (see function ConstructHalfLadder)
 **    A halfladder is a vertical assembly of several modules. The modules
 **    are placed vertically such that their sectors overlap by
 **    gkSectorOverlapY. They are displaced in z direction to allow for the
 **    overlap in y by gkSectorGapZ.
 **    The horizontal placement of modules in the halfladder can be choosen
 **    to left aligned or right aligned, which only matters if sensors of
 **    different x size are involved.
 **    Halfladders are constructed as TGeoVolumeAssembly.
 **
 ** 5. Ladders (see function CreateLadders and ConstructLadder)
 **    A ladder is a vertical assembly of two halfladders, and is such the
 **    vertical building block of a station. The second (bottom) half ladder
 **    is rotated upside down. The vertical arrangement is such that the
 **    inner sectors of the two halfladders have the overlap gkSectorOverlapY
 **    (function CreateLadder) or that there is a vertical gap for the beam
 **    hole (function CreateLadderWithGap).
 **    Ladders are constructed as TGeoVolumeAssembly.
 **
 ** 6. Stations (see function ConstructStation)
 **    A station represents one layer of the STS geometry: one measurement
 **    at (approximately) a given z position. It consist of several ladders
 **    arranged horizontally to cover the acceptance.
 **    The ladders are arranged such that there is a horizontal overlap
 **    between neighbouring ladders (gkLadderOverLapX) and a vertical gap
 **    to allow for this overlap (gkLadderGapZ). Each second ladder is
 **    rotated around its y axis to face away from or into the beam.
 **    Stations are constructed as TGeoVolumes, shape box minus tube (for
 **    the beam hole), material gStsMedium.
 **
 ** 7. STS
 **    The STS is a volume hosting the entire detectors system. It consists
 **    of several stations located at different z positions.
 **    The STS is constructed as TGeoVolume, shape box minus cone (for the
 **    beam pipe), material gStsMedium. The size of the box is computed to
 **    enclose all stations.
 *****************************************************************************/


// Remark: With the proper steering variables, this should exactly reproduce
// the geometry version v11b of A. Kotynia's described in the ASCII format.
// The only exception is a minimal difference in the z position of the
// sectors/sensors. This is because of ladder types 2 and 4 containing the half
// sensors around the beam hole (stations 1,2 and 3). In v11b, the two ladders
// covering the beam hole cannot be transformed into each other by rotations,
// but only by a reflection. This means they are constructionally different.
// To avoid introducing another two ladder types, the difference in z position
// was accepted.


// Differences to v12:
// gkChainGap reduced from 1 mm to 0
// gkCableThickness increased from 100 mum to 200 mum (2 cables per module)
// gkSectorOverlapY reduced from 3 mm to 2.4 mm
// New sensor types 05 and 06
// New sector types 07 and 08
// Re-definiton of ladders (17 types instead of 8)
// Re-definiton of station from new ladders


#include <iomanip>
#include <iostream>
#include "TGeoManager.h"

#include "TGeoTube.h"
#include "TGeoPara.h"
#include "TGeoCone.h"
#include "TGeoTrd2.h"
#include "TGeoCompositeShape.h"

// forward declarations
Int_t CreateSensors();
Int_t CreateSectors();
Int_t CreateLadders();
TGeoVolume* ConstructModule(const char* name,
			    TGeoVolume* sector,
			    Double_t cableLength);
TGeoVolume* ConstructHalfLadder(const TString& name,
				Int_t nSectors,
				Int_t* sectorTypes,
				char align);
TGeoVolume* ConstructLadder(Int_t LadderIndex,
			     TGeoVolume* halfLadderU,
			     TGeoVolume* halfLadderD,
			     Double_t gapY,
                             Double_t shiftZ);
TGeoVolume* ConstructStation(Int_t iStation,
                             Int_t nLadders,
                             Int_t* ladderTypes,
                             Double_t rHole);
void CheckVolume(TGeoVolume* volume);
void CheckVolume(TGeoVolume* volume, fstream& file);
Double_t BeamPipeRadius(Double_t z);
TGeoVolume* ConstructFrameElement(const TString& name, TGeoVolume* frameBoxVol, Double_t x);
TGeoVolume* ConstructSmallCone(Double_t coneDz);
TGeoVolume* ConstructBigCone(Double_t coneDz);

// -------------   Steering variables       -----------------------------------

// ---> Thickness of sensors [cm]
const Double_t gkSensorThickness = 0.03;

// ---> Vertical gap between chained sensors [cm]
const Double_t gkChainGapY       = 0.00;

// ---> Thickness of cables [cm]
const Double_t gkCableThickness  = 0.02;

// ---> Vertical overlap of neighbouring sectors in a ladder [cm]
const Double_t gkSectorOverlapY  = 0.27;

// ---> Gap in z between neighbouring sectors in a ladder [cm]
const Double_t gkSectorGapZ      = 0.02;

// ---> Horizontal overlap of neighbouring ladders [cm]
const Double_t gkLadderOverlapX  = 0.30;

// ---> Gap in z between neighbouring ladders [cm]
const Double_t gkLadderGapZ      = 1.00;  // 0.00;

// ---> Gap in z between lowest sector to carbon support structure [cm]
const Double_t gkSectorGapZFrame = 0.10;

// ---> Switch to construct / not to construct readout cables
const Bool_t   gkConstructCables = kTRUE;

// ---> Switch to construct / not to construct frames
const Bool_t   gkConstructCones       = kFALSE; // kTRUE;   // switch this false by default for v15c and v16x
const Bool_t   gkConstructFrames      = kTRUE;  // kFALSE;  // switch this true  by default for v15c and v16x
const Bool_t   gkConstructSmallFrames = kTRUE;  // kFALSE;
const Bool_t   gkCylindricalFrames    = kTRUE;  // kFALSE;

// ---> Size of the frame
const Double_t gkFrameThickness     = 0.2;
const Double_t gkThinFrameThickness = 0.05;
const Double_t gkFrameStep          = 4.0;  // size of frame cell along y direction

const Double_t gkCylinderDiaInner   = 0.07; // properties of cylindrical carbon supports, see CBM-STS Integration Meeting (10 Jul 2015)
const Double_t gkCylinderDiaOuter   = 0.15; // properties of cylindrical carbon supports, see CBM-STS Integration Meeting (10 Jul 2015)

// ----------------------------------------------------------------------------



// --------------   Parameters of beam pipe in the STS region    --------------
// ---> Needed to compute stations and STS such as to avoid overlaps
const Double_t gkPipeZ1 =  22.0;
const Double_t gkPipeR1 =   1.8;
const Double_t gkPipeZ2 =  50.0;
const Double_t gkPipeR2 =   1.8;
const Double_t gkPipeZ3 = 125.0;
const Double_t gkPipeR3 =   5.5;

//DE const Double_t gkPipeZ1 =  27.0;
//DE const Double_t gkPipeR1 =   1.05;
//DE const Double_t gkPipeZ2 = 160.0;
//DE const Double_t gkPipeR2 =   3.25;
// ----------------------------------------------------------------------------



// -------------   Other global variables   -----------------------------------
// ---> STS medium (for every volume except silicon)
TGeoMedium*    gStsMedium        = NULL;  // will be set later
// ---> TGeoManager (too lazy to write out 'Manager' all the time
TGeoManager*   gGeoMan           = NULL;  // will be set later
// ----------------------------------------------------------------------------




// ============================================================================
// ======                         Main function                           =====
// ============================================================================

void create_rootgeom_SSD_v17a(const char* geoTag="v17a")
{

  // -------   Geometry file name (output)   ----------------------------------
  //TString geoFileName = "sts_";
  //geoFileName = geoFileName + geoTag + ".geo.root";
  // --------------------------------------------------------------------------

  // ----  set working directory  --------------------------------------------
  TString gPath = gSystem->Getenv("VMCWORKDIR");

  // -------   Geometry file name (output)   ----------------------------------
  const TString geoDetectorName = "SSD";
  const TString geoDetectorVersion = geoTag;
  const TString geoFileName = gPath + "/geometry/" + geoDetectorName + "_"+ geoDetectorVersion + ".root";


  // -------   Open info file   -----------------------------------------------
  TString infoFileName = geoFileName;
  infoFileName.ReplaceAll("root", "info");
  fstream infoFile;
  infoFile.open(infoFileName.Data(), fstream::out);
  infoFile << "SSD geometry created with create_rootgeom_SSD_v17a.C" << endl << endl;
  infoFile << "Global variables: " << endl;
  infoFile << "Sensor thickness = " << gkSensorThickness << " cm" << endl;
  infoFile << "Vertical gap in sensor chain = "
	   << gkChainGapY << " cm" << endl;
  infoFile << "Vertical overlap of sensors = "
	   << gkSectorOverlapY << " cm" << endl;
  infoFile << "Gap in z between neighbour sensors = "
	   << gkSectorGapZ << " cm" << endl;
  infoFile << "Horizontal overlap of sensors = "
	   << gkLadderOverlapX << " cm" << endl;
  infoFile << "Gap in z between neighbour ladders = "
	   << gkLadderGapZ << " cm" << endl;
  if ( gkConstructCables )
    infoFile << "Cable thickness = " << gkCableThickness << " cm" << endl;
  else
    infoFile << "No cables" << endl;
  infoFile << endl;
  infoFile << "Beam pipe: R1 = " << gkPipeR1 << " cm at z = "
	   << gkPipeZ1 << " cm" << endl;
  infoFile << "Beam pipe: R2 = " << gkPipeR2 << " cm at z = "
	   << gkPipeZ2 << " cm" << endl;
  infoFile << "Beam pipe: R3 = " << gkPipeR3 << " cm at z = "
	   << gkPipeZ3 << " cm" << endl;
  // --------------------------------------------------------------------------


  // -------   Load media from media file   -----------------------------------
  FairGeoLoader*    geoLoad = new FairGeoLoader("TGeo","FairGeoLoader");
  FairGeoInterface* geoFace = geoLoad->getGeoInterface();
  TString geoPath = gSystem->Getenv("VMCWORKDIR");
  TString medFile = geoPath + "/geometry/media.geo";
  geoFace->setMediaFile(medFile);
  geoFace->readMedia();
  gGeoMan = gGeoManager;
  // --------------------------------------------------------------------------


  // -----------------   Get and create the required media    -----------------
  FairGeoMedia*   geoMedia = geoFace->getMedia();
  FairGeoBuilder* geoBuild = geoLoad->getGeoBuilder();

  // ---> air
  FairGeoMedium* mAir      = geoMedia->getMedium("air");
  if ( ! mAir ) Fatal("Main", "FairMedium air not found");
  geoBuild->createMedium(mAir);
  TGeoMedium* air = gGeoMan->GetMedium("air");
  if ( ! air ) Fatal("Main", "Medium air not found");

  // ---> silicon
  FairGeoMedium* mSilicon  = geoMedia->getMedium("silicon");
  if ( ! mSilicon ) Fatal("Main", "FairMedium silicon not found");
  geoBuild->createMedium(mSilicon);
  TGeoMedium* silicon = gGeoMan->GetMedium("silicon");
  if ( ! silicon ) Fatal("Main", "Medium silicon not found");

  // ---> carbon
  FairGeoMedium* mCarbon  = geoMedia->getMedium("carbon");
  if ( ! mCarbon ) Fatal("Main", "FairMedium carbon not found");
  geoBuild->createMedium(mCarbon);
  TGeoMedium* carbon = gGeoMan->GetMedium("carbon");
  if ( ! carbon ) Fatal("Main", "Medium carbon not found");

  // ---> STScable
  FairGeoMedium* mSTScable  = geoMedia->getMedium("STScable");
  if ( ! mSTScable ) Fatal("Main", "FairMedium STScable not found");
  geoBuild->createMedium(mSTScable);
  TGeoMedium* STScable = gGeoMan->GetMedium("STScable");
  if ( ! STScable ) Fatal("Main", "Medium STScable not found");

  // ---
  gStsMedium = air;
  // --------------------------------------------------------------------------


  // --------------   Create geometry and top volume  -------------------------
  gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");
  gGeoMan->SetName("STSgeom");
  TGeoVolume* top = new TGeoVolumeAssembly("TOP");
  gGeoMan->SetTopVolume(top);
  // --------------------------------------------------------------------------


  // --------------   Create media   ------------------------------------------
  /*
  cout << endl;
  cout << "===> Creating media....";
  cout << CreateMedia();
  cout << " media created" << endl;
  TList* media = gGeoMan->GetListOfMedia();
  for (Int_t iMedium = 0; iMedium < media->GetSize(); iMedium++ ) {
    cout << "Medium " << iMedium << ": "
	 << ((TGeoMedium*) media->At(iMedium))->GetName() << endl;
  }
  gStsMedium = gGeoMan->GetMedium("air");
  if ( ! gStsMedium ) Fatal("Main", "medium sts_air not found");
  */
  // --------------------------------------------------------------------------


  // ---------------   Create sensors   ---------------------------------------
  cout << endl << endl;
  cout << "===> Creating sensors...." << endl << endl;
  infoFile << endl << "Sensors: " << endl;
  Int_t nSensors = CreateSensors();
  for (Int_t iSensor = 1; iSensor <= nSensors; iSensor++) {
    TString name = Form("Sensor%02d",iSensor);
    TGeoVolume* sensor = gGeoMan->GetVolume(name);

    // add color to sensors
    if (iSensor == 1)
      sensor->SetLineColor(kRed);
    if (iSensor == 2)
      sensor->SetLineColor(kGreen);
    if (iSensor == 3)
      sensor->SetLineColor(kBlue);
    if (iSensor == 4)
      sensor->SetLineColor(kAzure);
    if (iSensor == 5)
      sensor->SetLineColor(kYellow);
    if (iSensor == 6)
      sensor->SetLineColor(kYellow);
    if (iSensor == 7)
      sensor->SetLineColor(kYellow);

    CheckVolume(sensor);
    CheckVolume(sensor, infoFile);
  }
  // --------------------------------------------------------------------------


  // ----------------   Create sectors   --------------------------------------
  cout << endl << endl;
  cout << "===> Creating sectors...." << endl;
  infoFile << endl << "Sectors: " << endl;
  Int_t nSectors = CreateSectors();
  for (Int_t iSector = 1; iSector <= nSectors; iSector++) {
    cout << endl;
    TString name = Form("Sector%02d", iSector);
    TGeoVolume* sector = gGeoMan->GetVolume(name);
    CheckVolume(sector);
    CheckVolume(sector, infoFile);
  }
  // --------------------------------------------------------------------------


  // ----------------   Create ladders   --------------------------------------
  cout << endl << endl;
  cout << "===> Creating ladders...." << endl;
  infoFile << endl << "Ladders:" << endl;
  Int_t nLadders = CreateLadders();
  for (Int_t iLadder = 1; iLadder <= nLadders; iLadder++) {
    cout << endl;
    TString name = Form("Ladder%02d", iLadder);
    TGeoVolume* ladder = gGeoMan->GetVolume(name);
    CheckVolume(ladder);
    CheckVolume(ladder, infoFile);
    CheckVolume(ladder->GetNode(0)->GetVolume(), infoFile);
  }
  // --------------------------------------------------------------------------


  // ----------------   Create cones   ----------------------------------------
  Double_t coneDz = 1.64;
  TGeoVolume* coneSmallVolum = ConstructSmallCone(coneDz);
  if (!coneSmallVolum) Fatal("ConstructSmallCone", "Volume Cone not found");
  TGeoVolume* coneBigVolum = ConstructBigCone(coneDz);
  if (!coneBigVolum) Fatal("ConstructBigCone", "Volume Cone not found");
  // --------------------------------------------------------------------------


  // ----------------   Create stations   -------------------------------------
  cout << endl << endl;
  cout << "===> Creating stations...." << endl;
  infoFile << endl << "Stations: ";
  Int_t angle = 0;
  nLadders = 0;
  Int_t ladderTypes[16];
  TGeoBBox*        statShape = NULL;
  TGeoTranslation* statTrans = NULL;

  TGeoVolume *mystation[8];

  //Int_t statPos[8]  = { 30, 40, 50, 60, 70, 80, 90, 100 };  // z positions of stations
  Int_t statPos[8]  = { 5, 15, 25, 40, 50, 60, 70, 80 };  // z positions of stations

  Double_t rHole[8] = { 2.0, 2.0, 2.0, 2.9, 3.7, 3.7, 4.2, 4.2 };  // size of cutouts in stations

  Int_t cone_size[8]      = { 0, 0, 0, 1, 1, 1, 1, 1 };  // size of cones: 0 = small, 1 = large

  Double_t cone_offset[2] = { 0.305, 0.285 };

  Int_t allLadderTypes[8][16]= { {   0,   0,   0,   0,  10, 109,   9, 101,   1, 109,   9, 110,   0,   0,   0,   0 },    // station 1
                                 {   0,   0, 111,  10, 110,   9, 109,   2, 102,   9, 109,  10, 110,  11,   0,   0 },    // station 2
                                 {   0,   0,  14, 113,  12, 112,  12, 103,   3, 112,  12, 112,  13, 114,   0,   0 },    // station 3
                                 {   0,  15, 114,  13, 112,  12, 112,   4, 104,  12, 112,  12, 113,  14, 115,   0 },    // station 4
                                 {   0, 119,  18, 117,  17, 116,  16, 105,   5, 116,  16, 117,  17, 118,  19,   0 },    // station 5
                                 {   0,  19, 118,  17, 117,  16, 116,   6, 106,  16, 116,  17, 117,  18, 119,   0 },    // station 6
                                 {  21, 119,  18, 120,  20, 120,  20, 107,   7, 120,  20, 120,  20, 118,  19, 121 },    // station 7
                                 { 119,  17, 123,  22, 122,  22, 122,   8, 108,  22, 122,  22, 122,  23, 117,  19 } };  // station 8

  Int_t allUnitTypes[16][16];

//  unitTypes[0]  = {   0,   0,   0,   0,  10,   0,   9,   0,   1,   0,   9,   0,   0,   0,   0,   0 };  // unit 0D
//  unitTypes[1]  = {   0,   0,   0,   0,   0, 109,   0, 101,   0, 109,   0, 110,   0,   0,   0,   0 };	 // unit 1U
//  unitTypes[2]  = {   0,   0,   0,  10,   0,   9,   0,   2,   0,   9,   0,  10,   0,  11,   0,   0 };	 // unit 1D
//  unitTypes[3]  = {   0,   0, 111,   0, 110,   0, 109,   0, 102,   0, 109,   0, 110,   0,   0,   0 };	 // unit 2U
//  unitTypes[4]  = {   0,   0,  14,   0,  12,   0,  12,   0,   3,   0,  12,   0,  13,   0,   0,   0 };  // unit 2D
//  unitTypes[5]  = {   0,   0,   0, 113,   0, 112,   0, 103,   0, 112,   0, 112,   0, 114,   0,   0 };	 // unit 3U
//  unitTypes[6]  = {   0,  15,   0,  13,   0,  12,   0,   4,   0,  12,   0,  12,   0,  14,   0,   0 };	 // unit 3D
//  unitTypes[7]  = {   0,   0, 114,   0, 112,   0, 112,   0, 104,   0, 112,   0, 113,   0, 115,   0 };	 // unit 4U
//  unitTypes[8]  = {   0,   0,  18,   0,  17,   0,  16,   0,   5,   0,  16,   0,  17,   0,  19,   0 };  // unit 4D
//  unitTypes[9]  = {   0, 119,   0, 117,   0, 116,   0, 105,   0, 116,   0, 117,   0, 118,   0,   0 };	 // unit 5U
//  unitTypes[10] = {   0,  19,   0,  17,   0,  16,   0,   6,   0,  16,   0,  17,   0,  18,   0,   0 };	 // unit 5D
//  unitTypes[11] = {   0,   0, 118,   0, 117,   0, 116,   0, 106,   0, 116,   0, 117,   0, 119,   0 };	 // unit 6U
//  unitTypes[12] = {  21,   0,  18,   0,  20,   0,  20,   0,   7,   0,  20,   0,  20,   0,  19,   0 };  // unit 6D
//  unitTypes[13] = {   0, 119,   0, 120,   0, 120,   0, 107,   0, 120,   0, 120,   0, 118,   0, 121 };	 // unit 7U
//  unitTypes[14] = {   0,  17,   0,  22,   0,  22,   0,   8,   0,  22,   0,  22,   0,  23,   0,  19 };	 // unit 7D
//  unitTypes[15] = { 119,   0, 123,   0, 122,   0, 122,   0, 108,   0, 122,   0, 122,   0, 117,   0 };	 // unit 8U


  // generate unit
  for (Int_t iUnit = 0; iUnit < 16; iUnit++)
    for (Int_t iLadder = 0; iLadder < 16; iLadder++)
    {
      allUnitTypes[iUnit][iLadder] = 0;
      if ((iUnit % 2 == 0) && (allLadderTypes[iUnit/2][iLadder] <  100))  // if carbon structure is oriented upstream
        allUnitTypes[iUnit][iLadder] = allLadderTypes[iUnit/2][iLadder];
      if ((iUnit % 2 == 1) && (allLadderTypes[iUnit/2][iLadder] >= 100))  // if carbon structure is oriented downstream
        allUnitTypes[iUnit][iLadder] = allLadderTypes[iUnit/2][iLadder];
    }


  // dump unit
  for (Int_t iUnit = 0; iUnit < 16; iUnit++)
  {
    cout << "DE unitTypes[" << iUnit << "] = { ";
    for (Int_t iLadder = 0; iLadder < 16; iLadder++)
    {
      cout << allUnitTypes[iUnit][iLadder];
      if (iLadder < 15)
        cout << ", ";
      else
        cout << " };";
    }
    cout << endl;
  }


  // --- Stations 01 - 08
  for (Int_t iStation = 0; iStation < 8; iStation++)
  {
    cout << endl;

    nLadders = 0;
    for (Int_t i=0; i < 16; i++)
      if (allLadderTypes[iStation][i] != 0)
      {
       ladderTypes[nLadders] = allLadderTypes[iStation][i];
       cout << "DE ladderTypes[" << nLadders << "] = " << allLadderTypes[iStation][i] << ";" << endl;
       nLadders++;
      }
    mystation[iStation] = ConstructStation(iStation, nLadders, ladderTypes, rHole[iStation]);

    if (gkConstructCones) {
      if (iStation%2 == 0)
        angle =  90;
      else
        angle = -90;

      // upstream
      TGeoRotation* coneRot11 = new TGeoRotation;
      coneRot11->RotateZ(angle);
      coneRot11->RotateY(180);
      //      TGeoCombiTrans* conePosRot11 = new TGeoCombiTrans(name+"conePosRot2", 0., 0., -coneDz-cone_offset[cone_size[iStation]]-gkLadderGapZ/2., coneRot11);
      TGeoCombiTrans* conePosRot11 = new TGeoCombiTrans("conePosRot2", 0., 0., -coneDz-cone_offset[cone_size[iStation]]-gkLadderGapZ/2., coneRot11);
      if (cone_size[iStation] == 0)
        mystation[iStation]->AddNode(coneSmallVolum, 1, conePosRot11);
      else
        mystation[iStation]->AddNode(coneBigVolum, 1, conePosRot11);

      // downstream
      TGeoRotation* coneRot12 = new TGeoRotation;
      coneRot12->RotateZ(angle);
      //      TGeoCombiTrans* conePosRot12 = new TGeoCombiTrans(name+"conePosRot1", 0., 0.,  coneDz+cone_offset[cone_size[iStation]]+gkLadderGapZ/2., coneRot12);
      TGeoCombiTrans* conePosRot12 = new TGeoCombiTrans("conePosRot1", 0., 0.,  coneDz+cone_offset[cone_size[iStation]]+gkLadderGapZ/2., coneRot12);
      if (cone_size[iStation] == 0)
        mystation[iStation]->AddNode(coneSmallVolum, 2, conePosRot12);
      else
        mystation[iStation]->AddNode(coneBigVolum, 2, conePosRot12);

      mystation[iStation]->GetShape()->ComputeBBox();
    }

    CheckVolume(mystation[iStation]);
    CheckVolume(mystation[iStation], infoFile);
    infoFile << "Position z = " << statPos[iStation] << endl;
  }

  // --------------------------------------------------------------------------


  // ---------------   Create STS volume   ------------------------------------
  cout << endl << endl;
  cout << "===> Creating STS...." << endl;

  // --- Determine size of STS box
  Double_t stsX = 0.;
  Double_t stsY = 0.;
  Double_t stsZ = 0.;
  Double_t stsBorder = 2*5.;  // 5 cm space for carbon ladders on each side
  for (Int_t iStation = 1; iStation<=8; iStation++) {
    TString statName = Form("Station%02d", iStation);
    TGeoVolume* station = gGeoMan->GetVolume(statName);
    TGeoBBox* shape = (TGeoBBox*) station->GetShape();
    stsX = TMath::Max(stsX, 2.* shape->GetDX() );
    stsY = TMath::Max(stsY, 2.* shape->GetDY() );
    cout << "Station " << iStation << ":  Y " << stsY << endl;
  }
  // --- Some border around the stations
  stsX += stsBorder;
  stsY += stsBorder;
  stsZ = ( statPos[7] - statPos[0] ) + stsBorder;
  Double_t stsPosZ = 0.5 * ( statPos[7] + statPos[0] );

  // --- Create box  around the stations
  new TGeoBBox("stsBox", stsX/2., stsY/2., stsZ/2.);
  cout << "size of STS box: x " <<  stsX << " - y " << stsY << " - z " << stsZ << endl;

  // --- Create cone hosting the beam pipe
  // --- One straight section with constant radius followed by a cone
  Double_t z1 = statPos[0] - 0.5 * stsBorder;  // start of STS box
  Double_t z2 = gkPipeZ2;
  Double_t z3 = statPos[7] + 0.5 * stsBorder;  // end of STS box
  Double_t r1 = BeamPipeRadius(z1);
  Double_t r2 = BeamPipeRadius(z2);
  Double_t r3 = BeamPipeRadius(z3);
  r1 += 0.01;    // safety margin
  r2 += 0.01;    // safety margin
  r3 += 0.01;    // safety margin

  cout << endl;
  cout << z1 << "  " << r1 << endl;
  cout << z2 << "  " << r2 << endl;
  cout << z3 << "  " << r3 << endl;

  cout << endl;
  cout << "station1 :  " << BeamPipeRadius(statPos[0]) << endl;
  cout << "station2 :  " << BeamPipeRadius(statPos[1]) << endl;
  cout << "station3 :  " << BeamPipeRadius(statPos[2]) << endl;
  cout << "station4 :  " << BeamPipeRadius(statPos[3]) << endl;
  cout << "station5 :  " << BeamPipeRadius(statPos[4]) << endl;
  cout << "station6 :  " << BeamPipeRadius(statPos[5]) << endl;
  cout << "station7 :  " << BeamPipeRadius(statPos[6]) << endl;
  cout << "station8 :  " << BeamPipeRadius(statPos[7]) << endl;

  //  TGeoPcon* cutout = new TGeoPcon("stsCone", 0., 360., 3); // 2.*TMath::Pi(), 3);
  //  cutout->DefineSection(0, z1, 0., r1);
  //  cutout->DefineSection(1, z2, 0., r2);
  //  cutout->DefineSection(2, z3, 0., r3);
  new TGeoTrd2("stsCone1", r1, r2, r1, r2, (z2-z1)/2.+.1);  // add .1 in z length for a clean cutout
  TGeoTranslation *trans1 = new TGeoTranslation("trans1", 0., 0., -(z3-z1)/2.+(z2-z1)/2.);
  trans1->RegisterYourself();
  new TGeoTrd2("stsCone2", r2, r3, r2, r3, (z3-z2)/2.+.1);  // add .1 in z length for a clean cutout
  TGeoTranslation *trans2 = new TGeoTranslation("trans2", 0., 0., +(z3-z1)/2.-(z3-z2)/2.);
  trans2->RegisterYourself();

//DE   Double_t z1 = statPos[0] - 0.5 * stsBorder;  // start of STS box
//DE   Double_t z2 = statPos[7] + 0.5 * stsBorder;  // end of STS box
//DE   Double_t slope = (gkPipeR2 - gkPipeR1) / (gkPipeZ2 - gkPipeZ1);
//DE   Double_t r1 = gkPipeR1 + slope * (z1 - gkPipeZ1); // at start of STS
//DE   Double_t r2 = gkPipeR1 + slope * (z2 - gkPipeZ1); // at end of STS
//DE   r1 += 0.1;    // safety margin
//DE   r2 += 0.1;    // safety margin
//DE   //  new TGeoCone("stsCone", stsZ/2., 0., r1, 0., r2);
//DE   new TGeoTrd2("stsCone", r1, r2, r1, r2, stsZ/2.);

  // --- Create STS volume
  TString stsName = "sts_";
  stsName += geoTag;
  TGeoShape* stsShape = new TGeoCompositeShape("stsShape",
                                               "stsBox-stsCone1:trans1-stsCone2:trans2");
  TGeoVolume* sts = new TGeoVolume(stsName.Data(), stsShape, gStsMedium);

  // --- Place stations in the STS
  for (Int_t iStation = 1; iStation <=3; iStation++) {
    TString statName = Form("Station%02d", iStation);
    TGeoVolume* station = gGeoMan->GetVolume(statName);
    Double_t posZ = statPos[iStation-1] - stsPosZ;
    //    Double_t posZ = statPos[iStation-1];
    TGeoTranslation* trans = new TGeoTranslation(0., 0., posZ);
    sts->AddNode(station, 1, trans);
    sts->GetShape()->ComputeBBox();
  }
  cout << endl;
  CheckVolume(sts);
  // --------------------------------------------------------------------------


  // ---------------   Finish   -----------------------------------------------
  TGeoTranslation* stsTrans = new TGeoTranslation(0., 0., stsPosZ);
  // TGeoTranslation* stsTrans = new TGeoTranslation(0., 0., 0.);
  top->AddNode(sts, 1, stsTrans);
  top->GetShape()->ComputeBBox();
  cout << endl << endl;
  CheckVolume(top);
  cout << endl << endl;
  gGeoMan->CloseGeometry();
  gGeoMan->CheckOverlaps(0.0001);
  gGeoMan->PrintOverlaps();
  gGeoMan->Test();

  top->Export(geoFileName);   // an alternative way of writing the sts volume

  TFile* geoFile = new TFile(geoFileName, "UPDATE");
  TGeoTranslation* sts_placement = new TGeoTranslation("ssd_trans", 0., 0., stsPosZ);
  sts_placement->Write();

  cout << endl;
  cout << "Geometry " << top->GetName() << " written to "
       << geoFileName << endl;
  geoFile->Close();

/*
  TString geoFileName_ = geoFileName;
  geoFileName_ = geoFileName_ + "_geo.root";

  geoFile = new TFile(geoFileName_, "RECREATE");
  gGeoMan->Write();  // use this is you want GeoManager format in the output
  geoFile->Close();

  TString geoFileName__ = geoFileName;
  geoFileName_ = geoFileName__ + "-geo.root";
  sts->Export(geoFileName_);

  geoFile = new TFile(geoFileName_, "UPDATE");
  stsTrans->Write();
  geoFile->Close();
*/

  top->Draw("ogl");
  gGeoManager->SetVisLevel(6);

  infoFile.close();

}
// ============================================================================
// ======                   End of main function                          =====
// ============================================================================





// ****************************************************************************
// *****      Definition of media, sensors, sectors and ladders           *****
// *****                                                                  *****
// *****     Decoupled from main function for better readability          *****
// ****************************************************************************


/** ===========================================================================
 ** Create media
 **
 ** Currently created: air, active silicon, passive silion
 **
 ** Not used for the time being
 **/
Int_t CreateMedia() {

  Int_t nMedia     = 0;
  Double_t density = 0.;

  // --- Material air
  density = 1.205e-3;  // [g/cm^3]
  TGeoMixture* matAir = new TGeoMixture("sts_air", 3, density);
  matAir->AddElement(14.0067, 7, 0.755);      // Nitrogen
  matAir->AddElement(15.999,  8, 0.231);      // Oxygen
  matAir->AddElement(39.948, 18, 0.014);      // Argon

  // --- Material silicon
  density    = 2.33;     // [g/cm^3]
  TGeoElement* elSi   = gGeoMan->GetElementTable()->GetElement(14);
  TGeoMaterial* matSi = new TGeoMaterial("matSi", elSi, density);


  // --- Air (passive)
  TGeoMedium* medAir = new TGeoMedium("air", nMedia++, matAir);
  medAir->SetParam(0, 0.);       // is passive
  medAir->SetParam(1, 1.);       // is in magnetic field
  medAir->SetParam(2, 20.);      // max. field [kG]
  medAir->SetParam(6, 0.001);    // boundary crossing precision [cm]


  // --- Active silicon for sensors
  TGeoMedium* medSiAct = new TGeoMedium("silicon",
					nMedia++, matSi);
  medSiAct->SetParam(0, 1.);     // is active
  medSiAct->SetParam(1, 1.);     // is in magnetic field
  medSiAct->SetParam(2, 20.);    // max. field [kG]
  medSiAct->SetParam(6, 0.001);  // boundary crossing precisison [cm]

  // --- Passive silicon for cables
  TGeoMedium* medSiPas = new TGeoMedium("carbon",
					nMedia++, matSi);
  medSiPas->SetParam(0, 0.);     // is passive
  medSiPas->SetParam(1, 1.);     // is in magnetic field
  medSiPas->SetParam(2, 20.);    // max. field [kG]
  medSiPas->SetParam(6, 0.001);  // boundary crossing precisison [cm]

  return nMedia;
}
/** ======================================================================= **/




/** ===========================================================================
 ** Create sensors
 **
 ** Sensors are created as volumes with box shape and active silicon as medium.
 ** Four kinds of sensors: 3.2x2.2, 6.2x2.2, 6.2x4.2, 6.2x6.2
 **/
Int_t CreateSensors() {

  Int_t nSensors = 0;

  Double_t xSize      = 0.;
  Double_t ySize      = 0.;
  Double_t zSize      = gkSensorThickness;
  TGeoMedium* silicon = gGeoMan->GetMedium("silicon");


  // --- Sensor type 01: Small sensor (6.2 cm x 2.2 cm)
  xSize = 6.2092;
  ySize = 2.2;
  TGeoBBox* shape_sensor01 = new TGeoBBox("sensor01",
					  xSize/2., ySize/2., zSize/2.);
  new TGeoVolume("Sensor01", shape_sensor01, silicon);
  nSensors++;


  // --- Sensor type 02: Medium sensor (6.2 cm x 4.2 cm)
  xSize = 6.2092;
  ySize = 4.2;
  TGeoBBox* shape_sensor02 = new TGeoBBox("sensor02",
					  xSize/2., ySize/2., zSize/2.);
  new TGeoVolume("Sensor02", shape_sensor02, silicon);
  nSensors++;


  // ---  Sensor type 03: Big sensor (6.2 cm x 6.2 cm)
  xSize = 6.2092;
  ySize = 6.2;
  TGeoBBox* shape_sensor03 = new TGeoBBox("sensor03",
					  xSize/2., ySize/2., zSize/2.);
  new TGeoVolume("Sensor03", shape_sensor03, silicon);
  nSensors++;


  // ---  Sensor type 04: Big sensor (6.2 cm x 12.4 cm)
  xSize = 6.2092;
  ySize = 12.4;
  TGeoBBox* shape_sensor04 = new TGeoBBox("sensor04",
					  xSize/2., ySize/2., zSize/2.);
  new TGeoVolume("Sensor04", shape_sensor04, silicon);
  nSensors++;


  // below are extra small sensors, those are not available in the CAD model

  // --- Sensor Type 05: Half small sensor (4 cm x 2.5 cm)
  xSize = 4.0;
  ySize = 2.5;
  TGeoBBox* shape_sensor05 = new TGeoBBox("sensor05",
					  xSize/2., ySize/2., zSize/2.);
  new TGeoVolume("Sensor05", shape_sensor05, silicon);
  nSensors++;


  // ---  Sensor type 06: Additional "in hole" sensor (3.1 cm x 4.2 cm)
  xSize = 3.1;
  ySize = 4.2;
  TGeoBBox* shape_sensor06 = new TGeoBBox("sensor06",
					  xSize/2., ySize/2., zSize/2.);
  new TGeoVolume("Sensor06", shape_sensor06, silicon);
  nSensors++;


  // ---  Sensor type 07: Mini Medium sensor (1.5 cm x 4.2 cm)
  xSize = 1.5;
  ySize = 4.2;
  TGeoBBox* shape_sensor07 = new TGeoBBox("sensor07",
					  xSize/2., ySize/2., zSize/2.);
  new TGeoVolume("Sensor07", shape_sensor07, silicon);
  nSensors++;


  return nSensors;
}
/** ======================================================================= **/




/** ===========================================================================
 ** Create sectors
 **
 ** A sector is either a single sensor or several chained sensors.
 ** It is implemented as TGeoVolumeAssembly.
 ** Currently available:
 ** - single sensors of type 1 - 4
 ** - two chained sensors of type 4
 ** - three chained sensors of type 4
 **/
Int_t CreateSectors() {

  Int_t nSectors = 0;

  TGeoVolume* sensor01 = gGeoMan->GetVolume("Sensor01");
  TGeoVolume* sensor02 = gGeoMan->GetVolume("Sensor02");
  TGeoVolume* sensor03 = gGeoMan->GetVolume("Sensor03");
  TGeoVolume* sensor04 = gGeoMan->GetVolume("Sensor04");
  TGeoVolume* sensor05 = gGeoMan->GetVolume("Sensor05");
  TGeoVolume* sensor06 = gGeoMan->GetVolume("Sensor06");
  TGeoVolume* sensor07 = gGeoMan->GetVolume("Sensor07");
  //  TGeoBBox*   box4     = (TGeoBBox*) sensor04->GetShape();

  // --- Sector type 1: single sensor of type 1
  TGeoVolumeAssembly* sector01 = new TGeoVolumeAssembly("Sector01");
  sector01->AddNode(sensor01, 1);
  sector01->GetShape()->ComputeBBox();
  nSectors++;

  // --- Sector type 2: single sensor of type 2
  TGeoVolumeAssembly* sector02 = new TGeoVolumeAssembly("Sector02");
  sector02->AddNode(sensor02, 1);
  sector02->GetShape()->ComputeBBox();
  nSectors++;

  // --- Sector type 3: single sensor of type 3
  TGeoVolumeAssembly* sector03 = new TGeoVolumeAssembly("Sector03");
  sector03->AddNode(sensor03, 1);
  sector03->GetShape()->ComputeBBox();
  nSectors++;

  // --- Sector type 4: single sensor of type 4
  TGeoVolumeAssembly* sector04 = new TGeoVolumeAssembly("Sector04");
  sector04->AddNode(sensor04, 1);
  sector04->GetShape()->ComputeBBox();
  nSectors++;

  // --- Sector type 5: single sensor of type 5
  TGeoVolumeAssembly* sector05 = new TGeoVolumeAssembly("Sector05");
  sector05->AddNode(sensor05, 1);
  sector05->GetShape()->ComputeBBox();
  nSectors++;

  // --- Sector type 6: single sensor of type 6
  TGeoVolumeAssembly* sector06 = new TGeoVolumeAssembly("Sector06");
  sector06->AddNode(sensor06, 1);
  sector06->GetShape()->ComputeBBox();
  nSectors++;

  // --- Sector type 7: single sensor of type 7
  TGeoVolumeAssembly* sector07 = new TGeoVolumeAssembly("Sector07");
  sector07->AddNode(sensor07, 1);
  sector07->GetShape()->ComputeBBox();
  nSectors++;

//  // --- Sector type 5: two sensors of type 4
//  TGeoVolumeAssembly* sector05 = new TGeoVolumeAssembly("Sector05");
//  Double_t shift5 = 0.5 * gkChainGapY + box4->GetDY();
//  TGeoTranslation* transD5 =
//    new TGeoTranslation("td", 0., -1. * shift5, 0.);
//  TGeoTranslation* transU5 =
//    new TGeoTranslation("tu", 0., shift5, 0.);
//  sector05->AddNode(sensor04, 1, transD5);
//  sector05->AddNode(sensor04, 2, transU5);
//  sector05->GetShape()->ComputeBBox();
//  nSectors++;

  return nSectors;
}
/** ======================================================================= **/




/** ===========================================================================
 ** Create ladders
 **
 ** Ladders are the building blocks of the stations. They contain
 ** several modules placed one after the other along the z axis
 ** such that the sectors are arranged vertically (with overlap).
 **
 ** A ladder is constructed out of two half ladders, the second of which
 ** is rotated in the x-y plane by 180 degrees and displaced
 ** in z direction.
 **/
Int_t CreateLadders() {

  Int_t nLadders = 0;

  // --- Some variables
  Int_t nSectors = 0;
  Int_t sectorTypes[10];
  TGeoBBox*   shape = NULL;
  TString     s0name;
  TString     hlname;
  char        align;
  TGeoVolume* s0vol = NULL;
  TGeoVolume* halfLadderU = NULL;
  TGeoVolume* halfLadderD = NULL;

  // --- Ladders 01-23
  Int_t allSectorTypes[23][6] = { { 5, 1, 2, 3, 3, -1 },    // ladder 01 - last column defines alignment of small sensors
                                  { 5, 1, 2, 3, 3,  0 },    // ladder 02 - last column defines alignment of small sensors
                                  { 6, 2, 2, 3, 4, -1 },    // ladder 03 - last column defines alignment of small sensors
                                  { 6, 2, 2, 3, 4,  0 },    // ladder 04 - last column defines alignment of small sensors
                                  { 7, 2, 3, 4, 4, -1 },    // ladder 05 - last column defines alignment of small sensors
                                  { 7, 2, 3, 4, 4,  0 },    // ladder 06 - last column defines alignment of small sensors
                                  { 2, 2, 3, 4, 4,  0 },    // ladder 07 - last column defines alignment of small sensors
                                  { 3, 4, 4, 4, 0,  0 },    // ladder 08 - last column defines alignment of small sensors
                                  { 1, 1, 2, 3, 3,  0 },    // ladder 09 - last column defines alignment of small sensors
                                  { 1, 1, 2, 2, 3,  0 },    // ladder 10 - last column defines alignment of small sensors
                                  { 2, 2, 0, 0, 0,  0 },    // ladder 11 - last column defines alignment of small sensors
                                  { 2, 2, 2, 3, 4,  0 },    // ladder 12 - last column defines alignment of small sensors
                                  { 2, 2, 3, 4, 0,  0 },    // ladder 13 - last column defines alignment of small sensors
                                  { 2, 3, 4, 0, 0,  0 },    // ladder 14 - last column defines alignment of small sensors
                                  { 3, 3, 0, 0, 0,  0 },    // ladder 15 - last column defines alignment of small sensors
                                  { 2, 2, 3, 4, 4,  0 },    // ladder 16 - last column defines alignment of small sensors
                                  { 2, 3, 4, 4, 0,  0 },    // ladder 17 - last column defines alignment of small sensors
                                  { 3, 4, 4, 0, 0,  0 },    // ladder 18 - last column defines alignment of small sensors
                                  { 4, 4, 0, 0, 0,  0 },    // ladder 19 - last column defines alignment of small sensors
                                  { 1, 2, 4, 4, 4,  0 },    // ladder 20 - last column defines alignment of small sensors
                                  { 4, 0, 0, 0, 0,  0 },    // ladder 21 - last column defines alignment of small sensors
                                  { 2, 3, 4, 4, 4,  0 },    // ladder 22 - last column defines alignment of small sensors
                                  { 2, 3, 3, 4, 4,  0 } };  // ladder 23 - last column defines alignment of small sensors

  Double_t gapXYZ[23][3]      = { { 0., -gkSectorOverlapY, 0. },     // ladder 01
			          { 0., -gkSectorOverlapY, 0. },     // ladder 02
			          { 0., -gkSectorOverlapY, 0. },     // ladder 03
			          { 0., -gkSectorOverlapY, 0. },     // ladder 04
			          { 0., -gkSectorOverlapY, 0. },     // ladder 05
			          { 0., -gkSectorOverlapY, 0. },     // ladder 06
			          { 0.,              8.80, 0. },     // ladder 07 - gapY = 2 * 4.40
			          { 0.,              9.14, 0. },     // ladder 08 - gapY = 2 * 4.57
			          { 0., -gkSectorOverlapY, 0. },     // ladder 09
			          { 0., -gkSectorOverlapY, 0. },     // ladder 10
			          { 0., -gkSectorOverlapY, 0. },     // ladder 11
			          { 0., -gkSectorOverlapY, 0. },     // ladder 12
			          { 0., -gkSectorOverlapY, 0. },     // ladder 13
			          { 0., -gkSectorOverlapY, 0. },     // ladder 14
			          { 0., -gkSectorOverlapY, 0. },     // ladder 15
			          { 0., -gkSectorOverlapY, 0. },     // ladder 16
			          { 0., -gkSectorOverlapY, 0. },     // ladder 17
			          { 0., -gkSectorOverlapY, 0. },     // ladder 18
			          { 0., -gkSectorOverlapY, 0. },     // ladder 19
			          { 0., -gkSectorOverlapY, 0. },     // ladder 20
			          { 0., -gkSectorOverlapY, 0. },     // ladder 21
			          { 0., -gkSectorOverlapY, 0. },     // ladder 22
			          { 0., -gkSectorOverlapY, 0. } };   // ladder 23

// ========================================================================

  // calculate Z shift for ladders with and without gaps in the center
  s0name = Form("Sector%02d", allSectorTypes[0][0]);
  s0vol  = gGeoMan->GetVolume(s0name);
  shape  = (TGeoBBox*) s0vol->GetShape();

  for (Int_t iLadder = 0; iLadder < 23; iLadder++)
  {
//    if (iLadder <= 7)
    if ((iLadder+1 == 7) || (iLadder+1 == 8))  // not for ladders with gap
      gapXYZ[iLadder][2] = 0;
    else
      gapXYZ[iLadder][2] = 2. * shape->GetDZ() + gkSectorGapZ;  // set displacement in z for overlapping half ladders
  }

// ========================================================================

  for (Int_t iLadder = 0; iLadder < 23; iLadder++)
  {
    cout << endl;
    nSectors = 0;
    for (Int_t i=0; i < 5; i++)
  	if (allSectorTypes[iLadder][i] != 0)
  	  {
  	    sectorTypes[nSectors] = allSectorTypes[iLadder][i];
  	    cout << "DE sectorTypes[" << nSectors << "] = " << allSectorTypes[iLadder][i] << ";" << endl;
  	    nSectors++;
  	  }

    if (allSectorTypes[iLadder][5] == 0)
      align = 'l';
    else
      align = 'r';
    hlname = Form("HalfLadder%02du", iLadder+1);
    halfLadderU = ConstructHalfLadder(hlname, nSectors, sectorTypes, align);  // mirrored

    if (allSectorTypes[iLadder][5] == 0)
      align = 'r';
    else
      align = 'l';
    hlname = Form("HalfLadder%02dd", iLadder+1);
    halfLadderD = ConstructHalfLadder(hlname, nSectors, sectorTypes, align);  // mirrored

    ConstructLadder(iLadder+1, halfLadderU, halfLadderD, gapXYZ[iLadder][1], gapXYZ[iLadder][2]);
    nLadders++;
  }

  return nLadders;
}
/** ======================================================================= **/



// ****************************************************************************
// *****                                                                  *****
// *****    Generic functions  for the construction of STS elements       *****
// *****                                                                  *****
// *****  module:     volume (made of a sector and a cable)               *****
// *****  haf ladder: assembly (made of modules)                          *****
// *****  ladder:     assembly (made of two half ladders)                 *****
// *****  station:    volume (made of ladders)                            *****
// *****                                                                  *****
// ****************************************************************************



/** ===========================================================================
 ** Construct a module
 **
 ** A module is a sector plus the readout cable extending from the
 ** top of the sector. The cable is made from passive silicon.
 ** The cable has the same x size as the sector.
 ** Its thickness is given by the global variable gkCableThickness.
 ** The cable length is a parameter.
 ** The sensor(s) of the sector is/are placed directly in the module;
 ** the sector is just auxiliary for the proper placement.
 **
 ** Arguments:
 **            name             volume name
 **            sector           pointer to sector volume
 **            cableLength      length of cable
 **/
TGeoVolume* ConstructModule(const char* name,
			    TGeoVolume* sector,
			    Double_t cableLength) {

  // --- Check sector volume
  if ( ! sector ) Fatal("CreateModule", "Sector volume not found!");

  // --- Get size of sector
  TGeoBBox* box = (TGeoBBox*) sector->GetShape();
  Double_t sectorX = 2. * box->GetDX();
  Double_t sectorY = 2. * box->GetDY();
  Double_t sectorZ = 2. * box->GetDZ();

  // --- Get size of cable
  Double_t cableX   = sectorX;
  Double_t cableY   = cableLength;
  Double_t cableZ   = gkCableThickness;

  // --- Create module volume
  Double_t moduleX = TMath::Max(sectorX, cableX);
  Double_t moduleY = sectorY + cableLength;
  Double_t moduleZ = TMath::Max(sectorZ, cableZ);
  TGeoVolume* module = gGeoManager->MakeBox(name, gStsMedium,
					    moduleX/2.,
					    moduleY/2.,
					    moduleZ/2.);

  // --- Position of sector in module
  // --- Sector is centred in x and z and aligned to the bottom
  Double_t sectorXpos = 0.;
  Double_t sectorYpos = 0.5 * (sectorY - moduleY);
  Double_t sectorZpos = 0.;


  // --- Get sensor(s) from sector
  Int_t nSensors = sector->GetNdaughters();
  for (Int_t iSensor = 0; iSensor < nSensors; iSensor++) {
    TGeoNode* sensor = sector->GetNode(iSensor);

    // --- Calculate position of sensor in module
    const Double_t* xSensTrans = sensor->GetMatrix()->GetTranslation();
    Double_t sensorXpos = 0.;
    Double_t sensorYpos = sectorYpos + xSensTrans[1];
    Double_t sensorZpos = 0.;
    TGeoTranslation* sensTrans = new TGeoTranslation("sensTrans",
						     sensorXpos,
						     sensorYpos,
						     sensorZpos);

    // --- Add sensor volume to module
    TGeoVolume* sensVol = sensor->GetVolume();
    module->AddNode(sensor->GetVolume(), iSensor+1, sensTrans);
    module->GetShape()->ComputeBBox();
  }


  // --- Create cable volume, if necessary, and place it in module
  // --- Cable is centred in x and z and aligned to the top
  if ( gkConstructCables && cableLength > 0.0001 ) {
    TString cableName = TString(name) + "_cable";
    TGeoMedium* cableMedium = gGeoMan->GetMedium("STScable");
    if ( ! cableMedium ) Fatal("CreateModule", "Medium STScable not found!");
    TGeoVolume* cable = gGeoManager->MakeBox(cableName.Data(),
					     cableMedium,
					     cableX / 2.,
					     cableY / 2.,
					     cableZ / 2.);
    // add color to cables
    cable->SetLineColor(kOrange);
    cable->SetTransparency(60);
    Double_t cableXpos = 0.;
    Double_t cableYpos = sectorY + 0.5 * cableY - 0.5 * moduleY;
    Double_t cableZpos = 0.;
    TGeoTranslation* cableTrans = new TGeoTranslation("cableTrans",
						      cableXpos,
						      cableYpos,
						      cableZpos);
    module->AddNode(cable, 1, cableTrans);
    module->GetShape()->ComputeBBox();
  }

  return module;
}
/** ======================================================================= **/




/** ===========================================================================
 ** Construct a half ladder
 **
 ** A half ladder is a virtual volume (TGeoVolumeAssembly) consisting
 ** of several modules arranged on top of each other. The modules
 ** have a given overlap in y and a displacement in z to allow for the
 ** overlap.
 **
 ** The typ of sectors / modules to be placed must be specified:
 **    1 = sensor01
 **    2 = sensor02
 **    3 = sensor03
 **    4 = sensor04
 **    5 = 2 x sensor04 (chained)
 **    6 = 3 x sensor04 (chained)
 ** The cable is added automatically from the top of each sensor to
 ** the top of the half ladder.
 ** The alignment can be left (l) or right (r), which matters in the
 ** case of different x sizes of sensors (e.g. SensorType01).
 **
 ** Arguments:
 **            name             volume name
 **            nSectors         number of sectors
 **            sectorTypes      array with sector types
 **            align            horizontal alignment of sectors
 **/
TGeoVolume* ConstructHalfLadder(const TString& name,
				Int_t nSectors,
				Int_t* sectorTypes,
				char align) {

  // --- Create half ladder volume assembly
  TGeoVolumeAssembly* halfLadder = new TGeoVolumeAssembly(name);

  // --- Determine size of ladder
  Double_t ladderX = 0.;
  Double_t ladderY = 0.;
  Double_t ladderZ = 0.;
  for (Int_t iSector = 0; iSector < nSectors; iSector++) {
    TString sectorName = Form("Sector%02d",
			      sectorTypes[iSector]);
    TGeoVolume* sector = gGeoMan->GetVolume(sectorName);
    if ( ! sector )
      Fatal("ConstructHalfLadder", Form("Volume %s not found", sectorName.Data()));
    TGeoBBox* box = (TGeoBBox*) sector->GetShape();
    // --- Ladder x size equals largest sector x size
    ladderX = TMath::Max(ladderX, 2. * box->GetDX());
    // --- Ladder y size is sum of sector ysizes
    ladderY += 2. * box->GetDY();
    // --- Ladder z size is sum of sector z sizes
    ladderZ += 2. * box->GetDZ();
  }
  // --- Subtract overlaps in y
  ladderY -= Double_t(nSectors-1) * gkSectorOverlapY;
  // --- Add gaps in z direction
  ladderZ += Double_t(nSectors-1) * gkSectorGapZ;


  // --- Create and place modules
  Double_t yPosSect = -0.5 * ladderY;
  Double_t zPosMod  = -0.5 * ladderZ;
  for (Int_t iSector = 0; iSector < nSectors; iSector++) {
    TString sectorName = Form("Sector%02d",
			      sectorTypes[iSector]);
    TGeoVolume* sector = gGeoMan->GetVolume(sectorName);
    TGeoBBox* box = (TGeoBBox*) sector->GetShape();
    Double_t sectorX = 2. * box->GetDX();
    Double_t sectorY = 2. * box->GetDY();
    Double_t sectorZ = 2. * box->GetDZ();
    yPosSect += 0.5 * sectorY;   // Position of sector in ladder
    Double_t cableLength = 0.5 * ladderY - yPosSect - 0.5 * sectorY;
    TString moduleName = name + "_" + Form("Module%02d",
					   sectorTypes[iSector]);
    TGeoVolume* module = ConstructModule(moduleName.Data(),
					 sector, cableLength);

    TGeoBBox* shapeMod = (TGeoBBox*) module->GetShape();
    Double_t moduleX = 2. * shapeMod->GetDX();
    Double_t moduleY = 2. * shapeMod->GetDY();
    Double_t moduleZ = 2. * shapeMod->GetDZ();
    Double_t xPosMod = 0.;
    if ( align == 'l' )
      xPosMod = 0.5 * (moduleX - ladderX);  // left aligned
    else if ( align == 'r' )
      xPosMod = 0.5 * (ladderX - moduleX);  // right aligned
    else
      xPosMod = 0.;                         // centred in x
    Double_t yPosMod = 0.5 * (ladderY - moduleY);  // top aligned
    zPosMod += 0.5 * moduleZ;
    TGeoTranslation* trans = new TGeoTranslation("t", xPosMod,
						 yPosMod, zPosMod);
    halfLadder->AddNode(module, iSector+1, trans);
    halfLadder->GetShape()->ComputeBBox();
    yPosSect += 0.5 * sectorY - gkSectorOverlapY;
    zPosMod  += 0.5 * moduleZ + gkSectorGapZ;
  }

  CheckVolume(halfLadder);
  cout << endl;

  return halfLadder;
}
/** ======================================================================= **/




/** ===========================================================================
 ** Add a carbon support to a ladder
 **
 ** Arguments:
 **            LadderIndex      ladder number
 **            ladder           pointer to ladder
 **            xu               size of halfladder
 **            ladderY          height of ladder along y
 **            ladderZ          thickness of ladder along z
 **/
void AddCarbonLadder(Int_t LadderIndex,
                     TGeoVolume* ladder,
                     Double_t xu,
                     Double_t ladderY,
                     Double_t ladderZ) {

  Int_t carbon_elem[23]= { 11, 11, 16, 16, 20, 20, 22, 24,
		  	   11, 10,  6, 16, 14, 12,  7, 20,
		           18, 16, 13, 22,  7, 24, 21      };  // number of carbon elements in ladder types

  // --- Some variables
  TString name = Form("Ladder%02d", LadderIndex);
  Int_t i;
  Double_t j;

// guess  Int_t YnumOfFrameBoxes = (Int_t)(ladderY / gkFrameStep)+1;    // calculate number of elements
// guess  if (LadderIndex == 1 || LadderIndex == 2)  // set even number of ladder elements for these ladders in station 1 and 2
// guess	YnumOfFrameBoxes--;
// guess//  if (LadderIndex == 3 || LadderIndex == 4)  // set even number of ladder elements for these ladders in station 3 and 4
// guess//	YnumOfFrameBoxes++;
// guess  YnumOfFrameBoxes += YnumOfFrameBoxes % 2;                     // use even number of frame elements for all ladders

  Int_t YnumOfFrameBoxes = carbon_elem[LadderIndex-1];

  //      cout << "DE: lad " << LadderIndex << " inum " << YnumOfFrameBoxes << endl;

  // DEDE
  TGeoBBox* fullFrameShp = new TGeoBBox (name+"_FullFrameBox_shp", xu/2., gkFrameStep/2., (xu/2.+sqrt(2.)*gkFrameThickness/2.)/2.);
  //  TGeoBBox* fullFrameShp = new TGeoBBox (name+"_FullFrameBox_shp", xu/2., gkFrameStep/2., (gkSectorGapZFrame+xu/2.+sqrt(2.)*gkFrameThickness/2.)/2.);
  TGeoVolume* fullFrameBoxVol = new TGeoVolume(name+"_FullFrameBox", fullFrameShp, gStsMedium);

  //  cout << "DE: frame Z size " << (xu/2.+sqrt(2.)*gkFrameThickness/2.) << " cm" << endl;

  ConstructFrameElement("FrameBox", fullFrameBoxVol, xu/2.);
  TGeoRotation* fullFrameRot = new TGeoRotation;
  fullFrameRot->RotateY(180);

  Int_t inum = YnumOfFrameBoxes; // 6; // 9;
  for (i=1; i<=inum; i++)
  {
    j=-(inum-1)/2.+(i-1);
 // -(10-1)/2. +0 +10-1 -> -4.5 .. +4.5 -> -0.5, +0.5 (= 2)
 // -(11-1)/2. +0 +11-1 -> -5.0 .. +5.0 -> -1, 0, 1   (= 3)
	//        cout << "DE: i " << i << " j " << j << endl;

    if (LadderIndex <= 3)  // central ladders in stations 1 to 3
    {
      if ((j>=-1) && (j<=1))   // keep the inner 2 (even) or 3 (odd) elements free for the cone
        continue;
    }
    else if (LadderIndex <= 8)  // central ladders in stations 4 to 8
    {
      if ((j>=-2) && (j<=2))   // keep the inner 4 elements free for the cone
        continue;
    }

    // DEDE
    ladder->AddNode(fullFrameBoxVol, i, new TGeoCombiTrans(name+"_FullFrameBox_posrot", 0., j*gkFrameStep, -ladderZ/2.-(xu/2.+sqrt(2.)*gkFrameThickness/2.)/2., fullFrameRot));
    //    ladder->AddNode(fullFrameBoxVol, i, new TGeoCombiTrans(name+"_FullFrameBox_posrot", 0., j*gkFrameStep, -ladderZ/2.-(gkSectorGapZFrame+xu/2.+sqrt(2.)*gkFrameThickness/2.)/2., fullFrameRot));
  }
  //      cout << endl;
  ladder->GetShape()->ComputeBBox();

}
/** ======================================================================= **/


/** ===========================================================================
 ** Construct a ladder out of two half ladders with vertical gap
 **
 ** The second half ladder will be rotated by 180 degrees
 ** in the x-y plane. The two half ladders will be put on top of each
 ** other with a vertical gap.
 **
 ** Arguments:
 **            name             volume name
 **            halfLadderU      pointer to upper half ladder
 **            halfLadderD      pointer to lower half ladder
 **            gapY             vertical gap
 **            shiftZ           relative displacement along the z axis
 **/

 TGeoVolume* ConstructLadder(Int_t LadderIndex,
			     TGeoVolume* halfLadderU,
			     TGeoVolume* halfLadderD,
			     Double_t gapY,
                             Double_t shiftZ) {

  // --- Some variables
  TGeoBBox* shape = NULL;

  // --- Dimensions of half ladders
  shape = (TGeoBBox*) halfLadderU->GetShape();
  Double_t xu = 2. * shape->GetDX();
  Double_t yu = 2. * shape->GetDY();
  Double_t zu = 2. * shape->GetDZ();

  shape = (TGeoBBox*) halfLadderD->GetShape();
  Double_t xd = 2. * shape->GetDX();
  Double_t yd = 2. * shape->GetDY();
  Double_t zd = 2. * shape->GetDZ();

  // --- Create ladder volume assembly
  TString name = Form("Ladder%02d", LadderIndex);
  TGeoVolumeAssembly* ladder = new TGeoVolumeAssembly(name);
  Double_t ladderX = TMath::Max(xu, xd);
  Double_t ladderY = yu + yd + gapY;
  Double_t ladderZ = TMath::Max(zu, zd + shiftZ);

  // --- Place half ladders
  Double_t xPosU = 0.;                      // centred in x
  Double_t yPosU = 0.5 * ( ladderY - yu );  // top aligned
  Double_t zPosU = 0.5 * ( ladderZ - zu );  // front aligned
  TGeoTranslation* tu = new TGeoTranslation("tu", xPosU, yPosU, zPosU);
  ladder->AddNode(halfLadderU, 1, tu);

  Double_t xPosD = 0.;                      // centred in x
  Double_t yPosD = 0.5 * ( yd - ladderY );  // bottom aligned
  Double_t zPosD = 0.5 * ( zd - ladderZ );  // back aligned
  TGeoRotation* rd = new TGeoRotation();
  rd->RotateZ(180.);
  TGeoCombiTrans* cd = new TGeoCombiTrans(xPosD, yPosD, zPosD, rd);
  ladder->AddNode(halfLadderD, 2, cd);
  ladder->GetShape()->ComputeBBox();

  // ----------------   Create and place frame boxes   ------------------------

  if (gkConstructFrames)
      AddCarbonLadder(LadderIndex, ladder, xu, ladderY, ladderZ);

  // --------------------------------------------------------------------------

  return ladder;
}
/** ======================================================================= **/




/** ===========================================================================
 ** Construct a station
 **
 ** The station volume is the minimal  box comprising all ladders
 ** minus a tube accomodating the beam pipe.
 **
 ** The ladders are arranged horizontally from left to right with
 ** a given overlap in x.
 ** Every second ladder is slightly displaced upstream from the centre
 ** z plane and facing downstream, the others are slightly displaced
 ** downstream and facing upstream (rotated around the y axis).
 **
 ** Arguments:
 **            name             volume name
 **            nLadders         number of ladders
 **            ladderTypes      array of ladder types
 **            rHole            radius of inner hole
 **/

 TGeoVolume* ConstructStation(Int_t iStation,
                              Int_t nLadders,
			      Int_t* ladderTypes,
                              Double_t rHole) {

  TString name;
  name = Form("Station%02d", iStation+1);  // 1,2,3,4,5,6,7,8
  //  name = Form("Station%02d", iStation);  // 0,1,2,3,4,5,6,7 - Station00 missing in output

  // --- Some local variables
  TGeoShape* statShape  = NULL;
  TGeoBBox* ladderShape = NULL;
  TGeoVolume* ladder    = NULL;
  TString ladderName;


  // --- Determine size of station from ladders
  Double_t statX     = 0.;
  Double_t statY     = 0.;
  Double_t statZeven = 0.;
  Double_t statZodd  = 0.;
  Double_t statZ     = 0.;
  for (Int_t iLadder = 0; iLadder < nLadders; iLadder++) {
    Int_t ladderType = ladderTypes[iLadder]%100;
    ladderName = Form("Ladder%02d", ladderType);
    ladder = gGeoManager->GetVolume(ladderName);
    if ( ! ladder ) Fatal("ConstructStation",
			  Form("Volume %s not found", ladderName.Data()));
    TGeoBBox* shape = (TGeoBBox*) ladder->GetShape();
    statX += 2. * shape->GetDX();
    statY = TMath::Max(statY, 2. * shape->GetDY());
    if ( iLadder % 2 ) statZeven = TMath::Max(statZeven, 2. * shape->GetDZ() );
    else statZodd = TMath::Max(statZodd, 2. * shape->GetDZ() );
  }
  statX -= Double_t(nLadders-1) * gkLadderOverlapX;
  statZ = statZeven + gkLadderGapZ + statZodd;

  // --- Create station volume
  TString boxName(name);
  boxName += "_box";

  cout << "before   statZ/2.: " << statZ/2. << endl;
  statZ = 2 * 4.5;  // changed Z size of the station for cone and gkLadderGapZ
  cout << "fixed to statZ/2.: " << statZ/2. << endl;
  TGeoBBox* statBox = new TGeoBBox(boxName, statX/2., statY/2., statZ/2.);

  TString tubName(name);
  tubName += "_tub";
  TString expression = boxName + "-" + tubName;
  //  TGeoTube* statTub = new TGeoTube(tubName, 0., rHole, statZ/2.);
  //  TGeoBBox* statTub = new TGeoBBox(tubName, rHole, rHole, statZ/2.);
  TGeoBBox* statTub = new TGeoBBox(tubName, rHole, rHole, statZ/2.+.1);  // .1 opens the hole in z direction

  statShape = new TGeoCompositeShape(name, expression.Data());
  TGeoVolume* station = new TGeoVolume(name, statShape, gStsMedium);

  Double_t subtractedVal;

  // --- Place ladders in station
  cout << "xPos0: " << statX << endl;
  Double_t xPos = -0.5 * statX;
  cout << "xPos1: " << xPos << endl;
  Double_t yPos = 0.;
  Double_t zPos = 0.;

  Double_t maxdz = 0.;
  for (Int_t iLadder = 0; iLadder < nLadders; iLadder++) {  // find maximum dz in this station
    Int_t ladderType = ladderTypes[iLadder]%100;
    ladderName = Form("Ladder%02d", ladderType);
    ladder = gGeoManager->GetVolume(ladderName);
    TGeoBBox* shape = (TGeoBBox*) ladder->GetShape();
    if (maxdz < shape->GetDZ())
      maxdz = shape->GetDZ();
  }

  for (Int_t iLadder = 0; iLadder < nLadders; iLadder++) {
    Int_t ladderType = ladderTypes[iLadder]%100;
    ladderName = Form("Ladder%02d", ladderType);
    ladder = gGeoManager->GetVolume(ladderName);
    TGeoBBox* shape = (TGeoBBox*) ladder->GetShape();
    xPos += shape->GetDX();
    cout << "xPos2: " << xPos << endl;
    yPos = 0.;    // vertically centred
    TGeoRotation* rot = new TGeoRotation();

    if (gkConstructFrames)
      // DEDE
      subtractedVal = sqrt(2.)*gkFrameThickness/2. + shape->GetDX();
    //      subtractedVal = 2*gkSectorGapZFrame + sqrt(2.)*gkFrameThickness/2. + shape->GetDX();
    else
      subtractedVal = 0.;

    //    zPos = 0.5 * gkLadderGapZ + (shape->GetDZ()-subtractedVal/2.);  // non z-aligned ladders
    zPos = 0.5 * gkLadderGapZ + (2*maxdz-shape->GetDZ()-subtractedVal/2.);  // z-aligned ladders

    cout << "DE ladder" << ladderTypes[iLadder]%100
	 << "  dx: " << shape->GetDX()
	 << "  dy: " << shape->GetDY()
	 << "  dz: " << shape->GetDZ()
	 << "  max dz: " << maxdz << endl;

    cout << "DE ladder" << ladderTypes[iLadder]%100
	 << "  fra: " << gkFrameThickness/2.
	 << "  sub: " << subtractedVal
	 << "  zpo: " << zPos << endl << endl;

    if (ladderTypes[iLadder]/100 == 1) // flip some of the ladders to reproduce the CAD layout
      rot->RotateY(180.);
    else
      zPos = -zPos;

    TGeoCombiTrans* trans = new TGeoCombiTrans(xPos, yPos, zPos, rot);
    station->AddNode(ladder, iLadder+1, trans);
    station->GetShape()->ComputeBBox();
    xPos += shape->GetDX() - gkLadderOverlapX;
    cout << "xPos3: " << xPos << endl;
  }

  return station;
 }
/** ======================================================================= **/




/** ===========================================================================
 ** Volume information for debugging
 **/
void CheckVolume(TGeoVolume* volume) {

  TGeoBBox* shape = (TGeoBBox*) volume->GetShape();
  cout << volume->GetName() << ": size " << fixed << setprecision(4)
       << setw(7) << 2. * shape->GetDX() << " x " << setw(7)
       << 2. * shape->GetDY() << " x " << setw(7)
       << 2. * shape->GetDZ();
  if ( volume->IsAssembly() ) cout << ", assembly";
  else {
    if ( volume->GetMedium() )
      cout << ", medium " << volume->GetMedium()->GetName();
    else cout << ", " << "\033[31m" << " no medium" << "\033[0m";
  }
  cout << endl;
  if ( volume->GetNdaughters() ) {
    cout << "Daughters: " << endl;
    for (Int_t iNode = 0; iNode < volume->GetNdaughters(); iNode++) {
      TGeoNode* node = volume->GetNode(iNode);
      TGeoBBox* shape = (TGeoBBox*) node->GetVolume()->GetShape();
      cout << setw(15) << node->GetName() << ", size "
	   << fixed << setprecision(3)
	   << setw(6) << 2. * shape->GetDX() << " x "
	   << setw(6) << 2. * shape->GetDY() << " x "
	   << setw(6) << 2. * shape->GetDZ() << ", position ( ";
      TGeoMatrix* matrix = node->GetMatrix();
      const Double_t* pos = matrix->GetTranslation();
      cout << setfill(' ');
      cout << fixed << setw(8) << pos[0] << ", "
	   << setw(8) << pos[1] << ", "
	   << setw(8) << pos[2] << " )" << endl;
    }
  }

}
/** ======================================================================= **/


/** ===========================================================================
 ** Volume information for output to file
 **/
void CheckVolume(TGeoVolume* volume, fstream& file) {

  if ( ! file ) return;

  TGeoBBox* shape = (TGeoBBox*) volume->GetShape();
  file << volume->GetName() << ": size " << fixed << setprecision(4)
       << setw(7) << 2. * shape->GetDX() << " x " << setw(7)
       << 2. * shape->GetDY() << " x " << setw(7)
       << 2. * shape->GetDZ();
  if ( volume->IsAssembly() ) file << ", assembly";
  else {
    if ( volume->GetMedium() )
      file << ", medium " << volume->GetMedium()->GetName();
    else file << ", " << "\033[31m" << " no medium" << "\033[0m";
  }
  file << endl;
  if ( volume->GetNdaughters() ) {
    file << "Contains: ";
    for (Int_t iNode = 0; iNode < volume->GetNdaughters(); iNode++)
      file << volume->GetNode(iNode)->GetVolume()->GetName() << " ";
    file << endl;
  }

}
/** ======================================================================= **/


/** ===========================================================================
 ** Calculate beam pipe outer radius for a given z
 **/
Double_t BeamPipeRadius(Double_t z) {
  if ( z < gkPipeZ2 ) return gkPipeR1;
  Double_t slope = (gkPipeR3 - gkPipeR2 ) / (gkPipeZ3 - gkPipeZ2);
  return gkPipeR2 + slope * (z - gkPipeZ2);
}
/** ======================================================================= **/



/** ======================================================================= **/
TGeoVolume* ConstructFrameElement(const TString& name, TGeoVolume* frameBoxVol, Double_t x)
{
	// --- Material of the frames
	TGeoMedium* framesMaterial = gGeoMan->GetMedium("carbon");

        TGeoBBox* frameVertPillarShp;

	Double_t t = gkFrameThickness/2.;

	// --- Main vertical pillars
//    	TGeoBBox* frameVertPillarShp = new TGeoBBox(name + "_vertpillar_shape", t, gkFrameStep/2., t);  // square crossection, along y
//	TGeoVolume* frameVertPillarVol = new TGeoVolume(name + "_vertpillar", frameVertPillarShp, framesMaterial);
//	frameVertPillarVol->SetLineColor(kGreen);
//	frameBoxVol->AddNode(frameVertPillarVol, 1, new TGeoTranslation(name + "_vertpillar_pos_1", x-t, 0., -(x+sqrt(2.)*t-2.*t)/2.));
//	frameBoxVol->AddNode(frameVertPillarVol, 2, new TGeoTranslation(name + "_vertpillar_pos_2", -(x-t), 0., -(x+sqrt(2.)*t-2.*t)/2.));

        if (gkCylindricalFrames)
	  //          TGeoBBox* frameVertPillarShp = new TGeoTube(name + "_vertpillar_shape", 0, t, gkFrameStep/2.);  // circle crossection, along z
          frameVertPillarShp = new TGeoTube(name + "_vertpillar_shape", gkCylinderDiaInner/2., gkCylinderDiaOuter/2., gkFrameStep/2.);  // circle crossection, along z
        else
          frameVertPillarShp = new TGeoBBox(name + "_vertpillar_shape", t, t, gkFrameStep/2.);  // square crossection, along z
	TGeoVolume* frameVertPillarVol = new TGeoVolume(name + "_vertpillar", frameVertPillarShp, framesMaterial);
	frameVertPillarVol->SetLineColor(kGreen);

        TGeoRotation* xRot90 = new TGeoRotation;
        xRot90->RotateX(90.);
	frameBoxVol->AddNode(frameVertPillarVol, 1, new TGeoCombiTrans(name + "_vertpillar_pos_1", x-t,    0., -(x+sqrt(2.)*t-2.*t)/2., xRot90));
	frameBoxVol->AddNode(frameVertPillarVol, 2, new TGeoCombiTrans(name + "_vertpillar_pos_2", -(x-t), 0., -(x+sqrt(2.)*t-2.*t)/2., xRot90));

	//	TGeoRotation* vertRot = new TGeoRotation(name + "_vertpillar_rot_1", 90., 45., -90.);
	TGeoRotation* vertRot = new TGeoRotation;
        vertRot->RotateX(90.);
        vertRot->RotateY(45.);
	frameBoxVol->AddNode(frameVertPillarVol, 3, new TGeoCombiTrans(name + "_vertpillar_pos_3", 0., 0., (x-sqrt(2.)*t)/2., vertRot));

	// --- Small horizontal pillar
	TGeoBBox* frameHorPillarShp = new TGeoBBox(name + "_horpillar_shape", x-2.*t, gkThinFrameThickness/2., gkThinFrameThickness/2.);
	TGeoVolume* frameHorPillarVol = new TGeoVolume(name + "_horpillar", frameHorPillarShp, framesMaterial);
	frameHorPillarVol->SetLineColor(kCyan);
	frameBoxVol->AddNode(frameHorPillarVol, 1, new TGeoTranslation(name + "_horpillar_pos_1", 0., -gkFrameStep/2.+gkThinFrameThickness/2., -(x+sqrt(2.)*t-2.*t)/2.));

	if (gkConstructSmallFrames) {

		// --- Small sloping pillar
		TGeoPara* frameSlopePillarShp = new TGeoPara(name + "_slopepillar_shape",
			(x-2.*t)/TMath::Cos(31.4/180.*TMath::Pi()), gkThinFrameThickness/2., gkThinFrameThickness/2., 31.4, 0., 90.);
		TGeoVolume* frameSlopePillarVol = new TGeoVolume(name + "_slopepillar", frameSlopePillarShp, framesMaterial);
		frameSlopePillarVol->SetLineColor(kCyan);
		TGeoRotation* slopeRot = new TGeoRotation(name + "_slopepillar_rot_1", 0., 0., 31.4);
		TGeoCombiTrans* slopeTrRot = new TGeoCombiTrans(name + "_slopepillar_posrot_1", 0., 0., -(x+sqrt(2.)*t-2.*t)/2., slopeRot);

		frameBoxVol->AddNode(frameSlopePillarVol, 1, slopeTrRot);

		Double_t angl = 23.;
		// --- Small sub pillar
		TGeoPara* frameSubPillarShp = new TGeoPara(name + "_subpillar_shape",
			(sqrt(2)*(x/2.-t)-t/2.)/TMath::Cos(angl/180.*TMath::Pi()), gkThinFrameThickness/2., gkThinFrameThickness/2., angl, 0., 90.);
		TGeoVolume* frameSubPillarVol = new TGeoVolume(name + "_subpillar", frameSubPillarShp, framesMaterial);
		frameSubPillarVol->SetLineColor(kMagenta);

		Double_t posZ = t * (1. - 3. / ( 2.*sqrt(2.) ));

		// one side of X direction
		TGeoRotation* subRot1 = new TGeoRotation(name + "_subpillar_rot_1", 90., 45., -90.+angl);
		TGeoCombiTrans* subTrRot1 = new TGeoCombiTrans(name + "_subpillar_posrot_1", -(-x/2.+t-t/(2.*sqrt(2.))), 1., posZ, subRot1);

		TGeoRotation* subRot2 = new TGeoRotation(name + "_subpillar_rot_2", 90., -90.-45., -90.+angl);
		TGeoCombiTrans* subTrRot2 = new TGeoCombiTrans(name + "_subpillar_posrot_2", -(-x/2.+t-t/(2.*sqrt(2.))), -1., posZ, subRot2);

		// other side of X direction
		TGeoRotation* subRot3 = new TGeoRotation(name + "_subpillar_rot_3", 90., 90.+45., -90.+angl);
		TGeoCombiTrans* subTrRot3 = new TGeoCombiTrans(name + "_subpillar_posrot_3", -x/2.+t-t/(2.*sqrt(2.)), 1., posZ, subRot3);

		TGeoRotation* subRot4 = new TGeoRotation(name + "_subpillar_rot_4", 90., -45., -90.+angl);
		TGeoCombiTrans* subTrRot4 = new TGeoCombiTrans(name + "_subpillar_posrot_4", -x/2.+t-t/(2.*sqrt(2.)), -1., posZ, subRot4);

		frameBoxVol->AddNode(frameSubPillarVol, 1, subTrRot1);
		frameBoxVol->AddNode(frameSubPillarVol, 2, subTrRot2);
		frameBoxVol->AddNode(frameSubPillarVol, 3, subTrRot3);
		frameBoxVol->AddNode(frameSubPillarVol, 4, subTrRot4);
		//                frameBoxVol->GetShape()->ComputeBBox();
	}

	return frameBoxVol;
}
/** ======================================================================= **/

/** ======================================================================= **/
TGeoVolume* ConstructSmallCone(Double_t coneDz)
{
	// --- Material of the frames
	TGeoMedium* framesMaterial = gGeoMan->GetMedium("carbon");

	// --- Outer cone
//	TGeoConeSeg* A = new TGeoConeSeg ("A", coneDz, 6., 7.6, 6., 6.04, 0., 180.);
//	TGeoBBox* B = new TGeoBBox ("B", 8., 6., 10.);

	Double_t radius    = 3.0;
	Double_t thickness = 0.04; // 0.4 mm
//	TGeoConeSeg* A = new TGeoConeSeg ("A", coneDz, 3., 3.2, 3., 3.2, 0., 180.);
	TGeoConeSeg* A = new TGeoConeSeg ("A", coneDz, radius, radius+thickness, radius, radius+thickness, 0., 180.);
	TGeoBBox* B = new TGeoBBox ("B", 8., 6., 10.);

	TGeoCombiTrans* M = new TGeoCombiTrans ("M");
	M->RotateX (45.);
	M->SetDy (-5.575);
	M->SetDz (6.935);
	M->RegisterYourself();

	TGeoShape* coneShp = new TGeoCompositeShape ("Cone_shp", "A-B:M");
	TGeoVolume* coneVol = new TGeoVolume ("Cone", coneShp, framesMaterial);
	coneVol->SetLineColor(kGreen);
//	coneVol->RegisterYourself();

//	// --- Inner cone
//	Double_t thickness = 0.02;
//	Double_t thickness2 = 0.022;
//	//	TGeoConeSeg* A2 = new TGeoConeSeg ("A2", coneDz-thickness, 6.+thickness, 7.6-thickness2, 5.99+thickness, 6.05-thickness2, 0., 180.);
//	TGeoConeSeg* A2 = new TGeoConeSeg ("A2", coneDz-thickness, 3.+thickness, 4.6-thickness2, 2.99+thickness, 3.05-thickness2, 0., 180.);
//
//	TGeoCombiTrans* M2 = new TGeoCombiTrans ("M2");
//	M2->RotateX (45.);
//	M2->SetDy (-5.575+thickness*sqrt(2.));
//	M2->SetDz (6.935);
//	M2->RegisterYourself();
//
//	TGeoShape* coneShp2 = new TGeoCompositeShape ("Cone2_shp", "A2-B:M2");
//	TGeoVolume* coneVol2 = new TGeoVolume ("Cone2", coneShp2, gStsMedium);
//	coneVol2->SetLineColor(kGreen);
////	coneVol2->RegisterYourself();
//
//	coneVol->AddNode(coneVol2, 1);

	return coneVol;
}
/** ======================================================================= **/

/** ======================================================================= **/
TGeoVolume* ConstructBigCone(Double_t coneDz)
{
	// --- Material of the frames
	TGeoMedium* framesMaterial = gGeoMan->GetMedium("carbon");

	// --- Outer cone
	TGeoConeSeg* bA = new TGeoConeSeg ("bA", coneDz, 6., 7.6, 6., 6.04, 0., 180.);
	TGeoBBox* bB = new TGeoBBox ("bB", 8., 6., 10.);

	TGeoCombiTrans* bM = new TGeoCombiTrans ("bM");
	bM->RotateX (45.);
	bM->SetDy (-5.575);
	bM->SetDz (6.935);
	bM->RegisterYourself();

	TGeoShape* coneBigShp = new TGeoCompositeShape ("ConeBig_shp", "bA-bB:bM");
	TGeoVolume* coneBigVol = new TGeoVolume ("ConeBig", coneBigShp, framesMaterial);
	coneBigVol->SetLineColor(kGreen);
//	coneBigVol->RegisterYourself();

	// --- Inner cone
	Double_t thickness = 0.02;
	Double_t thickness2 = 0.022;
	TGeoConeSeg* bA2 = new TGeoConeSeg ("bA2", coneDz-thickness, 6.+thickness, 7.6-thickness2, 5.99+thickness, 6.05-thickness2, 0., 180.);

	TGeoCombiTrans* bM2 = new TGeoCombiTrans ("bM2");
	bM2->RotateX (45.);
	bM2->SetDy (-5.575+thickness*sqrt(2.));
	bM2->SetDz (6.935);
	bM2->RegisterYourself();

	TGeoShape* coneBigShp2 = new TGeoCompositeShape ("ConeBig2_shp", "bA2-bB:bM2");
	TGeoVolume* coneBigVol2 = new TGeoVolume ("ConeBig2", coneBigShp2, gStsMedium);
	coneBigVol2->SetLineColor(kGreen);
//	coneBigVol2->RegisterYourself();

	coneBigVol->AddNode(coneBigVol2, 1);

	return coneBigVol;
}
/** ======================================================================= **/
