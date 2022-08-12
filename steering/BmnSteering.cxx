// @(#)bmnroot/steering:$Id$
// Author: Pavel Batyuk <pavel.batyuk@jinr.ru> 2018-09-12

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// BmnSteering                                                              //
//                                                                            //
//  Mechanism of steering files to set an appropriate values of parameters    //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include <cfloat>
#include <vector>
#include "BmnSteering.h"

BmnSteering::~BmnSteering()
{}

BmnSteering::BmnSteering() :
  fHitXCutMin(nullptr),
  fHitXCutMax(nullptr),
  fHitYCutMin(nullptr),
  fHitYCutMax(nullptr),
  fCellSlopeXZCutMin(nullptr),
  fCellSlopeXZCutMax(nullptr),
  fCellSlopeYZCutMin(nullptr),
  fCellSlopeYZCutMax(nullptr)
{}

BmnSteering::BmnSteering(TString fileName) :
  fSteerFile(fileName),
  fMarkerValue(-1000.)
{
    fDiffSlopeXZ0 = fMarkerValue;
    fDiffSlopeYZ0 = fMarkerValue;
    fDiffSlopeXZSlope = fMarkerValue;
    fDiffSlopeYZSlope = fMarkerValue;
    fChiSquareCut = fMarkerValue;

    fNIter = fMarkerValue;
    fNHitsCut = fMarkerValue;
    fNHitsCutTotal = fMarkerValue;

    ParseSteerFile(fSteerFile);
    TestParsedInput();
}

void BmnSteering::TestParsedInput() {
    //Check for non-initialized elements of arrays
    vector <Double_t> test1;
    vector <Double_t> test2;

    test1.resize(fNStatsInnerTracker);
    test2.resize(fNStatsInnerTracker);

    for (Int_t iEle = 0; iEle < fNStatsInnerTracker; iEle++) {
        test1.clear();
        test2.clear();

        test1.push_back(fHitXCutMin[iEle]);
        test1.push_back(fHitXCutMax[iEle]);
        test1.push_back(fHitYCutMin[iEle]);
        test1.push_back(fHitYCutMax[iEle]);
        test1.push_back(fCellSlopeXZCutMin[iEle]);
        test1.push_back(fCellSlopeXZCutMax[iEle]);
        test1.push_back(fCellSlopeYZCutMin[iEle]);
        test1.push_back(fCellSlopeYZCutMax[iEle]);

        test2.push_back(Abs(fHitXCutMin[iEle] - fHitXCutMax[iEle]));
        test2.push_back(Abs(fHitYCutMin[iEle] - fHitYCutMax[iEle]));
        test2.push_back(Abs(fCellSlopeXZCutMin[iEle] - fCellSlopeXZCutMax[iEle]));
        test2.push_back(Abs(fCellSlopeYZCutMin[iEle] - fCellSlopeYZCutMax[iEle]));

        for (auto it : test1)
            if (Abs(it - fMarkerValue) < DBL_EPSILON) {
                cout << "Steering containes missing / incorrect elements!!!" << endl;
                throw;
            }

        for (auto it : test2)
            if (it <= DBL_EPSILON) {
                cout << "Steering containes missing / incorrect elements!!!" << endl;
                throw;
            }
    }

    if (Abs(fDiffSlopeXZ0 - fMarkerValue) < DBL_EPSILON ||
            Abs(fDiffSlopeYZ0 - fMarkerValue) < DBL_EPSILON ||
            Abs(fDiffSlopeXZSlope - fMarkerValue) < DBL_EPSILON ||
            Abs(fDiffSlopeYZSlope - fMarkerValue) < DBL_EPSILON ||
            Abs(fChiSquareCut - fMarkerValue) < DBL_EPSILON) {
        cout << "Steering containes missing / incorrect elements!!!" << endl;
        throw;
    }

    if (fNIter == fMarkerValue || fNHitsCut == fMarkerValue ||
            fNHitsCutTotal == fMarkerValue) {
        cout << "Steering containes missing / incorrect elements!!!" << endl;
        throw;
    }
}

void BmnSteering::PrintParamTable() {
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

void BmnSteering::ParseSteerFile(TString fileName) {
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

    for (Int_t iEle = 0; iEle < fNStatsInnerTracker; iEle++) {
        fHitXCutMin[iEle] = fMarkerValue;
        fHitXCutMax[iEle] = fMarkerValue;
        fHitYCutMin[iEle] = fMarkerValue;
        fHitYCutMax[iEle] = fMarkerValue;
        fCellSlopeXZCutMin[iEle] = fMarkerValue;
        fCellSlopeXZCutMax[iEle] = fMarkerValue;
        fCellSlopeYZCutMin[iEle] = fMarkerValue;
        fCellSlopeYZCutMax[iEle] = fMarkerValue;
    }

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

