// @(#)bmnroot/steering:$Id$
// Author: Pavel Batyuk <pavel.batyuk@jinr.ru> 2018-09-12

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// BmnSteeringCA                                                              //
//                                                                            //
//  Mechanism of steering files to set an appropriate values of parameters    //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef BMNSTEERINGCA_H
#define BMNSTEERINGCA_H 1

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <TNamed.h>
#include <TVector3.h>
#include <TSystem.h>

using namespace std;

class BmnSteeringCA : public TNamed {
public:

    BmnSteeringCA();
    BmnSteeringCA(TString);

    virtual ~BmnSteeringCA();

    void PrintParamTable();

    Int_t GetNStations() {
        return fNStatsInnerTracker;
    }

    Double_t* GetHitXCutMin() {
        return fHitXCutMin;
    }

    Double_t* GetHitYCutMin() {
        return fHitYCutMin;
    }

    Double_t* GetHitXCutMax() {
        return fHitXCutMax;
    }

    Double_t* GetHitYCutMax() {
        return fHitYCutMax;
    }

    Double_t* GetCellSlopeXZCutMin() {
        return fCellSlopeXZCutMin;
    }

    Double_t* GetCellSlopeYZCutMin() {
        return fCellSlopeYZCutMin;
    }

    Double_t* GetCellSlopeXZCutMax() {
        return fCellSlopeXZCutMax;
    }

    Double_t* GetCellSlopeYZCutMax() {
        return fCellSlopeYZCutMax;
    }

    Double_t GetDiffSlopeXZ0() {
        return fDiffSlopeXZ0;
    }

    Double_t GetDiffSlopeYZ0() {
        return fDiffSlopeYZ0;
    }

    Double_t GetDiffSlopeXZSlope() {
        return fDiffSlopeXZSlope;
    }

    Double_t GetDiffSlopeYZSlope() {
        return fDiffSlopeYZSlope;
    }

    Double_t GetChiSquareCut() {
        return fChiSquareCut;
    }

    Int_t GetNIter() {
        return fNIter;
    }

    Int_t GetNHitsCut() {
        return fNHitsCut;
    }

    Int_t GetNHitsCutTotal() {
        return fNHitsCutTotal;
    }


private:
    void ParseSteerFile(TString);

    TString fSteerFile;

    Int_t fNStatsInnerTracker;

    Double_t* fHitXCutMin;
    Double_t* fHitXCutMax;

    Double_t* fHitYCutMin;
    Double_t* fHitYCutMax;

    Double_t* fCellSlopeXZCutMin;
    Double_t* fCellSlopeXZCutMax;

    Double_t* fCellSlopeYZCutMin;
    Double_t* fCellSlopeYZCutMax;

    Double_t fDiffSlopeXZ0;
    Double_t fDiffSlopeYZ0;

    Double_t fDiffSlopeXZSlope;
    Double_t fDiffSlopeYZSlope;

    Double_t fChiSquareCut;
    Int_t fNIter;

    Int_t fNHitsCut;
    Int_t fNHitsCutTotal;

    ClassDef(BmnSteeringCA, 1);
};

#endif