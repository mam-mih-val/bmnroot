// @(#)bmnroot/steering:$Id$
// Author: Pavel Batyuk <pavel.batyuk@jinr.ru> 2018-09-12

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// BmnSteeringCA                                                              //
//                                                                            //
//  Mechanism of steering files to set an appropriate values of parameters    //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef BMNSTEERING_H
#define BMNSTEERING_H 1

#include <TNamed.h>
#include <TMath.h>
#include <TVector3.h>
#include <TSystem.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;
using namespace TMath;

class BmnSteering : public TNamed
{
  public:
    BmnSteering();
    BmnSteering(TString);
    virtual ~BmnSteering();

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

    const Char_t* GetName() {
        return fSteerFile.Data();
    }

private:
    void ParseSteerFile(TString);
    void TestParsedInput();

    TString fSteerFile;

    Int_t fNStatsInnerTracker;

    Double_t* fHitXCutMin;          //[fNStatsInnerTracker]
    Double_t* fHitXCutMax;          //[fNStatsInnerTracker]

    Double_t* fHitYCutMin;          //[fNStatsInnerTracker]
    Double_t* fHitYCutMax;          //[fNStatsInnerTracker]

    Double_t* fCellSlopeXZCutMin;   //[fNStatsInnerTracker]
    Double_t* fCellSlopeXZCutMax;   //[fNStatsInnerTracker]

    Double_t* fCellSlopeYZCutMin;   //[fNStatsInnerTracker]
    Double_t* fCellSlopeYZCutMax;   //[fNStatsInnerTracker]

    Double_t fDiffSlopeXZ0;
    Double_t fDiffSlopeYZ0;

    Double_t fDiffSlopeXZSlope;
    Double_t fDiffSlopeYZSlope;

    Double_t fChiSquareCut;
    Int_t fNIter;

    Int_t fNHitsCut;
    Int_t fNHitsCutTotal;
    
    Double_t fMarkerValue;

  ClassDef(BmnSteering, 1);
};

#endif
