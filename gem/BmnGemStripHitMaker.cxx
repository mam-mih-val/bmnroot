#include <TChain.h>
#include <zlib.h>
#include <TRandom.h>
#include "TSystem.h"

#include "BmnGemStripHitMaker.h"

#include "BmnGemStripStationSet_RunSummer2016.h"
#include "BmnGemStripStationSet_RunWinter2016.h"
#include "BmnGemStripStationSet_RunSpring2017.h"
#include "BmnEventHeader.h"
#include "FairRunAna.h"

static Float_t workTime = 0.0;

BmnGemStripHitMaker::BmnGemStripHitMaker()
: fHitMatching(kTRUE) {

    fInputPointsBranchName = "StsPoint";
    fInputDigitsBranchName = "BmnGemStripDigit";
    fInputDigitMatchesBranchName = "BmnGemStripDigitMatch";

    fOutputHitsBranchName = "BmnGemStripHit";
    fOutputHitMatchesBranchName = "BmnGemStripHitMatch";

    fField = NULL;

    fCurrentConfig = BmnGemStripConfiguration::None;
    StationSet = nullptr;
    TransfSet = nullptr;
}

BmnGemStripHitMaker::BmnGemStripHitMaker(Int_t run_period, Int_t run_number, Bool_t isExp, TString alignFile)
: fHitMatching(kTRUE) {

    fInputPointsBranchName = "StsPoint";
    fInputDigitsBranchName = (!isExp) ? "BmnGemStripDigit" : "GEM";
    fIsExp = isExp;
    fIsSrc = kFALSE;

    fInputDigitMatchesBranchName = "BmnGemStripDigitMatch";

    fOutputHitsBranchName = "BmnGemStripHit";
    fOutputHitMatchesBranchName = "BmnGemStripHitMatch";

    fBmnEvQualityBranchName = "BmnEventQuality";

    fField = NULL;

    fCurrentConfig = BmnGemStripConfiguration::None;
    StationSet = nullptr;
    TransfSet = nullptr;

    switch(run_period) {
        case 5: //BM@N RUN-5
            fCurrentConfig = BmnGemStripConfiguration::RunWinter2016;
            break;
        case 6: //BM@N RUN-6
            fCurrentConfig = BmnGemStripConfiguration::RunSpring2017;
            break;
        case 7: //BM@N RUN-7 (and SRC)
            fCurrentConfig = BmnGemStripConfiguration::RunSpring2018;
            if(run_number >= 2041 && run_number <= 3588) {
                fCurrentConfig = BmnGemStripConfiguration::RunSRCSpring2018;
            }
            break;
    }

    if (fIsExp)
        fAlign = new BmnInnTrackerAlign(run_period, run_number, alignFile);
}

BmnGemStripHitMaker::~BmnGemStripHitMaker() {
    if (fIsExp)
        delete fAlign;
}

InitStatus BmnGemStripHitMaker::Init() {

    if (fVerbose > 1) cout << "=================== BmnGemStripHitMaker::Init() started ===============" << endl;

    //if GEM configuration is not set -> return a fatal error
    if (!fCurrentConfig) Fatal("BmnGemStripHitMaker::Init()", " !!! Current GEM config is not set !!! ");

    FairRootManager* ioman = FairRootManager::Instance();

    fBmnGemStripDigitsArray = (TClonesArray*) ioman->GetObject(fInputDigitsBranchName);
    if (!fBmnGemStripDigitsArray) {
        cout << "BmnGemStripHitMaker::Init(): branch " << fInputDigitsBranchName << " not found! Task will be deactivated" << endl;
        SetActive(kFALSE);
        return kERROR;
    }

    fBmnGemStripDigitMatchesArray = (TClonesArray*) ioman->GetObject(fInputDigitMatchesBranchName);

    if (fVerbose > 1) {
        if (fBmnGemStripDigitMatchesArray) cout << "  Strip matching information exists!\n";
        else cout << "  Strip matching information doesn`t exist!\n";
    }

    fBmnGemStripHitsArray = new TClonesArray(fOutputHitsBranchName);
    ioman->Register(fOutputHitsBranchName, "GEM", fBmnGemStripHitsArray, kTRUE);

    if (fHitMatching && fBmnGemStripDigitMatchesArray) {
        fBmnGemStripHitMatchesArray = new TClonesArray("BmnMatch");
        ioman->Register(fOutputHitMatchesBranchName, "GEM", fBmnGemStripHitMatchesArray, kTRUE);
    } else {
        fBmnGemStripHitMatchesArray = 0;
    }

    TString gPathGemConfig = gSystem->Getenv("VMCWORKDIR");
    gPathGemConfig += "/parameters/gem/XMLConfigs/";

    //Create GEM detector ------------------------------------------------------
    switch (fCurrentConfig) {
        case BmnGemStripConfiguration::RunSummer2016:
            StationSet = new BmnGemStripStationSet_RunSummer2016(fCurrentConfig);
            if (fVerbose > 1) cout << "   Current GEM Configuration : RunSummer2016" << "\n";
            break;

        case BmnGemStripConfiguration::RunWinter2016:
            StationSet = new BmnGemStripStationSet_RunWinter2016(fCurrentConfig);
            if (fVerbose > 1) cout << "   Current GEM Configuration : RunWinter2016" << "\n";
            break;

        case BmnGemStripConfiguration::RunSpring2017:
            StationSet = new BmnGemStripStationSet_RunSpring2017(fCurrentConfig);
            //StationSet = new BmnGemStripStationSet(gPathGemConfig + "GemRunSpring2017.xml");
            if (fVerbose > 1) cout << "   Current GEM Configuration : RunSpring2017" << "\n";
            break;

        case BmnGemStripConfiguration::RunSpring2018:
            StationSet = new BmnGemStripStationSet(gPathGemConfig + "GemRunSpring2018.xml");
            if (fVerbose > 1) cout << "   Current GEM Configuration : RunSpring2018" << "\n";
            break;

        case BmnGemStripConfiguration::RunSRCSpring2018 :
            StationSet = new BmnGemStripStationSet(gPathGemConfig + "GemRunSRCSpring2018.xml");
            TransfSet = new BmnGemStripTransform();
            TransfSet->LoadFromXMLFile(gPathGemConfig + "GemRunSRCSpring2018.xml");
            if (fVerbose > 1) cout << "   Current GEM Configuration : GemRunSRCSpring2018" << "\n";
            break;

        default:
            StationSet = NULL;
    }

    fField = FairRunAna::Instance()->GetField();
    if (!fField)
        Fatal("Init", "No Magnetic Field found");

    //--------------------------------------------------------------------------

    fBmnEvQuality = (TClonesArray*) ioman->GetObject(fBmnEvQualityBranchName);
    if (fIsExp && fVerbose > 1)
        fAlign->Print();

    if (fVerbose > 1) cout << "=================== BmnGemStripHitMaker::Init() finished ==============" << endl;

    return kSUCCESS;
}

void BmnGemStripHitMaker::Exec(Option_t* opt) {
    // Event separation by triggers ...
    if (fIsExp && fBmnEvQuality) {
        BmnEventQuality* evQual = (BmnEventQuality*) fBmnEvQuality->UncheckedAt(0);
        if (!evQual->GetIsGoodEvent())
            return;
    }
    fBmnGemStripHitsArray->Delete();

    if (fHitMatching && fBmnGemStripHitMatchesArray) {
        fBmnGemStripHitMatchesArray->Delete();
    }

    if (!IsActive())
        return;

    if (fVerbose > 1) cout << "=================== BmnGemStripHitMaker::Exec() started ===============" << endl;
    clock_t tStart = clock();

    fField = FairRunAna::Instance()->GetField();

    if (fVerbose > 1) cout << " BmnGemStripHitMaker::Exec(), Number of BmnGemStripDigits = " << fBmnGemStripDigitsArray->GetEntriesFast() << "\n";

    ProcessDigits();

    if (fVerbose > 1) cout << "=================== BmnGemStripHitMaker::Exec() finished ==============" << endl;
    clock_t tFinish = clock();
    workTime += ((Float_t) (tFinish - tStart)) / CLOCKS_PER_SEC;
}

void BmnGemStripHitMaker::ProcessDigits() {

    FairMCPoint* MCPoint;
    BmnGemStripDigit* digit;
    BmnMatch *strip_match;

    BmnGemStripStation* station;
    BmnGemStripModule* module;

    //Loading digits ---------------------------------------------------------------
    Int_t AddedDigits = 0;
    Int_t AddedStripDigitMatches = 0;

    for (UInt_t idigit = 0; idigit < fBmnGemStripDigitsArray->GetEntriesFast(); idigit++) {
        digit = (BmnGemStripDigit*) fBmnGemStripDigitsArray->At(idigit);
        if (!digit->IsGoodDigit())
            continue;
        station = StationSet->GetGemStation(digit->GetStation());
        module = station->GetModule(digit->GetModule());

        if (module->SetStripSignalInLayer(digit->GetStripLayer(), digit->GetStripNumber(), digit->GetStripSignal())) AddedDigits++;

        if (fBmnGemStripDigitMatchesArray) {
            strip_match = (BmnMatch*) fBmnGemStripDigitMatchesArray->At(idigit);
            if (module->SetStripMatchInLayer(digit->GetStripLayer(), digit->GetStripNumber(), *strip_match)) AddedStripDigitMatches++;
        }
    }

    if (fVerbose > 1) cout << "   Processed strip digits  : " << AddedDigits << "\n";
    if (fVerbose > 1 && fBmnGemStripDigitMatchesArray) cout << "   Added strip digit matches  : " << AddedStripDigitMatches << "\n";
    //------------------------------------------------------------------------------

    //Processing digits
    StationSet->ProcessPointsInDetector();

    Int_t NCalculatedPoints = StationSet->CountNProcessedPointsInDetector();
    if (fVerbose > 1) cout << "   Calculated points  : " << NCalculatedPoints << "\n";
    if (fVerbose == 1) cout << "BmnGemStripHitMaker: " << NCalculatedPoints << " hits\n";

    Int_t clear_matched_points_cnt = 0; // points with the only one match-index

    //for SRC only!!! //Temporary! FIXME: move into DB
    const Int_t nSt = 10;
    Double_t alignX[nSt] = {0.0, 0.0, 0.0, 0.0, +0.163, -0.061, -0.038, -0.037, +0.003, +0.074};
    Double_t alignY[nSt] = {0.0, 0.0, 0.0, 0.0, -0.009, -0.080, +0.074, +0.009, +0.036, -0.089};
    const Int_t nFields = 6;

    Double_t dX_ls[nSt][nFields] = {
        {0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
        {0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
        {0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
        {0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
        {0.0, -0.05, -0.04, -0.02, -0.00, -0.04},
        {0.0, +0.14, +0.21, +0.30, +0.38, +0.45},
        {0.0, -0.08, -0.11, -0.10, -0.13, -0.11},
        {0.0, +0.10, +0.13, +0.20, +0.22, +0.31},
        {0.0, -0.13, -0.19, -0.22, -0.30, -0.30},
        {0.0, +0.03, +0.02, +0.03, -0.01, +0.04}};

    Double_t dY_ls[nSt][nFields] = {
        {0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
        {0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
        {0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
        {0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
        {0.0, +0.00, -0.00, -0.00, -0.02, -0.01},
        {0.0, +0.01, +0.02, +0.06, +0.04, +0.04},
        {0.0, -0.02, -0.01, +0.03, -0.01, -0.02},
        {0.0, -0.00, +0.02, +0.07, +0.04, +0.01},
        {0.0, -0.02, -0.00, +0.03, -0.02, -0.07},
        {0.0, +0.05, +0.10, +0.17, +0.15, +0.09}};

    Int_t fieldFactors[nFields] = {0, 900, 1200, 1500, 1800, 2100};
    Int_t iField = 4;

    for (Int_t iStation = 0; iStation < StationSet->GetNStations(); ++iStation) {
        BmnGemStripStation *station = StationSet->GetGemStation(iStation);

        for (Int_t iModule = 0; iModule < station->GetNModules(); ++iModule) {
            BmnGemStripModule *module = station->GetModule(iModule);
            Double_t z = module->GetZPositionRegistered();
            z += fIsExp ? fAlign->GetGemCorrs()[iStation][iModule][2] : 0.; //alignment implementation

            Int_t NIntersectionPointsInModule = module->GetNIntersectionPoints();

            for (Int_t iPoint = 0; iPoint < NIntersectionPointsInModule; ++iPoint) {

                Double_t threshold = 2000000; // ADC
                Double_t sigL = module->GetIntersectionPoint_LowerLayerSripTotalSignal(iPoint);
                Double_t sigU = module->GetIntersectionPoint_UpperLayerSripTotalSignal(iPoint);

                Double_t x = module->GetIntersectionPointX(iPoint);
                Double_t y = module->GetIntersectionPointY(iPoint);

                Double_t x_err = module->GetIntersectionPointXError(iPoint);
                Double_t y_err = module->GetIntersectionPointYError(iPoint);
                Double_t z_err = 0.0;

                 //Transform hit coordinates from local coordinate system of GEM-planes to global
                if(TransfSet) {
                    Plane3D::Point loc_point = TransfSet->ApplyTransforms(Plane3D::Point(-x, y, z), iStation, iModule);
                    x = -loc_point.X();
                    y = loc_point.Y();
                    z = loc_point.Z();
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
                x *= -1; // invert to global X

                if (fIsExp) {
                    Double_t deltaX = fIsSrc ? alignX[iStation] : fAlign->GetGemCorrs()[iStation][iModule][0];
                    Double_t deltaY = fIsSrc ? alignY[iStation] : fAlign->GetGemCorrs()[iStation][iModule][1];

                    x += deltaX;
                    y += deltaY;

                    if (Abs(fField->GetBy(0., 0., 0.)) > FLT_EPSILON) {
                        Int_t sign = (module->GetElectronDriftDirection() == ForwardZAxisEDrift) ? +1 : -1;
                        Double_t lsX = fIsSrc ? dX_ls[iStation][iField] : (fAlign->GetLorentzCorrs(Abs(fField->GetBy(x, y, z)), iStation) * sign);
                        x += lsX;
                        Double_t lsY = fIsSrc ? dY_ls[iStation][iField] : (fAlign->GetLorentzCorrs(Abs(fField->GetBx(x, y, z)), iStation) * sign);
                        y += lsY;
                    }

                }

                new ((*fBmnGemStripHitsArray)[fBmnGemStripHitsArray->GetEntriesFast()])
                        BmnGemStripHit(0, TVector3(x, y, z), TVector3(x_err, y_err, z_err), RefMCIndex);

                BmnGemStripHit* hit = (BmnGemStripHit*) fBmnGemStripHitsArray->At(fBmnGemStripHitsArray->GetEntriesFast() - 1);
                hit->SetStation(iStation);
                hit->SetModule(iModule);
                hit->SetIndex(fBmnGemStripHitsArray->GetEntriesFast() - 1);
                hit->SetClusterSizeInLowerLayer(module->GetIntersectionPoint_LowerLayerClusterSize(iPoint)); //cluster size (lower layer |||)
                hit->SetClusterSizeInUpperLayer(module->GetIntersectionPoint_UpperLayerClusterSize(iPoint)); //cluster size (upper layer ///or\\\)
                hit->SetStripPositionInLowerLayer(module->GetIntersectionPoint_LowerLayerSripPosition(iPoint)); //strip position (lower layer |||)
                hit->SetStripPositionInUpperLayer(module->GetIntersectionPoint_UpperLayerSripPosition(iPoint)); //strip position (upper layer ///or\\\)
                hit->SetStripTotalSignalInLowerLayer(sigL);
                hit->SetStripTotalSignalInUpperLayer(sigU);
                //--------------------------------------------------------------

                //hit matching -------------------------------------------------
                if (fHitMatching && fBmnGemStripHitMatchesArray) {
                    new ((*fBmnGemStripHitMatchesArray)[fBmnGemStripHitMatchesArray->GetEntriesFast()])
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

void BmnGemStripHitMaker::Finish() {
    if (StationSet) {
        delete StationSet;
        StationSet = nullptr;
    }

    if(TransfSet) {
        delete TransfSet;
        TransfSet = nullptr;
    }

    cout << "Work time of the GEM hit maker: " << workTime << endl;
}

ClassImp(BmnGemStripHitMaker)
