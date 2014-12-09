// -------------------------------------------------------------------------
// -----                      BmnFieldMap source file                  -----
// -----         Created 12/01/04  by M. Al/Turany (BmnField.cxx)      -----
// -----                Redesign 13/02/06  by V. Friese                -----
// -------------------------------------------------------------------------
// Includes from CBMROOT
#include "BmnFieldMap.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"

#include "BmnFieldMapCreator.h"
#include "BmnFieldMapData.h"
#include "BmnFieldPar.h"

// Includes from ROOT
#include "TArrayF.h"
#include "TFile.h"
#include "TMath.h"
#include "FairRunSim.h"

// Includes from C
#include <iomanip>
#include <iostream>
#include <fstream>
#include <vector>

//using std::cout;
//using std::cerr;
//using std::endl;
//using std::right;
//using std::flush;
//using std::setw;
//using std::showpoint;

// -------------   Default constructor  ----------------------------------

BmnFieldMap::BmnFieldMap()
: FairField(),
fFileName(""),
fScale(1.),
fPosX(0.),
fPosY(0.),
fPosZ(0.),
fXmin(0.),
fXmax(0.),
fXstep(0.),
fYmin(0.),
fYmax(0.),
fYstep(0.),
fZmin(0.),
fZmax(0.),
fZstep(0.),
fNx(0),
fNy(0),
fNz(0),
fBx(NULL),
fBy(NULL),
fBz(NULL) {
    // Initilization of arrays is to my knowledge not
    // possible in member initalization lists
    for (Int_t i = 0; i < 2; i++) {
        fHc[i] = 0;
        for (Int_t j = 0; j < 2; j++) {
            fHb[i][j] = 0;
            for (Int_t k = 0; k < 2; k++) {
                fHa[i][j][k] = 0;
            }
        }
    }
    // Assign values to data members of base classes
    // There is no appropriate constructor of the base
    // class.
    fName = "";
    fType = 1;
}
// ------------------------------------------------------------------------



// -------------   Standard constructor   ---------------------------------

BmnFieldMap::BmnFieldMap(const char* mapName)
: FairField(),
fFileName(""),
fScale(1.),
fPosX(0.),
fPosY(0.),
fPosZ(0.),
fXmin(0.),
fXmax(0.),
fXstep(0.),
fYmin(0.),
fYmax(0.),
fYstep(0.),
fZmin(0.),
fZmax(0.),
fZstep(0.),
fNx(0),
fNy(0),
fNz(0),
fBx(NULL),
fBy(NULL),
fBz(NULL),
fDebugInfo(kFALSE) {
    // Initilization of arrays is to my knowledge not
    // possible in member initalization lists
    for (Int_t i = 0; i < 2; i++) {
        fHc[i] = 0;
        for (Int_t j = 0; j < 2; j++) {
            fHb[i][j] = 0;
            for (Int_t k = 0; k < 2; k++) {
                fHa[i][j][k] = 0;
            }
        }
    }
    // Assign values to data members of base classes
    // There is no appropriate constructor of the base
    // class.
    fName = mapName;
    TString dir = getenv("VMCWORKDIR");
    fFileName = dir + "/input/" + mapName;

    fType = 1;
}
// ------------------------------------------------------------------------



// ------------   Constructor from BmnFieldPar   --------------------------

BmnFieldMap::BmnFieldMap(BmnFieldPar* fieldPar)
: FairField(),
fFileName(""),
fScale(1.),
fPosX(0.),
fPosY(0.),
fPosZ(0.),
fXmin(0.),
fXmax(0.),
fXstep(0.),
fYmin(0.),
fYmax(0.),
fYstep(0.),
fZmin(0.),
fZmax(0.),
fZstep(0.),
fNx(0),
fNy(0),
fNz(0),
fBx(NULL),
fBy(NULL),
fBz(NULL) {
    // Initilization of arrays is to my knowledge not
    // possible in member initalization lists
    for (Int_t i = 0; i < 2; i++) {
        fHc[i] = 0;
        for (Int_t j = 0; j < 2; j++) {
            fHb[i][j] = 0;
            for (Int_t k = 0; k < 2; k++) {
                fHa[i][j][k] = 0;
            }
        }
    }
    // Assign values to data members of base classes
    // There is no appropriate constructor of the base
    // class.
    fName = "";
    fType = 1;
    if (!fieldPar) {
        cerr << "-W- BmnFieldConst::BmnFieldMap: empty parameter container!"
                << endl;
    } else {
        fieldPar->MapName(fName);
        fPosX = fieldPar->GetPositionX();
        fPosY = fieldPar->GetPositionY();
        fPosZ = fieldPar->GetPositionZ();
        fScale = fieldPar->GetScale();
        TString dir = getenv("VMCWORKDIR");
        fFileName = dir + "/input/" + fName;
        fType = fieldPar->GetType();
    }
}
// ------------------------------------------------------------------------



// ------------  Constructor from BmnFieldMapCreator  ---------------------

BmnFieldMap::BmnFieldMap(BmnFieldMapCreator* creator)
: FairField(),
fFileName(""),
fScale(1.),
fPosX(0.),
fPosY(0.),
fPosZ(0.),
fXmin(0.),
fXmax(0.),
fXstep(0.),
fYmin(0.),
fYmax(0.),
fYstep(0.),
fZmin(0.),
fZmax(0.),
fZstep(0.),
fNx(0),
fNy(0),
fNz(0),
fBx(NULL),
fBy(NULL),
fBz(NULL) {
    // Initilization of arrays is to my knowledge not
    // possible in member initalization lists
    for (Int_t i = 0; i < 2; i++) {
        fHc[i] = 0;
        for (Int_t j = 0; j < 2; j++) {
            fHb[i][j] = 0;
            for (Int_t k = 0; k < 2; k++) {
                fHa[i][j][k] = 0;
            }
        }
    }
    // Assign values to data members of base classes
    // There is no appropriate constructor of the base
    // class.
    fName = "";
    fType = 1;
    if (!creator) {
        cerr << "-W- BmnFieldMap: no creator given!" << endl;
    } else {
        fType = 1;
        fName = creator->GetMapName();
        fXmin = creator->GetXmin();
        fXmax = creator->GetXmax();
        fYmin = creator->GetYmin();
        fYmax = creator->GetYmax();
        fZmin = creator->GetZmin();
        fZmax = creator->GetZmax();
        fNx = creator->GetNx();
        fNy = creator->GetNy();
        fNz = creator->GetNz();
        fXstep = (fXmax - fXmin) / Double_t(fNx - 1);
        fYstep = (fYmax - fYmin) / Double_t(fNy - 1);
        fZstep = (fZmax - fZmin) / Double_t(fNz - 1);
        fBx = creator->GetBx();
        fBy = creator->GetBy();
        fBz = creator->GetBz();
    }
}

// ------------------------------------------------------------------------



// ------------   Destructor   --------------------------------------------

BmnFieldMap::~BmnFieldMap() {
    if (fBx) delete fBx;
    if (fBy) delete fBy;
    if (fBz) delete fBz;
}
// ------------------------------------------------------------------------



// -----------   Intialisation   ------------------------------------------

void BmnFieldMap::Init() {
    if (fFileName.EndsWith(".root")) ReadRootFile(fFileName, fName);
    else if (fFileName.EndsWith(".dat")) ReadAsciiFile(fFileName);
    else {
        cerr << "-E- BmnFieldMap::Init: No proper file name defined! ("
                << fFileName << ")" << endl;
        Fatal("Init", "No proper file name");
    }
}
// ------------------------------------------------------------------------



// -----------   Get x component of the field   ---------------------------

Double_t BmnFieldMap::GetBx(Double_t x, Double_t y, Double_t z) {

    Int_t ix = 0;
    Int_t iy = 0;
    Int_t iz = 0;
    Double_t dx = 0.;
    Double_t dy = 0.;
    Double_t dz = 0.;

    if (IsInside(x, y, z, ix, iy, iz, dx, dy, dz)) {

        // Get Bx field values at grid cell corners
        fHa[0][0][0] = fBx->At(ix * fNy * fNz + iy * fNz + iz);
        fHa[1][0][0] = fBx->At((ix + 1) * fNy * fNz + iy * fNz + iz);
        fHa[0][1][0] = fBx->At(ix * fNy * fNz + (iy + 1) * fNz + iz);
        fHa[1][1][0] = fBx->At((ix + 1) * fNy * fNz + (iy + 1) * fNz + iz);
        fHa[0][0][1] = fBx->At(ix * fNy * fNz + iy * fNz + (iz + 1));
        fHa[1][0][1] = fBx->At((ix + 1) * fNy * fNz + iy * fNz + (iz + 1));
        fHa[0][1][1] = fBx->At(ix * fNy * fNz + (iy + 1) * fNz + (iz + 1));
        fHa[1][1][1] = fBx->At((ix + 1) * fNy * fNz + (iy + 1) * fNz + (iz + 1));

        // Return interpolated field value
        return Interpolate(dx, dy, dz);

    }

    return 0.;
}

// -----------   Get y component of the field   ---------------------------

Double_t BmnFieldMap::GetBy(Double_t x, Double_t y, Double_t z) {
    
    Int_t ix = 0;
    Int_t iy = 0;
    Int_t iz = 0;
    Double_t dx = 0.;
    Double_t dy = 0.;
    Double_t dz = 0.;

    if (IsInside(x, y, z, ix, iy, iz, dx, dy, dz)) {

        // Get By field values at grid cell corners
        fHa[0][0][0] = fBy->At(ix * fNy * fNz + iy * fNz + iz);
        fHa[1][0][0] = fBy->At((ix + 1) * fNy * fNz + iy * fNz + iz);
        fHa[0][1][0] = fBy->At(ix * fNy * fNz + (iy + 1) * fNz + iz);
        fHa[1][1][0] = fBy->At((ix + 1) * fNy * fNz + (iy + 1) * fNz + iz);
        fHa[0][0][1] = fBy->At(ix * fNy * fNz + iy * fNz + (iz + 1));
        fHa[1][0][1] = fBy->At((ix + 1) * fNy * fNz + iy * fNz + (iz + 1));
        fHa[0][1][1] = fBy->At(ix * fNy * fNz + (iy + 1) * fNz + (iz + 1));
        fHa[1][1][1] = fBy->At((ix + 1) * fNy * fNz + (iy + 1) * fNz + (iz + 1));

        // Return interpolated field value
        return Interpolate(dx, dy, dz);

    }

    return 0.;
}

// -----------   Get z component of the field   ---------------------------

Double_t BmnFieldMap::GetBz(Double_t x, Double_t y, Double_t z) {

    Int_t ix = 0;
    Int_t iy = 0;
    Int_t iz = 0;
    Double_t dx = 0.;
    Double_t dy = 0.;
    Double_t dz = 0.;

    if (IsInside(x, y, z, ix, iy, iz, dx, dy, dz)) {

        // Get Bz field values at grid cell corners
        fHa[0][0][0] = fBz->At(ix * fNy * fNz + iy * fNz + iz);
        fHa[1][0][0] = fBz->At((ix + 1) * fNy * fNz + iy * fNz + iz);
        fHa[0][1][0] = fBz->At(ix * fNy * fNz + (iy + 1) * fNz + iz);
        fHa[1][1][0] = fBz->At((ix + 1) * fNy * fNz + (iy + 1) * fNz + iz);
        fHa[0][0][1] = fBz->At(ix * fNy * fNz + iy * fNz + (iz + 1));
        fHa[1][0][1] = fBz->At((ix + 1) * fNy * fNz + iy * fNz + (iz + 1));
        fHa[0][1][1] = fBz->At(ix * fNy * fNz + (iy + 1) * fNz + (iz + 1));
        fHa[1][1][1] = fBz->At((ix + 1) * fNy * fNz + (iy + 1) * fNz + (iz + 1));

        // Return interpolated field value
        return Interpolate(dx, dy, dz);

    }

    return 0.;
}
// ------------------------------------------------------------------------



// -----------   Check whether a poInt_t is inside the map   ----------------

Bool_t BmnFieldMap::IsInside(Double_t x, Double_t y, Double_t z,
        Int_t& ix, Int_t& iy, Int_t& iz,
        Double_t& dx, Double_t& dy, Double_t& dz) {

    // --- Transform into local coordinate system
    Double_t xl = x - fPosX;
    Double_t yl = y - fPosY;
    Double_t zl = z - fPosZ;

    // ---  Check for being outside the map range
    if (!(xl >= fXmin && xl < fXmax && yl >= fYmin && yl < fYmax &&
            zl >= fZmin && zl < fZmax)) {
        ix = iy = iz = 0;
        dx = dy = dz = 0.;
        return kFALSE;
    }

    // --- Determine grid cell
    ix = Int_t((xl - fXmin) / fXstep);
    iy = Int_t((yl - fYmin) / fYstep);
    iz = Int_t((zl - fZmin) / fZstep);


    // Relative distance from grid poInt_t (in units of cell size)
    dx = (xl - fXmin) / fXstep - Double_t(ix);
    dy = (yl - fYmin) / fYstep - Double_t(iy);
    dz = (zl - fZmin) / fZstep - Double_t(iz);

    return kTRUE;

}
// ------------------------------------------------------------------------



// ----------   Write the map to an ASCII file   --------------------------

void BmnFieldMap::WriteAsciiFile(const char* fileName) {

    // Open file
    cout << "-I- BmnFieldMap: Writing field map to ASCII file "
            << fileName << endl;
    ofstream mapFile(fileName);
    if (!mapFile.is_open()) {
        cerr << "-E- BmnFieldMap:ReadAsciiFile: Could not open file! " << endl;
        return;
    }

    // Write field map grid parameters
    mapFile.precision(6);
    mapFile << showpoint;
    if (fType == 1) mapFile << "nosym" << endl;
    if (fType == 2) mapFile << "sym2" << endl;
    if (fType == 3) mapFile << "sym3" << endl;
    mapFile << fXmin << " " << fXmax << " " << fNx << endl;
    mapFile << fYmin << " " << fYmax << " " << fNy << endl;
    mapFile << fZmin << " " << fZmax << " " << fNz << endl;

    // Write field values
    Double_t factor = 10. * fScale; // Takes out scaling and converts kG->T
    cout << right;
    Int_t nTot = fNx * fNy * fNz;
    cout << "-I- BmnFieldMap: " << fNx * fNy * fNz << " entries to write... "
            << setw(3) << 0 << " % ";
    Int_t index = 0;
    div_t modul;
    Int_t iDiv = TMath::Nint(nTot / 100.);
    for (Int_t ix = 0; ix < fNx; ix++) {
        for (Int_t iy = 0; iy < fNy; iy++) {
            for (Int_t iz = 0; iz < fNz; iz++) {
                index = ix * fNy * fNz + iy * fNz + iz;
                modul = div(index, iDiv);
                if (modul.rem == 0) {
                    Double_t perc = TMath::Nint(100. * index / nTot);
                    cout << "\b\b\b\b\b\b" << setw(3) << perc << " % " << flush;
                }
                mapFile << fBx->At(index) / factor << " " << fBy->At(index) / factor
                        << " " << fBz->At(index) / factor << endl;
            } // z-Loop
        } // y-Loop
    } // x-Loop
    cout << "   " << index + 1 << " written" << endl;
    mapFile.close();


}
// ------------------------------------------------------------------------



// -------   Write field map to a ROOT file   -----------------------------

void BmnFieldMap::WriteRootFile(const char* fileName,
        const char* mapName) {

    BmnFieldMapData* data = new BmnFieldMapData(mapName, *this);
    TFile* oldFile = gFile;
    TFile* file = new TFile(fileName, "RECREATE");
    data->Write();
    file->Close();
    if (oldFile) oldFile->cd();

}
// ------------------------------------------------------------------------



// -----  Set the position of the field centre in global coordinates  -----

void BmnFieldMap::SetPosition(Double_t x, Double_t y, Double_t z) {
    fPosX = x;
    fPosY = y;
    fPosZ = z;
}
// ------------------------------------------------------------------------



// ---------   Screen output   --------------------------------------------

void BmnFieldMap::Print() {
    TString type = "Map";
    if (fType == 2) type = "Map sym2";
    if (fType == 3) type = "Map sym3";
    cout << "======================================================" << endl;
    std::cout.precision(4);
    cout << showpoint;
    cout << "----  " << fTitle << " : " << fName << endl;
    cout << "----" << endl;
    cout << "----  Field type     : " << type << endl;
    cout << "----" << endl;
    cout << "----  Field map grid : " << endl;
    cout << "----  x = " << setw(4) << fXmin << " to " << setw(4) << fXmax
            << " cm, " << fNx << " grid points, dx = " << fXstep << " cm" << endl;
    cout << "----  y = " << setw(4) << fYmin << " to " << setw(4) << fYmax
            << " cm, " << fNy << " grid points, dy = " << fYstep << " cm" << endl;
    cout << "----  z = " << setw(4) << fZmin << " to " << setw(4) << fZmax
            << " cm, " << fNz << " grid points, dz = " << fZstep << " cm" << endl;
    cout << endl;
    cout << "----  Field centre position: ( " << setw(6) << fPosX << ", "
            << setw(6) << fPosY << ", " << setw(6) << fPosZ << ") cm" << endl;
    cout << "----  Field scaling factor: " << fScale << endl;
    Double_t bx = GetBx(fPosX, fPosY, fPosZ);
    Double_t by = GetBy(fPosX, fPosY, fPosZ);
    Double_t bz = GetBz(fPosX, fPosY, fPosZ);
    cout << "----" << endl;
    cout << "----  Field at Magnet Center ( " << setw(6) << bx << ", " << setw(6)
            << by << ", " << setw(6) << bz << ") kG" << endl;
    cout << "======================================================" << endl;
}
// ------------------------------------------------------------------------



// ---------    Reset parameters and data (private)  ----------------------

void BmnFieldMap::Reset() {
    fPosX = fPosY = fPosZ = 0.;
    fXmin = fYmin = fZmin = 0.;
    fXmax = fYmax = fZmax = 0.;
    fXstep = fYstep = fZstep = 0.;
    fNx = fNy = fNz = 0;
    fScale = 1.;
    if (fBx) {
        delete fBx;
        fBx = NULL;
    }
    if (fBy) {
        delete fBy;
        fBy = NULL;
    }
    if (fBz) {
        delete fBz;
        fBz = NULL;
    }
}
// ------------------------------------------------------------------------



// -----   Read field map from ASCII file (private)   ---------------------

void BmnFieldMap::ReadAsciiFile(const char* fileName) {

    Double_t bx = 0., by = 0., bz = 0.;

    // Open file
    LOG(INFO) << "BmnFieldMap: Reading field map from ASCII file "
            << fileName << FairLogger::endl;
    ifstream mapFile(fileName);
    if (!mapFile.is_open()) {
        LOG(ERROR) << "BmnFieldMap:ReadAsciiFile: Could not open file!" << FairLogger::endl;
        LOG(FATAL) << "BmnFieldMap:ReadAsciiFile: Could not open file!" << FairLogger::endl;
    }

    // Read map type
    TString type;
    mapFile >> type;
    Int_t iType = 0;
    if (type == "nosym") iType = 1;
    if (type == "sym2") iType = 2;
    if (type == "sym3") iType = 3;
    if (fType != iType) {
        cout << "-E- BmnFieldMap::ReadAsciiFile: Incompatible map types!"
                << endl;
        cout << "    Field map is of type " << fType
                << " but map on file is of type " << iType << endl;
        Fatal("ReadAsciiFile", "Incompatible map types");
    }

    // Read grid parameters
    mapFile >> fXmin >> fXmax >> fNx;
    cout << fXmin << " " << fXmax << " " << fNx;
    mapFile >> fYmin >> fYmax >> fNy;
    mapFile >> fZmin >> fZmax >> fNz;
    fXstep = (fXmax - fXmin) / Double_t(fNx - 1);
    fYstep = (fYmax - fYmin) / Double_t(fNy - 1);
    fZstep = (fZmax - fZmin) / Double_t(fNz - 1);

    // Create field arrays
    fBx = new TArrayF(fNx * fNy * fNz);
    fBy = new TArrayF(fNx * fNy * fNz);
    fBz = new TArrayF(fNx * fNy * fNz);

    // Read the field values
    Double_t factor = fScale * 10.; // Factor 10 for T -> kG
    cout << right;
    Int_t nTot = fNx * fNy * fNz;
    //cout << "-I- BmnFieldMap: " << nTot << " entries to read... " << setw(3) << 0 << " % ";
    Int_t index = 0;
    div_t modul;
    Int_t iDiv = TMath::Nint(nTot / 100.);
    for (Int_t ix = 0; ix < fNx; ix++) {
        for (Int_t iy = 0; iy < fNy; iy++) {
            for (Int_t iz = 0; iz < fNz; iz++) {
                if (!mapFile.good()) cerr << "-E- BmnFieldMap::ReadAsciiFile: "
                        << "I/O Error at " << ix << " "
                        << iy << " " << iz << endl;
                index = ix * fNy * fNz + iy * fNz + iz;
                modul = div(index, iDiv);
                if (modul.rem == 0) {
                    Double_t perc = TMath::Nint(100. * index / nTot);
                    // cout << "\b\b\b\b\b\b" << setw(3) << perc << " % " << flush;
                }
                mapFile >> bx >> by >> bz;
                fBx->AddAt(factor*bx, index);
                fBy->AddAt(factor*by, index);
                fBz->AddAt(factor*bz, index);
                if (mapFile.eof()) {
                    cerr << endl << "-E- BmnFieldMap::ReadAsciiFile: EOF"
                            << " reached at " << ix << " " << iy << " " << iz << endl;
                    mapFile.close();
                    break;
                }
            } // z-Loop
        } // y-Loop0)
    } // x-Loop

    cout << "   " << index + 1 << " read" << endl;

    mapFile.close();
    
    Print();

}
// ------------------------------------------------------------------------



// -------------   Read field map from ROOT file (private)  ---------------

void BmnFieldMap::ReadRootFile(const char* fileName,
        const char* mapName) {

    // Store gFile pointer
    TFile* oldFile = gFile;

    // Open root file
    LOG(INFO) << "BmnFieldMap: Reading field map from ROOT file "
            << fileName << FairLogger::endl;
    TFile* file = new TFile(fileName, "READ");
    if (!(file->IsOpen())) {
        LOG(ERROR) << "BmnFieldMap:ReadRootFile: Could not open file!" << FairLogger::endl;
        LOG(FATAL) << "BmnFieldMap:ReadRootFile: Could not open file!" << FairLogger::endl;
    }

    // Get the field data object
    BmnFieldMapData* data = NULL;
    file->GetObject(mapName, data);
    if (!data) {
        cout << "-E- BmnFieldMap::ReadRootFile: data object " << fileName
                << " not found in file! " << endl;
        exit(-1);
    }

    // Get the field parameters
    SetField(data);

    // Close the root file and delete the data object
    file->Close();
    delete data;
    if (oldFile) oldFile->cd();

}
// ------------------------------------------------------------------------



// ------------   Set field parameters and data (private)  ----------------

void BmnFieldMap::SetField(const BmnFieldMapData* data) {

    // Check compatibility
    if (data->GetType() != fType) {
        if (!((data->GetType() == 3)&&(fType == 5))) // E.Litvinenko
        {
            cout << "-E- BmnFieldMap::SetField: Incompatible map types!"
                    << endl;
            cout << "    Field map is of type " << fType
                    << " but map on file is of type " << data->GetType() << endl;
            Fatal("SetField", "Incompatible map types");
        } else
            cout << "   BmnFieldMap::SetField: Warning:  You are using PosDepScaled map (original map type = 3)" << endl;
    }


    fXmin = data->GetXmin();
    fYmin = data->GetYmin();
    fZmin = data->GetZmin();
    fXmax = data->GetXmax();
    fYmax = data->GetYmax();
    fZmax = data->GetZmax();
    fNx = data->GetNx();
    fNy = data->GetNy();
    fNz = data->GetNz();
    fXstep = (fXmax - fXmin) / Double_t(fNx - 1);
    fYstep = (fYmax - fYmin) / Double_t(fNy - 1);
    fZstep = (fZmax - fZmin) / Double_t(fNz - 1);
    if (fBx) delete fBx;
    if (fBy) delete fBy;
    if (fBz) delete fBz;
    fBx = new TArrayF(*(data->GetBx()));
    fBy = new TArrayF(*(data->GetBy()));
    fBz = new TArrayF(*(data->GetBz()));

    // Scale and convert from T to kG
    Double_t factor = fScale * 10.;
    Int_t index = 0;
    for (Int_t ix = 0; ix < fNx; ix++) {
        for (Int_t iy = 0; iy < fNy; iy++) {
            for (Int_t iz = 0; iz < fNz; iz++) {
                index = ix * fNy * fNz + iy * fNz + iz;
                if (fBx) (*fBx)[index] = (*fBx)[index] * factor;
                if (fBy) (*fBy)[index] = (*fBy)[index] * factor;
                if (fBz) (*fBz)[index] = (*fBz)[index] * factor;
            }
        }
    }
}
// ------------------------------------------------------------------------



// ------------   Interpolation in a grid cell (private)  -----------------

Double_t BmnFieldMap::Interpolate(Double_t dx, Double_t dy, Double_t dz) {

    // Interpolate in x coordinate
    fHb[0][0] = fHa[0][0][0] + (fHa[1][0][0] - fHa[0][0][0]) * dx;
    fHb[1][0] = fHa[0][1][0] + (fHa[1][1][0] - fHa[0][1][0]) * dx;
    fHb[0][1] = fHa[0][0][1] + (fHa[1][0][1] - fHa[0][0][1]) * dx;
    fHb[1][1] = fHa[0][1][1] + (fHa[1][1][1] - fHa[0][1][1]) * dx;

    // Interpolate in y coordinate
    fHc[0] = fHb[0][0] + (fHb[1][0] - fHb[0][0]) * dy;
    fHc[1] = fHb[0][1] + (fHb[1][1] - fHb[0][1]) * dy;

    // Interpolate in z coordinate
    return fHc[0] + (fHc[1] - fHc[0]) * dz;

}

// ------------------------------------------------------------------------

void BmnFieldMap::FillParContainer() {

}

// FUNCTIONS TO RECALCULATE MAG.FIELD, ITS APPROXIMATION AND ITS EXTRAPOLATION

Double_t* BmnFieldMap::InterpolateMagField(Int_t N, BmnFieldMap* magField, Char_t dir) {

    if (fDebugInfo)
        cout << "DIRECTION is " << dir << endl;

    const Int_t size = (Int_t) pow(N, 3);

    /// Ax = B - matrix form of a set of linear equations 

    Double_t** A = new Double_t* [size]; // A-matrix of coefficients (left side), A is a square matrix !!!
    Double_t** A_orig = NULL; // Copy of A-matrix to check solution
    Double_t* B = new Double_t[size]; // B-vector (right side) 
    Double_t* B_orig = NULL; // Copy of B-vector to check solution
    Double_t* coeff = new Double_t[size]; // Solution of the Ax = B  

    //
    for (Int_t i = 0; i < size; i++) 
        A[i] = new Double_t[size + 1];
       
    
    cout << "Size = " << sizeof (Double_t) * size * (size + 1) / pow(1024, 3) << " GBytes" << endl;

    BuildGrid(N, size, magField, A, B, coeff, dir, 0);

    if (fDebugInfo) {
        B_orig = new Double_t[size];
        A_orig = new Double_t* [size];
        for (Int_t i = 0; i < size; i++) {
            A_orig[i] = new Double_t[size];
            B_orig[i] = B[i];
            for (Int_t j = 0; j < size; j++)
                A_orig[i][j] = A[i][j];
        }
    }

       Double_t uncert = 1e-6;

        for (Int_t i = 0; i < size; i++) {
            if (TMath::Abs(A[i][i]) < uncert) {
                cout << " A[i][i] " << A[i][i] << endl;
                for (Int_t k = i + 1; k < size; k++) {
                    if (A[k][i] >= uncert) {
                        StringPermutation(size, A, B, i, k);
                        break;
                    }
                }
            }
        }

        MakeTransform(size, A, B);

    // Print Original Matrix
    //    for (Int_t i = 0; i < size; i++) {
    //        for (Int_t j = 0; j <= size; j++) {
    //            cout << A[i][j] << "\t";
    //        }
    //        cout << endl;
    //    }

    
    // coeff = DoConjugGradient(size, A, B, uncert);
    // coeff = DoSeidelSolution(size, A, coeff, B, uncert);
    coeff = DoGausSolution(size, A, A_orig);

    vector<Double_t> solution;

    for (Int_t i = 0; i < size; i++) {
        solution.push_back(coeff[i]);
        cout << " a[" << i << "] = " << coeff[i] << endl;
    }

    if (fDebugInfo) {
        // BuildGrid(N, size, magField, A, B, coeff, dir, 1);
        DoCheckSolution(A_orig, B_orig, solution, size);
    }

    for (Int_t i = 0; i < size; i++) {
        delete [] A[i];
        if (fDebugInfo)
            delete [] A_orig[i];
    }
    delete [] A;
    if (fDebugInfo) {
        delete [] A_orig;
        delete [] B_orig;
    }

    return coeff;
}

Double_t* BmnFieldMap::DoConjugGradient(Int_t N, Double_t** A, Double_t* B, Double_t eps) {

    Double_t alpha = 0.;
    Double_t beta = 0.;

    Double_t* p = new Double_t[N];
    for (Int_t i = 0; i < N; i++)
        p[i] = B[i];

    Double_t* tmp = MatrixByVectorMult(N, A, p);

    Double_t* r = new Double_t[N];
    Double_t* r_prev = new Double_t[N];

    Double_t* z = new Double_t[N];
    Double_t* z_prev = new Double_t[N];

    Double_t* x = new Double_t[N];

    //iteration = 0
    for (Int_t i = 0; i < N; i++) {
        r_prev[i] = B[i] - tmp[i]; // cout << tmp[i] << endl;
        z_prev[i] = r_prev[i];
    }

    Int_t k = 0;

    //iterations > 1
    do {
        k++;

        cout << "counter = " << k << endl;

        if (k >= 2) {
            for (Int_t i = 0; i < N; i++)
                p[i] = x[i];
        }

        alpha = VectorByVectorMult(N, r_prev, r_prev) / VectorByVectorMult(N, MatrixByVectorMult(N, A, z_prev), z_prev);
        // cout << " alpha = " << alpha << endl;

        for (Int_t i = 0; i < N; i++) {
            x[i] = p[i] + alpha * z_prev[i];
            tmp = MatrixByVectorMult(N, A, z_prev);
            r[i] = r_prev[i] - alpha * tmp[i];
        }

        beta = VectorByVectorMult(N, r, r) / VectorByVectorMult(N, r_prev, r_prev);

        for (Int_t i = 0; i < N; i++)
            z[i] = r[i] + beta * z_prev[i];

        for (Int_t i = 0; i < N; i++) {
            r_prev[i] = r[i];
            z_prev[i] = z[i];
        }

    } while (!ConvergeSeidel(x, p, N, eps));

    delete [] z_prev;
    delete [] z;
    delete [] r_prev;
    delete [] r;
    delete [] tmp;
    delete [] p;

    cout << "Number of Iterations = " << k << endl;

    return x;
}

Double_t* BmnFieldMap::MatrixByVectorMult(Int_t N, Double_t** matrix, Double_t* vector) {

    Double_t* res = new Double_t[N];
    Double_t sum = 0.;

    for (Int_t i = 0; i < N; i++) {
        sum = 0.0;

        for (Int_t j = 0; j < N; j++) {
            sum += matrix[i][j] * vector[j];
        }
        res[i] = sum;
    }
    return res;
}

Double_t BmnFieldMap::VectorByVectorMult(Int_t N, Double_t* vec1, Double_t* vec2) {

    Double_t sum = 0.;

    for (Int_t i = 0; i < N; i++)
        sum += vec1[i] * vec2[i];

    return sum;
}

Double_t* BmnFieldMap::DoSeidelSolution(Int_t N, Double_t** a, Double_t* x, Double_t* b, Double_t eps) {

    Int_t counter = 0;
    Double_t* p = new Double_t[N];

    do {
        //        cout << "counter: " << counter << " ";
        //        for (Int_t i = 0; i < N; i++) {
        //            cout << " a[" << i << "] = " << x[i] << " ";
        //        }
        //        cout << endl;
        counter++;
        for (Int_t i = 0; i < N; i++) {
            p[i] = x[i];
            // cout << "x = " << x[i] << " p = " << p[i] << endl; 
        }

        for (Int_t i = 0; i < N; i++) {

            Double_t var = 0.;

            for (Int_t j = 0; j < i; j++)
                var += (a[i][j] * x[j]);

            for (Int_t j = i; j < N; j++)
                var += (a[i][j] * p[j]);

            // cout << a[i][i] << endl;
            x[i] = (b[i] - var) / a[i][i];
            //            if (TMath::Abs(a[i][i]) < 0.0001) {
            //                cout << "b = " << b[i] << " var = " << var << " a = " << a[i][i] << endl;
            //            }

            //            x[i] = (a[i][N] - var) / a[i][i];
        }
    } while (!ConvergeSeidel(x, p, N, eps));

    // cout << counter << endl;

    return x;
}

Bool_t BmnFieldMap::ConvergeSeidel(Double_t* xk, Double_t* xkp, Int_t N, Double_t eps) {

    Double_t norm = 0;
    for (Int_t i = 0; i < N; i++) {
        norm += (xk[i] - xkp[i])*(xk[i] - xkp[i]);
        // cout << "x = " << xk[i] << " p = " << xkp[i]; 
    }

    // cout << " Norm = " << sqrt(norm) << endl;
    if (sqrt(norm) >= eps)
        return kFALSE;

    return kTRUE;
}

Double_t* BmnFieldMap::DoGausSolution(Int_t dim, Double_t** matrix, Double_t** matrix_orig) {

    const Int_t dim2 = dim + 1;

    Double_t* xx = new Double_t[dim2];
    Double_t tmp;
    Int_t k;

    Int_t counter = 0;

    for (Int_t i = 0; i < dim; i++) {
        counter++;
        tmp = matrix[i][i];
        for (Int_t j = dim; j >= i; j--)
            matrix[i][j] /= tmp;
        for (Int_t j = i + 1; j < dim; j++) {
            tmp = matrix[j][i];
            for (Int_t k = dim; k >= i; k--)
                matrix[j][k] -= tmp * matrix[i][k];
        }
        // cout << "Transformation N = " << counter << " DONE! " << endl; 
    }

    // cout << endl;

    xx[dim - 1] = matrix[dim - 1][dim];
    for (Int_t i = dim - 2; i >= 0; i--) {
        xx[i] = matrix[i][dim];
        for (Int_t j = i + 1; j < dim; j++) xx[i] -= matrix[i][j] * xx[j];
    }

    if (fDebugInfo)
        for (Int_t i = 0; i < dim; i++)
            cout << " a[" << i << "] = " << xx[i] << endl;

    vector<Double_t> solution;
    for (Int_t i = 0; i < dim; i++)
        solution.push_back(xx[i]);

    // if (fDebugInfo) DoCheckSolution(matrix_orig, solution, dim);

    return xx;
}

Bool_t BmnFieldMap::DoCheckSolution(Double_t** matrix_orig, Double_t* b_orig, vector<Double_t> &sol, Int_t dim) {

    vector<Double_t > sum;

    Double_t tmp = 0;
    Double_t tmp2 = 0;

    for (Int_t i = 0; i < dim; i++) {
        for (Int_t j = 0; j < dim; j++) {
            tmp += matrix_orig[i][j] * sol.at(j);
            //cout << tmp << endl;
        }

        sum.push_back(tmp);
        tmp = 0;
    }

    for (Int_t i = 0; i < dim; i++) {
        tmp2 = sum.at(i) - b_orig[i];
        cout << "Residual = " << tmp2 << endl;
    }

    return kTRUE;
}

void BmnFieldMap::BuildGrid(Int_t N, Int_t size, BmnFieldMap* magField, Double_t** arr, Double_t* &B, Double_t* arr1, Char_t dir, Int_t sw) {

    Double_t x, y, z;
    Double_t FieldFound = 0.0;

    fScale = 0.9;

    Double_t Xmax = magField->GetXmax() * fScale;
    Double_t Xmin = -Xmax * fScale;

    Double_t Ymax = magField->GetYmax() * fScale;
    Double_t Ymin = -Ymax * fScale;

    Double_t Zmin = magField->GetZmin(); //* fScale;
    Double_t Zmax = magField->GetZmax(); //* fScale;

    for (Int_t zIdx = 0; zIdx < N - 1; ++zIdx) {
        z = Zmin + (Zmax - Zmin) / (N - 1) * zIdx;
        for (Int_t xIdx = 0; xIdx < N; ++xIdx) {
            x = Xmin + (Xmax - Xmin) / (N - 1) * xIdx;
            for (Int_t yIdx = 0; yIdx < N; ++yIdx) {
                y = Ymin + (Ymax - Ymin) / (N - 1) * yIdx;
                for (Int_t i = 0; i < N; i++) {
                    for (Int_t j = 0; j < N; j++) {
                        for (Int_t k = 0; k < N; k++) {
                            Int_t first = xIdx * N * N + yIdx * N + zIdx;
                            Int_t second = i * N * N + j * N + k;

                            if (sw == 0)
                                arr[first][second] = pow(x, i) * pow(y, j) * pow(z, k);

                            else if (sw == 1)
                                FieldFound += arr1[i * N * N + j * N + k] * pow(x, i) * pow(y, j) * pow(z, k);
                            //                            cout << arr[xIdx * N * N + yIdx * N + zIdx][i * N * N + j * N + k] << "\t";
                            //                            cout <<endl;

                        }
                    }
                }
                if (sw == 0) {
                    if (dir == 'X') {
                        arr[xIdx * N * N + yIdx * N + zIdx][size] = magField->GetBx(x, y, z);
                        B[xIdx * N * N + yIdx * N + zIdx] = magField->GetBx(x, y, z);
                    } else if (dir == 'Y') {
                        arr[xIdx * N * N + yIdx * N + zIdx][size] = magField->GetBy(x, y, z);
                        B[xIdx * N * N + yIdx * N + zIdx] = magField->GetBy(x, y, z);
                    } else if (dir == 'Z') {
                        arr[xIdx * N * N + yIdx * N + zIdx][size] = magField->GetBz(x, y, z);
                        B[xIdx * N * N + yIdx * N + zIdx] = magField->GetBz(x, y, z);
                    }
                    // cout << arr[xIdx * N * N + yIdx * N + zIdx][size] << endl;

                    //cout << i << " " << j << " " << k << endl;
                }
                else if (sw == 1) {
                    if (dir == 'X')
                        cout << " Residuals at the grid used, X " << FieldFound - magField->GetBx(x, y, z) << endl;
                    else if (dir == 'Y')
                        cout << " Residuals at the grid used, Y " << FieldFound - magField->GetBy(x, y, z) << endl;
                    else if (dir == 'Z')
                        cout << " Residuals at the grid used, Z " << FieldFound - magField->GetBz(x, y, z) << endl;
                    FieldFound = 0.0;
                }
            }
        }
    }
}

Double_t BmnFieldMap::Lagrange(Int_t N, Double_t* arr, TVector3 coord) {

    Double_t res = 0;

    for (Int_t i = 0; i < N; i++) {
        for (Int_t j = 0; j < N; j++) {
            for (Int_t k = 0; k < N; k++)
                res += arr[i * N * N + j * N + k] * pow(coord.X(), i) * pow(coord.Y(), j) * pow(coord.Z(), k);
        }
    }
    return res;
}

void BmnFieldMap::StringPermutation(Int_t size, Double_t** A, Double_t* B, Int_t i, Int_t k) {

    Double_t tmp = 0.;

    for (Int_t j = 0; j < size; j++) {
        tmp = A[i][j];
        A[i][j] = A[k][j];
        A[k][j] = tmp;
    }

    tmp = 0.;

    tmp = B[i];
    B[i] = B[k];
    B[k] = tmp;
}

Double_t BmnFieldMap::FirstNorm(Double_t** A, Int_t n, Int_t m) {

    Int_t i, j;
    Double_t sum = 0, subSum;
    for (Int_t i = 0; i < n; i++) {
        subSum = 0;
        for (j = 0; j < m; j++) {
            subSum += abs(A[i][j]);
        }

        if (subSum > sum) {
            sum = subSum;
        }
    }
    return sum;
}

Double_t BmnFieldMap::SecondNorm(Double_t** A, Int_t n, Int_t m) {

    Int_t i, j;
    Double_t sum = 0, subSum;
    for (j = 0; j < n; j++) {
        subSum = 0;
        for (i = 0; i < m; i++) {
            subSum += abs(A[i][j]);
        }

        if (subSum > sum) {
            sum = subSum;
        }
    }
    return sum;
}

Double_t BmnFieldMap::ThirdNorm(Double_t** A, Int_t n, Int_t m) {

    Int_t i, j;
    Double_t sum = 0;
    for (i = 0; i < n; i++) {
        for (j = 0; j < m; j++) {
            sum += (A[i][j] * A[i][j]);
        }
    }
    sum = sqrt(sum);
    return sum;
}

void BmnFieldMap::MakeTransform(Int_t size, Double_t** A, Double_t* B) {

    Double_t** buff = new Double_t*[size]; // A^{transposed}
    Double_t** buff2 = new Double_t*[size]; // A^{transposed} * A
    Double_t* buff3 = new Double_t[size]; // A^{transposed} * B

    for (Int_t i = 0; i < size; i++) {
        buff[i] = new Double_t[size];
        buff2[i] = new Double_t[size];
    }

    // A ---> A^{transposed}
    for (Int_t i = 0; i < size; i++)
        for (Int_t j = 0; j < size; j++)
            buff[i][j] = A[j][i];

    Double_t sum = 0.;

    for (Int_t i = 0; i < size; i++)
        for (Int_t j = 0; j < size; j++) {
            sum = 0.0;
            if (i <= j) {
                for (Int_t k = 0; k < size; k++) {
                    sum += buff[i][k] * A[k][j];
                }
                buff2[i][j] = sum;
            }
        }

    for (Int_t i = 0; i < size; i++)
        for (Int_t j = 0; j < size; j++)
            if (i > j)
                buff2[i][j] = buff2[j][i];

    Double_t norm_factor = 2. / ThirdNorm(buff2, size, size);

    for (Int_t i = 0; i < size; i++)
        for (Int_t j = 0; j < size; j++) {
            A[i][j] = norm_factor * buff2[i][j];
            //  cout << A[i][j] << "\t";
        }

    for (Int_t i = 0; i < size; i++) {
        sum = 0.0;
        for (Int_t j = 0; j < size; j++) {
            sum += buff[i][j] * B[j];
        }
        buff3[i] = sum;
    }

    for (Int_t i = 0; i < size; i++)
        B[i] = norm_factor * buff3[i];

    for (Int_t i = 0; i < size; i++) {
        delete [] buff[i];
        delete [] buff2[i];
    }

    delete [] buff;
    delete [] buff2;
    delete [] buff3;
}

ClassImp(BmnFieldMap)
