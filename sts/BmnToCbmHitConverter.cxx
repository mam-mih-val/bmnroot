
#include "BmnToCbmHitConverter.h"
#include <TStopwatch.h>

static Double_t workTime = 0.0;

// -----   Default constructor   ------------------------------------------
BmnToCbmHitConverter::BmnToCbmHitConverter()
    : FairTask("BMN to CBM Hits Converter", 1),
    fBmnGemHitsArray(nullptr),
    fBmnSilHitsArray(nullptr),
    fCbmHitsArray(nullptr),
    fBmnGemHitsBranchName("BmnGemStripHit"),
    fBmnSilHitsBranchName("BmnSiliconHit"),
    fCbmHitsBranchName("StsHit")
{}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
BmnToCbmHitConverter::BmnToCbmHitConverter(Int_t iVerbose)
    : FairTask("BMN to CBM Hits Converter", iVerbose),
    fBmnGemHitsArray(nullptr),
    fBmnSilHitsArray(nullptr),
    fCbmHitsArray(nullptr),
    fBmnGemHitsBranchName("BmnGemStripHit"),
    fBmnSilHitsBranchName("BmnSiliconHit"),
    fCbmHitsBranchName("StsHit")
{}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
BmnToCbmHitConverter::~BmnToCbmHitConverter() {
}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void BmnToCbmHitConverter::Exec(Option_t* opt) {

    TStopwatch sw;
    sw.Start();

    if (!IsActive())
        return;
    
    fCbmHitsArray->Delete();
    FairRootManager::Instance()->SetUseFairLinks(kTRUE);
    for (Int_t iHit = 0; iHit < fBmnGemHitsArray->GetEntriesFast(); ++iHit) {
        BmnGemStripHit* bmnHit = (BmnGemStripHit*)fBmnGemHitsArray->At(iHit);
        TVector3 pos;
        bmnHit->Position(pos);
        TVector3 dpos;
        bmnHit->PositionError(dpos);

        Int_t stat = bmnHit->GetStation();
        Int_t mod = bmnHit->GetModule();

        const Int_t nLayers = GemStationSet->GetStation(stat)->GetModule(mod)->GetNStripLayers();
        Bool_t isHitInside[nLayers];

        Int_t lay = 0;
        for (lay = 0; lay < nLayers; lay++) {
            BmnGemStripLayer layer = GemStationSet->GetStation(stat)->GetModule(mod)->GetStripLayer(lay);
            if (layer.IsPointInsideStripLayer(-bmnHit->GetX(), bmnHit->GetY())) break;
        }

        Int_t sect = -1;

        if (mod == 0) {
            if (lay < 2) sect = 1;
            else sect = 2;
        }
        else if (mod == 1) {
            if (lay < 2) sect = 3;
            else sect = 4;
        }
        else if (mod == 2) {
            if (lay < 2) sect = 5;
            else sect = 6;
        }
        else if (mod == 3) {
            if (lay < 2) sect = 7;
            else sect = 8;
        }

        Int_t sens = 1;
        Int_t detId = kGEM << 24 | (stat + 1 + 3) << 16 | sect << 4 | sens << 1;
        new ((*fCbmHitsArray)[fCbmHitsArray->GetEntriesFast()]) CbmStsHit(detId, pos, dpos, 0.0, 0, 0);
        CbmStsHit* hit = (CbmStsHit*)fCbmHitsArray->At(fCbmHitsArray->GetEntriesFast() - 1);
        hit->ResetLinks();
        hit->SetLinks(bmnHit->GetLinks());
        hit->SetRefIndex(bmnHit->GetRefIndex());
    }
    
    for (Int_t iHit = 0; iHit < fBmnSilHitsArray->GetEntriesFast(); ++iHit) {
        BmnSiliconHit* bmnHit = (BmnSiliconHit*)fBmnSilHitsArray->At(iHit);
        TVector3 pos;
        bmnHit->Position(pos);
        TVector3 dpos;
        bmnHit->PositionError(dpos);
        Int_t sens = 1;
        Int_t detId = kSILICON << 24 | (bmnHit->GetStation() + 1) << 16 | (bmnHit->GetModule() + 1) << 4 | sens << 1;
        new ((*fCbmHitsArray)[fCbmHitsArray->GetEntriesFast()]) CbmStsHit(detId, pos, dpos, 0.0, 0, 0);
        CbmStsHit* hit = (CbmStsHit*)fCbmHitsArray->At(fCbmHitsArray->GetEntriesFast() - 1);
        hit->ResetLinks();
        hit->SetLinks(bmnHit->GetLinks());
        hit->SetRefIndex(bmnHit->GetRefIndex());
    }
    FairRootManager::Instance()->SetUseFairLinks(kFALSE);

    sw.Stop();
    workTime += sw.RealTime();
}
// -------------------------------------------------------------------------

// -----   Private method Init   -------------------------------------------
InitStatus BmnToCbmHitConverter::Init() {

    // Get input array
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman) Fatal("Init", "No FairRootManager");
    fBmnGemHitsArray = (TClonesArray*)ioman->GetObject(fBmnGemHitsBranchName);
    fBmnSilHitsArray = (TClonesArray*)ioman->GetObject(fBmnSilHitsBranchName);
    if (!fBmnGemHitsArray) {
        cout << "BmnToCbmHitConverter::Init(): branch " << fBmnGemHitsBranchName << " not found! Task will be deactivated" << endl;
        SetActive(kFALSE);
        return kERROR;
    }
    if (!fBmnSilHitsArray) {
        cout << "BmnToCbmHitConverter::Init(): branch " << fBmnSilHitsBranchName << " not found! Task will be deactivated" << endl;
        SetActive(kFALSE);
        return kERROR;
    }

    // Register output array
    fCbmHitsArray = new TClonesArray("CbmStsHit");
    ioman->Register("StsHit", "STSHIT", fCbmHitsArray, kTRUE);

    TString gPathConfig = gSystem->Getenv("VMCWORKDIR");

    //Create GEM detector ------------------------------------------------------
    GemStationSet = new BmnGemStripStationSet(gPathConfig + "/parameters/gem/XMLConfigs/GemRun8.xml");
    SilStationSet = new BmnSiliconStationSet(gPathConfig + "/parameters/silicon/XMLConfigs/SiliconRun8_3stations.xml");

    return kSUCCESS;
}
// -------------------------------------------------------------------------


void BmnToCbmHitConverter::Finish() {
    printf("Work time of BmnToCbmHitConverter: %4.2f sec.\n", workTime);
}

ClassImp(BmnToCbmHitConverter)
