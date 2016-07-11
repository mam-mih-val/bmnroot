#include "BmnGemStripStation_1stConfig.h"

#include "TWbox.h"
#include "TLine.h"
#include "TGaxis.h"
#include "TSystem.h"

BmnGemStripStation_1stConfig::BmnGemStripStation_1stConfig(Int_t iStation,
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

    if( iStation >= 0 && iStation <= 5 ) {
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
            StripAngle = -15.0; break; //two planes 66x41 with intersections
        case 4:
            StripAngle = 15.0; break; //one plane 163x45
        case 5:
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
    if(StationNumber >= 0 && StationNumber <= 2) {

        NModules = 2; //1 modules in the station (1 plane + 1 hot zone)
        XSize = XModuleSize_Plane66x41;
        YSize = YModuleSize_Plane66x41;
        ZSize = ZSizeGemModule;

        BuildModules_One66x41Plane();
    }

    if(StationNumber == 3) {
        NModules = 4; //2 modules in the station (2 plane + 2 hot zone)
        XSize = 2*XModuleSize_Plane66x41 - XIntersectionSize_Plane66x41;
        YSize = YModuleSize_Plane66x41;
        ZSize = 2*ZSizeGemModule;

        BuildModules_TwoIntersec66x41Plane();
    }

    if(StationNumber >= 4 && StationNumber <= 5) {
        NModules = 4; //2 modules in the station (2 plane + 2 hot zone)
        XSize = 2*XModuleSize_Plane163x45;
        YSize = YModuleSize_Plane163x45;
        ZSize = ZSizeGemModule;

        BuildModules_163x45Plane();
    }
    //end assembling the station -----------------------------------------------

    //visual test
    //DrawCreatedStation();
}

BmnGemStripStation_1stConfig::~BmnGemStripStation_1stConfig() {
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

Int_t BmnGemStripStation_1stConfig::GetPointModuleOwnership(Double_t xcoord, Double_t ycoord, Double_t zcoord) {

    if(StationNumber >= 0 && StationNumber <= 2) {
        for(Int_t i = 1; i >= 0; --i) {
            if((xcoord >= ReadoutModules[i]->GetXMinReadout() && xcoord <= ReadoutModules[i]->GetXMaxReadout())
            && (ycoord >= ReadoutModules[i]->GetYMinReadout() && ycoord <= ReadoutModules[i]->GetYMaxReadout()))
                return i;
        }
    }

    if(StationNumber == 3) {
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

    if(StationNumber >= 4 && StationNumber <= 5) {
        for(Int_t i = 3; i >= 0; --i) {
            if((xcoord >= ReadoutModules[i]->GetXMinReadout() && xcoord <= ReadoutModules[i]->GetXMaxReadout())
            && (ycoord >= ReadoutModules[i]->GetYMinReadout() && ycoord <= ReadoutModules[i]->GetYMaxReadout()))
                return i;
        }
    }

    return -1;
}

Int_t BmnGemStripStation_1stConfig::AddPointToStation(Double_t xcoord, Double_t ycoord, Double_t zcoord,
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

Int_t BmnGemStripStation_1stConfig::CountNAddedToStationPoints() {
    Int_t points_sum = 0;
    for(Int_t iModule = 0; iModule < NModules; iModule++) {
        points_sum += ReadoutModules[iModule]->GetNRealPoints();
    }
    return points_sum;
}

void BmnGemStripStation_1stConfig::ProcessPointsInStation() {
    for(Int_t iModule = 0; iModule < NModules; iModule++) {
        ReadoutModules[iModule]->CalculateStripHitIntersectionPoints();
    }
}

Int_t BmnGemStripStation_1stConfig::CountNProcessedPointInStation() {
    Int_t points_sum = 0;
    for(Int_t iModule = 0; iModule < NModules; iModule++) {
        points_sum += ReadoutModules[iModule]->GetNIntersectionPoints();
    }
    return points_sum;
}

BmnGemStripReadoutModule* BmnGemStripStation_1stConfig::GetReadoutModule(Int_t index) {
    if(index >= 0 && index < NModules) {
        return ReadoutModules[index];
    }
    return 0;
}

void BmnGemStripStation_1stConfig::BuildModules_One66x41Plane() {
    ReadoutModules = new BmnGemStripReadoutModule* [NModules];

    if(StripAngle >= 0.0) {

        //big module
        ReadoutModules[0] =
            new BmnGemStripReadoutModule(XModuleSize_Plane66x41, YModuleSize_Plane66x41, XPosition-XModuleSize_Plane66x41*0.5, YPosition-YModuleSize_Plane66x41*0.5, PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth, ZPosition, ForwardZAxisEDrift);
        ReadoutModules[0]->SetDeadZone(XPosition-XModuleSize_Plane66x41*0.5, XPosition-XModuleSize_Plane66x41*0.5+XHotZoneSize_Plane66x41, YPosition-YModuleSize_Plane66x41*0.5, YPosition-YModuleSize_Plane66x41*0.5+YHotZoneSize_Plane66x41);

        //hot zone
        ReadoutModules[1] =
            new BmnGemStripReadoutModule(XHotZoneSize_Plane66x41, YHotZoneSize_Plane66x41, XPosition-XModuleSize_Plane66x41*0.5, YPosition-YModuleSize_Plane66x41*0.5, PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth, ZPosition, ForwardZAxisEDrift);
    }
    else {
        //big module
        ReadoutModules[0] =
            new BmnGemStripReadoutModule(XModuleSize_Plane66x41, YModuleSize_Plane66x41, XPosition-XModuleSize_Plane66x41*0.5, YPosition-YModuleSize_Plane66x41*0.5, PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth, ZPosition, BackwardZAxisEDrift);
        ReadoutModules[0]->SetDeadZone(XPosition-XModuleSize_Plane66x41*0.5, XPosition-XModuleSize_Plane66x41*0.5+XHotZoneSize_Plane66x41, YPosition+YModuleSize_Plane66x41*0.5-YHotZoneSize_Plane66x41, YPosition+YModuleSize_Plane66x41*0.5);

        //hot zone
        ReadoutModules[1] =
            new BmnGemStripReadoutModule(XHotZoneSize_Plane66x41, YHotZoneSize_Plane66x41, XPosition-XModuleSize_Plane66x41*0.5, YPosition+YModuleSize_Plane66x41*0.5-YHotZoneSize_Plane66x41, PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth, ZPosition, BackwardZAxisEDrift);
    }
}

void BmnGemStripStation_1stConfig::BuildModules_TwoIntersec66x41Plane() {
    ReadoutModules = new BmnGemStripReadoutModule* [NModules];

    //big modules
    //left plane
    ReadoutModules[0] =
        new BmnGemStripReadoutModule(XModuleSize_Plane66x41, YModuleSize_Plane66x41, XPosition-XModuleSize_Plane66x41+XIntersectionSize_Plane66x41*0.5, YPosition-YModuleSize_Plane66x41*0.5, PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth, ZPosition+ZSizeReadoutModule, BackwardZAxisEDrift);
    ReadoutModules[0]->SetDeadZone(XPosition-XHotZoneSize_Plane66x41+XIntersectionSize_Plane66x41*0.5, XPosition+XIntersectionSize_Plane66x41*0.5, YPosition-YModuleSize_Plane66x41*0.5, YPosition-YModuleSize_Plane66x41*0.5+YHotZoneSize_Plane66x41);

    //right plane
    ReadoutModules[1] =
        new BmnGemStripReadoutModule(XModuleSize_Plane66x41, YModuleSize_Plane66x41, XPosition-XIntersectionSize_Plane66x41*0.5, YPosition-YModuleSize_Plane66x41*0.5, PitchValueModule, -StripAngle, LowerStripWidth, UpperStripWidth, ZPosition, ForwardZAxisEDrift);
    ReadoutModules[1]->SetDeadZone(XPosition-XIntersectionSize_Plane66x41*0.5, XPosition+XHotZoneSize_Plane66x41-XIntersectionSize_Plane66x41*0.5, YPosition-YModuleSize_Plane66x41*0.5, YPosition-YModuleSize_Plane66x41*0.5+YHotZoneSize_Plane66x41);

    //hot zones
    //left hot zone
    ReadoutModules[2] =
        new BmnGemStripReadoutModule(XHotZoneSize_Plane66x41, YHotZoneSize_Plane66x41, XPosition-XHotZoneSize_Plane66x41+XIntersectionSize_Plane66x41*0.5, YPosition-YModuleSize_Plane66x41*0.5, PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth, ZPosition+ZSizeReadoutModule, BackwardZAxisEDrift);

    //right hot zone
    ReadoutModules[3] =
        new BmnGemStripReadoutModule(XHotZoneSize_Plane66x41, YHotZoneSize_Plane66x41, XPosition-XIntersectionSize_Plane66x41*0.5, YPosition-YModuleSize_Plane66x41*0.5, PitchValueModule, -StripAngle, LowerStripWidth, UpperStripWidth, ZPosition, ForwardZAxisEDrift);
}

void BmnGemStripStation_1stConfig::BuildModules_163x45Plane() {
    ReadoutModules = new BmnGemStripReadoutModule* [NModules];

    //big modules
    //left module
    ReadoutModules[0] =
        new BmnGemStripReadoutModule(XModuleSize_Plane163x45, YModuleSize_Plane163x45, XPosition-XModuleSize_Plane163x45, YPosition-YModuleSize_Plane163x45*0.5, PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth, ZPosition, ForwardZAxisEDrift);
    if(StationNumber%2 == 0) {
        ReadoutModules[0]->SetDeadZone(XPosition-XSlopeHotZoneSize_Plane163x45[1], XPosition+0, YPosition-YModuleSize_Plane163x45*0.5, YPosition-YModuleSize_Plane163x45*0.5+YSlopeHotZoneSize_Plane163x45);
    }
    else {
        ReadoutModules[0]->SetDeadZone(XPosition-XRectHotZoneSize_Plane163x45, XPosition+0, YPosition-YModuleSize_Plane163x45*0.5, YPosition-YModuleSize_Plane163x45*0.5+YRectHotZoneSize_Plane163x45);
    }

    //right module
    ReadoutModules[1] =
        new BmnGemStripReadoutModule(XModuleSize_Plane163x45, YModuleSize_Plane163x45, XPosition+0, YPosition-YModuleSize_Plane163x45*0.5, PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth, ZPosition, ForwardZAxisEDrift);
    if(StationNumber%2 == 0) {
        ReadoutModules[1]->SetDeadZone(XPosition+0, XPosition+XRectHotZoneSize_Plane163x45, YPosition-YModuleSize_Plane163x45*0.5, YPosition-YModuleSize_Plane163x45*0.5+YRectHotZoneSize_Plane163x45);
    }
    else {
       ReadoutModules[1]->SetDeadZone(XPosition+0, XPosition+XSlopeHotZoneSize_Plane163x45[1], YPosition-YModuleSize_Plane163x45*0.5, YPosition-YModuleSize_Plane163x45*0.5+YSlopeHotZoneSize_Plane163x45);
    }


    //hot zones
    //left hot zone
    if(StationNumber%2 == 0) {
        ReadoutModules[2] =
            new BmnGemStripReadoutModule(XSlopeHotZoneSize_Plane163x45[1], YSlopeHotZoneSize_Plane163x45, XPosition-XSlopeHotZoneSize_Plane163x45[1], YPosition-YModuleSize_Plane163x45*0.5, PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth, ZPosition, ForwardZAxisEDrift);
    }
    else {
        ReadoutModules[2] =
            new BmnGemStripReadoutModule(XRectHotZoneSize_Plane163x45, YRectHotZoneSize_Plane163x45, XPosition-XRectHotZoneSize_Plane163x45, YPosition-YModuleSize_Plane163x45*0.5, PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth, ZPosition, ForwardZAxisEDrift);
    }
    ReadoutModules[2]->SetDeadZone(XPosition-BeamHoleRadius, XPosition+0, YPosition-YModuleSize_Plane163x45*0.5, YPosition-YModuleSize_Plane163x45*0.5+BeamHoleRadius);

    //right hot zone
    if(StationNumber%2 == 0) {
        ReadoutModules[3] =
            new BmnGemStripReadoutModule(XRectHotZoneSize_Plane163x45, YRectHotZoneSize_Plane163x45, XPosition+0, YPosition-YModuleSize_Plane163x45*0.5, PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth, ZPosition, ForwardZAxisEDrift);
    }
    else {
        ReadoutModules[3] =
            new BmnGemStripReadoutModule(XSlopeHotZoneSize_Plane163x45[1], YSlopeHotZoneSize_Plane163x45, XPosition+0, YPosition-YModuleSize_Plane163x45*0.5, PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth, ZPosition, ForwardZAxisEDrift);
    }
    ReadoutModules[3]->SetDeadZone(XPosition+0, XPosition+BeamHoleRadius, YPosition-YModuleSize_Plane163x45*0.5, YPosition-YModuleSize_Plane163x45*0.5+BeamHoleRadius);
}

//------------------------------------------------------------------------------

//visual test
void BmnGemStripStation_1stConfig::DrawCreatedStation() {

    //if(StationNumber != 3) return;

    TString rand_num = gRandom->Uniform(0,10);
    TCanvas *station_canv = new TCanvas("station_canv_"+rand_num, "station_canv", 10, 10, 1000, 1000);

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

ClassImp(BmnGemStripStation_1stConfig)