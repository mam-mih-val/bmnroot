#include "BmnGemStripStation_v3.h"

BmnGemStripStation::BmnGemStripStation(Int_t iStation,
                       Double_t zpos_station, Double_t beamradius) {

    //All linear values must be entered in units of cm
    StationNumber = iStation;
    ZPosition = zpos_station;
    BeamPipeRadius = beamradius;

    //must be changed in accordance with the configuration below
    NExistentModules = 0;
    XSizeStation = 0.0;
    YSizeStation = 0.0;
    ZSizeStation = 0.0;

    XCenterStation = 0.0;
    YCenterStation = 0.0;

    dXInnerFrame = 1.7;
    dYInnerFrame = 1.7;
    dXOuterFrame = 5.0;
    dYOuterFrame = 3.75;

    ZSizeGemModule = 2.5;
    ZSizeReadoutModule = 1.0;

    if(iStation >= 0 && iStation <=3) {
        PitchValueModule = 0.04;
        LowerStripWidth = 0.034;
        UpperStripWidth = 0.008;
    }
    if(iStation >= 4 && iStation <=7) {
        PitchValueModule = 0.08;
        LowerStripWidth = 0.068;
        UpperStripWidth = 0.016;
    }
    if(iStation >= 8 && iStation <=11) {
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

    //initialize module
    for(Int_t i = 0; i < MaxModulesInStation; ++i) { ReadoutModules[i] = 0; }

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

    XModuleSize_Station[5] = XModuleSize_Station[6] = XModuleSize_Station[7] = 163.2/2;
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

        NExistentModules = 16; //16 modules in the station (4 modules for each hot zone)
        XSizeStation = 2*XZoneSizes_Station[StationNumber][0];
        YSizeStation = 2*YZoneSizes_Station[StationNumber][0];
        ZSizeStation = ZSizeGemModule;

        //zone 1 ---------------------------------------------------------------
        //left-top quadrant
        ReadoutModules[0] =
            new BmnGemStripReadoutModule(XZoneSizes_Station[StationNumber][0], YZoneSizes_Station[StationNumber][0], XCenterStation-XZoneSizes_Station[StationNumber][0], YCenterStation+0, PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth, zpos_station+(ZSizeGemModule-ZSizeReadoutModule));
        ReadoutModules[0]->SetDeadZone(XCenterStation-XZoneSizes_Station[StationNumber][1], XCenterStation+0, YCenterStation+0, YCenterStation+YZoneSizes_Station[StationNumber][1]);

        //right-top quadrant
        ReadoutModules[1] =
            new BmnGemStripReadoutModule(XZoneSizes_Station[StationNumber][0], YZoneSizes_Station[StationNumber][0], XCenterStation+0, YCenterStation+0, PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth, zpos_station+(ZSizeGemModule-ZSizeReadoutModule));
        ReadoutModules[1]->SetDeadZone(XCenterStation+0, XCenterStation+XZoneSizes_Station[StationNumber][1], YCenterStation+0, YCenterStation+YZoneSizes_Station[StationNumber][1]);

        //right-bottom quadrant
        ReadoutModules[2] =
            new BmnGemStripReadoutModule(XZoneSizes_Station[StationNumber][0], YZoneSizes_Station[StationNumber][0], XCenterStation+0, YCenterStation-YZoneSizes_Station[StationNumber][0], PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth, zpos_station+(ZSizeGemModule-ZSizeReadoutModule));
        ReadoutModules[2]->SetDeadZone(XCenterStation+0, XCenterStation+XZoneSizes_Station[StationNumber][1], YCenterStation-YZoneSizes_Station[StationNumber][1], YCenterStation+0);

        //left-bottom quadrant
        ReadoutModules[3] =
            new BmnGemStripReadoutModule(XZoneSizes_Station[StationNumber][0], YZoneSizes_Station[StationNumber][0], XCenterStation-XZoneSizes_Station[StationNumber][0], YCenterStation-YZoneSizes_Station[StationNumber][0], PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth, zpos_station+(ZSizeGemModule-ZSizeReadoutModule));
        ReadoutModules[3]->SetDeadZone(XCenterStation-XZoneSizes_Station[StationNumber][1], XCenterStation+0, YCenterStation-YZoneSizes_Station[StationNumber][1], YCenterStation+0);
        //----------------------------------------------------------------------

        //zone 2 ---------------------------------------------------------------
        //left-top quadrant
        ReadoutModules[4] =
            new BmnGemStripReadoutModule(XZoneSizes_Station[StationNumber][1], YZoneSizes_Station[StationNumber][1], XCenterStation-XZoneSizes_Station[StationNumber][1], YCenterStation+0, PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth, zpos_station+(ZSizeGemModule-ZSizeReadoutModule));
        ReadoutModules[4]->SetDeadZone(XCenterStation-XZoneSizes_Station[StationNumber][2], XCenterStation+0, YCenterStation+0, YCenterStation+YZoneSizes_Station[StationNumber][2]);

        //right-top quadrant
        ReadoutModules[5] =
            new BmnGemStripReadoutModule(XZoneSizes_Station[StationNumber][1], YZoneSizes_Station[StationNumber][1], XCenterStation+0, YCenterStation+0, PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth, zpos_station+(ZSizeGemModule-ZSizeReadoutModule));
        ReadoutModules[5]->SetDeadZone(XCenterStation+0, XCenterStation+XZoneSizes_Station[StationNumber][2], YCenterStation+0, YCenterStation+YZoneSizes_Station[StationNumber][2]);

        //right-bottom quadrant
        ReadoutModules[6] =
            new BmnGemStripReadoutModule(XZoneSizes_Station[StationNumber][1], YZoneSizes_Station[StationNumber][1], XCenterStation+0, YCenterStation-YZoneSizes_Station[StationNumber][1], PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth, zpos_station+(ZSizeGemModule-ZSizeReadoutModule));
        ReadoutModules[6]->SetDeadZone(XCenterStation+0, XCenterStation+XZoneSizes_Station[StationNumber][2], YCenterStation-YZoneSizes_Station[StationNumber][2], YCenterStation+0);

        //left-bottom quadrant
        ReadoutModules[7] =
            new BmnGemStripReadoutModule(XZoneSizes_Station[StationNumber][1], YZoneSizes_Station[StationNumber][1], XCenterStation-XZoneSizes_Station[StationNumber][1], YCenterStation-YZoneSizes_Station[StationNumber][1], PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth, zpos_station+(ZSizeGemModule-ZSizeReadoutModule));
        ReadoutModules[7]->SetDeadZone(XCenterStation-XZoneSizes_Station[StationNumber][2], XCenterStation+0, YCenterStation-YZoneSizes_Station[StationNumber][2], YCenterStation+0);
        //----------------------------------------------------------------------

        //zone 3 ---------------------------------------------------------------
        //left-top quadrant
        ReadoutModules[8] =
            new BmnGemStripReadoutModule(XZoneSizes_Station[StationNumber][2], YZoneSizes_Station[StationNumber][2], XCenterStation-XZoneSizes_Station[StationNumber][2], YCenterStation+0, PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth, zpos_station+(ZSizeGemModule-ZSizeReadoutModule));
        ReadoutModules[8]->SetDeadZone(XCenterStation-XZoneSizes_Station[StationNumber][3], XCenterStation+0, YCenterStation+0, YCenterStation+YZoneSizes_Station[StationNumber][3]);

        //right-top quadrant
        ReadoutModules[9] =
            new BmnGemStripReadoutModule(XZoneSizes_Station[StationNumber][2], YZoneSizes_Station[StationNumber][2], XCenterStation+0, YCenterStation+0, PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth, zpos_station+(ZSizeGemModule-ZSizeReadoutModule));
        ReadoutModules[9]->SetDeadZone(XCenterStation+0, XCenterStation+XZoneSizes_Station[StationNumber][3], YCenterStation+0, YCenterStation+YZoneSizes_Station[StationNumber][3]);

        //right-bottom quadrant
        ReadoutModules[10] =
            new BmnGemStripReadoutModule(XZoneSizes_Station[StationNumber][2], YZoneSizes_Station[StationNumber][2], XCenterStation+0, YCenterStation-YZoneSizes_Station[StationNumber][2], PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth, zpos_station+(ZSizeGemModule-ZSizeReadoutModule));
        ReadoutModules[10]->SetDeadZone(XCenterStation+0, XCenterStation+XZoneSizes_Station[StationNumber][3], YCenterStation-YZoneSizes_Station[StationNumber][3], YCenterStation+0);

        //left-bottom quadrant
        ReadoutModules[11] =
            new BmnGemStripReadoutModule(XZoneSizes_Station[StationNumber][2], YZoneSizes_Station[StationNumber][2], XCenterStation-XZoneSizes_Station[StationNumber][2], YCenterStation-YZoneSizes_Station[StationNumber][2], PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth, zpos_station+(ZSizeGemModule-ZSizeReadoutModule));
        ReadoutModules[11]->SetDeadZone(XCenterStation-XZoneSizes_Station[StationNumber][3], XCenterStation+0, YCenterStation-YZoneSizes_Station[StationNumber][3], YCenterStation+0);
        //----------------------------------------------------------------------

        //zone 4---------------------------------------------------------------
        //left-top quadrant
        ReadoutModules[12] =
            new BmnGemStripReadoutModule(XZoneSizes_Station[StationNumber][3], YZoneSizes_Station[StationNumber][3], XCenterStation-XZoneSizes_Station[StationNumber][3], YCenterStation+0, PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth, zpos_station+(ZSizeGemModule-ZSizeReadoutModule));
        ReadoutModules[12]->SetDeadZone(XCenterStation-BeamPipeRadius, XCenterStation+0, YCenterStation+0, YCenterStation+BeamPipeRadius);

        //right-top quadrant
        ReadoutModules[13] =
            new BmnGemStripReadoutModule(XZoneSizes_Station[StationNumber][3], YZoneSizes_Station[StationNumber][3], XCenterStation+0, YCenterStation+0, PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth, zpos_station+(ZSizeGemModule-ZSizeReadoutModule));
        ReadoutModules[13]->SetDeadZone(XCenterStation+0, XCenterStation+BeamPipeRadius, YCenterStation+0, YCenterStation+BeamPipeRadius);

        //right-bottom quadrant
        ReadoutModules[14] =
            new BmnGemStripReadoutModule(XZoneSizes_Station[StationNumber][3], YZoneSizes_Station[StationNumber][3], XCenterStation+0, YCenterStation-YZoneSizes_Station[StationNumber][3], PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth, zpos_station+(ZSizeGemModule-ZSizeReadoutModule));
        ReadoutModules[14]->SetDeadZone(XCenterStation+0, XCenterStation+BeamPipeRadius, YCenterStation-BeamPipeRadius, YCenterStation+0);

        //left-bottom quadrant
        ReadoutModules[15] =
            new BmnGemStripReadoutModule(XZoneSizes_Station[StationNumber][3], YZoneSizes_Station[StationNumber][3], XCenterStation-XZoneSizes_Station[StationNumber][3], YCenterStation-YZoneSizes_Station[StationNumber][3], PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth, zpos_station+(ZSizeGemModule-ZSizeReadoutModule));
        ReadoutModules[15]->SetDeadZone(XCenterStation-BeamPipeRadius, XCenterStation+0, YCenterStation-BeamPipeRadius, YCenterStation+0);
        //----------------------------------------------------------------------

        //visual test ----------------------------------------------------------
        /*
        if(StationNumber >=0  && StationNumber <= 3)
        {
            station_0_3_canv = new TCanvas("station_0_3_canv", "station_0_3_canv", 10, 10, 1000, 1000);
            station_0_3_canv->Range(-50.0, -50.0, 50.0, 50.0);
            TWbox *modules[16];
            TWbox *deadzones[16];

            for(Int_t im = 0; im < 16; im++) {

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
        }
        */
        //----------------------------------------------------------------------
    }

    if(StationNumber == 4) {

        NExistentModules = 8; //8 modules in the station (4 big modules and 4 hot zones)
        XSizeStation = 2*XModuleSize_StationPrototype + 2*dXInnerFrame + 2*BeamPipeRadius;
        YSizeStation = 2*YModuleSize_StationPrototype + 2*dYInnerFrame + 2*BeamPipeRadius;
        ZSizeStation = 2*ZSizeGemModule;

        //big modules
        ReadoutModules[0] =
            new BmnGemStripReadoutModule(XModuleSize_StationPrototype, YModuleSize_StationPrototype, XCenterStation-XModuleSize_StationPrototype+BeamPipeRadius+dXInnerFrame, YCenterStation+0+BeamPipeRadius+dYInnerFrame, PitchValueModule, -StripAngle, LowerStripWidth, UpperStripWidth, ZPosition+ZSizeGemModule);
        ReadoutModules[0]->SetDeadZone(XCenterStation-XHotZoneSize_StationPrototype+BeamPipeRadius+dXInnerFrame, XCenterStation+0+BeamPipeRadius+dXInnerFrame, YCenterStation+0+BeamPipeRadius+dYInnerFrame, YCenterStation+YHotZoneSize_StationPrototype+BeamPipeRadius+dYInnerFrame);

        ReadoutModules[1] =
            new BmnGemStripReadoutModule(XModuleSize_StationPrototype, YModuleSize_StationPrototype, XCenterStation+0+BeamPipeRadius+dXInnerFrame, YCenterStation+0-BeamPipeRadius-dYInnerFrame, PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth, ZPosition+(ZSizeGemModule-ZSizeReadoutModule));
        ReadoutModules[1]->SetDeadZone(XCenterStation+0+BeamPipeRadius+dXInnerFrame, XCenterStation+XHotZoneSize_StationPrototype+BeamPipeRadius+dXInnerFrame, YCenterStation+0-BeamPipeRadius-dYInnerFrame, YCenterStation+YHotZoneSize_StationPrototype-BeamPipeRadius-dYInnerFrame);

        ReadoutModules[2] =
            new BmnGemStripReadoutModule(XModuleSize_StationPrototype, YModuleSize_StationPrototype, XCenterStation+0-BeamPipeRadius-dXInnerFrame, YCenterStation-YModuleSize_StationPrototype-BeamPipeRadius-dYInnerFrame, PitchValueModule, -StripAngle, LowerStripWidth, UpperStripWidth, ZPosition+ZSizeGemModule);
        ReadoutModules[2]->SetDeadZone(XCenterStation+0-BeamPipeRadius-dXInnerFrame, XCenterStation+XHotZoneSize_StationPrototype-BeamPipeRadius-dXInnerFrame, YCenterStation-YHotZoneSize_StationPrototype-BeamPipeRadius-dYInnerFrame, YCenterStation+0-BeamPipeRadius-dYInnerFrame);

        ReadoutModules[3] =
            new BmnGemStripReadoutModule(XModuleSize_StationPrototype, YModuleSize_StationPrototype, XCenterStation-XModuleSize_StationPrototype-BeamPipeRadius-dXInnerFrame, YCenterStation-YModuleSize_StationPrototype+BeamPipeRadius+dYInnerFrame, PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth, ZPosition+(ZSizeGemModule-ZSizeReadoutModule));
        ReadoutModules[3]->SetDeadZone(XCenterStation-XHotZoneSize_StationPrototype-BeamPipeRadius-dXInnerFrame, XCenterStation+0-BeamPipeRadius-dXInnerFrame, YCenterStation-YHotZoneSize_StationPrototype+BeamPipeRadius+dYInnerFrame, YCenterStation+0+BeamPipeRadius+dYInnerFrame);

        //hot zones
        ReadoutModules[4] =
            new BmnGemStripReadoutModule(XHotZoneSize_StationPrototype, YHotZoneSize_StationPrototype, XCenterStation-XHotZoneSize_StationPrototype+BeamPipeRadius+dXInnerFrame, YCenterStation+0+BeamPipeRadius+dYInnerFrame, PitchValueModule, -StripAngle, LowerStripWidth, UpperStripWidth, ZPosition+ZSizeGemModule);

        ReadoutModules[5] =
            new BmnGemStripReadoutModule(XHotZoneSize_StationPrototype, YHotZoneSize_StationPrototype, XCenterStation+0+BeamPipeRadius+dXInnerFrame, YCenterStation+0-BeamPipeRadius-dYInnerFrame, PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth, ZPosition+(ZSizeGemModule-ZSizeReadoutModule));

        ReadoutModules[6] =
            new BmnGemStripReadoutModule(XHotZoneSize_StationPrototype, YHotZoneSize_StationPrototype, XCenterStation+0-BeamPipeRadius-dXInnerFrame, YCenterStation-YHotZoneSize_StationPrototype-BeamPipeRadius-dYInnerFrame, PitchValueModule, -StripAngle, LowerStripWidth, UpperStripWidth, ZPosition+ZSizeGemModule);

        ReadoutModules[7] =
            new BmnGemStripReadoutModule(XHotZoneSize_StationPrototype, YHotZoneSize_StationPrototype, XCenterStation-XHotZoneSize_StationPrototype-BeamPipeRadius-dXInnerFrame, YCenterStation-YHotZoneSize_StationPrototype+BeamPipeRadius+dYInnerFrame, PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth, ZPosition+(ZSizeGemModule-ZSizeReadoutModule));

        //visual test ----------------------------------------------------------
        /*if(StationNumber == 4)
        {
            station_4_canv = new TCanvas("station_4_canv", "station_4_canv", 10, 10, 1000, 1000);
            station_4_canv->Range(-75.0, -75.0, 75.0, 75.0);
            TWbox *modules[8];
            TWbox *deadzones[8];

            for(Int_t im = 0; im < 8; im++) {

                Double_t xmin = ReadoutModules[im]->GetXMinReadout();
                Double_t xmax = ReadoutModules[im]->GetXMaxReadout();
                Double_t ymin = ReadoutModules[im]->GetYMinReadout();
                Double_t ymax = ReadoutModules[im]->GetYMaxReadout();

                modules[im] = new TWbox(xmin, ymin, xmax, ymax, 18, 1, 1);
                modules[im]->Draw();

                if(im < 4) {
                    Double_t xmin_deadzone = ReadoutModules[im]->GetXMinDeadZone();
                    Double_t xmax_deadzone = ReadoutModules[im]->GetXMaxDeadZone();
                    Double_t ymin_deadzone = ReadoutModules[im]->GetYMinDeadZone();
                    Double_t ymax_deadzone = ReadoutModules[im]->GetYMaxDeadZone();

                    deadzones[im] = new TWbox(xmin_deadzone, ymin_deadzone, xmax_deadzone, ymax_deadzone, TColor::GetColor("#800000"), 1, 1);
                    deadzones[im]->Draw();
                }
            }
        }
        */
        //----------------------------------------------------------------------
    }

    if(StationNumber >= 5 && StationNumber <= 11) {

        NExistentModules = 8; //8 modules in the station (4 big modules and 4 hot zones)
        XSizeStation = 2*XModuleSize_Station[StationNumber];
        YSizeStation = 2*YModuleSize_Station[StationNumber];
        ZSizeStation = 2*ZSizeGemModule;

        //big modules
        ReadoutModules[0] =
            new BmnGemStripReadoutModule(XModuleSize_Station[StationNumber], YModuleSize_Station[StationNumber], XCenterStation-XModuleSize_Station[StationNumber], YCenterStation+0, PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth, ZPosition+ZSizeGemModule);
        if(StationNumber%2 == 1)
            ReadoutModules[0]->SetDeadZone(XCenterStation-XRectHotZoneSize_Station[StationNumber], XCenterStation+0, YCenterStation+0, YCenterStation+YRectHotZoneSize_Station[StationNumber]);
        else
            ReadoutModules[0]->SetDeadZone(XCenterStation-XSlopeHotZoneSize_Station[StationNumber][1], XCenterStation+0, YCenterStation+0, YCenterStation+YSlopeHotZoneSize_Station[StationNumber]);

        ReadoutModules[1] =
            new BmnGemStripReadoutModule(XModuleSize_Station[StationNumber], YModuleSize_Station[StationNumber], XCenterStation+0, YCenterStation+0, PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth, ZPosition+ZSizeGemModule);
        if(StationNumber%2 == 1)
            ReadoutModules[1]->SetDeadZone(XCenterStation+0, XCenterStation+XSlopeHotZoneSize_Station[StationNumber][1], YCenterStation+0, YCenterStation+YSlopeHotZoneSize_Station[StationNumber]);
        else
            ReadoutModules[1]->SetDeadZone(XCenterStation+0, XCenterStation+XRectHotZoneSize_Station[StationNumber], YCenterStation+0, YCenterStation+YRectHotZoneSize_Station[StationNumber]);

        ReadoutModules[2] =
            new BmnGemStripReadoutModule(XModuleSize_Station[StationNumber], YModuleSize_Station[StationNumber], XCenterStation+0, YCenterStation-YModuleSize_Station[StationNumber], PitchValueModule, -StripAngle, LowerStripWidth, UpperStripWidth, ZPosition+(ZSizeGemModule-ZSizeReadoutModule));
        if(StationNumber%2 == 1)
            ReadoutModules[2]->SetDeadZone(XCenterStation+0, XCenterStation+XSlopeHotZoneSize_Station[StationNumber][1], YCenterStation-YSlopeHotZoneSize_Station[StationNumber], YCenterStation+0);
        else
            ReadoutModules[2]->SetDeadZone(XCenterStation+0, XCenterStation+XRectHotZoneSize_Station[StationNumber], YCenterStation-YRectHotZoneSize_Station[StationNumber], YCenterStation+0);

        ReadoutModules[3] =
            new BmnGemStripReadoutModule(XModuleSize_Station[StationNumber], YModuleSize_Station[StationNumber], XCenterStation-XModuleSize_Station[StationNumber], YCenterStation-YModuleSize_Station[StationNumber], PitchValueModule, -StripAngle, LowerStripWidth, UpperStripWidth, ZPosition+(ZSizeGemModule-ZSizeReadoutModule));
        if(StationNumber%2 == 1)
            ReadoutModules[3]->SetDeadZone(XCenterStation-XRectHotZoneSize_Station[StationNumber], XCenterStation+0, YCenterStation-YRectHotZoneSize_Station[StationNumber], YCenterStation+0);
        else
            ReadoutModules[3]->SetDeadZone(XCenterStation-XSlopeHotZoneSize_Station[StationNumber][1], XCenterStation+0, YCenterStation-YSlopeHotZoneSize_Station[StationNumber], YCenterStation+0);

        //hot zones
        if(StationNumber%2 == 1) {
            ReadoutModules[4] =
                new BmnGemStripReadoutModule(XRectHotZoneSize_Station[StationNumber], YRectHotZoneSize_Station[StationNumber], XCenterStation-XRectHotZoneSize_Station[StationNumber], YCenterStation+0, PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth, ZPosition+ZSizeGemModule);
        }
        else {
            ReadoutModules[4] =
                new BmnGemStripReadoutModule(XSlopeHotZoneSize_Station[StationNumber][1], YSlopeHotZoneSize_Station[StationNumber], XCenterStation-XSlopeHotZoneSize_Station[StationNumber][1], YCenterStation+0, PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth, ZPosition+ZSizeGemModule);
        }
        ReadoutModules[4]->SetDeadZone(XCenterStation-BeamPipeRadius, XCenterStation+0, YCenterStation+0, YCenterStation+BeamPipeRadius);

        if(StationNumber%2 == 1) {
            ReadoutModules[5] =
                new BmnGemStripReadoutModule(XSlopeHotZoneSize_Station[StationNumber][1], YSlopeHotZoneSize_Station[StationNumber], XCenterStation+0, YCenterStation+0, PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth, ZPosition+ZSizeGemModule);
        }
        else {
            ReadoutModules[5] =
                new BmnGemStripReadoutModule(XRectHotZoneSize_Station[StationNumber], YRectHotZoneSize_Station[StationNumber], XCenterStation+0, YCenterStation+0, PitchValueModule, StripAngle, LowerStripWidth, UpperStripWidth, ZPosition+ZSizeGemModule);
        }
        ReadoutModules[5]->SetDeadZone(XCenterStation+0, XCenterStation+BeamPipeRadius, YCenterStation+0, YCenterStation+BeamPipeRadius);

        if(StationNumber%2 == 1) {
            ReadoutModules[6] =
                new BmnGemStripReadoutModule(XSlopeHotZoneSize_Station[StationNumber][1], YSlopeHotZoneSize_Station[StationNumber], XCenterStation+0, YCenterStation-YSlopeHotZoneSize_Station[StationNumber], PitchValueModule, -StripAngle, LowerStripWidth, UpperStripWidth, ZPosition+(ZSizeGemModule-ZSizeReadoutModule));
        }
        else {
            ReadoutModules[6] =
                new BmnGemStripReadoutModule(XRectHotZoneSize_Station[StationNumber], YRectHotZoneSize_Station[StationNumber], XCenterStation+0, YCenterStation-YRectHotZoneSize_Station[StationNumber], PitchValueModule, -StripAngle, LowerStripWidth, UpperStripWidth, ZPosition+(ZSizeGemModule-ZSizeReadoutModule));
        }
        ReadoutModules[6]->SetDeadZone(XCenterStation+0, XCenterStation+BeamPipeRadius, YCenterStation-BeamPipeRadius, YCenterStation+0);

        if(StationNumber%2 == 1) {
            ReadoutModules[7] =
                new BmnGemStripReadoutModule(XRectHotZoneSize_Station[StationNumber], YRectHotZoneSize_Station[StationNumber], XCenterStation-XRectHotZoneSize_Station[StationNumber], YCenterStation-YRectHotZoneSize_Station[StationNumber], PitchValueModule, -StripAngle, LowerStripWidth, UpperStripWidth, ZPosition+(ZSizeGemModule-ZSizeReadoutModule));
        }
        else {
            ReadoutModules[7] =
                new BmnGemStripReadoutModule(XSlopeHotZoneSize_Station[StationNumber][1], YSlopeHotZoneSize_Station[StationNumber], XCenterStation-XSlopeHotZoneSize_Station[StationNumber][1], YCenterStation-YSlopeHotZoneSize_Station[StationNumber], PitchValueModule, -StripAngle, LowerStripWidth, UpperStripWidth, ZPosition+(ZSizeGemModule-ZSizeReadoutModule));
        }
        ReadoutModules[7]->SetDeadZone(XCenterStation-BeamPipeRadius, XCenterStation+0, YCenterStation-BeamPipeRadius, YCenterStation+0);

        //visual test ----------------------------------------------------------
        /*
        if(StationNumber == 11)
        {
            station_5_11_canv = new TCanvas("station_5_11_canv", "station_5_11_canv", 10, 10, 1000, 1000);
            if(StationNumber <= 7 )
                station_5_11_canv->Range(-85.0, -85.0, 85.0, 85.0);
            else
                station_5_11_canv->Range(-105.0, -105.0, 105.0, 105.0);
            TWbox *modules[8];
            TWbox *deadzones[8];

            for(Int_t im = 0; im < 8; im++) {

                Double_t xmin = ReadoutModules[im]->GetXMinReadout();
                Double_t xmax = ReadoutModules[im]->GetXMaxReadout();
                Double_t ymin = ReadoutModules[im]->GetYMinReadout();
                Double_t ymax = ReadoutModules[im]->GetYMaxReadout();

                modules[im] = new TWbox(xmin, ymin, xmax, ymax, 18, 1, 1);
                modules[im]->Draw();

                Double_t xmin_deadzone = ReadoutModules[im]->GetXMinDeadZone();
                Double_t xmax_deadzone = ReadoutModules[im]->GetXMaxDeadZone();
                Double_t ymin_deadzone = ReadoutModules[im]->GetYMinDeadZone();
                Double_t ymax_deadzone = ReadoutModules[im]->GetYMaxDeadZone();

                deadzones[im] = new TWbox(xmin_deadzone, ymin_deadzone, xmax_deadzone, ymax_deadzone, TColor::GetColor("#800000"), 1, 1);
                deadzones[im]->Draw();
            }
        }
        */
        //----------------------------------------------------------------------
    }
    //end assembling the station -----------------------------------------------
}

BmnGemStripStation::~BmnGemStripStation() {
    for(Int_t i = 0; i < MaxModulesInStation; ++i) {
        if(ReadoutModules[i]) delete ReadoutModules[i];
    }
}

Int_t BmnGemStripStation::GetPointModuleOwhership(Double_t xcoord, Double_t ycoord) {

    if(StationNumber >= 0 && StationNumber <= 3) {

        for(Int_t i = 3; i >= 0; --i) {
            if((xcoord >= (XCenterStation-XZoneSizes_Station[StationNumber][i]) && xcoord <= (XCenterStation+0))
            && (ycoord >= (YCenterStation+0)  && (ycoord <= YCenterStation+YZoneSizes_Station[StationNumber][i]))) return i*4;

            if((xcoord >= (XCenterStation+0) && xcoord <= (XCenterStation+XZoneSizes_Station[StationNumber][i]))
            && (ycoord >= (YCenterStation+0)  && (ycoord <= YCenterStation+YZoneSizes_Station[StationNumber][i]))) return i*4+1;

            if((xcoord >= (XCenterStation+0) && xcoord <= (XCenterStation+XZoneSizes_Station[StationNumber][i]))
            && (ycoord >= (YCenterStation-YZoneSizes_Station[StationNumber][i])  && (ycoord <= YCenterStation+0))) return i*4+2;

            if((xcoord >= (XCenterStation-XZoneSizes_Station[StationNumber][i]) && xcoord <= (XCenterStation+0))
            && (ycoord >= (YCenterStation-YZoneSizes_Station[StationNumber][i])  && (ycoord <= YCenterStation+0))) return i*4+3;
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

Int_t BmnGemStripStation::AddPointToStation(Double_t xcoord, Double_t ycoord, Double_t zcoord, Double_t dEloss, Int_t refID) {

    Int_t module = GetPointModuleOwhership(xcoord, ycoord);

    if(module >= 0) {
        if(ReadoutModules[module]->AddRealPointFullOne(xcoord, ycoord, zcoord, dEloss, refID)) {
            return module;
        }
        else {
            return -1;
        }
    }
    return -1;
}

Int_t BmnGemStripStation::CountNAddedToStationPoints() {
    Int_t points_sum = 0;
    for(Int_t iModule = 0; iModule < NExistentModules; iModule++) {
        points_sum += ReadoutModules[iModule]->GetNRealPoints();
    }
    return points_sum;
}

void BmnGemStripStation::ProcessPointsInStation() {
    for(Int_t iModule = 0; iModule < NExistentModules; iModule++) {
        ReadoutModules[iModule]->CalculateStripHitIntersectionPoints();
    }
}

Int_t BmnGemStripStation::CountNProcessedPointInStation() {
    Int_t points_sum = 0;
    for(Int_t iModule = 0; iModule < NExistentModules; iModule++) {
        points_sum += ReadoutModules[iModule]->GetNIntersectionPoints();
    }
    return points_sum;
}

BmnGemStripReadoutModule* BmnGemStripStation::GetReadoutModule(Int_t index) {
    if(index >= 0 && index < NExistentModules) {
        return ReadoutModules[index];
    }
    return 0;
}

ClassImp(BmnGemStripStation)