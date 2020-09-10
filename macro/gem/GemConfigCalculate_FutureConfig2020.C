/*
 * Description:
 * Macro for calculating parameters of GEM detector and writing them
 * into the XML-file needed for simulation and reconstruction procedures.
 * The obtained XML-file should be moved into the 'parameters/gem/XMLConfigs' directory
 *
 * GEM configuration: FutureConfig2020 (7 stations)
 *
 * Created: 12.05.2020
 * Author: Baranov D.
 */

#include "Rtypes.h"

R__ADD_INCLUDE_PATH($VMCWORKDIR)
R__LOAD_LIBRARY(libBmnData.so)

#include "macro/run/bmnloadlibs.C"

#include "TString.h"

#include <fstream>

using namespace TMath;
using namespace std;

//Set Parameters of GEMs -------------------------------------------------------
const Int_t NStations = 7;      //stations in the detector
const Int_t NMaxModules = 4;    //max. number of modules in a station

const Int_t NModulesInStations[NStations] = {4, 4, 4, 4, 4, 4, 4};

//(X-Y-Z)Positions of stations (sensitive volumes)
const Double_t XStationPositions[NStations] = { +0.0/*0*/, +0.804/*1*/, +1.568/*2*/, +2.372/*3*/, +3.88/*4*/, +5.442/*5*/, +6.964/*6*/}; //rough x-shifts
const Double_t YStationPositions[NStations] = { 0.0/*0*/, 0.0/*1*/, 0.0/*2*/, 0.0/*3*/, 0.0/*4*/, 0.0/*5*/, 0.0/*6*/};
const Double_t ZStationPositions[NStations] = { +50.52/*0*/, +81.72/*1*/, +112.92/*2*/, +144.12/*3*/, +175.32/*4*/, +206.52/*5*/, 237.72/*6*/};

//(X-Y-Z)Shifts of modules in each station
const Double_t XModuleShifts[NStations][NMaxModules] = {
    {163.2*0.25, -163.2*0.25, 163.2*0.25, -163.2*0.25}, //st. 0
    {163.2*0.25, -163.2*0.25, 163.2*0.25, -163.2*0.25}, //st. 1
    {163.2*0.25, -163.2*0.25, 163.2*0.25, -163.2*0.25}, //st. 2
    {163.2*0.25, -163.2*0.25, 163.2*0.25, -163.2*0.25}, //st. 3
    {163.2*0.25, -163.2*0.25, 163.2*0.25, -163.2*0.25}, //st. 4
    {163.2*0.25, -163.2*0.25, 163.2*0.25, -163.2*0.25}, //st. 5
    {163.2*0.25, -163.2*0.25, 163.2*0.25, -163.2*0.25}  //st. 6
};
const Double_t YModuleShifts[NStations][NMaxModules] = {
    {+22.5, +22.5, -22.5, -22.5},  //st. 0
    {+22.5, +22.5, -22.5, -22.5},  //st. 1
    {+22.5, +22.5, -22.5, -22.5},  //st. 2
    {+22.5, +22.5, -22.5, -22.5},  //st. 3
    {+22.5, +22.5, -22.5, -22.5},  //st. 4
    {+22.5, +22.5, -22.5, -22.5},  //st. 5
    {+22.5, +22.5, -22.5, -22.5}   //st. 6
};
const Double_t ZModuleShifts[NStations][NMaxModules] = {
    {+4.1, +4.1, 0.0, 0.0}, //st. 0
    {0.0, 0.0, +4.1, +4.1}, //st. 1
    {+4.1, +4.1, 0.0, 0.0}, //st. 2
    {0.0, 0.0, +4.1, +4.1}, //st. 3
    {+4.1, +4.1, 0.0, 0.0}, //st. 4
    {0.0, 0.0, +4.1, +4.1}, //st. 5
    {+4.1, +4.1, 0.0, 0.0}  //st. 6
};

/*
 * Basic station:
 *
 *  -------------------- --------------------
 * |                    |                    |
 * |  module 0          |  module 1          |    ^ beam direction (Z)
 * |                    |                    |   /
 * |        ------------|--------------      |  /
 * |       /            |             /      | /
 * |      /             |            /       |/
 *  -----------------------------------------
 * |      \             |            \       |
 * |       \            |             \      |
 * |        \___________|______________\     |
 * |                    |                    |
 * |                    |                    |
 * |  module 3          |  module 4          |
 *  -----------------------------------------
 *
*/

//rotations of stations around y-axis by 180 deg.
const Bool_t YStationRotations[NStations] = {true/*0*/, false/*1*/, true/*2*/, false/*3*/, true/*4*/, false/*5*/, true/*6*/};

//rotations of stations around x-axis by 180 deg.
const Bool_t XStationRotations[NStations] = {false/*0*/, false/*1*/, false/*2*/, false/*3*/, false/*4*/, false/*5*/, false/*6*/};

//drift direction in each module ("true" is "forward", "false" is "backward")
const Bool_t DriftDirectionInModules[NStations][NMaxModules] = {
    {false, false, true, true},  //st. 0
    {true, true, false, false},  //st. 1
    {false, false, true, true},  //st. 2
    {true, true, false, false},  //st. 3
    {false, false, true, true},  //st. 4
    {true, true, false, false},  //st. 5
    {false, false, true, true}   //st. 6
};

//thicknesses of gaps inside a GEM chamber (for all modules)
const Double_t Gap_Drift_Thickness = 0.3; //cm
const Double_t Gap_FirstTransfer_Thickness = 0.25; //cm
const Double_t Gap_SecondTransfer_Thickness = 0.2; //cm
const Double_t Gap_Induction_Thickness = 0.15; //cm


const Double_t StripPitch = 0.08; //cm
//const Double_t StripStereoAngle = 15.0; //deg. (plus - clockwise, minus - anticlockwise)

const Double_t XMainZoneLayerSize =  81.6; //cm
const Double_t YMainZoneLayerSize = 45.0; //cm

const Double_t XHotZoneLayerSize_StraightStrips = 40.0; //cm
const Double_t YHotZoneLayerSize_StraightStrips  = 15.0; //cm

const Double_t XHotZoneLayerSize_InclinedStrips[2] = {37.99, 42.01}; // [0] - min, [1] - max
const Double_t YHotZoneLayerSize_InclinedStrips = 15.0;

const Int_t NLayersInModule = 4;

const Int_t ZoneTypeOfLayers[NLayersInModule] = {0, 0, 1, 1}; // 0 - main, 1 - hot zone
const char* StripTypeOfLayes[NLayersInModule] = {"lower", "upper", "lower", "upper"}; // "lower" is straight strips, "upper " is inclined strips

//stereo angle for all layers with inclined strips in each module of a basic station
const Double_t StripStereoAngleInModules[NLayersInModule] = {+15.0, +15.0, -15.0, -15.0}; //deg. (plus - clockwise, minus - anticlockwise)

enum StripNumberingDirection {LeftToRight, RightToLeft};
StripNumberingDirection StripDirection[2] = { LeftToRight, RightToLeft };

Double_t HoleRadius = 4.0;

//dead zones
Double_t XDeadZone_Rect[4] = {
    0.0 - XMainZoneLayerSize*0.5 + XHotZoneLayerSize_StraightStrips,
    0.0 - XMainZoneLayerSize*0.5 + XHotZoneLayerSize_StraightStrips,
    0.0 - XMainZoneLayerSize*0.5,
    0.0 - XMainZoneLayerSize*0.5
};
Double_t YDeadZone_Rect[4] = {
    0.0 - YMainZoneLayerSize*0.5,
    0.0 - YMainZoneLayerSize*0.5 + YHotZoneLayerSize_StraightStrips,
    0.0 - YMainZoneLayerSize*0.5 + YHotZoneLayerSize_StraightStrips,
    0.0 - YMainZoneLayerSize*0.5
};

Double_t XDeadZone_TrapLeft[4] = {
    0.0 - XMainZoneLayerSize*0.5 + XHotZoneLayerSize_InclinedStrips[1],
    0.0 - XMainZoneLayerSize*0.5 + XHotZoneLayerSize_InclinedStrips[0],
    0.0 - XMainZoneLayerSize*0.5,
    0.0 - XMainZoneLayerSize*0.5
};
Double_t YDeadZone_TrapLeft[4] = {
    0.0 - YMainZoneLayerSize*0.5,
    0.0 - YMainZoneLayerSize*0.5 + YHotZoneLayerSize_InclinedStrips,
    0.0 - YMainZoneLayerSize*0.5 + YHotZoneLayerSize_InclinedStrips,
    0.0 - YMainZoneLayerSize*0.5
};

Double_t XDeadZone_TrapRight[4] = {
    0.0 - XMainZoneLayerSize*0.5 + XHotZoneLayerSize_InclinedStrips[0],
    0.0 - XMainZoneLayerSize*0.5 + XHotZoneLayerSize_InclinedStrips[1],
    0.0 - XMainZoneLayerSize*0.5,
    0.0 - XMainZoneLayerSize*0.5
};
Double_t YDeadZone_TrapRight[4] = {
    0.0 - YMainZoneLayerSize*0.5,
    0.0 - YMainZoneLayerSize*0.5 + YHotZoneLayerSize_InclinedStrips,
    0.0 - YMainZoneLayerSize*0.5 + YHotZoneLayerSize_InclinedStrips,
    0.0 - YMainZoneLayerSize*0.5
};

Double_t XDeadZone_TrianLeft[3] = {
    0.0 - XMainZoneLayerSize*0.5 + XHotZoneLayerSize_InclinedStrips[1],
    0.0 - XMainZoneLayerSize*0.5 + XHotZoneLayerSize_InclinedStrips[1],
    0.0 - XMainZoneLayerSize*0.5 + XHotZoneLayerSize_InclinedStrips[0]
};
Double_t YDeadZone_TrianLeft[3] = {
    0.0 - YMainZoneLayerSize*0.5,
    0.0 - YMainZoneLayerSize*0.5 + YHotZoneLayerSize_InclinedStrips,
    0.0 - YMainZoneLayerSize*0.5 + YHotZoneLayerSize_InclinedStrips
};

Double_t XDeadZone_TrianRight[3] = {
    0.0 - XMainZoneLayerSize*0.5 + XHotZoneLayerSize_InclinedStrips[1],
    0.0 - XMainZoneLayerSize*0.5 + XHotZoneLayerSize_InclinedStrips[1],
    0.0 - XMainZoneLayerSize*0.5 + XHotZoneLayerSize_InclinedStrips[0]
};
Double_t YDeadZone_TrianRight[3] = {
    0.0 - YMainZoneLayerSize*0.5,
    0.0 - YMainZoneLayerSize*0.5 + YHotZoneLayerSize_InclinedStrips,
    0.0 - YMainZoneLayerSize*0.5
};

Double_t XDeadZone_Hole[4] = {
    0.0 - XMainZoneLayerSize*0.5 + HoleRadius,
    0.0 - XMainZoneLayerSize*0.5 + HoleRadius,
    0.0 - XMainZoneLayerSize*0.5,
    0.0 - XMainZoneLayerSize*0.5
};
Double_t YDeadZone_Hole[4] = {
    0.0 - YMainZoneLayerSize*0.5,
    0.0 - YMainZoneLayerSize*0.5 + HoleRadius,
    0.0 - YMainZoneLayerSize*0.5 + HoleRadius,
    0.0 - YMainZoneLayerSize*0.5
};

//------------------------------------------------------------------------------

//Function declarations
//void PrintInfo();
void CalculateParameters(TString config_name, TString author, TString date);
//------------------------------------------------------------------------------


void GemConfigCalculate_FutureConfig2020() {

    // ----  Load libraries   --------------------------------------------------
#if ROOT_VERSION_CODE < ROOT_VERSION(5,99,99)
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
#endif
    bmnloadlibs(); // load BmnRoot libraries
    // -------------------------------------------------------------------------

    TString ConfigName = "GemFutureConfig2020";
    TString AuthorName = "Baranov D.";
    TString DateCreated = "27.02.2020";

    //PrintInfo();
    CalculateParameters(ConfigName, AuthorName, DateCreated);

    cout << "It's the end" << "\n";
    //gApplication->Terminate();
}
//------------------------------------------------------------------------------

void CalculateParameters(TString config_name, TString author, TString date) {

    TString FileName = config_name + TString(".xml");

    cout << "\nCreating XML configuration file...\n";
    cout << "  Configuration name: " << config_name << "\n";
    cout << "  Author: " << author << "\n";
    cout << "  Date created: " << date << "\n";
    cout << "  File path: " << FileName << "\n";
    cout << "\n";

    ofstream FileOut(FileName.Data(), ios::out);

    FileOut << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";

    //reference frame usage comment
    FileOut << "<!--\
        \r\tConfig: " << config_name <<  "\
        \r\tAuthor: " << author <<  "\
        \r\tDate: " << date <<  "\n\
        \r\tA right-handed coordinates are used.\
        \r\tIf you look at the GEMs from the target:\
        \r\t\tx-axis is directed to the left,\
        \r\t\ty-axis - to the top,\
        \r\t\tz-axis - to the GEMs.\
        \r-->\n\n";

    FileOut << "<StationSet>\n";
    FileOut << "\n";

    for(Int_t istation = 0; istation < NStations; ++istation) {

        //cout << "istation = " << istation << ": -----------------------------\n";

        FileOut << "<!-- Station " << istation << " -->\n"; //comment

        FileOut << "\t<Station";
        FileOut << " xPosition=\"" << XStationPositions[istation] << "\"";
        FileOut << " yPosition=\"" << YStationPositions[istation] << "\"";
        FileOut << " zPosition=\"" << ZStationPositions[istation] << "\"";
        FileOut << " beamHole=\"" << "4.0" << "\"";
        FileOut << ">\n";
        FileOut << "\n";

        for(Int_t imodule = 0; imodule < NModulesInStations[istation]; ++imodule) {

            //cout << "  imodule = " << imodule << "\n";

            FileOut << "\t\t<!-- module " << imodule << " -->\n"; //comment

            FileOut << "\t\t<Module";
            FileOut << " xShift=\"" << XModuleShifts[istation][imodule] << "\"";
            FileOut << " yShift=\"" << YModuleShifts[istation][imodule] << "\"";
            FileOut << " zShift=\"" << ZModuleShifts[istation][imodule] << "\"";
            FileOut << " driftDirection=\"" << (DriftDirectionInModules[istation][imodule] ? "forward" : "backward") << "\"";
            FileOut << " gap_drift_thick=\"" << Gap_Drift_Thickness << "\"";
            FileOut << " gap_ftransf_thick=\"" << Gap_FirstTransfer_Thickness << "\"";
            FileOut << " gap_stransf_thick=\"" << Gap_SecondTransfer_Thickness << "\"";
            FileOut << " gap_induct_thick=\"" << Gap_Induction_Thickness << "\"";
            FileOut << ">\n";

            for(Int_t ilayer = 0; ilayer < NLayersInModule; ++ilayer) {

                //cout << "    ilayer = "  <<  ilayer  << "\n";

                FileOut << "\t\t\t<Layer";

                FileOut << " zoneNumber=\"" << ZoneTypeOfLayers[ilayer] << "\"";
                FileOut << " type=\"" << StripTypeOfLayes[ilayer] << "\"";

                //define a strip angle for the current layer
                Double_t layer_angle;
                if(TString(StripTypeOfLayes[ilayer]) == TString("lower")) {
                    layer_angle = 0.0;
                }
                else {
                    layer_angle = StripStereoAngleInModules[imodule];
                    if(YStationRotations[istation]^XStationRotations[istation]) layer_angle *= -1;

                }

                FileOut << " stripAngle=\"" << layer_angle << "\"";
                FileOut << " pitch=\"" << StripPitch << "\"";

                //define 'xorig' and 'yorig' coordinates (left bottom point), 'xsize' and 'ysize'
                Double_t xorig = 0.0; //default
                Double_t yorig = 0.0; //default
                Double_t xsize = 0.0; //default
                Double_t ysize = 0.0; //default

                if(ZoneTypeOfLayers[ilayer] == 0) {  //main zone
                    xorig = 0.0 + XMainZoneLayerSize*0.5;
                    yorig = 0.0 - YMainZoneLayerSize*0.5;
                    xsize = XMainZoneLayerSize;
                    ysize = YMainZoneLayerSize;
                }
                else {  //hot zone
                    switch(imodule) {
                        case 0:
                            if(TString(StripTypeOfLayes[ilayer]) == TString("lower")) {
                                xorig = 0.0 - XMainZoneLayerSize*0.5 + XHotZoneLayerSize_StraightStrips;
                                yorig = 0.0 - YMainZoneLayerSize*0.5;
                                xsize = XHotZoneLayerSize_StraightStrips;
                                ysize = YHotZoneLayerSize_StraightStrips;
                            }
                            else {
                                xorig = 0.0 - XMainZoneLayerSize*0.5 + XHotZoneLayerSize_InclinedStrips[1];
                                yorig = 0.0 - YMainZoneLayerSize*0.5;
                                xsize = XHotZoneLayerSize_InclinedStrips[1];
                                ysize = YHotZoneLayerSize_InclinedStrips;
                            }
                            break;

                        case 1:
                            if(TString(StripTypeOfLayes[ilayer]) == TString("lower")) {
                                xorig = 0.0 + XMainZoneLayerSize*0.5;
                                yorig = 0.0 - YMainZoneLayerSize*0.5;
                                xsize = XHotZoneLayerSize_StraightStrips;
                                ysize = YHotZoneLayerSize_StraightStrips;
                            }
                            else {
                                xorig = 0.0 + XMainZoneLayerSize*0.5;
                                yorig = 0.0 - YMainZoneLayerSize*0.5;
                                xsize = XHotZoneLayerSize_InclinedStrips[1];
                                ysize = YHotZoneLayerSize_InclinedStrips;
                            }
                            break;

                        case 2:
                            if(TString(StripTypeOfLayes[ilayer]) == TString("lower")) {
                                xorig = 0.0 - XMainZoneLayerSize*0.5 + XHotZoneLayerSize_StraightStrips;
                                yorig = 0.0 + YMainZoneLayerSize*0.5 - YHotZoneLayerSize_StraightStrips;
                                xsize = XHotZoneLayerSize_StraightStrips;
                                ysize = YHotZoneLayerSize_StraightStrips;
                            }
                            else {
                                xorig = 0.0 - XMainZoneLayerSize*0.5 + XHotZoneLayerSize_InclinedStrips[1];
                                yorig = 0.0 + YMainZoneLayerSize*0.5 - YHotZoneLayerSize_StraightStrips;
                                xsize = XHotZoneLayerSize_InclinedStrips[1];
                                ysize = YHotZoneLayerSize_InclinedStrips;
                            }
                            break;

                        case 3:
                            if(TString(StripTypeOfLayes[ilayer]) == TString("lower")) {
                                xorig = 0.0 + XMainZoneLayerSize*0.5;
                                yorig = 0.0 + YMainZoneLayerSize*0.5 - YHotZoneLayerSize_StraightStrips;
                                xsize = XHotZoneLayerSize_StraightStrips;
                                ysize = YHotZoneLayerSize_StraightStrips;
                            }
                            else {
                                xorig = 0.0 + XMainZoneLayerSize*0.5;
                                yorig = 0.0 + YMainZoneLayerSize*0.5 - YHotZoneLayerSize_StraightStrips;
                                xsize = XHotZoneLayerSize_InclinedStrips[1];
                                ysize = YHotZoneLayerSize_InclinedStrips;
                            }
                            break;
                    }
                }

                FileOut << " xorig=\"" << xorig << "\"";
                FileOut << " yorig=\"" << yorig << "\"";
                FileOut << " xsize=\"" << xsize << "\"";
                FileOut << " ysize=\"" << ysize << "\"";

                if (YStationRotations[istation] == false) FileOut << " stripDirection=\"" << "LeftToRight" << "\"";
                else FileOut << " stripDirection=\"" << "RightToLeft" << "\"";

                //define 'lborder' and 'rborder'
                TString lborder = "";
                TString rborder = "";

                switch(imodule) {
                    case 0:
                        if(YStationRotations[istation] == false) {
                            if(ZoneTypeOfLayers[ilayer] == 0) { //main zone
                                lborder = "LeftTop";
                                rborder = "RightBottom";
                            }
                            else {  //hot zone
                                lborder = "LeftBottom";
                                rborder = "RightBottom";
                            }
                        }
                        else {
                            if(ZoneTypeOfLayers[ilayer] == 0) { //main zone
                                lborder = "LeftBottom";
                                rborder = "RightTop";
                            }
                            else {  //hot zone
                                lborder = "LeftTop";
                                rborder = "RightTop";
                            }
                        }
                        break;

                    case 1:
                        if(YStationRotations[istation] == false) {
                            if(ZoneTypeOfLayers[ilayer] == 0) { //main zone
                                lborder = "LeftTop";
                                rborder = "RightBottom";
                            }
                            else {  //hot zone
                                lborder = "LeftTop";
                                rborder = "RightTop";
                            }
                        }
                        else {
                            if(ZoneTypeOfLayers[ilayer] == 0) { //main zone
                                lborder = "LeftBottom";
                                rborder = "RightTop";
                            }
                            else {  //hot zone
                                lborder = "LeftBottom";
                                rborder = "RightBottom";
                            }
                        }
                        break;

                    case 2:
                        if(YStationRotations[istation] == false) {
                            if(ZoneTypeOfLayers[ilayer] == 0) { //main zone
                                lborder = "LeftBottom";
                                rborder = "RightTop";
                            }
                            else {  //hot zone
                                lborder = "LeftTop";
                                rborder = "RightTop";
                            }
                        }
                        else {
                            if(ZoneTypeOfLayers[ilayer] == 0) { //main zone
                                lborder = "LeftTop";
                                rborder = "RightBottom";
                            }
                            else {  //hot zone
                                lborder = "LeftBottom";
                                rborder = "RightBottom";
                            }
                        }
                        break;

                    case 3:
                        if(YStationRotations[istation] == false) {
                            if(ZoneTypeOfLayers[ilayer] == 0) { //main zone
                                lborder = "LeftBottom";
                                rborder = "RightTop";
                            }
                            else {  //hot zone
                                lborder = "LeftBottom";
                                rborder = "RightBottom";
                            }
                        }
                        else {
                            if(ZoneTypeOfLayers[ilayer] == 0) { //main zone
                                lborder = "LeftTop";
                                rborder = "RightBottom";
                            }
                            else {  //hot zone
                                lborder = "LeftTop";
                                rborder = "RightTop";
                            }
                        }
                        break;
                }

                FileOut << " lborder=\"" << lborder << "\"";
                FileOut << " rborder=\"" << rborder << "\"";
                FileOut << ">\n";

                //add dead zones

                switch(imodule) {
                    //module 0 -------------------------------------------------
                    case 0:
                        if(ZoneTypeOfLayers[ilayer] == 0) { //main zone
                            if(TString(StripTypeOfLayes[ilayer]) == TString("lower")) { //straight strips
                                FileOut << "\t\t\t\t<DeadZone>\n";
                                for(Int_t ipoint = 0; ipoint < 4; ++ipoint) {
                                    FileOut << "\t\t\t\t\t<DeadPoint";
                                    FileOut << " x=\"" << XDeadZone_Rect[ipoint] << "\"";
                                    FileOut << " y=\"" << YDeadZone_Rect[ipoint] << "\"";
                                    FileOut << "/>\n";
                                }
                                FileOut << "\t\t\t\t</DeadZone>\n";
                            }
                            else { //inclined strips
                                FileOut << "\t\t\t\t<DeadZone>\n";
                                for(Int_t ipoint = 0; ipoint < 4; ++ipoint) {
                                    FileOut << "\t\t\t\t\t<DeadPoint";
                                    if (YStationRotations[istation] == false) {
                                        FileOut << " x=\"" << XDeadZone_TrapLeft[ipoint] << "\"";
                                        FileOut << " y=\"" << YDeadZone_TrapLeft[ipoint] << "\"";
                                    }
                                    else {
                                        FileOut << " x=\"" << XDeadZone_TrapRight[ipoint] << "\"";
                                        FileOut << " y=\"" << YDeadZone_TrapRight[ipoint] << "\"";
                                    }
                                    FileOut << "/>\n";
                                }
                                FileOut << "\t\t\t\t</DeadZone>\n";
                            }
                        }
                        else { //hot zone
                            FileOut << "\t\t\t\t<DeadZone>"; //hole
                            FileOut << " <!-- hole -->\n";
                            for(Int_t ipoint = 0; ipoint < 4; ++ipoint) {
                                FileOut << "\t\t\t\t\t<DeadPoint";
                                FileOut << " x=\"" << XDeadZone_Hole[ipoint] << "\"";
                                FileOut << " y=\"" << YDeadZone_Hole[ipoint] << "\"";
                                FileOut << "/>\n";
                            }
                            FileOut << "\t\t\t\t</DeadZone>\n";

                            if(TString(StripTypeOfLayes[ilayer]) == TString("upper")) { //inclined strips
                                FileOut << "\t\t\t\t<DeadZone>"; //triangle
                                FileOut << " <!-- triangle -->\n";
                                for(Int_t ipoint = 0; ipoint < 3; ++ipoint) {
                                    FileOut << "\t\t\t\t\t<DeadPoint";
                                    if (YStationRotations[istation] == false) {
                                        FileOut << " x=\"" << XDeadZone_TrianLeft[ipoint] << "\"";
                                        FileOut << " y=\"" << YDeadZone_TrianLeft[ipoint] << "\"";
                                    }
                                    else {
                                        FileOut << " x=\"" << XDeadZone_TrianRight[ipoint] << "\"";
                                        FileOut << " y=\"" << YDeadZone_TrianRight[ipoint] << "\"";
                                    }
                                    FileOut << "/>\n";
                                }
                                FileOut << "\t\t\t\t</DeadZone>\n";
                            }
                        }
                        break;
                    //----------------------------------------------------------
                    //module 1 -------------------------------------------------
                    case 1:
                        if(ZoneTypeOfLayers[ilayer] == 0) { //main zone
                            if(TString(StripTypeOfLayes[ilayer]) == TString("lower")) { //straight strips
                                FileOut << "\t\t\t\t<DeadZone>\n";
                                for(Int_t ipoint = 0; ipoint < 4; ++ipoint) {
                                    FileOut << "\t\t\t\t\t<DeadPoint";
                                    FileOut << " x=\"" << XDeadZone_Rect[ipoint]*(-1) << "\"";
                                    FileOut << " y=\"" << YDeadZone_Rect[ipoint] << "\"";
                                    FileOut << "/>\n";
                                }
                                FileOut << "\t\t\t\t</DeadZone>\n";
                            }
                            else { //inclined strips
                                FileOut << "\t\t\t\t<DeadZone>\n";
                                for(Int_t ipoint = 0; ipoint < 4; ++ipoint) {
                                    FileOut << "\t\t\t\t\t<DeadPoint";
                                    if (YStationRotations[istation] == false) {
                                        FileOut << " x=\"" << XDeadZone_TrapRight[ipoint]*(-1) << "\"";
                                        FileOut << " y=\"" << YDeadZone_TrapRight[ipoint] << "\"";
                                    }
                                    else {
                                        FileOut << " x=\"" << XDeadZone_TrapLeft[ipoint]*(-1) << "\"";
                                        FileOut << " y=\"" << YDeadZone_TrapLeft[ipoint] << "\"";
                                    }
                                    FileOut << "/>\n";
                                }
                                FileOut << "\t\t\t\t</DeadZone>\n";
                            }
                        }
                        else { //hot zone
                            FileOut << "\t\t\t\t<DeadZone>"; //hole
                            FileOut << " <!-- hole -->\n";
                            for(Int_t ipoint = 0; ipoint < 4; ++ipoint) {
                                FileOut << "\t\t\t\t\t<DeadPoint";
                                FileOut << " x=\"" << XDeadZone_Hole[ipoint]*(-1) << "\"";
                                FileOut << " y=\"" << YDeadZone_Hole[ipoint] << "\"";
                                FileOut << "/>\n";
                            }
                            FileOut << "\t\t\t\t</DeadZone>\n";

                            if(TString(StripTypeOfLayes[ilayer]) == TString("upper")) { //inclined strips
                                FileOut << "\t\t\t\t<DeadZone>"; //triangle
                                FileOut << " <!-- triangle -->\n";
                                for(Int_t ipoint = 0; ipoint < 3; ++ipoint) {
                                    FileOut << "\t\t\t\t\t<DeadPoint";
                                    if (YStationRotations[istation] == false) {
                                        FileOut << " x=\"" << XDeadZone_TrianRight[ipoint]*(-1) << "\"";
                                        FileOut << " y=\"" << YDeadZone_TrianRight[ipoint] << "\"";
                                    }
                                    else {
                                        FileOut << " x=\"" << XDeadZone_TrianLeft[ipoint]*(-1) << "\"";
                                        FileOut << " y=\"" << YDeadZone_TrianLeft[ipoint] << "\"";
                                    }
                                    FileOut << "/>\n";
                                }
                                FileOut << "\t\t\t\t</DeadZone>\n";
                            }
                        }
                        break;
                    //----------------------------------------------------------
                    //module 2 -------------------------------------------------
                    case 2:
                        if(ZoneTypeOfLayers[ilayer] == 0) { //main zone
                            if(TString(StripTypeOfLayes[ilayer]) == TString("lower")) { //straight strips
                                FileOut << "\t\t\t\t<DeadZone>\n";
                                for(Int_t ipoint = 0; ipoint < 4; ++ipoint) {
                                    FileOut << "\t\t\t\t\t<DeadPoint";
                                    FileOut << " x=\"" << XDeadZone_Rect[ipoint] << "\"";
                                    FileOut << " y=\"" << YDeadZone_Rect[ipoint]*(-1) << "\"";
                                    FileOut << "/>\n";
                                }
                                FileOut << "\t\t\t\t</DeadZone>\n";
                            }
                            else { //inclined strips
                                FileOut << "\t\t\t\t<DeadZone>\n";
                                for(Int_t ipoint = 0; ipoint < 4; ++ipoint) {
                                    FileOut << "\t\t\t\t\t<DeadPoint";
                                    if (YStationRotations[istation] == false) {
                                        FileOut << " x=\"" << XDeadZone_TrapLeft[ipoint] << "\"";
                                        FileOut << " y=\"" << YDeadZone_TrapLeft[ipoint]*(-1) << "\"";
                                    }
                                    else {
                                        FileOut << " x=\"" << XDeadZone_TrapRight[ipoint] << "\"";
                                        FileOut << " y=\"" << YDeadZone_TrapRight[ipoint]*(-1) << "\"";
                                    }
                                    FileOut << "/>\n";
                                }
                                FileOut << "\t\t\t\t</DeadZone>\n";
                            }
                        }
                        else { //hot zone
                            FileOut << "\t\t\t\t<DeadZone>"; //hole
                            FileOut << " <!-- hole -->\n";
                            for(Int_t ipoint = 0; ipoint < 4; ++ipoint) {
                                FileOut << "\t\t\t\t\t<DeadPoint";
                                FileOut << " x=\"" << XDeadZone_Hole[ipoint] << "\"";
                                FileOut << " y=\"" << YDeadZone_Hole[ipoint]*(-1) << "\"";
                                FileOut << "/>\n";
                            }
                            FileOut << "\t\t\t\t</DeadZone>\n";

                            if(TString(StripTypeOfLayes[ilayer]) == TString("upper")) { //inclined strips
                                FileOut << "\t\t\t\t<DeadZone>"; //triangle
                                FileOut << " <!-- triangle -->\n";
                                for(Int_t ipoint = 0; ipoint < 3; ++ipoint) {
                                    FileOut << "\t\t\t\t\t<DeadPoint";
                                    if (YStationRotations[istation] == false) {
                                        FileOut << " x=\"" << XDeadZone_TrianLeft[ipoint] << "\"";
                                        FileOut << " y=\"" << YDeadZone_TrianLeft[ipoint]*(-1) << "\"";
                                    }
                                    else {
                                        FileOut << " x=\"" << XDeadZone_TrianRight[ipoint] << "\"";
                                        FileOut << " y=\"" << YDeadZone_TrianRight[ipoint]*(-1) << "\"";
                                    }
                                    FileOut << "/>\n";
                                }
                                FileOut << "\t\t\t\t</DeadZone>\n";
                            }
                        }
                        break;
                    //----------------------------------------------------------
                    //module 3 -------------------------------------------------
                    case 3:
                        if(ZoneTypeOfLayers[ilayer] == 0) { //main zone
                            if(TString(StripTypeOfLayes[ilayer]) == TString("lower")) { //straight strips
                                FileOut << "\t\t\t\t<DeadZone>\n";
                                for(Int_t ipoint = 0; ipoint < 4; ++ipoint) {
                                    FileOut << "\t\t\t\t\t<DeadPoint";
                                    FileOut << " x=\"" << XDeadZone_Rect[ipoint]*(-1) << "\"";
                                    FileOut << " y=\"" << YDeadZone_Rect[ipoint]*(-1) << "\"";
                                    FileOut << "/>\n";
                                }
                                FileOut << "\t\t\t\t</DeadZone>\n";
                            }
                            else { //inclined strips
                                FileOut << "\t\t\t\t<DeadZone>\n";
                                for(Int_t ipoint = 0; ipoint < 4; ++ipoint) {
                                    FileOut << "\t\t\t\t\t<DeadPoint";
                                    if (YStationRotations[istation] == false) {
                                        FileOut << " x=\"" << XDeadZone_TrapRight[ipoint]*(-1) << "\"";
                                        FileOut << " y=\"" << YDeadZone_TrapRight[ipoint]*(-1) << "\"";
                                    }
                                    else {
                                        FileOut << " x=\"" << XDeadZone_TrapLeft[ipoint]*(-1) << "\"";
                                        FileOut << " y=\"" << YDeadZone_TrapLeft[ipoint]*(-1) << "\"";
                                    }
                                    FileOut << "/>\n";
                                }
                                FileOut << "\t\t\t\t</DeadZone>\n";
                            }
                        }
                        else { //hot zone
                            FileOut << "\t\t\t\t<DeadZone>"; //hole
                            FileOut << " <!-- hole -->\n";
                            for(Int_t ipoint = 0; ipoint < 4; ++ipoint) {
                                FileOut << "\t\t\t\t\t<DeadPoint";
                                FileOut << " x=\"" << XDeadZone_Hole[ipoint]*(-1) << "\"";
                                FileOut << " y=\"" << YDeadZone_Hole[ipoint]*(-1) << "\"";
                                FileOut << "/>\n";
                            }
                            FileOut << "\t\t\t\t</DeadZone>\n";

                            if(TString(StripTypeOfLayes[ilayer]) == TString("upper")) { //inclined strips
                                FileOut << "\t\t\t\t<DeadZone>"; //triangle
                                FileOut << " <!-- triangle -->\n";
                                for(Int_t ipoint = 0; ipoint < 3; ++ipoint) {
                                    FileOut << "\t\t\t\t\t<DeadPoint";
                                    if (YStationRotations[istation] == false) {
                                        FileOut << " x=\"" << XDeadZone_TrianRight[ipoint]*(-1) << "\"";
                                        FileOut << " y=\"" << YDeadZone_TrianRight[ipoint]*(-1) << "\"";
                                    }
                                    else {
                                        FileOut << " x=\"" << XDeadZone_TrianLeft[ipoint]*(-1) << "\"";
                                        FileOut << " y=\"" << YDeadZone_TrianLeft[ipoint]*(-1) << "\"";
                                    }
                                    FileOut << "/>\n";
                                }
                                FileOut << "\t\t\t\t</DeadZone>\n";
                            }
                        }
                        break;
                    //----------------------------------------------------------
                }
                FileOut << "\t\t\t</Layer>\n";
            }
            FileOut << "\t\t</Module>\n";
            FileOut << "\n";
        }
        FileOut << "\t</Station>\n";
        FileOut << "\n";

        //cout << "------------------------------------------------------------\n";
    }

    FileOut << "</StationSet>\n";

    FileOut.close();
}