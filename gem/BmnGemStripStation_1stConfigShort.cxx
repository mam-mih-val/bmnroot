#include "BmnGemStripStation_1stConfigShort.h"

BmnGemStripStation_1stConfigShort::BmnGemStripStation_1stConfigShort(Int_t iStation,
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

    if( iStation >= 0 && iStation <= 4 ) {
        PitchValueModule = 0.08;
        LowerStripWidth = 0.068;
        UpperStripWidth = 0.016;
    }

    switch (iStation) {
        case 0:
            StripAngle = 15.0; break; //one plane 66x41
        case 1:
            StripAngle = -15.0; break; //one plane 66x41
        case 2:
            StripAngle = 15.0; break; //one plane 66x41
        case 3:
            StripAngle = -15.0; break; //one plane 66x41
        case 4:
            StripAngle = 15.0; break; //one plane 163x45
    }

    //SIZES

    //station 0-3 (plane 66x41)
    XModuleSize_Plane66x41 = 66.0;
    YModuleSize_Plane66x41 = 41.0;

    XHotZoneSize_Plane66x41 = 15.0;
    YHotZoneSize_Plane66x41 = 10.0;

    //Assembling a station -----------------------------------------------------
    if(StationNumber >= 0 && StationNumber <= 4) {

        NModules = 2; //1 modules in the station (1 plane + 1 hot zone)
        XSize = XModuleSize_Plane66x41;
        YSize = YModuleSize_Plane66x41;
        ZSize = ZSizeGemModule;

        BuildModules_One66x41Plane();
    }
    //end assembling the station -----------------------------------------------
}

BmnGemStripStation_1stConfigShort::~BmnGemStripStation_1stConfigShort() {
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

Int_t BmnGemStripStation_1stConfigShort::GetPointModuleOwnership(Double_t xcoord, Double_t ycoord, Double_t zcoord) {

    if(StationNumber >= 0 && StationNumber <= 4) {
        for(Int_t i = 1; i >= 0; --i) {
            if((xcoord >= ReadoutModules[i]->GetXMinReadout() && xcoord <= ReadoutModules[i]->GetXMaxReadout())
            && (ycoord >= ReadoutModules[i]->GetYMinReadout() && ycoord <= ReadoutModules[i]->GetYMaxReadout()))
                return i;
        }
    }

    return -1;
}

void BmnGemStripStation_1stConfigShort::BuildModules_One66x41Plane() {
    ReadoutModules = new BmnGemStripReadoutModule* [NModules];

    if(StripAngle >= 0.0) {

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
                                         ZPosition, BackwardZAxisEDrift);

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
                                         ZPosition, BackwardZAxisEDrift);
    }
}

ClassImp(BmnGemStripStation_1stConfigShort)