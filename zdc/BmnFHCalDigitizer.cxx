/*
 * File:   BmnFHCalDigitizer.cxx
 * Author: Sergey Morozov
 *
 * Created on 16.09.2021, 12:00
 */

#include <FairRun.h>
#include <FairRunSim.h>

#include "BmnFHCalDigitizer.h"
#include "BmnZdcPoint.h"

BmnFHCalDigitizer::BmnFHCalDigitizer() {
}

BmnFHCalDigitizer::~BmnFHCalDigitizer() {
}

InitStatus BmnFHCalDigitizer::Init() {

    fGeV2MIP = 0.005;
    fMIP2Pix = 15.;
    fMIPNoise = 0.2;

    FairRootManager* ioman = FairRootManager::Instance();
    fArrayOfZdcPoints = (TClonesArray*) ioman->GetObject("ZdcPoint");
    fArrayOfZdcDigits = new TClonesArray("BmnZDCDigit");
    ioman->Register("ZDC", "Zdc", fArrayOfZdcDigits, kTRUE);

    LoadMap("FHCal_map_v1.txt");

    Info(__func__,"ZDC digitizer ready");
    return kSUCCESS;
}

void BmnFHCalDigitizer::Exec(Option_t* opt) {

    // Initialize
    fArrayOfZdcDigits->Delete();

    float eSumModSect[55][11];

    for (Int_t i = 0; i < 55; i++) {
        fModules[i].SetAmp(0.);
        for (Int_t j = 0; j < 11; j++) eSumModSect[i][j] = 0;
    }


    // Collect points
    Int_t N = fArrayOfZdcPoints->GetEntries();
    for (Int_t i = 0; i < N; i++) {
        BmnZdcPoint * p = (BmnZdcPoint *)fArrayOfZdcPoints->At(i);

        Int_t iModule = p->GetCopyMother();
        Int_t iChannel = p->GetCopy();

        Int_t iSect = (iChannel / 6) + 1; //calculate section number

        if (iModule <= 54) {
            //collect and sum up energy losses for sections
            eSumModSect[iModule][iSect] += p->GetEnergyLoss();
            //fModules[ch].SetAmp(fModules[ch].GetAmp() + p->GetEnergyLoss());
        } else {
            Error(__func__,"ZDC module %d ignored", iModule);
        }
    }

    // Digitize SiPM response and store digits

    Double_t eSumFHCal = 0.;

    for (Int_t iModule = 1; iModule <= 54; iModule++) {

      Double_t eSumModule = 0.;

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

        eSumModule += amp;
        eSumFHCal += amp;

      } //for (Int_t iSect

      BmnZDCDigit * p = new((*fArrayOfZdcDigits)[fArrayOfZdcDigits->GetEntriesFast()]) BmnZDCDigit();
      *p = fModules[iModule];
      p->SetAmp(eSumModule);

    } //for (Int_t iModule

    BmnZDCDigit * p = new((*fArrayOfZdcDigits)[fArrayOfZdcDigits->GetEntriesFast()]) BmnZDCDigit();
    *p = fModules[0];
    p->SetAmp(eSumFHCal);

}

void BmnFHCalDigitizer::LoadMap(const char * fileName) {
    ifstream in;

    TString path = getenv("VMCWORKDIR");
    path += "/input/";
    path += fileName;
    in.open(path.Data());
    if (!in.is_open())
    {
        Fatal(__func__, "Loading ZDC Map from file: %s - file open error!\n",fileName);
	return;
    }
    Info(__func__, "Loading ZDC Map from file: %s\n", fileName);

    TString dummy;
    in >> dummy >> dummy >> dummy >> dummy >> dummy >> dummy >> dummy >> dummy >> dummy >> dummy;

    int ixmin = -1, ixmax = -1, iymin = -1, iymax = -1;
    int xmin = 10000., xmax = -10000., ymin = 10000., ymax = -10000.;
    while (!in.eof()) {
        int id,chan,front_chan,size,ix,iy,used;
	float x,y;
        in >> std::hex >> id >> std::dec >> chan >> front_chan >> size >> ix >> iy >> x >> y >> used;
        if (!in.good()) break;
	//printf("%0x %d %d %d %d %d %f %f\n",id,chan,front_chan,size,ix,iy,x,y);
        if (front_chan > 0 && front_chan < 55) {
            fModules[front_chan].SetChannel(front_chan);
            fModules[front_chan].SetSize(size);
            fModules[front_chan].SetIX(ix);
            fModules[front_chan].SetIY(iy);
            fModules[front_chan].SetX(x);
            fModules[front_chan].SetY(y);
        }
    }
    in.close();
}

ClassImp(BmnFHCalDigitizer)
