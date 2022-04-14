#include "BmnCSCStationSet.h"

BmnCSCStationSet::BmnCSCStationSet()
: NStations(0),
  XStationPositions(NULL), YStationPositions(NULL), ZStationPositions(NULL),
  Stations(NULL) { }

BmnCSCStationSet::BmnCSCStationSet(TString xml_config_file, map <Int_t, TVector3>* shifts)
: NStations(0),
  XStationPositions(NULL), YStationPositions(NULL), ZStationPositions(NULL),
  Stations(NULL), fStatShifts(shifts) {

    Bool_t create_status = CreateConfigurationFromXMLFile(xml_config_file);
    if(!create_status) {
        std::cerr << "Error: There are problems with creation of the configuration from XML (in BmnCSCStationSet)\n";
        throw(StationSet_Exception("Error in the constructor BmnCSCStationSet()"));
    }
}

 BmnCSCStationSet::~BmnCSCStationSet() {

    if (XStationPositions) {
        delete [] XStationPositions;
        XStationPositions = NULL;
    }
    if (YStationPositions) {
        delete [] YStationPositions;
        YStationPositions = NULL;
    }
    if (ZStationPositions) {
        delete [] ZStationPositions;
        ZStationPositions = NULL;
    }

    for (Int_t i = 0; i < NStations; i++) {
        if (Stations[i]) {
            delete Stations[i];
            Stations[i] = NULL;
        }
    }
    NStations = 0;

    if (Stations) {
        delete [] Stations;
        Stations = NULL;
    }

 }

Double_t BmnCSCStationSet::GetXStationPosition(Int_t station_num) {
    if(XStationPositions && station_num >=0 && station_num < NStations) {
        return XStationPositions[station_num];
    }
    else {
        throw(StationSet_Exception("Error in the function GetXStationPosition()"));
    }
}

Double_t BmnCSCStationSet::GetYStationPosition(Int_t station_num) {
    if(YStationPositions && station_num >=0 && station_num < NStations) {
        return YStationPositions[station_num];
    }
    else {
        throw(StationSet_Exception("Error in the function GetYStationPosition()"));
    }
}

Double_t BmnCSCStationSet::GetZStationPosition(Int_t station_num) {
    if(ZStationPositions && station_num >=0 && station_num < NStations) {
        return ZStationPositions[station_num];
    }
    else {
        throw(StationSet_Exception("Error in the function GetZStationPosition()"));
    }
}

BmnCSCStation* BmnCSCStationSet::GetStation(Int_t station_num) {
    if(Stations && station_num >= 0 && station_num < NStations) {
        return Stations[station_num];
    }
    else {
        throw(StationSet_Exception("Error in the function GetStation()"));
    }
}

void BmnCSCStationSet::Reset() {
    for(Int_t istation = 0; istation < NStations; ++istation) {
        Stations[istation]->Reset();
    }
}

Bool_t BmnCSCStationSet::AddPointToDetector(Double_t xcoord, Double_t ycoord, Double_t zcoord,
                                            Double_t px, Double_t py, Double_t pz,
                                            Double_t dEloss, Int_t refID) {

    Int_t station = GetPointStationOwnership(xcoord, ycoord, zcoord);

    if(station != -1) {
        if( Stations[station]->AddPointToStation(xcoord, ycoord, zcoord, px, py, pz, dEloss, refID) != -1 ) return true;
        else return false;
    }
    return false;
}

Int_t BmnCSCStationSet::CountNAddedToDetectorPoints() {
    Int_t points_sum = 0;
    for(Int_t iStation = 0; iStation < NStations; iStation++) {
        points_sum += Stations[iStation]->CountNAddedToStationPoints();
    }
    return points_sum;
}

void BmnCSCStationSet::ProcessPointsInDetector() {
    for(Int_t iStation = 0; iStation < NStations; iStation++) {
        Stations[iStation]->ProcessPointsInStation();
    }
}

Int_t BmnCSCStationSet::CountNProcessedPointsInDetector() {
    Int_t points_sum = 0;
    for(Int_t iStation = 0; iStation < NStations; iStation++) {
        points_sum += Stations[iStation]->CountNProcessedPointInStation();
    }
    return points_sum;
}

Int_t BmnCSCStationSet::GetPointStationOwnership(Double_t zcoord) {
    //for z-positions and z-shifts of all modules in a station
    for(Int_t iStation = 0; iStation < NStations; iStation++) {
        Int_t NModules = Stations[iStation]->GetNModules();
        for(Int_t iModule = 0; iModule < NModules; ++iModule) {
            if( Stations[iStation]->GetModule(iModule)->IsPointInsideZThickness(zcoord) ) {
                return iStation;
            }
        }
    }
    return -1;
}

Int_t BmnCSCStationSet::GetPointStationOwnership(Double_t xcoord, Double_t ycoord, Double_t zcoord) {
    //for z-positions and z-shifts of all modules in a station
    for(Int_t iStation = 0; iStation < NStations; iStation++) {
        Int_t NModules = Stations[iStation]->GetNModules();
        for(Int_t iModule = 0; iModule < NModules; ++iModule) {
            //if( Stations[iStation]->GetModule(iModule)->IsPointInsideZThickness(zcoord) ) {
            if( Stations[iStation]->GetModule(iModule)->IsPointInsideModule(xcoord, ycoord, zcoord) ) {
                return iStation;
            }
        }
    }
    return -1;
}

Bool_t BmnCSCStationSet::CreateConfigurationFromXMLFile(TString xml_config_file) {
    TDOMParser *parser = new TDOMParser();
    parser->SetValidate(false);

    Int_t parse_status = parser->ParseFile(xml_config_file);
    if(parse_status != 0) {
        std::cerr << "Error: An error occured when parsing the file! (in BmnCSCStationSet)\n";
        return false;
    }

    TXMLNode *node = parser->GetXMLDocument()->GetRootNode();

    if( strcmp(node->GetNodeName(), "StationSet") != 0 ) {
        std::cerr << "Error: Incorrect name of the root element! (in BmnCSCStationSet)\n";
        return false;
    }

    NStations = CountNumberOfStations(node);

    Stations = new BmnCSCStation* [NStations];
    XStationPositions = new Double_t[NStations];
    YStationPositions = new Double_t[NStations];
    ZStationPositions = new Double_t[NStations];

    //default values
    for(Int_t i = 0; i < NStations; ++i) {
        Stations[i] = 0; //zero-pointer
        XStationPositions[i] = 0.0;
        YStationPositions[i] = 0.0;
        ZStationPositions[i] = 0.0;
    }

    node = node->GetChildren();
    Int_t currentStationNum = 0;
    while(node) {
        if( strcmp(node->GetNodeName(), "Station") == 0 ) {
            Bool_t b_parse_status = ParseStation(node, currentStationNum);
            if(!b_parse_status) return false;
            currentStationNum++;
        }
        node = node->GetNextNode();
    }

    delete parser;

    return true;
}

Int_t BmnCSCStationSet::CountNumberOfStations(TXMLNode *node) {
    Int_t station_cnt = 0;
    node = node->GetChildren();
    while(node) {
        if( strcmp(node->GetNodeName(), "Station") == 0 ) {
            station_cnt++;
        }
        node = node->GetNextNode();
    }
    return station_cnt;
}

Bool_t BmnCSCStationSet::ParseStation(TXMLNode *node, Int_t iStation) {

    if( node->HasAttributes() ) {
        TList *attrList = node->GetAttributes();
        TXMLAttr *attr = 0;
        TIter next(attrList);

        while( attr = (TXMLAttr*)next() ) {
            if( strcmp(attr->GetName(), "xPosition") == 0 ) {
                XStationPositions[iStation] = -atof(attr->GetValue()); //inverted
            }
            if( strcmp(attr->GetName(), "yPosition") == 0 ) {
                YStationPositions[iStation] = atof(attr->GetValue());
            }
            if( strcmp(attr->GetName(), "zPosition") == 0 ) {
                ZStationPositions[iStation] = atof(attr->GetValue());
            }
        }
    }

    Double_t dx = 0.;
    Double_t dy = 0.;
    Double_t dz = 0.;

    if (fStatShifts)
        for (auto it : *fStatShifts) {
            Int_t stat = it.first;
            if (iStation == stat) {
                dx = it.second.X();
                dy = it.second.Y();
                dz = it.second.Z();
                break;
            }
        }

    Stations[iStation] =
            new BmnCSCStation(node, iStation,
                XStationPositions[iStation] + dx, YStationPositions[iStation] + dy, ZStationPositions[iStation] + dz);

    return true;
}

ClassImp(BmnCSCStationSet)
