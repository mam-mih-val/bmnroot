/*
 * Description:
 * Macro for calculating parameters of SILICON detector and writing them
 * into the XML-file needed for simulation and reconstruction procedures.
 *
 * !!! IMPORTANT !!!
 * The obtained XML-file SHOULD BE MOVED into the 'parameters/silicon/XMLConfigs' directory
 *
 * SILICON configuration: Run8_5stations (5 SI-stations (6-10-14-18-22 mods))
 *
 * Created: 05.01.2022
 * Author: D. Baranov
 */

#include "Rtypes.h"

R__ADD_INCLUDE_PATH($VMCWORKDIR)
R__LOAD_LIBRARY(libBmnData.so)

#include "TString.h"

#include <fstream>

using namespace TMath;
using namespace std;

//Configuration of modules -----------------------------------------------------

enum ModuleSize {STANDARD_MODULE, LONG_MODULE}; //STANDARD_MODULE is (63x126)mm, LONG_MODULE is (63x186)mm
const unsigned int NModuleSizes = 2;

Double_t YGapBetweenPartsOfModule[NModuleSizes] = { 0.0035/*STANDARD_MODULE*/, 0.0035/*LONG_MODULE*/ };

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

//The next variables have two indices: the first one is a mod-size type, the second - a layer type
// 0 - inclined strips (front side), 1 - straight strips (back side)

Double_t XLeftBorder[NModuleSizes][2] = {
    { 0.1145, 0.10995 }, /*STANDARD_MODULE*/
    { 0.1118, 0.11005 }  /*LONG_MODULE*/
};

Double_t XRightBorder[NModuleSizes][2] = {
    { 0.1118, 0.11005 }, /*STANDARD_MODULE*/
    { 0.1118, 0.11005 }  /*LONG_MODULE*/
};

Double_t YTopBorder[NModuleSizes][2] = {
    { 0.1143, 0.1143 }, /*STANDARD_MODULE*/
    { 0.1148, 0.1055 }  /*LONG_MODULE*/
};

Double_t YMiddleBorderUpperPart[NModuleSizes][2] = {
    { 0.1121, 0.1055 }, /*STANDARD_MODULE*/
    { 0.1148, 0.1055 }  /*LONG_MODULE*/
};

Double_t YMiddleBorderLowerPart[NModuleSizes][2] = {
    { 0.1147, 0.1055 }, /*STANDARD_MODULE*/
    { 0.1148, 0.1055 }  /*LONG_MODULE*/
};

Double_t YBottomBorder[NModuleSizes][2] = {
    { 0.1148, 0.1055 }, /*STANDARD_MODULE*/
    { 0.1148, 0.1055 }  /*LONG_MODULE*/
};

Double_t XYBevelBorder[NModuleSizes][2] = {
    { 0.1146, 0.1076 }, /*STANDARD_MODULE*/
    { 0.1148, 0.1055 }  /*LONG_MODULE*/
};
//------------------------------------------------------------------------------


//Parameters of layers ---------------------------------------------------------

Double_t XShiftFirstStripUpLayer[NModuleSizes][2] = {
    { 0.00795, 0.0 }, /*STANDARD_MODULE*/
    { 0.00261, 0.0 }  /*LONG_MODULE*/
};

Double_t XShiftFirstStripLowLayer[NModuleSizes][2] = {
    { 0.00780, 0.0 }, /*STANDARD_MODULE*/
    { 0.00261, 0.0 }  /*LONG_MODULE*/
};

Double_t XShiftLastStripUpLayer[NModuleSizes][2] = {
    { 0.00675, 0.0 }, /*STANDARD_MODULE*/
    { 0.00255, 0.0 }  /*LONG_MODULE*/
};

Double_t XShiftLastStripLowLayer[NModuleSizes][2] = {
    { 0.00675, 0.0 }, /*STANDARD_MODULE*/
    { 0.00255, 0.0 }  /*LONG_MODULE*/
};

Double_t YShiftFirstStripUpLayer[NModuleSizes][2] = {
    { 0.0, 0.0 }, /*STANDARD_MODULE*/
    { 0.0, 0.0 }  /*LONG_MODULE*/
};

Double_t YShiftFirstStripLowLayer[NModuleSizes][2] = {
    { 0.0, 0.0 }, /*STANDARD_MODULE*/
    { 0.0, 0.0 }  /*LONG_MODULE*/
};

Double_t YShiftLastStripUpLayer[NModuleSizes][2] = {
    { 0.0, 0.0 }, /*STANDARD_MODULE*/
    { 0.0, 0.0 }  /*LONG_MODULE*/
};

Double_t YShiftLastStripLowLayer[NModuleSizes][2] = {
    { 0.0, 0.0 }, /*STANDARD_MODULE*/
    { 0.0, 0.0 }  /*LONG_MODULE*/
};
//------------------------------------------------------------------------------

Double_t XOrigUpLayer[NModuleSizes][2] = {
    { XModuleSize[STANDARD_MODULE] - XLeftBorder[STANDARD_MODULE][0], XModuleSize[STANDARD_MODULE] - XLeftBorder[STANDARD_MODULE][1] }, /*STANDARD_MODULE*/
    { XModuleSize[LONG_MODULE] - XLeftBorder[LONG_MODULE][0], XModuleSize[LONG_MODULE] - XLeftBorder[LONG_MODULE][1] } /*LONG_MODULE*/
};
//Double_t XOrigUpLayer[2] = { XModuleSize - XLeftBorder[0], XModuleSize - XLeftBorder[1] };

Double_t XOrigLowLayer[NModuleSizes][2] = {
    { XModuleSize[STANDARD_MODULE] - XLeftBorder[STANDARD_MODULE][0], XModuleSize[STANDARD_MODULE] - XLeftBorder[STANDARD_MODULE][1] }, /*STANDARD_MODULE*/
    { XModuleSize[LONG_MODULE] - XLeftBorder[LONG_MODULE][0], XModuleSize[LONG_MODULE] - XLeftBorder[LONG_MODULE][1] }  /*LONG_MODULE*/
};
//Double_t XOrigLowLayer[2] = { XModuleSize - XLeftBorder[0], XModuleSize - XLeftBorder[1] };

Double_t YOrigUpLayer[NModuleSizes][2] = {
    { YModuleSize[STANDARD_MODULE] - YModuleSizeUpperPart[STANDARD_MODULE] + YMiddleBorderUpperPart[STANDARD_MODULE][0], YModuleSize[STANDARD_MODULE] - YModuleSizeUpperPart[STANDARD_MODULE] + YMiddleBorderUpperPart[STANDARD_MODULE][1] }, /*STANDARD_MODULE*/
    { YModuleSize[LONG_MODULE] - YModuleSizeUpperPart[LONG_MODULE] + YMiddleBorderUpperPart[LONG_MODULE][0], YModuleSize[LONG_MODULE] - YModuleSizeUpperPart[LONG_MODULE] + YMiddleBorderUpperPart[LONG_MODULE][1] }  /*LONG_MODULE*/
};
//Double_t YOrigUpLayer[2] = { YModuleSize - YModuleSizeUpperPart + YMiddleBorderUpperPart[0], YModuleSize - YModuleSizeUpperPart + YMiddleBorderUpperPart[1] };

Double_t YOrigLowLayer[NModuleSizes][2] = {
    { YBottomBorder[STANDARD_MODULE][0], YBottomBorder[STANDARD_MODULE][1] }, /*STANDARD_MODULE*/
    { YBottomBorder[LONG_MODULE][0], YBottomBorder[LONG_MODULE][1] }  /*LONG_MODULE*/
};
//Double_t YOrigLowLayer[2] = { YBottomBorder[0], YBottomBorder[1] };

Double_t XSizeUpLayer[NModuleSizes][2] = {
    { XModuleSize[STANDARD_MODULE] - (XLeftBorder[STANDARD_MODULE][0] + XRightBorder[STANDARD_MODULE][0]) , XModuleSize[STANDARD_MODULE] - (XLeftBorder[STANDARD_MODULE][1] + XRightBorder[STANDARD_MODULE][1]) }, /*STANDARD_MODULE*/
    { XModuleSize[LONG_MODULE] - (XLeftBorder[LONG_MODULE][0] + XRightBorder[LONG_MODULE][0]) , XModuleSize[LONG_MODULE] - (XLeftBorder[LONG_MODULE][1] + XRightBorder[LONG_MODULE][1]) }  /*LONG_MODULE*/
};
//Double_t XSizeUpLayer[2] = { XModuleSize - (XLeftBorder[0] + XRightBorder[0]) , XModuleSize - (XLeftBorder[1] + XRightBorder[1]) };

Double_t YSizeUpLayer[NModuleSizes][2] = {
    { YModuleSizeUpperPart[STANDARD_MODULE] - (YTopBorder[STANDARD_MODULE][0] + YMiddleBorderUpperPart[STANDARD_MODULE][0]), YModuleSizeUpperPart[STANDARD_MODULE] - (YTopBorder[STANDARD_MODULE][1] + YMiddleBorderUpperPart[STANDARD_MODULE][1]) }, /*STANDARD_MODULE*/
    { YModuleSizeUpperPart[LONG_MODULE] - (YTopBorder[LONG_MODULE][0] + YMiddleBorderUpperPart[LONG_MODULE][0]), YModuleSizeUpperPart[LONG_MODULE] - (YTopBorder[LONG_MODULE][1] + YMiddleBorderUpperPart[LONG_MODULE][1]) }  /*LONG_MODULE*/
};
//Double_t YSizeUpLayer[2] = { YModuleSizeUpperPart - (YTopBorder[0] + YMiddleBorderUpperPart[0]), YModuleSizeUpperPart - (YTopBorder[1] + YMiddleBorderUpperPart[1]) };

Double_t XSizeLowLayer[NModuleSizes][2] = {
    { XModuleSize[STANDARD_MODULE] - (XLeftBorder[STANDARD_MODULE][0] + XRightBorder[STANDARD_MODULE][0]) , XModuleSize[STANDARD_MODULE] - (XLeftBorder[STANDARD_MODULE][1] + XRightBorder[STANDARD_MODULE][1]) }, /*STANDARD_MODULE*/
    { XModuleSize[LONG_MODULE] - (XLeftBorder[LONG_MODULE][0] + XRightBorder[LONG_MODULE][0]) , XModuleSize[LONG_MODULE] - (XLeftBorder[LONG_MODULE][1] + XRightBorder[LONG_MODULE][1]) }  /*LONG_MODULE*/
};
//Double_t XSizeLowLayer[2] = { XModuleSize - (XLeftBorder[0] + XRightBorder[0]) , XModuleSize - (XLeftBorder[1] + XRightBorder[1]) };

Double_t YSizeLowLayer[NModuleSizes][2] = {
    { YModuleSizeLowerPart[STANDARD_MODULE] - (YBottomBorder[STANDARD_MODULE][0] + YMiddleBorderLowerPart[STANDARD_MODULE][0]), YModuleSizeLowerPart[STANDARD_MODULE] - (YBottomBorder[STANDARD_MODULE][1] + YMiddleBorderLowerPart[STANDARD_MODULE][1]) }, /*STANDARD_MODULE*/
    { YModuleSizeLowerPart[LONG_MODULE] - (YBottomBorder[LONG_MODULE][0] + YMiddleBorderLowerPart[LONG_MODULE][0]), YModuleSizeLowerPart[LONG_MODULE] - (YBottomBorder[LONG_MODULE][1] + YMiddleBorderLowerPart[LONG_MODULE][1]) }  /*LONG_MODULE*/
};
//Double_t YSizeLowLayer[2] = { YModuleSizeLowerPart - (YBottomBorder[0] + YMiddleBorderLowerPart[0]), YModuleSizeLowerPart - (YBottomBorder[1] + YMiddleBorderLowerPart[1]) };


Double_t XLeftBorderUpperlayer[NModuleSizes][2] = {
    { XOrigUpLayer[STANDARD_MODULE][0] - XShiftFirstStripUpLayer[STANDARD_MODULE][0], XOrigUpLayer[STANDARD_MODULE][1] - XShiftLastStripUpLayer[STANDARD_MODULE][1] + XShiftLastStripUpLayer[STANDARD_MODULE][1] }, /*STANDARD_MODULE*/
    { XOrigUpLayer[LONG_MODULE][0] - XShiftFirstStripUpLayer[LONG_MODULE][0], XOrigUpLayer[LONG_MODULE][1] - XShiftLastStripUpLayer[LONG_MODULE][1] + XShiftLastStripUpLayer[LONG_MODULE][1] }  /*LONG_MODULE*/
};
//Double_t XLeftBorderUpperlayer[2] = { XOrigUpLayer[0] - XShiftFirstStripUpLayer[0], XOrigUpLayer[1] - XShiftLastStripUpLayer[1] + XShiftLastStripUpLayer[1] };

Double_t XLeftBorderLowerlayer[NModuleSizes][2] = {
    { XOrigUpLayer[STANDARD_MODULE][0] - XShiftFirstStripLowLayer[STANDARD_MODULE][0], XOrigUpLayer[STANDARD_MODULE][1] - XShiftLastStripLowLayer[STANDARD_MODULE][1] + XShiftLastStripLowLayer[STANDARD_MODULE][1] }, /*STANDARD_MODULE*/
    { XOrigUpLayer[LONG_MODULE][0] - XShiftFirstStripLowLayer[LONG_MODULE][0], XOrigUpLayer[LONG_MODULE][1] - XShiftLastStripLowLayer[LONG_MODULE][1] + XShiftLastStripLowLayer[LONG_MODULE][1] }  /*LONG_MODULE*/
};
//Double_t XLeftBorderLowerlayer[2] = { XOrigUpLayer[0] - XShiftFirstStripLowLayer[0], XOrigUpLayer[1] - XShiftLastStripLowLayer[1] + XShiftLastStripLowLayer[1] };


Double_t XRightBorderUpperlayer[NModuleSizes][2] = {
    { XOrigUpLayer[STANDARD_MODULE][0] - XSizeUpLayer[STANDARD_MODULE][0] + XShiftLastStripUpLayer[STANDARD_MODULE][0], XOrigUpLayer[STANDARD_MODULE][1] - XSizeUpLayer[STANDARD_MODULE][1] + XShiftFirstStripUpLayer[STANDARD_MODULE][1] }, /*STANDARD_MODULE*/
    { XOrigUpLayer[LONG_MODULE][0] - XSizeUpLayer[LONG_MODULE][0] + XShiftLastStripUpLayer[LONG_MODULE][0], XOrigUpLayer[LONG_MODULE][1] - XSizeUpLayer[LONG_MODULE][1] + XShiftFirstStripUpLayer[LONG_MODULE][1] }  /*LONG_MODULE*/
};
//Double_t XRightBorderUpperlayer[2] = { XOrigUpLayer[0] - XSizeUpLayer[0] + XShiftLastStripUpLayer[0], XOrigUpLayer[1] - XSizeUpLayer[1] + XShiftFirstStripUpLayer[1] };


Double_t XRightBorderLowerlayer[NModuleSizes][2] = {
    { XOrigLowLayer[STANDARD_MODULE][0] - XSizeLowLayer[STANDARD_MODULE][0] + XShiftLastStripLowLayer[STANDARD_MODULE][0], XOrigLowLayer[STANDARD_MODULE][1] - XSizeLowLayer[STANDARD_MODULE][1] + XShiftFirstStripLowLayer[STANDARD_MODULE][1] }, /*STANDARD_MODULE*/
    { XOrigLowLayer[LONG_MODULE][0] - XSizeLowLayer[LONG_MODULE][0] + XShiftLastStripLowLayer[LONG_MODULE][0], XOrigLowLayer[LONG_MODULE][1] - XSizeLowLayer[LONG_MODULE][1] + XShiftFirstStripLowLayer[LONG_MODULE][1] }  /*LONG_MODULE*/
};
//Double_t XRightBorderLowerlayer[2] = { XOrigLowLayer[0] - XSizeLowLayer[0] + XShiftLastStripLowLayer[0], XOrigLowLayer[1] - XSizeLowLayer[1] + XShiftFirstStripLowLayer[1] };

Double_t YLeftBorderUpperlayer[NModuleSizes][2] = {
    { YOrigUpLayer[STANDARD_MODULE][0] + YSizeUpLayer[STANDARD_MODULE][0] - YShiftFirstStripUpLayer[STANDARD_MODULE][0], YOrigUpLayer[STANDARD_MODULE][1] + YSizeUpLayer[STANDARD_MODULE][1] - YShiftFirstStripUpLayer[STANDARD_MODULE][1] }, /*STANDARD_MODULE*/
    { YOrigUpLayer[LONG_MODULE][0] + YSizeUpLayer[LONG_MODULE][0] - YShiftFirstStripUpLayer[LONG_MODULE][0], YOrigUpLayer[LONG_MODULE][1] + YSizeUpLayer[LONG_MODULE][1] - YShiftFirstStripUpLayer[LONG_MODULE][1] }  /*LONG_MODULE*/
};
//Double_t YLeftBorderUpperlayer[2] = { YOrigUpLayer[0] + YSizeUpLayer[0] - YShiftFirstStripUpLayer[0], YOrigUpLayer[1] + YSizeUpLayer[1] - YShiftFirstStripUpLayer[1] };


Double_t YLeftBorderLowerlayer[NModuleSizes][2] = {
    { YOrigLowLayer[STANDARD_MODULE][0] + YSizeLowLayer[STANDARD_MODULE][0] - YShiftFirstStripLowLayer[STANDARD_MODULE][0], YOrigLowLayer[STANDARD_MODULE][1] + YSizeLowLayer[STANDARD_MODULE][1] - YShiftFirstStripLowLayer[STANDARD_MODULE][1] }, /*STANDARD_MODULE*/
    { YOrigLowLayer[LONG_MODULE][0] + YSizeLowLayer[LONG_MODULE][0] - YShiftFirstStripLowLayer[LONG_MODULE][0], YOrigLowLayer[LONG_MODULE][1] + YSizeLowLayer[LONG_MODULE][1] - YShiftFirstStripLowLayer[LONG_MODULE][1] }  /*LONG_MODULE*/
};
//Double_t YLeftBorderLowerlayer[2] = { YOrigLowLayer[0] + YSizeLowLayer[0] - YShiftFirstStripLowLayer[0], YOrigLowLayer[1] + YSizeLowLayer[1] - YShiftFirstStripLowLayer[1] };

Double_t YRightBorderUpperlayer[NModuleSizes][2] = {
    { YOrigUpLayer[STANDARD_MODULE][0], YOrigUpLayer[STANDARD_MODULE][1] }, /*STANDARD_MODULE*/
    { YOrigUpLayer[LONG_MODULE][0], YOrigUpLayer[LONG_MODULE][1] }  /*LONG_MODULE*/
};
//Double_t YRightBorderUpperlayer[2] = { YOrigUpLayer[0], YOrigUpLayer[1] };

Double_t YRightBorderLowerlayer[NModuleSizes][2] = {
    { YOrigLowLayer[STANDARD_MODULE][0], YOrigLowLayer[STANDARD_MODULE][1] }, /*STANDARD_MODULE*/
    { YOrigLowLayer[LONG_MODULE][0], YOrigLowLayer[LONG_MODULE][1] }  /*LONG_MODULE*/
};
//Double_t YRightBorderLowerlayer[2] = { YOrigLowLayer[0], YOrigLowLayer[1] };
//------------------------------------------------------------------------------

enum StripNumberingDirection {LeftToRight, RightToLeft};

StripNumberingDirection StripDirection[NModuleSizes][2] = {
    { LeftToRight, RightToLeft }, /*STANDARD_MODULE*/
    { LeftToRight, RightToLeft }  /*LONG_MODULE*/
};
//StripNumberingDirection StripDirection[2] = { LeftToRight, RightToLeft };

Double_t PitchValue[NModuleSizes][2] = {
    { 0.01029019668, 0.0095 }, /*STANDARD_MODULE*/
    { 0.0107150, 0.0095 } /*LONG_MODULE*/
};
//Double_t PitchValue[2] = { 0.01029019668, 0.0095 };

Double_t StripAngle[NModuleSizes][2] = {
    { +2.5, +0.0 }, /*STANDARD_MODULE*/
    { +2.5, +0.0 } /*LONG_MODULE*/
};
//Double_t StripAngle[2] = { +2.5, +0.0 }; // plus - clockwise, minus - anticlockwise

Double_t FirstStripNumberUpLayer[NModuleSizes][2] = {
    { 0, 0 }, /*STANDARD_MODULE*/
    { 0, 0 } /*LONG_MODULE*/
};
//Int_t FirstStripNumberUpLayer[2] = { 0, 0 };

Double_t FirstStripNumberLowLayer[NModuleSizes][2] = {
    { 26, 0 }, /*STANDARD_MODULE*/
    { 37, 0 } /*LONG_MODULE*/
};
//Int_t FirstStripNumberLowLayer[2] = { 26, 0 };

//Dead zones (if you need) -----------------------------------------------------

//find x and y projections of the bevel border thickness -----------------------

Double_t x_delta[NModuleSizes][2] = {
    { XYBevelBorder[STANDARD_MODULE][0]/TMath::Cos(BevelAngleRad[STANDARD_MODULE]), XYBevelBorder[STANDARD_MODULE][1]/TMath::Cos(BevelAngleRad[STANDARD_MODULE]) }, /*STANDARD_MODULE*/
    { XYBevelBorder[LONG_MODULE][0]/TMath::Cos(BevelAngleRad[LONG_MODULE]), XYBevelBorder[LONG_MODULE][1]/TMath::Cos(BevelAngleRad[LONG_MODULE]) } /*LONG_MODULE*/
};
//Double_t x_delta[2] = { XYBevelBorder[0]/TMath::Cos(BevelAngleRad), XYBevelBorder[1]/TMath::Cos(BevelAngleRad) };

Double_t y_delta[NModuleSizes][2] = {
    { XYBevelBorder[STANDARD_MODULE][0]/TMath::Sin(BevelAngleRad[STANDARD_MODULE]), XYBevelBorder[STANDARD_MODULE][1]/TMath::Sin(BevelAngleRad[STANDARD_MODULE]) }, /*STANDARD_MODULE*/
    { XYBevelBorder[LONG_MODULE][0]/TMath::Sin(BevelAngleRad[LONG_MODULE]), XYBevelBorder[LONG_MODULE][1]/TMath::Sin(BevelAngleRad[LONG_MODULE]) } /*LONG_MODULE*/
};
//Double_t y_delta[2] = { XYBevelBorder[0]/TMath::Sin(BevelAngleRad), XYBevelBorder[1]/TMath::Sin(BevelAngleRad) };
//------------------------------------------------------------------------------


//find shifts of the sensitive zone from module borders via the equation of a straight line for the inner bevel

Double_t x_delta_top_BP[NModuleSizes][2] = {
    { XLeftBorder[STANDARD_MODULE][0], XLeftBorder[STANDARD_MODULE][1] }, /*STANDARD_MODULE*/
    { XLeftBorder[LONG_MODULE][0], XLeftBorder[LONG_MODULE][1] } /*LONG_MODULE*/
};
//Double_t x_delta_top_BP[2] = { XLeftBorder[0], XLeftBorder[1] };

Double_t y_delta_top_BP[NModuleSizes][2] = {
    { TMath::Tan(TMath::PiOver2()+BevelAngleRad[STANDARD_MODULE])*x_delta_top_BP[STANDARD_MODULE][0] + (YBevelSize[STANDARD_MODULE]+y_delta[STANDARD_MODULE][0]), TMath::Tan(TMath::PiOver2()+BevelAngleRad[STANDARD_MODULE])*x_delta_top_BP[STANDARD_MODULE][1] + (YBevelSize[STANDARD_MODULE]+y_delta[STANDARD_MODULE][1]) }, /*STANDARD_MODULE*/
    { TMath::Tan(TMath::PiOver2()+BevelAngleRad[LONG_MODULE])*x_delta_top_BP[LONG_MODULE][0] + (YBevelSize[LONG_MODULE]+y_delta[LONG_MODULE][0]), TMath::Tan(TMath::PiOver2()+BevelAngleRad[LONG_MODULE])*x_delta_top_BP[LONG_MODULE][1] + (YBevelSize[LONG_MODULE]+y_delta[LONG_MODULE][1]) } /*LONG_MODULE*/
};
//Double_t y_delta_top_BP[2] = { TMath::Tan(TMath::PiOver2()+BevelAngleRad)*x_delta_top_BP[0] + (YBevelSize+y_delta[0]), TMath::Tan(TMath::PiOver2()+BevelAngleRad)*x_delta_top_BP[1] + (YBevelSize+y_delta[1]) };


Double_t y_delta_bottom_BP[NModuleSizes][2] = {
    { YBottomBorder[STANDARD_MODULE][0], YBottomBorder[STANDARD_MODULE][1] }, /*STANDARD_MODULE*/
    { YBottomBorder[LONG_MODULE][0], YBottomBorder[LONG_MODULE][1] } /*LONG_MODULE*/
};
//Double_t y_delta_bottom_BP[2] = { YBottomBorder[0], YBottomBorder[1] };

Double_t x_delta_bottom_BP[NModuleSizes][2] = {
    { (y_delta_bottom_BP[STANDARD_MODULE][0] - (YBevelSize[STANDARD_MODULE] + y_delta[STANDARD_MODULE][0]))/(TMath::Tan(TMath::PiOver2()+BevelAngleRad[STANDARD_MODULE])), (y_delta_bottom_BP[STANDARD_MODULE][1] - (YBevelSize[STANDARD_MODULE] + y_delta[STANDARD_MODULE][1]))/(TMath::Tan(TMath::PiOver2()+BevelAngleRad[STANDARD_MODULE])) }, /*STANDARD_MODULE*/
    { (y_delta_bottom_BP[LONG_MODULE][0] - (YBevelSize[LONG_MODULE] + y_delta[LONG_MODULE][0]))/(TMath::Tan(TMath::PiOver2()+BevelAngleRad[LONG_MODULE])), (y_delta_bottom_BP[LONG_MODULE][1] - (YBevelSize[LONG_MODULE] + y_delta[LONG_MODULE][1]))/(TMath::Tan(TMath::PiOver2()+BevelAngleRad[LONG_MODULE])) } /*LONG_MODULE*/
};
//Double_t x_delta_bottom_BP[2] = { (y_delta_bottom_BP[0] - (YBevelSize + y_delta[0]))/(TMath::Tan(TMath::PiOver2()+BevelAngleRad)), (y_delta_bottom_BP[1] - (YBevelSize + y_delta[1]))/(TMath::Tan(TMath::PiOver2()+BevelAngleRad)) };

Double_t XDeadZone[NModuleSizes][2][3] = {
    {
        { XOrigLowLayer[STANDARD_MODULE][0], XOrigLowLayer[STANDARD_MODULE][0], XModuleSize[STANDARD_MODULE] - x_delta_bottom_BP[STANDARD_MODULE][0] },
        { XOrigLowLayer[STANDARD_MODULE][1], XOrigLowLayer[STANDARD_MODULE][1], XModuleSize[STANDARD_MODULE] - x_delta_bottom_BP[STANDARD_MODULE][1] }
    }, /*STANDARD_MODULE*/
    {
        { XOrigLowLayer[LONG_MODULE][0], XOrigLowLayer[LONG_MODULE][0], XModuleSize[LONG_MODULE] - x_delta_bottom_BP[LONG_MODULE][0] },
        { XOrigLowLayer[LONG_MODULE][1], XOrigLowLayer[LONG_MODULE][1], XModuleSize[LONG_MODULE] - x_delta_bottom_BP[LONG_MODULE][1] }
    }  /*LONG_MODULE*/
};
//Double_t XDeadZone[2][3] = {
//    { XOrigLowLayer[0], XOrigLowLayer[0], XModuleSize - x_delta_bottom_BP[0] },
//    { XOrigLowLayer[1], XOrigLowLayer[1], XModuleSize - x_delta_bottom_BP[1] }
//};

Double_t YDeadZone[NModuleSizes][2][3] = {
    {
        { YOrigLowLayer[STANDARD_MODULE][0], y_delta_top_BP[STANDARD_MODULE][0], YOrigLowLayer[STANDARD_MODULE][0] },
        { YOrigLowLayer[STANDARD_MODULE][1], y_delta_top_BP[STANDARD_MODULE][1], YOrigLowLayer[STANDARD_MODULE][1] }
    }, /*STANDARD_MODULE*/
    {
        { YOrigLowLayer[LONG_MODULE][0], y_delta_top_BP[LONG_MODULE][0], YOrigLowLayer[LONG_MODULE][0] },
        { YOrigLowLayer[LONG_MODULE][1], y_delta_top_BP[LONG_MODULE][1], YOrigLowLayer[LONG_MODULE][1] }
    }  /*LONG_MODULE*/
};
//Double_t YDeadZone[2][3] = {
//    { YOrigLowLayer[0], y_delta_top_BP[0], YOrigLowLayer[0] },
//    { YOrigLowLayer[1], y_delta_top_BP[1], YOrigLowLayer[1] }
//};
//------------------------------------------------------------------------------

//Configuration of SI-stations -------------------------------------------------

//Set Parameters of SILICON detector -------------------------------------------
const Int_t NStations = 5;      //stations in the detector
const Int_t NMaxModules = 22;    //max. number of modules in a station

const Int_t NModulesInStations[NStations] = {6, 10, 14, 18, 22};

//(X-Y-Z)Positions of stations (sensitive volumes)
const Double_t XStationPositions[NStations] = { 0.0, 0.0, 0.0, 0.0 };
const Double_t YStationPositions[NStations] = { 0.0, 0.0, 0.0, 0.0 };
const Double_t ZStationPositions[NStations] = { 15.7-8.64/*7.06*/, +15.7, +15.7+8.64, +15.7+8.64+8.64, +15.7+8.64+8.64+8.64 };

//(X-Y-Z)Shifts of modules in each station
const Double_t XModuleShifts[NStations][NMaxModules] = {
    {+9.15/*0*/, +3.15/*1*/, -9.15/*2*/, +9.15/*3*/, +3.15/*4*/, -9.15/*5*/},
    {+15.15/*0*/, +9.15/*1*/, +3.15/*2*/, -9.15/*3*/, -15.15/*4*/, +15.15/*5*/, +9.15/*6*/, +3.15/*7*/, -9.15/*8*/, -15.15/*9*/},
    {+21.15/*0*/, +15.15/*1*/, +9.15/*2*/, +3.15/*3*/, -9.15/*4*/, -15.15/*5*/, -21.15/*6*/, +21.15/*7*/, +15.15/*8*/, +9.15/*9*/, +3.15/*10*/, -9.15/*11*/, -15.15/*12*/, -21.15/*13*/},
    {+27.15/*0*/, +21.15/*1*/, +15.15/*2*/, +9.15/*3*/, +3.15/*4*/, -9.15/*5*/, -15.15/*6*/, -21.15/*7*/, -27.15/*8*/, +27.15/*9*/, +21.15/*10*/, +15.15/*11*/, +9.15/*12*/, +3.15/*13*/, -9.15/*14*/, -15.15/*15*/, -21.15/*16*/, -27.15/*17*/},
    {+33.15/*0*/, +27.15/*1*/, +21.15/*2*/, +15.15/*3*/, +9.15/*4*/, +3.15/*5*/, -9.15/*6*/, -15.15/*7*/, -21.15/*8*/, -27.15/*9*/, -33.15/*10*/, +33.15/*11*/, +27.15/*12*/, +21.15/*13*/, +15.15/*14*/, +9.15/*15*/, +3.15/*16*/, -9.15/*17*/, -15.15/*18*/, -21.15/*19*/, -27.15/*20*/, -33.15/*21*/}
};
const Double_t YModuleShifts[NStations][NMaxModules] = {
    {-0.15/*0*/, +2.85/*1*/, -0.15/*2*/, +0.15/*3*/, -2.85/*4*/, +0.15/*5*/},
    {-0.15/*0*/, -0.15/*1*/, +2.85/*2*/, -0.15/*3*/, -0.15/*4*/, +0.15/*5*/, +0.15/*6*/, -2.85/*7*/, +0.15/*8*/, +0.15/*9*/},
    {-0.15/*0*/, -0.15/*1*/, -0.15/*2*/, +2.85/*3*/, -0.15/*4*/, -0.15/*5*/, -0.15/*6*/, +0.15/*7*/, +0.15/*8*/, +0.15/*9*/, -2.85/*10*/, +0.15/*11*/, +0.15/*12*/, +0.15/*13*/},
    {-0.15/*0*/, -0.15/*1*/, -0.15/*2*/, -0.15/*3*/, +2.85/*4*/, -0.15/*5*/, -0.15/*6*/, -0.15/*7*/, -0.15/*8*/, +0.15/*9*/, +0.15/*10*/, +0.15/*11*/, +0.15/*12*/, -2.85/*13*/, +0.15/*14*/, +0.15/*15*/, +0.15/*16*/, +0.15/*17*/},
    {-0.15/*0*/, -0.15/*1*/, -0.15/*2*/, -0.15/*3*/, -0.15/*4*/, +2.85/*5*/, -0.15/*6*/, -0.15/*7*/, -0.15/*8*/, -0.15/*9*/, -0.15/*10*/, +0.15/*11*/, +0.15/*12*/, +0.15/*13*/, +0.15/*14*/, +0.15/*15*/, -2.85/*16*/, +0.15/*17*/, +0.15/*18*/, +0.15/*19*/, +0.15/*20*/, +0.15/*21*/}
};
const Double_t ZModuleShifts[NStations][NMaxModules] = {
    {+0.14+4.31/*0*/, +1.36+4.31/*1*/, +0.0+4.31/*2*/, +1.22/*3*/, 0.0/*4*/, +1.36/*5*/},
    {+1.36+4.31/*0*/, +0.14+4.31/*1*/, +1.36+4.31/*2*/, +0.0+4.31/*3*/, +1.22+4.31/*4*/, +0.0/*5*/, +1.22/*6*/, 0.0/*7*/, +1.36/*8*/, +0.14/*9*/},
    {+0.14+4.31/*0*/, +1.36+4.31/*1*/, +0.14+4.31/*2*/, +1.36+4.31/*3*/, +0.0+4.31/*4*/, +1.22+4.31/*5*/, +0.0+4.31/*6*/, +1.22/*7*/, +0.0/*8*/, +1.22/*9*/, +0.0/*10*/, +1.36/*11*/, +0.14/*12*/, +1.36/*13*/},
    {+1.36+4.31/*0*/, +0.14+4.31/*1*/, +1.36+4.31/*2*/, +0.14+4.31/*3*/, +1.36+4.31/*4*/, +0.0+4.31/*5*/, +1.22+4.31/*6*/, +0.0+4.31/*7*/, +1.22+4.31/*8*/, +0.0/*9*/, +1.22/*10*/, +0.0/*11*/, +1.22/*12*/, +0.0/*13*/, +1.36/*14*/, +0.14/*15*/, +1.36/*16*/, +0.14/*17*/},
    {+0.14+4.31/*0*/, +1.36+4.31/*1*/, +0.14+4.31/*2*/, +1.36+4.31/*3*/, +0.14+4.31/*4*/, +1.36+4.31/*5*/, +0.0+4.31/*6*/, +1.22+4.31/*7*/, +0.0+4.31/*8*/, +1.22+4.31/*9*/, +0.0+4.31/*10*/, +1.22/*11*/, +0.0/*12*/, +1.22/*13*/, +0.0/*14*/, +1.22/*15*/, +0.0/*16*/, +1.36/*17*/, +0.14/*18*/, +1.36/*19*/, +0.14/*20*/, +1.36/*21*/}
};

//Geometry parameters of modules
enum ModuleType {RIGHT_ANGLED_MODULE, BEVEL_MODULE, RIGHT_ANGLED_MODULE_HALF, BEVEL_MODULE_HALF};

const ModuleType ModulesTypes[NStations][NMaxModules] = {
    {RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE},
    {RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE},
    {RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE},
    {RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE},
    {RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE, RIGHT_ANGLED_MODULE}
};

//rotations of modules around y-axis by 180 deg.
const Bool_t ModulesYRotations[NStations][NMaxModules] = {
    {true/*0*/, true/*1*/, false/*2*/, true/*3*/, true/*4*/, false/*5*/},
    {true/*0*/, true/*1*/, true/*2*/, false/*3*/, false/*4*/, true/*5*/, true/*6*/, true/*7*/, false/*8*/, false/*9*/},
    {true/*0*/, true/*1*/, true/*2*/, true/*3*/, false/*4*/, false/*5*/, false/*6*/, true/*7*/, true/*8*/, true/*9*/, true/*10*/, false/*11*/, false/*12*/, false/*13*/},
    {true/*0*/, true/*1*/, true/*2*/, true/*3*/, true/*4*/, false/*5*/, false/*6*/, false/*7*/, false/*8*/, true/*9*/, true/*10*/, true/*11*/, true/*12*/, true/*13*/, false/*14*/, false/*15*/, false/*16*/, false/*17*/},
    {true/*0*/, true/*1*/, true/*2*/, true/*3*/, true/*4*/, true/*5*/, false/*6*/, false/*7*/, false/*8*/, false/*9*/, false/*10*/, true/*11*/, true/*12*/, true/*13*/, true/*14*/, true/*15*/, true/*16*/, false/*17*/, false/*18*/, false/*19*/, false/*20*/, false/*21*/}

};

//rotations of modules around x-axis by 180 deg.
const Bool_t ModulesXRotations[NStations][NMaxModules] = {
    {false/*0*/, false/*1*/, false/*2*/, true/*3*/, true/*4*/, true/*5*/},
    {false/*0*/, false/*1*/, false/*2*/, false/*3*/, false/*4*/, true/*5*/, true/*6*/, true/*7*/, true/*8*/, true/*9*/},
    {false/*0*/, false/*1*/, false/*2*/, false/*3*/, false/*4*/, false/*5*/, false/*6*/, true/*7*/, true/*8*/, true/*9*/, true/*10*/, true/*11*/, true/*12*/, true/*13*/},
    {false/*0*/, false/*1*/, false/*2*/, false/*3*/, false/*4*/, false/*5*/, false/*6*/, false/*7*/, false/*8*/, true/*9*/, true/*10*/, true/*11*/, true/*12*/, true/*13*/, true/*14*/, true/*15*/, true/*16*/, true/*17*/},
    {false/*0*/, false/*1*/, false/*2*/, false/*3*/, false/*4*/, false/*5*/, false/*6*/, false/*7*/, false/*8*/, false/*9*/, false/*10*/, true/*11*/, true/*12*/, true/*13*/, true/*14*/, true/*15*/, true/*16*/, true/*17*/, true/*18*/, true/*19*/, true/*20*/, true/*21*/}
};
//------------------------------------------------------------------------------

//Function declarations
void PrintInfo();
void CalculateParameters(TString xml_file_name);
//------------------------------------------------------------------------------


void SiliconConfigCalculate_Run8_5stations() {

    PrintInfo();
    CalculateParameters("SiliconRun8_5stations.xml");

    cout << "It's the end" << "\n";
    //gApplication->Terminate();
}

//------------------------------------------------------------------------------

void PrintInfo() {

    cout << "----------------------------------------------------------------\n";
    cout << "Parameters of the standard module 63x126 mm:\n";
    cout << "\n";
    cout << " Upper layer (inclined strips):\n";
    cout << "    orig(x:y) = ( " << XOrigUpLayer[STANDARD_MODULE][0] << " : " << YOrigUpLayer[STANDARD_MODULE][0] << " )\n";
    cout << "    size(dx:dy) = ( " << XSizeUpLayer[STANDARD_MODULE][0] << " : " << YSizeUpLayer[STANDARD_MODULE][0] << " )\n";
    cout << "    lborder(x:y) = ( " << XLeftBorderUpperlayer[STANDARD_MODULE][0] << " : " << YLeftBorderUpperlayer[STANDARD_MODULE][0] << " )\n";
    cout << "    rborder(x:y) = ( " << XRightBorderUpperlayer[STANDARD_MODULE][0] << " : " << YRightBorderUpperlayer[STANDARD_MODULE][0] << " )\n";
    cout << "    strip_direction = ";
        if(StripDirection[STANDARD_MODULE][0] == LeftToRight) cout << "LeftToRight\n";
        else cout << "RightToLeft\n";
    cout << "    pitch = " << PitchValue[STANDARD_MODULE][0] << "\n";
    cout << "    angle = " << StripAngle[STANDARD_MODULE][0] << " deg\n";
    cout << "    first_strip_number = " << FirstStripNumberUpLayer[STANDARD_MODULE][0] << "\n";
    cout << "\n";

    cout << " Lower layer (inclined strips):\n";
    cout << "    orig(x:y) = ( " << XOrigLowLayer[STANDARD_MODULE][0] << " : " << YOrigLowLayer[STANDARD_MODULE][0] << " )\n";
    cout << "    size(dx:dy) = ( " << XSizeLowLayer[STANDARD_MODULE][0] << " : " << YSizeLowLayer[STANDARD_MODULE][0] << " )\n";
    cout << "    lborder(x:y) = ( " << XLeftBorderLowerlayer[STANDARD_MODULE][0] << " : " << YLeftBorderLowerlayer[STANDARD_MODULE][0] << " )\n";
    cout << "    rborder(x:y) = ( " << XRightBorderLowerlayer[STANDARD_MODULE][0] << " : " << YRightBorderLowerlayer[STANDARD_MODULE][0] << " )\n";
    cout << "    strip_direction = ";
        if(StripDirection[STANDARD_MODULE][0] == LeftToRight) cout << "LeftToRight\n";
        else cout << "RightToLeft\n";
    cout << "    pitch = " << PitchValue[STANDARD_MODULE][0] << "\n";
    cout << "    angle = " << StripAngle[STANDARD_MODULE][0] << " deg\n";
    cout << "    first_strip_number = " << FirstStripNumberLowLayer[0] << "\n";
    cout << "    dead_zone(3 points):\n";
    cout << "       p0(x:y) = ( " << XDeadZone[STANDARD_MODULE][0][0] << " : " << YDeadZone[STANDARD_MODULE][0][0] << " )\n";
    cout << "       p1(x:y) = ( " << XDeadZone[STANDARD_MODULE][0][1] << " : " << YDeadZone[STANDARD_MODULE][0][1] << " )\n";
    cout << "       p2(x:y) = ( " << XDeadZone[STANDARD_MODULE][0][2] << " : " << YDeadZone[STANDARD_MODULE][0][2] << " )\n";
    cout << "\n";

    cout << " Upper layer (straight strips):\n";
    cout << "    orig(x:y) = ( " << XOrigUpLayer[STANDARD_MODULE][1] << " : " << YOrigUpLayer[STANDARD_MODULE][1] << " )\n";
    cout << "    size(dx:dy) = ( " << XSizeUpLayer[STANDARD_MODULE][1] << " : " << YSizeUpLayer[STANDARD_MODULE][1] << " )\n";
    cout << "    lborder(x:y) = ( " << XLeftBorderUpperlayer[STANDARD_MODULE][1] << " : " << YLeftBorderUpperlayer[STANDARD_MODULE][1] << " )\n";
    cout << "    rborder(x:y) = ( " << XRightBorderUpperlayer[STANDARD_MODULE][1] << " : " << YRightBorderUpperlayer[STANDARD_MODULE][1] << " )\n";
    cout << "    strip_direction = ";
        if(StripDirection[STANDARD_MODULE][1] == LeftToRight) cout << "LeftToRight\n";
        else cout << "RightToLeft\n";
    cout << "    pitch = " << PitchValue[STANDARD_MODULE][1] << "\n";
    cout << "    angle = " << StripAngle[STANDARD_MODULE][1] << " deg\n";
    cout << "    first_strip_number = " << FirstStripNumberUpLayer[STANDARD_MODULE][1] << "\n";
    cout << "\n";

    cout << " Lower layer (straight strips):\n";
    cout << "    orig(x:y) = ( " << XOrigLowLayer[STANDARD_MODULE][1] << " : " << YOrigLowLayer[STANDARD_MODULE][1] << " )\n";
    cout << "    size(dx:dy) = ( " << XSizeLowLayer[STANDARD_MODULE][1] << " : " << YSizeLowLayer[STANDARD_MODULE][1] << " )\n";
    cout << "    lborder(x:y) = ( " << XLeftBorderLowerlayer[STANDARD_MODULE][1] << " : " << YLeftBorderLowerlayer[STANDARD_MODULE][1] << " )\n";
    cout << "    rborder(x:y) = ( " << XRightBorderLowerlayer[STANDARD_MODULE][1] << " : " << YRightBorderLowerlayer[STANDARD_MODULE][1] << " )\n";
    cout << "    strip_direction = ";
        if(StripDirection[STANDARD_MODULE][1] == LeftToRight) cout << "LeftToRight\n";
        else cout << "RightToLeft\n";
    cout << "    pitch = " << PitchValue[STANDARD_MODULE][1] << "\n";
    cout << "    angle = " << StripAngle[STANDARD_MODULE][1] << " deg\n";
    cout << "    first_strip_number = " << FirstStripNumberLowLayer[1] << "\n";
    cout << "    dead_zone(3 points):\n";
    cout << "       p0(x:y) = ( " << XDeadZone[STANDARD_MODULE][1][0] << " : " << YDeadZone[STANDARD_MODULE][1][0] << " )\n";
    cout << "       p1(x:y) = ( " << XDeadZone[STANDARD_MODULE][1][1] << " : " << YDeadZone[STANDARD_MODULE][1][1] << " )\n";
    cout << "       p2(x:y) = ( " << XDeadZone[STANDARD_MODULE][1][2] << " : " << YDeadZone[STANDARD_MODULE][1][2] << " )\n";
    cout << "\n";

    cout << "----------------------------------------------------------------\n";

    cout << "----------------------------------------------------------------\n";
    cout << "Parameters of the long module 63x186 mm:\n";
    cout << "\n";
    cout << " Upper layer (inclined strips):\n";
    cout << "    orig(x:y) = ( " << XOrigUpLayer[LONG_MODULE][0] << " : " << YOrigUpLayer[LONG_MODULE][0] << " )\n";
    cout << "    size(dx:dy) = ( " << XSizeUpLayer[LONG_MODULE][0] << " : " << YSizeUpLayer[LONG_MODULE][0] << " )\n";
    cout << "    lborder(x:y) = ( " << XLeftBorderUpperlayer[LONG_MODULE][0] << " : " << YLeftBorderUpperlayer[LONG_MODULE][0] << " )\n";
    cout << "    rborder(x:y) = ( " << XRightBorderUpperlayer[LONG_MODULE][0] << " : " << YRightBorderUpperlayer[LONG_MODULE][0] << " )\n";
    cout << "    strip_direction = ";
        if(StripDirection[LONG_MODULE][0] == LeftToRight) cout << "LeftToRight\n";
        else cout << "RightToLeft\n";
    cout << "    pitch = " << PitchValue[LONG_MODULE][0] << "\n";
    cout << "    angle = " << StripAngle[LONG_MODULE][0] << " deg\n";
    cout << "    first_strip_number = " << FirstStripNumberUpLayer[LONG_MODULE][0] << "\n";
    cout << "\n";

    cout << " Lower layer (inclined strips):\n";
    cout << "    orig(x:y) = ( " << XOrigLowLayer[LONG_MODULE][0] << " : " << YOrigLowLayer[LONG_MODULE][0] << " )\n";
    cout << "    size(dx:dy) = ( " << XSizeLowLayer[LONG_MODULE][0] << " : " << YSizeLowLayer[LONG_MODULE][0] << " )\n";
    cout << "    lborder(x:y) = ( " << XLeftBorderLowerlayer[LONG_MODULE][0] << " : " << YLeftBorderLowerlayer[LONG_MODULE][0] << " )\n";
    cout << "    rborder(x:y) = ( " << XRightBorderLowerlayer[LONG_MODULE][0] << " : " << YRightBorderLowerlayer[LONG_MODULE][0] << " )\n";
    cout << "    strip_direction = ";
        if(StripDirection[LONG_MODULE][0] == LeftToRight) cout << "LeftToRight\n";
        else cout << "RightToLeft\n";
    cout << "    pitch = " << PitchValue[LONG_MODULE][0] << "\n";
    cout << "    angle = " << StripAngle[LONG_MODULE][0] << " deg\n";
    cout << "    first_strip_number = " << FirstStripNumberLowLayer[0] << "\n";
    cout << "    dead_zone(3 points):\n";
    cout << "       p0(x:y) = ( " << XDeadZone[LONG_MODULE][0][0] << " : " << YDeadZone[LONG_MODULE][0][0] << " )\n";
    cout << "       p1(x:y) = ( " << XDeadZone[LONG_MODULE][0][1] << " : " << YDeadZone[LONG_MODULE][0][1] << " )\n";
    cout << "       p2(x:y) = ( " << XDeadZone[LONG_MODULE][0][2] << " : " << YDeadZone[LONG_MODULE][0][2] << " )\n";
    cout << "\n";

    cout << " Upper layer (straight strips):\n";
    cout << "    orig(x:y) = ( " << XOrigUpLayer[LONG_MODULE][1] << " : " << YOrigUpLayer[LONG_MODULE][1] << " )\n";
    cout << "    size(dx:dy) = ( " << XSizeUpLayer[LONG_MODULE][1] << " : " << YSizeUpLayer[LONG_MODULE][1] << " )\n";
    cout << "    lborder(x:y) = ( " << XLeftBorderUpperlayer[LONG_MODULE][1] << " : " << YLeftBorderUpperlayer[LONG_MODULE][1] << " )\n";
    cout << "    rborder(x:y) = ( " << XRightBorderUpperlayer[LONG_MODULE][1] << " : " << YRightBorderUpperlayer[LONG_MODULE][1] << " )\n";
    cout << "    strip_direction = ";
        if(StripDirection[LONG_MODULE][1] == LeftToRight) cout << "LeftToRight\n";
        else cout << "RightToLeft\n";
    cout << "    pitch = " << PitchValue[LONG_MODULE][1] << "\n";
    cout << "    angle = " << StripAngle[LONG_MODULE][1] << " deg\n";
    cout << "    first_strip_number = " << FirstStripNumberUpLayer[LONG_MODULE][1] << "\n";
    cout << "\n";

    cout << " Lower layer (straight strips):\n";
    cout << "    orig(x:y) = ( " << XOrigLowLayer[LONG_MODULE][1] << " : " << YOrigLowLayer[LONG_MODULE][1] << " )\n";
    cout << "    size(dx:dy) = ( " << XSizeLowLayer[LONG_MODULE][1] << " : " << YSizeLowLayer[LONG_MODULE][1] << " )\n";
    cout << "    lborder(x:y) = ( " << XLeftBorderLowerlayer[LONG_MODULE][1] << " : " << YLeftBorderLowerlayer[LONG_MODULE][1] << " )\n";
    cout << "    rborder(x:y) = ( " << XRightBorderLowerlayer[LONG_MODULE][1] << " : " << YRightBorderLowerlayer[LONG_MODULE][1] << " )\n";
    cout << "    strip_direction = ";
        if(StripDirection[LONG_MODULE][1] == LeftToRight) cout << "LeftToRight\n";
        else cout << "RightToLeft\n";
    cout << "    pitch = " << PitchValue[LONG_MODULE][1] << "\n";
    cout << "    angle = " << StripAngle[LONG_MODULE][1] << " deg\n";
    cout << "    first_strip_number = " << FirstStripNumberLowLayer[1] << "\n";
    cout << "    dead_zone(3 points):\n";
    cout << "       p0(x:y) = ( " << XDeadZone[LONG_MODULE][1][0] << " : " << YDeadZone[LONG_MODULE][1][0] << " )\n";
    cout << "       p1(x:y) = ( " << XDeadZone[LONG_MODULE][1][1] << " : " << YDeadZone[LONG_MODULE][1][1] << " )\n";
    cout << "       p2(x:y) = ( " << XDeadZone[LONG_MODULE][1][2] << " : " << YDeadZone[LONG_MODULE][1][2] << " )\n";
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

        //module size in a station
        ModuleSize module_size = STANDARD_MODULE;

        //(!!!) the last (fourth) station has long modules (!!!)
        if(istation == 4) module_size = LONG_MODULE;

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
            Double_t pitchValue[2] = {PitchValue[module_size][0], PitchValue[module_size][1]};
            Double_t stripAngle[2];
            Int_t firstStripNumber_UpLayer[2];
            Int_t firstStripNumber_LowLayer[2];

            if( ModulesYRotations[istation][imodule] == false && ModulesXRotations[istation][imodule] == false ) {

                xOrig_UpLayer[0] = XOrigUpLayer[module_size][0] + XModuleShifts[istation][imodule];
                yOrig_UpLayer[0] = YOrigUpLayer[module_size][0] + YModuleShifts[istation][imodule];
                xOrig_LowLayer[0] = XOrigLowLayer[module_size][0] + XModuleShifts[istation][imodule];
                yOrig_LowLayer[0] = YOrigLowLayer[module_size][0] + YModuleShifts[istation][imodule];

                xOrig_UpLayer[1] = XOrigUpLayer[module_size][1] + XModuleShifts[istation][imodule];
                yOrig_UpLayer[1] = YOrigUpLayer[module_size][1] + YModuleShifts[istation][imodule];
                xOrig_LowLayer[1] = XOrigLowLayer[module_size][1] + XModuleShifts[istation][imodule];
                yOrig_LowLayer[1] = YOrigLowLayer[module_size][1] + YModuleShifts[istation][imodule];

                xSize_UpLayer[0] = XSizeUpLayer[module_size][0];
                ySize_UpLayer[0] = YSizeUpLayer[module_size][0];
                xSize_LowLayer[0] = XSizeLowLayer[module_size][0];
                ySize_LowLayer[0] = YSizeLowLayer[module_size][0];

                xSize_UpLayer[1] = XSizeUpLayer[module_size][1];
                ySize_UpLayer[1] = YSizeUpLayer[module_size][1];
                xSize_LowLayer[1] = XSizeLowLayer[module_size][1];
                ySize_LowLayer[1] = YSizeLowLayer[module_size][1];

                xLBorder_UpLayer[0] = XLeftBorderUpperlayer[module_size][0] + XModuleShifts[istation][imodule];
                xLBorder_UpLayer[1] = XLeftBorderUpperlayer[module_size][1] + XModuleShifts[istation][imodule];
                yLBorder_UpLayer[0] = YLeftBorderUpperlayer[module_size][0] + YModuleShifts[istation][imodule];
                yLBorder_UpLayer[1] = YLeftBorderUpperlayer[module_size][1] + YModuleShifts[istation][imodule];
                xLBorder_LowLayer[0] = XLeftBorderLowerlayer[module_size][0] + XModuleShifts[istation][imodule];
                xLBorder_LowLayer[1] = XLeftBorderLowerlayer[module_size][1] + XModuleShifts[istation][imodule];
                yLBorder_LowLayer[0] = YLeftBorderLowerlayer[module_size][0] + YModuleShifts[istation][imodule];
                yLBorder_LowLayer[1] = YLeftBorderLowerlayer[module_size][1] + YModuleShifts[istation][imodule];

                xRBorder_UpLayer[0] = XRightBorderUpperlayer[module_size][0] + XModuleShifts[istation][imodule];
                xRBorder_UpLayer[1] = XRightBorderUpperlayer[module_size][1] + XModuleShifts[istation][imodule];
                yRBorder_UpLayer[0] = YRightBorderUpperlayer[module_size][0] + YModuleShifts[istation][imodule];
                yRBorder_UpLayer[1] = YRightBorderUpperlayer[module_size][1] + YModuleShifts[istation][imodule];
                xRBorder_LowLayer[0] = XRightBorderLowerlayer[module_size][0] + XModuleShifts[istation][imodule];
                xRBorder_LowLayer[1] = XRightBorderLowerlayer[module_size][1] + XModuleShifts[istation][imodule];
                yRBorder_LowLayer[0] = YRightBorderLowerlayer[module_size][0] + YModuleShifts[istation][imodule];
                yRBorder_LowLayer[1] = YRightBorderLowerlayer[module_size][1] + YModuleShifts[istation][imodule];

                for(Int_t ilayer = 0; ilayer < 2; ++ilayer) {
                    for(Int_t ipoint = 0; ipoint < 3; ++ipoint) {
                        xDeadZone[ilayer][ipoint] = XDeadZone[module_size][ilayer][ipoint] + XModuleShifts[istation][imodule];
                        yDeadZone[ilayer][ipoint] = YDeadZone[module_size][ilayer][ipoint] + YModuleShifts[istation][imodule];
                    }
                }

                if(ModulesTypes[istation][imodule] == ModuleType::BEVEL_MODULE || ModulesTypes[istation][imodule] == ModuleType::BEVEL_MODULE_HALF) {
                    isDeadZone_UpLayer = false;
                    isDeadZone_LowLayer = true;
                }

                if(StripDirection[module_size][0] == LeftToRight) stripDirection[0] = LeftToRight; else stripDirection[0] = RightToLeft;
                if(StripDirection[module_size][1] == LeftToRight) stripDirection[1] = LeftToRight; else stripDirection[1] = RightToLeft;

                stripAngle[0] = StripAngle[module_size][0];
                stripAngle[1] = StripAngle[module_size][1];

                firstStripNumber_UpLayer[0] = FirstStripNumberUpLayer[module_size][0];
                firstStripNumber_UpLayer[1] = FirstStripNumberUpLayer[module_size][1];
                firstStripNumber_LowLayer[0] = FirstStripNumberLowLayer[module_size][0];
                firstStripNumber_LowLayer[1] = FirstStripNumberLowLayer[module_size][1];
            }

            if( ModulesYRotations[istation][imodule] == true && ModulesXRotations[istation][imodule] == false ) {

                xOrig_UpLayer[0] = -XOrigUpLayer[module_size][0] + XSizeUpLayer[module_size][0] + XModuleShifts[istation][imodule];
                yOrig_UpLayer[0] = YOrigUpLayer[module_size][0] + YModuleShifts[istation][imodule];
                xOrig_LowLayer[0] = -XOrigLowLayer[module_size][0] + XSizeLowLayer[module_size][0] + XModuleShifts[istation][imodule];
                yOrig_LowLayer[0] = YOrigLowLayer[module_size][0] + YModuleShifts[istation][imodule];

                xOrig_UpLayer[1] = -XOrigUpLayer[module_size][1] + XSizeUpLayer[module_size][1] + XModuleShifts[istation][imodule];
                yOrig_UpLayer[1] = YOrigUpLayer[module_size][1] + YModuleShifts[istation][imodule];
                xOrig_LowLayer[1] = -XOrigLowLayer[module_size][1] + XSizeLowLayer[module_size][1] + XModuleShifts[istation][imodule];
                yOrig_LowLayer[1] = YOrigLowLayer[module_size][1] + YModuleShifts[istation][imodule];

                xSize_UpLayer[0] = XSizeUpLayer[module_size][0];
                ySize_UpLayer[0] = YSizeUpLayer[module_size][0];
                xSize_LowLayer[0] = XSizeLowLayer[module_size][0];
                ySize_LowLayer[0] = YSizeLowLayer[module_size][0];

                xSize_UpLayer[1] = XSizeUpLayer[module_size][1];
                ySize_UpLayer[1] = YSizeUpLayer[module_size][1];
                xSize_LowLayer[1] = XSizeLowLayer[module_size][1];
                ySize_LowLayer[1] = YSizeLowLayer[module_size][1];

                xLBorder_UpLayer[0] = -XRightBorderUpperlayer[module_size][0] + XModuleShifts[istation][imodule];
                xLBorder_UpLayer[1] = -XRightBorderUpperlayer[module_size][1] + XModuleShifts[istation][imodule];
                yLBorder_UpLayer[0] = YRightBorderUpperlayer[module_size][0] + YModuleShifts[istation][imodule];
                yLBorder_UpLayer[1] = YRightBorderUpperlayer[module_size][1] + YModuleShifts[istation][imodule];
                xLBorder_LowLayer[0] = -XRightBorderLowerlayer[module_size][0] + XModuleShifts[istation][imodule];
                xLBorder_LowLayer[1] = -XRightBorderLowerlayer[module_size][1] + XModuleShifts[istation][imodule];
                yLBorder_LowLayer[0] = YRightBorderLowerlayer[module_size][0] + YModuleShifts[istation][imodule];
                yLBorder_LowLayer[1] = YRightBorderLowerlayer[module_size][1] + YModuleShifts[istation][imodule];

                xRBorder_UpLayer[0] = -XLeftBorderUpperlayer[module_size][0] + XModuleShifts[istation][imodule];
                xRBorder_UpLayer[1] = -XLeftBorderUpperlayer[module_size][1] + XModuleShifts[istation][imodule];
                yRBorder_UpLayer[0] = YLeftBorderUpperlayer[module_size][0] + YModuleShifts[istation][imodule];
                yRBorder_UpLayer[1] = YLeftBorderUpperlayer[module_size][1] + YModuleShifts[istation][imodule];
                xRBorder_LowLayer[0] = -XLeftBorderLowerlayer[module_size][0] + XModuleShifts[istation][imodule];
                xRBorder_LowLayer[1] = -XLeftBorderLowerlayer[module_size][1] + XModuleShifts[istation][imodule];
                yRBorder_LowLayer[0] = YLeftBorderLowerlayer[module_size][0] + YModuleShifts[istation][imodule];
                yRBorder_LowLayer[1] = YLeftBorderLowerlayer[module_size][1] + YModuleShifts[istation][imodule];

                for(Int_t ilayer = 0; ilayer < 2; ++ilayer) {
                    for(Int_t ipoint = 0; ipoint < 3; ++ipoint) {
                        xDeadZone[ilayer][ipoint] = -XDeadZone[module_size][ilayer][ipoint] + XModuleShifts[istation][imodule];
                        yDeadZone[ilayer][ipoint] = YDeadZone[module_size][ilayer][ipoint] + YModuleShifts[istation][imodule];
                    }
                }

                if(ModulesTypes[istation][imodule] == ModuleType::BEVEL_MODULE || ModulesTypes[istation][imodule] == ModuleType::BEVEL_MODULE_HALF) {
                    isDeadZone_UpLayer = false;
                    isDeadZone_LowLayer = true;
                }

                if(StripDirection[module_size][0] == LeftToRight) stripDirection[0] = RightToLeft; else stripDirection[0] = LeftToRight;
                if(StripDirection[module_size][1] == LeftToRight) stripDirection[1] = RightToLeft; else stripDirection[1] = LeftToRight;

                stripAngle[0] = -StripAngle[module_size][0];
                stripAngle[1] = -StripAngle[module_size][1];

                firstStripNumber_UpLayer[0] = FirstStripNumberUpLayer[module_size][0];
                firstStripNumber_UpLayer[1] = FirstStripNumberUpLayer[module_size][1];
                firstStripNumber_LowLayer[0] = FirstStripNumberLowLayer[module_size][0];
                firstStripNumber_LowLayer[1] = FirstStripNumberLowLayer[module_size][1];
            }

            if( ModulesYRotations[istation][imodule] == false && ModulesXRotations[istation][imodule] == true ) {

                xOrig_UpLayer[0] = XOrigUpLayer[module_size][0] + XModuleShifts[istation][imodule];
                yOrig_UpLayer[0] = -YOrigUpLayer[module_size][0] + YMiddleBorderLowerPart[module_size][0] + YMiddleBorderUpperPart[module_size][0] + YGapBetweenPartsOfModule[module_size] + YModuleShifts[istation][imodule];
                xOrig_LowLayer[0] = XOrigLowLayer[module_size][0] + XModuleShifts[istation][imodule];
                yOrig_LowLayer[0] = -YOrigLowLayer[module_size][0] - YSizeLowLayer[module_size][0] - YMiddleBorderLowerPart[module_size][0] - YMiddleBorderUpperPart[module_size][0] - YGapBetweenPartsOfModule[module_size] - YSizeUpLayer[module_size][0] + YModuleShifts[istation][imodule];

                xOrig_UpLayer[1] = XOrigUpLayer[module_size][1] + XModuleShifts[istation][imodule];
                yOrig_UpLayer[1] = -YOrigUpLayer[module_size][1] + YMiddleBorderLowerPart[module_size][1] + YMiddleBorderUpperPart[module_size][1] + YGapBetweenPartsOfModule[module_size] + YModuleShifts[istation][imodule];
                xOrig_LowLayer[1] = XOrigLowLayer[module_size][1] + XModuleShifts[istation][imodule];
                yOrig_LowLayer[1] = -YOrigLowLayer[module_size][1] - YSizeLowLayer[module_size][1] - YMiddleBorderLowerPart[module_size][1] - YMiddleBorderUpperPart[module_size][1] - YGapBetweenPartsOfModule[module_size] - YSizeUpLayer[module_size][1] + YModuleShifts[istation][imodule];

                xSize_UpLayer[0] = XSizeLowLayer[module_size][0];
                ySize_UpLayer[0] = YSizeLowLayer[module_size][0];
                xSize_LowLayer[0] = XSizeUpLayer[module_size][0];
                ySize_LowLayer[0] = YSizeUpLayer[module_size][0];

                xSize_UpLayer[1] = XSizeLowLayer[module_size][1];
                ySize_UpLayer[1] = YSizeLowLayer[module_size][1];
                xSize_LowLayer[1] = XSizeUpLayer[module_size][1];
                ySize_LowLayer[1] = YSizeUpLayer[module_size][1];

                xLBorder_UpLayer[0] = XLeftBorderLowerlayer[module_size][0] + XModuleShifts[istation][imodule];
                xLBorder_UpLayer[1] = XLeftBorderLowerlayer[module_size][1] + XModuleShifts[istation][imodule];
                yLBorder_UpLayer[0] = -YLeftBorderLowerlayer[module_size][0] + YModuleShifts[istation][imodule];
                yLBorder_UpLayer[1] = -YLeftBorderLowerlayer[module_size][1] + YModuleShifts[istation][imodule];
                xLBorder_LowLayer[0] = XLeftBorderUpperlayer[module_size][0] + XModuleShifts[istation][imodule];
                xLBorder_LowLayer[1] = XLeftBorderUpperlayer[module_size][1] + XModuleShifts[istation][imodule];
                yLBorder_LowLayer[0] = -YLeftBorderUpperlayer[module_size][0] + YModuleShifts[istation][imodule];
                yLBorder_LowLayer[1] = -YLeftBorderUpperlayer[module_size][1] + YModuleShifts[istation][imodule];

                xRBorder_UpLayer[0] = XRightBorderLowerlayer[module_size][0] + XModuleShifts[istation][imodule];
                xRBorder_UpLayer[1] = XRightBorderLowerlayer[module_size][1] + XModuleShifts[istation][imodule];
                yRBorder_UpLayer[0] = -YRightBorderLowerlayer[module_size][0] + YModuleShifts[istation][imodule];
                yRBorder_UpLayer[1] = -YRightBorderLowerlayer[module_size][1] + YModuleShifts[istation][imodule];
                xRBorder_LowLayer[0] = XRightBorderUpperlayer[module_size][0] + XModuleShifts[istation][imodule];
                xRBorder_LowLayer[1] = XRightBorderUpperlayer[module_size][1] + XModuleShifts[istation][imodule];
                yRBorder_LowLayer[0] = -YRightBorderUpperlayer[module_size][0] + YModuleShifts[istation][imodule];
                yRBorder_LowLayer[1] = -YRightBorderUpperlayer[module_size][1] + YModuleShifts[istation][imodule];

                for(Int_t ilayer = 0; ilayer < 2; ++ilayer) {
                    for(Int_t ipoint = 0; ipoint < 3; ++ipoint) {
                        xDeadZone[ilayer][ipoint] = XDeadZone[module_size][ilayer][ipoint] + XModuleShifts[istation][imodule];
                        yDeadZone[ilayer][ipoint] = -YDeadZone[module_size][ilayer][ipoint] + YModuleShifts[istation][imodule];
                    }
                }

                if(ModulesTypes[istation][imodule] == ModuleType::BEVEL_MODULE || ModulesTypes[istation][imodule] == ModuleType::BEVEL_MODULE_HALF) {
                    isDeadZone_UpLayer = true;
                    isDeadZone_LowLayer = false;
                }

                if(StripDirection[module_size][0] == LeftToRight) stripDirection[0] = LeftToRight; else stripDirection[0] = RightToLeft;
                if(StripDirection[module_size][1] == LeftToRight) stripDirection[1] = LeftToRight; else stripDirection[1] = RightToLeft;

                stripAngle[0] = -StripAngle[module_size][0];
                stripAngle[1] = -StripAngle[module_size][1];

                firstStripNumber_UpLayer[0] = FirstStripNumberLowLayer[module_size][0];
                firstStripNumber_UpLayer[1] = FirstStripNumberLowLayer[module_size][1];
                firstStripNumber_LowLayer[0] = FirstStripNumberUpLayer[module_size][0];
                firstStripNumber_LowLayer[1] = FirstStripNumberUpLayer[module_size][1];
            }

            if( ModulesYRotations[istation][imodule] == true && ModulesXRotations[istation][imodule] == true ) {

                xOrig_UpLayer[0] = -XOrigUpLayer[module_size][0] + XSizeUpLayer[module_size][0] + XModuleShifts[istation][imodule];
                yOrig_UpLayer[0] = -YOrigUpLayer[module_size][0] + YMiddleBorderLowerPart[module_size][0] + YMiddleBorderUpperPart[module_size][0] + YGapBetweenPartsOfModule[module_size] + YModuleShifts[istation][imodule];
                xOrig_LowLayer[0] = -XOrigLowLayer[module_size][0] + XSizeLowLayer[module_size][0] + XModuleShifts[istation][imodule];
                yOrig_LowLayer[0] = -YOrigLowLayer[module_size][0] - YSizeLowLayer[module_size][0] - YMiddleBorderLowerPart[module_size][0] - YMiddleBorderUpperPart[module_size][0] - YGapBetweenPartsOfModule[module_size] - YSizeUpLayer[module_size][0] + YModuleShifts[istation][imodule];

                xOrig_UpLayer[1] = -XOrigUpLayer[module_size][1] + XSizeUpLayer[module_size][1] + XModuleShifts[istation][imodule];
                yOrig_UpLayer[1] = -YOrigUpLayer[module_size][1] + YMiddleBorderLowerPart[module_size][1] + YMiddleBorderUpperPart[module_size][1] + YGapBetweenPartsOfModule[module_size] + YModuleShifts[istation][imodule];
                xOrig_LowLayer[1] = -XOrigLowLayer[module_size][1] + XSizeLowLayer[module_size][1] + XModuleShifts[istation][imodule];
                yOrig_LowLayer[1] = -YOrigLowLayer[module_size][1] - YSizeLowLayer[module_size][1] - YMiddleBorderLowerPart[module_size][1] - YMiddleBorderUpperPart[module_size][1] - YGapBetweenPartsOfModule[module_size] - YSizeUpLayer[module_size][1] + YModuleShifts[istation][imodule];

                xSize_UpLayer[0] = XSizeLowLayer[module_size][0];
                ySize_UpLayer[0] = YSizeLowLayer[module_size][0];
                xSize_LowLayer[0] = XSizeUpLayer[module_size][0];
                ySize_LowLayer[0] = YSizeUpLayer[module_size][0];

                xSize_UpLayer[1] = XSizeLowLayer[module_size][1];
                ySize_UpLayer[1] = YSizeLowLayer[module_size][1];
                xSize_LowLayer[1] = XSizeUpLayer[module_size][1];
                ySize_LowLayer[1] = YSizeUpLayer[module_size][1];

                xLBorder_UpLayer[0] = -XRightBorderLowerlayer[module_size][0] + XModuleShifts[istation][imodule];
                xLBorder_UpLayer[1] = -XRightBorderLowerlayer[module_size][1] + XModuleShifts[istation][imodule];
                yLBorder_UpLayer[0] = -YRightBorderLowerlayer[module_size][0] + YModuleShifts[istation][imodule];
                yLBorder_UpLayer[1] = -YRightBorderLowerlayer[module_size][1] + YModuleShifts[istation][imodule];
                xLBorder_LowLayer[0] = -XRightBorderUpperlayer[module_size][0] + XModuleShifts[istation][imodule];
                xLBorder_LowLayer[1] = -XRightBorderUpperlayer[module_size][1] + XModuleShifts[istation][imodule];
                yLBorder_LowLayer[0] = -YRightBorderUpperlayer[module_size][0] + YModuleShifts[istation][imodule];
                yLBorder_LowLayer[1] = -YRightBorderUpperlayer[module_size][1] + YModuleShifts[istation][imodule];

                xRBorder_UpLayer[0] = -XLeftBorderLowerlayer[module_size][0] + XModuleShifts[istation][imodule];
                xRBorder_UpLayer[1] = -XLeftBorderLowerlayer[module_size][1] + XModuleShifts[istation][imodule];
                yRBorder_UpLayer[0] = -YLeftBorderLowerlayer[module_size][0] + YModuleShifts[istation][imodule];
                yRBorder_UpLayer[1] = -YLeftBorderLowerlayer[module_size][1] + YModuleShifts[istation][imodule];
                xRBorder_LowLayer[0] = -XLeftBorderUpperlayer[module_size][0] + XModuleShifts[istation][imodule];
                xRBorder_LowLayer[1] = -XLeftBorderUpperlayer[module_size][1] + XModuleShifts[istation][imodule];
                yRBorder_LowLayer[0] = -YLeftBorderUpperlayer[module_size][0] + YModuleShifts[istation][imodule];
                yRBorder_LowLayer[1] = -YLeftBorderUpperlayer[module_size][1] + YModuleShifts[istation][imodule];

                for(Int_t ilayer = 0; ilayer < 2; ++ilayer) {
                    for(Int_t ipoint = 0; ipoint < 3; ++ipoint) {
                        xDeadZone[ilayer][ipoint] = -XDeadZone[module_size][ilayer][ipoint] + XModuleShifts[istation][imodule];
                        yDeadZone[ilayer][ipoint] = -YDeadZone[module_size][ilayer][ipoint] + YModuleShifts[istation][imodule];
                    }
                }

                if(ModulesTypes[istation][imodule] == ModuleType::BEVEL_MODULE || ModulesTypes[istation][imodule] == ModuleType::BEVEL_MODULE_HALF) {
                    isDeadZone_UpLayer = true;
                    isDeadZone_LowLayer = false;
                }

                if(StripDirection[module_size][0] == LeftToRight) stripDirection[0] = RightToLeft; else stripDirection[0] = LeftToRight;
                if(StripDirection[module_size][1] == LeftToRight) stripDirection[1] = RightToLeft; else stripDirection[1] = LeftToRight;

                stripAngle[0] = StripAngle[module_size][0];
                stripAngle[1] = StripAngle[module_size][1];

                firstStripNumber_UpLayer[0] = FirstStripNumberLowLayer[module_size][0];
                firstStripNumber_UpLayer[1] = FirstStripNumberLowLayer[module_size][1];
                firstStripNumber_LowLayer[0] = FirstStripNumberUpLayer[module_size][0];
                firstStripNumber_LowLayer[1] = FirstStripNumberUpLayer[module_size][1];
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