/*
 * File:   BmnFHCalDigitizer.cxx
 * Author: Sergey Morozov
 *
 * Created on 16.09.2021, 12:00
 */

#include <FairRun.h>
#include <FairRunSim.h>

#include "BmnFHCalDigitizer.h"
#include "BmnFHCalPoint.h"

BmnFHCalDigitizer::BmnFHCalDigitizer() {
}

BmnFHCalDigitizer::~BmnFHCalDigitizer() {
}

InitStatus BmnFHCalDigitizer::Init() {

    fGeV2MIP = 0.005;
    fMIP2Pix = 15.;
    fMIPNoise = 0.2;

    FairRootManager* ioman = FairRootManager::Instance();
    fArrayOfFHCalPoints = (TClonesArray*)ioman->GetObject("FHCalPoint");
    if (!fArrayOfFHCalPoints) {
        cout << "BmnFHCalDigitizer::Init(): branch FHCalPoint not found! Task will be deactivated" << endl;
        SetActive(kFALSE);
        return kERROR;
    }
    fArrayOfFHCalDigits = new TClonesArray("BmnFHCalDigit");
    ioman->Register("FHCalDigit", "FHCal", fArrayOfFHCalDigits, kTRUE);

    Info(__func__,"FHCal digitizer ready");
    return kSUCCESS;
}

void BmnFHCalDigitizer::Exec(Option_t* opt) {
    
    if (!IsActive())
        return;
    
    // Initialize
    fArrayOfFHCalDigits->Delete();

    float eSumModSect[55][11];

    for (Int_t i = 0; i < 55; i++) {
        for (Int_t j = 0; j < 11; j++) eSumModSect[i][j] = 0;
    }


    // Collect points
    Int_t N = fArrayOfFHCalPoints->GetEntriesFast();
    for (Int_t i = 0; i < N; i++) {
        BmnFHCalPoint * p = (BmnFHCalPoint *)fArrayOfFHCalPoints->At(i);

        Int_t iModule = p->GetCopyMother();
        Int_t iChannel = p->GetCopy();

        Int_t iSect = (iChannel / 6) + 1; //calculate section number

        if (iModule <= 54) {
            //collect and sum up energy losses for sections
            eSumModSect[iModule][iSect] += p->GetEnergyLoss();
        } else {
            Error(__func__,"FHCal module %d ignored", iModule);
        }
    }

    // Digitize SiPM response and store digits

    Double_t eSumFHCal = 0.;
    Double_t eSumFHCalMC = 0.;

    for (Int_t iModule = 1; iModule <= 54; iModule++) {

      Double_t eSumModule = 0.;
      Double_t eSumModuleMC = 0.;

      Int_t nSect = 10;
      if (iModule <= 34) nSect = 7;

      for (Int_t iSect = 1; iSect <= nSect; iSect++) {

        Double_t eSumGeant = eSumModSect[iModule][iSect];
        Double_t eSumMIP = eSumGeant / fGeV2MIP; // convert energy to MIP
        Double_t eSumPix = eSumMIP * fMIP2Pix; // convert MIP to Npix in SiPM
        Double_t eSumMIPSmeared =
          gRandom->Gaus(eSumPix, sqrt(eSumPix)) / fMIP2Pix;
        Double_t eMIPNoise = gRandom->Gaus(0,fMIPNoise); // MIP electronic noise
        eSumMIPSmeared += eMIPNoise;
        Double_t eSumSmeared = eSumMIPSmeared * fGeV2MIP; //from MIP to energy

        Double_t amp = eSumSmeared * fScale;

        if (amp == 0.) continue;

        if (iModule <= 34) {
            if (amp < fSmallModThreshold) continue;
        } else {
            if (amp < fLargeModThreshold) continue;
        }

        eSumModuleMC += eSumGeant;
        eSumFHCalMC += eSumGeant;
        eSumModule += amp;
        eSumFHCal += amp;

        BmnFHCalDigit * p = new((*fArrayOfFHCalDigits)[fArrayOfFHCalDigits->GetEntriesFast()]) BmnFHCalDigit();
        p->SetModuleID(iModule);
        p->SetSectionID(iSect);
        p->SetELoss(eSumGeant);
        p->SetELossDigi(amp);

      } //for (Int_t iSect

      BmnFHCalDigit * p = new((*fArrayOfFHCalDigits)[fArrayOfFHCalDigits->GetEntriesFast()]) BmnFHCalDigit();
      p->SetModuleID(iModule);
      p->SetSectionID(0);
      p->SetELoss(eSumModuleMC);
      p->SetELossDigi(eSumModule);

    } //for (Int_t iModule

    BmnFHCalDigit * p = new((*fArrayOfFHCalDigits)[fArrayOfFHCalDigits->GetEntriesFast()]) BmnFHCalDigit();
    p->SetModuleID(0);
    p->SetSectionID(0);
    p->SetELoss(eSumFHCalMC);
    p->SetELossDigi(eSumFHCalMC);

}

ClassImp(BmnFHCalDigitizer)
