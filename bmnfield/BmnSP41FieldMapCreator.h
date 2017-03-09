#ifndef BMNSP41FIELDMAPCREATOR_H
#define BMNSP41FIELDMAPCREATOR_H 1

#include <TCanvas.h>
#include <TPad.h>
#include <TFile.h>
#include <TAxis.h>
#include <TMath.h>
#include <TNamed.h>
#include <TF1.h>
#include <TGraph.h>
#include <TGraph2D.h>
#include <TVector3.h>
#include <TFitResult.h>
#include <TFitResultPtr.h>
#include <TString.h>
#include "BmnFieldMap.h"
#include "BmnNewFieldMap.h"

using namespace std;
using namespace TMath;

class BmnSP41FieldMapCreator : public TNamed {
public:

    BmnSP41FieldMapCreator() {
    };
    BmnSP41FieldMapCreator(TString);
    virtual ~BmnSP41FieldMapCreator();

    void AnalyzeFieldProfiles(Int_t, Int_t);
    void CreateNewMap(TString name = "");

    void SetOutParamFileName(TString name) {
        fParamFileName = name;
    }
    
    void SetDrawProfiles(Bool_t flag) {
        isDrawProfile = flag;
    }
    
    void SetDebug(Bool_t flag) {
        fDebug = flag;
    }
    
    void SetNodeNumber(Int_t nNodes) {
        dimGrid = nNodes;
    }

private:
    TString nameOutput;
    
    Bool_t isDrawProfile;
    Bool_t fDebug;

    Double_t Zmax; // Upper Z-limit of the used extrapolation
    Double_t Zmin; // Lower Z-limit of the used extrapolation

    Double_t Zmin_RIGHT; // Lower Z-limit to find extrapolation functions 
    Double_t Zmax_RIGHT; // Upper Z-limit to find extrapolation functions 

    Double_t Zmin_LEFT; // Lower Z-limit to find extrapolation functions 
    Double_t Zmax_LEFT; // Upper Z-limit to find extrapolation functions 

    Int_t Npoints; // Number of nodes along Z-axis
    Int_t dimGrid; // dimGrid x dimGrid corresponds to number of nodes to build a grid along X(Y)-axis 
    Int_t dimNhist; // dimNhist corresponds to number of profiles drawn

    Double_t const_restrict; // ??? DO WE NEED IT?
    Double_t factor; // Transition from T to kG
    Double_t WinXmin;
    Double_t WinXmax;
    Double_t WinYmin;
    Double_t WinYmax;
    Double_t winXstep;
    Double_t winYstep;

    Int_t nComponents;
    Int_t nParts;

    TGraph2D*** constant;
    TGraph2D*** mean;
    TGraph2D*** sigma;

    BmnFieldMap* fMap;

    TString fParamFileName;

    void DrawProfileExtrap(Int_t, TGraph***, TF1***, TString, TString, Double_t, Double_t, Double_t, Double_t);
    Double_t FieldExtrapolate(Double_t, Double_t, Double_t, TGraph2D*, TGraph2D*, TGraph2D*);
    void Print(BmnFieldMap*);
   
    ClassDef(BmnSP41FieldMapCreator, 1)
};

#endif