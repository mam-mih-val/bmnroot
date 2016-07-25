#include "BmnGemStripStation.h"

BmnGemStripStation::BmnGemStripStation()
: StationNumber(0), NModules(0),
  XSize(0.0), YSize(0.0), ZSize(0.0),
  XPosition(0.0), YPosition(0.0), ZPosition(0.0),
  BeamHoleRadius(0.0),
  ReadoutModules(NULL) {

}

BmnGemStripReadoutModule* BmnGemStripStation::GetReadoutModule(Int_t module_num) {
    if(ReadoutModules && module_num >= 0 && module_num < NModules) {
        return ReadoutModules[module_num];
    }
    else {
        throw(Station_Exception("Error in the function GetReadoutModule()"));
    }
}

void BmnGemStripStation::Reset() {
    for(Int_t imodule = 0; imodule < NModules; ++imodule) {
        ReadoutModules[imodule]->RebuildReadoutPlanes();
    }
}

Int_t BmnGemStripStation::AddPointToStation(Double_t xcoord, Double_t ycoord, Double_t zcoord,
                                    Double_t px, Double_t py, Double_t pz,
                                    Double_t dEloss, Int_t refID) {

    Int_t module = GetPointModuleOwnership(xcoord, ycoord, zcoord);

    if(module >= 0) {
        //if(ReadoutModules[module]->AddRealPointFullOne(xcoord, ycoord, zcoord, dEloss, refID)) {
        if(ReadoutModules[module]->AddRealPointFull(xcoord, ycoord, zcoord, px, py, pz, dEloss, refID)) {
            return module;
        }
        else {
            return -1;
        }
    }
    return -1;
}

Int_t BmnGemStripStation::CountNAddedToStationPoints() {
    Int_t points_sum = 0;
    for(Int_t iModule = 0; iModule < NModules; iModule++) {
        points_sum += ReadoutModules[iModule]->GetNRealPoints();
    }
    return points_sum;
}

void BmnGemStripStation::ProcessPointsInStation(){
    for(Int_t iModule = 0; iModule < NModules; iModule++) {
        ReadoutModules[iModule]->CalculateStripHitIntersectionPoints();
    }
}

Int_t BmnGemStripStation::CountNProcessedPointInStation() {
    Int_t points_sum = 0;
    for(Int_t iModule = 0; iModule < NModules; iModule++) {
        points_sum += ReadoutModules[iModule]->GetNIntersectionPoints();
    }
    return points_sum;
}

ClassImp(BmnGemStripStation)
