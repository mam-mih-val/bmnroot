#include "BmnGemStripStation_RunSummer2016.h"

BmnGemStripStation_RunSummer2016::BmnGemStripStation_RunSummer2016(Int_t iStation,
                       Double_t xpos_station, Double_t ypos_station, Double_t zpos_station,
                       Double_t beamradius) {

    //All linear values must be entered in units of cm

    StationNumber = iStation;

    //Station shift position
    XPosition = xpos_station;
    YPosition = ypos_station;
    ZPosition = zpos_station;

    BeamHoleRadius = beamradius;

    //frames (if we have need for it)
    dXInnerFrame = 1.7;
    dYInnerFrame = 1.7;
    dXOuterFrame = 5.0;
    dYOuterFrame = 3.75;

    //Parameters for each station
        //ZSizeGemModule : GEM thickness (frames+sensitive)
        //ZSizeReadoutModule : sensitive volume thickness
        //XSize : x-size of the station
        //YSize : y-size of the station
        //ZSize : z-size of the station

    switch (iStation) {
        case 0: //one plane 10x10
            StripAngle = +90.0;
            PitchValueModule = 0.04;
            LowerStripWidth = PitchValueModule;
            UpperStripWidth = PitchValueModule;
            ZSizeGemModule = 1.3;
            ZSizeReadoutModule = 0.9;
            EDriftDirection = ForwardZAxisEDrift;
            LowerStripDirection = RightToLeft;
            UpperStripDirection = LeftToRight;
            LeftStripBorder = LeftTop;
            RightStripBorder = RightBottom;
            break;

        case 1: //one plane 66x41
            StripAngle = +15.0;
            PitchValueModule = 0.08;
            LowerStripWidth = PitchValueModule;
            UpperStripWidth = PitchValueModule;
            ZSizeGemModule = 2.3;
            ZSizeReadoutModule = 0.9;
            EDriftDirection = ForwardZAxisEDrift;
            LowerStripDirection = LeftToRight;
            UpperStripDirection = LeftToRight;
            LeftStripBorder = LeftTop;
            RightStripBorder = RightBottom;
            break;

        case 2: //one plane 66x41
            StripAngle = -15.0;
            PitchValueModule = 0.08;
            LowerStripWidth = PitchValueModule;
            UpperStripWidth = PitchValueModule;
            ZSizeGemModule = 2.3;
            ZSizeReadoutModule = 0.9;
            EDriftDirection = BackwardZAxisEDrift;
            LowerStripDirection = LeftToRight;
            UpperStripDirection = LeftToRight;
            LeftStripBorder = LeftBottom;
            RightStripBorder = RightTop;
            break;

        case 3: //one plane 66x41
            StripAngle = +15.0;
            PitchValueModule = 0.08;
            LowerStripWidth = PitchValueModule;
            UpperStripWidth = PitchValueModule;
            ZSizeGemModule = 2.3;
            ZSizeReadoutModule = 0.9;
            EDriftDirection = ForwardZAxisEDrift;
            LowerStripDirection = LeftToRight;
            UpperStripDirection = LeftToRight;
            LeftStripBorder = LeftTop;
            RightStripBorder = RightBottom;
            break;

        case 4: //one plane 66x41
            StripAngle = -15.0;
            PitchValueModule = 0.08;
            LowerStripWidth = PitchValueModule;
            UpperStripWidth = PitchValueModule;
            ZSizeGemModule = 2.3;
            ZSizeReadoutModule = 0.9;
            EDriftDirection = BackwardZAxisEDrift;
            LowerStripDirection = LeftToRight;
            UpperStripDirection = LeftToRight;
            LeftStripBorder = LeftBottom;
            RightStripBorder = RightTop;
            break;

        case 5: //one plane 66x41
            StripAngle = +15.0;
            PitchValueModule = 0.08;
            LowerStripWidth = PitchValueModule;
            UpperStripWidth = PitchValueModule;
            ZSizeGemModule = 2.3;
            ZSizeReadoutModule = 0.9;
            EDriftDirection = ForwardZAxisEDrift;
            LowerStripDirection = LeftToRight;
            UpperStripDirection = LeftToRight;
            LeftStripBorder = LeftTop;
            RightStripBorder = RightBottom;
            break;

        case 6: //one plane 163x45
            StripAngle = -15.0;
            PitchValueModule = 0.08;
            LowerStripWidth = PitchValueModule;
            UpperStripWidth = PitchValueModule;
            ZSizeGemModule = 3.9;
            ZSizeReadoutModule = 0.9;
            EDriftDirection = BackwardZAxisEDrift;
            LowerStripDirection = RightToLeft;
            UpperStripDirection = RightToLeft;
            LeftStripBorder = LeftBottom;
            RightStripBorder = RightTop;
            break;
    }

    //Planes SIZES

    //station 0 (plane 10x10)
    XModuleSize_Plane10x10 = 10.0;
    YModuleSize_Plane10x10 = 10.0;

    //station 1-5 (plane 66x41)
    XModuleSize_Plane66x41 = 66.0;
    YModuleSize_Plane66x41 = 41.0;

    XHotZoneSize_Plane66x41 = 15.0;
    YHotZoneSize_Plane66x41 = 10.0;

    //station 6 (plane 163x45)
    XModuleSize_Plane163x45 = 163.2*0.5;
    YModuleSize_Plane163x45 = 45.0;

    XRectHotZoneSize_Plane163x45 = 40.0;
    YRectHotZoneSize_Plane163x45 = 15.0;

    XSlopeHotZoneSize_Plane163x45[0] = 37.99;
    XSlopeHotZoneSize_Plane163x45[1] = 42.01;

    YSlopeHotZoneSize_Plane163x45 = 15.0;


    //Assembling a station -----------------------------------------------------
    if(StationNumber == 0) {
        NModules = 1; //1 modules in the station
        XSize = XModuleSize_Plane10x10;
        YSize = YModuleSize_Plane10x10;
        ZSize = ZSizeGemModule;
        BuildModules_One10x10Plane();
    }

    if(StationNumber >= 1 && StationNumber <= 5) {
        NModules = 2; //1 modules in the station (1 plane + 1 hot zone)
        XSize = XModuleSize_Plane66x41;
        YSize = YModuleSize_Plane66x41;
        ZSize = ZSizeGemModule;
        BuildModules_One66x41Plane();
    }

    if(StationNumber == 6) {
        NModules = 4; //2 modules in the station (2 plane + 2 hot zone)
        XSize = 2*XModuleSize_Plane163x45;
        YSize = YModuleSize_Plane163x45;
        ZSize = ZSizeGemModule;
        BuildModules_One163x45Plane();
    }
    //end assembling the station -----------------------------------------------
}

BmnGemStripStation_RunSummer2016::~BmnGemStripStation_RunSummer2016() {
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

Int_t BmnGemStripStation_RunSummer2016::GetPointModuleOwnership(Double_t xcoord, Double_t ycoord, Double_t zcoord) {

    if(StationNumber == 0) {
        Int_t mod = 0;
        if((xcoord >= ReadoutModules[mod]->GetXMinReadout() && xcoord <= ReadoutModules[mod]->GetXMaxReadout())
        && (ycoord >= ReadoutModules[mod]->GetYMinReadout() && ycoord <= ReadoutModules[mod]->GetYMaxReadout()))
            return mod;
    }

    if(StationNumber >= 1 && StationNumber <= 5) {
        for(Int_t i = 1; i >= 0; --i) {
            if((xcoord >= ReadoutModules[i]->GetXMinReadout() && xcoord <= ReadoutModules[i]->GetXMaxReadout())
            && (ycoord >= ReadoutModules[i]->GetYMinReadout() && ycoord <= ReadoutModules[i]->GetYMaxReadout()))
                return i;
        }
    }

    if(StationNumber == 6) {
        for(Int_t i = 3; i >= 0; --i) {
            if((xcoord >= ReadoutModules[i]->GetXMinReadout() && xcoord <= ReadoutModules[i]->GetXMaxReadout())
            && (ycoord >= ReadoutModules[i]->GetYMinReadout() && ycoord <= ReadoutModules[i]->GetYMaxReadout())) {
                if(ReadoutModules[i]->IsPointInsideDeadZones(xcoord, ycoord)) continue;
                    return i;
            }
        }
    }

    return -1;
}

Int_t BmnGemStripStation_RunSummer2016::AddPointToStation(Double_t xcoord, Double_t ycoord, Double_t zcoord,
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

Int_t BmnGemStripStation_RunSummer2016::CountNAddedToStationPoints() {
    Int_t points_sum = 0;
    for(Int_t iModule = 0; iModule < NModules; iModule++) {
        points_sum += ReadoutModules[iModule]->GetNRealPoints();
    }
    return points_sum;
}

void BmnGemStripStation_RunSummer2016::ProcessPointsInStation() {
    for(Int_t iModule = 0; iModule < NModules; iModule++) {
        ReadoutModules[iModule]->CalculateStripHitIntersectionPoints();
    }
}

Int_t BmnGemStripStation_RunSummer2016::CountNProcessedPointInStation() {
    Int_t points_sum = 0;
    for(Int_t iModule = 0; iModule < NModules; iModule++) {
        points_sum += ReadoutModules[iModule]->GetNIntersectionPoints();
    }
    return points_sum;
}

BmnGemStripReadoutModule* BmnGemStripStation_RunSummer2016::GetReadoutModule(Int_t index) {
    if(index >= 0 && index < NModules) {
        return ReadoutModules[index];
    }
    return 0;
}
//------------------------------------------------------------------------------

void BmnGemStripStation_RunSummer2016::BuildModules_One10x10Plane() {
    ReadoutModules = new BmnGemStripReadoutModule* [NModules];

    //module
    ReadoutModules[0] =
            new BmnGemStripReadoutModule(XModuleSize_Plane10x10, YModuleSize_Plane10x10,
                                         XPosition-XModuleSize_Plane10x10*0.5, YPosition-YModuleSize_Plane10x10*0.5,
                                         PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth,
                                         ZPosition, EDriftDirection);

    //strip numbering direction
    ReadoutModules[0]->SetStripNumberingOrder(LowerStripDirection, UpperStripDirection);

    //strip numbering borders
    ReadoutModules[0]->SetStripNumberingBorders(LeftStripBorder, RightStripBorder);

    return;
}

void BmnGemStripStation_RunSummer2016::BuildModules_One66x41Plane() {
    ReadoutModules = new BmnGemStripReadoutModule* [NModules];

    if(StripAngle >= 0.0) {

        //big module
        ReadoutModules[0] =
            new BmnGemStripReadoutModule(XModuleSize_Plane66x41, YModuleSize_Plane66x41,
                                         XPosition-XModuleSize_Plane66x41*0.5, YPosition-YModuleSize_Plane66x41*0.5,
                                         PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth,
                                         ZPosition, EDriftDirection);

        //dead zone
        const Int_t NPointsDeadZone = 4;
        Double_t XPointsDeadZone[NPointsDeadZone];
            XPointsDeadZone[0] = XPosition-XModuleSize_Plane66x41*0.5;
            XPointsDeadZone[1] = XPosition-XModuleSize_Plane66x41*0.5;
            XPointsDeadZone[2] = XPosition-XModuleSize_Plane66x41*0.5 + XHotZoneSize_Plane66x41;
            XPointsDeadZone[3] = XPosition-XModuleSize_Plane66x41*0.5 + XHotZoneSize_Plane66x41;
        Double_t YPointsDeadZone[NPointsDeadZone];
            YPointsDeadZone[0] = YPosition-YModuleSize_Plane66x41*0.5;
            YPointsDeadZone[1] = YPosition-YModuleSize_Plane66x41*0.5 + YHotZoneSize_Plane66x41;
            YPointsDeadZone[2] = YPosition-YModuleSize_Plane66x41*0.5 + YHotZoneSize_Plane66x41;
            YPointsDeadZone[3] = YPosition-YModuleSize_Plane66x41*0.5;
        ReadoutModules[0]->AddDeadZone(NPointsDeadZone, XPointsDeadZone, YPointsDeadZone);

        //strip numbering direction
        ReadoutModules[0]->SetStripNumberingOrder(LowerStripDirection, UpperStripDirection);

        //strip numbering borders
        ReadoutModules[0]->SetStripNumberingBorders(LeftStripBorder, RightStripBorder);


        //hot zone
        ReadoutModules[1] =
            new BmnGemStripReadoutModule(XHotZoneSize_Plane66x41, YHotZoneSize_Plane66x41,
                                         XPosition-XModuleSize_Plane66x41*0.5, YPosition-YModuleSize_Plane66x41*0.5,
                                         PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth,
                                         ZPosition, EDriftDirection);
        //strip numbering direction
        ReadoutModules[1]->SetStripNumberingOrder(LowerStripDirection, UpperStripDirection);

        //strip numbering borders
        ReadoutModules[1]->SetStripNumberingBorders(LeftStripBorder, RightStripBorder);
    }
    else {
        //big module
        ReadoutModules[0] =
            new BmnGemStripReadoutModule(XModuleSize_Plane66x41, YModuleSize_Plane66x41,
                                         XPosition-XModuleSize_Plane66x41*0.5, YPosition-YModuleSize_Plane66x41*0.5,
                                         PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth,
                                         ZPosition, EDriftDirection);

        const Int_t NPointsDeadZone = 4;
        Double_t XPointsDeadZone[NPointsDeadZone];
            XPointsDeadZone[0] = XPosition-XModuleSize_Plane66x41*0.5;
            XPointsDeadZone[1] = XPosition-XModuleSize_Plane66x41*0.5;
            XPointsDeadZone[2] = XPosition-XModuleSize_Plane66x41*0.5 + XHotZoneSize_Plane66x41;
            XPointsDeadZone[3] = XPosition-XModuleSize_Plane66x41*0.5 + XHotZoneSize_Plane66x41;
        Double_t YPointsDeadZone[NPointsDeadZone];
            YPointsDeadZone[0] = YPosition+YModuleSize_Plane66x41*0.5 - YHotZoneSize_Plane66x41;
            YPointsDeadZone[1] = YPosition+YModuleSize_Plane66x41*0.5;
            YPointsDeadZone[2] = YPosition+YModuleSize_Plane66x41*0.5;
            YPointsDeadZone[3] = YPosition+YModuleSize_Plane66x41*0.5 - YHotZoneSize_Plane66x41;
        ReadoutModules[0]->AddDeadZone(NPointsDeadZone, XPointsDeadZone, YPointsDeadZone);

        //strip numbering direction
        ReadoutModules[0]->SetStripNumberingOrder(LowerStripDirection, UpperStripDirection);

        //strip numbering borders
        ReadoutModules[0]->SetStripNumberingBorders(LeftStripBorder, RightStripBorder);


        //hot zone
        ReadoutModules[1] =
            new BmnGemStripReadoutModule(XHotZoneSize_Plane66x41, YHotZoneSize_Plane66x41,
                                         XPosition-XModuleSize_Plane66x41*0.5, YPosition+YModuleSize_Plane66x41*0.5-YHotZoneSize_Plane66x41,
                                         PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth,
                                         ZPosition, EDriftDirection);

        //strip numbering direction
        ReadoutModules[1]->SetStripNumberingOrder(LowerStripDirection, UpperStripDirection);

        //strip numbering borders
        ReadoutModules[1]->SetStripNumberingBorders(LeftStripBorder, RightStripBorder);
    }

    return;
}

void BmnGemStripStation_RunSummer2016::BuildModules_One163x45Plane() {
    ReadoutModules = new BmnGemStripReadoutModule* [NModules];

    //big modules --------------------------------------------------------------
    //left module
    ReadoutModules[0] =
        new BmnGemStripReadoutModule(XModuleSize_Plane163x45, YModuleSize_Plane163x45,
                                     XPosition-XModuleSize_Plane163x45, YPosition-YModuleSize_Plane163x45*0.5,
                                     PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth,
                                     ZPosition, EDriftDirection);

    //dead zone as the hot zone rectangle --------------------------------------
    const Int_t NPointsDeadZone = 4;
    Double_t XPointsDeadZone[NPointsDeadZone];
        XPointsDeadZone[0] = XPosition-XRectHotZoneSize_Plane163x45;
        XPointsDeadZone[1] = XPosition-XRectHotZoneSize_Plane163x45;
        XPointsDeadZone[2] = XPosition+0.0;
        XPointsDeadZone[3] = XPosition+0.0;
    Double_t YPointsDeadZone[NPointsDeadZone];
        YPointsDeadZone[0] = YPosition-YModuleSize_Plane163x45*0.5;
        YPointsDeadZone[1] = YPosition-YModuleSize_Plane163x45*0.5 + YRectHotZoneSize_Plane163x45;
        YPointsDeadZone[2] = YPosition-YModuleSize_Plane163x45*0.5 + YRectHotZoneSize_Plane163x45;
        YPointsDeadZone[3] = YPosition-YModuleSize_Plane163x45*0.5;
    ReadoutModules[0]->AddDeadZone(NPointsDeadZone, XPointsDeadZone, YPointsDeadZone);
    //--------------------------------------------------------------------------


    //strip numbering direction
    ReadoutModules[0]->SetStripNumberingOrder(LowerStripDirection, UpperStripDirection);

    //strip numbering borders
    ReadoutModules[0]->SetStripNumberingBorders(LeftStripBorder, RightStripBorder);

    //right module
    ReadoutModules[1] =
        new BmnGemStripReadoutModule(XModuleSize_Plane163x45, YModuleSize_Plane163x45,
                                     XPosition+0.0, YPosition-YModuleSize_Plane163x45*0.5,
                                     PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth,
                                     ZPosition, EDriftDirection);

    //dead zone as the hot zone rectangle --------------------------------------
    XPointsDeadZone[0] = XPosition+0.0;
    XPointsDeadZone[1] = XPosition+0.0;
    XPointsDeadZone[2] = XPosition+XSlopeHotZoneSize_Plane163x45[0];
    XPointsDeadZone[3] = XPosition+XSlopeHotZoneSize_Plane163x45[1];

    YPointsDeadZone[0] = YPosition-YModuleSize_Plane163x45*0.5;
    YPointsDeadZone[1] = YPosition-YModuleSize_Plane163x45*0.5 + YSlopeHotZoneSize_Plane163x45;
    YPointsDeadZone[2] = YPosition-YModuleSize_Plane163x45*0.5 + YSlopeHotZoneSize_Plane163x45;
    YPointsDeadZone[3] = YPosition-YModuleSize_Plane163x45*0.5;

    ReadoutModules[1]->AddDeadZone(NPointsDeadZone, XPointsDeadZone, YPointsDeadZone);
    //--------------------------------------------------------------------------

    //strip numbering direction
    ReadoutModules[1]->SetStripNumberingOrder(LowerStripDirection, UpperStripDirection);

    //strip numbering borders
    ReadoutModules[1]->SetStripNumberingBorders(LeftStripBorder, RightStripBorder);
    //--------------------------------------------------------------------------

    //hot zones ----------------------------------------------------------------
    //left hot zone
    ReadoutModules[2] =
        new BmnGemStripReadoutModule(XRectHotZoneSize_Plane163x45, YRectHotZoneSize_Plane163x45,
                                     XPosition-XRectHotZoneSize_Plane163x45, YPosition-YModuleSize_Plane163x45*0.5,
                                     PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth,
                                     ZPosition, EDriftDirection);

    //dead zone as the hole ----------------------------------------------------
    XPointsDeadZone[0] = XPosition-BeamHoleRadius;
    XPointsDeadZone[1] = XPosition-BeamHoleRadius;
    XPointsDeadZone[2] = XPosition+0.0;
    XPointsDeadZone[3] = XPosition+0.0;

    YPointsDeadZone[0] = YPosition-YModuleSize_Plane163x45*0.5;
    YPointsDeadZone[1] = YPosition-YModuleSize_Plane163x45*0.5 + BeamHoleRadius;
    YPointsDeadZone[2] = YPosition-YModuleSize_Plane163x45*0.5 + BeamHoleRadius;
    YPointsDeadZone[3] = YPosition-YModuleSize_Plane163x45*0.5;

    ReadoutModules[2]->AddDeadZone(NPointsDeadZone, XPointsDeadZone, YPointsDeadZone);
    //--------------------------------------------------------------------------

    //strip numbering direction
    ReadoutModules[2]->SetStripNumberingOrder(LowerStripDirection, UpperStripDirection);

    //strip numbering borders
    ReadoutModules[2]->SetStripNumberingBorders(LeftStripBorder, RightStripBorder);


    //right hot zone
    ReadoutModules[3] =
        new BmnGemStripReadoutModule(XSlopeHotZoneSize_Plane163x45[1], YSlopeHotZoneSize_Plane163x45,
                                     XPosition+0.0, YPosition-YModuleSize_Plane163x45*0.5,
                                     PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth,
                                     ZPosition, EDriftDirection);

    //dead zone as the hole ----------------------------------------------------
    XPointsDeadZone[0] = XPosition+0.0;
    XPointsDeadZone[1] = XPosition+0.0;
    XPointsDeadZone[2] = XPosition+BeamHoleRadius;
    XPointsDeadZone[3] = XPosition+BeamHoleRadius;

    YPointsDeadZone[0] = YPosition-YModuleSize_Plane163x45*0.5;
    YPointsDeadZone[1] = YPosition-YModuleSize_Plane163x45*0.5 + BeamHoleRadius;
    YPointsDeadZone[2] = YPosition-YModuleSize_Plane163x45*0.5 + BeamHoleRadius;
    YPointsDeadZone[3] = YPosition-YModuleSize_Plane163x45*0.5;

    ReadoutModules[3]->AddDeadZone(NPointsDeadZone, XPointsDeadZone, YPointsDeadZone);
    //--------------------------------------------------------------------------

    //dead zone as the triangle ------------------------------------------------
    XPointsDeadZone[0] = XPosition+XSlopeHotZoneSize_Plane163x45[1];
    XPointsDeadZone[1] = XPosition+XSlopeHotZoneSize_Plane163x45[0];
    XPointsDeadZone[2] = XPosition+XSlopeHotZoneSize_Plane163x45[1];

    YPointsDeadZone[0] = YPosition-YModuleSize_Plane163x45*0.5;
    YPointsDeadZone[1] = YPosition-YModuleSize_Plane163x45*0.5 + YSlopeHotZoneSize_Plane163x45;
    YPointsDeadZone[2] = YPosition-YModuleSize_Plane163x45*0.5 + YSlopeHotZoneSize_Plane163x45;

    ReadoutModules[3]->AddDeadZone(3, XPointsDeadZone, YPointsDeadZone);
    //--------------------------------------------------------------------------

    //strip numbering direction
    ReadoutModules[3]->SetStripNumberingOrder(LowerStripDirection, UpperStripDirection);

    //strip numbering borders
    //ReadoutModules[3]->SetStripNumberingBorders(LeftStripBorder, RightStripBorder);
    ReadoutModules[3]->SetStripNumberingBorders(LeftBottom, RightBottom); //slope zone
    //--------------------------------------------------------------------------

    return;
}

ClassImp(BmnGemStripStation_RunSummer2016)