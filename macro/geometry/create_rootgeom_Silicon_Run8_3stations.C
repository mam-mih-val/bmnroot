/*
 * Baranov D.
 * 20.01.2020
 * Use this macro to create ROOT geometry for the RUN-8 config. of SILICON
 * detector
 *
 * WARNING: all units is in cm!!!
 *
 */

using namespace TMath;

//Set Parameters of SILICON detector -------------------------------------------
const Int_t NStations = 3;      //stations in the detector
const Int_t NMaxModules = 18;    //max. number of modules in a station

const Int_t NModulesInStations[NStations] = {10, 14, 18};

//(X-Y-Z)Positions of stations (sensitive volumes)

const Double_t XStationPositions[NStations] = { 0.0, 0.0, 0.0 };
const Double_t YStationPositions[NStations] = { 0.0, 0.0, 0.0 };
//const Double_t ZStationPositions[NStations] = { +12.37, +12.37+8.64, +12.37+8.64+8.64 };
const Double_t ZStationPositions[NStations] = { +15.7, +15.7+8.64, +15.7+8.64+8.64 };

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
    {+27.15/*0*/, +21.15/*1*/, +15.15/*2*/, +9.15/*3*/, +3.15/*4*/, -9.15/*5*/, -15.15/*6*/, -21.15/*7*/, -27.15/*8*/, +27.15/*9*/, +21.15/*10*/, +15.15/*11*/, +9.15/*12*/, +3.15/*13*/, -9.15/*14*/, -15.15/*15*/, -21.15/*16*/, -27.15/*17*/}
};
const Double_t YModuleShifts[NStations][NMaxModules] = {
    {-0.15/*0*/, -0.15/*1*/, +2.85/*2*/, -0.15/*3*/, -0.15/*4*/, +0.15/*5*/, +0.15/*6*/, -2.85/*7*/, +0.15/*8*/, +0.15/*9*/},
    {-0.15/*0*/, -0.15/*1*/, -0.15/*2*/, +2.85/*3*/, -0.15/*4*/, -0.15/*5*/, -0.15/*6*/, +0.15/*7*/, +0.15/*8*/, +0.15/*9*/, -2.85/*10*/, +0.15/*11*/, +0.15/*12*/, +0.15/*13*/},
    {-0.15/*0*/, -0.15/*1*/, -0.15/*2*/, -0.15/*3*/, +2.85/*4*/, -0.15/*5*/, -0.15/*6*/, -0.15/*7*/, -0.15/*8*/, +0.15/*9*/, +0.15/*10*/, +0.15/*11*/, +0.15/*12*/, -2.85/*13*/, +0.15/*14*/, +0.15/*15*/, +0.15/*16*/, +0.15/*17*/}
};
const Double_t ZModuleShifts[NStations][NMaxModules] = {
    {+1.36+4.31/*0*/, +0.14+4.31/*1*/, +1.36+4.31/*2*/, +0.0+4.31/*3*/, +1.22+4.31/*4*/, +0.0/*5*/, +1.22/*6*/, 0.0/*7*/, +1.36/*8*/, +0.14/*9*/},
    {+0.14+4.31/*0*/, +1.36+4.31/*1*/, +0.14+4.31/*2*/, +1.36+4.31/*3*/, +0.0+4.31/*4*/, +1.22+4.31/*5*/, +0.0+4.31/*6*/, +1.22/*7*/, +0.0/*8*/, +1.22/*9*/, +0.0/*10*/, +1.36/*11*/, +0.14/*12*/, +1.36/*13*/},
    {+1.36+4.31/*0*/, +0.14+4.31/*1*/, +1.36+4.31/*2*/, +0.14+4.31/*3*/, +1.36+4.31/*4*/, +0.0+4.31/*5*/, +1.22+4.31/*6*/, +0.0+4.31/*7*/, +1.22+4.31/*8*/, +0.0/*9*/, +1.22/*10*/, +0.0/*11*/, +1.22/*12*/, +0.0/*13*/, +1.36/*14*/, +0.14/*15*/, +1.36/*16*/, +0.14/*17*/}
};

//Geometry parameters of modules
enum ModuleType {RIGHT_ANGLED_MODULE, BEVEL_MODULE, RIGHT_ANGLED_MODULE_HALF, BEVEL_MODULE_HALF};

const ModuleType ModulesTypes[NStations][NMaxModules] = {
    {RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE},
    {RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE},
    {RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE}
};

//rotations of modules around y-axis by 180 deg.
const Bool_t ModulesYRotations[NStations][NMaxModules] = {
    {true/*0*/, true/*1*/, true/*2*/, false/*3*/, false/*4*/, true/*5*/, true/*6*/, true/*7*/, false/*8*/, false/*9*/},
    {true/*0*/, true/*1*/, true/*2*/, true/*3*/, false/*4*/, false/*5*/, false/*6*/, true/*7*/, true/*8*/, true/*9*/, true/*10*/, false/*11*/, false/*12*/, false/*13*/},
    {true/*0*/, true/*1*/, true/*2*/, true/*3*/, true/*4*/, false/*5*/, false/*6*/, false/*7*/, false/*8*/, true/*9*/, true/*10*/, true/*11*/, true/*12*/, true/*13*/, false/*14*/, false/*15*/, false/*16*/, false/*17*/}
};

//rotations of modules around x-axis by 180 deg.
const Bool_t ModulesXRotations[NStations][NMaxModules] = {
    {false/*0*/, false/*1*/, false/*2*/, false/*3*/, false/*4*/, true/*5*/, true/*6*/, true/*7*/, true/*8*/, true/*9*/},
    {false/*0*/, false/*1*/, false/*2*/, false/*3*/, false/*4*/, false/*5*/, false/*6*/, true/*7*/, true/*8*/, true/*9*/, true/*10*/, true/*11*/, true/*12*/, true/*13*/},
    {false/*0*/, false/*1*/, false/*2*/, false/*3*/, false/*4*/, false/*5*/, false/*6*/, false/*7*/, false/*8*/, true/*9*/, true/*10*/, true/*11*/, true/*12*/, true/*13*/, true/*14*/, true/*15*/, true/*16*/, true/*17*/}
};

//------------------------------------------------------------------------------

//GeoManager
TGeoManager* gGeoMan = 0;

//Module -----------------------------------------------------------------------
Double_t YGapBetweenPartsOfModule = 0.0035; //Mrs. Slit

Double_t XModuleSize = 6.3;
Double_t YModuleSizeUpperPart = XModuleSize;
Double_t YModuleSizeLowerPart = XModuleSize;

Double_t YModuleSize = YModuleSizeUpperPart+YModuleSizeLowerPart + YGapBetweenPartsOfModule;
Double_t ZModuleSize = 0.03;

Double_t XBevelSize = 3.82; //x-bevel for a angled cut
Double_t YBevelSize = XBevelSize; //y-bevel for a angled cut

Double_t BevelAngleRad = TMath::ATan(XBevelSize/YBevelSize);
Double_t BevelAngleDeg = BevelAngleRad*TMath::RadToDeg();

Double_t XLeftBorder = 0.1145;
Double_t XRightBorder = 0.1118;
Double_t YTopBorder = 0.1143;
Double_t YMiddleBorderUpperPart = 0.1121;
Double_t YMiddleBorderLowerPart = 0.1147;
Double_t YBottomBorder = 0.1148;
Double_t XYBevelBorder = 0.1146;
//------------------------------------------------------------------------------

//Sensor zones -----------------------------------------------------------------
Double_t XSensorZoneSize = XModuleSize - (XLeftBorder + XRightBorder);
Double_t YSensorZoneSizeUpperPart = YModuleSizeUpperPart - (YTopBorder + YMiddleBorderUpperPart);
Double_t YSensorZoneSizeLowerPart = YModuleSizeLowerPart - (YBottomBorder + YMiddleBorderLowerPart);
Double_t ZSensorZoneSize = ZModuleSize;
//------------------------------------------------------------------------------

//Module frames ----------------------------------------------------------------
Double_t XModuleFrameSize = XModuleSize;
Double_t YModuleFrameSize = YModuleSize;

Double_t ZModuleFrameSize = 0.18;
//------------------------------------------------------------------------------

//Y-shift from center of the gap to the global x-axis (for y-alignment of the module)
Double_t YShiftFromCenterToAxis = (YModuleSizeUpperPart-YModuleSizeLowerPart)*0.5;

//X-shift from center of the module to the origin of the local x-axis
Double_t XShiftFromCenterToLocalX0 = +XModuleSize*0.5;

//Y-shift from center of the module to the origin of the local y-axis
Double_t YShiftFromCenterToLocalY0 = +(YModuleSizeLowerPart + YGapBetweenPartsOfModule*0.5);

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

class FairGeoMedia;
class FairGeoBuilder;

//function declarations
TGeoVolume *CreateStation(TString station_name);
TGeoVolume *CreateModule(TString module_name, ModuleType module_type);


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
}

void PrintInfoAboutModuleParameters() {
    cout << "\n";
    cout << "Module parameters information: ---------------------------" << "\n";
    cout << "  XModuleSize = " << XModuleSize << " cm" << "\n";
    cout << "  YModuleSize = " << YModuleSize << " cm" << " ( " << YModuleSizeUpperPart << " + " << YGapBetweenPartsOfModule << " + " << YModuleSizeLowerPart << " )"   << "\n";
    cout << "     YModuleSizeUpperPart = " << YModuleSizeUpperPart << " cm" << "\n";
    cout << "     YModuleSizeLowerPart = " << YModuleSizeLowerPart << " cm" << "\n";
    cout << "     YGapBetweenPartsOfModule = " << YGapBetweenPartsOfModule << " cm" << "\n";
    cout << "  ZModuleSize = " << ZModuleSize << " cm" << "\n";
    cout << "\n";
    cout << "  XBevelSize = " << XBevelSize << " cm" << "\n";
    cout << "  YBevelSize = " << YBevelSize << " cm" << "\n";
    cout << "     BevelAngleDeg = " << BevelAngleDeg << " deg" << "\n";
    cout << "\n";
    cout << "  XLeftBorder = " << XLeftBorder << " cm" << "\n";
    cout << "  XRightBorder = " << XRightBorder << " cm" << "\n";
    cout << "  YTopBorder = " << YTopBorder << " cm" << "\n";
    cout << "  YMiddleBorderUpperPart = " << YMiddleBorderUpperPart << " cm" << "\n";
    cout << "  YGapBetweenPartsOfModule = " << YGapBetweenPartsOfModule << " cm" << " (gap between two parts)\n";
    cout << "  YMiddleBorderLowerPart = " << YMiddleBorderLowerPart << " cm" << "\n";
    cout << "  YBottomBorder = " << YBottomBorder << " cm" << "\n";
    cout << "  XYBevelBorder = " << XYBevelBorder << " cm" << "\n";
    cout << "\n";
    cout << "  XSensorZoneSize = " << XSensorZoneSize << " cm" << "\n";
    cout << "  YSensorZoneSizeUpperPart = " << YSensorZoneSizeUpperPart << " cm" << "\n";
    cout << "  YSensorZoneSizeLowerPart = " << YSensorZoneSizeLowerPart << " cm" << "\n";
    cout << "  ZSensorZoneSize = " << ZSensorZoneSize << " cm" << "\n";

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

void create_rootgeom_Silicon_Run8_3stations() {

    // ----  set working directory  --------------------------------------------
    TString gPath = gSystem->Getenv("VMCWORKDIR");

    // -------   Geometry file name (output)   ----------------------------------
    const TString geoDetectorName = "Silicon";
    const TString geoDetectorVersion = "Run8_3stations";
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

        TGeoVolume *station = CreateStation(TString("station")+ TString::Itoa(istation, 10));

        Int_t NModules = NModulesInStations[istation];

        TGeoVolume *modules[NModules];
        TGeoCombiTrans *modTransforms[NModules];

        for(Int_t imodule = 0; imodule < NModules; ++imodule) {

            TString module_name = TString("module") + TString::Itoa(imodule, 10) + TString("_") + station->GetName();
            modules[imodule] = CreateModule(module_name, ModulesTypes[istation][imodule]);


            modTransforms[imodule] = new TGeoCombiTrans();

            if( ModulesYRotations[istation][imodule] ) {
                modTransforms[imodule]->RotateY(180.0);
                modTransforms[imodule]->SetDx(XModuleShifts[istation][imodule] - XShiftFromCenterToLocalX0);
            }
            else {
                modTransforms[imodule]->SetDx(XModuleShifts[istation][imodule] + XShiftFromCenterToLocalX0);
            }

            if( ModulesXRotations[istation][imodule] ) {
                modTransforms[imodule]->RotateX(180.0);
                if(ModulesTypes[istation][imodule] == ModuleType::BEVEL_MODULE_HALF || ModulesTypes[istation][imodule] == ModuleType::RIGHT_ANGLED_MODULE_HALF) {
                    modTransforms[imodule]->SetDy(YModuleShifts[istation][imodule] - YModuleSizeUpperPart*0.5);
                }
                else {
                    modTransforms[imodule]->SetDy(YModuleShifts[istation][imodule] + YShiftFromCenterToAxis - YShiftFromCenterToLocalY0);
                }
            }
            else {
                if(ModulesTypes[istation][imodule] == ModuleType::BEVEL_MODULE_HALF || ModulesTypes[istation][imodule] == ModuleType::RIGHT_ANGLED_MODULE_HALF) {
                    modTransforms[imodule]->SetDy(YModuleShifts[istation][imodule] + YModuleSizeUpperPart*0.5);
                }
                else {
                    modTransforms[imodule]->SetDy(YModuleShifts[istation][imodule] + YShiftFromCenterToAxis + YShiftFromCenterToLocalY0);
                }
            }

            Double_t z_module_shift = +ZModuleSize*0.5; //To z-module position is at the beginning of the sensitive volume
            modTransforms[imodule]->SetDz(ZModuleShifts[istation][imodule] + z_module_shift);

            //if(imodule == 1) station->AddNode(modules[imodule], 0, modTransforms[imodule]);
            station->AddNode(modules[imodule], 0, modTransforms[imodule]);
        }

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

TGeoVolume *CreateModule(TString module_name, ModuleType module_type) {

    //Shapes -------------------------------------------------------------------
    //module shape
    TGeoShape *moduleS = 0;

    switch(module_type) {
        case RIGHT_ANGLED_MODULE: {
            moduleS = new TGeoBBox("moduleS", XModuleSize*0.5, YModuleSize*0.5, ZModuleSize*0.5);
            break;
        }
        case RIGHT_ANGLED_MODULE_HALF: {
            moduleS = new TGeoBBox("moduleS", XModuleSize*0.5, YModuleSizeUpperPart*0.5, ZModuleSize*0.5);
            break;
        }
        case BEVEL_MODULE: {
            moduleS = new TGeoXtru(2);
            moduleS->SetName("moduleS");
                Double_t x_pos[5] = {+XModuleSize*0.5, -XModuleSize*0.5, -XModuleSize*0.5, +XModuleSize*0.5-XBevelSize, +XModuleSize*0.5};
                Double_t y_pos[5] = {+YModuleSize*0.5, +YModuleSize*0.5, -YModuleSize*0.5, -YModuleSize*0.5, -YModuleSize*0.5+YBevelSize};
            ((TGeoXtru*)moduleS)->DefinePolygon(5, x_pos, y_pos);
            ((TGeoXtru*)moduleS)->DefineSection(0, -ZModuleSize*0.5);
            ((TGeoXtru*)moduleS)->DefineSection(1, +ZModuleSize*0.5);
            break;
        }
        case BEVEL_MODULE_HALF: {
            moduleS = new TGeoXtru(2);
            moduleS->SetName("moduleS");
                Double_t x_pos[5] = {+XModuleSize*0.5, -XModuleSize*0.5, -XModuleSize*0.5, +XModuleSize*0.5-XBevelSize, +XModuleSize*0.5};
                Double_t y_pos[5] = {+YModuleSizeUpperPart*0.5, +YModuleSizeUpperPart*0.5, -YModuleSizeUpperPart*0.5, -YModuleSizeUpperPart*0.5, -YModuleSizeUpperPart*0.5+YBevelSize};
            ((TGeoXtru*)moduleS)->DefinePolygon(5, x_pos, y_pos);
            ((TGeoXtru*)moduleS)->DefineSection(0, -ZModuleSize*0.5);
            ((TGeoXtru*)moduleS)->DefineSection(1, +ZModuleSize*0.5);
            break;
        }
        default:
            Error("CreateModule()", "Incorrect module type selected!!!");
    }

    //sensor zone shape (upper part of the module: rectangle zone)
    TGeoShape *sensor_zone_upper_part_rectS = new TGeoBBox("sensor_zone_upper_part_rectS", XSensorZoneSize*0.5, YSensorZoneSizeUpperPart*0.5, ZSensorZoneSize*0.5);

    //sensor zone shape (upper part of the module: bevel zone)
    TGeoShape *sensor_zone_upper_part_bevelS = new TGeoXtru(2);
        sensor_zone_upper_part_bevelS->SetName("sensor_zone_upper_part_bevelS");
        //Calculate coordinates of the bottom and the top points of the inner bevel
        {
            //find x and y projections of the bevel border thickness
            Double_t x_delta = XYBevelBorder/TMath::Cos(BevelAngleRad);
            Double_t y_delta = XYBevelBorder/TMath::Sin(BevelAngleRad);

            //find shifts of the sensetive zone from module borders via the equation of the straight line for the inner bevel
            Double_t x_delta_top_BP = XLeftBorder;
            Double_t y_delta_top_BP = TMath::Tan(TMath::PiOver2()+BevelAngleRad)*x_delta_top_BP + (YBevelSize+y_delta);

            Double_t y_delta_bottom_BP = YMiddleBorderUpperPart;
            Double_t x_delta_bottom_BP = (y_delta_bottom_BP - (YBevelSize + y_delta))/(TMath::Tan(TMath::PiOver2()+BevelAngleRad));
            //----------------------------------------------------------------------

            Double_t x_pos_sensor_zone_bevel[5] = { +XModuleSize*0.5-XLeftBorder, -XModuleSize*0.5+XRightBorder, -XModuleSize*0.5+XRightBorder,       +XModuleSize*0.5-x_delta_bottom_BP, +XModuleSize*0.5-XLeftBorder  };
            Double_t y_pos_sensor_zone_bevel[5] = { +YModuleSizeUpperPart*0.5-YTopBorder, +YModuleSizeUpperPart*0.5-YTopBorder, -YModuleSizeUpperPart*0.5+YMiddleBorderUpperPart,      -YModuleSizeUpperPart*0.5+YMiddleBorderUpperPart, -YModuleSizeUpperPart*0.5+y_delta_top_BP};

            ((TGeoXtru*)sensor_zone_upper_part_bevelS)->DefinePolygon(5, x_pos_sensor_zone_bevel, y_pos_sensor_zone_bevel);
            ((TGeoXtru*)sensor_zone_upper_part_bevelS)->DefineSection(0, -ZModuleSize*0.5);
            ((TGeoXtru*)sensor_zone_upper_part_bevelS)->DefineSection(1, +ZModuleSize*0.5);
        }


    //sensor zone shape (lower part of the module: rectangle zone)
    TGeoShape *sensor_zone_lower_part_rectS = new TGeoBBox("sensor_zone_lower_part_rectS", XSensorZoneSize*0.5, YSensorZoneSizeLowerPart*0.5, ZSensorZoneSize*0.5);

    //sensor zone shape (lower part of the module: bevel zone)
    TGeoShape *sensor_zone_lower_part_bevelS = new TGeoXtru(2);
        sensor_zone_lower_part_bevelS->SetName("sensor_zone_lower_part_bevelS");
        //Calculate coordinates of the bottom and the top points of the inner bevel
        {
            //find x and y projections of the bevel border thickness
            Double_t x_delta = XYBevelBorder/TMath::Cos(BevelAngleRad);
            Double_t y_delta = XYBevelBorder/TMath::Sin(BevelAngleRad);

            //find shifts of the sensetive zone from module borders via the equation of the straight line for the inner bevel
            Double_t x_delta_top_BP = XLeftBorder;
            Double_t y_delta_top_BP = TMath::Tan(TMath::PiOver2()+BevelAngleRad)*x_delta_top_BP + (YBevelSize+y_delta);

            Double_t y_delta_bottom_BP = YBottomBorder;
            Double_t x_delta_bottom_BP = (y_delta_bottom_BP - (YBevelSize + y_delta))/(TMath::Tan(TMath::PiOver2()+BevelAngleRad));
            //----------------------------------------------------------------------

            Double_t x_pos_sensor_zone_bevel[5] = { +XModuleSize*0.5-XLeftBorder, -XModuleSize*0.5+XRightBorder, -XModuleSize*0.5+XRightBorder,       +XModuleSize*0.5-x_delta_bottom_BP, +XModuleSize*0.5-XLeftBorder  };
            Double_t y_pos_sensor_zone_bevel[5] = { +YModuleSizeLowerPart*0.5-YMiddleBorderLowerPart, +YModuleSizeLowerPart*0.5-YMiddleBorderLowerPart, -YModuleSizeLowerPart*0.5+YBottomBorder,      -YModuleSizeLowerPart*0.5+YBottomBorder, -YModuleSizeLowerPart*0.5+y_delta_top_BP};

            ((TGeoXtru*)sensor_zone_lower_part_bevelS)->DefinePolygon(5, x_pos_sensor_zone_bevel, y_pos_sensor_zone_bevel);
            ((TGeoXtru*)sensor_zone_lower_part_bevelS)->DefineSection(0, -ZModuleSize*0.5);
            ((TGeoXtru*)sensor_zone_lower_part_bevelS)->DefineSection(1, +ZModuleSize*0.5);
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
                sensor_zone_upper_part_pos->SetDx(+(XLeftBorder+XRightBorder)*0.5 - XLeftBorder);
                sensor_zone_upper_part_pos->SetDy(+YSensorZoneSizeUpperPart*0.5 + YGapBetweenPartsOfModule*0.5 + YMiddleBorderUpperPart - YShiftFromCenterToAxis);
                sensor_zone_upper_part_pos->SetDz(0.0);

            //position of the sensor zone in the lower part of the module
            TGeoTranslation *sensor_zone_lower_part_pos = new TGeoTranslation();
                sensor_zone_lower_part_pos->SetDx(+(XLeftBorder+XRightBorder)*0.5 - XLeftBorder);
                sensor_zone_lower_part_pos->SetDy(-YSensorZoneSizeLowerPart*0.5 - YGapBetweenPartsOfModule*0.5 - YMiddleBorderLowerPart - YShiftFromCenterToAxis);
                sensor_zone_lower_part_pos->SetDz(0.0);

            moduleV->AddNode(sensor_zone_upper_part_rectV, 0, sensor_zone_upper_part_pos);
            moduleV->AddNode(sensor_zone_lower_part_rectV, 0, sensor_zone_lower_part_pos);
            break;
        }

        case RIGHT_ANGLED_MODULE_HALF: {
            //position of the sensor zone in the upper part of the module
            TGeoTranslation *sensor_zone_upper_part_pos = new TGeoTranslation();
                sensor_zone_upper_part_pos->SetDx(+(XLeftBorder+XRightBorder)*0.5 - XLeftBorder);
                sensor_zone_upper_part_pos->SetDy(+(YTopBorder+YMiddleBorderUpperPart)*0.5 - YTopBorder);
                sensor_zone_upper_part_pos->SetDz(0.0);

            moduleV->AddNode(sensor_zone_upper_part_rectV, 0, sensor_zone_upper_part_pos);
            break;
        }

        case BEVEL_MODULE: {

            //position of the sensor zone in the upper part of the module
            TGeoTranslation *sensor_zone_upper_part_pos = new TGeoTranslation();
                sensor_zone_upper_part_pos->SetDx(+(XLeftBorder+XRightBorder)*0.5 - XLeftBorder);
                sensor_zone_upper_part_pos->SetDy(+YSensorZoneSizeUpperPart*0.5 + YGapBetweenPartsOfModule*0.5 + YMiddleBorderUpperPart - YShiftFromCenterToAxis);
                sensor_zone_upper_part_pos->SetDz(0.0);

            //position of the sensor zone in the lower part of the module
            TGeoTranslation *sensor_zone_lower_part_bevel_pos = new TGeoTranslation();
                sensor_zone_lower_part_bevel_pos->SetDx(0.0);
                sensor_zone_lower_part_bevel_pos->SetDy(-YModuleSizeLowerPart*0.5 - YGapBetweenPartsOfModule*0.5 - YShiftFromCenterToAxis);
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
