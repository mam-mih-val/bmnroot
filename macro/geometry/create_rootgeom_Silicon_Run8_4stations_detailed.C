/*
 * Baranov D.
 * 20.07.2021
 * Use this macro to create ROOT geometry for the RUN-8 config. of SILICON
 * detector including 4 Si-Stations with PASSIVE FRAMES!!!
 *
 * WARNING: all units is in cm!!!
 *
 */

using namespace TMath;

//Set Parameters of SILICON detector -------------------------------------------
const Int_t NStations = 4;      //stations in the detector
const Int_t NMaxModules = 22;    //max. number of modules in a station

const Int_t NModulesInStations[NStations] = {10, 14, 18, 22};

//(X-Y-Z)Positions of stations (sensitive volumes)

const Double_t XStationPositions[NStations] = { 0.0, 0.0, 0.0 };
const Double_t YStationPositions[NStations] = { 0.0, 0.0, 0.0 };
//const Double_t ZStationPositions[NStations] = { +12.37, +12.37+8.64, +12.37+8.64+8.64 }; // something old
const Double_t ZStationPositions[NStations] = { +15.7, +15.7+8.64, +15.7+8.64+8.64, +15.7+8.64+8.64+8.64 };

/*
 * where
 * (1)
 *
 * (2)
 */

//(X-Y-Z)Shifts of modules in each station
const Double_t XModuleShifts[NStations][NMaxModules] = {
    {+15.15/*0*/, +9.15/*1*/, +3.15/*2*/, -9.15/*3*/, -15.15/*4*/, +15.15/*5*/, +9.15/*6*/, +3.15/*7*/, -9.15/*8*/, -15.15/*9*/},
    {+21.15/*0*/, +15.15/*1*/, +9.15/*2*/, +3.15/*3*/, -9.15/*4*/, -15.15/*5*/, -21.15/*6*/, +21.15/*7*/, +15.15/*8*/, +9.15/*9*/, +3.15/*10*/, -9.15/*11*/, -15.15/*12*/, -21.15/*13*/},
    {+27.15/*0*/, +21.15/*1*/, +15.15/*2*/, +9.15/*3*/, +3.15/*4*/, -9.15/*5*/, -15.15/*6*/, -21.15/*7*/, -27.15/*8*/, +27.15/*9*/, +21.15/*10*/, +15.15/*11*/, +9.15/*12*/, +3.15/*13*/, -9.15/*14*/, -15.15/*15*/, -21.15/*16*/, -27.15/*17*/},
    {+33.15/*0*/, +27.15/*1*/, +21.15/*2*/, +15.15/*3*/, +9.15/*4*/, +3.15/*5*/, -9.15/*6*/, -15.15/*7*/, -21.15/*8*/, -27.15/*9*/, -33.15/*10*/, +33.15/*11*/, +27.15/*12*/, +21.15/*13*/, +15.15/*14*/, +9.15/*15*/, +3.15/*16*/, -9.15/*17*/, -15.15/*18*/, -21.15/*19*/, -27.15/*20*/, -33.15/*21*/}
};
const Double_t YModuleShifts[NStations][NMaxModules] = {
    {-0.15/*0*/, -0.15/*1*/, +2.85/*2*/, -0.15/*3*/, -0.15/*4*/, +0.15/*5*/, +0.15/*6*/, -2.85/*7*/, +0.15/*8*/, +0.15/*9*/},
    {-0.15/*0*/, -0.15/*1*/, -0.15/*2*/, +2.85/*3*/, -0.15/*4*/, -0.15/*5*/, -0.15/*6*/, +0.15/*7*/, +0.15/*8*/, +0.15/*9*/, -2.85/*10*/, +0.15/*11*/, +0.15/*12*/, +0.15/*13*/},
    {-0.15/*0*/, -0.15/*1*/, -0.15/*2*/, -0.15/*3*/, +2.85/*4*/, -0.15/*5*/, -0.15/*6*/, -0.15/*7*/, -0.15/*8*/, +0.15/*9*/, +0.15/*10*/, +0.15/*11*/, +0.15/*12*/, -2.85/*13*/, +0.15/*14*/, +0.15/*15*/, +0.15/*16*/, +0.15/*17*/},
    {-0.15/*0*/, -0.15/*1*/, -0.15/*2*/, -0.15/*3*/, -0.15/*4*/, +2.85/*5*/, -0.15/*6*/, -0.15/*7*/, -0.15/*8*/, -0.15/*9*/, -0.15/*10*/, +0.15/*11*/, +0.15/*12*/, +0.15/*13*/, +0.15/*14*/, +0.15/*15*/, -2.85/*16*/, +0.15/*17*/, +0.15/*18*/, +0.15/*19*/, +0.15/*20*/, +0.15/*21*/}
};
const Double_t ZModuleShifts[NStations][NMaxModules] = {
    {+1.36+4.31/*0*/, +0.14+4.31/*1*/, +1.36+4.31/*2*/, +0.0+4.31/*3*/, +1.22+4.31/*4*/, +0.0/*5*/, +1.22/*6*/, 0.0/*7*/, +1.36/*8*/, +0.14/*9*/},
    {+0.14+4.31/*0*/, +1.36+4.31/*1*/, +0.14+4.31/*2*/, +1.36+4.31/*3*/, +0.0+4.31/*4*/, +1.22+4.31/*5*/, +0.0+4.31/*6*/, +1.22/*7*/, +0.0/*8*/, +1.22/*9*/, +0.0/*10*/, +1.36/*11*/, +0.14/*12*/, +1.36/*13*/},
    {+1.36+4.31/*0*/, +0.14+4.31/*1*/, +1.36+4.31/*2*/, +0.14+4.31/*3*/, +1.36+4.31/*4*/, +0.0+4.31/*5*/, +1.22+4.31/*6*/, +0.0+4.31/*7*/, +1.22+4.31/*8*/, +0.0/*9*/, +1.22/*10*/, +0.0/*11*/, +1.22/*12*/, +0.0/*13*/, +1.36/*14*/, +0.14/*15*/, +1.36/*16*/, +0.14/*17*/},
    {+0.14+4.31/*0*/, +1.36+4.31/*1*/, +0.14+4.31/*2*/, +1.36+4.31/*3*/, +0.14+4.31/*4*/, +1.36+4.31/*5*/, +0.0+4.31/*6*/, +1.22+4.31/*7*/, +0.0+4.31/*8*/, +1.22+4.31/*9*/, +0.0+4.31/*10*/, +1.22/*11*/, +0.0/*12*/, +1.22/*13*/, +0.0/*14*/, +1.22/*15*/, +0.0/*16*/, +1.36/*17*/, +0.14/*18*/, +1.36/*19*/, +0.14/*20*/, +1.36/*21*/}
};

//Geometry parameters of modules
enum ModuleType {RIGHT_ANGLED_MODULE, BEVEL_MODULE, RIGHT_ANGLED_MODULE_HALF, BEVEL_MODULE_HALF};

const ModuleType ModulesTypes[NStations][NMaxModules] = {
    {RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE},
    {RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE},
    {RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE},
    {RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE}
};

//rotations of modules around y-axis by 180 deg.
const Bool_t ModulesYRotations[NStations][NMaxModules] = {
    {true/*0*/, true/*1*/, true/*2*/, false/*3*/, false/*4*/, true/*5*/, true/*6*/, true/*7*/, false/*8*/, false/*9*/},
    {true/*0*/, true/*1*/, true/*2*/, true/*3*/, false/*4*/, false/*5*/, false/*6*/, true/*7*/, true/*8*/, true/*9*/, true/*10*/, false/*11*/, false/*12*/, false/*13*/},
    {true/*0*/, true/*1*/, true/*2*/, true/*3*/, true/*4*/, false/*5*/, false/*6*/, false/*7*/, false/*8*/, true/*9*/, true/*10*/, true/*11*/, true/*12*/, true/*13*/, false/*14*/, false/*15*/, false/*16*/, false/*17*/},
    {true/*0*/, true/*1*/, true/*2*/, true/*3*/, true/*4*/, true/*5*/, false/*6*/, false/*7*/, false/*8*/, false/*9*/, false/*10*/, true/*11*/, true/*12*/, true/*13*/, true/*14*/, true/*15*/, true/*16*/, false/*17*/, false/*18*/, false/*19*/, false/*20*/, false/*21*/}

};

//rotations of modules around x-axis by 180 deg.
const Bool_t ModulesXRotations[NStations][NMaxModules] = {
    {false/*0*/, false/*1*/, false/*2*/, false/*3*/, false/*4*/, true/*5*/, true/*6*/, true/*7*/, true/*8*/, true/*9*/},
    {false/*0*/, false/*1*/, false/*2*/, false/*3*/, false/*4*/, false/*5*/, false/*6*/, true/*7*/, true/*8*/, true/*9*/, true/*10*/, true/*11*/, true/*12*/, true/*13*/},
    {false/*0*/, false/*1*/, false/*2*/, false/*3*/, false/*4*/, false/*5*/, false/*6*/, false/*7*/, false/*8*/, true/*9*/, true/*10*/, true/*11*/, true/*12*/, true/*13*/, true/*14*/, true/*15*/, true/*16*/, true/*17*/},
    {false/*0*/, false/*1*/, false/*2*/, false/*3*/, false/*4*/, false/*5*/, false/*6*/, false/*7*/, false/*8*/, false/*9*/, false/*10*/, true/*11*/, true/*12*/, true/*13*/, true/*14*/, true/*15*/, true/*16*/, true/*17*/, true/*18*/, true/*19*/, true/*20*/, true/*21*/}
};


//GeoManager
TGeoManager* gGeoMan = 0;

//Module parameters ------------------------------------------------------------
enum ModuleSize {STANDARD_MODULE, LONG_MODULE}; //STANDARD_MODULE is (63x126)mm, LONG_MODULE is (63x186)mm
const unsigned int NModuleSizes = 2;

Double_t YGapBetweenPartsOfModule[NModuleSizes] = { 0.0035/*STANDARD_MODULE*/, 0.0035/*LONG_MODULE*/ }; //Mrs. Slit

Double_t XModuleSize[NModuleSizes] = { 6.3/*STANDARD_MODULE*/, 6.3/*LONG_MODULE*/ };

Double_t YModuleSizeUpperPart[NModuleSizes] = { 6.3/*STANDARD_MODULE*/, 9.3/*LONG_MODULE*/ };
Double_t YModuleSizeLowerPart[NModuleSizes] = { 6.3/*STANDARD_MODULE*/, 9.3/*LONG_MODULE*/ };

Double_t YModuleSize[NModuleSizes] = {
    YModuleSizeUpperPart[STANDARD_MODULE] + YModuleSizeLowerPart[STANDARD_MODULE] + YGapBetweenPartsOfModule[STANDARD_MODULE], /*STANDARD_MODULE*/
    YModuleSizeUpperPart[LONG_MODULE] + YModuleSizeLowerPart[LONG_MODULE] + YGapBetweenPartsOfModule[LONG_MODULE] /*LONG_MODULE*/
};

Double_t ZModuleSize[NModuleSizes] = { 0.03/*STANDARD_MODULE*/, 0.03/*LONG_MODULE*/ };

Double_t XBevelSize[NModuleSizes] = { 3.82/*STANDARD_MODULE*/, 3.82/*LONG_MODULE*/ }; //x-bevel for a angled cut
Double_t YBevelSize[NModuleSizes] = { 3.82/*STANDARD_MODULE*/, 3.82/*LONG_MODULE*/ }; //y-bevel for a angled cut

Double_t BevelAngleRad[NModuleSizes] = {
    TMath::ATan(XBevelSize[STANDARD_MODULE]/YBevelSize[STANDARD_MODULE]), /*STANDARD_MODULE*/
    TMath::ATan(XBevelSize[LONG_MODULE]/YBevelSize[LONG_MODULE]) /*LONG_MODULE*/
};

Double_t BevelAngleDeg[NModuleSizes] = {
    BevelAngleRad[STANDARD_MODULE]*TMath::RadToDeg(), /*STANDARD_MODULE*/
    BevelAngleRad[LONG_MODULE]*TMath::RadToDeg() /*LONG_MODULE*/
};

Double_t XLeftBorder[NModuleSizes] = { 0.1145/*STANDARD_MODULE*/, 0.1118/*LONG_MODULE*/ };
Double_t XRightBorder[NModuleSizes] = { 0.1118/*STANDARD_MODULE*/, 0.1118/*LONG_MODULE*/ };
Double_t YTopBorder[NModuleSizes] = { 0.1143/*STANDARD_MODULE*/, 0.1148/*LONG_MODULE*/ };
Double_t YMiddleBorderUpperPart[NModuleSizes] = { 0.1121/*STANDARD_MODULE*/, 0.1148/*LONG_MODULE*/ };
Double_t YMiddleBorderLowerPart[NModuleSizes] = { 0.1147/*STANDARD_MODULE*/, 0.1148/*LONG_MODULE*/ };
Double_t YBottomBorder[NModuleSizes] = { 0.1148/*STANDARD_MODULE*/, 0.1148/*LONG_MODULE*/ };
Double_t XYBevelBorder[NModuleSizes] = { 0.1146/*STANDARD_MODULE*/, 0.1146/*LONG_MODULE*/ };

//Sensor zones ------------------------------------
Double_t XSensorZoneSize[NModuleSizes] = {
    XModuleSize[STANDARD_MODULE] - (XLeftBorder[STANDARD_MODULE] + XRightBorder[STANDARD_MODULE]),/*STANDARD_MODULE*/
    XModuleSize[LONG_MODULE] - (XLeftBorder[LONG_MODULE] + XRightBorder[LONG_MODULE]) /*LONG_MODULE*/
};

Double_t YSensorZoneSizeUpperPart[NModuleSizes] = {
    YModuleSizeUpperPart[STANDARD_MODULE] - (YTopBorder[STANDARD_MODULE] + YMiddleBorderUpperPart[STANDARD_MODULE]), /*STANDARD_MODULE*/
    YModuleSizeUpperPart[LONG_MODULE] - (YTopBorder[LONG_MODULE] + YMiddleBorderUpperPart[LONG_MODULE]) /*LONG_MODULE*/
};

Double_t YSensorZoneSizeLowerPart[NModuleSizes] = {
    YModuleSizeLowerPart[STANDARD_MODULE] - (YBottomBorder[STANDARD_MODULE] + YMiddleBorderLowerPart[STANDARD_MODULE]), /*STANDARD_MODULE*/
    YModuleSizeLowerPart[LONG_MODULE] - (YBottomBorder[LONG_MODULE] + YMiddleBorderLowerPart[LONG_MODULE]) /*LONG_MODULE*/
};

Double_t ZSensorZoneSize[NModuleSizes] = {
    ZModuleSize[STANDARD_MODULE], /*STANDARD_MODULE*/
    ZModuleSize[LONG_MODULE] /*LONG_MODULE*/
};
//-------------------------------------------------

//Module frames -----------------------------------
Double_t XModuleFrameSize[NModuleSizes] = {
    XModuleSize[STANDARD_MODULE], /*STANDARD_MODULE*/
    XModuleSize[LONG_MODULE] /*LONG_MODULE*/
};

Double_t YModuleFrameSize[NModuleSizes] = {
    YModuleSize[STANDARD_MODULE], /*STANDARD_MODULE*/
    YModuleSize[LONG_MODULE] /*LONG_MODULE*/
};

Double_t ZModuleFrameSize[NModuleSizes] = { 0.18/*STANDARD_MODULE*/, 0.18/*LONG_MODULE*/ };
//-------------------------------------------------

//Y-shift from center of the gap to the global x-axis (for y-alignment of the module)
Double_t YShiftFromCenterToAxis[NModuleSizes] = {
    (YModuleSizeUpperPart[STANDARD_MODULE]-YModuleSizeLowerPart[STANDARD_MODULE])*0.5, /*STANDARD_MODULE*/
    (YModuleSizeUpperPart[LONG_MODULE]-YModuleSizeLowerPart[LONG_MODULE])*0.5 /*LONG_MODULE*/
};

//X-shift from center of the module to the origin of the local x-axis
Double_t XShiftFromCenterToLocalX0[NModuleSizes] = {
    +XModuleSize[STANDARD_MODULE]*0.5, /*STANDARD_MODULE*/
    +XModuleSize[LONG_MODULE]*0.5 /*LONG_MODULE*/
};

//Y-shift from center of the module to the origin of the local y-axis
Double_t YShiftFromCenterToLocalY0[NModuleSizes] = {
    +(YModuleSizeLowerPart[STANDARD_MODULE] + YGapBetweenPartsOfModule[STANDARD_MODULE]*0.5), /*STANDARD_MODULE*/
    +(YModuleSizeLowerPart[LONG_MODULE] + YGapBetweenPartsOfModule[LONG_MODULE]*0.5) /*LONG_MODULE*/
};
//------------------------------------------------------------------------------

/*
     ASCII module picture (view from target)

        |<---- XModuleSize ---------->|                     ^
        |                             |                     | Y-axis
        ------------------------------  ---------
        |   |    top border       |   |         ^
        ------------------------------          |
        |   |                     |   |         |
        | l |    rectangle        | r |         |
        | e |      sensor         | i |         |
        | f |       zone          | g |         |
        | t |                     | h |         |
        |   |                     | t |         |
        |   |                     |   |         |
        ------------------------------          |
        |   | middle (upper part) |   |
        ------ gap between parts -----      YModuleSize
        |   | middle (lower part) |   |
        ------------------------------          |
        |   |                     |   |         |
        | b |   bevel (or rect.)  | r |         |
        | e |      sensor         | i |         |
         \ v \      zone          | g |         |
          \ e \                   | h |         |
           \ l \                  | t |         |
            \   \                 |   |         |
             -------------------------          |
              \   \  bottom       |   |         v
                ----------------------  ---------

 *  x-axis <----------------------------------(x0,y0) local point

 */

//media
TGeoMedium *pMedAir = 0;
TGeoMedium *pMedCarbon = 0;
TGeoMedium *pMedFiberGlass = 0;
TGeoMedium *pMedCopper = 0;
TGeoMedium *pMedSilicon = 0;
TGeoMedium *pMedAluminium = 0;
TGeoMedium *pMedPolystyrene = 0;

class FairGeoMedia;
class FairGeoBuilder;

//function declarations
TGeoVolume *CreateStation(TString station_name);
TGeoVolume *CreateModule(TString module_name, ModuleType module_type, ModuleSize module_size);
TGeoVolume *CreateModuleFrame(TString module_name, ModuleType module_type, ModuleSize module_size);
TGeoVolume *CreateFrameForStation_StandardModule(TString frame_name, Int_t current_station_num);
TGeoVolume *CreateFrameForStation_LongModule(TString frame_name, Int_t current_station_num);

void DefineRequiredMedia(FairGeoMedia* geoMedia, FairGeoBuilder* geoBuild) {

    //air medium
    FairGeoMedium* mAir = geoMedia->getMedium("air");
    if ( ! mAir ) Fatal("Main", "FairMedium air not found");
    geoBuild->createMedium(mAir);
    pMedAir = gGeoManager->GetMedium("air");
    if ( ! pMedAir ) Fatal("Main", "Medium air not found");

    //carbon medium
    FairGeoMedium* mCarbon = geoMedia->getMedium("carbon");
    if ( ! mCarbon  ) Fatal("Main", "FairMedium carbon not found");
    geoBuild->createMedium(mCarbon);
    pMedCarbon = gGeoManager->GetMedium("carbon");
    if ( ! pMedCarbon ) Fatal("Main", "Medium carbon not found");

    //fiberglass medium
    FairGeoMedium* mFiberGlass = geoMedia->getMedium("fiberglass");
    if ( ! mFiberGlass  ) Fatal("Main", "FairMedium fiberglass not found");
    geoBuild->createMedium(mFiberGlass);
    pMedFiberGlass = gGeoManager->GetMedium("fiberglass");
    if ( ! pMedFiberGlass  ) Fatal("Main", "Medium fiberglass not found");

    //copper medium
    FairGeoMedium* mCopper = geoMedia->getMedium("copper");
    if ( ! mCopper  ) Fatal("Main", "FairMedium copper not found");
    geoBuild->createMedium(mCopper);
    pMedCopper= gGeoManager->GetMedium("copper");
    if ( ! pMedCopper  ) Fatal("Main", "Medium copper not found");

    //silicon medium
    FairGeoMedium* mSilicon = geoMedia->getMedium("silicon");
    if ( ! mSilicon  ) Fatal("Main", "FairMedium silicon not found");
    geoBuild->createMedium(mSilicon);
    pMedSilicon= gGeoManager->GetMedium("silicon");
    if ( ! pMedSilicon  ) Fatal("Main", "Medium silicon not found");

    //aluminium medium
    FairGeoMedium* mAluminium = geoMedia->getMedium("aluminium");
    if ( ! mAluminium  ) Fatal("Main", "FairMedium aluminium not found");
    geoBuild->createMedium(mAluminium);
    pMedAluminium= gGeoManager->GetMedium("aluminium");
    if ( ! pMedAluminium  ) Fatal("Main", "Medium aluminium not found");

    //polystyrene medium
    FairGeoMedium* mPolystyrene = geoMedia->getMedium("polystyrene");
    if ( ! mPolystyrene  ) Fatal("Main", "FairMedium polystyrene not found");
    geoBuild->createMedium(mPolystyrene);
    pMedPolystyrene= gGeoManager->GetMedium("polystyrene");
    if ( ! pMedPolystyrene  ) Fatal("Main", "Medium polystyrene not found");

}

void PrintInfoAboutModuleParameters() {
    cout << "\n";
    cout << "Module parameters information [STANDARD_MODULE]: ------------------" << "\n";
    cout << "  XModuleSize = " << XModuleSize[STANDARD_MODULE] << " cm" << "\n";
    cout << "  YModuleSize = " << YModuleSize[STANDARD_MODULE] << " cm" << " ( " << YModuleSizeUpperPart[STANDARD_MODULE] << " + " << YGapBetweenPartsOfModule[STANDARD_MODULE] << " + " << YModuleSizeLowerPart[STANDARD_MODULE] << " )"   << "\n";
    cout << "     YModuleSizeUpperPart = " << YModuleSizeUpperPart[STANDARD_MODULE] << " cm" << "\n";
    cout << "     YModuleSizeLowerPart = " << YModuleSizeLowerPart[STANDARD_MODULE] << " cm" << "\n";
    cout << "     YGapBetweenPartsOfModule = " << YGapBetweenPartsOfModule[STANDARD_MODULE] << " cm" << "\n";
    cout << "  ZModuleSize = " << ZModuleSize[STANDARD_MODULE] << " cm" << "\n";
    cout << "\n";
    cout << "  XBevelSize = " << XBevelSize[STANDARD_MODULE] << " cm" << "\n";
    cout << "  YBevelSize = " << YBevelSize[STANDARD_MODULE] << " cm" << "\n";
    cout << "     BevelAngleDeg = " << BevelAngleDeg[STANDARD_MODULE] << " deg" << "\n";
    cout << "\n";
    cout << "  XLeftBorder = " << XLeftBorder[STANDARD_MODULE] << " cm" << "\n";
    cout << "  XRightBorder = " << XRightBorder[STANDARD_MODULE] << " cm" << "\n";
    cout << "  YTopBorder = " << YTopBorder[STANDARD_MODULE] << " cm" << "\n";
    cout << "  YMiddleBorderUpperPart = " << YMiddleBorderUpperPart[STANDARD_MODULE] << " cm" << "\n";
    cout << "  YGapBetweenPartsOfModule = " << YGapBetweenPartsOfModule[STANDARD_MODULE] << " cm" << " (gap between two parts)\n";
    cout << "  YMiddleBorderLowerPart = " << YMiddleBorderLowerPart[STANDARD_MODULE] << " cm" << "\n";
    cout << "  YBottomBorder = " << YBottomBorder[STANDARD_MODULE] << " cm" << "\n";
    cout << "  XYBevelBorder = " << XYBevelBorder[STANDARD_MODULE] << " cm" << "\n";
    cout << "\n";
    cout << "  XSensorZoneSize = " << XSensorZoneSize[STANDARD_MODULE] << " cm" << "\n";
    cout << "  YSensorZoneSizeUpperPart = " << YSensorZoneSizeUpperPart[STANDARD_MODULE] << " cm" << "\n";
    cout << "  YSensorZoneSizeLowerPart = " << YSensorZoneSizeLowerPart[STANDARD_MODULE] << " cm" << "\n";
    cout << "  ZSensorZoneSize = " << ZSensorZoneSize[STANDARD_MODULE] << " cm" << "\n";

    cout << "-------------------------------------------------------------\n";

    cout << "\n";
    cout << "Module parameters information [LONG_MODULE]: ------------------" << "\n";
    cout << "  XModuleSize = " << XModuleSize[LONG_MODULE] << " cm" << "\n";
    cout << "  YModuleSize = " << YModuleSize[LONG_MODULE] << " cm" << " ( " << YModuleSizeUpperPart[LONG_MODULE] << " + " << YGapBetweenPartsOfModule[LONG_MODULE] << " + " << YModuleSizeLowerPart[LONG_MODULE] << " )"   << "\n";
    cout << "     YModuleSizeUpperPart = " << YModuleSizeUpperPart[LONG_MODULE] << " cm" << "\n";
    cout << "     YModuleSizeLowerPart = " << YModuleSizeLowerPart[LONG_MODULE] << " cm" << "\n";
    cout << "     YGapBetweenPartsOfModule = " << YGapBetweenPartsOfModule[LONG_MODULE] << " cm" << "\n";
    cout << "  ZModuleSize = " << ZModuleSize[LONG_MODULE] << " cm" << "\n";
    cout << "\n";
    cout << "  XBevelSize = " << XBevelSize[LONG_MODULE] << " cm" << "\n";
    cout << "  YBevelSize = " << YBevelSize[LONG_MODULE] << " cm" << "\n";
    cout << "     BevelAngleDeg = " << BevelAngleDeg[LONG_MODULE] << " deg" << "\n";
    cout << "\n";
    cout << "  XLeftBorder = " << XLeftBorder[LONG_MODULE] << " cm" << "\n";
    cout << "  XRightBorder = " << XRightBorder[LONG_MODULE] << " cm" << "\n";
    cout << "  YTopBorder = " << YTopBorder[LONG_MODULE] << " cm" << "\n";
    cout << "  YMiddleBorderUpperPart = " << YMiddleBorderUpperPart[LONG_MODULE] << " cm" << "\n";
    cout << "  YGapBetweenPartsOfModule = " << YGapBetweenPartsOfModule[LONG_MODULE] << " cm" << " (gap between two parts)\n";
    cout << "  YMiddleBorderLowerPart = " << YMiddleBorderLowerPart[LONG_MODULE] << " cm" << "\n";
    cout << "  YBottomBorder = " << YBottomBorder[LONG_MODULE] << " cm" << "\n";
    cout << "  XYBevelBorder = " << XYBevelBorder[LONG_MODULE] << " cm" << "\n";
    cout << "\n";
    cout << "  XSensorZoneSize = " << XSensorZoneSize[LONG_MODULE] << " cm" << "\n";
    cout << "  YSensorZoneSizeUpperPart = " << YSensorZoneSizeUpperPart[LONG_MODULE] << " cm" << "\n";
    cout << "  YSensorZoneSizeLowerPart = " << YSensorZoneSizeLowerPart[LONG_MODULE] << " cm" << "\n";
    cout << "  ZSensorZoneSize = " << ZSensorZoneSize[LONG_MODULE] << " cm" << "\n";

    cout << "-------------------------------------------------------------\n";
}

void PrintInformationAboutStations() {
    cout << "\n";
    for(Int_t istation = 0; istation < NStations; ++istation) {
        cout << "station " << istation << ":\n";
        cout << "   XStationPositions[" << istation << "] = " << XStationPositions[istation] << " cm\n";
        cout << "   YStationPositions[" << istation << "] = " << YStationPositions[istation] << " cm\n";
        cout << "   ZStationPositions[" << istation << "] = " << ZStationPositions[istation] << " cm\n";
        cout << "\n";

        for(Int_t imodule = 0; imodule < NModulesInStations[istation]; ++imodule) {
            cout << "   module " << imodule << ":\n";
            cout << "      XModuleShifts[" << istation << "][" << imodule << "] = " << XModuleShifts[istation][imodule] << " cm\n";
            cout << "      YModuleShifts[" << istation << "][" << imodule << "] = " << YModuleShifts[istation][imodule] << " cm\n";
            cout << "      ZModuleShifts[" << istation << "][" << imodule << "] = " << ZModuleShifts[istation][imodule] << " cm\n";
            cout << "\n";

            Double_t XGlobalModulePos = XModuleShifts[istation][imodule] + XStationPositions[istation];
            Double_t YGlobalModulePos = YModuleShifts[istation][imodule] + YStationPositions[istation];
            Double_t ZGlobalModulePos = ZModuleShifts[istation][imodule] + ZStationPositions[istation];

            cout << "      XGlobalModulePos = " << XGlobalModulePos << " cm\n";
            cout << "      YGlobalModulePos = " << YGlobalModulePos << " cm\n";
            cout << "      ZGlobalModulePos = " << ZGlobalModulePos << " cm\n";
            cout << "\n";

            cout << "      ModulesTypes[" << istation << "][" << imodule << "] = ";
                if(ModulesTypes[istation][imodule] == 0) cout << "RIGHT_ANGLED_MODULE\n";
                if(ModulesTypes[istation][imodule] == 1) cout << "BEVEL_MODULE\n";
                if(ModulesTypes[istation][imodule] == 2) cout << "RIGHT_ANGLED_MODULE_HALF\n";
                if(ModulesTypes[istation][imodule] == 3) cout << "BEVEL_MODULE_HALF\n";
            cout << "      ModulesYRotations[" << istation << "][" << imodule << "] = ";
                if(ModulesYRotations[istation][imodule] == false) cout << "not rotated around Y\n";
                if(ModulesYRotations[istation][imodule] == true) cout << "rotated around Y\n";
            cout << "      ModulesXRotations[" << istation << "][" << imodule << "] = ";
                if(ModulesXRotations[istation][imodule] == false) cout << "not rotated around X\n";
                if(ModulesXRotations[istation][imodule] == true) cout << "rotated around X\n";
            cout << "\n";
        }
        cout << "-------------------------------------------------------------\n";
    }
    cout << "-------------------------------------------------------------\n";
}

//create geometry of silicon detector with passive frames
void create_rootgeom_Silicon_Run8_4stations_detailed() {

    // ----  set working directory  --------------------------------------------
    TString gPath = gSystem->Getenv("VMCWORKDIR");

    // -------   Geometry file name (output)   ----------------------------------
    const TString geoDetectorName = "Silicon";
    const TString geoDetectorVersion = "Run8_4stations_detailed";
    const TString geoFileName = gPath + "/geometry/" + geoDetectorName + "_"+ geoDetectorVersion + ".root";

    // ----  global geometry parameters  ---------------------------------------
    FairGeoLoader*    geoLoad = new FairGeoLoader("TGeo","FairGeoLoader");
    FairGeoInterface* geoFace = geoLoad->getGeoInterface();

    // -------   Load media from media file   ----------------------------------
    TString medFile = gPath + "/geometry/media.geo";
    geoFace->setMediaFile(medFile);
    geoFace->readMedia();
    FairGeoMedia*   geoMedia = geoFace->getMedia();
    FairGeoBuilder* geoBuild = geoLoad->getGeoBuilder();

    DefineRequiredMedia(geoMedia, geoBuild);
    // -------------------------------------------------------------------------

    //Print information about parameters of SILICON
    PrintInfoAboutModuleParameters();
    PrintInformationAboutStations();

    // --------------   Create geometry and top volume  -------------------------
    gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");
    gGeoMan->SetName(geoDetectorName + "_geom");
    TGeoVolume* top = new TGeoVolumeAssembly("TOP");
    top->SetMedium(pMedAir);
    gGeoMan->SetTopVolume(top);
    //gGeoMan->SetTopVisible(1);
    // --------------------------------------------------------------------------

    // Define TOP Geometry
    TGeoVolume* SILICON = new TGeoVolumeAssembly(geoDetectorName);
    SILICON->SetMedium(pMedAir);

    //Let's create SILICON stations filled with modules
    for(Int_t istation = 0; istation < NStations; ++istation) {
    //for(Int_t istation = 3; istation < 4; ++istation) {

        //module size in a station
        ModuleSize module_size = STANDARD_MODULE;

        //(!!!) the third station has long modules (!!!)
        if(istation == 3) module_size = LONG_MODULE;

        TGeoVolume *station = CreateStation(TString("station")+ TString::Itoa(istation, 10));

        Int_t NModules = NModulesInStations[istation];

        TGeoVolume *modules[NModules];
        TGeoVolume *moduleFrames[NModules];

        TGeoCombiTrans *modTransforms[NModules];

        for(Int_t imodule = 0; imodule < NModules; ++imodule) {

            TString module_name = TString("module") + TString::Itoa(imodule, 10) + TString("_") + station->GetName();
            modules[imodule] = CreateModule(module_name, ModulesTypes[istation][imodule], module_size);

            TString moduleFrame_name = TString("moduleFrame") + TString::Itoa(imodule, 10) + TString("_") + station->GetName();
            moduleFrames[imodule] = CreateModuleFrame(moduleFrame_name, ModulesTypes[istation][imodule], module_size);

            modTransforms[imodule] = new TGeoCombiTrans();

            if( ModulesYRotations[istation][imodule] ) {
                modTransforms[imodule]->RotateY(180.0);
                modTransforms[imodule]->SetDx(XModuleShifts[istation][imodule] - XShiftFromCenterToLocalX0[module_size]);
            }
            else {
                modTransforms[imodule]->SetDx(XModuleShifts[istation][imodule] + XShiftFromCenterToLocalX0[module_size]);
            }

            if( ModulesXRotations[istation][imodule] ) {
                modTransforms[imodule]->RotateX(180.0);
                if(ModulesTypes[istation][imodule] == ModuleType::BEVEL_MODULE_HALF || ModulesTypes[istation][imodule] == ModuleType::RIGHT_ANGLED_MODULE_HALF) {
                    modTransforms[imodule]->SetDy(YModuleShifts[istation][imodule] - YModuleSizeUpperPart[module_size]*0.5);
                }
                else {
                    modTransforms[imodule]->SetDy(YModuleShifts[istation][imodule] + YShiftFromCenterToAxis[module_size] - YShiftFromCenterToLocalY0[module_size]);
                }
            }
            else {
                if(ModulesTypes[istation][imodule] == ModuleType::BEVEL_MODULE_HALF || ModulesTypes[istation][imodule] == ModuleType::RIGHT_ANGLED_MODULE_HALF) {
                    modTransforms[imodule]->SetDy(YModuleShifts[istation][imodule] + YModuleSizeUpperPart[module_size]*0.5);
                }
                else {
                    modTransforms[imodule]->SetDy(YModuleShifts[istation][imodule] + YShiftFromCenterToAxis[module_size] + YShiftFromCenterToLocalY0[module_size]);
                }
            }

            Double_t z_module_shift = +ZModuleSize[module_size]*0.5; //To z-module position is at the beginning of the sensitive volume
            modTransforms[imodule]->SetDz(ZModuleShifts[istation][imodule] + z_module_shift);

            TGeoCombiTrans *modFrameTransform = new TGeoCombiTrans(*modTransforms[imodule]);

            if(  ModulesYRotations[istation][imodule] ^ ModulesXRotations[istation][imodule] ) {
                modFrameTransform->SetDz(modFrameTransform->GetTranslation()[2] - (ZModuleSize[module_size]*0.5 + ZModuleFrameSize[module_size]*0.5));
            }
            else {
                modFrameTransform->SetDz(modFrameTransform->GetTranslation()[2] + (ZModuleSize[module_size]*0.5 + ZModuleFrameSize[module_size]*0.5));
            }

            //modFrameTransform->SetDz(modFrameTransform->GetTranslation()[2] + 1.0);

            //if(imodule == 1) station->AddNode(modules[imodule], 0, modTransforms[imodule]);
            station->AddNode(modules[imodule], 0, modTransforms[imodule]);
            station->AddNode(moduleFrames[imodule], 0, modFrameTransform);
        }

        //frames ---------------------------------------------------------------
        TGeoVolume *frames;
        if (module_size == STANDARD_MODULE) frames= CreateFrameForStation_StandardModule(TString("frames")+=TString("_") + station->GetName(), istation);
        if (module_size == LONG_MODULE) frames= CreateFrameForStation_LongModule(TString("frames")+=TString("_") + station->GetName(), istation);
        //----------------------------------------------------------------------

        station->AddNode(frames, 0, new TGeoCombiTrans());

        //----------------------------------------------------------------------

        TGeoCombiTrans *station_transform = new TGeoCombiTrans();
        station_transform->SetTranslation(XStationPositions[istation], YStationPositions[istation], ZStationPositions[istation]);

        SILICON->AddNode(station, 0, station_transform);
    }


    top->AddNode(SILICON, 0);
    top->SetVisContainers(kTRUE);

    // ---------------   Finish   -----------------------------------------------
    gGeoMan->CloseGeometry();
    gGeoMan->CheckOverlaps(0.0001);
    gGeoMan->PrintOverlaps();
    gGeoMan->Test();

    TFile* geoFile = new TFile(geoFileName, "RECREATE");
    top->Write();
    geoFile->Close();
    top->Draw("ogl");
}
//------------------------------------------------------------------------------


TGeoVolume *CreateStation(TString station_name) {

    TGeoVolume *stationA = new TGeoVolumeAssembly(station_name);
    stationA->SetMedium(pMedAir);

    return stationA;
}

TGeoVolume *CreateModule(TString module_name, ModuleType module_type, ModuleSize module_size) {

    //Shapes -------------------------------------------------------------------
    //module shape
    TGeoShape *moduleS = 0;

    switch(module_type) {
        case RIGHT_ANGLED_MODULE: {
            moduleS = new TGeoBBox("moduleS", XModuleSize[module_size]*0.5, YModuleSize[module_size]*0.5, ZModuleSize[module_size]*0.5);
            break;
        }
        case RIGHT_ANGLED_MODULE_HALF: {
            moduleS = new TGeoBBox("moduleS", XModuleSize[module_size]*0.5, YModuleSizeUpperPart[module_size]*0.5, ZModuleSize[module_size]*0.5);
            break;
        }
        case BEVEL_MODULE: {
            moduleS = new TGeoXtru(2);
            moduleS->SetName("moduleS");
                Double_t x_pos[5] = {+XModuleSize[module_size]*0.5, -XModuleSize[module_size]*0.5, -XModuleSize[module_size]*0.5, +XModuleSize[module_size]*0.5-XBevelSize[module_size], +XModuleSize[module_size]*0.5};
                Double_t y_pos[5] = {+YModuleSize[module_size]*0.5, +YModuleSize[module_size]*0.5, -YModuleSize[module_size]*0.5, -YModuleSize[module_size]*0.5, -YModuleSize[module_size]*0.5+YBevelSize[module_size]};
            ((TGeoXtru*)moduleS)->DefinePolygon(5, x_pos, y_pos);
            ((TGeoXtru*)moduleS)->DefineSection(0, -ZModuleSize[module_size]*0.5);
            ((TGeoXtru*)moduleS)->DefineSection(1, +ZModuleSize[module_size]*0.5);
            break;
        }
        case BEVEL_MODULE_HALF: {
            moduleS = new TGeoXtru(2);
            moduleS->SetName("moduleS");
                Double_t x_pos[5] = {+XModuleSize[module_size]*0.5, -XModuleSize[module_size]*0.5, -XModuleSize[module_size]*0.5, +XModuleSize[module_size]*0.5-XBevelSize[module_size], +XModuleSize[module_size]*0.5};
                Double_t y_pos[5] = {+YModuleSizeUpperPart[module_size]*0.5, +YModuleSizeUpperPart[module_size]*0.5, -YModuleSizeUpperPart[module_size]*0.5, -YModuleSizeUpperPart[module_size]*0.5, -YModuleSizeUpperPart[module_size]*0.5+YBevelSize[module_size]};
            ((TGeoXtru*)moduleS)->DefinePolygon(5, x_pos, y_pos);
            ((TGeoXtru*)moduleS)->DefineSection(0, -ZModuleSize[module_size]*0.5);
            ((TGeoXtru*)moduleS)->DefineSection(1, +ZModuleSize[module_size]*0.5);
            break;
        }
        default:
            Error("CreateModule()", "Incorrect module type selected!!!");
    }

    //sensor zone shape (upper part of the module: rectangle zone)
    TGeoShape *sensor_zone_upper_part_rectS = new TGeoBBox("sensor_zone_upper_part_rectS", XSensorZoneSize[module_size]*0.5, YSensorZoneSizeUpperPart[module_size]*0.5, ZSensorZoneSize[module_size]*0.5);

    //sensor zone shape (upper part of the module: bevel zone)
    TGeoShape *sensor_zone_upper_part_bevelS = new TGeoXtru(2);
        sensor_zone_upper_part_bevelS->SetName("sensor_zone_upper_part_bevelS");
        //Calculate coordinates of the bottom and the top points of the inner bevel
        {
            //find x and y projections of the bevel border thickness
            Double_t x_delta = XYBevelBorder[module_size]/TMath::Cos(BevelAngleRad[module_size]);
            Double_t y_delta = XYBevelBorder[module_size]/TMath::Sin(BevelAngleRad[module_size]);

            //find shifts of the sensitive zone from module borders via the equation of the straight line for the inner bevel
            Double_t x_delta_top_BP = XLeftBorder[module_size];
            Double_t y_delta_top_BP = TMath::Tan(TMath::PiOver2()+BevelAngleRad[module_size])*x_delta_top_BP + (YBevelSize[module_size]+y_delta);

            Double_t y_delta_bottom_BP = YMiddleBorderUpperPart[module_size];
            Double_t x_delta_bottom_BP = (y_delta_bottom_BP - (YBevelSize[module_size] + y_delta))/(TMath::Tan(TMath::PiOver2()+BevelAngleRad[module_size]));
            //----------------------------------------------------------------------

            Double_t x_pos_sensor_zone_bevel[5] = { +XModuleSize[module_size]*0.5-XLeftBorder[module_size], -XModuleSize[module_size]*0.5+XRightBorder[module_size], -XModuleSize[module_size]*0.5+XRightBorder[module_size],       +XModuleSize[module_size]*0.5-x_delta_bottom_BP, +XModuleSize[module_size]*0.5-XLeftBorder[module_size]};
            Double_t y_pos_sensor_zone_bevel[5] = { +YModuleSizeUpperPart[module_size]*0.5-YTopBorder[module_size], +YModuleSizeUpperPart[module_size]*0.5-YTopBorder[module_size], -YModuleSizeUpperPart[module_size]*0.5+YMiddleBorderUpperPart[module_size],      -YModuleSizeUpperPart[module_size]*0.5+YMiddleBorderUpperPart[module_size], -YModuleSizeUpperPart[module_size]*0.5+y_delta_top_BP};

            ((TGeoXtru*)sensor_zone_upper_part_bevelS)->DefinePolygon(5, x_pos_sensor_zone_bevel, y_pos_sensor_zone_bevel);
            ((TGeoXtru*)sensor_zone_upper_part_bevelS)->DefineSection(0, -ZModuleSize[module_size]*0.5);
            ((TGeoXtru*)sensor_zone_upper_part_bevelS)->DefineSection(1, +ZModuleSize[module_size]*0.5);
        }


    //sensor zone shape (lower part of the module: rectangle zone)
    TGeoShape *sensor_zone_lower_part_rectS = new TGeoBBox("sensor_zone_lower_part_rectS", XSensorZoneSize[module_size]*0.5, YSensorZoneSizeLowerPart[module_size]*0.5, ZSensorZoneSize[module_size]*0.5);

    //sensor zone shape (lower part of the module: bevel zone)
    TGeoShape *sensor_zone_lower_part_bevelS = new TGeoXtru(2);
        sensor_zone_lower_part_bevelS->SetName("sensor_zone_lower_part_bevelS");
        //Calculate coordinates of the bottom and the top points of the inner bevel
        {
            //find x and y projections of the bevel border thickness
            Double_t x_delta = XYBevelBorder[module_size]/TMath::Cos(BevelAngleRad[module_size]);
            Double_t y_delta = XYBevelBorder[module_size]/TMath::Sin(BevelAngleRad[module_size]);

            //find shifts of the sensitive zone from module borders via the equation of the straight line for the inner bevel
            Double_t x_delta_top_BP = XLeftBorder[module_size];
            Double_t y_delta_top_BP = TMath::Tan(TMath::PiOver2()+BevelAngleRad[module_size])*x_delta_top_BP + (YBevelSize[module_size]+y_delta);

            Double_t y_delta_bottom_BP = YBottomBorder[module_size];
            Double_t x_delta_bottom_BP = (y_delta_bottom_BP - (YBevelSize[module_size] + y_delta))/(TMath::Tan(TMath::PiOver2()+BevelAngleRad[module_size]));
            //----------------------------------------------------------------------

            Double_t x_pos_sensor_zone_bevel[5] = { +XModuleSize[module_size]*0.5-XLeftBorder[module_size], -XModuleSize[module_size]*0.5+XRightBorder[module_size], -XModuleSize[module_size]*0.5+XRightBorder[module_size],       +XModuleSize[module_size]*0.5-x_delta_bottom_BP, +XModuleSize[module_size]*0.5-XLeftBorder[module_size]};
            Double_t y_pos_sensor_zone_bevel[5] = { +YModuleSizeLowerPart[module_size]*0.5-YMiddleBorderLowerPart[module_size], +YModuleSizeLowerPart[module_size]*0.5-YMiddleBorderLowerPart[module_size], -YModuleSizeLowerPart[module_size]*0.5+YBottomBorder[module_size],      -YModuleSizeLowerPart[module_size]*0.5+YBottomBorder[module_size], -YModuleSizeLowerPart[module_size]*0.5+y_delta_top_BP};

            ((TGeoXtru*)sensor_zone_lower_part_bevelS)->DefinePolygon(5, x_pos_sensor_zone_bevel, y_pos_sensor_zone_bevel);
            ((TGeoXtru*)sensor_zone_lower_part_bevelS)->DefineSection(0, -ZModuleSize[module_size]*0.5);
            ((TGeoXtru*)sensor_zone_lower_part_bevelS)->DefineSection(1, +ZModuleSize[module_size]*0.5);
        }

    //--------------------------------------------------------------------------

    //Volumes ------------------------------------------------------------------
    //module volume
    //TString module_name = "moduleV_";

    TGeoVolume *moduleV = new TGeoVolume(module_name, moduleS);

    //sensor zone volume (upper part of the module)
    TGeoVolume *sensor_zone_upper_part_rectV = new TGeoVolume("Sensor_zone_upper_part_rectV", sensor_zone_upper_part_rectS);

    //sensor zone volume (lower part of the module)
    TGeoVolume *sensor_zone_upper_part_bevelV = new TGeoVolume("Sensor_zone_upper_part_bevelV", sensor_zone_upper_part_bevelS);

    //sensor zone volume (lower part of the module: rectangle zone)
    TGeoVolume *sensor_zone_lower_part_rectV = new TGeoVolume("Sensor_zone_lower_part_rectV", sensor_zone_lower_part_rectS);

    //sensor zone volume (lower part of the module: bevel zone)
    TGeoVolume *sensor_zone_lower_part_bevelV = new TGeoVolume("Sensor_zone_lower_part_bevelV", sensor_zone_lower_part_bevelS);


    //volumes media
    TGeoMedium *silicon_medium = pMedSilicon;
    if(silicon_medium) {
        moduleV->SetMedium(silicon_medium);
        sensor_zone_upper_part_rectV->SetMedium(silicon_medium);
        sensor_zone_upper_part_bevelV->SetMedium(silicon_medium);
        sensor_zone_lower_part_rectV->SetMedium(silicon_medium);
        sensor_zone_lower_part_bevelV->SetMedium(silicon_medium);
    }
    else Fatal("Main", "Invalid medium for modules and sensor zones!");


    //volumes visual property (transparency)
    moduleV->SetLineColor(TColor::GetColor("#ff47ca"));
    sensor_zone_upper_part_rectV->SetLineColor(TColor::GetColor("#e0b0ff"));
    sensor_zone_upper_part_bevelV->SetLineColor(TColor::GetColor("#e0b0ff"));
    sensor_zone_lower_part_rectV->SetLineColor(TColor::GetColor("#e0b0ff"));
    sensor_zone_lower_part_bevelV->SetLineColor(TColor::GetColor("#e0b0ff"));
    moduleV->SetTransparency(50);


    switch(module_type) {

        case RIGHT_ANGLED_MODULE: {

            //position of the sensor zone in the upper part of the module
            TGeoTranslation *sensor_zone_upper_part_pos = new TGeoTranslation();
                sensor_zone_upper_part_pos->SetDx(+(XLeftBorder[module_size]+XRightBorder[module_size])*0.5 - XLeftBorder[module_size]);
                sensor_zone_upper_part_pos->SetDy(+YSensorZoneSizeUpperPart[module_size]*0.5 + YGapBetweenPartsOfModule[module_size]*0.5 + YMiddleBorderUpperPart[module_size] - YShiftFromCenterToAxis[module_size]);
                sensor_zone_upper_part_pos->SetDz(0.0);

            //position of the sensor zone in the lower part of the module
            TGeoTranslation *sensor_zone_lower_part_pos = new TGeoTranslation();
                sensor_zone_lower_part_pos->SetDx(+(XLeftBorder[module_size]+XRightBorder[module_size])*0.5 - XLeftBorder[module_size]);
                sensor_zone_lower_part_pos->SetDy(-YSensorZoneSizeLowerPart[module_size]*0.5 - YGapBetweenPartsOfModule[module_size]*0.5 - YMiddleBorderLowerPart[module_size] - YShiftFromCenterToAxis[module_size]);
                sensor_zone_lower_part_pos->SetDz(0.0);

            moduleV->AddNode(sensor_zone_upper_part_rectV, 0, sensor_zone_upper_part_pos);
            moduleV->AddNode(sensor_zone_lower_part_rectV, 0, sensor_zone_lower_part_pos);
            break;
        }

        case RIGHT_ANGLED_MODULE_HALF: {
            //position of the sensor zone in the upper part of the module
            TGeoTranslation *sensor_zone_upper_part_pos = new TGeoTranslation();
                sensor_zone_upper_part_pos->SetDx(+(XLeftBorder[module_size]+XRightBorder[module_size])*0.5 - XLeftBorder[module_size]);
                sensor_zone_upper_part_pos->SetDy(+(YTopBorder[module_size]+YMiddleBorderUpperPart[module_size])*0.5 - YTopBorder[module_size]);
                sensor_zone_upper_part_pos->SetDz(0.0);

            moduleV->AddNode(sensor_zone_upper_part_rectV, 0, sensor_zone_upper_part_pos);
            break;
        }

        case BEVEL_MODULE: {

            //position of the sensor zone in the upper part of the module
            TGeoTranslation *sensor_zone_upper_part_pos = new TGeoTranslation();
                sensor_zone_upper_part_pos->SetDx(+(XLeftBorder[module_size]+XRightBorder[module_size])*0.5 - XLeftBorder[module_size]);
                sensor_zone_upper_part_pos->SetDy(+YSensorZoneSizeUpperPart[module_size]*0.5 + YGapBetweenPartsOfModule[module_size]*0.5 + YMiddleBorderUpperPart[module_size] - YShiftFromCenterToAxis[module_size]);
                sensor_zone_upper_part_pos->SetDz(0.0);

            //position of the sensor zone in the lower part of the module
            TGeoTranslation *sensor_zone_lower_part_bevel_pos = new TGeoTranslation();
                sensor_zone_lower_part_bevel_pos->SetDx(0.0);
                sensor_zone_lower_part_bevel_pos->SetDy(-YModuleSizeLowerPart[module_size]*0.5 - YGapBetweenPartsOfModule[module_size]*0.5 - YShiftFromCenterToAxis[module_size]);
                sensor_zone_lower_part_bevel_pos->SetDz(0.0);

            moduleV->AddNode(sensor_zone_upper_part_rectV, 0, sensor_zone_upper_part_pos);
            moduleV->AddNode(sensor_zone_lower_part_bevelV, 0, sensor_zone_lower_part_bevel_pos);
            break;
        }

        case BEVEL_MODULE_HALF: {

            //position of the sensor zone in the upper part of the module
            TGeoTranslation *sensor_zone_upper_part_pos = new TGeoTranslation();
                sensor_zone_upper_part_pos->SetDx(0);
                sensor_zone_upper_part_pos->SetDy(0);
                sensor_zone_upper_part_pos->SetDz(0.0);

            moduleV->AddNode(sensor_zone_upper_part_bevelV, 0, sensor_zone_upper_part_pos);
            break;
        }

        default:
            Error("CreateModule()", "Incorrect module type selected!!!");
    }

    return moduleV;
}

//------------------------------------------------------------------------------

TGeoVolume *CreateModuleFrame(TString module_name, ModuleType module_type, ModuleSize module_size) {

    //Shapes -------------------------------------------------------------------
    //module shape
    TGeoShape *frameS = 0;

    //frame margins for a basic si-module
    Double_t leftMargin = 0.1; //cm
    Double_t rightMargin = 0.4; //cm
    Double_t bottomMargin = 0.1; //cm
    Double_t topMargin = 5.0; //cm

    switch(module_type) {
        case RIGHT_ANGLED_MODULE: {
            //frameS = new TGeoBBox("frameS", XModuleSize*0.5, YModuleSize*0.5, ZModuleFrameSize*0.5);
            frameS = new TGeoXtru(2);
            frameS->SetName(TString("frameS")+=module_name);
                Double_t x_pos[12] = {
                    +(XModuleSize[module_size]*0.5 + leftMargin)/*0*/,
                    +(XModuleSize[module_size]*0.5 + leftMargin)/*1*/,
                    +(XModuleSize[module_size]*0.5 + leftMargin + 1.0)/*2*/,
                    +(XModuleSize[module_size]*0.5 + leftMargin + 1.0)/*3*/,
                    +(XModuleSize[module_size]*0.5 + leftMargin)/*4*/,
                    +(XModuleSize[module_size]*0.5 + leftMargin)/*5*/,
                    -(XModuleSize[module_size]*0.5 + rightMargin)/*6*/,
                    -(XModuleSize[module_size]*0.5 + rightMargin)/*7*/,
                    -(XModuleSize[module_size]*0.5 + rightMargin + 1.0)/*8*/,
                    -(XModuleSize[module_size]*0.5 + rightMargin + 1.0)/*9*/,
                    -(XModuleSize[module_size]*0.5 + rightMargin)/*10*/,
                    -(XModuleSize[module_size]*0.5 + rightMargin)/*11*/,
                };
                Double_t y_pos[12] = {
                    -(YModuleSize[module_size]*0.5 + bottomMargin)/*0*/,
                    +(YModuleSize[module_size]*0.5 - 1.6)/*1*/,
                    +(YModuleSize[module_size]*0.5 - 0.5)/*2*/,
                    +(YModuleSize[module_size]*0.5 + 5.0)/*3*/,
                    +(YModuleSize[module_size]*0.5 + 5.0)/*4*/,
                    +(YModuleSize[module_size]*0.5 + 9.0)/*5*/,
                    +(YModuleSize[module_size]*0.5 + 9.0)/*6*/,
                    +(YModuleSize[module_size]*0.5 + 5.0)/*7*/,
                    +(YModuleSize[module_size]*0.5 + 5.0)/*8*/,
                    +(YModuleSize[module_size]*0.5 - 0.5)/*9*/,
                    +(YModuleSize[module_size]*0.5 - 1.6)/*10*/,
                    -(YModuleSize[module_size]*0.5)/*11*/,
                };
            ((TGeoXtru*)frameS)->DefinePolygon(12, x_pos, y_pos);
            ((TGeoXtru*)frameS)->DefineSection(0, -ZModuleFrameSize[module_size]*0.5);
            ((TGeoXtru*)frameS)->DefineSection(1, +ZModuleFrameSize[module_size]*0.5);
            break;
        }
        case RIGHT_ANGLED_MODULE_HALF: {
            //frameS = new TGeoBBox("frameS", XModuleSize*0.5, YModuleSizeUpperPart*0.5, ZModuleFrameSize*0.5);
            frameS = new TGeoXtru(2);
            frameS->SetName(TString("frameS")+=module_name);
                Double_t x_pos[12] = {
                    +(XModuleSize[module_size]*0.5 + leftMargin)/*0*/,
                    +(XModuleSize[module_size]*0.5 + leftMargin)/*1*/,
                    +(XModuleSize[module_size]*0.5 + leftMargin + 1.0)/*2*/,
                    +(XModuleSize[module_size]*0.5 + leftMargin + 1.0)/*3*/,
                    +(XModuleSize[module_size]*0.5 + leftMargin)/*4*/,
                    +(XModuleSize[module_size]*0.5 + leftMargin)/*5*/,
                    -(XModuleSize[module_size]*0.5 + rightMargin)/*6*/,
                    -(XModuleSize[module_size]*0.5 + rightMargin)/*7*/,
                    -(XModuleSize[module_size]*0.5 + rightMargin + 1.0)/*8*/,
                    -(XModuleSize[module_size]*0.5 + rightMargin + 1.0)/*9*/,
                    -(XModuleSize[module_size]*0.5 + rightMargin)/*10*/,
                    -(XModuleSize[module_size]*0.5 + rightMargin)/*11*/,
                };
                Double_t y_pos[12] = {
                    -(YModuleSizeUpperPart[module_size]*0.5 + bottomMargin)/*0*/,
                    +(YModuleSizeUpperPart[module_size]*0.5 - 1.6)/*1*/,
                    +(YModuleSizeUpperPart[module_size]*0.5 - 0.5)/*2*/,
                    +(YModuleSizeUpperPart[module_size]*0.5 + 5.0)/*3*/,
                    +(YModuleSizeUpperPart[module_size]*0.5 + 5.0)/*4*/,
                    +(YModuleSizeUpperPart[module_size]*0.5 + 9.0)/*5*/,
                    +(YModuleSizeUpperPart[module_size]*0.5 + 9.0)/*6*/,
                    +(YModuleSizeUpperPart[module_size]*0.5 + 5.0)/*7*/,
                    +(YModuleSizeUpperPart[module_size]*0.5 + 5.0)/*8*/,
                    +(YModuleSizeUpperPart[module_size]*0.5 - 0.5)/*9*/,
                    +(YModuleSizeUpperPart[module_size]*0.5 - 1.6)/*10*/,
                    -(YModuleSizeUpperPart[module_size]*0.5)/*11*/,
                };
            ((TGeoXtru*)frameS)->DefinePolygon(12, x_pos, y_pos);
            ((TGeoXtru*)frameS)->DefineSection(0, -ZModuleFrameSize[module_size]*0.5);
            ((TGeoXtru*)frameS)->DefineSection(1, +ZModuleFrameSize[module_size]*0.5);
            break;
        }
        case BEVEL_MODULE: {
            frameS = new TGeoXtru(2);
            frameS->SetName(TString("frameS")+=module_name);
                Double_t x_pos[13] = {
                    +(XModuleSize[module_size]*0.5 - XBevelSize[module_size] + leftMargin)/*0*/,
                    +(XModuleSize[module_size]*0.5 + leftMargin)/*1*/,
                    +(XModuleSize[module_size]*0.5 + leftMargin)/*2*/,
                    +(XModuleSize[module_size]*0.5 + leftMargin + 1.0)/*3*/,
                    +(XModuleSize[module_size]*0.5 + leftMargin + 1.0)/*4*/,
                    +(XModuleSize[module_size]*0.5 + leftMargin)/*5*/,
                    +(XModuleSize[module_size]*0.5 + leftMargin)/*6*/,
                    -(XModuleSize[module_size]*0.5 + rightMargin)/*7*/,
                    -(XModuleSize[module_size]*0.5 + rightMargin)/*8*/,
                    -(XModuleSize[module_size]*0.5 + rightMargin + 1.0)/*9*/,
                    -(XModuleSize[module_size]*0.5 + rightMargin + 1.0)/*10*/,
                    -(XModuleSize[module_size]*0.5 + rightMargin)/*11*/,
                    -(XModuleSize[module_size]*0.5 + rightMargin)/*12*/,
                };
                Double_t y_pos[13] = {
                    -(YModuleSize[module_size]*0.5 + bottomMargin)/*0*/,
                    -(YModuleSize[module_size]*0.5 - YBevelSize[module_size] + bottomMargin)/*1*/,
                    +(YModuleSize[module_size]*0.5 - 1.6)/*2*/,
                    +(YModuleSize[module_size]*0.5 - 0.5)/*3*/,
                    +(YModuleSize[module_size]*0.5 + 5.0)/*4*/,
                    +(YModuleSize[module_size]*0.5 + 5.0)/*5*/,
                    +(YModuleSize[module_size]*0.5 + 9.0)/*6*/,
                    +(YModuleSize[module_size]*0.5 + 9.0)/*7*/,
                    +(YModuleSize[module_size]*0.5 + 5.0)/*8*/,
                    +(YModuleSize[module_size]*0.5 + 5.0)/*9*/,
                    +(YModuleSize[module_size]*0.5 - 0.5)/*10*/,
                    +(YModuleSize[module_size]*0.5 - 1.6)/*11*/,
                    -(YModuleSize[module_size]*0.5)/*12*/,
                };
            ((TGeoXtru*)frameS)->DefinePolygon(13, x_pos, y_pos);
            ((TGeoXtru*)frameS)->DefineSection(0, -ZModuleFrameSize[module_size]*0.5);
            ((TGeoXtru*)frameS)->DefineSection(1, +ZModuleFrameSize[module_size]*0.5);
            break;
        }
        case BEVEL_MODULE_HALF: {
            frameS = new TGeoXtru(2);
            frameS->SetName(TString("frameS")+=module_name);
                Double_t x_pos[13] = {
                    +(XModuleSize[module_size]*0.5 - XBevelSize[module_size] + leftMargin)/*0*/,
                    +(XModuleSize[module_size]*0.5 + leftMargin)/*1*/,
                    +(XModuleSize[module_size]*0.5 + leftMargin)/*2*/,
                    +(XModuleSize[module_size]*0.5 + leftMargin + 1.0)/*3*/,
                    +(XModuleSize[module_size]*0.5 + leftMargin + 1.0)/*4*/,
                    +(XModuleSize[module_size]*0.5 + leftMargin)/*5*/,
                    +(XModuleSize[module_size]*0.5 + leftMargin)/*6*/,
                    -(XModuleSize[module_size]*0.5 + rightMargin)/*7*/,
                    -(XModuleSize[module_size]*0.5 + rightMargin)/*8*/,
                    -(XModuleSize[module_size]*0.5 + rightMargin + 1.0)/*9*/,
                    -(XModuleSize[module_size]*0.5 + rightMargin + 1.0)/*10*/,
                    -(XModuleSize[module_size]*0.5 + rightMargin)/*11*/,
                    -(XModuleSize[module_size]*0.5 + rightMargin)/*12*/,
                };
                Double_t y_pos[13] = {
                    -(YModuleSizeUpperPart[module_size]*0.5 + bottomMargin)/*0*/,
                    -(YModuleSizeUpperPart[module_size]*0.5 - YBevelSize[module_size] + bottomMargin)/*1*/,
                    +(YModuleSizeUpperPart[module_size]*0.5 - 1.6)/*2*/,
                    +(YModuleSizeUpperPart[module_size]*0.5 - 0.5)/*3*/,
                    +(YModuleSizeUpperPart[module_size]*0.5 + 5.0)/*4*/,
                    +(YModuleSizeUpperPart[module_size]*0.5 + 5.0)/*5*/,
                    +(YModuleSizeUpperPart[module_size]*0.5 + 9.0)/*6*/,
                    +(YModuleSizeUpperPart[module_size]*0.5 + 9.0)/*7*/,
                    +(YModuleSizeUpperPart[module_size]*0.5 + 5.0)/*8*/,
                    +(YModuleSizeUpperPart[module_size]*0.5 + 5.0)/*9*/,
                    +(YModuleSizeUpperPart[module_size]*0.5 - 0.5)/*10*/,
                    +(YModuleSizeUpperPart[module_size]*0.5 - 1.6)/*11*/,
                    -(YModuleSizeUpperPart[module_size]*0.5)/*12*/,
                };
            ((TGeoXtru*)frameS)->DefinePolygon(13, x_pos, y_pos);
            ((TGeoXtru*)frameS)->DefineSection(0, -ZModuleFrameSize[module_size]*0.5);
            ((TGeoXtru*)frameS)->DefineSection(1, +ZModuleFrameSize[module_size]*0.5);
            break;
        }
        default:
            Error("CreateModule()", "Incorrect module type selected!!!");
    }

    Double_t innerMargin = 0.5; //cm, frame inner margin from edge of sens.

    //rectangle window of the frame
    TGeoShape *window_rectS = new TGeoBBox(TString("window_rectS")+=module_name, (XModuleSize[module_size]-innerMargin*2)*0.5, (YModuleSize[module_size]*0.5-innerMargin*2)*0.5, (ZModuleFrameSize[module_size]+0.001)*0.5);

    //bevel window of the frame
    TGeoShape *window_bevelS = new TGeoXtru(2);
        window_bevelS->SetName(TString("window_bevelS")+=module_name);
        //Calculate coordinates of the bottom and the top points of the inner bevel
        {
            //find x and y projections of the bevel border thickness
            Double_t x_delta = innerMargin/TMath::Cos(BevelAngleRad[module_size]);
            Double_t y_delta = innerMargin/TMath::Sin(BevelAngleRad[module_size]);

            //find shifts of the sensitive zone from module borders via the equation of the straight line for the inner bevel
            Double_t x_delta_top_BP = innerMargin;
            Double_t y_delta_top_BP = TMath::Tan(TMath::PiOver2()+BevelAngleRad[module_size])*x_delta_top_BP + (YBevelSize[module_size]+y_delta);

            Double_t y_delta_bottom_BP = innerMargin;
            Double_t x_delta_bottom_BP = (y_delta_bottom_BP - (YBevelSize[module_size] + y_delta))/(TMath::Tan(TMath::PiOver2()+BevelAngleRad[module_size]));
            //----------------------------------------------------------------------

            Double_t x_pos[5] = { +XModuleSize[module_size]*0.5-innerMargin, -XModuleSize[module_size]*0.5+innerMargin, -XModuleSize[module_size]*0.5+innerMargin,       +XModuleSize[module_size]*0.5-x_delta_bottom_BP, +XModuleSize[module_size]*0.5-innerMargin  };
            Double_t y_pos[5] = { +YModuleSize[module_size]*0.25-innerMargin, +YModuleSize[module_size]*0.25-innerMargin, -YModuleSize[module_size]*0.25+innerMargin,      -YModuleSize[module_size]*0.25+innerMargin, -YModuleSize[module_size]*0.25+y_delta_top_BP};

            ((TGeoXtru*)window_bevelS)->DefinePolygon(5, x_pos, y_pos);
            ((TGeoXtru*)window_bevelS)->DefineSection(0, -(ZModuleFrameSize[module_size]+0.001)*0.5);
            ((TGeoXtru*)window_bevelS)->DefineSection(1, +(ZModuleFrameSize[module_size]+0.001)*0.5);
        }

        //translations
        TGeoTranslation *upper_window_frame_pos = new TGeoTranslation();
            upper_window_frame_pos->SetName(TString("upper_window_frame_pos")+=module_name);
            upper_window_frame_pos->SetDx(0.0);
            upper_window_frame_pos->SetDy(+YSensorZoneSizeUpperPart[module_size]*0.5 + YGapBetweenPartsOfModule[module_size]*0.5 + YMiddleBorderUpperPart[module_size] - YShiftFromCenterToAxis[module_size]);
            upper_window_frame_pos->SetDz(0.0);
            upper_window_frame_pos->RegisterYourself();

        TGeoTranslation *lower_window_frame_pos = new TGeoTranslation();
            lower_window_frame_pos->SetName(TString("lower_window_frame_pos")+=module_name);
            lower_window_frame_pos->SetDx(0.0);
            lower_window_frame_pos->SetDy(-YSensorZoneSizeLowerPart[module_size]*0.5 - YGapBetweenPartsOfModule[module_size]*0.5 - YMiddleBorderLowerPart[module_size] - YShiftFromCenterToAxis[module_size]);
            lower_window_frame_pos->SetDz(0.0);
            lower_window_frame_pos->RegisterYourself();

        TGeoTranslation *window_half_frame_pos = new TGeoTranslation();
            window_half_frame_pos->SetName(TString("window_half_frame_pos")+=module_name);
            window_half_frame_pos->SetDx(0.0);
            window_half_frame_pos->SetDy(0.0);
            window_half_frame_pos->SetDz(0.0);
            window_half_frame_pos->RegisterYourself();


        TGeoCompositeShape *frame_with_windowsS = new TGeoCompositeShape();
        frame_with_windowsS->SetName("frame_with_windowsS");

        switch(module_type) {

            case RIGHT_ANGLED_MODULE: {
                TString expression = "(frameS"; expression += module_name;
                    expression +="-window_rectS"; expression += module_name;
                    expression += ":upper_window_frame_pos"; expression += module_name;
                    expression += ")-window_rectS"; expression += module_name;
                    expression += ":lower_window_frame_pos"; expression += module_name;
                frame_with_windowsS->MakeNode(expression);
                frame_with_windowsS->ComputeBBox();
                break;
            }

            case RIGHT_ANGLED_MODULE_HALF: {
                TString expression = "(frameS"; expression += module_name;
                    expression +=")-window_rectS"; expression += module_name;
                    expression += ":window_half_frame_pos"; expression += module_name;
                frame_with_windowsS->MakeNode(expression);
                frame_with_windowsS->ComputeBBox();
                break;
            }

            case BEVEL_MODULE: {
                TString expression = "(frameS"; expression += module_name;
                    expression +="-window_rectS"; expression += module_name;
                    expression += ":upper_window_frame_pos"; expression += module_name;
                    expression += ")-window_bevelS"; expression += module_name;
                    expression += ":lower_window_frame_pos"; expression += module_name;
                frame_with_windowsS->MakeNode(expression);
                frame_with_windowsS->ComputeBBox();
                break;
            }

            case BEVEL_MODULE_HALF: {
                TString expression = "(frameS"; expression += module_name;
                    expression +=")-window_bevelS"; expression += module_name;
                    expression += ":window_half_frame_pos"; expression += module_name;
                frame_with_windowsS->MakeNode(expression);
                frame_with_windowsS->ComputeBBox();
                break;
            }

            default:
                Error("CreateModule()", "Incorrect module type selected!!!");
        }
    //--------------------------------------------------------------------------

    //Volumes ------------------------------------------------------------------
    //module frame volume
    TGeoVolume *frameV = new TGeoVolume(module_name, frame_with_windowsS);

    //volumes media
    TGeoMedium *carbon_medium = pMedCarbon;
    if(carbon_medium) {
        frameV->SetMedium(carbon_medium);
    }
    else Fatal("Main", "Invalid medium for frameV!");

    //volumes visual property (transparency)
    frameV->SetLineColor(TColor::GetColor("#cccccc"));
    frameV->SetTransparency(0);

    return frameV;
}
//------------------------------------------------------------------------------

TGeoVolume *CreateFrameForStation_StandardModule(TString frame_name, Int_t current_station_num) {

    //TGeoVolume *frames = new TGeoVolumeAssembly(frame_name);

    TGeoVolume *frames = new TGeoVolumeAssembly(frame_name);
    frames->SetMedium(pMedAir);

    Int_t NModules = NModulesInStations[current_station_num];

    //face shield ----------------------------------------------------------
    Double_t faceShild_XSize = (NModules/2.0)*6.3 - (NModules/2.0)*0.3 + 8.6; //cm
    Double_t faceShild_YSize = 14.74; //cm
    Double_t faceShild_ZSize = 0.3; //cm

    TGeoShape *faceShildS = new TGeoXtru(2);
    faceShildS->SetName("faceShildS");
    {
        Double_t x_pos[8] = {faceShild_XSize*0.5, faceShild_XSize*0.5, -faceShild_XSize*0.5, -faceShild_XSize*0.5, -2.75, -2.3, 2.3, 2.75};
        Double_t y_pos[8] = {-faceShild_YSize*0.5, faceShild_YSize*0.5, faceShild_YSize*0.5, -faceShild_YSize*0.5, -faceShild_YSize*0.5, -3.8, -3.8, -faceShild_YSize*0.5};
        ((TGeoXtru*)faceShildS)->DefinePolygon(8, x_pos, y_pos);
        ((TGeoXtru*)faceShildS)->DefineSection(0, -faceShild_ZSize*0.5);
        ((TGeoXtru*)faceShildS)->DefineSection(1, +faceShild_ZSize*0.5);
    }

    TGeoVolume *faceShildV = new TGeoVolume(TString("faceShildV")+=TString("_") + frames->GetName(), faceShildS);

    //volume medium
    TGeoMedium *faceShildV_medium = pMedPolystyrene;
    if(faceShildV_medium) {
        faceShildV->SetMedium(faceShildV_medium);
    }
    else Fatal("Main", "Invalid medium for faceShildV!");

    //volume visual property (transparency)
    faceShildV->SetLineColor(TColor::GetColor("#ccffcc"));
    faceShildV->SetTransparency(50);

    TGeoCombiTrans *faceShild_transf[4];

    faceShild_transf[0] = new TGeoCombiTrans();
    faceShild_transf[0]->SetDy(+faceShild_YSize*0.5 - 0.95);
    faceShild_transf[0]->SetDz(+0.695/*center of half-plane*/ - 1.35/*half-thick of frame box*/ - faceShild_ZSize*0.5/*face shield shift*/ + 4.31/*half-plane shift*/);

    faceShild_transf[1] = new TGeoCombiTrans();
    faceShild_transf[1]->SetDy(+faceShild_YSize*0.5 - 0.95);
    faceShild_transf[1]->SetDz(+0.695/*center of half-plane*/ + 1.35/*half-thick of frame box*/ + faceShild_ZSize*0.5/*face shield shift*/ + 4.31/*half-plane shift*/);

    faceShild_transf[2] = new TGeoCombiTrans();
    faceShild_transf[2]->RotateX(180.0);
    faceShild_transf[2]->SetDy(-faceShild_YSize*0.5 + 0.95);
    faceShild_transf[2]->SetDz(+0.695/*center of half-plane*/ - 1.35/*half-thick of frame box*/ - faceShild_ZSize*0.5/*face shield shift*/);

    faceShild_transf[3] = new TGeoCombiTrans();
    faceShild_transf[3]->RotateX(180.0);
    faceShild_transf[3]->SetDy(-faceShild_YSize*0.5 + 0.95);
    faceShild_transf[3]->SetDz(+0.695/*center of half-plane*/ + 1.35/*half-thick of frame box*/ + faceShild_ZSize*0.5/*face shield shift*/);

    frames->AddNode(faceShildV, 0, faceShild_transf[0]);
    frames->AddNode(faceShildV, 1, faceShild_transf[1]);
    frames->AddNode(faceShildV, 2, faceShild_transf[2]);
    frames->AddNode(faceShildV, 3, faceShild_transf[3]);
    //----------------------------------------------------------------------

    //sidewall of shield ---------------------------------------------------
    Double_t sideShild_XSize = 0.95; //cm
    Double_t sideShild_YSize = 15.2; //cm
    Double_t sideShild_ZSize = 2.7; //cm

    TGeoShape *sideShieldS = new TGeoBBox("sideShieldS", sideShild_XSize*0.5, sideShild_YSize*0.5, sideShild_ZSize*0.5);

    TGeoVolume *sideShieldV =  new TGeoVolume(TString("sideShieldV")+=TString("_") + frames->GetName(), sideShieldS);

    //volume medium
    TGeoMedium *sideShieldV_medium = pMedAluminium;
    if(sideShieldV_medium) {
        sideShieldV->SetMedium(sideShieldV_medium);
    }
    else Fatal("Main", "Invalid medium for sideShieldV!");

    //volume visual property (transparency)
    sideShieldV->SetLineColor(TColor::GetColor("#ffcccc"));
    sideShieldV->SetTransparency(50);

    TGeoCombiTrans *sideShild_transf[4];

    sideShild_transf[0] = new TGeoCombiTrans();
    sideShild_transf[0]->SetDx(+(faceShild_XSize*0.5 - sideShild_XSize*0.5));
    sideShild_transf[0]->SetDy(+faceShild_YSize*0.5 - 0.95 + (sideShild_YSize - faceShild_YSize)*0.5);
    sideShild_transf[0]->SetDz(+0.695/*center of half-plane*/ + 4.31/*half-plane shift*/);

    sideShild_transf[1] = new TGeoCombiTrans();
    sideShild_transf[1]->SetDx(-(faceShild_XSize*0.5 - sideShild_XSize*0.5));
    sideShild_transf[1]->SetDy(+faceShild_YSize*0.5 - 0.95 + (sideShild_YSize - faceShild_YSize)*0.5);
    sideShild_transf[1]->SetDz(+0.695/*center of half-plane*/ + 4.31/*half-plane shift*/);

    sideShild_transf[2] = new TGeoCombiTrans();
    sideShild_transf[2]->SetDx(+(faceShild_XSize*0.5 - sideShild_XSize*0.5));
    sideShild_transf[2]->SetDy(-faceShild_YSize*0.5 + 0.95 - (sideShild_YSize - faceShild_YSize)*0.5);
    sideShild_transf[2]->SetDz(+0.695/*center of half-plane*/);

    sideShild_transf[3] = new TGeoCombiTrans();
    sideShild_transf[3]->SetDx(-(faceShild_XSize*0.5 - sideShild_XSize*0.5));
    sideShild_transf[3]->SetDy(-faceShild_YSize*0.5 + 0.95 - (sideShild_YSize - faceShild_YSize)*0.5);
    sideShild_transf[3]->SetDz(+0.695/*center of half-plane*/);

    frames->AddNode(sideShieldV, 0, sideShild_transf[0]);
    frames->AddNode(sideShieldV, 1, sideShild_transf[1]);
    frames->AddNode(sideShieldV, 2, sideShild_transf[2]);
    frames->AddNode(sideShieldV, 3, sideShild_transf[3]);
    //----------------------------------------------------------------------

    //horizontal plane of shield -------------------------------------------
    Double_t horizShild_XSize = faceShild_XSize*0.5 - sideShild_XSize - 2.75/*half-size of hole*/; //cm
    Double_t horizShild_YSize = 0.3; //cm
    Double_t horizShild_ZSize = 2.7; //cm

    TGeoShape *horizShieldS = new TGeoBBox("horizShieldS", horizShild_XSize*0.5, horizShild_YSize*0.5, horizShild_ZSize*0.5);

    TGeoVolume *horizShieldV = new TGeoVolume(TString("horizShieldV")+=TString("_") + frames->GetName(), horizShieldS);

    //volume medium
    TGeoMedium *horizShieldV_medium = pMedPolystyrene;
    if(horizShieldV_medium) {
        horizShieldV->SetMedium(horizShieldV_medium);
    }
    else Fatal("Main", "Invalid medium for horizShieldV!");

    //volume visual property (transparency)
    horizShieldV->SetLineColor(TColor::GetColor("#ccffcc"));
    horizShieldV->SetTransparency(50);

    TGeoCombiTrans *horizShild_transf[4];

    horizShild_transf[0] = new TGeoCombiTrans();
    horizShild_transf[0]->SetDx(+(faceShild_XSize*0.5 - horizShild_XSize*0.5 - sideShild_XSize));
    horizShild_transf[0]->SetDy(+(-0.95 + horizShild_YSize*0.5));
    horizShild_transf[0]->SetDz(+0.695/*center of half-plane*/ + 4.31/*half-plane shift*/);

    horizShild_transf[1] = new TGeoCombiTrans();
    horizShild_transf[1]->SetDx(-(faceShild_XSize*0.5 - horizShild_XSize*0.5 - sideShild_XSize));
    horizShild_transf[1]->SetDy(+(-0.95 + horizShild_YSize*0.5));
    horizShild_transf[1]->SetDz(+0.695/*center of half-plane*/ + 4.31/*half-plane shift*/);

    horizShild_transf[2] = new TGeoCombiTrans();
    horizShild_transf[2]->SetDx(+(faceShild_XSize*0.5 - horizShild_XSize*0.5 - sideShild_XSize));
    horizShild_transf[2]->SetDy(-(-0.95 + horizShild_YSize*0.5));
    horizShild_transf[2]->SetDz(+0.695/*center of half-plane*/);

    horizShild_transf[3] = new TGeoCombiTrans();
    horizShild_transf[3]->SetDx(-(faceShild_XSize*0.5 - horizShild_XSize*0.5 - sideShild_XSize));
    horizShild_transf[3]->SetDy(-(-0.95 + horizShild_YSize*0.5));
    horizShild_transf[3]->SetDz(+0.695/*center of half-plane*/);

    frames->AddNode(horizShieldV, 0, horizShild_transf[0]);
    frames->AddNode(horizShieldV, 1, horizShild_transf[1]);
    frames->AddNode(horizShieldV, 2, horizShild_transf[2]);
    frames->AddNode(horizShieldV, 3, horizShild_transf[3]);
    //----------------------------------------------------------------------

    //plank of shield ------------------------------------------------------
    Double_t plankShild_XSize = faceShild_XSize; //cm
    Double_t plankShild_YSize = 1.5; //cm
    Double_t plankShild_ZSize = 0.3; //cm

    TGeoShape *plankShieldS = new TGeoBBox("plankShieldS", plankShild_XSize*0.5, plankShild_YSize*0.5, plankShild_ZSize*0.5);

    TGeoVolume *plankShieldV = new TGeoVolume(TString("plankShieldV")+=TString("_") + frames->GetName(), plankShieldS);

    //volume medium
    TGeoMedium *plankShieldV_medium = pMedAluminium;
    if(plankShieldV_medium) {
        plankShieldV->SetMedium(plankShieldV_medium);
    }
    else Fatal("Main", "Invalid medium for plankShieldV!");

    //volume visual property (transparency)
    plankShieldV->SetLineColor(TColor::GetColor("#ffcccc"));
    plankShieldV->SetTransparency(50);

    TGeoCombiTrans *plankShild_transf[4];

    plankShild_transf[0] = new TGeoCombiTrans();
    plankShild_transf[0]->SetDx(0.0);
    plankShild_transf[0]->SetDy(+(-0.95 + faceShild_YSize) - 0.05);
    plankShild_transf[0]->SetDz(+0.695/*center of half-plane*/ - (1.35/*half-thick of frame box*/ + faceShild_ZSize + plankShild_ZSize*0.5) + 4.31/*half-plane shift*/);

    plankShild_transf[1] = new TGeoCombiTrans();
    plankShild_transf[1]->SetDx(0.0);
    plankShild_transf[1]->SetDy(+(-0.95 + faceShild_YSize) - 0.05);
    plankShild_transf[1]->SetDz(+0.695/*center of half-plane*/ + (1.35/*half-thick of frame box*/ + faceShild_ZSize + plankShild_ZSize*0.5) + 4.31/*half-plane shift*/);

    plankShild_transf[2] = new TGeoCombiTrans();
    plankShild_transf[2]->SetDx(0.0);
    plankShild_transf[2]->SetDy(-(-0.95 + faceShild_YSize) + 0.05);
    plankShild_transf[2]->SetDz(+0.695/*center of half-plane*/ - (1.35/*half-thick of frame box*/ + faceShild_ZSize + plankShild_ZSize*0.5));

    plankShild_transf[3] = new TGeoCombiTrans();
    plankShild_transf[3]->SetDx(0.0);
    plankShild_transf[3]->SetDy(-(-0.95 + faceShild_YSize) + 0.05);
    plankShild_transf[3]->SetDz(+0.695/*center of half-plane*/ + (1.35/*half-thick of frame box*/ + faceShild_ZSize + plankShild_ZSize*0.5));

    frames->AddNode(plankShieldV, 0, plankShild_transf[0]);
    frames->AddNode(plankShieldV, 1, plankShild_transf[1]);
    frames->AddNode(plankShieldV, 2, plankShild_transf[2]);
    frames->AddNode(plankShieldV, 3, plankShild_transf[3]);
    //----------------------------------------------------------------------

    //base rail
    Double_t baseRailMainPart_XSize = faceShild_XSize + 3.4*2/*margin*/; //cm
    Double_t baseRailMainPart_YSize = 5.0; //cm
    Double_t baseRailMainPart_ZSize = 0.7; //cm

    cout << "baseRailMainPart_XSize = " << baseRailMainPart_XSize << "\n";

    Double_t baseRailUpperPart_XSize = baseRailMainPart_XSize; //cm
    Double_t baseRailUpperPart_YSize = 1.0; //cm
    Double_t baseRailUpperPart_ZSize = 0.3; //cm

    Double_t baseRailLowerPart_XSize = baseRailMainPart_XSize-8.8/*cut*/; //cm
    Double_t baseRailLowerPart_YSize = 0.7; //cm
    Double_t baseRailLowerPart_ZSize = 0.3; //cm

    TGeoShape *baseRailMainPartS = new TGeoBBox(TString("baseRailMainPartS")+=TString("_") + frames->GetName(), baseRailMainPart_XSize*0.5, baseRailMainPart_YSize*0.5, baseRailMainPart_ZSize*0.5);
    TGeoShape *baseRailUpperPartS = new TGeoBBox(TString("baseRailUpperPartS")+=TString("_") + frames->GetName(), baseRailUpperPart_XSize*0.5, baseRailUpperPart_YSize*0.5, baseRailUpperPart_ZSize*0.5);
    TGeoShape *baseRailLowerPartS = new TGeoBBox(TString("baseRailLowerPartS")+=TString("_") + frames->GetName(), baseRailLowerPart_XSize*0.5, baseRailLowerPart_YSize*0.5, baseRailLowerPart_ZSize*0.5);

    TGeoTranslation *baseRailUpperPart_pos = new TGeoTranslation();
    baseRailUpperPart_pos->SetName(TString("baseRailUpperPart_pos")+=TString("_") + frames->GetName());
    baseRailUpperPart_pos->SetDx(0.0);
    baseRailUpperPart_pos->SetDy(+(baseRailMainPart_YSize*0.5 + baseRailUpperPart_YSize*0.5));
    baseRailUpperPart_pos->SetDz(baseRailMainPart_ZSize*0.5 - baseRailUpperPart_ZSize*0.5);
    baseRailUpperPart_pos->RegisterYourself();

    TGeoTranslation *baseRailLowerPart_pos = new TGeoTranslation();
    baseRailLowerPart_pos->SetName(TString("baseRailLowerPart_pos")+=TString("_") + frames->GetName());
    baseRailLowerPart_pos->SetDx(0.0);
    baseRailLowerPart_pos->SetDy(-(baseRailMainPart_YSize*0.5 + baseRailLowerPart_YSize*0.5));
    baseRailLowerPart_pos->SetDz(baseRailMainPart_ZSize*0.5 - baseRailLowerPart_ZSize*0.5);
    baseRailLowerPart_pos->RegisterYourself();

    TGeoCompositeShape *baseRailS = new TGeoCompositeShape();
    baseRailS->SetName(TString("baseRailS")+=TString("_") + frames->GetName());
    {
        TString expression = "baseRailMainPartS"; expression += TString("_") + frames->GetName();
            expression += "+baseRailUpperPartS"; expression += TString("_") + frames->GetName();
            expression += ":baseRailUpperPart_pos"; expression += TString("_") + frames->GetName();
            expression += "+baseRailLowerPartS"; expression += TString("_") + frames->GetName();
            expression += ":baseRailLowerPart_pos"; expression += TString("_") + frames->GetName();
        baseRailS->MakeNode(expression);
        baseRailS->ComputeBBox(); //need to compute a bounding box
    }

    TGeoVolume *baseRailV = new TGeoVolume(TString("baseRailV")+=TString("_") + frames->GetName(), baseRailS);

    //volume medium
    TGeoMedium *baseRailV_medium = pMedAluminium;
    if(baseRailV_medium) {
        baseRailV->SetMedium(baseRailV_medium);
    }
    else Fatal("Main", "Invalid medium for baseRailV!");

    //volume visual property (transparency)
    baseRailV->SetLineColor(TColor::GetColor("#ffcccc"));
    baseRailV->SetTransparency(50);

    TGeoCombiTrans *baseRail_transf[2];

    baseRail_transf[0] = new TGeoCombiTrans();
    baseRail_transf[0]->RotateY(180.0);
    baseRail_transf[0]->SetDx(0.1);
    baseRail_transf[0]->SetDy(+(14.5 + baseRailMainPart_YSize*0.5));
    baseRail_transf[0]->SetDz(+0.695/*center of half-plane*/ + (1.35/*half-thick of frame box*/ + faceShild_ZSize  +baseRailMainPart_ZSize*0.5 - baseRailLowerPart_ZSize)  + 4.31/*half-plane shift*/);

    baseRail_transf[1] = new TGeoCombiTrans();
    baseRail_transf[1]->ReflectY(true);
    baseRail_transf[1]->SetDx(0.0);
    baseRail_transf[1]->SetDy(-(14.5 + baseRailMainPart_YSize*0.5));
    baseRail_transf[1]->SetDz(+0.695/*center of half-plane*/ - (1.35/*half-thick of frame box*/ + faceShild_ZSize  +baseRailMainPart_ZSize*0.5 - baseRailLowerPart_ZSize));

    frames->AddNode(baseRailV, 0, baseRail_transf[0]);
    frames->AddNode(baseRailV, 1, baseRail_transf[1]);
    //----------------------------------------------------------------------

    //back rail ------------------------------------------------------------
    Double_t backRailMainPart_XSize = faceShild_XSize + - 0.3*2/*margin*/; //cm
    Double_t backRailMainPart_YSize = 3.0; //cm
    Double_t backRailMainPart_ZSize = 0.9; //cm, average 6..12 cm

    Double_t backRailLowerPart_XSize = backRailMainPart_XSize - 1.4; //cm
    Double_t backRailLowerPart_YSize = 0.7; //cm
    Double_t backRailLowerPart_ZSize = 0.6; //cm

    Double_t backRailSidePart_XSize = 1.2; //cm
    Double_t backRailSidePart_YSize = 3.0; //cm
    Double_t backRailSidePart_ZSize = 1.62; //cm

    TGeoShape *backRailMainPartS = new TGeoBBox(TString("backRailMainPartS")+=TString("_") + frames->GetName(), backRailMainPart_XSize*0.5, backRailMainPart_YSize*0.5, backRailMainPart_ZSize*0.5);
    TGeoShape *backRailLowerPartS = new TGeoBBox(TString("backRailLowerPartS")+=TString("_") + frames->GetName(), backRailLowerPart_XSize*0.5, backRailLowerPart_YSize*0.5, backRailLowerPart_ZSize*0.5);
    TGeoShape *backRailSidePartS = new TGeoBBox(TString("backRailSidePartS")+=TString("_") + frames->GetName(), backRailSidePart_XSize*0.5, backRailSidePart_YSize*0.5, backRailSidePart_ZSize*0.5);

    TGeoTranslation *backRailLowerPart_pos = new TGeoTranslation();
    backRailLowerPart_pos->SetName(TString("backRailLowerPart_pos")+=TString("_") + frames->GetName());
    backRailLowerPart_pos->SetDx(0.0);
    backRailLowerPart_pos->SetDy(-(backRailMainPart_YSize*0.5 + backRailLowerPart_YSize*0.5));
    backRailLowerPart_pos->SetDz(backRailMainPart_ZSize*0.5 - backRailLowerPart_ZSize*0.5);
    backRailLowerPart_pos->RegisterYourself();

    TGeoTranslation *backRailSidePart1_pos = new TGeoTranslation();
    backRailSidePart1_pos->SetName(TString("backRailSidePart1_pos")+=TString("_") + frames->GetName());
    backRailSidePart1_pos->SetDx(+(backRailMainPart_XSize*0.5 - backRailSidePart_XSize*0.5));
    backRailSidePart1_pos->SetDy(0.0);
    backRailSidePart1_pos->SetDz(backRailMainPart_ZSize*0.5 + backRailSidePart_ZSize*0.5);
    backRailSidePart1_pos->RegisterYourself();

    TGeoTranslation *backRailSidePart2_pos = new TGeoTranslation();
    backRailSidePart2_pos->SetName(TString("backRailSidePart2_pos")+=TString("_") + frames->GetName());
    backRailSidePart2_pos->SetDx(-(backRailMainPart_XSize*0.5 - backRailSidePart_XSize*0.5));
    backRailSidePart2_pos->SetDy(0.0);
    backRailSidePart2_pos->SetDz(backRailMainPart_ZSize*0.5 + backRailSidePart_ZSize*0.5);
    backRailSidePart2_pos->RegisterYourself();

    TGeoCompositeShape *backRailS = new TGeoCompositeShape();
    backRailS->SetName(TString("backRailS")+=TString("_") + frames->GetName());
    {
        TString expression = "backRailMainPartS"; expression += TString("_") + frames->GetName();
            expression += "+backRailLowerPartS"; expression += TString("_") + frames->GetName();
            expression += ":backRailLowerPart_pos"; expression += TString("_") + frames->GetName();
            expression += "+backRailSidePartS"; expression += TString("_") + frames->GetName();
            expression += ":backRailSidePart1_pos"; expression += TString("_") + frames->GetName();
            expression += "+backRailSidePartS"; expression += TString("_") + frames->GetName();
            expression += ":backRailSidePart2_pos"; expression += TString("_") + frames->GetName();
        backRailS->MakeNode(expression);
        backRailS->ComputeBBox(); //need to compute a bounding box
    }

    TGeoVolume *backRailV = new TGeoVolume(TString("backRailV")+=TString("_") + frames->GetName(), backRailS);

    //volume medium
    TGeoMedium *backRailV_medium = pMedAluminium;
    if(backRailV_medium) {
        backRailV->SetMedium(backRailV_medium);
    }
    else Fatal("Main", "Invalid medium for backRailV!");

    //volume visual property (transparency)
    backRailV->SetLineColor(TColor::GetColor("#ffcccc"));
    backRailV->SetTransparency(50);

    TGeoCombiTrans *backRail_transf[2];

    backRail_transf[0] = new TGeoCombiTrans();
    backRail_transf[0]->SetDx(0.0);
    backRail_transf[0]->SetDy(+(14.5 + backRailMainPart_YSize*0.5));
    backRail_transf[0]->SetDz(+0.695/*center of half-plane*/ - (1.35/*half-thick of frame box*/ + faceShild_ZSize + (backRailMainPart_ZSize*0.5 - backRailLowerPart_ZSize))  + 4.31/*half-plane shift*/);

    backRail_transf[1] = new TGeoCombiTrans();
    backRail_transf[1]->RotateX(180.0);
    backRail_transf[1]->SetDx(0.0);
    backRail_transf[1]->SetDy(-(14.5 + backRailMainPart_YSize*0.5));
    backRail_transf[1]->SetDz(+0.695/*center of half-plane*/ + (1.35/*half-thick of frame box*/ + faceShild_ZSize + (backRailMainPart_ZSize*0.5 - backRailLowerPart_ZSize)));

    frames->AddNode(backRailV, 0, backRail_transf[0]);
    frames->AddNode(backRailV, 1, backRail_transf[1]);
    //----------------------------------------------------------------------

    //holding back rail ----------------------------------------------------
    Double_t holdBackRail_XSize = backRailMainPart_XSize;
    Double_t holdBackRail_YSize = 1.8; //cm
    Double_t holdBackRail_ZSize = 1.0;

    TGeoShape *holdBackRailS = new TGeoXtru(2);
    holdBackRailS->SetName("holdBackRailS");
    {
        Double_t x_pos[5] = {
            +0.0/*0*/,
            +0.1/*1*/,
            +holdBackRail_ZSize/*2*/,
            +holdBackRail_ZSize/*3*/,
            +0.0/*4*/
        };
        Double_t y_pos[5] = {
            -holdBackRail_YSize/*0*/,
            -holdBackRail_YSize/*1*/,
            -holdBackRail_YSize*0.5/*2*/,
            -0.0/*3*/,
            -0.0/*4*/,
        };
        ((TGeoXtru*)holdBackRailS)->DefinePolygon(5, x_pos, y_pos);
        ((TGeoXtru*)holdBackRailS)->DefineSection(0, -holdBackRail_XSize*0.5);
        ((TGeoXtru*)holdBackRailS)->DefineSection(1, +holdBackRail_XSize*0.5);
    }

    TGeoVolume *holdBackRailV = new TGeoVolume(TString("holdBackRailV")+=TString("_") + frames->GetName(), holdBackRailS);

    //volume medium
    TGeoMedium *holdBackRailV_medium = pMedAluminium;
    if(holdBackRailV_medium) {
        holdBackRailV->SetMedium(holdBackRailV_medium);
    }
    else Fatal("Main", "Invalid medium for holdBackRailV!");

    //volume visual property (transparency)
    holdBackRailV->SetLineColor(TColor::GetColor("#ffcccc"));
    holdBackRailV->SetTransparency(10);

    TGeoCombiTrans *holdBackRail_transf[2];

    holdBackRail_transf[0] = new TGeoCombiTrans();
    holdBackRail_transf[0]->RotateY(90.0);
    holdBackRail_transf[0]->SetDx(0.0);
    holdBackRail_transf[0]->SetDy(+(14.5 + backRailMainPart_YSize));
    holdBackRail_transf[0]->SetDz(+0.695/*center of half-plane*/ - (1.35/*half-thick of frame box*/ + faceShild_ZSize + plankShild_ZSize)  + 4.31/*half-plane shift*/);

    holdBackRail_transf[1] = new TGeoCombiTrans();
    holdBackRail_transf[1]->RotateY(90.0);
    holdBackRail_transf[1]->RotateX(180.0);
    holdBackRail_transf[1]->SetDx(0.0);
    holdBackRail_transf[1]->SetDy(-(14.5 + backRailMainPart_YSize));
    holdBackRail_transf[1]->SetDz(+0.695/*center of half-plane*/ + (1.35/*half-thick of frame box*/ + faceShild_ZSize + plankShild_ZSize));

    frames->AddNode(holdBackRailV, 0, holdBackRail_transf[0]);
    frames->AddNode(holdBackRailV, 1, holdBackRail_transf[1]);
    //----------------------------------------------------------------------

    //back plane -----------------------------------------------------------
    Double_t backPlane_XSize = (NModules/2.0)*6.3 - (NModules/2.0)*0.3 + 3.95*2/*margin*/; //cm
    Double_t backPlane_YSize = 13.1; //cm
    Double_t backPlane_ZSize = 0.15; //cm

    TGeoShape *backPlaneS = new TGeoBBox("backPlaneS", backPlane_XSize*0.5, backPlane_YSize*0.5, backPlane_ZSize*0.5);

    TGeoVolume *backPlaneV = new TGeoVolume(TString("backPlaneV")+=TString("_") + frames->GetName(), backPlaneS);

    //volume medium
    TGeoMedium *backPlaneV_medium = pMedAluminium;
    if(backPlaneV_medium) {
        backPlaneV->SetMedium(backPlaneV_medium);
    }
    else Fatal("Main", "Invalid medium for backPlaneV!");

    //volume visual property (transparency)
    backPlaneV->SetLineColor(TColor::GetColor("#ffcccc"));
    backPlaneV->SetTransparency(10);

    TGeoCombiTrans *backPlane_transf[2];

    backPlane_transf[0] = new TGeoCombiTrans();
    backPlane_transf[0]->SetDx(0.0);
    backPlane_transf[0]->SetDy(+(16.75 + backPlane_YSize*0.5));
    backPlane_transf[0]->SetDz(+0.695/*center of half-plane*/ - (1.35/*half-thick of frame box*/ + faceShild_ZSize + plankShild_ZSize + holdBackRail_ZSize + backPlane_ZSize*0.5)  + 4.31/*half-plane shift*/);

    backPlane_transf[1] = new TGeoCombiTrans();
    backPlane_transf[1]->SetDx(0.0);
    backPlane_transf[1]->SetDy(-(16.75 + backPlane_YSize*0.5));
    backPlane_transf[1]->SetDz(+0.695/*center of half-plane*/ + (1.35/*half-thick of frame box*/ + faceShild_ZSize + plankShild_ZSize + holdBackRail_ZSize + backPlane_ZSize*0.5));

    frames->AddNode(backPlaneV, 0, backPlane_transf[0]);
    frames->AddNode(backPlaneV, 1, backPlane_transf[1]);
    //----------------------------------------------------------------------

    //front plane ----------------------------------------------------------
    Double_t frontPlane_XSize = (NModules/2.0)*6.3 - (NModules/2.0)*0.3 + 1.8*2/*margin*/; //cm
    Double_t frontPlane_YSize = 10.8; //cm
    Double_t frontPlane_ZSize = 0.15; //cm

    TGeoShape *frontPlaneS = new TGeoBBox("frontPlaneS", frontPlane_XSize*0.5, frontPlane_YSize*0.5, frontPlane_ZSize*0.5);

    TGeoVolume *frontPlaneV = new TGeoVolume(TString("frontPlaneV")+=TString("_") + frames->GetName(), frontPlaneS);

    //volume medium
    TGeoMedium *frontPlaneV_medium = pMedAluminium;
    if(frontPlaneV_medium) {
        frontPlaneV->SetMedium(frontPlaneV_medium);
    }
    else Fatal("Main", "Invalid medium for frontPlaneV!");

    //volume visual property (transparency)
    frontPlaneV->SetLineColor(TColor::GetColor("#ffcccc"));
    frontPlaneV->SetTransparency(10);

    TGeoCombiTrans *frontPlane_transf[2];

    frontPlane_transf[0] = new TGeoCombiTrans();
    frontPlane_transf[0]->SetDx(0.0);
    frontPlane_transf[0]->SetDy(+(19.7 + frontPlane_YSize*0.5));
    frontPlane_transf[0]->SetDz(+0.695/*center of half-plane*/ + (1.35/*half-thick of frame box*/ + faceShild_ZSize + plankShild_ZSize + frontPlane_ZSize*0.5)  + 4.31/*half-plane shift*/);

    frontPlane_transf[1] = new TGeoCombiTrans();
    frontPlane_transf[1]->SetDx(0.0);
    frontPlane_transf[1]->SetDy(-(19.7 + frontPlane_YSize*0.5));
    frontPlane_transf[1]->SetDz(+0.695/*center of half-plane*/ - (1.35/*half-thick of frame box*/ + faceShild_ZSize + plankShild_ZSize + frontPlane_ZSize*0.5));

    frames->AddNode(frontPlaneV, 0, frontPlane_transf[0]);
    frames->AddNode(frontPlaneV, 1, frontPlane_transf[1]);
    //----------------------------------------------------------------------

    //front plane with a window --------------------------------------------
    Double_t frontPlaneWithWindow_XSize = (NModules/2.0)*6.3 - (NModules/2.0)*0.3 + 4.0*2/*margin*/; //cm
    Double_t frontPlaneWithWindow_YSize = 18.3; //cm
    Double_t frontPlaneWithWindow_ZSize = 0.3; //cm

    Double_t frontPlaneWindow_XSize = frontPlaneWithWindow_XSize - 3.0*2/*margin*/; //cm
    Double_t frontPlaneWindow_YSize = 8.6; //cm

    TGeoShape *frontPlaneWithWindowBlankS = new TGeoBBox(TString("frontPlaneWithWindowBlankS")+=TString("_") + frames->GetName(), frontPlaneWithWindow_XSize*0.5, frontPlaneWithWindow_YSize*0.5, frontPlaneWithWindow_ZSize*0.5);
    TGeoShape *frontPlaneWindowS = new TGeoBBox(TString("frontPlaneWindowS")+=TString("_") + frames->GetName(), frontPlaneWindow_XSize*0.5, frontPlaneWindow_YSize*0.5, (frontPlaneWithWindow_ZSize+0.001)*0.5);

    TGeoTranslation *frontPlaneWindow_pos = new TGeoTranslation();
    frontPlaneWindow_pos->SetName(TString("frontPlaneWindow_pos")+=TString("_") + frames->GetName());
    frontPlaneWindow_pos->SetDx(0.0);
    frontPlaneWindow_pos->SetDy(-2.85/*shift*/);
    frontPlaneWindow_pos->SetDz(0.0);
    frontPlaneWindow_pos->RegisterYourself();

    TGeoCompositeShape *frontPlaneWithWindowS = new TGeoCompositeShape();
    frontPlaneWithWindowS->SetName(TString("frontPlaneWithWindowS")+=TString("_") + frames->GetName());
    {
        TString expression = "frontPlaneWithWindowBlankS"; expression += TString("_") + frames->GetName();
            expression += "-frontPlaneWindowS"; expression += TString("_") + frames->GetName();
            expression += ":frontPlaneWindow_pos"; expression += TString("_") + frames->GetName();
        frontPlaneWithWindowS->MakeNode(expression);
        frontPlaneWithWindowS->ComputeBBox(); //need to compute a bounding box
    }

    TGeoVolume *frontPlaneWithWindowV = new TGeoVolume(TString("frontPlaneWithWindowV")+=TString("_") + frames->GetName(), frontPlaneWithWindowS);

    //volume medium
    TGeoMedium *frontPlaneWithWindowV_medium = pMedAluminium;
    if(frontPlaneWithWindowV_medium) {
        frontPlaneWithWindowV->SetMedium(frontPlaneWithWindowV_medium);
    }
    else Fatal("Main", "Invalid medium for frontPlaneWithWindowV!");

    //volume visual property (transparency)
    frontPlaneWithWindowV->SetLineColor(TColor::GetColor("#ffcccc"));
    frontPlaneWithWindowV->SetTransparency(50);

    TGeoCombiTrans *frontPlaneWithWindow_transf[2];

    frontPlaneWithWindow_transf[0] = new TGeoCombiTrans();
    frontPlaneWithWindow_transf[0]->SetDx(0.0);
    frontPlaneWithWindow_transf[0]->SetDy(+(19.5 + frontPlaneWithWindow_YSize*0.5));
    frontPlaneWithWindow_transf[0]->SetDz(+0.695/*center of half-plane*/ + (1.35/*half-thick of frame box*/ + faceShild_ZSize + frontPlaneWithWindow_ZSize*0.5)  + 4.31/*half-plane shift*/);

    frontPlaneWithWindow_transf[1] = new TGeoCombiTrans();
    frontPlaneWithWindow_transf[1]->RotateX(180.0);
    frontPlaneWithWindow_transf[1]->SetDx(0.0);
    frontPlaneWithWindow_transf[1]->SetDy(-(19.5 + frontPlaneWithWindow_YSize*0.5));
    frontPlaneWithWindow_transf[1]->SetDz(+0.695/*center of half-plane*/ - (1.35/*half-thick of frame box*/ + faceShild_ZSize + frontPlaneWithWindow_ZSize*0.5));

    frames->AddNode(frontPlaneWithWindowV, 0, frontPlaneWithWindow_transf[0]);
    frames->AddNode(frontPlaneWithWindowV, 1, frontPlaneWithWindow_transf[1]);
    //----------------------------------------------------------------------

    //PCB (Printed Circuit Board) ------------------------------------------
    Double_t pcb_XSize = (NModules/2.0)*6.3 - (NModules/2.0)*0.3 + 4.865/*margin*/; //cm
    Double_t pcb_YSize = 10.0; //cm
    Double_t pcb_ZSize = 0.2; //cm

    TGeoShape *pcbS = new TGeoBBox("pcbS", frontPlane_XSize*0.5, frontPlane_YSize*0.5, frontPlane_ZSize*0.5);

    TGeoVolume *pcbV = new TGeoVolume(TString("pcbV")+=TString("_") + frames->GetName(), pcbS);

    //volume medium
    TGeoMedium *pcbV_medium = pMedFiberGlass;
    if(pcbV_medium) {
        pcbV->SetMedium(pcbV_medium);
    }
    else Fatal("Main", "Invalid medium for pcbV!");

    //volume visual property (transparency)
    pcbV->SetLineColor(TColor::GetColor("#ccccff"));
    pcbV->SetTransparency(10);

    TGeoCombiTrans *pcb_transf[2];

    pcb_transf[0] = new TGeoCombiTrans();
    pcb_transf[0]->SetDx(0.0);
    pcb_transf[0]->SetDy(+(26.5 + pcb_YSize*0.5));
    pcb_transf[0]->SetDz(+0.695/*center of half-plane*/ + 4.31/*half-plane shift*/);

    pcb_transf[1] = new TGeoCombiTrans();
    pcb_transf[1]->SetDx(0.0);
    pcb_transf[1]->SetDy(-(26.5 + pcb_YSize*0.5));
    pcb_transf[1]->SetDz(+0.695/*center of half-plane*/);

    frames->AddNode(pcbV, 0, pcb_transf[0]);
    frames->AddNode(pcbV, 1, pcb_transf[1]);
    //----------------------------------------------------------------------

    //upper side shield ----------------------------------------------------
    Double_t upperSideShield_XSize = 4.4; //cm
    Double_t upperSideShield_YSize = 12.3; //cm
    Double_t upperSideShield_ZSize = 0.8; //cm

    Double_t upperSideShieldCut_XSize = 0.9; //cm
    Double_t upperSideShieldCut_YSize = 2.4; //cm

    Double_t upperSideShieldWindow_XSize = 3.0; //cm
    Double_t upperSideShieldWindow_YSize = 7.0; //cm

    TGeoShape *upperSideShieldBlankS = new TGeoXtru(2);
    upperSideShieldBlankS->SetName(TString("upperSideShieldBlankS")+=TString("_") + frames->GetName());
    {
        Double_t x_pos[6] = {
            +upperSideShield_XSize*0.5/*0*/,
            +upperSideShield_XSize*0.5/*1*/,
            -upperSideShield_XSize*0.5/*2*/,
            -upperSideShield_XSize*0.5/*3*/,
            -upperSideShield_XSize*0.5 + upperSideShieldCut_XSize/*4*/,
            -upperSideShield_XSize*0.5 + upperSideShieldCut_XSize/*5*/

        };
        Double_t y_pos[6] = {
            -upperSideShield_YSize*0.5/*0*/,
            +upperSideShield_YSize*0.5/*1*/,
            +upperSideShield_YSize*0.5/*2*/,
            -upperSideShield_YSize*0.5 + upperSideShieldCut_YSize/*3*/,
            -upperSideShield_YSize*0.5 + upperSideShieldCut_YSize/*4*/,
            -upperSideShield_YSize*0.5/*5*/

        };
        ((TGeoXtru*)upperSideShieldBlankS)->DefinePolygon(6, x_pos, y_pos);
        ((TGeoXtru*)upperSideShieldBlankS)->DefineSection(0, -upperSideShield_ZSize*0.5);
        ((TGeoXtru*)upperSideShieldBlankS)->DefineSection(1, +upperSideShield_ZSize*0.5);
    }

    TGeoShape *upperSideShieldWindowS = new TGeoBBox(TString("upperSideShieldWindowS")+=TString("_") + frames->GetName(), upperSideShieldWindow_XSize*0.5, upperSideShieldWindow_YSize*0.5, (upperSideShield_ZSize+0.001)*0.5);

    TGeoTranslation *upperSideShieldWindow_pos = new TGeoTranslation();
    upperSideShieldWindow_pos->SetName(TString("upperSideShieldWindow_pos")+=TString("_") + frames->GetName());
    upperSideShieldWindow_pos->SetDx(0.0);
    upperSideShieldWindow_pos->SetDy(+1.65/*shift*/);
    upperSideShieldWindow_pos->SetDz(0.0);
    upperSideShieldWindow_pos->RegisterYourself();

    TGeoCompositeShape *upperSideShieldWithWindowS = new TGeoCompositeShape();
    upperSideShieldWithWindowS->SetName(TString("upperSideShieldWithWindowS")+=TString("_") + frames->GetName());
    {
        TString expression = "upperSideShieldBlankS"; expression += TString("_") + frames->GetName();
            expression += "-upperSideShieldWindowS"; expression += TString("_") + frames->GetName();
            expression += ":upperSideShieldWindow_pos"; expression += TString("_") + frames->GetName();
        upperSideShieldWithWindowS->MakeNode(expression);
        upperSideShieldWithWindowS->ComputeBBox(); //need to compute a bounding box
    }

    TGeoVolume *upperSideShieldV = new TGeoVolume(TString("upperSideShieldV")+=TString("_") + frames->GetName(), upperSideShieldWithWindowS);

    //volume medium
    TGeoMedium *upperSideShieldV_medium = pMedAluminium;
    if(upperSideShieldV_medium) {
        upperSideShieldV->SetMedium(upperSideShieldV_medium);
    }
    else Fatal("Main", "Invalid medium for upperSideShieldV!");

    //volume visual property (transparency)
    upperSideShieldV->SetLineColor(TColor::GetColor("#ffcccc"));
    upperSideShieldV->SetTransparency(50);

    TGeoCombiTrans *upperSideShield_transf[4];

    upperSideShield_transf[0] = new TGeoCombiTrans();
    upperSideShield_transf[0]->RotateY(90.0);
    upperSideShield_transf[0]->SetDx(+(frontPlaneWithWindow_XSize*0.5 - upperSideShield_ZSize*0.5));
    upperSideShield_transf[0]->SetDy(+(17.6 + upperSideShield_YSize*0.5));
    upperSideShield_transf[0]->SetDz(+0.695/*center of half-plane*/ - 0.65/*shift*/ + 4.31/*half-plane shift*/);

    upperSideShield_transf[1] = new TGeoCombiTrans();
    upperSideShield_transf[1]->RotateY(90.0);
    upperSideShield_transf[1]->SetDx(-(frontPlaneWithWindow_XSize*0.5 - upperSideShield_ZSize*0.5));
    upperSideShield_transf[1]->SetDy(+(17.6 + upperSideShield_YSize*0.5));
    upperSideShield_transf[1]->SetDz(+0.695/*center of half-plane*/ - 0.65/*shift*/ + 4.31/*half-plane shift*/);

    upperSideShield_transf[2] = new TGeoCombiTrans();
    upperSideShield_transf[2]->RotateY(-90.0);
    upperSideShield_transf[2]->ReflectY(true);
    upperSideShield_transf[2]->SetDx(+(frontPlaneWithWindow_XSize*0.5 - upperSideShield_ZSize*0.5));
    upperSideShield_transf[2]->SetDy(-(17.6 + upperSideShield_YSize*0.5));
    upperSideShield_transf[2]->SetDz(+0.695/*center of half-plane*/ + 0.65/*shift*/);

    upperSideShield_transf[3] = new TGeoCombiTrans();
    upperSideShield_transf[3]->RotateY(-90.0);
    upperSideShield_transf[3]->ReflectY(true);
    upperSideShield_transf[3]->SetDx(-(frontPlaneWithWindow_XSize*0.5 - upperSideShield_ZSize*0.5));
    upperSideShield_transf[3]->SetDy(-(17.6 + upperSideShield_YSize*0.5));
    upperSideShield_transf[3]->SetDz(+0.695/*center of half-plane*/ + 0.65/*shift*/);

    frames->AddNode(upperSideShieldV, 0, upperSideShield_transf[0]);
    frames->AddNode(upperSideShieldV, 1, upperSideShield_transf[1]);
    frames->AddNode(upperSideShieldV, 2, upperSideShield_transf[2]);
    frames->AddNode(upperSideShieldV, 3, upperSideShield_transf[3]);
    //----------------------------------------------------------------------

    //upper side shield  cover----------------------------------------------
    Double_t upperSideShieldCover_XSize = 4.4; //cm
    Double_t upperSideShieldCover_YSize = 8.5; //cm
    Double_t upperSideShieldCover_ZSize = 0.15; //cm

    TGeoShape *upperSideShieldCoverS = new TGeoBBox("upperSideShieldCoverS", upperSideShieldCover_XSize*0.5, upperSideShieldCover_YSize*0.5, upperSideShieldCover_ZSize*0.5);

    TGeoVolume *upperSideShieldCoverV = new TGeoVolume(TString("upperSideShieldCoverV")+=TString("_") + frames->GetName(), upperSideShieldCoverS);

    //volume medium
    TGeoMedium *upperSideShieldCoverV_medium = pMedAluminium;
    if(upperSideShieldCoverV_medium) {
        upperSideShieldCoverV->SetMedium(upperSideShieldCoverV_medium);
    }
    else Fatal("Main", "Invalid medium for upperSideShieldCoverV!");

    //volume visual property (transparency)
    upperSideShieldCoverV->SetLineColor(TColor::GetColor("#ffcccc"));
    upperSideShieldCoverV->SetTransparency(10);

    TGeoCombiTrans *upperSideShieldCover_transf[4];

    upperSideShieldCover_transf[0] = new TGeoCombiTrans();
    upperSideShieldCover_transf[0]->RotateY(90.0);
    upperSideShieldCover_transf[0]->SetDx(+(frontPlaneWithWindow_XSize*0.5 + upperSideShieldCover_ZSize*0.5));
    upperSideShieldCover_transf[0]->SetDy(+(20.7 + upperSideShieldCover_YSize*0.5));
    upperSideShieldCover_transf[0]->SetDz(+0.695/*center of half-plane*/ - 0.65/*shift*/ + 4.31/*half-plane shift*/);

    upperSideShieldCover_transf[1] = new TGeoCombiTrans();
    upperSideShieldCover_transf[1]->RotateY(90.0);
    upperSideShieldCover_transf[1]->SetDx(-(frontPlaneWithWindow_XSize*0.5 + upperSideShieldCover_ZSize*0.5));
    upperSideShieldCover_transf[1]->SetDy(+(20.7 + upperSideShieldCover_YSize*0.5));
    upperSideShieldCover_transf[1]->SetDz(+0.695/*center of half-plane*/ - 0.65/*shift*/ + 4.31/*half-plane shift*/);

    upperSideShieldCover_transf[2] = new TGeoCombiTrans();
    upperSideShieldCover_transf[2]->RotateY(90.0);
    upperSideShieldCover_transf[2]->SetDx(+(frontPlaneWithWindow_XSize*0.5 + upperSideShieldCover_ZSize*0.5));
    upperSideShieldCover_transf[2]->SetDy(-(20.7 + upperSideShieldCover_YSize*0.5));
    upperSideShieldCover_transf[2]->SetDz(+0.695/*center of half-plane*/ + 0.65/*shift*/);

    upperSideShieldCover_transf[3] = new TGeoCombiTrans();
    upperSideShieldCover_transf[3]->RotateY(90.0);
    upperSideShieldCover_transf[3]->SetDx(-(frontPlaneWithWindow_XSize*0.5 + upperSideShieldCover_ZSize*0.5));
    upperSideShieldCover_transf[3]->SetDy(-(20.7 + upperSideShieldCover_YSize*0.5));
    upperSideShieldCover_transf[3]->SetDz(+0.695/*center of half-plane*/ + 0.65/*shift*/);

    frames->AddNode(upperSideShieldCoverV, 0, upperSideShieldCover_transf[0]);
    frames->AddNode(upperSideShieldCoverV, 1, upperSideShieldCover_transf[1]);
    frames->AddNode(upperSideShieldCoverV, 2, upperSideShieldCover_transf[2]);
    frames->AddNode(upperSideShieldCoverV, 3, upperSideShieldCover_transf[3]);
    //--------------------------------------------------------------------------

    return frames;
}
//------------------------------------------------------------------------------

TGeoVolume *CreateFrameForStation_LongModule(TString frame_name, Int_t current_station_num) {

    //TGeoVolume *frames = new TGeoVolumeAssembly(frame_name);

    TGeoVolume *frames = new TGeoVolumeAssembly(frame_name);
    frames->SetMedium(pMedAir);

    Int_t NModules = NModulesInStations[current_station_num];

    //face shield ----------------------------------------------------------
    Double_t faceShild_XSize = (NModules/2.0)*6.3 - (NModules/2.0)*0.3 + 8.6; //cm
    Double_t faceShild_YSize = 14.74 + 6.0; //cm
    Double_t faceShild_ZSize = 0.3; //cm

    TGeoShape *faceShildS = new TGeoXtru(2);
    faceShildS->SetName("faceShildS");
    {
        Double_t x_pos[8] = {faceShild_XSize*0.5, faceShild_XSize*0.5, -faceShild_XSize*0.5, -faceShild_XSize*0.5, -2.75, -2.3, 2.3, 2.75};
        Double_t y_pos[8] = {-faceShild_YSize*0.5, faceShild_YSize*0.5, faceShild_YSize*0.5, -faceShild_YSize*0.5, -faceShild_YSize*0.5, -3.8, -3.8, -faceShild_YSize*0.5};
        ((TGeoXtru*)faceShildS)->DefinePolygon(8, x_pos, y_pos);
        ((TGeoXtru*)faceShildS)->DefineSection(0, -faceShild_ZSize*0.5);
        ((TGeoXtru*)faceShildS)->DefineSection(1, +faceShild_ZSize*0.5);
    }

    TGeoVolume *faceShildV = new TGeoVolume(TString("faceShildV")+=TString("_") + frames->GetName(), faceShildS);

    //volume medium
    TGeoMedium *faceShildV_medium = pMedPolystyrene;
    if(faceShildV_medium) {
        faceShildV->SetMedium(faceShildV_medium);
    }
    else Fatal("Main", "Invalid medium for faceShildV!");

    //volume visual property (transparency)
    faceShildV->SetLineColor(TColor::GetColor("#ccffcc"));
    faceShildV->SetTransparency(50);

    TGeoCombiTrans *faceShild_transf[4];

    faceShild_transf[0] = new TGeoCombiTrans();
    faceShild_transf[0]->SetDy(+faceShild_YSize*0.5 - 0.95);
    faceShild_transf[0]->SetDz(+0.695/*center of half-plane*/ - 1.35/*half-thick of frame box*/ - faceShild_ZSize*0.5/*face shield shift*/ + 4.31/*half-plane shift*/);

    faceShild_transf[1] = new TGeoCombiTrans();
    faceShild_transf[1]->SetDy(+faceShild_YSize*0.5 - 0.95);
    faceShild_transf[1]->SetDz(+0.695/*center of half-plane*/ + 1.35/*half-thick of frame box*/ + faceShild_ZSize*0.5/*face shield shift*/ + 4.31/*half-plane shift*/);

    faceShild_transf[2] = new TGeoCombiTrans();
    faceShild_transf[2]->RotateX(180.0);
    faceShild_transf[2]->SetDy(-faceShild_YSize*0.5 + 0.95);
    faceShild_transf[2]->SetDz(+0.695/*center of half-plane*/ - 1.35/*half-thick of frame box*/ - faceShild_ZSize*0.5/*face shield shift*/);

    faceShild_transf[3] = new TGeoCombiTrans();
    faceShild_transf[3]->RotateX(180.0);
    faceShild_transf[3]->SetDy(-faceShild_YSize*0.5 + 0.95);
    faceShild_transf[3]->SetDz(+0.695/*center of half-plane*/ + 1.35/*half-thick of frame box*/ + faceShild_ZSize*0.5/*face shield shift*/);

    frames->AddNode(faceShildV, 0, faceShild_transf[0]);
    frames->AddNode(faceShildV, 1, faceShild_transf[1]);
    frames->AddNode(faceShildV, 2, faceShild_transf[2]);
    frames->AddNode(faceShildV, 3, faceShild_transf[3]);
    //----------------------------------------------------------------------

    //sidewall of shield ---------------------------------------------------
    Double_t sideShild_XSize = 0.95; //cm
    Double_t sideShild_YSize = 15.2 + 6.0; //cm
    Double_t sideShild_ZSize = 2.7; //cm

    TGeoShape *sideShieldS = new TGeoBBox("sideShieldS", sideShild_XSize*0.5, sideShild_YSize*0.5, sideShild_ZSize*0.5);

    TGeoVolume *sideShieldV =  new TGeoVolume(TString("sideShieldV")+=TString("_") + frames->GetName(), sideShieldS);

    //volume medium
    TGeoMedium *sideShieldV_medium = pMedAluminium;
    if(sideShieldV_medium) {
        sideShieldV->SetMedium(sideShieldV_medium);
    }
    else Fatal("Main", "Invalid medium for sideShieldV!");

    //volume visual property (transparency)
    sideShieldV->SetLineColor(TColor::GetColor("#ffcccc"));
    sideShieldV->SetTransparency(50);

    TGeoCombiTrans *sideShild_transf[4];

    sideShild_transf[0] = new TGeoCombiTrans();
    sideShild_transf[0]->SetDx(+(faceShild_XSize*0.5 - sideShild_XSize*0.5));
    sideShild_transf[0]->SetDy(+faceShild_YSize*0.5 - 0.95 + (sideShild_YSize - faceShild_YSize)*0.5);
    sideShild_transf[0]->SetDz(+0.695/*center of half-plane*/ + 4.31/*half-plane shift*/);

    sideShild_transf[1] = new TGeoCombiTrans();
    sideShild_transf[1]->SetDx(-(faceShild_XSize*0.5 - sideShild_XSize*0.5));
    sideShild_transf[1]->SetDy(+faceShild_YSize*0.5 - 0.95 + (sideShild_YSize - faceShild_YSize)*0.5);
    sideShild_transf[1]->SetDz(+0.695/*center of half-plane*/ + 4.31/*half-plane shift*/);

    sideShild_transf[2] = new TGeoCombiTrans();
    sideShild_transf[2]->SetDx(+(faceShild_XSize*0.5 - sideShild_XSize*0.5));
    sideShild_transf[2]->SetDy(-faceShild_YSize*0.5 + 0.95 - (sideShild_YSize - faceShild_YSize)*0.5);
    sideShild_transf[2]->SetDz(+0.695/*center of half-plane*/);

    sideShild_transf[3] = new TGeoCombiTrans();
    sideShild_transf[3]->SetDx(-(faceShild_XSize*0.5 - sideShild_XSize*0.5));
    sideShild_transf[3]->SetDy(-faceShild_YSize*0.5 + 0.95 - (sideShild_YSize - faceShild_YSize)*0.5);
    sideShild_transf[3]->SetDz(+0.695/*center of half-plane*/);

    frames->AddNode(sideShieldV, 0, sideShild_transf[0]);
    frames->AddNode(sideShieldV, 1, sideShild_transf[1]);
    frames->AddNode(sideShieldV, 2, sideShild_transf[2]);
    frames->AddNode(sideShieldV, 3, sideShild_transf[3]);
    //----------------------------------------------------------------------

    //horizontal plane of shield -------------------------------------------
    Double_t horizShild_XSize = faceShild_XSize*0.5 - sideShild_XSize - 2.75/*half-size of hole*/; //cm
    Double_t horizShild_YSize = 0.3; //cm
    Double_t horizShild_ZSize = 2.7; //cm

    TGeoShape *horizShieldS = new TGeoBBox("horizShieldS", horizShild_XSize*0.5, horizShild_YSize*0.5, horizShild_ZSize*0.5);

    TGeoVolume *horizShieldV = new TGeoVolume(TString("horizShieldV")+=TString("_") + frames->GetName(), horizShieldS);

    //volume medium
    TGeoMedium *horizShieldV_medium = pMedPolystyrene;
    if(horizShieldV_medium) {
        horizShieldV->SetMedium(horizShieldV_medium);
    }
    else Fatal("Main", "Invalid medium for horizShieldV!");

    //volume visual property (transparency)
    horizShieldV->SetLineColor(TColor::GetColor("#ccffcc"));
    horizShieldV->SetTransparency(50);

    TGeoCombiTrans *horizShild_transf[4];

    horizShild_transf[0] = new TGeoCombiTrans();
    horizShild_transf[0]->SetDx(+(faceShild_XSize*0.5 - horizShild_XSize*0.5 - sideShild_XSize));
    horizShild_transf[0]->SetDy(+(-0.95 + horizShild_YSize*0.5));
    horizShild_transf[0]->SetDz(+0.695/*center of half-plane*/ + 4.31/*half-plane shift*/);

    horizShild_transf[1] = new TGeoCombiTrans();
    horizShild_transf[1]->SetDx(-(faceShild_XSize*0.5 - horizShild_XSize*0.5 - sideShild_XSize));
    horizShild_transf[1]->SetDy(+(-0.95 + horizShild_YSize*0.5));
    horizShild_transf[1]->SetDz(+0.695/*center of half-plane*/ + 4.31/*half-plane shift*/);

    horizShild_transf[2] = new TGeoCombiTrans();
    horizShild_transf[2]->SetDx(+(faceShild_XSize*0.5 - horizShild_XSize*0.5 - sideShild_XSize));
    horizShild_transf[2]->SetDy(-(-0.95 + horizShild_YSize*0.5));
    horizShild_transf[2]->SetDz(+0.695/*center of half-plane*/);

    horizShild_transf[3] = new TGeoCombiTrans();
    horizShild_transf[3]->SetDx(-(faceShild_XSize*0.5 - horizShild_XSize*0.5 - sideShild_XSize));
    horizShild_transf[3]->SetDy(-(-0.95 + horizShild_YSize*0.5));
    horizShild_transf[3]->SetDz(+0.695/*center of half-plane*/);

    frames->AddNode(horizShieldV, 0, horizShild_transf[0]);
    frames->AddNode(horizShieldV, 1, horizShild_transf[1]);
    frames->AddNode(horizShieldV, 2, horizShild_transf[2]);
    frames->AddNode(horizShieldV, 3, horizShild_transf[3]);
    //----------------------------------------------------------------------

    //plank of shield ------------------------------------------------------
    Double_t plankShild_XSize = faceShild_XSize; //cm
    Double_t plankShild_YSize = 1.5; //cm
    Double_t plankShild_ZSize = 0.3; //cm

    TGeoShape *plankShieldS = new TGeoBBox("plankShieldS", plankShild_XSize*0.5, plankShild_YSize*0.5, plankShild_ZSize*0.5);

    TGeoVolume *plankShieldV = new TGeoVolume(TString("plankShieldV")+=TString("_") + frames->GetName(), plankShieldS);

    //volume medium
    TGeoMedium *plankShieldV_medium = pMedAluminium;
    if(plankShieldV_medium) {
        plankShieldV->SetMedium(plankShieldV_medium);
    }
    else Fatal("Main", "Invalid medium for plankShieldV!");

    //volume visual property (transparency)
    plankShieldV->SetLineColor(TColor::GetColor("#ffcccc"));
    plankShieldV->SetTransparency(50);

    TGeoCombiTrans *plankShild_transf[4];

    plankShild_transf[0] = new TGeoCombiTrans();
    plankShild_transf[0]->SetDx(0.0);
    plankShild_transf[0]->SetDy(+(-0.95 + faceShild_YSize) - 0.05);
    plankShild_transf[0]->SetDz(+0.695/*center of half-plane*/ - (1.35/*half-thick of frame box*/ + faceShild_ZSize + plankShild_ZSize*0.5) + 4.31/*half-plane shift*/);

    plankShild_transf[1] = new TGeoCombiTrans();
    plankShild_transf[1]->SetDx(0.0);
    plankShild_transf[1]->SetDy(+(-0.95 + faceShild_YSize) - 0.05);
    plankShild_transf[1]->SetDz(+0.695/*center of half-plane*/ + (1.35/*half-thick of frame box*/ + faceShild_ZSize + plankShild_ZSize*0.5) + 4.31/*half-plane shift*/);

    plankShild_transf[2] = new TGeoCombiTrans();
    plankShild_transf[2]->SetDx(0.0);
    plankShild_transf[2]->SetDy(-(-0.95 + faceShild_YSize) + 0.05);
    plankShild_transf[2]->SetDz(+0.695/*center of half-plane*/ - (1.35/*half-thick of frame box*/ + faceShild_ZSize + plankShild_ZSize*0.5));

    plankShild_transf[3] = new TGeoCombiTrans();
    plankShild_transf[3]->SetDx(0.0);
    plankShild_transf[3]->SetDy(-(-0.95 + faceShild_YSize) + 0.05);
    plankShild_transf[3]->SetDz(+0.695/*center of half-plane*/ + (1.35/*half-thick of frame box*/ + faceShild_ZSize + plankShild_ZSize*0.5));

    frames->AddNode(plankShieldV, 0, plankShild_transf[0]);
    frames->AddNode(plankShieldV, 1, plankShild_transf[1]);
    frames->AddNode(plankShieldV, 2, plankShild_transf[2]);
    frames->AddNode(plankShieldV, 3, plankShild_transf[3]);
    //----------------------------------------------------------------------

    //base rail
    Double_t baseRailMainPart_XSize = faceShild_XSize + 3.4*2/*margin*/; //cm
    Double_t baseRailMainPart_YSize = 5.0; //cm
    Double_t baseRailMainPart_ZSize = 0.7; //cm

    cout << "baseRailMainPart_XSize = " << baseRailMainPart_XSize << "\n";

    Double_t baseRailUpperPart_XSize = baseRailMainPart_XSize; //cm
    Double_t baseRailUpperPart_YSize = 1.0; //cm
    Double_t baseRailUpperPart_ZSize = 0.3; //cm

    Double_t baseRailLowerPart_XSize = baseRailMainPart_XSize-8.8/*cut*/; //cm
    Double_t baseRailLowerPart_YSize = 0.7; //cm
    Double_t baseRailLowerPart_ZSize = 0.3; //cm

    TGeoShape *baseRailMainPartS = new TGeoBBox(TString("baseRailMainPartS")+=TString("_") + frames->GetName(), baseRailMainPart_XSize*0.5, baseRailMainPart_YSize*0.5, baseRailMainPart_ZSize*0.5);
    TGeoShape *baseRailUpperPartS = new TGeoBBox(TString("baseRailUpperPartS")+=TString("_") + frames->GetName(), baseRailUpperPart_XSize*0.5, baseRailUpperPart_YSize*0.5, baseRailUpperPart_ZSize*0.5);
    TGeoShape *baseRailLowerPartS = new TGeoBBox(TString("baseRailLowerPartS")+=TString("_") + frames->GetName(), baseRailLowerPart_XSize*0.5, baseRailLowerPart_YSize*0.5, baseRailLowerPart_ZSize*0.5);

    TGeoTranslation *baseRailUpperPart_pos = new TGeoTranslation();
    baseRailUpperPart_pos->SetName(TString("baseRailUpperPart_pos")+=TString("_") + frames->GetName());
    baseRailUpperPart_pos->SetDx(0.0);
    baseRailUpperPart_pos->SetDy(+(baseRailMainPart_YSize*0.5 + baseRailUpperPart_YSize*0.5));
    baseRailUpperPart_pos->SetDz(baseRailMainPart_ZSize*0.5 - baseRailUpperPart_ZSize*0.5);
    baseRailUpperPart_pos->RegisterYourself();

    TGeoTranslation *baseRailLowerPart_pos = new TGeoTranslation();
    baseRailLowerPart_pos->SetName(TString("baseRailLowerPart_pos")+=TString("_") + frames->GetName());
    baseRailLowerPart_pos->SetDx(0.0);
    baseRailLowerPart_pos->SetDy(-(baseRailMainPart_YSize*0.5 + baseRailLowerPart_YSize*0.5));
    baseRailLowerPart_pos->SetDz(baseRailMainPart_ZSize*0.5 - baseRailLowerPart_ZSize*0.5);
    baseRailLowerPart_pos->RegisterYourself();

    TGeoCompositeShape *baseRailS = new TGeoCompositeShape();
    baseRailS->SetName(TString("baseRailS")+=TString("_") + frames->GetName());
    {
        TString expression = "baseRailMainPartS"; expression += TString("_") + frames->GetName();
            expression += "+baseRailUpperPartS"; expression += TString("_") + frames->GetName();
            expression += ":baseRailUpperPart_pos"; expression += TString("_") + frames->GetName();
            expression += "+baseRailLowerPartS"; expression += TString("_") + frames->GetName();
            expression += ":baseRailLowerPart_pos"; expression += TString("_") + frames->GetName();
        baseRailS->MakeNode(expression);
        baseRailS->ComputeBBox(); //need to compute a bounding box
    }

    TGeoVolume *baseRailV = new TGeoVolume(TString("baseRailV")+=TString("_") + frames->GetName(), baseRailS);

    //volume medium
    TGeoMedium *baseRailV_medium = pMedAluminium;
    if(baseRailV_medium) {
        baseRailV->SetMedium(baseRailV_medium);
    }
    else Fatal("Main", "Invalid medium for baseRailV!");

    //volume visual property (transparency)
    baseRailV->SetLineColor(TColor::GetColor("#ffcccc"));
    baseRailV->SetTransparency(50);

    TGeoCombiTrans *baseRail_transf[2];

    baseRail_transf[0] = new TGeoCombiTrans();
    baseRail_transf[0]->RotateY(180.0);
    baseRail_transf[0]->SetDx(0.1);
    baseRail_transf[0]->SetDy(+(14.5 + baseRailMainPart_YSize*0.5 + 6.0));
    baseRail_transf[0]->SetDz(+0.695/*center of half-plane*/ + (1.35/*half-thick of frame box*/ + faceShild_ZSize  +baseRailMainPart_ZSize*0.5 - baseRailLowerPart_ZSize)  + 4.31/*half-plane shift*/);

    baseRail_transf[1] = new TGeoCombiTrans();
    baseRail_transf[1]->ReflectY(true);
    baseRail_transf[1]->SetDx(0.0);
    baseRail_transf[1]->SetDy(-(14.5 + baseRailMainPart_YSize*0.5 + 6.0));
    baseRail_transf[1]->SetDz(+0.695/*center of half-plane*/ - (1.35/*half-thick of frame box*/ + faceShild_ZSize  +baseRailMainPart_ZSize*0.5 - baseRailLowerPart_ZSize));

    frames->AddNode(baseRailV, 0, baseRail_transf[0]);
    frames->AddNode(baseRailV, 1, baseRail_transf[1]);
    //----------------------------------------------------------------------

    //back rail ------------------------------------------------------------
    Double_t backRailMainPart_XSize = faceShild_XSize + - 0.3*2/*margin*/; //cm
    Double_t backRailMainPart_YSize = 3.0; //cm
    Double_t backRailMainPart_ZSize = 0.9; //cm, average 6..12 cm

    Double_t backRailLowerPart_XSize = backRailMainPart_XSize - 1.4; //cm
    Double_t backRailLowerPart_YSize = 0.7; //cm
    Double_t backRailLowerPart_ZSize = 0.6; //cm

    Double_t backRailSidePart_XSize = 1.2; //cm
    Double_t backRailSidePart_YSize = 3.0; //cm
    Double_t backRailSidePart_ZSize = 1.62; //cm

    TGeoShape *backRailMainPartS = new TGeoBBox(TString("backRailMainPartS")+=TString("_") + frames->GetName(), backRailMainPart_XSize*0.5, backRailMainPart_YSize*0.5, backRailMainPart_ZSize*0.5);
    TGeoShape *backRailLowerPartS = new TGeoBBox(TString("backRailLowerPartS")+=TString("_") + frames->GetName(), backRailLowerPart_XSize*0.5, backRailLowerPart_YSize*0.5, backRailLowerPart_ZSize*0.5);
    TGeoShape *backRailSidePartS = new TGeoBBox(TString("backRailSidePartS")+=TString("_") + frames->GetName(), backRailSidePart_XSize*0.5, backRailSidePart_YSize*0.5, backRailSidePart_ZSize*0.5);

    TGeoTranslation *backRailLowerPart_pos = new TGeoTranslation();
    backRailLowerPart_pos->SetName(TString("backRailLowerPart_pos")+=TString("_") + frames->GetName());
    backRailLowerPart_pos->SetDx(0.0);
    backRailLowerPart_pos->SetDy(-(backRailMainPart_YSize*0.5 + backRailLowerPart_YSize*0.5));
    backRailLowerPart_pos->SetDz(backRailMainPart_ZSize*0.5 - backRailLowerPart_ZSize*0.5);
    backRailLowerPart_pos->RegisterYourself();

    TGeoTranslation *backRailSidePart1_pos = new TGeoTranslation();
    backRailSidePart1_pos->SetName(TString("backRailSidePart1_pos")+=TString("_") + frames->GetName());
    backRailSidePart1_pos->SetDx(+(backRailMainPart_XSize*0.5 - backRailSidePart_XSize*0.5));
    backRailSidePart1_pos->SetDy(0.0);
    backRailSidePart1_pos->SetDz(backRailMainPart_ZSize*0.5 + backRailSidePart_ZSize*0.5);
    backRailSidePart1_pos->RegisterYourself();

    TGeoTranslation *backRailSidePart2_pos = new TGeoTranslation();
    backRailSidePart2_pos->SetName(TString("backRailSidePart2_pos")+=TString("_") + frames->GetName());
    backRailSidePart2_pos->SetDx(-(backRailMainPart_XSize*0.5 - backRailSidePart_XSize*0.5));
    backRailSidePart2_pos->SetDy(0.0);
    backRailSidePart2_pos->SetDz(backRailMainPart_ZSize*0.5 + backRailSidePart_ZSize*0.5);
    backRailSidePart2_pos->RegisterYourself();

    TGeoCompositeShape *backRailS = new TGeoCompositeShape();
    backRailS->SetName(TString("backRailS")+=TString("_") + frames->GetName());
    {
        TString expression = "backRailMainPartS"; expression += TString("_") + frames->GetName();
            expression += "+backRailLowerPartS"; expression += TString("_") + frames->GetName();
            expression += ":backRailLowerPart_pos"; expression += TString("_") + frames->GetName();
            expression += "+backRailSidePartS"; expression += TString("_") + frames->GetName();
            expression += ":backRailSidePart1_pos"; expression += TString("_") + frames->GetName();
            expression += "+backRailSidePartS"; expression += TString("_") + frames->GetName();
            expression += ":backRailSidePart2_pos"; expression += TString("_") + frames->GetName();
        backRailS->MakeNode(expression);
        backRailS->ComputeBBox(); //need to compute a bounding box
    }

    TGeoVolume *backRailV = new TGeoVolume(TString("backRailV")+=TString("_") + frames->GetName(), backRailS);

    //volume medium
    TGeoMedium *backRailV_medium = pMedAluminium;
    if(backRailV_medium) {
        backRailV->SetMedium(backRailV_medium);
    }
    else Fatal("Main", "Invalid medium for backRailV!");

    //volume visual property (transparency)
    backRailV->SetLineColor(TColor::GetColor("#ffcccc"));
    backRailV->SetTransparency(50);

    TGeoCombiTrans *backRail_transf[2];

    backRail_transf[0] = new TGeoCombiTrans();
    backRail_transf[0]->SetDx(0.0);
    backRail_transf[0]->SetDy(+(14.5 + backRailMainPart_YSize*0.5 + 6.0));
    backRail_transf[0]->SetDz(+0.695/*center of half-plane*/ - (1.35/*half-thick of frame box*/ + faceShild_ZSize + (backRailMainPart_ZSize*0.5 - backRailLowerPart_ZSize))  + 4.31/*half-plane shift*/);

    backRail_transf[1] = new TGeoCombiTrans();
    backRail_transf[1]->RotateX(180.0);
    backRail_transf[1]->SetDx(0.0);
    backRail_transf[1]->SetDy(-(14.5 + backRailMainPart_YSize*0.5 + 6.0));
    backRail_transf[1]->SetDz(+0.695/*center of half-plane*/ + (1.35/*half-thick of frame box*/ + faceShild_ZSize + (backRailMainPart_ZSize*0.5 - backRailLowerPart_ZSize)));

    frames->AddNode(backRailV, 0, backRail_transf[0]);
    frames->AddNode(backRailV, 1, backRail_transf[1]);
    //----------------------------------------------------------------------

    //holding back rail ----------------------------------------------------
    Double_t holdBackRail_XSize = backRailMainPart_XSize;
    Double_t holdBackRail_YSize = 1.8; //cm
    Double_t holdBackRail_ZSize = 1.0;

    TGeoShape *holdBackRailS = new TGeoXtru(2);
    holdBackRailS->SetName("holdBackRailS");
    {
        Double_t x_pos[5] = {
            +0.0/*0*/,
            +0.1/*1*/,
            +holdBackRail_ZSize/*2*/,
            +holdBackRail_ZSize/*3*/,
            +0.0/*4*/
        };
        Double_t y_pos[5] = {
            -holdBackRail_YSize/*0*/,
            -holdBackRail_YSize/*1*/,
            -holdBackRail_YSize*0.5/*2*/,
            -0.0/*3*/,
            -0.0/*4*/,
        };
        ((TGeoXtru*)holdBackRailS)->DefinePolygon(5, x_pos, y_pos);
        ((TGeoXtru*)holdBackRailS)->DefineSection(0, -holdBackRail_XSize*0.5);
        ((TGeoXtru*)holdBackRailS)->DefineSection(1, +holdBackRail_XSize*0.5);
    }

    TGeoVolume *holdBackRailV = new TGeoVolume(TString("holdBackRailV")+=TString("_") + frames->GetName(), holdBackRailS);

    //volume medium
    TGeoMedium *holdBackRailV_medium = pMedAluminium;
    if(holdBackRailV_medium) {
        holdBackRailV->SetMedium(holdBackRailV_medium);
    }
    else Fatal("Main", "Invalid medium for holdBackRailV!");

    //volume visual property (transparency)
    holdBackRailV->SetLineColor(TColor::GetColor("#ffcccc"));
    holdBackRailV->SetTransparency(10);

    TGeoCombiTrans *holdBackRail_transf[2];

    holdBackRail_transf[0] = new TGeoCombiTrans();
    holdBackRail_transf[0]->RotateY(90.0);
    holdBackRail_transf[0]->SetDx(0.0);
    holdBackRail_transf[0]->SetDy(+(14.5 + backRailMainPart_YSize + 6.0));
    holdBackRail_transf[0]->SetDz(+0.695/*center of half-plane*/ - (1.35/*half-thick of frame box*/ + faceShild_ZSize + plankShild_ZSize)  + 4.31/*half-plane shift*/);

    holdBackRail_transf[1] = new TGeoCombiTrans();
    holdBackRail_transf[1]->RotateY(90.0);
    holdBackRail_transf[1]->RotateX(180.0);
    holdBackRail_transf[1]->SetDx(0.0);
    holdBackRail_transf[1]->SetDy(-(14.5 + backRailMainPart_YSize + 6.0));
    holdBackRail_transf[1]->SetDz(+0.695/*center of half-plane*/ + (1.35/*half-thick of frame box*/ + faceShild_ZSize + plankShild_ZSize));

    frames->AddNode(holdBackRailV, 0, holdBackRail_transf[0]);
    frames->AddNode(holdBackRailV, 1, holdBackRail_transf[1]);
    //----------------------------------------------------------------------

    //back plane -----------------------------------------------------------
    Double_t backPlane_XSize = (NModules/2.0)*6.3 - (NModules/2.0)*0.3 + 3.95*2/*margin*/; //cm
    Double_t backPlane_YSize = 13.1; //cm
    Double_t backPlane_ZSize = 0.15; //cm

    TGeoShape *backPlaneS = new TGeoBBox("backPlaneS", backPlane_XSize*0.5, backPlane_YSize*0.5, backPlane_ZSize*0.5);

    TGeoVolume *backPlaneV = new TGeoVolume(TString("backPlaneV")+=TString("_") + frames->GetName(), backPlaneS);

    //volume medium
    TGeoMedium *backPlaneV_medium = pMedAluminium;
    if(backPlaneV_medium) {
        backPlaneV->SetMedium(backPlaneV_medium);
    }
    else Fatal("Main", "Invalid medium for backPlaneV!");

    //volume visual property (transparency)
    backPlaneV->SetLineColor(TColor::GetColor("#ffcccc"));
    backPlaneV->SetTransparency(10);

    TGeoCombiTrans *backPlane_transf[2];

    backPlane_transf[0] = new TGeoCombiTrans();
    backPlane_transf[0]->SetDx(0.0);
    backPlane_transf[0]->SetDy(+(16.75 + backPlane_YSize*0.5 + 6.0));
    backPlane_transf[0]->SetDz(+0.695/*center of half-plane*/ - (1.35/*half-thick of frame box*/ + faceShild_ZSize + plankShild_ZSize + holdBackRail_ZSize + backPlane_ZSize*0.5)  + 4.31/*half-plane shift*/);

    backPlane_transf[1] = new TGeoCombiTrans();
    backPlane_transf[1]->SetDx(0.0);
    backPlane_transf[1]->SetDy(-(16.75 + backPlane_YSize*0.5 + 6.0));
    backPlane_transf[1]->SetDz(+0.695/*center of half-plane*/ + (1.35/*half-thick of frame box*/ + faceShild_ZSize + plankShild_ZSize + holdBackRail_ZSize + backPlane_ZSize*0.5));

    frames->AddNode(backPlaneV, 0, backPlane_transf[0]);
    frames->AddNode(backPlaneV, 1, backPlane_transf[1]);
    //----------------------------------------------------------------------

    //front plane ----------------------------------------------------------
    Double_t frontPlane_XSize = (NModules/2.0)*6.3 - (NModules/2.0)*0.3 + 1.8*2/*margin*/; //cm
    Double_t frontPlane_YSize = 10.8; //cm
    Double_t frontPlane_ZSize = 0.15; //cm

    TGeoShape *frontPlaneS = new TGeoBBox("frontPlaneS", frontPlane_XSize*0.5, frontPlane_YSize*0.5, frontPlane_ZSize*0.5);

    TGeoVolume *frontPlaneV = new TGeoVolume(TString("frontPlaneV")+=TString("_") + frames->GetName(), frontPlaneS);

    //volume medium
    TGeoMedium *frontPlaneV_medium = pMedAluminium;
    if(frontPlaneV_medium) {
        frontPlaneV->SetMedium(frontPlaneV_medium);
    }
    else Fatal("Main", "Invalid medium for frontPlaneV!");

    //volume visual property (transparency)
    frontPlaneV->SetLineColor(TColor::GetColor("#ffcccc"));
    frontPlaneV->SetTransparency(10);

    TGeoCombiTrans *frontPlane_transf[2];

    frontPlane_transf[0] = new TGeoCombiTrans();
    frontPlane_transf[0]->SetDx(0.0);
    frontPlane_transf[0]->SetDy(+(19.7 + frontPlane_YSize*0.5 + 6.0));
    frontPlane_transf[0]->SetDz(+0.695/*center of half-plane*/ + (1.35/*half-thick of frame box*/ + faceShild_ZSize + plankShild_ZSize + frontPlane_ZSize*0.5)  + 4.31/*half-plane shift*/);

    frontPlane_transf[1] = new TGeoCombiTrans();
    frontPlane_transf[1]->SetDx(0.0);
    frontPlane_transf[1]->SetDy(-(19.7 + frontPlane_YSize*0.5 + 6.0));
    frontPlane_transf[1]->SetDz(+0.695/*center of half-plane*/ - (1.35/*half-thick of frame box*/ + faceShild_ZSize + plankShild_ZSize + frontPlane_ZSize*0.5));

    frames->AddNode(frontPlaneV, 0, frontPlane_transf[0]);
    frames->AddNode(frontPlaneV, 1, frontPlane_transf[1]);
    //----------------------------------------------------------------------

    //front plane with a window --------------------------------------------
    Double_t frontPlaneWithWindow_XSize = (NModules/2.0)*6.3 - (NModules/2.0)*0.3 + 4.0*2/*margin*/; //cm
    Double_t frontPlaneWithWindow_YSize = 18.3; //cm
    Double_t frontPlaneWithWindow_ZSize = 0.3; //cm

    Double_t frontPlaneWindow_XSize = frontPlaneWithWindow_XSize - 3.0*2/*margin*/; //cm
    Double_t frontPlaneWindow_YSize = 8.6; //cm

    TGeoShape *frontPlaneWithWindowBlankS = new TGeoBBox(TString("frontPlaneWithWindowBlankS")+=TString("_") + frames->GetName(), frontPlaneWithWindow_XSize*0.5, frontPlaneWithWindow_YSize*0.5, frontPlaneWithWindow_ZSize*0.5);
    TGeoShape *frontPlaneWindowS = new TGeoBBox(TString("frontPlaneWindowS")+=TString("_") + frames->GetName(), frontPlaneWindow_XSize*0.5, frontPlaneWindow_YSize*0.5, (frontPlaneWithWindow_ZSize+0.001)*0.5);

    TGeoTranslation *frontPlaneWindow_pos = new TGeoTranslation();
    frontPlaneWindow_pos->SetName(TString("frontPlaneWindow_pos")+=TString("_") + frames->GetName());
    frontPlaneWindow_pos->SetDx(0.0);
    frontPlaneWindow_pos->SetDy(-2.85/*shift*/);
    frontPlaneWindow_pos->SetDz(0.0);
    frontPlaneWindow_pos->RegisterYourself();

    TGeoCompositeShape *frontPlaneWithWindowS = new TGeoCompositeShape();
    frontPlaneWithWindowS->SetName(TString("frontPlaneWithWindowS")+=TString("_") + frames->GetName());
    {
        TString expression = "frontPlaneWithWindowBlankS"; expression += TString("_") + frames->GetName();
            expression += "-frontPlaneWindowS"; expression += TString("_") + frames->GetName();
            expression += ":frontPlaneWindow_pos"; expression += TString("_") + frames->GetName();
        frontPlaneWithWindowS->MakeNode(expression);
        frontPlaneWithWindowS->ComputeBBox(); //need to compute a bounding box
    }

    TGeoVolume *frontPlaneWithWindowV = new TGeoVolume(TString("frontPlaneWithWindowV")+=TString("_") + frames->GetName(), frontPlaneWithWindowS);

    //volume medium
    TGeoMedium *frontPlaneWithWindowV_medium = pMedAluminium;
    if(frontPlaneWithWindowV_medium) {
        frontPlaneWithWindowV->SetMedium(frontPlaneWithWindowV_medium);
    }
    else Fatal("Main", "Invalid medium for frontPlaneWithWindowV!");

    //volume visual property (transparency)
    frontPlaneWithWindowV->SetLineColor(TColor::GetColor("#ffcccc"));
    frontPlaneWithWindowV->SetTransparency(50);

    TGeoCombiTrans *frontPlaneWithWindow_transf[2];

    frontPlaneWithWindow_transf[0] = new TGeoCombiTrans();
    frontPlaneWithWindow_transf[0]->SetDx(0.0);
    frontPlaneWithWindow_transf[0]->SetDy(+(19.5 + frontPlaneWithWindow_YSize*0.5 + 6.0));
    frontPlaneWithWindow_transf[0]->SetDz(+0.695/*center of half-plane*/ + (1.35/*half-thick of frame box*/ + faceShild_ZSize + frontPlaneWithWindow_ZSize*0.5)  + 4.31/*half-plane shift*/);

    frontPlaneWithWindow_transf[1] = new TGeoCombiTrans();
    frontPlaneWithWindow_transf[1]->RotateX(180.0);
    frontPlaneWithWindow_transf[1]->SetDx(0.0);
    frontPlaneWithWindow_transf[1]->SetDy(-(19.5 + frontPlaneWithWindow_YSize*0.5 + 6.0));
    frontPlaneWithWindow_transf[1]->SetDz(+0.695/*center of half-plane*/ - (1.35/*half-thick of frame box*/ + faceShild_ZSize + frontPlaneWithWindow_ZSize*0.5));

    frames->AddNode(frontPlaneWithWindowV, 0, frontPlaneWithWindow_transf[0]);
    frames->AddNode(frontPlaneWithWindowV, 1, frontPlaneWithWindow_transf[1]);
    //----------------------------------------------------------------------

    //PCB (Printed Circuit Board) ------------------------------------------
    Double_t pcb_XSize = (NModules/2.0)*6.3 - (NModules/2.0)*0.3 + 4.865/*margin*/; //cm
    Double_t pcb_YSize = 10.0; //cm
    Double_t pcb_ZSize = 0.2; //cm

    TGeoShape *pcbS = new TGeoBBox("pcbS", frontPlane_XSize*0.5, frontPlane_YSize*0.5, frontPlane_ZSize*0.5);

    TGeoVolume *pcbV = new TGeoVolume(TString("pcbV")+=TString("_") + frames->GetName(), pcbS);

    //volume medium
    TGeoMedium *pcbV_medium = pMedFiberGlass;
    if(pcbV_medium) {
        pcbV->SetMedium(pcbV_medium);
    }
    else Fatal("Main", "Invalid medium for pcbV!");

    //volume visual property (transparency)
    pcbV->SetLineColor(TColor::GetColor("#ccccff"));
    pcbV->SetTransparency(10);

    TGeoCombiTrans *pcb_transf[2];

    pcb_transf[0] = new TGeoCombiTrans();
    pcb_transf[0]->SetDx(0.0);
    pcb_transf[0]->SetDy(+(26.5 + pcb_YSize*0.5 + 6.0));
    pcb_transf[0]->SetDz(+0.695/*center of half-plane*/ + 4.31/*half-plane shift*/);

    pcb_transf[1] = new TGeoCombiTrans();
    pcb_transf[1]->SetDx(0.0);
    pcb_transf[1]->SetDy(-(26.5 + pcb_YSize*0.5 + 6.0));
    pcb_transf[1]->SetDz(+0.695/*center of half-plane*/);

    frames->AddNode(pcbV, 0, pcb_transf[0]);
    frames->AddNode(pcbV, 1, pcb_transf[1]);
    //----------------------------------------------------------------------

    //upper side shield ----------------------------------------------------
    Double_t upperSideShield_XSize = 4.4; //cm
    Double_t upperSideShield_YSize = 12.3; //cm
    Double_t upperSideShield_ZSize = 0.8; //cm

    Double_t upperSideShieldCut_XSize = 0.9; //cm
    Double_t upperSideShieldCut_YSize = 2.4; //cm

    Double_t upperSideShieldWindow_XSize = 3.0; //cm
    Double_t upperSideShieldWindow_YSize = 7.0; //cm

    TGeoShape *upperSideShieldBlankS = new TGeoXtru(2);
    upperSideShieldBlankS->SetName(TString("upperSideShieldBlankS")+=TString("_") + frames->GetName());
    {
        Double_t x_pos[6] = {
            +upperSideShield_XSize*0.5/*0*/,
            +upperSideShield_XSize*0.5/*1*/,
            -upperSideShield_XSize*0.5/*2*/,
            -upperSideShield_XSize*0.5/*3*/,
            -upperSideShield_XSize*0.5 + upperSideShieldCut_XSize/*4*/,
            -upperSideShield_XSize*0.5 + upperSideShieldCut_XSize/*5*/

        };
        Double_t y_pos[6] = {
            -upperSideShield_YSize*0.5/*0*/,
            +upperSideShield_YSize*0.5/*1*/,
            +upperSideShield_YSize*0.5/*2*/,
            -upperSideShield_YSize*0.5 + upperSideShieldCut_YSize/*3*/,
            -upperSideShield_YSize*0.5 + upperSideShieldCut_YSize/*4*/,
            -upperSideShield_YSize*0.5/*5*/

        };
        ((TGeoXtru*)upperSideShieldBlankS)->DefinePolygon(6, x_pos, y_pos);
        ((TGeoXtru*)upperSideShieldBlankS)->DefineSection(0, -upperSideShield_ZSize*0.5);
        ((TGeoXtru*)upperSideShieldBlankS)->DefineSection(1, +upperSideShield_ZSize*0.5);
    }

    TGeoShape *upperSideShieldWindowS = new TGeoBBox(TString("upperSideShieldWindowS")+=TString("_") + frames->GetName(), upperSideShieldWindow_XSize*0.5, upperSideShieldWindow_YSize*0.5, (upperSideShield_ZSize+0.001)*0.5);

    TGeoTranslation *upperSideShieldWindow_pos = new TGeoTranslation();
    upperSideShieldWindow_pos->SetName(TString("upperSideShieldWindow_pos")+=TString("_") + frames->GetName());
    upperSideShieldWindow_pos->SetDx(0.0);
    upperSideShieldWindow_pos->SetDy(+1.65/*shift*/);
    upperSideShieldWindow_pos->SetDz(0.0);
    upperSideShieldWindow_pos->RegisterYourself();

    TGeoCompositeShape *upperSideShieldWithWindowS = new TGeoCompositeShape();
    upperSideShieldWithWindowS->SetName(TString("upperSideShieldWithWindowS")+=TString("_") + frames->GetName());
    {
        TString expression = "upperSideShieldBlankS"; expression += TString("_") + frames->GetName();
            expression += "-upperSideShieldWindowS"; expression += TString("_") + frames->GetName();
            expression += ":upperSideShieldWindow_pos"; expression += TString("_") + frames->GetName();
        upperSideShieldWithWindowS->MakeNode(expression);
        upperSideShieldWithWindowS->ComputeBBox(); //need to compute a bounding box
    }

    TGeoVolume *upperSideShieldV = new TGeoVolume(TString("upperSideShieldV")+=TString("_") + frames->GetName(), upperSideShieldWithWindowS);

    //volume medium
    TGeoMedium *upperSideShieldV_medium = pMedAluminium;
    if(upperSideShieldV_medium) {
        upperSideShieldV->SetMedium(upperSideShieldV_medium);
    }
    else Fatal("Main", "Invalid medium for upperSideShieldV!");

    //volume visual property (transparency)
    upperSideShieldV->SetLineColor(TColor::GetColor("#ffcccc"));
    upperSideShieldV->SetTransparency(50);

    TGeoCombiTrans *upperSideShield_transf[4];

    upperSideShield_transf[0] = new TGeoCombiTrans();
    upperSideShield_transf[0]->RotateY(90.0);
    upperSideShield_transf[0]->SetDx(+(frontPlaneWithWindow_XSize*0.5 - upperSideShield_ZSize*0.5));
    upperSideShield_transf[0]->SetDy(+(17.6 + upperSideShield_YSize*0.5 + 6.0));
    upperSideShield_transf[0]->SetDz(+0.695/*center of half-plane*/ - 0.65/*shift*/ + 4.31/*half-plane shift*/);

    upperSideShield_transf[1] = new TGeoCombiTrans();
    upperSideShield_transf[1]->RotateY(90.0);
    upperSideShield_transf[1]->SetDx(-(frontPlaneWithWindow_XSize*0.5 - upperSideShield_ZSize*0.5));
    upperSideShield_transf[1]->SetDy(+(17.6 + upperSideShield_YSize*0.5 + 6.0));
    upperSideShield_transf[1]->SetDz(+0.695/*center of half-plane*/ - 0.65/*shift*/ + 4.31/*half-plane shift*/);

    upperSideShield_transf[2] = new TGeoCombiTrans();
    upperSideShield_transf[2]->RotateY(-90.0);
    upperSideShield_transf[2]->ReflectY(true);
    upperSideShield_transf[2]->SetDx(+(frontPlaneWithWindow_XSize*0.5 - upperSideShield_ZSize*0.5));
    upperSideShield_transf[2]->SetDy(-(17.6 + upperSideShield_YSize*0.5 + 6.0));
    upperSideShield_transf[2]->SetDz(+0.695/*center of half-plane*/ + 0.65/*shift*/);

    upperSideShield_transf[3] = new TGeoCombiTrans();
    upperSideShield_transf[3]->RotateY(-90.0);
    upperSideShield_transf[3]->ReflectY(true);
    upperSideShield_transf[3]->SetDx(-(frontPlaneWithWindow_XSize*0.5 - upperSideShield_ZSize*0.5));
    upperSideShield_transf[3]->SetDy(-(17.6 + upperSideShield_YSize*0.5 + 6.0));
    upperSideShield_transf[3]->SetDz(+0.695/*center of half-plane*/ + 0.65/*shift*/);

    frames->AddNode(upperSideShieldV, 0, upperSideShield_transf[0]);
    frames->AddNode(upperSideShieldV, 1, upperSideShield_transf[1]);
    frames->AddNode(upperSideShieldV, 2, upperSideShield_transf[2]);
    frames->AddNode(upperSideShieldV, 3, upperSideShield_transf[3]);
    //----------------------------------------------------------------------

    //upper side shield  cover----------------------------------------------
    Double_t upperSideShieldCover_XSize = 4.4; //cm
    Double_t upperSideShieldCover_YSize = 8.5; //cm
    Double_t upperSideShieldCover_ZSize = 0.15; //cm

    TGeoShape *upperSideShieldCoverS = new TGeoBBox("upperSideShieldCoverS", upperSideShieldCover_XSize*0.5, upperSideShieldCover_YSize*0.5, upperSideShieldCover_ZSize*0.5);

    TGeoVolume *upperSideShieldCoverV = new TGeoVolume(TString("upperSideShieldCoverV")+=TString("_") + frames->GetName(), upperSideShieldCoverS);

    //volume medium
    TGeoMedium *upperSideShieldCoverV_medium = pMedAluminium;
    if(upperSideShieldCoverV_medium) {
        upperSideShieldCoverV->SetMedium(upperSideShieldCoverV_medium);
    }
    else Fatal("Main", "Invalid medium for upperSideShieldCoverV!");

    //volume visual property (transparency)
    upperSideShieldCoverV->SetLineColor(TColor::GetColor("#ffcccc"));
    upperSideShieldCoverV->SetTransparency(10);

    TGeoCombiTrans *upperSideShieldCover_transf[4];

    upperSideShieldCover_transf[0] = new TGeoCombiTrans();
    upperSideShieldCover_transf[0]->RotateY(90.0);
    upperSideShieldCover_transf[0]->SetDx(+(frontPlaneWithWindow_XSize*0.5 + upperSideShieldCover_ZSize*0.5));
    upperSideShieldCover_transf[0]->SetDy(+(20.7 + upperSideShieldCover_YSize*0.5 + 6.0));
    upperSideShieldCover_transf[0]->SetDz(+0.695/*center of half-plane*/ - 0.65/*shift*/ + 4.31/*half-plane shift*/);

    upperSideShieldCover_transf[1] = new TGeoCombiTrans();
    upperSideShieldCover_transf[1]->RotateY(90.0);
    upperSideShieldCover_transf[1]->SetDx(-(frontPlaneWithWindow_XSize*0.5 + upperSideShieldCover_ZSize*0.5));
    upperSideShieldCover_transf[1]->SetDy(+(20.7 + upperSideShieldCover_YSize*0.5 + 6.0));
    upperSideShieldCover_transf[1]->SetDz(+0.695/*center of half-plane*/ - 0.65/*shift*/ + 4.31/*half-plane shift*/);

    upperSideShieldCover_transf[2] = new TGeoCombiTrans();
    upperSideShieldCover_transf[2]->RotateY(90.0);
    upperSideShieldCover_transf[2]->SetDx(+(frontPlaneWithWindow_XSize*0.5 + upperSideShieldCover_ZSize*0.5));
    upperSideShieldCover_transf[2]->SetDy(-(20.7 + upperSideShieldCover_YSize*0.5 + 6.0));
    upperSideShieldCover_transf[2]->SetDz(+0.695/*center of half-plane*/ + 0.65/*shift*/);

    upperSideShieldCover_transf[3] = new TGeoCombiTrans();
    upperSideShieldCover_transf[3]->RotateY(90.0);
    upperSideShieldCover_transf[3]->SetDx(-(frontPlaneWithWindow_XSize*0.5 + upperSideShieldCover_ZSize*0.5));
    upperSideShieldCover_transf[3]->SetDy(-(20.7 + upperSideShieldCover_YSize*0.5 + 6.0));
    upperSideShieldCover_transf[3]->SetDz(+0.695/*center of half-plane*/ + 0.65/*shift*/);

    frames->AddNode(upperSideShieldCoverV, 0, upperSideShieldCover_transf[0]);
    frames->AddNode(upperSideShieldCoverV, 1, upperSideShieldCover_transf[1]);
    frames->AddNode(upperSideShieldCoverV, 2, upperSideShieldCover_transf[2]);
    frames->AddNode(upperSideShieldCoverV, 3, upperSideShieldCover_transf[3]);
    //--------------------------------------------------------------------------

    return frames;
}

