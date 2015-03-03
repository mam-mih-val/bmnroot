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

//Find hits and fakes-----------------------------------------------------------
            Int_t *PointTypeArray = new Int_t[module->GetNIntersectionPoints()];
            Double_t *PointSignalDiffArray = new Double_t[module->GetNIntersectionPoints()];
            for(Int_t i = 0; i < module->GetNIntersectionPoints(); ++i) {
                PointTypeArray[i] = -1; //set undef feature
                PointSignalDiffArray[i] = -1; //set unknown signal diff
            }
            FindHitsAndFakes(PointTypeArray, PointSignalDiffArray, station, module);
//------------------------------------------------------------------------------

            for(Int_t iPoint = 0; iPoint < module->GetNIntersectionPoints(); ++iPoint) {
                Double_t x = module->GetIntersectionPointX(iPoint);
                Double_t y = module->GetIntersectionPointY(iPoint);

                Double_t low_pos_intersec = module->GetIntersectionPointLowerStripPos(iPoint);
                Double_t up_pos_intersec = module->GetIntersectionPointUpperStripPos(iPoint);

                Double_t x_err = module->GetIntersectionPointXError(iPoint);
                Double_t y_err = module->GetIntersectionPointYError(iPoint);
                Double_t z_err = 0.0;

                //match intersection points with MC-points (find RefMCIndex)
                Int_t RefMCIndex = -1;
                Double_t min_distance = 1E10;
                Int_t *MCPointsStatArray = new Int_t[fBmnGemStripPointsArray->GetEntriesFast()]; // 0 - not used MC point, 1 - used
                for(Int_t i = 0; i < fBmnGemStripPointsArray->GetEntriesFast(); ++i) {
                    MCPointsStatArray[i] = 0; //unused status
                }

                for(Int_t iMCPoint = 0; iMCPoint < fBmnGemStripPointsArray->GetEntriesFast(); iMCPoint++) {
                    if(MCPointsStatArray[iMCPoint] == 1) continue;

                    MCPoint = (FairMCPoint*) fBmnGemStripPointsArray->At(iMCPoint);
                    Double_t xmc = -MCPoint->GetX();
                    Double_t ymc = MCPoint->GetY();
                    Double_t zmc = MCPoint->GetZ();
                    Int_t StationNum = StationSet.GetPointStationOwnership(zmc);
                    Int_t ModuleNum = station->GetPointModuleOwhership(xmc, ymc);

                    if((StationNum == iStation) && (ModuleNum == iModule)) {
                        Double_t low_pos_mc = module->CalculateLowerStripZonePosition(xmc, ymc);
                        Double_t up_pos_mc = module->CalculateUpperStripZonePosition(xmc, ymc);

                        Double_t low_diff_pos = Abs(low_pos_intersec - low_pos_mc);
                        Double_t up_diff_pos = Abs(up_pos_intersec - up_pos_mc);

                        if( low_diff_pos <= 1.5 && up_diff_pos <= 1.5 ) {
                            Double_t cur_distance = low_diff_pos + up_diff_pos;
                            if(cur_distance < min_distance) {
                                min_distance = cur_distance;
                                RefMCIndex = iMCPoint;
                            }
                        }
                    }
                }
                if(RefMCIndex != -1)  {
                    MCPointsStatArray[RefMCIndex] = 1;
                    match_cnt++;
                }
                delete [] MCPointsStatArray;
//------------------------------------------------------------------------------

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

                hit->SetType(PointTypeArray[iPoint]);
                hit->SetSignalDiff(PointSignalDiffArray[iPoint]);
                //--------------------------------------------------------------
            }
            delete [] PointTypeArray;
            delete [] PointSignalDiffArray;
        }
    }
    if(fVerbose) cout << "   N matches with MC-points = " << match_cnt << "\n";
//------------------------------------------------------------------------------
}

void BmnGemStripHitMaker::FindHitsAndFakes(Int_t *PointTypeArray, Double_t *PointSignalDiffArray, BmnGemStripStation* station, BmnGemStripReadoutModule* module) {
    Double_t hit_rate_threshold = 0.01;
    Double_t fake_rate_threshold = 0.5;

    for(Int_t iPoint = 0; iPoint < module->GetNIntersectionPoints(); ++iPoint) {
        Double_t lower_pos = module->GetIntersectionPointLowerStripPos(iPoint);
        Double_t upper_pos = module->GetIntersectionPointUpperStripPos(iPoint);

        Double_t lower_signal = module->GetIntersectionPointsLowerTotalSignal(iPoint);
        Double_t upper_signal = module->GetIntersectionPointsUpperTotalSignal(iPoint);

        Double_t diff_signal = lower_signal - upper_signal;
        Double_t diff_signal_normalized = Abs(diff_signal/(lower_signal+upper_signal));

        PointSignalDiffArray[iPoint] = diff_signal_normalized;

        if( diff_signal_normalized <= hit_rate_threshold ) {
            PointTypeArray[iPoint] = 1; // hit

            for(Int_t i = 0; i < module->GetNIntersectionPoints(); ++i) {
                if(i != iPoint) {
                    if(PointTypeArray[i] != 1) {
                       Double_t lpos = module->GetIntersectionPointLowerStripPos(i);
                       Double_t upos = module->GetIntersectionPointUpperStripPos(i);
                       if(lpos == lower_pos || upos == upper_pos) {
                           PointTypeArray[i] = 0; //fake
                       }
                    }
                }
            }
        }
        else {
            if( Abs(diff_signal_normalized) > fake_rate_threshold ) {
                //PointTypeArray[iPoint] = 0; //fake
            }
        }
    }
}

void BmnGemStripHitMaker::Finish() {

}

ClassImp(BmnGemStripHitMaker)