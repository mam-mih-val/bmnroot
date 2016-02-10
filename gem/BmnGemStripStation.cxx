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
        throw(Station_Exeption("Error in the function GetReadoutModule()"));
    }
}

ClassImp(BmnGemStripStation)
