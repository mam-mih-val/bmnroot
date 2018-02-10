#include "BmnGemStripStationSet_RunSpring2017.h"
#include "BmnGemStripStation_RunSpring2017.h"

BmnGemStripStationSet_RunSpring2017::BmnGemStripStationSet_RunSpring2017(BmnGemStripConfiguration::GEM_CONFIG config) {

    NStations = BmnGemStationPositions_RunSpring2017::NStations; //must be 6! (check it if you doubt))

    // !!! Classical coordinate system is used !!!
    XStationPositions = new Double_t[NStations];
    YStationPositions = new Double_t[NStations];
    ZStationPositions = new Double_t[NStations];

    fCurrentConfig = config;

    switch(fCurrentConfig) {
        case BmnGemStripConfiguration::RunSpring2017:
            for (Int_t iStat = 0; iStat < NStations; iStat++) {
                XStationPositions[iStat] = -BmnGemStationPositions_RunSpring2017::XStationPositions[iStat]; //inverted : (bm@n x-coord -> classical x-coord)
                YStationPositions[iStat] = BmnGemStationPositions_RunSpring2017::YStationPositions[iStat];
                ZStationPositions[iStat] = BmnGemStationPositions_RunSpring2017::ZStationPositions[iStat];
            }
            break;

        default:
            for (Int_t iStat = 0; iStat < NStations; iStat++) {
                XStationPositions[iStat] = -BmnGemStationPositions_RunSpring2017::XStationPositions[iStat]; //inverted : (bm@n x-coord -> classical x-coord)
                YStationPositions[iStat] = BmnGemStationPositions_RunSpring2017::YStationPositions[iStat];
                ZStationPositions[iStat] = BmnGemStationPositions_RunSpring2017::ZStationPositions[iStat];
            }
            break;
    }

    BeamHoleRadiuses = new Double_t[NStations];

    DefineBeamHoleRadiuses();

    BuildStations();
}

BmnGemStripStationSet_RunSpring2017::~BmnGemStripStationSet_RunSpring2017() {

}

void BmnGemStripStationSet_RunSpring2017::DefineBeamHoleRadiuses() {
    for (UInt_t iStation = 0; iStation < NStations; iStation++) {
        BeamHoleRadiuses[iStation] = 0.0;
    }
    BeamHoleRadiuses[4] = 4.0; // real hole (without a frame) in the plane (163x45)
    BeamHoleRadiuses[5] = 4.0;
}

void BmnGemStripStationSet_RunSpring2017::BuildStations() {
    GemStations = new BmnGemStripStation* [NStations];

    for (Int_t iStation = 0; iStation < NStations; iStation++) {
        GemStations[iStation] =
                new BmnGemStripStation_RunSpring2017(iStation,
                XStationPositions[iStation], YStationPositions[iStation], ZStationPositions[iStation],
                BeamHoleRadiuses[iStation], fCurrentConfig);
    }
}

ClassImp(BmnGemStripStationSet_RunSpring2017)
