/*
 * File:   BmnHodoDigitizer.cxx
 * Author: Sergey Morozov
 *
 * Created on 16.09.2021, 12:00
 */

#include <FairRun.h>
#include <FairRunSim.h>

#include "BmnHodoDigitizer.h"
#include "BmnHodoPoint.h"

BmnHodoDigitizer::BmnHodoDigitizer() {

    fGeV2MIP = 0.005;
    fMIP2Pix = 15.;
    fMIPNoise = 0.2;
    fThreshold = 0.;

}

BmnHodoDigitizer::~BmnHodoDigitizer() {

}

InitStatus BmnHodoDigitizer::Init() {

    FairRootManager* ioman = FairRootManager::Instance();
    fArrayOfHodoPoints = (TClonesArray*) ioman->GetObject("HodoPoint");
    if (!fArrayOfHodoPoints) {
        cout << "BmnHodoDigitizer::Init(): branch HodoPoint not found! Task will be deactivated" << endl;
        SetActive(kFALSE);
        return kERROR;
    }
    fArrayOfHodoDigits = new TClonesArray("BmnHodoDigit");
    ioman->Register("HodoDigit", "Hodo", fArrayOfHodoDigits, kTRUE);

    Info(__func__,"Hodo digitizer ready");
    return kSUCCESS;
}

void BmnHodoDigitizer::Exec(Option_t* opt) {

    if (!IsActive())
        return;
    
    // Initialize
    fArrayOfHodoDigits->Delete();

    float eSumStrip[17];

    for (Int_t i = 0; i < 17; i++) eSumStrip[i] = 0;


    // Collect points
    Int_t N = fArrayOfHodoPoints->GetEntries();

    for (Int_t i = 0; i < N; i++) {
        BmnHodoPoint * p = (BmnHodoPoint *)fArrayOfHodoPoints->At(i);

        Int_t iStrip = p->GetCopyMother();

        if (iStrip <= 16) {
            //collect and sum up energy losses for sections
            eSumStrip[iStrip] += p->GetEnergyLoss();
        } else {
            Error(__func__,"Hodo Strip %d ignored", iStrip);
        }
    }

    // Digitize SiPM response and store digits

    Double_t eSumHodo = 0.;
    Double_t eSumHodoMC = 0.;

    for (Int_t iStrip = 1; iStrip <= 16; iStrip++) {

      Double_t eSumGeant = eSumStrip[iStrip];
      Double_t eSumMIP = eSumGeant / fGeV2MIP; // convert energy to MIP
      Double_t eSumPix = eSumMIP * fMIP2Pix; // convert MIP to Npix in SiPM
      Double_t eSumMIPSmeared =
        gRandom->Gaus(eSumPix, sqrt(eSumPix)) / fMIP2Pix;
      Double_t eMIPNoise = gRandom->Gaus(0,fMIPNoise); // MIP electronic noise
      eSumMIPSmeared += eMIPNoise;
      Double_t eSumSmeared = eSumMIPSmeared * fGeV2MIP; //from MIP to energy

      Double_t amp = eSumSmeared * fScale;

      //if (amp == 0.) continue;

      if (amp < fThreshold) continue;

      eSumHodoMC += eSumGeant;
      eSumHodo += amp;

      BmnHodoDigit * p = new((*fArrayOfHodoDigits)[fArrayOfHodoDigits->GetEntriesFast()]) BmnHodoDigit();
      p->SetStripID(iStrip);
      p->SetELoss(eSumGeant);
      p->SetELossDigi(amp);

    } //for (Int_t iStrip

    //BmnHodoDigit * p = new((*fArrayOfHodoDigits)[fArrayOfHodoDigits->GetEntriesFast()]) BmnHodoDigit();
    //p->SetStripID(0);
    //p->SetELoss(eSumHodoMC);
    //p->SetELossDigi(eSumHodo);

}

ClassImp(BmnHodoDigitizer)
