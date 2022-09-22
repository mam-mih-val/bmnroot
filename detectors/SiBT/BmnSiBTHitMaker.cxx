#include <TChain.h>
#include <zlib.h>
#include <TRandom.h>
#include "TSystem.h"

#include "BmnSiBTHitMaker.h"

#include "BmnEventHeader.h"
#include "FairRunAna.h"
#include <TStopwatch.h>

static Double_t workTime = 0.0;

BmnSiBTHitMaker::BmnSiBTHitMaker()
: fHitMatching(kTRUE) {

    fInputPointsBranchName = "SiBTPoint";
    fInputDigitsBranchName = "BmnSiBTDigit";
    fInputDigitMatchesBranchName = "BmnSiBTDigitMatch";

    fOutputHitsBranchName = "BmnSiBTHit";
    fOutputHitMatchesBranchName = "BmnSiBTHitMatch";

    fVerbose = 1;
    fField = NULL;

    fCurrentConfig = BmnSiBTConfiguration::None;
    StationSet = nullptr;
    TransfSet = nullptr;
}

BmnSiBTHitMaker::BmnSiBTHitMaker(Int_t run_period, Int_t run_number, Bool_t isExp, TString alignFile)
: fHitMatching(kTRUE) {

    fInputPointsBranchName = "SiBTPoint";
    fInputDigitsBranchName = (!isExp) ? "BmnSiBTDigit" : "SiBT";
    fIsExp = isExp;

    fInputDigitMatchesBranchName = "BmnSiBTDigitMatch";

    fOutputHitsBranchName = "BmnSiBTHit";
    fOutputHitMatchesBranchName = "BmnSiBTHitMatch";

    fBmnEvQualityBranchName = "BmnEventQuality";

    fVerbose = 1;
    fField = NULL;

    fCurrentConfig = BmnSiBTConfiguration::None;
    StationSet = nullptr;
    TransfSet = nullptr;

    switch(run_period) {
        case 8: //BM@N RUN-8
            fCurrentConfig = BmnSiBTConfiguration::Run8;
            break;
    }
}

BmnSiBTHitMaker::~BmnSiBTHitMaker() {
    if (StationSet) {
        delete StationSet;
    }

    if (TransfSet) {
        delete TransfSet;
    }
}

void BmnSiBTHitMaker::LoadDetectorConfiguration()
{
    TString gPathSiBTConfig = gSystem->Getenv("VMCWORKDIR");
    gPathSiBTConfig += "/parameters/SiBT/XMLConfigs/";

    switch (fCurrentConfig)
    {
        case BmnSiBTConfiguration::Run8:
            StationSet = new BmnSiBTStationSet(gPathSiBTConfig + "SiBTRun8.xml");
            TransfSet = new BmnSiBTTransform();
            TransfSet->LoadFromXMLFile(gPathSiBTConfig + "SiBTRun8.xml");
            if (fVerbose) cout << "   Current SiBT Configuration : Run8" << "\n";
            break;
        default:
            StationSet = nullptr;
    }
}

InitStatus BmnSiBTHitMaker::Init() {

    if (fVerbose > 1) cout << "=================== BmnSiBTHitMaker::Init() started ====================" << endl;

    //if the configuration is not set -> return a fatal error
    if (!fCurrentConfig) Fatal("BmnSiBTHitMaker::Init()", " !!! Current SiBT config is not set !!! ");

    FairRootManager* ioman = FairRootManager::Instance();

    fBmnSiBTDigitsArray = (TClonesArray*) ioman->GetObject(fInputDigitsBranchName);
    if (!fBmnSiBTDigitsArray) {
        cout << "BmnSiBTHitMaker::Init(): branch " << fInputDigitsBranchName << " not found! Task will be deactivated" << endl;
        SetActive(kFALSE);
        return kERROR;
    }

    fBmnSiBTDigitMatchesArray = (TClonesArray*) ioman->GetObject(fInputDigitMatchesBranchName);

    if (fVerbose > 1) {
        if (fBmnSiBTDigitMatchesArray) cout << "  Strip matching information exists!\n";
        else cout << "  Strip matching information doesn`t exist!\n";
    }

    fBmnSiBTHitsArray = new TClonesArray(fOutputHitsBranchName);
    ioman->Register(fOutputHitsBranchName, "SiBT", fBmnSiBTHitsArray, kTRUE);

    if (fHitMatching && fBmnSiBTDigitMatchesArray) {
        fBmnSiBTHitMatchesArray = new TClonesArray("BmnMatch");
        ioman->Register(fOutputHitMatchesBranchName, "SiBT", fBmnSiBTHitMatchesArray, kTRUE);
    } else {
        fBmnSiBTHitMatchesArray = 0;
    }

    //Create SiBT detector ------------------------------------------------------
    LoadDetectorConfiguration();

    fField = FairRunAna::Instance()->GetField();
    if (!fField)
        Fatal("Init", "No Magnetic Field found");

    //--------------------------------------------------------------------------

    fBmnEvQuality = (TClonesArray*) ioman->GetObject(fBmnEvQualityBranchName);

    if (fVerbose > 1) cout << "=================== BmnSiBTHitMaker::Init() finished ===================" << endl;

    return kSUCCESS;
}

InitStatus BmnSiBTHitMaker::OnlineInit() {
    // if the configuration is not set -> return a fatal error
    if (!fCurrentConfig) LOG(fatal) << "BmnSiBTHitMaker():OnlineInit() Current CSC config is not set !!! ";

    fBmnSiBTDigitsArray = new TClonesArray("BmnSiBTDigit");
    fBmnSiBTDigitMatchesArray = nullptr;
    fBmnEvQuality = new TClonesArray("BmnEventQuality");

    fBmnSiBTHitsArray = new TClonesArray(fOutputHitsBranchName);

    LoadDetectorConfiguration();

    if (!fField) LOG(fatal) << "BmnCSCHitMaker::OnlineInit() No Magnetic Field found!";

    return kSUCCESS;
}

InitStatus BmnSiBTHitMaker::OnlineRead(const std::unique_ptr<TTree> &dataTree, const std::unique_ptr<TTree> &resultTree)
{
    if (!IsActive()) return kERROR;

    SetOnlineActive();

    fBmnSiBTDigitsArray->Delete();
    if (dataTree->SetBranchAddress(fInputDigitsBranchName, &fBmnSiBTDigitsArray)) {
        LOG(error) << "BmnOnlineEDSiBTHitMaker::SetPayload(): branch " << fInputDigitsBranchName
                   << " not found! Task will be deactivated";
        SetOnlineActive(kFALSE);
        return kERROR;
    }

    fBmnEvQuality->Delete();
    dataTree->SetBranchAddress(fBmnEvQualityBranchName, &fBmnEvQuality);

    fBmnSiBTHitsArray->Delete();

    return kSUCCESS;
}

void BmnSiBTHitMaker::Exec(Option_t* opt) {

    TStopwatch sw;
    sw.Start();

    if (!IsActive() || !IsOnlineActive()) return;

    // Event separation by triggers ...
    if (fIsExp && fBmnEvQuality) {
        BmnEventQuality* evQual = (BmnEventQuality*) fBmnEvQuality->UncheckedAt(0);
        if (!evQual->GetIsGoodEvent())
            return;
    }
    fBmnSiBTHitsArray->Delete();

    if (fHitMatching && fBmnSiBTHitMatchesArray) {
        fBmnSiBTHitMatchesArray->Delete();
    }

    if (fVerbose > 1) cout << "=================== BmnSiBTHitMaker::Exec() started ====================" << endl;

    fField = FairRunAna::Instance()->GetField();

    if (fVerbose > 1) cout << " BmnSiBTHitMaker::Exec(), Number of BmnSiBTDigits = " << fBmnSiBTDigitsArray->GetEntriesFast() << "\n";

    ProcessDigits();

    if (fVerbose > 1) cout << "=================== BmnSiBTHitMaker::Exec() finished ===================" << endl;

    sw.Stop();
    workTime += sw.RealTime();
}

void BmnSiBTHitMaker::ProcessDigits() {

    FairMCPoint* MCPoint;
    BmnSiBTDigit* digit;
    BmnMatch *strip_match;

    BmnSiBTStation* station;
    BmnSiBTModule* module;

    //Loading digits ---------------------------------------------------------------
    Int_t AddedDigits = 0;
    Int_t AddedStripDigitMatches = 0;

    for (UInt_t idigit = 0; idigit < fBmnSiBTDigitsArray->GetEntriesFast(); idigit++) {
        digit = (BmnSiBTDigit*) fBmnSiBTDigitsArray->At(idigit);
        if (!digit->IsGoodDigit())
            continue;
        station = StationSet->GetStation(digit->GetStation());
        module = station->GetModule(digit->GetModule());

        if (module->SetStripSignalInLayer(digit->GetStripLayer(), digit->GetStripNumber(), digit->GetStripSignal())) AddedDigits++;

        if (fBmnSiBTDigitMatchesArray) {
            strip_match = (BmnMatch*) fBmnSiBTDigitMatchesArray->At(idigit);
            if (module->SetStripMatchInLayer(digit->GetStripLayer(), digit->GetStripNumber(), *strip_match)) AddedStripDigitMatches++;
        }
    }

    if (fVerbose > 1) cout << "   Processed strip digits  : " << AddedDigits << "\n";
    if (fVerbose > 1 && fBmnSiBTDigitMatchesArray) cout << "   Added strip digit matches  : " << AddedStripDigitMatches << "\n";
    //------------------------------------------------------------------------------

    //Processing digits
    StationSet->ProcessPointsInDetector();

    Int_t NCalculatedPoints = StationSet->CountNProcessedPointsInDetector();
    if (fVerbose > 1) cout << "   Calculated points  : " << NCalculatedPoints << "\n";
    if (fVerbose == 1) cout << "BmnSiBTHitMaker: " << NCalculatedPoints << " hits\n";

    Int_t clear_matched_points_cnt = 0; // points with the only one match-index

    for (Int_t iStation = 0; iStation < StationSet->GetNStations(); ++iStation) {
        station = StationSet->GetStation(iStation);

        for (Int_t iModule = 0; iModule < station->GetNModules(); ++iModule) {
            module = station->GetModule(iModule);

            Int_t NIntersectionPointsInModule = module->GetNIntersectionPoints();

            for (Int_t iPoint = 0; iPoint < NIntersectionPointsInModule; ++iPoint) {

                Double_t sigL = module->GetIntersectionPoint_LowerLayerSripTotalSignal(iPoint);
                Double_t sigU = module->GetIntersectionPoint_UpperLayerSripTotalSignal(iPoint);

                Double_t x = module->GetIntersectionPointX(iPoint);
                Double_t y = module->GetIntersectionPointY(iPoint);
                Double_t z = module->GetZPositionRegistered();

                Double_t x_err = module->GetIntersectionPointXError(iPoint);
                Double_t y_err = module->GetIntersectionPointYError(iPoint);
                Double_t z_err = 0.0;

                //Transform hit coordinates from local coordinate system of GEM-planes to global
                if(TransfSet) {
                    Plane3D::Point glob_point = TransfSet->ApplyTransforms(Plane3D::Point(-x, y, z), iStation, iModule);
                    x = -glob_point.X();
                    y = glob_point.Y();
                    z = glob_point.Z();
                }

                Int_t RefMCIndex = 0;

                //hit matching (define RefMCIndex)) ----------------------------
                BmnMatch match = module->GetIntersectionPointMatch(iPoint);

                Int_t most_probably_index = -1;
                Double_t max_weight = 0;

                Int_t n_links = match.GetNofLinks();
                if (n_links == 1) clear_matched_points_cnt++;
                for (Int_t ilink = 0; ilink < n_links; ilink++) {
                    Int_t index = match.GetLink(ilink).GetIndex();
                    Double_t weight = match.GetLink(ilink).GetWeight();
                    if (weight > max_weight) {
                        max_weight = weight;
                        most_probably_index = index;
                    }
                }

                RefMCIndex = most_probably_index;
                //--------------------------------------------------------------

                //Add hit ------------------------------------------------------
                x *= -1; // invert to global X //Temporary switched off

                new ((*fBmnSiBTHitsArray)[fBmnSiBTHitsArray->GetEntriesFast()])
                        BmnSiBTHit(0, TVector3(x, y, z), TVector3(x_err, y_err, z_err), RefMCIndex);

                BmnSiBTHit* hit = (BmnSiBTHit*) fBmnSiBTHitsArray->At(fBmnSiBTHitsArray->GetEntriesFast() - 1);
                hit->SetStation(iStation);
                hit->SetModule(iModule);
                hit->SetIndex(fBmnSiBTHitsArray->GetEntriesFast() - 1);
                hit->SetClusterSizeInLowerLayer(module->GetIntersectionPoint_LowerLayerClusterSize(iPoint)); //cluster size (lower layer |||)
                hit->SetClusterSizeInUpperLayer(module->GetIntersectionPoint_UpperLayerClusterSize(iPoint)); //cluster size (upper layer ///or\\\)
                hit->SetStripPositionInLowerLayer(module->GetIntersectionPoint_LowerLayerSripPosition(iPoint)); //strip position (lower layer |||)
                hit->SetStripPositionInUpperLayer(module->GetIntersectionPoint_UpperLayerSripPosition(iPoint)); //strip position (upper layer ///or\\\)
                hit->SetStripTotalSignalInLowerLayer(sigL);
                hit->SetStripTotalSignalInUpperLayer(sigU);

                if (fVerbose) {
                    cout << "  glob(x:y:z) = ( " << x << " : " << y << " : " << z << "\n";
                    cout << "  hit(x:y:z) = ( " << hit->GetX() << " : " << hit->GetY() << " : " << hit->GetZ() << "\n";
                    cout << "\n";
                }
                //--------------------------------------------------------------

                //hit matching -------------------------------------------------
                if (fHitMatching && fBmnSiBTHitMatchesArray) {
                    new ((*fBmnSiBTHitMatchesArray)[fBmnSiBTHitMatchesArray->GetEntriesFast()])
                            BmnMatch(module->GetIntersectionPointMatch(iPoint));
                }
                //--------------------------------------------------------------
            }
        }
    }
    if (fVerbose > 1) cout << "   N clear matches with MC-points = " << clear_matched_points_cnt << "\n";
    //------------------------------------------------------------------------------
    StationSet->Reset();
}

void BmnSiBTHitMaker::OnlineWrite(const std::unique_ptr<TTree> &dataTree)
{
    if (!IsActive() || !IsOnlineActive()) return;

    dataTree->Branch(fOutputHitsBranchName, &fBmnSiBTHitsArray);
    dataTree->Fill();
}

void BmnSiBTHitMaker::Finish() {
     if (StationSet) {
        delete StationSet;
        StationSet = nullptr;
    }

    if(TransfSet) {
        delete TransfSet;
        TransfSet = nullptr;
    }

    printf("Work time of BmnSiBTHitMaker: %4.2f sec.\n", workTime);
}

ClassImp(BmnSiBTHitMaker)
