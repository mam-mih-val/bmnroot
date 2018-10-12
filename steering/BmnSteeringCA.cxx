// @(#)bmnroot/steering:$Id$
// Author: Pavel Batyuk <pavel.batyuk@jinr.ru> 2018-09-12

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// BmnSteeringCA                                                              //
//                                                                            //
//  Mechanism of steering files to set an appropriate values of parameters    //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include <cfloat>
#include <vector>
#include "BmnSteeringCA.h"

BmnSteeringCA::~BmnSteeringCA() {

}

BmnSteeringCA::BmnSteeringCA() {

}

BmnSteeringCA::BmnSteeringCA(TString fileName) :
fSteerFile(fileName) {

   
    
    ParseSteerFile(fSteerFile);
}

void BmnSteeringCA::PrintParamTable() {
    cout << "Steering file: " << fSteerFile << endl;
    
    cout << "fNStations = " << fNStatsInnerTracker << endl;
    
    cout << "hitXCutMin: " << endl;
    for (Int_t iStat = 0; iStat < fNStatsInnerTracker; iStat++)
        cout << iStat << " " << fHitXCutMin[iStat] << endl;
    
    cout << "hitXCutMax: " << endl;
    for (Int_t iStat = 0; iStat < fNStatsInnerTracker; iStat++)
        cout << iStat << " " << fHitXCutMax[iStat] << endl;
    
    cout << "hitYCutMin: " << endl;
    for (Int_t iStat = 0; iStat < fNStatsInnerTracker; iStat++)
        cout << iStat << " " << fHitYCutMin[iStat] << endl;
    
    cout << "hitYCutMax: " << endl;
    for (Int_t iStat = 0; iStat < fNStatsInnerTracker; iStat++)
        cout << iStat << " " << fHitYCutMax[iStat] << endl;
    
    cout << "cellSlopeXZCutMin: " << endl;
    for (Int_t iStat = 0; iStat < fNStatsInnerTracker; iStat++)
        cout << iStat << " " << fCellSlopeXZCutMin[iStat] << endl;
    
    cout << "cellSlopeXZCutMax: " << endl;
    for (Int_t iStat = 0; iStat < fNStatsInnerTracker; iStat++)
        cout << iStat << " " << fCellSlopeXZCutMax[iStat] << endl;

    cout << "cellSlopeYZCutMin: " << endl;
    for (Int_t iStat = 0; iStat < fNStatsInnerTracker; iStat++)
        cout << iStat << " " << fCellSlopeYZCutMin[iStat] << endl;
    
    cout << "cellSlopeYZCutMax: " << endl;
    for (Int_t iStat = 0; iStat < fNStatsInnerTracker; iStat++)
        cout << iStat << " " << fCellSlopeYZCutMax[iStat] << endl;
    
    cout << "diffSlopeXZ0 = " << fDiffSlopeXZ0 << endl;
    cout << "diffSlopeXZSlope = " << fDiffSlopeXZSlope << endl;
    cout << "diffSlopeYZ0 = " << fDiffSlopeYZ0 << endl;
    cout << "diffSlopeYZSlope = " << fDiffSlopeYZSlope << endl;
    cout << "chiSquareCut = " << fChiSquareCut << endl;
    cout << "nIter = " << fNIter << endl;
    cout << "nHitsCut = " << fNHitsCut << endl;
    cout << "nHitsCutTotal = " << fNHitsCutTotal << endl;
}

void BmnSteeringCA::ParseSteerFile(TString fileName) {
    TString gPathConfig = gSystem->Getenv("VMCWORKDIR");
    TString gPathFull = gPathConfig + "/macro/steering/" + fileName;

    TString tmp = "";
    string line;
    ifstream f(gPathFull.Data(), ios::in);

    f >> tmp >> fNStatsInnerTracker;
    
    fHitXCutMin = new Double_t[fNStatsInnerTracker];
    fHitXCutMax = new Double_t[fNStatsInnerTracker];

    fHitYCutMin = new Double_t[fNStatsInnerTracker];
    fHitYCutMax = new Double_t[fNStatsInnerTracker];

    fCellSlopeXZCutMin = new Double_t[fNStatsInnerTracker];
    fCellSlopeXZCutMax = new Double_t[fNStatsInnerTracker];

    fCellSlopeYZCutMin = new Double_t[fNStatsInnerTracker];
    fCellSlopeYZCutMax = new Double_t[fNStatsInnerTracker];

    getline(f, line);
    f >> tmp;
    for (Int_t iStat = 0; iStat < fNStatsInnerTracker; iStat++)
        f >> fHitXCutMin[iStat];

    getline(f, line);
    f >> tmp;
    for (Int_t iStat = 0; iStat < fNStatsInnerTracker; iStat++)
        f >> fHitXCutMax[iStat];

    getline(f, line);
    f >> tmp;
    for (Int_t iStat = 0; iStat < fNStatsInnerTracker; iStat++)
        f >> fHitYCutMin[iStat];

    getline(f, line);
    f >> tmp;
    for (Int_t iStat = 0; iStat < fNStatsInnerTracker; iStat++)
        f >> fHitYCutMax[iStat];

    getline(f, line);
    f >> tmp;
    for (Int_t iStat = 0; iStat < fNStatsInnerTracker; iStat++)
        f >> fCellSlopeXZCutMin[iStat];

    getline(f, line);
    f >> tmp;
    for (Int_t iStat = 0; iStat < fNStatsInnerTracker; iStat++)
        f >> fCellSlopeXZCutMax[iStat];

    getline(f, line);
    f >> tmp;
    for (Int_t iStat = 0; iStat < fNStatsInnerTracker; iStat++)
        f >> fCellSlopeYZCutMin[iStat];

    getline(f, line);
    f >> tmp;
    for (Int_t iStat = 0; iStat < fNStatsInnerTracker; iStat++)
        f >> fCellSlopeYZCutMax[iStat];

    getline(f, line);
    f >> tmp >> fDiffSlopeXZ0;

    getline(f, line);
    f >> tmp >> fDiffSlopeXZSlope;

    getline(f, line);
    f >> tmp >> fDiffSlopeYZ0;

    getline(f, line);
    f >> tmp >> fDiffSlopeYZSlope;

    getline(f, line);
    f >> tmp >> fChiSquareCut;

    getline(f, line);
    f >> tmp >> fNIter;

    getline(f, line);
    f >> tmp >> fNHitsCut;

    getline(f, line);
    f >> tmp >> fNHitsCutTotal;
}

