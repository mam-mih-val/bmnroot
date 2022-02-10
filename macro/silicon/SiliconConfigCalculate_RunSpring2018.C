/*
 * Description:
 * Macro for calculating parameters of SILICON detector and writing them
 * into the XML-file needed for simulation and reconstruction procedures.
 * The obtained XML-file should be moved into the 'parameters/silicon/XMLConfigs' directory
 *
 * SILICON configuration: RunSpring2018 (BM@N RUN-7 where 3 SI-stations were)
 *
 * Created: 08.06.2018
 * Modified: 30.10.2018 (z-positions of stations corrected)
 * Author: D. Baranov
 */

#include "Rtypes.h"

R__ADD_INCLUDE_PATH($VMCWORKDIR)
R__LOAD_LIBRARY(libBmnData.so)

#include "TString.h"

#include <fstream>

using namespace TMath;
using namespace std;

//Configuration of the module

//Module 63x126 mm -------------------------------------------------------------

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

// 0 - inclined strips (front side), 1 - straight strips (back side)
Double_t XLeftBorder[2] = { 0.1145, 0.10995 };
Double_t XRightBorder[2] = { 0.1118, 0.11005 };

Double_t YTopBorder[2] = { 0.1143, 0.1143 };
Double_t YMiddleBorderUpperPart[2] = { 0.1121, 0.1055 };
Double_t YMiddleBorderLowerPart[2] = { 0.1147, 0.1055 };

Double_t YBottomBorder[2] = { 0.1148, 0.1055 };

Double_t XYBevelBorder[2] = { 0.1146, 0.1076 };


//layers parameters

Double_t XShiftFirstStripUpLayer[2] = { 0.00795 , 0.0 }; //distance between the beginning of the first strip and the inner border of the frame
Double_t XShiftFirstStripLowLayer[2] = { 0.00780 , 0.0 };

Double_t XShiftLastStripUpLayer[2] = { 0.00675 , 0.0 }; //distance between the end of the last strip and the inner border of the frame
Double_t XShiftLastStripLowLayer[2] = { 0.00675 , 0.0 };

Double_t YShiftFirstStripUpLayer[2] = { 0.0 , 0.0 };
Double_t YShiftFirstStripLowLayer[2] = { 0.0 , 0.0 };

Double_t YShiftLastStripUpLayer[2] = { 0.0 , 0.0 };
Double_t YShiftLastStripLowLayer[2] = { 0.0 , 0.0 };

//----------

Double_t XOrigUpLayer[2] = { XModuleSize - XLeftBorder[0], XModuleSize - XLeftBorder[1] };
Double_t XOrigLowLayer[2] = { XModuleSize - XLeftBorder[0], XModuleSize - XLeftBorder[1] };

Double_t YOrigUpLayer[2] = { YModuleSize - YModuleSizeUpperPart + YMiddleBorderUpperPart[0], YModuleSize - YModuleSizeUpperPart + YMiddleBorderUpperPart[1] };
Double_t YOrigLowLayer[2] = { YBottomBorder[0], YBottomBorder[1] };

Double_t XSizeUpLayer[2] = { XModuleSize - (XLeftBorder[0] + XRightBorder[0]) , XModuleSize - (XLeftBorder[1] + XRightBorder[1]) };
Double_t YSizeUpLayer[2] = { YModuleSizeUpperPart - (YTopBorder[0] + YMiddleBorderUpperPart[0]), YModuleSizeUpperPart - (YTopBorder[1] + YMiddleBorderUpperPart[1]) };

Double_t XSizeLowLayer[2] = { XModuleSize - (XLeftBorder[0] + XRightBorder[0]) , XModuleSize - (XLeftBorder[1] + XRightBorder[1]) };
Double_t YSizeLowLayer[2] = { YModuleSizeLowerPart - (YBottomBorder[0] + YMiddleBorderLowerPart[0]), YModuleSizeLowerPart - (YBottomBorder[1] + YMiddleBorderLowerPart[1]) };

Double_t XLeftBorderUpperlayer[2] = { XOrigUpLayer[0] - XShiftFirstStripUpLayer[0], XOrigUpLayer[1] - XShiftLastStripUpLayer[1] + XShiftLastStripUpLayer[1] };
Double_t XLeftBorderLowerlayer[2] = { XOrigUpLayer[0] - XShiftFirstStripLowLayer[0], XOrigUpLayer[1] - XShiftLastStripLowLayer[1] + XShiftLastStripLowLayer[1] };

Double_t XRightBorderUpperlayer[2] = { XOrigUpLayer[0] - XSizeUpLayer[0] + XShiftLastStripUpLayer[0], XOrigUpLayer[1] - XSizeUpLayer[1] + XShiftFirstStripUpLayer[1] };
Double_t XRightBorderLowerlayer[2] = { XOrigLowLayer[0] - XSizeLowLayer[0] + XShiftLastStripLowLayer[0], XOrigLowLayer[1] - XSizeLowLayer[1] + XShiftFirstStripLowLayer[1] };

Double_t YLeftBorderUpperlayer[2] = { YOrigUpLayer[0] + YSizeUpLayer[0] - YShiftFirstStripUpLayer[0], YOrigUpLayer[1] + YSizeUpLayer[1] - YShiftFirstStripUpLayer[1] };
Double_t YLeftBorderLowerlayer[2] = { YOrigLowLayer[0] + YSizeLowLayer[0] - YShiftFirstStripLowLayer[0], YOrigLowLayer[1] + YSizeLowLayer[1] - YShiftFirstStripLowLayer[1] };

Double_t YRightBorderUpperlayer[2] = { YOrigUpLayer[0], YOrigUpLayer[1] };
Double_t YRightBorderLowerlayer[2] = { YOrigLowLayer[0], YOrigLowLayer[1] };

enum StripNumberingDirection {LeftToRight, RightToLeft};
StripNumberingDirection StripDirection[2] = { LeftToRight, RightToLeft };

Double_t PitchValue[2] = { 0.01029019668, 0.0095 };
Double_t StripAngle[2] = { +2.5, +0.0 }; // plus - clockwise, minus - anticlockwise
Int_t FirstStripNumberUpLayer[2] = { 0, 0 };
Int_t FirstStripNumberLowLayer[2] = { 26, 0 };

//dead zone (if you need)

//find x and y projections of the bevel border thickness
Double_t x_delta[2] = { XYBevelBorder[0]/TMath::Cos(BevelAngleRad), XYBevelBorder[1]/TMath::Cos(BevelAngleRad) };
Double_t y_delta[2] = { XYBevelBorder[0]/TMath::Sin(BevelAngleRad), XYBevelBorder[1]/TMath::Sin(BevelAngleRad) };

//find shifts of the sensetive zone from module borders via the equation of the straight line for the inner bevel
Double_t x_delta_top_BP[2] = { XLeftBorder[0], XLeftBorder[1] };
Double_t y_delta_top_BP[2] = { TMath::Tan(TMath::PiOver2()+BevelAngleRad)*x_delta_top_BP[0] + (YBevelSize+y_delta[0]), TMath::Tan(TMath::PiOver2()+BevelAngleRad)*x_delta_top_BP[1] + (YBevelSize+y_delta[1]) };

Double_t y_delta_bottom_BP[2] = { YBottomBorder[0], YBottomBorder[1] };
Double_t x_delta_bottom_BP[2] = { (y_delta_bottom_BP[0] - (YBevelSize + y_delta[0]))/(TMath::Tan(TMath::PiOver2()+BevelAngleRad)), (y_delta_bottom_BP[1] - (YBevelSize + y_delta[1]))/(TMath::Tan(TMath::PiOver2()+BevelAngleRad)) };

Double_t XDeadZone[2][3] = {
    { XOrigLowLayer[0], XOrigLowLayer[0], XModuleSize - x_delta_bottom_BP[0] },
    { XOrigLowLayer[1], XOrigLowLayer[1], XModuleSize - x_delta_bottom_BP[1] }
};

Double_t YDeadZone[2][3] = {
    { YOrigLowLayer[0], y_delta_top_BP[0], YOrigLowLayer[0] },
    { YOrigLowLayer[1], y_delta_top_BP[1], YOrigLowLayer[1] }
};
//------------------------------------------------------------------------------

//Configuration of the SI-stations ---------------------------------------------

//Set Parameters of SILICON detector -------------------------------------------
const Int_t NStations = 3;      //stations in the detector
const Int_t NMaxModules = 8;    //max. number of modules in a station

const Int_t NModulesInStations[NStations] = {4, 2, 8};

//(X-Y-Z)Positions of stations (sensitive volumes)
const Double_t XStationPositions[NStations] = { +0.6295, +0.6295, +0.46525 };
const Double_t YStationPositions[NStations] = { -4.53225, -4.53225, -4.67125 };
//const Double_t XStationPositions[NStations] = { 0.0, 0.0, 0.0 };
//const Double_t YStationPositions[NStations] = { -0.0, 0.0, 0.0 };
//const Double_t ZStationPositions[NStations] = { +13.424, +13.424, +25.717 };
const Double_t ZStationPositions[NStations] = { +10.524, +10.524, +21.604 }; // in accordance with rough alignment correction

//(X-Y-Z)Shifts of modules in each station
const Double_t XModuleShifts[NStations][NMaxModules] = {
    {+6.4, -5.9, +5.9, -6.4},
    {+5.9, -6.4},
    {+12.15, +6.15, -6.15, -12.15, +12.15, +6.15, -6.15, -12.15}
    //{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}
};
const Double_t YModuleShifts[NStations][NMaxModules] = {
    {-0.4, +0.1, -0.1, +0.4},
    {-6.22, -6.72},
    {-0.15, -0.15, -0.15, -0.15, +0.15, +0.15, +0.15, +0.15}
    //{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}
};
const Double_t ZModuleShifts[NStations][NMaxModules] = {
    {+1.44, +2.17, +2.17, +1.44},
    {+4.71, +5.44},
    {3.51/*0*/, 1.98/*1*/, 3.29/*2*/, 1.76/*3*/, 2.25/*4*/, 3.78/*5*/, 2.47/*6*/, 4.0/*7*/}
};

//Geometry parameters of modules
enum ModuleType {RIGHT_ANGLED_MODULE, BEVEL_MODULE, RIGHT_ANGLED_MODULE_HALF, BEVEL_MODULE_HALF};

const ModuleType ModulesTypes[NStations][NMaxModules] = {
    {RIGHT_ANGLED_MODULE_HALF, RIGHT_ANGLED_MODULE_HALF, RIGHT_ANGLED_MODULE_HALF, RIGHT_ANGLED_MODULE_HALF},
    //{BEVEL_MODULE_HALF, BEVEL_MODULE_HALF, BEVEL_MODULE_HALF, BEVEL_MODULE_HALF},
    {RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE},
    {RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, BEVEL_MODULE}
};

//rotations of modules around y-axis by 180 deg.
const Bool_t ModulesYRotations[NStations][NMaxModules] = {
    {true, false, true, false},
    {true, false},
    {true, true, false, false, true, true, false, false}
};

//rotations of modules around x-axis by 180 deg.
const Bool_t ModulesXRotations[NStations][NMaxModules] = {
    {false, false, true, true},
    {false, false},
    {false, false, false, false, true, true, true, true}
};


//------------------------------------------------------------------------------

//Function declarations
void PrintInfo();
void CalculateParameters(TString xml_file_name);
//------------------------------------------------------------------------------


void SiliconConfigCalculate_RunSpring2018() {

    PrintInfo();
    CalculateParameters("SiliconRunSpring2018.xml");

    cout << "It's the end" << "\n";
    //gApplication->Terminate();
}

//------------------------------------------------------------------------------

void PrintInfo() {

    cout << "----------------------------------------------------------------\n";
    cout << "Parameters of the standard module 63x126 mm:\n";
    cout << "\n";
    cout << " Upper layer (inclined strips):\n";
    cout << "    orig(x:y) = ( " << XOrigUpLayer[0] << " : " << YOrigUpLayer[0] << " )\n";
    cout << "    size(dx:dy) = ( " << XSizeUpLayer[0] << " : " << YSizeUpLayer[0] << " )\n";
    cout << "    lborder(x:y) = ( " << XLeftBorderUpperlayer[0] << " : " << YLeftBorderUpperlayer[0] << " )\n";
    cout << "    rborder(x:y) = ( " << XRightBorderUpperlayer[0] << " : " << YRightBorderUpperlayer[0] << " )\n";
    cout << "    strip_direction = ";
        if(StripDirection[0] == LeftToRight) cout << "LeftToRight\n";
        else cout << "RightToLeft\n";
    cout << "    pitch = " << PitchValue[0] << "\n";
    cout << "    angle = " << StripAngle[0] << " deg\n";
    cout << "    first_strip_number = " << FirstStripNumberUpLayer[0] << "\n";
    cout << "\n";

    cout << " Lower layer (inclined strips):\n";
    cout << "    orig(x:y) = ( " << XOrigLowLayer[0] << " : " << YOrigLowLayer[0] << " )\n";
    cout << "    size(dx:dy) = ( " << XSizeLowLayer[0] << " : " << YSizeLowLayer[0] << " )\n";
    cout << "    lborder(x:y) = ( " << XLeftBorderLowerlayer[0] << " : " << YLeftBorderLowerlayer[0] << " )\n";
    cout << "    rborder(x:y) = ( " << XRightBorderLowerlayer[0] << " : " << YRightBorderLowerlayer[0] << " )\n";
    cout << "    strip_direction = ";
        if(StripDirection[0] == LeftToRight) cout << "LeftToRight\n";
        else cout << "RightToLeft\n";
    cout << "    pitch = " << PitchValue[0] << "\n";
    cout << "    angle = " << StripAngle[0] << " deg\n";
    cout << "    first_strip_number = " << FirstStripNumberLowLayer[0] << "\n";
    cout << "    dead_zone(3 points):\n";
    cout << "       p0(x:y) = ( " << XDeadZone[0][0] << " : " << YDeadZone[0][0] << " )\n";
    cout << "       p1(x:y) = ( " << XDeadZone[0][1] << " : " << YDeadZone[0][1] << " )\n";
    cout << "       p2(x:y) = ( " << XDeadZone[0][2] << " : " << YDeadZone[0][2] << " )\n";
    cout << "\n";

    cout << " Upper layer (straight strips):\n";
    cout << "    orig(x:y) = ( " << XOrigUpLayer[1] << " : " << YOrigUpLayer[1] << " )\n";
    cout << "    size(dx:dy) = ( " << XSizeUpLayer[1] << " : " << YSizeUpLayer[1] << " )\n";
    cout << "    lborder(x:y) = ( " << XLeftBorderUpperlayer[1] << " : " << YLeftBorderUpperlayer[1] << " )\n";
    cout << "    rborder(x:y) = ( " << XRightBorderUpperlayer[1] << " : " << YRightBorderUpperlayer[1] << " )\n";
    cout << "    strip_direction = ";
        if(StripDirection[1] == LeftToRight) cout << "LeftToRight\n";
        else cout << "RightToLeft\n";
    cout << "    pitch = " << PitchValue[1] << "\n";
    cout << "    angle = " << StripAngle[1] << " deg\n";
    cout << "    first_strip_number = " << FirstStripNumberUpLayer[1] << "\n";
    cout << "\n";

    cout << " Lower layer (straight strips):\n";
    cout << "    orig(x:y) = ( " << XOrigLowLayer[1] << " : " << YOrigLowLayer[1] << " )\n";
    cout << "    size(dx:dy) = ( " << XSizeLowLayer[1] << " : " << YSizeLowLayer[1] << " )\n";
    cout << "    lborder(x:y) = ( " << XLeftBorderLowerlayer[1] << " : " << YLeftBorderLowerlayer[1] << " )\n";
    cout << "    rborder(x:y) = ( " << XRightBorderLowerlayer[1] << " : " << YRightBorderLowerlayer[1] << " )\n";
    cout << "    strip_direction = ";
        if(StripDirection[1] == LeftToRight) cout << "LeftToRight\n";
        else cout << "RightToLeft\n";
    cout << "    pitch = " << PitchValue[1] << "\n";
    cout << "    angle = " << StripAngle[1] << " deg\n";
    cout << "    first_strip_number = " << FirstStripNumberLowLayer[1] << "\n";
    cout << "    dead_zone(3 points):\n";
    cout << "       p0(x:y) = ( " << XDeadZone[1][0] << " : " << YDeadZone[1][0] << " )\n";
    cout << "       p1(x:y) = ( " << XDeadZone[1][1] << " : " << YDeadZone[1][1] << " )\n";
    cout << "       p2(x:y) = ( " << XDeadZone[1][2] << " : " << YDeadZone[1][2] << " )\n";
    cout << "\n";

    cout << "----------------------------------------------------------------\n";

}
//------------------------------------------------------------------------------

void CalculateParameters(TString xml_file_name) {

    ofstream SiFileOut(xml_file_name.Data(), ios::out);

    SiFileOut << "<StationSet>\n";
    SiFileOut << "\n";

    for(Int_t istation = 0; istation < NStations; ++istation) {

        cout << "istation = " << istation << ": -----------------------------\n";

        SiFileOut << "<!-- Station " << istation << " -->\n";

        SiFileOut << "\t<Station";
        SiFileOut << " xPosition=\"" << XStationPositions[istation] << "\"";
        SiFileOut << " yPosition=\"" << YStationPositions[istation] << "\"";
        SiFileOut << " zPosition=\"" << ZStationPositions[istation] << "\"";
        SiFileOut << " rotAngleDeg=\"" << "0.0" << "\"";
        SiFileOut << " rotCenterX=\"" << "0.0" << "\"";
        SiFileOut << " rotCenterY=\"" << "0.0" << "\"";
        SiFileOut << ">\n";
        SiFileOut << "\n";

        for(Int_t imodule = 0; imodule < NModulesInStations[istation]; ++imodule) {

            cout << "  imodule = " << imodule << "\n";

            SiFileOut << "\t\t<!-- module " << imodule << " -->\n";

            SiFileOut << "\t\t<Module";
            //SiFileOut << " xShift=\"" << XModuleShifts[istation][imodule] << "\"";
            //SiFileOut << " yShift=\"" << YModuleShifts[istation][imodule] << "\"";
            SiFileOut << " xShift=\"" << 0.0 << "\"";
            SiFileOut << " yShift=\"" << 0.0 << "\"";
            SiFileOut << " zShift=\"" << ZModuleShifts[istation][imodule] << "\"";
            SiFileOut << ">\n";

            //Notice!!!: here:
            //  index "0" is a layer with inclined strips (the front side of the module)
            //  index "1" is a layer with straight strips (the back side of the module)
            //  "UpLayer" is the upper part of the module
            //  "LowLayer" is the lower part of the module

            Double_t xOrig_UpLayer[2];
            Double_t yOrig_UpLayer[2];
            Double_t xOrig_LowLayer[2];
            Double_t yOrig_LowLayer[2];

            Double_t xSize_UpLayer[2];
            Double_t ySize_UpLayer[2];
            Double_t xSize_LowLayer[2];
            Double_t ySize_LowLayer[2];

            Double_t xLBorder_UpLayer[2];
            Double_t yLBorder_UpLayer[2];
            Double_t xLBorder_LowLayer[2];
            Double_t yLBorder_LowLayer[2];
            Double_t xRBorder_UpLayer[2];
            Double_t yRBorder_UpLayer[2];
            Double_t xRBorder_LowLayer[2];
            Double_t yRBorder_LowLayer[2];

            Double_t xDeadZone[2][3];
            Double_t yDeadZone[2][3];

            Bool_t isDeadZone_UpLayer = false;
            Bool_t isDeadZone_LowLayer = false;

            StripNumberingDirection stripDirection[2];
            Double_t pitchValue[2] = {PitchValue[0], PitchValue[1]};
            Double_t stripAngle[2];
            Int_t firstStripNumber_UpLayer[2];
            Int_t firstStripNumber_LowLayer[2];

            if( ModulesYRotations[istation][imodule] == false && ModulesXRotations[istation][imodule] == false ) {

                xOrig_UpLayer[0] = XOrigUpLayer[0] + XModuleShifts[istation][imodule];
                yOrig_UpLayer[0] = YOrigUpLayer[0] + YModuleShifts[istation][imodule];
                xOrig_LowLayer[0] = XOrigLowLayer[0] + XModuleShifts[istation][imodule];
                yOrig_LowLayer[0] = YOrigLowLayer[0] + YModuleShifts[istation][imodule];

                xOrig_UpLayer[1] = XOrigUpLayer[1] + XModuleShifts[istation][imodule];
                yOrig_UpLayer[1] = YOrigUpLayer[1] + YModuleShifts[istation][imodule];
                xOrig_LowLayer[1] = XOrigLowLayer[1] + XModuleShifts[istation][imodule];
                yOrig_LowLayer[1] = YOrigLowLayer[1] + YModuleShifts[istation][imodule];

                xSize_UpLayer[0] = XSizeUpLayer[0];
                ySize_UpLayer[0] = YSizeUpLayer[0];
                xSize_LowLayer[0] = XSizeLowLayer[0];
                ySize_LowLayer[0] = YSizeLowLayer[0];

                xSize_UpLayer[1] = XSizeUpLayer[1];
                ySize_UpLayer[1] = YSizeUpLayer[1];
                xSize_LowLayer[1] = XSizeLowLayer[1];
                ySize_LowLayer[1] = YSizeLowLayer[1];

                xLBorder_UpLayer[0] = XLeftBorderUpperlayer[0] + XModuleShifts[istation][imodule];
                xLBorder_UpLayer[1] = XLeftBorderUpperlayer[1] + XModuleShifts[istation][imodule];
                yLBorder_UpLayer[0] = YLeftBorderUpperlayer[0] + YModuleShifts[istation][imodule];
                yLBorder_UpLayer[1] = YLeftBorderUpperlayer[1] + YModuleShifts[istation][imodule];
                xLBorder_LowLayer[0] = XLeftBorderLowerlayer[0] + XModuleShifts[istation][imodule];
                xLBorder_LowLayer[1] = XLeftBorderLowerlayer[1] + XModuleShifts[istation][imodule];
                yLBorder_LowLayer[0] = YLeftBorderLowerlayer[0] + YModuleShifts[istation][imodule];
                yLBorder_LowLayer[1] = YLeftBorderLowerlayer[1] + YModuleShifts[istation][imodule];

                xRBorder_UpLayer[0] = XRightBorderUpperlayer[0] + XModuleShifts[istation][imodule];
                xRBorder_UpLayer[1] = XRightBorderUpperlayer[1] + XModuleShifts[istation][imodule];
                yRBorder_UpLayer[0] = YRightBorderUpperlayer[0] + YModuleShifts[istation][imodule];
                yRBorder_UpLayer[1] = YRightBorderUpperlayer[1] + YModuleShifts[istation][imodule];
                xRBorder_LowLayer[0] = XRightBorderLowerlayer[0] + XModuleShifts[istation][imodule];
                xRBorder_LowLayer[1] = XRightBorderLowerlayer[1] + XModuleShifts[istation][imodule];
                yRBorder_LowLayer[0] = YRightBorderLowerlayer[0] + YModuleShifts[istation][imodule];
                yRBorder_LowLayer[1] = YRightBorderLowerlayer[1] + YModuleShifts[istation][imodule];

                for(Int_t ilayer = 0; ilayer < 2; ++ilayer) {
                    for(Int_t ipoint = 0; ipoint < 3; ++ipoint) {
                        xDeadZone[ilayer][ipoint] = XDeadZone[ilayer][ipoint] + XModuleShifts[istation][imodule];
                        yDeadZone[ilayer][ipoint] = YDeadZone[ilayer][ipoint] + YModuleShifts[istation][imodule];
                    }
                }

                if(ModulesTypes[istation][imodule] == ModuleType::BEVEL_MODULE || ModulesTypes[istation][imodule] == ModuleType::BEVEL_MODULE_HALF) {
                    isDeadZone_UpLayer = false;
                    isDeadZone_LowLayer = true;
                }

                if(StripDirection[0] == LeftToRight) stripDirection[0] = LeftToRight; else stripDirection[0] = RightToLeft;
                if(StripDirection[1] == LeftToRight) stripDirection[1] = LeftToRight; else stripDirection[1] = RightToLeft;

                stripAngle[0] = StripAngle[0];
                stripAngle[1] = StripAngle[1];

                firstStripNumber_UpLayer[0] = FirstStripNumberUpLayer[0];
                firstStripNumber_UpLayer[1] = FirstStripNumberUpLayer[1];
                firstStripNumber_LowLayer[0] = FirstStripNumberLowLayer[0];
                firstStripNumber_LowLayer[1] = FirstStripNumberLowLayer[1];
            }

            if( ModulesYRotations[istation][imodule] == true && ModulesXRotations[istation][imodule] == false ) {

                xOrig_UpLayer[0] = -XOrigUpLayer[0] + XSizeUpLayer[0] + XModuleShifts[istation][imodule];
                yOrig_UpLayer[0] = YOrigUpLayer[0] + YModuleShifts[istation][imodule];
                xOrig_LowLayer[0] = -XOrigLowLayer[0] + XSizeLowLayer[0] + XModuleShifts[istation][imodule];
                yOrig_LowLayer[0] = YOrigLowLayer[0] + YModuleShifts[istation][imodule];

                xOrig_UpLayer[1] = -XOrigUpLayer[1] + XSizeUpLayer[1] + XModuleShifts[istation][imodule];
                yOrig_UpLayer[1] = YOrigUpLayer[1] + YModuleShifts[istation][imodule];
                xOrig_LowLayer[1] = -XOrigLowLayer[1] + XSizeLowLayer[1] + XModuleShifts[istation][imodule];
                yOrig_LowLayer[1] = YOrigLowLayer[1] + YModuleShifts[istation][imodule];

                xSize_UpLayer[0] = XSizeUpLayer[0];
                ySize_UpLayer[0] = YSizeUpLayer[0];
                xSize_LowLayer[0] = XSizeLowLayer[0];
                ySize_LowLayer[0] = YSizeLowLayer[0];

                xSize_UpLayer[1] = XSizeUpLayer[1];
                ySize_UpLayer[1] = YSizeUpLayer[1];
                xSize_LowLayer[1] = XSizeLowLayer[1];
                ySize_LowLayer[1] = YSizeLowLayer[1];

                xLBorder_UpLayer[0] = -XRightBorderUpperlayer[0] + XModuleShifts[istation][imodule];
                xLBorder_UpLayer[1] = -XRightBorderUpperlayer[1] + XModuleShifts[istation][imodule];
                yLBorder_UpLayer[0] = YRightBorderUpperlayer[0] + YModuleShifts[istation][imodule];
                yLBorder_UpLayer[1] = YRightBorderUpperlayer[1] + YModuleShifts[istation][imodule];
                xLBorder_LowLayer[0] = -XRightBorderLowerlayer[0] + XModuleShifts[istation][imodule];
                xLBorder_LowLayer[1] = -XRightBorderLowerlayer[1] + XModuleShifts[istation][imodule];
                yLBorder_LowLayer[0] = YRightBorderLowerlayer[0] + YModuleShifts[istation][imodule];
                yLBorder_LowLayer[1] = YRightBorderLowerlayer[1] + YModuleShifts[istation][imodule];

                xRBorder_UpLayer[0] = -XLeftBorderUpperlayer[0] + XModuleShifts[istation][imodule];
                xRBorder_UpLayer[1] = -XLeftBorderUpperlayer[1] + XModuleShifts[istation][imodule];
                yRBorder_UpLayer[0] = YLeftBorderUpperlayer[0] + YModuleShifts[istation][imodule];
                yRBorder_UpLayer[1] = YLeftBorderUpperlayer[1] + YModuleShifts[istation][imodule];
                xRBorder_LowLayer[0] = -XLeftBorderLowerlayer[0] + XModuleShifts[istation][imodule];
                xRBorder_LowLayer[1] = -XLeftBorderLowerlayer[1] + XModuleShifts[istation][imodule];
                yRBorder_LowLayer[0] = YLeftBorderLowerlayer[0] + YModuleShifts[istation][imodule];
                yRBorder_LowLayer[1] = YLeftBorderLowerlayer[1] + YModuleShifts[istation][imodule];

                for(Int_t ilayer = 0; ilayer < 2; ++ilayer) {
                    for(Int_t ipoint = 0; ipoint < 3; ++ipoint) {
                        xDeadZone[ilayer][ipoint] = -XDeadZone[ilayer][ipoint] + XModuleShifts[istation][imodule];
                        yDeadZone[ilayer][ipoint] = YDeadZone[ilayer][ipoint] + YModuleShifts[istation][imodule];
                    }
                }

                if(ModulesTypes[istation][imodule] == ModuleType::BEVEL_MODULE || ModulesTypes[istation][imodule] == ModuleType::BEVEL_MODULE_HALF) {
                    isDeadZone_UpLayer = false;
                    isDeadZone_LowLayer = true;
                }

                if(StripDirection[0] == LeftToRight) stripDirection[0] = RightToLeft; else stripDirection[0] = LeftToRight;
                if(StripDirection[1] == LeftToRight) stripDirection[1] = RightToLeft; else stripDirection[1] = LeftToRight;

                stripAngle[0] = -StripAngle[0];
                stripAngle[1] = -StripAngle[1];

                firstStripNumber_UpLayer[0] = FirstStripNumberUpLayer[0];
                firstStripNumber_UpLayer[1] = FirstStripNumberUpLayer[1];
                firstStripNumber_LowLayer[0] = FirstStripNumberLowLayer[0];
                firstStripNumber_LowLayer[1] = FirstStripNumberLowLayer[1];
            }

            if( ModulesYRotations[istation][imodule] == false && ModulesXRotations[istation][imodule] == true ) {

                xOrig_UpLayer[0] = XOrigUpLayer[0] + XModuleShifts[istation][imodule];
                yOrig_UpLayer[0] = -YOrigUpLayer[0] + YMiddleBorderLowerPart[0] + YMiddleBorderUpperPart[0] + YGapBetweenPartsOfModule + YModuleShifts[istation][imodule];
                xOrig_LowLayer[0] = XOrigLowLayer[0] + XModuleShifts[istation][imodule];
                yOrig_LowLayer[0] = -YOrigLowLayer[0] - YSizeLowLayer[0] - YMiddleBorderLowerPart[0] - YMiddleBorderUpperPart[0] - YGapBetweenPartsOfModule - YSizeUpLayer[0] + YModuleShifts[istation][imodule];

                xOrig_UpLayer[1] = XOrigUpLayer[1] + XModuleShifts[istation][imodule];
                yOrig_UpLayer[1] = -YOrigUpLayer[1] + YMiddleBorderLowerPart[1] + YMiddleBorderUpperPart[1] + YGapBetweenPartsOfModule + YModuleShifts[istation][imodule];
                xOrig_LowLayer[1] = XOrigLowLayer[1] + XModuleShifts[istation][imodule];
                yOrig_LowLayer[1] = -YOrigLowLayer[1] - YSizeLowLayer[1] - YMiddleBorderLowerPart[1] - YMiddleBorderUpperPart[1] - YGapBetweenPartsOfModule - YSizeUpLayer[1] + YModuleShifts[istation][imodule];

                xSize_UpLayer[0] = XSizeLowLayer[0];
                ySize_UpLayer[0] = YSizeLowLayer[0];
                xSize_LowLayer[0] = XSizeUpLayer[0];
                ySize_LowLayer[0] = YSizeUpLayer[0];

                xSize_UpLayer[1] = XSizeLowLayer[1];
                ySize_UpLayer[1] = YSizeLowLayer[1];
                xSize_LowLayer[1] = XSizeUpLayer[1];
                ySize_LowLayer[1] = YSizeUpLayer[1];

                xLBorder_UpLayer[0] = XLeftBorderLowerlayer[0] + XModuleShifts[istation][imodule];
                xLBorder_UpLayer[1] = XLeftBorderLowerlayer[1] + XModuleShifts[istation][imodule];
                yLBorder_UpLayer[0] = -YLeftBorderLowerlayer[0] + YModuleShifts[istation][imodule];
                yLBorder_UpLayer[1] = -YLeftBorderLowerlayer[1] + YModuleShifts[istation][imodule];
                xLBorder_LowLayer[0] = XLeftBorderUpperlayer[0] + XModuleShifts[istation][imodule];
                xLBorder_LowLayer[1] = XLeftBorderUpperlayer[1] + XModuleShifts[istation][imodule];
                yLBorder_LowLayer[0] = -YLeftBorderUpperlayer[0] + YModuleShifts[istation][imodule];
                yLBorder_LowLayer[1] = -YLeftBorderUpperlayer[1] + YModuleShifts[istation][imodule];

                xRBorder_UpLayer[0] = XRightBorderLowerlayer[0] + XModuleShifts[istation][imodule];
                xRBorder_UpLayer[1] = XRightBorderLowerlayer[1] + XModuleShifts[istation][imodule];
                yRBorder_UpLayer[0] = -YRightBorderLowerlayer[0] + YModuleShifts[istation][imodule];
                yRBorder_UpLayer[1] = -YRightBorderLowerlayer[1] + YModuleShifts[istation][imodule];
                xRBorder_LowLayer[0] = XRightBorderUpperlayer[0] + XModuleShifts[istation][imodule];
                xRBorder_LowLayer[1] = XRightBorderUpperlayer[1] + XModuleShifts[istation][imodule];
                yRBorder_LowLayer[0] = -YRightBorderUpperlayer[0] + YModuleShifts[istation][imodule];
                yRBorder_LowLayer[1] = -YRightBorderUpperlayer[1] + YModuleShifts[istation][imodule];

                for(Int_t ilayer = 0; ilayer < 2; ++ilayer) {
                    for(Int_t ipoint = 0; ipoint < 3; ++ipoint) {
                        xDeadZone[ilayer][ipoint] = XDeadZone[ilayer][ipoint] + XModuleShifts[istation][imodule];
                        yDeadZone[ilayer][ipoint] = -YDeadZone[ilayer][ipoint] + YModuleShifts[istation][imodule];
                    }
                }

                if(ModulesTypes[istation][imodule] == ModuleType::BEVEL_MODULE || ModulesTypes[istation][imodule] == ModuleType::BEVEL_MODULE_HALF) {
                    isDeadZone_UpLayer = true;
                    isDeadZone_LowLayer = false;
                }

                if(StripDirection[0] == LeftToRight) stripDirection[0] = LeftToRight; else stripDirection[0] = RightToLeft;
                if(StripDirection[1] == LeftToRight) stripDirection[1] = LeftToRight; else stripDirection[1] = RightToLeft;

                stripAngle[0] = -StripAngle[0];
                stripAngle[1] = -StripAngle[1];

                firstStripNumber_UpLayer[0] = FirstStripNumberLowLayer[0];
                firstStripNumber_UpLayer[1] = FirstStripNumberLowLayer[1];
                firstStripNumber_LowLayer[0] = FirstStripNumberUpLayer[0];
                firstStripNumber_LowLayer[1] = FirstStripNumberUpLayer[1];
            }

            if( ModulesYRotations[istation][imodule] == true && ModulesXRotations[istation][imodule] == true ) {

                xOrig_UpLayer[0] = -XOrigUpLayer[0] + XSizeUpLayer[0] + XModuleShifts[istation][imodule];
                yOrig_UpLayer[0] = -YOrigUpLayer[0] + YMiddleBorderLowerPart[0] + YMiddleBorderUpperPart[0] + YGapBetweenPartsOfModule + YModuleShifts[istation][imodule];
                xOrig_LowLayer[0] = -XOrigLowLayer[0] + XSizeLowLayer[0] + XModuleShifts[istation][imodule];
                yOrig_LowLayer[0] = -YOrigLowLayer[0] - YSizeLowLayer[0] - YMiddleBorderLowerPart[0] - YMiddleBorderUpperPart[0] - YGapBetweenPartsOfModule - YSizeUpLayer[0] + YModuleShifts[istation][imodule];

                xOrig_UpLayer[1] = -XOrigUpLayer[1] + XSizeUpLayer[1] + XModuleShifts[istation][imodule];
                yOrig_UpLayer[1] = -YOrigUpLayer[1] + YMiddleBorderLowerPart[1] + YMiddleBorderUpperPart[1] + YGapBetweenPartsOfModule + YModuleShifts[istation][imodule];
                xOrig_LowLayer[1] = -XOrigLowLayer[1] + XSizeLowLayer[1] + XModuleShifts[istation][imodule];
                yOrig_LowLayer[1] = -YOrigLowLayer[1] - YSizeLowLayer[1] - YMiddleBorderLowerPart[1] - YMiddleBorderUpperPart[1] - YGapBetweenPartsOfModule - YSizeUpLayer[1] + YModuleShifts[istation][imodule];

                xSize_UpLayer[0] = XSizeLowLayer[0];
                ySize_UpLayer[0] = YSizeLowLayer[0];
                xSize_LowLayer[0] = XSizeUpLayer[0];
                ySize_LowLayer[0] = YSizeUpLayer[0];

                xSize_UpLayer[1] = XSizeLowLayer[1];
                ySize_UpLayer[1] = YSizeLowLayer[1];
                xSize_LowLayer[1] = XSizeUpLayer[1];
                ySize_LowLayer[1] = YSizeUpLayer[1];

                xLBorder_UpLayer[0] = -XRightBorderLowerlayer[0] + XModuleShifts[istation][imodule];
                xLBorder_UpLayer[1] = -XRightBorderLowerlayer[1] + XModuleShifts[istation][imodule];
                yLBorder_UpLayer[0] = -YRightBorderLowerlayer[0] + YModuleShifts[istation][imodule];
                yLBorder_UpLayer[1] = -YRightBorderLowerlayer[1] + YModuleShifts[istation][imodule];
                xLBorder_LowLayer[0] = -XRightBorderUpperlayer[0] + XModuleShifts[istation][imodule];
                xLBorder_LowLayer[1] = -XRightBorderUpperlayer[1] + XModuleShifts[istation][imodule];
                yLBorder_LowLayer[0] = -YRightBorderUpperlayer[0] + YModuleShifts[istation][imodule];
                yLBorder_LowLayer[1] = -YRightBorderUpperlayer[1] + YModuleShifts[istation][imodule];

                xRBorder_UpLayer[0] = -XLeftBorderLowerlayer[0] + XModuleShifts[istation][imodule];
                xRBorder_UpLayer[1] = -XLeftBorderLowerlayer[1] + XModuleShifts[istation][imodule];
                yRBorder_UpLayer[0] = -YLeftBorderLowerlayer[0] + YModuleShifts[istation][imodule];
                yRBorder_UpLayer[1] = -YLeftBorderLowerlayer[1] + YModuleShifts[istation][imodule];
                xRBorder_LowLayer[0] = -XLeftBorderUpperlayer[0] + XModuleShifts[istation][imodule];
                xRBorder_LowLayer[1] = -XLeftBorderUpperlayer[1] + XModuleShifts[istation][imodule];
                yRBorder_LowLayer[0] = -YLeftBorderUpperlayer[0] + YModuleShifts[istation][imodule];
                yRBorder_LowLayer[1] = -YLeftBorderUpperlayer[1] + YModuleShifts[istation][imodule];

                for(Int_t ilayer = 0; ilayer < 2; ++ilayer) {
                    for(Int_t ipoint = 0; ipoint < 3; ++ipoint) {
                        xDeadZone[ilayer][ipoint] = -XDeadZone[ilayer][ipoint] + XModuleShifts[istation][imodule];
                        yDeadZone[ilayer][ipoint] = -YDeadZone[ilayer][ipoint] + YModuleShifts[istation][imodule];
                    }
                }

                if(ModulesTypes[istation][imodule] == ModuleType::BEVEL_MODULE || ModulesTypes[istation][imodule] == ModuleType::BEVEL_MODULE_HALF) {
                    isDeadZone_UpLayer = true;
                    isDeadZone_LowLayer = false;
                }

                if(StripDirection[0] == LeftToRight) stripDirection[0] = RightToLeft; else stripDirection[0] = LeftToRight;
                if(StripDirection[1] == LeftToRight) stripDirection[1] = RightToLeft; else stripDirection[1] = LeftToRight;

                stripAngle[0] = StripAngle[0];
                stripAngle[1] = StripAngle[1];

                firstStripNumber_UpLayer[0] = FirstStripNumberLowLayer[0];
                firstStripNumber_UpLayer[1] = FirstStripNumberLowLayer[1];
                firstStripNumber_LowLayer[0] = FirstStripNumberUpLayer[0];
                firstStripNumber_LowLayer[1] = FirstStripNumberUpLayer[1];
            }

            cout << "   ---\n";
            cout << "   Orig_UpLayer[0](x:y) = ( " << xOrig_UpLayer[0] << " : " << yOrig_UpLayer[0] << " )\n";
            cout << "   Orig_LowLayer[0](x:y) = ( " << xOrig_LowLayer[0] << " : " << yOrig_LowLayer[0] << " )\n";
            cout << "   Orig_UpLayer[1](x:y) = ( " << xOrig_UpLayer[1] << " : " << yOrig_UpLayer[1] << " )\n";
            cout << "   Orig_LowLayer[1](x:y) = ( " << xOrig_LowLayer[1] << " : " << yOrig_LowLayer[1] << " )\n";
            cout << "\n\n";

            cout << "   ---\n";
            cout << "   Size_UpLayer[0](dx:dy) = ( " << xSize_UpLayer[0] << " : " << ySize_UpLayer[0] << " )\n";
            cout << "   Size_LowLayer[0](dx:dy) = ( " << xSize_LowLayer[0] << " : " << ySize_LowLayer[0] << " )\n";
            cout << "   Size_UpLayer[1](dx:dy) = ( " << xSize_UpLayer[1] << " : " << ySize_UpLayer[1] << " )\n";
            cout << "   Size_LowLayer[1](dx:dy) = ( " << xSize_LowLayer[1] << " : " << ySize_LowLayer[1] << " )\n";
            cout << "\n\n";

            cout << "   ---\n";
            cout << "   LBorder_UpLayer[0](x:y) = ( " << xLBorder_UpLayer[0] << " : " << yLBorder_UpLayer[0] << " )\n";
            cout << "   RBorder_UpLayer[0](x:y) = ( " << xRBorder_UpLayer[0] << " : " << yRBorder_UpLayer[0] << " )\n";
            cout << "\n";
            cout << "   LBorder_LowLayer[0](x:y) = ( " << xLBorder_LowLayer[0] << " : " << yLBorder_LowLayer[0] << " )\n";
            cout << "   RBorder_LowLayer[0](x:y) = ( " << xRBorder_LowLayer[0] << " : " << yRBorder_LowLayer[0] << " )\n";
            cout << "\n";

            cout << "   LBorder_UpLayer[1](x:y) = ( " << xLBorder_UpLayer[1] << " : " << yLBorder_UpLayer[1] << " )\n";
            cout << "   RBorder_UpLayer[1](x:y) = ( " << xRBorder_UpLayer[1] << " : " << yRBorder_UpLayer[1] << " )\n";
            cout << "\n";
            cout << "   LBorder_LowLayer[1](x:y) = ( " << xLBorder_LowLayer[1] << " : " << yLBorder_LowLayer[1] << " )\n";
            cout << "   RBorder_LowLayer[1](x:y) = ( " << xRBorder_LowLayer[1] << " : " << yRBorder_LowLayer[1] << " )\n";
            cout << "\n\n";

            cout << "   ---\n";
            cout << "   DeadZone[0]:\n";
            cout << "       p0(x:y) = ( " << xDeadZone[0][0] << " : " << yDeadZone[0][0]<< " )\n";
            cout << "       p1(x:y) = ( " << xDeadZone[0][1] << " : " << yDeadZone[0][1]<< " )\n";
            cout << "       p2(x:y) = ( " << xDeadZone[0][2] << " : " << yDeadZone[0][2]<< " )\n";
            cout << "\n";

            cout << "   DeadZone[1]:\n";
            cout << "       p0(x:y) = ( " << xDeadZone[1][0] << " : " << yDeadZone[1][0]<< " )\n";
            cout << "       p1(x:y) = ( " << xDeadZone[1][1] << " : " << yDeadZone[1][1]<< " )\n";
            cout << "       p2(x:y) = ( " << xDeadZone[1][2] << " : " << yDeadZone[1][2]<< " )\n";
            cout << "\n";

            cout << "   ---\n";
            cout << "   stripDirection[0] = ";
            if(stripDirection[0] == LeftToRight) cout << "LeftToRight\n"; else cout << "RightToLeft\n";
            cout << "   stripDirection[1] = ";
            if(stripDirection[1] == LeftToRight) cout << "LeftToRight\n"; else cout << "RightToLeft\n";
            cout << "\n";

            cout << "   ---\n";
            cout << "   pitchValue[0] = " << pitchValue[0] << "\n";
            cout << "   pitchValue[1] = " << pitchValue[1] << "\n";
            cout << "\n";

            cout << "---\n";
            cout << "   stripAngle[0] = " << stripAngle[0] << "\n";
            cout << "   stripAngle[1] = " << stripAngle[1] << "\n";
            cout << "\n";

            cout << "---\n";
            cout << "   firstStripNumber_UpLayer[0] = " << firstStripNumber_UpLayer[0] << "\n";
            cout << "   firstStripNumber_LowLayer[0] = " << firstStripNumber_LowLayer[0] << "\n";
            cout << "   firstStripNumber_UpLayer[1] = " << firstStripNumber_UpLayer[1] << "\n";
            cout << "   firstStripNumber_LowLayer[1] = " << firstStripNumber_LowLayer[1] << "\n";
            cout << "\n";

            if(ModulesTypes[istation][imodule] == ModuleType::BEVEL_MODULE || ModulesTypes[istation][imodule] == ModuleType::RIGHT_ANGLED_MODULE) {

                //upper part of module: straight strips = back side ------------
                SiFileOut << "\t\t\t<Layer";
                SiFileOut << " zoneID=\"" << "0" << "\"";
                SiFileOut << " type=\"" << "lower" << "\"";
                SiFileOut << " firstStripNumber=\"" << firstStripNumber_UpLayer[1] << "\"";
                SiFileOut << " stripAngle=\"" << stripAngle[1] << "\"";
                SiFileOut << " pitch=\"" << pitchValue[1] << "\"";
                SiFileOut << " xorig=\"" << xOrig_UpLayer[1] << "\"";
                SiFileOut << " yorig=\"" << yOrig_UpLayer[1] << "\"";
                SiFileOut << " xsize=\"" << xSize_UpLayer[1] << "\"";
                SiFileOut << " ysize=\"" << ySize_UpLayer[1] << "\"";
                SiFileOut << " stripDirection=\"";
                    if(stripDirection[1] == LeftToRight) SiFileOut << "LeftToRight\""; else SiFileOut << "RightToLeft\"";
                SiFileOut << " lxborder=\"" << xLBorder_UpLayer[1] << "\"";
                SiFileOut << " lyborder=\"" << yLBorder_UpLayer[1] << "\"";
                SiFileOut << " rxborder=\"" << xRBorder_UpLayer[1] << "\"";
                SiFileOut << " ryborder=\"" << yRBorder_UpLayer[1] << "\"";
                SiFileOut << ">";

                if(isDeadZone_UpLayer == true) {
                    SiFileOut << "\n";
                    SiFileOut << "\t\t\t\t<DeadZone>\n";
                    for(Int_t ipoint = 0; ipoint < 3; ++ipoint) {
                        SiFileOut << "\t\t\t\t\t<DeadPoint";
                        SiFileOut << " x=\"" << xDeadZone[1][ipoint] << "\"";
                        SiFileOut << " y=\"" << yDeadZone[1][ipoint] << "\"";
                        SiFileOut << " />\n";
                    }
                    SiFileOut << "\t\t\t\t</DeadZone>\n\t\t\t";
                }
                SiFileOut << "</Layer>\n";
                //--------------------------------------------------------------

                //lower part of module: straight strips = back side ------------
                SiFileOut << "\t\t\t<Layer";
                SiFileOut << " zoneID=\"" << "0" << "\"";
                SiFileOut << " type=\"" << "lower" << "\"";
                SiFileOut << " firstStripNumber=\"" << firstStripNumber_LowLayer[1] << "\"";
                SiFileOut << " stripAngle=\"" << stripAngle[1] << "\"";
                SiFileOut << " pitch=\"" << pitchValue[1] << "\"";
                SiFileOut << " xorig=\"" << xOrig_LowLayer[1] << "\"";
                SiFileOut << " yorig=\"" << yOrig_LowLayer[1] << "\"";
                SiFileOut << " xsize=\"" << xSize_LowLayer[1] << "\"";
                SiFileOut << " ysize=\"" << ySize_LowLayer[1] << "\"";
                SiFileOut << " stripDirection=\"";
                    if(stripDirection[1] == LeftToRight) SiFileOut << "LeftToRight\""; else SiFileOut << "RightToLeft\"";
                SiFileOut << " lxborder=\"" << xLBorder_LowLayer[1] << "\"";
                SiFileOut << " lyborder=\"" << yLBorder_LowLayer[1] << "\"";
                SiFileOut << " rxborder=\"" << xRBorder_LowLayer[1] << "\"";
                SiFileOut << " ryborder=\"" << yRBorder_LowLayer[1] << "\"";
                SiFileOut << ">";

                if(isDeadZone_LowLayer == true) {
                    SiFileOut << "\n";
                    SiFileOut << "\t\t\t\t<DeadZone>\n";
                    for(Int_t ipoint = 0; ipoint < 3; ++ipoint) {
                        SiFileOut << "\t\t\t\t\t<DeadPoint";
                        SiFileOut << " x=\"" << xDeadZone[1][ipoint] << "\"";
                        SiFileOut << " y=\"" << yDeadZone[1][ipoint] << "\"";
                        SiFileOut << " />\n";
                    }
                    SiFileOut << "\t\t\t\t</DeadZone>\n\t\t\t";
                }
                SiFileOut << "</Layer>\n";
                //--------------------------------------------------------------

                //upper part of module: inclined strips = front side -----------
                SiFileOut << "\t\t\t<Layer";
                SiFileOut << " zoneID=\"" << "1" << "\"";
                SiFileOut << " type=\"" << "upper" << "\"";
                SiFileOut << " firstStripNumber=\"" << firstStripNumber_UpLayer[0] << "\"";
                SiFileOut << " stripAngle=\"" << stripAngle[0] << "\"";
                SiFileOut << " pitch=\"" << pitchValue[0] << "\"";
                SiFileOut << " xorig=\"" << xOrig_UpLayer[0] << "\"";
                SiFileOut << " yorig=\"" << yOrig_UpLayer[0] << "\"";
                SiFileOut << " xsize=\"" << xSize_UpLayer[0] << "\"";
                SiFileOut << " ysize=\"" << ySize_UpLayer[0] << "\"";
                SiFileOut << " stripDirection=\"";
                    if(stripDirection[0] == LeftToRight) SiFileOut << "LeftToRight\""; else SiFileOut << "RightToLeft\"";
                SiFileOut << " lxborder=\"" << xLBorder_UpLayer[0] << "\"";
                SiFileOut << " lyborder=\"" << yLBorder_UpLayer[0] << "\"";
                SiFileOut << " rxborder=\"" << xRBorder_UpLayer[0] << "\"";
                SiFileOut << " ryborder=\"" << yRBorder_UpLayer[0] << "\"";
                SiFileOut << ">";

                if(isDeadZone_UpLayer == true) {
                    SiFileOut << "\n";
                    SiFileOut << "\t\t\t\t<DeadZone>\n";
                    for(Int_t ipoint = 0; ipoint < 3; ++ipoint) {
                        SiFileOut << "\t\t\t\t\t<DeadPoint";
                        SiFileOut << " x=\"" << xDeadZone[0][ipoint] << "\"";
                        SiFileOut << " y=\"" << yDeadZone[0][ipoint] << "\"";
                        SiFileOut << " />\n";
                    }
                    SiFileOut << "\t\t\t\t</DeadZone>\n\t\t\t";
                }
                SiFileOut << "</Layer>\n";
                //--------------------------------------------------------------

                //lower part of module: inclined strips = front side -----------
                SiFileOut << "\t\t\t<Layer";
                SiFileOut << " zoneID=\"" << "1" << "\"";
                SiFileOut << " type=\"" << "upper" << "\"";
                SiFileOut << " firstStripNumber=\"" << firstStripNumber_LowLayer[0] << "\"";
                SiFileOut << " stripAngle=\"" << stripAngle[0] << "\"";
                SiFileOut << " pitch=\"" << pitchValue[0] << "\"";
                SiFileOut << " xorig=\"" << xOrig_LowLayer[0] << "\"";
                SiFileOut << " yorig=\"" << yOrig_LowLayer[0] << "\"";
                SiFileOut << " xsize=\"" << xSize_LowLayer[0] << "\"";
                SiFileOut << " ysize=\"" << ySize_LowLayer[0] << "\"";
                SiFileOut << " stripDirection=\"";
                    if(stripDirection[0] == LeftToRight) SiFileOut << "LeftToRight\""; else SiFileOut << "RightToLeft\"";
                SiFileOut << " lxborder=\"" << xLBorder_LowLayer[0] << "\"";
                SiFileOut << " lyborder=\"" << yLBorder_LowLayer[0] << "\"";
                SiFileOut << " rxborder=\"" << xRBorder_LowLayer[0] << "\"";
                SiFileOut << " ryborder=\"" << yRBorder_LowLayer[0] << "\"";
                SiFileOut << ">";

                if(isDeadZone_LowLayer == true) {
                    SiFileOut << "\n";
                    SiFileOut << "\t\t\t\t<DeadZone>\n";
                    for(Int_t ipoint = 0; ipoint < 3; ++ipoint) {
                        SiFileOut << "\t\t\t\t\t<DeadPoint";
                        SiFileOut << " x=\"" << xDeadZone[0][ipoint] << "\"";
                        SiFileOut << " y=\"" << yDeadZone[0][ipoint] << "\"";
                        SiFileOut << " />\n";
                    }
                    SiFileOut << "\t\t\t\t</DeadZone>\n\t\t\t";
                }
                SiFileOut << "</Layer>\n";
                //--------------------------------------------------------------

                SiFileOut << "\t\t</Module>\n";
                SiFileOut << "\n";
            }

            if(ModulesTypes[istation][imodule] == ModuleType::BEVEL_MODULE_HALF || ModulesTypes[istation][imodule] == ModuleType::RIGHT_ANGLED_MODULE_HALF) {

                Double_t xOrig[2];
                Double_t yOrig[2];
                Double_t xSize[2];
                Double_t ySize[2];
                Double_t xLBorder[2];
                Double_t yLBorder[2];
                Double_t xRBorder[2];
                Double_t yRBorder[2];
                Bool_t isDeadZone;

                if(ModulesXRotations[istation][imodule] == false) {
                    xOrig[0] =  xOrig_LowLayer[0];
                    xOrig[1] =  xOrig_LowLayer[1];
                    yOrig[0] = yOrig_LowLayer[0];
                    yOrig[1] = yOrig_LowLayer[1];
                    xSize[0] = xSize_LowLayer[0];
                    xSize[1] = xSize_LowLayer[1];
                    ySize[0] = ySize_LowLayer[0];
                    ySize[1] = ySize_LowLayer[1];
                    xLBorder[0] = xLBorder_LowLayer[0];
                    xLBorder[1] = xLBorder_LowLayer[1];
                    yLBorder[0] = yLBorder_LowLayer[0];
                    yLBorder[1] = yLBorder_LowLayer[1];
                    xRBorder[0] = xRBorder_LowLayer[0];
                    xRBorder[1] = xRBorder_LowLayer[1];
                    yRBorder[0] = yRBorder_LowLayer[0];
                    yRBorder[1] = yRBorder_LowLayer[1];
                    isDeadZone = isDeadZone_LowLayer;
                }
                if(ModulesXRotations[istation][imodule] == true) {
                    xOrig[0] =  xOrig_UpLayer[0];
                    xOrig[1] =  xOrig_UpLayer[1];
                    yOrig[0] = yOrig_UpLayer[0];
                    yOrig[1] = yOrig_UpLayer[1];
                    xSize[0] = xSize_UpLayer[0];
                    xSize[1] = xSize_UpLayer[1];
                    ySize[0] = ySize_UpLayer[0];
                    ySize[1] = ySize_UpLayer[1];
                    xLBorder[0] = xLBorder_UpLayer[0];
                    xLBorder[1] = xLBorder_UpLayer[1];
                    yLBorder[0] = yLBorder_UpLayer[0];
                    yLBorder[1] = yLBorder_UpLayer[1];
                    xRBorder[0] = xRBorder_UpLayer[0];
                    xRBorder[1] = xRBorder_UpLayer[1];
                    yRBorder[0] = yRBorder_UpLayer[0];
                    yRBorder[1] = yRBorder_UpLayer[1];
                    isDeadZone = isDeadZone_UpLayer;
                }

                //one part of module: straight strips = back side --------------
                SiFileOut << "\t\t\t<Layer";
                SiFileOut << " zoneID=\"" << "0" << "\"";
                SiFileOut << " type=\"" << "lower" << "\"";
                SiFileOut << " firstStripNumber=\"" << 0 << "\"";
                SiFileOut << " stripAngle=\"" << stripAngle[1] << "\"";
                SiFileOut << " pitch=\"" << pitchValue[1] << "\"";
                SiFileOut << " xorig=\"" << xOrig[1] << "\"";
                SiFileOut << " yorig=\"" << yOrig[1] << "\"";
                SiFileOut << " xsize=\"" << xSize[1] << "\"";
                SiFileOut << " ysize=\"" << ySize[1] << "\"";
                SiFileOut << " stripDirection=\"";
                    if(stripDirection[1] == LeftToRight) SiFileOut << "LeftToRight\""; else SiFileOut << "RightToLeft\"";
                SiFileOut << " lxborder=\"" << xLBorder[1] << "\"";
                SiFileOut << " lyborder=\"" << yLBorder[1] << "\"";
                SiFileOut << " rxborder=\"" << xRBorder[1] << "\"";
                SiFileOut << " ryborder=\"" << yRBorder[1] << "\"";
                SiFileOut << ">";

                if(isDeadZone == true) {
                    SiFileOut << "\n";
                    SiFileOut << "\t\t\t\t<DeadZone>\n";
                    for(Int_t ipoint = 0; ipoint < 3; ++ipoint) {
                        SiFileOut << "\t\t\t\t\t<DeadPoint";
                        SiFileOut << " x=\"" << xDeadZone[1][ipoint] << "\"";
                        SiFileOut << " y=\"" << yDeadZone[1][ipoint] << "\"";
                        SiFileOut << " />\n";
                    }
                    SiFileOut << "\t\t\t\t</DeadZone>\n\t\t\t";
                }
                SiFileOut << "</Layer>\n";
                //--------------------------------------------------------------

                //one part of module: inclined strips = front side -------------
                SiFileOut << "\t\t\t<Layer";
                SiFileOut << " zoneID=\"" << "1" << "\"";
                SiFileOut << " type=\"" << "upper" << "\"";
                SiFileOut << " firstStripNumber=\"" << 0 << "\"";
                SiFileOut << " stripAngle=\"" << stripAngle[0] << "\"";
                SiFileOut << " pitch=\"" << pitchValue[0] << "\"";
                SiFileOut << " xorig=\"" << xOrig[0] << "\"";
                SiFileOut << " yorig=\"" << yOrig[0] << "\"";
                SiFileOut << " xsize=\"" << xSize[0] << "\"";
                SiFileOut << " ysize=\"" << ySize[0] << "\"";
                SiFileOut << " stripDirection=\"";
                    if(stripDirection[0] == LeftToRight) SiFileOut << "LeftToRight\""; else SiFileOut << "RightToLeft\"";
                SiFileOut << " lxborder=\"" << xLBorder[0] << "\"";
                SiFileOut << " lyborder=\"" << yLBorder[0] << "\"";
                SiFileOut << " rxborder=\"" << xRBorder[0] << "\"";
                SiFileOut << " ryborder=\"" << yRBorder[0] << "\"";
                SiFileOut << ">";

                if(isDeadZone == true) {
                    SiFileOut << "\n";
                    SiFileOut << "\t\t\t\t<DeadZone>\n";
                    for(Int_t ipoint = 0; ipoint < 3; ++ipoint) {
                        SiFileOut << "\t\t\t\t\t<DeadPoint";
                        SiFileOut << " x=\"" << xDeadZone[0][ipoint] << "\"";
                        SiFileOut << " y=\"" << yDeadZone[0][ipoint] << "\"";
                        SiFileOut << " />\n";
                    }
                    SiFileOut << "\t\t\t\t</DeadZone>\n\t\t\t";
                }
                SiFileOut << "</Layer>\n";
                //--------------------------------------------------------------

                SiFileOut << "\t\t</Module>\n";
                SiFileOut << "\n";
            }
        }

        SiFileOut << "\t</Station>\n";
        SiFileOut << "\n";

        cout << "------------------------------------------------------------\n";
    }

    SiFileOut << "</StationSet>\n";

    SiFileOut.close();
}