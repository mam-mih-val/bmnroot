#include "BmnGemStripStationSet_1stConfig.h"
#include "BmnGemStripStation_1stConfig.h"
#include "BmnGemStripConfiguration.h"

BmnGemStripStationSet_1stConfig::BmnGemStripStationSet_1stConfig() {

    NStations = 6;

    // !!! Classical coordinate system is used !!!
    XStationPositions = new Double_t[NStations];
        XStationPositions[0] = -BmnGemStripPositions_FirstConfig::XStationPositions[0]; //inverted : (bm@n x-coord -> classical x-coord)
        XStationPositions[1] = -BmnGemStripPositions_FirstConfig::XStationPositions[1];
        XStationPositions[2] = -BmnGemStripPositions_FirstConfig::XStationPositions[2];
        XStationPositions[3] = -BmnGemStripPositions_FirstConfig::XStationPositions[3];
        XStationPositions[4] = -BmnGemStripPositions_FirstConfig::XStationPositions[4];
        XStationPositions[5] = -BmnGemStripPositions_FirstConfig::XStationPositions[5];

    YStationPositions = new Double_t[NStations];
        YStationPositions[0] = BmnGemStripPositions_FirstConfig::YStationPositions[0];
        YStationPositions[1] = BmnGemStripPositions_FirstConfig::YStationPositions[1];
        YStationPositions[2] = BmnGemStripPositions_FirstConfig::YStationPositions[2];
        YStationPositions[3] = BmnGemStripPositions_FirstConfig::YStationPositions[3];
        YStationPositions[4] = BmnGemStripPositions_FirstConfig::YStationPositions[4];
        YStationPositions[5] = BmnGemStripPositions_FirstConfig::YStationPositions[5];

    ZStationPositions = new Double_t[NStations];
        ZStationPositions[0] = BmnGemStripPositions_FirstConfig::ZStationPositions[0];
        ZStationPositions[1] = BmnGemStripPositions_FirstConfig::ZStationPositions[1];
        ZStationPositions[2] = BmnGemStripPositions_FirstConfig::ZStationPositions[2];
        ZStationPositions[3] = BmnGemStripPositions_FirstConfig::ZStationPositions[3];
        ZStationPositions[4] = BmnGemStripPositions_FirstConfig::ZStationPositions[4];
        ZStationPositions[5] = BmnGemStripPositions_FirstConfig::ZStationPositions[5];

    BeamHoleRadiuses = new Double_t[NStations];

    DefineBeamHoleRadiuses();

    BuildStations();
}

BmnGemStripStationSet_1stConfig::~BmnGemStripStationSet_1stConfig() {

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

Int_t BmnGemStripStationSet_1stConfig::GetPointStationOwnership(Double_t zcoord) {

    for(Int_t iStation = 0; iStation < NStations; iStation++) {
        if( (zcoord >= ZStationPositions[iStation]) && (zcoord <= (ZStationPositions[iStation]+GemStations[iStation]->GetZSize()))) {
            return iStation;
        }
    }

    return -1;
}

Bool_t BmnGemStripStationSet_1stConfig::AddPointToDetector(Double_t xcoord, Double_t ycoord, Double_t zcoord,
                                                           Double_t px, Double_t py, Double_t pz,
                                                           Double_t dEloss, Int_t refID) {

    Int_t station = GetPointStationOwnership(zcoord);

    if(station != -1) {
        if( GemStations[station]->AddPointToStation(xcoord, ycoord, zcoord, px, py, pz, dEloss, refID) != -1 ) return true;
        else return false;
    }

    return false;
}

Int_t BmnGemStripStationSet_1stConfig::CountNAddedToDetectorPoints() {
    Int_t points_sum = 0;
    for(Int_t iStation = 0; iStation < NStations; iStation++) {
        points_sum += GemStations[iStation]->CountNAddedToStationPoints();
    }
    return points_sum;
}

void BmnGemStripStationSet_1stConfig::ProcessPointsInDetector() {
    for(Int_t iStation = 0; iStation < NStations; iStation++) {
        GemStations[iStation]->ProcessPointsInStation();
    }
}

Int_t BmnGemStripStationSet_1stConfig::CountNProcessedPointsInDetector() {
    Int_t points_sum = 0;
    for(Int_t iStation = 0; iStation < NStations; iStation++) {
        points_sum += GemStations[iStation]->CountNProcessedPointInStation();
    }
    return points_sum;
}

void BmnGemStripStationSet_1stConfig::DefineBeamHoleRadiuses() {
    for(UInt_t iStation = 0; iStation < NStations; iStation++) {
        BeamHoleRadiuses[iStation] = 0.0;
    }
    BeamHoleRadiuses[4] = 4.0; // real hole (without a frame) in the plane (163x45)
    BeamHoleRadiuses[5] = 4.0;
}

void BmnGemStripStationSet_1stConfig::BuildStations() {
    GemStations = new BmnGemStripStation* [NStations];

    for(Int_t iStation = 0; iStation < NStations; iStation++) {
        GemStations[iStation] =
            new BmnGemStripStation_1stConfig(iStation,
                                   XStationPositions[iStation], YStationPositions[iStation], ZStationPositions[iStation],
                                   BeamHoleRadiuses[iStation]);
    }
}

ClassImp(BmnGemStripStationSet_1stConfig)