#include "BmnGemStripStationSet.h"

BmnGemStripStationSet::BmnGemStripStationSet() {

    NStations = 12;

    ZStantionPositions = new Double_t[NStations];
        ZStantionPositions[0] = 30.0;
        ZStantionPositions[1] = 50.0;
        ZStantionPositions[2] = 70.0;
        ZStantionPositions[3] = 90.0;
        ZStantionPositions[4] = 110.0;
        ZStantionPositions[5] = 140.0;
        ZStantionPositions[6] = 170.0;
        ZStantionPositions[7] = 200.0;
        ZStantionPositions[8] = 240.0;
        ZStantionPositions[9] = 280.0;
        ZStantionPositions[10] = 320.0;
        ZStantionPositions[11] = 360.0;

    XModulesSizes = new Double_t[NStations];
        XModulesSizes[0] = 60.0/2;
        XModulesSizes[1] = 60.0/2;
        XModulesSizes[2] = 80.0/2;
        XModulesSizes[3] = 132.0/2;
        XModulesSizes[4] = 160.0/2;
        XModulesSizes[5] = 160.0/2;
        XModulesSizes[6] = 240.0/2;
        XModulesSizes[7] = 240.0/2;
        XModulesSizes[8] = 240.0/2;
        XModulesSizes[9] = 240.0/2;
        XModulesSizes[10] = 240.0/2;
        XModulesSizes[11] = 240.0/2;

    YModulesSizes = new Double_t[NStations];
        YModulesSizes[0] = 60.0/2;
        YModulesSizes[1] = 60.0/2;
        YModulesSizes[2] = 80.0/2;
        YModulesSizes[3] = 82.0/2;
        YModulesSizes[4] = 80.0/2;
        YModulesSizes[5] = 80.0/2;
        YModulesSizes[6] = 80.0/2;
        YModulesSizes[7] = 80.0/2;
        YModulesSizes[8] = 80.0/2;
        YModulesSizes[9] = 80.0/2;
        YModulesSizes[10] = 80.0/2;
        YModulesSizes[11] = 80.0/2;

    BeamPipeMinRadius = 2.5;
    BeamPipeMaxRadius = 5.0;
    BeamPipeRadiuses = new Double_t[NStations];
    CalculateBeamPipeRadiuses();

    BuildStations();
}

BmnGemStripStationSet::~BmnGemStripStationSet() {

    delete [] ZStantionPositions;
    delete [] XModulesSizes;
    delete [] YModulesSizes;
    delete [] BeamPipeRadiuses;

    for(Int_t i = 0; i < NStations; i++) {
        if(GemStations[i]) delete GemStations[i];
    }
}

Int_t BmnGemStripStationSet::GetPointStationOwnership(Double_t zcoord) {

    for(Int_t iStation = 0; iStation < NStations; iStation++) {
/*
        if( ( (zcoord-GemStations[iStation]->ZSizeStation/2) >= ZStantionPositions[iStation] && zcoord <= (ZStantionPositions[iStation]+GemStations[iStation]->ZSizeStation/2) ) { // if pos-z of station is in geometry center of station
 */
        if( (zcoord >= ZStantionPositions[iStation]) && (zcoord <= (ZStantionPositions[iStation]+GemStations[iStation]->ZSizeStation))) {
            return iStation;
        }
    }

    return -1;
}

Bool_t BmnGemStripStationSet::AddPointToDetector(Double_t xcoord, Double_t ycoord, Double_t zcoord, Double_t dEloss) {

    Int_t station = GetPointStationOwnership(zcoord);

    if(station != -1) {
        if( GemStations[station]->AddPointToStation(xcoord, ycoord, zcoord, dEloss) != -1 ) return true;
        else return false;
    }

    return false;
}

Int_t BmnGemStripStationSet::CountNAddedToDetectorPoints() {
    Int_t points_sum = 0;
    for(Int_t iStation = 0; iStation < NStations; iStation++) {
        points_sum += GemStations[iStation]->CountNAddedToStationPoints();
    }
    return points_sum;
}


void BmnGemStripStationSet::ProcessPointsInDetector() {
    for(Int_t iStation = 0; iStation < NStations; iStation++) {
        GemStations[iStation]->ProcessPointsInStation();
    }
}

Int_t BmnGemStripStationSet::CountNProcessedPointsInDetector() {
    Int_t points_sum = 0;
    for(Int_t iStation = 0; iStation < NStations; iStation++) {
        points_sum += GemStations[iStation]->CountNProcessedPointInStation();
    }
    return points_sum;
}

BmnGemStripStation* BmnGemStripStationSet::GetGemStation(Int_t index) {
    if(index >= 0 && index < NStations) {
        return GemStations[index];
    }
    return 0;
}


void BmnGemStripStationSet::CalculateBeamPipeRadiuses() {
    Double_t BeamPipeAngle = (ATan((BeamPipeMaxRadius-BeamPipeMinRadius) / (ZStantionPositions[NStations-1] - ZStantionPositions[0])))*180/Pi();
    BeamPipeRadiuses[0] = BeamPipeMinRadius;
    BeamPipeRadiuses[NStations-1] = BeamPipeMaxRadius;
    Double_t delta_const = BeamPipeMaxRadius - BeamPipeMinRadius;
    for(UInt_t iStation = 1; iStation < NStations-1; iStation++) {
        BeamPipeRadiuses[iStation] = delta_const + (ZStantionPositions[iStation] - ZStantionPositions[0])*Tan((BeamPipeAngle*Pi()/180));
    }
}

void BmnGemStripStationSet::BuildStations() {

    for(Int_t iStation = 0; iStation < NStations; iStation++) {
        GemStations[iStation] =
            new BmnGemStripStation(iStation,
                                   XModulesSizes[iStation], YModulesSizes[iStation],
                                   ZStantionPositions[iStation], BeamPipeRadiuses[iStation]);
    }
}

ClassImp(BmnGemStripStationSet)