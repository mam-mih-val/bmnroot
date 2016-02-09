#include "BmnGemStripStationSet_1stConfigShort.h"
#include "BmnGemStripStation_1stConfigShort.h"

BmnGemStripStationSet_1stConfigShort::BmnGemStripStationSet_1stConfigShort() {

    NStations = 5;

    XStationPositions = new Double_t[NStations];
        XStationPositions[0] = 2.2;
        XStationPositions[1] = -3.0;
        XStationPositions[2] = 1.8;
        XStationPositions[3] = -3.5;
        XStationPositions[4] = 1.1;

    YStationPositions = new Double_t[NStations];
        YStationPositions[0] = 0.0;
        YStationPositions[1] = 0.0;
        YStationPositions[2] = 0.0;
        YStationPositions[3] = 0.0;
        YStationPositions[4] = 0.0;

    ZStationPositions = new Double_t[NStations];
        ZStationPositions[0] = 30.0;
        ZStationPositions[1] = 45.0;
        ZStationPositions[2] = 60.0;
        ZStationPositions[3] = 80.0;
        ZStationPositions[4] = 100.0;

    BeamHoleRadiuses = new Double_t[NStations];

    DefineBeamHoleRadiuses();

    BuildStations();
}

BmnGemStripStationSet_1stConfigShort::~BmnGemStripStationSet_1stConfigShort() {

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

Int_t BmnGemStripStationSet_1stConfigShort::GetPointStationOwnership(Double_t zcoord) {

    for(Int_t iStation = 0; iStation < NStations; iStation++) {
        if( (zcoord >= ZStationPositions[iStation]) && (zcoord <= (ZStationPositions[iStation]+GemStations[iStation]->GetZSize()))) {
            return iStation;
        }
    }

    return -1;
}

Bool_t BmnGemStripStationSet_1stConfigShort::AddPointToDetector(Double_t xcoord, Double_t ycoord, Double_t zcoord, Double_t dEloss, Int_t refID) {

    Int_t station = GetPointStationOwnership(zcoord);

    if(station != -1) {
        if( GemStations[station]->AddPointToStation(xcoord, ycoord, zcoord, dEloss, refID) != -1 ) return true;
        else return false;
    }

    return false;
}

Int_t BmnGemStripStationSet_1stConfigShort::CountNAddedToDetectorPoints() {
    Int_t points_sum = 0;
    for(Int_t iStation = 0; iStation < NStations; iStation++) {
        points_sum += GemStations[iStation]->CountNAddedToStationPoints();
    }
    return points_sum;
}

void BmnGemStripStationSet_1stConfigShort::ProcessPointsInDetector() {
    for(Int_t iStation = 0; iStation < NStations; iStation++) {
        GemStations[iStation]->ProcessPointsInStation();
    }
}

Int_t BmnGemStripStationSet_1stConfigShort::CountNProcessedPointsInDetector() {
    Int_t points_sum = 0;
    for(Int_t iStation = 0; iStation < NStations; iStation++) {
        points_sum += GemStations[iStation]->CountNProcessedPointInStation();
    }
    return points_sum;
}

void BmnGemStripStationSet_1stConfigShort::DefineBeamHoleRadiuses() {
    for(UInt_t iStation = 1; iStation < NStations; iStation++) {
        BeamHoleRadiuses[iStation] = 0.0;
    }
    //All stations in the configuration have not beam holes!
}

void BmnGemStripStationSet_1stConfigShort::BuildStations() {
    GemStations = new BmnGemStripStation* [NStations];

    for(Int_t iStation = 0; iStation < NStations; iStation++) {
        GemStations[iStation] =
            new BmnGemStripStation_1stConfigShort(iStation,
                                   XStationPositions[iStation], YStationPositions[iStation], ZStationPositions[iStation],
                                   BeamHoleRadiuses[iStation]);
    }
}

ClassImp(BmnGemStripStationSet_1stConfigShort)