#include "BmnSP41FieldMapCreator.h"

BmnSP41FieldMapCreator::~BmnSP41FieldMapCreator() {
    for (Int_t iPart = 0; iPart < 2; iPart++) {
        for (Int_t iComponent = 0; iComponent < 3; iComponent++) {
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


    if (fMap)
        delete fMap;

    if (fConditions)
        delete fConditions;

    if (fMonitor)
        delete fMonitor;
}

BmnSP41FieldMapCreator::BmnSP41FieldMapCreator(TString in, TString out) :
fDebug(kFALSE),
fMap(nullptr),
fConditions(nullptr),
fMonitor(nullptr),
nameOutput(out),
dimGrid(200),
dimNhist(6), factor(10.) {
    // Read initial field map 
    fMap = new BmnNewFieldMap(in);
    fMap->SetScale(1.);
    fMap->Init();

    // Extrapolation conditions ...
    fConditions = new extrapolationConditions();

    // Fit par monitor ...
    fMonitor = new fitParMonitor(out);

    WinXmin = fMap->GetXmin();
    WinXmax = fMap->GetXmax();

    WinYmin = fMap->GetYmin();
    WinYmax = fMap->GetYmax();

    WinZmin = fMap->GetZmin();
    WinZmax = fMap->GetZmax();

    winXstep = (WinXmax - WinXmin) / (dimNhist - 1);
    winYstep = (WinYmax - WinYmin) / (dimNhist - 1);
    winZstep = (WinZmax - WinZmin) / (dimNhist - 1);

    const Int_t nComponents = 3;
    const Int_t nParts = 2;

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
}

void BmnSP41FieldMapCreator::Extrapolation() {
    TString axis = fConditions->direction;

    // Setting predefined tail ranges (left and right) ...
    // They are considered as subjects to change if necessary ...
    if (axis.Contains("X")) {
        fConditions->tailRangeLeft = make_pair(-100., -80.);
        fConditions->tailRangeRight = make_pair(80., 100.);
    } else if (axis.Contains("Z")) {
        fConditions->tailRangeLeft = make_pair(-28., 0.);
        fConditions->tailRangeRight = make_pair(340., 360.);
    }

    const Int_t nParts = 2;
    const Int_t nComponents = 3;

    if (axis.Contains("X")) {
        for (Int_t iPart = 0; iPart < nParts; iPart++)
            for (Int_t iComp = 0; iComp < nComponents; iComp++)
                DoExtrapolationAlongX(iPart, iComp);
    } else if (axis.Contains("Z")) {
        for (Int_t iPart = 0; iPart < nParts; iPart++)
            for (Int_t iComp = 0; iComp < nComponents; iComp++)
                DoExtrapolationAlongZ(iPart, iComp);
    } else {
        cout << "Not supported direction!!! Exiting ..." << endl;
        throw;
    }

    CreateExtrapolatedMap();
}

void BmnSP41FieldMapCreator::DoExtrapolationAlongX(Int_t iPart, Int_t iComp) {
    TGraph*** bField = new TGraph**[dimGrid];
    TF1*** fit_func = new TF1**[dimNhist];
    TString comp = (iComp == 0) ? "x" : (iComp == 1) ? "y" : (iComp == 2) ? "z" : "";

    Double_t Ymax = fMap->GetYmax();
    Double_t Ymin = fMap->GetYmin();

    Double_t Zmax = fMap->GetZmax();
    Double_t Zmin = fMap->GetZmin();

    // Define number of slices on Y(Z)-dimension of the grid used
    Int_t nSlicesY = dimGrid - 1;
    Int_t nSlicesZ = dimGrid - 1;

    // Define step along X(Y, Z)-axises of the used grid
    Double_t dz = (Zmax - Zmin) / nSlicesZ;
    Double_t dy = (Ymax - Ymin) / nSlicesY;

    Int_t counter = 0;

    for (Int_t i = 0; i < dimGrid; i++) {
        if (fDebug)
            if (i % 10 == 0)
                cout << "Processing node " << i << endl;
        bField[i] = new TGraph*[dimGrid];
        Double_t ySlice = Ymin + i * dy;

        for (Int_t j = 0; j < dimGrid; j++) {
            Float_t zSlice = Zmin + j * dz;
            bField[i][j] = new TGraph();

            for (Int_t k = 0; k < fMap->GetNx(); k++) {
                Double_t x = fMap->GetXmin() + k * fMap->GetXstep();
                bField[i][j]->SetPoint(k, x, (comp == "x") ? fMap->GetBx(x, ySlice, zSlice) :
                        (comp == "y") ? fMap->GetBy(x, ySlice, zSlice) :
                        (comp == "z") ? fMap->GetBz(x, ySlice, zSlice) : 0.0);
            }

            vector <Double_t> fit_par;

            TF1* f = nullptr;
            if (iPart == 0) {
                f = new TF1("f", "gaus", fConditions->tailRangeLeft.first, fConditions->tailRangeLeft.second);
                f->SetParameter(2, 50.);
                fit_par = bField[i][j]->Fit(f, "SQ", "", f->GetXmin(), f->GetXmax())->Parameters();
            } else {
                f = new TF1("f", "gaus", fConditions->tailRangeRight.first, fConditions->tailRangeRight.second);
                f->SetParameter(2, 100.);
                fit_par = bField[i][j]->Fit(f, "SQ", "", f->GetXmin(), f->GetXmax())->Parameters();
            }

            fMonitor->C[iPart][iComp]->Fill(fit_par[0]);
            fMonitor->M[iPart][iComp]->Fill(fit_par[1]);
            fMonitor->S[iPart][iComp]->Fill(fit_par[2]);

            constant[iPart][iComp]->SetPoint(counter, ySlice, zSlice, fit_par[0]);
            mean[iPart][iComp]->SetPoint(counter, ySlice, zSlice, fit_par[1]);
            sigma[iPart][iComp]->SetPoint(counter, ySlice, zSlice, fit_par[2]);

            if (f)
                delete f;

            counter++;
        }
    }

    for (Int_t i = 0; i < dimNhist; i++) {
        fit_func[i] = new TF1*[dimNhist];
        Double_t y = WinYmin + i * winYstep;

        for (Int_t j = 0; j < dimNhist; j++) {
            Double_t z = WinZmin + j * winZstep;
            fit_func[i][j] = new TF1("func", "[0]*exp(-0.5*((x-[1])/[2])**2)",
                    (iPart == 1) ? fConditions->tailRangeRight.second : fConditions->min, (iPart == 1) ? fConditions->max : fConditions->tailRangeLeft.first);
            fit_func[i][j]->SetParameter(0, constant[iPart][iComp]->Interpolate(y, z));
            fit_func[i][j]->SetParameter(1, mean[iPart][iComp]->Interpolate(y, z));
            fit_func[i][j]->SetParameter(2, sigma[iPart][iComp]->Interpolate(y, z));
        }
    }

    DrawProfileExtrapX(dimNhist, bField, fit_func, "B" + comp + ".png", comp, Ymin, Zmin, dy, dz);

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

void BmnSP41FieldMapCreator::DoExtrapolationAlongZ(Int_t iPart, Int_t iComp) {
    TGraph*** bField = new TGraph**[dimGrid];
    TF1*** fit_func = new TF1**[dimNhist];
    TString comp = (iComp == 0) ? "x" : (iComp == 1) ? "y" : (iComp == 2) ? "z" : "";

    Double_t Xmax = fMap->GetXmax();
    Double_t Xmin = fMap->GetXmin();

    Double_t Ymax = fMap->GetYmax();
    Double_t Ymin = fMap->GetYmin();

    // Define number of slices on X(Y)-dimension of the grid used
    Int_t nSlicesX = dimGrid - 1;
    Int_t nSlicesY = dimGrid - 1;

    // Define step along X(Y, Z)-axises of the used grid
    Double_t dx = (Xmax - Xmin) / nSlicesX;
    Double_t dy = (Ymax - Ymin) / nSlicesY;

    Int_t counter = 0;

    for (Int_t i = 0; i < dimGrid; i++) {
        if (fDebug)
            if (i % 10 == 0)
                cout << "Processing node " << i << endl;
        bField[i] = new TGraph*[dimGrid];
        Double_t xSlice = Xmin + i * dx;

        for (Int_t j = 0; j < dimGrid; j++) {
            Float_t ySlice = Ymin + j * dy;
            bField[i][j] = new TGraph();

            for (Int_t k = 0; k < fMap->GetNz(); k++) {
                Double_t z = fMap->GetZmin() + k * fMap->GetZstep();
                bField[i][j]->SetPoint(k, z, (comp == "x") ? fMap->GetBx(xSlice, ySlice, z) :
                        (comp == "y") ? fMap->GetBy(xSlice, ySlice, z) :
                        (comp == "z") ? fMap->GetBz(xSlice, ySlice, z) : 0.0);
            }

            vector <Double_t> fit_par;

            TF1* f = nullptr;
            if (iPart == 0) {
                f = new TF1("f", "gaus", fConditions->tailRangeLeft.first, fConditions->tailRangeLeft.second);
                f->SetParameter(2, 100.);
                fit_par = bField[i][j]->Fit(f, "SQ", "G", f->GetXmin(), f->GetXmax())->Parameters();
            } else {
                f = new TF1("f", "gaus", fConditions->tailRangeRight.first, fConditions->tailRangeRight.second);
                f->SetParameter(2, 120.);
                fit_par = bField[i][j]->Fit(f, "SQ", "G", f->GetXmin(), f->GetXmax())->Parameters();
            }

            fMonitor->C[iPart][iComp]->Fill(fit_par[0]);
            fMonitor->M[iPart][iComp]->Fill(fit_par[1]);
            fMonitor->S[iPart][iComp]->Fill(fit_par[2]);

            constant[iPart][iComp]->SetPoint(counter, xSlice, ySlice, fit_par[0]);
            mean[iPart][iComp]->SetPoint(counter, xSlice, ySlice, fit_par[1]);
            sigma[iPart][iComp]->SetPoint(counter, xSlice, ySlice, fit_par[2]);

            counter++;

            if (f)
                delete f;
        }
    }

    for (Int_t i = 0; i < dimNhist; i++) {
        fit_func[i] = new TF1*[dimNhist];
        Double_t x = WinXmin + i * winXstep;

        for (Int_t j = 0; j < dimNhist; j++) {
            Double_t y = WinYmin + j * winYstep;
            fit_func[i][j] = new TF1("func", "[0]*exp(-0.5*((x-[1])/[2])**2)", (iPart == 1) ? fConditions->tailRangeRight.second : fConditions->min,
                    (iPart == 1) ? fConditions->max : fConditions->tailRangeLeft.first);
            fit_func[i][j]->SetParameter(0, constant[iPart][iComp]->Interpolate(x, y));
            fit_func[i][j]->SetParameter(1, mean[iPart][iComp]->Interpolate(x, y));
            fit_func[i][j]->SetParameter(2, sigma[iPart][iComp]->Interpolate(x, y));
        }
    }

    DrawProfileExtrapZ(dimNhist, bField, fit_func, "B" + comp + ".pdf", comp, Xmin, Ymin, dx, dy);

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

void BmnSP41FieldMapCreator::DrawProfileExtrapZ(Int_t dim, TGraph*** gr, TF1*** f, TString name, TString comp, Double_t Xmin, Double_t Ymin, Double_t dx, Double_t dy) {
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
            c->cd(counter);
            gr[shiftI][shiftJ]->Draw();
            gr[shiftI][shiftJ]->GetXaxis()->SetLimits(fConditions->min, fConditions->max);
            gr[shiftI][shiftJ]->SetTitle("B" + comp + TString(Form(", x = %G [cm]", X)) + TString(Form(",  y = %G [cm]", Y)));
            gr[shiftI][shiftJ]->GetXaxis()->SetTitleSize(0.1);
            gr[shiftI][shiftJ]->GetXaxis()->SetLabelSize(0.08);
            gr[shiftI][shiftJ]->GetYaxis()->SetTitleSize(0.1);
            gr[shiftI][shiftJ]->GetYaxis()->SetLabelSize(0.1);
            gr[shiftI][shiftJ]->GetXaxis()->SetTitle("Z, cm");
            gr[shiftI][shiftJ]->GetYaxis()->SetTitle("B" + comp + ", kG");
            counter++;
            f[i][j]->Draw("same");
        }
    }
    c->SaveAs(name);
    delete c;
}

void BmnSP41FieldMapCreator::DrawProfileExtrapX(Int_t dim, TGraph*** gr, TF1*** f, TString name, TString comp, Double_t Ymin, Double_t Zmin, Double_t dy, Double_t dz) {
    gStyle->SetTitleFontSize(0.1);
    TCanvas* c = new TCanvas("1", "1", 4000, 4000);
    c->Divide(dim, dim);
    Int_t counter = 1;
    for (Int_t i = 0; i < dim; i++) {
        Double_t Y = WinYmin + i * winYstep;
        Double_t sliceY = (Y - Ymin) / dy;
        Int_t shiftI = Int_t(sliceY);
        for (Int_t j = 0; j < dim; j++) {
            Double_t Z = WinZmin + j * winZstep;
            Double_t sliceZ = (Z - Zmin) / dz;
            Int_t shiftJ = Int_t(sliceZ);
            c->cd(counter);
            gr[shiftI][shiftJ]->Draw();
            gr[shiftI][shiftJ]->GetXaxis()->SetLimits(fConditions->min, fConditions->max);
            gr[shiftI][shiftJ]->SetLineWidth(2);
            gr[shiftI][shiftJ]->GetXaxis()->SetNdivisions(5);
            gr[shiftI][shiftJ]->SetTitle("B" + comp + TString(Form(", y = %G [cm]", Y)) + TString(Form(",  z = %G [cm]", Z)));
            gr[shiftI][shiftJ]->GetXaxis()->SetTitleSize(0.1);
            gr[shiftI][shiftJ]->GetXaxis()->SetLabelSize(0.08);
            gr[shiftI][shiftJ]->GetYaxis()->SetTitleSize(0.1);
            gr[shiftI][shiftJ]->GetYaxis()->SetLabelSize(0.1);
            gr[shiftI][shiftJ]->GetXaxis()->SetTitle("X, cm");
            gr[shiftI][shiftJ]->GetYaxis()->SetTitle("B" + comp + ", kG");
            counter++;
            f[i][j]->Draw("same");
            f[i][j]->SetLineWidth(2);
        }
    }
    c->SaveAs(name);
    delete c;
}

void BmnSP41FieldMapCreator::CreateExtrapolatedMap() {

    Double_t Xmin = fMap->GetXmin();
    Double_t Ymin = fMap->GetYmin();
    Double_t Zmin = fMap->GetZmin();

    Double_t Xmax = fMap->GetXmax();
    Double_t Ymax = fMap->GetYmax();
    Double_t Zmax = fMap->GetZmax();

    Double_t dx = fMap->GetXstep();
    Double_t dy = fMap->GetYstep();
    Double_t dz = fMap->GetZstep();

    Int_t Nx = fMap->GetNx();
    Int_t Ny = fMap->GetNy();
    Int_t Nz = fMap->GetNz();
    // Int_t Nz = (Zmax - Zmin) / dz + 1;

    // Print(fMap);

    Double_t x = -1., y = -1., z = -1.;

    Int_t index = 0;

    ofstream mapFile(nameOutput);

    mapFile.precision(6);
    mapFile << showpoint;
    mapFile << "nosym" << endl;

    TString axis = fConditions->direction;

    // Filling parameters of the extrapolated map ...
    if (axis.Contains("X")) {
        Xmin = fConditions->min;
        Xmax = fConditions->max;
        Nx = Int_t((Xmax - Xmin) / 2.) + 1;
        mapFile << Xmin << " " << Xmax << " " << Nx << endl;
    } else
        mapFile << Xmin << " " << Xmax << " " << Nx << endl;

    mapFile << Ymin << " " << Ymax << " " << Ny << endl;

    if (axis.Contains("Z")) {
        Zmin = fConditions->min;
        Zmax = fConditions->max;
        Nz = Int_t((Zmax - Zmin) / 2.) + 1;
        mapFile << Zmin << " " << Zmax << " " << Nz << endl;
    } else
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

                Double_t bx = -1., by = -1., bz = -1.;

                Bool_t isInMap = kTRUE;

                if (axis.Contains("X")) {
                    if (x < fMap->GetXmin()) {
                        isInMap = kFALSE;
                        bx = FieldExtrapolate(x, y, z, constant[0][0], mean[0][0], sigma[0][0]);
                        by = FieldExtrapolate(x, y, z, constant[0][1], mean[0][1], sigma[0][1]);
                        bz = FieldExtrapolate(x, y, z, constant[0][2], mean[0][2], sigma[0][2]);
                    } else if (x > fMap->GetXmax() - 1.) {
                        isInMap = kFALSE;
                        bx = FieldExtrapolate(x, y, z, constant[1][0], mean[1][0], sigma[1][0]);
                        by = FieldExtrapolate(x, y, z, constant[1][1], mean[1][1], sigma[1][1]);
                        bz = FieldExtrapolate(x, y, z, constant[1][2], mean[1][2], sigma[1][2]);
                    }
                } else if (axis.Contains("Z")) {
                    if (z < fMap->GetZmin()) {
                        isInMap = kFALSE;
                        bx = FieldExtrapolate(x, y, z, constant[0][0], mean[0][0], sigma[0][0]);
                        by = FieldExtrapolate(x, y, z, constant[0][1], mean[0][1], sigma[0][1]);
                        bz = FieldExtrapolate(x, y, z, constant[0][2], mean[0][2], sigma[0][2]);
                    } else if (z > fMap->GetZmax() - 1.) {
                        isInMap = kFALSE;
                        bx = FieldExtrapolate(x, y, z, constant[1][0], mean[1][0], sigma[1][0]);
                        by = FieldExtrapolate(x, y, z, constant[1][1], mean[1][1], sigma[1][1]);
                        bz = FieldExtrapolate(x, y, z, constant[1][2], mean[1][2], sigma[1][2]);
                    }
                }

                if (isInMap) {
                    bx = fMap->GetBx(x, y, z);
                    by = fMap->GetBy(x, y, z);
                    bz = fMap->GetBz(x, y, z);
                }

                // Checking obtained extrapolated values ...
                mapFile << checkedFieldValue(bx, by, bz).at(0) / factor << " " << checkedFieldValue(bx, by, bz).at(1) / factor << " " <<
                        checkedFieldValue(bx, by, bz).at(2) / factor << endl;
            } // z-Loop
        } // y-Loop
    } // x-Loop   
}

void BmnSP41FieldMapCreator::SmoothMap(TClonesArray* sValues) {

    vector <TLorentzVector> vSmoo;

    for (Int_t iSmooth = 0; iSmooth < sValues->GetEntriesFast(); iSmooth++) {
        smoothedValues* sValue = (smoothedValues*) sValues->UncheckedAt(iSmooth);

        TLorentzVector tmp(sValue->X, sValue->Y, sValue->Z, sValue->fieldValue);
        vSmoo.push_back(tmp);
    }

    Double_t Xmin = fMap->GetXmin();
    Double_t Ymin = fMap->GetYmin();
    Double_t Zmin = fMap->GetZmin();
    Double_t Zmax = fMap->GetZmax();

    Double_t dx = fMap->GetXstep();
    Double_t dy = fMap->GetYstep();
    Double_t dz = fMap->GetZstep();

    Int_t Nx = fMap->GetNx();
    Int_t Ny = fMap->GetNy();
    Int_t Nz = fMap->GetNz();

    Print(fMap);

    Double_t x, y, z, bx, by, bz;

    UInt_t index = 0;
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

            for (Int_t iz = 0; iz < Nz; iz++) {
                index = ix * Ny * Nz + iy * Nz + iz;
                z = Zmin + dz * iz;

                by = (fMap->GetBy(x, y, z) / factor);
                for (auto it = vSmoo.begin(); it != vSmoo.end(); it++) {

                    TLorentzVector v = *it;

                    if (v.X() != x || v.Y() != y || v.Z() != z)
                        continue;

                    by = v.T() / factor;

                    vSmoo.erase(it--);
                    break;
                }

                bx = (Abs(fMap->GetBx(x, y, z)) / factor > 10e-8) ? (fMap->GetBx(x, y, z) / factor) : 0.0;
                bz = (Abs(fMap->GetBz(x, y, z)) / factor > 10e-8) ? (fMap->GetBz(x, y, z) / factor) : 0.0;
                // mapFile << x << " " << y << " " << z << " " << bx << " " << by << " " << bz << endl;
                mapFile << bx << " " << by << " " << bz << endl;

                if (index % 1000 == 0)
                    cout << "nNodes processed# " << index << endl;

            } // z-Loop
        } // y-Loop
    } // x-Loop
}

Double_t BmnSP41FieldMapCreator::FieldExtrapolate(Double_t x, Double_t y, Double_t z, TGraph2D* c, TGraph2D* m, TGraph2D* s) {
    vector <Double_t> varToBePassed;

    if (fConditions->direction.Contains("X")) {
        varToBePassed.push_back(y);
        varToBePassed.push_back(z);
        varToBePassed.push_back(x);
    } else if (fConditions->direction.Contains("Z")) {
        varToBePassed.push_back(x);
        varToBePassed.push_back(y);
        varToBePassed.push_back(z);
    }

    Double_t Mean = m->Interpolate(varToBePassed[0], varToBePassed[1]);
    Double_t Sigma = s->Interpolate(varToBePassed[0], varToBePassed[1]);

    return c->Interpolate(varToBePassed[0], varToBePassed[1]) * TMath::Exp(-0.5 * (varToBePassed[2] - Mean) * (varToBePassed[2] - Mean) / Sigma / Sigma);
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