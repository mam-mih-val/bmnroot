#include "BmnGemStripStationSet.h"

BmnGemStripStationSet::BmnGemStripStationSet()
: NStations(0),
  XStationPositions(NULL), YStationPositions(NULL), ZStationPositions(NULL),
  BeamHoleRadiuses(NULL),
  GemStations(NULL) {

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

    Int_t station = GetPointStationOwnership(zcoord);

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
    for(Int_t iStation = 0; iStation < NStations; iStation++) {
        if( (zcoord >= ZStationPositions[iStation]) && (zcoord <= (ZStationPositions[iStation]+GemStations[iStation]->GetZSize())) ) {
            return iStation;
        }
    }
    return -1;
}

ClassImp(BmnGemStripStationSet)