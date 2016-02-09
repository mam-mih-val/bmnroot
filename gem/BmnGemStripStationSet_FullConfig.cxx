#include "BmnGemStripStationSet_FullConfig.h"
#include "BmnGemStripStation_FullConfig.h"

BmnGemStripStationSet_FullConfig::BmnGemStripStationSet_FullConfig() {

    NStations = 12;

    XStationPositions = new Double_t[NStations];
        XStationPositions[0] = 0.0;
        XStationPositions[1] = 0.0;
        XStationPositions[2] = 0.0;
        XStationPositions[3] = 0.0;
        XStationPositions[4] = 0.0; //prototype
        XStationPositions[5] = 0.0;
        XStationPositions[6] = 0.0;
        XStationPositions[7] = 0.0;
        XStationPositions[8] = 0.0;
        XStationPositions[9] = 0.0;
        XStationPositions[10] = 0.0;
        XStationPositions[11] = 0.0;

    YStationPositions = new Double_t[NStations];
        YStationPositions[0] = 0.0;
        YStationPositions[1] = 0.0;
        YStationPositions[2] = 0.0;
        YStationPositions[3] = 0.0;
        YStationPositions[4] = 0.0; //prototype
        YStationPositions[5] = 0.0;
        YStationPositions[6] = 0.0;
        YStationPositions[7] = 0.0;
        YStationPositions[8] = 0.0;
        YStationPositions[9] = 0.0;
        YStationPositions[10] = 0.0;
        YStationPositions[11] = 0.0;

    ZStationPositions = new Double_t[NStations];
        ZStationPositions[0] = 30.0;
        ZStationPositions[1] = 45.0;
        ZStationPositions[2] = 60.0;
        ZStationPositions[3] = 80.0;
        ZStationPositions[4] = 100.0; //prototype
        ZStationPositions[5] = 130.0;
        ZStationPositions[6] = 160.0;
        ZStationPositions[7] = 190.0;
        ZStationPositions[8] = 230.0;
        ZStationPositions[9] = 270.0;
        ZStationPositions[10] = 315.0;
        ZStationPositions[11] = 360.0;


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

Int_t BmnGemStripStationSet_FullConfig::GetPointStationOwnership(Double_t zcoord) {

    for(Int_t iStation = 0; iStation < NStations; iStation++) {
        if( (zcoord >= ZStationPositions[iStation]) && (zcoord <= (ZStationPositions[iStation]+GemStations[iStation]->GetZSize()))) {
            return iStation;
        }
    }

    return -1;
}

Bool_t BmnGemStripStationSet_FullConfig::AddPointToDetector(Double_t xcoord, Double_t ycoord, Double_t zcoord, Double_t dEloss, Int_t refID) {

    Int_t station = GetPointStationOwnership(zcoord);

    if(station != -1) {
        if( GemStations[station]->AddPointToStation(xcoord, ycoord, zcoord, dEloss, refID) != -1 ) return true;
        else return false;
    }

    return false;
}

Int_t BmnGemStripStationSet_FullConfig::CountNAddedToDetectorPoints() {
    Int_t points_sum = 0;
    for(Int_t iStation = 0; iStation < NStations; iStation++) {
        points_sum += GemStations[iStation]->CountNAddedToStationPoints();
    }
    return points_sum;
}

void BmnGemStripStationSet_FullConfig::ProcessPointsInDetector() {
    for(Int_t iStation = 0; iStation < NStations; iStation++) {
        GemStations[iStation]->ProcessPointsInStation();
    }
}

Int_t BmnGemStripStationSet_FullConfig::CountNProcessedPointsInDetector() {
    Int_t points_sum = 0;
    for(Int_t iStation = 0; iStation < NStations; iStation++) {
        points_sum += GemStations[iStation]->CountNProcessedPointInStation();
    }
    return points_sum;
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