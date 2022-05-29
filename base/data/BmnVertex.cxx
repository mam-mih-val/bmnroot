#include "BmnVertex.h"

#include <iomanip>
#include <iostream>
#include <sstream>

#include "FairLogger.h"

// -----   Default constructor   -------------------------------------------
BmnVertex::BmnVertex()
    : fX(-1000.),
      fY(-1000.),
      fZ(-1000.),
      fChi2(-1.),
      fNDF(-1),
      fNTracks(-1),
      fCovMatrix(),
      fFlag(-1),
      fTrackIdxs() {
    for (Int_t i = 0; i < 6; i++) fCovMatrix[i] = 0;
}
// -------------------------------------------------------------------------

// -----   Constructor with all parameters   -------------------------------
BmnVertex::BmnVertex(Double_t x, Double_t y, Double_t z, Double_t chi2,
                     Int_t ndf, Int_t nTracks, const TMatrixFSym& covMat,
                     Int_t flag, vector<Int_t> idxs)
    : fX(x),
      fY(y),
      fZ(z),
      fChi2(chi2),
      fNDF(ndf),
      fNTracks(nTracks),
      fCovMatrix(),
      fFlag(flag),
      fTrackIdxs(idxs) {
    if ((covMat.GetNrows() != 3) && (covMat.GetNcols() != 3)) {
        LOG(error) << "Wrong dimension of passed covariance matrix. Clear the covariance matrix.";
        for (Int_t i = 0; i < 6; i++) fCovMatrix[i] = 0;
    } else {
        Int_t index = 0;
        for (Int_t i = 0; i < 3; i++) {
            for (Int_t j = i; j < 3; j++) fCovMatrix[index++] = covMat[i][j];
        }
    }
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
BmnVertex::~BmnVertex() {}
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void BmnVertex::Print() {
    Double_t chi2ndf;
    if (fNDF)
        chi2ndf = fChi2 / Double_t(fNDF);
    else
        chi2ndf = 0.;
    cout << "Vertex coord. (" << fX << "," << fY << "," << fZ << ") cm, "
         << "chi2/ndf = " << chi2ndf << ", " << fNTracks
         << " tracks used, Vertex type: " << fFlag << endl;
}
// -------------------------------------------------------------------------

// -----   Accessor to covariance matrix    --------------------------------
void BmnVertex::CovMatrix(TMatrixFSym& covMat) const {
    Int_t index = 0;
    for (int i = 0; i < 3; i++) {
        for (int j = i; j < 3; j++) {
            covMat[i][j] = fCovMatrix[index];
            covMat[j][i] = fCovMatrix[index];
            index++;
        }
    }
}
// -------------------------------------------------------------------------

// -----   Accessor to covariance matrix elements   ------------------------
Double_t BmnVertex::GetCovariance(Int_t i, Int_t j) const {
    TMatrixFSym* mat = new TMatrixFSym(3);
    CovMatrix(*mat);
    Double_t element = (*mat)[i][j];
    delete mat;
    return element;
}
// -------------------------------------------------------------------------

// -----   Public method SetVertex   ---------------------------------------
void BmnVertex::SetVertex(Double_t x, Double_t y, Double_t z, Double_t chi2,
                          Int_t ndf, Int_t nTracks, const TMatrixFSym& covMat, Int_t flag, vector<Int_t> idxs) {
    fX = x;
    fY = y;
    fZ = z;
    fChi2 = chi2;
    fNDF = ndf;
    fNTracks = nTracks;
    fFlag = flag;
    fTrackIdxs = idxs;
    if ((covMat.GetNrows() != 3) && (covMat.GetNcols() != 3)) {
        LOG(error) << "Wrong dimension of passed covariance matrix. Clear the covariance matrix.";
        for (Int_t i = 0; i < 6; i++) fCovMatrix[i] = 0;
    } else {
        Int_t index = 0;
        for (Int_t i = 0; i < 3; i++) {
            for (Int_t j = i; j < 3; j++) fCovMatrix[index++] = covMat[i][j];
        }
    }
}
// -------------------------------------------------------------------------

// -----   Public method Reset   -------------------------------------------
void BmnVertex::Reset() {
    fX = fY = fZ = fChi2 = 0.;
    fNDF = fNTracks = 0;
    fFlag = -1;
    fTrackIdxs.clear();
    for (Int_t i = 0; i < 6; i++) fCovMatrix[i] = 0;
}
// -------------------------------------------------------------------------

// --- String output  ------------------------------------------------------
string BmnVertex::ToString() const {
    Double_t chi2ndf = (fNDF ? fChi2 / Double_t(fNDF) : 0.);
    stringstream ss;
    ss << "Vertex: position (" << fixed << setprecision(4) << fX << ", " << fY << ", " << fZ
       << ") cm, chi2/ndf = " << chi2ndf << ", tracks used: " << fNTracks << ", Vertex type: " << fFlag;
    return ss.str();
}
// -------------------------------------------------------------------------

ClassImp(BmnVertex)
