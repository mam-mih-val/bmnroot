
#include "Cbm2BmnHitConverter.h"

Cbm2BmnHitConverter::Cbm2BmnHitConverter() {
    ioman = NULL;
    fEventNo = 0;
    fInputStsHitsBranchName = "StsHit";
    fOutputGemHitsBranchName = "BmnGemStripHit";
    fOutputSilHitsBranchName = "BmnSiliconHit";
    fBmnGemStripHitsArray = NULL;
    fBmnSiliconHitsArray = NULL;
    fVerbose = 1;

    isGem = kTRUE;
    isSil = kTRUE;
}

InitStatus Cbm2BmnHitConverter::Init() {
    if (fVerbose) cout << "\nCbm2BmnHitConverter::Init()\n ";

    ioman = FairRootManager::Instance();

    fCbmStsHitsArray = (TClonesArray*) ioman->GetObject(fInputStsHitsBranchName);
    if (!fCbmStsHitsArray) {
        cout << "Cbm2BmnHitConverter::Init(): branch " << fInputStsHitsBranchName << " not found! Task will be deactivated" << endl;
        SetActive(kFALSE);
        return kERROR;
    }

    fBmnGemStripHitsArray = new TClonesArray(BmnGemStripHit::Class());
    ioman->Register(fOutputGemHitsBranchName, "GEM", fBmnGemStripHitsArray, kTRUE);
    fBmnSiliconHitsArray = new TClonesArray(BmnSiliconHit::Class());
    ioman->Register(fOutputSilHitsBranchName, "SILICON", fBmnSiliconHitsArray, kTRUE);

    if (fVerbose) cout << "Cbm2BmnHitConverter::Init() finished\n";
}

/* Warning! Valid only for period 7 */
void Cbm2BmnHitConverter::Exec(Option_t* opt) {
    fBmnGemStripHitsArray->Delete();
    fBmnSiliconHitsArray->Delete();

    TVector3 shift(0.7, -5.77, -1.79);
    Int_t gemCount = 0;
    Int_t silCount = 0;

    for (UInt_t ihit = 0; ihit < fCbmStsHitsArray->GetEntriesFast(); ihit++) {
        CbmStsHit* hit = (CbmStsHit*) fCbmStsHitsArray->UncheckedAt(ihit);
        Int_t stat = hit->GetStationNr();
        Int_t sector = hit->GetSectorNr();
        Int_t mod = 0;
        if (stat >= 4 && stat <= 9) {
            if (isGem) {
                stat -= 4;
                mod = (hit->GetX() > 0) ? 0 : 1;
                BmnGemStripHit* gem = new((*fBmnGemStripHitsArray)[fBmnGemStripHitsArray->GetEntriesFast()]) BmnGemStripHit(
                        kGEM,
                        TVector3(hit->GetX() + shift.X(), hit->GetY() + shift.Y(), hit->GetZ() + shift.Z()),
                        TVector3(hit->GetDx(), hit->GetDy(), hit->GetDz()),
                        gemCount++);
                gem->SetStation(stat);
            }

        } else
            if (stat > 0 && stat <= 3) {
            if (isSil) {
                stat--;
                mod = sector - 1;
                BmnSiliconHit* sil = new((*fBmnSiliconHitsArray)[fBmnSiliconHitsArray->GetEntriesFast()]) BmnSiliconHit(
                        kSILICON,
                        TVector3(hit->GetX() + shift.X(), hit->GetY() + shift.Y(), hit->GetZ() + shift.Z()),
                        TVector3(hit->GetDx(), hit->GetDy(), hit->GetDz()),
                        silCount++);
                sil->SetStation(stat);
                sil->SetModule(mod);
            }
        }
    }

}

void Cbm2BmnHitConverter::Finish() {
}

Cbm2BmnHitConverter::~Cbm2BmnHitConverter() {
}


ClassImp(Cbm2BmnHitConverter);