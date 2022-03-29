
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
    for (Int_t iHit = 0; iHit < fBmnGemHitsArray->GetEntriesFast(); ++iHit) {
        BmnGemStripHit* bmnHit = (BmnGemStripHit*)fBmnGemHitsArray->At(iHit);
        TVector3 pos;
        bmnHit->Position(pos);
        TVector3 dpos;
        bmnHit->PositionError(dpos);
	//dpos[0] = 0.08/TMath::Sqrt(12); //AZ
	//dpos[1] = 0.1234; //AZ
	dpos[0] = 0.015; //AZ - as in cbmroot
	dpos[1] = 0.058; //AZ - as in cbmroot

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
	if (lay == GemStationSet->GetStation(stat)->GetModule(mod)->GetNStripLayers()) continue; //AZ-230322 - strange case

        //formula for converting from the bm@n system of modules and layers to the cbm one
        //AZ Int_t sect = 2 * mod + 1 + lay / 2;
	int sect = (mod % 2) * 2 + (mod / 2 * 4) + 1 + lay / 2; //AZ-130322

        Int_t sens = 1;
	if (fBmnSilHitsArray->GetEntriesFast() == 0) stat -= 3; //!!!AZ-190322 - configuration w/out 3 stations of Si
        Int_t detId = kGEM << 24 | (stat + 1 + 3) << 16 | sect << 4 | sens << 1;
	//Int_t detId = kGEM << 24 | (stat + 1 + 3) << 16 | (mod+1) << 4 | sens << 1; //AZ
        new ((*fCbmHitsArray)[fCbmHitsArray->GetEntriesFast()]) CbmStsHit(detId, pos, dpos, 0.0, 0, 0);
        CbmStsHit* hit = (CbmStsHit*)fCbmHitsArray->At(fCbmHitsArray->GetEntriesFast() - 1);

        FairRootManager::Instance()->SetUseFairLinks(kTRUE);
        hit->ResetLinks();
        hit->SetLinks(bmnHit->GetLinks());
        FairRootManager::Instance()->SetUseFairLinks(kFALSE);
        hit->SetRefIndex(bmnHit->GetRefIndex());
        hit->fDigiF = bmnHit->GetUpperClusterIndex();
        //AZ-230322 hit->fDigiB = bmnHit->GetLowerClusterIndex() + 1000000; //!!!AZ - for VectorFinder (to have unique indices)
        hit->fDigiB = bmnHit->GetLowerClusterIndex() + 0; //!!!AZ-230322 - for VectorFinder (to have unique indices)
	hit->fDigiF = bmnHit->GetLowerClusterIndex() + 0; //AZ-250322
	hit->fDigiB = bmnHit->GetUpperClusterIndex() + 1000000; //AZ-250322
    }
    
    for (Int_t iHit = 0; iHit < fBmnSilHitsArray->GetEntriesFast(); ++iHit) {
        BmnSiliconHit* bmnHit = (BmnSiliconHit*)fBmnSilHitsArray->At(iHit);
        TVector3 pos;
        bmnHit->Position(pos);
	pos[2] -= 0.0150; //AZ - shift to the entrance
        TVector3 dpos;
        bmnHit->PositionError(dpos);
	dpos[0]= 0.01/TMath::Sqrt(12); //AZ
	//dpos[1] = 0.1234; //AZ
	dpos[1] = 0.021; //AZ - as in cbmroot

        Int_t sens = 1;
        Int_t detId = kSILICON << 24 | (bmnHit->GetStation() + 1) << 16 | (bmnHit->GetModule() + 1) << 4 | sens << 1;
        new ((*fCbmHitsArray)[fCbmHitsArray->GetEntriesFast()]) CbmStsHit(detId, pos, dpos, 0.0, 0, 0);
        CbmStsHit* hit = (CbmStsHit*)fCbmHitsArray->At(fCbmHitsArray->GetEntriesFast() - 1);

        FairRootManager::Instance()->SetUseFairLinks(kTRUE);
        hit->ResetLinks();
        hit->SetLinks(bmnHit->GetLinks());
        FairRootManager::Instance()->SetUseFairLinks(kFALSE);
        hit->SetRefIndex(bmnHit->GetRefIndex());
        //AZ-260322 hit->fDigiF = bmnHit->GetUpperClusterIndex() + 2000000; //!!!AZ - for VectorFinder (to have unique indices)
        //AZ-260322 hit->fDigiB = bmnHit->GetLowerClusterIndex() + 3000000; //!!!AZ - for VectorFinder (to have unique indices)
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
