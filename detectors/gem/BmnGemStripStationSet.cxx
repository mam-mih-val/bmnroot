#include "BmnGemStripStationSet.h"

BmnGemStripStationSet::BmnGemStripStationSet()
: NStations(0),
  XStationPositions(nullptr), YStationPositions(nullptr), ZStationPositions(nullptr),
  BeamHoleRadiuses(nullptr),
  GemStations(nullptr), fStatShifts(nullptr)
{
}

BmnGemStripStationSet::BmnGemStripStationSet(TString xml_config_file, map <Int_t, TVector3>* shifts)
: NStations(0),
  XStationPositions(nullptr), YStationPositions(nullptr), ZStationPositions(nullptr),
  BeamHoleRadiuses(nullptr),
  GemStations(nullptr), fStatShifts(shifts)
{
    Bool_t create_status = CreateConfigurationFromXMLFile(xml_config_file);
    if(!create_status) {
        std::cerr << "Error: There are problems with creation of the configuration from XML (in BmnGemStripStationSet)\n";
        throw(StationSet_Exception("Error in the constructor BmnGemStripStationSet()"));
    }
}

BmnGemStripStationSet::BmnGemStripStationSet(Int_t period, BmnSetup stp, map <Int_t, TVector3>* shifts): NStations(0),
  XStationPositions(nullptr), YStationPositions(nullptr), ZStationPositions(nullptr),
  BeamHoleRadiuses(nullptr),
  GemStations(nullptr), fStatShifts(shifts)
{
    TString gPathConfig = getenv("VMCWORKDIR");
    TString xml_config_file;
    switch (period) {
        case 8:
            if (stp == kBMNSETUP) {
                xml_config_file = "GemRun8.xml";
            } else {
                xml_config_file = "GemRunSRC2021.xml";
            }
            break;
        case 7:
            if (stp == kBMNSETUP) {
                xml_config_file = "GemRunSpring2018.xml";
            } else {
                xml_config_file = "GemRunSRCSpring2018.xml";
            }
            break;
        case 6:
            xml_config_file = "GemRunSpring2017.xml";
            break;
        default:
            printf("Error! Unknown config!\n");
            xml_config_file = "";
            break;
    }
    if (xml_config_file.Length()){
        xml_config_file = gPathConfig + "/parameters/gem/XMLConfigs/" + xml_config_file;
        Bool_t create_status = CreateConfigurationFromXMLFile(xml_config_file);
        if(!create_status) {
            std::cerr << "Error: There are problems with creation of the configuration from XML (in BmnGemStripStationSet)\n";
            throw(StationSet_Exception("Error in the constructor BmnGemStripStationSet()"));
        }
    }
}

 BmnGemStripStationSet::~BmnGemStripStationSet()
 {
    if (XStationPositions) {
        delete [] XStationPositions;
        XStationPositions = nullptr;
    }
    if (YStationPositions) {
        delete [] YStationPositions;
        YStationPositions = nullptr;
    }
    if (ZStationPositions) {
        delete [] ZStationPositions;
        ZStationPositions = nullptr;
    }
    if (BeamHoleRadiuses) {
        delete [] BeamHoleRadiuses;
        BeamHoleRadiuses = nullptr;
    }

    for (Int_t i = 0; i < NStations; i++) {
        if (GemStations[i]) {
            delete GemStations[i];
            GemStations[i] = nullptr;
        }
    }
    NStations = 0;

    if (GemStations) {
        delete [] GemStations;
        GemStations = nullptr;
    }

 }

Double_t BmnGemStripStationSet::GetXStationPosition(Int_t station_num) {
    if(XStationPositions && station_num >=0 && station_num < NStations) {
        return XStationPositions[station_num];
    }
    else {
        throw(StationSet_Exception("Error in the function GetXStationPosition()"));
    }
}

Double_t BmnGemStripStationSet::GetYStationPosition(Int_t station_num) {
    if(YStationPositions && station_num >=0 && station_num < NStations) {
        return YStationPositions[station_num];
    }
    else {
        throw(StationSet_Exception("Error in the function GetYStationPosition()"));
    }
}

Double_t BmnGemStripStationSet::GetZStationPosition(Int_t station_num) {
    if(ZStationPositions && station_num >=0 && station_num < NStations) {
        return ZStationPositions[station_num];
    }
    else {
        throw(StationSet_Exception("Error in the function GetZStationPosition()"));
    }
}

Double_t BmnGemStripStationSet::GetBeamHoleRadius(Int_t station_num) {
    if(BeamHoleRadiuses && station_num >=0 && station_num < NStations) {
        return BeamHoleRadiuses[station_num];
    }
    else {
        throw(StationSet_Exception("Error in the function GetBeamHoleRadius()"));
    }
}

BmnGemStripStation* BmnGemStripStationSet::GetGemStation(Int_t station_num) {
    if(GemStations && station_num >= 0 && station_num < NStations) {
        return GemStations[station_num];
    }
    else {
        throw(StationSet_Exception("Error in the function GetGemStation()"));
    }
}

void BmnGemStripStationSet::Reset() {
    for(Int_t istation = 0; istation < NStations; ++istation) {
        GemStations[istation]->Reset();
    }
}

Bool_t BmnGemStripStationSet::AddPointToDetector(Double_t xcoord, Double_t ycoord, Double_t zcoord,
                                      Double_t px, Double_t py, Double_t pz,
                                      Double_t dEloss, Int_t refID) {

    Int_t station = GetPointStationOwnership(xcoord, ycoord, zcoord);

    if(station != -1) {
        if( GemStations[station]->AddPointToStation(xcoord, ycoord, zcoord, px, py, pz, dEloss, refID) != -1 ) return true;
        else return false;
    }
    return false;
}

Int_t BmnGemStripStationSet::CountNAddedToDetectorPoints() {
    Int_t points_sum = 0;
    for(Int_t iStation = 0; iStation < NStations; iStation++) {
        points_sum += GemStations[iStation]->CountNAddedToStationPoints();
    }
    return points_sum;
}

void BmnGemStripStationSet::ProcessPointsInDetector() {
    for(Int_t iStation = 0; iStation < NStations; iStation++) {
        GemStations[iStation]->ProcessPointsInStation();
    }
}

Int_t BmnGemStripStationSet::CountNProcessedPointsInDetector() {
    Int_t points_sum = 0;
    for(Int_t iStation = 0; iStation < NStations; iStation++) {
        points_sum += GemStations[iStation]->CountNProcessedPointInStation();
    }
    return points_sum;
}

Int_t BmnGemStripStationSet::GetPointStationOwnership(Double_t zcoord) {
    /*for(Int_t iStation = 0; iStation < NStations; iStation++) {
        if( (zcoord >= ZStationPositions[iStation]) && (zcoord <= (ZStationPositions[iStation]+GemStations[iStation]->GetZSize())) ) {
            return iStation;
        }
    }*/

    //for z-positions and z-shifts of all modules in a station
    for(Int_t iStation = 0; iStation < NStations; iStation++) {
        Int_t NModules = GemStations[iStation]->GetNModules();
        for(Int_t iModule = 0; iModule < NModules; ++iModule) {
            if( GemStations[iStation]->GetModule(iModule)->IsPointInsideZThickness(zcoord) ) {
                return iStation;
            }
        }
    }
    return -1;
}

Int_t BmnGemStripStationSet::GetPointStationOwnership(Double_t xcoord, Double_t ycoord, Double_t zcoord) {
    //for z-positions and z-shifts of all modules in a station
    for(Int_t iStation = 0; iStation < NStations; iStation++) {
        Int_t NModules = GemStations[iStation]->GetNModules();
        for(Int_t iModule = 0; iModule < NModules; ++iModule) {
            //if( GemStations[iStation]->GetModule(iModule)->IsPointInsideZThickness(zcoord) ) {
            if( GemStations[iStation]->GetModule(iModule)->IsPointInsideModule(xcoord, ycoord, zcoord) ) {
                return iStation;
            }
        }
    }
    return -1;
}

Bool_t BmnGemStripStationSet::CreateConfigurationFromXMLFile(TString xml_config_file) {
    TDOMParser *parser = new TDOMParser();
    parser->SetValidate(false);

    Int_t parse_status = parser->ParseFile(xml_config_file);
    if(parse_status != 0) {
        std::cerr << "Error: An error occured when parsing the file! (in BmnGemStripStationSet)\n";
        return false;
    }

    TXMLNode *node = parser->GetXMLDocument()->GetRootNode();

    if( strcmp(node->GetNodeName(), "StationSet") != 0 ) {
        std::cerr << "Error: Incorrect name of the root element! (in BmnGemStripStationSet)\n";
        return false;
    }

    NStations = CountNumberOfStations(node);

    GemStations = new BmnGemStripStation* [NStations];
    XStationPositions = new Double_t[NStations];
    YStationPositions = new Double_t[NStations];
    ZStationPositions = new Double_t[NStations];
    BeamHoleRadiuses = new Double_t[NStations];

    //default values
    for(Int_t i = 0; i < NStations; ++i) {
        GemStations[i] = 0; //zero-pointer
        XStationPositions[i] = 0.0;
        YStationPositions[i] = 0.0;
        ZStationPositions[i] = 0.0;
        BeamHoleRadiuses[i] = 0.0;
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

Int_t BmnGemStripStationSet::CountNumberOfStations(TXMLNode *node) {
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

Bool_t BmnGemStripStationSet::ParseStation(TXMLNode *node, Int_t iStation) {

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
            if( strcmp(attr->GetName(), "beamHole") == 0 ) {
                BeamHoleRadiuses[iStation] = atof(attr->GetValue());
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

    GemStations[iStation] =
            new BmnGemStripStation(node, iStation,
                XStationPositions[iStation] + dx, YStationPositions[iStation] + dy, ZStationPositions[iStation] + dz,
                BeamHoleRadiuses[iStation]);

    return true;
}

ClassImp(BmnGemStripStationSet)
