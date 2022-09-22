/* 
 * File:   BmnEcalDigitizer.cxx
 * Author: Petr Alekseev
 * 
 * Created on 05.08.2020, 19:31
 */

#include "BmnEcalDigitizer.h"
#include "BmnEcalPoint.h"

#include "FairLogger.h"
#include "TGeoManager.h"

#include <iostream>


BmnEcalDigitizer::BmnEcalDigitizer() {
}

BmnEcalDigitizer::~BmnEcalDigitizer() {
}

InitStatus BmnEcalDigitizer::Init() {

    FairRootManager* ioman = FairRootManager::Instance();
    fArrayOfEcalPoints = (TClonesArray*) ioman->GetObject("EcalPoint");
    if (fArrayOfEcalPoints == nullptr)
    {
        LOG(error)<<"BmnEcalDigitizer::Init() branch 'EcalPoint' not found! Task will be deactivated";
        SetActive(kFALSE);
        return kERROR;
    }

    fArrayOfEcalDigits = new TClonesArray("BmnECALDigit");
    ioman->Register("EcalDigit", "Ecal", fArrayOfEcalDigits, kTRUE);
    
    if (LoadGeometry() != 0)
    {
        SetActive(kFALSE);
        return kERROR;
    }
    
    Info(__func__,"ECAL digitizer ready");
    return kSUCCESS;
    
}

void BmnEcalDigitizer::Exec(Option_t* opt)
{
    if (!IsActive()) return;
    
    // Initialize
    fArrayOfEcalDigits->Delete();

    for (Int_t i = 0; i < fCellsSize; i++) {
        fCells[i].SetAmp(0.);
        fCells[i].SetStartTime(0.);
    }
    
    // Collect points
    Int_t N = fArrayOfEcalPoints->GetEntries();
    for (Int_t i = 0; i < N; i++) {
        BmnEcalPoint * p = (BmnEcalPoint *)fArrayOfEcalPoints->At(i);
        
        Int_t ch = p->GetCopyMother();
        if (ch < fCellsSize) {
            if (fCells[ch].GetChannel() == ch) {
                Float_t energyLoss = p->GetEnergyLoss();
                if (energyLoss > 0) {
                    Float_t pointTime = p->GetTime();
                    if (fMaxPointTime > 0 && pointTime > fMaxPointTime) continue;
                    if (fFiberSOL > 0.) {
                        pointTime += (fFiberLength - fLayerThickness * (p->GetCopy()+1))/fFiberSOL; 
                    }
                    fCells[ch].SetAmp(fCells[ch].GetAmp() + energyLoss);
                    fCells[ch].SetStartTime(fCells[ch].GetStartTime() + pointTime * energyLoss);
                }
            } else {
                Error(__func__, "ECAL ch %d was not initialized",ch);
            }   
        } else {
            Error(__func__,"ECAL ch %d ignored",ch);
        }
    }
    
    // Store digits
    for (Int_t i = 0; i < fCellsSize; i++) {
        if (fCells[i].GetChannel() != i) continue;
        
        Float_t amp = fCells[i].GetAmp() * 1000.;
        
        if (amp == 0.) continue;
        if (amp < fThreshold) continue;
        
        Float_t time = fCells[i].GetStartTime()/fCells[i].GetAmp();
        
        BmnECALDigit * p = new((*fArrayOfEcalDigits)[fArrayOfEcalDigits->GetEntriesFast()]) BmnECALDigit();
        *p = fCells[i];
        p->SetAmp(amp);
        p->SetPeakAmp(amp);
        p->SetStartTime(time);
        p->SetPeakTime(time + fPeakTimeDelay);
    }
    
}

void BmnEcalDigitizer::Print(Option_t *option) const {

    printf("\n" "\e[1;92m");
    printf("BMN ECAL Digitizer\n");
    printf("ECAL geometry fileN: %s\n", fEcalGeometryFileName);
    printf("ECAL interaction depth shift: %f\n", fDepthShift);    
    printf("ECAL cells coords (((\n");
    Float_t x, y, z;
    for (Int_t i = 1; i < fCellsSize; i++) {
        if (fCells[i].GetChannel() == i) {
            fCells[i].GetLabCoords(x,y,z);
            printf("    % 4d [%3.0f,%3.0f] -> [%7.2f,%7.2f,%7.2f]\n", i, fCells[i].GetX(), fCells[i].GetY(), x, y, z);
        } else {
            //printf("    % 04d - Not initialized\n", i);
        }
    }
    printf("))) ECAL cells coords\n"); 
    printf("\e[0m" "\n");
    
}

int BmnEcalDigitizer::LoadGeometry()
{
    Bool_t loadFromFile = fEcalGeometryFileName != nullptr;
    TGeoNode *ecal1 = 0, *ecal2 = 0;
    if (gGeoManager) {
        TGeoVolume * ecal = gGeoManager->FindVolumeFast("ecal");
        if (ecal) {
            if (ecal->GetNdaughters() > 0) ecal1 = ecal->GetNode(0);
            if (ecal->GetNdaughters() > 1) ecal2 = ecal->GetNode(1);
            loadFromFile = kFALSE;
        } else {
            Info(__func__, "Ecal geometry not found by TGeoManager\n");
        }
    } 
    
    if (loadFromFile) {
        Info(__func__, "Loading coordinates of ECAL cells from %s\n", fEcalGeometryFileName);
        TGeoVolume * top = TGeoVolume::Import(fEcalGeometryFileName,"TOP");

        if (!top) {
            LOG(error)<<"<BmnEcalDigitizer::LoadGeometry>: Volume TOP not found in "<<(fEcalGeometryFileName == nullptr? "(null)" : fEcalGeometryFileName);
            //Fatal(__func__, "Volume TOP not found in %s\n", fEcalGeometryFileName);
            return -1;
        }
        
        TGeoNode * ecal = top->GetNode(0);
    
        if (!ecal /*|| ecal->GetNdaughters() < 2*/) {
            LOG(error)<<"<BmnEcalDigitizer::LoadGeometry>: Unexpected geometry structure "<<(fEcalGeometryFileName == nullptr? "(null)" : fEcalGeometryFileName);
            //Fatal(__func__, "Unexpected geometry structure %s\n",fEcalGeometryFileName);
            return -1;
        }        
        ecal1 = ecal->GetDaughter(0);
        ecal2 = ecal->GetDaughter(1);
    }
    
    if (ecal1 == 0 && ecal2 == 0) {
        Info(__func__, "BmnEcalDigitizer was set in inactive state, because ECAL geometry was not found");
        return -1;
    }
    
    for (Int_t i = 0; i < fCellsSize; i++) {
        fCells[i].SetChannel(0);
    }

    Double_t coords[] = {0.,0.,0.};
    Double_t ecalCoords[3];
    Double_t labCoords[3];
    
    coords[2] = fDepthShift;
  
    if (ecal1) {
        Int_t n = ecal1->GetNdaughters();
        if (n > 504) {
            LOG(error)<<"<BmnEcalDigitizer::LoadGeometry>: Expected ecal node 1 with 504 daughters or less in "<<(fEcalGeometryFileName == nullptr? "(null)" : fEcalGeometryFileName);
            return -1;
        }
        for (Int_t i = 0; i < n; i++) {
            Int_t ch = ecal1->GetDaughter(i)->GetNumber();
            if (ch < 1 || ch > 504) {
                LOG(error)<<"<BmnEcalDigitizer::LoadGeometry>: Unexpected chan "<<ch<<" at ecal node 1 in "<<(fEcalGeometryFileName == nullptr? "(null)" : fEcalGeometryFileName);
                return -1;
            }
            ecal1->GetDaughter(i)->LocalToMaster(coords, ecalCoords);
            ecal1->LocalToMaster(ecalCoords,labCoords);
            fCells[ch].SetChannel(ch);
            fCells[ch].SetX(ecalCoords[0]);
            fCells[ch].SetY(ecalCoords[1]);
            fCells[ch].SetLabCoords(labCoords[0],labCoords[1],labCoords[2]);
        }
    }

    if (ecal2) {
        Int_t n = ecal2->GetNdaughters();
        if (n > 504) {
            LOG(error)<<"<BmnEcalDigitizer::LoadGeometry>: Expected ecal node 2 with 504 daughters or less in "<<(fEcalGeometryFileName == nullptr? "(null)" : fEcalGeometryFileName);
            return -1;
        }
        for (Int_t i = 0; i < n; i++) {
            Int_t ch = ecal2->GetDaughter(i)->GetNumber();
            if (ch < 505 || ch > 1008) {
                LOG(error)<<"<BmnEcalDigitizer::LoadGeometry>: Unexpected chan="<<ch<<" at ecal node 2 in "<<(fEcalGeometryFileName == nullptr? "(null)" : fEcalGeometryFileName);
                return -1;
            }
            ecal2->GetDaughter(i)->LocalToMaster(coords, ecalCoords);
            ecal2->LocalToMaster(ecalCoords,labCoords);
            fCells[ch].SetChannel(ch);
            fCells[ch].SetX(ecalCoords[0]);
            fCells[ch].SetY(ecalCoords[1]);
            fCells[ch].SetLabCoords(labCoords[0],labCoords[1],labCoords[2]);
        }
    }

    return 0;
}

ClassImp(BmnEcalDigitizer)
