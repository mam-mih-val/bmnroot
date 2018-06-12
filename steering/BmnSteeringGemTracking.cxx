// @(#)bmnroot/steering:$Id$
// Author: Pavel Batyuk <pavel.batyuk@jinr.ru> 2018-06-12

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// BmnSteeringGemTracking                                                     //
//                                                                            //
//  Mechanism of steering files to set an appropriate values of parameters    //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include <cfloat>
#include <vector>
#include "BmnSteeringGemTracking.h"

BmnSteeringGemTracking::~BmnSteeringGemTracking() {

}

BmnSteeringGemTracking::BmnSteeringGemTracking() {

}

BmnSteeringGemTracking::BmnSteeringGemTracking(TString fileName) :
fSteerFile(fileName),
fPDG(2212),
fNHitsCut(-1),
fLineFitCut(DBL_MAX),
fNHitsInGemCut(-1),
fNSeedsCut(-1),
fNSharedHits(-1),
isChi2SortUsed(kFALSE),
isNHitsSortUsed(kFALSE),
fYStep(-1),
fSigX(DBL_MAX),
fNBins(-1),
fMax(DBL_MAX),
fMin(-DBL_MAX),
fLorentzThresh(DBL_MAX),
fCoeffYStep(-1),
fCoeffLineFitCut(DBL_MAX),
fCoeffGemDistCut(DBL_MAX),
fNStations(0),
isHitErrorsScaleUsed(kFALSE),
isCovMatrixApproxUsed(kFALSE),
isUseRoughVertex(kFALSE),
fNCombs(0) {
    fStatsForSeeding.resize(fNCombs);

    for (Int_t iEle = 0; iEle < 15; iEle++)
        fCovMatrix[iEle] = 0.;

    fRoughVertex.SetXYZ(0., 0., 0.);

    for (Int_t iEle = 0; iEle < 3; iEle++)
        fErrScaleFact[iEle] = 1.;

    ParseSteerFile(fSteerFile);
}

void BmnSteeringGemTracking::PrintParamTable() {
    cout << "Params to be used when BmnGemTracking executing ..." << endl;
    cout << "Steering file: " << fSteerFile << endl;
    cout << "fPDG = " << fPDG << endl;
    cout << "fNStations = " << fNStations << endl;

    cout << "fGemDistCut: " << endl;
    for (Int_t iStat = 0; iStat < fNStations; iStat++)
        cout << iStat << " " << fGemDistCut[iStat] << endl;

    cout << "Stats for seeding: " << endl;
    for (Int_t iComb = 0; iComb < fNCombs; iComb++) {
        cout << "Trio " << iComb << endl;
        for (Int_t iEle = 0; iEle < 3; iEle++)
            cout << _fStatsForSeeding[iComb][iEle] << " ";
        cout << endl;
    }

    cout << "Cut values: " << endl;
    cout << "fNHitsCut = " << fNHitsCut << endl;
    cout << "fLineFitCut = " << fLineFitCut << endl;
    cout << "fNHitsInGemCut = " << fNHitsInGemCut << endl;
    cout << "fNSeedsCut = " << fNSeedsCut << endl;
    cout << "kNSharedHits = " << fNSharedHits << endl;

    cout << "Sort options: " << endl;
    cout << "isChi2SortUsed = " << isChi2SortUsed << endl;
    cout << "isNHitsSortUsed = " << isNHitsSortUsed << endl;

    cout << "Lorentz filtration params: " << endl;
    cout << "fYStep = " << fYStep << endl;
    cout << "fSigX = " << fSigX << endl;
    cout << "fNBins = " << fNBins << endl;
    cout << "fMax = " << fMax << endl;
    cout << "fMin = " << fMin << endl;
    cout << "fLorentzThresh = " << fLorentzThresh << endl;
    cout << "fCoeffYStep = " << fCoeffYStep << endl;
    cout << "fCoeffLineFitCut = " << fCoeffLineFitCut << endl;
    cout << "fCoeffGemDistCut = " << fCoeffGemDistCut << endl;

    cout << "Hit errs. params:" << endl;
    cout << "isHitErrorsScaleUsed = " << isHitErrorsScaleUsed << endl;
    for (Int_t iEle = 0; iEle < 3; iEle++)
        cout << iEle << " " << fErrScaleFact[iEle] << endl;

    cout << "Other approximations to be used: " << endl;
    cout << "isCovMatrixApproxUsed = " << isCovMatrixApproxUsed << endl;
    cout << "Cov. matrix elements: " << endl;
    for (Int_t iEle = 0; iEle < 15; iEle++)
        cout << iEle << " " << fCovMatrix[iEle] << endl;

    cout << "isUseRoughVertex = " << isUseRoughVertex << endl;
    cout << "Rough vertex: " << endl;
    fRoughVertex.Print();
}

void BmnSteeringGemTracking::ParseSteerFile(TString fileName) {
    TString gPathConfig = gSystem->Getenv("VMCWORKDIR");
    TString gPathFull = gPathConfig + "/macro/steering/" + fileName;

    TString tmp = "";
    string line;
    ifstream f(gPathFull.Data(), ios::in);
    f >> tmp >> fNStations;
    fGemDistCut = new Double_t[fNStations];
    
    getline(f, line);
    f >> tmp;
    for (Int_t iStat = 0; iStat < fNStations; iStat++)
        f >> fGemDistCut[iStat];
    
    getline(f, line);
    f >> tmp >> fNHitsCut;
    
    getline(f, line);
    f >> tmp >> fLineFitCut;
    
    getline(f, line);
    f >> tmp >> fNHitsInGemCut;
    
    getline(f, line);
    f >> tmp >> fNSeedsCut;
    
    getline(f, line);
    f >> tmp >> fNSharedHits;
    
    getline(f, line);
    f >> tmp >> fYStep;

    getline(f, line);
    f >> tmp >> fSigX;

    getline(f, line);
    f >> tmp >> fNBins;

    getline(f, line);
    f >> tmp >> fMin;

    getline(f, line);
    f >> tmp >> fMax;

    getline(f, line);
    f >> tmp >> fLorentzThresh;

    getline(f, line);
    f >> tmp >> fCoeffYStep;

    getline(f, line);
    f >> tmp >> fCoeffLineFitCut;

    getline(f, line);
    f >> tmp >> fCoeffGemDistCut;

    //
    getline(f, line);
    f >> tmp >> isCovMatrixApproxUsed;
    
    getline(f, line);
    f >> tmp;
    for (Int_t iEle = 0; iEle < 15; iEle++) 
        if (isCovMatrixApproxUsed)
            f >> fCovMatrix[iEle];
 
    getline(f, line);
    f >> tmp >> isUseRoughVertex;

    getline(f, line);
    Double_t x, y, z;
    f >> tmp >> x >> y >> z;
    if (isUseRoughVertex)
        fRoughVertex.SetXYZ(x, y, z);

    getline(f, line);
    f >> tmp >> isHitErrorsScaleUsed;

    getline(f, line);
    if (isHitErrorsScaleUsed)
        f >> tmp >> fErrScaleFact[0] >> fErrScaleFact[1] >> fErrScaleFact[2];

    getline(f, line);
    f >> tmp >> fNCombs;

    getline(f, line);
    f >> tmp;
    for (Int_t iComb = 0; iComb < fNCombs; iComb++) {
        TString comb = "";
        f >> comb;
        fStatsForSeeding.push_back(comb);
    }

    _fStatsForSeeding = new Int_t*[fNCombs];
    for (Int_t iComb = 0; iComb < fNCombs; iComb++) {
        _fStatsForSeeding[iComb] = new Int_t[3];
        for (Int_t iEle = 0; iEle < 3; iEle++)
            _fStatsForSeeding[iComb][iEle] = TString(fStatsForSeeding.at(iComb).Data()[iEle]).Atoi();
    }
    
    getline(f, line);
    f >> tmp >> isChi2SortUsed;
    
    getline(f, line);
    f >> tmp >> isNHitsSortUsed;
}

