#include "BmnSP41FieldMapCreator.h"

BmnSP41FieldMapCreator::~BmnSP41FieldMapCreator() {
    for (Int_t iPart = 0; iPart < nParts; iPart++) {
        for (Int_t iComponent = 0; iComponent < nParts; iComponent++) {
            delete constant[iPart][iComponent];
            delete mean[iPart][iComponent];
            delete sigma[iPart][iComponent];
        }
        delete [] constant[iPart];
        delete [] mean[iPart];
        delete [] sigma[iPart];
    }
    delete [] constant;
    delete [] mean;
    delete [] sigma;

    delete fMap;
}

BmnSP41FieldMapCreator::BmnSP41FieldMapCreator(TString fieldMapName) :
isDrawProfile(kTRUE),
fDebug(kFALSE),
fMap(NULL),
nameOutput("test.dat") {
    Zmax = 700.;
    Zmin = -200.0;

    Zmin_RIGHT = 340.;
    Zmax_RIGHT = 360.;

    Zmin_LEFT = -28.;
    Zmax_LEFT = 0.;

    Npoints = 900;
    dimGrid = 0;
    dimNhist = 5;

    const_restrict = 10000.;
    factor = 10.;
    WinXmin = -112.;
    WinXmax = -WinXmin;
    WinYmin = -44.;
    WinYmax = -WinYmin;
    winXstep = (WinXmax - WinXmin) / (dimNhist - 1);
    winYstep = (WinYmax - WinYmin) / (dimNhist - 1);

    nComponents = 3;
    nParts = 2;

    constant = new TGraph2D**[nParts];
    mean = new TGraph2D**[nParts];
    sigma = new TGraph2D**[nParts];
    for (Int_t iPart = 0; iPart < nParts; iPart++) {
        constant[iPart] = new TGraph2D*[nComponents];
        mean[iPart] = new TGraph2D*[nComponents];
        sigma[iPart] = new TGraph2D*[nComponents];
        for (Int_t iComponent = 0; iComponent < nComponents; iComponent++) {
            constant[iPart][iComponent] = new TGraph2D();
            mean[iPart][iComponent] = new TGraph2D();
            sigma[iPart][iComponent] = new TGraph2D();
        }
    }

    // Read initial field map 
    fMap = new BmnNewFieldMap(fieldMapName);
    fMap->SetScale(1.);
    fMap->Init();
}

void BmnSP41FieldMapCreator::AnalyzeFieldProfiles(Int_t iPart, Int_t iComp) {
    if (dimGrid == 0) {
        cout << "!!! Dim. of grid should not be equal to 0 !!!" << endl;
        throw;
    }
    TGraph*** bField = new TGraph**[dimGrid];
    TF1*** fit_func = new TF1**[dimNhist];
    TString comp = (iComp == 0) ? "x" : (iComp == 1) ? "y" : (iComp == 2) ? "z" : "";

    Double_t Xmax = fMap->GetXmax() - 1.;
    Double_t Xmin = -Xmax;
    Double_t Ymax = fMap->GetYmax() - 1.;
    Double_t Ymin = -Ymax;

    // Define number of slices on X(Y)-dimension of the grid used
    Int_t nSlicesX = dimGrid - 1;
    Int_t nSlicesY = dimGrid - 1;

    // Define step along X(Y, Z)-axises of the used grid
    Double_t dx = (Xmax - Xmin) / nSlicesX;
    Double_t dy = (Ymax - Ymin) / nSlicesY;
    Double_t dz = (Zmax - Zmin) / Npoints;

    Int_t counter = 0;
    const Int_t shift = 0;

    TVector3 params(0., 0., 0.); // Vector to store fit parameters

    for (Int_t i = 0; i < dimGrid; i++) {
        if (fDebug)
            if (i % 10 == 0)
                cout << "Processing node " << i << endl;
        bField[i] = new TGraph*[dimGrid];
        Double_t xSlice = Xmin + (i + shift) * dx;

        for (Int_t j = 0; j < dimGrid; j++) {
            Float_t ySlice = Ymin + (j + shift) * dy;
            bField[i][j] = new TGraph();

            for (Int_t k = 0; k < Npoints; k++) {
                Double_t z = Zmin + k * dz;
                bField[i][j]->SetPoint(k, z, (comp == "x") ? fMap->GetBx(xSlice, ySlice, z) :
                        (comp == "y") ? fMap->GetBy(xSlice, ySlice, z) :
                        (comp == "z") ? fMap->GetBz(xSlice, ySlice, z) : 0.0);
            }

            vector <Double_t> fit_par;

            if (iPart == 0)
                fit_par = bField[i][j]->Fit("gaus", "SQww", "G", Zmin_LEFT, Zmax_LEFT)->Parameters();
            else
                fit_par = bField[i][j]->Fit("gaus", "SQww", "G", Zmin_RIGHT, Zmax_RIGHT)->Parameters();

            if (Abs(fit_par[0]) < const_restrict)
                params.SetXYZ(fit_par[0], fit_par[1], fit_par[2]);

            constant[iPart][iComp]->SetPoint(counter, xSlice, ySlice, params.X());
            mean[iPart][iComp]->SetPoint(counter, xSlice, ySlice, params.Y());
            sigma[iPart][iComp]->SetPoint(counter, xSlice, ySlice, params.Z());

            counter++;
        }
    }

    if (isDrawProfile) {
        for (Int_t i = 0; i < dimNhist; i++) {
            if (fDebug)
                cout << i << endl;
            fit_func[i] = new TF1*[dimNhist];
            Double_t x = WinXmin + i * winXstep;

            for (Int_t j = 0; j < dimNhist; j++) {
                Double_t y = WinYmin + j * winYstep;
                fit_func[i][j] = new TF1("func", "[0]*exp(-0.5*((x-[1])/[2])**2)", (iPart == 1) ? Zmax_RIGHT : Zmin, (iPart == 1) ? Zmax : Zmin_LEFT);
                fit_func[i][j]->SetParameter(0, constant[iPart][iComp]->Interpolate(x, y));
                fit_func[i][j]->SetParameter(1, mean[iPart][iComp]->Interpolate(x, y));
                fit_func[i][j]->SetParameter(2, sigma[iPart][iComp]->Interpolate(x, y));
            }
        }
        DrawProfileExtrap(dimNhist, bField, fit_func, "B" + comp + ".pdf", comp, Xmin, Ymin, dx, dy);
    }

    for (Int_t i = 0; i < dimNhist; i++) {
        for (Int_t j = 0; j < dimNhist; j++)
            delete fit_func[i][j];
        delete [] fit_func[i];
    }
    delete [] fit_func;

    for (Int_t i = 0; i < dimGrid; i++) {
        for (Int_t j = 0; j < dimGrid; j++)
            delete bField[i][j];
        delete [] bField[i];
    }
    delete [] bField;
}

void BmnSP41FieldMapCreator::DrawProfileExtrap(Int_t dim, TGraph*** gr, TF1*** f, TString name, TString comp, Double_t Xmin, Double_t Ymin, Double_t dx, Double_t dy) {
    TCanvas* c = new TCanvas("1", "1", 1200, 800);
    c->Divide(dim, dim);
    Int_t counter = 1;
    for (Int_t i = 0; i < dim; i++) {
        Double_t X = WinXmin + i * winXstep;
        Double_t sliceX = (X - Xmin) / dx;
        Int_t shiftI = Int_t(sliceX);
        for (Int_t j = 0; j < dim; j++) {
            Double_t Y = WinYmin + j * winYstep;
            Double_t sliceY = (Y - Ymin) / dy;
            Int_t shiftJ = Int_t(sliceY);
            c->GetPad(counter)->SetGridx();
            c->GetPad(counter)->SetGridy();
            c->cd(counter);
            gr[shiftI][shiftJ]->Draw();
            gr[shiftI][shiftJ]->SetTitle("B" + comp + TString(Form(", x = %G [cm]", X)) + TString(Form(",  y = %G [cm]", Y)));
            gr[shiftI][shiftJ]->GetXaxis()->SetTitleSize(0.05);
            gr[shiftI][shiftJ]->GetXaxis()->SetLabelSize(0.07);
            gr[shiftI][shiftJ]->GetYaxis()->SetTitleSize(0.05);
            gr[shiftI][shiftJ]->GetYaxis()->SetLabelSize(0.07);
            gr[shiftI][shiftJ]->GetXaxis()->SetTitle("Z, cm");
            gr[shiftI][shiftJ]->GetYaxis()->SetTitle("B" + comp + ", kG");
            counter++;
            f[i][j]->Draw("same");
        }
    }
    c->SaveAs(name);
    delete c;
}

void BmnSP41FieldMapCreator::CreateNewMap(TString fieldMapName) {
    Double_t shift = 1e-6;

    Double_t Xmin = fMap->GetXmin(); // - shift + 1.;
    Double_t Ymin = fMap->GetYmin(); //- shift + 1.;

    Double_t dx = fMap->GetXstep();
    Double_t dy = fMap->GetYstep();
    Double_t dz = fMap->GetZstep();

    Int_t Nx = fMap->GetNx();
    Int_t Ny = fMap->GetNy();
    Int_t Nz = (Zmax - Zmin) / dz + 1;

    Print(fMap);

    Double_t x, y, z, bx, by, bz;

    Int_t index = 0;

    if (fieldMapName != "")
        nameOutput = fieldMapName;
    ofstream mapFile(nameOutput);

    mapFile.precision(6);
    mapFile << showpoint;
    mapFile << "nosym" << endl;
    mapFile << Xmin << " " << -Xmin << " " << Nx << endl;
    mapFile << Ymin << " " << -Ymin << " " << Ny << endl;
    mapFile << Zmin << " " << Zmax << " " << Nz << endl;

    for (Int_t ix = 0; ix < Nx; ix++) {
        x = Xmin + dx * ix;
        for (Int_t iy = 0; iy < Ny; iy++) {
            y = Ymin + dy * iy;
            if (fDebug)
                cout << x << " " << y << endl; 
            for (Int_t iz = 0; iz < Nz; iz++) {
                index = ix * Ny * Nz + iy * Nz + iz;
                z = Zmin + dz * iz;

                if (z < Zmin_LEFT) {
                    // cout << "X, Y, Z = " << x << " " << y << " " << z << endl;
                    // z += fieldZ;
                    bx = FieldExtrapolate(x, y, z, constant[0][0], mean[0][0], sigma[0][0]);
                    by = FieldExtrapolate(x, y, z, constant[0][1], mean[0][1], sigma[0][1]);
                    bz = FieldExtrapolate(x, y, z, constant[0][2], mean[0][2], sigma[0][2]);
                    // mapFile << x << " " << y << " " << z << " " << bx / factor << " " << by / factor << " " << bz / factor << endl;
                    mapFile << bx / factor << " " << by / factor << " " << bz / factor << endl;
                    //cout << bz / factor << endl;
                } else if (z > Zmax_RIGHT) {
                    // cout << "X, Y, Z = " << x << " " << y << " " << z << endl;
                    // z += fieldZ;
                    bx = FieldExtrapolate(x, y, z, constant[1][0], mean[1][0], sigma[1][0]);
                    by = FieldExtrapolate(x, y, z, constant[1][1], mean[1][1], sigma[1][1]);
                    bz = FieldExtrapolate(x, y, z, constant[1][2], mean[1][2], sigma[1][2]);
                   // mapFile << x << " " << y << " " << z << " " << bx / factor << " " << by / factor << " " << bz / factor << endl;
                    mapFile << bx / factor << " " << by / factor << " " << bz / factor << endl;
                    //cout << bz / factor << endl;

                } else {
                    bx = (Abs(fMap->GetBx(x, y, z)) / factor > 10e-8) ? (fMap->GetBx(x, y, z) / factor) : 0.0;
                    by = (Abs(fMap->GetBy(x, y, z)) / factor > 10e-8) ? (fMap->GetBy(x, y, z) / factor) : 0.0;
                    bz = (Abs(fMap->GetBz(x, y, z)) / factor > 10e-8) ? (fMap->GetBz(x, y, z) / factor) : 0.0;
                    // mapFile << x << " " << y << " " << z << " " << bx << " " << by << " " << bz << endl;
                    mapFile << bx << " " << by << " " << bz << endl;
                }
            } // z-Loop
        } // y-Loop
    } // x-Loop
}

Double_t BmnSP41FieldMapCreator::FieldExtrapolate(Double_t x, Double_t y, Double_t z, TGraph2D* c, TGraph2D* m, TGraph2D* s) {

    Double_t Mean = m->Interpolate(x, y);
    Double_t Sigma = s->Interpolate(x, y);

    return c->Interpolate(x, y) * TMath::Exp(-0.5 * (z - Mean) * (z - Mean) / Sigma / Sigma);
}

void BmnSP41FieldMapCreator::Print(BmnFieldMap* magField) {
    cout << "Nx = " << magField->GetNx();
    cout << "; Ny = " << magField->GetNy();
    cout << "; Nz = " << magField->GetNz();

    Double_t shift = 1e-6;

    cout << "; Xmin = " << magField->GetXmin() - shift;
    cout << "; dx = " << magField->GetXstep();
    cout << "; Ymin = " << magField->GetYmin() - shift;
    cout << "; dy = " << magField->GetYstep();
    cout << "; Zmin = " << magField->GetZmin() - shift;
    cout << "; dz = " << magField->GetZstep() << endl;
}