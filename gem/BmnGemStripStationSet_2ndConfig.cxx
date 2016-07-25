#include "BmnGemStripStationSet_2ndConfig.h"
#include "BmnGemStripStation_2ndConfig.h"
#include "BmnGemStripConfiguration.h"

BmnGemStripStationSet_2ndConfig::BmnGemStripStationSet_2ndConfig() {

    NStations = 7;

    // !!! Classical coordinate system is used !!!
    XStationPositions = new Double_t[NStations];
        XStationPositions[0] = -BmnGemStripPositions_SecondConfig::XStationPositions[0]; //inverted : (bm@n x-coord -> classical x-coord)
        XStationPositions[1] = -BmnGemStripPositions_SecondConfig::XStationPositions[1];
        XStationPositions[2] = -BmnGemStripPositions_SecondConfig::XStationPositions[2];
        XStationPositions[3] = -BmnGemStripPositions_SecondConfig::XStationPositions[3];
        XStationPositions[4] = -BmnGemStripPositions_SecondConfig::XStationPositions[4];
        XStationPositions[5] = -BmnGemStripPositions_SecondConfig::XStationPositions[5];
        XStationPositions[6] = -BmnGemStripPositions_SecondConfig::XStationPositions[6];

    YStationPositions = new Double_t[NStations];
        YStationPositions[0] = BmnGemStripPositions_SecondConfig::YStationPositions[0];
        YStationPositions[1] = BmnGemStripPositions_SecondConfig::YStationPositions[1];
        YStationPositions[2] = BmnGemStripPositions_SecondConfig::YStationPositions[2];
        YStationPositions[3] = BmnGemStripPositions_SecondConfig::YStationPositions[3];
        YStationPositions[4] = BmnGemStripPositions_SecondConfig::YStationPositions[4];
        YStationPositions[5] = BmnGemStripPositions_SecondConfig::YStationPositions[5];
        YStationPositions[6] = BmnGemStripPositions_SecondConfig::YStationPositions[6];

    ZStationPositions = new Double_t[NStations];
        ZStationPositions[0] = BmnGemStripPositions_SecondConfig::ZStationPositions[0];
        ZStationPositions[1] = BmnGemStripPositions_SecondConfig::ZStationPositions[1];
        ZStationPositions[2] = BmnGemStripPositions_SecondConfig::ZStationPositions[2];
        ZStationPositions[3] = BmnGemStripPositions_SecondConfig::ZStationPositions[3];
        ZStationPositions[4] = BmnGemStripPositions_SecondConfig::ZStationPositions[4];
        ZStationPositions[5] = BmnGemStripPositions_SecondConfig::ZStationPositions[5];
        ZStationPositions[6] = BmnGemStripPositions_SecondConfig::ZStationPositions[6];

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

void BmnGemStripStationSet_2ndConfig::DefineBeamHoleRadiuses() {
    for(UInt_t iStation = 0; iStation < NStations; iStation++) {
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