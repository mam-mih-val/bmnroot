
#include "BmnToCbmHitConverter.h"
#include <TStopwatch.h>

static Double_t workTime = 0.0;

// -----   Default constructor   ------------------------------------------
BmnToCbmHitConverter::BmnToCbmHitConverter()
    : FairTask("BMN to CBM Hits Converter", 1),
    fBmnGemHitsArray(nullptr),
    fBmnGemLowerClusters(nullptr),
    fBmnGemUpperClusters(nullptr),
    fBmnSilHitsArray(nullptr),
    fBmnSilLowerClusters(nullptr),
    fBmnSilUpperClusters(nullptr),
    fCbmHitsArray(nullptr),
    fUseFixedErrors(kFALSE),
    fBmnGemHitsBranchName("BmnGemStripHit"),
    fBmnSilHitsBranchName("BmnSiliconHit"),
    fCbmHitsBranchName("StsHit"),
    fGemConfigFile("GemRun8.xml"),
    fSilConfigFile("SiliconRun8_4stations.xml") {}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
BmnToCbmHitConverter::BmnToCbmHitConverter(Int_t iVerbose)
    : FairTask("BMN to CBM Hits Converter", iVerbose),
    fBmnGemHitsArray(nullptr),
    fBmnGemLowerClusters(nullptr),
    fBmnGemUpperClusters(nullptr),
    fBmnSilHitsArray(nullptr),
    fBmnSilLowerClusters(nullptr),
    fBmnSilUpperClusters(nullptr),
    fCbmHitsArray(nullptr),
    fUseFixedErrors(kFALSE),
    fBmnGemHitsBranchName("BmnGemStripHit"),
    fBmnSilHitsBranchName("BmnSiliconHit"),
    fCbmHitsBranchName("StsHit"),
    fGemConfigFile("GemRun8.xml"),
    fSilConfigFile("SiliconRun8_4stations.xml") {}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
BmnToCbmHitConverter::~BmnToCbmHitConverter() {}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void BmnToCbmHitConverter::Exec(Option_t* opt) {

    TStopwatch sw;
    sw.Start();

    if (!IsActive())
        return;

    fCbmHitsArray->Delete();
    for (Int_t iHit = 0; iHit < fBmnGemHitsArray->GetEntriesFast(); ++iHit) {
        BmnGemStripHit* bmnHit = (BmnGemStripHit*)fBmnGemHitsArray->At(iHit);

        //if (bmnHit->GetRefIndex() == -1) continue;

        //Section for hit filtration by signal asymmetry
        // StripCluster* uc = (StripCluster*)fBmnGemUpperClusters->At(bmnHit->GetUpperClusterIndex());
        // StripCluster* lc = (StripCluster*)fBmnGemLowerClusters->At(bmnHit->GetLowerClusterIndex());
        // Float_t ls = lc->TotalSignal;
        // Float_t us = uc->TotalSignal;
        // if (us < ls - 1000 || us > ls + 1000) continue;
        // if (Abs((us - ls) / (us + ls)) > 0.75) //asymmetry
        //     continue;

        TVector3 pos;
        bmnHit->Position(pos);
        TVector3 dpos;
        if (fUseFixedErrors) {
            //dpos[0] = 0.08/TMath::Sqrt(12); //AZ
            //dpos[1] = 0.1234; //AZ
            dpos[0] = fDXgem; //0.015; //AZ - as in cbmroot
            dpos[1] = fDYgem; //0.058; //AZ - as in cbmroot
        } else {
            bmnHit->PositionError(dpos);
        }

        Int_t stat = bmnHit->GetStation();
        Int_t mod = bmnHit->GetModule();
        //ElectronDriftDirectionInModule driftDir = 
        //GemStationSet->GetStation(stat)->GetModule(mod)->GetElectronDriftDirection(); //AZ-200322
        //if (driftDir == BackwardZAxisEDrift) pos[2] -= 0.9; //AZ-200322
        //if (driftDir == ForwardZAxisEDrift) pos[2] -= 0.9; //AZ-200322

        Int_t lay = 0;
        for (lay = 0; lay < GemStationSet->GetStation(stat)->GetModule(mod)->GetNStripLayers(); lay++) {
            BmnGemStripLayer* layer = &(GemStationSet->GetStation(stat)->GetModule(mod)->GetStripLayer(lay));
            if (layer->IsPointInsideStripLayer(-bmnHit->GetX(), bmnHit->GetY())) break;
        }
        if (lay == GemStationSet->GetStation(stat)->GetModule(mod)->GetNStripLayers()) continue; //AZ-230322 - strange case //CHECK IT!!!

            //formula for converting from the bm@n system of modules and layers to the cbm one
        Int_t sect = 2 * mod + 1 + lay / 2;

        Int_t sens = 1;

        Int_t nSilStations = SilStationSet->GetNStations();
        Int_t detId = kGEM << 24 | (stat + 1 + nSilStations) << 16 | sect << 4 | sens << 1;

        new ((*fCbmHitsArray)[fCbmHitsArray->GetEntriesFast()]) CbmStsHit(detId, pos, dpos, 0.0, 0, 0);
        CbmStsHit* hit = (CbmStsHit*)fCbmHitsArray->At(fCbmHitsArray->GetEntriesFast() - 1);

        FairRootManager::Instance()->SetUseFairLinks(kTRUE);
        hit->ResetLinks();
        hit->SetLinks(bmnHit->GetLinks());
        FairRootManager::Instance()->SetUseFairLinks(kFALSE);
        hit->SetRefIndex(bmnHit->GetRefIndex());
        hit->fDigiF = bmnHit->GetLowerClusterIndex() + 0; //AZ-250322
        hit->fDigiB = bmnHit->GetUpperClusterIndex() + 1000000; //AZ-250322
    }

    for (Int_t iHit = 0; iHit < fBmnSilHitsArray->GetEntriesFast(); ++iHit) {
        BmnSiliconHit* bmnHit = (BmnSiliconHit*)fBmnSilHitsArray->At(iHit);
        //if (bmnHit->GetRefIndex() == -1) continue;
        //Section for hit filtration by signal asymmetry
        // StripCluster* uc = (StripCluster*)fBmnSilUpperClusters->At(bmnHit->GetUpperClusterIndex());
        // StripCluster* lc = (StripCluster*)fBmnSilLowerClusters->At(bmnHit->GetLowerClusterIndex());
        // Float_t ls = lc->TotalSignal;
        // Float_t us = uc->TotalSignal;
        // if (us < ls - 1000 || us > ls + 1000) continue;
        // if (Abs((us - ls) / (us + ls)) > 0.75) //asymmetry
        //     continue;

        TVector3 pos;
        bmnHit->Position(pos);
        pos[2] -= 0.0150; //AZ - shift to the entrance
        TVector3 dpos;
        if (fUseFixedErrors) {
            dpos[0] = fDXsil; //0.01 / TMath::Sqrt(12); //AZ
            //dpos[1] = 0.1234; //AZ
            dpos[1] = fDYsil; //0.021; //AZ - as in cbmroot
        } else {
            bmnHit->PositionError(dpos);
        }
        
        Int_t sens = 1;
        Int_t detId = kSILICON << 24 | (bmnHit->GetStation() + 1) << 16 | (bmnHit->GetModule() + 1) << 4 | sens << 1;
        new ((*fCbmHitsArray)[fCbmHitsArray->GetEntriesFast()]) CbmStsHit(detId, pos, dpos, 0.0, 0, 0);
        CbmStsHit* hit = (CbmStsHit*)fCbmHitsArray->At(fCbmHitsArray->GetEntriesFast() - 1);

        FairRootManager::Instance()->SetUseFairLinks(kTRUE);
        hit->ResetLinks();
        hit->SetLinks(bmnHit->GetLinks());
        FairRootManager::Instance()->SetUseFairLinks(kFALSE);
        hit->SetRefIndex(bmnHit->GetRefIndex());
        hit->fDigiF = bmnHit->GetLowerClusterIndex() + 2000000; //!!!AZ - for VectorFinder (to have unique indices) - 260322
        hit->fDigiB = bmnHit->GetUpperClusterIndex() + 3000000; //!!!AZ - for VectorFinder (to have unique indices) - 260322
    }

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

    fBmnGemLowerClusters = (TClonesArray*)ioman->GetObject("BmnGemLowerCluster");
    fBmnGemUpperClusters = (TClonesArray*)ioman->GetObject("BmnGemUpperCluster");
    fBmnSilLowerClusters = (TClonesArray*)ioman->GetObject("BmnSiliconLowerCluster");
    fBmnSilUpperClusters = (TClonesArray*)ioman->GetObject("BmnSiliconUpperCluster");
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
    GemStationSet = new BmnGemStripStationSet(gPathConfig + "/parameters/gem/XMLConfigs/" + fGemConfigFile);
    SilStationSet = new BmnSiliconStationSet(gPathConfig + "/parameters/silicon/XMLConfigs/" + fSilConfigFile);

    return kSUCCESS;
}
// -------------------------------------------------------------------------

void BmnToCbmHitConverter::SetFixedErrors(Float_t dXgem, Float_t dYgem, Float_t dXsil, Float_t dYsil) {
    fUseFixedErrors = kTRUE;
    fDXgem = dXgem;
    fDYgem = dYgem;
    fDXsil = dXsil;
    fDYsil = dYsil;
}

void BmnToCbmHitConverter::Finish() {
    printf("Work time of BmnToCbmHitConverter: %4.2f sec.\n", workTime);
}


ClassImp(BmnToCbmHitConverter)
