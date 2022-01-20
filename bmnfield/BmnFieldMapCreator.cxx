// -------------------------------------------------------------------------
// -----                   BmnFieldMapCreator source file              -----
// -----         Created 15/01/08                    by V. Friese      -----
// -------------------------------------------------------------------------

#include "BmnFieldMapCreator.h"
#include "BmnFieldMap.h"
#include <iostream>

using namespace std;


// -------------   Default constructor  -----------------------------------
BmnFieldMapCreator::BmnFieldMapCreator() 
  : fMapName(""),
    fNx(0), 
    fNy(0), 
    fNz(0),
    fXmin(0.), 
    fXmax(0.),
    fYmin(0.), 
    fYmax(0.),
    fZmin(0.), 
    fZmax(0.),
    fBx(NULL),
    fBy(NULL),
    fBz(NULL),
    fFieldList(),
    fInit(kFALSE)
{
}

// -------------   Standard constructor  ----------------------------------
BmnFieldMapCreator::BmnFieldMapCreator(const char* mapName) 
  : fMapName(mapName),
    fNx(0), 
    fNy(0), 
    fNz(0),
    fXmin(0.), 
    fXmax(0.),
    fYmin(0.), 
    fYmax(0.),
    fZmin(0.), 
    fZmax(0.),
    fBx(NULL),
    fBy(NULL),
    fBz(NULL),
    fFieldList(),
    fInit(kFALSE)

{
}

// ------------   Destructor   --------------------------------------------
BmnFieldMapCreator::~BmnFieldMapCreator() {
  if (fBx) delete fBx;
  if (fBy) delete fBy;
  if (fBz) delete fBz;
  fFieldList.Clear();
}


// -------------   Public method SetGridParameters   ----------------------
void BmnFieldMapCreator::SetGridParameters(Int_t nx, Double_t xmin, 
					   Double_t xmax, Int_t ny,
					   Double_t ymin, Double_t ymax,
					   Int_t nz, Double_t zmin,
					   Double_t zmax) {
  fNx = nx;
  fNy = ny;
  fNz = nz;
  fXmin = xmin;
  fYmin = ymin;
  fZmin = zmin;
  fXmax = xmax;
  fYmax = ymax;
  fZmax = zmax;
  fInit = kTRUE;
}


// -----------   Public method CreateMap   --------------------------------
Bool_t BmnFieldMapCreator::CreateMap(const char* fileName) {

  // Define output file name
  TString outFileName = fileName;
  if (fileName == "") outFileName = fMapName + ".root";

  // Check for proper intialisation
  if ( ! fInit ) {
    cout << "-E- BmnFieldMapCreator::CreateMap: "
	 << "Grid parameters are not specified!" << endl;
    return kFALSE;
  }
  if ( fFieldList.IsEmpty() ) {
    cout << "-E- BmnFieldMapCreator::CreateMap: "
	 << "No input field(s) specified!" << endl;
      return kFALSE;
  }

  // Create field arrays  
  fBx = new TArrayF(fNx * fNy * fNz);
  fBy = new TArrayF(fNx * fNy * fNz);
  fBz = new TArrayF(fNx * fNy * fNz);

  // Calculate grid step sizes
  Double_t xStep = ( fXmax - fXmin ) / Double_t ( fNx - 1 );
  Double_t yStep = ( fYmax - fYmin ) / Double_t ( fNy - 1 );
  Double_t zStep = ( fZmax - fZmin ) / Double_t ( fNz - 1 );

  // Control output
  cout.precision(2);
  cout << "-I- BmnFieldMapCreator: Grid step sizes are " << xStep
       << ", " << yStep << ", " << zStep << " cm" << endl;
  cout << "                        Using " << fFieldList.GetSize()
       << " input fields." << endl;
  cout << "                        Total number of grid points is "
       << fNx * fNy * fNz << endl;

  // Triple loop over grid points
  Double_t x = 0.;
  Double_t y = 0.;
  Double_t z = 0.;
  for (Int_t ix=0; ix<fNx; ix++) {
    x = fXmin + Double_t(ix) * xStep;
    for (Int_t iy=0; iy<fNy; iy++) {
      y = fYmin + Double_t(iy) * yStep;
      for (Int_t iz=0; iz<fNz; iz++) {
	z = fZmin + Double_t(iz) * zStep;

	// Get and add all field values at this grid point
	Double_t bx = 0.;
	Double_t by = 0.;
	Double_t bz = 0.;
	TIter next(&fFieldList);
	while ( FairField* field = ((FairField*)next()) ) {
	  bx += field->GetBx(x, y, z);
	  by += field->GetBy(x, y, z);
	  bz += field->GetBz(x, y, z);
	}

	// Store field values in arrays
	Int_t index = ix*fNy*fNz + iy*fNz + iz;
	fBx->AddAt(Float_t(bx), index);
	fBy->AddAt(Float_t(by), index);
	fBz->AddAt(Float_t(bz), index);

      }
    }
  }

  // Create new field map
  BmnFieldMap* fieldMap = new BmnFieldMap(this);

  // Write the new field map to the ROOT file
  fieldMap->WriteRootFile(outFileName.Data(), fMapName.Data());

  // Delete new field map
  delete fieldMap;

  cout << "-I- BmnFieldMapCreator: Field map " << fMapName
       << " was stored in " << outFileName << endl;
  return kTRUE;

}
