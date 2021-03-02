#include "TArrayF.h"
#include "TFile.h"
#include "TTree.h"
#include "TMath.h"
#include <iomanip>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include <Rtypes.h>

using namespace std;

R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"

typedef struct {
    Int_t N;
    Float_t min;
    Float_t max;
    Float_t step;
} Coordinate_info_t;

vector <Double_t> Field_remaking(Double_t* B);

void createRootMagField(TString in = "", TString out = "") {
    bmnloadlibs(); // load BmnRoot libraries

    BmnFieldMap* magField = new BmnNewFieldMap(in.Data());
    magField->SetScale(1.);

    magField->Init();

    Coordinate_info_t X;
    Coordinate_info_t Y;
    Coordinate_info_t Z;

    TString field_type;

    Double_t B[3];
    vector <Double_t> Field;

    // ofstream myout("ASCII_field_data_test.dat");

    TFile *f = new TFile(out.Data(), "RECREATE");
    TTree *tree = new TTree("Main_info", "data from ASCII");
    TTree *t = new TTree("Field_map", "data from ASCII");

    tree->Branch("Field_type", &field_type);
    tree->Branch("Main_info_X", &X, "N/I:min/F:max:step");
    tree->Branch("Main_info_Y", &Y, "N/I:min/F:max:step");
    tree->Branch("Main_info_Z", &Z, "N/I:min/F:max:step");
    t->Branch("field_map", &Field);

    TString dir = getenv("VMCWORKDIR");
    ifstream mapFile(TString(dir + "/input/" + in).Data());
    mapFile >> field_type;

    X.N = magField->GetNx();
    X.min = magField->GetXmin();
    X.max = magField->GetXmax();
    X.step = (X.max - X.min) / Float_t(X.N - 1);

    Y.N = magField->GetNy();
    Y.min = magField->GetYmin();
    Y.max = magField->GetYmax();
    Y.step = (Y.max - Y.min) / Float_t(Y.N - 1);

    Z.N = magField->GetNz();
    Z.min = magField->GetZmin();
    Z.max = magField->GetZmax();
    Z.step = (Z.max - Z.min) / Float_t(Z.N - 1);

    for (Int_t i = X.min; i <= X.max; i += Int_t(Z.step))
        for (Int_t j = Y.min; j <= Y.max; j += Int_t(Y.step))
            for (Int_t k = Z.min; k <= Z.max; k += Int_t(Z.step)) {
                B[0] = magField->GetBx(i, j, k);
                B[1] = magField->GetBy(i, j, k);
                B[2] = magField->GetBz(i, j, k);
                Field.clear();
                Field = Field_remaking(B);
                t->Fill();
            }

    tree->Fill();

    delete magField;

    f->Write();
}

vector<Double_t> Field_remaking(Double_t* B) {
    vector<Double_t> field_parameters;
    for (int i = 0; i < 3; ++i) {
        field_parameters.push_back(B[i]);
    }
    return field_parameters;
}
