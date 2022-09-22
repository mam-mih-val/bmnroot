// ---------------------------------------------------------------------------------
// -----                    BmnFieldMap header file                    -------------
// -----                 Created 03/02/2015  by P. Batyuk              -------------
// ---------------------------------------------------------------------------------

#include "BmnFieldMap.h"
#include "TFile.h"
#include "TMath.h"
#include "TTree.h"

#include <iomanip>
#include <iostream>
using namespace std;

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
fBz(NULL),
fDebugInfo(kFALSE),
fIsOff(kFALSE)
{
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
fDebugInfo(kFALSE),
fIsOff(kFALSE)
{
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
fBz(NULL),
fDebugInfo(kFALSE),
fIsOff(kFALSE)
{
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
        fIsOff = fieldPar->IsFieldOff();
        TString dir = getenv("VMCWORKDIR");
        fFileName = dir + "/input/" + fName;
        fType = fieldPar->GetType();
    }
}

BmnFieldMap::~BmnFieldMap()
{
    if (fBx) delete fBx;
    if (fBy) delete fBy;
    if (fBz) delete fBz;
}

void BmnFieldMap::Init()
{
    if (fFileName.EndsWith(".root")) ReadRootFile(fFileName);
    else if (fFileName.EndsWith(".dat")) ReadAsciiFile(fFileName);
    else {
        cerr << "-E- BmnFieldMap::Init: No proper file name defined! ("
                << fFileName << ")" << endl;
        Fatal("Init", "No proper file name");
    }
}

void BmnFieldMap::WriteAsciiFile(const char* fileName)
{
    // Open file
    cout << "-I- BmnFieldMap: Writing field map to ASCII file " << fileName << endl;
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

void BmnFieldMap::WriteRootFile(const char* fileName, const char* mapName)
{
    BmnFieldMapData* data = new BmnFieldMapData(mapName, *this);
    TFile* oldFile = gFile;
    TFile* file = new TFile(fileName, "RECREATE");
    data->Write();
    file->Close();
    if (oldFile) oldFile->cd();
}

void BmnFieldMap::SetPosition(Double_t x, Double_t y, Double_t z)
{
    fPosX = x; fPosY = y; fPosZ = z;
}

void BmnFieldMap::Print(Option_t*)
{
    TString type = "Map";
    if (fType == 2) type = "Map sym2";
    if (fType == 3) type = "Map sym3";
    cout << "======================================================" << endl;
    cout.precision(4);
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
    cout << "----  Target position: ( " << setw(6) << fPosX << ", "
            << setw(6) << fPosY << ", " << setw(6) << fPosZ << ") cm" << endl;
    cout << "----  Field scaling factor: " << fScale << endl;
    if (fIsOff) cout << "----  Field is off  ----" << endl;
    Double_t bx = GetBx(fPosX, fPosY, fPosX);
    Double_t by = GetBy(fPosX, fPosY, fPosY);
    Double_t bz = GetBz(fPosX, fPosY, fPosZ);
    cout << "----" << endl;
    cout << "----  Field at Target Position ( " << setw(6) << bx << ", " << setw(6)
            << by << ", " << setw(6) << bz << ") kG" << endl;
    cout << "======================================================" << endl;
}

void BmnFieldMap::Reset()
{
    fPosX = fPosY = fPosZ = 0.;
    fXmin = fYmin = fZmin = 0.;
    fXmax = fYmax = fZmax = 0.;
    fXstep = fYstep = fZstep = 0.;
    fNx = fNy = fNz = 0;
    fScale = 1.;
    fIsOff = kFALSE;
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

void BmnFieldMap::ReadAsciiFile(const char* fileName)
{
    Double_t bx = 0., by = 0., bz = 0.;

    // Open file
    LOG(info) << "BmnFieldMap: Reading field map from ASCII file "
            << fileName;
    ifstream mapFile(fileName);
    if (!mapFile.is_open()) {
        LOG(error) << "BmnFieldMap:ReadAsciiFile: Could not open file!";
        LOG(fatal) << "BmnFieldMap:ReadAsciiFile: Could not open file!";
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
  
    fXmin += fPosX;
    fXmax += fPosX;
    
    fYmin += fPosY;
    fYmax += fPosY;
    
    fZmin += fPosZ;
    fZmax += fPosZ;

    mapFile.close();
}

void BmnFieldMap::ReadRootFile(const char* fileName)
{
    TString *type = 0;
    coordinate_info_t X;
    coordinate_info_t Y;
    coordinate_info_t Z;
    vector <Double_t> *read_Field = NULL;

    // Open root file
    LOG(info) << "BmnFieldMap: Reading field map from ROOT file "
            << fileName;
    TFile* file = new TFile(fileName, "READ");

    if (!(file->IsOpen())) {
        LOG(error) << "BmnFieldMap:ReadRootFile: Could not open file!";
        LOG(fatal) << "BmnFieldMap:ReadRootFile: Could not open file!";
    }
  
    TTree *tree = (TTree*) file->Get("Main_info");
    TTree *t = (TTree*) file->Get("Field_map");
    tree->SetBranchAddress("Field_type", &type);
    tree->SetBranchAddress("Main_info_X", &X);
    tree->SetBranchAddress("Main_info_Y", &Y);
    tree->SetBranchAddress("Main_info_Z", &Z);
    t->SetBranchAddress("field_map", &read_Field);

    tree->GetEntry(0);
    Int_t iType = 0;

    if (*type == "nosym") iType = 1;
    if (*type == "sym2") iType = 2;
    if (*type == "sym3") iType = 3;
    if (fType != iType) {
        cout << "-E- BmnFieldMap::ReadRootFile: Incompatible map types!"
                << endl;
        cout << "    Field map is of type " << fType
                << " but map on file is of type " << iType << endl;
        Fatal("ReadRootFile", "Incompatible map types");
    }

    fNx = X.N;
    fXmin = X.min + fPosX;
    fXmax = X.max + fPosX;
    fXstep = X.step;

    fNy = Y.N;
    fYmin = Y.min + fPosY;
    fYmax = Y.max + fPosY;
    fYstep = Y.step;

    fNz = Z.N;
    fZmin = Z.min + fPosZ;
    fZmax = Z.max + fPosZ;
    fZstep = Z.step;

    fBx = new TArrayF(fNx * fNy * fNz);
    fBy = new TArrayF(fNx * fNy * fNz);
    fBz = new TArrayF(fNx * fNy * fNz);

    Double_t factor = fScale; 
    Int_t nTot = fNx * fNy * fNz;
    Int_t index = 0;
    div_t modul;
    Int_t iDiv = TMath::Nint(nTot / 100.);

    for (Int_t ix = 0; ix < fNx; ix++) {
        for (Int_t iy = 0; iy < fNy; iy++) {
            for (Int_t iz = 0; iz < fNz; iz++) {
                modul = div(index, iDiv);
                if (modul.rem == 0) {
                    Double_t perc = TMath::Nint(100. * index / nTot);
                }
                index = ix * fNy * fNz + iy * fNz + iz;
                t->GetEntry(index);
                vector <Double_t> v = (*read_Field);
                fBx->AddAt(factor*v[0], index);
                fBy->AddAt(factor*v[1], index);
                fBz->AddAt(factor*v[2], index);
            } // z-Loop
        } // y-Loop
    } // x-Loop
    t->Delete();
    tree->Delete();
    delete type;
    delete read_Field;
    delete file;
}

void BmnFieldMap::SetField(const BmnFieldMapData* data)
{
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

Double_t BmnFieldMap::Interpolate(Double_t dx, Double_t dy, Double_t dz)
{
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

ClassImp(BmnFieldMap)
