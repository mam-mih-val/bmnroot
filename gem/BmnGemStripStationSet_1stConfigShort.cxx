#include "BmnGemStripStationSet_1stConfigShort.h"
#include "BmnGemStripStation_1stConfigShort.h"
#include "BmnGemStripConfiguration.h"

BmnGemStripStationSet_1stConfigShort::BmnGemStripStationSet_1stConfigShort() {

    NStations = 5;

    // !!! Classical coordinate system is used !!!
    XStationPositions = new Double_t[NStations];
        XStationPositions[0] = -BmnGemStripPositions_FirstConfigShort::XStationPositions[0]; //inverted : (bm@n x-coord -> classical x-coord)
        XStationPositions[1] = -BmnGemStripPositions_FirstConfigShort::XStationPositions[1];
        XStationPositions[2] = -BmnGemStripPositions_FirstConfigShort::XStationPositions[2];
        XStationPositions[3] = -BmnGemStripPositions_FirstConfigShort::XStationPositions[3];
        XStationPositions[4] = -BmnGemStripPositions_FirstConfigShort::XStationPositions[4];

    YStationPositions = new Double_t[NStations];
        YStationPositions[0] = BmnGemStripPositions_FirstConfigShort::YStationPositions[0];
        YStationPositions[1] = BmnGemStripPositions_FirstConfigShort::YStationPositions[1];
        YStationPositions[2] = BmnGemStripPositions_FirstConfigShort::YStationPositions[2];
        YStationPositions[3] = BmnGemStripPositions_FirstConfigShort::YStationPositions[3];
        YStationPositions[4] = BmnGemStripPositions_FirstConfigShort::YStationPositions[4];

    ZStationPositions = new Double_t[NStations];
        ZStationPositions[0] = BmnGemStripPositions_FirstConfigShort::ZStationPositions[0];
        ZStationPositions[1] = BmnGemStripPositions_FirstConfigShort::ZStationPositions[1];
        ZStationPositions[2] = BmnGemStripPositions_FirstConfigShort::ZStationPositions[2];
        ZStationPositions[3] = BmnGemStripPositions_FirstConfigShort::ZStationPositions[3];
        ZStationPositions[4] = BmnGemStripPositions_FirstConfigShort::ZStationPositions[4];

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