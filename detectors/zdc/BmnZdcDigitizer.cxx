/* 
 * File:   BmnZdcDigitizer.cxx
 * Author: Petr Alekseev
 * 
 * Created on 29.05.2020, 18:00
 */

#include "BmnZdcDigitizer.h"
#include "BmnZdcPoint.h"

#include <FairRun.h>
#include <FairRunSim.h>
#include "FairLogger.h"

BmnZdcDigitizer::BmnZdcDigitizer() {
    fArrayOfZdcPoints = nullptr;
    fArrayOfZdcDigits = nullptr;
}

BmnZdcDigitizer::~BmnZdcDigitizer() {
}

InitStatus BmnZdcDigitizer::Init() {

    FairRootManager* ioman = FairRootManager::Instance();
    fArrayOfZdcPoints = (TClonesArray*) ioman->GetObject("ZdcPoint");
    if (fArrayOfZdcPoints == nullptr)
    {
        LOG(error)<<"BmnZdcDigitizer::Init() branch 'ZdcPoint' not found! Task will be deactivated";
        SetActive(kFALSE);
        return kERROR;
    }

    fArrayOfZdcDigits = new TClonesArray("BmnZDCDigit");
    ioman->Register("ZdcDigit", "Zdc", fArrayOfZdcDigits, kTRUE);
    
    LoadMap("ZDC_map_period_5.txt");
    
    Info(__func__,"ZDC digitizer ready");
    return kSUCCESS;
}

void BmnZdcDigitizer::Exec(Option_t* opt) {
    if (!IsActive())
      return;

    // Reset arrays
    fArrayOfZdcDigits->Delete();
    for (Int_t i = 0; i < 106; i++) {
        fModules[i].SetAmp(0.);
    }
    
    // Collect points
    Int_t N = fArrayOfZdcPoints->GetEntries();
    for (Int_t i = 0; i < N; i++) {
        BmnZdcPoint * p = (BmnZdcPoint *)fArrayOfZdcPoints->At(i);
        
        Int_t ch = p->GetCopyMother();
        if (ch < 106) {
            fModules[ch].SetAmp(fModules[ch].GetAmp() + p->GetEnergyLoss());
        } else {
            Error(__func__,"ZDC module %d ignored",ch);
        }
    }
    
    // Store digits
    for (Int_t i = 1; i < 105; i++) {
        Double_t amp = fModules[i].GetAmp() * fScale;
        
        if (amp == 0.) continue;
        if (i < 69) {
            if (amp < fLargeModThreshold) continue;
        } else {
            if (amp < fSmallModThreshold) continue;
        }
        
        BmnZDCDigit * p = new((*fArrayOfZdcDigits)[fArrayOfZdcDigits->GetEntriesFast()]) BmnZDCDigit();
        *p = fModules[i];
        p->SetAmp(amp);
    }
    
    if (fModules[105].GetAmp() > 0) {
        BmnZDCDigit * p = new((*fArrayOfZdcDigits)[fArrayOfZdcDigits->GetEntriesFast()]) BmnZDCDigit();
        p->SetChannel(111);
        p->SetAmp(fModules[105].GetAmp());
    }
}

void BmnZdcDigitizer::LoadMap(const char * fileName) {
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
        if (front_chan > 0 && front_chan < 105) {
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

ClassImp(BmnZdcDigitizer)
