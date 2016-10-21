#include "BmnGemStripStationSet_RunSummer2016.h"
#include "BmnGemStripStation_RunSummer2016.h"
#include "BmnGemStripConfiguration.h"

BmnGemStripStationSet_RunSummer2016::BmnGemStripStationSet_RunSummer2016() {

    NStations = 7;

    // !!! Classical coordinate system is used !!!
    XStationPositions = new Double_t[NStations];
    YStationPositions = new Double_t[NStations];
    ZStationPositions = new Double_t[NStations];

    for (Int_t iStat = 0; iStat < NStations; iStat++) {
        XStationPositions[iStat] = -BmnGemStationPositions_RunSummer2016::XStationPositions[iStat]; //inverted : (bm@n x-coord -> classical x-coord)   
        YStationPositions[iStat] = BmnGemStationPositions_RunSummer2016::YStationPositions[iStat];
        ZStationPositions[iStat] = BmnGemStationPositions_RunSummer2016::ZStationPositions[iStat];
    }

    BeamHoleRadiuses = new Double_t[NStations];

    DefineBeamHoleRadiuses();

    BuildStations();
}

BmnGemStripStationSet_RunSummer2016::BmnGemStripStationSet_RunSummer2016(TString set) {
    NStations = 7;
    XStationPositions = new Double_t[NStations];
    YStationPositions = new Double_t[NStations];
    ZStationPositions = new Double_t[NStations];

    for (Int_t iStat = 0; iStat < NStations; iStat++)
        ZStationPositions[iStat] = BmnGemStationPositions_RunSummer2016::ZStationPositions[iStat];

    if (set == "SET1")
        for (Int_t iStat = 0; iStat < NStations; iStat++) {
            XStationPositions[iStat] = -BmnGemStationPositions_RunSummer2016_set1::XStationPositions[iStat]; //inverted : (bm@n x-coord -> classical x-coord)      
            YStationPositions[iStat] = BmnGemStationPositions_RunSummer2016_set1::YStationPositions[iStat];
        }
    else if (set == "SET2")
        for (Int_t iStat = 0; iStat < NStations; iStat++) {
            XStationPositions[iStat] = -BmnGemStationPositions_RunSummer2016_set2::XStationPositions[iStat]; //inverted : (bm@n x-coord -> classical x-coord)      
            YStationPositions[iStat] = BmnGemStationPositions_RunSummer2016_set2::YStationPositions[iStat];
        }

    else
        BmnGemStripStationSet_RunSummer2016();

    BeamHoleRadiuses = new Double_t[NStations];

    DefineBeamHoleRadiuses();

    BuildStations();
}

BmnGemStripStationSet_RunSummer2016::~BmnGemStripStationSet_RunSummer2016() {

    if (XStationPositions) {
        delete [] XStationPositions;
        XStationPositions = NULL;
    }
    if (YStationPositions) {
        delete [] YStationPositions;
        YStationPositions = NULL;
    }
    if (ZStationPositions) {
        delete [] ZStationPositions;
        ZStationPositions = NULL;
    }
    if (BeamHoleRadiuses) {
        delete [] BeamHoleRadiuses;
        BeamHoleRadiuses = NULL;
    }

    for (Int_t i = 0; i < NStations; i++) {
        if (GemStations[i]) {
            delete GemStations[i];
            GemStations[i] = NULL;
        }
    }
    if (GemStations) {
        delete [] GemStations;
        GemStations = NULL;
    }

}

void BmnGemStripStationSet_RunSummer2016::DefineBeamHoleRadiuses() {
    for (UInt_t iStation = 0; iStation < NStations; iStation++) {
        BeamHoleRadiuses[iStation] = 0.0;
    }
    BeamHoleRadiuses[6] = 4.0; // real hole (without a frame) in the plane (163x45)
}

void BmnGemStripStationSet_RunSummer2016::BuildStations() {
    GemStations = new BmnGemStripStation* [NStations];

    for (Int_t iStation = 0; iStation < NStations; iStation++) {
        GemStations[iStation] =
                new BmnGemStripStation_RunSummer2016(iStation,
                XStationPositions[iStation], YStationPositions[iStation], ZStationPositions[iStation],
                BeamHoleRadiuses[iStation]);
    }
}

ClassImp(BmnGemStripStationSet_RunSummer2016)