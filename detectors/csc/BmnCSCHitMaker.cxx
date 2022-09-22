#include <TChain.h>
#include <zlib.h>
#include <TRandom.h>
#include "TSystem.h"

#include "BmnCSCHitMaker.h"

#include "BmnEventHeader.h"
#include "FairRunAna.h"
#include <TStopwatch.h>

static Double_t workTime = 0.0;

BmnCSCHitMaker::BmnCSCHitMaker()
: fHitMatching(kTRUE) {

    fInputPointsBranchName = "CSCPoint";
    fInputDigitsBranchName = "BmnCSCDigit";
    fInputDigitMatchesBranchName = "BmnCSCDigitMatch";

    fOutputHitsBranchName = "BmnCSCHit";

    fVerbose = 1;
    fField = NULL;

    fCurrentConfig = BmnCSCConfiguration::None;
    StationSet = nullptr;
    TransfSet = nullptr;
}

BmnCSCHitMaker::BmnCSCHitMaker(Int_t run_period, Int_t run_number, Bool_t isExp, TString alignFile, Bool_t isSrc)
: fHitMatching(kTRUE) {

    fInputPointsBranchName = "CSCPoint";
    fInputDigitsBranchName = (!isExp) ? "BmnCSCDigit" : "CSC";
    fIsExp = isExp;
    fIsSrc = isSrc;

    fInputDigitMatchesBranchName = "BmnCSCDigitMatch";

    fOutputHitsBranchName = "BmnCSCHit";

    fVerbose = 1;
    fField = NULL;

    fCurrentConfig = BmnCSCConfiguration::None;
    StationSet = nullptr;
    TransfSet = nullptr;

    switch (run_period) {
        case 7: //BM@N RUN-7 (and SRC)
            fCurrentConfig = BmnCSCConfiguration::RunSpring2018;
            if (run_number >= 2041 && run_number <= 3588) {
                fCurrentConfig = BmnCSCConfiguration::RunSRCSpring2018;
            }
            break;
        case 8:
            if (fIsSrc) {
                fCurrentConfig = BmnCSCConfiguration::RunSRC2021;
            } else {
                fCurrentConfig = BmnCSCConfiguration::Run8;
            }
            break;
    }
}

BmnCSCHitMaker::~BmnCSCHitMaker() {
    if (StationSet) {
        delete StationSet;
    }

    if (TransfSet) {
        delete TransfSet;
    }
}

void BmnCSCHitMaker::LoadDetectorConfiguration() {
    TString gPathCSCConfig = gSystem->Getenv("VMCWORKDIR");
    gPathCSCConfig += "/parameters/csc/XMLConfigs/";

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

        case BmnCSCConfiguration::LargeCSC_Run8:
            StationSet = new BmnCSCStationSet(gPathCSCConfig + "LargeCSCRun8.xml");
            TransfSet = new BmnCSCTransform();
            TransfSet->LoadFromXMLFile(gPathCSCConfig + "LargeCSCRun8.xml");
            if (fVerbose) cout << "   Current CSC Configuration : LargeCSCRun8" << "\n";
            break;

        default:
            StationSet = nullptr;
    }
}

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
    fBmnCSCUpperClustersArray = new TClonesArray("StripCluster");
    ioman->Register("BmnCSCUpperCluster", "CSC", fBmnCSCUpperClustersArray, kTRUE);
    fBmnCSCLowerClustersArray = new TClonesArray("StripCluster");
    ioman->Register("BmnCSCLowerCluster", "CSC", fBmnCSCLowerClustersArray, kTRUE);

    //Create CSC detector ------------------------------------------------------

    LoadDetectorConfiguration();

    fField = FairRunAna::Instance()->GetField();
    if (!fField)
        Fatal("Init", "No Magnetic Field found");

    //--------------------------------------------------------------------------

    if (fVerbose > 1) cout << "=================== BmnCSCHitMaker::Init() finished ===================" << endl;

    return kSUCCESS;
}

InitStatus BmnCSCHitMaker::OnlineInit()
{
    if (!fCurrentConfig) LOG(fatal) << "BmnCSCHitMaker():OnlineInit() Current CSC config is not set !!! ";

    fBmnCSCDigitsArray = new TClonesArray("BmnCSCDigit");
    fBmnCSCDigitMatchesArray = nullptr;

    fBmnCSCHitsArray = new TClonesArray(fOutputHitsBranchName);
    fBmnCSCUpperClustersArray = new TClonesArray("StripCluster");
    fBmnCSCLowerClustersArray = new TClonesArray("StripCluster");

    LoadDetectorConfiguration();

    if (!fField) LOG(fatal) << "BmnCSCHitMaker::OnlineInit() No Magnetic Field found!";
    return kSUCCESS;
}

InitStatus BmnCSCHitMaker::OnlineRead(const std::unique_ptr<TTree> &dataTree, const std::unique_ptr<TTree> &resultTree)
{
    if (!IsActive()) return kERROR;

    SetOnlineActive();

    fBmnCSCDigitsArray->Clear();
    if (dataTree->SetBranchAddress(fInputDigitsBranchName, &fBmnCSCDigitsArray)) {
        LOG(error) << "BmnOnlineEDCscHitMaker::SetPayload(): branch " << fInputDigitsBranchName
                   << " not found! Task will be deactivated";
        SetOnlineActive(kFALSE);
        return kERROR;
    }

    fBmnCSCHitsArray->Clear();
    fBmnCSCUpperClustersArray->Clear();
    fBmnCSCLowerClustersArray->Clear();

    return kSUCCESS;
}

void BmnCSCHitMaker::Exec(Option_t* opt) {

    TStopwatch sw;
    sw.Start();

    if (!IsActive() || !IsOnlineActive()) return;

    fBmnCSCHitsArray->Delete();
    fBmnCSCUpperClustersArray->Delete();
    fBmnCSCLowerClustersArray->Delete();

    BmnCSCLayer::SetLowerUniqueID(0);
    BmnCSCLayer::SetUpperUniqueID(0);

    if (fVerbose > 1) cout << "=================== BmnCSCHitMaker::Exec() started ====================" << endl;

    if (fVerbose > 1) cout << " BmnCSCHitMaker::Exec(), Number of BmnCSCDigits = " << fBmnCSCDigitsArray->GetEntriesFast() << "\n";

    ProcessDigits();

    if (fVerbose > 1) cout << "=================== BmnCSCHitMaker::Exec() finished ===================" << endl;

    sw.Stop();
    workTime += sw.RealTime();
}

void BmnCSCHitMaker::ProcessDigits() {

    FairMCPoint* MCPoint;
    BmnCSCDigit* digit;
    BmnMatch* strip_match;

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

    map<Int_t, StripCluster> UniqueUpperClusters;
    map<Int_t, StripCluster> UniqueLowerClusters;

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
                if (TransfSet) {
                    Plane3D::Point glob_point = TransfSet->ApplyTransforms(Plane3D::Point(-x, y, z), iStation, iModule);
                    x = -glob_point.X();
                    y = glob_point.Y();
                    z = glob_point.Z();
                }

                Int_t RefMCIndex = -1;

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

                if (fVerbose) {
                    cout << "  glob(x:y:z) = ( " << x << " : " << y << " : " << z << "\n";
                    cout << "  hit(x:y:z) = ( " << hit->GetX() << " : " << hit->GetY() << " : " << hit->GetZ() << "\n";
                    cout << "\n";
                }

                StripCluster ucls = module->GetUpperCluster(iPoint);
                StripCluster lcls = module->GetLowerCluster(iPoint);
                ucls.SetModule(iModule);
                lcls.SetModule(iModule);
                ucls.SetStation(iStation);
                lcls.SetStation(iStation);
                UniqueUpperClusters[ucls.GetUniqueID()] = ucls;
                UniqueLowerClusters[lcls.GetUniqueID()] = lcls;
                hit->SetUpperClusterIndex(ucls.GetUniqueID());
                hit->SetLowerClusterIndex(lcls.GetUniqueID());

                if (fHitMatching) {
                    //For future update. Add link to DigiNumberMatch

                    // BmnMatch digiMatch = module->GetIntersectionPointDigitNumberMatch(iPoint);
                    // Int_t idx0 = digiMatch.GetLink(0).GetIndex();
                    // Int_t idx1 = digiMatch.GetLink(1).GetIndex();
                    // BmnMatch* digiMatch0 = (BmnMatch*)fBmnCSCDigitMatchesArray->At(idx0);
                    // BmnMatch* digiMatch1 = (BmnMatch*)fBmnCSCDigitMatchesArray->At(idx1);

                    // Bool_t hitOk = kFALSE;
                    // for (Int_t ilink = 0; ilink < digiMatch0->GetNofLinks(); ilink++) {
                    //     Int_t iindex = digiMatch0->GetLink(ilink).GetIndex();
                    //     for (Int_t jlink = 0; jlink < digiMatch1->GetNofLinks(); jlink++) {
                    //         Int_t jindex = digiMatch1->GetLink(jlink).GetIndex();
                    //         if (iindex == jindex) {
                    //             hitOk = kTRUE;
                    //             break;
                    //         }
                    //     }
                    //     if (hitOk) break;
                    // }

                    // hit->SetType(hitOk);
                    // if (!hitOk) hit->SetRefIndex(-1);

                    //--------------------------------------------------------------

                    //hit matching -------------------------------------------------
                    FairRootManager::Instance()->SetUseFairLinks(kTRUE);
                    BmnMatch hitMatch = module->GetIntersectionPointMatch(iPoint);
                    for (BmnLink lnk : hitMatch.GetLinks())
                        hit->AddLink(FairLink(-1, lnk.GetIndex(), lnk.GetWeight()));
                    FairRootManager::Instance()->SetUseFairLinks(kFALSE);
                }
                //--------------------------------------------------------------
            }
        }
    }

    for (auto it : UniqueUpperClusters) {
        for (Int_t i = 0; i < fBmnCSCHitsArray->GetEntriesFast(); i++) {
            BmnCSCHit* hit = (BmnCSCHit*) fBmnCSCHitsArray->At(i);
            if (hit->GetUpperClusterIndex() != it.first) continue;
            hit->SetUpperClusterIndex(fBmnCSCUpperClustersArray->GetEntriesFast());
        }
        it.second.SetUniqueID(fBmnCSCUpperClustersArray->GetEntriesFast());
        new ((*fBmnCSCUpperClustersArray)[fBmnCSCUpperClustersArray->GetEntriesFast()]) StripCluster(it.second);
    }
    for (auto it : UniqueLowerClusters) {
        for (Int_t i = 0; i < fBmnCSCHitsArray->GetEntriesFast(); i++) {
            BmnCSCHit* hit = (BmnCSCHit*) fBmnCSCHitsArray->At(i);
            if (hit->GetLowerClusterIndex() != it.first) continue;
            hit->SetLowerClusterIndex(fBmnCSCLowerClustersArray->GetEntriesFast());
        }
        it.second.SetUniqueID(fBmnCSCLowerClustersArray->GetEntriesFast());
        new ((*fBmnCSCLowerClustersArray)[fBmnCSCLowerClustersArray->GetEntriesFast()]) StripCluster(it.second);
    }

    if (fVerbose > 1) cout << "   N clear matches with MC-points = " << clear_matched_points_cnt << "\n";
    //------------------------------------------------------------------------------
    StationSet->Reset();
}

void BmnCSCHitMaker::OnlineWrite(const std::unique_ptr<TTree>& dataTree)
{
    if (!IsActive() || !IsOnlineActive()) return;

    dataTree->Branch(fOutputHitsBranchName, &fBmnCSCHitsArray);
    dataTree->Branch("BmnCSCUpperCluster", &fBmnCSCUpperClustersArray);
    dataTree->Branch("BmnCSCLowerCluster", &fBmnCSCLowerClustersArray);
    dataTree->Fill();
}

void BmnCSCHitMaker::Finish() {
    if (StationSet) {
        delete StationSet;
        StationSet = nullptr;
    }

    if (TransfSet) {
        delete TransfSet;
        TransfSet = nullptr;
    }

    printf("Work time of BmnCSCHitMaker: %4.2f sec.\n", workTime);
}

ClassImp(BmnCSCHitMaker)
