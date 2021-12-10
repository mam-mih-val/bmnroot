// -------------------------------------------------------------------------
// -----                   BmnSiMDDigitizer source file                -----
// -----                  Created 23/12/20  by I. Kozlov (SPSU)        -----
// -------------------------------------------------------------------------

#include "BmnSiMDDigitizer.h"
#include "BmnSiMDPoint.h"
#include "BmnTrigDigit.h"

#include "FairRootManager.h"

#include "TClonesArray.h"
#include "TMath.h"

#include <iostream>

using namespace std;
using namespace TMath;

// -----   Default constructor   -------------------------------------------
BmnSiMDDigitizer::BmnSiMDDigitizer()
  : FairTask("Bmn SiMD Digitizer"),
  fPointArray(nullptr),
  fDigitArray(),
  fNMod(64) {
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
BmnSiMDDigitizer::~BmnSiMDDigitizer() {}
// -------------------------------------------------------------------------

// -----   Public method Init   --------------------------------------------
InitStatus BmnSiMDDigitizer::Init() {

  if (fVerbose) cout << "BmnSiMDDigitizer::Init() started\n ";

  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if (!ioman) {
    cout << "-E- BmnSiMDDigitizer::Init: " << "RootManager not instantiated!" << endl;
    return kFATAL;
  }

  // Get input array
  fPointArray = (TClonesArray*)ioman->GetObject("SiMDPoint");
  if (!fPointArray) {
    cout << "-W- BmnSiMDDigitizer::Init: " << "No SiMDPoint array!" << endl;
    return kERROR;
  }

  // Create and register output array
  fDigitArray = new TClonesArray("BmnTrigDigit");
  ioman->Register("SiMDDigit", "SiMD", fDigitArray, kTRUE);

  fModsAngles = new Double_t[fNMod];
  for (Int_t i = 0; i < fNMod; i++) {
    fModsAngles[i] = i * 5.625;
  }

  if (fVerbose) cout << "BmnSiMDDigitizer::Init() finished\n";
  return kSUCCESS;

}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
// -----   Public method Exec   --------------------------------------------
void BmnSiMDDigitizer::Exec(Option_t* opt) {

  if (fVerbose) cout << " BmnSiMDDigitizer::Exec() started\n";

  // Reset output array
  if (!fDigitArray) Fatal("Exec", "No SiMDDigit array");

  fDigitArray->Delete();

  //map for storage energies at modules
  map<Int_t, Double_t> ampMap;

  // Loop over MCPoints
  for (Int_t iPoint = 0; iPoint < fPointArray->GetEntriesFast(); iPoint++) {
    BmnSiMDPoint* point = (BmnSiMDPoint*)fPointArray->At(iPoint);
    if (!point) continue;

    Int_t mod = ModByPoint(point->GetXOut(), point->GetYOut());

    auto it = ampMap.find(mod);
    if (it != ampMap.end()) {
      ampMap[mod] += point->GetEnergyLoss();
    } else {
      ampMap[mod] = point->GetEnergyLoss();
    }
  }

  //for (map<Int_t, Double_t>::iterator mit = ampMap.begin(); mit != ampMap.end(); ++mit) {
  for (auto mit : ampMap) {
    BmnTrigDigit digi;
    digi.SetMod((Short_t)mit.first);
    digi.SetAmp(mit.second);
    new ((*fDigitArray)[fDigitArray->GetEntriesFast()]) BmnTrigDigit(digi);
  }

  if (fVerbose) {
    printf("   Number of BmnSiMDPoints = %d\n", fPointArray->GetEntriesFast());
    printf("   Number of BmnSiMDDigits = %d\n", fDigitArray->GetEntriesFast());
    cout << " BmnSiMDDigitizer::Exec() finished\n";
  }
}

Int_t BmnSiMDDigitizer::ModByPoint(Double_t x, Double_t y) {

  Double_t XPOS = 0.5;
  Double_t YPOS = -4.6;

  Double_t newX = x;// -XPOS;
  Double_t newY = y;// -YPOS;
  // Double_t pAngle = GetPointAngle(newX, newY);
  Double_t pAngle = TVector2(newX, newY).Phi() * RadToDeg();
  for (Int_t i = 0; i < fNMod; i++) {
    if (pAngle >= fModsAngles[i] && pAngle <= (fModsAngles[i] + 5.6))
      return i;
  }
  return -1;
}

// -------------------------------------------------------------------------

ClassImp(BmnSiMDDigitizer)