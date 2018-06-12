// @(#)bmnroot/steering:$Id$
// Author: Pavel Batyuk <pavel.batyuk@jinr.ru> 2018-06-12

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// BmnSteeringGemTracking                                                     //
//                                                                            //
//  Mechanism of steering files to set an appropriate values of parameters    //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef BMNSTEERINGGEMTRACKING_H
#define BMNSTEERINGGEMTRACKING_H 1

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <TNamed.h>
#include <TVector3.h>
#include <TSystem.h>

using namespace std;

class BmnSteeringGemTracking : public TNamed {
public:

    BmnSteeringGemTracking();
    BmnSteeringGemTracking(TString);

    virtual ~BmnSteeringGemTracking();

    void PrintParamTable();

    Int_t GetYStep() {
        return fYStep;
    }

    Double_t GetSigX() {
        return fSigX;
    }

    Int_t GetNBins() {
        return fNBins;
    }

    Double_t GetMin() {
        return fMin;
    }

    Double_t GetMax() {
        return fMax;
    }

    Double_t GetLorentzThresh() {
        return fLorentzThresh;
    }

    Int_t GetCoeffYStep() {
        return fCoeffYStep;
    }

    Double_t GetCoeffLineFitCut() {
        return fCoeffLineFitCut;
    }

    Double_t GetCoeffGemDistCut() {
        return fCoeffGemDistCut;
    }
    
    Int_t** GetStatsForSeeding() {
        return _fStatsForSeeding;
    }
    
    Int_t GetNCombs() {
        return fNCombs;
    }
    
    Int_t GetNStations() {
        return fNStations;
    }
    
    Double_t* GetGemDistCut() {
        return fGemDistCut;
    }
    
    Double_t GetLineFitCut() {
        return fLineFitCut;
    }
    
    Int_t GetNHitsCut() {
        return fNHitsCut;
    }
    
    Int_t GetNSeedsCut() {
        return fNSeedsCut;
    }
    
    Int_t GetNHitsInGemCut() {
        return fNHitsInGemCut;
    }
    
    Int_t GetNSharedHits() {
        return fNSharedHits;
    }
    
    Bool_t IsCovMatrixApproxUsed() {
        return isCovMatrixApproxUsed;
    }
    
    Bool_t IsRoughVertexApproxUsed() {
        return isUseRoughVertex;
    }
    
    Bool_t IsChi2SortUsed() {
        return isChi2SortUsed;
    }
    
    Bool_t IsNHitsSortUsed() {
        return isNHitsSortUsed;
    }
    
    Bool_t IsHitErrorsScaleUsed() {
        return isHitErrorsScaleUsed;
    }
    
    Double_t* GetCovMatrix() {
        return fCovMatrix;
    }
    
    Double_t* GetErrorScales() {
        return fErrScaleFact;
    }
    
    TVector3 GetRoughVertex() {
        return fRoughVertex;
    }

private:   
    void ParseSteerFile(TString);

    TString fSteerFile;

    Int_t fPDG;

    Int_t fNHitsCut; 
    Double_t fLineFitCut; 
    Int_t fNHitsInGemCut; 
    Int_t fNSeedsCut; 
    Int_t fNSharedHits;

    Bool_t isChi2SortUsed;
    Bool_t isNHitsSortUsed;
   
    Int_t fYStep; 
    Double_t fSigX;
    Int_t fNBins; 
    Double_t fMax;
    Double_t fMin;
    Double_t fLorentzThresh;
    Int_t fCoeffYStep;
    Double_t fCoeffLineFitCut;
    Double_t fCoeffGemDistCut;

    Int_t fNStations; 
    Double_t* fGemDistCut;

    Bool_t isHitErrorsScaleUsed;
    Double_t fErrScaleFact[3];

    Int_t fNCombs;
    vector <TString> fStatsForSeeding;
    Int_t** _fStatsForSeeding;

    Bool_t isCovMatrixApproxUsed;
    Double_t fCovMatrix[15];

    Bool_t isUseRoughVertex;
    TVector3 fRoughVertex;

    ClassDef(BmnSteeringGemTracking, 1);
};

#endif