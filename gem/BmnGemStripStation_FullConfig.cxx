#include "BmnGemStripStation_FullConfig.h"

#include "TWbox.h"
#include "TLine.h"
#include "TGaxis.h"
#include "TSystem.h"

BmnGemStripStation_FullConfig::BmnGemStripStation_FullConfig(Int_t iStation,
                       Double_t xpos_station, Double_t ypos_station, Double_t zpos_station,
                       Double_t beamradius) {

    //All linear values must be entered in units of cm

    StationNumber = iStation;

    XPosition = xpos_station;
    YPosition = ypos_station;
    ZPosition = zpos_station;

    BeamHoleRadius = beamradius;

    dXInnerFrame = 1.7;
    dYInnerFrame = 1.7;
    dXOuterFrame = 5.0;
    dYOuterFrame = 3.75;

    ZSizeGemModule = 2.5;
    ZSizeReadoutModule = 0.9;

    XSize = 0.0;
    YSize = 0.0;
    ZSize = 0.0;

    if( iStation >= 0 && iStation <= 3 ) {
        PitchValueModule = 0.04;
        LowerStripWidth = 0.034;
        UpperStripWidth = 0.008;
    }
    if( iStation >= 4 && iStation <= 7 ) {
        PitchValueModule = 0.08;
        LowerStripWidth = 0.068;
        UpperStripWidth = 0.016;
    }
    if( iStation >= 8 && iStation <= 11 ) {
        PitchValueModule = 0.1;
        LowerStripWidth = 0.085;
        UpperStripWidth = 0.02;
    }

    switch (iStation) {
        case 0:
            StripAngle = -7.5; break;
        case 1:
            StripAngle = 7.5; break;
        case 2:
            StripAngle = -7.5; break;
        case 3:
            StripAngle = 7.5; break;
        case 4:
            StripAngle = 15.0; break; //prototype
        case 5:
            StripAngle = -15.0; break;
        case 6:
            StripAngle = 15.0; break;
        case 7:
            StripAngle = -15.0; break;
        case 8:
            StripAngle = 15.0; break;
        case 9:
            StripAngle = -15.0; break;
        case 10:
            StripAngle = 15.0; break;
        case 11:
            StripAngle = -15.0; break;
    }

    //SIZES

    //station 0 and 1
    XZoneSizes_Station[0][0] = XZoneSizes_Station[1][0] = 30.0; // x size of zone 1 (outer)
    XZoneSizes_Station[0][1] = XZoneSizes_Station[1][1] = 16.0; // x size of zone 2
    XZoneSizes_Station[0][2] = XZoneSizes_Station[1][2] = 12.0; // x size of zone 3
    XZoneSizes_Station[0][3] = XZoneSizes_Station[1][3] = 8.0;  // x size of zone 4 (inner)

    YZoneSizes_Station[0][0] = YZoneSizes_Station[1][0] = 30.0; // y size of zone 1 (outer)
    YZoneSizes_Station[0][1] = YZoneSizes_Station[1][1] = 12.0; // y size of zone 2
    YZoneSizes_Station[0][2] = YZoneSizes_Station[1][2] = 7.0; //  y size of zone 3
    YZoneSizes_Station[0][3] = YZoneSizes_Station[1][3] = 3.0; //  y size of zone 4 (inner)

    //station 2 and 3
    XZoneSizes_Station[2][0] = XZoneSizes_Station[3][0] = 40.0; // x size of zone 1 (outer)
    XZoneSizes_Station[2][1] = XZoneSizes_Station[3][1] = 20.0; // x size of zone 2
    XZoneSizes_Station[2][2] = XZoneSizes_Station[3][2] = 15.0; // x size of zone 3
    XZoneSizes_Station[2][3] = XZoneSizes_Station[3][3] = 10.0;  // x size of zone 4 (inner)

    YZoneSizes_Station[2][0] = YZoneSizes_Station[3][0] = 40.0; // y size of zone 1 (outer)
    YZoneSizes_Station[2][1] = YZoneSizes_Station[3][1] = 15.0; // y size of zone 2
    YZoneSizes_Station[2][2] = YZoneSizes_Station[3][2] = 9.0; //  y size of zone 3
    YZoneSizes_Station[2][3] = YZoneSizes_Station[3][3] = 4.0; //  y size of zone 4 (inner)

    //station 4 (prototype)
    XModuleSize_StationPrototype = 66.0;
    YModuleSize_StationPrototype = 41.0;

    XHotZoneSize_StationPrototype = 15.0;
    YHotZoneSize_StationPrototype = 10.0;

    //station 5, 6, 7
    //set zero for unused indices
    for(Int_t i = 0; i < 5; ++i) {
        XModuleSize_Station[i] = 0.0;
        YModuleSize_Station[i] = 0.0;
        XRectHotZoneSize_Station[i] = 0.0;
        YRectHotZoneSize_Station[i] = 0.0;
        XSlopeHotZoneSize_Station[i][0] = 0.0;
        XSlopeHotZoneSize_Station[i][1] = 0.0;
        YSlopeHotZoneSize_Station[i] = 0.0;
        YSlopeHotZoneSize_Station[i] = 0.0;
    }

    XModuleSize_Station[5] = XModuleSize_Station[6] = XModuleSize_Station[7] = 163.2*0.5;
    YModuleSize_Station[5] = YModuleSize_Station[6] = YModuleSize_Station[7] = 45.0;

    XRectHotZoneSize_Station[5] = XRectHotZoneSize_Station[6] = XRectHotZoneSize_Station[7] = 40.0;
    YRectHotZoneSize_Station[5] = YRectHotZoneSize_Station[6] = YRectHotZoneSize_Station[7] = 15.0;

    XSlopeHotZoneSize_Station[5][0] = XSlopeHotZoneSize_Station[6][0] = XSlopeHotZoneSize_Station[7][0] = 37.99;
    XSlopeHotZoneSize_Station[5][1] = XSlopeHotZoneSize_Station[6][1] = XSlopeHotZoneSize_Station[7][1] = 42.01;

    YSlopeHotZoneSize_Station[5] = YSlopeHotZoneSize_Station[6] = YSlopeHotZoneSize_Station[7] = 15.0;

    //station 8, 9, 10, 11
    XModuleSize_Station[8] = XModuleSize_Station[9] = XModuleSize_Station[10] = XModuleSize_Station[11] = 200.0/2;
    YModuleSize_Station[8] = YModuleSize_Station[9] = YModuleSize_Station[10] = YModuleSize_Station[11] = 45.0;

    XRectHotZoneSize_Station[8] = XRectHotZoneSize_Station[9] = XRectHotZoneSize_Station[10] = XRectHotZoneSize_Station[11] = 50.0;
    YRectHotZoneSize_Station[8] = YRectHotZoneSize_Station[9] = YRectHotZoneSize_Station[10] = YRectHotZoneSize_Station[11] = 20.0;

    XSlopeHotZoneSize_Station[8][0] = XSlopeHotZoneSize_Station[9][0] = XSlopeHotZoneSize_Station[10][0] = XSlopeHotZoneSize_Station[11][0] = 47.14;
    XSlopeHotZoneSize_Station[8][1] = XSlopeHotZoneSize_Station[9][1] = XSlopeHotZoneSize_Station[10][1] = XSlopeHotZoneSize_Station[11][1] = 52.5;

    YSlopeHotZoneSize_Station[8] = YSlopeHotZoneSize_Station[9] = YSlopeHotZoneSize_Station[10] = YSlopeHotZoneSize_Station[11] = 20.0;


    //Assembling a station -----------------------------------------------------
    if(StationNumber >= 0 && StationNumber <= 3) {

        NModules = 16; //16 modules in the station (4 modules for each hot zone)
        XSize = 2*XZoneSizes_Station[StationNumber][0];
        YSize = 2*YZoneSizes_Station[StationNumber][0];
        ZSize = ZSizeGemModule;

        BuildModules_4ZoneType();
    }

    if(StationNumber == 4) {

        NModules = 8; //8 modules in the station (4 big modules and 4 hot zones)
        XSize = 2*XModuleSize_StationPrototype + 2*dXInnerFrame + 2*BeamHoleRadius;
        YSize = 2*YModuleSize_StationPrototype + 2*dYInnerFrame + 2*BeamHoleRadius;
        ZSize = 2*ZSizeGemModule;

        BuildModules_Prototype();
    }

    if(StationNumber >= 5 && StationNumber <= 11) {

        NModules = 8; //8 modules in the station (4 big modules and 4 hot zones)
        XSize = 2*XModuleSize_Station[StationNumber];
        YSize = 2*YModuleSize_Station[StationNumber];
        ZSize = 2*ZSizeGemModule;

        BuildModules_2PartType();
    }
    //end assembling the station -----------------------------------------------

    //visual test
    //DrawCreatedStation();
}

BmnGemStripStation_FullConfig::~BmnGemStripStation_FullConfig() {
    for(Int_t i = 0; i < NModules; ++i) {
        if(ReadoutModules[i]) {
            delete ReadoutModules[i];
            ReadoutModules[i] = NULL;
        }
    }
    if(ReadoutModules) {
        delete [] ReadoutModules;
        ReadoutModules = NULL;
    }
}

Int_t BmnGemStripStation_FullConfig::GetPointModuleOwnership(Double_t xcoord, Double_t ycoord, Double_t zcoord) {

    if(StationNumber >= 0 && StationNumber <= 3) {

        for(Int_t i = 3; i >= 0; --i) {
            if((xcoord >= (XPosition-XZoneSizes_Station[StationNumber][i]) && xcoord <= (XPosition+0))
            && (ycoord >= (YPosition+0)  && (ycoord <= YPosition+YZoneSizes_Station[StationNumber][i]))) return i*4;

            if((xcoord >= (XPosition+0) && xcoord <= (XPosition+XZoneSizes_Station[StationNumber][i]))
            && (ycoord >= (YPosition+0) && (ycoord <= YPosition+YZoneSizes_Station[StationNumber][i]))) return i*4+1;

            if((xcoord >= (XPosition+0) && xcoord <= (XPosition+XZoneSizes_Station[StationNumber][i]))
            && (ycoord >= (YPosition-YZoneSizes_Station[StationNumber][i])  && (ycoord <= YPosition+0))) return i*4+2;

            if((xcoord >= (XPosition-XZoneSizes_Station[StationNumber][i]) && xcoord <= (XPosition+0))
            && (ycoord >= (YPosition-YZoneSizes_Station[StationNumber][i])  && (ycoord <= YPosition+0))) return i*4+3;
        }
    }

    if(StationNumber == 4) {
        for(Int_t i = 7; i >= 0; --i) {
            if((xcoord >= ReadoutModules[i]->GetXMinReadout()) && (xcoord <= ReadoutModules[i]->GetXMaxReadout())
            && (ycoord >= ReadoutModules[i]->GetYMinReadout()) && (ycoord <= ReadoutModules[i]->GetYMaxReadout()))
                return i;
        }
    }

    if(StationNumber >= 5 && StationNumber <= 11) {
        for(Int_t i = 7; i >= 0; --i) {
            if((xcoord >= ReadoutModules[i]->GetXMinReadout()) && (xcoord <= ReadoutModules[i]->GetXMaxReadout())
            && (ycoord >= ReadoutModules[i]->GetYMinReadout()) && (ycoord <= ReadoutModules[i]->GetYMaxReadout()))
                return i;
        }
    }

    return -1;
}

Int_t BmnGemStripStation_FullConfig::AddPointToStation(Double_t xcoord, Double_t ycoord, Double_t zcoord,
                                                       Double_t px, Double_t py, Double_t pz,
                                                       Double_t dEloss, Int_t refID) {

    Int_t module = GetPointModuleOwnership(xcoord, ycoord, zcoord);

    if(module >= 0) {
        //if(ReadoutModules[module]->AddRealPointFullOne(xcoord, ycoord, zcoord, dEloss, refID)) {
        if(ReadoutModules[module]->AddRealPointFull(xcoord, ycoord, zcoord, px, py, pz, dEloss, refID)) {
            return module;
        }
        else {
            return -1;
        }
    }
    return -1;
}

Int_t BmnGemStripStation_FullConfig::CountNAddedToStationPoints() {
    Int_t points_sum = 0;
    for(Int_t iModule = 0; iModule < NModules; iModule++) {
        points_sum += ReadoutModules[iModule]->GetNRealPoints();
    }
    return points_sum;
}

void BmnGemStripStation_FullConfig::ProcessPointsInStation() {
    for(Int_t iModule = 0; iModule < NModules; iModule++) {
        ReadoutModules[iModule]->CalculateStripHitIntersectionPoints();
    }
}

Int_t BmnGemStripStation_FullConfig::CountNProcessedPointInStation() {
    Int_t points_sum = 0;
    for(Int_t iModule = 0; iModule < NModules; iModule++) {
        points_sum += ReadoutModules[iModule]->GetNIntersectionPoints();
    }
    return points_sum;
}

BmnGemStripReadoutModule* BmnGemStripStation_FullConfig::GetReadoutModule(Int_t index) {
    if(index >= 0 && index < NModules) {
        return ReadoutModules[index];
    }
    return 0;
}

void BmnGemStripStation_FullConfig::BuildModules_4ZoneType() {

    ReadoutModules = new BmnGemStripReadoutModule* [NModules];

    //zone 1 ---------------------------------------------------------------
    //left-top quadrant
    ReadoutModules[0] =
        new BmnGemStripReadoutModule(XZoneSizes_Station[StationNumber][0], YZoneSizes_Station[StationNumber][0], XPosition-XZoneSizes_Station[StationNumber][0], YPosition+0, PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth, ZPosition+(ZSizeGemModule-ZSizeReadoutModule), ForwardZAxisEDrift);
    ReadoutModules[0]->SetDeadZone(XPosition-XZoneSizes_Station[StationNumber][1], XPosition+0, YPosition+0, YPosition+YZoneSizes_Station[StationNumber][1]);

    //right-top quadrant
    ReadoutModules[1] =
        new BmnGemStripReadoutModule(XZoneSizes_Station[StationNumber][0], YZoneSizes_Station[StationNumber][0], XPosition+0, YPosition+0, PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth, ZPosition+(ZSizeGemModule-ZSizeReadoutModule), ForwardZAxisEDrift);
    ReadoutModules[1]->SetDeadZone(XPosition+0, XPosition+XZoneSizes_Station[StationNumber][1], YPosition+0, YPosition+YZoneSizes_Station[StationNumber][1]);

    //right-bottom quadrant
    ReadoutModules[2] =
        new BmnGemStripReadoutModule(XZoneSizes_Station[StationNumber][0], YZoneSizes_Station[StationNumber][0], XPosition+0, YPosition-YZoneSizes_Station[StationNumber][0], PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth, ZPosition+(ZSizeGemModule-ZSizeReadoutModule), ForwardZAxisEDrift);
    ReadoutModules[2]->SetDeadZone(XPosition+0, XPosition+XZoneSizes_Station[StationNumber][1], YPosition-YZoneSizes_Station[StationNumber][1], YPosition+0);

    //left-bottom quadrant
    ReadoutModules[3] =
        new BmnGemStripReadoutModule(XZoneSizes_Station[StationNumber][0], YZoneSizes_Station[StationNumber][0], XPosition-XZoneSizes_Station[StationNumber][0], YPosition-YZoneSizes_Station[StationNumber][0], PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth, ZPosition+(ZSizeGemModule-ZSizeReadoutModule), ForwardZAxisEDrift);
    ReadoutModules[3]->SetDeadZone(XPosition-XZoneSizes_Station[StationNumber][1], XPosition+0, YPosition-YZoneSizes_Station[StationNumber][1], YPosition+0);
    //----------------------------------------------------------------------

    //zone 2 ---------------------------------------------------------------
    //left-top quadrant
    ReadoutModules[4] =
        new BmnGemStripReadoutModule(XZoneSizes_Station[StationNumber][1], YZoneSizes_Station[StationNumber][1], XPosition-XZoneSizes_Station[StationNumber][1], YPosition+0, PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth, ZPosition+(ZSizeGemModule-ZSizeReadoutModule), ForwardZAxisEDrift);
    ReadoutModules[4]->SetDeadZone(XPosition-XZoneSizes_Station[StationNumber][2], XPosition+0, YPosition+0, YPosition+YZoneSizes_Station[StationNumber][2]);

    //right-top quadrant
    ReadoutModules[5] =
        new BmnGemStripReadoutModule(XZoneSizes_Station[StationNumber][1], YZoneSizes_Station[StationNumber][1], XPosition+0, YPosition+0, PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth, ZPosition+(ZSizeGemModule-ZSizeReadoutModule), ForwardZAxisEDrift);
    ReadoutModules[5]->SetDeadZone(XPosition+0, XPosition+XZoneSizes_Station[StationNumber][2], YPosition+0, YPosition+YZoneSizes_Station[StationNumber][2]);

    //right-bottom quadrant
    ReadoutModules[6] =
        new BmnGemStripReadoutModule(XZoneSizes_Station[StationNumber][1], YZoneSizes_Station[StationNumber][1], XPosition+0, YPosition-YZoneSizes_Station[StationNumber][1], PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth, ZPosition+(ZSizeGemModule-ZSizeReadoutModule), ForwardZAxisEDrift);
    ReadoutModules[6]->SetDeadZone(XPosition+0, XPosition+XZoneSizes_Station[StationNumber][2], YPosition-YZoneSizes_Station[StationNumber][2], YPosition+0);

    //left-bottom quadrant
    ReadoutModules[7] =
        new BmnGemStripReadoutModule(XZoneSizes_Station[StationNumber][1], YZoneSizes_Station[StationNumber][1], XPosition-XZoneSizes_Station[StationNumber][1], YPosition-YZoneSizes_Station[StationNumber][1], PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth, ZPosition+(ZSizeGemModule-ZSizeReadoutModule), ForwardZAxisEDrift);
    ReadoutModules[7]->SetDeadZone(XPosition-XZoneSizes_Station[StationNumber][2], XPosition+0, YPosition-YZoneSizes_Station[StationNumber][2], YPosition+0);
    //----------------------------------------------------------------------

    //zone 3 ---------------------------------------------------------------
    //left-top quadrant
    ReadoutModules[8] =
        new BmnGemStripReadoutModule(XZoneSizes_Station[StationNumber][2], YZoneSizes_Station[StationNumber][2], XPosition-XZoneSizes_Station[StationNumber][2], YPosition+0, PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth, ZPosition+(ZSizeGemModule-ZSizeReadoutModule), ForwardZAxisEDrift);
    ReadoutModules[8]->SetDeadZone(XPosition-XZoneSizes_Station[StationNumber][3], XPosition+0, YPosition+0, YPosition+YZoneSizes_Station[StationNumber][3]);

    //right-top quadrant
    ReadoutModules[9] =
        new BmnGemStripReadoutModule(XZoneSizes_Station[StationNumber][2], YZoneSizes_Station[StationNumber][2], XPosition+0, YPosition+0, PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth, ZPosition+(ZSizeGemModule-ZSizeReadoutModule), ForwardZAxisEDrift);
    ReadoutModules[9]->SetDeadZone(XPosition+0, XPosition+XZoneSizes_Station[StationNumber][3], YPosition+0, YPosition+YZoneSizes_Station[StationNumber][3]);

    //right-bottom quadrant
    ReadoutModules[10] =
        new BmnGemStripReadoutModule(XZoneSizes_Station[StationNumber][2], YZoneSizes_Station[StationNumber][2], XPosition+0, YPosition-YZoneSizes_Station[StationNumber][2], PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth, ZPosition+(ZSizeGemModule-ZSizeReadoutModule), ForwardZAxisEDrift);
    ReadoutModules[10]->SetDeadZone(XPosition+0, XPosition+XZoneSizes_Station[StationNumber][3], YPosition-YZoneSizes_Station[StationNumber][3], YPosition+0);

    //left-bottom quadrant
    ReadoutModules[11] =
        new BmnGemStripReadoutModule(XZoneSizes_Station[StationNumber][2], YZoneSizes_Station[StationNumber][2], XPosition-XZoneSizes_Station[StationNumber][2], YPosition-YZoneSizes_Station[StationNumber][2], PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth, ZPosition+(ZSizeGemModule-ZSizeReadoutModule), ForwardZAxisEDrift);
    ReadoutModules[11]->SetDeadZone(XPosition-XZoneSizes_Station[StationNumber][3], XPosition+0, YPosition-YZoneSizes_Station[StationNumber][3], YPosition+0);
    //----------------------------------------------------------------------

    //zone 4---------------------------------------------------------------
    //left-top quadrant
    ReadoutModules[12] =
        new BmnGemStripReadoutModule(XZoneSizes_Station[StationNumber][3], YZoneSizes_Station[StationNumber][3], XPosition-XZoneSizes_Station[StationNumber][3], YPosition+0, PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth, ZPosition+(ZSizeGemModule-ZSizeReadoutModule), ForwardZAxisEDrift);
    ReadoutModules[12]->SetDeadZone(XPosition-BeamHoleRadius, XPosition+0, YPosition+0, YPosition+BeamHoleRadius);

    //right-top quadrant
    ReadoutModules[13] =
        new BmnGemStripReadoutModule(XZoneSizes_Station[StationNumber][3], YZoneSizes_Station[StationNumber][3], XPosition+0, YPosition+0, PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth, ZPosition+(ZSizeGemModule-ZSizeReadoutModule), ForwardZAxisEDrift);
    ReadoutModules[13]->SetDeadZone(XPosition+0, XPosition+BeamHoleRadius, YPosition+0, YPosition+BeamHoleRadius);

    //right-bottom quadrant
    ReadoutModules[14] =
        new BmnGemStripReadoutModule(XZoneSizes_Station[StationNumber][3], YZoneSizes_Station[StationNumber][3], XPosition+0, YPosition-YZoneSizes_Station[StationNumber][3], PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth, ZPosition+(ZSizeGemModule-ZSizeReadoutModule), ForwardZAxisEDrift);
    ReadoutModules[14]->SetDeadZone(XPosition+0, XPosition+BeamHoleRadius, YPosition-BeamHoleRadius, YPosition+0);

    //left-bottom quadrant
    ReadoutModules[15] =
        new BmnGemStripReadoutModule(XZoneSizes_Station[StationNumber][3], YZoneSizes_Station[StationNumber][3], XPosition-XZoneSizes_Station[StationNumber][3], YPosition-YZoneSizes_Station[StationNumber][3], PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth, ZPosition+(ZSizeGemModule-ZSizeReadoutModule), ForwardZAxisEDrift);
    ReadoutModules[15]->SetDeadZone(XPosition-BeamHoleRadius, XPosition+0, YPosition-BeamHoleRadius, YPosition+0);
    //----------------------------------------------------------------------
}

void BmnGemStripStation_FullConfig::BuildModules_Prototype() {

    ReadoutModules = new BmnGemStripReadoutModule* [NModules];

    //big modules
    ReadoutModules[0] =
        new BmnGemStripReadoutModule(XModuleSize_StationPrototype, YModuleSize_StationPrototype, XPosition-XModuleSize_StationPrototype+BeamHoleRadius+dXInnerFrame, YPosition+0+BeamHoleRadius+dYInnerFrame, PitchValueModule, -StripAngle, LowerStripWidth, UpperStripWidth, ZPosition+ZSizeGemModule, ForwardZAxisEDrift);
    ReadoutModules[0]->SetDeadZone(XPosition-XHotZoneSize_StationPrototype+BeamHoleRadius+dXInnerFrame, XPosition+0+BeamHoleRadius+dXInnerFrame, YPosition+0+BeamHoleRadius+dYInnerFrame, YPosition+YHotZoneSize_StationPrototype+BeamHoleRadius+dYInnerFrame);

    ReadoutModules[1] =
        new BmnGemStripReadoutModule(XModuleSize_StationPrototype, YModuleSize_StationPrototype, XPosition+0+BeamHoleRadius+dXInnerFrame, YPosition+0-BeamHoleRadius-dYInnerFrame, PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth, ZPosition+(ZSizeGemModule-ZSizeReadoutModule), ForwardZAxisEDrift);
    ReadoutModules[1]->SetDeadZone(XPosition+0+BeamHoleRadius+dXInnerFrame, XPosition+XHotZoneSize_StationPrototype+BeamHoleRadius+dXInnerFrame, YPosition+0-BeamHoleRadius-dYInnerFrame, YPosition+YHotZoneSize_StationPrototype-BeamHoleRadius-dYInnerFrame);

    ReadoutModules[2] =
        new BmnGemStripReadoutModule(XModuleSize_StationPrototype, YModuleSize_StationPrototype, XPosition+0-BeamHoleRadius-dXInnerFrame, YPosition-YModuleSize_StationPrototype-BeamHoleRadius-dYInnerFrame, PitchValueModule, -StripAngle, LowerStripWidth, UpperStripWidth, ZPosition+ZSizeGemModule, ForwardZAxisEDrift);
    ReadoutModules[2]->SetDeadZone(XPosition+0-BeamHoleRadius-dXInnerFrame, XPosition+XHotZoneSize_StationPrototype-BeamHoleRadius-dXInnerFrame, YPosition-YHotZoneSize_StationPrototype-BeamHoleRadius-dYInnerFrame, YPosition+0-BeamHoleRadius-dYInnerFrame);

    ReadoutModules[3] =
        new BmnGemStripReadoutModule(XModuleSize_StationPrototype, YModuleSize_StationPrototype, XPosition-XModuleSize_StationPrototype-BeamHoleRadius-dXInnerFrame, YPosition-YModuleSize_StationPrototype+BeamHoleRadius+dYInnerFrame, PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth, ZPosition+(ZSizeGemModule-ZSizeReadoutModule), ForwardZAxisEDrift);
    ReadoutModules[3]->SetDeadZone(XPosition-XHotZoneSize_StationPrototype-BeamHoleRadius-dXInnerFrame, XPosition+0-BeamHoleRadius-dXInnerFrame, YPosition-YHotZoneSize_StationPrototype+BeamHoleRadius+dYInnerFrame, YPosition+0+BeamHoleRadius+dYInnerFrame);

    //hot zones
    ReadoutModules[4] =
        new BmnGemStripReadoutModule(XHotZoneSize_StationPrototype, YHotZoneSize_StationPrototype, XPosition-XHotZoneSize_StationPrototype+BeamHoleRadius+dXInnerFrame, YPosition+0+BeamHoleRadius+dYInnerFrame, PitchValueModule, -StripAngle, LowerStripWidth, UpperStripWidth, ZPosition+ZSizeGemModule, ForwardZAxisEDrift);

    ReadoutModules[5] =
        new BmnGemStripReadoutModule(XHotZoneSize_StationPrototype, YHotZoneSize_StationPrototype, XPosition+0+BeamHoleRadius+dXInnerFrame, YPosition+0-BeamHoleRadius-dYInnerFrame, PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth, ZPosition+(ZSizeGemModule-ZSizeReadoutModule), ForwardZAxisEDrift);

    ReadoutModules[6] =
        new BmnGemStripReadoutModule(XHotZoneSize_StationPrototype, YHotZoneSize_StationPrototype, XPosition+0-BeamHoleRadius-dXInnerFrame, YPosition-YHotZoneSize_StationPrototype-BeamHoleRadius-dYInnerFrame, PitchValueModule, -StripAngle, LowerStripWidth, UpperStripWidth, ZPosition+ZSizeGemModule, ForwardZAxisEDrift);

    ReadoutModules[7] =
        new BmnGemStripReadoutModule(XHotZoneSize_StationPrototype, YHotZoneSize_StationPrototype, XPosition-XHotZoneSize_StationPrototype-BeamHoleRadius-dXInnerFrame, YPosition-YHotZoneSize_StationPrototype+BeamHoleRadius+dYInnerFrame, PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth, ZPosition+(ZSizeGemModule-ZSizeReadoutModule), ForwardZAxisEDrift);
}

void BmnGemStripStation_FullConfig::BuildModules_2PartType() {

    ReadoutModules = new BmnGemStripReadoutModule* [NModules];

    //big modules
    ReadoutModules[0] =
        new BmnGemStripReadoutModule(XModuleSize_Station[StationNumber], YModuleSize_Station[StationNumber], XPosition-XModuleSize_Station[StationNumber], YPosition+0, PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth, ZPosition+ZSizeGemModule, ForwardZAxisEDrift);
    if(StationNumber%2 == 1)
        ReadoutModules[0]->SetDeadZone(XPosition-XRectHotZoneSize_Station[StationNumber], XPosition+0, YPosition+0, YPosition+YRectHotZoneSize_Station[StationNumber]);
    else
        ReadoutModules[0]->SetDeadZone(XPosition-XSlopeHotZoneSize_Station[StationNumber][1], XPosition+0, YPosition+0, YPosition+YSlopeHotZoneSize_Station[StationNumber]);

    ReadoutModules[1] =
        new BmnGemStripReadoutModule(XModuleSize_Station[StationNumber], YModuleSize_Station[StationNumber], XPosition+0, YPosition+0, PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth, ZPosition+ZSizeGemModule, ForwardZAxisEDrift);
    if(StationNumber%2 == 1)
        ReadoutModules[1]->SetDeadZone(XPosition+0, XPosition+XSlopeHotZoneSize_Station[StationNumber][1], YPosition+0, YPosition+YSlopeHotZoneSize_Station[StationNumber]);
    else
        ReadoutModules[1]->SetDeadZone(XPosition+0, XPosition+XRectHotZoneSize_Station[StationNumber], YPosition+0, YPosition+YRectHotZoneSize_Station[StationNumber]);

    ReadoutModules[2] =
        new BmnGemStripReadoutModule(XModuleSize_Station[StationNumber], YModuleSize_Station[StationNumber], XPosition+0, YPosition-YModuleSize_Station[StationNumber], PitchValueModule, -StripAngle, LowerStripWidth, UpperStripWidth, ZPosition+(ZSizeGemModule-ZSizeReadoutModule), ForwardZAxisEDrift);
    if(StationNumber%2 == 1)
        ReadoutModules[2]->SetDeadZone(XPosition+0, XPosition+XSlopeHotZoneSize_Station[StationNumber][1], YPosition-YSlopeHotZoneSize_Station[StationNumber], YPosition+0);
    else
        ReadoutModules[2]->SetDeadZone(XPosition+0, XPosition+XRectHotZoneSize_Station[StationNumber], YPosition-YRectHotZoneSize_Station[StationNumber], YPosition+0);

    ReadoutModules[3] =
        new BmnGemStripReadoutModule(XModuleSize_Station[StationNumber], YModuleSize_Station[StationNumber], XPosition-XModuleSize_Station[StationNumber], YPosition-YModuleSize_Station[StationNumber], PitchValueModule, -StripAngle, LowerStripWidth, UpperStripWidth, ZPosition+(ZSizeGemModule-ZSizeReadoutModule), ForwardZAxisEDrift);
    if(StationNumber%2 == 1)
        ReadoutModules[3]->SetDeadZone(XPosition-XRectHotZoneSize_Station[StationNumber], XPosition+0, YPosition-YRectHotZoneSize_Station[StationNumber], YPosition+0);
    else
        ReadoutModules[3]->SetDeadZone(XPosition-XSlopeHotZoneSize_Station[StationNumber][1], XPosition+0, YPosition-YSlopeHotZoneSize_Station[StationNumber], YPosition+0);

    //hot zones
    if(StationNumber%2 == 1) {
        ReadoutModules[4] =
            new BmnGemStripReadoutModule(XRectHotZoneSize_Station[StationNumber], YRectHotZoneSize_Station[StationNumber], XPosition-XRectHotZoneSize_Station[StationNumber], YPosition+0, PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth, ZPosition+ZSizeGemModule, ForwardZAxisEDrift);
    }
    else {
        ReadoutModules[4] =
            new BmnGemStripReadoutModule(XSlopeHotZoneSize_Station[StationNumber][1], YSlopeHotZoneSize_Station[StationNumber], XPosition-XSlopeHotZoneSize_Station[StationNumber][1], YPosition+0, PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth, ZPosition+ZSizeGemModule, ForwardZAxisEDrift);
    }
    ReadoutModules[4]->SetDeadZone(XPosition-BeamHoleRadius, XPosition+0, YPosition+0, YPosition+BeamHoleRadius);

    if(StationNumber%2 == 1) {
        ReadoutModules[5] =
            new BmnGemStripReadoutModule(XSlopeHotZoneSize_Station[StationNumber][1], YSlopeHotZoneSize_Station[StationNumber], XPosition+0, YPosition+0, PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth, ZPosition+ZSizeGemModule, ForwardZAxisEDrift);
    }
    else {
        ReadoutModules[5] =
            new BmnGemStripReadoutModule(XRectHotZoneSize_Station[StationNumber], YRectHotZoneSize_Station[StationNumber], XPosition+0, YPosition+0, PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth, ZPosition+ZSizeGemModule, ForwardZAxisEDrift);
    }
    ReadoutModules[5]->SetDeadZone(XPosition+0, XPosition+BeamHoleRadius, YPosition+0, YPosition+BeamHoleRadius);

    if(StationNumber%2 == 1) {
        ReadoutModules[6] =
            new BmnGemStripReadoutModule(XSlopeHotZoneSize_Station[StationNumber][1], YSlopeHotZoneSize_Station[StationNumber], XPosition+0, YPosition-YSlopeHotZoneSize_Station[StationNumber], PitchValueModule, -StripAngle, LowerStripWidth, UpperStripWidth, ZPosition+(ZSizeGemModule-ZSizeReadoutModule), ForwardZAxisEDrift);
    }
    else {
        ReadoutModules[6] =
            new BmnGemStripReadoutModule(XRectHotZoneSize_Station[StationNumber], YRectHotZoneSize_Station[StationNumber], XPosition+0, YPosition-YRectHotZoneSize_Station[StationNumber], PitchValueModule, -StripAngle, LowerStripWidth, UpperStripWidth, ZPosition+(ZSizeGemModule-ZSizeReadoutModule), ForwardZAxisEDrift);
    }
    ReadoutModules[6]->SetDeadZone(XPosition+0, XPosition+BeamHoleRadius, YPosition-BeamHoleRadius, YPosition+0);

    if(StationNumber%2 == 1) {
        ReadoutModules[7] =
            new BmnGemStripReadoutModule(XRectHotZoneSize_Station[StationNumber], YRectHotZoneSize_Station[StationNumber], XPosition-XRectHotZoneSize_Station[StationNumber], YPosition-YRectHotZoneSize_Station[StationNumber], PitchValueModule, -StripAngle, LowerStripWidth, UpperStripWidth, ZPosition+(ZSizeGemModule-ZSizeReadoutModule), ForwardZAxisEDrift);
    }
    else {
        ReadoutModules[7] =
            new BmnGemStripReadoutModule(XSlopeHotZoneSize_Station[StationNumber][1], YSlopeHotZoneSize_Station[StationNumber], XPosition-XSlopeHotZoneSize_Station[StationNumber][1], YPosition-YSlopeHotZoneSize_Station[StationNumber], PitchValueModule, -StripAngle, LowerStripWidth, UpperStripWidth, ZPosition+(ZSizeGemModule-ZSizeReadoutModule), ForwardZAxisEDrift);
    }
    ReadoutModules[7]->SetDeadZone(XPosition-BeamHoleRadius, XPosition+0, YPosition-BeamHoleRadius, YPosition+0);
}

//------------------------------------------------------------------------------

//visual test
void BmnGemStripStation_FullConfig::DrawCreatedStation() {

    //if(StationNumber != 0) return;

    TCanvas *station_canv = new TCanvas("station_canv", "station_canv", 10, 10, 1000, 1000);

    Double_t range_size = 0.0;
    if( XSize > YSize ) range_size = XSize ;
    else range_size = YSize;
    range_size *= 0.5 + 0.05;
    if( abs(XPosition) > abs(YPosition) ) range_size+= abs(XPosition);
    else range_size+= abs(YPosition);

    station_canv->Range(-range_size, -range_size, range_size, range_size);

    TWbox **modules = new TWbox*[NModules];
    TWbox **deadzones = new TWbox*[NModules];

    for(Int_t im = 0; im < NModules; im++) {
        Double_t xmin = ReadoutModules[im]->GetXMinReadout();
        Double_t xmax = ReadoutModules[im]->GetXMaxReadout();
        Double_t ymin = ReadoutModules[im]->GetYMinReadout();
        Double_t ymax = ReadoutModules[im]->GetYMaxReadout();

        Double_t xmin_deadzone = ReadoutModules[im]->GetXMinDeadZone();
        Double_t xmax_deadzone = ReadoutModules[im]->GetXMaxDeadZone();
        Double_t ymin_deadzone = ReadoutModules[im]->GetYMinDeadZone();
        Double_t ymax_deadzone = ReadoutModules[im]->GetYMaxDeadZone();

        modules[im] = new TWbox(xmin, ymin, xmax, ymax, 18, 1, 1);
        modules[im]->Draw();

        deadzones[im] = new TWbox(xmin_deadzone, ymin_deadzone, xmax_deadzone, ymax_deadzone, TColor::GetColor("#800000"), 1, 1);
        deadzones[im]->Draw();
    }
    //center axes
    Double_t xmin_range = station_canv->GetUxmin();
    Double_t xmax_range = station_canv->GetUxmax();
    Double_t ymin_range = station_canv->GetUymin();
    Double_t ymax_range = station_canv->GetUymax();

    TLine *x_center_axis = new TLine(xmin_range, 0.0, xmax_range, 0.0);
    x_center_axis->SetLineColor(TColor::GetColor("#ee0000"));
    x_center_axis->Draw();
    TLine *y_center_axis = new TLine(0.0, ymin_range, 0.0, ymax_range);
    y_center_axis->SetLineColor(TColor::GetColor("#ee0000"));
    y_center_axis->Draw();

    //Draw xy axes
    TGaxis *xaxis = new TGaxis(xmin_range, ymin_range-0.06*ymin_range, xmax_range, ymin_range-0.06*ymin_range, xmin_range, ymax_range);
    xaxis->SetNdivisions(525);
    xaxis->SetLabelSize(0.015);
    xaxis->SetTitleSize(0.015);
    xaxis->SetTitle("x [cm]"); xaxis->CenterTitle();
    xaxis->Draw();

    TGaxis *yaxis = new TGaxis(xmin_range-0.06*xmin_range, ymin_range, xmin_range-0.06*xmin_range, ymax_range, ymin_range, ymax_range);
    yaxis->SetNdivisions(525);
    yaxis->SetLabelSize(0.015);
    yaxis->SetTitleSize(0.015);
    yaxis->SetTitle("y [cm]"); yaxis->CenterTitle();
    yaxis->Draw();

    TString file_name = "/home/diman/Software/test/tmp/station_";
        file_name += StationNumber;
        file_name += "_pos_";
        file_name += "x"; file_name += XPosition; file_name += "_";
        file_name += "y"; file_name += YPosition; file_name += "_";
        file_name += "z"; file_name += ZPosition;
        file_name += ".png";
    station_canv->SaveAs(file_name);

    delete station_canv;
    for(int i = 0; i < NModules; i++) {
        delete modules[i];
        delete deadzones[i];
    }
    delete [] modules;
    delete [] deadzones;
    delete x_center_axis;
    delete y_center_axis;
    delete xaxis;
    delete yaxis;
}

ClassImp(BmnGemStripStation_FullConfig)