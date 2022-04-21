#ifndef BMNSP41FIELDMAPCREATOR_H
#define BMNSP41FIELDMAPCREATOR_H 1

#include <TCanvas.h>
#include <TStyle.h>
#include <TPad.h>
#include <TFile.h>
#include <TAxis.h>
#include <TMath.h>
#include <TNamed.h>
#include <TClonesArray.h>
#include <TLorentzVector.h>
#include <TF1.h>
#include <TH1.h>
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

class fitParMonitor : public TObject {
public:

    fitParMonitor() {
        ;
    }

    fitParMonitor(TString out) :
    C(nullptr),
    M(nullptr),
    S(nullptr),
    f(nullptr) {
        const Int_t nParts = 2; // Left part, right part of extrapolation to be done ...
        const Int_t nComp = 3; // Left part, right part of extrapolation to be done ...

        C = new TH1F**[nParts];
        M = new TH1F**[nParts];
        S = new TH1F**[nParts];

        f = new TFile(Form("fitParMonitor_%s.root", out.Data()), "recreate");

        for (Int_t iPart = 0; iPart < nParts; iPart++) {
            C[iPart] = new TH1F*[nComp];
            M[iPart] = new TH1F*[nComp];
            S[iPart] = new TH1F*[nComp];
            for (Int_t iComp = 0; iComp < nComp; iComp++) {
                C[iPart][iComp] = new TH1F(Form("Constant_%d_%d", iPart, iComp), "Constant", 100, 0., 0.);
                M[iPart][iComp] = new TH1F(Form("Mean_%d_%d", iPart, iComp), "Mean", 100, 0., 0.);
                S[iPart][iComp] = new TH1F(Form("Sigma_%d_%d", iPart, iComp), "Sigma", 100, 0., 0.);
            }
        }
    }

    virtual ~fitParMonitor() {
        for (Int_t iPart = 0; iPart < 2; iPart++) {
            for (Int_t iComp = 0; iComp < 3; iComp++) {
                C[iPart][iComp]->Write();
                M[iPart][iComp]->Write();
                S[iPart][iComp]->Write();
            }
        }

        if (f)
            delete f;
    }

public:
    TFile* f;

    TH1F*** C;
    TH1F*** M;
    TH1F*** S;

    ClassDef(fitParMonitor, 1)
};

class extrapolationConditions : public TObject {
public:

    extrapolationConditions() {
        direction = "Z";
        min = -DBL_MAX;
        max = DBL_MAX;
    }

    virtual ~extrapolationConditions() {
        ;
    }

public:
    TString direction;
    Double_t min;
    Double_t max;

    pair <Double_t, Double_t> tailRangeLeft;
    pair <Double_t, Double_t> tailRangeRight;

    ClassDef(extrapolationConditions, 1)
};

class smoothedValues : public TObject {
public:

    smoothedValues() {
        X = 0.;
        Y = 0.;
        Z = 0.;

        fieldValue = 0.;
    }

    virtual ~smoothedValues() {
        ;
    }

public:
    Double_t X;
    Double_t Y;
    Double_t Z;

    Double_t fieldValue;

    ClassDef(smoothedValues, 1)
};

class BmnSP41FieldMapCreator : public TNamed {
public:

    BmnSP41FieldMapCreator() {
    };
    BmnSP41FieldMapCreator(TString, TString);
    virtual ~BmnSP41FieldMapCreator();

    void CreateExtrapolatedMap();
    void SmoothMap(TClonesArray*);

    void SetOutParamFileName(TString name) {
        fParamFileName = name;
    }

    void SetDebug(Bool_t flag) {
        fDebug = flag;
    }

    void SetNodeNumber(Int_t nNodes) {
        dimGrid = nNodes;
    }

    // Extrapolation new ranges ...

    void SetNewRanges(Double_t min, Double_t max) {
        fConditions->min = min;
        fConditions->max = max;
    }

    void SetExtrapolationAxis(TString axis) {
        fConditions->direction = axis;
    }

    // Main method to be called ...
    void Extrapolation();

private:
    TString nameOutput;

    Bool_t fDebug;

    Int_t dimGrid; // dimGrid x dimGrid corresponds to number of nodes to build a grid along X(Y)-axis 
    Int_t dimNhist; // dimNhist corresponds to number of profiles drawn

    Double_t factor; // Transition from T to kG

    Double_t WinXmin;
    Double_t WinXmax;
    Double_t WinYmin;
    Double_t WinYmax;
    Double_t WinZmin;
    Double_t WinZmax;
    Double_t winXstep;
    Double_t winYstep;
    Double_t winZstep;

    TGraph2D*** constant;
    TGraph2D*** mean;
    TGraph2D*** sigma;

    BmnFieldMap* fMap;
    extrapolationConditions* fConditions;
    fitParMonitor* fMonitor;

    TString fParamFileName;

    void DrawProfileExtrapZ(Int_t, TGraph***, TF1***, TString, TString, Double_t, Double_t, Double_t, Double_t);
    void DrawProfileExtrapX(Int_t, TGraph***, TF1***, TString, TString, Double_t, Double_t, Double_t, Double_t);
    Double_t FieldExtrapolate(Double_t, Double_t, Double_t, TGraph2D*, TGraph2D*, TGraph2D*);
    void Print(BmnFieldMap*);

    void DoExtrapolationAlongZ(Int_t, Int_t);
    void DoExtrapolationAlongX(Int_t, Int_t);

    vector <Double_t> checkedFieldValue(Double_t bx, Double_t by, Double_t bz) {
        // To be not more than 5kG at scale = 1
        const Double_t min = 0.001;
        const Double_t max = 7.;

        vector <Double_t> out;

        out.push_back((TMath::Abs(bx) > min && TMath::Abs(bx) < max) ? bx : 0.);
        out.push_back((TMath::Abs(by) > min && TMath::Abs(by) < max) ? by : 0.);
        out.push_back((TMath::Abs(bz) > min && TMath::Abs(bz) < max) ? bz : 0.);

        return out;
    }

    ClassDef(BmnSP41FieldMapCreator, 1)
};

#endif
