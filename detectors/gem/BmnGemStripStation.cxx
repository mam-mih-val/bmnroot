#include "BmnGemStripStation.h"

BmnGemStripStation::BmnGemStripStation()
: StationNumber(0), NModules(0),
  XMinStation(0.0), XMaxStation(0.0),
  YMinStation(0.0), YMaxStation(0.0),
  ZMinStation(0.0), ZMaxStation(0.0),
  XSize(0.0), YSize(0.0), ZSize(0.0),
  XPosition(0.0), YPosition(0.0), ZPosition(0.0),
  XShiftOfModules(NULL), YShiftOfModules(NULL), ZShiftOfModules(NULL),
  DriftGapThicknessOfModules(NULL), FTransfGapThicknessOfModules(NULL), STransfGapThicknessOfModules(NULL), InductionGapThicknessOfModules(NULL),
  BeamHoleRadius(0.0),
  Modules(NULL) { }

BmnGemStripStation::BmnGemStripStation(TXMLNode *stationNode, Int_t iStation,
                                       Double_t xpos_station, Double_t ypos_station, Double_t zpos_station,
                                       Double_t beamradius)

: StationNumber(iStation), NModules(0),
  XSize(0.0), YSize(0.0), ZSize(0.0),
  XMinStation(0.0), XMaxStation(0.0),
  YMinStation(0.0), YMaxStation(0.0),
  ZMinStation(0.0), ZMaxStation(0.0),
  XPosition(xpos_station), YPosition(ypos_station), ZPosition(zpos_station),
  XShiftOfModules(NULL), YShiftOfModules(NULL), ZShiftOfModules(NULL),
  DriftGapThicknessOfModules(NULL), FTransfGapThicknessOfModules(NULL), STransfGapThicknessOfModules(NULL), InductionGapThicknessOfModules(NULL),
  BeamHoleRadius(beamradius),
  Modules(NULL) {

    Bool_t create_status = CreateConfigurationFromXMLNode(stationNode);
    if(!create_status) {
        std::cerr << "Error: There are problems with creation of the configuration from XML node (in BmnGemStripStation)\n";
    }

    //define station borders and sizes
    DefineStationBorders();

    XSize = XMaxStation-XMinStation;
    YSize = YMaxStation-YMinStation;
    ZSize = ZMaxStation-ZMinStation;
}

BmnGemStripStation::~BmnGemStripStation() {
    for (Int_t i = 0; i < NModules; ++i) {
        if (Modules[i]) {
            delete Modules[i];
            Modules[i] = NULL;
        }
    }
    NModules = 0;

    if (Modules) {
        delete [] Modules;
        Modules = NULL;
    }
    if (XShiftOfModules) {
        delete [] XShiftOfModules;
        XShiftOfModules = NULL;
    }
    if (YShiftOfModules) {
        delete [] YShiftOfModules;
        YShiftOfModules = NULL;
    }
    if (ZShiftOfModules) {
        delete [] ZShiftOfModules;
        ZShiftOfModules = NULL;
    }

    if (DriftGapThicknessOfModules) {
        delete [] DriftGapThicknessOfModules;
        DriftGapThicknessOfModules = NULL;
    }
    if (FTransfGapThicknessOfModules) {
        delete [] FTransfGapThicknessOfModules;
        FTransfGapThicknessOfModules = NULL;
    }
    if (STransfGapThicknessOfModules) {
        delete [] STransfGapThicknessOfModules;
        STransfGapThicknessOfModules = NULL;
    }
    if (InductionGapThicknessOfModules) {
        delete [] InductionGapThicknessOfModules;
        InductionGapThicknessOfModules = NULL;
    }
}

Double_t BmnGemStripStation::GetXShiftOfModule(Int_t module_num) {
    if(XShiftOfModules && module_num >= 0 && module_num < NModules) {
        return XShiftOfModules[module_num];
    }
    else {
        throw(Station_Exception("Error in the function GetXShiftOfModule()"));
    }
}

Double_t BmnGemStripStation::GetYShiftOfModule(Int_t module_num) {
    if(YShiftOfModules && module_num >= 0 && module_num < NModules) {
        return YShiftOfModules[module_num];
    }
    else {
        throw(Station_Exception("Error in the function GetYShiftOfModule()"));
    }
}

Double_t BmnGemStripStation::GetZShiftOfModule(Int_t module_num) {
    if(ZShiftOfModules && module_num >= 0 && module_num < NModules) {
        return ZShiftOfModules[module_num];
    }
    else {
        throw(Station_Exception("Error in the function GetZShiftOfModule()"));
    }
}

Double_t BmnGemStripStation::GetDriftGapThicknessOfModule(Int_t module_num) {
    if(DriftGapThicknessOfModules && module_num >= 0 && module_num < NModules) {
        return DriftGapThicknessOfModules[module_num];
    }
    else {
        throw(Station_Exception("Error in the function GetDriftGapThicknessOfModule()"));
    }
}

Double_t BmnGemStripStation::GetFTransfGapThicknessOfModule(Int_t module_num) {
    if(FTransfGapThicknessOfModules && module_num >= 0 && module_num < NModules) {
        return FTransfGapThicknessOfModules[module_num];
    }
    else {
        throw(Station_Exception("Error in the function GetFTransfGapThicknessOfModule()"));
    }
}

Double_t BmnGemStripStation::GetSTransfGapThicknessOfModule(Int_t module_num) {
    if(STransfGapThicknessOfModules && module_num >= 0 && module_num < NModules) {
        return STransfGapThicknessOfModules[module_num];
    }
    else {
        throw(Station_Exception("Error in the function GetSTransfGapThicknessOfModule()"));
    }
}

Double_t BmnGemStripStation::GetInductionGapThicknessOfModule(Int_t module_num) {
    if(InductionGapThicknessOfModules && module_num >= 0 && module_num < NModules) {
        return InductionGapThicknessOfModules[module_num];
    }
    else {
        throw(Station_Exception("Error in the function GetInductionGapThicknessOfModule()"));
    }
}

BmnGemStripModule* BmnGemStripStation::GetModule(Int_t module_num) {
    if(Modules && module_num >= 0 && module_num < NModules) {
        return Modules[module_num];
    }
    else {
        throw(Station_Exception("Error in the function GetReadoutModule()"));
    }
}

void BmnGemStripStation::Reset() {
    for(Int_t imodule = 0; imodule < NModules; ++imodule) {
        Modules[imodule]->ResetModuleData();
    }
}

Int_t BmnGemStripStation::AddPointToStation(Double_t xcoord, Double_t ycoord, Double_t zcoord,
                                    Double_t px, Double_t py, Double_t pz,
                                    Double_t dEloss, Int_t refID) {

    Int_t module = GetPointModuleOwnership(xcoord, ycoord, zcoord);

    if(module >= 0) {
        //if(ReadoutModules[module]->AddRealPointFullOne(xcoord, ycoord, zcoord, dEloss, refID)) {
        if(Modules[module]->AddRealPointFull(xcoord, ycoord, zcoord, px, py, pz, dEloss, refID)) {
//        if(Modules[module]->AddRealPointSimple(xcoord, ycoord, zcoord, px, py, pz, dEloss, refID)) {
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
    for(Int_t iModule = 0; iModule < NModules; iModule++) {
        points_sum += Modules[iModule]->GetNRealPoints();
    }
    return points_sum;
}

void BmnGemStripStation::ProcessPointsInStation(){
    for(Int_t iModule = 0; iModule < NModules; iModule++) {
        Modules[iModule]->CalculateStripHitIntersectionPoints();
    }
}

Int_t BmnGemStripStation::CountNProcessedPointInStation() {
    Int_t points_sum = 0;
    for(Int_t iModule = 0; iModule < NModules; iModule++) {
        points_sum += Modules[iModule]->GetNIntersectionPoints();
    }
    return points_sum;
}

Int_t BmnGemStripStation::GetPointModuleOwnership(Double_t xcoord, Double_t ycoord, Double_t zcoord) {
    for(Int_t imodule = 0; imodule < NModules; ++imodule) {
        if( Modules[imodule]->IsPointInsideModule(xcoord, ycoord, zcoord) ) return imodule;
    }

    return -1;
}

void BmnGemStripStation::DefineStationBorders() {
    XMinStation = 1.0E+10;
    XMaxStation = -1.0E+10;
    YMinStation = 1.0E+10;
    YMaxStation = -1.0E+10;
    ZMinStation = 1.0E+10;
    ZMaxStation = -1.0E+10;

    for(Int_t i = 0; i < NModules; ++i) {
        if( Modules[i]->GetXMinModule() < XMinStation ) XMinStation = Modules[i]->GetXMinModule();
        if( Modules[i]->GetXMaxModule() > XMaxStation ) XMaxStation = Modules[i]->GetXMaxModule();
        if( Modules[i]->GetYMinModule() < YMinStation ) YMinStation = Modules[i]->GetYMinModule();
        if( Modules[i]->GetYMaxModule() > YMaxStation ) YMaxStation = Modules[i]->GetYMaxModule();

        Double_t zstart = Modules[i]->GetZStartModulePosition();
        Double_t zend = Modules[i]->GetZStartModulePosition() + Modules[i]->GetModuleThickness();
        if( zstart < ZMinStation ) ZMinStation = zstart;
        if( zend > ZMaxStation ) ZMaxStation = zend;
    }
}

Bool_t BmnGemStripStation::CreateConfigurationFromXMLNode(TXMLNode *node) {
    NModules = CountNumberOfModules(node);

    Modules = new BmnGemStripModule* [NModules];
    XShiftOfModules = new Double_t[NModules];
    YShiftOfModules = new Double_t[NModules];
    ZShiftOfModules = new Double_t[NModules];

    DriftGapThicknessOfModules = new Double_t[NModules];
    FTransfGapThicknessOfModules = new Double_t[NModules];
    STransfGapThicknessOfModules = new Double_t[NModules];
    InductionGapThicknessOfModules = new Double_t[NModules];

    //default values
    for(Int_t i = 0; i < NModules; ++i) {
        Modules[i] = 0; //zero-pointer
        XShiftOfModules[i] = 0.0;
        YShiftOfModules[i] = 0.0;
        ZShiftOfModules[i] = 0.0;

        DriftGapThicknessOfModules[i] = 0.3;
        FTransfGapThicknessOfModules[i] = 0.25;
        STransfGapThicknessOfModules[i] = 0.2;
        InductionGapThicknessOfModules[i] = 0.15;
    }

    node = node->GetChildren();
    Int_t currentModuleNum = 0;
    while(node) {
        if( strcmp(node->GetNodeName(), "Module") == 0 ) {
            Bool_t parse_status = ParseModule(node, currentModuleNum);
            if(!parse_status) return false;
            currentModuleNum++;
        }
        node = node->GetNextNode();
    }

    return true;
}

Int_t BmnGemStripStation::CountNumberOfModules(TXMLNode *node) {
    Int_t module_cnt = 0;
    node = node->GetChildren();
    while(node) {
        if( strcmp(node->GetNodeName(), "Module") == 0 ) {
            module_cnt++;
        }
        node = node->GetNextNode();
    }
    return module_cnt;
}

Bool_t BmnGemStripStation::ParseModule(TXMLNode *node, Int_t iModule) {

    ElectronDriftDirectionInModule edrift_direction = ForwardZAxisEDrift; //default

    if( node->HasAttributes() ) {
        TList *attrList = node->GetAttributes();
        TXMLAttr *attr = 0;
        TIter next(attrList);

        while( attr = (TXMLAttr*)next() ) {
            if( strcmp(attr->GetName(), "xShift") == 0 ) {
                XShiftOfModules[iModule] = -atof(attr->GetValue()); //inverted
            }
            if( strcmp(attr->GetName(), "yShift") == 0 ) {
                YShiftOfModules[iModule] = atof(attr->GetValue());
            }
            if( strcmp(attr->GetName(), "zShift") == 0 ) {
                ZShiftOfModules[iModule] = atof(attr->GetValue());
            }
            if( strcmp(attr->GetName(), "driftDirection") == 0 ) {
                if( strcmp(attr->GetValue(), "forward") == 0 ) {
                    edrift_direction = ForwardZAxisEDrift;
                }
                if( strcmp(attr->GetValue(), "backward") == 0 ) {
                    edrift_direction = BackwardZAxisEDrift;
                }
            }

            if( strcmp(attr->GetName(), "gap_drift_thick") == 0 ) {
                DriftGapThicknessOfModules[iModule] = atof(attr->GetValue());
            }
            if( strcmp(attr->GetName(), "gap_ftransf_thick") == 0 ) {
                FTransfGapThicknessOfModules[iModule] = atof(attr->GetValue());
            }
            if( strcmp(attr->GetName(), "gap_stransf_thick") == 0 ) {
                STransfGapThicknessOfModules[iModule] = atof(attr->GetValue());
            }
            if( strcmp(attr->GetName(), "gap_induct_thick") == 0 ) {
                InductionGapThicknessOfModules[iModule] = atof(attr->GetValue());
            }
        }
    }

    Modules[iModule] = new BmnGemStripModule(ZPosition+ZShiftOfModules[iModule], edrift_direction,
                                             DriftGapThicknessOfModules[iModule], FTransfGapThicknessOfModules[iModule],
                                             STransfGapThicknessOfModules[iModule], InductionGapThicknessOfModules[iModule]);

    //Layers
    node = node->GetChildren();
    Int_t currentLayerNum = 0;
    while(node) {
        if( strcmp(node->GetNodeName(), "Layer") == 0 ) {
            Modules[iModule]->AddStripLayer(ParseLayer(node, currentLayerNum, iModule));
            currentLayerNum++;
        }
        node = node->GetNextNode();
    }

    return true;
}

BmnGemStripLayer BmnGemStripStation::ParseLayer(TXMLNode *node, Int_t iLayer, Int_t iModule) {
    Int_t zone_number; //id-number of a zone in the module (e.g. big or small)
    StripLayerType layer_type;
    Double_t adeg; //strip angle (positive value in degrees is clockwise from vertical)
    Double_t pitch;
    Double_t xsize, ysize, xorig, yorig;
    StripNumberingDirection strip_direction;
    StripBorderPoint left_border, right_border;

    if( node->HasAttributes() ) {
        TList *attrList = node->GetAttributes();
        TXMLAttr *attr = 0;
        TIter next(attrList);

        while( attr = (TXMLAttr*)next() ) {
            if( strcmp(attr->GetName(), "zoneNumber") == 0 ) {
                zone_number = atoi(attr->GetValue());
            }
            if( strcmp(attr->GetName(), "type") == 0 ) {
                if( strcmp(attr->GetValue(), "lower") == 0 ) {
                    layer_type = LowerStripLayer;
                }
                if( strcmp(attr->GetValue(), "upper") == 0 ) {
                    layer_type = UpperStripLayer;
                }
            }
            if( strcmp(attr->GetName(), "stripAngle") == 0 ) {
                adeg = atof(attr->GetValue());
            }
            if( strcmp(attr->GetName(), "pitch") == 0 ) {
                pitch = atof(attr->GetValue());
            }
            if( strcmp(attr->GetName(), "xorig") == 0 ) {
                xorig = -atof(attr->GetValue()); //inverted
            }
            if( strcmp(attr->GetName(), "yorig") == 0 ) {
                yorig = atof(attr->GetValue());
            }
            if( strcmp(attr->GetName(), "xsize") == 0 ) {
                xsize = atof(attr->GetValue());
            }
            if( strcmp(attr->GetName(), "ysize") == 0 ) {
                ysize = atof(attr->GetValue());
            }
            if( strcmp(attr->GetName(), "stripDirection") == 0 ) {
                if( strcmp(attr->GetValue(), "LeftToRight") == 0 ) {
                    strip_direction = LeftToRight;
                }
                if( strcmp(attr->GetValue(), "RightToLeft") == 0 ) {
                    strip_direction = RightToLeft;
                }
            }
            if( strcmp(attr->GetName(), "lborder") == 0 ) {
                if( strcmp(attr->GetValue(), "LeftTop") == 0 ) {
                    left_border = LeftTop;
                }
                if( strcmp(attr->GetValue(), "LeftBottom") == 0 ) {
                    left_border = LeftBottom;
                }
            }
            if( strcmp(attr->GetName(), "rborder") == 0 ) {
                if( strcmp(attr->GetValue(), "RightTop") == 0 ) {
                    right_border = RightTop;
                }
                if( strcmp(attr->GetValue(), "RightBottom") == 0 ) {
                    right_border = RightBottom;
                }
            }
        }
    }

    BmnGemStripLayer layer(zone_number, layer_type,
                           xsize, ysize,
                           XShiftOfModules[iModule]+XPosition+xorig, YShiftOfModules[iModule]+YPosition+yorig,
                           pitch, adeg);

    layer.SetStripNumberingOrder(strip_direction);
    layer.SetStripNumberingBorders(left_border, right_border);

    //Dead zones
    node = node->GetChildren();
    Int_t currentDeadZoneNum = 0;
    while(node) {
        if( strcmp(node->GetNodeName(), "DeadZone") == 0 ) {
            layer.AddDeadZone(ParseDeadZone(node, iModule));
            currentDeadZoneNum++;
        }
        node = node->GetNextNode();
    }

    return layer;
}

DeadZoneOfStripLayer BmnGemStripStation::ParseDeadZone(TXMLNode *node, Int_t iModule) {
    DeadZoneOfStripLayer dead_zone;

    Int_t n_dead_points = CountDeadZonePoints(node);
    Double_t *xpoints = new Double_t[n_dead_points];
    Double_t *ypoints = new Double_t[n_dead_points];

    //Dead points
    node = node->GetChildren();
    Int_t currentDeadPointNum = 0;
    while(node) {
        if( strcmp(node->GetNodeName(), "DeadPoint") == 0 ) {

            if( node->HasAttributes() ) {
                TList *attrList = node->GetAttributes();
                TXMLAttr *attr = 0;
                TIter next(attrList);

                while( attr = (TXMLAttr*)next() ) {
                    if( strcmp(attr->GetName(), "x") == 0 ) {
                        xpoints[currentDeadPointNum] = -atof(attr->GetValue())+XShiftOfModules[iModule]+XPosition; //inverted
                    }
                    if( strcmp(attr->GetName(), "y") == 0 ) {
                        ypoints[currentDeadPointNum] = atof(attr->GetValue())+YShiftOfModules[iModule]+YPosition;
                    }
                }
            }

            currentDeadPointNum++;
        }
        node = node->GetNextNode();
    }

    dead_zone.SetDeadZone(n_dead_points, xpoints, ypoints);

    delete [] xpoints;
    delete [] ypoints;

    return dead_zone;
}

Int_t BmnGemStripStation::CountDeadZonePoints(TXMLNode *node) {
    Int_t dead_point_cnt = 0;
    node = node->GetChildren();
    while(node) {
        if( strcmp(node->GetNodeName(), "DeadPoint") == 0 ) {
            dead_point_cnt++;
        }
        node = node->GetNextNode();
    }
    return dead_point_cnt;
}

ClassImp(BmnGemStripStation)
