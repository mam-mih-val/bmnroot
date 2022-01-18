#include <TChain.h>
#include <zlib.h>
#include <TRandom.h>
#include "TSystem.h"

#include "BmnCSCHitMaker.h"

#include "BmnEventHeader.h"
#include "FairRunAna.h"

static Float_t workTime = 0.0;

BmnCSCHitMaker::BmnCSCHitMaker()
: fHitMatching(kTRUE) {

    fInputPointsBranchName = "CSCPoint";
    fInputDigitsBranchName = "BmnCSCDigit";
    fInputDigitMatchesBranchName = "BmnCSCDigitMatch";

    fOutputHitsBranchName = "BmnCSCHit";
    fOutputHitMatchesBranchName = "BmnCSCHitMatch";

    fVerbose = 1;
    fField = NULL;

    fCurrentConfig = BmnCSCConfiguration::None;
    StationSet = nullptr;
    TransfSet = nullptr;
}

BmnCSCHitMaker::BmnCSCHitMaker(Int_t run_period, Int_t run_number, Bool_t isExp, TString alignFile)
: fHitMatching(kTRUE) {

    fInputPointsBranchName = "CSCPoint";
    fInputDigitsBranchName = (!isExp) ? "BmnCSCDigit" : "CSC";
    fIsExp = isExp;

    fInputDigitMatchesBranchName = "BmnCSCDigitMatch";

    fOutputHitsBranchName = "BmnCSCHit";
    fOutputHitMatchesBranchName = "BmnCSCHitMatch";

    fBmnEvQualityBranchName = "BmnEventQuality";

    fVerbose = 1;
    fField = NULL;

    fCurrentConfig = BmnCSCConfiguration::None;
    StationSet = nullptr;
    TransfSet = nullptr;

    switch(run_period) {
        case 7: //BM@N RUN-7 (and SRC)
            fCurrentConfig = BmnCSCConfiguration::RunSpring2018;
            if(run_number >= 2041 && run_number <= 3588) {
                fCurrentConfig = BmnCSCConfiguration::RunSRCSpring2018;
            }
            break;
    }
}

BmnCSCHitMaker::~BmnCSCHitMaker() { }

InitStatus BmnCSCHitMaker::Init() {

    if (fVerbose > 1) cout << "=================== BmnCSCHitMaker::Init() started ====================" << endl;

    //if the configuration is not set -> return a fatal error
    if (!fCurrentConfig) Fatal("BmnCSCHitMaker::Init()", " !!! Current CSC config is not set !!! ");

    FairRootManager* ioman = FairRootManager::Instance();

    fBmnCSCDigitsArray = (TClonesArray*) ioman->GetObject(fInputDigitsBranchName);
    if (!fBmnCSCDigitsArray) {
        cout << "BmnCSCHitMaker::Init(): branch " << fInputDigitsBranchName << " not found! Task will be deactivated" << endl;
        SetActive(kFALSE);
        return kERROR;
    }

    fBmnCSCDigitMatchesArray = (TClonesArray*) ioman->GetObject(fInputDigitMatchesBranchName);

    if (fVerbose > 1) {
        if (fBmnCSCDigitMatchesArray) cout << "  Strip matching information exists!\n";
        else cout << "  Strip matching information doesn`t exist!\n";
    }

    fBmnCSCHitsArray = new TClonesArray(fOutputHitsBranchName);
    ioman->Register(fOutputHitsBranchName, "CSC", fBmnCSCHitsArray, kTRUE);

    if (fHitMatching && fBmnCSCDigitMatchesArray) {
        fBmnCSCHitMatchesArray = new TClonesArray("BmnMatch");
        ioman->Register(fOutputHitMatchesBranchName, "CSC", fBmnCSCHitMatchesArray, kTRUE);
    } else {
        fBmnCSCHitMatchesArray = 0;
    }

    TString gPathCSCConfig = gSystem->Getenv("VMCWORKDIR");
    gPathCSCConfig += "/parameters/csc/XMLConfigs/";

    //Create CSC detector ------------------------------------------------------
    switch (fCurrentConfig) {
        case BmnCSCConfiguration::RunSpring2018:
            StationSet = new BmnCSCStationSet(gPathCSCConfig + "CSCRunSpring2018.xml");
            TransfSet = new BmnCSCTransform();
            TransfSet->LoadFromXMLFile(gPathCSCConfig + "CSCRunSpring2018.xml");
            if (fVerbose > 1) cout << "   Current CSC Configuration : RunSpring2018" << "\n";
            break;

        case BmnCSCConfiguration::RunSRCSpring2018:
            StationSet = new BmnCSCStationSet(gPathCSCConfig + "CSCRunSRCSpring2018.xml");
            TransfSet = new BmnCSCTransform();
            TransfSet->LoadFromXMLFile(gPathCSCConfig + "CSCRunSRCSpring2018.xml");
            if (fVerbose > 1) cout << "   Current CSC Configuration : RunSRCSpring2018" << "\n";
            break;

        case BmnCSCConfiguration::Run8:
            StationSet = new BmnCSCStationSet(gPathCSCConfig + "CSCRun8.xml");
            TransfSet = new BmnCSCTransform();
            TransfSet->LoadFromXMLFile(gPathCSCConfig + "CSCRun8.xml");
            if (fVerbose) cout << "   Current CSC Configuration : Run8" << "\n";
            break;

        case BmnCSCConfiguration::RunSRC2021:
            StationSet = new BmnCSCStationSet(gPathCSCConfig + "CSCRunSRC2021.xml");
            TransfSet = new BmnCSCTransform();
            TransfSet->LoadFromXMLFile(gPathCSCConfig + "CSCRunSRC2021.xml");
            if (fVerbose) cout << "   Current CSC Configuration : RunSRC2021" << "\n";
            break;

        default:
            StationSet = nullptr;
    }

    fField = FairRunAna::Instance()->GetField();
    if (!fField)
        Fatal("Init", "No Magnetic Field found");

    //--------------------------------------------------------------------------

    fBmnEvQuality = (TClonesArray*) ioman->GetObject(fBmnEvQualityBranchName);

    if (fVerbose > 1) cout << "=================== BmnCSCHitMaker::Init() finished ===================" << endl;

    return kSUCCESS;
}

void BmnCSCHitMaker::Exec(Option_t* opt) {
    // Event separation by triggers ...
    if (fIsExp && fBmnEvQuality) {
        BmnEventQuality* evQual = (BmnEventQuality*) fBmnEvQuality->UncheckedAt(0);
        if (!evQual->GetIsGoodEvent())
            return;
    }
    fBmnCSCHitsArray->Delete();

    if (fHitMatching && fBmnCSCHitMatchesArray) {
        fBmnCSCHitMatchesArray->Delete();
    }

    if (!IsActive())
        return;

    if (fVerbose > 1) cout << "=================== BmnCSCHitMaker::Exec() started ====================" << endl;
    clock_t tStart = clock();

    fField = FairRunAna::Instance()->GetField();

    if (fVerbose > 1) cout << " BmnCSCHitMaker::Exec(), Number of BmnCSCDigits = " << fBmnCSCDigitsArray->GetEntriesFast() << "\n";

    ProcessDigits();

    if (fVerbose > 1) cout << "=================== BmnCSCHitMaker::Exec() finished ===================" << endl;
    clock_t tFinish = clock();
    workTime += ((Float_t) (tFinish - tStart)) / CLOCKS_PER_SEC;
}

void BmnCSCHitMaker::ProcessDigits() {

    FairMCPoint* MCPoint;
    BmnCSCDigit* digit;
    BmnMatch *strip_match;

    BmnCSCStation* station;
    BmnCSCModule* module;

    //Loading digits ---------------------------------------------------------------
    Int_t AddedDigits = 0;
    Int_t AddedStripDigitMatches = 0;

    for (UInt_t idigit = 0; idigit < fBmnCSCDigitsArray->GetEntriesFast(); idigit++) {
        digit = (BmnCSCDigit*) fBmnCSCDigitsArray->At(idigit);
        if (!digit->IsGoodDigit())
            continue;
        station = StationSet->GetStation(digit->GetStation());
        module = station->GetModule(digit->GetModule());

        if (module->SetStripSignalInLayer(digit->GetStripLayer(), digit->GetStripNumber(), digit->GetStripSignal())) AddedDigits++;

        if (fBmnCSCDigitMatchesArray) {
            strip_match = (BmnMatch*) fBmnCSCDigitMatchesArray->At(idigit);
            if (module->SetStripMatchInLayer(digit->GetStripLayer(), digit->GetStripNumber(), *strip_match)) AddedStripDigitMatches++;
        }
    }

    if (fVerbose > 1) cout << "   Processed strip digits  : " << AddedDigits << "\n";
    if (fVerbose > 1 && fBmnCSCDigitMatchesArray) cout << "   Added strip digit matches  : " << AddedStripDigitMatches << "\n";
    //------------------------------------------------------------------------------

    //Processing digits
    StationSet->ProcessPointsInDetector();

    Int_t NCalculatedPoints = StationSet->CountNProcessedPointsInDetector();
    if (fVerbose > 1) cout << "   Calculated points  : " << NCalculatedPoints << "\n";
    if (fVerbose == 1) cout << "BmnCSCHitMaker: " << NCalculatedPoints << " hits\n";

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

                new ((*fBmnCSCHitsArray)[fBmnCSCHitsArray->GetEntriesFast()])
                        BmnCSCHit(0, TVector3(x, y, z), TVector3(x_err, y_err, z_err), RefMCIndex);

                BmnCSCHit* hit = (BmnCSCHit*) fBmnCSCHitsArray->At(fBmnCSCHitsArray->GetEntriesFast() - 1);
                hit->SetStation(iStation);
                hit->SetModule(iModule);
                hit->SetIndex(fBmnCSCHitsArray->GetEntriesFast() - 1);
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
                if (fHitMatching && fBmnCSCHitMatchesArray) {
                    FairRootManager::Instance()->SetUseFairLinks(kTRUE);
                    new ((*fBmnCSCHitMatchesArray)[fBmnCSCHitMatchesArray->GetEntriesFast()])
                            BmnMatch(module->GetIntersectionPointMatch(iPoint));
                    BmnMatch* hitMatch = (BmnMatch*) fBmnCSCHitMatchesArray->At(fBmnCSCHitMatchesArray->GetEntriesFast() - 1);
                    for(BmnLink lnk : hitMatch->GetLinks())
                        hit->AddLink(FairLink(-1, lnk.GetIndex(), lnk.GetWeight()));
                    FairRootManager::Instance()->SetUseFairLinks(kFALSE);
                }                
                //--------------------------------------------------------------
            }
        }
    }
    if (fVerbose > 1) cout << "   N clear matches with MC-points = " << clear_matched_points_cnt << "\n";
    //------------------------------------------------------------------------------
    StationSet->Reset();
}

void BmnCSCHitMaker::Finish() {
     if (StationSet) {
        delete StationSet;
        StationSet = nullptr;
    }

    if(TransfSet) {
        delete TransfSet;
        TransfSet = nullptr;
    }

    if (fVerbose > 0) cout << "Work time of the CSC hit maker: " << workTime << endl;
}

ClassImp(BmnCSCHitMaker)
