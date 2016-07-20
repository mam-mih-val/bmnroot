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



ClassImp(BmnGemStripStationSet)