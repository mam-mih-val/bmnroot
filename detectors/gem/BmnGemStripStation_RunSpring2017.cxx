#include "BmnGemStripStation_RunSpring2017.h"

BmnGemStripStation_RunSpring2017::BmnGemStripStation_RunSpring2017(Int_t iStation,
                       Double_t xpos_station, Double_t ypos_station, Double_t zpos_station,
                       Double_t beamradius, BmnGemStripConfiguration::GEM_CONFIG config) {

    //All linear values must be entered in units of cm

    fCurrentConfig = config;

    StationNumber = iStation;

    //Station shift position
    XPosition = xpos_station;
    YPosition = ypos_station;
    ZPosition = zpos_station;

    BeamHoleRadius = beamradius;


    //Parameters for each station
        //ZSizeModuleWithFrames : GEM thickness (frames+sensitive)
        //ZSizeModule : sensitive volume thickness
        //XSize : x-size of the station
        //YSize : y-size of the station
        //ZSize : z-size of the station

    switch (iStation) {

        case 0: //one plane 66x41
            LowerLayerStripAngle = 0.0;
            UpperLayerStripAngle = +15.0;
            LowerLayerPitch = 0.08;
            UpperLayerPitch = 0.08;
            EDriftDirection = ForwardZAxisEDrift;
            NModules = 1; //Number of modules in the station
            break;

        case 1: //one plane 66x41
            LowerLayerStripAngle = 0.0;
            UpperLayerStripAngle = -15.0;
            LowerLayerPitch = 0.08;
            UpperLayerPitch = 0.08;
            EDriftDirection = BackwardZAxisEDrift;
            NModules = 1; //Number of modules in the station
            break;

        case 2: //one plane 66x41
            LowerLayerStripAngle = 0.0;
            UpperLayerStripAngle = +15.0;
            LowerLayerPitch = 0.08;
            UpperLayerPitch = 0.08;
            EDriftDirection = ForwardZAxisEDrift;
            NModules = 1; //Number of modules in the station
            break;

        case 3: //two plane 66x41
            LowerLayerStripAngle = 0.0;
            UpperLayerStripAngle = -15.0;
            LowerLayerPitch = 0.08;
            UpperLayerPitch = 0.08;
            EDriftDirection = BackwardZAxisEDrift;
            NModules = 2; //Number of modules in the station
            break;

        case 4: //one plane 163x45
            LowerLayerStripAngle = 0.0;
            UpperLayerStripAngle = +15.0;
            LowerLayerPitch = 0.08;
            UpperLayerPitch = 0.08;
            EDriftDirection = ForwardZAxisEDrift;
            NModules = 2; //Number of modules in the station
            break;

        case 5: //one plane 163x45
            LowerLayerStripAngle = 0.0;
            UpperLayerStripAngle = -15.0;
            LowerLayerPitch = 0.08;
            UpperLayerPitch = 0.08;
            EDriftDirection = BackwardZAxisEDrift;
            NModules = 2; //Number of modules in the station
            break;
    }

    //Planes SIZES

    //station 0-3 (plane 66x41)
    XModuleSize_Plane66x41 = 66.0;
    YModuleSize_Plane66x41 = 41.0;

    XHotZoneSize_Plane66x41 = 15.0;
    YHotZoneSize_Plane66x41 = 10.0;

    //station 4-5 (plane 163x45)
    XModuleSize_Plane163x45 = 163.2*0.5;
    YModuleSize_Plane163x45 = 45.0;

    XRectHotZoneSize_Plane163x45 = 40.0;
    YRectHotZoneSize_Plane163x45 = 15.0;

    XSlopeHotZoneSize_Plane163x45[0] = 37.99;
    XSlopeHotZoneSize_Plane163x45[1] = 42.01;

    YSlopeHotZoneSize_Plane163x45 = 15.0;

    //Module shifts ------------------------------------------------------------
    XShiftOfModules = new Double_t[NModules];
    YShiftOfModules = new Double_t[NModules];
    ZShiftOfModules = new Double_t[NModules];

    switch(fCurrentConfig) {
        case BmnGemStripConfiguration::RunSpring2017:
            for(Int_t iModule = 0; iModule < NModules; ++iModule) {
                XShiftOfModules[iModule] = -BmnGemStationPositions_RunSpring2017::XModuleShifts[iStation][iModule]; //inverted : (bm@n x-coord -> classical x-coord)
                YShiftOfModules[iModule] = BmnGemStationPositions_RunSpring2017::YModuleShifts[iStation][iModule];
                ZShiftOfModules[iModule] = BmnGemStationPositions_RunSpring2017::ZModuleShifts[iStation][iModule];
            }
            break;

        default:
            for(Int_t iModule = 0; iModule < NModules; ++iModule) {
                XShiftOfModules[iModule] = -BmnGemStationPositions_RunSpring2017::XModuleShifts[iStation][iModule]; //inverted : (bm@n x-coord -> classical x-coord)
                YShiftOfModules[iModule] = BmnGemStationPositions_RunSpring2017::YModuleShifts[iStation][iModule];
                ZShiftOfModules[iModule] = BmnGemStationPositions_RunSpring2017::ZModuleShifts[iStation][iModule];
            }
    }
    //--------------------------------------------------------------------------

    //Assembling a station -----------------------------------------------------

    if(StationNumber >= 0 && StationNumber <= 2) {
        BuildModules_One66x41Plane();
    }

    if(StationNumber == 3) {
        BuildModules_Two66x41Plane();
    }

    if(StationNumber >= 4 && StationNumber <= 5) {
        BuildModules_One163x45Plane();
    }
    //end assembling the station -----------------------------------------------

    //define station borders and sizes
    DefineStationBorders();
}

BmnGemStripStation_RunSpring2017::~BmnGemStripStation_RunSpring2017() {

}
//------------------------------------------------------------------------------

void BmnGemStripStation_RunSpring2017::BuildModules_One66x41Plane() {

    Modules = new BmnGemStripModule* [NModules];

    Double_t DriftGapThickness = 0.3;
    Double_t FirstTransferGapThickness = 0.25;
    Double_t SecondTransferGapThickness = 0.2;
    Double_t InductionGapThickness = 0.15;

    //module
    Modules[0] = new BmnGemStripModule(ZPosition+ZShiftOfModules[0], EDriftDirection,
                                       DriftGapThickness, FirstTransferGapThickness,
                                       SecondTransferGapThickness, InductionGapThickness);

    //zone 0 (big) -------------------------------------------------------------

        //lower strip layer (zone 0) -------------------------------------------
        BmnGemStripLayer lower_layer_zone0(0, LowerStripLayer,
                                           XModuleSize_Plane66x41, YModuleSize_Plane66x41,
                                           XShiftOfModules[0]+XPosition-XModuleSize_Plane66x41*0.5, YShiftOfModules[0]+YPosition-YModuleSize_Plane66x41*0.5,
                                           LowerLayerPitch, LowerLayerStripAngle);

        if(UpperLayerStripAngle >= 0.0) {
            lower_layer_zone0.SetStripNumberingOrder(LeftToRight);
            lower_layer_zone0.SetStripNumberingBorders(LeftTop, RightBottom);
        }
        else {
            lower_layer_zone0.SetStripNumberingOrder(LeftToRight);
            lower_layer_zone0.SetStripNumberingBorders(LeftBottom, RightTop);
        }

        //dead zone for layers in zone 0
        const Int_t NPoints_DeadZone_zone0 = 4;
        Double_t XPoints_DeadZone_zone0[NPoints_DeadZone_zone0];
        Double_t YPoints_DeadZone_zone0[NPoints_DeadZone_zone0];

        if(UpperLayerStripAngle >= 0.0) {
            XPoints_DeadZone_zone0[0] = XShiftOfModules[0]+XPosition-XModuleSize_Plane66x41*0.5;
            XPoints_DeadZone_zone0[1] = XShiftOfModules[0]+XPosition-XModuleSize_Plane66x41*0.5;
            XPoints_DeadZone_zone0[2] = XShiftOfModules[0]+XPosition-XModuleSize_Plane66x41*0.5 + XHotZoneSize_Plane66x41;
            XPoints_DeadZone_zone0[3] = XShiftOfModules[0]+XPosition-XModuleSize_Plane66x41*0.5 + XHotZoneSize_Plane66x41;

            YPoints_DeadZone_zone0[0] = YShiftOfModules[0]+YPosition-YModuleSize_Plane66x41*0.5;
            YPoints_DeadZone_zone0[1] = YShiftOfModules[0]+YPosition-YModuleSize_Plane66x41*0.5 + YHotZoneSize_Plane66x41;
            YPoints_DeadZone_zone0[2] = YShiftOfModules[0]+YPosition-YModuleSize_Plane66x41*0.5 + YHotZoneSize_Plane66x41;
            YPoints_DeadZone_zone0[3] = YShiftOfModules[0]+YPosition-YModuleSize_Plane66x41*0.5;
        }
        else {
            XPoints_DeadZone_zone0[0] = XShiftOfModules[0]+XPosition-XModuleSize_Plane66x41*0.5;
            XPoints_DeadZone_zone0[1] = XShiftOfModules[0]+XPosition-XModuleSize_Plane66x41*0.5;
            XPoints_DeadZone_zone0[2] = XShiftOfModules[0]+XPosition-XModuleSize_Plane66x41*0.5 + XHotZoneSize_Plane66x41;
            XPoints_DeadZone_zone0[3] = XShiftOfModules[0]+XPosition-XModuleSize_Plane66x41*0.5 + XHotZoneSize_Plane66x41;

            YPoints_DeadZone_zone0[0] = YShiftOfModules[0]+YPosition+YModuleSize_Plane66x41*0.5 - YHotZoneSize_Plane66x41;
            YPoints_DeadZone_zone0[1] = YShiftOfModules[0]+YPosition+YModuleSize_Plane66x41*0.5;
            YPoints_DeadZone_zone0[2] = YShiftOfModules[0]+YPosition+YModuleSize_Plane66x41*0.5;
            YPoints_DeadZone_zone0[3] = YShiftOfModules[0]+YPosition+YModuleSize_Plane66x41*0.5 - YHotZoneSize_Plane66x41;
        }

        lower_layer_zone0.AddDeadZone(NPoints_DeadZone_zone0, XPoints_DeadZone_zone0, YPoints_DeadZone_zone0);
        //----------------------------------------------------------------------

        //upper strip layer (zone 0) -------------------------------------------
        BmnGemStripLayer upper_layer_zone0(0, UpperStripLayer,
                                           XModuleSize_Plane66x41, YModuleSize_Plane66x41,
                                           XShiftOfModules[0]+XPosition-XModuleSize_Plane66x41*0.5, YShiftOfModules[0]+YPosition-YModuleSize_Plane66x41*0.5,
                                           UpperLayerPitch, UpperLayerStripAngle);

        if(UpperLayerStripAngle >= 0.0) {
            upper_layer_zone0.SetStripNumberingOrder(LeftToRight);
            upper_layer_zone0.SetStripNumberingBorders(LeftTop, RightBottom);
        }
        else {
            upper_layer_zone0.SetStripNumberingOrder(LeftToRight);
            upper_layer_zone0.SetStripNumberingBorders(LeftBottom, RightTop);
        }

        upper_layer_zone0.AddDeadZone(NPoints_DeadZone_zone0, XPoints_DeadZone_zone0, YPoints_DeadZone_zone0);
        //----------------------------------------------------------------------

    //--------------------------------------------------------------------------

    //zone 1 (hot) -------------------------------------------------------------

        BmnGemStripLayer lower_layer_zone1;
        BmnGemStripLayer upper_layer_zone1;

        if(UpperLayerStripAngle >= 0.0) {
            //lower strip layer (zone 1) -------------------------------------------
            lower_layer_zone1 = BmnGemStripLayer(1, LowerStripLayer,
                                               XHotZoneSize_Plane66x41, YHotZoneSize_Plane66x41,
                                               XShiftOfModules[0]+XPosition-XModuleSize_Plane66x41*0.5, YShiftOfModules[0]+YPosition-YModuleSize_Plane66x41*0.5,
                                               LowerLayerPitch, LowerLayerStripAngle);

            lower_layer_zone1.SetStripNumberingOrder(LeftToRight);
            lower_layer_zone1.SetStripNumberingBorders(LeftTop, RightBottom);
            //----------------------------------------------------------------------

            //upper strip layer (zone 1) -------------------------------------------
            upper_layer_zone1 = BmnGemStripLayer(1, UpperStripLayer,
                                               XHotZoneSize_Plane66x41, YHotZoneSize_Plane66x41,
                                               XShiftOfModules[0]+XPosition-XModuleSize_Plane66x41*0.5, YShiftOfModules[0]+YPosition-YModuleSize_Plane66x41*0.5,
                                               UpperLayerPitch, UpperLayerStripAngle);

            upper_layer_zone1.SetStripNumberingOrder(LeftToRight);
            upper_layer_zone1.SetStripNumberingBorders(LeftTop, RightBottom);
            //----------------------------------------------------------------------
        }
        else {
            //lower strip layer (zone 1) -------------------------------------------
            lower_layer_zone1 = BmnGemStripLayer(1, LowerStripLayer,
                                               XHotZoneSize_Plane66x41, YHotZoneSize_Plane66x41,
                                               XShiftOfModules[0]+XPosition-XModuleSize_Plane66x41*0.5, YShiftOfModules[0]+YPosition+YModuleSize_Plane66x41*0.5-YHotZoneSize_Plane66x41,
                                               LowerLayerPitch, LowerLayerStripAngle);

            lower_layer_zone1.SetStripNumberingOrder(LeftToRight);
            lower_layer_zone1.SetStripNumberingBorders(LeftTop, RightBottom);
            //----------------------------------------------------------------------

            //upper strip layer (zone 1) -------------------------------------------
            upper_layer_zone1 = BmnGemStripLayer(1, UpperStripLayer,
                                               XHotZoneSize_Plane66x41, YHotZoneSize_Plane66x41,
                                               XShiftOfModules[0]+XPosition-XModuleSize_Plane66x41*0.5, YShiftOfModules[0]+YPosition+YModuleSize_Plane66x41*0.5-YHotZoneSize_Plane66x41,
                                               UpperLayerPitch, UpperLayerStripAngle);

            upper_layer_zone1.SetStripNumberingOrder(LeftToRight);
            upper_layer_zone1.SetStripNumberingBorders(LeftBottom, RightTop);
            //----------------------------------------------------------------------
        }
    //--------------------------------------------------------------------------

    Modules[0]->AddStripLayer(lower_layer_zone0);
    Modules[0]->AddStripLayer(upper_layer_zone0);
    Modules[0]->AddStripLayer(lower_layer_zone1);
    Modules[0]->AddStripLayer(upper_layer_zone1);

    return;
}

void BmnGemStripStation_RunSpring2017::BuildModules_Two66x41Plane() {

    Modules = new BmnGemStripModule* [NModules];

    //module 0 (left)
    {
        Double_t DriftGapThickness = 0.5;
        Double_t FirstTransferGapThickness = 0.25;
        Double_t SecondTransferGapThickness = 0.2;
        Double_t InductionGapThickness = 0.15;

        Modules[0] = new BmnGemStripModule(ZPosition+ZShiftOfModules[0], EDriftDirection,
                                           DriftGapThickness, FirstTransferGapThickness,
                                           SecondTransferGapThickness, InductionGapThickness);

        //zone 0 (big) -------------------------------------------------------------

            //lower strip layer (zone 0) -------------------------------------------
            BmnGemStripLayer lower_layer_zone0(0, LowerStripLayer,
                                               XModuleSize_Plane66x41, YModuleSize_Plane66x41,
                                               XShiftOfModules[0]+XPosition-XModuleSize_Plane66x41, YShiftOfModules[0]+YPosition-YModuleSize_Plane66x41*0.5,
                                               LowerLayerPitch, LowerLayerStripAngle);

            lower_layer_zone0.SetStripNumberingOrder(RightToLeft);
            lower_layer_zone0.SetStripNumberingBorders(LeftBottom, RightTop);


            //dead zone for layers in zone 0
            const Int_t NPoints_DeadZone_zone0 = 4;
            Double_t XPoints_DeadZone_zone0[NPoints_DeadZone_zone0];
            Double_t YPoints_DeadZone_zone0[NPoints_DeadZone_zone0];


            XPoints_DeadZone_zone0[0] = XShiftOfModules[0]+XPosition-0.0-XHotZoneSize_Plane66x41;
            XPoints_DeadZone_zone0[1] = XShiftOfModules[0]+XPosition-0.0-XHotZoneSize_Plane66x41;
            XPoints_DeadZone_zone0[2] = XShiftOfModules[0]+XPosition-0.0;
            XPoints_DeadZone_zone0[3] = XShiftOfModules[0]+XPosition-0.0;

            YPoints_DeadZone_zone0[0] = YShiftOfModules[0]+YPosition-YModuleSize_Plane66x41*0.5;
            YPoints_DeadZone_zone0[1] = YShiftOfModules[0]+YPosition-YModuleSize_Plane66x41*0.5 + YHotZoneSize_Plane66x41;
            YPoints_DeadZone_zone0[2] = YShiftOfModules[0]+YPosition-YModuleSize_Plane66x41*0.5 + YHotZoneSize_Plane66x41;
            YPoints_DeadZone_zone0[3] = YShiftOfModules[0]+YPosition-YModuleSize_Plane66x41*0.5;


            lower_layer_zone0.AddDeadZone(NPoints_DeadZone_zone0, XPoints_DeadZone_zone0, YPoints_DeadZone_zone0);
            //----------------------------------------------------------------------

            //upper strip layer (zone 0) -------------------------------------------
            BmnGemStripLayer upper_layer_zone0(0, UpperStripLayer,
                                               XModuleSize_Plane66x41, YModuleSize_Plane66x41,
                                               XShiftOfModules[0]+XPosition-XModuleSize_Plane66x41, YShiftOfModules[0]+YPosition-YModuleSize_Plane66x41*0.5,
                                               UpperLayerPitch, UpperLayerStripAngle);

            upper_layer_zone0.SetStripNumberingOrder(RightToLeft);
            upper_layer_zone0.SetStripNumberingBorders(LeftBottom, RightTop);


            upper_layer_zone0.AddDeadZone(NPoints_DeadZone_zone0, XPoints_DeadZone_zone0, YPoints_DeadZone_zone0);
            //----------------------------------------------------------------------

        //--------------------------------------------------------------------------

            //zone 1 (hot) -------------------------------------------------------------

            BmnGemStripLayer lower_layer_zone1;
            BmnGemStripLayer upper_layer_zone1;

            //lower strip layer (zone 1) -------------------------------------------
            lower_layer_zone1 = BmnGemStripLayer(1, LowerStripLayer,
                                               XHotZoneSize_Plane66x41, YHotZoneSize_Plane66x41,
                                               XShiftOfModules[0]+XPosition-XHotZoneSize_Plane66x41, YShiftOfModules[0]+YPosition-YModuleSize_Plane66x41*0.5,
                                               LowerLayerPitch, LowerLayerStripAngle);

            lower_layer_zone1.SetStripNumberingOrder(RightToLeft);
            lower_layer_zone1.SetStripNumberingBorders(LeftBottom, RightTop);
            //----------------------------------------------------------------------

            //upper strip layer (zone 1) -------------------------------------------
            upper_layer_zone1 = BmnGemStripLayer(1, UpperStripLayer,
                                               XHotZoneSize_Plane66x41, YHotZoneSize_Plane66x41,
                                               XShiftOfModules[0]+XPosition-XHotZoneSize_Plane66x41, YShiftOfModules[0]+YPosition-YModuleSize_Plane66x41*0.5,
                                               UpperLayerPitch, UpperLayerStripAngle);

            upper_layer_zone1.SetStripNumberingOrder(RightToLeft);
            upper_layer_zone1.SetStripNumberingBorders(LeftBottom, RightTop);
            //----------------------------------------------------------------------

        //--------------------------------------------------------------------------

        Modules[0]->AddStripLayer(lower_layer_zone0);
        Modules[0]->AddStripLayer(upper_layer_zone0);
        Modules[0]->AddStripLayer(lower_layer_zone1);
        Modules[0]->AddStripLayer(upper_layer_zone1);
    }
    //--------------------------------------------------------------------------


    //module 1 (right)
    {
        Double_t DriftGapThickness = 0.3;
        Double_t FirstTransferGapThickness = 0.25;
        Double_t SecondTransferGapThickness = 0.2;
        Double_t InductionGapThickness = 0.15;

        Modules[1] = new BmnGemStripModule(ZPosition+ZShiftOfModules[1], EDriftDirection,
                                           DriftGapThickness, FirstTransferGapThickness,
                                           SecondTransferGapThickness, InductionGapThickness);

        //zone 0 (big) -------------------------------------------------------------

            //lower strip layer (zone 0) -------------------------------------------
            BmnGemStripLayer lower_layer_zone0(0, LowerStripLayer,
                                               XModuleSize_Plane66x41, YModuleSize_Plane66x41,
                                               XShiftOfModules[1]+XPosition+0.0, YShiftOfModules[1]+YPosition-YModuleSize_Plane66x41*0.5,
                                               LowerLayerPitch, LowerLayerStripAngle);

            lower_layer_zone0.SetStripNumberingOrder(LeftToRight);
            lower_layer_zone0.SetStripNumberingBorders(LeftBottom, RightTop);


            //dead zone for layers in zone 0
            const Int_t NPoints_DeadZone_zone0 = 4;
            Double_t XPoints_DeadZone_zone0[NPoints_DeadZone_zone0];
            Double_t YPoints_DeadZone_zone0[NPoints_DeadZone_zone0];


            XPoints_DeadZone_zone0[0] = XShiftOfModules[1]+XPosition+0.0;
            XPoints_DeadZone_zone0[1] = XShiftOfModules[1]+XPosition+0.0;
            XPoints_DeadZone_zone0[2] = XShiftOfModules[1]+XPosition+0.0+XHotZoneSize_Plane66x41;
            XPoints_DeadZone_zone0[3] = XShiftOfModules[1]+XPosition+0.0+XHotZoneSize_Plane66x41;

            YPoints_DeadZone_zone0[0] = YShiftOfModules[1]+YPosition+YModuleSize_Plane66x41*0.5 - YHotZoneSize_Plane66x41;
            YPoints_DeadZone_zone0[1] = YShiftOfModules[1]+YPosition+YModuleSize_Plane66x41*0.5;
            YPoints_DeadZone_zone0[2] = YShiftOfModules[1]+YPosition+YModuleSize_Plane66x41*0.5;
            YPoints_DeadZone_zone0[3] = YShiftOfModules[1]+YPosition+YModuleSize_Plane66x41*0.5 - YHotZoneSize_Plane66x41;


            lower_layer_zone0.AddDeadZone(NPoints_DeadZone_zone0, XPoints_DeadZone_zone0, YPoints_DeadZone_zone0);
            //----------------------------------------------------------------------

            //upper strip layer (zone 0) -------------------------------------------
            BmnGemStripLayer upper_layer_zone0(0, UpperStripLayer,
                                               XModuleSize_Plane66x41, YModuleSize_Plane66x41,
                                               XShiftOfModules[1]+XPosition+0.0, YShiftOfModules[1]+YPosition-YModuleSize_Plane66x41*0.5,
                                               UpperLayerPitch, UpperLayerStripAngle);

            upper_layer_zone0.SetStripNumberingOrder(LeftToRight);
            upper_layer_zone0.SetStripNumberingBorders(LeftBottom, RightTop);


            upper_layer_zone0.AddDeadZone(NPoints_DeadZone_zone0, XPoints_DeadZone_zone0, YPoints_DeadZone_zone0);
            //----------------------------------------------------------------------

            //--------------------------------------------------------------------------

            //zone 1 (hot) -------------------------------------------------------------

            BmnGemStripLayer lower_layer_zone1;
            BmnGemStripLayer upper_layer_zone1;

            //lower strip layer (zone 1) -------------------------------------------
            lower_layer_zone1 = BmnGemStripLayer(1, LowerStripLayer,
                                               XHotZoneSize_Plane66x41, YHotZoneSize_Plane66x41,
                                               XShiftOfModules[1]+XPosition+0.0, YShiftOfModules[1]+YPosition+YModuleSize_Plane66x41*0.5-YHotZoneSize_Plane66x41,
                                               LowerLayerPitch, LowerLayerStripAngle);

            lower_layer_zone1.SetStripNumberingOrder(LeftToRight);
            lower_layer_zone1.SetStripNumberingBorders(LeftBottom, RightTop);
            //----------------------------------------------------------------------

            //upper strip layer (zone 1) -------------------------------------------
            upper_layer_zone1 = BmnGemStripLayer(1, UpperStripLayer,
                                               XHotZoneSize_Plane66x41, YHotZoneSize_Plane66x41,
                                               XShiftOfModules[1]+XPosition+0.0, YShiftOfModules[1]+YPosition+YModuleSize_Plane66x41*0.5-YHotZoneSize_Plane66x41,
                                               UpperLayerPitch, UpperLayerStripAngle);

            upper_layer_zone1.SetStripNumberingOrder(LeftToRight);
            upper_layer_zone1.SetStripNumberingBorders(LeftBottom, RightTop);
            //----------------------------------------------------------------------

        //--------------------------------------------------------------------------

        Modules[1]->AddStripLayer(lower_layer_zone0);
        Modules[1]->AddStripLayer(upper_layer_zone0);
        Modules[1]->AddStripLayer(lower_layer_zone1);
        Modules[1]->AddStripLayer(upper_layer_zone1);
    }

    return;
}

void BmnGemStripStation_RunSpring2017::BuildModules_One163x45Plane() {
    Modules = new BmnGemStripModule* [NModules];


//module 0 (left) --------------------------------------------------------------
    {
        Double_t DriftGapThickness = 0.3;
        Double_t FirstTransferGapThickness = 0.25;
        Double_t SecondTransferGapThickness = 0.2;
        Double_t InductionGapThickness = 0.15;

        Modules[0] = new BmnGemStripModule(ZPosition+ZShiftOfModules[0], EDriftDirection,
                                           DriftGapThickness, FirstTransferGapThickness,
                                           SecondTransferGapThickness, InductionGapThickness);

            //zone 0 (big) ---------------------------------------------------------

            //lower strip layer (zone 0) -------------------------------------------
            BmnGemStripLayer lower_layer_zone0(0, LowerStripLayer,
                                               XModuleSize_Plane163x45, YModuleSize_Plane163x45,
                                               XShiftOfModules[0]+XPosition-XModuleSize_Plane163x45, YShiftOfModules[0]+YPosition-YModuleSize_Plane163x45*0.5,
                                               LowerLayerPitch, LowerLayerStripAngle);

            if(UpperLayerStripAngle >= 0.0) {
                lower_layer_zone0.SetStripNumberingOrder(LeftToRight);
                lower_layer_zone0.SetStripNumberingBorders(LeftTop, RightBottom);
            }
            else {
                lower_layer_zone0.SetStripNumberingOrder(RightToLeft);
                lower_layer_zone0.SetStripNumberingBorders(LeftBottom, RightTop);
            }


            //dead zone for lower layer in zone 0
            const Int_t NPoints_DeadZone_lower_zone0 = 4;
            Double_t XPoints_DeadZone_lower_zone0[NPoints_DeadZone_lower_zone0];
            Double_t YPoints_DeadZone_lower_zone0[NPoints_DeadZone_lower_zone0];

            XPoints_DeadZone_lower_zone0[0] = XShiftOfModules[0]+XPosition-XRectHotZoneSize_Plane163x45;
            XPoints_DeadZone_lower_zone0[1] = XShiftOfModules[0]+XPosition-XRectHotZoneSize_Plane163x45;
            XPoints_DeadZone_lower_zone0[2] = XShiftOfModules[0]+XPosition+0.0;
            XPoints_DeadZone_lower_zone0[3] = XShiftOfModules[0]+XPosition+0.0;

            YPoints_DeadZone_lower_zone0[0] = YShiftOfModules[0]+YPosition-YModuleSize_Plane163x45*0.5;
            YPoints_DeadZone_lower_zone0[1] = YShiftOfModules[0]+YPosition-YModuleSize_Plane163x45*0.5 + YRectHotZoneSize_Plane163x45;
            YPoints_DeadZone_lower_zone0[2] = YShiftOfModules[0]+YPosition-YModuleSize_Plane163x45*0.5 + YRectHotZoneSize_Plane163x45;
            YPoints_DeadZone_lower_zone0[3] = YShiftOfModules[0]+YPosition-YModuleSize_Plane163x45*0.5;

            lower_layer_zone0.AddDeadZone(NPoints_DeadZone_lower_zone0, XPoints_DeadZone_lower_zone0, YPoints_DeadZone_lower_zone0);
            //----------------------------------------------------------------------

            //upper strip layer (zone 0) -------------------------------------------
            BmnGemStripLayer upper_layer_zone0(0, UpperStripLayer,
                                               XModuleSize_Plane163x45, YModuleSize_Plane163x45,
                                               XShiftOfModules[0]+XPosition-XModuleSize_Plane163x45, YShiftOfModules[0]+YPosition-YModuleSize_Plane163x45*0.5,
                                               UpperLayerPitch, UpperLayerStripAngle);

            if(UpperLayerStripAngle >= 0.0) {
                upper_layer_zone0.SetStripNumberingOrder(LeftToRight);
                upper_layer_zone0.SetStripNumberingBorders(LeftTop, RightBottom);
            }
            else {
                upper_layer_zone0.SetStripNumberingOrder(RightToLeft);
                upper_layer_zone0.SetStripNumberingBorders(LeftBottom, RightTop);
            }

            //dead zone for upper layer in zone 0
            const Int_t NPoints_DeadZone_upper_zone0 = 4;
            Double_t XPoints_DeadZone_upper_zone0[NPoints_DeadZone_upper_zone0];
            Double_t YPoints_DeadZone_upper_zone0[NPoints_DeadZone_upper_zone0];

            if(UpperLayerStripAngle >= 0.0) {
                XPoints_DeadZone_upper_zone0[0] = XShiftOfModules[0]+XPosition-XSlopeHotZoneSize_Plane163x45[1];
                XPoints_DeadZone_upper_zone0[1] = XShiftOfModules[0]+XPosition-XSlopeHotZoneSize_Plane163x45[0];
                XPoints_DeadZone_upper_zone0[2] = XShiftOfModules[0]+XPosition+0.0;
                XPoints_DeadZone_upper_zone0[3] = XShiftOfModules[0]+XPosition+0.0;

                YPoints_DeadZone_upper_zone0[0] = YShiftOfModules[0]+YPosition-YModuleSize_Plane163x45*0.5;
                YPoints_DeadZone_upper_zone0[1] = YShiftOfModules[0]+YPosition-YModuleSize_Plane163x45*0.5 + YSlopeHotZoneSize_Plane163x45;
                YPoints_DeadZone_upper_zone0[2] = YShiftOfModules[0]+YPosition-YModuleSize_Plane163x45*0.5 + YSlopeHotZoneSize_Plane163x45;
                YPoints_DeadZone_upper_zone0[3] = YShiftOfModules[0]+YPosition-YModuleSize_Plane163x45*0.5;
            }
            else {
                XPoints_DeadZone_upper_zone0[0] = XShiftOfModules[0]+XPosition-XSlopeHotZoneSize_Plane163x45[0];
                XPoints_DeadZone_upper_zone0[1] = XShiftOfModules[0]+XPosition-XSlopeHotZoneSize_Plane163x45[1];
                XPoints_DeadZone_upper_zone0[2] = XShiftOfModules[0]+XPosition+0.0;
                XPoints_DeadZone_upper_zone0[3] = XShiftOfModules[0]+XPosition+0.0;

                YPoints_DeadZone_upper_zone0[0] = YShiftOfModules[0]+YPosition-YModuleSize_Plane163x45*0.5;
                YPoints_DeadZone_upper_zone0[1] = YShiftOfModules[0]+YPosition-YModuleSize_Plane163x45*0.5 + YSlopeHotZoneSize_Plane163x45;
                YPoints_DeadZone_upper_zone0[2] = YShiftOfModules[0]+YPosition-YModuleSize_Plane163x45*0.5 + YSlopeHotZoneSize_Plane163x45;
                YPoints_DeadZone_upper_zone0[3] = YShiftOfModules[0]+YPosition-YModuleSize_Plane163x45*0.5;
            }



            upper_layer_zone0.AddDeadZone(NPoints_DeadZone_upper_zone0, XPoints_DeadZone_upper_zone0, YPoints_DeadZone_upper_zone0);
            //----------------------------------------------------------------------

            //lower strip layer (zone 1) -------------------------------------------
            BmnGemStripLayer lower_layer_zone1(1, LowerStripLayer,
                                               XRectHotZoneSize_Plane163x45, YRectHotZoneSize_Plane163x45,
                                               XShiftOfModules[0]+XPosition-XRectHotZoneSize_Plane163x45, YShiftOfModules[0]+YPosition-YModuleSize_Plane163x45*0.5,
                                               LowerLayerPitch, LowerLayerStripAngle);

            if(UpperLayerStripAngle >= 0.0) {
                lower_layer_zone1.SetStripNumberingOrder(LeftToRight);
                lower_layer_zone1.SetStripNumberingBorders(LeftTop, RightBottom);
            }
            else {
                lower_layer_zone1.SetStripNumberingOrder(RightToLeft);
                lower_layer_zone1.SetStripNumberingBorders(LeftBottom, RightTop);
            }


            //dead zone for lower layer in zone 1 (hole)
            const Int_t NPoints_DeadZone_lower_zone1 = 4;
            Double_t XPoints_DeadZone_lower_zone1[NPoints_DeadZone_lower_zone1];
            Double_t YPoints_DeadZone_lower_zone1[NPoints_DeadZone_lower_zone1];

            XPoints_DeadZone_lower_zone1[0] = XShiftOfModules[0]+XPosition-BeamHoleRadius;
            XPoints_DeadZone_lower_zone1[1] = XShiftOfModules[0]+XPosition-BeamHoleRadius;
            XPoints_DeadZone_lower_zone1[2] = XShiftOfModules[0]+XPosition+0.0;
            XPoints_DeadZone_lower_zone1[3] = XShiftOfModules[0]+XPosition+0.0;

            YPoints_DeadZone_lower_zone1[0] = YShiftOfModules[0]+YPosition-YModuleSize_Plane163x45*0.5;
            YPoints_DeadZone_lower_zone1[1] = YShiftOfModules[0]+YPosition-YModuleSize_Plane163x45*0.5 + BeamHoleRadius;
            YPoints_DeadZone_lower_zone1[2] = YShiftOfModules[0]+YPosition-YModuleSize_Plane163x45*0.5 + BeamHoleRadius;
            YPoints_DeadZone_lower_zone1[3] = YShiftOfModules[0]+YPosition-YModuleSize_Plane163x45*0.5;

            lower_layer_zone1.AddDeadZone(NPoints_DeadZone_lower_zone1, XPoints_DeadZone_lower_zone1, YPoints_DeadZone_lower_zone1);
            //----------------------------------------------------------------------

            //upper strip layer (zone 1) -------------------------------------------
            BmnGemStripLayer upper_layer_zone1(1, UpperStripLayer,
                                               XSlopeHotZoneSize_Plane163x45[1], YSlopeHotZoneSize_Plane163x45,
                                               XShiftOfModules[0]+XPosition-XSlopeHotZoneSize_Plane163x45[1], YShiftOfModules[0]+YPosition-YModuleSize_Plane163x45*0.5,
                                               UpperLayerPitch, UpperLayerStripAngle);

            if(UpperLayerStripAngle >= 0.0) {
                upper_layer_zone1.SetStripNumberingOrder(LeftToRight);
                upper_layer_zone1.SetStripNumberingBorders(LeftBottom, RightBottom);
            }
            else {
                upper_layer_zone1.SetStripNumberingOrder(RightToLeft);
                upper_layer_zone1.SetStripNumberingBorders(LeftTop, RightTop);
            }

            //dead zone for upper layer in zone 1 (hole)
            const Int_t NPoints_DeadZone_upper_zone1_hole = 4;
            Double_t XPoints_DeadZone_upper_zone1_hole[NPoints_DeadZone_upper_zone1_hole];
            Double_t YPoints_DeadZone_upper_zone1_hole[NPoints_DeadZone_upper_zone1_hole];

            XPoints_DeadZone_upper_zone1_hole[0] = XShiftOfModules[0]+XPosition-BeamHoleRadius;
            XPoints_DeadZone_upper_zone1_hole[1] = XShiftOfModules[0]+XPosition-BeamHoleRadius;
            XPoints_DeadZone_upper_zone1_hole[2] = XShiftOfModules[0]+XPosition+0.0;
            XPoints_DeadZone_upper_zone1_hole[3] = XShiftOfModules[0]+XPosition+0.0;

            YPoints_DeadZone_upper_zone1_hole[0] = YShiftOfModules[0]+YPosition-YModuleSize_Plane163x45*0.5;
            YPoints_DeadZone_upper_zone1_hole[1] = YShiftOfModules[0]+YPosition-YModuleSize_Plane163x45*0.5 + BeamHoleRadius;
            YPoints_DeadZone_upper_zone1_hole[2] = YShiftOfModules[0]+YPosition-YModuleSize_Plane163x45*0.5 + BeamHoleRadius;
            YPoints_DeadZone_upper_zone1_hole[3] = YShiftOfModules[0]+YPosition-YModuleSize_Plane163x45*0.5;

            upper_layer_zone1.AddDeadZone(NPoints_DeadZone_upper_zone1_hole, XPoints_DeadZone_upper_zone1_hole, YPoints_DeadZone_upper_zone1_hole);

            //dead zone for upper layer in zone 1 (triangle)
            const Int_t NPoints_DeadZone_upper_zone1_triang = 3;
            Double_t XPoints_DeadZone_upper_zone1_triang[NPoints_DeadZone_upper_zone1_triang];
            Double_t YPoints_DeadZone_upper_zone1_triang[NPoints_DeadZone_upper_zone1_triang];

            if(UpperLayerStripAngle >= 0.0) {
                XPoints_DeadZone_upper_zone1_triang[0] = XShiftOfModules[0]+XPosition-XSlopeHotZoneSize_Plane163x45[1];
                XPoints_DeadZone_upper_zone1_triang[1] = XShiftOfModules[0]+XPosition-XSlopeHotZoneSize_Plane163x45[1];
                XPoints_DeadZone_upper_zone1_triang[2] = XShiftOfModules[0]+XPosition-XSlopeHotZoneSize_Plane163x45[0];

                YPoints_DeadZone_upper_zone1_triang[0] = YShiftOfModules[0]+YPosition-YModuleSize_Plane163x45*0.5;
                YPoints_DeadZone_upper_zone1_triang[1] = YShiftOfModules[0]+YPosition-YModuleSize_Plane163x45*0.5 + YSlopeHotZoneSize_Plane163x45;
                YPoints_DeadZone_upper_zone1_triang[2] = YShiftOfModules[0]+YPosition-YModuleSize_Plane163x45*0.5 + YSlopeHotZoneSize_Plane163x45;
            }
            else {
                XPoints_DeadZone_upper_zone1_triang[0] = XShiftOfModules[0]+XPosition-XSlopeHotZoneSize_Plane163x45[1];
                XPoints_DeadZone_upper_zone1_triang[1] = XShiftOfModules[0]+XPosition-XSlopeHotZoneSize_Plane163x45[1];
                XPoints_DeadZone_upper_zone1_triang[2] = XShiftOfModules[0]+XPosition-XSlopeHotZoneSize_Plane163x45[0];

                YPoints_DeadZone_upper_zone1_triang[0] = YShiftOfModules[0]+YPosition-YModuleSize_Plane163x45*0.5;
                YPoints_DeadZone_upper_zone1_triang[1] = YShiftOfModules[0]+YPosition-YModuleSize_Plane163x45*0.5 + YSlopeHotZoneSize_Plane163x45;
                YPoints_DeadZone_upper_zone1_triang[2] = YShiftOfModules[0]+YPosition-YModuleSize_Plane163x45*0.5;
            }

            upper_layer_zone1.AddDeadZone(NPoints_DeadZone_upper_zone1_triang, XPoints_DeadZone_upper_zone1_triang, YPoints_DeadZone_upper_zone1_triang);

            //----------------------------------------------------------------------

            Modules[0]->AddStripLayer(lower_layer_zone0);
            Modules[0]->AddStripLayer(upper_layer_zone0);
            Modules[0]->AddStripLayer(lower_layer_zone1);
            Modules[0]->AddStripLayer(upper_layer_zone1);

    }
//--------------------------------------------------------------------------

//module 1 (right) -------------------------------------------------------------
    {
        Double_t DriftGapThickness = 0.3;
        Double_t FirstTransferGapThickness = 0.25;
        Double_t SecondTransferGapThickness = 0.2;
        Double_t InductionGapThickness = 0.15;
        
        Modules[1] = new BmnGemStripModule(ZPosition+ZShiftOfModules[1], EDriftDirection,
                                           DriftGapThickness, FirstTransferGapThickness,
                                           SecondTransferGapThickness, InductionGapThickness);

            //zone 0 (big) ---------------------------------------------------------

            //lower strip layer (zone 0) -------------------------------------------
            BmnGemStripLayer lower_layer_zone0(0, LowerStripLayer,
                                               XModuleSize_Plane163x45, YModuleSize_Plane163x45,
                                               XShiftOfModules[1]+XPosition+0.0, YShiftOfModules[1]+YPosition-YModuleSize_Plane163x45*0.5,
                                               LowerLayerPitch, LowerLayerStripAngle);

            if(UpperLayerStripAngle >= 0.0) {
                lower_layer_zone0.SetStripNumberingOrder(LeftToRight);
                lower_layer_zone0.SetStripNumberingBorders(LeftTop, RightBottom);
            }
            else {
                lower_layer_zone0.SetStripNumberingOrder(RightToLeft);
                lower_layer_zone0.SetStripNumberingBorders(LeftBottom, RightTop);
            }

            //dead zone for lower layer in zone 0
            const Int_t NPoints_DeadZone_lower_zone0 = 4;
            Double_t XPoints_DeadZone_lower_zone0[NPoints_DeadZone_lower_zone0];
            Double_t YPoints_DeadZone_lower_zone0[NPoints_DeadZone_lower_zone0];

            XPoints_DeadZone_lower_zone0[0] = XShiftOfModules[1]+XPosition+0.0;
            XPoints_DeadZone_lower_zone0[1] = XShiftOfModules[1]+XPosition+0.0;
            XPoints_DeadZone_lower_zone0[2] = XShiftOfModules[1]+XPosition+XRectHotZoneSize_Plane163x45;
            XPoints_DeadZone_lower_zone0[3] = XShiftOfModules[1]+XPosition+XRectHotZoneSize_Plane163x45;

            YPoints_DeadZone_lower_zone0[0] = YShiftOfModules[1]+YPosition-YModuleSize_Plane163x45*0.5;
            YPoints_DeadZone_lower_zone0[1] = YShiftOfModules[1]+YPosition-YModuleSize_Plane163x45*0.5 + YRectHotZoneSize_Plane163x45;
            YPoints_DeadZone_lower_zone0[2] = YShiftOfModules[1]+YPosition-YModuleSize_Plane163x45*0.5 + YRectHotZoneSize_Plane163x45;
            YPoints_DeadZone_lower_zone0[3] = YShiftOfModules[1]+YPosition-YModuleSize_Plane163x45*0.5;

            lower_layer_zone0.AddDeadZone(NPoints_DeadZone_lower_zone0, XPoints_DeadZone_lower_zone0, YPoints_DeadZone_lower_zone0);
            //----------------------------------------------------------------------

            //upper strip layer (zone 0) -------------------------------------------
            BmnGemStripLayer upper_layer_zone0(0, UpperStripLayer,
                                               XModuleSize_Plane163x45, YModuleSize_Plane163x45,
                                               XShiftOfModules[1]+XPosition+0.0, YShiftOfModules[1]+YPosition-YModuleSize_Plane163x45*0.5,
                                               UpperLayerPitch, UpperLayerStripAngle);

            if(UpperLayerStripAngle >= 0.0) {
                upper_layer_zone0.SetStripNumberingOrder(LeftToRight);
                upper_layer_zone0.SetStripNumberingBorders(LeftTop, RightBottom);
            }
            else {
                upper_layer_zone0.SetStripNumberingOrder(RightToLeft);
                upper_layer_zone0.SetStripNumberingBorders(LeftBottom, RightTop);
            }

            //dead zone for upper layer in zone 0
            const Int_t NPoints_DeadZone_upper_zone0 = 4;
            Double_t XPoints_DeadZone_upper_zone0[NPoints_DeadZone_upper_zone0];
            Double_t YPoints_DeadZone_upper_zone0[NPoints_DeadZone_upper_zone0];

            if(UpperLayerStripAngle >= 0.0) {
                XPoints_DeadZone_upper_zone0[0] = XShiftOfModules[1]+XPosition+0.0;
                XPoints_DeadZone_upper_zone0[1] = XShiftOfModules[1]+XPosition+0.0;
                XPoints_DeadZone_upper_zone0[2] = XShiftOfModules[1]+XPosition+XSlopeHotZoneSize_Plane163x45[1];
                XPoints_DeadZone_upper_zone0[3] = XShiftOfModules[1]+XPosition+XSlopeHotZoneSize_Plane163x45[0];

                YPoints_DeadZone_upper_zone0[0] = YShiftOfModules[1]+YPosition-YModuleSize_Plane163x45*0.5;
                YPoints_DeadZone_upper_zone0[1] = YShiftOfModules[1]+YPosition-YModuleSize_Plane163x45*0.5 + YSlopeHotZoneSize_Plane163x45;
                YPoints_DeadZone_upper_zone0[2] = YShiftOfModules[1]+YPosition-YModuleSize_Plane163x45*0.5 + YSlopeHotZoneSize_Plane163x45;
                YPoints_DeadZone_upper_zone0[3] = YShiftOfModules[1]+YPosition-YModuleSize_Plane163x45*0.5;
            }
            else {
                XPoints_DeadZone_upper_zone0[0] = XShiftOfModules[1]+XPosition+0.0;
                XPoints_DeadZone_upper_zone0[1] = XShiftOfModules[1]+XPosition+0.0;
                XPoints_DeadZone_upper_zone0[2] = XShiftOfModules[1]+XPosition+XSlopeHotZoneSize_Plane163x45[0];
                XPoints_DeadZone_upper_zone0[3] = XShiftOfModules[1]+XPosition+XSlopeHotZoneSize_Plane163x45[1];

                YPoints_DeadZone_upper_zone0[0] = YShiftOfModules[1]+YPosition-YModuleSize_Plane163x45*0.5;
                YPoints_DeadZone_upper_zone0[1] = YShiftOfModules[1]+YPosition-YModuleSize_Plane163x45*0.5 + YSlopeHotZoneSize_Plane163x45;
                YPoints_DeadZone_upper_zone0[2] = YShiftOfModules[1]+YPosition-YModuleSize_Plane163x45*0.5 + YSlopeHotZoneSize_Plane163x45;
                YPoints_DeadZone_upper_zone0[3] = YShiftOfModules[1]+YPosition-YModuleSize_Plane163x45*0.5;
            }

            upper_layer_zone0.AddDeadZone(NPoints_DeadZone_upper_zone0, XPoints_DeadZone_upper_zone0, YPoints_DeadZone_upper_zone0);
            //----------------------------------------------------------------------

            //lower strip layer (zone 1) -------------------------------------------
            BmnGemStripLayer lower_layer_zone1(1, LowerStripLayer,
                                               XRectHotZoneSize_Plane163x45, YRectHotZoneSize_Plane163x45,
                                               XShiftOfModules[1]+XPosition+0.0, YShiftOfModules[1]+YPosition-YModuleSize_Plane163x45*0.5,
                                               LowerLayerPitch, LowerLayerStripAngle);

            if(UpperLayerStripAngle >= 0.0) {
                lower_layer_zone1.SetStripNumberingOrder(LeftToRight);
                lower_layer_zone1.SetStripNumberingBorders(LeftTop, RightBottom);
            }
            else {
                lower_layer_zone1.SetStripNumberingOrder(RightToLeft);
                lower_layer_zone1.SetStripNumberingBorders(LeftBottom, RightTop);
            }

            //dead zone for lower layer in zone 1 (hole)
            const Int_t NPoints_DeadZone_lower_zone1 = 4;
            Double_t XPoints_DeadZone_lower_zone1[NPoints_DeadZone_lower_zone1];
            Double_t YPoints_DeadZone_lower_zone1[NPoints_DeadZone_lower_zone1];

            XPoints_DeadZone_lower_zone1[0] = XShiftOfModules[1]+XPosition+0.0;
            XPoints_DeadZone_lower_zone1[1] = XShiftOfModules[1]+XPosition+0.0;
            XPoints_DeadZone_lower_zone1[2] = XShiftOfModules[1]+XPosition+BeamHoleRadius;
            XPoints_DeadZone_lower_zone1[3] = XShiftOfModules[1]+XPosition+BeamHoleRadius;

            YPoints_DeadZone_lower_zone1[0] = YShiftOfModules[1]+YPosition-YModuleSize_Plane163x45*0.5;
            YPoints_DeadZone_lower_zone1[1] = YShiftOfModules[1]+YPosition-YModuleSize_Plane163x45*0.5 + BeamHoleRadius;
            YPoints_DeadZone_lower_zone1[2] = YShiftOfModules[1]+YPosition-YModuleSize_Plane163x45*0.5 + BeamHoleRadius;
            YPoints_DeadZone_lower_zone1[3] = YShiftOfModules[1]+YPosition-YModuleSize_Plane163x45*0.5;

            lower_layer_zone1.AddDeadZone(NPoints_DeadZone_lower_zone1, XPoints_DeadZone_lower_zone1, YPoints_DeadZone_lower_zone1);
            //----------------------------------------------------------------------

            //upper strip layer (zone 1) -------------------------------------------
            BmnGemStripLayer upper_layer_zone1(1, UpperStripLayer,
                                               XSlopeHotZoneSize_Plane163x45[1], YSlopeHotZoneSize_Plane163x45,
                                               XShiftOfModules[1]+XPosition+0.0, YShiftOfModules[1]+YPosition-YModuleSize_Plane163x45*0.5,
                                               UpperLayerPitch, UpperLayerStripAngle);

            if(UpperLayerStripAngle >= 0.0) {
                upper_layer_zone1.SetStripNumberingOrder(LeftToRight);
                upper_layer_zone1.SetStripNumberingBorders(LeftTop, RightTop);
            }
            else {
                upper_layer_zone1.SetStripNumberingOrder(RightToLeft);
                upper_layer_zone1.SetStripNumberingBorders(LeftBottom, RightBottom);
            }

            //dead zone for upper layer in zone 1 (hole)
            const Int_t NPoints_DeadZone_upper_zone1_hole = 4;
            Double_t XPoints_DeadZone_upper_zone1_hole[NPoints_DeadZone_upper_zone1_hole];
            Double_t YPoints_DeadZone_upper_zone1_hole[NPoints_DeadZone_upper_zone1_hole];

            XPoints_DeadZone_upper_zone1_hole[0] = XShiftOfModules[1]+XPosition+0.0;
            XPoints_DeadZone_upper_zone1_hole[1] = XShiftOfModules[1]+XPosition+0.0;
            XPoints_DeadZone_upper_zone1_hole[2] = XShiftOfModules[1]+XPosition+BeamHoleRadius;
            XPoints_DeadZone_upper_zone1_hole[3] = XShiftOfModules[1]+XPosition+BeamHoleRadius;

            YPoints_DeadZone_upper_zone1_hole[0] = YShiftOfModules[1]+YPosition-YModuleSize_Plane163x45*0.5;
            YPoints_DeadZone_upper_zone1_hole[1] = YShiftOfModules[1]+YPosition-YModuleSize_Plane163x45*0.5 + BeamHoleRadius;
            YPoints_DeadZone_upper_zone1_hole[2] = YShiftOfModules[1]+YPosition-YModuleSize_Plane163x45*0.5 + BeamHoleRadius;
            YPoints_DeadZone_upper_zone1_hole[3] = YShiftOfModules[1]+YPosition-YModuleSize_Plane163x45*0.5;

            upper_layer_zone1.AddDeadZone(NPoints_DeadZone_upper_zone1_hole, XPoints_DeadZone_upper_zone1_hole, YPoints_DeadZone_upper_zone1_hole);

            //dead zone for upper layer in zone 1 (triangle)
            const Int_t NPoints_DeadZone_upper_zone1_triang = 3;
            Double_t XPoints_DeadZone_upper_zone1_triang[NPoints_DeadZone_upper_zone1_triang];
            Double_t YPoints_DeadZone_upper_zone1_triang[NPoints_DeadZone_upper_zone1_triang];

            if(UpperLayerStripAngle >= 0.0) {
                XPoints_DeadZone_upper_zone1_triang[0] = XShiftOfModules[1]+XPosition+XSlopeHotZoneSize_Plane163x45[0];
                XPoints_DeadZone_upper_zone1_triang[1] = XShiftOfModules[1]+XPosition+XSlopeHotZoneSize_Plane163x45[1];
                XPoints_DeadZone_upper_zone1_triang[2] = XShiftOfModules[1]+XPosition+XSlopeHotZoneSize_Plane163x45[1];

                YPoints_DeadZone_upper_zone1_triang[0] = YShiftOfModules[1]+YPosition-YModuleSize_Plane163x45*0.5;
                YPoints_DeadZone_upper_zone1_triang[1] = YShiftOfModules[1]+YPosition-YModuleSize_Plane163x45*0.5 + YSlopeHotZoneSize_Plane163x45;
                YPoints_DeadZone_upper_zone1_triang[2] = YShiftOfModules[1]+YPosition-YModuleSize_Plane163x45*0.5;
            }
            else {
                XPoints_DeadZone_upper_zone1_triang[0] = XShiftOfModules[1]+XPosition+XSlopeHotZoneSize_Plane163x45[1];
                XPoints_DeadZone_upper_zone1_triang[1] = XShiftOfModules[1]+XPosition+XSlopeHotZoneSize_Plane163x45[0];
                XPoints_DeadZone_upper_zone1_triang[2] = XShiftOfModules[1]+XPosition+XSlopeHotZoneSize_Plane163x45[1];

                YPoints_DeadZone_upper_zone1_triang[0] = YShiftOfModules[1]+YPosition-YModuleSize_Plane163x45*0.5;
                YPoints_DeadZone_upper_zone1_triang[1] = YShiftOfModules[1]+YPosition-YModuleSize_Plane163x45*0.5 + YSlopeHotZoneSize_Plane163x45;
                YPoints_DeadZone_upper_zone1_triang[2] = YShiftOfModules[1]+YPosition-YModuleSize_Plane163x45*0.5 + YSlopeHotZoneSize_Plane163x45;
            }

            upper_layer_zone1.AddDeadZone(NPoints_DeadZone_upper_zone1_triang, XPoints_DeadZone_upper_zone1_triang, YPoints_DeadZone_upper_zone1_triang);

            //----------------------------------------------------------------------

            Modules[1]->AddStripLayer(lower_layer_zone0);
            Modules[1]->AddStripLayer(upper_layer_zone0);
            Modules[1]->AddStripLayer(lower_layer_zone1);
            Modules[1]->AddStripLayer(upper_layer_zone1);

    }
//--------------------------------------------------------------------------

    return;
}

ClassImp(BmnGemStripStation_RunSpring2017)
