#include "BmnSiliconStationSet.h"

BmnSiliconStationSet::BmnSiliconStationSet()
: NStations(0),
  XStationPositions(NULL), YStationPositions(NULL), ZStationPositions(NULL),
  SiliconStations(NULL),
  fStatShifts(NULL)
{}

BmnSiliconStationSet::BmnSiliconStationSet(TString xml_config_file, map <Int_t, TVector3>* shifts)
: NStations(0),
  XStationPositions(NULL), YStationPositions(NULL), ZStationPositions(NULL),
  SiliconStations(NULL), fStatShifts(shifts)
{
    Bool_t create_status = CreateConfigurationFromXMLFile(xml_config_file);
    if (!create_status) {
        std::cerr << "Error: There are problems with creation of the configuration from XML (in BmnSiliconStationSet)\n";
    }
}

BmnSiliconStationSet::~BmnSiliconStationSet()
{
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
        if (SiliconStations[i]) {
            delete SiliconStations[i];
            SiliconStations[i] = NULL;
        }
    }
    NStations = 0;

    if (SiliconStations) {
        delete [] SiliconStations;
        SiliconStations = NULL;
    }

}

Double_t BmnSiliconStationSet::GetXStationPosition(Int_t station_num) {
    if (XStationPositions && station_num >= 0 && station_num < NStations) {
        return XStationPositions[station_num];
    } else {
        throw (StationSet_Exception("Error in the function GetXStationPosition()"));
    }
}

Double_t BmnSiliconStationSet::GetYStationPosition(Int_t station_num) {
    if (YStationPositions && station_num >= 0 && station_num < NStations) {
        return YStationPositions[station_num];
    } else {
        throw (StationSet_Exception("Error in the function GetYStationPosition()"));
    }
}

Double_t BmnSiliconStationSet::GetZStationPosition(Int_t station_num) {
    if (ZStationPositions && station_num >= 0 && station_num < NStations) {
        return ZStationPositions[station_num];
    } else {
        throw (StationSet_Exception("Error in the function GetZStationPosition()"));
    }
}

BmnSiliconStation* BmnSiliconStationSet::GetSiliconStation(Int_t station_num) {
    if (SiliconStations && station_num >= 0 && station_num < NStations) {
        return SiliconStations[station_num];
    } else {
        throw (StationSet_Exception("Error in the function GetSiliconStation()"));
    }
}

void BmnSiliconStationSet::Reset() {
    for (Int_t istation = 0; istation < NStations; ++istation) {
        SiliconStations[istation]->Reset();
    }
}

Bool_t BmnSiliconStationSet::AddPointToDetector(Double_t xcoord, Double_t ycoord, Double_t zcoord,
        Double_t px, Double_t py, Double_t pz,
        Double_t dEloss, Int_t refID) {

    Int_t station = GetPointStationOwnership(zcoord);

    if (station != -1) {
        if (SiliconStations[station]->AddPointToStation(xcoord, ycoord, zcoord, px, py, pz, dEloss, refID) != -1) return true;
        else return false;
    }
    return false;
}

Int_t BmnSiliconStationSet::CountNAddedToDetectorPoints() {
    Int_t points_sum = 0;
    for (Int_t iStation = 0; iStation < NStations; iStation++) {
        points_sum += SiliconStations[iStation]->CountNAddedToStationPoints();
    }
    return points_sum;
}

void BmnSiliconStationSet::ProcessPointsInDetector() {
    for (Int_t iStation = 0; iStation < NStations; iStation++) {
        SiliconStations[iStation]->ProcessPointsInStation();
    }
}

Int_t BmnSiliconStationSet::CountNProcessedPointsInDetector() {
    Int_t points_sum = 0;
    for (Int_t iStation = 0; iStation < NStations; iStation++) {
        points_sum += SiliconStations[iStation]->CountNProcessedPointInStation();
    }
    return points_sum;
}

Int_t BmnSiliconStationSet::GetPointStationOwnership(Double_t zcoord) {
    //for z-positions and z-shifts of all modules in a station
    for (Int_t iStation = 0; iStation < NStations; iStation++) {
        Int_t NModules = SiliconStations[iStation]->GetNModules();
        for (Int_t iModule = 0; iModule < NModules; ++iModule) {
            if (SiliconStations[iStation]->GetModule(iModule)->IsPointInsideZThickness(zcoord)) {
                return iStation;
            }
        }
    }
    return -1;
}

Int_t BmnSiliconStationSet::GetPointStationOwnership(Double_t xcoord, Double_t ycoord, Double_t zcoord) {
    //for z-positions and z-shifts of all modules in a station
    for(Int_t iStation = 0; iStation < NStations; iStation++) {
        Int_t NModules = SiliconStations[iStation]->GetNModules();
        for(Int_t iModule = 0; iModule < NModules; ++iModule) {
            //if( SiliconStations[iStation]->GetModule(iModule)->IsPointInsideZThickness(zcoord) ) {
            if( SiliconStations[iStation]->GetModule(iModule)->IsPointInsideModule(xcoord, ycoord, zcoord) ) {
                return iStation;
            }
        }
    }
    return -1;
}

Bool_t BmnSiliconStationSet::CreateConfigurationFromXMLFile(TString xml_config_file) {
    TDOMParser *parser = new TDOMParser();
    parser->SetValidate(false);

    Int_t parse_status = parser->ParseFile(xml_config_file);
    if (parse_status != 0) {
        std::cerr << "Error: An error occured when parsing the file! (in BmnSiliconStationSet)\n";
        return false;
    }

    TXMLNode *node = parser->GetXMLDocument()->GetRootNode();

    if (strcmp(node->GetNodeName(), "StationSet") != 0) {
        std::cerr << "Error: Incorrect name of the root element! (in BmnSiliconStationSet)\n";
        return false;
    }

    NStations = CountNumberOfStations(node);

    SiliconStations = new BmnSiliconStation* [NStations];
    XStationPositions = new Double_t[NStations];
    YStationPositions = new Double_t[NStations];
    ZStationPositions = new Double_t[NStations];

    //default values
    for (Int_t i = 0; i < NStations; ++i) {
        SiliconStations[i] = 0; //zero-pointer
        XStationPositions[i] = 0.0;
        YStationPositions[i] = 0.0;
        ZStationPositions[i] = 0.0;
    }

    node = node->GetChildren();
    Int_t currentStationNum = 0;
    while (node) {
        if (strcmp(node->GetNodeName(), "Station") == 0) {
            parse_status = ParseStation(node, currentStationNum);
            if (!parse_status) return false;
            currentStationNum++;
        }
        node = node->GetNextNode();
    }

    delete parser;

    return true;
}

Int_t BmnSiliconStationSet::CountNumberOfStations(TXMLNode *node) {
    Int_t station_cnt = 0;
    node = node->GetChildren();
    while (node) {
        if (strcmp(node->GetNodeName(), "Station") == 0) {
            station_cnt++;
        }
        node = node->GetNextNode();
    }
    return station_cnt;
}

Bool_t BmnSiliconStationSet::ParseStation(TXMLNode *node, Int_t iStation) {

    if (node->HasAttributes()) {
        TList *attrList = node->GetAttributes();
        TXMLAttr *attr = 0;
        TIter next(attrList);

        while (attr = (TXMLAttr*) next()) {
            if (strcmp(attr->GetName(), "xPosition") == 0) {
                XStationPositions[iStation] = -atof(attr->GetValue()); //inverted
            }
            if (strcmp(attr->GetName(), "yPosition") == 0) {
                YStationPositions[iStation] = atof(attr->GetValue());
            }
            if (strcmp(attr->GetName(), "zPosition") == 0) {
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

    SiliconStations[iStation] =
            new BmnSiliconStation(node, iStation,
            XStationPositions[iStation] + dx, YStationPositions[iStation] + dy, ZStationPositions[iStation] + dz);
    return true;
}

//------------------------------------------------------------------------------

ClassImp(BmnSiliconStationSet)
