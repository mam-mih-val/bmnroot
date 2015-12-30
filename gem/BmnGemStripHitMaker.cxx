#include "BmnGemStripHitMaker.h"

BmnGemStripHitMaker::BmnGemStripHitMaker()
: fHitMatching(kTRUE) {

    fInputPointsBranchName = "StsPoint";
    fInputDigitsBranchName = "BmnGemStripDigit";
    fInputDigitMatchesBranchName = "BmnGemStripDigitMatch";

    fOutputHitsBranchName = "BmnGemStripHit";
    fOutputHitMatchesBranchName = "BmnGemStripHitMatch";

    fVerbose = 1;
}

BmnGemStripHitMaker::~BmnGemStripHitMaker() {

}

InitStatus BmnGemStripHitMaker::Init() {

    if(fVerbose) cout << "\nBmnGemStripHitMaker::Init()\n ";

    FairRootManager* ioman = FairRootManager::Instance();

    fBmnGemStripPointsArray = (TClonesArray*) ioman->GetObject(fInputPointsBranchName);
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

    if(fVerbose) cout << "BmnGemStripHitMaker::Init() finished\n\n ";

    return kSUCCESS;
}

void BmnGemStripHitMaker::Exec(Option_t* opt) {

    fBmnGemStripHitsArray->Clear();

    if(fHitMatching && fBmnGemStripHitMatchesArray) {
        fBmnGemStripHitMatchesArray->Clear();
    }

    if (!fBmnGemStripPointsArray) {
        Error("BmnGemStripHitMaker::Exec()", " !!! Unknown branch name !!! ");
        return;
    }
    if (!fBmnGemStripDigitsArray) {
        Error("BmnGemStripHitMaker::Exec()", " !!! Unknown branch name !!! ");
        return;
    }

    if(fVerbose) cout << " BmnGemStripHitMaker::Exec(), Number of BmnGemStripDigits = " << fBmnGemStripDigitsArray->GetEntriesFast() << "\n";

    ProcessDigits();

    if(fVerbose) cout << " BmnGemStripHitMaker::Exec() finished\n";
}

void BmnGemStripHitMaker::ProcessDigits() {

    FairMCPoint* MCPoint;
    BmnGemStripDigit* digit;
    BmnMatch *strip_match;

    BmnGemStripStationSet StationSet;

    BmnGemStripStation* station;
    BmnGemStripReadoutModule* module;

//Loading digits ---------------------------------------------------------------
    Int_t AddedDigits = 0;
    Int_t AddedStripDigitMatches = 0;

    for(UInt_t idigit = 0; idigit < fBmnGemStripDigitsArray->GetEntriesFast(); idigit++) {
        digit = (BmnGemStripDigit*)fBmnGemStripDigitsArray->At(idigit);
        station = StationSet.GetGemStation(digit->GetStation());
        module = station->GetReadoutModule(digit->GetModule());

        if(digit->GetStripLayer() == 0) {
            if( module->SetValueOfLowerStrip(digit->GetStripNumber(), digit->GetStripSignal()) ) AddedDigits++;
        }
        if(digit->GetStripLayer() == 1) {
            if( module->SetValueOfUpperStrip(digit->GetStripNumber(), digit->GetStripSignal()) ) AddedDigits++;
        }

        if(fBmnGemStripDigitMatchesArray) {
            strip_match = (BmnMatch*)fBmnGemStripDigitMatchesArray->At(idigit);

            if(digit->GetStripLayer() == 0) {
                if( module->SetMatchOfLowerStrip(digit->GetStripNumber(), *strip_match) ) AddedStripDigitMatches++;
            }
            if(digit->GetStripLayer() == 1) {
                if( module->SetMatchOfUpperStrip(digit->GetStripNumber(), *strip_match) ) AddedStripDigitMatches++;
            }
        }
   }

   if(fVerbose) cout << "   Processed strip digits  : " << AddedDigits << "\n";
   if(fVerbose && fBmnGemStripDigitMatchesArray) cout << "   Added strip digit matches  : " << AddedStripDigitMatches << "\n";
//------------------------------------------------------------------------------

//Processing digits
    StationSet.ProcessPointsInDetector();

    Int_t NCalculatedPoints = StationSet.CountNProcessedPointsInDetector();
    if(fVerbose) cout << "   Calculated points  : " << NCalculatedPoints << "\n";

    Int_t NMCPoints = fBmnGemStripPointsArray->GetEntriesFast();
    Int_t matched_points_cnt = 0;

    for(Int_t iStation = 0; iStation < StationSet.GetNStations(); ++iStation) {
        BmnGemStripStation *station = StationSet.GetGemStation(iStation);

        for(Int_t iModule = 0; iModule < station->GetNModules(); ++iModule) {
            BmnGemStripReadoutModule *module = station->GetReadoutModule(iModule);
            Double_t z = module->GetZPositionReadout();

            Int_t NIntersectionPointsInModule = module->GetNIntersectionPoints();

//Find hits and fakes ----------------------------------------------------------
            Int_t *PointTypeArray = new Int_t[NIntersectionPointsInModule]; // -1 - undefined, 0 - fake, 1 - hit
            Double_t *PointSignalDiffArray = new Double_t[NIntersectionPointsInModule];
            for(Int_t i = 0; i < NIntersectionPointsInModule; ++i) {
                PointTypeArray[i] = -1; //set undef feature
                PointSignalDiffArray[i] = -1; //set unknown signal diff
            }
            FindHitsAndFakes(PointTypeArray, PointSignalDiffArray, station, module);
//------------------------------------------------------------------------------

//Matching intersection points with MC-points (finding RefMCIndex) -------------
            Double_t max_distance = module->GetPitch()*5; //cm  +- 5 strips residual

            Int_t *RefIndex = new Int_t[NIntersectionPointsInModule];

            for(Int_t iPoint = 0; iPoint < NIntersectionPointsInModule; ++iPoint) {
                RefIndex[iPoint] = -1; // -1 is undefined status
            }

            for(Int_t iPoint = 0; iPoint < NIntersectionPointsInModule; ++iPoint) {

                Double_t x = module->GetIntersectionPointX(iPoint);
                Double_t y = module->GetIntersectionPointY(iPoint);

                Double_t min_distance = 1E10;

                for(Int_t iMCPoint = 0; iMCPoint < NMCPoints; iMCPoint++) {
                    MCPoint = (FairMCPoint*) fBmnGemStripPointsArray->At(iMCPoint);
                    Double_t xmc = -MCPoint->GetX();
                    Double_t ymc = MCPoint->GetY();
                    Double_t zmc = MCPoint->GetZ();
                    Int_t StationNum = StationSet.GetPointStationOwnership(zmc);
                    Int_t ModuleNum = station->GetPointModuleOwhership(xmc, ymc);

                    if((StationNum == iStation) && (ModuleNum == iModule)) {
                        //Double_t distance = sqrt((x-xmc)*(x-xmc) + (y-ymc)*(y-ymc));
                        Double_t distance = (x-xmc)*(x-xmc) + (y-ymc)*(y-ymc);

                        if(distance > max_distance*max_distance) continue;

                        if(distance < min_distance) {
                            min_distance = distance;
                            RefIndex[iPoint] = iMCPoint;
                        }
                    }
                }
            }

            for(Int_t iPoint = 0; iPoint < NIntersectionPointsInModule; ++iPoint) {
                Int_t ref_index = RefIndex[iPoint];

                for(Int_t iPoint_check = 0; iPoint_check < NIntersectionPointsInModule; ++iPoint_check) {
                    Int_t ref_index_check = RefIndex[iPoint_check];

                    if(ref_index != -1 && ref_index == ref_index_check) {
                        Double_t x = module->GetIntersectionPointX(iPoint);
                        Double_t y = module->GetIntersectionPointY(iPoint);

                        Double_t x_check = module->GetIntersectionPointX(iPoint_check);
                        Double_t y_check = module->GetIntersectionPointY(iPoint_check);

                        MCPoint = (FairMCPoint*) fBmnGemStripPointsArray->At(ref_index);
                        Double_t xmc = -MCPoint->GetX();
                        Double_t ymc = MCPoint->GetY();

                        //Double_t distance = sqrt((x-xmc)*(x-xmc) + (y-ymc)*(y-ymc));
                        //Double_t distance_check = sqrt((x_check-xmc)*(x_check-xmc) + (y_check-ymc)*(y_check-ymc));
                        Double_t distance = (x-xmc)*(x-xmc) + (y-ymc)*(y-ymc); // fix it (put off sqrt func)
                        Double_t distance_check = (x_check-xmc)*(x_check-xmc) + (y_check-ymc)*(y_check-ymc);

                        Bool_t use_point_status = true; //use additional information from FindHitsAndFakes algorithm

                        if(use_point_status) {
                            Int_t point_status = PointTypeArray[iPoint];
                            Int_t point_check_status = PointTypeArray[iPoint_check];

                            if(distance > distance_check) {
                                if(point_status == 1 && point_check_status == 0) {
                                    RefIndex[iPoint_check] = -1;
                                }
                                else {
                                    RefIndex[iPoint] = -1;
                                }
                            }
                        }
                        else {
                            if(distance > distance_check) {
                                RefIndex[iPoint] = -1;
                            }
                        }
                    }
                }
            }
//------------------------------------------------------------------------------

            for(Int_t iPoint = 0; iPoint < NIntersectionPointsInModule; ++iPoint) {
                Double_t x = module->GetIntersectionPointX(iPoint);
                Double_t y = module->GetIntersectionPointY(iPoint);

                Double_t x_err = module->GetIntersectionPointXError(iPoint);
                Double_t y_err = module->GetIntersectionPointYError(iPoint);
                Double_t z_err = 0.0;

                Int_t RefMCIndex = RefIndex[iPoint];

                if(RefMCIndex != -1)  {
                    matched_points_cnt++;
                }

                //Add hit ------------------------------------------------------
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

                //hit matching -------------------------------------------------
                if(fHitMatching && fBmnGemStripHitMatchesArray) {
                    new ((*fBmnGemStripHitMatchesArray)[fBmnGemStripHitMatchesArray->GetEntriesFast()])
                        BmnMatch(module->GetIntersectionPointMatch(iPoint));
                }
                //--------------------------------------------------------------
            }

            delete [] PointTypeArray;
            delete [] PointSignalDiffArray;
            delete [] RefIndex;
        }
    }
    if(fVerbose) cout << "   N matches with MC-points = " << matched_points_cnt << "\n";
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