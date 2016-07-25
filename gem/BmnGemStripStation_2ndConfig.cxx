#include "BmnGemStripStation_2ndConfig.h"

BmnGemStripStation_2ndConfig::BmnGemStripStation_2ndConfig(Int_t iStation,
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

    if( iStation >= 0 && iStation <= 6 ) {
        PitchValueModule = 0.08;
        LowerStripWidth = 0.068;
        UpperStripWidth = 0.016;
    }

    switch (iStation) {
        case 0:
            StripAngle = 15.0; break; //one plane 66x41
        case 1:
            StripAngle = -15.0; break; //two planes 66x41 with intersections
        case 2:
            StripAngle = 15.0; break; //two planes 66x41 with intersections
        case 3:
            StripAngle = 15.0; break; //one plane 163x45
        case 4:
            StripAngle = -15.0; break; //one plane 163x45
        case 5:
            StripAngle = 15.0; break; //one plane 163x45
        case 6:
        StripAngle = -15.0; break; //one plane 163x45
    }

    //SIZES

    //station 0-3 (plane 66x41)
    XModuleSize_Plane66x41 = 66.0;
    YModuleSize_Plane66x41 = 41.0;

    XHotZoneSize_Plane66x41 = 15.0;
    YHotZoneSize_Plane66x41 = 10.0;

    XIntersectionSize_Plane66x41 = 5.0;

    //station 4-5
    XModuleSize_Plane163x45 = 163.2*0.5;
    YModuleSize_Plane163x45 = 45.0;

    XRectHotZoneSize_Plane163x45 = 40.0;
    YRectHotZoneSize_Plane163x45 = 15.0;

    XSlopeHotZoneSize_Plane163x45[0] = 37.99;
    XSlopeHotZoneSize_Plane163x45[1] = 42.01;

    YSlopeHotZoneSize_Plane163x45 = 15.0;

    //Assembling a station -----------------------------------------------------
    if(StationNumber == 0) {

        NModules = 2; //1 modules in the station (1 plane + 1 hot zone)
        XSize = XModuleSize_Plane66x41;
        YSize = YModuleSize_Plane66x41;
        ZSize = ZSizeGemModule;

        BuildModules_One66x41Plane();
    }

    if(StationNumber >= 1 && StationNumber <= 2) {
        NModules = 4; //2 modules in the station (2 plane + 2 hot zone)
        XSize = 2*XModuleSize_Plane66x41 - XIntersectionSize_Plane66x41;
        YSize = YModuleSize_Plane66x41;
        ZSize = 2*ZSizeGemModule;

        BuildModules_TwoIntersec66x41Plane();
    }

    if(StationNumber >= 3 && StationNumber <= 6) {
        NModules = 4; //2 modules in the station (2 plane + 2 hot zone)
        XSize = 2*XModuleSize_Plane163x45;
        YSize = YModuleSize_Plane163x45;
        ZSize = ZSizeGemModule;

        BuildModules_163x45Plane();
    }
    //end assembling the station -----------------------------------------------
}

BmnGemStripStation_2ndConfig::~BmnGemStripStation_2ndConfig() {
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

Int_t BmnGemStripStation_2ndConfig::GetPointModuleOwnership(Double_t xcoord, Double_t ycoord, Double_t zcoord) {

    if(StationNumber == 0) {
        for(Int_t i = 1; i >= 0; --i) {
            if((xcoord >= ReadoutModules[i]->GetXMinReadout() && xcoord <= ReadoutModules[i]->GetXMaxReadout())
            && (ycoord >= ReadoutModules[i]->GetYMinReadout() && ycoord <= ReadoutModules[i]->GetYMaxReadout()))
                return i;
        }
    }

    if(StationNumber == 1) {
        Int_t mod = -1;
        if(zcoord < ZPosition+ZSizeReadoutModule) {
            mod = 3;
            if((xcoord >= ReadoutModules[mod]->GetXMinReadout() && xcoord <= ReadoutModules[mod]->GetXMaxReadout())
            && (ycoord >= ReadoutModules[mod]->GetYMinReadout() && ycoord <= ReadoutModules[mod]->GetYMaxReadout()))
                return mod;

            mod = 1;
            if((xcoord >= ReadoutModules[mod]->GetXMinReadout() && xcoord <= ReadoutModules[mod]->GetXMaxReadout())
            && (ycoord >= ReadoutModules[mod]->GetYMinReadout() && ycoord <= ReadoutModules[mod]->GetYMaxReadout()))
                return mod;
        }
        else {
            mod = 2;
            if((xcoord >= ReadoutModules[mod]->GetXMinReadout() && xcoord <= ReadoutModules[mod]->GetXMaxReadout())
            && (ycoord >= ReadoutModules[mod]->GetYMinReadout() && ycoord <= ReadoutModules[mod]->GetYMaxReadout()))
                return mod;

            mod = 0;
            if((xcoord >= ReadoutModules[mod]->GetXMinReadout() && xcoord <= ReadoutModules[mod]->GetXMaxReadout())
            && (ycoord >= ReadoutModules[mod]->GetYMinReadout() && ycoord <= ReadoutModules[mod]->GetYMaxReadout()))
                return mod;
        }
    }
    if(StationNumber == 2) {
        Int_t mod = -1;
        if(zcoord < ZPosition+ZSizeReadoutModule) {
            mod = 2;
            if((xcoord >= ReadoutModules[mod]->GetXMinReadout() && xcoord <= ReadoutModules[mod]->GetXMaxReadout())
            && (ycoord >= ReadoutModules[mod]->GetYMinReadout() && ycoord <= ReadoutModules[mod]->GetYMaxReadout()))
                return mod;

            mod = 0;
            if((xcoord >= ReadoutModules[mod]->GetXMinReadout() && xcoord <= ReadoutModules[mod]->GetXMaxReadout())
            && (ycoord >= ReadoutModules[mod]->GetYMinReadout() && ycoord <= ReadoutModules[mod]->GetYMaxReadout()))
                return mod;
        }
        else {
            mod = 3;
            if((xcoord >= ReadoutModules[mod]->GetXMinReadout() && xcoord <= ReadoutModules[mod]->GetXMaxReadout())
            && (ycoord >= ReadoutModules[mod]->GetYMinReadout() && ycoord <= ReadoutModules[mod]->GetYMaxReadout()))
                return mod;

            mod = 1;
            if((xcoord >= ReadoutModules[mod]->GetXMinReadout() && xcoord <= ReadoutModules[mod]->GetXMaxReadout())
            && (ycoord >= ReadoutModules[mod]->GetYMinReadout() && ycoord <= ReadoutModules[mod]->GetYMaxReadout()))
                return mod;
        }
    }


    if(StationNumber >= 3 && StationNumber <= 6) {
        for(Int_t i = 3; i >= 0; --i) {
            if((xcoord >= ReadoutModules[i]->GetXMinReadout() && xcoord <= ReadoutModules[i]->GetXMaxReadout())
            && (ycoord >= ReadoutModules[i]->GetYMinReadout() && ycoord <= ReadoutModules[i]->GetYMaxReadout()))
                return i;
        }
    }

    return -1;
}

void BmnGemStripStation_2ndConfig::BuildModules_One66x41Plane() {
    ReadoutModules = new BmnGemStripReadoutModule* [NModules];

    if(StripAngle >= 0.0) {

        //big module
        ReadoutModules[0] =
            new BmnGemStripReadoutModule(XModuleSize_Plane66x41, YModuleSize_Plane66x41,
                                         XPosition-XModuleSize_Plane66x41*0.5, YPosition-YModuleSize_Plane66x41*0.5, PitchValueModule,
                                         StripAngle, LowerStripWidth, UpperStripWidth,
                                         ZPosition, ForwardZAxisEDrift);

        const Int_t NPointsDeadZone = 4;
        Double_t XPointsDeadZone[NPointsDeadZone];
            XPointsDeadZone[0] = XPosition-XModuleSize_Plane66x41*0.5;
            XPointsDeadZone[1] = XPosition-XModuleSize_Plane66x41*0.5;
            XPointsDeadZone[2] = XPosition-XModuleSize_Plane66x41*0.5+XHotZoneSize_Plane66x41;
            XPointsDeadZone[3] = XPosition-XModuleSize_Plane66x41*0.5+XHotZoneSize_Plane66x41;
        Double_t YPointsDeadZone[NPointsDeadZone];
            YPointsDeadZone[0] = YPosition-YModuleSize_Plane66x41*0.5;
            YPointsDeadZone[1] = YPosition-YModuleSize_Plane66x41*0.5+YHotZoneSize_Plane66x41;
            YPointsDeadZone[2] = YPosition-YModuleSize_Plane66x41*0.5+YHotZoneSize_Plane66x41;
            YPointsDeadZone[3] = YPosition-YModuleSize_Plane66x41*0.5;
        ReadoutModules[0]->AddDeadZone(NPointsDeadZone, XPointsDeadZone, YPointsDeadZone);

        //hot zone
        ReadoutModules[1] =
            new BmnGemStripReadoutModule(XHotZoneSize_Plane66x41, YHotZoneSize_Plane66x41,
                                         XPosition-XModuleSize_Plane66x41*0.5, YPosition-YModuleSize_Plane66x41*0.5,
                                         PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth,
                                         ZPosition, ForwardZAxisEDrift);
    }
    else {
        //big module
        ReadoutModules[0] =
            new BmnGemStripReadoutModule(XModuleSize_Plane66x41, YModuleSize_Plane66x41,
                                         XPosition-XModuleSize_Plane66x41*0.5, YPosition-YModuleSize_Plane66x41*0.5,
                                         PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth,
                                         ZPosition, ForwardZAxisEDrift);

        const Int_t NPointsDeadZone = 4;
        Double_t XPointsDeadZone[NPointsDeadZone];
            XPointsDeadZone[0] = XPosition-XModuleSize_Plane66x41*0.5;
            XPointsDeadZone[1] = XPosition-XModuleSize_Plane66x41*0.5;
            XPointsDeadZone[2] = XPosition-XModuleSize_Plane66x41*0.5+XHotZoneSize_Plane66x41;
            XPointsDeadZone[3] = XPosition-XModuleSize_Plane66x41*0.5+XHotZoneSize_Plane66x41;
        Double_t YPointsDeadZone[NPointsDeadZone];
            YPointsDeadZone[0] = YPosition+YModuleSize_Plane66x41*0.5-YHotZoneSize_Plane66x41;
            YPointsDeadZone[1] = YPosition+YModuleSize_Plane66x41*0.5;
            YPointsDeadZone[2] = YPosition+YModuleSize_Plane66x41*0.5;
            YPointsDeadZone[3] = YPosition+YModuleSize_Plane66x41*0.5-YHotZoneSize_Plane66x41;
        ReadoutModules[0]->AddDeadZone(NPointsDeadZone, XPointsDeadZone, YPointsDeadZone);

        //hot zone
        ReadoutModules[1] =
            new BmnGemStripReadoutModule(XHotZoneSize_Plane66x41, YHotZoneSize_Plane66x41,
                                         XPosition-XModuleSize_Plane66x41*0.5, YPosition+YModuleSize_Plane66x41*0.5-YHotZoneSize_Plane66x41,
                                         PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth,
                                         ZPosition, ForwardZAxisEDrift);
    }
}

void BmnGemStripStation_2ndConfig::BuildModules_TwoIntersec66x41Plane() {
    ReadoutModules = new BmnGemStripReadoutModule* [NModules];

    if(StripAngle >= 0.0) {
        //left plane
        ReadoutModules[0] =
            new BmnGemStripReadoutModule(XModuleSize_Plane66x41, YModuleSize_Plane66x41,
                                         XPosition-XModuleSize_Plane66x41+XIntersectionSize_Plane66x41*0.5, YPosition-YModuleSize_Plane66x41*0.5,
                                         PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth,
                                         ZPosition, ForwardZAxisEDrift);

        const Int_t NPointsDeadZone = 4;
        Double_t XPointsDeadZone[NPointsDeadZone];
            XPointsDeadZone[0] = XPosition-XHotZoneSize_Plane66x41+XIntersectionSize_Plane66x41*0.5;
            XPointsDeadZone[1] = XPosition-XHotZoneSize_Plane66x41+XIntersectionSize_Plane66x41*0.5;
            XPointsDeadZone[2] = XPosition+XIntersectionSize_Plane66x41*0.5;
            XPointsDeadZone[3] = XPosition+XIntersectionSize_Plane66x41*0.5;
        Double_t YPointsDeadZone[NPointsDeadZone];
            YPointsDeadZone[0] = YPosition+YModuleSize_Plane66x41*0.5-YHotZoneSize_Plane66x41;
            YPointsDeadZone[1] = YPosition+YModuleSize_Plane66x41*0.5;
            YPointsDeadZone[2] = YPosition+YModuleSize_Plane66x41*0.5;
            YPointsDeadZone[3] = YPosition+YModuleSize_Plane66x41*0.5-YHotZoneSize_Plane66x41;
        ReadoutModules[0]->AddDeadZone(NPointsDeadZone, XPointsDeadZone, YPointsDeadZone);


        //right plane
        ReadoutModules[1] =
            new BmnGemStripReadoutModule(XModuleSize_Plane66x41, YModuleSize_Plane66x41,
                                         XPosition-XIntersectionSize_Plane66x41*0.5, YPosition-YModuleSize_Plane66x41*0.5,
                                         PitchValueModule, -StripAngle, LowerStripWidth, UpperStripWidth,
                                         ZPosition+ZSizeReadoutModule, ForwardZAxisEDrift);

        XPointsDeadZone[0] = XPosition-XIntersectionSize_Plane66x41*0.5;
        XPointsDeadZone[1] = XPosition-XIntersectionSize_Plane66x41*0.5;
        XPointsDeadZone[2] = XPosition+XHotZoneSize_Plane66x41-XIntersectionSize_Plane66x41*0.5;
        XPointsDeadZone[3] = XPosition+XHotZoneSize_Plane66x41-XIntersectionSize_Plane66x41*0.5;

        YPointsDeadZone[0] = YPosition+YModuleSize_Plane66x41*0.5-YHotZoneSize_Plane66x41;
        YPointsDeadZone[1] = YPosition+YModuleSize_Plane66x41*0.5;
        YPointsDeadZone[2] = YPosition+YModuleSize_Plane66x41*0.5;
        YPointsDeadZone[3] = YPosition+YModuleSize_Plane66x41*0.5-YHotZoneSize_Plane66x41;

        ReadoutModules[1]->AddDeadZone(NPointsDeadZone, XPointsDeadZone, YPointsDeadZone);


        //left hot zone
        ReadoutModules[2] =
            new BmnGemStripReadoutModule(XHotZoneSize_Plane66x41, YHotZoneSize_Plane66x41,
                                         XPosition-XHotZoneSize_Plane66x41+XIntersectionSize_Plane66x41*0.5, YPosition+YModuleSize_Plane66x41*0.5-YHotZoneSize_Plane66x41,
                                         PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth,
                                         ZPosition, ForwardZAxisEDrift);

        //right hot zone
        ReadoutModules[3] =
            new BmnGemStripReadoutModule(XHotZoneSize_Plane66x41, YHotZoneSize_Plane66x41,
                                         XPosition-XIntersectionSize_Plane66x41*0.5, YPosition+YModuleSize_Plane66x41*0.5-YHotZoneSize_Plane66x41,
                                         PitchValueModule, -StripAngle, LowerStripWidth, UpperStripWidth,
                                         ZPosition+ZSizeReadoutModule, ForwardZAxisEDrift);
    }
    else {
        //left plane
        ReadoutModules[0] =
            new BmnGemStripReadoutModule(XModuleSize_Plane66x41, YModuleSize_Plane66x41,
                                         XPosition-XModuleSize_Plane66x41+XIntersectionSize_Plane66x41*0.5, YPosition-YModuleSize_Plane66x41*0.5,
                                         PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth,
                                         ZPosition+ZSizeReadoutModule, ForwardZAxisEDrift);

        const Int_t NPointsDeadZone = 4;
        Double_t XPointsDeadZone[NPointsDeadZone];
            XPointsDeadZone[0] = XPosition-XHotZoneSize_Plane66x41+XIntersectionSize_Plane66x41*0.5;
            XPointsDeadZone[1] = XPosition-XHotZoneSize_Plane66x41+XIntersectionSize_Plane66x41*0.5;
            XPointsDeadZone[2] = XPosition+XIntersectionSize_Plane66x41*0.5;
            XPointsDeadZone[3] = XPosition+XIntersectionSize_Plane66x41*0.5;
        Double_t YPointsDeadZone[NPointsDeadZone];
            YPointsDeadZone[0] = YPosition-YModuleSize_Plane66x41*0.5;
            YPointsDeadZone[1] = YPosition-YModuleSize_Plane66x41*0.5+YHotZoneSize_Plane66x41;
            YPointsDeadZone[2] = YPosition-YModuleSize_Plane66x41*0.5+YHotZoneSize_Plane66x41;
            YPointsDeadZone[3] = YPosition-YModuleSize_Plane66x41*0.5;
        ReadoutModules[0]->AddDeadZone(NPointsDeadZone, XPointsDeadZone, YPointsDeadZone);

        //right plane
        ReadoutModules[1] =
            new BmnGemStripReadoutModule(XModuleSize_Plane66x41, YModuleSize_Plane66x41,
                                         XPosition-XIntersectionSize_Plane66x41*0.5, YPosition-YModuleSize_Plane66x41*0.5,
                                         PitchValueModule, -StripAngle, LowerStripWidth, UpperStripWidth,
                                         ZPosition, ForwardZAxisEDrift);

        XPointsDeadZone[0] = XPosition-XIntersectionSize_Plane66x41*0.5;
        XPointsDeadZone[1] = XPosition-XIntersectionSize_Plane66x41*0.5;
        XPointsDeadZone[2] = XPosition+XHotZoneSize_Plane66x41-XIntersectionSize_Plane66x41*0.5;
        XPointsDeadZone[3] = XPosition+XHotZoneSize_Plane66x41-XIntersectionSize_Plane66x41*0.5;

        YPointsDeadZone[0] = YPosition-YModuleSize_Plane66x41*0.5;
        YPointsDeadZone[1] = YPosition-YModuleSize_Plane66x41*0.5+YHotZoneSize_Plane66x41;
        YPointsDeadZone[2] = YPosition-YModuleSize_Plane66x41*0.5+YHotZoneSize_Plane66x41;
        YPointsDeadZone[3] = YPosition-YModuleSize_Plane66x41*0.5;

        ReadoutModules[1]->AddDeadZone(NPointsDeadZone, XPointsDeadZone, YPointsDeadZone);

        //left hot zone
        ReadoutModules[2] =
            new BmnGemStripReadoutModule(XHotZoneSize_Plane66x41, YHotZoneSize_Plane66x41,
                                         XPosition-XHotZoneSize_Plane66x41+XIntersectionSize_Plane66x41*0.5, YPosition-YModuleSize_Plane66x41*0.5,
                                         PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth,
                                         ZPosition+ZSizeReadoutModule, ForwardZAxisEDrift);

        //right hot zone
        ReadoutModules[3] =
            new BmnGemStripReadoutModule(XHotZoneSize_Plane66x41, YHotZoneSize_Plane66x41,
                                         XPosition-XIntersectionSize_Plane66x41*0.5, YPosition-YModuleSize_Plane66x41*0.5,
                                         PitchValueModule, -StripAngle, LowerStripWidth, UpperStripWidth,
                                         ZPosition, ForwardZAxisEDrift);
    }
 }

void BmnGemStripStation_2ndConfig::BuildModules_163x45Plane() {
    ReadoutModules = new BmnGemStripReadoutModule* [NModules];

    //big modules
    //left module
    ReadoutModules[0] =
        new BmnGemStripReadoutModule(XModuleSize_Plane163x45, YModuleSize_Plane163x45,
                                     XPosition-XModuleSize_Plane163x45, YPosition-YModuleSize_Plane163x45*0.5,
                                     PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth,
                                     ZPosition, ForwardZAxisEDrift);

    if(StripAngle >= 0.0) {
        const Int_t NPointsDeadZone = 4;
        Double_t XPointsDeadZone[NPointsDeadZone];
            XPointsDeadZone[0] = XPosition-XSlopeHotZoneSize_Plane163x45[1];
            XPointsDeadZone[1] = XPosition-XSlopeHotZoneSize_Plane163x45[0];
            XPointsDeadZone[2] = XPosition+0.0;
            XPointsDeadZone[3] = XPosition+0.0;
        Double_t YPointsDeadZone[NPointsDeadZone];
            YPointsDeadZone[0] = YPosition-YModuleSize_Plane163x45*0.5;
            YPointsDeadZone[1] = YPosition-YModuleSize_Plane163x45*0.5+YSlopeHotZoneSize_Plane163x45;
            YPointsDeadZone[2] = YPosition-YModuleSize_Plane163x45*0.5+YSlopeHotZoneSize_Plane163x45;
            YPointsDeadZone[3] = YPosition-YModuleSize_Plane163x45*0.5;
        ReadoutModules[0]->AddDeadZone(NPointsDeadZone, XPointsDeadZone, YPointsDeadZone);

    }
    else {
        const Int_t NPointsDeadZone = 4;
        Double_t XPointsDeadZone[NPointsDeadZone];
            XPointsDeadZone[0] = XPosition-XRectHotZoneSize_Plane163x45;
            XPointsDeadZone[1] = XPosition-XRectHotZoneSize_Plane163x45;
            XPointsDeadZone[2] = XPosition+0.0;
            XPointsDeadZone[3] = XPosition+0.0;
        Double_t YPointsDeadZone[NPointsDeadZone];
            YPointsDeadZone[0] = YPosition-YModuleSize_Plane163x45*0.5;
            YPointsDeadZone[1] = YPosition-YModuleSize_Plane163x45*0.5+YRectHotZoneSize_Plane163x45;
            YPointsDeadZone[2] = YPosition-YModuleSize_Plane163x45*0.5+YRectHotZoneSize_Plane163x45;
            YPointsDeadZone[3] = YPosition-YModuleSize_Plane163x45*0.5;
        ReadoutModules[0]->AddDeadZone(NPointsDeadZone, XPointsDeadZone, YPointsDeadZone);
    }

    //right module
    ReadoutModules[1] =
        new BmnGemStripReadoutModule(XModuleSize_Plane163x45, YModuleSize_Plane163x45,
                                     XPosition+0.0, YPosition-YModuleSize_Plane163x45*0.5, PitchValueModule,
                                     StripAngle, LowerStripWidth, UpperStripWidth,
                                     ZPosition, ForwardZAxisEDrift);

    if(StripAngle >= 0.0) {
        const Int_t NPointsDeadZone = 4;
        Double_t XPointsDeadZone[NPointsDeadZone];
            XPointsDeadZone[0] = XPosition+0.0;
            XPointsDeadZone[1] = XPosition+0.0;
            XPointsDeadZone[2] = XPosition+XRectHotZoneSize_Plane163x45;
            XPointsDeadZone[3] = XPosition+XRectHotZoneSize_Plane163x45;
        Double_t YPointsDeadZone[NPointsDeadZone];
            YPointsDeadZone[0] = YPosition-YModuleSize_Plane163x45*0.5;
            YPointsDeadZone[1] = YPosition-YModuleSize_Plane163x45*0.5+YRectHotZoneSize_Plane163x45;
            YPointsDeadZone[2] = YPosition-YModuleSize_Plane163x45*0.5+YRectHotZoneSize_Plane163x45;
            YPointsDeadZone[3] = YPosition-YModuleSize_Plane163x45*0.5;
        ReadoutModules[1]->AddDeadZone(NPointsDeadZone, XPointsDeadZone, YPointsDeadZone);

    }
    else {
       const Int_t NPointsDeadZone = 4;
        Double_t XPointsDeadZone[NPointsDeadZone];
            XPointsDeadZone[0] = XPosition+0.0;
            XPointsDeadZone[1] = XPosition+0.0;
            XPointsDeadZone[2] = XPosition+XSlopeHotZoneSize_Plane163x45[0];
            XPointsDeadZone[3] = XPosition+XSlopeHotZoneSize_Plane163x45[1];
        Double_t YPointsDeadZone[NPointsDeadZone];
            YPointsDeadZone[0] = YPosition-YModuleSize_Plane163x45*0.5;
            YPointsDeadZone[1] = YPosition-YModuleSize_Plane163x45*0.5+YSlopeHotZoneSize_Plane163x45;
            YPointsDeadZone[2] = YPosition-YModuleSize_Plane163x45*0.5+YSlopeHotZoneSize_Plane163x45;
            YPointsDeadZone[3] = YPosition-YModuleSize_Plane163x45*0.5;
        ReadoutModules[1]->AddDeadZone(NPointsDeadZone, XPointsDeadZone, YPointsDeadZone);
    }

    //hot zones
    //left hot zone
    if(StripAngle >= 0.0) {
        ReadoutModules[2] =
            new BmnGemStripReadoutModule(XSlopeHotZoneSize_Plane163x45[1], YSlopeHotZoneSize_Plane163x45,
                                         XPosition-XSlopeHotZoneSize_Plane163x45[1], YPosition-YModuleSize_Plane163x45*0.5,
                                         PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth,
                                         ZPosition, ForwardZAxisEDrift);
    }
    else {
        ReadoutModules[2] =
            new BmnGemStripReadoutModule(XRectHotZoneSize_Plane163x45, YRectHotZoneSize_Plane163x45,
                                         XPosition-XRectHotZoneSize_Plane163x45, YPosition-YModuleSize_Plane163x45*0.5,
                                         PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth,
                                         ZPosition, ForwardZAxisEDrift);
    }

    const Int_t NPointsDeadZone = 4;
    Double_t XPointsDeadZone[NPointsDeadZone];
        XPointsDeadZone[0] = XPosition-BeamHoleRadius;
        XPointsDeadZone[1] = XPosition-BeamHoleRadius;
        XPointsDeadZone[2] = XPosition+0.0;
        XPointsDeadZone[3] = XPosition+0.0;
    Double_t YPointsDeadZone[NPointsDeadZone];
        YPointsDeadZone[0] = YPosition-YModuleSize_Plane163x45*0.5;
        YPointsDeadZone[1] = YPosition-YModuleSize_Plane163x45*0.5+BeamHoleRadius;
        YPointsDeadZone[2] = YPosition-YModuleSize_Plane163x45*0.5+BeamHoleRadius;
        YPointsDeadZone[3] = YPosition-YModuleSize_Plane163x45*0.5;
    ReadoutModules[2]->AddDeadZone(NPointsDeadZone, XPointsDeadZone, YPointsDeadZone);


    //right hot zone
    if(StripAngle >= 0.0) {
        ReadoutModules[3] =
            new BmnGemStripReadoutModule(XRectHotZoneSize_Plane163x45, YRectHotZoneSize_Plane163x45,
                                         XPosition+0.0, YPosition-YModuleSize_Plane163x45*0.5,
                                         PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth,
                                         ZPosition, ForwardZAxisEDrift);
    }
    else {
        ReadoutModules[3] =
            new BmnGemStripReadoutModule(XSlopeHotZoneSize_Plane163x45[1], YSlopeHotZoneSize_Plane163x45,
                                         XPosition+0.0, YPosition-YModuleSize_Plane163x45*0.5, PitchValueModule,
                                         StripAngle, LowerStripWidth, UpperStripWidth,
                                         ZPosition, ForwardZAxisEDrift);
    }

    XPointsDeadZone[0] = XPosition+0.0;
    XPointsDeadZone[1] = XPosition+0.0;
    XPointsDeadZone[2] = XPosition+BeamHoleRadius;
    XPointsDeadZone[3] = XPosition+BeamHoleRadius;

    YPointsDeadZone[0] = YPosition-YModuleSize_Plane163x45*0.5;
    YPointsDeadZone[1] = YPosition-YModuleSize_Plane163x45*0.5+BeamHoleRadius;
    YPointsDeadZone[2] = YPosition-YModuleSize_Plane163x45*0.5+BeamHoleRadius;
    YPointsDeadZone[3] = YPosition-YModuleSize_Plane163x45*0.5;

    ReadoutModules[3]->AddDeadZone(NPointsDeadZone, XPointsDeadZone, YPointsDeadZone);
}

ClassImp(BmnGemStripStation_2ndConfig)