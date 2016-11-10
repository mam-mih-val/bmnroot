#include "BmnGemStripHitMaker.h"

#include "BmnGemStripStationSet_RunSummer2016.h"
#include "BmnGemStripStationSet_RunWinter2016.h"

static Float_t workTime = 0.0;

BmnGemStripHitMaker::BmnGemStripHitMaker()
: fHitMatching(kTRUE) {

    fInputPointsBranchName = "StsPoint";
    fInputDigitsBranchName = "BmnGemStripDigit";
    fInputDigitMatchesBranchName = "BmnGemStripDigitMatch";

    fOutputHitsBranchName = "BmnGemStripHit";
    fOutputHitMatchesBranchName = "BmnGemStripHitMatch";

    fVerbose = 1;

    fCurrentConfig = BmnGemStripConfiguration::None;
    StationSet = NULL;
}

BmnGemStripHitMaker::BmnGemStripHitMaker(Bool_t isExp)
: fHitMatching(kTRUE) {

    fInputPointsBranchName = "StsPoint";
    fInputDigitsBranchName = (!isExp) ? "BmnGemStripDigit" : "GEM";

    fInputDigitMatchesBranchName = "BmnGemStripDigitMatch";

    fOutputHitsBranchName = "BmnGemStripHit";
    fOutputHitMatchesBranchName = "BmnGemStripHitMatch";

    fVerbose = 1;

    fCurrentConfig = BmnGemStripConfiguration::None;
    StationSet = NULL;
}

BmnGemStripHitMaker::~BmnGemStripHitMaker() {

}

InitStatus BmnGemStripHitMaker::Init() {

    if(fVerbose) cout << "\nBmnGemStripHitMaker::Init()\n ";

    //if GEM configuration is not set -> return a fatal error
    if(!fCurrentConfig) Fatal("BmnGemStripHitMaker::Init()", " !!! Current GEM config is not set !!! ");

    FairRootManager* ioman = FairRootManager::Instance();

    fBmnGemStripDigitsArray = (TClonesArray*) ioman->GetObject(fInputDigitsBranchName);
    fBmnGemStripDigitMatchesArray = (TClonesArray*) ioman->GetObject(fInputDigitMatchesBranchName);

    if(fVerbose && fBmnGemStripDigitMatchesArray) cout << "  Strip matching information exists!\n";
    else cout << "  Strip matching information doesn`t exist!\n";

    fBmnGemStripHitsArray = new TClonesArray(fOutputHitsBranchName);
    ioman->Register(fOutputHitsBranchName, "GEM", fBmnGemStripHitsArray, kTRUE);

    if(fHitMatching && fBmnGemStripDigitMatchesArray) {
        fBmnGemStripHitMatchesArray = new TClonesArray("BmnMatch");
        ioman->Register(fOutputHitMatchesBranchName, "GEM", fBmnGemStripHitMatchesArray, kTRUE);
    }
    else {
        fBmnGemStripHitMatchesArray = 0;
    }

    //Create GEM detector ------------------------------------------------------
    switch (fCurrentConfig) {
        case BmnGemStripConfiguration::RunSummer2016 :
            StationSet = new BmnGemStripStationSet_RunSummer2016(fCurrentConfig);
            cout << "   Current Configuration : RunSummer2016" << "\n";
            break;

        case BmnGemStripConfiguration::RunSummer2016_set1 :
            StationSet = new BmnGemStripStationSet_RunSummer2016(fCurrentConfig);
            cout << "   Current Configuration : RunSummer2016_set1" << "\n";
            break;

        case BmnGemStripConfiguration::RunSummer2016_set2 :
            StationSet = new BmnGemStripStationSet_RunSummer2016(fCurrentConfig);
            cout << "   Current Configuration : RunSummer2016_set2" << "\n";
            break;

        case BmnGemStripConfiguration::RunWinter2016 :
            StationSet = new BmnGemStripStationSet_RunWinter2016(fCurrentConfig);
            cout << "   Current Configuration : RunWinter2016" << "\n";
            break;

        default:
            StationSet = NULL;
    }
    //--------------------------------------------------------------------------

    if(fVerbose) cout << "BmnGemStripHitMaker::Init() finished\n\n ";

    return kSUCCESS;
}

void BmnGemStripHitMaker::Exec(Option_t* opt) {
    clock_t tStart = clock();
    fBmnGemStripHitsArray->Clear();

    if(fHitMatching && fBmnGemStripHitMatchesArray) {
        fBmnGemStripHitMatchesArray->Clear();
    }

    if (!fBmnGemStripDigitsArray) {
        Error("BmnGemStripHitMaker::Exec()", " !!! Unknown branch name !!! ");
        return;
    }

    if(fVerbose) cout << " BmnGemStripHitMaker::Exec(), Number of BmnGemStripDigits = " << fBmnGemStripDigitsArray->GetEntriesFast() << "\n";

    ProcessDigits();

    if(fVerbose) cout << " BmnGemStripHitMaker::Exec() finished\n";
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

    for(UInt_t idigit = 0; idigit < fBmnGemStripDigitsArray->GetEntriesFast(); idigit++) {
        digit = (BmnGemStripDigit*)fBmnGemStripDigitsArray->At(idigit);
        station = StationSet->GetGemStation(digit->GetStation());
        module = station->GetModule(digit->GetModule());

        if( module->SetStripSignalInLayer(digit->GetStripLayer(), digit->GetStripNumber(), digit->GetStripSignal()) ) AddedDigits++;

        if(fBmnGemStripDigitMatchesArray) {
            strip_match = (BmnMatch*)fBmnGemStripDigitMatchesArray->At(idigit);
            if( module->SetStripMatchInLayer(digit->GetStripLayer(), digit->GetStripNumber(), *strip_match) ) AddedStripDigitMatches++;
        }
   }

   if(fVerbose) cout << "   Processed strip digits  : " << AddedDigits << "\n";
   if(fVerbose && fBmnGemStripDigitMatchesArray) cout << "   Added strip digit matches  : " << AddedStripDigitMatches << "\n";
//------------------------------------------------------------------------------

//Processing digits
    StationSet->ProcessPointsInDetector();

    Int_t NCalculatedPoints = StationSet->CountNProcessedPointsInDetector();
    if(fVerbose) cout << "   Calculated points  : " << NCalculatedPoints << "\n";

    Int_t clear_matched_points_cnt = 0; // points with the only one match-index

    for(Int_t iStation = 0; iStation < StationSet->GetNStations(); ++iStation) {
        BmnGemStripStation *station = StationSet->GetGemStation(iStation);

        for(Int_t iModule = 0; iModule < station->GetNModules(); ++iModule) {
            BmnGemStripModule *module = station->GetModule(iModule);
            Double_t z = module->GetZPositionRegistered();

            Int_t NIntersectionPointsInModule = module->GetNIntersectionPoints();

            for(Int_t iPoint = 0; iPoint < NIntersectionPointsInModule; ++iPoint) {
                Double_t x = module->GetIntersectionPointX(iPoint);
                Double_t y = module->GetIntersectionPointY(iPoint);

                Double_t x_err = module->GetIntersectionPointXError(iPoint);
                Double_t y_err = module->GetIntersectionPointYError(iPoint);
                Double_t z_err = 0.0;

                Int_t RefMCIndex = 0;

                //hit matching (define RefMCIndex)) ----------------------------
                BmnMatch match = module->GetIntersectionPointMatch(iPoint);

                Int_t most_probably_index = -1;
                Double_t max_weight = 0;

                Int_t n_links = match.GetNofLinks();
                if(n_links == 1) clear_matched_points_cnt++;
                for(Int_t ilink = 0; ilink < n_links; ilink++) {
                    Int_t index = match.GetLink(ilink).GetIndex();
                    Double_t weight = match.GetLink(ilink).GetWeight();
                    if(weight > max_weight) {
                        max_weight = weight;
                        most_probably_index = index;
                    }
                }

                RefMCIndex = most_probably_index;
                //--------------------------------------------------------------

                //Add hit ------------------------------------------------------
                x *= -1; // invert to global X

                new ((*fBmnGemStripHitsArray)[fBmnGemStripHitsArray->GetEntriesFast()])
                    BmnGemStripHit(0, TVector3(x, y, z), TVector3(x_err, y_err, z_err), RefMCIndex);

                BmnGemStripHit* hit = (BmnGemStripHit*) fBmnGemStripHitsArray->At(fBmnGemStripHitsArray->GetEntriesFast() - 1);
                hit->SetStation(iStation);
                hit->SetModule(iModule);
                hit->SetIndex(fBmnGemStripHitsArray->GetEntriesFast() - 1);
                //--------------------------------------------------------------

                //hit matching -------------------------------------------------
                if(fHitMatching && fBmnGemStripHitMatchesArray) {
                    new ((*fBmnGemStripHitMatchesArray)[fBmnGemStripHitMatchesArray->GetEntriesFast()])
                        BmnMatch(module->GetIntersectionPointMatch(iPoint));
                }
                //--------------------------------------------------------------
            }
        }
    }
    if(fVerbose) cout << "   N clear matches with MC-points = " << clear_matched_points_cnt << "\n";
//------------------------------------------------------------------------------
    StationSet->Reset();
}

void BmnGemStripHitMaker::Finish() {
    if(StationSet) {
        delete StationSet;
        StationSet = NULL;
    }
    cout << "Work time of the GEM hit maker: " << workTime << endl;
}

ClassImp(BmnGemStripHitMaker)