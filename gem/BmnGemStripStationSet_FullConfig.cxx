#include "BmnGemStripStationSet_FullConfig.h"
#include "BmnGemStripStation_FullConfig.h"
#include "BmnGemStripConfiguration.h"

BmnGemStripStationSet_FullConfig::BmnGemStripStationSet_FullConfig() {

    NStations = 12;

    // !!! Classical coordinate system is used !!!
    XStationPositions = new Double_t[NStations];
        XStationPositions[0] = -BmnGemStripPositions_FullConfig::XStationPositions[0]; //inverted : (bm@n x-coord -> classical x-coord)
        XStationPositions[1] = -BmnGemStripPositions_FullConfig::XStationPositions[1];
        XStationPositions[2] = -BmnGemStripPositions_FullConfig::XStationPositions[2];
        XStationPositions[3] = -BmnGemStripPositions_FullConfig::XStationPositions[3];
        XStationPositions[4] = -BmnGemStripPositions_FullConfig::XStationPositions[4];
        XStationPositions[5] = -BmnGemStripPositions_FullConfig::XStationPositions[5];
        XStationPositions[6] = -BmnGemStripPositions_FullConfig::XStationPositions[6];
        XStationPositions[7] = -BmnGemStripPositions_FullConfig::XStationPositions[7];
        XStationPositions[8] = -BmnGemStripPositions_FullConfig::XStationPositions[8];
        XStationPositions[9] = -BmnGemStripPositions_FullConfig::XStationPositions[9];
        XStationPositions[10] = -BmnGemStripPositions_FullConfig::XStationPositions[10];
        XStationPositions[11] = -BmnGemStripPositions_FullConfig::XStationPositions[11];

    YStationPositions = new Double_t[NStations];
        YStationPositions[0] = BmnGemStripPositions_FullConfig::YStationPositions[0];
        YStationPositions[1] = BmnGemStripPositions_FullConfig::YStationPositions[1];
        YStationPositions[2] = BmnGemStripPositions_FullConfig::YStationPositions[2];
        YStationPositions[3] = BmnGemStripPositions_FullConfig::YStationPositions[3];
        YStationPositions[4] = BmnGemStripPositions_FullConfig::YStationPositions[4];
        YStationPositions[5] = BmnGemStripPositions_FullConfig::YStationPositions[5];
        YStationPositions[6] = BmnGemStripPositions_FullConfig::YStationPositions[6];
        YStationPositions[7] = BmnGemStripPositions_FullConfig::YStationPositions[7];
        YStationPositions[8] = BmnGemStripPositions_FullConfig::YStationPositions[8];
        YStationPositions[9] = BmnGemStripPositions_FullConfig::YStationPositions[9];
        YStationPositions[10] = BmnGemStripPositions_FullConfig::YStationPositions[10];
        YStationPositions[11] = BmnGemStripPositions_FullConfig::YStationPositions[11];

    ZStationPositions = new Double_t[NStations];
        ZStationPositions[0] = BmnGemStripPositions_FullConfig::ZStationPositions[0];
        ZStationPositions[1] = BmnGemStripPositions_FullConfig::ZStationPositions[1];
        ZStationPositions[2] = BmnGemStripPositions_FullConfig::ZStationPositions[2];
        ZStationPositions[3] = BmnGemStripPositions_FullConfig::ZStationPositions[3];
        ZStationPositions[4] = BmnGemStripPositions_FullConfig::ZStationPositions[4];
        ZStationPositions[5] = BmnGemStripPositions_FullConfig::ZStationPositions[5];
        ZStationPositions[6] = BmnGemStripPositions_FullConfig::ZStationPositions[6];
        ZStationPositions[7] = BmnGemStripPositions_FullConfig::ZStationPositions[7];
        ZStationPositions[8] = BmnGemStripPositions_FullConfig::ZStationPositions[8];
        ZStationPositions[9] = BmnGemStripPositions_FullConfig::ZStationPositions[9];
        ZStationPositions[10] = BmnGemStripPositions_FullConfig::ZStationPositions[10];
        ZStationPositions[11] = BmnGemStripPositions_FullConfig::ZStationPositions[11];

    BeamPipeMinRadius = 2.5;
    BeamPipeMaxRadius = 5.0;
    BeamHoleRadiuses = new Double_t[NStations];

    DefineBeamHoleRadiuses();

    BuildStations();
}

BmnGemStripStationSet_FullConfig::~BmnGemStripStationSet_FullConfig() {

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

void BmnGemStripStationSet_FullConfig::DefineBeamHoleRadiuses() {
    Double_t BeamPipeAngle = (ATan((BeamPipeMaxRadius-BeamPipeMinRadius) / (ZStationPositions[NStations-1] - ZStationPositions[0])))*180/Pi();
    BeamHoleRadiuses[0] = BeamPipeMinRadius;
    BeamHoleRadiuses[NStations-1] = BeamPipeMaxRadius;
    Double_t delta_const = BeamPipeMaxRadius - BeamPipeMinRadius;
    for(UInt_t iStation = 1; iStation < NStations-1; iStation++) {
        BeamHoleRadiuses[iStation] = delta_const + (ZStationPositions[iStation] - ZStationPositions[0])*Tan((BeamPipeAngle*Pi()/180));
    }
}

void BmnGemStripStationSet_FullConfig::BuildStations() {
    GemStations = new BmnGemStripStation* [NStations];

    for(Int_t iStation = 0; iStation < NStations; iStation++) {
        GemStations[iStation] =
            new BmnGemStripStation_FullConfig(iStation,
                                   XStationPositions[iStation], YStationPositions[iStation], ZStationPositions[iStation],
                                   BeamHoleRadiuses[iStation]);
    }
}

ClassImp(BmnGemStripStationSet_FullConfig)