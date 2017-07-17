#include "BmnSiliconHitMaker.h"

#include "BmnSiliconModule.h"

#include "TClonesArray.h"
#include "TSystem.h"

static Float_t workTime = 0.0;

BmnSiliconHitMaker::BmnSiliconHitMaker() {

    fInputPointsBranchName = "SiliconPoint";
    fInputDigitsBranchName = "BmnSiliconDigit";

    fOutputHitsBranchName = "BmnSiliconHit";

    fVerbose = 1;

    StationSet = NULL;
}

BmnSiliconHitMaker::~BmnSiliconHitMaker() {

}

InitStatus BmnSiliconHitMaker::Init() {

    if (fVerbose) cout << "\nBmnSiliconHitMaker::Init()\n ";

    FairRootManager* ioman = FairRootManager::Instance();

    fBmnSiliconDigitsArray = (TClonesArray*) ioman->GetObject(fInputDigitsBranchName);

    fBmnSiliconHitsArray = new TClonesArray(fOutputHitsBranchName);
    ioman->Register(fOutputHitsBranchName, "SILICON", fBmnSiliconHitsArray, kTRUE);

    TString gPathSiliconConfig = gSystem->Getenv("VMCWORKDIR");
        gPathSiliconConfig += "/silicon/XMLConfigs/";

    //Create Silicon detector --------------------------------------------------

    StationSet = new BmnSiliconStationSet(gPathSiliconConfig + "SiliconRunSpring2017.xml");
    cout << "   Current Configuration : RunSpring2017" << "\n";

    //--------------------------------------------------------------------------

    if (fVerbose) cout << "BmnGemStripHitMaker::Init() finished\n";

    return kSUCCESS;
}

void BmnSiliconHitMaker::Exec(Option_t* opt) {
    clock_t tStart = clock();

    fBmnSiliconHitsArray->Delete();

    if (!fBmnSiliconDigitsArray) {
        Error("BmnSiliconHitMaker::Exec()", " !!! Unknown branch name !!! ");
        return;
    }

    if (fVerbose) cout << " BmnSiliconHitMaker::Exec(), Number of BmnSiliconDigits = " << fBmnSiliconDigitsArray->GetEntriesFast() << "\n";

    ProcessDigits();

    if (fVerbose) cout << " BmnSiliconHitMaker::Exec() finished\n";
    clock_t tFinish = clock();
    workTime += ((Float_t) (tFinish - tStart)) / CLOCKS_PER_SEC;
}

void BmnSiliconHitMaker::ProcessDigits() {

    FairMCPoint* MCPoint;
    BmnSiliconDigit* digit;

    BmnSiliconStation* station;
    BmnSiliconModule* module;

    //Loading digits ---------------------------------------------------------------
    Int_t AddedDigits = 0;

    for (UInt_t idigit = 0; idigit < fBmnSiliconDigitsArray->GetEntriesFast(); idigit++) {
        digit = (BmnSiliconDigit*) fBmnSiliconDigitsArray->At(idigit);
        station = StationSet->GetSiliconStation(digit->GetStation());
        module = station->GetModule(digit->GetModule());

        if (module->SetStripSignalInLayerByZoneId(digit->GetStripLayer(), digit->GetStripNumber(), digit->GetStripSignal())) AddedDigits++;
    }

    if (fVerbose) cout << "   Processed strip digits  : " << AddedDigits << "\n";
    //------------------------------------------------------------------------------

    //Processing digits
    StationSet->ProcessPointsInDetector();

    Int_t NCalculatedPoints = StationSet->CountNProcessedPointsInDetector();
    if (fVerbose) cout << "   Calculated points  : " << NCalculatedPoints << "\n";

    for (Int_t iStation = 0; iStation < StationSet->GetNStations(); ++iStation) {
        BmnSiliconStation *station = StationSet->GetSiliconStation(iStation);

        for (Int_t iModule = 0; iModule < station->GetNModules(); ++iModule) {
            BmnSiliconModule *module = station->GetModule(iModule);
            Double_t z = module->GetZPositionRegistered();

            Int_t NIntersectionPointsInModule = module->GetNIntersectionPoints();

            for (Int_t iPoint = 0; iPoint < NIntersectionPointsInModule; ++iPoint) {
                Double_t x = module->GetIntersectionPointX(iPoint);
                Double_t y = module->GetIntersectionPointY(iPoint);

                Double_t x_err = module->GetIntersectionPointXError(iPoint);
                Double_t y_err = module->GetIntersectionPointYError(iPoint);
                Double_t z_err = 0.0;

                Int_t RefMCIndex = 0;

                //Add hit ------------------------------------------------------
                x *= -1; // invert to global X

                new ((*fBmnSiliconHitsArray)[fBmnSiliconHitsArray->GetEntriesFast()])
                        BmnSiliconHit(0, TVector3(x, y, z), TVector3(x_err, y_err, z_err), RefMCIndex);

                BmnSiliconHit* hit = (BmnSiliconHit*) fBmnSiliconHitsArray->At(fBmnSiliconHitsArray->GetEntriesFast() - 1);
                hit->SetStation(iStation);
                hit->SetModule(iModule);
                hit->SetIndex(fBmnSiliconHitsArray->GetEntriesFast() - 1);
                hit->SetClusterSizeInLowerLayer(module->GetIntersectionPoint_LowerLayerClusterSize(iPoint)); //cluster size (lower layer |||)
                hit->SetClusterSizeInUpperLayer(module->GetIntersectionPoint_UpperLayerClusterSize(iPoint)); //cluster size (upper layer ///or\\\)
                hit->SetStripPositionInLowerLayer(module->GetIntersectionPoint_LowerLayerSripPosition(iPoint)); //strip position (lower layer |||)
                hit->SetStripPositionInUpperLayer(module->GetIntersectionPoint_UpperLayerSripPosition(iPoint)); //strip position (upper layer ///or\\\)
                //--------------------------------------------------------------
            }
        }
    }
    //------------------------------------------------------------------------------
    StationSet->Reset();
}

void BmnSiliconHitMaker::Finish() {
    if (StationSet) {
        delete StationSet;
        StationSet = NULL;
    }
    cout << "Work time of the Silicon hit maker: " << workTime << endl;
}

ClassImp(BmnSiliconHitMaker)

