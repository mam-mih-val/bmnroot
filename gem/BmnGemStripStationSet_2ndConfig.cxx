#include "BmnGemStripStationSet_2ndConfig.h"
#include "BmnGemStripStation_2ndConfig.h"

BmnGemStripStationSet_2ndConfig::BmnGemStripStationSet_2ndConfig() {

    NStations = 7;

    XStationPositions = new Double_t[NStations];
        XStationPositions[0] = 2.2;
        XStationPositions[1] = -0.5;
        XStationPositions[2] = -0.7;
        XStationPositions[3] = -1.0;
        XStationPositions[4] = -1.4;
        XStationPositions[5] = -2.1;
        XStationPositions[6] = -2.9;

    YStationPositions = new Double_t[NStations];
        YStationPositions[0] = 0.0;
        YStationPositions[1] = 0.0;
        YStationPositions[2] = 0.0;
        YStationPositions[3] = 0.0;
        YStationPositions[4] = 0.0;
        YStationPositions[5] = 0.0;
        YStationPositions[6] = 0.0;

    ZStationPositions = new Double_t[NStations];
        ZStationPositions[0] = 30.0;
        ZStationPositions[1] = 45.0;
        ZStationPositions[2] = 60.0;
        ZStationPositions[3] = 80.0;
        ZStationPositions[4] = 100.0;
        ZStationPositions[5] = 130.0;
        ZStationPositions[6] = 160.0;

    BeamHoleRadiuses = new Double_t[NStations];

    DefineBeamHoleRadiuses();

    BuildStations();
}

BmnGemStripStationSet_2ndConfig::~BmnGemStripStationSet_2ndConfig() {

    if(XStationPositions) {
        delete [] XStationPositions;
        XStationPositions = NULL;
    }
    if(YStationPositions)  {
        delete [] YStationPositions;
        YStationPositions = NULL;
    }
    if(ZStationPositions) {
        delete [] ZStationPositions;
        ZStationPositions = NULL;
    }
    if(BeamHoleRadiuses) {
        delete [] BeamHoleRadiuses;
        BeamHoleRadiuses = NULL;
    }

    for(Int_t i = 0; i < NStations; i++) {
        if(GemStations[i])  {
            delete GemStations[i];
            GemStations[i] = NULL;
        }
    }
    if(GemStations) {
        delete [] GemStations;
        GemStations = NULL;
    }

}

Int_t BmnGemStripStationSet_2ndConfig::GetPointStationOwnership(Double_t zcoord) {

    for(Int_t iStation = 0; iStation < NStations; iStation++) {
        if( (zcoord >= ZStationPositions[iStation]) && (zcoord <= (ZStationPositions[iStation]+GemStations[iStation]->GetZSize()))) {
            return iStation;
        }
    }

    return -1;
}

Bool_t BmnGemStripStationSet_2ndConfig::AddPointToDetector(Double_t xcoord, Double_t ycoord, Double_t zcoord, Double_t dEloss, Int_t refID) {

    Int_t station = GetPointStationOwnership(zcoord);

    if(station != -1) {
        if( GemStations[station]->AddPointToStation(xcoord, ycoord, zcoord, dEloss, refID) != -1 ) return true;
        else return false;
    }

    return false;
}

Int_t BmnGemStripStationSet_2ndConfig::CountNAddedToDetectorPoints() {
    Int_t points_sum = 0;
    for(Int_t iStation = 0; iStation < NStations; iStation++) {
        points_sum += GemStations[iStation]->CountNAddedToStationPoints();
    }
    return points_sum;
}

void BmnGemStripStationSet_2ndConfig::ProcessPointsInDetector() {
    for(Int_t iStation = 0; iStation < NStations; iStation++) {
        GemStations[iStation]->ProcessPointsInStation();
    }
}

Int_t BmnGemStripStationSet_2ndConfig::CountNProcessedPointsInDetector() {
    Int_t points_sum = 0;
    for(Int_t iStation = 0; iStation < NStations; iStation++) {
        points_sum += GemStations[iStation]->CountNProcessedPointInStation();
    }
    return points_sum;
}

void BmnGemStripStationSet_2ndConfig::DefineBeamHoleRadiuses() {
    for(UInt_t iStation = 1; iStation < NStations; iStation++) {
        BeamHoleRadiuses[iStation] = 0.0;
    }
    BeamHoleRadiuses[3] = 4.0; // real hole (without a frame) in the plane (163x45)
    BeamHoleRadiuses[4] = 4.0;
    BeamHoleRadiuses[5] = 4.0;
    BeamHoleRadiuses[6] = 4.0;
}

void BmnGemStripStationSet_2ndConfig::BuildStations() {
    GemStations = new BmnGemStripStation* [NStations];

    for(Int_t iStation = 0; iStation < NStations; iStation++) {
        GemStations[iStation] =
            new BmnGemStripStation_2ndConfig(iStation,
                                   XStationPositions[iStation], YStationPositions[iStation], ZStationPositions[iStation],
                                   BeamHoleRadiuses[iStation]);
    }
}

ClassImp(BmnGemStripStationSet_2ndConfig)