// -------------------------------------------------------------------------
// -----                CbmPsdIdealDigitizer source file             -----
// -----                  Created 15/05/12  by     Alla                -----
// -------------------------------------------------------------------------
#include <iostream>

#include "TClonesArray.h"

#include "FairRootManager.h"

#include "CbmPsdDigi.h"
#include "CbmPsdIdealDigitizer.h"
#include "CbmPsdPoint.h"

using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
CbmPsdIdealDigitizer::CbmPsdIdealDigitizer() :
  FairTask("Ideal Psd Digitizer",1),
  fPointArray(NULL),
  fDigiArray(NULL),
  fNDigis(0)
{ 
  //  Reset();
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmPsdIdealDigitizer::~CbmPsdIdealDigitizer() 
{
  if ( fDigiArray ) {
    fDigiArray->Delete();
    delete fDigiArray;
  }
}
// -------------------------------------------------------------------------



// -----   Public method Init   --------------------------------------------
InitStatus CbmPsdIdealDigitizer::Init() {

  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
    cout << "-E- CbmPsdIdealDigitizer::Init: "
	 << "RootManager not instantised!" << endl;
    return kFATAL;
  }

  // Get input array
  fPointArray = (TClonesArray*) ioman->GetObject("PsdPoint");
  if ( ! fPointArray ) {
    cout << "-W- CbmPsdIdealDigitizer::Init: "
	 << "No PSDPoint array!" << endl;
    return kERROR;
  }

  // Create and register output array
  fDigiArray = new TClonesArray("CbmPsdDigi", 1000);
  ioman->Register("PsdDigi", "PSD", fDigiArray, kTRUE);

  cout << "-I- CbmPsdIdealDigitizer: Intialisation successfull " << kSUCCESS<< endl;
  return kSUCCESS;

}


// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void CbmPsdIdealDigitizer::Exec(Option_t* opt) {

//  cout<<" CbmPsdIdealDigitizer::Exec begin "<<endl;
  // Reset output array
   Reset();
   if ( ! fDigiArray ) Fatal("Exec", "No PsdDigi array");

   
  // Declare some variables
  CbmPsdPoint* point = NULL;
  Int_t modID   = -1;        // module ID
  Int_t scinID = -1;        // #sciillator
  Double_t x, y, z;         // Position
  Float_t edep[66][104];
  Float_t edepTOT=0.;
  TVector3 pos;       // Position vector
  fNDigis=0;
  for (Int_t imod=0; imod<104; imod++) {
    for (Int_t isec=0; isec<66; isec++) edep[isec][imod]=0;
  }  
  // Loop over PsdPoints
  Int_t nPoints = fPointArray->GetEntriesFast();
//  cout<<" nPoints "<<nPoints<<endl;
  for (Int_t iPoint=0; iPoint<nPoints; iPoint++) {
    point = (CbmPsdPoint*) fPointArray->At(iPoint);
    if ( ! point) continue;

    // Detector ID
    scinID = point->GetDetectorID();
    modID = point->GetModuleID();
    if (scinID < 0 || scinID >= 66)
    {
	cout << "PSD Digitizer : Wrong scintillator number : " << scinID << endl;
	continue;
    }
    if (modID < 0 || modID >= 104)
    {
	cout << "PSD Digitizer : Wrong module number : " << modID << endl;
	continue;
    }
    edep[scinID][modID] += point->GetEnergyLoss();
    edepTOT += point->GetEnergyLoss();
//    cout << "PSD Digitizer : Point number : " << iPoint << ", Module number : " << modID << ", Plate number : " << scinID << ", Edep : " << point->GetEnergyLoss() << endl;
  }// Loop over MCPoints
//  cout << "PSD Digitizer : Edep total with points : " << edepTOT << endl;

  for (Int_t imod=0; imod<104; imod++) {
    for (Int_t isec=0; isec<66; isec++) {
      if (edep[isec][imod]>0) {
	new ((*fDigiArray)[fNDigis]) CbmPsdDigi(isec, imod, edep[isec][imod]);
	fNDigis++;
      }
    }   // section
  }//module
  
  // Event summary
//  cout << "-I- CbmPsdIdealDigitizer: " <<fNDigis<< " CbmPsdDigi created." << endl;

}
// -------------------------------------------------------------------------

// -----   Private method Reset   ------------------------------------------
void CbmPsdIdealDigitizer::Reset() {
 fNDigis = 0;
 if ( fDigiArray ) fDigiArray->Delete();

}


ClassImp(CbmPsdIdealDigitizer)
