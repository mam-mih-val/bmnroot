#include "BmnGemStripStationSet_RunWinter2016.h"
#include "BmnGemStripStation_RunWinter2016.h"

BmnGemStripStationSet_RunWinter2016::BmnGemStripStationSet_RunWinter2016(BmnGemStripConfiguration::GEM_CONFIG config) {

    NStations = BmnGemStationPositions_RunWinter2016::NStations; //must be 7! (check it if you doubt))

    // !!! Classical coordinate system is used !!!
    XStationPositions = new Double_t[NStations];
    YStationPositions = new Double_t[NStations];
    ZStationPositions = new Double_t[NStations];

    fCurrentConfig = config;

    switch(fCurrentConfig) {
        case BmnGemStripConfiguration::RunWinter2016:
            for (Int_t iStat = 0; iStat < NStations; iStat++) {
                XStationPositions[iStat] = -BmnGemStationPositions_RunWinter2016::XStationPositions[iStat]; //inverted : (bm@n x-coord -> classical x-coord)
                YStationPositions[iStat] = BmnGemStationPositions_RunWinter2016::YStationPositions[iStat];
                ZStationPositions[iStat] = BmnGemStationPositions_RunWinter2016::ZStationPositions[iStat];
            }
            break;

        default:
            for (Int_t iStat = 0; iStat < NStations; iStat++) {
                XStationPositions[iStat] = -BmnGemStationPositions_RunWinter2016::XStationPositions[iStat]; //inverted : (bm@n x-coord -> classical x-coord)
                YStationPositions[iStat] = BmnGemStationPositions_RunWinter2016::YStationPositions[iStat];
                ZStationPositions[iStat] = BmnGemStationPositions_RunWinter2016::ZStationPositions[iStat];
            }
            break;
    }

    BeamHoleRadiuses = new Double_t[NStations];

    DefineBeamHoleRadiuses();

    BuildStations();
}

BmnGemStripStationSet_RunWinter2016::~BmnGemStripStationSet_RunWinter2016() {

}

void BmnGemStripStationSet_RunWinter2016::DefineBeamHoleRadiuses() {
    for (UInt_t iStation = 0; iStation < NStations; iStation++) {
        BeamHoleRadiuses[iStation] = 0.0;
    }
    BeamHoleRadiuses[5] = 4.0; // real hole (without a frame) in the plane (163x45)
    BeamHoleRadiuses[6] = 4.0;
}

void BmnGemStripStationSet_RunWinter2016::BuildStations() {
    GemStations = new BmnGemStripStation* [NStations];

    for (Int_t iStation = 0; iStation < NStations; iStation++) {
        GemStations[iStation] =
                new BmnGemStripStation_RunWinter2016(iStation,
                XStationPositions[iStation], YStationPositions[iStation], ZStationPositions[iStation],
                BeamHoleRadiuses[iStation], fCurrentConfig);
    }
}

ClassImp(BmnGemStripStationSet_RunWinter2016)
