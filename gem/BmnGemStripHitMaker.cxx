#include "BmnGemStripHitMaker.h"

BmnGemStripHitMaker::BmnGemStripHitMaker() {

    fInputPointsBranchName = "StsPoint";
    fInputDigitsBranchName = "BmnGemStripDigit";
    fOutputHitsBranchName = "BmnGemStripHit";

    fVerbose = 1;
}

BmnGemStripHitMaker::~BmnGemStripHitMaker() {

}

InitStatus BmnGemStripHitMaker::Init() {

    if(fVerbose) cout << " BmnGemStripHitMaker::Init()\n ";

    FairRootManager* ioman = FairRootManager::Instance();

    fBmnGemStripPointsArray = (TClonesArray*) ioman->GetObject(fInputPointsBranchName);
    fBmnGemStripDigitsArray = (TClonesArray*) ioman->GetObject(fInputDigitsBranchName);

    fBmnGemStripHitsArray = new TClonesArray(fOutputHitsBranchName);
    ioman->Register(fOutputHitsBranchName, "GEM", fBmnGemStripHitsArray, kTRUE);

    return kSUCCESS;
}

void BmnGemStripHitMaker::Exec(Option_t* opt) {

    fBmnGemStripHitsArray->Clear();

    if (!fBmnGemStripPointsArray) {
        Error("BmnGemStripHitMaker::Init()", " !!! Unknown branch name !!! ");
        return;
    }
    if (!fBmnGemStripDigitsArray) {
        Error("BmnGemStripHitMaker::Init()", " !!! Unknown branch name !!! ");
        return;
    }

    if(fVerbose) cout << " BmnGemStripHitMaker::Exec(), Number of BmnGemStripDigits = " << fBmnGemStripDigitsArray->GetEntriesFast() << "\n";

    ProcessDigits();

    if(fVerbose) cout << " BmnGemStripHitMaker::Exec() finished\n";
}

void BmnGemStripHitMaker::ProcessDigits() {

    FairMCPoint* MCPoint;
    BmnGemStripDigit* digit;

    BmnGemStripStationSet StationSet;

    BmnGemStripStation* station;
    BmnGemStripReadoutModule* module;

//Loading digits
    Int_t AddedDigits = 0;
    for(UInt_t idigit = 0; idigit < fBmnGemStripDigitsArray->GetEntriesFast(); idigit++) {
        digit = (BmnGemStripDigit*)fBmnGemStripDigitsArray->At(idigit);
        station = StationSet.GetGemStation(digit->GetStation());
        module = station->GetReadoutModule(digit->GetModule());

        if(digit->GetStripLayer() == 0) {
            if(module->SetValueOfLowerStrip(digit->GetStripNumber(), digit->GetStripSignal())) AddedDigits++;
        }
        if(digit->GetStripLayer() == 1) {
            if(module->SetValueOfUpperStrip(digit->GetStripNumber(), digit->GetStripSignal())) AddedDigits++;
        }
    }
    if(fVerbose) cout << "   Processed strip digits  : " << AddedDigits << "\n";
//------------------------------------------------------------------------------

//Processing digits
    StationSet.ProcessPointsInDetector();

    Int_t NCalculatedPoints = StationSet.CountNProcessedPointsInDetector();
    if(fVerbose) cout << "   Calculated points  : " << NCalculatedPoints << "\n";
    
    Int_t match_cnt = 0;
    for(Int_t iStation = 0; iStation < StationSet.GetNStations(); ++iStation) {
        BmnGemStripStation *station = StationSet.GetGemStation(iStation);

        for(Int_t iModule = 0; iModule < station->GetNModules(); ++iModule) {
            BmnGemStripReadoutModule *module = station->GetReadoutModule(iModule);
            Double_t z = module->GetZPositionReadout();

            for(Int_t iPoint = 0; iPoint < module->GetNIntersectionPoints(); ++iPoint) {
                Double_t x = module->GetIntersectionPointX(iPoint);
                Double_t y = module->GetIntersectionPointY(iPoint);

                Double_t x_err = module->GetIntersectionPointXError(iPoint);
                Double_t y_err = module->GetIntersectionPointYError(iPoint);
                Double_t z_err = 0.0;

                //match intersection points with MC-points (find RefMCIndex)
                Int_t RefMCIndex = -1;
                Double_t min_distance = 1E10;

                for(Int_t iMCPoint = 0; iMCPoint < fBmnGemStripPointsArray->GetEntriesFast(); iMCPoint++) {
                    MCPoint = (FairMCPoint*) fBmnGemStripPointsArray->At(iMCPoint);
                    Double_t xmc = -MCPoint->GetX();
                    Double_t ymc = MCPoint->GetY();
                    Double_t zmc = MCPoint->GetZ();
                    Int_t StationNum = StationSet.GetPointStationOwnership(zmc);
                    Int_t ModuleNum = station->GetPointModuleOwhership(xmc, ymc);

                    if((StationNum == iStation) && (ModuleNum == iModule)) {
                        Double_t pitch = StationSet.GetGemStation(iStation)->GetReadoutModule(iModule)->GetPitch();
                        Double_t angle = StationSet.GetGemStation(iStation)->GetReadoutModule(iModule)->GetAngleDeg();

                        Double_t max_xdeviation = pitch;
                        Double_t max_ydeviation = pitch/Sin(Abs(angle)*Pi()/180.0);

                        if( (Abs(x-xmc) <= max_xdeviation) && (Abs(y-ymc) <= max_ydeviation) ) {
                            Double_t cur_distance = (x-xmc)*(x-xmc) + (y-ymc)*(y-ymc);
                            if(cur_distance < min_distance) {
                                min_distance = cur_distance;
                                RefMCIndex = iMCPoint;
                            }
                            match_cnt++;
                        }
                    }
                }
                //--------------------------------------------------------------

                //Add hit
                x *= -1; // invert to global X

                new ((*fBmnGemStripHitsArray)[fBmnGemStripHitsArray->GetEntriesFast()])
                    BmnGemStripHit(0, TVector3(x, y, z), TVector3(x_err, y_err, z_err), RefMCIndex);

                BmnGemStripHit* hit = (BmnGemStripHit*) fBmnGemStripHitsArray->At(fBmnGemStripHitsArray->GetEntriesFast() - 1);
                hit->SetStation(iStation);
                hit->SetModule(iModule);
                hit->SetIndex(fBmnGemStripHitsArray->GetEntriesFast() - 1);

                Double_t deloss_lower = module->GetIntersectionPointsLowerTotalSignal(iPoint);
                Double_t deloss_upper = module->GetIntersectionPointsUpperTotalSignal(iPoint);
                hit->SetEnergyLoss(deloss_lower+deloss_upper);
                //--------------------------------------------------------------
            }
        }
    }
    if(fVerbose) cout << "   N matches with MC-points = " << match_cnt << "\n";
//------------------------------------------------------------------------------
}

void BmnGemStripHitMaker::Finish() {

}

ClassImp(BmnGemStripHitMaker)