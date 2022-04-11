// -------------------------------------------------------------------------
// -----                   BmnBdDigitizer source file                  -----
// -----                  Created 11/04/22  by S. Merts                -----
// -------------------------------------------------------------------------

#include "BmnBdDigitizer.h"
#include "BmnBdPoint.h"
#include "BmnTrigDigit.h"

#include "FairRootManager.h"

#include "TClonesArray.h"
#include "TMath.h"

#include <iostream>

using namespace std;
using namespace TMath;

// -----   Default constructor   -------------------------------------------
BmnBdDigitizer::BmnBdDigitizer()
  : FairTask("Bmn BD Digitizer"),
  fPointArray(nullptr),
  fDigitArray(),
  fNMod(64) {
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
BmnBdDigitizer::~BmnBdDigitizer() {}
// -------------------------------------------------------------------------

// -----   Public method Init   --------------------------------------------
InitStatus BmnBdDigitizer::Init() {

  if (fVerbose) cout << "BmnBdDigitizer::Init() started\n ";

  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if (!ioman) {
    cout << "-E- BmnBdDigitizer::Init: " << "RootManager not instantiated!" << endl;
    return kFATAL;
  }

  // Get input array
  fPointArray = (TClonesArray*)ioman->GetObject("BdPoint");
  if (!fPointArray) {
    cout << "-W- BmnBdDigitizer::Init: " << "No BdPoint array!" << endl;
    return kERROR;
  }

  // Create and register output array
  fDigitArray = new TClonesArray("BmnTrigDigit");
  ioman->Register("BdDigit", "BD", fDigitArray, kTRUE);

  if (fVerbose) cout << "BmnBdDigitizer::Init() finished\n";
  return kSUCCESS;

}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
// -----   Public method Exec   --------------------------------------------
void BmnBdDigitizer::Exec(Option_t* opt) {

  if (fVerbose) cout << " BmnBdDigitizer::Exec() started\n";

  // Reset output array
  if (!fDigitArray) Fatal("Exec", "No BdDigit array");

  fDigitArray->Delete();

  //map for storage energies at modules
  map<Int_t, Double_t> ampMap;

  // Loop over MCPoints
  for (Int_t iPoint = 0; iPoint < fPointArray->GetEntriesFast(); iPoint++) {
    BmnBdPoint* point = (BmnBdPoint*)fPointArray->At(iPoint);
    if (!point) continue;

    Int_t mod = point->GetCopy() - 1;

    auto it = ampMap.find(mod);
    if (it != ampMap.end()) {
      ampMap[mod] += point->GetEnergyLoss();
    } else {
      ampMap[mod] = point->GetEnergyLoss();
    }
  }

  for (auto mit : ampMap) {
    BmnTrigDigit digi;
    digi.SetMod((Short_t)mit.first);
    digi.SetAmp(mit.second);
    new ((*fDigitArray)[fDigitArray->GetEntriesFast()]) BmnTrigDigit(digi);
  }

  if (fVerbose) {
    printf("   Number of BmnBdPoints = %d\n", fPointArray->GetEntriesFast());
    printf("   Number of BmnBdDigits = %d\n", fDigitArray->GetEntriesFast());
    cout << " BmnBdDigitizer::Exec() finished\n";
  }
}

// -------------------------------------------------------------------------

ClassImp(BmnBdDigitizer)