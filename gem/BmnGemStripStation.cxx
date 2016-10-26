#include "BmnGemStripStation.h"

BmnGemStripStation::BmnGemStripStation()
: StationNumber(0), NModules(0),
  XSize(0.0), YSize(0.0), ZSize(0.0),
  XPosition(0.0), YPosition(0.0), ZPosition(0.0),
  XShiftOfModules(NULL), YShiftOfModules(NULL), ZShiftOfModules(NULL),
  BeamHoleRadius(0.0),
  Modules(NULL) { }

Double_t BmnGemStripStation::GetXShiftOfModule(Int_t module_num) {
    if(XShiftOfModules && module_num >= 0 && module_num < NModules) {
        return XShiftOfModules[module_num];
    }
    else {
        throw(Station_Exception("Error in the function GetXShiftOfModule()"));
    }
}

Double_t BmnGemStripStation::GetYShiftOfModule(Int_t module_num) {
    if(YShiftOfModules && module_num >= 0 && module_num < NModules) {
        return YShiftOfModules[module_num];
    }
    else {
        throw(Station_Exception("Error in the function GetYShiftOfModule()"));
    }
}

Double_t BmnGemStripStation::GetZShiftOfModule(Int_t module_num) {
    if(ZShiftOfModules && module_num >= 0 && module_num < NModules) {
        return ZShiftOfModules[module_num];
    }
    else {
        throw(Station_Exception("Error in the function GetZShiftOfModule()"));
    }
}

BmnGemStripModule* BmnGemStripStation::GetModule(Int_t module_num) {
    if(Modules && module_num >= 0 && module_num < NModules) {
        return Modules[module_num];
    }
    else {
        throw(Station_Exception("Error in the function GetReadoutModule()"));
    }
}

void BmnGemStripStation::Reset() {
    for(Int_t imodule = 0; imodule < NModules; ++imodule) {
        Modules[imodule]->ResetModuleData();
    }
}

Int_t BmnGemStripStation::AddPointToStation(Double_t xcoord, Double_t ycoord, Double_t zcoord,
                                    Double_t px, Double_t py, Double_t pz,
                                    Double_t dEloss, Int_t refID) {

    Int_t module = GetPointModuleOwnership(xcoord, ycoord, zcoord);

    if(module >= 0) {
        //if(ReadoutModules[module]->AddRealPointFullOne(xcoord, ycoord, zcoord, dEloss, refID)) {
        if(Modules[module]->AddRealPointFull(xcoord, ycoord, zcoord, px, py, pz, dEloss, refID)) {
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
        points_sum += Modules[iModule]->GetNRealPoints();
    }
    return points_sum;
}

void BmnGemStripStation::ProcessPointsInStation(){
    for(Int_t iModule = 0; iModule < NModules; iModule++) {
        Modules[iModule]->CalculateStripHitIntersectionPoints();
    }
}

Int_t BmnGemStripStation::CountNProcessedPointInStation() {
    Int_t points_sum = 0;
    for(Int_t iModule = 0; iModule < NModules; iModule++) {
        points_sum += Modules[iModule]->GetNIntersectionPoints();
    }
    return points_sum;
}

ClassImp(BmnGemStripStation)
