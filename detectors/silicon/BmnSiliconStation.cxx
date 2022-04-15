#include "BmnSiliconStation.h"

BmnSiliconStation::BmnSiliconStation()
: StationNumber(0), NModules(0),
  XMinStation(0.0), XMaxStation(0.0),
  YMinStation(0.0), YMaxStation(0.0),
  ZMinStation(0.0), ZMaxStation(0.0),
  XSize(0.0), YSize(0.0), ZSize(0.0),
  XPosition(0.0), YPosition(0.0), ZPosition(0.0),
  RotationAngleDeg(0.0), RotationCenterX(0.0), RotationCenterY(0.0),
  XShiftOfModules(NULL), YShiftOfModules(NULL), ZShiftOfModules(NULL),
  Modules(NULL) { }

BmnSiliconStation::BmnSiliconStation(TXMLNode *stationNode, Int_t iStation,
                                       Double_t xpos_station, Double_t ypos_station, Double_t zpos_station)

: StationNumber(iStation), NModules(0),
  XSize(0.0), YSize(0.0), ZSize(0.0),
  XMinStation(0.0), XMaxStation(0.0),
  YMinStation(0.0), YMaxStation(0.0),
  ZMinStation(0.0), ZMaxStation(0.0),
  XPosition(xpos_station), YPosition(ypos_station), ZPosition(zpos_station),
  RotationAngleDeg(0.0), RotationCenterX(0.0), RotationCenterY(0.0),
  XShiftOfModules(NULL), YShiftOfModules(NULL), ZShiftOfModules(NULL),
  Modules(NULL) {

    Bool_t create_status = CreateConfigurationFromXMLNode(stationNode);
    if(!create_status) {
        std::cerr << "Error: There are problems with creation of the configuration from XML node (in BmnSiliconStation)\n";
    }

    //define station borders and sizes
    DefineStationBorders();

    XSize = XMaxStation-XMinStation;
    YSize = YMaxStation-YMinStation;
    ZSize = ZMaxStation-ZMinStation;
}

BmnSiliconStation::~BmnSiliconStation() {
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
}

Double_t BmnSiliconStation::GetXShiftOfModule(Int_t module_num) {
    if(XShiftOfModules && module_num >= 0 && module_num < NModules) {
        return XShiftOfModules[module_num];
    }
    else {
        throw(Station_Exception("Error in the function GetXShiftOfModule()"));
    }
}

Double_t BmnSiliconStation::GetYShiftOfModule(Int_t module_num) {
    if(YShiftOfModules && module_num >= 0 && module_num < NModules) {
        return YShiftOfModules[module_num];
    }
    else {
        throw(Station_Exception("Error in the function GetYShiftOfModule()"));
    }
}

Double_t BmnSiliconStation::GetZShiftOfModule(Int_t module_num) {
    if(ZShiftOfModules && module_num >= 0 && module_num < NModules) {
        return ZShiftOfModules[module_num];
    }
    else {
        throw(Station_Exception("Error in the function GetZShiftOfModule()"));
    }
}

BmnSiliconModule* BmnSiliconStation::GetModule(Int_t module_num) {
    if(Modules && module_num >= 0 && module_num < NModules) {
        return Modules[module_num];
    }
    else {
        //throw(Station_Exception("Error in the function GetReadoutModule()"));
        return NULL;
    }
}

void BmnSiliconStation::Reset() {
    for(Int_t imodule = 0; imodule < NModules; ++imodule) {
        Modules[imodule]->ResetModuleData();
    }
}

Int_t BmnSiliconStation::AddPointToStation(Double_t xcoord, Double_t ycoord, Double_t zcoord,
                                    Double_t px, Double_t py, Double_t pz,
                                    Double_t dEloss, Int_t refID) {

    Int_t module = GetPointModuleOwnership(xcoord, ycoord, zcoord);

    if(module >= 0) {
        //if(Modules[module]->AddRealPointSimple(xcoord, ycoord, zcoord, px, py, pz, dEloss, refID)) {
        if(Modules[module]->AddRealPointFullOne(xcoord, ycoord, zcoord, px, py, pz, dEloss, refID)) {
            return module;
        }
        else {
            return -1;
        }
    }
    return -1;
}

Int_t BmnSiliconStation::CountNAddedToStationPoints() {
    Int_t points_sum = 0;
    for(Int_t iModule = 0; iModule < NModules; iModule++) {
        points_sum += Modules[iModule]->GetNRealPoints();
    }
    return points_sum;
}

void BmnSiliconStation::ProcessPointsInStation(){
    for(Int_t iModule = 0; iModule < NModules; iModule++) {
        Modules[iModule]->CalculateStripHitIntersectionPoints();
    }
}

Int_t BmnSiliconStation::CountNProcessedPointInStation() {
    Int_t points_sum = 0;
    for(Int_t iModule = 0; iModule < NModules; iModule++) {
        points_sum += Modules[iModule]->GetNIntersectionPoints();
    }
    return points_sum;
}

Int_t BmnSiliconStation::GetPointModuleOwnership(Double_t xcoord, Double_t ycoord, Double_t zcoord) {
    for(Int_t imodule = 0; imodule < NModules; ++imodule) {
        if( Modules[imodule]->IsPointInsideModule(xcoord, ycoord, zcoord) ) return imodule;
    }
    return -1;
}

void BmnSiliconStation::DefineStationBorders() {
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

Bool_t BmnSiliconStation::CreateConfigurationFromXMLNode(TXMLNode *node) {
    NModules = CountNumberOfModules(node);

    Modules = new BmnSiliconModule* [NModules];
    XShiftOfModules = new Double_t[NModules];
    YShiftOfModules = new Double_t[NModules];
    ZShiftOfModules = new Double_t[NModules];

    //default values
    for(Int_t i = 0; i < NModules; ++i) {
        Modules[i] = 0; //zero-pointer
        XShiftOfModules[i] = 0.0;
        YShiftOfModules[i] = 0.0;
        ZShiftOfModules[i] = 0.0;
    }

    //for station rotation parameters
    if( node->HasAttributes() ) {
        TList *attrList = node->GetAttributes();
        TXMLAttr *attr = 0;
        TIter next(attrList);

        while( attr = (TXMLAttr*)next() ) {
            if( strcmp(attr->GetName(), "rotAngleDeg") == 0 ) {
                RotationAngleDeg = atof(attr->GetValue()); //clockwise direction is plus, if we look at the module in the direction of the beam axis
            }
            if( strcmp(attr->GetName(), "rotCenterX") == 0 ) {
                RotationCenterX = -atof(attr->GetValue()); //inverted (from global to local system)
            }
            if( strcmp(attr->GetName(), "rotCenterY") == 0 ) {
                RotationCenterY = atof(attr->GetValue());
            }
        }
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

Int_t BmnSiliconStation::CountNumberOfModules(TXMLNode *node) {
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

Bool_t BmnSiliconStation::ParseModule(TXMLNode *node, Int_t iModule) {

    if( node->HasAttributes() ) {
        TList *attrList = node->GetAttributes();
        TXMLAttr *attr = 0;
        TIter next(attrList);

        while( attr = (TXMLAttr*)next() ) {
            if( strcmp(attr->GetName(), "xShift") == 0 ) {
                XShiftOfModules[iModule] = -atof(attr->GetValue()); //inverted (from global to local system)
            }
            if( strcmp(attr->GetName(), "yShift") == 0 ) {
                YShiftOfModules[iModule] = atof(attr->GetValue());
            }
            if( strcmp(attr->GetName(), "zShift") == 0 ) {
                ZShiftOfModules[iModule] = atof(attr->GetValue());
            }
        }
    }

    Modules[iModule] = new BmnSiliconModule(ZPosition+ZShiftOfModules[iModule]);
    Modules[iModule]->SetModuleRotation(RotationAngleDeg, RotationCenterX, RotationCenterY);

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

BmnSiliconLayer BmnSiliconStation::ParseLayer(TXMLNode *node, Int_t iLayer, Int_t iModule) {
    Int_t zone_id; //id-number of a zone in the module (e.g. big or small)
    StripLayerType layer_type; // lower or upper strips
    Int_t first_strip_number; //a number that numbered strips start with
    Double_t adeg; //strip angle (positive value in degrees is clockwise from vertical)
    Double_t pitch;
    Double_t xsize, ysize, xorig, yorig;
    StripNumberingDirection strip_direction;
    Double_t lx_border, ly_border; //left border
    Double_t rx_border, ry_border; //right border

    if( node->HasAttributes() ) {
        TList *attrList = node->GetAttributes();
        TXMLAttr *attr = 0;
        TIter next(attrList);

        while( attr = (TXMLAttr*)next() ) {
            if( strcmp(attr->GetName(), "zoneID") == 0 ) {
                zone_id = atoi(attr->GetValue());
            }
            if( strcmp(attr->GetName(), "type") == 0 ) {
                if( strcmp(attr->GetValue(), "lower") == 0 ) {
                    layer_type = LowerStripLayer;
                }
                if( strcmp(attr->GetValue(), "upper") == 0 ) {
                    layer_type = UpperStripLayer;
                }
            }
            if( strcmp(attr->GetName(), "firstStripNumber") == 0 ) {
                first_strip_number = atoi(attr->GetValue());
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
            if( strcmp(attr->GetName(), "lxborder") == 0 ) {
                lx_border = -atof(attr->GetValue()); //inverted
            }
            if( strcmp(attr->GetName(), "lyborder") == 0 ) {
                ly_border = atof(attr->GetValue());
            }
            if( strcmp(attr->GetName(), "rxborder") == 0 ) {
                rx_border = -atof(attr->GetValue()); //inverted
            }
            if( strcmp(attr->GetName(), "ryborder") == 0 ) {
                ry_border = atof(attr->GetValue());
            }
        }
    }

    BmnSiliconLayer layer(zone_id, layer_type, first_strip_number,
                          xsize, ysize,
                          XShiftOfModules[iModule]+XPosition+xorig, YShiftOfModules[iModule]+YPosition+yorig,
                          pitch, adeg);

    layer.SetStripNumberingOrder(strip_direction);

    layer.SetStripNumberingBorders(XShiftOfModules[iModule]+XPosition+lx_border,
                                   YShiftOfModules[iModule]+YPosition+ly_border,
                                   XShiftOfModules[iModule]+XPosition+rx_border,
                                   YShiftOfModules[iModule]+YPosition+ry_border);

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

DeadZoneOfStripLayer BmnSiliconStation::ParseDeadZone(TXMLNode *node, Int_t iModule) {
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

Int_t BmnSiliconStation::CountDeadZonePoints(TXMLNode *node) {
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

//------------------------------------------------------------------------------

ClassImp(BmnSiliconStation)