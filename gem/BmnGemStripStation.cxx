#include "BmnGemStripStation.h"

BmnGemStripStation::BmnGemStripStation(Int_t iStation,
                       Double_t xsize_gem_module, Double_t ysize_gem_module,
                       Double_t zpos_station, Double_t beamradius) {

    //All linear values must be entered in units of cm
    StationNumber = iStation;

    XSizeGemModule = xsize_gem_module;
    YSizeGemModule = ysize_gem_module;

    ZPosition = zpos_station;

    BeamPipeRadius = beamradius;

    XCenterStation = 0.0;
    YCenterStation = 0.0;

    dXInnerFrame = 1.7;
    dYInnerFrame = 1.7;
    dXOuterFrame = 5.0;
    dYOuterFrame = 3.75;

    ZSizeGemModule = 2.5;
    ZSizeReadoutModule = 1.0;

    if(iStation >= 0 && iStation <=2) {
        PitchValueBigModule = 0.04;
        PitchValueSmallModule = 0.04;
        LowerStripWidth = 0.034;
        UpperStripWidth = 0.008;
    }
    if(iStation >= 3 && iStation <=11) {
        PitchValueBigModule = 0.08;
        PitchValueSmallModule = 0.08;
        LowerStripWidth = 0.068;
        UpperStripWidth = 0.016;
    }

    if(iStation%2 == 0)
        StripAngle = -15.0; // deg (negative value - clockwise direction, positive - anticlockwise)
    else
        StripAngle = 15.0;

    NBigReadoutModules = 4;
    NSmallReadoutModules = 4;

    XSizeStation = 2*XSizeGemModule + 2*BeamPipeRadius;
    YSizeStation = 2*YSizeGemModule + 2*BeamPipeRadius;
    ZSizeStation = ZSizeGemModule*2;

    XSizeBigReadoutModule = XSizeGemModule - (dXInnerFrame + dXOuterFrame);
    YSizeBigReadoutModule = YSizeGemModule - (dYInnerFrame + dYOuterFrame);

//Sizes of small modules depend on number of a current station
    //XSizeSmallReadoutModule = XSizeBigReadoutModule/2;
    //YSizeSmallReadoutModule = YSizeBigReadoutModule/2;
    XSizeSmallReadoutModule[0] = 12.0;
    XSizeSmallReadoutModule[1] = 12.0;
    XSizeSmallReadoutModule[2] = 12.0;
    XSizeSmallReadoutModule[3] = 15.0;
    XSizeSmallReadoutModule[4] = 40.0;
    XSizeSmallReadoutModule[5] = 40.0;
    XSizeSmallReadoutModule[6] = 60.0;
    XSizeSmallReadoutModule[7] = 60.0;
    XSizeSmallReadoutModule[8] = 60.0;
    XSizeSmallReadoutModule[9] = 60.0;
    XSizeSmallReadoutModule[10] = 60.0;
    XSizeSmallReadoutModule[11] = 60.0;

    YSizeSmallReadoutModule[0] = 8.0;
    YSizeSmallReadoutModule[1] = 10.0;
    YSizeSmallReadoutModule[2] = 12.0;
    YSizeSmallReadoutModule[3] = 10.0;
    YSizeSmallReadoutModule[4] = 15.0;
    YSizeSmallReadoutModule[5] = 20.0;
    YSizeSmallReadoutModule[6] = 20.0;
    YSizeSmallReadoutModule[7] = 20.0;
    YSizeSmallReadoutModule[8] = 20.0;
    YSizeSmallReadoutModule[9] = 20.0;
    YSizeSmallReadoutModule[10] = 20.0;
    YSizeSmallReadoutModule[11] = 20.0;

    //Module_0 (big left-botton module)
    XMin_ReadoutModule[0] = XCenterStation - (BeamPipeRadius + dXInnerFrame + XSizeBigReadoutModule);
    XMax_ReadoutModule[0] = XCenterStation - (BeamPipeRadius + dXInnerFrame);
    YMin_ReadoutModule[0] = YCenterStation + (BeamPipeRadius + dYInnerFrame) - YSizeBigReadoutModule;
    YMax_ReadoutModule[0] = YCenterStation + (BeamPipeRadius + dYInnerFrame);

    //Module_1 (big right-botton module)
    XMin_ReadoutModule[1] = XCenterStation - (BeamPipeRadius + dXInnerFrame);
    XMax_ReadoutModule[1] = XCenterStation - (BeamPipeRadius + dXInnerFrame) + XSizeBigReadoutModule;
    YMin_ReadoutModule[1] = YCenterStation - (BeamPipeRadius + dYInnerFrame + YSizeBigReadoutModule);
    YMax_ReadoutModule[1] = YCenterStation - (BeamPipeRadius + dYInnerFrame);

    //Module_2  (big right-top module)
    XMin_ReadoutModule[2] = XCenterStation + (BeamPipeRadius + dXInnerFrame);
    XMax_ReadoutModule[2] = XCenterStation + (BeamPipeRadius + dXInnerFrame + XSizeBigReadoutModule);
    YMin_ReadoutModule[2] = YCenterStation - (BeamPipeRadius + dYInnerFrame);
    YMax_ReadoutModule[2] = YCenterStation - (BeamPipeRadius + dYInnerFrame) + YSizeBigReadoutModule;

    //Module_3  (big left-top module)
    XMin_ReadoutModule[3] = XCenterStation + (BeamPipeRadius + dXInnerFrame) - XSizeBigReadoutModule;
    XMax_ReadoutModule[3] = XCenterStation + (BeamPipeRadius + dXInnerFrame);
    YMin_ReadoutModule[3] = YCenterStation + (BeamPipeRadius + dYInnerFrame);
    YMax_ReadoutModule[3] = YCenterStation + (BeamPipeRadius + dYInnerFrame + YSizeBigReadoutModule);

    //Module_4 (small left-botton module)
    XMin_ReadoutModule[4] = XMax_ReadoutModule[0] - XSizeSmallReadoutModule[iStation];
    XMax_ReadoutModule[4] = XMax_ReadoutModule[0];
    YMin_ReadoutModule[4] = YMax_ReadoutModule[0] - YSizeSmallReadoutModule[iStation];
    YMax_ReadoutModule[4] = YMax_ReadoutModule[0];

    //Module_5 (small right-botton module)
    XMin_ReadoutModule[5] = XMin_ReadoutModule[1];
    XMax_ReadoutModule[5] = XMin_ReadoutModule[1] + XSizeSmallReadoutModule[iStation];
    YMin_ReadoutModule[5] = YMax_ReadoutModule[1] - YSizeSmallReadoutModule[iStation];
    YMax_ReadoutModule[5] = YMax_ReadoutModule[1];

    //Module_6  (small right-top module)
    XMin_ReadoutModule[6] = XMin_ReadoutModule[2];
    XMax_ReadoutModule[6] = XMin_ReadoutModule[2] + XSizeSmallReadoutModule[iStation];
    YMin_ReadoutModule[6] = YMin_ReadoutModule[2];
    YMax_ReadoutModule[6] = YMin_ReadoutModule[2] + YSizeSmallReadoutModule[iStation];

    //Module_7  (small left-top module)
    XMin_ReadoutModule[7] = XMax_ReadoutModule[3] - XSizeSmallReadoutModule[iStation];
    XMax_ReadoutModule[7] = XMax_ReadoutModule[3];
    YMin_ReadoutModule[7] = YMin_ReadoutModule[3];
    YMax_ReadoutModule[7] = YMin_ReadoutModule[3] + YSizeSmallReadoutModule[iStation];

/* Create Readout modules */
    Double_t displ = ZPosition + (ZSizeGemModule-ZSizeReadoutModule); //displacement of active volume in GEM module

//Big readout modules
    ReadoutModules[0] =
        new BmnGemStripReadoutModule(XSizeBigReadoutModule, YSizeBigReadoutModule, XMin_ReadoutModule[0], YMin_ReadoutModule[0], PitchValueBigModule, StripAngle, LowerStripWidth, UpperStripWidth, displ);
    ReadoutModules[0]->SetDeadZone(XMin_ReadoutModule[4], XMax_ReadoutModule[4], YMin_ReadoutModule[4], YMax_ReadoutModule[4]);

    ReadoutModules[1] =
        new BmnGemStripReadoutModule(XSizeBigReadoutModule, YSizeBigReadoutModule, XMin_ReadoutModule[1], YMin_ReadoutModule[1], PitchValueBigModule, -StripAngle, LowerStripWidth, UpperStripWidth, displ+ZSizeReadoutModule);
    ReadoutModules[1]->SetDeadZone(XMin_ReadoutModule[5], XMax_ReadoutModule[5], YMin_ReadoutModule[5], YMax_ReadoutModule[5]);

    ReadoutModules[2] =
        new BmnGemStripReadoutModule(XSizeBigReadoutModule, YSizeBigReadoutModule, XMin_ReadoutModule[2], YMin_ReadoutModule[2], PitchValueBigModule, StripAngle, LowerStripWidth, UpperStripWidth, displ);
    ReadoutModules[2]->SetDeadZone(XMin_ReadoutModule[6], XMax_ReadoutModule[6], YMin_ReadoutModule[6], YMax_ReadoutModule[6]);

    ReadoutModules[3] =
        new BmnGemStripReadoutModule(XSizeBigReadoutModule, YSizeBigReadoutModule, XMin_ReadoutModule[3], YMin_ReadoutModule[3], PitchValueBigModule, -StripAngle, LowerStripWidth, UpperStripWidth, displ+ZSizeReadoutModule);
    ReadoutModules[3]->SetDeadZone(XMin_ReadoutModule[7], XMax_ReadoutModule[7], YMin_ReadoutModule[7], YMax_ReadoutModule[7]);

//Small readout modules (inners))
    ReadoutModules[4] =
        new BmnGemStripReadoutModule(XSizeSmallReadoutModule[iStation], YSizeSmallReadoutModule[iStation], XMin_ReadoutModule[4], YMin_ReadoutModule[4], PitchValueSmallModule, StripAngle, LowerStripWidth, UpperStripWidth, displ);

    ReadoutModules[5] =
        new BmnGemStripReadoutModule(XSizeSmallReadoutModule[iStation], YSizeSmallReadoutModule[iStation], XMin_ReadoutModule[5], YMin_ReadoutModule[5], PitchValueSmallModule, -StripAngle, LowerStripWidth, UpperStripWidth, displ+ZSizeReadoutModule);

    ReadoutModules[6] =
        new BmnGemStripReadoutModule(XSizeSmallReadoutModule[iStation], YSizeSmallReadoutModule[iStation], XMin_ReadoutModule[6], YMin_ReadoutModule[6], PitchValueSmallModule, StripAngle, LowerStripWidth, UpperStripWidth, displ);

    ReadoutModules[7] =
        new BmnGemStripReadoutModule(XSizeSmallReadoutModule[iStation], YSizeSmallReadoutModule[iStation], XMin_ReadoutModule[7], YMin_ReadoutModule[7], PitchValueSmallModule, -StripAngle, LowerStripWidth, UpperStripWidth, displ+ZSizeReadoutModule);

}

BmnGemStripStation::~BmnGemStripStation() {
    for(UInt_t i = 0; i < 8; i++) {
        if(ReadoutModules[i]) delete ReadoutModules[i];
    }
}

Bool_t BmnGemStripStation::CheckPointModuleOwnership(Double_t xcoord, Double_t ycoord, Int_t readout_module) {
    //Info: readout_module can be from 0 to 7, where 0-3 - indices of big modules, 4-7 - small (inner) modules
    if(readout_module >= 0 && readout_module <= 7) {
        if((xcoord >= XMin_ReadoutModule[readout_module]) && (xcoord <= XMax_ReadoutModule[readout_module])
        && (ycoord >= YMin_ReadoutModule[readout_module]) && (ycoord <= YMax_ReadoutModule[readout_module]))
            return true;
        else return false;
    }
    return false;
}

Int_t BmnGemStripStation::GetPointModuleOwhership(Double_t xcoord, Double_t ycoord) {
    for(Int_t iModule = 7; iModule >= 0; iModule--) {
            if(CheckPointModuleOwnership(xcoord, ycoord, iModule)) return iModule;
            //if(ReadoutModules[iModule]->AddRealPoint(xcoord, ycoord, zcoord)) return iModule;
        }
    return -1;
}

Int_t BmnGemStripStation::AddPointToStation(Double_t xcoord, Double_t ycoord, Double_t zcoord, Double_t dEloss) {
        for(Int_t iModule = 7; iModule >= 0; iModule--) {
            if(CheckPointModuleOwnership(xcoord, ycoord, iModule)) {
                //if(ReadoutModules[iModule]->AddRealPoint(xcoord, ycoord, zcoord)) return iModule;
                if(ReadoutModules[iModule]->AddRealPointFullOne(xcoord, ycoord, zcoord, dEloss)) return iModule;
                else return -1;
            }
        }
    return -1;
}

Int_t BmnGemStripStation::CountNAddedToStationPoints() {
    Int_t points_sum = 0;
    for(Int_t iModule = 0; iModule < (NBigReadoutModules+NSmallReadoutModules); iModule++) {
        points_sum += ReadoutModules[iModule]->GetNRealPoints();
    }
    return points_sum;
}

void BmnGemStripStation::ProcessPointsInStation() {
    for(Int_t iModule = 0; iModule < (NBigReadoutModules+NSmallReadoutModules); iModule++) {
        //ReadoutModules[iModule]->CalculateBorderIntersectionPoints();
        //ReadoutModules[iModule]->FindClusterHitsInReadoutPlane();
        ReadoutModules[iModule]->CalculateStripHitIntersectionPoints();
    }
}

Int_t BmnGemStripStation::CountNProcessedPointInStation() {
    Int_t points_sum = 0;
    for(Int_t iModule = 0; iModule < (NBigReadoutModules+NSmallReadoutModules); iModule++) {
        points_sum += ReadoutModules[iModule]->GetNIntersectionPoints();
    }
    return points_sum;
}

BmnGemStripReadoutModule* BmnGemStripStation::GetReadoutModule(Int_t index) {
    if(index >= 0 && index < (NBigReadoutModules+NSmallReadoutModules)) {
        return ReadoutModules[index];
    }
    return 0;
}

ClassImp(BmnGemStripStation)
